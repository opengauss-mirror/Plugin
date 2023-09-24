//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 - 2011 EMC Corp.
//
//	@filename:
//		CEngine.cpp
//
//	@doc:
//		Implementation of optimization engine
//---------------------------------------------------------------------------
#include "spqopt/engine/CEngine.h"

#include "spqos/base.h"
#include "spqos/common/CAutoTimer.h"
#include "spqos/common/syslibwrapper.h"
#include "spqos/error/CAutoTrace.h"
#include "spqos/io/COstreamString.h"
#include "spqos/memory/CAutoMemoryPool.h"
#include "spqos/string/CWStringDynamic.h"
#include "spqos/task/CAutoTaskProxy.h"
#include "spqos/task/CAutoTraceFlag.h"

#include "spqopt/base/CCostContext.h"
#include "spqopt/base/CDrvdPropCtxtPlan.h"
#include "spqopt/base/COptCtxt.h"
#include "spqopt/base/COptimizationContext.h"
#include "spqopt/base/CQueryContext.h"
#include "spqopt/base/CReqdPropPlan.h"
#include "spqopt/base/CReqdPropRelational.h"
#include "spqopt/engine/CEnumeratorConfig.h"
#include "spqopt/engine/CStatisticsConfig.h"
#include "spqopt/exception.h"
#include "spqopt/minidump/CSerializableStackTrace.h"
#include "spqopt/operators/CExpression.h"
#include "spqopt/operators/CExpressionHandle.h"
#include "spqopt/operators/CLogical.h"
#include "spqopt/operators/CPattern.h"
#include "spqopt/operators/CPatternLeaf.h"
#include "spqopt/operators/CPhysicalAgg.h"
#include "spqopt/operators/CPhysicalMotionGather.h"
#include "spqopt/operators/CPhysicalSort.h"
#include "spqopt/optimizer/COptimizerConfig.h"
#include "spqopt/search/CBinding.h"
#include "spqopt/search/CGroup.h"
#include "spqopt/search/CGroupExpression.h"
#include "spqopt/search/CGroupProxy.h"
#include "spqopt/search/CJob.h"
#include "spqopt/search/CJobFactory.h"
#include "spqopt/search/CMemo.h"
#include "spqopt/search/CScheduler.h"
#include "spqopt/search/CSchedulerContext.h"
#include "spqopt/xforms/CXformFactory.h"
#include "naucrates/traceflags/traceflags.h"


#define SPQOPT_SAMPLING_MAX_ITERS 30
#define SPQOPT_JOBS_CAP 5000	 // maximum number of initial optimization jobs
#define SPQOPT_JOBS_PER_GROUP \
	20	// estimated number of needed optimization jobs per memo group

// memory consumption unit in bytes -- currently MB
#define SPQOPT_MEM_UNIT (1024 * 1024)
#define SPQOPT_MEM_UNIT_NAME "MB"

using namespace spqopt;

FORCE_GENERATE_DBGSTR(CEngine);

//---------------------------------------------------------------------------
//	@function:
//		CEngine::CEngine
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CEngine::CEngine(CMemoryPool *mp)
	: m_mp(mp),
	  m_pqc(NULL),
	  m_search_stage_array(NULL),
	  m_ulCurrSearchStage(0),
	  m_pmemo(NULL),
	  m_pexprEnforcerPattern(NULL),
	  m_xforms(NULL),
	  m_pdrspqulpXformCalls(NULL),
	  m_pdrspqulpXformTimes(NULL),
	  m_pdrspqulpXformBindings(NULL),
	  m_pdrspqulpXformResults(NULL)
{
	m_pmemo = SPQOS_NEW(mp) CMemo(mp);
	m_pexprEnforcerPattern =
		SPQOS_NEW(mp) CExpression(mp, SPQOS_NEW(mp) CPatternLeaf(mp));
	m_xforms = SPQOS_NEW(mp) CXformSet(mp);
	m_pdrspqulpXformCalls = SPQOS_NEW(mp) UlongPtrArray(mp);
	m_pdrspqulpXformTimes = SPQOS_NEW(mp) UlongPtrArray(mp);
	m_pdrspqulpXformBindings = SPQOS_NEW(mp) UlongPtrArray(mp);
	m_pdrspqulpXformResults = SPQOS_NEW(mp) UlongPtrArray(mp);
}


//---------------------------------------------------------------------------
//	@function:
//		CEngine::~CEngine
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CEngine::~CEngine()
{
#ifdef SPQOS_DEBUG
	// in optimized build, we flush-down memory pools without leak checking,
	// we can save time in optimized build by skipping all de-allocations here,
	// we still have all de-llocations enabled in debug-build to detect any possible leaks
	SPQOS_DELETE(m_pmemo);
	CRefCount::SafeRelease(m_xforms);
	m_pdrspqulpXformCalls->Release();
	m_pdrspqulpXformTimes->Release();
	m_pdrspqulpXformBindings->Release();
	m_pdrspqulpXformResults->Release();
	m_pexprEnforcerPattern->Release();
	CRefCount::SafeRelease(m_search_stage_array);
#endif	// SPQOS_DEBUG
}


//---------------------------------------------------------------------------
//	@function:
//		CEngine::InitLogicalExpression
//
//	@doc:
//		Initialize engine with a given expression
//
//---------------------------------------------------------------------------
void
CEngine::InitLogicalExpression(CExpression *pexpr)
{
	SPQOS_ASSERT(NULL == m_pmemo->PgroupRoot() && "Root is already set");
	SPQOS_ASSERT(pexpr->Pop()->FLogical());

	CGroup *pgroupRoot =
		PgroupInsert(NULL /*pgroupTarget*/, pexpr, CXform::ExfInvalid,
					 NULL /*pgexprOrigin*/, false /*fIntermediate*/);
	m_pmemo->SetRoot(pgroupRoot);
}


//---------------------------------------------------------------------------
//	@function:
//		CEngine::Init
//
//	@doc:
//		Initialize engine using a given query context
//
//---------------------------------------------------------------------------
void
CEngine::Init(CQueryContext *pqc, CSearchStageArray *search_stage_array)
{
	SPQOS_ASSERT(NULL == m_pqc);
	SPQOS_ASSERT(NULL != pqc);
	SPQOS_ASSERT_IMP(0 == pqc->Pexpr()->DeriveOutputColumns()->Size(),
					0 == pqc->Prpp()->PcrsRequired()->Size() &&
						"requiring columns from a zero column expression");

	m_search_stage_array = search_stage_array;
	if (NULL == search_stage_array)
	{
		m_search_stage_array = CSearchStage::PdrspqssDefault(m_mp);
	}
	SPQOS_ASSERT(0 < m_search_stage_array->Size());

	if (SPQOS_FTRACE(EopttracePrintOptimizationStatistics))
	{
		// initialize per-stage xform calls array
		const ULONG ulStages = m_search_stage_array->Size();
		for (ULONG ul = 0; ul < ulStages; ul++)
		{
			ULONG_PTR *pulpXformCalls =
				SPQOS_NEW_ARRAY(m_mp, ULONG_PTR, CXform::ExfSentinel);
			ULONG_PTR *pulpXformTimes =
				SPQOS_NEW_ARRAY(m_mp, ULONG_PTR, CXform::ExfSentinel);
			ULONG_PTR *pulpXformBindings =
				SPQOS_NEW_ARRAY(m_mp, ULONG_PTR, CXform::ExfSentinel);
			ULONG_PTR *pulpXformResults =
				SPQOS_NEW_ARRAY(m_mp, ULONG_PTR, CXform::ExfSentinel);
			for (ULONG ulXform = 0; ulXform < CXform::ExfSentinel; ulXform++)
			{
				pulpXformCalls[ulXform] = 0;
				pulpXformTimes[ulXform] = 0;
				pulpXformBindings[ulXform] = 0;
				pulpXformResults[ulXform] = 0;
			}
			m_pdrspqulpXformCalls->Append(pulpXformCalls);
			m_pdrspqulpXformTimes->Append(pulpXformTimes);
			m_pdrspqulpXformBindings->Append(pulpXformBindings);
			m_pdrspqulpXformResults->Append(pulpXformResults);
		}
	}

	m_pqc = pqc;
	InitLogicalExpression(m_pqc->Pexpr());

	m_pqc->PdrspqcrSystemCols()->AddRef();
	COptCtxt::PoctxtFromTLS()->SetReqdSystemCols(m_pqc->PdrspqcrSystemCols());
}


//---------------------------------------------------------------------------
//	@function:
//		CEngine::AddEnforcers
//
//	@doc:
//		Add enforcers to a memo group
//
//---------------------------------------------------------------------------
void
CEngine::AddEnforcers(
	CGroupExpression
		*pgexpr,  // belongs to group where we need to add enforcers
	CExpressionArray *pdrspqexprEnforcers)
{
	SPQOS_ASSERT(NULL != pdrspqexprEnforcers);
	SPQOS_ASSERT(NULL != pgexpr);

	for (ULONG ul = 0; ul < pdrspqexprEnforcers->Size(); ul++)
	{
		// assemble an expression rooted by the enforcer operator
		CExpression *pexprEnforcer = (*pdrspqexprEnforcers)[ul];
#ifdef SPQOS_DEBUG
		CGroup *pgroup =
#endif	// SPQOS_DEBUG
			PgroupInsert(pgexpr->Pgroup(), pexprEnforcer, CXform::ExfInvalid,
						 NULL /*pgexprOrigin*/, false /*fIntermediate*/);
		SPQOS_ASSERT(pgroup == pgexpr->Pgroup());
	}
}


//---------------------------------------------------------------------------
//	@function:
//		CEngine::InsertExpressionChildren
//
//	@doc:
//		Insert children of the given expression to memo, and copy the groups
//		they end up at to the given group array
//
//---------------------------------------------------------------------------
void
CEngine::InsertExpressionChildren(CExpression *pexpr,
								  CGroupArray *pdrspqgroupChildren,
								  CXform::EXformId exfidOrigin,
								  CGroupExpression *pgexprOrigin)
{
	SPQOS_ASSERT(NULL != pexpr);
	SPQOS_ASSERT(NULL != pdrspqgroupChildren);

	ULONG arity = pexpr->Arity();

	for (ULONG i = 0; i < arity; i++)
	{
		CGroup *pgroupChild = NULL;
		COperator *popChild = (*pexpr)[i]->Pop();
		if (popChild->FPattern() && CPattern::PopConvert(popChild)->FLeaf())
		{
			SPQOS_ASSERT(NULL != (*pexpr)[i]->Pgexpr()->Pgroup());

			// group is already assigned during binding extraction;
			pgroupChild = (*pexpr)[i]->Pgexpr()->Pgroup();
		}
		else
		{
			// insert child expression recursively
			pgroupChild =
				PgroupInsert(NULL /*pgroupTarget*/, (*pexpr)[i], exfidOrigin,
							 pgexprOrigin, true /*fIntermediate*/);
		}
		pdrspqgroupChildren->Append(pgroupChild);
	}
}


