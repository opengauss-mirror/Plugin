//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 Greenplum, Inc.
//
//	@filename:
//		CJobGroupExpressionImplementation.cpp
//
//	@doc:
//		Implementation of group expression implementation job
//---------------------------------------------------------------------------

#include "spqopt/search/CJobGroupExpressionImplementation.h"

#include "spqopt/engine/CEngine.h"
#include "spqopt/operators/CLogical.h"
#include "spqopt/search/CGroup.h"
#include "spqopt/search/CGroupExpression.h"
#include "spqopt/search/CJobFactory.h"
#include "spqopt/search/CJobGroupImplementation.h"
#include "spqopt/search/CJobTransformation.h"
#include "spqopt/search/CScheduler.h"
#include "spqopt/search/CSchedulerContext.h"
#include "spqopt/xforms/CXformFactory.h"


using namespace spqopt;

// State transition diagram for group expression implementation job state machine;
//
// +-------------------------+   eevImplementingChildren
// |     estInitialized:     | --------------------------+
// | EevtImplementChildren() |                           |
// |                         | <-------------------------+
// +-------------------------+
//   |
//   | eevChildrenImplemented
//   v
// +-------------------------+   eevImplementingSelf
// | estChildrenImplemented: | --------------------------+
// |   EevtImplementSelf()   |                           |
// |                         | <-------------------------+
// +-------------------------+
//   |
//   | eevSelfImplemented
//   v
// +-------------------------+
// |   estSelfImplemented:   |
// |     EevtFinalize()      |
// +-------------------------+
//   |
//   | eevFinalized
//   v
// +-------------------------+
// |      estCompleted       |
// +-------------------------+
//
const CJobGroupExpressionImplementation::EEvent
	rgeev[CJobGroupExpressionImplementation::estSentinel]
		 [CJobGroupExpressionImplementation::estSentinel] = {
			 {// estInitialized
			  CJobGroupExpressionImplementation::eevImplementingChildren,
			  CJobGroupExpressionImplementation::eevChildrenImplemented,
			  CJobGroupExpressionImplementation::eevSentinel,
			  CJobGroupExpressionImplementation::eevSentinel},
			 {// estChildrenImplemented
			  CJobGroupExpressionImplementation::eevSentinel,
			  CJobGroupExpressionImplementation::eevImplementingSelf,
			  CJobGroupExpressionImplementation::eevSelfImplemented,
			  CJobGroupExpressionImplementation::eevSentinel},
			 {// estSelfImplemented
			  CJobGroupExpressionImplementation::eevSentinel,
			  CJobGroupExpressionImplementation::eevSentinel,
			  CJobGroupExpressionImplementation::eevSentinel,
			  CJobGroupExpressionImplementation::eevFinalized},
			 {// estCompleted
			  CJobGroupExpressionImplementation::eevSentinel,
			  CJobGroupExpressionImplementation::eevSentinel,
			  CJobGroupExpressionImplementation::eevSentinel,
			  CJobGroupExpressionImplementation::eevSentinel},
};

#ifdef SPQOS_DEBUG

// names for states
const WCHAR rgwszStates[CJobGroupExpressionImplementation::estSentinel]
					   [SPQOPT_FSM_NAME_LENGTH] = {
						   SPQOS_WSZ_LIT("initialized"),
						   SPQOS_WSZ_LIT("children implemented"),
						   SPQOS_WSZ_LIT("self implemented"),
						   SPQOS_WSZ_LIT("completed")};

// names for events
const WCHAR rgwszEvents[CJobGroupExpressionImplementation::eevSentinel]
					   [SPQOPT_FSM_NAME_LENGTH] = {
						   SPQOS_WSZ_LIT("implementing child groups"),
						   SPQOS_WSZ_LIT("implemented children groups"),
						   SPQOS_WSZ_LIT("applying implementation xforms"),
						   SPQOS_WSZ_LIT("applied implementation xforms"),
						   SPQOS_WSZ_LIT("finalized")};

#endif	// SPQOS_DEBUG


//---------------------------------------------------------------------------
//	@function:
//		CJobGroupExpressionImplementation::CJobGroupExpressionImplementation
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CJobGroupExpressionImplementation::CJobGroupExpressionImplementation()
{
}


//---------------------------------------------------------------------------
//	@function:
//		CJobGroupExpressionImplementation::~CJobGroupExpressionImplementation
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CJobGroupExpressionImplementation::~CJobGroupExpressionImplementation()
{
}


//---------------------------------------------------------------------------
//	@function:
//		CJobGroupExpressionImplementation::Init
//
//	@doc:
//		Initialize job
//
//---------------------------------------------------------------------------
void
CJobGroupExpressionImplementation::Init(CGroupExpression *pgexpr)
{
	CJobGroupExpression::Init(pgexpr);
	SPQOS_ASSERT(pgexpr->Pop()->FLogical());

	m_jsm.Init(rgeev
#ifdef SPQOS_DEBUG
			   ,
			   rgwszStates, rgwszEvents
#endif	// SPQOS_DEBUG
	);

	// set job actions
	m_jsm.SetAction(estInitialized, EevtImplementChildren);
	m_jsm.SetAction(estChildrenImplemented, EevtImplementSelf);
	m_jsm.SetAction(estSelfImplemented, EevtFinalize);

	CJob::SetInit();
}


