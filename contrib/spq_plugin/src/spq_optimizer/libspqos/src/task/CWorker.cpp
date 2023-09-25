//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2008 - 2010 Greenplum, Inc.
//
//	@filename:
//		CWorker.cpp
//
//	@doc:
//		Worker abstraction, e.g. thread
//---------------------------------------------------------------------------

#include "spqos/task/CWorker.h"

#include "spqos/common/syslibwrapper.h"
#include "spqos/error/CFSimulator.h"
#include "spqos/memory/CMemoryPoolManager.h"
#include "spqos/string/CWStringStatic.h"
#include "spqos/task/CWorkerPoolManager.h"

using namespace spqos;

// host system callback function to report abort requests
bool (*CWorker::abort_requested_by_system)(void);


//---------------------------------------------------------------------------
//	@function:
//		CWorker::CWorker
//
//	@doc:
//		ctor
//
//---------------------------------------------------------------------------
CWorker::CWorker(ULONG stack_size, ULONG_PTR stack_start)
	: m_task(NULL), m_stack_size(stack_size), m_stack_start(stack_start)
{
	SPQOS_ASSERT(stack_size >= 2 * 1024 &&
				"Worker has to have at least 2KB stack");

	// register worker
	SPQOS_ASSERT(NULL == Self() && "Found registered worker!");

	CWorkerPoolManager::WorkerPoolManager()->RegisterWorker(this);
	SPQOS_ASSERT(this == CWorkerPoolManager::WorkerPoolManager()->Self());
}


//---------------------------------------------------------------------------
//	@function:
//		CWorker::~CWorker
//
//	@doc:
//		dtor
//
//---------------------------------------------------------------------------
CWorker::~CWorker()
{
	// unregister worker
	SPQOS_ASSERT(this == Self() && "Unidentified worker found.");
	CWorkerPoolManager::WorkerPoolManager()->RemoveWorker();
}


//---------------------------------------------------------------------------
//	@function:
//		CWorker::Execute
//
//	@doc:
//		Execute single task
//
//---------------------------------------------------------------------------
void
CWorker::Execute(CTask *task)
{
	SPQOS_ASSERT(task);
	SPQOS_ASSERT(NULL == m_task && "Another task is assigned to worker");

	m_task = task;
	SPQOS_TRY
	{
		m_task->Execute();
		m_task = NULL;
	}
	SPQOS_CATCH_EX(ex)
	{
		m_task = NULL;
		SPQOS_RETHROW(ex);
	}
	SPQOS_CATCH_END;
}


//---------------------------------------------------------------------------
//	@function:
//		CWorker::CheckForAbort
//
//	@doc:
//		Check pending abort flag; throw if abort is flagged
//
//---------------------------------------------------------------------------
void
CWorker::CheckForAbort(
#ifdef SPQOS_FPSIMULATOR
	const CHAR *file, ULONG line_num
#else
	const CHAR *, ULONG
#endif	// SPQOS_FPSIMULATOR
)
{
	// check if there is a task assigned to worker,
	// task is still running and CFA is not suspended
	if (NULL != m_task && m_task->IsRunning() && !m_task->IsAbortSuspended())
	{
		SPQOS_ASSERT(!m_task->GetErrCtxt()->IsPending() &&
					"Check-For-Abort while an exception is pending");

#ifdef SPQOS_FPSIMULATOR
		SimulateAbort(file, line_num);
#endif	// SPQOS_FPSIMULATOR

		if ((NULL != abort_requested_by_system &&
			 abort_requested_by_system()) ||
			m_task->IsCanceled())
		{
			// raise exception
			SPQOS_ABORT;
		}
	}
}


//---------------------------------------------------------------------------
//	@function:
//		CWorker::CheckStackSize
//
//	@doc:
//		Size of stack within context of this worker;
//		effectively calculates distance of local variable to stack start;
//		if stack space is exhausted we throw an exception;
//		else we check if requested space can fit in stack
//
//---------------------------------------------------------------------------
BOOL
CWorker::CheckStackSize(ULONG request) const
{
	ULONG_PTR ptr = 0;

	// get current stack size
	ULONG_PTR size = m_stack_start - (ULONG_PTR) &ptr;

	// check if we have exceeded stack space
	if (size >= m_stack_size)
	{
		// raise stack overflow exception
		SPQOS_RAISE(CException::ExmaSystem, CException::ExmiOutOfStack);
	}

	// check if there is enough stack space for request
	if (size + request >= m_stack_size)
	{
		return false;
	}
	return true;
}


#ifdef SPQOS_FPSIMULATOR

//---------------------------------------------------------------------------
//	@function:
//		CWorker::SimulateAbort
//
//	@doc:
//		Simulate abort request, log abort injection
//
//---------------------------------------------------------------------------
void
CWorker::SimulateAbort(const CHAR *file, ULONG line_num)
{
	if (m_task->IsTraceSet(EtraceSimulateAbort) &&
		CFSimulator::FSim()->NewStack(CException::ExmaSystem,
									  CException::ExmiAbort))
	{
		// SPQOS_TRACE has CFA, disable simulation temporarily
		m_task->SetTrace(EtraceSimulateAbort, false);

		SPQOS_TRACE_FORMAT_ERR("Simulating Abort at %s:%d", file, line_num);

		// resume simulation
		m_task->SetTrace(EtraceSimulateAbort, true);

		m_task->Cancel();
	}
}

#endif	// SPQOS_FPSIMULATOR


// EOF
