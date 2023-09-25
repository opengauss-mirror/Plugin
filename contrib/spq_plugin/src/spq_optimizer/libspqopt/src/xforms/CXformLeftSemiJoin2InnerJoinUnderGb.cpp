//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 Pivotal, Inc.
//
//	@filename:
//		CXformLeftSemiJoin2InnerJoinUnderGb.cpp
//
//	@doc:
//		Implementation of transform
//---------------------------------------------------------------------------

#include "spqopt/xforms/CXformLeftSemiJoin2InnerJoinUnderGb.h"

#include "spqos/base.h"
#include "spqos/memory/CAutoMemoryPool.h"

#include "spqopt/base/CColRefSetIter.h"
#include "spqopt/base/CColRefTable.h"
#include "spqopt/base/CKeyCollection.h"
#include "spqopt/operators/CLogicalGbAggDeduplicate.h"
#include "spqopt/operators/CLogicalInnerJoin.h"
#include "spqopt/operators/CLogicalLeftSemiJoin.h"
#include "spqopt/operators/CPatternLeaf.h"
#include "spqopt/operators/CPredicateUtils.h"
#include "spqopt/operators/CScalarProjectList.h"

using namespace spqopt;

//---------------------------------------------------------------------------
//	@function:
//		CXformLeftSemiJoin2InnerJoinUnderGb::CXformLeftSemiJoin2InnerJoinUnderGb
//
//	@doc:
//		ctor
//
//---------------------------------------------------------------------------
CXformLeftSemiJoin2InnerJoinUnderGb::CXformLeftSemiJoin2InnerJoinUnderGb(
	CMemoryPool *mp)
	:  // pattern
	  CXformExploration(SPQOS_NEW(mp) CExpression(
		  mp, SPQOS_NEW(mp) CLogicalLeftSemiJoin(mp),
		  SPQOS_NEW(mp)
			  CExpression(mp, SPQOS_NEW(mp) CPatternLeaf(mp)),  // left child
		  SPQOS_NEW(mp)
			  CExpression(mp, SPQOS_NEW(mp) CPatternLeaf(mp)),  // right child
		  SPQOS_NEW(mp)
			  CExpression(mp, SPQOS_NEW(mp) CPatternLeaf(mp))  // predicate
		  ))
{
}

//---------------------------------------------------------------------------
//	@function:
//		CXformLeftSemiJoin2InnerJoinUnderGb::Exfp
//
//	@doc:
//		Compute xform promise for a given expression handle;
//
//---------------------------------------------------------------------------
CXform::EXformPromise
CXformLeftSemiJoin2InnerJoinUnderGb::Exfp(CExpressionHandle &exprhdl) const
{
	CColRefSet *pcrsInnerOutput = exprhdl.DeriveOutputColumns(1);
	CExpression *pexprScalar = exprhdl.PexprScalarExactChild(2);
	CAutoMemoryPool amp;
	if (exprhdl.HasOuterRefs() || NULL == exprhdl.DeriveKeyCollection(0) ||
		NULL == pexprScalar ||
		CPredicateUtils::FSimpleEqualityUsingCols(amp.Pmp(), pexprScalar,
												  pcrsInnerOutput))
	{
		return ExfpNone;
	}

	return ExfpHigh;
}

//---------------------------------------------------------------------------
//	@function:
//		CXformLeftSemiJoin2InnerJoinUnderGb::Transform
//
//	@doc:
//		actual transformation
//
//---------------------------------------------------------------------------
void
CXformLeftSemiJoin2InnerJoinUnderGb::Transform(CXformContext *pxfctxt,
											   CXformResult *pxfres,
											   CExpression *pexpr) const
{
	SPQOS_ASSERT(NULL != pxfctxt);
	SPQOS_ASSERT(FPromising(pxfctxt->Pmp(), this, pexpr));
	SPQOS_ASSERT(FCheckPattern(pexpr));

	CMemoryPool *mp = pxfctxt->Pmp();

	// extract components
	CExpression *pexprOuter = (*pexpr)[0];
	CExpression *pexprInner = (*pexpr)[1];
	CExpression *pexprScalar = (*pexpr)[2];

	pexprOuter->AddRef();
	pexprInner->AddRef();
	pexprScalar->AddRef();

	CColRefArray *pdrspqcrKeys = NULL;
	CColRefArray *pdrspqcrGrouping =
		CUtils::PdrspqcrGroupingKey(mp, pexprOuter, &pdrspqcrKeys);
	SPQOS_ASSERT(NULL != pdrspqcrKeys);

	CExpression *pexprInnerJoin = CUtils::PexprLogicalJoin<CLogicalInnerJoin>(
		mp, pexprOuter, pexprInner, pexprScalar);

	CExpression *pexprGb = SPQOS_NEW(mp) CExpression(
		mp,
		SPQOS_NEW(mp) CLogicalGbAggDeduplicate(
			mp, pdrspqcrGrouping, COperator::EgbaggtypeGlobal /*egbaggtype*/,
			pdrspqcrKeys),
		pexprInnerJoin,
		SPQOS_NEW(mp) CExpression(mp, SPQOS_NEW(mp) CScalarProjectList(mp)));

	pxfres->Add(pexprGb);
}

// EOF
