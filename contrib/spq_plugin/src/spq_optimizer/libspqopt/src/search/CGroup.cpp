//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 -2011 EMC Corp.
//
//	@filename:
//		CGroup.cpp
//
//	@doc:
//		Implementation of Memo groups; database agnostic
//---------------------------------------------------------------------------

#include "spqopt/search/CGroup.h"

#include "spqos/base.h"
#include "spqos/error/CAutoTrace.h"
#include "spqos/task/CAutoSuspendAbort.h"
#include "spqos/task/CAutoTraceFlag.h"
#include "spqos/task/CWorker.h"

#include "spqopt/base/CDrvdProp.h"
#include "spqopt/base/CDrvdPropCtxtPlan.h"
#include "spqopt/base/CDrvdPropCtxtRelational.h"
#include "spqopt/base/COptimizationContext.h"
#include "spqopt/exception.h"
#include "spqopt/operators/CExpressionHandle.h"
#include "spqopt/operators/CLogicalCTEConsumer.h"
#include "spqopt/operators/CLogicalCTEProducer.h"
#include "spqopt/operators/CLogicalInnerJoin.h"
#include "spqopt/operators/COperator.h"
#include "spqopt/operators/CPhysicalMotionGather.h"
#include "spqopt/operators/CScalarSubquery.h"
#include "spqopt/search/CGroupProxy.h"
#include "spqopt/search/CJobGroup.h"
#include "naucrates/statistics/CStatistics.h"
#include "naucrates/traceflags/traceflags.h"

using namespace spqnaucrates;
using namespace spqopt;

FORCE_GENERATE_DBGSTR(CGroup);

#define SPQOPT_OPTCTXT_HT_BUCKETS 100


//---------------------------------------------------------------------------
//	@function:
//		CGroup::SContextLink::SContextLink
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CGroup::SContextLink::SContextLink(CCostContext *pccParent, ULONG child_index,
								   COptimizationContext *poc)
	: m_pccParent(pccParent), m_ulChildIndex(child_index), m_poc(poc)
{
}


//---------------------------------------------------------------------------
//	@function:
//		CGroup::SContextLink::~SContextLink
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CGroup::SContextLink::~SContextLink()
{
	CRefCount::SafeRelease(m_pccParent);
	CRefCount::SafeRelease(m_poc);
}


//---------------------------------------------------------------------------
//	@function:
//		CGroup::SContextLink::HashValue
//
//	@doc:
//		Hash function
//
//---------------------------------------------------------------------------
ULONG
CGroup::SContextLink::HashValue(const SContextLink *pclink)
{
	ULONG ulHashPcc = 0;
	if (NULL != pclink->m_pccParent)
	{
		ulHashPcc = CCostContext::HashValue(*pclink->m_pccParent);
	}

	ULONG ulHashPoc = 0;
	if (NULL != pclink->m_poc)
	{
		ulHashPoc = COptimizationContext::HashValue(*pclink->m_poc);
	}

	return CombineHashes(pclink->m_ulChildIndex,
						 CombineHashes(ulHashPcc, ulHashPoc));
}


//---------------------------------------------------------------------------
//	@function:
//		CGroup::SContextLink::Equals
//
//	@doc:
//		Equality function
//
//---------------------------------------------------------------------------
BOOL
CGroup::SContextLink::Equals(const SContextLink *pclink1,
							 const SContextLink *pclink2)
{
	BOOL fEqualChildIndexes =
		(pclink1->m_ulChildIndex == pclink2->m_ulChildIndex);
	BOOL fEqual = false;
	if (fEqualChildIndexes)
	{
		if (NULL == pclink1->m_pccParent || NULL == pclink2->m_pccParent)
		{
			fEqual =
				(NULL == pclink1->m_pccParent && NULL == pclink2->m_pccParent);
		}
		else
		{
			fEqual = (*pclink1->m_pccParent == *pclink2->m_pccParent);
		}
	}

	if (fEqual)
	{
		if (NULL == pclink1->m_poc || NULL == pclink2->m_poc)
		{
			return (NULL == pclink1->m_poc && NULL == pclink2->m_poc);
		}

		return COptimizationContext::Equals(*pclink1->m_poc, *pclink2->m_poc);
	}

	return fEqual;
}


//---------------------------------------------------------------------------
//	@function:
//		CGroup::CGroup
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CGroup::CGroup(CMemoryPool *mp, BOOL fScalar)
	: m_mp(mp),
	  m_id(SPQOPT_INVALID_GROUP_ID),
	  m_fScalar(fScalar),
	  m_pdrspqexprJoinKeysOuter(NULL),
	  m_pdrspqexprJoinKeysInner(NULL),
	  m_join_opfamilies(NULL),
	  m_pdp(NULL),
	  m_pstats(NULL),
	  m_pexprScalarRep(NULL),
	  m_pexprScalarRepIsExact(false),
	  m_pccDummy(NULL),
	  m_pgroupDuplicate(NULL),
	  m_plinkmap(NULL),
	  m_pstatsmap(NULL),
	  m_ulGExprs(0),
	  m_pcostmap(NULL),
	  m_ulpOptCtxts(0),
	  m_estate(estUnexplored),
	  m_eolMax(EolLow),
	  m_fHasNewLogicalOperators(false),
	  m_ulCTEProducerId(spqos::ulong_max),
	  m_fCTEConsumer(false)
{
	SPQOS_ASSERT(NULL != mp);

	m_listGExprs.Init(SPQOS_OFFSET(CGroupExpression, m_linkGroup));
	m_listDupGExprs.Init(SPQOS_OFFSET(CGroupExpression, m_linkGroup));

	m_sht.Init(
		mp, SPQOPT_OPTCTXT_HT_BUCKETS, SPQOS_OFFSET(COptimizationContext, m_link),
		0, /*cKeyOffset (0 because we use COptimizationContext class as key)*/
		&(COptimizationContext::m_ocInvalid), COptimizationContext::HashValue,
		COptimizationContext::Equals);
	m_plinkmap = SPQOS_NEW(mp) LinkMap(mp);
	m_pstatsmap = SPQOS_NEW(mp) OptCtxtToIStatisticsMap(mp);
	m_pcostmap = SPQOS_NEW(mp) ReqdPropPlanToCostMap(mp);
}


//---------------------------------------------------------------------------
//	@function:
//		CGroup::~CGroup
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CGroup::~CGroup()
{
	CRefCount::SafeRelease(m_pdrspqexprJoinKeysOuter);
	CRefCount::SafeRelease(m_pdrspqexprJoinKeysInner);
	CRefCount::SafeRelease(m_join_opfamilies);
	CRefCount::SafeRelease(m_pdp);
	CRefCount::SafeRelease(m_pexprScalarRep);
	CRefCount::SafeRelease(m_pccDummy);
	CRefCount::SafeRelease(m_pstats);
	m_plinkmap->Release();
	m_pstatsmap->Release();
	m_pcostmap->Release();

	// cleaning-up group expressions
	CGroupExpression *pgexpr = m_listGExprs.First();
	while (NULL != pgexpr)
	{
		CGroupExpression *pgexprNext = m_listGExprs.Next(pgexpr);
		pgexpr->CleanupContexts();
		pgexpr->Release();

		pgexpr = pgexprNext;
	}

	// cleaning-up duplicate expressions
	pgexpr = m_listDupGExprs.First();
	while (NULL != pgexpr)
	{
		CGroupExpression *pgexprNext = m_listDupGExprs.Next(pgexpr);
		pgexpr->CleanupContexts();
		pgexpr->Release();

		pgexpr = pgexprNext;
	}

	// cleanup optimization contexts
	CleanupContexts();
}


//---------------------------------------------------------------------------
//	@function:
//		CGroup::CleanupContexts
//
//	@doc:
//		 Destroy stored contexts in hash table
//
//---------------------------------------------------------------------------
void
CGroup::CleanupContexts()
{
	// need to suspend cancellation while cleaning up
	{
		CAutoSuspendAbort asa;

		COptimizationContext *poc = NULL;
		ShtIter shtit(m_sht);

		while (NULL != poc || shtit.Advance())
		{
			CRefCount::SafeRelease(poc);

			// iter's accessor scope
			{
				ShtAccIter shtitacc(shtit);
				if (NULL != (poc = shtitacc.Value()))
				{
					shtitacc.Remove(poc);
				}
			}
		}
	}
}