//---------------------------------------------------------------------------
//	@function:
//		CJobGroupExpressionImplementation::ScheduleApplicableTransformations
//
//	@doc:
//		Schedule transformation jobs for all applicable xforms
//
//---------------------------------------------------------------------------
void
CJobGroupExpressionImplementation::ScheduleApplicableTransformations(
	CSchedulerContext *psc)
{
	SPQOS_ASSERT(!FXformsScheduled());

	// get all applicable xforms
	COperator *pop = m_pgexpr->Pop();
	CXformSet *xform_set =
		CLogical::PopConvert(pop)->PxfsCandidates(psc->GetGlobalMemoryPool());

	// intersect them with required xforms and schedule jobs
	xform_set->Intersection(CXformFactory::Pxff()->PxfsImplementation());
	xform_set->Intersection(psc->Peng()->PxfsCurrentStage());
	ScheduleTransformations(psc, xform_set);
	xform_set->Release();

	SetXformsScheduled();
}


//---------------------------------------------------------------------------
//	@function:
//		CJobGroupExpressionImplementation::ScheduleChildGroupsJobs
//
//	@doc:
//		Schedule implementation jobs for all child groups
//
//---------------------------------------------------------------------------
void
CJobGroupExpressionImplementation::ScheduleChildGroupsJobs(
	CSchedulerContext *psc)
{
	SPQOS_ASSERT(!FChildrenScheduled());

	ULONG arity = m_pgexpr->Arity();

	for (ULONG i = 0; i < arity; i++)
	{
		CJobGroupImplementation::ScheduleJob(psc, (*(m_pgexpr))[i], this);
	}

	SetChildrenScheduled();
}


//---------------------------------------------------------------------------
//	@function:
//		CJobGroupExpressionImplementation::EevtImplementChildren
//
//	@doc:
//		Implement child groups
//
//---------------------------------------------------------------------------
CJobGroupExpressionImplementation::EEvent
CJobGroupExpressionImplementation::EevtImplementChildren(CSchedulerContext *psc,
														 CJob *pjOwner)
{
	// get a job pointer
	CJobGroupExpressionImplementation *pjgei = PjConvert(pjOwner);
	SPQOS_ASSERT(pjgei->m_pgexpr->FExplored());

	if (!pjgei->FChildrenScheduled())
	{
		pjgei->m_pgexpr->SetState(CGroupExpression::estImplementing);
		pjgei->ScheduleChildGroupsJobs(psc);

		return eevImplementingChildren;
	}
	else
	{
		return eevChildrenImplemented;
	}
}


//---------------------------------------------------------------------------
//	@function:
//		CJobGroupExpressionImplementation::EevtImplementSelf
//
//	@doc:
//		Implement group expression
//
//---------------------------------------------------------------------------
CJobGroupExpressionImplementation::EEvent
CJobGroupExpressionImplementation::EevtImplementSelf(CSchedulerContext *psc,
													 CJob *pjOwner)
{
	// get a job pointer
	CJobGroupExpressionImplementation *pjgei = PjConvert(pjOwner);
	if (!pjgei->FXformsScheduled())
	{
		pjgei->ScheduleApplicableTransformations(psc);
		return eevImplementingSelf;
	}
	else
	{
		return eevSelfImplemented;
	}
}


//---------------------------------------------------------------------------
//	@function:
//		CJobGroupExpressionImplementation::EevtFinalize
//
//	@doc:
//		Finalize implementation
//
//---------------------------------------------------------------------------
CJobGroupExpressionImplementation::EEvent
CJobGroupExpressionImplementation::EevtFinalize(CSchedulerContext *,  //psc
												CJob *pjOwner)
{
	// get a job pointer
	CJobGroupExpressionImplementation *pjgei = PjConvert(pjOwner);
	pjgei->m_pgexpr->SetState(CGroupExpression::estImplemented);

	return eevFinalized;
}


//---------------------------------------------------------------------------
//	@function:
//		CJobGroupExpressionImplementation::FExecute
//
//	@doc:
//		Main job function
//
//---------------------------------------------------------------------------
BOOL
CJobGroupExpressionImplementation::FExecute(CSchedulerContext *psc)
{
	SPQOS_ASSERT(FInit());

	return m_jsm.FRun(psc, this);
}


//---------------------------------------------------------------------------
//	@function:
//		CJobGroupExpressionImplementation::ScheduleJob
//
//	@doc:
//		Schedule a new group expression implementation job
//
//---------------------------------------------------------------------------
void
CJobGroupExpressionImplementation::ScheduleJob(CSchedulerContext *psc,
											   CGroupExpression *pgexpr,
											   CJob *pjParent)
{
	CJob *pj = psc->Pjf()->PjCreate(CJob::EjtGroupExpressionImplementation);

	// initialize job
	CJobGroupExpressionImplementation *pjige = PjConvert(pj);
	pjige->Init(pgexpr);
	psc->Psched()->Add(pjige, pjParent);
}

#ifdef SPQOS_DEBUG

//---------------------------------------------------------------------------
//	@function:
//		CJobGroupExpressionImplementation::OsPrint
//
//	@doc:
//		Print function
//
//---------------------------------------------------------------------------
IOstream &
CJobGroupExpressionImplementation::OsPrint(IOstream &os) const
{
	return m_jsm.OsHistory(os);
}

#endif	// SPQOS_DEBUG

// EOF
