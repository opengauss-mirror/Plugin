//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CJobGroupImplementation.cpp
//
//	@doc:
//		Implementation of group implementation job
//---------------------------------------------------------------------------

#include "spqopt/search/CJobGroupImplementation.h"

#include "spqopt/base/CQueryContext.h"
#include "spqopt/engine/CEngine.h"
#include "spqopt/search/CGroup.h"
#include "spqopt/search/CGroupExpression.h"
#include "spqopt/search/CGroupProxy.h"
#include "spqopt/search/CJobFactory.h"
#include "spqopt/search/CJobGroupExploration.h"
#include "spqopt/search/CJobGroupExpressionImplementation.h"
#include "spqopt/search/CJobQueue.h"
#include "spqopt/search/CScheduler.h"
#include "spqopt/search/CSchedulerContext.h"
#include "naucrates/traceflags/traceflags.h"

using namespace spqopt;


// State transition diagram for group implementation job state machine;
//
// +---------------------------+   eevExploring
// |      estInitialized:      | ------------------+
// | EevtStartImplementation() |                   |
// |                           | <-----------------+
// +---------------------------+
//   |
//   | eevExplored
//   v
// +---------------------------+   eevImplementing
// | estImplementingChildren:  | ------------------+
// |  EevtImplementChildren()  |                   |
// |                           | <-----------------+
// +---------------------------+
//   |
//   | eevImplemented
//   v
// +---------------------------+
// |       estCompleted        |
// +---------------------------+
//
const CJobGroupImplementation::EEvent
	rgeev[CJobGroupImplementation::estSentinel]
		 [CJobGroupImplementation::estSentinel] = {
			 {// estInitialized
			  CJobGroupImplementation::eevExploring,
			  CJobGroupImplementation::eevExplored,
			  CJobGroupImplementation::eevSentinel},
			 {// estImplementingChildren
			  CJobGroupImplementation::eevSentinel,
			  CJobGroupImplementation::eevImplementing,
			  CJobGroupImplementation::eevImplemented},
			 {// estCompleted
			  CJobGroupImplementation::eevSentinel,
			  CJobGroupImplementation::eevSentinel,
			  CJobGroupImplementation::eevSentinel},
};

#ifdef SPQOS_DEBUG

// names for states
const WCHAR
	rgwszStates[CJobGroupImplementation::estSentinel][SPQOPT_FSM_NAME_LENGTH] = {
		SPQOS_WSZ_LIT("initialized"), SPQOS_WSZ_LIT("implementing children"),
		SPQOS_WSZ_LIT("completed")};

// names for events
const WCHAR
	rgwszEvents[CJobGroupImplementation::eevSentinel][SPQOPT_FSM_NAME_LENGTH] = {
		SPQOS_WSZ_LIT("ongoing exploration"), SPQOS_WSZ_LIT("done exploration"),
		SPQOS_WSZ_LIT("ongoing implementation"), SPQOS_WSZ_LIT("finalizing")};

#endif	// SPQOS_DEBUG


//---------------------------------------------------------------------------
//	@function:
//		CJobGroupImplementation::CJobGroupImplementation
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CJobGroupImplementation::CJobGroupImplementation()
{
}


//---------------------------------------------------------------------------
//	@function:
//		CJobGroupImplementation::~CJobGroupImplementation
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CJobGroupImplementation::~CJobGroupImplementation()
{
}


//---------------------------------------------------------------------------
//	@function:
//		CJobGroupImplementation::Init
//
//	@doc:
//		Initialize job
//
//---------------------------------------------------------------------------
void
CJobGroupImplementation::Init(CGroup *pgroup)
{
	CJobGroup::Init(pgroup);

	m_jsm.Init(rgeev
#ifdef SPQOS_DEBUG
			   ,
			   rgwszStates, rgwszEvents
#endif	// SPQOS_DEBUG
	);

	// set job actions
	m_jsm.SetAction(estInitialized, EevtStartImplementation);
	m_jsm.SetAction(estImplementingChildren, EevtImplementChildren);

	SetJobQueue(pgroup->PjqImplementation());

	CJob::SetInit();
}


