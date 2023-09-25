//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 Greenplum, Inc.
//
//	@filename:
//		CJobGroupExpressionOptimization.cpp
//
//	@doc:
//		Implementation of group expression optimization job
//---------------------------------------------------------------------------

#include "spqopt/search/CJobGroupExpressionOptimization.h"

#include "spqopt/base/CCostContext.h"
#include "spqopt/base/CDrvdPropCtxtPlan.h"
#include "spqopt/base/CReqdPropPlan.h"
#include "spqopt/engine/CEngine.h"
#include "spqopt/operators/CExpressionHandle.h"
#include "spqopt/operators/CLogical.h"
#include "spqopt/search/CGroup.h"
#include "spqopt/search/CGroupExpression.h"
#include "spqopt/search/CJobFactory.h"
#include "spqopt/search/CJobGroupImplementation.h"
#include "spqopt/search/CJobTransformation.h"
#include "spqopt/search/CScheduler.h"
#include "spqopt/search/CSchedulerContext.h"


using namespace spqopt;

// State transition diagram for group expression optimization job state machine;
//
//                 +------------------------+
//                 |    estInitialized:     |
//  +------------- |    EevtInitialize()    |
//  |              +------------------------+
//  |                |
//  |                | eevOptimizingChildren
//  |                v
//  |              +------------------------+   eevOptimizingChildren
//  |              | estOptimizingChildren: | ------------------------+
//  |              | EevtOptimizeChildren() |                         |
//  +------------- |                        | <-----------------------+
//  |              +------------------------+
//  |                |
//  | eevFinalized   | eevChildrenOptimized
//  |                v
//  |              +------------------------+
//  |              | estChildrenOptimized:  |
//  +------------- |   EevtAddEnforcers()   |
//  |              +------------------------+
//  |                |
//  |                | eevOptimizingSelf
//  |                v
//  |              +------------------------+   eevOptimizingSelf
//  |              |  estEnfdPropsChecked:  | ------------------------+
//  |              |   EevtOptimizeSelf()   |                         |
//  +------------- |                        | <-----------------------+
//  |              +------------------------+
//  |                |
//  |                | eevSelfOptimized
//  |                v
//  |              +------------------------+
//  |              |   estSelfOptimized:    |
//  | eevFinalized |     EevtFinalize()     |
//  |              +------------------------+
//  |                |
//  |                |
//  |                |
//  |                |
//  +----------------+
//                   |
//                   |
//                   | eevFinalized
//                   v
//                 +------------------------+
//                 |      estCompleted      |
//                 +------------------------+
//
const CJobGroupExpressionOptimization::EEvent
	rgeev[CJobGroupExpressionOptimization::estSentinel]
		 [CJobGroupExpressionOptimization::estSentinel] = {
			 {// estInitialized
			  CJobGroupExpressionOptimization::eevSentinel,
			  CJobGroupExpressionOptimization::eevOptimizingChildren,
			  CJobGroupExpressionOptimization::eevSentinel,
			  CJobGroupExpressionOptimization::eevSentinel,
			  CJobGroupExpressionOptimization::eevSentinel,
			  CJobGroupExpressionOptimization::eevFinalized},
			 {// estOptimizingChildren
			  CJobGroupExpressionOptimization::eevSentinel,
			  CJobGroupExpressionOptimization::eevOptimizingChildren,
			  CJobGroupExpressionOptimization::eevChildrenOptimized,
			  CJobGroupExpressionOptimization::eevSentinel,
			  CJobGroupExpressionOptimization::eevSentinel,
			  CJobGroupExpressionOptimization::eevFinalized},
			 {// estChildrenOptimized
			  CJobGroupExpressionOptimization::eevSentinel,
			  CJobGroupExpressionOptimization::eevSentinel,
			  CJobGroupExpressionOptimization::eevSentinel,
			  CJobGroupExpressionOptimization::eevOptimizingSelf,
			  CJobGroupExpressionOptimization::eevSentinel,
			  CJobGroupExpressionOptimization::eevFinalized},
			 {// estEnfdPropsChecked
			  CJobGroupExpressionOptimization::eevSentinel,
			  CJobGroupExpressionOptimization::eevSentinel,
			  CJobGroupExpressionOptimization::eevSentinel,
			  CJobGroupExpressionOptimization::eevOptimizingSelf,
			  CJobGroupExpressionOptimization::eevSelfOptimized,
			  CJobGroupExpressionOptimization::eevFinalized},
			 {// estSelfOptimized
			  CJobGroupExpressionOptimization::eevSentinel,
			  CJobGroupExpressionOptimization::eevSentinel,
			  CJobGroupExpressionOptimization::eevSentinel,
			  CJobGroupExpressionOptimization::eevSentinel,
			  CJobGroupExpressionOptimization::eevSentinel,
			  CJobGroupExpressionOptimization::eevFinalized},
			 {// estCompleted
			  CJobGroupExpressionOptimization::eevSentinel,
			  CJobGroupExpressionOptimization::eevSentinel,
			  CJobGroupExpressionOptimization::eevSentinel,
			  CJobGroupExpressionOptimization::eevSentinel,
			  CJobGroupExpressionOptimization::eevSentinel,
			  CJobGroupExpressionOptimization::eevSentinel},
};

