//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 EMC Corp.
//
//	@filename:
//		CJoinOrderDP.cpp
//
//	@doc:
//		Implementation of dynamic programming-based join order generation
//---------------------------------------------------------------------------

#include "spqopt/xforms/CJoinOrderDP.h"

#include "spqos/base.h"
#include "spqos/common/CBitSet.h"
#include "spqos/common/CBitSetIter.h"
#include "spqos/common/clibwrapper.h"
#include "spqos/error/CAutoTrace.h"

#include "spqopt/base/CDrvdPropScalar.h"
#include "spqopt/base/CUtils.h"
#include "spqopt/exception.h"
#include "spqopt/operators/CLogicalInnerJoin.h"
#include "spqopt/operators/CNormalizer.h"
#include "spqopt/operators/CPatternLeaf.h"
#include "spqopt/operators/CPredicateUtils.h"

using namespace spqopt;

#define SPQOPT_DP_JOIN_ORDERING_TOPK 10

//---------------------------------------------------------------------------
//	@function:
//		CJoinOrderDP::SComponentPair::SComponentPair
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CJoinOrderDP::SComponentPair::SComponentPair(CBitSet *pbsFst, CBitSet *pbsSnd)
	: m_pbsFst(pbsFst), m_pbsSnd(pbsSnd)
{
	SPQOS_ASSERT(NULL != pbsFst);
	SPQOS_ASSERT(NULL != pbsSnd);
	SPQOS_ASSERT(pbsFst->IsDisjoint(pbsSnd));
}


//---------------------------------------------------------------------------
//	@function:
//		CJoinOrderDP::SComponentPair::HashValue
//
//	@doc:
//		Hash function
//
//---------------------------------------------------------------------------
ULONG
CJoinOrderDP::SComponentPair::HashValue(const SComponentPair *pcomppair)
{
	SPQOS_ASSERT(NULL != pcomppair);

	return CombineHashes(pcomppair->m_pbsFst->HashValue(),
						 pcomppair->m_pbsSnd->HashValue());
}


//---------------------------------------------------------------------------
//	@function:
//		CJoinOrderDP::SComponentPair::Equals
//
//	@doc:
//		Equality function
//
//---------------------------------------------------------------------------
BOOL
CJoinOrderDP::SComponentPair::Equals(const SComponentPair *pcomppairFst,
									 const SComponentPair *pcomppairSnd)
{
	SPQOS_ASSERT(NULL != pcomppairFst);
	SPQOS_ASSERT(NULL != pcomppairSnd);

	return pcomppairFst->m_pbsFst->Equals(pcomppairSnd->m_pbsFst) &&
		   pcomppairFst->m_pbsSnd->Equals(pcomppairSnd->m_pbsSnd);
}


//---------------------------------------------------------------------------
//	@function:
//		CJoinOrderDP::SComponentPair::~SComponentPair
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CJoinOrderDP::SComponentPair::~SComponentPair()
{
	m_pbsFst->Release();
	m_pbsSnd->Release();
}


//---------------------------------------------------------------------------
//	@function:
//		CJoinOrderDP::CJoinOrderDP
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CJoinOrderDP::CJoinOrderDP(CMemoryPool *mp,
						   CExpressionArray *pdrspqexprComponents,
						   CExpressionArray *pdrspqexprConjuncts)
	: CJoinOrder(mp, pdrspqexprComponents, pdrspqexprConjuncts,
				 false /* m_include_loj_childs */)
{
	m_phmcomplink = SPQOS_NEW(mp) ComponentPairToExpressionMap(mp);
	m_phmbsexpr = SPQOS_NEW(mp) BitSetToExpressionMap(mp);
	m_phmexprcost = SPQOS_NEW(mp) ExpressionToCostMap(mp);
	m_pdrspqexprTopKOrders = SPQOS_NEW(mp) CExpressionArray(mp);
	m_pexprDummy = SPQOS_NEW(mp) CExpression(mp, SPQOS_NEW(mp) CPatternLeaf(mp));

#ifdef SPQOS_DEBUG
	for (ULONG ul = 0; ul < m_ulComps; ul++)
	{
		SPQOS_ASSERT(NULL != m_rspqcomp[ul]->m_pexpr->Pstats() &&
					"stats were not derived on input component");
	}
#endif	// SPQOS_DEBUG
}