//---------------------------------------------------------------------------
//	@function:
//		CEngine::PgroupInsert
//
//	@doc:
//		Insert an expression tree into the memo, with explicit target group;
//		the function returns a pointer to the group that contains the given
//		group expression
//
//---------------------------------------------------------------------------
CGroup *
CEngine::PgroupInsert(CGroup *pgroupTarget, CExpression *pexpr,
					  CXform::EXformId exfidOrigin,
					  CGroupExpression *pgexprOrigin, BOOL fIntermediate)
{
	// recursive function - check stack
	SPQOS_CHECK_STACK_SIZE;
	SPQOS_CHECK_ABORT;
	SPQOS_ASSERT_IMP(CXform::ExfInvalid != exfidOrigin, NULL != pgexprOrigin);

	CGroup *pgroupOrigin = NULL;

	// check if expression was produced by extracting
	// a binding from the memo
	if (NULL != pexpr->Pgexpr())
	{
		pgroupOrigin = pexpr->Pgexpr()->Pgroup();
		SPQOS_ASSERT(NULL != pgroupOrigin && NULL == pgroupTarget &&
					"A valid group is expected");

		// if parent has group pointer, all children must have group pointers;
		// terminate recursive insertion here
		return pgroupOrigin;
	}

	// if we have a valid origin group, target group must be NULL
	SPQOS_ASSERT_IMP(NULL != pgroupOrigin, NULL == pgroupTarget);

	// insert expression's children to memo by recursive call
	CGroupArray *pdrspqgroupChildren =
		SPQOS_NEW(m_mp) CGroupArray(m_mp, pexpr->Arity());
	InsertExpressionChildren(pexpr, pdrspqgroupChildren, exfidOrigin,
							 pgexprOrigin);

	COperator *pop = pexpr->Pop();
	pop->AddRef();
	CGroupExpression *pgexpr = SPQOS_NEW(m_mp)
		CGroupExpression(m_mp, pop, pdrspqgroupChildren, exfidOrigin,
						 pgexprOrigin, fIntermediate);

	// find the group that contains created group expression
	CGroup *pgroupContainer =
		m_pmemo->PgroupInsert(pgroupTarget, pexpr, pgexpr);

	if (NULL == pgexpr->Pgroup())
	{
		// insertion failed, release created group expression
		pgexpr->Release();
	}

	return pgroupContainer;
}


//---------------------------------------------------------------------------
//	@function:
//		CEngine::InsertXformResult
//
//	@doc:
//		Insert a set of transformation results to memo
//
//---------------------------------------------------------------------------
void
CEngine::InsertXformResult(
	CGroup *pgroupOrigin, CXformResult *pxfres, CXform::EXformId exfidOrigin,
	CGroupExpression *pgexprOrigin,
	ULONG ulXformTime,	// time consumed by transformation in msec
	ULONG ulNumberOfBindings)
{
	SPQOS_ASSERT(NULL != pxfres);
	SPQOS_ASSERT(NULL != pgroupOrigin);
	SPQOS_ASSERT(CXform::ExfInvalid != exfidOrigin);
	SPQOS_ASSERT(NULL != pgexprOrigin);

	if (SPQOS_FTRACE(EopttracePrintOptimizationStatistics) &&
		0 < pxfres->Pdrspqexpr()->Size())
	{
		(void) m_xforms->ExchangeSet(exfidOrigin);
		(*m_pdrspqulpXformCalls)[m_ulCurrSearchStage][exfidOrigin] += 1;
		(*m_pdrspqulpXformTimes)[m_ulCurrSearchStage][exfidOrigin] +=
			ulXformTime;
		(*m_pdrspqulpXformBindings)[m_ulCurrSearchStage][exfidOrigin] +=
			ulNumberOfBindings;
		(*m_pdrspqulpXformResults)[m_ulCurrSearchStage][exfidOrigin] +=
			pxfres->Pdrspqexpr()->Size();
	}

	CExpression *pexpr = pxfres->PexprNext();
	while (NULL != pexpr)
	{
		CGroup *pgroupContainer =
			PgroupInsert(pgroupOrigin, pexpr, exfidOrigin, pgexprOrigin,
						 false /*fIntermediate*/);
		if (pgroupContainer != pgroupOrigin &&
			FPossibleDuplicateGroups(pgroupContainer, pgroupOrigin))
		{
			m_pmemo->MarkDuplicates(pgroupOrigin, pgroupContainer);
		}

		pexpr = pxfres->PexprNext();
	}
}

//---------------------------------------------------------------------------
//	@function:
//		CEngine::FPossibleDuplicateGroups
//
//	@doc:
//		Check whether the given memo groups can be marked as duplicates. This is
//		true only if they have the same logical properties
//
//---------------------------------------------------------------------------
BOOL
CEngine::FPossibleDuplicateGroups(CGroup *pgroupFst, CGroup *pgroupSnd)
{
	SPQOS_ASSERT(NULL != pgroupFst);
	SPQOS_ASSERT(NULL != pgroupSnd);

	CDrvdPropRelational *pdprelFst =
		CDrvdPropRelational::GetRelationalProperties(pgroupFst->Pdp());
	CDrvdPropRelational *pdprelSnd =
		CDrvdPropRelational::GetRelationalProperties(pgroupSnd->Pdp());

	// right now we only check the output columns, but we may possibly need to
	// check other properties as well
	return pdprelFst->GetOutputColumns()->Equals(pdprelSnd->GetOutputColumns());
}

//---------------------------------------------------------------------------
//	@function:
//		CEngine::DeriveStats
//
//	@doc:
//		Derive statistics on the root group
//
//---------------------------------------------------------------------------
void
CEngine::DeriveStats(CMemoryPool *pmpLocal)
{
	CWStringDynamic str(m_mp);
	COstreamString oss(&str);
	oss << "\n[OPT]: Statistics Derivation Time (stage " << m_ulCurrSearchStage
		<< ") ";
	CHAR *sz = CUtils::CreateMultiByteCharStringFromWCString(
		m_mp, const_cast<WCHAR *>(str.GetBuffer()));

	{
		CAutoTimer at(sz, SPQOS_FTRACE(EopttracePrintOptimizationStatistics));
		// derive stats on root group
		CEngine::DeriveStats(pmpLocal, m_mp, PgroupRoot(), NULL /*prprel*/);
	}

	SPQOS_DELETE_ARRAY(sz);
}

//---------------------------------------------------------------------------
//	@function:
//		CEngine::DeriveStats
//
//	@doc:
//		Derive statistics on the group
//
//---------------------------------------------------------------------------
void
CEngine::DeriveStats(CMemoryPool *pmpLocal, CMemoryPool *pmpGlobal,
					 CGroup *pgroup, CReqdPropRelational *prprel)
{
	CGroupExpression *pgexprFirst = CEngine::PgexprFirst(pgroup);
	CExpressionHandle exprhdl(pmpGlobal);
	exprhdl.Attach(pgexprFirst);
	exprhdl.DeriveStats(pmpLocal, pmpGlobal, prprel, NULL /*stats_ctxt*/);
}

//---------------------------------------------------------------------------
//	@function:
//		CEngine::PgexprFirst
//
//	@doc:
//		Return the first group expression in a given group
//
//---------------------------------------------------------------------------
CGroupExpression *
CEngine::PgexprFirst(CGroup *pgroup)
{
	CGroupExpression *pgexprFirst = NULL;
	{
		// group proxy scope
		CGroupProxy spq(pgroup);
		pgexprFirst = spq.PgexprFirst();
	}
	SPQOS_ASSERT(NULL != pgexprFirst);

	return pgexprFirst;
}


//---------------------------------------------------------------------------
//	@function:
//		CEngine::EolDamp
//
//	@doc:
//		Damp optimization level
//
//---------------------------------------------------------------------------
EOptimizationLevel
CEngine::EolDamp(EOptimizationLevel eol)
{
	if (EolHigh == eol)
	{
		return EolLow;
	}

	return EolSentinel;
}


//---------------------------------------------------------------------------
//	@function:
//		CEngine::FOptimizeChild
//
//	@doc:
//		Check if parent group expression needs to optimize child group expression.
//		This method is called right before a group optimization job is about to
//		schedule a group expression optimization job.
//
//		Relation properties as well the optimizing parent group expression is
//		available to make the decision. So, operators can reject being optimized
//		under specific parent operators. For example, a GatherMerge under a Sort
//		can be prevented here since it destroys the order from a GatherMerge.
//---------------------------------------------------------------------------
BOOL
CEngine::FOptimizeChild(
	CGroupExpression *pgexprParent, CGroupExpression *pgexprChild,
	COptimizationContext *pocChild,
	EOptimizationLevel eolCurrent  // current optimization level in child group
)
{
	SPQOS_ASSERT(NULL != PgroupRoot());
	SPQOS_ASSERT(PgroupRoot()->FImplemented());
	SPQOS_ASSERT(NULL != pgexprChild);
	SPQOS_ASSERT_IMP(NULL == pgexprParent,
					pgexprChild->Pgroup() == PgroupRoot());

	if (pgexprParent == pgexprChild)
	{
		// a group expression cannot optimize itself
		return false;
	}

	if (pgexprChild->Eol() != eolCurrent)
	{
		// child group expression does not match current optimization level
		return false;
	}

	COperator *popChild = pgexprChild->Pop();

	if (NULL != pgexprParent &&
		COperator::EopPhysicalSort == pgexprParent->Pop()->Eopid() &&
		COperator::EopPhysicalMotionGather == popChild->Eopid())
	{
		// prevent (Sort --> GatherMerge), since Sort destroys order maintained by GatherMerge
		return !CPhysicalMotionGather::PopConvert(popChild)->FOrderPreserving();
	}


	return COptimizationContext::FOptimize(m_mp, pgexprParent, pgexprChild,
										   pocChild, UlSearchStages());
}

