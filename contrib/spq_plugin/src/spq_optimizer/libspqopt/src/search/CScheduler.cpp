//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2008-2011 Greenplum, Inc.
//
//	@filename:
//		CScheduler.cpp
//
//	@doc:
//		Implementation of optimizer job scheduler
//---------------------------------------------------------------------------

#include "spqopt/search/CScheduler.h"

#include "spqos/base.h"
#include "spqos/error/CAutoTrace.h"

#include "spqopt/search/CJobFactory.h"
#include "spqopt/search/CSchedulerContext.h"
#include "naucrates/traceflags/traceflags.h"

using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CScheduler::CScheduler
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CScheduler::CScheduler(CMemoryPool *mp, ULONG ulJobs
#ifdef SPQOS_DEBUG
					   ,
					   BOOL fTrackingJobs
#endif	// SPQOS_DEBUG
					   )
	: m_spjl(mp, ulJobs),
	  m_ulpTotal(0),
	  m_ulpRunning(0),
	  m_ulpQueued(0),
	  m_ulpStatsQueued(0),
	  m_ulpStatsDequeued(0),
	  m_ulpStatsSuspended(0),
	  m_ulpStatsCompleted(0),
	  m_ulpStatsCompletedQueued(0),
	  m_ulpStatsResumed(0)
#ifdef SPQOS_DEBUG
	  ,
	  m_fTrackingJobs(fTrackingJobs)
#endif	// SPQOS_DEBUG
{
	// initialize pool of job links
	m_spjl.Init(SPQOS_OFFSET(SJobLink, m_id));

	// initialize list of waiting new jobs
	m_listjlWaiting.Init(SPQOS_OFFSET(SJobLink, m_link));

#ifdef SPQOS_DEBUG
	// initialize list of running jobs
	m_listjRunning.Init(SPQOS_OFFSET(CJob, m_linkRunning));

	// initialize list of suspended jobs
	m_listjSuspended.Init(SPQOS_OFFSET(CJob, m_linkSuspended));
#endif	// SPQOS_DEBUG
}


//---------------------------------------------------------------------------
//	@function:
//		CScheduler::~CScheduler
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CScheduler::~CScheduler()
{
	SPQOS_ASSERT_IMP(!ITask::Self()->HasPendingExceptions(), 0 == m_ulpTotal);
}


//---------------------------------------------------------------------------
//	@function:
//		CScheduler::Run
//
//	@doc:
//		Main job processing task
//
//---------------------------------------------------------------------------
void *
CScheduler::Run(void *pv)
{
	CSchedulerContext *psc = reinterpret_cast<CSchedulerContext *>(pv);
	psc->Psched()->ExecuteJobs(psc);

	return NULL;
}


//---------------------------------------------------------------------------
//	@function:
//		CScheduler::ExecuteJobs
//
//	@doc:
// 		Job processing loop;
//		keeps executing jobs as long as there is work queued;
//
//---------------------------------------------------------------------------
void
CScheduler::ExecuteJobs(CSchedulerContext *psc)
{
	CJob *pj = NULL;
	ULONG count = 0;

	// keep retrieving jobs
	while (NULL != (pj = PjRetrieve()))
	{
		// prepare for job execution
		PreExecute(pj);

		// execute job
		BOOL fCompleted = FExecute(pj, psc);

#ifdef SPQOS_DEBUG
		// restrict parallelism to keep track of jobs
		if (FTrackingJobs())
		{
			m_listjRunning.Remove(pj);
		}
#endif	// SPQOS_DEBUG

		// process job result
		switch (EjrPostExecute(pj, fCompleted))
		{
			case EjrCompleted:
				// job is completed
				Complete(pj);

#ifdef SPQOS_DEBUG
				if (SPQOS_FTRACE(EopttracePrintJobScheduler))
				{
					CAutoTrace at(psc->GetGlobalMemoryPool());

					at.Os() << "Print scheduler:" << std::endl << *this;
				}
#endif	// SPQOS_DEBUG

				psc->Pjf()->Release(pj);
				break;

			case EjrRunnable:
				// child jobs have completed, job can immediately resume
				Resume(pj);
				continue;

			case EjrSuspended:
				// job is suspended until child jobs complete
				Suspend(pj);
				break;

			default:
				SPQOS_ASSERT(!"Invalid job execution result");
		}

		if (++count == OPT_SCHED_CFA)
		{
			SPQOS_CHECK_ABORT;
			count = 0;
		}
	}
}


//---------------------------------------------------------------------------
//	@function:
//		CScheduler::Add
//
//	@doc:
//		Add new job for execution
//
//---------------------------------------------------------------------------
void
CScheduler::Add(CJob *pj, CJob *pjParent)
{
	SPQOS_ASSERT(NULL != pj);
	SPQOS_ASSERT(0 == pj->UlpRefs());

	// increment ref counter for parent job
	if (NULL != pjParent)
	{
		pjParent->IncRefs();
	}

	// set current job as parent of its child
	pj->SetParent(pjParent);

	// increment total number of jobs
	m_ulpTotal++;

	Schedule(pj);
}