//---------------------------------------------------------------------------
//	@function:
//		CGroup::UpdateBestCost
//
//	@doc:
//		 Update the group expression with best cost under the given
//		 optimization context
//
//---------------------------------------------------------------------------
void
CGroup::UpdateBestCost(COptimizationContext *poc, CCostContext *pcc)
{
	SPQOS_ASSERT(CCostContext::estCosted == pcc->Est());

	COptimizationContext *pocFound = NULL;

	{
		// scope for accessor
		ShtAcc shta(Sht(), *poc);
		pocFound = shta.Find();
	}

	if (NULL == pocFound)
	{
		// it should never happen, but instead of crashing, raise an exception
		SPQOS_RAISE(CException::ExmaInvalid, CException::ExmiInvalid,
				   SPQOS_WSZ_LIT(
					   "Updating cost for non-existing optimization context"));
	}
    if (SPQOS_FTRACE(EopttracePrintOptimizationCost)) {
        PrintUpdateBestCost(poc, pcc);
    }
    

	// update best cost context
	CCostContext *pccBest = pocFound->PccBest();
	if (SPQOPT_INVALID_COST != pcc->Cost() &&
		(NULL == pccBest || pcc->FBetterThan(pccBest)))
	{
		pocFound->SetBest(pcc);
	}
}


//---------------------------------------------------------------------------
//	@function:
//		CGroup::PocLookup
//
//	@doc:
//		Lookup a given context in contexts hash table
//
//---------------------------------------------------------------------------
COptimizationContext *
CGroup::PocLookup(CMemoryPool *mp, CReqdPropPlan *prpp,
				  ULONG ulSearchStageIndex)
{
	prpp->AddRef();
	COptimizationContext *poc = SPQOS_NEW(mp) COptimizationContext(
		mp, this, prpp,
		SPQOS_NEW(mp) CReqdPropRelational(SPQOS_NEW(mp) CColRefSet(
			mp)),  // required relational props is not used when looking up contexts
		SPQOS_NEW(mp) IStatisticsArray(
			mp),  // stats context is not used when looking up contexts
		ulSearchStageIndex);

	COptimizationContext *pocFound = NULL;
	{
		ShtAcc shta(Sht(), *poc);
		pocFound = shta.Find();
	}
	poc->Release();

	return pocFound;
}



//---------------------------------------------------------------------------
//	@function:
//		CGroup::PocLookupBest
//
//	@doc:
//		Lookup the best context across all stages for the given required
//		properties
//
//---------------------------------------------------------------------------
COptimizationContext *
CGroup::PocLookupBest(CMemoryPool *mp, ULONG ulSearchStages,
					  CReqdPropPlan *prpp)
{
	COptimizationContext *pocBest = NULL;
	CCostContext *pccBest = NULL;
	for (ULONG ul = 0; ul < ulSearchStages; ul++)
	{
		COptimizationContext *pocCurrent = PocLookup(mp, prpp, ul);
		if (NULL == pocCurrent)
		{
			continue;
		}

		CCostContext *pccCurrent = pocCurrent->PccBest();
		if (NULL == pccBest ||
			(NULL != pccCurrent && pccCurrent->FBetterThan(pccBest)))
		{
			pocBest = pocCurrent;
			pccBest = pccCurrent;
		}
	}

	return pocBest;
}


//---------------------------------------------------------------------------
//	@function:
//		CGroup::Ppoc
//
//	@doc:
//		Lookup a context by id
//
//---------------------------------------------------------------------------
COptimizationContext *
CGroup::Ppoc(ULONG id) const
{
	COptimizationContext *poc = NULL;
	ShtIter shtit(const_cast<CGroup *>(this)->m_sht);
	while (shtit.Advance())
	{
		{
			ShtAccIter shtitacc(shtit);
			poc = shtitacc.Value();

			if (poc->Id() == id)
			{
				return poc;
			}
		}
	}
	return NULL;
}


//---------------------------------------------------------------------------
//	@function:
//		CGroup::PocInsert
//
//	@doc:
//		Insert a given context into contexts hash table only if a matching
//		context does not already exist;
//		return either the inserted or the existing matching context
//
//---------------------------------------------------------------------------
COptimizationContext *
CGroup::PocInsert(COptimizationContext *poc)
{
	ShtAcc shta(Sht(), *poc);

	COptimizationContext *pocFound = shta.Find();
	if (NULL == pocFound)
	{
		poc->SetId((ULONG) UlpIncOptCtxts());
		shta.Insert(poc);
        if (SPQOS_FTRACE(EopttracePrintOptimizationCost)) {
            PrintNewOptContext(poc);
        }
		return poc;
	}

	return pocFound;
}


//---------------------------------------------------------------------------
//	@function:
//		CGroup::PgexprBest
//
//	@doc:
//		Lookup best group expression under optimization context
//
//---------------------------------------------------------------------------
CGroupExpression *
CGroup::PgexprBest(COptimizationContext *poc)
{
	ShtAcc shta(Sht(), *poc);
	COptimizationContext *pocFound = shta.Find();
	if (NULL != pocFound)
	{
		return pocFound->PgexprBest();
	}

	return NULL;
}


//---------------------------------------------------------------------------
//	@function:
//		CGroup::SetId
//
//	@doc:
//		Set group id;
//		separated from constructor to avoid synchronization issues
//
//---------------------------------------------------------------------------
void
CGroup::SetId(ULONG id)
{
	SPQOS_ASSERT(SPQOPT_INVALID_GROUP_ID == m_id &&
				"Overwriting previously assigned group id");

	m_id = id;
}


//---------------------------------------------------------------------------
//	@function:
//		CGroup::InitProperties
//
//	@doc:
//		Initialize group's properties
//
//---------------------------------------------------------------------------
void
CGroup::InitProperties(CDrvdProp *pdp)
{
	SPQOS_ASSERT(NULL == m_pdp);
	SPQOS_ASSERT(NULL != pdp);
	SPQOS_ASSERT_IMP(FScalar(), CDrvdProp::EptScalar == pdp->Ept());
	SPQOS_ASSERT_IMP(!FScalar(), CDrvdProp::EptRelational == pdp->Ept());

	m_pdp = pdp;
}


//---------------------------------------------------------------------------
//	@function:
//		CGroup::InitStats
//
//	@doc:
//		Initialize group's stats
//
//---------------------------------------------------------------------------
void
CGroup::InitStats(IStatistics *stats)
{
	SPQOS_ASSERT(NULL == m_pstats);
	SPQOS_ASSERT(NULL != stats);

	m_pstats = stats;
}


//---------------------------------------------------------------------------
//	@function:
//		CGroup::SetState
//
//	@doc:
//		Set group state;
//
//---------------------------------------------------------------------------
void
CGroup::SetState(EState estNewState)
{
	SPQOS_ASSERT(estNewState == (EState)(m_estate + 1));

	m_estate = estNewState;
}


void
CGroup::SetJoinKeys(CExpressionArray *pdrspqexprOuter,
					CExpressionArray *pdrspqexprInner,
					IMdIdArray *join_opfamilies)
{
	SPQOS_ASSERT(m_fScalar);
	SPQOS_ASSERT(NULL != pdrspqexprOuter);
	SPQOS_ASSERT(NULL != pdrspqexprInner);

	if (NULL != m_pdrspqexprJoinKeysOuter)
	{
		SPQOS_ASSERT(NULL != m_pdrspqexprJoinKeysInner);

		// hash join keys have been already set, exit here
		return;
	}

	pdrspqexprOuter->AddRef();
	m_pdrspqexprJoinKeysOuter = pdrspqexprOuter;

	pdrspqexprInner->AddRef();
	m_pdrspqexprJoinKeysInner = pdrspqexprInner;

	if (SPQOS_FTRACE(EopttraceConsiderOpfamiliesForDistribution))
	{
		SPQOS_ASSERT(join_opfamilies != NULL);
		join_opfamilies->AddRef();
	}
	else
	{
		SPQOS_ASSERT(NULL == join_opfamilies);
	}
	m_join_opfamilies = join_opfamilies;
}


