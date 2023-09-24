//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CJobTest.cpp
//
//	@doc:
//		Implementation of optimizer job test class
//---------------------------------------------------------------------------

#include "spqopt/search/CJobTest.h"

#include "spqos/io/COstreamString.h"
#include "spqos/string/CWStringDynamic.h"

#include "spqopt/search/CJobFactory.h"
#include "spqopt/search/CScheduler.h"
#include "spqopt/search/CSchedulerContext.h"

using namespace spqopt;
using namespace spqos;

#define SPQOPT_JOB_TEST_DUMMY_CONST 45

// initialization of static members
ULONG_PTR CJobTest::m_ulpCnt;

//---------------------------------------------------------------------------
//	@function:
//		CJobTest::CJobTest
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CJobTest::CJobTest()
	: CJob(),
	  m_ett(EttSpawn),
	  m_ulRounds(spqos::ulong_max),
	  m_ulFanout(spqos::ulong_max),
	  m_ulIters(spqos::ulong_max)
{
}


//---------------------------------------------------------------------------
//	@function:
//		CJobTest::~CJobTest
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CJobTest::~CJobTest()
{
}


//---------------------------------------------------------------------------
//	@function:
//		CJobTest::FExecute
//
//	@doc:
//		Execution of test job
//
//---------------------------------------------------------------------------
BOOL
CJobTest::FExecute(CSchedulerContext *psc)
{
	BOOL fRes = false;

	switch (m_ett)
	{
		case EttSpawn:
			fRes = FSpawn(psc);
			break;

		case EttQueueu:
			fRes = FQueue(psc);
			break;

		case EttStartQueue:
			fRes = FStartQueue(psc);
			break;
	}

	return fRes;
}


//---------------------------------------------------------------------------
//	@function:
//		CJobTest::FSpawn
//
//	@doc:
//		Test job spawning
//
//---------------------------------------------------------------------------
BOOL
CJobTest::FSpawn(CSchedulerContext *psc)
{
	ULONG_PTR ulpOffset = m_ulpCnt++;

#ifdef SPQOS_DEBUG
	if (10 == ulpOffset && psc->Psched()->FTrackingJobs())
	{
		CWStringDynamic str(psc->GetGlobalMemoryPool());
		COstreamString oss(&str);

		psc->Psched()->OsPrintActiveJobs(oss);

		SPQOS_TRACE(str.GetBuffer());
	}
#endif	// SPQOS_DEBUG

	if (m_ulRounds > ulpOffset)
	{
		for (ULONG i = 0; i < m_ulFanout; i++)
		{
			// get new job from factory
			CJob *pj = psc->Pjf()->PjCreate(CJob::EjtTest);

			// initialize test job
			CJobTest *pjt = PjConvert(pj);
			pjt->Init(this);

			// schedule new job for execution as child
			psc->Psched()->Add(pj, this);

			SPQOS_CHECK_ABORT;
		}

		// after forking jobs burn a few CPU cycles
		// to simulate an actual transformation
		Loop();

		return false;
	}

	return true;
}


//---------------------------------------------------------------------------
//	@function:
//		CJobTest::FStartQueue
//
//	@doc:
//		Start jobs to be queued
//
//---------------------------------------------------------------------------
BOOL
CJobTest::FStartQueue(CSchedulerContext *psc)
{
	ULONG_PTR ulpOffset = m_ulpCnt++;

	if (0 == ulpOffset)
	{
		for (ULONG i = 0; i < m_ulFanout; i++)
		{
			// get new job from factory
			CJob *pj = psc->Pjf()->PjCreate(CJob::EjtTest);

			// initialize test job
			CJobTest *pjt = PjConvert(pj);
			pjt->Init(CJobTest::EttQueueu, m_ulRounds, m_ulFanout, m_ulIters,
					  m_pjq);

			// schedule new job for execution as child
			psc->Psched()->Add(pj, this);

			SPQOS_CHECK_ABORT;
		}

		return false;
	}

	return true;
}


//---------------------------------------------------------------------------
//	@function:
//		CJobTest::FQueue
//
//	@doc:
//		Test job queueing
//
//---------------------------------------------------------------------------
BOOL
CJobTest::FQueue(CSchedulerContext *psc)
{
	BOOL fCompleted = true;

	switch (m_pjq->EjqrAdd(this))
	{
		case CJobQueue::EjqrMain:
			if (10 > m_ulFanout)
			{
				SPQOS_TRACE(SPQOS_WSZ_LIT("Queued job is not executing -> run"));
			}
			Loop();
#ifdef SPQOS_DEBUG
			if (10 > m_ulFanout)
			{
				CWStringDynamic str(psc->GetGlobalMemoryPool());
				COstreamString oss(&str);
				m_pjq->OsPrintQueuedJobs(oss);

				SPQOS_TRACE(str.GetBuffer());
			}
#endif	// SPQOS_DEBUG
			m_pjq->NotifyCompleted(psc);
			break;
			;

		case CJobQueue::EjqrQueued:
			if (10 > m_ulFanout)
			{
				SPQOS_TRACE(SPQOS_WSZ_LIT("Queued job is executing -> wait"));
			}
			fCompleted = false;
			break;

		case CJobQueue::EjqrCompleted:
			if (10 > m_ulFanout)
			{
				SPQOS_TRACE(SPQOS_WSZ_LIT("Queued job has already completed"));
			}
			break;
	}

	return fCompleted;
}


//---------------------------------------------------------------------------
//	@function:
//		CJobTest::Loop
//
//	@doc:
//		Burn some CPU to simulate actual work
//
//---------------------------------------------------------------------------
void
CJobTest::Loop()
{
	ULONG ulOuter = 0;
	while (ulOuter < m_ulIters)
	{
		for (ULONG ulInner = ulOuter; ulInner > 0; ulInner--)
		{
			if (0 < ulOuter * (ulOuter + SPQOPT_JOB_TEST_DUMMY_CONST))
			{
				ulOuter++;
			}
		}
		ulOuter++;

		SPQOS_CHECK_ABORT;
	}
}


#ifdef SPQOS_DEBUG

//---------------------------------------------------------------------------
//	@function:
//		CJobTest::OsPrint
//
//	@doc:
//		Print job
//
//---------------------------------------------------------------------------
IOstream &
CJobTest::OsPrint(IOstream &os) const
{
	os << "Test job, ";
	return CJob::OsPrint(os);
}

#endif	// SPQOS_DEBUG

// EOF