#ifdef SPQOS_DEBUG

// names for states
const WCHAR rgwszStates[CJobGroupExpressionOptimization::estSentinel]
					   [SPQOPT_FSM_NAME_LENGTH] = {
						   SPQOS_WSZ_LIT("initialized"),
						   SPQOS_WSZ_LIT("optimizing children"),
						   SPQOS_WSZ_LIT("children optimized"),
						   SPQOS_WSZ_LIT("enforceable properties checked"),
						   SPQOS_WSZ_LIT("self optimized"),
						   SPQOS_WSZ_LIT("completed")};

// names for events
const WCHAR rgwszEvents[CJobGroupExpressionOptimization::eevSentinel]
					   [SPQOPT_FSM_NAME_LENGTH] = {
						   SPQOS_WSZ_LIT("optimizing child groups"),
						   SPQOS_WSZ_LIT("optimized child groups"),
						   SPQOS_WSZ_LIT("checking enforceable properties"),
						   SPQOS_WSZ_LIT("computing group expression cost"),
						   SPQOS_WSZ_LIT("computed group expression cost"),
						   SPQOS_WSZ_LIT("finalized")};

#endif	// SPQOS_DEBUG


//---------------------------------------------------------------------------
//	@function:
//		CJobGroupExpressionOptimization::CJobGroupExpressionOptimization
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CJobGroupExpressionOptimization::CJobGroupExpressionOptimization()
{
}


//---------------------------------------------------------------------------
//	@function:
//		CJobGroupExpressionOptimization::~CJobGroupExpressionOptimization
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CJobGroupExpressionOptimization::~CJobGroupExpressionOptimization()
{
}


//---------------------------------------------------------------------------
//	@function:
//		CJobGroupExpressionOptimization::Init
//
//	@doc:
//		Initialize job
//
//---------------------------------------------------------------------------
void
CJobGroupExpressionOptimization::Init(CGroupExpression *pgexpr,
									  COptimizationContext *poc, ULONG ulOptReq,
									  CReqdPropPlan *prppCTEProducer)
{
	SPQOS_ASSERT(NULL != poc);

	CJobGroupExpression::Init(pgexpr);
	SPQOS_ASSERT(pgexpr->Pop()->FPhysical());
	SPQOS_ASSERT(pgexpr->Pgroup() == poc->Pgroup());
	SPQOS_ASSERT(ulOptReq <=
				CPhysical::PopConvert(pgexpr->Pop())->UlOptRequests());

	m_jsm.Init(rgeev
#ifdef SPQOS_DEBUG
			   ,
			   rgwszStates, rgwszEvents
#endif	// SPQOS_DEBUG
	);

	// set job actions
	m_jsm.SetAction(estInitialized, EevtInitialize);
	m_jsm.SetAction(estOptimizingChildren, EevtOptimizeChildren);
	m_jsm.SetAction(estChildrenOptimized, EevtAddEnforcers);
	m_jsm.SetAction(estEnfdPropsChecked, EevtOptimizeSelf);
	m_jsm.SetAction(estSelfOptimized, EevtFinalize);

	m_pdrspqoc = NULL;
	m_pdrspqstatCurrentCtxt = NULL;
	m_pdrspqdp = NULL;
	m_pexprhdlPlan = NULL;
	m_pexprhdlRel = NULL;
	m_eceo = CPhysical::PopConvert(pgexpr->Pop())->Eceo();
	m_ulArity = pgexpr->Arity();
	m_ulChildIndex = spqos::ulong_max;

	m_poc = poc;
	m_ulOptReq = ulOptReq;
	m_fChildOptimizationFailed = false;
	m_fOptimizeCTESequence =
		(COperator::EopPhysicalSequence == pgexpr->Pop()->Eopid() &&
		 (*pgexpr)[0]->FHasCTEProducer());
	if (NULL != prppCTEProducer)
	{
		prppCTEProducer->AddRef();
	}
	m_prppCTEProducer = prppCTEProducer;
	m_fScheduledCTEOptimization = false;

	CJob::SetInit();
}