//---------------------------------------------------------------------------
//	@function:
//		CGroup::HashValue
//
//	@doc:
//		Hash function for group identification
//
//---------------------------------------------------------------------------
ULONG
CGroup::HashValue() const
{
	ULONG id = m_id;
	if (FDuplicateGroup() && 0 == m_ulGExprs)
	{
		// group has been merged into another group
		id = PgroupDuplicate()->Id();
	}

	return spqos::HashValue<ULONG>(&id);
}


//---------------------------------------------------------------------------
//	@function:
//		CGroup::Insert
//
//	@doc:
//		Insert group expression
//
//---------------------------------------------------------------------------
void
CGroup::Insert(CGroupExpression *pgexpr)
{
	m_listGExprs.Append(pgexpr);
	COperator *pop = pgexpr->Pop();
	if (pop->FLogical())
	{
		m_fHasNewLogicalOperators = true;
		if (COperator::EopLogicalCTEConsumer == pop->Eopid())
		{
			m_fCTEConsumer = true;
		}

		if (COperator::EopLogicalCTEProducer == pop->Eopid())
		{
			SPQOS_ASSERT(spqos::ulong_max == m_ulCTEProducerId);
			m_ulCTEProducerId = CLogicalCTEProducer::PopConvert(pop)->UlCTEId();
			;
		}
	}

	if (pgexpr->Eol() > m_eolMax)
	{
		m_eolMax = pgexpr->Eol();
	}
}


//---------------------------------------------------------------------------
//	@function:
//		CGroup::MoveDuplicateGExpr
//
//	@doc:
//		Move duplicate group expression to duplicates list
//
//---------------------------------------------------------------------------
void
CGroup::MoveDuplicateGExpr(CGroupExpression *pgexpr)
{
	m_listGExprs.Remove(pgexpr);
	m_ulGExprs--;

	m_listDupGExprs.Append(pgexpr);
}


//---------------------------------------------------------------------------
//	@function:
//		CGroup::PgexprAnyCTEConsumer
//
//	@doc:
//		Retrieve the group expression containing a CTE Consumer operator
//
//---------------------------------------------------------------------------
CGroupExpression *
CGroup::PgexprAnyCTEConsumer()
{
	BOOL fFoundCTEConsumer = false;
	// get first logical group expression
	CGroupExpression *pgexprCurrent = NULL;
	{
		CGroupProxy spq(this);
		pgexprCurrent = spq.PgexprNextLogical(NULL /*pgexpr*/);
		fFoundCTEConsumer =
			(COperator::EopLogicalCTEConsumer == pgexprCurrent->Pop()->Eopid());
	}

	while (NULL != pgexprCurrent && !fFoundCTEConsumer)
	{
		SPQOS_CHECK_ABORT;
		// move to next logical group expression
		{
			CGroupProxy spq(this);
			pgexprCurrent = spq.PgexprNextLogical(pgexprCurrent);
		}

		if (NULL != pgexprCurrent)
		{
			COperator *popCurrent = pgexprCurrent->Pop();
			fFoundCTEConsumer =
				(COperator::EopLogicalCTEConsumer == popCurrent->Eopid());
		}
	}

	if (fFoundCTEConsumer)
	{
		return pgexprCurrent;
	}

	return NULL;
}

//---------------------------------------------------------------------------
//	@function:
//		CGroup::PgexprFirst
//
//	@doc:
//		Retrieve first expression in group
//
//---------------------------------------------------------------------------
CGroupExpression *
CGroup::PgexprFirst()
{
	return m_listGExprs.First();
}


//---------------------------------------------------------------------------
//	@function:
//		CGroup::PgexprNext
//
//	@doc:
//		Retrieve next expression in group
//
//---------------------------------------------------------------------------
CGroupExpression *
CGroup::PgexprNext(CGroupExpression *pgexpr)
{
	return m_listGExprs.Next(pgexpr);
}


//---------------------------------------------------------------------------
//	@function:
//		CGroup::FMatchGroups
//
//	@doc:
//		Determine whether two arrays of groups are equivalent
//
//---------------------------------------------------------------------------
BOOL
CGroup::FMatchGroups(CGroupArray *pdrspqgroupFst, CGroupArray *pdrspqgroupSnd)
{
	ULONG arity = pdrspqgroupFst->Size();
	SPQOS_ASSERT(pdrspqgroupSnd->Size() == arity);

	for (ULONG i = 0; i < arity; i++)
	{
		CGroup *pgroupFst = (*pdrspqgroupFst)[i];
		CGroup *pgroupSnd = (*pdrspqgroupSnd)[i];
		if (pgroupFst != pgroupSnd && !FDuplicateGroups(pgroupFst, pgroupSnd))
		{
			return false;
		}
	}

	return true;
}

//---------------------------------------------------------------------------
//	@function:
//		CGroup::FMatchNonScalarGroups
//
//	@doc:
//		 Matching of pairs of arrays of groups while skipping scalar groups
//
//---------------------------------------------------------------------------
BOOL
CGroup::FMatchNonScalarGroups(CGroupArray *pdrspqgroupFst,
							  CGroupArray *pdrspqgroupSnd)
{
	SPQOS_ASSERT(NULL != pdrspqgroupFst);
	SPQOS_ASSERT(NULL != pdrspqgroupSnd);

	if (pdrspqgroupFst->Size() != pdrspqgroupSnd->Size())
	{
		return false;
	}

	ULONG arity = pdrspqgroupFst->Size();
	SPQOS_ASSERT(pdrspqgroupSnd->Size() == arity);

	for (ULONG i = 0; i < arity; i++)
	{
		CGroup *pgroupFst = (*pdrspqgroupFst)[i];
		CGroup *pgroupSnd = (*pdrspqgroupSnd)[i];
		if (pgroupFst->FScalar())
		{
			// skip scalar groups
			continue;
		}

		if (pgroupFst != pgroupSnd && !FDuplicateGroups(pgroupFst, pgroupSnd))
		{
			return false;
		}
	}

	return true;
}


//---------------------------------------------------------------------------
//	@function:
//		CGroup::FDuplicateGroups
//
//	@doc:
//		Determine whether two groups are equivalent
//
//---------------------------------------------------------------------------
BOOL
CGroup::FDuplicateGroups(CGroup *pgroupFst, CGroup *pgroupSnd)
{
	SPQOS_ASSERT(NULL != pgroupFst);
	SPQOS_ASSERT(NULL != pgroupSnd);

	CGroup *pgroupFstDup = pgroupFst->PgroupDuplicate();
	CGroup *pgroupSndDup = pgroupSnd->PgroupDuplicate();

	return (pgroupFst == pgroupSnd) ||	// pointer equality
		   (pgroupFst ==
			pgroupSndDup) ||  // first group is duplicate of second group
		   (pgroupSnd ==
			pgroupFstDup) ||  // second group is duplicate of first group
		   // both groups have the same duplicate group
		   (NULL != pgroupFstDup && NULL != pgroupSndDup &&
			pgroupFstDup == pgroupSndDup);
}


//---------------------------------------------------------------------------
//	@function:
//		CGroup::FInitStats
//
//	@doc:
//		Attempt initializing stats with the given stat object
//
//---------------------------------------------------------------------------
BOOL
CGroup::FInitStats(IStatistics *stats)
{
	SPQOS_ASSERT(NULL != stats);

	CGroupProxy spq(this);
	if (NULL == Pstats())
	{
		spq.InitStats(stats);
		return true;
	}

	// mark group as having no new logical operators to disallow
	// resetting stats until a new logical operator is inserted
	ResetHasNewLogicalOperators();

	return false;
}

