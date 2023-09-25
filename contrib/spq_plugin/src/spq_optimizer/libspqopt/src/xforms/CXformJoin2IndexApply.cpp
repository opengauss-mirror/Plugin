//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 Pivotal, Inc.
//
//	@filename:
//		CXformJoin2IndexApply.cpp
//
//	@doc:
//		Implementation of Inner/Outer Join to Apply transform
//---------------------------------------------------------------------------

#include "spqopt/xforms/CXformJoin2IndexApply.h"

#include "spqos/base.h"

#include "spqopt/base/CUtils.h"
#include "spqopt/operators/CLogicalApply.h"
#include "spqopt/operators/CLogicalCTEAnchor.h"
#include "spqopt/operators/CLogicalDynamicGet.h"
#include "spqopt/operators/CLogicalIndexApply.h"
#include "spqopt/operators/CLogicalJoin.h"
#include "spqopt/operators/CLogicalSelect.h"
#include "spqopt/operators/CLogicalUnionAll.h"
#include "spqopt/operators/CNormalizer.h"
#include "spqopt/operators/CPredicateUtils.h"
#include "spqopt/xforms/CSubqueryHandler.h"
#include "spqopt/xforms/CXformUtils.h"
#include "naucrates/md/IMDIndex.h"

using namespace spqmd;
using namespace spqopt;

//---------------------------------------------------------------------------
//	@function:
//		CXformJoin2IndexApply::Exfp
//
//	@doc:
//		Compute xform promise for a given expression handle;
//
//---------------------------------------------------------------------------
CXform::EXformPromise
CXformJoin2IndexApply::Exfp(CExpressionHandle &exprhdl) const
{
	if (0 == exprhdl.DeriveUsedColumns(2)->Size() ||
		exprhdl.DeriveHasSubquery(2) || exprhdl.HasOuterRefs())
	{
		return CXform::ExfpNone;
	}

	return CXform::ExfpHigh;
}

//---------------------------------------------------------------------------
//	@function:
//		CXformJoin2IndexApply::ComputeColumnSets
//
//	@doc:
//		Based on the inner and the scalar expression, it computes scalar expression
//		columns, outer references and required columns.
//		Caller does not take ownership of ppcrsScalarExpr.
//		Caller takes ownership of ppcrsOuterRefs and ppcrsReqd.
//
//---------------------------------------------------------------------------
void
CXformJoin2IndexApply::ComputeColumnSets(CMemoryPool *mp,
										 CExpression *pexprInner,
										 CExpression *pexprScalar,
										 CColRefSet **ppcrsScalarExpr,
										 CColRefSet **ppcrsOuterRefs,
										 CColRefSet **ppcrsReqd) const
{
	CColRefSet *pcrsInnerOutput = pexprInner->DeriveOutputColumns();
	*ppcrsScalarExpr = pexprScalar->DeriveUsedColumns();
	*ppcrsOuterRefs = SPQOS_NEW(mp) CColRefSet(mp, **ppcrsScalarExpr);
	(*ppcrsOuterRefs)->Difference(pcrsInnerOutput);

	*ppcrsReqd = SPQOS_NEW(mp) CColRefSet(mp);
	(*ppcrsReqd)->Include(pcrsInnerOutput);
	(*ppcrsReqd)->Include(*ppcrsScalarExpr);
	(*ppcrsReqd)->Difference(*ppcrsOuterRefs);
}