//---------------------------------------------------------------------------
//	@function:
//		CJobGroupExpressionOptimization::Cleanup
//
//	@doc:
//		Cleanup allocated memory
//
//---------------------------------------------------------------------------
void
CJobGroupExpressionOptimization::Cleanup()
{
	CRefCount::SafeRelease(m_pdrspqoc);
	CRefCount::SafeRelease(m_pdrspqstatCurrentCtxt);
	CRefCount::SafeRelease(m_pdrspqdp);
	CRefCount::SafeRelease(m_prppCTEProducer);
	SPQOS_DELETE(m_pexprhdlPlan);
	SPQOS_DELETE(m_pexprhdlRel);
}


//---------------------------------------------------------------------------
//	@function:
//		CJobGroupExpressionOptimization::InitChildGroupsOptimization
//
//	@doc:
//		Initialization routine for child groups optimization
//
//---------------------------------------------------------------------------
void
CJobGroupExpressionOptimization::InitChildGroupsOptimization(
	CSchedulerContext *psc)
{
	SPQOS_ASSERT(NULL == m_pexprhdlPlan);
	SPQOS_ASSERT(NULL == m_pexprhdlRel);

	// initialize required plan properties computation
	m_pexprhdlPlan = SPQOS_NEW(psc->GetGlobalMemoryPool())
		CExpressionHandle(psc->GetGlobalMemoryPool());
	m_pexprhdlPlan->Attach(m_pgexpr);
	if (0 < m_ulArity)
	{
		m_ulChildIndex = m_pexprhdlPlan->UlFirstOptimizedChildIndex();
	}
	m_pexprhdlPlan->DeriveProps(NULL /*pdpctxt*/);
	m_pexprhdlPlan->InitReqdProps(m_poc->Prpp());

	// initialize required relational properties computation
	m_pexprhdlRel = SPQOS_NEW(psc->GetGlobalMemoryPool())
		CExpressionHandle(psc->GetGlobalMemoryPool());
	CGroupExpression *pgexprForStats = m_pgexpr->Pgroup()->PgexprBestPromise(
		psc->GetGlobalMemoryPool(), m_pgexpr);
	if (NULL != pgexprForStats)
	{
		m_pexprhdlRel->Attach(pgexprForStats);
		m_pexprhdlRel->DeriveProps(NULL /*pdpctxt*/);
		m_pexprhdlRel->ComputeReqdProps(m_poc->GetReqdRelationalProps(),
										0 /*ulOptReq*/);
	}

	// create child groups derived properties
	m_pdrspqdp = SPQOS_NEW(psc->GetGlobalMemoryPool())
		CDrvdPropArray(psc->GetGlobalMemoryPool());

	// initialize stats context with input stats context
	m_pdrspqstatCurrentCtxt = SPQOS_NEW(psc->GetGlobalMemoryPool())
		IStatisticsArray(psc->GetGlobalMemoryPool());
	CUtils::AddRefAppend<IStatistics, CleanupStats>(m_pdrspqstatCurrentCtxt,
													m_poc->Pdrspqstat());
}


