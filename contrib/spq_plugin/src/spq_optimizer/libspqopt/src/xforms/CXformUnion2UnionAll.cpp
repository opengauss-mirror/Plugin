//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CXformUnion2UnionAll.cpp
//
//	@doc:
//		Implementation of the transformation that takes a logical union and
//		coverts it into an aggregate over a logical union all
//---------------------------------------------------------------------------

#include "spqopt/xforms/CXformUnion2UnionAll.h"

#include "spqos/base.h"

#include "spqopt/operators/CLogicalGbAgg.h"
#include "spqopt/operators/CLogicalUnion.h"
#include "spqopt/operators/CLogicalUnionAll.h"
#include "spqopt/operators/CPatternMultiLeaf.h"
#include "spqopt/operators/CScalarProjectList.h"

using namespace spqopt;

//---------------------------------------------------------------------------
//	@function:
//		CXformUnion2UnionAll::CXformUnion2UnionAll
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CXformUnion2UnionAll::CXformUnion2UnionAll(CMemoryPool *mp)
	:  // pattern
	  CXformExploration(SPQOS_NEW(mp) CExpression(
		  mp, SPQOS_NEW(mp) CLogicalUnion(mp),
		  SPQOS_NEW(mp) CExpression(mp, SPQOS_NEW(mp) CPatternMultiLeaf(mp))))
{
}

//---------------------------------------------------------------------------
//	@function:
//		CXformUnion2UnionAll::Transform
//
//	@doc:
//		Actual transformation
//
//---------------------------------------------------------------------------
void
CXformUnion2UnionAll::Transform(CXformContext *pxfctxt, CXformResult *pxfres,
								CExpression *pexpr) const
{
	SPQOS_ASSERT(NULL != pxfctxt);
	SPQOS_ASSERT(FPromising(pxfctxt->Pmp(), this, pexpr));
	SPQOS_ASSERT(FCheckPattern(pexpr));

	CMemoryPool *mp = pxfctxt->Pmp();

	// extract components
	CLogicalUnion *popUnion = CLogicalUnion::PopConvert(pexpr->Pop());
	CColRefArray *pdrspqcrOutput = popUnion->PdrspqcrOutput();
	CColRef2dArray *pdrspqdrspqcrInput = popUnion->PdrspqdrspqcrInput();

	CExpressionArray *pdrspqexpr = SPQOS_NEW(mp) CExpressionArray(mp);
	const ULONG arity = pexpr->Arity();

	for (ULONG ul = 0; ul < arity; ul++)
	{
		CExpression *pexprChild = (*pexpr)[ul];
		pexprChild->AddRef();
		pdrspqexpr->Append(pexprChild);
	}

	pdrspqcrOutput->AddRef();
	pdrspqdrspqcrInput->AddRef();

	// assemble new logical operator
	CExpression *pexprUnionAll = SPQOS_NEW(mp) CExpression(
		mp, SPQOS_NEW(mp) CLogicalUnionAll(mp, pdrspqcrOutput, pdrspqdrspqcrInput),
		pdrspqexpr);

	pdrspqcrOutput->AddRef();

	CExpression *pexprProjList =
		SPQOS_NEW(mp) CExpression(mp, SPQOS_NEW(mp) CScalarProjectList(mp),
								 SPQOS_NEW(mp) CExpressionArray(mp));

	CExpression *pexprAgg = SPQOS_NEW(mp) CExpression(
		mp,
		SPQOS_NEW(mp) CLogicalGbAgg(mp, pdrspqcrOutput,
								   COperator::EgbaggtypeGlobal /*egbaggtype*/),
		pexprUnionAll, pexprProjList);

	// add alternative to results
	pxfres->Add(pexprAgg);
}

// EOF