//---------------------------------------------------------------------------
//	@function:
//		CXformJoin2IndexApply::CreateFullIndexApplyAlternatives
//
//	@doc:
//		Helper to add IndexApply expression to given xform results container
//
//---------------------------------------------------------------------------
void
CXformJoin2IndexApply::CreateHomogeneousIndexApplyAlternatives(
	CMemoryPool *mp, COperator *joinOp, CExpression *pexprOuter,
	CExpression *pexprInner, CExpression *pexprScalar,
	CExpression *origJoinPred, CExpression *nodesToInsertAboveIndexGet,
	CExpression *endOfNodesToInsertAboveIndexGet,
	CTableDescriptor *ptabdescInner, CLogicalDynamicGet *popDynamicGet,
	CXformResult *pxfres, IMDIndex::EmdindexType emdtype) const
{
	SPQOS_ASSERT(NULL != pexprOuter);
	SPQOS_ASSERT(NULL != pexprInner);
	SPQOS_ASSERT(NULL != pexprScalar);
	SPQOS_ASSERT(NULL != ptabdescInner);
	SPQOS_ASSERT(NULL != pxfres);
	SPQOS_ASSERT(IMDIndex::EmdindBtree == emdtype ||
				IMDIndex::EmdindBitmap == emdtype);

	const ULONG ulIndices = ptabdescInner->IndexCount();
	if (0 == ulIndices)
	{
		return;
	}

	// derive the scalar and relational properties to build set of required columns
	CColRefSet *pcrsScalarExpr = NULL;
	CColRefSet *outer_refs = NULL;
	CColRefSet *pcrsReqd = NULL;
	ComputeColumnSets(mp, pexprInner, pexprScalar, &pcrsScalarExpr, &outer_refs,
					  &pcrsReqd);

	if (IMDIndex::EmdindBtree == emdtype)
	{
		CreateHomogeneousBtreeIndexApplyAlternatives(
			mp, joinOp, pexprOuter, pexprInner, pexprScalar, origJoinPred,
			nodesToInsertAboveIndexGet, endOfNodesToInsertAboveIndexGet,
			ptabdescInner, popDynamicGet, pcrsScalarExpr, outer_refs, pcrsReqd,
			ulIndices, pxfres);
	}
	else
	{
		CreateHomogeneousBitmapIndexApplyAlternatives(
			mp, joinOp, pexprOuter, pexprInner, pexprScalar, origJoinPred,
			nodesToInsertAboveIndexGet, endOfNodesToInsertAboveIndexGet,
			ptabdescInner, outer_refs, pcrsReqd, pxfres);
	}

	//clean-up
	pcrsReqd->Release();
	outer_refs->Release();
}

//---------------------------------------------------------------------------
//	@function:
//		CXformJoin2IndexApply::CreateHomogeneousBtreeIndexApplyAlternatives
//
//	@doc:
//		Helper to add IndexApply expression to given xform results container
//		for homogeneous b-tree indexes
//
//---------------------------------------------------------------------------
void
CXformJoin2IndexApply::CreateHomogeneousBtreeIndexApplyAlternatives(
	CMemoryPool *mp, COperator *joinOp, CExpression *pexprOuter,
	CExpression *pexprInner, CExpression *pexprScalar,
	CExpression *origJoinPred, CExpression *nodesToInsertAboveIndexGet,
	CExpression *endOfNodesToInsertAboveIndexGet,
	CTableDescriptor *ptabdescInner, CLogicalDynamicGet *popDynamicGet,
	CColRefSet *pcrsScalarExpr, CColRefSet *outer_refs, CColRefSet *pcrsReqd,
	ULONG ulIndices, CXformResult *pxfres) const
{
	// array of expressions in the scalar expression
	CExpressionArray *pdrspqexpr =
		CPredicateUtils::PdrspqexprConjuncts(mp, pexprScalar);
	SPQOS_ASSERT(pdrspqexpr->Size() > 0);

	// find the indexes whose included columns meet the required columns
	CMDAccessor *md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();
	const IMDRelation *pmdrel = md_accessor->RetrieveRel(ptabdescInner->MDId());

	for (ULONG ul = 0; ul < ulIndices; ul++)
	{
		IMDId *pmdidIndex = pmdrel->IndexMDidAt(ul);
		const IMDIndex *pmdindex = md_accessor->RetrieveIndex(pmdidIndex);

		CPartConstraint *ppartcnstrIndex = NULL;
		if (NULL != popDynamicGet)
		{
			// Partition constraints are expensive to compute and needed only for
			// partial scans. For all other cases, pass along dummy constraints
			const BOOL fDummyConstraint = !popDynamicGet->IsPartial();
			ppartcnstrIndex = CUtils::PpartcnstrFromMDPartCnstr(
				mp, COptCtxt::PoctxtFromTLS()->Pmda(),
				popDynamicGet->PdrspqdrspqcrPart(), pmdindex->MDPartConstraint(),
				popDynamicGet->PdrspqcrOutput(), fDummyConstraint);
		}
		CreateAlternativesForBtreeIndex(
			mp, joinOp, pexprOuter, pexprInner, origJoinPred,
			nodesToInsertAboveIndexGet, endOfNodesToInsertAboveIndexGet,
			md_accessor, pdrspqexpr, pcrsScalarExpr, outer_refs, pcrsReqd,
			pmdrel, pmdindex, ppartcnstrIndex, pxfres);
	}

	//clean-up
	pdrspqexpr->Release();
}