//---------------------------------------------------------------------------
//	@function:
//		CGroup::AppendStats
//
//	@doc:
//		Append given stats to group stats
//
//---------------------------------------------------------------------------
void
CGroup::AppendStats(CMemoryPool *mp, IStatistics *stats)
{
	SPQOS_ASSERT(NULL != stats);
	SPQOS_ASSERT(NULL != Pstats());

	IStatistics *stats_copy = Pstats()->CopyStats(mp);
	stats_copy->AppendStats(mp, stats);

	IStatistics *current_stats = NULL;
	{
		CGroupProxy spq(this);
		current_stats = m_pstats;
		m_pstats = NULL;
		spq.InitStats(stats_copy);
	}

	current_stats->Release();
	current_stats = NULL;
}


//---------------------------------------------------------------------------
//	@function:
//		CGroup::AddDuplicateGrp
//
//	@doc:
//		Add duplicate group
//
//---------------------------------------------------------------------------
void
CGroup::AddDuplicateGrp(CGroup *pgroup)
{
	SPQOS_ASSERT(NULL != pgroup);

	// add link following monotonic ordering of group IDs
	CGroup *pgroupSrc = this;
	CGroup *pgroupDest = pgroup;
	if (pgroupSrc->Id() > pgroupDest->Id())
	{
		std::swap(pgroupSrc, pgroupDest);
	}

	// keep looping until we add link
	while (pgroupSrc->m_pgroupDuplicate != pgroupDest)
	{
		if (NULL == pgroupSrc->m_pgroupDuplicate)
		{
			pgroupSrc->m_pgroupDuplicate = pgroupDest;
		}
		else
		{
			pgroupSrc = pgroupSrc->m_pgroupDuplicate;
			if (pgroupSrc->Id() > pgroupDest->Id())
			{
				std::swap(pgroupSrc, pgroupDest);
			}
		}
	}
}


//---------------------------------------------------------------------------
//	@function:
//		CGroup::ResolveDuplicateMaster
//
//	@doc:
//		Resolve master duplicate group
//
//---------------------------------------------------------------------------
void
CGroup::ResolveDuplicateMaster()
{
	if (!FDuplicateGroup())
	{
		return;
	}
	CGroup *pgroupTarget = m_pgroupDuplicate;
	while (NULL != pgroupTarget->m_pgroupDuplicate)
	{
		SPQOS_ASSERT(pgroupTarget->Id() < pgroupTarget->m_pgroupDuplicate->Id());
		pgroupTarget = pgroupTarget->m_pgroupDuplicate;
	}

	// update reference to target group
	m_pgroupDuplicate = pgroupTarget;
}


//---------------------------------------------------------------------------
//	@function:
//		CGroup::MergeGroup
//
//	@doc:
//		Merge group with its duplicate - not thread-safe
//
//---------------------------------------------------------------------------
void
CGroup::MergeGroup()
{
	if (!FDuplicateGroup())
	{
		return;
	}
	SPQOS_ASSERT(FExplored());
	SPQOS_ASSERT(!FImplemented());

	// resolve target group
	ResolveDuplicateMaster();
	CGroup *pgroupTarget = m_pgroupDuplicate;

	// move group expressions from this group to target
	while (!m_listGExprs.IsEmpty())
	{
		CGroupExpression *pgexpr = m_listGExprs.RemoveHead();
		m_ulGExprs--;

		pgexpr->Reset(pgroupTarget, pgroupTarget->m_ulGExprs++);
		pgroupTarget->Insert(pgexpr);

		SPQOS_CHECK_ABORT;
	}

	SPQOS_ASSERT(0 == m_ulGExprs);
}


//---------------------------------------------------------------------------
//	@function:
//		CGroup::CreateScalarExpression
//
//	@doc:
//		Materialize a scalar expression for stat derivation only if
//		this is a scalar group
//
//---------------------------------------------------------------------------
void
CGroup::CreateScalarExpression()
{
	SPQOS_ASSERT(FScalar());
	SPQOS_ASSERT(NULL == m_pexprScalarRep);

	CGroupExpression *pgexprFirst = NULL;
	{
		CGroupProxy spq(this);
		pgexprFirst = spq.PgexprFirst();
	}
	SPQOS_ASSERT(NULL != pgexprFirst);
	COperator *pop = pgexprFirst->Pop();

	if (CUtils::FSubquery(pop))
	{
		if (COperator::EopScalarSubquery == pop->Eopid())
		{
			CScalarSubquery *subquery_pop = CScalarSubquery::PopConvert(pop);
			const CColRef *subquery_colref = subquery_pop->Pcr();

			// replace the scalar subquery with a NULL value of the same type
			m_pexprScalarRep = CUtils::PexprScalarConstNull(
				m_mp, subquery_colref->RetrieveType(),
				subquery_colref->TypeModifier());
		}
		else
		{
			// for subqueries that are predicates, make a "true" constant
			m_pexprScalarRep = CUtils::PexprScalarConstBool(
				m_mp, true /* make a "true" constant*/);
		}

		m_pexprScalarRepIsExact = false;
		return;
	}

	m_pexprScalarRepIsExact = true;

	CExpressionArray *pdrspqexpr = SPQOS_NEW(m_mp) CExpressionArray(m_mp);
	const ULONG arity = pgexprFirst->Arity();
	for (ULONG ul = 0; ul < arity; ul++)
	{
		CGroup *pgroupChild = (*pgexprFirst)[ul];
		SPQOS_ASSERT(pgroupChild->FScalar());

		CExpression *pexprChild = pgroupChild->PexprScalarRep();
		pexprChild->AddRef();
		pdrspqexpr->Append(pexprChild);

		if (!pgroupChild->FScalarRepIsExact())
		{
			m_pexprScalarRepIsExact = false;
		}
	}

	pop->AddRef();
	m_pexprScalarRep = SPQOS_NEW(m_mp) CExpression(m_mp, pop, pdrspqexpr);
}


//---------------------------------------------------------------------------
//	@function:
//		CGroup::CreateDummyCostContext
//
//	@doc:
//		Create a dummy cost context attached to the first group expression,
//		used for plan enumeration for scalar groups
//
//
//---------------------------------------------------------------------------
void
CGroup::CreateDummyCostContext()
{
	SPQOS_ASSERT(FScalar());
	SPQOS_ASSERT(NULL == m_pccDummy);

	CGroupExpression *pgexprFirst = NULL;
	{
		CGroupProxy spq(this);
		pgexprFirst = spq.PgexprFirst();
	}
	SPQOS_ASSERT(NULL != pgexprFirst);

	COptimizationContext *poc = SPQOS_NEW(m_mp) COptimizationContext(
		m_mp, this, CReqdPropPlan::PrppEmpty(m_mp),
		SPQOS_NEW(m_mp) CReqdPropRelational(SPQOS_NEW(m_mp) CColRefSet(m_mp)),
		SPQOS_NEW(m_mp) IStatisticsArray(m_mp),
		0  // ulSearchStageIndex
	);

	pgexprFirst->AddRef();
	m_pccDummy =
		SPQOS_NEW(m_mp) CCostContext(m_mp, poc, 0 /*ulOptReq*/, pgexprFirst);
	m_pccDummy->SetState(CCostContext::estCosting);
	m_pccDummy->SetCost(CCost(0.0));
	m_pccDummy->SetState(CCostContext::estCosted);

#ifdef SPQOS_DEBUG
	CGroupExpression *pgexprNext = NULL;
	{
		CGroupProxy spq(this);
		pgexprNext = spq.PgexprNext(pgexprFirst);
	}
	SPQOS_ASSERT(NULL == pgexprNext &&
				"scalar group can only have one group expression");
#endif	// SPQOS_DEBUG
}


