//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal, Inc.
//
//	@filename:
//		CXformLeftOuter2InnerUnionAllLeftAntiSemiJoin.cpp
//
//	@doc:
//		Transform
//      LOJ
//        |--Small
//        +--Big
//
// 		to
//
//      UnionAll
//      |---CTEConsumer(A)
//      +---Project_{append nulls)
//          +---LASJ_(key(Small))
//                   |---CTEConsumer(B)
//                   +---Gb(keys(Small))
//                        +---CTEConsumer(A)
//
//		where B is the CTE that produces Small
//		and A is the CTE that produces InnerJoin(Big, CTEConsumer(B)).
//
//	@owner:
//
//
//	@test:
//
//---------------------------------------------------------------------------

#include "spqopt/xforms/CXformLeftOuter2InnerUnionAllLeftAntiSemiJoin.h"

#include "spqos/memory/CAutoMemoryPool.h"

#include "spqopt/base/CColRefSetIter.h"
#include "spqopt/base/CUtils.h"
#include "spqopt/operators/CLogicalCTEAnchor.h"
#include "spqopt/operators/CLogicalGbAgg.h"
#include "spqopt/operators/CLogicalInnerJoin.h"
#include "spqopt/operators/CLogicalLeftAntiSemiJoin.h"
#include "spqopt/operators/CLogicalLeftOuterJoin.h"
#include "spqopt/operators/CLogicalUnionAll.h"
#include "spqopt/operators/CPatternTree.h"
#include "spqopt/operators/CScalarProjectList.h"
#include "spqopt/xforms/CXformUtils.h"
#include "naucrates/traceflags/traceflags.h"

using namespace spqopt;

// if ratio of the cardinalities outer/inner is below this value, we apply the xform
const DOUBLE
	CXformLeftOuter2InnerUnionAllLeftAntiSemiJoin::m_dOuterInnerRatioThreshold =
		0.001;

//---------------------------------------------------------------------------
//	@function:
//		CXformLeftOuter2InnerUnionAllLeftAntiSemiJoin::CXformLeftOuter2InnerUnionAllLeftAntiSemiJoin
//
//	@doc:
//		ctor
//
//---------------------------------------------------------------------------
CXformLeftOuter2InnerUnionAllLeftAntiSemiJoin::
	CXformLeftOuter2InnerUnionAllLeftAntiSemiJoin(CMemoryPool *mp)
	:  // pattern
	  CXformExploration(SPQOS_NEW(mp) CExpression(
		  mp, SPQOS_NEW(mp) CLogicalLeftOuterJoin(mp),
		  SPQOS_NEW(mp)
			  CExpression(mp, SPQOS_NEW(mp) CPatternTree(mp)),  // left child
		  SPQOS_NEW(mp)
			  CExpression(mp, SPQOS_NEW(mp) CPatternTree(mp)),  // right child
		  SPQOS_NEW(mp)
			  CExpression(mp, SPQOS_NEW(mp) CPatternTree(mp))  // predicate
		  ))
{
}

//---------------------------------------------------------------------------
//	@function:
//		CXformLeftOuter2InnerUnionAllLeftAntiSemiJoin::Exfp
//
//	@doc:
//		Compute xform promise for a given expression handle.
//
//---------------------------------------------------------------------------
CXform::EXformPromise
CXformLeftOuter2InnerUnionAllLeftAntiSemiJoin::Exfp(
	CExpressionHandle &exprhdl) const
{
	CColRefSet *pcrsInner = exprhdl.DeriveOutputColumns(1 /*child_index*/);
	CExpression *pexprScalar = exprhdl.PexprScalarExactChild(2 /*child_index*/);
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	if (NULL == pexprScalar ||
		!CPredicateUtils::FSimpleEqualityUsingCols(mp, pexprScalar, pcrsInner))
	{
		return ExfpNone;
	}

	if (SPQOS_FTRACE(
			spqos::
				EopttraceApplyLeftOuter2InnerUnionAllLeftAntiSemiJoinDisregardingStats) ||
		NULL == exprhdl.Pgexpr())
	{
		return CXform::ExfpHigh;
	}

	// check if stats are derivable on child groups
	const ULONG arity = exprhdl.Arity();
	for (ULONG ul = 0; ul < arity; ul++)
	{
		CGroup *pgroupChild = (*exprhdl.Pgexpr())[ul];
		if (!pgroupChild->FScalar() && !pgroupChild->FStatsDerivable(mp))
		{
			// stats must be derivable on every child
			return CXform::ExfpNone;
		}
	}

	return CXform::ExfpHigh;
}