//---------------------------------------------------------------------------
//	@function:
//		CJoinOrderDP::~CJoinOrderDP
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CJoinOrderDP::~CJoinOrderDP()
{
#ifdef SPQOS_DEBUG
	// in optimized build, we flush-down memory pools without leak checking,
	// we can save time in optimized build by skipping all de-allocations here,
	// we still have all de-llocations enabled in debug-build to detect any possible leaks
	m_phmcomplink->Release();
	m_phmbsexpr->Release();
	m_phmexprcost->Release();
	m_pdrspqexprTopKOrders->Release();
	m_pexprDummy->Release();
#endif	// SPQOS_DEBUG
}


//---------------------------------------------------------------------------
//	@function:
//		CJoinOrderDP::AddJoinOrder
//
//	@doc:
//		Add given join order to top k join orders
//
//---------------------------------------------------------------------------
void
CJoinOrderDP::AddJoinOrder(CExpression *pexprJoin, CDouble dCost)
{
	SPQOS_ASSERT(NULL != pexprJoin);
	SPQOS_ASSERT(NULL != m_pdrspqexprTopKOrders);

	// length of the array will not be more than 10
	INT ulResults = m_pdrspqexprTopKOrders->Size();
	INT iReplacePos = -1;
	BOOL fAddJoinOrder = false;
	if (ulResults < SPQOPT_DP_JOIN_ORDERING_TOPK)
	{
		// we have less than K results, always add the given expression
		fAddJoinOrder = true;
	}
	else
	{
		CDouble dmaxCost = 0.0;
		// we have stored K expressions, evict worst expression
		for (INT ul = 0; ul < ulResults; ul++)
		{
			CExpression *pexpr = (*m_pdrspqexprTopKOrders)[ul];
			CDouble *pd = m_phmexprcost->Find(pexpr);
			SPQOS_ASSERT(NULL != pd);

			if (dmaxCost < *pd && dCost < *pd)
			{
				// found a worse expression
				dmaxCost = *pd;
				fAddJoinOrder = true;
				iReplacePos = ul;
			}
		}
	}

	if (fAddJoinOrder)
	{
		pexprJoin->AddRef();
		if (iReplacePos > -1)
		{
			m_pdrspqexprTopKOrders->Replace((ULONG) iReplacePos, pexprJoin);
		}
		else
		{
			m_pdrspqexprTopKOrders->Append(pexprJoin);
		}

		InsertExpressionCost(pexprJoin, dCost, false /*fValidateInsert*/);
	}
}


//---------------------------------------------------------------------------
//	@function:
//		CJoinOrderDP::PexprLookup
//
//	@doc:
//		Lookup best join order for given set
//
//---------------------------------------------------------------------------
CExpression *
CJoinOrderDP::PexprLookup(CBitSet *pbs)
{
	// if set has size 1, return expression directly
	if (1 == pbs->Size())
	{
		CBitSetIter bsi(*pbs);
		(void) bsi.Advance();

		return m_rspqcomp[bsi.Bit()]->m_pexpr;
	}

	// otherwise, return expression by looking up DP table
	return m_phmbsexpr->Find(pbs);
}