//---------------------------------------------------------------------------
//	@function:
//		CJobGroupExpressionOptimization::EevtInitialize
//
//	@doc:
//		Initialize internal data structures;
//
//---------------------------------------------------------------------------
CJobGroupExpressionOptimization::EEvent
CJobGroupExpressionOptimization::EevtInitialize(CSchedulerContext *psc,
												CJob *pjOwner)
{
	// get a job pointer
	CJobGroupExpressionOptimization *pjgeo = PjConvert(pjOwner);

	CExpressionHandle exprhdl(psc->GetGlobalMemoryPool());
	exprhdl.Attach(pjgeo->m_pgexpr);
	exprhdl.DeriveProps(NULL /*pdpctxt*/);
	if (!psc->Peng()->FCheckReqdProps(exprhdl, pjgeo->m_poc->Prpp(),
									  pjgeo->m_ulOptReq))
	{
		return eevFinalized;
	}

	// check if job can be early terminated without optimizing any child
	CCost costLowerBound(SPQOPT_INVALID_COST);
	if (psc->Peng()->FSafeToPrune(
			pjgeo->m_pgexpr, pjgeo->m_poc->Prpp(), NULL /*pccChild*/,
			spqos::ulong_max /*child_index*/, &costLowerBound))
	{
		(void) pjgeo->m_pgexpr->PccComputeCost(
			psc->GetGlobalMemoryPool(), pjgeo->m_poc, pjgeo->m_ulOptReq,
			NULL /*pdrspqoc*/, true /*fPruned*/, costLowerBound);
		return eevFinalized;
	}

	pjgeo->InitChildGroupsOptimization(psc);

	return eevOptimizingChildren;
}


//---------------------------------------------------------------------------
//	@function:
//		CJobGroupExpressionOptimization::DerivePrevChildProps
//
//	@doc:
//		Derive plan properties and stats of the child previous to
//		the one being optimized
//
//---------------------------------------------------------------------------
void
CJobGroupExpressionOptimization::DerivePrevChildProps(CSchedulerContext *psc)
{
	ULONG ulPrevChildIndex =
		m_pexprhdlPlan->UlPreviousOptimizedChildIndex(m_ulChildIndex);

	// retrieve plan properties of the optimal implementation of previous child group
	CGroup *pgroupChild = (*m_pgexpr)[ulPrevChildIndex];
	if (pgroupChild->FScalar())
	{
		// exit if previous child is a scalar group
		return;
	}

	COptimizationContext *pocChild = pgroupChild->PocLookupBest(
		psc->GetGlobalMemoryPool(), psc->Peng()->UlSearchStages(),
		m_pexprhdlPlan->Prpp(ulPrevChildIndex));
	SPQOS_ASSERT(NULL != pocChild);

	CCostContext *pccChildBest = pocChild->PccBest();
	if (NULL == pccChildBest)
	{
		// failed to optimize child
		m_fChildOptimizationFailed = true;
		return;
	}

	// check if job can be early terminated after previous children have been optimized
	CCost costLowerBound(SPQOPT_INVALID_COST);
	if (psc->Peng()->FSafeToPrune(m_pgexpr, m_poc->Prpp(), pccChildBest,
								  ulPrevChildIndex, &costLowerBound))
	{
		// failed to optimize child due to cost bounding
		(void) m_pgexpr->PccComputeCost(psc->GetGlobalMemoryPool(), m_poc,
										m_ulOptReq, NULL /*pdrspqoc*/,
										true /*fPruned*/, costLowerBound);
		m_fChildOptimizationFailed = true;
		return;
	}

	CExpressionHandle exprhdl(psc->GetGlobalMemoryPool());
	exprhdl.Attach(pccChildBest);
	exprhdl.DerivePlanPropsForCostContext();
	exprhdl.Pdp()->AddRef();
	m_pdrspqdp->Append(exprhdl.Pdp());

	// copy stats of child's best cost context to current stats context
	IStatistics *pstat = pccChildBest->Pstats();
	pstat->AddRef();
	m_pdrspqstatCurrentCtxt->Append(pstat);
}


