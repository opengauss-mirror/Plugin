//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CJobQueue.cpp
//
//	@doc:
//		Implementation of class controlling unique execution of an operation
//		that is potentially assigned to many jobs.

//	@owner:
//
//
//	@test:
//
//
//---------------------------------------------------------------------------

#include "spqopt/search/CJobQueue.h"

#include "spqopt/search/CJobFactory.h"
#include "spqopt/search/CScheduler.h"
#include "spqopt/search/CSchedulerContext.h"

using namespace spqos;
using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CJobQueue::EjqrAdd
//
//	@doc:
//		Add job as a waiter;
//
//---------------------------------------------------------------------------
CJobQueue::EJobQueueResult
CJobQueue::EjqrAdd(CJob *pj)
{
	SPQOS_ASSERT(NULL != pj);

	EJobQueueResult ejer = EjqrCompleted;

	// check if job has completed before getting the lock
	if (!m_fCompleted)
	{
		// check if this is the main job
		if (pj == m_pj)
		{
			SPQOS_ASSERT(!m_fCompleted);
			ejer = EjqrMain;
			pj->IncRefs();
		}
		else
		{
			// check if job is completed
			if (!m_fCompleted)
			{
				m_listjQueued.Append(pj);
				BOOL fOwner = (pj == m_listjQueued.First());

				// first caller becomes the owner
				if (fOwner)
				{
					SPQOS_ASSERT(NULL == m_pj);

					m_pj = pj;
					ejer = EjqrMain;
				}
				else
				{
					ejer = EjqrQueued;
				}

				pj->IncRefs();
			}
		}
	}

	return ejer;
}


//---------------------------------------------------------------------------
//	@function:
//		CJobQueue::NotifyCompleted
//
//	@doc:
//		Notify waiting jobs of job completion
//
//---------------------------------------------------------------------------
void
CJobQueue::NotifyCompleted(CSchedulerContext *psc)
{
	SPQOS_ASSERT(!m_fCompleted);
	m_fCompleted = true;

	SPQOS_ASSERT(!m_listjQueued.IsEmpty());
	while (!m_listjQueued.IsEmpty())
	{
		CJob *pj = m_listjQueued.RemoveHead();

		// check if job execution has completed
		if (1 == pj->UlpDecrRefs())
		{
			// update job as completed
			psc->Psched()->CompleteQueued(pj);

			// recycle job
			psc->Pjf()->Release(pj);
		}
	}
}

#ifdef SPQOS_DEBUG

//---------------------------------------------------------------------------
//	@function:
//		CJobQueue::OsPrintQueuedJobs
//
//	@doc:
//		Print queue - not thread-safe
//
//---------------------------------------------------------------------------
IOstream &
CJobQueue::OsPrintQueuedJobs(IOstream &os)
{
	os << "Job queue: " << std::endl;

	CJob *pj = m_listjQueued.First();
	while (NULL != pj)
	{
		pj->OsPrint(os);
		pj = m_listjQueued.Next(pj);
	}

	return os;
}

#endif	// SPQOS_DEBUG

// EOF