//---------------------------------------------------------------------------
//	@function:
//		CJoinOrderDP::PexprPred
//
//	@doc:
//		Extract predicate joining the two given sets
//
//---------------------------------------------------------------------------
CExpression *
CJoinOrderDP::PexprPred(CBitSet *pbsFst, CBitSet *pbsSnd)
{
	SPQOS_ASSERT(NULL != pbsFst);
	SPQOS_ASSERT(NULL != pbsSnd);

	if (!pbsFst->IsDisjoint(pbsSnd) || 0 == pbsFst->Size() ||
		0 == pbsSnd->Size())
	{
		// components must be non-empty and disjoint
		return NULL;
	}

	CExpression *pexprPred = NULL;
	SComponentPair *pcomppair = NULL;

	// lookup link map
	for (ULONG ul = 0; ul < 2; ul++)
	{
		pbsFst->AddRef();
		pbsSnd->AddRef();
		pcomppair = SPQOS_NEW(m_mp) SComponentPair(pbsFst, pbsSnd);
		pexprPred = m_phmcomplink->Find(pcomppair);
		if (NULL != pexprPred)
		{
			pcomppair->Release();
			if (m_pexprDummy == pexprPred)
			{
				return NULL;
			}
			return pexprPred;
		}

		// try again after swapping sets
		if (0 == ul)
		{
			pcomppair->Release();
			std::swap(pbsFst, pbsSnd);
		}
	}

	// could not find link in the map, construct it from edge set
	pexprPred = PexprBuildPred(pbsFst, pbsSnd);
	if (NULL == pexprPred)
	{
		m_pexprDummy->AddRef();
		pexprPred = m_pexprDummy;
	}

	// store predicate in link map
#ifdef SPQOS_DEBUG
	BOOL fInserted =
#endif	// SPQOS_DEBUG
		m_phmcomplink->Insert(pcomppair, pexprPred);
	SPQOS_ASSERT(fInserted);

	if (m_pexprDummy != pexprPred)
	{
		return pexprPred;
	}

	return NULL;
}


//---------------------------------------------------------------------------
//	@function:
//		CJoinOrderDP::PexprJoin
//
//	@doc:
//		Join expressions in the given two sets
//
//---------------------------------------------------------------------------
CExpression *
CJoinOrderDP::PexprJoin(CBitSet *pbsFst, CBitSet *pbsSnd)
{
	SPQOS_ASSERT(NULL != pbsFst);
	SPQOS_ASSERT(NULL != pbsSnd);

	CExpression *pexprFst = PexprLookup(pbsFst);
	SPQOS_ASSERT(NULL != pexprFst);

	CExpression *pexprSnd = PexprLookup(pbsSnd);
	SPQOS_ASSERT(NULL != pexprSnd);

	CExpression *pexprScalar = PexprPred(pbsFst, pbsSnd);
	SPQOS_ASSERT(NULL != pexprScalar);

	pexprFst->AddRef();
	pexprSnd->AddRef();
	pexprScalar->AddRef();

	return CUtils::PexprLogicalJoin<CLogicalInnerJoin>(m_mp, pexprFst, pexprSnd,
													   pexprScalar);
}


//---------------------------------------------------------------------------
//	@function:
//		CJoinOrderDP::DeriveStats
//
//	@doc:
//		Derive stats on given expression
//
//---------------------------------------------------------------------------
void
CJoinOrderDP::DeriveStats(CExpression *pexpr)
{
	SPQOS_ASSERT(NULL != pexpr);

	if (m_pexprDummy != pexpr && NULL == pexpr->Pstats())
	{
		CExpressionHandle exprhdl(m_mp);
		exprhdl.Attach(pexpr);
		exprhdl.DeriveStats(m_mp, m_mp, NULL /*prprel*/, NULL /*stats_ctxt*/);
	}
}

//---------------------------------------------------------------------------
//	@function:
//		CJoinOrderDP::InsertExpressionCost
//
//	@doc:
//		Add expression to cost map
//
//---------------------------------------------------------------------------
void
CJoinOrderDP::InsertExpressionCost(
	CExpression *pexpr, CDouble dCost,
	BOOL fValidateInsert  // if true, insertion must succeed
)
{
	SPQOS_ASSERT(NULL != pexpr);

	if (m_pexprDummy == pexpr)
	{
		// ignore dummy expression
		return;
	}

	if (!fValidateInsert && NULL != m_phmexprcost->Find(pexpr))
	{
		// expression already exists in cost map
		return;
	}

	pexpr->AddRef();
#ifdef SPQOS_DEBUG
	BOOL fInserted =
#endif	// SPQOS_DEBUG
		m_phmexprcost->Insert(pexpr, SPQOS_NEW(m_mp) CDouble(dCost));
	SPQOS_ASSERT(fInserted);
}