//---------------------------------------------------------------------------
//	@function:
//		CScheduler::Resume
//
//	@doc:
//		Resume suspended job
//
//---------------------------------------------------------------------------
void
CScheduler::Resume(CJob *pj)
{
	SPQOS_ASSERT(NULL != pj);
	SPQOS_ASSERT(0 == pj->UlpRefs());

	Schedule(pj);
}


//---------------------------------------------------------------------------
//	@function:
//		CScheduler::Schedule
//
//	@doc:
//		Schedule job for execution
//
//---------------------------------------------------------------------------
void
CScheduler::Schedule(CJob *pj)
{
	SPQOS_ASSERT(NULL != pj);

	// get job link
	SJobLink *pjl = m_spjl.PtRetrieve();

	// throw OOM if no link can be retrieved
	if (NULL == pjl)
	{
		SPQOS_OOM_CHECK(NULL);
	}
	pjl->Init(pj);

#ifdef SPQOS_DEBUG
	if (FTrackingJobs())
	{
		SPQOS_ASSERT(CJob::EjsInit == pj->Ejs() ||
					CJob::EjsRunning == pj->Ejs() ||
					CJob::EjsSuspended == pj->Ejs());
		pj->SetState(CJob::EjsWaiting);
	}
#endif	// SPQOS_DEBUG

	// add to waiting list
	m_listjlWaiting.Push(pjl);

	// increment number of queued jobs
	m_ulpQueued++;

	// update statistics
	m_ulpStatsQueued++;
}


//---------------------------------------------------------------------------
//	@function:
//		CScheduler::PreExecute
//
//	@doc:
// 		Prepare for job execution
//
//---------------------------------------------------------------------------
void
CScheduler::PreExecute(CJob *pj)
{
	SPQOS_ASSERT(NULL != pj);
	SPQOS_ASSERT(0 == pj->UlpRefs() &&
				"Runnable job cannot have pending children");

	// increment number of running jobs
	m_ulpRunning++;

	// increment job ref counter
	pj->IncRefs();
}


//---------------------------------------------------------------------------
//	@function:
//		CScheduler::FExecute
//
//	@doc:
//		Execution function using job queue
//
//---------------------------------------------------------------------------
BOOL
CScheduler::FExecute(CJob *pj, CSchedulerContext *psc)
{
	BOOL fCompleted = true;
	CJobQueue *pjq = pj->Pjq();

	// check if job is associated to a job queue
	if (NULL == pjq)
	{
		fCompleted = pj->FExecute(psc);
	}
	else
	{
		switch (pjq->EjqrAdd(pj))
		{
			case CJobQueue::EjqrMain:
				// main job, runs job operation
				fCompleted = pj->FExecute(psc);
				if (fCompleted)
				{
					// notify queued jobs
					pjq->NotifyCompleted(psc);
				}
				else
				{
					// task is suspended
					(void) pj->UlpDecrRefs();
				}
				break;

			case CJobQueue::EjqrQueued:
				// queued job
				fCompleted = false;
				break;

			case CJobQueue::EjqrCompleted:
				break;
		}
	}

	return fCompleted;
}


//---------------------------------------------------------------------------
//	@function:
//		CScheduler::EjrPostExecute
//
//	@doc:
// 		Process job execution outcome
//
//---------------------------------------------------------------------------
CScheduler::EJobResult
CScheduler::EjrPostExecute(CJob *pj, BOOL fCompleted)
{
	SPQOS_ASSERT(NULL != pj);
	SPQOS_ASSERT(0 < pj->UlpRefs() && "IsRunning job is marked as completed");

	// decrement job ref counter
	ULONG_PTR ulRefs = pj->UlpDecrRefs();

	// decrement number of running jobs
	m_ulpRunning--;

	// check if job completed
	if (fCompleted)
	{
		SPQOS_ASSERT(1 == ulRefs);

		return EjrCompleted;
	}

	// check if all children have completed
	if (1 == ulRefs)
	{
		return EjrRunnable;
	}

	return EjrSuspended;
}


//---------------------------------------------------------------------------
//	@function:
//		CScheduler::PjRetrieve
//
//	@doc:
//		Retrieve next runnable job from queue
//
//---------------------------------------------------------------------------
CJob *
CScheduler::PjRetrieve()
{
	// retrieve runnable job from lists of waiting jobs
	SJobLink *pjl = m_listjlWaiting.Pop();
	CJob *pj = NULL;

	if (NULL != pjl)
	{
		pj = pjl->m_pj;

		SPQOS_ASSERT(NULL != pj);
		SPQOS_ASSERT(0 == pj->UlpRefs());

		// decrement number of queued jobs
		m_ulpQueued--;

		// update statistics
		m_ulpStatsDequeued++;

		// recycle job link
		m_spjl.Recycle(pjl);

#ifdef SPQOS_DEBUG
		// add job to running list
		if (FTrackingJobs())
		{
			SPQOS_ASSERT(CJob::EjsWaiting == pj->Ejs());

			pj->SetState(CJob::EjsRunning);
			m_listjRunning.Append(pj);
		}
#endif	// SPQOS_DEBUG
	}

	return pj;
}