//---------------------------------------------------------------------------
//	@function:
//		CJobGroupImplementation::FScheduleGroupExpressions
//
//	@doc:
//		Schedule implementation jobs for all unimplemented group expressions;
//		the function returns true if it could schedule any new jobs
//
//---------------------------------------------------------------------------
BOOL
CJobGroupImplementation::FScheduleGroupExpressions(CSchedulerContext *psc)
{
	CGroupExpression *pgexprLast = m_pgexprLastScheduled;

	// iterate on expression and schedule them as needed
	CGroupExpression *pgexpr = PgexprFirstUnsched();
	while (NULL != pgexpr)
	{
		if (!pgexpr->FTransitioned(CGroupExpression::estImplemented) &&
			!pgexpr->ContainsCircularDependencies())
		{
			CJobGroupExpressionImplementation::ScheduleJob(psc, pgexpr, this);
			pgexprLast = pgexpr;
		}

		// move to next expression
		{
			CGroupProxy spq(m_pgroup);
			pgexpr = spq.PgexprNext(pgexpr);
		}
	}

	BOOL fNewJobs = (m_pgexprLastScheduled != pgexprLast);

	// set last scheduled expression
	m_pgexprLastScheduled = pgexprLast;

	return fNewJobs;
}


//---------------------------------------------------------------------------
//	@function:
//		CJobGroupImplementation::EevtStartImplementation
//
//	@doc:
//		Start group implementation
//
//---------------------------------------------------------------------------
CJobGroupImplementation::EEvent
CJobGroupImplementation::EevtStartImplementation(CSchedulerContext *psc,
												 CJob *pjOwner)
{
	// get a job pointer
	CJobGroupImplementation *pjgi = PjConvert(pjOwner);
	CGroup *pgroup = pjgi->m_pgroup;

	if (!pgroup->FExplored())
	{
		// schedule a child exploration job
		CJobGroupExploration::ScheduleJob(psc, pgroup, pjgi);
		return eevExploring;
	}
	else
	{
		// move group to implementation state
		{
			CGroupProxy spq(pgroup);
			spq.SetState(CGroup::estImplementing);
		}

		// if this is the root, release exploration jobs
		if (psc->Peng()->FRoot(pgroup))
		{
			psc->Pjf()->Truncate(EjtGroupExploration);
			psc->Pjf()->Truncate(EjtGroupExpressionExploration);
		}

		return eevExplored;
	}
}


//---------------------------------------------------------------------------
//	@function:
//		CJobGroupImplementation::EevtImplementChildren
//
//	@doc:
//		Implement child group expressions
//
//---------------------------------------------------------------------------
CJobGroupImplementation::EEvent
CJobGroupImplementation::EevtImplementChildren(CSchedulerContext *psc,
											   CJob *pjOwner)
{
	// get a job pointer
	CJobGroupImplementation *pjgi = PjConvert(pjOwner);
	if (pjgi->FScheduleGroupExpressions(psc))
	{
		// implementation is in progress
		return eevImplementing;
	}
	else
	{
		// move group to implemented state
		{
			CGroupProxy spq(pjgi->m_pgroup);
			spq.SetState(CGroup::estImplemented);
		}

		// if this is the root, complete implementation phase
		if (psc->Peng()->FRoot(pjgi->m_pgroup))
		{
			psc->Peng()->FinalizeImplementation();
		}

		return eevImplemented;
	}
}


//---------------------------------------------------------------------------
//	@function:
//		CJobGroupImplementation::FExecute
//
//	@doc:
//		Main job function
//
//---------------------------------------------------------------------------
BOOL
CJobGroupImplementation::FExecute(CSchedulerContext *psc)
{
	SPQOS_ASSERT(FInit());

	return m_jsm.FRun(psc, this);
}


//---------------------------------------------------------------------------
//	@function:
//		CJobGroupImplementation::ScheduleJob
//
//	@doc:
//		Schedule a new group implementation job
//
//---------------------------------------------------------------------------
void
CJobGroupImplementation::ScheduleJob(CSchedulerContext *psc, CGroup *pgroup,
									 CJob *pjParent)
{
	CJob *pj = psc->Pjf()->PjCreate(CJob::EjtGroupImplementation);

	// initialize job
	CJobGroupImplementation *pjgi = PjConvert(pj);
	pjgi->Init(pgroup);
	psc->Psched()->Add(pjgi, pjParent);
}


#ifdef SPQOS_DEBUG

//---------------------------------------------------------------------------
//	@function:
//		CJobGroupImplementation::OsPrint
//
//	@doc:
//		Print function
//
//---------------------------------------------------------------------------
IOstream &
CJobGroupImplementation::OsPrint(IOstream &os) const
{
	return m_jsm.OsHistory(os);
}

#endif	// SPQOS_DEBUG

// EOF