//---------------------------------------------------------------------------
//	@function:
//		CJoinOrderDP::PexprJoin
//
//	@doc:
//		Join expressions in the given set
//
//---------------------------------------------------------------------------
CExpression *
CJoinOrderDP::PexprJoin(CBitSet *pbs)
{
	SPQOS_ASSERT(2 == pbs->Size());

	CBitSetIter bsi(*pbs);
	(void) bsi.Advance();
	ULONG ulCompFst = bsi.Bit();
	(void) bsi.Advance();
	ULONG ulCompSnd = bsi.Bit();
	SPQOS_ASSERT(!bsi.Advance());

	CBitSet *pbsFst = SPQOS_NEW(m_mp) CBitSet(m_mp);
	(void) pbsFst->ExchangeSet(ulCompFst);
	CBitSet *pbsSnd = SPQOS_NEW(m_mp) CBitSet(m_mp);
	(void) pbsSnd->ExchangeSet(ulCompSnd);
	CExpression *pexprScalar = PexprPred(pbsFst, pbsSnd);
	pbsFst->Release();
	pbsSnd->Release();

	if (NULL == pexprScalar)
	{
		return NULL;
	}

	CExpression *pexprLeft = m_rspqcomp[ulCompFst]->m_pexpr;
	CExpression *pexprRight = m_rspqcomp[ulCompSnd]->m_pexpr;
	pexprLeft->AddRef();
	pexprRight->AddRef();
	pexprScalar->AddRef();
	CExpression *pexprJoin = CUtils::PexprLogicalJoin<CLogicalInnerJoin>(
		m_mp, pexprLeft, pexprRight, pexprScalar);

	DeriveStats(pexprJoin);
	// store solution in DP table
	pbs->AddRef();
#ifdef SPQOS_DEBUG
	BOOL fInserted =
#endif	// SPQOS_DEBUG
		m_phmbsexpr->Insert(pbs, pexprJoin);
	SPQOS_ASSERT(fInserted);

	return pexprJoin;
}


//---------------------------------------------------------------------------
//	@function:
//		CJoinOrderDP::PexprBestJoinOrderDP
//
//	@doc:
//		Find the best join order of a given set of elements using dynamic
//		programming;
//		given a set of elements (e.g., {A, B, C}), we find all possible splits
//		of the set (e.g., {A}, {B, C}) where at least one edge connects the
//		two subsets resulting from the split,
//		for each split, we find the best join orders of left and right subsets
//		recursively,
//		the function finds the split with the least cost, and stores the join
//		of its two subsets as the best join order of the given set
//
//
//---------------------------------------------------------------------------
CExpression *
CJoinOrderDP::PexprBestJoinOrderDP(CBitSet *pbs	 // set of elements to be joined
)
{
	CDouble dMinCost(0.0);
	CExpression *pexprResult = NULL;

	CBitSetArray *pdrspqbsSubsets = PdrspqbsSubsets(m_mp, pbs);
	const ULONG ulSubsets = pdrspqbsSubsets->Size();
	for (ULONG ul = 0; ul < ulSubsets; ul++)
	{
		CBitSet *pbsCurrent = (*pdrspqbsSubsets)[ul];
		CBitSet *pbsRemaining = SPQOS_NEW(m_mp) CBitSet(m_mp, *pbs);
		pbsRemaining->Difference(pbsCurrent);

		// check if subsets are connected with one or more edges
		CExpression *pexprPred = PexprPred(pbsCurrent, pbsRemaining);
		if (NULL != pexprPred)
		{
			// compute solutions of left and right subsets recursively
			CExpression *pexprLeft = PexprBestJoinOrder(pbsCurrent);
			CExpression *pexprRight = PexprBestJoinOrder(pbsRemaining);

			if (NULL != pexprLeft && NULL != pexprRight)
			{
				// we found solutions of left and right subsets, we check if
				// this gives a better solution for the input set
				CExpression *pexprJoin = PexprJoin(pbsCurrent, pbsRemaining);
				CDouble dCost = DCost(pexprJoin);

				if (NULL == pexprResult || dCost < dMinCost)
				{
					// this is the first solution, or we found a better solution
					dMinCost = dCost;
					CRefCount::SafeRelease(pexprResult);
					pexprJoin->AddRef();
					pexprResult = pexprJoin;
				}

				if (m_ulComps == pbs->Size())
				{
					AddJoinOrder(pexprJoin, dCost);
				}

				pexprJoin->Release();
			}
		}
		pbsRemaining->Release();
	}
	pdrspqbsSubsets->Release();

	// store solution in DP table
	if (NULL == pexprResult)
	{
		m_pexprDummy->AddRef();
		pexprResult = m_pexprDummy;
	}

	DeriveStats(pexprResult);
	pbs->AddRef();
#ifdef SPQOS_DEBUG
	BOOL fInserted =
#endif	// SPQOS_DEBUG
		m_phmbsexpr->Insert(pbs, pexprResult);
	SPQOS_ASSERT(fInserted);

	// add expression cost to cost map
	InsertExpressionCost(pexprResult, dMinCost, false /*fValidateInsert*/);

	return pexprResult;
}