//---------------------------------------------------------------------------
//	@function:
//		CXformJoin2IndexApply::CreateAlternativesForBtreeIndex
//
//	@doc:
//		Helper to add IndexApply expression to given xform results container
//		for homogeneous b-tree indexes.
//
//---------------------------------------------------------------------------
void
CXformJoin2IndexApply::CreateAlternativesForBtreeIndex(
	CMemoryPool *mp, COperator *joinOp, CExpression *pexprOuter,
	CExpression *pexprInner, CExpression *origJoinPred,
	CExpression *nodesToInsertAboveIndexGet,
	CExpression *endOfNodesToInsertAboveIndexGet, CMDAccessor *md_accessor,
	CExpressionArray *pdrspqexprConjuncts, CColRefSet *pcrsScalarExpr,
	CColRefSet *outer_refs, CColRefSet *pcrsReqd, const IMDRelation *pmdrel,
	const IMDIndex *pmdindex, CPartConstraint *ppartcnstrIndex,
	CXformResult *pxfres) const
{
	CExpression *pexprLogicalIndexGet = CXformUtils::PexprLogicalIndexGet(
		mp, md_accessor, pexprInner, joinOp->UlOpId(), pdrspqexprConjuncts,
		pcrsReqd, pcrsScalarExpr, outer_refs, pmdindex, pmdrel,
		false /*fAllowPartialIndex*/, ppartcnstrIndex);
	if (NULL != pexprLogicalIndexGet)
	{
		// second child has residual predicates, create an apply of outer and inner
		// and add it to xform results
		CColRefArray *colref_array = outer_refs->Pdrspqcr(mp);
		CExpression *indexGetWithOptionalSelect = pexprLogicalIndexGet;

		if (COperator::EopLogicalDynamicGet == pexprInner->Pop()->Eopid())
		{
			indexGetWithOptionalSelect =
				CXformUtils::PexprRedundantSelectForDynamicIndex(
					mp, pexprLogicalIndexGet);
			pexprLogicalIndexGet->Release();
		}

		CExpression *rightChildOfApply =
			CXformUtils::AddALinearStackOfUnaryExpressions(
				mp, indexGetWithOptionalSelect, nodesToInsertAboveIndexGet,
				endOfNodesToInsertAboveIndexGet);
		BOOL isOuterJoin = false;

		switch (joinOp->Eopid())
		{
			case COperator::EopLogicalInnerJoin:
				isOuterJoin = false;
				break;

			case COperator::EopLogicalLeftOuterJoin:
				isOuterJoin = true;
				break;

			default:
				// this type of join operator is not supported
				return;
		}

		pexprOuter->AddRef();
		CExpression *pexprIndexApply = SPQOS_NEW(mp) CExpression(
			mp,
			SPQOS_NEW(mp)
				CLogicalIndexApply(mp, colref_array, isOuterJoin, origJoinPred),
			pexprOuter, rightChildOfApply,
			CPredicateUtils::PexprConjunction(mp, NULL /*pdrspqexpr*/));
		pxfres->Add(pexprIndexApply);
	}
}

//---------------------------------------------------------------------------
//	@function:
//		CXformJoin2IndexApply::CreateHomogeneousBitmapIndexApplyAlternatives
//
//	@doc:
//		Helper to add IndexApply expression to given xform results container
//		for homogeneous bitmap indexes.
//
//---------------------------------------------------------------------------
void
CXformJoin2IndexApply::CreateHomogeneousBitmapIndexApplyAlternatives(
	CMemoryPool *mp, COperator *joinOp, CExpression *pexprOuter,
	CExpression *pexprInner, CExpression *pexprScalar,
	CExpression *origJoinPred, CExpression *nodesToInsertAboveIndexGet,
	CExpression *endOfNodesToInsertAboveIndexGet,
	CTableDescriptor *ptabdescInner, CColRefSet *outer_refs,
	CColRefSet *pcrsReqd, CXformResult *pxfres) const
{
	CLogical *popGet = CLogical::PopConvert(pexprInner->Pop());
	CExpression *pexprLogicalIndexGet = CXformUtils::PexprBitmapTableGet(
		mp, popGet, joinOp->UlOpId(), ptabdescInner, pexprScalar, outer_refs,
		pcrsReqd);
	if (NULL != pexprLogicalIndexGet)
	{
		// second child has residual predicates, create an apply of outer and inner
		// and add it to xform results
		CColRefArray *colref_array = outer_refs->Pdrspqcr(mp);
		CExpression *indexGetWithOptionalSelect = pexprLogicalIndexGet;

		if (COperator::EopLogicalDynamicGet == popGet->Eopid())
		{
			indexGetWithOptionalSelect =
				CXformUtils::PexprRedundantSelectForDynamicIndex(
					mp, pexprLogicalIndexGet);
			pexprLogicalIndexGet->Release();
		}

		CExpression *rightChildOfApply =
			CXformUtils::AddALinearStackOfUnaryExpressions(
				mp, indexGetWithOptionalSelect, nodesToInsertAboveIndexGet,
				endOfNodesToInsertAboveIndexGet);
		BOOL isOuterJoin = false;

		switch (joinOp->Eopid())
		{
			case COperator::EopLogicalInnerJoin:
				isOuterJoin = false;
				break;

			case COperator::EopLogicalLeftOuterJoin:
				isOuterJoin = true;
				break;

			default:
				// this type of join operator is not supported
				return;
		}

		pexprOuter->AddRef();
		CExpression *pexprIndexApply = SPQOS_NEW(mp) CExpression(
			mp,
			SPQOS_NEW(mp)
				CLogicalIndexApply(mp, colref_array, isOuterJoin, origJoinPred),
			pexprOuter, rightChildOfApply,
			CPredicateUtils::PexprConjunction(mp, NULL /*pdrspqexpr*/));
		pxfres->Add(pexprIndexApply);
	}
}