//---------------------------------------------------------------------------
//	@function:
//		CXformLeftOuter2InnerUnionAllLeftAntiSemiJoin::FCheckStats
//
//	@doc:
//		Check the stats ratio to decide whether to apply the xform or not.
//
//---------------------------------------------------------------------------
BOOL
CXformLeftOuter2InnerUnionAllLeftAntiSemiJoin::FApplyXformUsingStatsInfo(
	const IStatistics *outer_stats, const IStatistics *inner_side_stats) const
{
	if (SPQOS_FTRACE(
			spqos::
				EopttraceApplyLeftOuter2InnerUnionAllLeftAntiSemiJoinDisregardingStats))
	{
		return true;
	}

	if (NULL == outer_stats || NULL == inner_side_stats)
	{
		return false;
	}

	DOUBLE num_rows_outer = outer_stats->Rows().Get();
	DOUBLE dRowsInner = inner_side_stats->Rows().Get();
	SPQOS_ASSERT(0 < dRowsInner);

	return num_rows_outer / dRowsInner <= m_dOuterInnerRatioThreshold;
}


// Apply the transformation, e.g.
//
// clang-format off
// Input:
//  +--CLogicalLeftOuterJoin
//     |--CLogicalGet "items", Columns: ["i_item_sk" (95)]
//     |--CLogicalGet "store_sales", Columns: ["ss_item_sk" (124)]
//     +--CScalarCmp (=)
//        |--CScalarIdent "i_item_sk"
//        +--CScalarIdent "ss_item_sk"
//  Output:
//  Alternatives:
//  0:
//  +--CLogicalCTEAnchor (2)
//     +--CLogicalCTEAnchor (3)
//        +--CLogicalUnionAll ["i_item_sk" (95), "ss_item_sk" (124)]
//           |--CLogicalCTEConsumer (3), Columns: ["i_item_sk" (95), "ss_item_sk" (124)]
//           +--CLogicalProject
//              |--CLogicalLeftAntiSemiJoin
//              |  |--CLogicalCTEConsumer (2), Columns: ["i_item_sk" (342)]
//              |  |--CLogicalGbAgg( GetGlobalMemoryPool ) Grp Cols: ["i_item_sk" (343)][Global], Minimal Grp Cols: [], Generates Duplicates :[ 0 ]
//              |  |  |--CLogicalCTEConsumer (3), Columns: ["i_item_sk" (343), "ss_item_sk" (344)]
//              |  |  +--CScalarProjectList
//              |  +--CScalarBoolOp (EboolopNot)
//              |     +--CScalarIsDistinctFrom (=)
//              |        |--CScalarIdent "i_item_sk" (342)
//              |        +--CScalarIdent "i_item_sk" (343)
//              +--CScalarProjectList
//                 +--CScalarProjectElement "ss_item_sk" (466)
//                    +--CScalarConst (null)
//
//  +--CLogicalCTEProducer (2), Columns: ["i_item_sk" (190)]
//     +--CLogicalGet "items", Columns: ["i_item_sk" (190)]
//
//  +--CLogicalCTEProducer (3), Columns: ["i_item_sk" (247), "ss_item_sk" (248)]
//      +--CLogicalInnerJoin
//         |--CLogicalCTEConsumer (0), Columns: ["ss_item_sk" (248)]
//         |--CLogicalCTEConsumer (2), Columns: ["i_item_sk" (247)]
//         +--CScalarCmp (=)
//            |--CScalarIdent "i_item_sk" (247)
//            +--CScalarIdent "ss_item_sk" (248)
//
// clang-format on
void
CXformLeftOuter2InnerUnionAllLeftAntiSemiJoin::Transform(
	CXformContext *pxfctxt, CXformResult *pxfres, CExpression *pexpr) const
{
	SPQOS_ASSERT(NULL != pxfctxt);
	SPQOS_ASSERT(FPromising(pxfctxt->Pmp(), this, pexpr));
	SPQOS_ASSERT(FCheckPattern(pexpr));

	CMemoryPool *mp = pxfctxt->Pmp();
	// extract components
	CExpression *pexprOuter = (*pexpr)[0];
	CExpression *pexprInner = (*pexpr)[1];
	CExpression *pexprScalar = (*pexpr)[2];

	if (!FValidInnerExpr(pexprInner))
	{
		return;
	}

	if (!FApplyXformUsingStatsInfo(pexprOuter->Pstats(), pexprInner->Pstats()))
	{
		return;
	}

	const ULONG ulCTEOuterId = COptCtxt::PoctxtFromTLS()->Pcteinfo()->next_id();
	CColRefSet *outer_refs = pexprOuter->DeriveOutputColumns();
	CColRefArray *pdrspqcrOuter = outer_refs->Pdrspqcr(mp);
	(void) CXformUtils::PexprAddCTEProducer(mp, ulCTEOuterId, pdrspqcrOuter,
											pexprOuter);

	// invert the order of the branches of the original join, so that the small one becomes
	// inner
	pexprInner->AddRef();
	pexprScalar->AddRef();
	CExpression *pexprInnerJoin = SPQOS_NEW(mp) CExpression(
		mp, SPQOS_NEW(mp) CLogicalInnerJoin(mp), pexprInner,
		CXformUtils::PexprCTEConsumer(mp, ulCTEOuterId, pdrspqcrOuter),
		pexprScalar);

	CColRefSet *pcrsJoinOutput = pexpr->DeriveOutputColumns();
	CColRefArray *pdrspqcrJoinOutput = pcrsJoinOutput->Pdrspqcr(mp);
	const ULONG ulCTEJoinId = COptCtxt::PoctxtFromTLS()->Pcteinfo()->next_id();
	(void) CXformUtils::PexprAddCTEProducer(mp, ulCTEJoinId, pdrspqcrJoinOutput,
											pexprInnerJoin);

	CColRefSet *pcrsScalar = pexprScalar->DeriveUsedColumns();
	CColRefSet *pcrsInner = pexprInner->DeriveOutputColumns();

	CColRefArray *pdrspqcrProjectOutput = NULL;
	CExpression *pexprProjectAppendNulls = PexprProjectOverLeftAntiSemiJoin(
		mp, pdrspqcrOuter, pcrsScalar, pcrsInner, pdrspqcrJoinOutput, ulCTEJoinId,
		ulCTEOuterId, &pdrspqcrProjectOutput);
	SPQOS_ASSERT(NULL != pdrspqcrProjectOutput);

	CColRef2dArray *pdrspqdrspqcrUnionInput = SPQOS_NEW(mp) CColRef2dArray(mp);
	pdrspqcrJoinOutput->AddRef();
	pdrspqdrspqcrUnionInput->Append(pdrspqcrJoinOutput);
	pdrspqdrspqcrUnionInput->Append(pdrspqcrProjectOutput);
	pdrspqcrJoinOutput->AddRef();

	CExpression *pexprUnionAll = SPQOS_NEW(mp) CExpression(
		mp,
		SPQOS_NEW(mp)
			CLogicalUnionAll(mp, pdrspqcrJoinOutput, pdrspqdrspqcrUnionInput),
		CXformUtils::PexprCTEConsumer(mp, ulCTEJoinId, pdrspqcrJoinOutput),
		pexprProjectAppendNulls);
	CExpression *pexprJoinAnchor = SPQOS_NEW(mp) CExpression(
		mp, SPQOS_NEW(mp) CLogicalCTEAnchor(mp, ulCTEJoinId), pexprUnionAll);
	CExpression *pexprOuterAnchor = SPQOS_NEW(mp) CExpression(
		mp, SPQOS_NEW(mp) CLogicalCTEAnchor(mp, ulCTEOuterId), pexprJoinAnchor);
	pexprInnerJoin->Release();

	pxfres->Add(pexprOuterAnchor);
}