//---------------------------------------------------------------------------
//	@function:
//		CJoinOrderDP::GenerateSubsets
//
//	@doc:
//		Generate all subsets of given array of elements
//
//---------------------------------------------------------------------------
void
CJoinOrderDP::GenerateSubsets(CMemoryPool *mp, CBitSet *pbsCurrent,
							  ULONG *pulElems, ULONG size, ULONG ulIndex,
							  CBitSetArray *pdrspqbsSubsets)
{
	SPQOS_CHECK_STACK_SIZE;
	SPQOS_CHECK_ABORT;

	SPQOS_ASSERT(ulIndex <= size);
	SPQOS_ASSERT(NULL != pbsCurrent);
	SPQOS_ASSERT(NULL != pulElems);
	SPQOS_ASSERT(NULL != pdrspqbsSubsets);

	if (ulIndex == size)
	{
		pdrspqbsSubsets->Append(pbsCurrent);
		return;
	}

	CBitSet *pbsCopy = SPQOS_NEW(mp) CBitSet(mp, *pbsCurrent);
#ifdef SPQOS_DEBUG
	BOOL fSet =
#endif	// SPQOS_DEBUG
		pbsCopy->ExchangeSet(pulElems[ulIndex]);
	SPQOS_ASSERT(!fSet);

	GenerateSubsets(mp, pbsCopy, pulElems, size, ulIndex + 1, pdrspqbsSubsets);
	GenerateSubsets(mp, pbsCurrent, pulElems, size, ulIndex + 1,
					pdrspqbsSubsets);
}

//---------------------------------------------------------------------------
//	@function:
//		CJoinOrderDP::PdrspqbsSubsets
//
//	@doc:
//		 Driver of subset generation
//
//---------------------------------------------------------------------------
CBitSetArray *
CJoinOrderDP::PdrspqbsSubsets(CMemoryPool *mp, CBitSet *pbs)
{
	const ULONG size = pbs->Size();
	ULONG *pulElems = SPQOS_NEW_ARRAY(mp, ULONG, size);
	ULONG ul = 0;
	CBitSetIter bsi(*pbs);
	while (bsi.Advance())
	{
		pulElems[ul++] = bsi.Bit();
	}

	CBitSet *pbsCurrent = SPQOS_NEW(mp) CBitSet(mp);
	CBitSetArray *pdrspqbsSubsets = SPQOS_NEW(mp) CBitSetArray(mp);
	GenerateSubsets(mp, pbsCurrent, pulElems, size, 0, pdrspqbsSubsets);
	SPQOS_DELETE_ARRAY(pulElems);

	return pdrspqbsSubsets;
}