//---------------------------------------------------------------------------
//	@function:
//		CJobGroupExpressionOptimization::ComputeCurrentChildRequirements
//
//	@doc:
//		Compute required plan properties for current child
//
//---------------------------------------------------------------------------
void
CJobGroupExpressionOptimization::ComputeCurrentChildRequirements(
	CSchedulerContext *psc)
{
	// derive plan properties of previous child group
	if (m_ulChildIndex != m_pexprhdlPlan->UlFirstOptimizedChildIndex())
	{
		DerivePrevChildProps(psc);
		if (m_fChildOptimizationFailed)
		{
			return;
		}
	}

	// compute required plan properties of current child group
	if (0 == m_ulChildIndex && NULL != m_prppCTEProducer)
	{
		SPQOS_ASSERT(m_fOptimizeCTESequence);
		SPQOS_ASSERT((*m_pgexpr)[m_ulChildIndex]->FHasCTEProducer());

		m_prppCTEProducer->AddRef();
		m_pexprhdlPlan->CopyChildReqdProps(m_ulChildIndex, m_prppCTEProducer);
	}
	else
	{
		m_pexprhdlPlan->ComputeChildReqdProps(m_ulChildIndex, m_pdrspqdp,
											  m_ulOptReq);
	}
}


//---------------------------------------------------------------------------
//	@function:
//		CJobGroupExpressionOptimization::ScheduleChildGroupsJobs
//
//	@doc:
//		Schedule optimization job for the next child group; skip child groups
//		as they do not require optimization
//
//---------------------------------------------------------------------------
void
CJobGroupExpressionOptimization::ScheduleChildGroupsJobs(CSchedulerContext *psc)
{
	SPQOS_ASSERT(!FChildrenScheduled());

	CGroup *pgroupChild = (*m_pgexpr)[m_ulChildIndex];
	if (pgroupChild->FScalar())
	{
		if (!m_pexprhdlPlan->FNextChildIndex(&m_ulChildIndex))
		{
			// child group optimization is complete
			SetChildrenScheduled();
		}

		return;
	}

	ComputeCurrentChildRequirements(psc);
	if (m_fChildOptimizationFailed)
	{
		return;
	}
	m_pexprhdlPlan->Prpp(m_ulChildIndex)->AddRef();

	// use current stats for optimizing current child
	IStatisticsArray *stats_ctxt = SPQOS_NEW(psc->GetGlobalMemoryPool())
		IStatisticsArray(psc->GetGlobalMemoryPool());
	CUtils::AddRefAppend<IStatistics, CleanupStats>(stats_ctxt,
													m_pdrspqstatCurrentCtxt);

	// compute required relational properties
	CReqdPropRelational *prprel = NULL;
	if (CPhysical::PopConvert(m_pgexpr->Pop())->FPassThruStats())
	{
		// copy requirements from origin context
		prprel = m_poc->GetReqdRelationalProps();
	}
	else
	{
		// retrieve requirements from handle
		prprel = m_pexprhdlRel->GetReqdRelationalProps(m_ulChildIndex);
	}
	SPQOS_ASSERT(NULL != prprel);
	prprel->AddRef();

	// schedule optimization job for current child group
	COptimizationContext *pocChild = SPQOS_NEW(psc->GetGlobalMemoryPool())
		COptimizationContext(psc->GetGlobalMemoryPool(), pgroupChild,
							 m_pexprhdlPlan->Prpp(m_ulChildIndex), prprel,
							 stats_ctxt, psc->Peng()->UlCurrSearchStage());
    /* SPQ begin: print cost*/
    pocChild->pgexprOrigin_ID = m_pgexpr->Id();

	if (pgroupChild == m_pgexpr->Pgroup() && pocChild->Matches(m_poc))
	{
		// this is to prevent deadlocks, child context cannot be the same as parent context
		m_fChildOptimizationFailed = true;
		pocChild->Release();

		return;
	}

	CJobGroupOptimization::ScheduleJob(psc, pgroupChild, m_pgexpr, pocChild,
									   this);
	pocChild->Release();

	// advance to next child
	if (!m_pexprhdlPlan->FNextChildIndex(&m_ulChildIndex))
	{
		// child group optimization is complete
		SetChildrenScheduled();
	}
}


//---------------------------------------------------------------------------
//	@function:
//		CJobGroupExpressionOptimization::EevtOptimizeChildren
//
//	@doc:
//		Optimize child groups
//
//---------------------------------------------------------------------------
CJobGroupExpressionOptimization::EEvent
CJobGroupExpressionOptimization::EevtOptimizeChildren(CSchedulerContext *psc,
													  CJob *pjOwner)
{
	// get a job pointer
	CJobGroupExpressionOptimization *pjgeo = PjConvert(pjOwner);
	if (0 < pjgeo->m_ulArity && !pjgeo->FChildrenScheduled())
	{
		pjgeo->ScheduleChildGroupsJobs(psc);
		if (pjgeo->m_fChildOptimizationFailed)
		{
			// failed to optimize child, terminate job
			pjgeo->Cleanup();
			return eevFinalized;
		}

		return eevOptimizingChildren;
	}

	return eevChildrenOptimized;
}


