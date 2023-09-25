//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 Pivotal Inc.
//
//	@filename:
//		CXformIntersect2Join.cpp
//
//	@doc:
//		Implement the transformation of Intersect into a Join
//---------------------------------------------------------------------------

#include "spqopt/xforms/CXformIntersect2Join.h"

#include "spqos/base.h"

#include "spqopt/base/CColRefComputed.h"
#include "spqopt/base/CUtils.h"
#include "spqopt/exception.h"
#include "spqopt/operators/CLogicalIntersect.h"
#include "spqopt/operators/COperator.h"
#include "spqopt/operators/CPatternLeaf.h"
#include "spqopt/translate/CTranslatorDXLToExpr.h"
#include "spqopt/xforms/CXformUtils.h"

using namespace spqmd;
using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CXformIntersect2Join::CXformIntersect2Join
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CXformIntersect2Join::CXformIntersect2Join(CMemoryPool *mp)
	: CXformExploration(
		  // pattern
		  SPQOS_NEW(mp) CExpression(
			  mp, SPQOS_NEW(mp) CLogicalIntersect(mp),
			  SPQOS_NEW(mp) CExpression(
				  mp, SPQOS_NEW(mp) CPatternLeaf(mp)),  // left relational child
			  SPQOS_NEW(mp) CExpression(
				  mp, SPQOS_NEW(mp) CPatternLeaf(mp))  // right relational child
			  ))
{
}

//---------------------------------------------------------------------------
//	@function:
//		CXformIntersect2Join::Transform
//
//	@doc:
//		Actual transformation that transforms an intersect into a join
//		over a group by over the inputs
//
//---------------------------------------------------------------------------
void
CXformIntersect2Join::Transform(CXformContext *pxfctxt, CXformResult *pxfres,
								CExpression *pexpr) const
{
	SPQOS_ASSERT(NULL != pxfctxt);
	SPQOS_ASSERT(NULL != pxfres);
	SPQOS_ASSERT(FPromising(pxfctxt->Pmp(), this, pexpr));
	SPQOS_ASSERT(FCheckPattern(pexpr));

	CMemoryPool *mp = pxfctxt->Pmp();

	SPQOS_ASSERT(2 == pexpr->Arity());

	// extract components
	CExpression *pexprLeftChild = (*pexpr)[0];
	CExpression *pexprRightChild = (*pexpr)[1];

	CLogicalIntersect *popIntersect =
		CLogicalIntersect::PopConvert(pexpr->Pop());
	CColRef2dArray *pdrspqdrspqcrInput = popIntersect->PdrspqdrspqcrInput();

	// construct group by over the left and right expressions

	CExpression *pexprProjList =
		SPQOS_NEW(mp) CExpression(mp, SPQOS_NEW(mp) CScalarProjectList(mp),
								 SPQOS_NEW(mp) CExpressionArray(mp));
	pexprProjList->AddRef();
	pexprLeftChild->AddRef();
	pexprRightChild->AddRef();
	(*pdrspqdrspqcrInput)[0]->AddRef();
	(*pdrspqdrspqcrInput)[1]->AddRef();

	CExpression *pexprLeftAgg = CUtils::PexprLogicalGbAggGlobal(
		mp, (*pdrspqdrspqcrInput)[0], pexprLeftChild, pexprProjList);
	CExpression *pexprRightAgg = CUtils::PexprLogicalGbAggGlobal(
		mp, (*pdrspqdrspqcrInput)[1], pexprRightChild, pexprProjList);

	CExpression *pexprScCond = CUtils::PexprConjINDFCond(mp, pdrspqdrspqcrInput);

	CExpression *pexprJoin =
		SPQOS_NEW(mp) CExpression(mp, SPQOS_NEW(mp) CLogicalInnerJoin(mp, CXform::ExfIntersect2Join),
								 pexprLeftAgg, pexprRightAgg, pexprScCond);


	pxfres->Add(pexprJoin);
}

// EOF
