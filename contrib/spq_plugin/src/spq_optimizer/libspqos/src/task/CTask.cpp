//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2008 - 2010 Greenplum, Inc.
//
//	@filename:
//		CTask.cpp
//
//	@doc:
//		Task implementation
//---------------------------------------------------------------------------

#include "spqos/task/CTask.h"

#include "spqos/error/CErrorContext.h"
#include "spqos/error/CErrorHandlerStandard.h"
#include "spqos/task/CAutoSuspendAbort.h"
#include "spqos/task/CWorker.h"

using namespace spqos;

// init CTaskId's atomic counter
ULONG_PTR CTaskId::m_counter(0);

const CTaskId CTaskId::m_invalid_tid;

//---------------------------------------------------------------------------
//	@function:
//		CTask::~CTask
//
//	@doc:
//		ctor
//
//---------------------------------------------------------------------------
CTask::CTask(CMemoryPool *mp, CTaskContext *task_ctxt, IErrorContext *err_ctxt,
			 BOOL *cancel)
	: m_mp(mp),
	  m_task_ctxt(task_ctxt),
	  m_err_ctxt(err_ctxt),
	  m_err_handle(NULL),
	  m_func(NULL),
	  m_arg(NULL),
	  m_res(NULL),
	  m_status(EtsInit),
	  m_cancel(cancel),
	  m_cancel_local(false),
	  m_abort_suspend_count(false),
	  m_reported(false)
{
	SPQOS_ASSERT(NULL != mp);
	SPQOS_ASSERT(NULL != task_ctxt);
	SPQOS_ASSERT(NULL != err_ctxt);

	if (NULL == cancel)
	{
		m_cancel = &m_cancel_local;
	}
}


//---------------------------------------------------------------------------
//	@function:
//		CTask::~CTask
//
//	@doc:
//		dtor
//
//---------------------------------------------------------------------------
CTask::~CTask()
{
	SPQOS_ASSERT(0 == m_abort_suspend_count);

	// suspend cancellation
	CAutoSuspendAbort asa;

	SPQOS_DELETE(m_task_ctxt);
	SPQOS_DELETE(m_err_ctxt);

	CMemoryPoolManager::GetMemoryPoolMgr()->Destroy(m_mp);
}


//---------------------------------------------------------------------------
//	@function:
//		CTask::Bind
//
//	@doc:
//		Bind task to function and arguments
//
//---------------------------------------------------------------------------
void
CTask::Bind(void *(*func)(void *), void *arg)
{
	SPQOS_ASSERT(NULL != func);

	m_func = func;
	m_arg = arg;
}


//---------------------------------------------------------------------------
//	@function:
//		CTask::Execute
//
//	@doc:
//		Execution of task function; wrapped in asserts to prevent leaks
//
//---------------------------------------------------------------------------
void
CTask::Execute()
{
	SPQOS_ASSERT(EtsDequeued == m_status);

	// final task status
	ETaskStatus ets = m_status;

	// check for cancel
	if (*m_cancel)
	{
		ets = EtsError;
	}
	else
	{
		CErrorHandlerStandard errhdl;
		SPQOS_TRY_HDL(&errhdl)
		{
			// mark task as running
			SetStatus(EtsRunning);

			// call executable function
			m_res = m_func(m_arg);

#ifdef SPQOS_DEBUG
			// check interval since last CFA
			SPQOS_CHECK_ABORT;
#endif	// SPQOS_DEBUG

			// task completed
			ets = EtsCompleted;
		}
		SPQOS_CATCH_EX(ex)
		{
			// not reset error context with error propagation
			ets = EtsError;
		}
		SPQOS_CATCH_END;
	}

	// signal end of task execution
	SetStatus(ets);
}


//---------------------------------------------------------------------------
//	@function:
//		CTask::SetStatus
//
//	@doc:
//		Set task status;
//		Locking is required if updating more than one variable;
//
//---------------------------------------------------------------------------
void
CTask::SetStatus(ETaskStatus ets)
{
	// status changes are monotonic
	SPQOS_ASSERT(ets >= m_status && "Invalid task status transition");

	m_status = ets;
}


//---------------------------------------------------------------------------
//	@function:
//  	CTask::IsScheduled
//
//	@doc:
//		Check if task has been scheduled
//
//---------------------------------------------------------------------------
BOOL
CTask::IsScheduled() const
{
	switch (m_status)
	{
		case EtsInit:
			return false;
			break;
		case EtsQueued:
		case EtsDequeued:
		case EtsRunning:
		case EtsCompleted:
		case EtsError:
			return true;
			break;
		default:
			SPQOS_ASSERT(!"Invalid task status");
			return false;
	}
}


//---------------------------------------------------------------------------
//	@function:
//		CTask::IsFinished
//
//	@doc:
//		Check if task finished executing
//
//---------------------------------------------------------------------------
BOOL
CTask::IsFinished() const
{
	switch (m_status)
	{
		case EtsInit:
		case EtsQueued:
		case EtsDequeued:
		case EtsRunning:
			return false;
			break;
		case EtsCompleted:
		case EtsError:
			return true;
			break;
		default:
			SPQOS_ASSERT(!"Invalid task status");
			return false;
	}
}


//---------------------------------------------------------------------------
//	@function:
//		CTask::ResumeAbort
//
//	@doc:
//		Decrement counter for requests to suspend abort
//
//---------------------------------------------------------------------------
void
CTask::ResumeAbort()
{
	SPQOS_ASSERT(0 < m_abort_suspend_count);

	m_abort_suspend_count--;

#ifdef SPQOS_DEBUG
	CWorker *worker = CWorker::Self();

	SPQOS_ASSERT(NULL != worker);
#endif
}


#ifdef SPQOS_DEBUG


//---------------------------------------------------------------------------
//	@function:
//		CTask::CheckStatus
//
//	@doc:
//		Check if task has expected status
//
//---------------------------------------------------------------------------
BOOL
CTask::CheckStatus(BOOL completed)
{
	SPQOS_ASSERT(!IsCanceled());
	if (completed)
	{
		// task must have completed without an error
		return (CTask::EtsCompleted == GetStatus());
	}
	else
	{
		// task must still be running
		return (IsScheduled() && !IsFinished());
	}
}

#endif	// SPQOS_DEBUG

// EOF