//---------------------------------------------------------------------------
//	@function:
//		CEngine::FSafeToPruneWithDPEStats
//
//	@doc:
//		Determine if a plan rooted by given group expression can be safely
//		pruned during optimization when stats for Dynamic Partition Elimination
//		are derived
//
//---------------------------------------------------------------------------
BOOL
CEngine::FSafeToPruneWithDPEStats(CGroupExpression *pgexpr,
								  CReqdPropPlan *,	// prpp
								  CCostContext *pccChild, ULONG child_index)
{
	SPQOS_ASSERT(SPQOS_FTRACE(EopttraceDeriveStatsForDPE));
	SPQOS_ASSERT(SPQOS_FTRACE(EopttraceEnableSpacePruning));

	if (NULL == pccChild)
	{
		// group expression has not been optimized yet

		CDrvdPropRelational *pdprel =
			CDrvdPropRelational::GetRelationalProperties(
				pgexpr->Pgroup()->Pdp());
		if (0 < pdprel->GetPartitionInfo()->UlConsumers())
		{
			// we cannot bound cost here because of possible DPE that can happen below the operator
			return false;
		}

		return true;
	}

	// first child has been optimized
	CExpressionHandle exprhdl(m_mp);
	exprhdl.Attach(pgexpr);
	ULONG ulNextChild = exprhdl.UlNextOptimizedChildIndex(child_index);
	CDrvdPropRelational *pdprelChild =
		CDrvdPropRelational::GetRelationalProperties(
			(*pgexpr)[ulNextChild]->Pdp());
	if (0 < pdprelChild->GetPartitionInfo()->UlConsumers())
	{
		// we cannot bound cost here because of possible DPE that can happen for the unoptimized child
		return false;
	}

	return true;
}


//---------------------------------------------------------------------------
//	@function:
//		CEngine::FSafeToPrune
//
//	@doc:
//		Determine if a plan rooted by given group expression can be safely
//		pruned during optimization
//
//---------------------------------------------------------------------------
BOOL
CEngine::FSafeToPrune(
	CGroupExpression *pgexpr, CReqdPropPlan *prpp, CCostContext *pccChild,
	ULONG child_index,
	CCost *pcostLowerBound	// output: a lower bound on plan's cost
)
{
	SPQOS_ASSERT(NULL != pcostLowerBound);
	*pcostLowerBound = SPQOPT_INVALID_COST;

	if (!SPQOS_FTRACE(EopttraceEnableSpacePruning))
	{
		// space pruning is disabled
		return false;
	}

	if (SPQOS_FTRACE(EopttraceDeriveStatsForDPE) &&
		!FSafeToPruneWithDPEStats(pgexpr, prpp, pccChild, child_index))
	{
		// stat derivation for Dynamic Partition Elimination may not allow non-trivial cost bounds

		return false;
	}

	// check if container group has a plan for given properties
	CGroup *pgroup = pgexpr->Pgroup();
	COptimizationContext *pocGroup =
		pgroup->PocLookupBest(m_mp, UlSearchStages(), prpp);
	if (NULL != pocGroup && NULL != pocGroup->PccBest())
	{
		// compute a cost lower bound for the equivalent plan rooted by given group expression
		CCost costLowerBound =
			pgexpr->CostLowerBound(m_mp, prpp, pccChild, child_index);
		*pcostLowerBound = costLowerBound;
		if (costLowerBound > pocGroup->PccBest()->Cost())
		{
			// group expression cannot deliver a better plan for given properties and can be safely pruned
			return true;
		}
	}

	return false;
}


//---------------------------------------------------------------------------
//	@function:
//		CEngine::Pmemotmap
//
//	@doc:
//		Build tree map on memo
//
//---------------------------------------------------------------------------
MemoTreeMap *
CEngine::Pmemotmap()
{
	COptimizerConfig *optimizer_config =
		COptCtxt::PoctxtFromTLS()->GetOptimizerConfig();

	if (NULL == m_pmemo->Pmemotmap())
	{
		m_pqc->Prpp()->AddRef();
		COptimizationContext *poc = SPQOS_NEW(m_mp) COptimizationContext(
			m_mp, PgroupRoot(), m_pqc->Prpp(),
			SPQOS_NEW(m_mp) CReqdPropRelational(SPQOS_NEW(m_mp) CColRefSet(
				m_mp)),	 // pass empty required relational properties initially
			SPQOS_NEW(m_mp)
				IStatisticsArray(m_mp),	 // pass empty stats context initially
			0							 // ulSearchStageIndex
		);

		m_pmemo->BuildTreeMap(poc);
		optimizer_config->GetEnumeratorCfg()->SetPlanSpaceSize(
			m_pmemo->Pmemotmap()->UllCount());

		poc->Release();
	}

	return m_pmemo->Pmemotmap();
}


#ifdef SPQOS_DEBUG

//---------------------------------------------------------------------------
//	@function:
//		CEngine::ApplyTransformations
//
//	@doc:
//		Applies given set of xforms to group expression and insert
//		results to memo
//
//---------------------------------------------------------------------------
void
CEngine::ApplyTransformations(CMemoryPool *pmpLocal, CXformSet *xform_set,
							  CGroupExpression *pgexpr)
{
	// iterate over xforms
	CXformSetIter xsi(*xform_set);
	while (xsi.Advance())
	{
		SPQOS_CHECK_ABORT;
		CXform *pxform = CXformFactory::Pxff()->Pxf(xsi.TBit());

		// transform group expression, and insert results to memo
		CXformResult *pxfres = SPQOS_NEW(m_mp) CXformResult(m_mp);
		ULONG ulElapsedTime = 0;
		ULONG ulNumberOfBindings = 0;
		pgexpr->Transform(m_mp, pmpLocal, pxform, pxfres, &ulElapsedTime,
						  &ulNumberOfBindings);
		InsertXformResult(pgexpr->Pgroup(), pxfres, pxform->Exfid(), pgexpr,
						  ulElapsedTime, ulNumberOfBindings);
		pxfres->Release();

		if (PssCurrent()->FTimedOut())
		{
			break;
		}
	}
}


//---------------------------------------------------------------------------
//	@function:
//		CEngine::TransitionGroupExpression
//
//	@doc:
//		Transition group expression to a given target state
//
//---------------------------------------------------------------------------
void
CEngine::TransitionGroupExpression(CMemoryPool *pmpLocal,
								   CGroupExpression *pgexpr,
								   CGroupExpression::EState estTarget)
{
	SPQOS_ASSERT(CGroupExpression::estExplored == estTarget ||
				CGroupExpression::estImplemented == estTarget);

	if (PssCurrent()->FTimedOut())
	{
		return;
	}

	CGroupExpression::EState estInitial = CGroupExpression::estExploring;
	CGroup::EState estGroupTargetState = CGroup::estExplored;
	if (CGroupExpression::estImplemented == estTarget)
	{
		estInitial = CGroupExpression::estImplementing;
		estGroupTargetState = CGroup::estImplemented;
	}

	pgexpr->SetState(estInitial);

	// transition all child groups
	ULONG arity = pgexpr->Arity();
	for (ULONG i = 0; i < arity; i++)
	{
		TransitionGroup(pmpLocal, (*pgexpr)[i], estGroupTargetState);

		SPQOS_CHECK_ABORT;
	}

	// find which set of xforms should be used
	CXformSet *xform_set = CXformFactory::Pxff()->PxfsExploration();
	if (CGroupExpression::estImplemented == estTarget)
	{
		xform_set = CXformFactory::Pxff()->PxfsImplementation();
	}

	// get all applicable xforms
	COperator *pop = pgexpr->Pop();
	CXformSet *pxfsCandidates = CLogical::PopConvert(pop)->PxfsCandidates(m_mp);

	// intersect them with the required set of xforms, then apply transformations
	pxfsCandidates->Intersection(xform_set);
	pxfsCandidates->Intersection(PxfsCurrentStage());
	ApplyTransformations(pmpLocal, pxfsCandidates, pgexpr);
	pxfsCandidates->Release();

	pgexpr->SetState(estTarget);
}


//---------------------------------------------------------------------------
//	@function:
//		CEngine::TransitionGroup
//
//	@doc:
//		Transition group to a target state
//
//---------------------------------------------------------------------------
void
CEngine::TransitionGroup(CMemoryPool *pmpLocal, CGroup *pgroup,
						 CGroup::EState estTarget)
{
	// check stack size
	SPQOS_CHECK_STACK_SIZE;

	if (PssCurrent()->FTimedOut())
	{
		return;
	}

	SPQOS_ASSERT(CGroup::estExplored == estTarget ||
				CGroup::estImplemented == estTarget);

	BOOL fTransitioned = false;
	{
		CGroupProxy spq(pgroup);
		fTransitioned = spq.FTransitioned(estTarget);
	}

	// check if we can end recursion early
	if (!fTransitioned)
	{
		CGroup::EState estInitial = CGroup::estExploring;
		CGroupExpression::EState estGExprTargetState =
			CGroupExpression::estExplored;
		if (CGroup::estImplemented == estTarget)
		{
			estInitial = CGroup::estImplementing;
			estGExprTargetState = CGroupExpression::estImplemented;
		}

		CGroupExpression *pgexprCurrent = NULL;

		// transition group's state to initial state
		{
			CGroupProxy spq(pgroup);
			spq.SetState(estInitial);
		}

		// get first group expression
		{
			CGroupProxy spq(pgroup);
			pgexprCurrent = spq.PgexprFirst();
		}

		while (NULL != pgexprCurrent)
		{
			if (!pgexprCurrent->FTransitioned(estGExprTargetState))
			{
				TransitionGroupExpression(pmpLocal, pgexprCurrent,
										  estGExprTargetState);
			}

			if (PssCurrent()->FTimedOut())
			{
				break;
			}

			// get next group expression
			{
				CGroupProxy spq(pgroup);
				pgexprCurrent = spq.PgexprNext(pgexprCurrent);
			}

			SPQOS_CHECK_ABORT;
		}

		// transition group to target state
		{
			CGroupProxy spq(pgroup);
			spq.SetState(estTarget);
		}
	}
}