// Helper to add IndexApply expressions to given xform results container
// for partial indexes. For the partitions without indexes we add one
// regular inner join.
// For instance, if there are two partial indexes plus some partitions
// not covered by any index, the result of this xform on:
//
// clang-format off
// +--CLogicalInnerJoin
//   |--CLogicalGet "my_tt_agg_small", Columns: ["symbol" (0), "event_ts" (1)]
//   |--CLogicalDynamicGet "my_tq_agg_small_part", Columns: ["ets" (11), "end_ts" (15)]
//   +--CScalarBoolOp (EboolopAnd)
//     |--CScalarCmp (>=)
//     |  |--CScalarIdent "event_ts" (1)
//     |  +--CScalarIdent "ets" (11)
//     +--CScalarCmp (<)
//        |--CScalarIdent "event_ts" (1)
//        +--CScalarIdent "end_ts" (15)
//
// will look like:
//
// +--CLogicalCTEAnchor (0)
//    +--CLogicalUnionAll ["symbol" (0), "event_ts" (1),  ...]
//     |--CLogicalUnionAll ["symbol" (0), "event_ts" (1),  ...]
//     |  |--CLogicalInnerIndexApply
//     |  |  |--CLogicalCTEConsumer (0), Columns: ["symbol" (0), "event_ts" (1), "spq_segment_id" (10)]
//     |  |  |--CLogicalDynamicIndexGet   Index Name: ("my_tq_agg_small_part_ets_end_ts_ix_1"),
//     |  |  |      Columns: ["ets" (11), "end_ts" (15) "spq_segment_id" (22)]
//     |  |  |  |--CScalarBoolOp (EboolopAnd)
//     |  |  |  |  |--CScalarCmp (<=)
//     |  |  |  |  |  |--CScalarIdent "ets" (11)
//     |  |  |  |  |  +--CScalarIdent "event_ts" (1)
//     |  |  |  |  +--CScalarCmp (>)
//     |  |  |  |     |--CScalarIdent "end_ts" (15)
//     |  |  |  |     +--CScalarIdent "event_ts" (1)
//     |  |  |  +--CScalarConst (TRUE)
//     |  |  +--CScalarConst (TRUE)
//     |  +--CLogicalInnerIndexApply
//     |     |--CLogicalCTEConsumer (0), Columns: ["symbol" (35), "event_ts" (36), "spq_segment_id" (45)]
//     |     |--CLogicalDynamicIndexGet   Index Name: ("my_tq_agg_small_part_ets_end_ts_ix_2"),
//     |     |    Columns: ["ets" (46),  "end_ts" (50), "spq_segment_id" (57)]
//     |     |  |--CScalarCmp (<=)
//     |     |  |  |--CScalarIdent "ets" (46)
//     |     |  |  +--CScalarIdent "event_ts" (36)
//     |     |  +--CScalarCmp (<)
//     |     |     |--CScalarIdent "event_ts" (36)
//     |     |     +--CScalarIdent "end_ts" (50)
//     |     +--CScalarConst (TRUE)
//     +--CLogicalInnerJoin
//          |--CLogicalCTEConsumer (0), Columns: ["symbol" (58), "event_ts" (59), "spq_segment_id" (68)]
//          |--CLogicalDynamicGet "my_tq_agg_small_part" ,
//         Columns: ["ets" (69), "end_ts" (73), "spq_segment_id" (80)]
//            +--CScalarBoolOp (EboolopAnd)
//            |--CScalarCmp (>=)
//            |  |--CScalarIdent "event_ts" (59)
//            |  +--CScalarIdent "ets" (69)
//            +--CScalarCmp (<)
//               |--CScalarIdent "event_ts" (59)
//               +--CScalarIdent "end_ts" (73)
//
// clang-format on
void
CXformJoin2IndexApply::CreatePartialIndexApplyAlternatives(
	CMemoryPool *mp, COperator *joinOp, CExpression *pexprOuter,
	CExpression *pexprInner, CExpression *pexprScalar,
	CTableDescriptor *ptabdescInner, CLogicalDynamicGet *popDynamicGet,
	CXformResult *pxfres) const
{
	SPQOS_ASSERT(NULL != pexprOuter);
	SPQOS_ASSERT(NULL != pexprInner);
	SPQOS_ASSERT(NULL != pexprScalar);
	SPQOS_ASSERT(NULL != ptabdescInner);
	SPQOS_ASSERT(NULL != pxfres);

	if (NULL == popDynamicGet || popDynamicGet->IsPartial())
	{
		// not a dynamic get or
		// already a partial dynamic get; do not try to split further
		return;
	}

	const ULONG ulIndices = ptabdescInner->IndexCount();
	if (0 == ulIndices)
	{
		return;
	}

	CPartConstraint *ppartcnstr = popDynamicGet->Ppartcnstr();
	ppartcnstr->AddRef();

	CColRefSet *pcrsScalarExpr = NULL;
	CColRefSet *outer_refs = NULL;
	CColRefSet *pcrsReqd = NULL;
	ComputeColumnSets(mp, pexprInner, pexprScalar, &pcrsScalarExpr, &outer_refs,
					  &pcrsReqd);

	// find a candidate set of partial index combinations
	CMDAccessor *md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();
	const IMDRelation *pmdrel = md_accessor->RetrieveRel(ptabdescInner->MDId());

	// array of expressions in the scalar expression
	CExpressionArray *pdrspqexpr =
		CPredicateUtils::PdrspqexprConjuncts(mp, pexprScalar);

	SPQOS_ASSERT(0 < pdrspqexpr->Size());

	SPartDynamicIndexGetInfoArrays *pdrspqdrspqpartdig =
		CXformUtils::PdrspqdrspqpartdigCandidates(
			mp, COptCtxt::PoctxtFromTLS()->Pmda(), pdrspqexpr,
			popDynamicGet->PdrspqdrspqcrPart(), pmdrel, ppartcnstr,
			popDynamicGet->PdrspqcrOutput(), pcrsReqd, pcrsScalarExpr,
			outer_refs);



	// construct alternative partial index apply plans
	const ULONG ulCandidates = pdrspqdrspqpartdig->Size();
	for (ULONG ul = 0; ul < ulCandidates; ul++)
	{
		SPartDynamicIndexGetInfoArray *pdrspqpartdig = (*pdrspqdrspqpartdig)[ul];
		if (0 < pdrspqpartdig->Size())
		{
			CreatePartialIndexApplyPlan(mp, joinOp, pexprOuter, pexprScalar,
										outer_refs, popDynamicGet, pdrspqpartdig,
										pmdrel, pxfres);
		}
	}

	ppartcnstr->Release();
	pdrspqdrspqpartdig->Release();
	pcrsReqd->Release();
	outer_refs->Release();
	pdrspqexpr->Release();
}