//---------------------------------------------------------------------------
//	@function:
//		CJobGroupExpressionOptimization::EevtAddEnforcers
//
//	@doc:
//		Add required enforcers to owning group
//
//---------------------------------------------------------------------------
CJobGroupExpressionOptimization::EEvent
CJobGroupExpressionOptimization::EevtAddEnforcers(CSchedulerContext *psc,
												  CJob *pjOwner)
{
	// get a job pointer
	CJobGroupExpressionOptimization *pjgeo = PjConvert(pjOwner);

	// build child contexts array
	SPQOS_ASSERT(NULL == pjgeo->m_pdrspqoc);
	pjgeo->m_pdrspqoc = psc->Peng()->PdrspqocChildren(psc->GetGlobalMemoryPool(),
													*pjgeo->m_pexprhdlPlan);

	// enforce physical properties
	BOOL fCheckEnfdProps = psc->Peng()->FCheckEnfdProps(
		psc->GetGlobalMemoryPool(), pjgeo->m_pgexpr, pjgeo->m_poc,
		pjgeo->m_ulOptReq, pjgeo->m_pdrspqoc);
	if (fCheckEnfdProps)
	{
		// No new enforcers group expressions were added because they were either
		// optional or unnecessary. So, move on to optimize the current group
		// expression.
		return eevOptimizingSelf;
	}

	// Either adding enforcers was prohibited or at least one enforcer was added
	// because it was required. In any case, this job can be finalized, since
	// optimizing the current group expression is not needed (because of the
	// prohibition) or the newly created enforcer group expression job will get
	// to it later on.
	pjgeo->Cleanup();
	return eevFinalized;
}


//---------------------------------------------------------------------------
//	@function:
//		CJobGroupExpressionOptimization::EevtOptimizeSelf
//
//	@doc:
//		Optimize group expression
//
//---------------------------------------------------------------------------
CJobGroupExpressionOptimization::EEvent
CJobGroupExpressionOptimization::EevtOptimizeSelf(CSchedulerContext *psc,
												  CJob *pjOwner)
{
	// get a job pointer
	CJobGroupExpressionOptimization *pjgeo = PjConvert(pjOwner);

	if (pjgeo->m_fScheduledCTEOptimization)
	{
		// current job has triggered a CTE optimization child job,
		// we can only come here if the child job is complete,
		// we can now safely terminate current job
		return eevSelfOptimized;
	}

	// compute group expression cost under current context
	COptimizationContext *poc = pjgeo->m_poc;
	CGroupExpression *pgexpr = pjgeo->m_pgexpr;
	COptimizationContextArray *pdrspqoc = pjgeo->m_pdrspqoc;
	ULONG ulOptReq = pjgeo->m_ulOptReq;

	CCostContext *pcc =
		pgexpr->PccComputeCost(psc->GetGlobalMemoryPool(), poc, ulOptReq,
							   pdrspqoc, false /*fPruned*/, CCost(0.0));

	if (NULL == pcc)
	{
		pjgeo->Cleanup();

		// failed to create cost context, terminate optimization job
		return eevFinalized;
	}

	pgexpr->Pgroup()->UpdateBestCost(poc, pcc);

	if (FScheduleCTEOptimization(psc, pgexpr, poc, ulOptReq, pjgeo))
	{
		// a child job for optimizing CTE has been scheduled
		pjgeo->m_fScheduledCTEOptimization = true;

		// suspend current job until CTE optimization child job is complete
		return eevOptimizingSelf;
	}

	return eevSelfOptimized;
}