//---------------------------------------------------------------------------
//	@function:
//		CEngine::PocChild
//
//	@doc:
//		Create optimization context for child group
//
//---------------------------------------------------------------------------
COptimizationContext *
CEngine::PocChild(
	CGroupExpression *pgexpr,		  // parent expression
	COptimizationContext *pocOrigin,  // optimization context of parent operator
	CExpressionHandle
		&exprhdlPlan,  // handle to compute required plan properties
	CExpressionHandle
		&exprhdlRel,  // handle to compute required relational properties
	CDrvdPropArray
		*pdrspqdpChildren,  // derived plan properties of optimized children
	IStatisticsArray *pdrspqstatCurrentCtxt, ULONG child_index, ULONG ulOptReq)
{
	SPQOS_ASSERT(exprhdlPlan.Pgexpr() == pgexpr);
	SPQOS_ASSERT(NULL != pocOrigin);
	SPQOS_ASSERT(NULL != pdrspqdpChildren);
	SPQOS_ASSERT(NULL != pdrspqstatCurrentCtxt);

	CGroup *pgroupChild = (*pgexpr)[child_index];

	// compute required properties of the n-th child
	exprhdlPlan.ComputeChildReqdProps(child_index, pdrspqdpChildren, ulOptReq);
	exprhdlPlan.Prpp(child_index)->AddRef();

	// use current stats for optimizing current child
	IStatisticsArray *stats_ctxt = SPQOS_NEW(m_mp) IStatisticsArray(m_mp);
	CUtils::AddRefAppend<IStatistics, CleanupStats>(stats_ctxt,
													pdrspqstatCurrentCtxt);

	// compute required relational properties
	CReqdPropRelational *prprel = NULL;
	if (CPhysical::PopConvert(pgexpr->Pop())->FPassThruStats())
	{
		// copy requirements from origin context
		prprel = pocOrigin->GetReqdRelationalProps();
	}
	else
	{
		// retrieve requirements from handle
		prprel = exprhdlRel.GetReqdRelationalProps(child_index);
	}
	SPQOS_ASSERT(NULL != prprel);
	prprel->AddRef();

	COptimizationContext *pocChild = SPQOS_NEW(m_mp)
		COptimizationContext(m_mp, pgroupChild, exprhdlPlan.Prpp(child_index),
							 prprel, stats_ctxt, m_ulCurrSearchStage);

	return pocChild;
}

//---------------------------------------------------------------------------
//	@function:
//		CEngine::PccOptimizeChild
//
//	@doc:
//		Optimize child group and return best cost context satisfying
//		required properties
//
//---------------------------------------------------------------------------
CCostContext *
CEngine::PccOptimizeChild(
	CExpressionHandle &exprhdl,	 // initialized with required properties
	CExpressionHandle &exprhdlRel,
	COptimizationContext *pocOrigin,  // optimization context of parent operator
	CDrvdPropArray *pdrspqdp, IStatisticsArray *pdrspqstatCurrentCtxt,
	ULONG child_index, ULONG ulOptReq)
{
	CGroupExpression *pgexpr = exprhdl.Pgexpr();
	CGroup *pgroupChild = (*exprhdl.Pgexpr())[child_index];

	// create optimization context for child group
	COptimizationContext *pocChild =
		PocChild(pgexpr, pocOrigin, exprhdl, exprhdlRel, pdrspqdp,
				 pdrspqstatCurrentCtxt, child_index, ulOptReq);

	if (pgroupChild == pgexpr->Pgroup() && pocChild->Matches(pocOrigin))
	{
		// child context is the same as origin context, this is a deadlock
		pocChild->Release();
		return NULL;
	}

	// optimize child group
	CGroupExpression *pgexprChildBest =
		PgexprOptimize(pgroupChild, pocChild, pgexpr);
	pocChild->Release();
	if (NULL == pgexprChildBest || PssCurrent()->FTimedOut())
	{
		// failed to generate a plan for the child, or search stage is timed-out
		return NULL;
	}

	// derive plan properties of child group optimal implementation
	COptimizationContext *pocFound = pgroupChild->PocLookupBest(
		m_mp, m_search_stage_array->Size(), exprhdl.Prpp(child_index));
	SPQOS_ASSERT(NULL != pocFound);

	CCostContext *pccChildBest = pocFound->PccBest();
	SPQOS_ASSERT(NULL != pccChildBest);

	// check if optimization can be early terminated after first child has been optimized
	CCost costLowerBound(SPQOPT_INVALID_COST);
	if (exprhdl.UlFirstOptimizedChildIndex() == child_index &&
		FSafeToPrune(pgexpr, pocOrigin->Prpp(), pccChildBest, child_index,
					 &costLowerBound))
	{
		// failed to optimize child due to cost bounding
		(void) pgexpr->PccComputeCost(m_mp, pocOrigin, ulOptReq,
									  NULL /*pdrspqoc*/, true /*fPruned*/,
									  costLowerBound);
		return NULL;
	}

	return pccChildBest;
}


//---------------------------------------------------------------------------
//	@function:
//		CEngine::PdrspqocOptimizeChildren
//
//	@doc:
//		Optimize child groups of a given group expression;
//
//---------------------------------------------------------------------------
COptimizationContextArray *
CEngine::PdrspqocOptimizeChildren(
	CExpressionHandle &exprhdl,		  // initialized with required properties
	COptimizationContext *pocOrigin,  // optimization context of parent operator
	ULONG ulOptReq)
{
	SPQOS_ASSERT(NULL != exprhdl.Pgexpr());

	CGroupExpression *pgexpr = exprhdl.Pgexpr();
	const ULONG arity = exprhdl.Arity();
	if (0 == arity)
	{
		// return empty array if no children
		return SPQOS_NEW(m_mp) COptimizationContextArray(m_mp);
	}

	// create array of child derived properties
	CDrvdPropArray *pdrspqdp = SPQOS_NEW(m_mp) CDrvdPropArray(m_mp);

	// initialize current stats context with input stats context
	IStatisticsArray *pdrspqstatCurrentCtxt =
		SPQOS_NEW(m_mp) IStatisticsArray(m_mp);
	CUtils::AddRefAppend<IStatistics, CleanupStats>(pdrspqstatCurrentCtxt,
													pocOrigin->Pdrspqstat());

	// initialize required relational properties computation
	CExpressionHandle exprhdlRel(m_mp);
	CGroupExpression *pgexprForStats =
		pgexpr->Pgroup()->PgexprBestPromise(m_mp, pgexpr);
	if (NULL != pgexprForStats)
	{
		exprhdlRel.Attach(pgexprForStats);
		exprhdlRel.DeriveProps(NULL /*pdpctxt*/);
		exprhdlRel.ComputeReqdProps(pocOrigin->GetReqdRelationalProps(),
									0 /*ulOptReq*/);
	}

	// iterate over child groups and optimize them
	BOOL fSuccess = true;
	ULONG child_index = exprhdl.UlFirstOptimizedChildIndex();
	do
	{
		CGroup *pgroupChild = (*exprhdl.Pgexpr())[child_index];
		if (pgroupChild->FScalar())
		{
			// skip scalar groups from optimization
			continue;
		}

		CCostContext *pccChildBest =
			PccOptimizeChild(exprhdl, exprhdlRel, pocOrigin, pdrspqdp,
							 pdrspqstatCurrentCtxt, child_index, ulOptReq);
		if (NULL == pccChildBest)
		{
			fSuccess = false;
			break;
		}

		CExpressionHandle exprhdlChild(m_mp);
		exprhdlChild.Attach(pccChildBest);
		exprhdlChild.DerivePlanPropsForCostContext();
		exprhdlChild.Pdp()->AddRef();
		pdrspqdp->Append(exprhdlChild.Pdp());

		// copy stats of child's best cost context to current stats context
		IStatistics *pstat = pccChildBest->Pstats();
		pstat->AddRef();
		pdrspqstatCurrentCtxt->Append(pstat);

		SPQOS_CHECK_ABORT;
	} while (exprhdl.FNextChildIndex(&child_index));
	pdrspqdp->Release();
	pdrspqstatCurrentCtxt->Release();

	if (!fSuccess)
	{
		return NULL;
	}

	// return child optimization contexts array
	return PdrspqocChildren(m_mp, exprhdl);
}


//---------------------------------------------------------------------------
//	@function:
//		CEngine::OptimizeGroupExpression
//
//	@doc:
//		Optimize group expression under a given context
//
//---------------------------------------------------------------------------
void
CEngine::OptimizeGroupExpression(CGroupExpression *pgexpr,
								 COptimizationContext *poc)
{
	CGroup *pgroup = pgexpr->Pgroup();
	const ULONG ulOptRequests =
		CPhysical::PopConvert(pgexpr->Pop())->UlOptRequests();
	for (ULONG ul = 0; ul < ulOptRequests; ul++)
	{
		CExpressionHandle exprhdl(m_mp);
		exprhdl.Attach(pgexpr);
		exprhdl.DeriveProps(NULL /*pdpctxt*/);

		// check if group expression optimization can be early terminated without optimizing any child
		CCost costLowerBound(SPQOPT_INVALID_COST);
		if (FSafeToPrune(pgexpr, poc->Prpp(), NULL /*pccChild*/,
						 spqos::ulong_max /*child_index*/, &costLowerBound))
		{
			(void) pgexpr->PccComputeCost(m_mp, poc, ul, NULL /*pdrspqoc*/,
										  true /*fPruned*/, costLowerBound);
			continue;
		}

		if (FCheckReqdProps(exprhdl, poc->Prpp(), ul))
		{
			// load required properties on the handle
			exprhdl.InitReqdProps(poc->Prpp());

			// optimize child groups
			COptimizationContextArray *pdrspqoc =
				PdrspqocOptimizeChildren(exprhdl, poc, ul);

			if (NULL != pdrspqoc &&
				FCheckEnfdProps(m_mp, pgexpr, poc, ul, pdrspqoc))
			{
				// compute group expression cost under the current optimization context
				CCostContext *pccComputed = pgexpr->PccComputeCost(
					m_mp, poc, ul, pdrspqoc, false /*fPruned*/, CCost(0.0));

				if (NULL != pccComputed)
				{
					// update best group expression under the current optimization context
					pgroup->UpdateBestCost(poc, pccComputed);
				}
			}

			CRefCount::SafeRelease(pdrspqoc);
		}

		SPQOS_CHECK_ABORT;
	}
}


