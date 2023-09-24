//-----------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2008 - 2010 Greenplum, Inc.
//
//	@filename:
//		CWorkerPoolManager.h
//
//	@doc:
//		Central worker pool manager;
//		* maintains worker local storage
//		* hosts task scheduler
//		* assigns tasks to workers
//-----------------------------------------------------------------------------
#ifndef SPQOS_CWorkerPoolManager_H
#define SPQOS_CWorkerPoolManager_H

#include "spqos/base.h"
#include "spqos/common/CSyncHashtable.h"
#include "spqos/common/CSyncHashtableAccessByKey.h"
#include "spqos/task/CTask.h"
#include "spqos/task/CTaskId.h"
#include "spqos/task/CTaskSchedulerFifo.h"
#include "spqos/task/CWorker.h"

#define SPQOS_WORKERPOOL_HT_SIZE (1024)		 // number of buckets in hash tables
#define SPQOS_WORKER_STACK_SIZE (500 * 1024)	 // max worker stack size

namespace spqos
{
//------------------------------------------------------------------------
//	@class:
//		CWorkerPoolManager
//
//	@doc:
//		Singleton object to handle worker pool;
//		maintains WLS (worker local storage);
//		assigns tasks to workers;
//
//------------------------------------------------------------------------
class CWorkerPoolManager
{
	friend class CWorker;
	friend class CAutoTaskProxy;

private:
	// response to worker scheduling request
	enum EScheduleResponse
	{
		EsrExecTask,   // run assigned task
		EsrWorkerExit  // clean up and exit
	};

	// memory pool
	CMemoryPool *m_mp;

	// task scheduler
	CTaskSchedulerFifo m_task_scheduler;

	// auto task proxy counter
	ULONG_PTR m_auto_task_proxy_counter;

	// active flag
	BOOL m_active;

	// we only support a single worker now
	CWorker *m_single_worker;

	// task storage
	CSyncHashtable<CTask, CTaskId> m_shtTS;

	//-------------------------------------------------------------------
	// Interface for CAutoTaskProxy
	//-------------------------------------------------------------------

	// add task to scheduler
	void Schedule(CTask *task);

	// increment AutoTaskProxy reference counter
	void
	AddRef()
	{
		m_auto_task_proxy_counter++;
	}

	// decrement AutoTaskProxy reference counter
	void
	RemoveRef()
	{
		SPQOS_ASSERT(m_auto_task_proxy_counter != 0 &&
					"AutoTaskProxy counter decremented from 0");
		m_auto_task_proxy_counter--;
	}

	// insert task in table
	void RegisterTask(CTask *task);

	// remove task from table
	CTask *RemoveTask(CTaskId tid);

	//-------------------------------------------------------------------
	// Interface for CWorker
	//-------------------------------------------------------------------

	// insert worker in table
	void RegisterWorker(CWorker *worker);

	// remove worker from table
	void RemoveWorker();

	//-------------------------------------------------------------------
	// Methods for internal use
	//-------------------------------------------------------------------

	// no copy ctor
	CWorkerPoolManager(const CWorkerPoolManager &);

	// private ctor
	CWorkerPoolManager(CMemoryPool *mp);

	// static singleton - global instance of worker pool manager

public:
	// lookup own worker
	inline CWorker *
	Self()
	{
		return m_single_worker;
	}

	// dtor
	~CWorkerPoolManager();

	// initialize worker pool manager
	static SPQOS_RESULT Init();

	// de-init global instance
	static void Shutdown();

	// global accessor
	static CWorkerPoolManager *WorkerPoolManager();

	// cancel task by task id
	void Cancel(CTaskId tid);

};	// class CWorkerPoolManager

}  // namespace spqos

#endif	// !SPQOS_CWorkerPoolManager_H

// EOF