//---------------------------------------------------------------------------
//	@function:
//		CGroup::RecursiveBuildTreeMap
//
//	@doc:
//		Find all cost contexts of current group expression that carry valid
//		implementation of the given optimization context,
//		for all such cost contexts, introduce a link from parent cost context
//		to child cost context and then process child groups recursively
//
//
//---------------------------------------------------------------------------
void
CGroup::RecursiveBuildTreeMap(
	CMemoryPool *mp, COptimizationContext *poc, CCostContext *pccParent,
	CGroupExpression *pgexprCurrent, ULONG child_index,
	CTreeMap<CCostContext, CExpression, CDrvdPropCtxtPlan,
			 CCostContext::HashValue, CCostContext::Equals> *ptmap)
{
	SPQOS_ASSERT(pgexprCurrent->Pop()->FPhysical());
	SPQOS_ASSERT(NULL != ptmap);
	SPQOS_ASSERT_IMP(NULL != pccParent,
					child_index < pccParent->Pgexpr()->Arity());

	CCostContextArray *pdrspqcc = pgexprCurrent->PdrspqccLookupAll(mp, poc);
	const ULONG ulCCSize = pdrspqcc->Size();

	if (0 == ulCCSize)
	{
		// current group expression has no valid implementations of optimization context
		pdrspqcc->Release();
		return;
	}

	// iterate over all valid implementations of given optimization context
	for (ULONG ulCC = 0; ulCC < ulCCSize; ulCC++)
	{
		SPQOS_CHECK_ABORT;

		CCostContext *pccCurrent = (*pdrspqcc)[ulCC];
		if (NULL != pccParent)
		{
			// link parent cost context to child cost context
			ptmap->Insert(pccParent, child_index, pccCurrent);
		}

		COptimizationContextArray *pdrspqoc = pccCurrent->Pdrspqoc();
		if (NULL != pdrspqoc)
		{
			// process children recursively
			const ULONG arity = pgexprCurrent->Arity();
			for (ULONG ul = 0; ul < arity; ul++)
			{
				SPQOS_CHECK_ABORT;

				CGroup *pgroupChild = (*pgexprCurrent)[ul];
				COptimizationContext *pocChild = NULL;
				if (!pgroupChild->FScalar())
				{
					pocChild = (*pdrspqoc)[ul];
					SPQOS_ASSERT(NULL != pocChild);
				}
				pgroupChild->BuildTreeMap(mp, pocChild, pccCurrent, ul, ptmap);
			}
		}
	}

	pdrspqcc->Release();
}


//---------------------------------------------------------------------------
//	@function:
//		CGroup::BuildTreeMap
//
//	@doc:
//		Given a parent cost context and an optimization context,
//		link parent cost context to all cost contexts in current group
//		that carry valid implementation of the given optimization context
//
//
//---------------------------------------------------------------------------
void
CGroup::BuildTreeMap(
	CMemoryPool *mp,
	COptimizationContext *poc,	// NULL if we are in a Scalar group
	CCostContext *pccParent,	// NULL if we are in the Root group
	ULONG
		child_index,  // index used for treating group as child of parent context
	CTreeMap<CCostContext, CExpression, CDrvdPropCtxtPlan,
			 CCostContext::HashValue, CCostContext::Equals>
		*ptmap	// map structure
)
{
	SPQOS_CHECK_STACK_SIZE;
	SPQOS_ASSERT(NULL != ptmap);
	SPQOS_ASSERT_IMP(NULL == poc, FScalar());

#ifdef SPQOS_DEBUG
	CGroupExpression *pgexprParent = NULL;
#endif	// SPQOS_DEBUG
	if (NULL != pccParent)
	{
		pccParent->AddRef();
#ifdef SPQOS_DEBUG
		pgexprParent = pccParent->Pgexpr();
#endif	// SPQOS_DEBUG
	}
	if (NULL != poc)
	{
		poc->AddRef();
	}

	// check if link has been processed before,
	// this is crucial to eliminate unnecessary recursive calls
	SContextLink *pclink =
		SPQOS_NEW(m_mp) SContextLink(pccParent, child_index, poc);
	if (m_plinkmap->Find(pclink))
	{
		// link is already processed
		SPQOS_DELETE(pclink);
		return;
	}

	// start with first non-logical group expression
	CGroupExpression *pgexprCurrent = NULL;
	{
		CGroupProxy spq(this);
		pgexprCurrent = spq.PgexprSkipLogical(NULL /*pgexpr*/);
	}

	while (NULL != pgexprCurrent)
	{
		COperator *pop = pgexprCurrent->Pop();
		if (pop->FPhysical())
		{
			// create links recursively
			RecursiveBuildTreeMap(mp, poc, pccParent, pgexprCurrent,
								  child_index, ptmap);
		}
		else
		{
			SPQOS_ASSERT(pop->FScalar());
			SPQOS_ASSERT(NULL == poc);
			SPQOS_ASSERT(child_index < pgexprParent->Arity());

			// this is a scalar group, link parent cost context to group's dummy context
			ptmap->Insert(pccParent, child_index, PccDummy());

			// recursively link group's dummy context to child contexts
			const ULONG arity = pgexprCurrent->Arity();
			for (ULONG ul = 0; ul < arity; ul++)
			{
				CGroup *pgroupChild = (*pgexprCurrent)[ul];
				SPQOS_ASSERT(pgroupChild->FScalar());

				pgroupChild->BuildTreeMap(mp, NULL /*poc*/, PccDummy(), ul,
										  ptmap);
			}
		}

		// move to next non-logical group expression
		{
			CGroupProxy spq(this);
			pgexprCurrent = spq.PgexprSkipLogical(pgexprCurrent);
		}
		SPQOS_ASSERT_IMP(
			FScalar(), NULL == pgexprCurrent &&
						   "a scalar group can only have one group expression");

		SPQOS_CHECK_ABORT;
	}

	// remember processed links to avoid re-processing them later
#ifdef SPQOS_DEBUG
	BOOL fInserted =
#endif	// SPQOS_DEBUG
		m_plinkmap->Insert(pclink, SPQOS_NEW(m_mp) BOOL(true));
	SPQOS_ASSERT(fInserted);
}


//---------------------------------------------------------------------------
//	@function:
//		CGroup::FStatsDerivable
//
//	@doc:
//		Returns true if stats can be derived on this group
//
//---------------------------------------------------------------------------
BOOL
CGroup::FStatsDerivable(CMemoryPool *mp)
{
	SPQOS_CHECK_STACK_SIZE;

	if (NULL != m_pstats)
	{
		return true;
	}

	CGroupExpression *pgexprBest = NULL;
	CLogical::EStatPromise espBest = CLogical::EspNone;

	CGroupExpression *pgexprCurrent = NULL;
	// get first logical group expression
	{
		CGroupProxy spq(this);
		pgexprCurrent = spq.PgexprNextLogical(NULL /*pgexpr*/);
	}

	while (NULL != pgexprCurrent)
	{
		CExpressionHandle exprhdl(mp);
		exprhdl.Attach(pgexprCurrent);
		CDrvdPropCtxtRelational *pdpctxtrel =
			SPQOS_NEW(mp) CDrvdPropCtxtRelational(mp);
		exprhdl.DeriveProps(pdpctxtrel);
		pdpctxtrel->Release();

		CLogical *popLogical = CLogical::PopConvert(pgexprCurrent->Pop());
		CLogical::EStatPromise esp = popLogical->Esp(exprhdl);

		if (esp > espBest)
		{
			pgexprBest = pgexprCurrent;
			espBest = esp;
		}

		// move to next logical group expression
		{
			CGroupProxy spq(this);
			pgexprCurrent = spq.PgexprNextLogical(pgexprCurrent);
		}

		SPQOS_CHECK_ABORT;
	}

	if (NULL == pgexprBest)
	{
		return false;
	}

	BOOL fStatsDerivable = true;
	const ULONG arity = pgexprBest->Arity();
	for (ULONG ul = 0; fStatsDerivable && ul < arity; ul++)
	{
		CGroup *pgroupChild = (*pgexprBest)[ul];
		fStatsDerivable =
			(pgroupChild->FScalar() || pgroupChild->FStatsDerivable(mp));
	}

	return fStatsDerivable;
}


//---------------------------------------------------------------------------
//	@function:
//		CGroup::FBetterPromise
//
//	@doc:
//		Return true if first promise is better than second promise
//
//---------------------------------------------------------------------------
BOOL
CGroup::FBetterPromise(CMemoryPool *mp, CLogical::EStatPromise espFst,
					   CGroupExpression *pgexprFst,
					   CLogical::EStatPromise espSnd,
					   CGroupExpression *pgexprSnd) const
{
	// if there is a tie and both group expressions are inner join, we prioritize
	// the inner join having less predicates
	return espFst > espSnd ||
		   (espFst == espSnd &&
			CLogicalInnerJoin::FFewerConj(mp, pgexprFst, pgexprSnd));
}


