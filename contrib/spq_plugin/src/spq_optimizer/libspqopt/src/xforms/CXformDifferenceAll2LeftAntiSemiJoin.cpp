//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 EMC Corp.
//
//	@filename:
//		CXformDifferenceAll2LeftAntiSemiJoin.cpp
//
//	@doc:
//		Implementation of the transformation a logical difference all into LASJ
//---------------------------------------------------------------------------

#include "spqopt/xforms/CXformDifferenceAll2LeftAntiSemiJoin.h"

#include "spqos/base.h"

#include "spqopt/exception.h"
#include "spqopt/operators/CLogicalDifferenceAll.h"
#include "spqopt/operators/CLogicalLeftAntiSemiJoin.h"
#include "spqopt/operators/CPatternMultiLeaf.h"
#include "spqopt/xforms/CXformIntersectAll2LeftSemiJoin.h"
#include "spqopt/xforms/CXformUtils.h"

using namespace spqopt;

//---------------------------------------------------------------------------
//	@function:
//		CXformDifferenceAll2LeftAntiSemiJoin::CXformDifferenceAll2LeftAntiSemiJoin
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CXformDifferenceAll2LeftAntiSemiJoin::CXformDifferenceAll2LeftAntiSemiJoin(
	CMemoryPool *mp)
	:  // pattern
	  CXformExploration(SPQOS_NEW(mp) CExpression(
		  mp, SPQOS_NEW(mp) CLogicalDifferenceAll(mp),
		  SPQOS_NEW(mp) CExpression(mp, SPQOS_NEW(mp) CPatternMultiLeaf(mp))))
{
}

//---------------------------------------------------------------------------
//	@function:
//		CXformDifferenceAll2LeftAntiSemiJoin::Transform
//
//	@doc:
//		Actual transformation
//
//---------------------------------------------------------------------------
void
CXformDifferenceAll2LeftAntiSemiJoin::Transform(CXformContext *pxfctxt,
												CXformResult *pxfres,
												CExpression *pexpr) const
{
	SPQOS_ASSERT(NULL != pxfctxt);
	SPQOS_ASSERT(FPromising(pxfctxt->Pmp(), this, pexpr));
	SPQOS_ASSERT(FCheckPattern(pexpr));

	CMemoryPool *mp = pxfctxt->Pmp();

	// TODO: , Jan 8th 2013, we currently only handle difference all
	//  operators with two children
	SPQOS_ASSERT(2 == pexpr->Arity());

	// extract components
	CExpression *pexprLeftChild = (*pexpr)[0];
	CExpression *pexprRightChild = (*pexpr)[1];

	CLogicalDifferenceAll *popDifferenceAll =
		CLogicalDifferenceAll::PopConvert(pexpr->Pop());
	CColRef2dArray *pdrspqdrspqcrInput = popDifferenceAll->PdrspqdrspqcrInput();

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

	// generate the scalar condition for the left anti-semi join
	CExpression *pexprScCond =
		CUtils::PexprConjINDFCond(mp, pdrspqdrspqcrInputNew);

	// assemble the new left anti-semi join logical operator
	CExpression *pexprLASJ = SPQOS_NEW(mp)
		CExpression(mp, SPQOS_NEW(mp) CLogicalLeftAntiSemiJoin(mp, CXform::ExfDifferenceAll2LeftAntiSemiJoin),
					pexprLeftWindow, pexprRightWindow, pexprScCond);

	// clean up
	pdrspqdrspqcrInputNew->Release();

	// add alternative to results
	pxfres->Add(pexprLASJ);
}

// EOF