//---------------------------------------------------------------------------
//	@function:
//		CXformLeftOuter2InnerUnionAllLeftAntiSemiJoin::FValidInnerExpr
//
//	@doc:
//		Check if the inner expression is of a type which should be considered
//		by this xform.
//
//---------------------------------------------------------------------------
BOOL
CXformLeftOuter2InnerUnionAllLeftAntiSemiJoin::FValidInnerExpr(
	CExpression *pexprInner)
{
	SPQOS_ASSERT(NULL != pexprInner);

	// set of inner operator ids that should not be considered because they usually
	// generate a relatively small number of tuples
	COperator::EOperatorId rgeopids[] = {
		COperator::EopLogicalConstTableGet,
		COperator::EopLogicalGbAgg,
		COperator::EopLogicalLimit,
	};

	const COperator::EOperatorId op_id = pexprInner->Pop()->Eopid();
	for (ULONG ul = 0; ul < SPQOS_ARRAY_SIZE(rgeopids); ++ul)
	{
		if (rgeopids[ul] == op_id)
		{
			return false;
		}
	}

	return true;
}

//---------------------------------------------------------------------------
//	@function:
//		CXformLeftOuter2InnerUnionAllLeftAntiSemiJoin::PexprLeftAntiSemiJoinWithInnerGroupBy
//
//	@doc:
//		Construct a left anti semi join with the CTE consumer (ulCTEJoinId) as outer
//		and a group by as inner.
//
//---------------------------------------------------------------------------
CExpression *
CXformLeftOuter2InnerUnionAllLeftAntiSemiJoin::
	PexprLeftAntiSemiJoinWithInnerGroupBy(CMemoryPool *mp,
										  CColRefArray *pdrspqcrOuter,
										  CColRefArray *pdrspqcrOuterCopy,
										  CColRefSet *pcrsScalar,
										  CColRefSet *pcrsInner,
										  CColRefArray *pdrspqcrJoinOutput,
										  ULONG ulCTEJoinId, ULONG ulCTEOuterId)
{
	// compute the original outer keys and their correspondent keys on the two branches
	// of the LASJ
	CColRefSet *pcrsOuterKeys = SPQOS_NEW(mp) CColRefSet(mp);
	pcrsOuterKeys->Include(pcrsScalar);
	pcrsOuterKeys->Difference(pcrsInner);
	CColRefArray *pdrspqcrOuterKeys = pcrsOuterKeys->Pdrspqcr(mp);

	CColRefArray *pdrspqcrConsumer2Output =
		CUtils::PdrspqcrCopy(mp, pdrspqcrJoinOutput);
	ULongPtrArray *pdrspqulIndexesOfOuterInGby =
		pdrspqcrJoinOutput->IndexesOfSubsequence(pdrspqcrOuterKeys);

	SPQOS_ASSERT(NULL != pdrspqulIndexesOfOuterInGby);
	CColRefArray *pdrspqcrGbyKeys = CXformUtils::PdrspqcrReorderedSubsequence(
		mp, pdrspqcrConsumer2Output, pdrspqulIndexesOfOuterInGby);

	CExpression *pexprGby = SPQOS_NEW(mp) CExpression(
		mp,
		SPQOS_NEW(mp)
			CLogicalGbAgg(mp, pdrspqcrGbyKeys, COperator::EgbaggtypeGlobal),
		CXformUtils::PexprCTEConsumer(mp, ulCTEJoinId, pdrspqcrConsumer2Output),
		SPQOS_NEW(mp) CExpression(mp, SPQOS_NEW(mp) CScalarProjectList(mp)));

	ULongPtrArray *pdrspqulIndexesOfOuterKeys =
		pdrspqcrOuter->IndexesOfSubsequence(pdrspqcrOuterKeys);
	SPQOS_ASSERT(NULL != pdrspqulIndexesOfOuterKeys);
	CColRefArray *pdrspqcrKeysInOuterCopy =
		CXformUtils::PdrspqcrReorderedSubsequence(mp, pdrspqcrOuterCopy,
												 pdrspqulIndexesOfOuterKeys);

	CColRef2dArray *pdrspqdrspqcrLASJInput = SPQOS_NEW(mp) CColRef2dArray(mp);
	pdrspqdrspqcrLASJInput->Append(pdrspqcrKeysInOuterCopy);
	pdrspqcrGbyKeys->AddRef();
	pdrspqdrspqcrLASJInput->Append(pdrspqcrGbyKeys);

	pcrsOuterKeys->Release();
	pdrspqcrOuterKeys->Release();

	CExpression *pexprLeftAntiSemi = SPQOS_NEW(mp) CExpression(
		mp, SPQOS_NEW(mp) CLogicalLeftAntiSemiJoin(mp),
		CXformUtils::PexprCTEConsumer(mp, ulCTEOuterId, pdrspqcrOuterCopy),
		pexprGby, CUtils::PexprConjINDFCond(mp, pdrspqdrspqcrLASJInput));

	pdrspqdrspqcrLASJInput->Release();
	pdrspqulIndexesOfOuterInGby->Release();
	pdrspqulIndexesOfOuterKeys->Release();

	return pexprLeftAntiSemi;
}