//---------------------------------------------------------------------------
//	@function:
//		CGroup::EspDerive
//
//	@doc:
//		Derive statistics recursively on group expression
//
//---------------------------------------------------------------------------
CLogical::EStatPromise
CGroup::EspDerive(CMemoryPool *pmpLocal, CMemoryPool *pmpGlobal,
				  CGroupExpression *pgexpr, CReqdPropRelational *prprel,
				  IStatisticsArray *stats_ctxt, BOOL fDeriveChildStats)
{
	SPQOS_ASSERT(pgexpr->Pop()->FLogical());

	CExpressionHandle exprhdl(pmpGlobal);
	exprhdl.Attach(pgexpr);
	CDrvdPropCtxtRelational *pdpctxtrel =
		SPQOS_NEW(pmpGlobal) CDrvdPropCtxtRelational(pmpGlobal);
	exprhdl.DeriveProps(pdpctxtrel);
	pdpctxtrel->Release();

	// compute stat promise for gexpr
	CLogical *popLogical = CLogical::PopConvert(pgexpr->Pop());
	CLogical::EStatPromise esp = popLogical->Esp(exprhdl);

	// override promise if outer child references columns of inner children
	if (2 < exprhdl.Arity() && !exprhdl.FScalarChild(0 /*child_index*/) &&
		exprhdl.HasOuterRefs(0 /*child_index*/) && !exprhdl.HasOuterRefs())
	{
		// stat derivation always starts by outer child,
		// any outer references in outer child cannot be resolved for stats derivation purposes
		esp = CLogical::EspLow;
	}

	if (fDeriveChildStats && esp > CLogical::EspNone)
	{
		// we only aim here at triggering stat derivation on child groups recursively,
		// there is no need to compute stats for group expression's root operator
		IStatistics *stats = pgexpr->PstatsRecursiveDerive(
			pmpLocal, pmpGlobal, prprel, stats_ctxt,
			false /*fComputeRootStats*/);
		CRefCount::SafeRelease(stats);
	}

	return esp;
}


//---------------------------------------------------------------------------
//	@function:
//		CGroup::PstatsRecursiveDerive
//
//	@doc:
//		Derive statistics recursively on group
//
//---------------------------------------------------------------------------
IStatistics *
CGroup::PstatsRecursiveDerive(CMemoryPool *pmpLocal, CMemoryPool *pmpGlobal,
							  CReqdPropRelational *prprel,
							  IStatisticsArray *stats_ctxt)
{
	SPQOS_CHECK_STACK_SIZE;

	SPQOS_ASSERT(!FImplemented());
	SPQOS_ASSERT(NULL != stats_ctxt);
	SPQOS_CHECK_ABORT;

	// create empty stats if a scalar group
	if (FScalar())
	{
		return PstatsInitEmpty(pmpGlobal);
	}

	IStatistics *stats = NULL;
	// if a duplicate group is found, the stats is derived of the duplicate
	// group and returned, no stats will be derived of the group itself
	if (FDuplicateGroup())
	{
		// call stat derivation on the duplicate group
		stats = PgroupDuplicate()->PstatsRecursiveDerive(pmpLocal, pmpGlobal,
														 prprel, stats_ctxt);
	}
	SPQOS_ASSERT(0 < m_ulGExprs);

	prprel->AddRef();
	CReqdPropRelational *prprelInput = prprel;

	if (NULL == stats)
	{
		stats = Pstats();
	}
	// if group has derived stats, check if requirements are covered
	// by what's already derived

	if (NULL != stats)
	{
		prprelInput->Release();
		CReqdPropRelational *prprelExisting =
			stats->GetReqdRelationalProps(pmpGlobal);
		prprelInput = prprel->PrprelDifference(pmpGlobal, prprelExisting);
		prprelExisting->Release();
		if (prprelInput->IsEmpty())
		{
			// required stat columns are already covered by existing stats
			prprelInput->Release();
			return stats;
		}
	}

	// required stat columns are not covered by existing stats, we need to
	// derive the missing ones

	// find the best group expression to derive stats on
	CGroupExpression *pgexprBest =
		PgexprBestPromise(pmpLocal, pmpGlobal, prprelInput, stats_ctxt);

	if (NULL == pgexprBest)
	{
		SPQOS_RAISE(
			spqopt::ExmaSPQOPT, spqopt::ExmiNoPlanFound,
			SPQOS_WSZ_LIT(
				"Could not choose a group expression for statistics derivation"));
	}

	// derive stats on group expression and copy them to group
	stats = pgexprBest->PstatsRecursiveDerive(pmpLocal, pmpGlobal, prprelInput,
											  stats_ctxt);
	if (!FInitStats(stats))
	{
		// a group stat object already exists, we append derived stats to that object
		AppendStats(pmpGlobal, stats);
		stats->Release();
	}
	SPQOS_ASSERT(NULL != Pstats());

	prprelInput->Release();

	return Pstats();
}

//---------------------------------------------------------------------------
//	@function:
//		CGroup::PgexprBestPromise
//
//	@doc:
//		Find group expression with best stats promise and the
//		same children as given expression
//
//---------------------------------------------------------------------------
CGroupExpression *
CGroup::PgexprBestPromise(CMemoryPool *mp, CGroupExpression *pgexprToMatch)
{
	SPQOS_ASSERT(NULL != pgexprToMatch);

	CReqdPropRelational *prprel =
		SPQOS_NEW(mp) CReqdPropRelational(SPQOS_NEW(mp) CColRefSet(mp));
	IStatisticsArray *stats_ctxt = SPQOS_NEW(mp) IStatisticsArray(mp);

	CLogical::EStatPromise espBest = CLogical::EspNone;
	CGroupExpression *pgexprCurrent = NULL;
	CGroupExpression *pgexprBest = NULL;
	// get first logical group expression
	{
		CGroupProxy spq(this);
		pgexprCurrent = spq.PgexprNextLogical(NULL /*pgexpr*/);
	}

	while (NULL != pgexprCurrent)
	{
		CLogical::EStatPromise espCurrent =
			EspDerive(mp, mp, pgexprCurrent, prprel, stats_ctxt,
					  false /*fDeriveChildStats*/);

		if (pgexprCurrent->FMatchNonScalarChildren(pgexprToMatch) &&
			FBetterPromise(mp, espCurrent, pgexprCurrent, espBest, pgexprBest))
		{
			pgexprBest = pgexprCurrent;
			espBest = espCurrent;
		}

		// move to next logical group expression
		{
			CGroupProxy spq(this);
			pgexprCurrent = spq.PgexprNextLogical(pgexprCurrent);
		}

		SPQOS_CHECK_ABORT;
	}

	prprel->Release();
	stats_ctxt->Release();

	return pgexprBest;
}


//---------------------------------------------------------------------------
//	@function:
//		CGroup::PgexprBestPromise
//
//	@doc:
//		Find the group expression having the best stats promise for this group
//
//---------------------------------------------------------------------------
CGroupExpression *
CGroup::PgexprBestPromise(CMemoryPool *pmpLocal, CMemoryPool *pmpGlobal,
						  CReqdPropRelational *prprelInput,
						  IStatisticsArray *stats_ctxt)
{
	CGroupExpression *pgexprBest = NULL;
	CLogical::EStatPromise espBest = CLogical::EspNone;
	CGroupExpression *pgexprCurrent = NULL;
	// get first logical group expression
	{
		CGroupProxy spq(this);
		pgexprCurrent = spq.PgexprNextLogical(NULL /*pgexpr*/);
	}

	while (NULL != pgexprCurrent)
	{
		if (!pgexprCurrent->ContainsCircularDependencies())
		{
			CLogical::EStatPromise espCurrent =
				EspDerive(pmpLocal, pmpGlobal, pgexprCurrent, prprelInput,
						  stats_ctxt, true /*fDeriveChildStats*/);

			if (FBetterPromise(pmpLocal, espCurrent, pgexprCurrent, espBest,
							   pgexprBest))
			{
				pgexprBest = pgexprCurrent;
				espBest = espCurrent;
			}
		}

		// move to next logical group expression
		{
			CGroupProxy spq(this);
			pgexprCurrent = spq.PgexprNextLogical(pgexprCurrent);
		}

		SPQOS_CHECK_ABORT;
	}

	return pgexprBest;
}