//---------------------------------------------------------------------------
//	@function:
//		CXformJoin2IndexApply::CreatePartialIndexApplyPlan
//
//	@doc:
//		Create a plan as a union of the given partial index apply candidates and
//		possibly a regular inner join with a dynamic table scan on the inner branch.
//
//---------------------------------------------------------------------------
void
CXformJoin2IndexApply::CreatePartialIndexApplyPlan(
	CMemoryPool *mp, COperator *joinOp, CExpression *pexprOuter,
	CExpression *pexprScalar, CColRefSet *outer_refs,
	CLogicalDynamicGet *popDynamicGet,
	SPartDynamicIndexGetInfoArray *pdrspqpartdig, const IMDRelation *pmdrel,
	CXformResult *pxfres) const
{
	const ULONG ulPartialIndexes = pdrspqpartdig->Size();
	if (0 == ulPartialIndexes)
	{
		return;
	}

	CColRefArray *pdrspqcrGet = popDynamicGet->PdrspqcrOutput();
	const ULONG ulCTEId = COptCtxt::PoctxtFromTLS()->Pcteinfo()->next_id();

	// outer references mentioned in the scan filter: we need them to generate the IndexApply
	CColRefArray *pdrspqcrOuterRefsInScan = outer_refs->Pdrspqcr(mp);

	// columns from the outer branch of the IndexApply or Join
	// we will create copies of these columns because every CTE consumer needs to have its
	// own column ids
	CColRefArray *pdrspqcrOuter = pexprOuter->DeriveOutputColumns()->Pdrspqcr(mp);

	// positions in pdrspqcrOuter of the outer references mentioned in the scan filter
	// we need them because each time we create copies of pdrspqcrOuter, we will extract the
	// subsequence corresponding to pdrspqcrOuterRefsInScan
	ULongPtrArray *pdrspqulIndexesOfRefsInScan =
		pdrspqcrOuter->IndexesOfSubsequence(pdrspqcrOuterRefsInScan);

	SPQOS_ASSERT(NULL != pdrspqulIndexesOfRefsInScan);

	(void) CXformUtils::PexprAddCTEProducer(mp, ulCTEId, pdrspqcrOuter,
											pexprOuter);
	BOOL fFirst = true;

	CColRefArray *pdrspqcrOutput = NULL;
	CExpressionArray *pdrspqexprInput = SPQOS_NEW(mp) CExpressionArray(mp);
	CColRef2dArray *pdrspqdrspqcrInput = SPQOS_NEW(mp) CColRef2dArray(mp);

	for (ULONG ul = 0; ul < ulPartialIndexes; ul++)
	{
		SPartDynamicIndexGetInfo *ppartdig = (*pdrspqpartdig)[ul];

		const IMDIndex *pmdindex = ppartdig->m_pmdindex;
		CPartConstraint *ppartcnstr = ppartdig->m_part_constraint;
		CExpressionArray *pdrspqexprIndex = ppartdig->m_pdrspqexprIndex;
		CExpressionArray *pdrspqexprResidual = ppartdig->m_pdrspqexprResidual;

		CColRefArray *pdrspqcrOuterNew = pdrspqcrOuter;
		CColRefArray *pdrspqcrIndexGet = pdrspqcrGet;
		UlongToColRefMap *colref_mapping = NULL;
		if (fFirst)
		{
			// For the first child of the union, reuse the initial columns
			// because the output schema of the union must be identical to its first child's.
			pdrspqcrIndexGet->AddRef();
		}
		else
		{
			colref_mapping = SPQOS_NEW(mp) UlongToColRefMap(mp);
			pdrspqcrOuterNew = CUtils::PdrspqcrCopy(
				mp, pdrspqcrOuter, false /*fAllComputed*/, colref_mapping);
			pdrspqcrIndexGet = CUtils::PdrspqcrCopy(
				mp, pdrspqcrGet, false /*fAllComputed*/, colref_mapping);
		}

		CExpression *pexprUnionAllChild = NULL;
		if (NULL != pmdindex)
		{
			pexprUnionAllChild = PexprIndexApplyOverCTEConsumer(
				mp, joinOp, popDynamicGet, pdrspqexprIndex, pdrspqexprResidual,
				pdrspqcrIndexGet, pmdindex, pmdrel, fFirst, ulCTEId, ppartcnstr,
				outer_refs, pdrspqcrOuter, pdrspqcrOuterNew,
				pdrspqcrOuterRefsInScan, pdrspqulIndexesOfRefsInScan);
		}
		else
		{
			pexprUnionAllChild = PexprJoinOverCTEConsumer(
				mp, joinOp, popDynamicGet, ulCTEId, pexprScalar,
				pdrspqcrIndexGet, ppartcnstr, pdrspqcrOuter, pdrspqcrOuterNew);
		}

		CRefCount::SafeRelease(pdrspqcrIndexGet);

		// if we failed to create a DynamicIndexScan, we give up
		SPQOS_ASSERT(NULL != pexprUnionAllChild);

		CColRefArray *pdrspqcrNew = NULL;
		if (NULL == colref_mapping)
		{
			pdrspqcrNew = pexprUnionAllChild->DeriveOutputColumns()->Pdrspqcr(mp);
		}
		else
		{
			// inner branches of the union-all needs columns in the same order as in the outer branch
			pdrspqcrNew = CUtils::PdrspqcrRemap(mp, pdrspqcrOutput, colref_mapping,
											  true /*must_exist*/);
		}

		if (fFirst)
		{
			SPQOS_ASSERT(NULL != pdrspqcrNew);

			// the output columns of a UnionAll need to be exactly the input
			// columns of its first input branch
			pdrspqcrNew->AddRef();
			pdrspqcrOutput = pdrspqcrNew;
		}
		fFirst = false;

		pdrspqdrspqcrInput->Append(pdrspqcrNew);
		pdrspqexprInput->Append(pexprUnionAllChild);

		CRefCount::SafeRelease(colref_mapping);
	}

	SPQOS_ASSERT(pdrspqexprInput->Size() == pdrspqdrspqcrInput->Size());
	SPQOS_ASSERT(1 < pdrspqexprInput->Size());

	CExpression *pexprResult = NULL;
	if (2 <= pdrspqexprInput->Size())
	{
		// construct a new union operator
		pexprResult = SPQOS_NEW(mp) CExpression(
			mp,
			SPQOS_NEW(mp) CLogicalUnionAll(mp, pdrspqcrOutput, pdrspqdrspqcrInput,
										  popDynamicGet->ScanId()),
			pdrspqexprInput);
	}
	else
	{
		SPQOS_ASSERT(1 == pdrspqexprInput->Size());
		pexprResult = (*pdrspqexprInput)[0];
		pexprResult->AddRef();

		// clean up
		pdrspqdrspqcrInput->Release();
		pdrspqexprInput->Release();
		pdrspqcrOutput->Release();
	}

	pdrspqcrOuterRefsInScan->Release();
	pdrspqulIndexesOfRefsInScan->Release();

	AddUnionPlanForPartialIndexes(mp, popDynamicGet, ulCTEId, pexprResult,
								  pexprScalar, pxfres);
}

