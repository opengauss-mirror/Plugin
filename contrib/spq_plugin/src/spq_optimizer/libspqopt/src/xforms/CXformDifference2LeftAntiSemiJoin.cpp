//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CXformDifference2LeftAntiSemiJoin.cpp
//
//	@doc:
//		Implementation of the transformation that takes a logical difference and
//		converts it into an aggregate over a left anti-semi join
//---------------------------------------------------------------------------

#include "spqopt/xforms/CXformDifference2LeftAntiSemiJoin.h"

#include "spqos/base.h"

#include "spqopt/exception.h"
#include "spqopt/operators/CLogicalDifference.h"
#include "spqopt/operators/CLogicalLeftAntiSemiJoin.h"
#include "spqopt/operators/CPatternMultiLeaf.h"
#include "spqopt/operators/CScalarProjectList.h"
#include "spqopt/xforms/CXformUtils.h"

using namespace spqopt;

//---------------------------------------------------------------------------
//	@function:
//		CXformDifference2LeftAntiSemiJoin::CXformDifference2LeftAntiSemiJoin
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CXformDifference2LeftAntiSemiJoin::CXformDifference2LeftAntiSemiJoin(
	CMemoryPool *mp)
	:  // pattern
	  CXformExploration(SPQOS_NEW(mp) CExpression(
		  mp, SPQOS_NEW(mp) CLogicalDifference(mp),
		  SPQOS_NEW(mp) CExpression(mp, SPQOS_NEW(mp) CPatternMultiLeaf(mp))))
{
}

//---------------------------------------------------------------------------
//	@function:
//		CXformDifference2LeftAntiSemiJoin::Transform
//
//	@doc:
//		Actual transformation
//
//---------------------------------------------------------------------------
void
CXformDifference2LeftAntiSemiJoin::Transform(CXformContext *pxfctxt,
											 CXformResult *pxfres,
											 CExpression *pexpr) const
{
	SPQOS_ASSERT(NULL != pxfctxt);
	SPQOS_ASSERT(FPromising(pxfctxt->Pmp(), this, pexpr));
	SPQOS_ASSERT(FCheckPattern(pexpr));

	CMemoryPool *mp = pxfctxt->Pmp();

	// TODO: Oct 24th 2012, we currently only handle difference all
	//  operators with two children
	SPQOS_ASSERT(2 == pexpr->Arity());

	// extract components
	CExpression *pexprLeftChild = (*pexpr)[0];
	CExpression *pexprRightChild = (*pexpr)[1];

	CLogicalDifference *popDifference =
		CLogicalDifference::PopConvert(pexpr->Pop());
	CColRefArray *pdrspqcrOutput = popDifference->PdrspqcrOutput();
	CColRef2dArray *pdrspqdrspqcrInput = popDifference->PdrspqdrspqcrInput();

	// generate the scalar condition for the left anti-semi join
	CExpression *pexprScCond = CUtils::PexprConjINDFCond(mp, pdrspqdrspqcrInput);

	pexprLeftChild->AddRef();
	pexprRightChild->AddRef();

	// assemble the new left anti-semi join logical operator
	CExpression *pexprLASJ =
		SPQOS_NEW(mp) CExpression(mp, SPQOS_NEW(mp) CLogicalLeftAntiSemiJoin(mp, CXform::ExfDifference2LeftAntiSemiJoin),
								 pexprLeftChild, pexprRightChild, pexprScCond);

	// assemble the aggregate operator
	pdrspqcrOutput->AddRef();

	CExpression *pexprProjList =
		SPQOS_NEW(mp) CExpression(mp, SPQOS_NEW(mp) CScalarProjectList(mp),
								 SPQOS_NEW(mp) CExpressionArray(mp));

	CExpression *pexprAgg = CUtils::PexprLogicalGbAggGlobal(
		mp, pdrspqcrOutput, pexprLASJ, pexprProjList);

	// add alternative to results
	pxfres->Add(pexprAgg);
}

// EOF