//---------------------------------------------------------------------------
//	@function:
//		CGroup::PstatsInitEmpty
//
//	@doc:
//		Initialize and return empty stats for this group
//
//---------------------------------------------------------------------------
IStatistics *
CGroup::PstatsInitEmpty(CMemoryPool *pmpGlobal)
{
	CStatistics *stats = CStatistics::MakeEmptyStats(pmpGlobal);
	if (!FInitStats(stats))
	{
		stats->Release();
	}

	return Pstats();
}


//---------------------------------------------------------------------------
//	@function:
//		CGroup::OsPrintGrpOptCtxts
//
//	@doc:
//		Print group optimization contexts
//
//---------------------------------------------------------------------------
IOstream &
CGroup::OsPrintGrpOptCtxts(IOstream &os, const CHAR *szPrefix) const
{
	if (!FScalar() && !FDuplicateGroup() &&
		SPQOS_FTRACE(EopttracePrintOptimizationContext))
	{
		os << szPrefix << "Grp OptCtxts:" << std::endl;

		ULONG num_opt_contexts = m_sht.Size();

		for (ULONG ul = 0; ul < num_opt_contexts; ul++)
		{
			COptimizationContext *poc = Ppoc(ul);

			if (NULL != poc)
			{
				os << szPrefix;
				(void) poc->OsPrintWithPrefix(os, szPrefix);
			}

			SPQOS_CHECK_ABORT;
		}
	}

	return os;
}


//---------------------------------------------------------------------------
//	@function:
//		CGroup::OsPrintGrpScalarProps
//
//	@doc:
//		Print scalar group properties
//
//---------------------------------------------------------------------------
IOstream &
CGroup::OsPrintGrpScalarProps(IOstream &os, const CHAR *szPrefix) const
{
	SPQOS_ASSERT(FScalar());

	if (NULL != PexprScalarRep())
	{
		os << szPrefix << "Scalar Expression:";
		if (!FScalarRepIsExact())
		{
			os << " (subqueries replaced with true or NULL):";
		}
		os << std::endl << szPrefix << *PexprScalarRep() << std::endl;
	}

	SPQOS_CHECK_ABORT;

	if (NULL != m_pdrspqexprJoinKeysOuter)
	{
		os << szPrefix << "Outer Join Keys: " << std::endl;

		const ULONG size = m_pdrspqexprJoinKeysOuter->Size();
		for (ULONG ul = 0; ul < size; ul++)
		{
			os << szPrefix << *(*m_pdrspqexprJoinKeysOuter)[ul] << std::endl;
		}
	}

	SPQOS_CHECK_ABORT;

	if (NULL != m_pdrspqexprJoinKeysInner)
	{
		os << szPrefix << "Inner Join Keys: " << std::endl;

		const ULONG size = m_pdrspqexprJoinKeysInner->Size();
		for (ULONG ul = 0; ul < size; ul++)
		{
			os << szPrefix << *(*m_pdrspqexprJoinKeysInner)[ul] << std::endl;
		}
	}

	SPQOS_CHECK_ABORT;

	if (NULL != m_join_opfamilies)
	{
		os << szPrefix << "Inner Join Opfamilies: " << std::endl;

		const ULONG size = m_join_opfamilies->Size();
		for (ULONG ul = 0; ul < size; ul++)
		{
			os << szPrefix;
			(*m_join_opfamilies)[ul]->OsPrint(os);
			os << std::endl;
		}
	}

	return os;
}


//---------------------------------------------------------------------------
//	@function:
//		CGroup::OsPrintGrpProps
//
//	@doc:
//		Print group properties and stats
//
//---------------------------------------------------------------------------
IOstream &
CGroup::OsPrintGrpProps(IOstream &os, const CHAR *szPrefix) const
{
	if (!FDuplicateGroup() && SPQOS_FTRACE(EopttracePrintGroupProperties))
	{
		os << szPrefix << "Grp Props:" << std::endl
		   << szPrefix << szPrefix << *m_pdp << std::endl;
		if (!FScalar() && NULL != m_pstats)
		{
			os << szPrefix << "Grp Stats:" << std::endl
			   << szPrefix << szPrefix << *m_pstats;
		}

		if (FScalar())
		{
			(void) OsPrintGrpScalarProps(os, szPrefix);
		}

		SPQOS_CHECK_ABORT;
	}

	return os;
}


//---------------------------------------------------------------------------
//	@function:
//		CGroup::ResetGroupState
//
//	@doc:
//		Reset group state;
//		resetting state is not thread-safe
//
//---------------------------------------------------------------------------
void
CGroup::ResetGroupState()
{
	// reset group expression states
	CGroupExpression *pgexpr = m_listGExprs.First();
	while (NULL != pgexpr)
	{
		pgexpr->ResetState();
		pgexpr = m_listGExprs.Next(pgexpr);

		SPQOS_CHECK_ABORT;
	}

	// reset group state
	{
		CGroupProxy spq(this);
		m_estate = estUnexplored;
	}
}


//---------------------------------------------------------------------------
//	@function:
//		CGroup::Pstats
//
//	@doc:
//		Group stats accessor
//
//---------------------------------------------------------------------------
IStatistics *
CGroup::Pstats() const
{
	if (NULL != m_pstats)
	{
		return m_pstats;
	}

	if (FDuplicateGroup())
	{
		return PgroupDuplicate()->Pstats();
	}

	return NULL;
}


//---------------------------------------------------------------------------
//	@function:
//		CGroup::ResetStats
//
//	@doc:
//		Reset computed stats;
//
//
//---------------------------------------------------------------------------
void
CGroup::ResetStats()
{
	SPQOS_ASSERT(!FScalar());

	IStatistics *stats = NULL;
	{
		CGroupProxy spq(this);
		stats = m_pstats;
		m_pstats = NULL;
	}
	CRefCount::SafeRelease(stats);
	stats = NULL;
}


//---------------------------------------------------------------------------
//	@function:
//		CGroup::ResetLinkMap
//
//	@doc:
//		Reset link map for plan enumeration;
//		this operation is not thread safe
//
//
//---------------------------------------------------------------------------
void
CGroup::ResetLinkMap()
{
	SPQOS_ASSERT(NULL != m_plinkmap);

	m_plinkmap->Release();
	m_plinkmap = SPQOS_NEW(m_mp) LinkMap(m_mp);
}


//---------------------------------------------------------------------------
//	@function:
//		CGroup::FResetStats
//
//	@doc:
//		Check if we need to reset group stats before deriving statistics;
//		this function reset group stats in the following two cases:
//		(1) current group has newly-added logical operators, this can happen
//		during multi-stage search, where stage_{i+1} may append new logical
//		operators to a group created at stage_{i}
//		(2) a child group, reachable from current group at any depth, has new
//		logical group expressions
//
//---------------------------------------------------------------------------
BOOL
CGroup::FResetStats()
{
	SPQOS_CHECK_STACK_SIZE;

	if (NULL == Pstats())
	{
		// end recursion early if group stats have been already reset
		return true;
	}

	// if a duplicate group is found, the duplicate group stats will be
	// reset, whereas the group's own stats (most likely NULL) won't be
	// reset
	if (FDuplicateGroup())
	{
		return PgroupDuplicate()->FResetStats();
	}

	BOOL fResetStats = false;
	if (FHasNewLogicalOperators())
	{
		fResetStats = true;
	}

	// get first logical group expression
	CGroupExpression *pgexprCurrent = NULL;
	{
		CGroupProxy spq(this);
		pgexprCurrent = spq.PgexprNextLogical(NULL /*pgexpr*/);
	}

	// recursively process child groups reachable from current group
	while (NULL != pgexprCurrent)
	{
		const ULONG arity = pgexprCurrent->Arity();
		for (ULONG ul = 0; ul < arity; ul++)
		{
			SPQOS_CHECK_ABORT;

			CGroup *pgroupChild = (*pgexprCurrent)[ul];
			if (!pgroupChild->FScalar() &&
				// FIXME: Scenario exists where this group is child's duplicate
				//        group. When the case arises, the child's children
				//        won’t be reset. Otherwise we can hit infinite loop
				//        that resets child->this->child.
				(this == pgroupChild->PgroupDuplicate() ||
				 pgroupChild->FResetStats()))
			{
				fResetStats = true;
				// we cannot break here since we must visit all child groups
			}
		}

		// move to next logical group expression
		{
			CGroupProxy spq(this);
			pgexprCurrent = spq.PgexprNextLogical(pgexprCurrent);
		}

		SPQOS_CHECK_ABORT;
	}

	if (fResetStats)
	{
		ResetStats();
	}

	return fResetStats;
}