//---------------------------------------------------------------------------
//	@function:
//		CEngine::PgexprOptimize
//
//	@doc:
//		Optimize group under a given context;
//
//---------------------------------------------------------------------------
CGroupExpression *
CEngine::PgexprOptimize(CGroup *pgroup, COptimizationContext *poc,
						CGroupExpression *pgexprOrigin)
{
	// recursive function - check stack
	SPQOS_CHECK_STACK_SIZE;

	COptimizationContext *pocFound = pgroup->PocInsert(poc);
	if (poc != pocFound)
	{
		SPQOS_ASSERT(COptimizationContext::estOptimized == pocFound->Est());
		return pocFound->PgexprBest();
	}

	// add-ref context to pin it in hash table
	poc->AddRef();
	poc->SetState(COptimizationContext::estOptimizing);

	EOptimizationLevel eolCurrent = pgroup->EolMax();
	while (EolSentinel != eolCurrent)
	{
		CGroupExpression *pgexprCurrent = NULL;
		{
			CGroupProxy spq(pgroup);
			pgexprCurrent = spq.PgexprSkipLogical(NULL /*pgexpr*/);
		}

		while (NULL != pgexprCurrent)
		{
			if (FOptimizeChild(pgexprOrigin, pgexprCurrent, poc, eolCurrent))
			{
				OptimizeGroupExpression(pgexprCurrent, poc);
			}

			if (PssCurrent()->FTimedOut())
			{
				break;
			}

			// move to next group expression
			{
				CGroupProxy spq(pgroup);
				pgexprCurrent = spq.PgexprSkipLogical(pgexprCurrent);
			}
		}

		// damp optimization level
		eolCurrent = EolDamp(eolCurrent);

		SPQOS_CHECK_ABORT;
	}
	poc->SetState(COptimizationContext::estOptimized);

	return poc->PgexprBest();
}


//---------------------------------------------------------------------------
//	@function:
//		CEngine::Explore
//
//	@doc:
//		Apply all exploration xforms
//
//---------------------------------------------------------------------------
void
CEngine::Explore()
{
	SPQOS_ASSERT(NULL != m_pqc);
	SPQOS_ASSERT(NULL != PgroupRoot());

	// explore root group
	SPQOS_ASSERT(!PgroupRoot()->FExplored());

	TransitionGroup(m_mp, PgroupRoot(), CGroup::estExplored /*estTarget*/);
	SPQOS_ASSERT_IMP(!PssCurrent()->FTimedOut(), PgroupRoot()->FExplored());
}


//---------------------------------------------------------------------------
//	@function:
//		CEngine::Implement
//
//	@doc:
//		Apply all implementation xforms
//
//---------------------------------------------------------------------------
void
CEngine::Implement()
{
	SPQOS_ASSERT(NULL != m_pqc);
	SPQOS_ASSERT(NULL != PgroupRoot());

	// implement root group
	SPQOS_ASSERT(!PgroupRoot()->FImplemented());

	TransitionGroup(m_mp, PgroupRoot(), CGroup::estImplemented /*estTarget*/);
	SPQOS_ASSERT_IMP(!PssCurrent()->FTimedOut(), PgroupRoot()->FImplemented());
}


//---------------------------------------------------------------------------
//	@function:
//		CEngine::RecursiveOptimize
//
//	@doc:
//		Recursive optimization
//
//---------------------------------------------------------------------------
void
CEngine::RecursiveOptimize()
{
	COptimizerConfig *optimizer_config =
		COptCtxt::PoctxtFromTLS()->GetOptimizerConfig();

	CAutoTimer at("\n[OPT]: Total Optimization Time",
				  SPQOS_FTRACE(EopttracePrintOptimizationStatistics));

	const ULONG ulSearchStages = m_search_stage_array->Size();
	for (ULONG ul = 0; !FSearchTerminated() && ul < ulSearchStages; ul++)
	{
		PssCurrent()->RestartTimer();

		// apply exploration xforms
		Explore();

		// run exploration completion operations
		FinalizeExploration();

		// apply implementation xforms
		Implement();

		// run implementation completion operations
		FinalizeImplementation();

		// optimize root group
		m_pqc->Prpp()->AddRef();
		COptimizationContext *poc = SPQOS_NEW(m_mp) COptimizationContext(
			m_mp, PgroupRoot(), m_pqc->Prpp(),
			SPQOS_NEW(m_mp) CReqdPropRelational(SPQOS_NEW(m_mp) CColRefSet(
				m_mp)),	 // pass empty required relational properties initially
			SPQOS_NEW(m_mp) IStatisticsArray(
				m_mp),	// pass an empty stats context initially
			m_ulCurrSearchStage);
		(void) PgexprOptimize(PgroupRoot(), poc, NULL /*pgexprOrigin*/);
		poc->Release();

		// extract best plan found at the end of current search stage
		CExpression *pexprPlan = m_pmemo->PexprExtractPlan(
			m_mp, m_pmemo->PgroupRoot(), m_pqc->Prpp(),
			m_search_stage_array->Size());
		PssCurrent()->SetBestExpr(pexprPlan);

		FinalizeSearchStage();
	}

	{
		CAutoTrace atSearch(m_mp);
		atSearch.Os() << "[OPT]: Search terminated at stage "
					  << m_ulCurrSearchStage << "/"
					  << m_search_stage_array->Size();
	}

	if (optimizer_config->GetEnumeratorCfg()->FSample())
	{
		SamplePlans();
	}
}

void
CEngine::DbgPrintExpr(int group_no, int context_no)
{
	CAutoTrace at(m_mp);

	SPQOS_TRY
	{
		CGroup *top_group = m_pmemo->Pgroup(group_no);
		if (NULL != top_group)
		{
			COptimizationContext *poc = top_group->Ppoc(context_no);

			if (NULL != poc)
			{
				CExpression *extracted_expr = m_pmemo->PexprExtractPlan(
					m_mp, top_group, poc->Prpp(), m_search_stage_array->Size());
				extracted_expr->OsPrint(at.Os());
				extracted_expr->Release();
			}
			else
			{
				at.Os() << "error: invalid context number";
			}
		}
		else
		{
			at.Os() << "error: invalid group number";
		}
	}
	SPQOS_CATCH_EX(ex)
	{
		at.Os() << "error, couldn't complete the request";
	}
	SPQOS_CATCH_END;
}

#endif	// SPQOS_DEBUG


//---------------------------------------------------------------------------
//	@function:
//		CEngine::PdrspqocChildren
//
//	@doc:
//		Return array of child optimization contexts corresponding
//		to handle requirements
//
//---------------------------------------------------------------------------
COptimizationContextArray *
CEngine::PdrspqocChildren(CMemoryPool *mp, CExpressionHandle &exprhdl)
{
	SPQOS_ASSERT(NULL != exprhdl.Pgexpr());

	COptimizationContextArray *pdrspqoc =
		SPQOS_NEW(mp) COptimizationContextArray(mp);
	const ULONG arity = exprhdl.Arity();
	for (ULONG ul = 0; ul < arity; ul++)
	{
		CGroup *pgroupChild = (*exprhdl.Pgexpr())[ul];
		if (!pgroupChild->FScalar())
		{
			COptimizationContext *poc = pgroupChild->PocLookupBest(
				mp, m_search_stage_array->Size(), exprhdl.Prpp(ul));
			SPQOS_ASSERT(NULL != poc);

			poc->AddRef();
			pdrspqoc->Append(poc);
		}
	}

	return pdrspqoc;
}


//---------------------------------------------------------------------------
//	@function:
//		CEngine::ScheduleMainJob
//
//	@doc:
//		Create and schedule the main optimization job
//
//---------------------------------------------------------------------------
void
CEngine::ScheduleMainJob(CSchedulerContext *psc, COptimizationContext *poc)
{
	SPQOS_ASSERT(NULL != PgroupRoot());

	CJobGroupOptimization::ScheduleJob(psc, PgroupRoot(), NULL /*pgexprOrigin*/,
									   poc, NULL /*pjParent*/);
}


//---------------------------------------------------------------------------
//	@function:
//		CEngine::FinalizeExploration
//
//	@doc:
//		Execute operations after exploration completes
//
//---------------------------------------------------------------------------
void
CEngine::FinalizeExploration()
{
	GroupMerge();

	if (m_pqc->FDeriveStats())
	{
		// derive statistics
		m_pmemo->ResetStats();
		DeriveStats(m_mp);
	}

	if (!SPQOS_FTRACE(EopttraceDonotDeriveStatsForAllGroups))
	{
		// derive stats for every group without stats
		m_pmemo->DeriveStatsIfAbsent(m_mp);
	}

	if (SPQOS_FTRACE(EopttracePrintMemoAfterExploration))
	{
		{
			CAutoTrace at(m_mp);
			at.Os() << "MEMO after exploration (stage " << m_ulCurrSearchStage
					<< ")" << std::endl;
		}

		{
			CAutoTrace at(m_mp);
			at.Os() << *this;
		}
	}

	if (SPQOS_FTRACE(EopttracePrintOptimizationStatistics))
	{
		CAutoTrace at(m_mp);
		(void) OsPrintMemoryConsumption(
			at.Os(), "Memory consumption after exploration ");
	}
}


//---------------------------------------------------------------------------
//	@function:
//		CEngine::FinalizeImplementation
//
//	@doc:
//		Execute operations after implementation completes
//
//---------------------------------------------------------------------------
void
CEngine::FinalizeImplementation()
{
	if (SPQOS_FTRACE(EopttracePrintMemoAfterImplementation))
	{
		{
			CAutoTrace at(m_mp);
			at.Os() << "MEMO after implementation (stage "
					<< m_ulCurrSearchStage << ")" << std::endl;
		}

		{
			CAutoTrace at(m_mp);
			at.Os() << *this;
		}
	}

	if (SPQOS_FTRACE(EopttracePrintOptimizationStatistics))
	{
		CAutoTrace at(m_mp);
		(void) OsPrintMemoryConsumption(
			at.Os(), "Memory consumption after implementation ");
	}
}

//---------------------------------------------------------------------------
//	@function:
//		CEngine::FinalizeSearchStage
//
//	@doc:
//		Execute operations after search stage completes
//
//---------------------------------------------------------------------------
void
CEngine::FinalizeSearchStage()
{
	ProcessTraceFlags();

	m_xforms->Release();
	m_xforms = NULL;
	m_xforms = SPQOS_NEW(m_mp) CXformSet(m_mp);

	m_ulCurrSearchStage++;
	m_pmemo->ResetGroupStates();
}