//---------------------------------------------------------------------------
//	@function:
//		CJoinOrderDP::DCost
//
//	@doc:
//		Primitive costing of join expressions;
//		cost of a join expression is the summation of the costs of its
//		children plus its local cost;
//		cost of a leaf expression is the estimated number of rows
//
//---------------------------------------------------------------------------
CDouble
CJoinOrderDP::DCost(CExpression *pexpr)
{
	SPQOS_CHECK_STACK_SIZE;
	SPQOS_ASSERT(NULL != pexpr);

	CDouble *pd = m_phmexprcost->Find(pexpr);
	if (NULL != pd)
	{
		// stop recursion if cost was already cashed
		return *pd;
	}

	CDouble dCost(0.0);
	const ULONG arity = pexpr->Arity();
	if (0 == arity)
	{
		if (NULL == pexpr->Pstats())
		{
			SPQOS_RAISE(
				CException::ExmaInvalid, CException::ExmiAssert,
				SPQOS_WSZ_LIT("stats were not derived on an input component"));
		}

		// leaf operator, use its estimated number of rows as cost
		dCost = CDouble(pexpr->Pstats()->Rows());
	}
	else
	{
		// inner join operator, sum-up cost of its children
		DOUBLE rgdRows[2] = {0.0, 0.0};
		for (ULONG ul = 0; ul < arity - 1; ul++)
		{
			CExpression *pexprChild = (*pexpr)[ul];

			// call function recursively to find child cost
			dCost = dCost + DCost(pexprChild);
			DeriveStats(pexprChild);
			rgdRows[ul] = pexprChild->Pstats()->Rows().Get();
		}

		// add inner join local cost
		dCost = dCost + (rgdRows[0] + rgdRows[1]);
	}

	return dCost;
}


//---------------------------------------------------------------------------
//	@function:
//		CJoinOrderDP::PbsCovered
//
//	@doc:
//		Return a subset of the given set covered by one or more edges
//
//---------------------------------------------------------------------------
CBitSet *
CJoinOrderDP::PbsCovered(CBitSet *pbsInput)
{
	SPQOS_ASSERT(NULL != pbsInput);
	CBitSet *pbs = SPQOS_NEW(m_mp) CBitSet(m_mp);

	for (ULONG ul = 0; ul < m_ulEdges; ul++)
	{
		SEdge *pedge = m_rspqedge[ul];
		if (pbsInput->ContainsAll(pedge->m_pbs))
		{
			pbs->Union(pedge->m_pbs);
		}
	}

	return pbs;
}


//---------------------------------------------------------------------------
//	@function:
//		CJoinOrderDP::PexprCross
//
//	@doc:
//		Generate cross product for the given components
//
//---------------------------------------------------------------------------
CExpression *
CJoinOrderDP::PexprCross(CBitSet *pbs)
{
	SPQOS_ASSERT(NULL != pbs);

	CExpression *pexpr = PexprLookup(pbs);
	if (NULL != pexpr)
	{
		// join order is already created
		return pexpr;
	}

	CBitSetIter bsi(*pbs);
	(void) bsi.Advance();
	CExpression *pexprComp = m_rspqcomp[bsi.Bit()]->m_pexpr;
	pexprComp->AddRef();
	CExpression *pexprCross = pexprComp;
	while (bsi.Advance())
	{
		pexprComp = m_rspqcomp[bsi.Bit()]->m_pexpr;
		pexprComp->AddRef();
		pexprCross = CUtils::PexprLogicalJoin<CLogicalInnerJoin>(
			m_mp, pexprComp, pexprCross,
			CPredicateUtils::PexprConjunction(m_mp, NULL /*pdrspqexpr*/));
	}

	pbs->AddRef();
#ifdef SPQOS_DEBUG
	BOOL fInserted =
#endif	// SPQOS_DEBUG
		m_phmbsexpr->Insert(pbs, pexprCross);
	SPQOS_ASSERT(fInserted);

	return pexprCross;
}