//---------------------------------------------------------------------------
//	@function:
//		CGroup::ResetGroupJobQueues
//
//	@doc:
//		Reset group job queues;
//
//---------------------------------------------------------------------------
void
CGroup::ResetGroupJobQueues()
{
	CGroupProxy spq(this);
	m_jqExploration.Reset();
	m_jqImplementation.Reset();
}

//---------------------------------------------------------------------------
//	@function:
//		CGroup::PstatsCompute
//
//	@doc:
//		Compute stats during costing
//
//---------------------------------------------------------------------------
IStatistics *
CGroup::PstatsCompute(COptimizationContext *poc, CExpressionHandle &exprhdl,
					  CGroupExpression *pgexpr)
{
	SPQOS_ASSERT(NULL != poc);
	SPQOS_ASSERT(NULL != pgexpr);
	SPQOS_ASSERT(this == pgexpr->Pgroup());

	IStatistics *stats = m_pstatsmap->Find(poc);
	if (NULL != stats)
	{
		return stats;
	}

	stats = CLogical::PopConvert(pgexpr->Pop())
				->PstatsDerive(m_mp, exprhdl, poc->Pdrspqstat());
	SPQOS_ASSERT(NULL != stats);

	// add computed stats to local map
	poc->AddRef();
#ifdef SPQOS_DEBUG
	BOOL fSuccess =
#endif	// SPQOS_DEBUG
		m_pstatsmap->Insert(poc, stats);
	SPQOS_ASSERT(fSuccess);

	return stats;
}


//---------------------------------------------------------------------------
//	@function:
//		CGroup::OsPrint
//
//	@doc:
//		Print function;
//		printing is not thread-safe
//
//---------------------------------------------------------------------------
IOstream &
CGroup::OsPrint(IOstream &os) const
{
	const CHAR *szPrefix = "  ";
	os << std::endl << "Group " << m_id << " (";
	if (!FScalar())
	{
		os << "#GExprs: " << m_listGExprs.Size();

		if (0 < m_listDupGExprs.Size())
		{
			os << ", #Duplicate GExprs: " << m_listDupGExprs.Size();
		}
		if (FDuplicateGroup())
		{
			os << ", Duplicate Group: " << m_pgroupDuplicate->Id();
		}
	}
	os << "):" << std::endl;

	CGroupExpression *pgexpr = m_listGExprs.First();
	while (NULL != pgexpr)
	{
		(void) pgexpr->OsPrintWithPrefix(os, szPrefix);
		pgexpr = m_listGExprs.Next(pgexpr);

		SPQOS_CHECK_ABORT;
	}

	(void) OsPrintGrpProps(os, szPrefix);
	(void) OsPrintGrpOptCtxts(os, szPrefix);

	return os;
}


//---------------------------------------------------------------------------
//	@function:
//		CGroup::CostLowerBound
//
//	@doc:
//		Compute a cost lower bound on plans, rooted by a group expression
//		in current group, and satisfying the given required properties
//
//---------------------------------------------------------------------------
CCost
CGroup::CostLowerBound(CMemoryPool *mp, CReqdPropPlan *prppInput)
{
	SPQOS_ASSERT(NULL != prppInput);
	SPQOS_ASSERT(!FScalar());

	CCost *pcostLowerBound = m_pcostmap->Find(prppInput);
	if (NULL != pcostLowerBound)
	{
		return *pcostLowerBound;
	}

	CCost costLowerBound = SPQOPT_INFINITE_COST;

	// start with first non-logical group expression
	CGroupExpression *pgexprCurrent = NULL;
	{
		CGroupProxy spq(this);
		pgexprCurrent = spq.PgexprSkipLogical(NULL /*pgexpr*/);
	}

	while (NULL != pgexprCurrent)
	{
		// considering an enforcer introduces a deadlock here since its child is
		// the same group that contains it,
		// since an enforcer must reside on top of another operator from the same
		// group, it cannot produce a better cost lower-bound and can be skipped here

		if (!CUtils::FEnforcer(pgexprCurrent->Pop()))
		{
			CCost costLowerBoundGExpr =
				pgexprCurrent->CostLowerBound(mp, prppInput, NULL /*pccChild*/,
											  spqos::ulong_max /*child_index*/);
			if (costLowerBoundGExpr < costLowerBound)
			{
				costLowerBound = costLowerBoundGExpr;
			}
		}

		// move to next non-logical group expression
		{
			CGroupProxy spq(this);
			pgexprCurrent = spq.PgexprSkipLogical(pgexprCurrent);
		}
	}


	prppInput->AddRef();
#ifdef SPQOS_DEBUG
	BOOL fSuccess =
#endif	// SPQOS_DEBUG
		m_pcostmap->Insert(prppInput, SPQOS_NEW(mp) CCost(costLowerBound.Get()));
	SPQOS_ASSERT(fSuccess);

	return costLowerBound;
}

#ifdef SPQOS_DEBUG
//---------------------------------------------------------------------------
//	@function:
//		CGroup::DbgPrint
//
//	@doc:
//		Print driving function for use in interactive debugging;
//		always prints to stderr;
//
//---------------------------------------------------------------------------
void
CGroup::DbgPrintWithProperties()
{
	CAutoTraceFlag atf(EopttracePrintGroupProperties, true);
	CAutoTrace at(m_mp);
	(void) this->OsPrint(at.Os());
}

#endif

void CGroup::PrintNewOptContext(const COptimizationContext *poc) const
{
    CAutoTrace at(m_mp);
    IOstream &os = at.Os();
    os << std::endl << "NewOptContext group:" << m_id << ", poc:" << poc << ", poc-id:" << poc->Id();
    os << ", StageIndex:" << poc->UlSearchStageIndex() << ", origin: [Grp:" << m_id;
    os << ", GrpExpr:"<< poc->pgexprOrigin_ID << "]" << std::endl;
}

void CGroup::PrintUpdateBestCost(const COptimizationContext *poc, const CCostContext *pcc) const
{
    CAutoTrace at(m_mp);
    IOstream &os = at.Os();
    os << std::endl << "PrintUpdateBestCost group:" << m_id << ", poc:" << poc << ", poc-id:" << poc->Id();
    os << ", origin: [Grp:" << m_id << ", GrpExpr:"<< poc->pgexprOrigin_ID << "]" << std::endl;
    CCostContext *oldBest = poc->PccBest();
    if (SPQOPT_INVALID_COST == pcc->Cost()) {
        os << "New PCC is INVALID_COST" << std::endl;
        return;
    }
    bool better = (oldBest == nullptr) ? true : pcc->FBetterThan(oldBest);
    if (better) {
        os << "new is better" << std::endl;
    } else {
        os << "old is better" << std::endl;
    }
    os << std::endl;
    if (oldBest != nullptr) {
        os << "old expression: "<< oldBest->Pgexpr()->Pop()->SzId();
        os << " [Grp:"<< m_id  << ", GrpExpr:"<< oldBest->Pgexpr()->Id() << ", req:"<< oldBest->UlOptReq() << "]" << std::endl;
        oldBest->OsCostPrint(os, "");
    }
    os << std::endl;
    os << "new expression: "<< pcc->Pgexpr()->Pop()->SzId();
    os << " [Grp:"<< m_id  << ", GrpExpr:"<< pcc->Pgexpr()->Id() << ", req:"<< pcc->UlOptReq() << "]" << std::endl;
    pcc->OsCostPrint(os, "");
}
// EOF
