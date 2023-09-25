//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CXformIntersectAll2LeftSemiJoin.cpp
//
//	@doc:
//		Implement the transformation of CLogicalIntersectAll into a left semi join
//---------------------------------------------------------------------------

#include "spqopt/xforms/CXformIntersectAll2LeftSemiJoin.h"

#include "spqos/base.h"

#include "spqopt/base/CColRefComputed.h"
#include "spqopt/base/CUtils.h"
#include "spqopt/exception.h"
#include "spqopt/operators/CLogicalIntersectAll.h"
#include "spqopt/operators/COperator.h"
#include "spqopt/operators/CPatternLeaf.h"
#include "spqopt/translate/CTranslatorDXLToExpr.h"
#include "spqopt/xforms/CXformUtils.h"

using namespace spqmd;
using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CXformIntersectAll2LeftSemiJoin::CXformIntersectAll2LeftSemiJoin
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CXformIntersectAll2LeftSemiJoin::CXformIntersectAll2LeftSemiJoin(
	CMemoryPool *mp)
	: CXformExploration(
		  // pattern
		  SPQOS_NEW(mp) CExpression(
			  mp, SPQOS_NEW(mp) CLogicalIntersectAll(mp),
			  SPQOS_NEW(mp) CExpression(
				  mp, SPQOS_NEW(mp) CPatternLeaf(mp)),  // left relational child
			  SPQOS_NEW(mp) CExpression(
				  mp, SPQOS_NEW(mp) CPatternLeaf(mp))  // right relational child
			  ))
{
}

//---------------------------------------------------------------------------
//	@function:
//		CXformIntersectAll2LeftSemiJoin::Transform
//
//	@doc:
//		Actual transformation that transforms a intersect all into a left semi
//		join over a window operation over the inputs
//
//---------------------------------------------------------------------------
void
CXformIntersectAll2LeftSemiJoin::Transform(CXformContext *pxfctxt,
										   CXformResult *pxfres,
										   CExpression *pexpr) const
{
	SPQOS_ASSERT(NULL != pxfctxt);
	SPQOS_ASSERT(NULL != pxfres);
	SPQOS_ASSERT(FPromising(pxfctxt->Pmp(), this, pexpr));
	SPQOS_ASSERT(FCheckPattern(pexpr));

	CMemoryPool *mp = pxfctxt->Pmp();

	// TODO: we currently only handle intersect all operators that
	// have two children
	SPQOS_ASSERT(2 == pexpr->Arity());

	// extract components
	CExpression *pexprLeftChild = (*pexpr)[0];
	CExpression *pexprRightChild = (*pexpr)[1];

	CLogicalIntersectAll *popIntersectAll =
		CLogicalIntersectAll::PopConvert(pexpr->Pop());
	CColRef2dArray *pdrspqdrspqcrInput = popIntersectAll->PdrspqdrspqcrInput();

	CExpression *pexprLeftWindow = CXformUtils::PexprWindowWithRowNumber(
		mp, pexprLeftChild, (*pdrspqdrspqcrInput)[0]);
	CExpression *pexprRightWindow = CXformUtils::PexprWindowWithRowNumber(
		mp, pexprRightChild, (*pdrspqdrspqcrInput)[1]);

	CColRef2dArray *pdrspqdrspqcrInputNew = SPQOS_NEW(mp) CColRef2dArray(mp);
	CColRefArray *pdrspqcrLeftNew =
		CUtils::PdrspqcrExactCopy(mp, (*pdrspqdrspqcrInput)[0]);
	pdrspqcrLeftNew->Append(CXformUtils::PcrProjectElement(
		pexprLeftWindow, 0 /* row_number window function*/));

	CColRefArray *pdrspqcrRightNew =
		CUtils::PdrspqcrExactCopy(mp, (*pdrspqdrspqcrInput)[1]);
	pdrspqcrRightNew->Append(CXformUtils::PcrProjectElement(
		pexprRightWindow, 0 /* row_number window function*/));

	pdrspqdrspqcrInputNew->Append(pdrspqcrLeftNew);
	pdrspqdrspqcrInputNew->Append(pdrspqcrRightNew);

	CExpression *pexprScCond =
		CUtils::PexprConjINDFCond(mp, pdrspqdrspqcrInputNew);

	// assemble the new logical operator
	CExpression *pexprLSJ = SPQOS_NEW(mp)
		CExpression(mp, SPQOS_NEW(mp) CLogicalLeftSemiJoin(mp, CXform::ExfIntersectAll2LeftSemiJoin), pexprLeftWindow,
					pexprRightWindow, pexprScCond);

	// clean up
	pdrspqdrspqcrInputNew->Release();

	pxfres->Add(pexprLSJ);
}

// EOF
