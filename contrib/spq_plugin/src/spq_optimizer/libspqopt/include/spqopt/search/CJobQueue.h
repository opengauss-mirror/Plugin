//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CJobQueue.h
//
//	@doc:
//		Class controlling unique execution of an operation that is
//		potentially assigned to many jobs.
//---------------------------------------------------------------------------
#ifndef SPQOPT_CJobQueue_H
#define SPQOPT_CJobQueue_H

#include "spqos/base.h"
#include "spqos/common/CList.h"

#include "spqopt/search/CJob.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CJobQueue
//
//	@doc:
//		Forces unique execution of an operation assigned to many jobs.
//
//---------------------------------------------------------------------------
class CJobQueue
{
private:
	// main job
	CJob *m_pj;

	// flag indicating if main job has completed
	BOOL m_fCompleted;

	// list of jobs waiting for main job to complete
	CList<CJob> m_listjQueued;

public:
	// enum indicating job queueing result
	enum EJobQueueResult
	{
		EjqrMain = 0,
		EjqrQueued,
		EjqrCompleted
	};

	// ctor
	CJobQueue() : m_pj(NULL), m_fCompleted(false)
	{
		m_listjQueued.Init(SPQOS_OFFSET(CJob, m_linkQueue));
	}

	// dtor
	~CJobQueue()
	{
		SPQOS_ASSERT_IMP(
			NULL != ITask::Self() && !ITask::Self()->HasPendingExceptions(),
			m_listjQueued.IsEmpty());
	}

	// reset job queue
	void
	Reset()
	{
		SPQOS_ASSERT(m_listjQueued.IsEmpty());

		m_pj = NULL;
		m_fCompleted = false;
	}

	// add job as a waiter;
	EJobQueueResult EjqrAdd(CJob *pj);

	// notify waiting jobs of job completion
	void NotifyCompleted(CSchedulerContext *psc);

#ifdef SPQOS_DEBUG
	// print queue - not thread-safe
	IOstream &OsPrintQueuedJobs(IOstream &);
#endif	// SPQOS_DEBUG

};	// class CJobQueue

}  // namespace spqopt

#endif	// !SPQOPT_CJobQueue_H


// EOF