//---------------------------------------------------------------------------
//	@function:
//		CJoinOrderDP::PexprJoinCoveredSubsetWithUncoveredSubset
//
//	@doc:
//		Join a covered subset with uncovered subset
//
//---------------------------------------------------------------------------
CExpression *
CJoinOrderDP::PexprJoinCoveredSubsetWithUncoveredSubset(CBitSet *pbs,
														CBitSet *pbsCovered,
														CBitSet *pbsUncovered)
{
	SPQOS_ASSERT(NULL != pbs);
	SPQOS_ASSERT(NULL != pbsCovered);
	SPQOS_ASSERT(NULL != pbsUncovered);
	SPQOS_ASSERT(pbsCovered->IsDisjoint(pbsUncovered));
	SPQOS_ASSERT(pbs->ContainsAll(pbsCovered));
	SPQOS_ASSERT(pbs->ContainsAll(pbsUncovered));

	// find best join order for covered subset
	CExpression *pexprJoin = PexprBestJoinOrder(pbsCovered);
	if (NULL == pexprJoin)
	{
		return NULL;
	}

	// create a cross product for uncovered subset
	CExpression *pexprCross = PexprCross(pbsUncovered);

	// join the results with a cross product
	pexprJoin->AddRef();
	pexprCross->AddRef();
	CExpression *pexprResult = CUtils::PexprLogicalJoin<CLogicalInnerJoin>(
		m_mp, pexprJoin, pexprCross,
		CPredicateUtils::PexprConjunction(m_mp, NULL));
	pbs->AddRef();
#ifdef SPQOS_DEBUG
	BOOL fInserted =
#endif	// SPQOS_DEBUG
		m_phmbsexpr->Insert(pbs, pexprResult);
	SPQOS_ASSERT(fInserted);

	return pexprResult;
}


//---------------------------------------------------------------------------
//	@function:
//		CJoinOrderDP::PexprBestJoinOrder
//
//	@doc:
//		find best join order for a given set of elements;
//
//---------------------------------------------------------------------------
CExpression *
CJoinOrderDP::PexprBestJoinOrder(CBitSet *pbs)
{
	SPQOS_CHECK_STACK_SIZE;
	SPQOS_CHECK_ABORT;

	SPQOS_ASSERT(NULL != pbs);

	// start by looking-up cost in the DP map
	CExpression *pexpr = PexprLookup(pbs);

	if (pexpr == m_pexprDummy)
	{
		// no join order could be created
		return NULL;
	}

	if (NULL != pexpr)
	{
		// join order is found by looking up map
		return pexpr;
	}

	// find maximal covered subset
	CBitSet *pbsCovered = PbsCovered(pbs);
	if (0 == pbsCovered->Size())
	{
		// set is not covered, return a cross product
		pbsCovered->Release();

		return PexprCross(pbs);
	}

	if (!pbsCovered->Equals(pbs))
	{
		// create a cross product for uncovered subset
		CBitSet *pbsUncovered = SPQOS_NEW(m_mp) CBitSet(m_mp, *pbs);
		pbsUncovered->Difference(pbsCovered);
		CExpression *pexprResult = PexprJoinCoveredSubsetWithUncoveredSubset(
			pbs, pbsCovered, pbsUncovered);
		pbsCovered->Release();
		pbsUncovered->Release();

		return pexprResult;
	}
	pbsCovered->Release();

	// if set has size 2, there is only one possible solution
	if (2 == pbs->Size())
	{
		return PexprJoin(pbs);
	}

	// otherwise, compute best join order using dynamic programming
	CExpression *pexprBestJoinOrder = PexprBestJoinOrderDP(pbs);
	if (pexprBestJoinOrder == m_pexprDummy)
	{
		// no join order could be created
		return NULL;
	}

	return pexprBestJoinOrder;
}