//---------------------------------------------------------------------------
//	@function:
//		CScheduler::Suspend
//
//	@doc:
//		Transition job to suspended
//
//---------------------------------------------------------------------------
void
CScheduler::Suspend(CJob *
#ifdef SPQOS_DEBUG
						pj
#endif	// SPQOS_DEBUG
)
{
	SPQOS_ASSERT(NULL != pj);

#ifdef SPQOS_DEBUG
	if (FTrackingJobs())
	{
		SPQOS_ASSERT(CJob::EjsRunning == pj->Ejs());

		pj->SetState(CJob::EjsSuspended);
		m_listjSuspended.Append(pj);
	}
#endif	// SPQOS_DEBUG)

	m_ulpStatsSuspended++;
}


//---------------------------------------------------------------------------
//	@function:
//		CScheduler::Complete
//
//	@doc:
//		Transition job to completed
//
//---------------------------------------------------------------------------
void
CScheduler::Complete(CJob *pj)
{
	SPQOS_ASSERT(0 == pj->UlpRefs());

#ifdef SPQOS_DEBUG
	if (FTrackingJobs())
	{
		SPQOS_ASSERT(CJob::EjsRunning == pj->Ejs());

		pj->SetState(CJob::EjsCompleted);
	}
#endif	// SPQOS_DEBUG

	ResumeParent(pj);

	// update statistics
	m_ulpTotal--;
	m_ulpStatsCompleted++;
}


//---------------------------------------------------------------------------
//	@function:
//		CScheduler::CompleteQueued
//
//	@doc:
//		Transition queued job to completed
//
//---------------------------------------------------------------------------
void
CScheduler::CompleteQueued(CJob *pj)
{
	SPQOS_ASSERT(0 == pj->UlpRefs());

#ifdef SPQOS_DEBUG
	if (FTrackingJobs())
	{
		SPQOS_ASSERT(CJob::EjsSuspended == pj->Ejs());

		m_listjSuspended.Remove(pj);
		pj->SetState(CJob::EjsCompleted);
	}
#endif	// SPQOS_DEBUG

	ResumeParent(pj);

	// update statistics
	m_ulpTotal--;
	m_ulpStatsCompleted++;
	m_ulpStatsCompletedQueued++;
}


//---------------------------------------------------------------------------
//	@function:
//		CScheduler::ResumeParent
//
//	@doc:
//		Resume parent job
//
//---------------------------------------------------------------------------
void
CScheduler::ResumeParent(CJob *pj)
{
	SPQOS_ASSERT(0 == pj->UlpRefs());

	CJob *pjParent = pj->PjParent();

	if (NULL != pjParent)
	{
		// notify parent job
		if (pj->FResumeParent())
		{
#ifdef SPQOS_DEBUG
			if (FTrackingJobs())
			{
				m_listjSuspended.Remove(pjParent);
			}
#endif	// SPQOS_DEBUG)

			// reschedule parent
			Resume(pjParent);

			// update statistics
			m_ulpStatsResumed++;
		}
	}
}


//---------------------------------------------------------------------------
//	@function:
//		CScheduler::PrintStats
//
//	@doc:
//		Print statistics
//
//---------------------------------------------------------------------------
void
CScheduler::PrintStats() const
{
	SPQOS_TRACE_FORMAT(
		"Job statistics: Queued=%d Dequeued=%d Suspended=%d "
		"Resumed=%d CompletedQueued=%d Completed=%d",
		m_ulpStatsQueued, m_ulpStatsDequeued, m_ulpStatsSuspended,
		m_ulpStatsResumed, m_ulpStatsCompletedQueued, m_ulpStatsCompleted);
}


#ifdef SPQOS_DEBUG

//---------------------------------------------------------------------------
//	@function:
//		CScheduler::OsPrintActiveJobs
//
//	@doc:
//		Print scheduler queue
//
//---------------------------------------------------------------------------
IOstream &
CScheduler::OsPrintActiveJobs(IOstream &os)
{
	os << "Scheduler - active jobs: " << std::endl << std::endl;

	os << "List of running jobs: " << std::endl;
	CJob *pj = m_listjRunning.First();
	while (NULL != pj)
	{
		pj->OsPrint(os);
		pj = m_listjRunning.Next(pj);
	}

	os << std::endl << "List of waiting jobs: " << std::endl;

	SJobLink *pjl = m_listjlWaiting.PtFirst();
	while (NULL != pjl)
	{
		pjl->m_pj->OsPrint(os);
		pjl = m_listjlWaiting.Next(pjl);
	}

	os << std::endl << "List of suspended jobs: " << std::endl;
	pj = m_listjSuspended.First();
	while (NULL != pj)
	{
		pj->OsPrint(os);
		pj = m_listjSuspended.Next(pj);
	}

	return os;
}


#endif	// SPQOS_DEBUG

// EOF