//---------------------------------------------------------------------------
//	@function:
//		CXformJoin2IndexApply::PexprJoinOverCTEConsumer
//
//	@doc:
//		Create an join with a CTE consumer on the inner branch,
//		with the given partition constraint
//
//---------------------------------------------------------------------------
CExpression *
CXformJoin2IndexApply::PexprJoinOverCTEConsumer(
	CMemoryPool *mp,
	COperator *,  // joinOp
	CLogicalDynamicGet *popDynamicGet, ULONG ulCTEId, CExpression *pexprScalar,
	CColRefArray *pdrspqcrDynamicGet, CPartConstraint *ppartcnstr,
	CColRefArray *pdrspqcrOuter, CColRefArray *pdrspqcrOuterNew) const
{
	UlongToColRefMap *colref_mapping = CUtils::PhmulcrMapping(
		mp, popDynamicGet->PdrspqcrOutput(), pdrspqcrDynamicGet);

	// construct a partial dynamic get with the negated constraint
	CPartConstraint *ppartcnstrPartialDynamicGet =
		ppartcnstr->PpartcnstrCopyWithRemappedColumns(mp, colref_mapping,
													  true /*must_exist*/);

	CLogicalDynamicGet *popPartialDynamicGet =
		(CLogicalDynamicGet *) popDynamicGet->PopCopyWithRemappedColumns(
			mp, colref_mapping, true /*must_exist*/
		);
	popPartialDynamicGet->SetPartConstraint(ppartcnstrPartialDynamicGet);
	popPartialDynamicGet->SetSecondaryScanId(
		COptCtxt::PoctxtFromTLS()->UlPartIndexNextVal());
	popPartialDynamicGet->SetPartial();

	// if there are any outer references, add them to the mapping
	ULONG ulOuterPcrs = pdrspqcrOuter->Size();
	SPQOS_ASSERT(ulOuterPcrs == pdrspqcrOuterNew->Size());

	for (ULONG ul = 0; ul < ulOuterPcrs; ul++)
	{
		CColRef *pcrOld = (*pdrspqcrOuter)[ul];
		CColRef *new_colref = (*pdrspqcrOuterNew)[ul];
#ifdef SPQOS_DEBUG
		BOOL fInserted =
#endif
			colref_mapping->Insert(SPQOS_NEW(mp) ULONG(pcrOld->Id()),
								   new_colref);
		SPQOS_ASSERT(fInserted);
	}

	CExpression *pexprJoin = SPQOS_NEW(mp)
		CExpression(mp, PopLogicalJoin(mp),
					CXformUtils::PexprCTEConsumer(mp, ulCTEId, pdrspqcrOuterNew),
					SPQOS_NEW(mp) CExpression(mp, popPartialDynamicGet),
					pexprScalar->PexprCopyWithRemappedColumns(
						mp, colref_mapping, true /*must_exist*/
						));
	colref_mapping->Release();

	return pexprJoin;
}

