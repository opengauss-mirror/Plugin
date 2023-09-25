//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2010 Greenplum, Inc.
//
//	@filename:
//		CAutoTaskProxy.h
//
//	@doc:
//		Interface class for task management and execution.
//---------------------------------------------------------------------------

#ifndef SPQOS_CAutoTaskProxy_H
#define SPQOS_CAutoTaskProxy_H

#include "spqos/base.h"
#include "spqos/common/CList.h"
#include "spqos/task/CTask.h"
#include "spqos/task/CTaskContext.h"
#include "spqos/task/CWorkerPoolManager.h"

namespace spqos
{
//---------------------------------------------------------------------------
//	@class:
//		CAutoTaskProxy
//
//	@doc:
//		Auto task proxy (ATP) to handle task creation, execution and cleanup
//		ATP operations are not thread-safe; only one worker can use each ATP
//		object.
//
//---------------------------------------------------------------------------
class CAutoTaskProxy : CStackObject
{
private:
	// memory pool
	CMemoryPool *m_mp;

	// worker pool
	CWorkerPoolManager *m_pwpm;

	// task list
	CList<CTask> m_list;

	// propagate error of sub-task or not
	BOOL m_propagate_error;

	// find finished task
	SPQOS_RESULT
	FindFinished(CTask **task);

	// no copy ctor
	CAutoTaskProxy(const CAutoTaskProxy &);

	// propagate the error from sub-task to current task
	void PropagateError(CTask *sub_task);

	// check error from sub-task
	void CheckError(CTask *sub_task);

public:
	// ctor
	CAutoTaskProxy(CMemoryPool *mp, CWorkerPoolManager *m_pwpm,
				   BOOL propagate_error = true);

	// dtor
	~CAutoTaskProxy();

	// task count
	ULONG
	TaskCount()
	{
		return m_list.Size();
	}

	// disable/enable error propagation
	void
	SetPropagateError(BOOL propagate_error)
	{
		m_propagate_error = propagate_error;
	}

	// create new task
	CTask *Create(void *(*pfunc)(void *), void *argv, BOOL *cancel = NULL);

	// schedule task for execution
	void Schedule(CTask *task);

	// execute task in thread owning ATP (synchronous execution)
	void Execute(CTask *task);

	// cancel task
	void Cancel(CTask *task);

	// unregister and release task
	void Destroy(CTask *task);

	// unregister and release all tasks
	void DestroyAll();

};	// class CAutoTaskProxy

}  // namespace spqos

#endif	// !SPQOS_CAutoTaskProxy_H_

// EOF