//---------------------------------------------------------------------------
//	@function:
//		CEngine::PrintActivatedXforms
//
//	@doc:
//		Print activated xform
//
//---------------------------------------------------------------------------
void
CEngine::PrintActivatedXforms(IOstream &os) const
{
	if (SPQOS_FTRACE(EopttracePrintOptimizationStatistics))
	{
		os << std::endl
		   << "[OPT]: <Begin Xforms - stage " << m_ulCurrSearchStage << ">"
		   << std::endl;
		CXformSetIter xsi(*m_xforms);
		while (xsi.Advance())
		{
			CXform *pxform = CXformFactory::Pxff()->Pxf(xsi.TBit());
			ULONG ulCalls = (ULONG)(
				*m_pdrspqulpXformCalls)[m_ulCurrSearchStage][pxform->Exfid()];
			ULONG ulTime = (ULONG)(
				*m_pdrspqulpXformTimes)[m_ulCurrSearchStage][pxform->Exfid()];
			ULONG ulBindings = (ULONG)(
				*m_pdrspqulpXformBindings)[m_ulCurrSearchStage][pxform->Exfid()];
			ULONG ulResults = (ULONG)(
				*m_pdrspqulpXformResults)[m_ulCurrSearchStage][pxform->Exfid()];
			os << pxform->SzId() << ": " << ulCalls << " calls, " << ulBindings
			   << " total bindings, " << ulResults
			   << " alternatives generated, " << ulTime << "ms" << std::endl;
		}
		os << "[OPT]: <End Xforms - stage " << m_ulCurrSearchStage << ">"
		   << std::endl;
	}
}



//---------------------------------------------------------------------------
//	@function:
//		CEngine::PrintMemoryConsumption
//
//	@doc:
//		Print current memory consumption
//
//---------------------------------------------------------------------------
IOstream &
CEngine::OsPrintMemoryConsumption(IOstream &os, const CHAR *szHeader) const
{
	CMDAccessor *md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();
	CMDAccessor::MDCache *pcache = md_accessor->Pcache();

	os << std::endl
	   << szHeader << "Engine: ["
	   << (DOUBLE) m_mp->TotalAllocatedSize() / SPQOPT_MEM_UNIT << "] "
	   << SPQOPT_MEM_UNIT_NAME << ", MD Cache: ["
	   << (DOUBLE)(pcache->TotalAllocatedSize()) / SPQOPT_MEM_UNIT << "] "
	   << SPQOPT_MEM_UNIT_NAME << ", Total: ["
	   << (DOUBLE)(
			  CMemoryPoolManager::GetMemoryPoolMgr()->TotalAllocatedSize()) /
			  SPQOPT_MEM_UNIT
	   << "] " << SPQOPT_MEM_UNIT_NAME;

	return os;
}


//---------------------------------------------------------------------------
//	@function:
//		CEngine::ProcessTraceFlags
//
//	@doc:
//		Process trace flags after optimization is complete
//
//---------------------------------------------------------------------------
void
CEngine::ProcessTraceFlags()
{
	if (SPQOS_FTRACE(EopttracePrintMemoAfterOptimization))
	{
		{
			CAutoTrace at(m_mp);
			at.Os() << "MEMO after optimization (stage " << m_ulCurrSearchStage
					<< "):" << std::endl;
		}

		{
			CAutoTrace at(m_mp);
			at.Os() << *this;
		}
	}

	if (SPQOS_FTRACE(EopttracePrintOptimizationStatistics))
	{
		CAutoTrace at(m_mp);

		// print optimization stats
		at.Os() << std::endl
				<< "[OPT]: Memo (stage " << m_ulCurrSearchStage << "): ["
				<< (ULONG)(m_pmemo->UlpGroups()) << " groups"
				<< ", " << m_pmemo->UlDuplicateGroups() << " duplicate groups"
				<< ", " << m_pmemo->UlGrpExprs() << " group expressions"
				<< ", " << m_xforms->Size() << " activated xforms]";

		at.Os() << std::endl
				<< "[OPT]: stage " << m_ulCurrSearchStage << " completed in "
				<< PssCurrent()->UlElapsedTime() << " msec, ";
		if (NULL == PssCurrent()->PexprBest())
		{
			at.Os() << " no plan was found";
		}
		else
		{
			at.Os() << " plan with cost " << PssCurrent()->CostBest()
					<< " was found";
		}

		PrintActivatedXforms(at.Os());

		(void) OsPrintMemoryConsumption(
			at.Os(), "Memory consumption after optimization ");
	}
}


//---------------------------------------------------------------------------
//	@function:
//		CEngine::Optimize
//
//	@doc:
//		Main driver of optimization engine
//
//---------------------------------------------------------------------------
void
CEngine::Optimize()
{
	COptimizerConfig *optimizer_config =
		COptCtxt::PoctxtFromTLS()->GetOptimizerConfig();

	CAutoTimer at("\n[OPT]: Total Optimization Time",
				  SPQOS_FTRACE(EopttracePrintOptimizationStatistics));

	SPQOS_ASSERT(NULL != PgroupRoot());
	SPQOS_ASSERT(NULL != COptCtxt::PoctxtFromTLS());

	const ULONG ulJobs =
		std::min((ULONG) SPQOPT_JOBS_CAP,
				 (ULONG)(m_pmemo->UlpGroups() * SPQOPT_JOBS_PER_GROUP));
	CJobFactory jf(m_mp, ulJobs);
	CScheduler sched(m_mp, ulJobs);

	CSchedulerContext sc;
	sc.Init(m_mp, &jf, &sched, this);

	const ULONG ulSearchStages = m_search_stage_array->Size();
	for (ULONG ul = 0; !FSearchTerminated() && ul < ulSearchStages; ul++)
	{
		PssCurrent()->RestartTimer();

		// optimize root group
		m_pqc->Prpp()->AddRef();
		COptimizationContext *poc = SPQOS_NEW(m_mp) COptimizationContext(
			m_mp, PgroupRoot(), m_pqc->Prpp(),
			SPQOS_NEW(m_mp) CReqdPropRelational(SPQOS_NEW(m_mp) CColRefSet(
				m_mp)),	 // pass empty required relational properties initially
			SPQOS_NEW(m_mp)
				IStatisticsArray(m_mp),	 // pass empty stats context initially
			m_ulCurrSearchStage);

		// schedule main optimization job
		ScheduleMainJob(&sc, poc);

		// run optimization job
		CScheduler::Run(&sc);

		poc->Release();

		// extract best plan found at the end of current search stage
		CExpression *pexprPlan = m_pmemo->PexprExtractPlan(
			m_mp, m_pmemo->PgroupRoot(), m_pqc->Prpp(),
			m_search_stage_array->Size());
		PssCurrent()->SetBestExpr(pexprPlan);

		FinalizeSearchStage();
	}


	if (SPQOS_FTRACE(EopttracePrintOptimizationStatistics))
	{
		CAutoTrace atSearch(m_mp);
		atSearch.Os() << "[OPT]: Search terminated at stage "
					  << m_ulCurrSearchStage << "/"
					  << m_search_stage_array->Size();
	}


	if (optimizer_config->GetEnumeratorCfg()->FSample())
	{
		SamplePlans();
	}
}


//---------------------------------------------------------------------------
//	@function:
//		CEngine::CEngine
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CExpression *
CEngine::PexprUnrank(ULLONG plan_id)
{
	// The CTE map will be updated by the Producer instead of the Sequence operator
	// because we are doing a DFS traversal of the TreeMap.
	CDrvdPropCtxtPlan *pdpctxtplan =
		SPQOS_NEW(m_mp) CDrvdPropCtxtPlan(m_mp, false /*fUpdateCTEMap*/);
	CExpression *pexpr = Pmemotmap()->PrUnrank(m_mp, pdpctxtplan, plan_id);
	pdpctxtplan->Release();

#ifdef SPQOS_DEBUG
	// check plan using configured plan checker, if any
	COptimizerConfig *optimizer_config =
		COptCtxt::PoctxtFromTLS()->GetOptimizerConfig();
	CEnumeratorConfig *pec = optimizer_config->GetEnumeratorCfg();
	BOOL fCheck = pec->FCheckPlan(pexpr);
	if (!fCheck)
	{
		CAutoTrace at(m_mp);
		at.Os() << "\nextracted plan failed PlanChecker function: " << std::endl
				<< *pexpr;
	}
	SPQOS_ASSERT(fCheck);
#endif	// SPQOS_DEBUG

	return pexpr;
}

//---------------------------------------------------------------------------
//	@function:
//		CEngine::PexprExtractPlan
//
//	@doc:
//		Extract a physical plan from the memo
//
//---------------------------------------------------------------------------
CExpression *
CEngine::PexprExtractPlan()
{
	SPQOS_ASSERT(NULL != m_pqc);
	SPQOS_ASSERT(NULL != m_pmemo);
	SPQOS_ASSERT(NULL != m_pmemo->PgroupRoot());

	BOOL fGenerateAlt = false;
	COptimizerConfig *optimizer_config =
		COptCtxt::PoctxtFromTLS()->GetOptimizerConfig();
	CEnumeratorConfig *pec = optimizer_config->GetEnumeratorCfg();
	if (pec->FEnumerate())
	{
		CAutoTrace at(m_mp);
		ULLONG ullCount = Pmemotmap()->UllCount();
		at.Os() << "[OPT]: Number of plan alternatives: " << ullCount
				<< std::endl;

		if (0 < pec->GetPlanId())
		{
			if (pec->GetPlanId() > ullCount)
			{
				// an invalid plan number is chosen
				SPQOS_RAISE(spqopt::ExmaSPQOPT, spqopt::ExmiInvalidPlanAlternative,
						   pec->GetPlanId(), ullCount);
			}

			// a valid plan number was chosen
			fGenerateAlt = true;
		}
	}

	CExpression *pexpr = NULL;
	if (fGenerateAlt)
	{
		pexpr = PexprUnrank(pec->GetPlanId() -
							1 /*rank of plan alternative is zero-based*/);
		CAutoTrace at(m_mp);
		at.Os() << "[OPT]: Successfully generated plan: " << pec->GetPlanId()
				<< std::endl;
	}
	else
	{
		pexpr = m_pmemo->PexprExtractPlan(m_mp, m_pmemo->PgroupRoot(),
										  m_pqc->Prpp(),
										  m_search_stage_array->Size());
	}

	if (NULL == pexpr)
	{
		SPQOS_RAISE(spqopt::ExmaSPQOPT, spqopt::ExmiNoPlanFound);
	}

	return pexpr;
}