//---------------------------------------------------------------------------
//	@function:
//		CJobGroupExpressionOptimization::EevtFinalize
//
//	@doc:
//		Finalize optimization
//
//---------------------------------------------------------------------------
CJobGroupExpressionOptimization::EEvent
CJobGroupExpressionOptimization::EevtFinalize(CSchedulerContext *,	// psc
											  CJob *pjOwner)
{
	// get a job pointer
	CJobGroupExpressionOptimization *pjgeo = PjConvert(pjOwner);
	SPQOS_ASSERT(!pjgeo->m_fChildOptimizationFailed);

#ifdef SPQOS_DEBUG
	CCostContext *pcc = pjgeo->m_poc->PccBest();
#endif	// SPQOS_DEBUG

	SPQOS_ASSERT(NULL != pcc);
	SPQOS_ASSERT(CCostContext::estCosted == pcc->Est());

	pjgeo->Cleanup();

	return eevFinalized;
}


//---------------------------------------------------------------------------
//	@function:
//		CJobGroupExpressionOptimization::FExecute
//
//	@doc:
//		Main job function
//
//---------------------------------------------------------------------------
BOOL
CJobGroupExpressionOptimization::FExecute(CSchedulerContext *psc)
{
	SPQOS_ASSERT(FInit());

	return m_jsm.FRun(psc, this);
}


//---------------------------------------------------------------------------
//	@function:
//		CJobGroupExpressionOptimization::ScheduleJob
//
//	@doc:
//		Schedule a new group expression optimization job
//
//---------------------------------------------------------------------------
void
CJobGroupExpressionOptimization::ScheduleJob(CSchedulerContext *psc,
											 CGroupExpression *pgexpr,
											 COptimizationContext *poc,
											 ULONG ulOptReq, CJob *pjParent)
{
	CJob *pj = psc->Pjf()->PjCreate(CJob::EjtGroupExpressionOptimization);

	// initialize job
	CJobGroupExpressionOptimization *pjgeo = PjConvert(pj);
	pjgeo->Init(pgexpr, poc, ulOptReq);
	psc->Psched()->Add(pjgeo, pjParent);
}


//---------------------------------------------------------------------------
//	@function:
//		CJobGroupExpressionOptimization::FScheduleCTEOptimization
//
//	@doc:
//		Schedule a new job for CTE optimization
//
//---------------------------------------------------------------------------
BOOL
CJobGroupExpressionOptimization::FScheduleCTEOptimization(
	CSchedulerContext *psc, CGroupExpression *pgexpr, COptimizationContext *poc,
	ULONG ulOptReq, CJob *pjParent)
{
	SPQOS_ASSERT(NULL != psc);

	if (SPQOS_FTRACE(EopttraceDisablePushingCTEConsumerReqsToCTEProducer))
	{
		// pushing CTE consumer requirements to producer is disabled
		return false;
	}

	CJobGroupExpressionOptimization *pjgeoParent = PjConvert(pjParent);
	SPQOS_ASSERT(!pjgeoParent->m_fScheduledCTEOptimization);

	if (!pjgeoParent->m_fOptimizeCTESequence)
	{
		// root operator is not a Sequence
		return false;
	}

	if (NULL != pjgeoParent->m_prppCTEProducer)
	{
		// parent job is already a CTE optimization job
		return false;
	}

	// compute new requirements for CTE producer based on delivered properties of consumers plan
	CReqdPropPlan *prppCTEProducer = COptimizationContext::PrppCTEProducer(
		psc->GetGlobalMemoryPool(), poc, psc->Peng()->UlSearchStages());
	if (NULL == prppCTEProducer)
	{
		// failed to create CTE producer requirements
		return false;
	}

	// schedule CTE optimization job
	CJob *pj = psc->Pjf()->PjCreate(CJob::EjtGroupExpressionOptimization);
	CJobGroupExpressionOptimization *pjgeo = PjConvert(pj);

	// initialize job
	pjgeo->Init(pgexpr, poc, ulOptReq, prppCTEProducer);
	psc->Psched()->Add(pjgeo, pjParent);
	prppCTEProducer->Release();

	return true;
}


#ifdef SPQOS_DEBUG

//---------------------------------------------------------------------------
//	@function:
//		CJobGroupExpressionOptimization::OsPrint
//
//	@doc:
//		Print function
//
//---------------------------------------------------------------------------
IOstream &
CJobGroupExpressionOptimization::OsPrint(IOstream &os) const
{
	os << "Group expr: ";
	m_pgexpr->OsPrint(os);
	os << std::endl;

	return m_jsm.OsHistory(os);
}

#endif	// SPQOS_DEBUG

// EOF