//---------------------------------------------------------------------------
//	@function:
//		CXformJoin2IndexApply::PexprIndexApplyOverCTEConsumer
//
//	@doc:
//		Create an index apply with a CTE consumer on the outer branch
//		and a dynamic get on the inner one
//
//---------------------------------------------------------------------------
CExpression *
CXformJoin2IndexApply::PexprIndexApplyOverCTEConsumer(
	CMemoryPool *mp, COperator *joinOp, CLogicalDynamicGet *popDynamicGet,
	CExpressionArray *pdrspqexprIndex, CExpressionArray *pdrspqexprResidual,
	CColRefArray *pdrspqcrIndexGet, const IMDIndex *pmdindex,
	const IMDRelation *pmdrel, BOOL fFirst, ULONG ulCTEId,
	CPartConstraint *ppartcnstr, CColRefSet *outer_refs,
	CColRefArray *pdrspqcrOuter, CColRefArray *pdrspqcrOuterNew,
	CColRefArray *pdrspqcrOuterRefsInScan,
	ULongPtrArray *pdrspqulIndexesOfRefsInScan) const
{
	CExpression *pexprDynamicScan = CXformUtils::PexprPartialDynamicIndexGet(
		mp, popDynamicGet, joinOp->UlOpId(), pdrspqexprIndex, pdrspqexprResidual,
		pdrspqcrIndexGet, pmdindex, pmdrel, ppartcnstr, outer_refs, pdrspqcrOuter,
		pdrspqcrOuterNew);

	if (NULL == pexprDynamicScan)
	{
		return NULL;
	}

	// create an apply of outer and inner and add it to the union
	CColRefArray *pdrspqcrOuterRefsInScanNew = pdrspqcrOuterRefsInScan;
	if (fFirst)
	{
		pdrspqcrOuterRefsInScanNew->AddRef();
	}
	else
	{
		pdrspqcrOuterRefsInScanNew = CXformUtils::PdrspqcrReorderedSubsequence(
			mp, pdrspqcrOuterNew, pdrspqulIndexesOfRefsInScan);
	}

	return SPQOS_NEW(mp)
		CExpression(mp, PopLogicalApply(mp, pdrspqcrOuterRefsInScanNew, NULL),
					CXformUtils::PexprCTEConsumer(mp, ulCTEId, pdrspqcrOuterNew),
					pexprDynamicScan,
					CPredicateUtils::PexprConjunction(mp, NULL /*pdrspqexpr*/));
}