//---------------------------------------------------------------------------
//	@function:
//		CEngine::UllRandomPlanId
//
//	@doc:
//		Generate random plan id
//
//---------------------------------------------------------------------------
ULLONG
CEngine::UllRandomPlanId(ULONG *seed)
{
	ULLONG ullCount = Pmemotmap()->UllCount();
	ULLONG plan_id = 0;
	do
	{
		plan_id = clib::Rand(seed);
	} while (plan_id >= ullCount);

	return plan_id;
}

//---------------------------------------------------------------------------
//	@function:
//		CEngine::FValidPlanSample
//
//	@doc:
//		Extract a plan sample and handle exceptions according to enumerator
//		configurations
//
//---------------------------------------------------------------------------
BOOL
CEngine::FValidPlanSample(CEnumeratorConfig *pec, ULLONG plan_id,
						  CExpression **ppexpr	// output: extracted plan
)
{
	SPQOS_ASSERT(NULL != pec);
	SPQOS_ASSERT(NULL != ppexpr);

	BOOL fValidPlan = true;
	if (pec->FSampleValidPlans())
	{
		// if enumerator is configured to extract valid plans only,
		// we extract plan and catch invalid plan exception here
		SPQOS_TRY
		{
			*ppexpr = PexprUnrank(plan_id);
		}
		SPQOS_CATCH_EX(ex)
		{
			if (SPQOS_MATCH_EX(ex, spqopt::ExmaSPQOPT,
							  spqopt::ExmiUnsatisfiedRequiredProperties))
			{
				SPQOS_RESET_EX;
				fValidPlan = false;
			}
			else
			{
				// for all other exceptions, we bail out
				SPQOS_RETHROW(ex);
			}
		}
		SPQOS_CATCH_END;
	}
	else
	{
		// otherwise, we extract plan and leave exception handling to the caller
		*ppexpr = PexprUnrank(plan_id);
	}

	return fValidPlan;
}


//---------------------------------------------------------------------------
//	@function:
//		CEngine::SamplePlans
//
//	@doc:
//		Sample distribution of possible plans uniformly;
//
//---------------------------------------------------------------------------
void
CEngine::SamplePlans()
{
	COptimizerConfig *optimizer_config =
		COptCtxt::PoctxtFromTLS()->GetOptimizerConfig();
	SPQOS_ASSERT(NULL != optimizer_config);

	CEnumeratorConfig *pec = optimizer_config->GetEnumeratorCfg();

	ULLONG ullSamples = pec->UllInputSamples();
	SPQOS_ASSERT(0 < ullSamples);

	pec->ClearSamples();

	ULLONG ullCount = Pmemotmap()->UllCount();
	if (0 == ullCount)
	{
		// optimizer generated no plans
		return;
	}

	// generate full plan space when space size is less than or equal to
	// the required number of samples
	BOOL fGenerateAll = (ullSamples >= ullCount);

	ULLONG ullTargetSamples = ullSamples;
	if (fGenerateAll)
	{
		ullTargetSamples = ullCount;
	}

	// find cost of best plan
	CExpression *pexpr =
		m_pmemo->PexprExtractPlan(m_mp, m_pmemo->PgroupRoot(), m_pqc->Prpp(),
								  m_search_stage_array->Size());
	CCost costBest = pexpr->Cost();
	pec->SetBestCost(costBest);
	pexpr->Release();

	// generate randomized seed using local time
	TIMEVAL tv;
	syslib::GetTimeOfDay(&tv, NULL /*timezone*/);
	ULONG seed = CombineHashes((ULONG) tv.tv_sec, (ULONG) tv.tv_usec);

	// set maximum number of iterations based number of samples
	// we use maximum iteration to prevent infinite looping below
	const ULLONG ullMaxIters = ullTargetSamples * SPQOPT_SAMPLING_MAX_ITERS;
	ULLONG ullIters = 0;
	ULLONG ull = 0;
	while (ullIters < ullMaxIters && ull < ullTargetSamples)
	{
		// generate id of plan to be extracted
		ULLONG plan_id = ull;
		if (!fGenerateAll)
		{
			plan_id = UllRandomPlanId(&seed);
		}

		pexpr = NULL;
		BOOL fAccept = false;
		if (FValidPlanSample(pec, plan_id, &pexpr))
		{
			// add plan to the sample if it is below cost threshold
			CCost cost = pexpr->Cost();
			fAccept = pec->FAddSample(plan_id, cost);
			pexpr->Release();
		}

		if (fGenerateAll || fAccept)
		{
			ull++;
		}

		ullIters++;
	}

	pec->PrintPlanSample();
}


//---------------------------------------------------------------------------
//	@function:
//		CEngine::FCheckEnfdProps
//
//	@doc:
//		Check enforceable properties and append enforcers to the current group if
//		required.
//
//		This check is done in two steps:
//
//		First, it determines if any particular property needs to be enforced at
//		all. For example, the EopttraceDisableSort traceflag can disable order
//		enforcement. Also, if there are no partitioned tables referenced in the
//		subtree, partition propagation enforcement can be skipped.
//
//		Second, EPET methods are called for each property to determine if an
//		enforcer needs to be added. These methods in turn call into virtual
//		methods in the different operators. For example, CPhysical::EpetOrder()
//		is used to determine a Sort node needs to be added to the group. These
//		methods are passed an expression handle (to access derived properties of
//		the subtree) and the required properties as a object of a subclass of
//		CEnfdProp.
//
//		Finally, based on return values of the EPET methods,
//		CEnfdProp::AppendEnforcers() is called for each of the enforced
//		properties.
//
//		Returns true if no enforcers were created because they were deemed
//		unnecessary or optional i.e all enforced properties were satisfied for
//		the group expression under the current optimization context.  Returns
//		false otherwise.
//
//		NB: This method is only concerned with a certain enforcer needs to be
//		added into the group. Once added, there is no connection between the
//		enforcer and the operator that created it. That is although some group
//		expression X created the enforcer E, later, during costing, E can still
//		decide to pick some other group expression Y for its child, since
//		theoretically, all group expressions in a group are equivalent.
//
//---------------------------------------------------------------------------
BOOL
CEngine::FCheckEnfdProps(CMemoryPool *mp, CGroupExpression *pgexpr,
						 COptimizationContext *poc, ULONG ulOptReq,
						 COptimizationContextArray *pdrspqoc)
{
	SPQOS_CHECK_ABORT;

	if (SPQOS_FTRACE(EopttracePrintMemoEnforcement))
	{
		CAutoTrace at(m_mp);
		at.Os() << "CEngine::FCheckEnfdProps (Group ID: "
				<< pgexpr->Pgroup()->Id() << " Expression ID: " << pgexpr->Id()
				<< ")" << std::endl;
		m_pmemo->OsPrint(at.Os());
	}

	// check if all children could be successfully optimized
	if (!FChildrenOptimized(pdrspqoc))
	{
		return false;
	}

	// load a handle with derived plan properties
	poc->AddRef();
	pgexpr->AddRef();
	pdrspqoc->AddRef();
	CCostContext *pcc = SPQOS_NEW(mp) CCostContext(mp, poc, ulOptReq, pgexpr);
	pcc->SetChildContexts(pdrspqoc);
	CExpressionHandle exprhdl(mp);
	exprhdl.Attach(pcc);
	exprhdl.DerivePlanPropsForCostContext();


	CPhysical *popPhysical = CPhysical::PopConvert(exprhdl.Pop());
	CReqdPropPlan *prpp = poc->Prpp();

	// check whether the current physical operator satisfies the CTE requirements
	// and whether it is a motion over unresolved part consumers
	if (!FValidCTEAndPartitionProperties(mp, exprhdl, prpp))
	{
		pcc->Release();
		return false;
	}

	// Determine if any property enforcement is disable or unnecessary
	BOOL fOrderReqd = !SPQOS_FTRACE(EopttraceDisableSort) &&
					  !prpp->Peo()->PosRequired()->IsEmpty();

	// CPhysicalLeftOuterIndexNLJoin requires the inner child to be any
	// distribution but random. The OR makes an exception in this case.
	// This should be generalized when more physical operators require
	// this pattern. We need an explicit check for CPhysicalLeftOuterIndexNLJoin
	// when there are no motions, therefore we need to handle this exceptional
	// case here.
	//
	// Similar exceptions should be OR'd into fDistributionReqdException to
	// force checking EpetDistribution on the physical operation
	BOOL fDistributionReqdException =
		popPhysical->Eopid() == COperator::EopPhysicalLeftOuterIndexNLJoin;
	BOOL fDistributionReqd =
		!SPQOS_FTRACE(EopttraceDisableMotions) &&
		((CDistributionSpec::EdtAny != prpp->Ped()->PdsRequired()->Edt()) ||
		 fDistributionReqdException);

	BOOL fRewindabilityReqd = !SPQOS_FTRACE(EopttraceDisableSpool) &&
							  (prpp->Per()->PrsRequired()->IsCheckRequired());

	BOOL fPartPropagationReqd =
		!SPQOS_FTRACE(EopttraceDisablePartPropagation) &&
		prpp->Pepp()->PppsRequired()->FPartPropagationReqd();

	// Determine if adding an enforcer to the group is required, optional,
	// unnecessary or prohibited over the group expression and given the current
	// optimization context (required properties)

	// get order enforcing type
	CEnfdProp::EPropEnforcingType epetOrder =
		prpp->Peo()->Epet(exprhdl, popPhysical, fOrderReqd);

	// get distribution enforcing type
	CEnfdProp::EPropEnforcingType epetDistribution = prpp->Ped()->Epet(
		exprhdl, popPhysical, prpp->Pepp()->PppsRequired(), fDistributionReqd);

	// get rewindability enforcing type
	CEnfdProp::EPropEnforcingType epetRewindability =
		prpp->Per()->Epet(exprhdl, popPhysical, fRewindabilityReqd);

	// get partition propagation enforcing type
	CEnfdProp::EPropEnforcingType epetPartitionPropagation =
		prpp->Pepp()->Epet(exprhdl, popPhysical, fPartPropagationReqd);

	// Skip adding enforcers entirely if any property determines it to be
	// 'prohibited'. In this way, a property may veto out the creation of an
	// enforcer for the current group expression and optimization context.
	//
	// NB: Even though an enforcer E is not added because of some group
	// expression G because it was prohibited, some other group expression H may
	// decide to add it. And if E is added, it is possible for E to consider both
	// G and H as its child.
	if (FProhibited(epetOrder, epetDistribution, epetRewindability,
					epetPartitionPropagation))
	{
		pcc->Release();
		return false;
	}

	CExpressionArray *pdrspqexprEnforcers = SPQOS_NEW(mp) CExpressionArray(mp);

	// extract a leaf pattern from target group
	CBinding binding;
	CExpression *pexpr = binding.PexprExtract(
		m_mp, exprhdl.Pgexpr(), m_pexprEnforcerPattern, NULL /* pexprLast */);
	SPQOS_ASSERT(NULL != pexpr);
	SPQOS_ASSERT(pexpr->Pgexpr()->Pgroup() == pgexpr->Pgroup());

	prpp->Peo()->AppendEnforcers(mp, prpp, pdrspqexprEnforcers, pexpr, epetOrder,
								 exprhdl);
	prpp->Ped()->AppendEnforcers(mp, prpp, pdrspqexprEnforcers, pexpr,
								 epetDistribution, exprhdl);
	prpp->Per()->AppendEnforcers(mp, prpp, pdrspqexprEnforcers, pexpr,
								 epetRewindability, exprhdl);
	prpp->Pepp()->AppendEnforcers(mp, prpp, pdrspqexprEnforcers, pexpr,
								  epetPartitionPropagation, exprhdl);

	if (0 < pdrspqexprEnforcers->Size())
	{
		AddEnforcers(exprhdl.Pgexpr(), pdrspqexprEnforcers);
	}
	pdrspqexprEnforcers->Release();
	pexpr->Release();
	pcc->Release();

	return FOptimize(epetOrder, epetDistribution, epetRewindability,
					 epetPartitionPropagation);
}