//---------------------------------------------------------------------------
//	@function:
//		CXformLeftOuter2InnerUnionAllLeftAntiSemiJoin::PexprProjectOverLeftAntiSemiJoin
//
//	@doc:
//		Return a project over a left anti semi join that appends nulls for all
//		columns in the original inner child.
//
//---------------------------------------------------------------------------
CExpression *
CXformLeftOuter2InnerUnionAllLeftAntiSemiJoin::PexprProjectOverLeftAntiSemiJoin(
	CMemoryPool *mp, CColRefArray *pdrspqcrOuter, CColRefSet *pcrsScalar,
	CColRefSet *pcrsInner, CColRefArray *pdrspqcrJoinOutput, ULONG ulCTEJoinId,
	ULONG ulCTEOuterId, CColRefArray **ppdrspqcrProjectOutput)
{
	SPQOS_ASSERT(NULL != pdrspqcrOuter);
	SPQOS_ASSERT(NULL != pcrsScalar);
	SPQOS_ASSERT(NULL != pcrsInner);
	SPQOS_ASSERT(NULL != pdrspqcrJoinOutput);

	// make a copy of outer for the second CTE consumer (outer of LASJ)
	CColRefArray *pdrspqcrOuterCopy = CUtils::PdrspqcrCopy(mp, pdrspqcrOuter);

	CExpression *pexprLeftAntiSemi = PexprLeftAntiSemiJoinWithInnerGroupBy(
		mp, pdrspqcrOuter, pdrspqcrOuterCopy, pcrsScalar, pcrsInner,
		pdrspqcrJoinOutput, ulCTEJoinId, ulCTEOuterId);

	ULongPtrArray *pdrspqulIndexesOfOuter =
		pdrspqcrJoinOutput->IndexesOfSubsequence(pdrspqcrOuter);
	SPQOS_ASSERT(NULL != pdrspqulIndexesOfOuter);

	UlongToColRefMap *colref_mapping = SPQOS_NEW(mp) UlongToColRefMap(mp);
	const ULONG ulOuterCopyLength = pdrspqcrOuterCopy->Size();

	for (ULONG ul = 0; ul < ulOuterCopyLength; ++ul)
	{
		ULONG ulOrigIndex = *(*pdrspqulIndexesOfOuter)[ul];
		CColRef *pcrOriginal = (*pdrspqcrJoinOutput)[ulOrigIndex];
#ifdef SPQOS_DEBUG
		BOOL fInserted =
#endif
			colref_mapping->Insert(SPQOS_NEW(mp) ULONG(pcrOriginal->Id()),
								   (*pdrspqcrOuterCopy)[ul]);
		SPQOS_ASSERT(fInserted);
	}

	CColRefArray *pdrspqcrInner = pcrsInner->Pdrspqcr(mp);
	CExpression *pexprProject = CUtils::PexprLogicalProjectNulls(
		mp, pdrspqcrInner, pexprLeftAntiSemi, colref_mapping);

	// compute the output array in the order needed by the union-all above the projection
	*ppdrspqcrProjectOutput = CUtils::PdrspqcrRemap(
		mp, pdrspqcrJoinOutput, colref_mapping, true /*must_exist*/);

	pdrspqcrInner->Release();
	colref_mapping->Release();
	pdrspqulIndexesOfOuter->Release();

	return pexprProject;
}

BOOL
CXformLeftOuter2InnerUnionAllLeftAntiSemiJoin::IsApplyOnce()
{
	return true;
}
// EOF
