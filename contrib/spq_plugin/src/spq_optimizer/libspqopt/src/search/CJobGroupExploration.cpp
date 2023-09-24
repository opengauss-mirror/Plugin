//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CJobGroupExploration.cpp
//
//	@doc:
//		Implementation of group exploration job
//---------------------------------------------------------------------------

#include "spqopt/search/CJobGroupExploration.h"

#include "spqopt/engine/CEngine.h"
#include "spqopt/search/CGroup.h"
#include "spqopt/search/CGroupExpression.h"
#include "spqopt/search/CGroupProxy.h"
#include "spqopt/search/CJobFactory.h"
#include "spqopt/search/CJobGroupExpressionExploration.h"
#include "spqopt/search/CJobQueue.h"
#include "spqopt/search/CScheduler.h"
#include "spqopt/search/CSchedulerContext.h"

using namespace spqopt;

// State transition diagram for group exploration job state machine;
//
// +------------------------+
// |    estInitialized:     |
// | EevtStartExploration() |
// +------------------------+
//   |
//   | eevStartedExploration
//   v
// +------------------------+   eevNewChildren
// | estExploringChildren:  | -----------------+
// | EevtExploreChildren()  |                  |
// |                        | <----------------+
// +------------------------+
//   |
//   | eevExplored
//   v
// +------------------------+
// |      estCompleted      |
// +------------------------+
//
const CJobGroupExploration::EEvent
	rgeev[CJobGroupExploration::estSentinel]
		 [CJobGroupExploration::estSentinel] = {
			 {// estInitialized
			  CJobGroupExploration::eevSentinel,
			  CJobGroupExploration::eevStartedExploration,
			  CJobGroupExploration::eevSentinel},
			 {// estExploringChildren
			  CJobGroupExploration::eevSentinel,
			  CJobGroupExploration::eevNewChildren,
			  CJobGroupExploration::eevExplored},
			 {// estCompleted
			  CJobGroupExploration::eevSentinel,
			  CJobGroupExploration::eevSentinel,
			  CJobGroupExploration::eevSentinel},
};

#ifdef SPQOS_DEBUG

// names for states
const WCHAR
	rgwszStates[CJobGroupExploration::estSentinel][SPQOPT_FSM_NAME_LENGTH] = {
		SPQOS_WSZ_LIT("initialized"), SPQOS_WSZ_LIT("children explored"),
		SPQOS_WSZ_LIT("completed")};

// names for events
const WCHAR
	rgwszEvents[CJobGroupExploration::eevSentinel][SPQOPT_FSM_NAME_LENGTH] = {
		SPQOS_WSZ_LIT("started exploration"), SPQOS_WSZ_LIT("exploring children"),
		SPQOS_WSZ_LIT("finalized")};

#endif	// SPQOS_DEBUG


//---------------------------------------------------------------------------
//	@function:
//		CJobGroupExploration::CJobGroupExploration
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CJobGroupExploration::CJobGroupExploration()
{
}


//---------------------------------------------------------------------------
//	@function:
//		CJobGroupExploration::~CJobGroupExploration
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CJobGroupExploration::~CJobGroupExploration()
{
}


//---------------------------------------------------------------------------
//	@function:
//		CJobGroupExploration::Init
//
//	@doc:
//		Initialize job
//
//---------------------------------------------------------------------------
void
CJobGroupExploration::Init(CGroup *pgroup)
{
	CJobGroup::Init(pgroup);

	m_jsm.Init(rgeev
#ifdef SPQOS_DEBUG
			   ,
			   rgwszStates, rgwszEvents
#endif	// SPQOS_DEBUG
	);

	// set job actions
	m_jsm.SetAction(estInitialized, EevtStartExploration);
	m_jsm.SetAction(estExploringChildren, EevtExploreChildren);

	SetJobQueue(pgroup->PjqExploration());

	CJob::SetInit();
}



//---------------------------------------------------------------------------
//	@function:
//		CJobGroupExploration::FScheduleGroupExpressions
//
//	@doc:
//		Schedule exploration jobs for all unexplored group expressions;
//		the function returns true if it could schedule any new jobs
//
//---------------------------------------------------------------------------
BOOL
CJobGroupExploration::FScheduleGroupExpressions(CSchedulerContext *psc)
{
	CGroupExpression *pgexprLast = m_pgexprLastScheduled;

	// iterate on expressions and schedule them as needed
	CGroupExpression *pgexpr = PgexprFirstUnsched();
	while (NULL != pgexpr)
	{
		if (!pgexpr->FTransitioned(CGroupExpression::estExplored))
		{
			CJobGroupExpressionExploration::ScheduleJob(psc, pgexpr, this);
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
//		CJobGroupExploration::EevtStartExploration
//
//	@doc:
//		Start group exploration
//
//---------------------------------------------------------------------------
CJobGroupExploration::EEvent
CJobGroupExploration::EevtStartExploration(CSchedulerContext *,	 //psc
										   CJob *pjOwner)
{
	// get a job pointer
	CJobGroupExploration *pjge = PjConvert(pjOwner);
	CGroup *pgroup = pjge->m_pgroup;

	// move group to exploration state
	{
		CGroupProxy spq(pgroup);
		spq.SetState(CGroup::estExploring);
	}

	return eevStartedExploration;
}


//---------------------------------------------------------------------------
//	@function:
//		CJobGroupExploration::EevtExploreChildren
//
//	@doc:
//		Explore child group expressions
//
//---------------------------------------------------------------------------
CJobGroupExploration::EEvent
CJobGroupExploration::EevtExploreChildren(CSchedulerContext *psc, CJob *pjOwner)
{
	// get a job pointer
	CJobGroupExploration *pjge = PjConvert(pjOwner);
	if (pjge->FScheduleGroupExpressions(psc))
	{
		// new expressions have been added to group
		return eevNewChildren;
	}
	else
	{
		// no new expressions have been added to group, move to explored state
		{
			CGroupProxy spq(pjge->m_pgroup);
			spq.SetState(CGroup::estExplored);
		}

		// if this is the root, complete exploration phase
		if (psc->Peng()->FRoot(pjge->m_pgroup))
		{
			psc->Peng()->FinalizeExploration();
		}

		return eevExplored;
	}
}


//---------------------------------------------------------------------------
//	@function:
//		CJobGroupExploration::FExecute
//
//	@doc:
//		Main job function
//
//---------------------------------------------------------------------------
BOOL
CJobGroupExploration::FExecute(CSchedulerContext *psc)
{
	SPQOS_ASSERT(FInit());

	return m_jsm.FRun(psc, this);
}


//---------------------------------------------------------------------------
//	@function:
//		CJobGroupExploration::ScheduleJob
//
//	@doc:
//		Schedule a new group exploration job
//
//---------------------------------------------------------------------------
void
CJobGroupExploration::ScheduleJob(CSchedulerContext *psc, CGroup *pgroup,
								  CJob *pjParent)
{
	CJob *pj = psc->Pjf()->PjCreate(CJob::EjtGroupExploration);

	// initialize job
	CJobGroupExploration *pjge = PjConvert(pj);
	pjge->Init(pgroup);
	psc->Psched()->Add(pjge, pjParent);
}

#ifdef SPQOS_DEBUG

//---------------------------------------------------------------------------
//	@function:
//		CJobGroupExploration::OsPrint
//
//	@doc:
//		Print function
//
//---------------------------------------------------------------------------
IOstream &
CJobGroupExploration::OsPrint(IOstream &os) const
{
	return m_jsm.OsHistory(os);
}

#endif	// SPQOS_DEBUG

// EOF