//---------------------------------------------------------------------------
//	@function:
//		CEngine::FValidCTEAndPartitionProperties
//
//	@doc:
//		Check if the given expression has valid cte and partition properties
//		with respect to the given requirements. This function returns true iff
//		ALL the following conditions are met:
//		1. The expression satisfies the CTE requirements
//		2. The root of the expression is not a motion over an unresolved part consumer
//		3. The expression does not have an unneeded part propagator
//
//---------------------------------------------------------------------------
BOOL
CEngine::FValidCTEAndPartitionProperties(CMemoryPool *mp,
										 CExpressionHandle &exprhdl,
										 CReqdPropPlan *prpp)
{
	CPhysical *popPhysical = CPhysical::PopConvert(exprhdl.Pop());
	CPartIndexMap *ppimDrvd = CDrvdPropPlan::Pdpplan(exprhdl.Pdp())->Ppim();

	return popPhysical->FProvidesReqdCTEs(exprhdl, prpp->Pcter()) &&
		   !CUtils::FMotionOverUnresolvedPartConsumers(
			   mp, exprhdl, prpp->Pepp()->PppsRequired()->Ppim()) &&
		   !ppimDrvd->FContainsRedundantPartitionSelectors(
			   prpp->Pepp()->PppsRequired()->Ppim());
}

//---------------------------------------------------------------------------
//	@function:
//		CEngine::FChildrenOptimized
//
//	@doc:
//		Check if all children were successfully optimized
//
//---------------------------------------------------------------------------
BOOL
CEngine::FChildrenOptimized(COptimizationContextArray *pdrspqoc)
{
	SPQOS_ASSERT(NULL != pdrspqoc);

	const ULONG length = pdrspqoc->Size();
	for (ULONG ul = 0; ul < length; ul++)
	{
		if (NULL == (*pdrspqoc)[ul]->PgexprBest())
		{
			return false;
		}
	}

	return true;
}

//---------------------------------------------------------------------------
//	@function:
//		CEngine::FOptimize
//
//	@doc:
//		Check if optimization is possible under the given property enforcing
//		types
//
//---------------------------------------------------------------------------
BOOL
CEngine::FOptimize(CEnfdProp::EPropEnforcingType epetOrder,
				   CEnfdProp::EPropEnforcingType epetDistribution,
				   CEnfdProp::EPropEnforcingType epetRewindability,
				   CEnfdProp::EPropEnforcingType epetPropagation)
{
	return CEnfdProp::FOptimize(epetOrder) &&
		   CEnfdProp::FOptimize(epetDistribution) &&
		   CEnfdProp::FOptimize(epetRewindability) &&
		   CEnfdProp::FOptimize(epetPropagation);
}

//---------------------------------------------------------------------------
//	@function:
//		CEngine::FProhibited
//
//	@doc:
//		Check if any of the given property enforcing types prohibits enforcement
//
//---------------------------------------------------------------------------
BOOL
CEngine::FProhibited(CEnfdProp::EPropEnforcingType epetOrder,
					 CEnfdProp::EPropEnforcingType epetDistribution,
					 CEnfdProp::EPropEnforcingType epetRewindability,
					 CEnfdProp::EPropEnforcingType epetPropagation)
{
	return (CEnfdProp::EpetProhibited == epetOrder ||
			CEnfdProp::EpetProhibited == epetDistribution ||
			CEnfdProp::EpetProhibited == epetRewindability ||
			CEnfdProp::EpetProhibited == epetPropagation);
}

//---------------------------------------------------------------------------
//	@function:
//		CEngine::FCheckReqdPartPropagation
//
//	@doc:
//		Check if partition propagation resolver is passed an empty part propagation
// 		spec
//
//---------------------------------------------------------------------------
BOOL
CEngine::FCheckReqdPartPropagation(CPhysical *pop,
								   CEnfdPartitionPropagation *pepp)
{
	BOOL fPartPropagationReqd =
		(NULL != pepp &&
		 pepp->PppsRequired()->Ppim()->FContainsUnresolvedZeroPropagators());

	return fPartPropagationReqd ||
		   COperator::EopPhysicalPartitionSelector != pop->Eopid();
}

//---------------------------------------------------------------------------
//	@function:
//		CEngine::FCheckReqdProps
//
//	@doc:
//		Determine if checking required properties is needed.
//		This method is called after a group expression optimization job has
//		started executing and can be used to cancel the job early.
//
//		This is useful to prevent deadlocks when an enforcer optimizes same
//		group with the same optimization context. Also, in case the subtree
//		doesn't provide the required columns we can save optimization time by
//		skipping this optimization request.
//
//		NB: Only relational properties are available at this stage to make this
//		decision.
//---------------------------------------------------------------------------
BOOL
CEngine::FCheckReqdProps(CExpressionHandle &exprhdl, CReqdPropPlan *prpp,
						 ULONG ulOptReq)
{
	SPQOS_CHECK_ABORT;

	if (SPQOS_FTRACE(EopttracePrintMemoEnforcement))
	{
		CAutoTrace at(m_mp);
		at.Os() << "CEngine::FCheckReqdProps (Group ID: "
				<< exprhdl.Pgexpr()->Pgroup()->Id()
				<< " Expression ID: " << exprhdl.Pgexpr()->Id() << ")"
				<< std::endl;
		m_pmemo->OsPrint(at.Os());
	}

	// check if operator provides required columns
	if (!prpp->FProvidesReqdCols(m_mp, exprhdl, ulOptReq))
	{
		return false;
	}

	CPhysical *popPhysical = CPhysical::PopConvert(exprhdl.Pop());
	COperator::EOperatorId op_id = popPhysical->Eopid();

	// check if sort operator is passed an empty order spec;
	// this check is required to avoid self-deadlocks, i.e.
	// sort optimizing same group with the same optimization context;
	BOOL fOrderReqd = !prpp->Peo()->PosRequired()->IsEmpty();
	if (!fOrderReqd && COperator::EopPhysicalSort == op_id)
	{
		return false;
	}

	// check if motion operator is passed an ANY distribution spec;
	// this check is required to avoid self-deadlocks, i.e.
	// motion optimizing same group with the same optimization context;
	BOOL fDistributionReqd =
		(CDistributionSpec::EdtAny != prpp->Ped()->PdsRequired()->Edt());
	if (!fDistributionReqd && CUtils::FPhysicalMotion(popPhysical))
	{
		return false;
	}

	// check if spool operator is passed a non-rewindable spec;
	// this check is required to avoid self-deadlocks, i.e.
	// spool optimizing same group with the same optimization context;
	if (!prpp->Per()->PrsRequired()->IsCheckRequired() &&
		COperator::EopPhysicalSpool == op_id)
	{
		return false;
	}

	return FCheckReqdPartPropagation(popPhysical, prpp->Pepp());
}

UlongPtrArray *
CEngine::GetNumberOfBindings()
{
	return m_pdrspqulpXformBindings;
}

#ifdef SPQOS_DEBUG
//---------------------------------------------------------------------------
//	@function:
//		CEngine::PrintRoot
//
//	@doc:
//		Print root group
//
//---------------------------------------------------------------------------
void
CEngine::PrintRoot()
{
	CAutoTrace at(m_mp);
	at.Os() << "Root Group:" << std::endl;
	m_pmemo->Pgroup(m_pmemo->PgroupRoot()->Id())->OsPrint(at.Os());
	at.Os() << std::endl;
}


//---------------------------------------------------------------------------
//	@function:
//		CEngine::PrintOptCtxts
//
//	@doc:
//		Print main optimization context and optimal cost context
//
//---------------------------------------------------------------------------
void
CEngine::PrintOptCtxts()
{
	CAutoTrace at(m_mp);
	COptimizationContext *poc = m_pmemo->PgroupRoot()->PocLookupBest(
		m_mp, m_search_stage_array->Size(), m_pqc->Prpp());
	SPQOS_ASSERT(NULL != poc);

	at.Os() << std::endl << "Main Opt Ctxt:" << std::endl;
	(void) poc->OsPrintWithPrefix(at.Os(), " ");
	at.Os() << std::endl;
	at.Os() << "Optimal Cost Ctxt:" << std::endl;
	(void) poc->PccBest()->OsPrint(at.Os());
	at.Os() << std::endl;
}
#endif	// SPQOS_DEBUG


//---------------------------------------------------------------------------
//	@function:
//		CEngine::OsPrint
//
//	@doc:
//		print function
//
//---------------------------------------------------------------------------
IOstream &
CEngine::OsPrint(IOstream &os) const
{
	return m_pmemo->OsPrint(os);
}

// EOF