//---------------------------------------------------------------------------
//	@function:
//		CJoinOrderDP::PexprBuildPred
//
//	@doc:
//		Build predicate connecting the two given sets
//
//---------------------------------------------------------------------------
CExpression *
CJoinOrderDP::PexprBuildPred(CBitSet *pbsFst, CBitSet *pbsSnd)
{
	// collect edges connecting the given sets
	CBitSet *pbsEdges = SPQOS_NEW(m_mp) CBitSet(m_mp);
	CBitSet *pbs = SPQOS_NEW(m_mp) CBitSet(m_mp, *pbsFst);
	pbs->Union(pbsSnd);

	for (ULONG ul = 0; ul < m_ulEdges; ul++)
	{
		SEdge *pedge = m_rspqedge[ul];
		if (pbs->ContainsAll(pedge->m_pbs) &&
			!pbsFst->IsDisjoint(pedge->m_pbs) &&
			!pbsSnd->IsDisjoint(pedge->m_pbs))
		{
#ifdef SPQOS_DEBUG
			BOOL fSet =
#endif	// SPQOS_DEBUG
				pbsEdges->ExchangeSet(ul);
			SPQOS_ASSERT(!fSet);
		}
	}
	pbs->Release();

	CExpression *pexprPred = NULL;
	if (0 < pbsEdges->Size())
	{
		CExpressionArray *pdrspqexpr = SPQOS_NEW(m_mp) CExpressionArray(m_mp);
		CBitSetIter bsi(*pbsEdges);
		while (bsi.Advance())
		{
			ULONG ul = bsi.Bit();
			SEdge *pedge = m_rspqedge[ul];
			pedge->m_pexpr->AddRef();
			pdrspqexpr->Append(pedge->m_pexpr);
		}

		pexprPred = CPredicateUtils::PexprConjunction(m_mp, pdrspqexpr);
	}

	pbsEdges->Release();
	return pexprPred;
}


//---------------------------------------------------------------------------
//	@function:
//		CJoinOrderDP::PexprExpand
//
//	@doc:
//		Create join order
//
//---------------------------------------------------------------------------
CExpression *
CJoinOrderDP::PexprExpand()
{
	CBitSet *pbs = SPQOS_NEW(m_mp) CBitSet(m_mp);
	for (ULONG ul = 0; ul < m_ulComps; ul++)
	{
		(void) pbs->ExchangeSet(ul);
	}

	CExpression *pexprResult = PexprBestJoinOrder(pbs);
	if (NULL != pexprResult)
	{
		pexprResult->AddRef();
	}
	pbs->Release();

	return pexprResult;
}


FORCE_GENERATE_DBGSTR(spqopt::CJoinOrderDP);

//---------------------------------------------------------------------------
//	@function:
//		CJoinOrderDP::OsPrint
//
//	@doc:
//		Print created join order
//
//---------------------------------------------------------------------------
IOstream &
CJoinOrderDP::OsPrint(IOstream &os) const
{
	// increase SPQOS_LOG_MESSAGE_BUFFER_SIZE in file ILogger.h if the output of this method gets truncated
	CHashMapIter<CBitSet, CExpression, UlHashBitSet, FEqualBitSet,
				 CleanupRelease<CBitSet>, CleanupRelease<CExpression> >
		bitset_to_expr_map_iterator(m_phmbsexpr);
	CPrintPrefix pref(NULL, "      ");

	while (bitset_to_expr_map_iterator.Advance())
	{
		CDouble *cost =
			m_phmexprcost->Find(bitset_to_expr_map_iterator.Value());

		os << "Bitset: ";
		bitset_to_expr_map_iterator.Key()->OsPrint(os);
		os << std::endl;
		if (NULL != cost)
		{
			os << "Cost: " << *cost << std::endl;
		}
		else
		{
			os << "Cost: None" << std::endl;
		}
		os << "Best expression: " << std::endl;
		bitset_to_expr_map_iterator.Value()->OsPrintExpression(os, &pref);
	}

	for (ULONG k = 0; k < m_pdrspqexprTopKOrders->Size(); k++)
	{
		CDouble *cost = m_phmexprcost->Find((*m_pdrspqexprTopKOrders)[k]);

		os << "Best top-level expression [" << k << "]: " << std::endl;
		if (NULL != cost)
		{
			os << "Cost: " << *cost << std::endl;
		}
		else
		{
			os << "Cost: None" << std::endl;
		}
		(*m_pdrspqexprTopKOrders)[k]->OsPrintExpression(os, &pref);
	}
	os << std::endl;

	return os;
}