//---------------------------------------------------------------------------
//	@function:
//		CXformJoin2IndexApply::PexprConstructUnionAll
//
//	@doc:
//		Create a union-all with the given children. Takes ownership of all
//		arguments.
//
//---------------------------------------------------------------------------
CExpression *
CXformJoin2IndexApply::PexprConstructUnionAll(CMemoryPool *mp,
											  CColRefArray *pdrspqcrLeftSchema,
											  CColRefArray *pdrspqcrRightSchema,
											  CExpression *pexprLeftChild,
											  CExpression *pexprRightChild,
											  ULONG scan_id) const
{
	CColRef2dArray *pdrspqdrspqcrInput = SPQOS_NEW(mp) CColRef2dArray(mp);
	pdrspqdrspqcrInput->Append(pdrspqcrLeftSchema);
	pdrspqdrspqcrInput->Append(pdrspqcrRightSchema);
	pdrspqcrLeftSchema->AddRef();

	return SPQOS_NEW(mp) CExpression(
		mp,
		SPQOS_NEW(mp)
			CLogicalUnionAll(mp, pdrspqcrLeftSchema, pdrspqdrspqcrInput, scan_id),
		pexprLeftChild, pexprRightChild);
}

//---------------------------------------------------------------------------
//	@function:
//		CXformJoin2IndexApply::AddUnionPlanForPartialIndexes
//
//	@doc:
//		Constructs a CTE Anchor over the given UnionAll and adds it to the given
//		Xform result
//
//---------------------------------------------------------------------------
void
CXformJoin2IndexApply::AddUnionPlanForPartialIndexes(
	CMemoryPool *mp, CLogicalDynamicGet *popDynamicGet, ULONG ulCTEId,
	CExpression *pexprUnion, CExpression *pexprScalar,
	CXformResult *pxfres) const
{
	if (NULL == pexprUnion)
	{
		return;
	}

	// if scalar expression involves the partitioning key, keep a SELECT node
	// on top for the purposes of partition selection
	CColRef2dArray *pdrspqdrspqcrPartKeys = popDynamicGet->PdrspqdrspqcrPart();
	CExpression *pexprPredOnPartKey =
		CPredicateUtils::PexprExtractPredicatesOnPartKeys(
			mp, pexprScalar, pdrspqdrspqcrPartKeys, NULL, /*pcrsAllowedRefs*/
			true										/*fUseConstraints*/
		);

	if (NULL != pexprPredOnPartKey)
	{
		pexprUnion =
			SPQOS_NEW(mp) CExpression(mp, SPQOS_NEW(mp) CLogicalSelect(mp),
									 pexprUnion, pexprPredOnPartKey);
	}

	CExpression *pexprAnchor = SPQOS_NEW(mp) CExpression(
		mp, SPQOS_NEW(mp) CLogicalCTEAnchor(mp, ulCTEId), pexprUnion);
	pxfres->Add(pexprAnchor);
}

// EOF
