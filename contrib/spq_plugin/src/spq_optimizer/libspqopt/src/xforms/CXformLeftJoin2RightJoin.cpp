//---------------------------------------------------------------------------
//	Greenplum Database
//  Copyright (c) 2020 VMware, Inc.
//
//	@filename:
//		CXformLeftJoin2RightJoin.cpp
//
//	@doc:
//		Implementation of transform
//---------------------------------------------------------------------------

#include "spqopt/xforms/CXformLeftJoin2RightJoin.h"

#include "spqos/base.h"
#include "spqos/memory/CAutoMemoryPool.h"

#include "spqopt/operators/CLogicalLeftOuterJoin.h"
#include "spqopt/operators/CLogicalRightOuterJoin.h"
#include "spqopt/operators/CPatternLeaf.h"
#include "spqopt/operators/CPredicateUtils.h"
#include "spqopt/operators/CScalarProjectList.h"



using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CXformLeftJoin2RightJoin::CXformLeftJoin2RightJoin
//
//	@doc:
//		ctor
//
//---------------------------------------------------------------------------
CXformLeftJoin2RightJoin::CXformLeftJoin2RightJoin(CMemoryPool *mp)
	:  // pattern
	  CXformExploration(
		  // pattern
		  SPQOS_NEW(mp) CExpression(
			  mp, SPQOS_NEW(mp) CLogicalLeftOuterJoin(mp),
			  SPQOS_NEW(mp)
				  CExpression(mp, SPQOS_NEW(mp) CPatternLeaf(mp)),  // left child
			  SPQOS_NEW(mp) CExpression(
				  mp, SPQOS_NEW(mp) CPatternLeaf(mp)),  // right child
			  SPQOS_NEW(mp) CExpression(
				  mp, SPQOS_NEW(mp) CPatternTree(mp))  // predicate tree
			  ))
{
}


//---------------------------------------------------------------------------
//	@function:
//		CXformLeftJoin2RightJoin::Exfp
//
//	@doc:
//		Compute xform promise for a given expression handle;
//
//---------------------------------------------------------------------------
CXform::EXformPromise
CXformLeftJoin2RightJoin::Exfp(CExpressionHandle &exprhdl) const
{
	// if scalar predicate has a subquery, we must have an
	// equivalent logical Apply expression created during exploration;
	// no need for generating a physical join
	if (exprhdl.DeriveHasSubquery(2))
	{
		return CXform::ExfpNone;
	}

	return CXform::ExfpHigh;
}


//---------------------------------------------------------------------------
//	@function:
//		CXformLeftJoin2RightJoin::Transform
//
//	@doc:
//		actual transformation
//
//---------------------------------------------------------------------------
void
CXformLeftJoin2RightJoin::Transform(CXformContext *pxfctxt,
									CXformResult *pxfres,
									CExpression *pexpr) const
{
	SPQOS_ASSERT(NULL != pxfctxt);
	SPQOS_ASSERT(FPromising(pxfctxt->Pmp(), this, pexpr));
	SPQOS_ASSERT(FCheckPattern(pexpr));

	CMemoryPool *mp = pxfctxt->Pmp();

	// A ROJ is identical to a LOJ except the children are swapped
	CExpression *pexprOuter = (*pexpr)[0];
	CExpression *pexprInner = (*pexpr)[1];
	CExpression *pexprScalar = (*pexpr)[2];
	pexprOuter->AddRef();
	pexprInner->AddRef();
	pexprScalar->AddRef();

	CExpression *pexprRightJoin =
		CUtils::PexprLogicalJoin<CLogicalRightOuterJoin>(
			mp, pexprInner, pexprOuter, pexprScalar);
	pxfres->Add(pexprRightJoin);
}


// EOF
