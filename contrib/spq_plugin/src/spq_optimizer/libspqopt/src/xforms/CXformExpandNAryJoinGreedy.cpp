//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2018 Pivotal Software Inc.
//
//	@filename:
//		CXformExpandNAryJoinGreedy.cpp
//
//	@doc:
//		Implementation of n-ary join expansion based on cardinality
//		of intermediate results and delay cross joins to
//		the end
//---------------------------------------------------------------------------

#include "spqopt/xforms/CXformExpandNAryJoinGreedy.h"

#include "spqos/base.h"

#include "spqopt/base/CUtils.h"
#include "spqopt/operators/CExpressionUtils.h"
#include "spqopt/operators/CLogicalNAryJoin.h"
#include "spqopt/operators/CNormalizer.h"
#include "spqopt/operators/CPatternMultiTree.h"
#include "spqopt/operators/CPredicateUtils.h"
#include "spqopt/xforms/CJoinOrderGreedy.h"
#include "spqopt/xforms/CXformUtils.h"


using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CXformExpandNAryJoinGreedy::CXformExpandNAryJoinGreedy
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CXformExpandNAryJoinGreedy::CXformExpandNAryJoinGreedy(CMemoryPool *pmp)
	: CXformExploration(
		  // pattern
		  SPQOS_NEW(pmp) CExpression(
			  pmp, SPQOS_NEW(pmp) CLogicalNAryJoin(pmp),
			  SPQOS_NEW(pmp)
				  CExpression(pmp, SPQOS_NEW(pmp) CPatternMultiTree(pmp)),
			  SPQOS_NEW(pmp) CExpression(pmp, SPQOS_NEW(pmp) CPatternTree(pmp))))
{
}


//---------------------------------------------------------------------------
//	@function:
//		CXformExpandNAryJoinGreedy::Exfp
//
//	@doc:
//		Compute xform promise for a given expression handle
//
//---------------------------------------------------------------------------
CXform::EXformPromise
CXformExpandNAryJoinGreedy::Exfp(CExpressionHandle &exprhdl) const
{
	return CXformUtils::ExfpExpandJoinOrder(exprhdl, this);
}


//---------------------------------------------------------------------------
//	@function:
//		CXformExpandNAryJoinGreedy::Transform
//
//	@doc:
//		Actual transformation of n-ary join to cluster of inner joins
//
//---------------------------------------------------------------------------
void
CXformExpandNAryJoinGreedy::Transform(CXformContext *pxfctxt,
									  CXformResult *pxfres,
									  CExpression *pexpr) const
{
	SPQOS_ASSERT(NULL != pxfctxt);
	SPQOS_ASSERT(NULL != pxfres);
	SPQOS_ASSERT(FPromising(pxfctxt->Pmp(), this, pexpr));
	SPQOS_ASSERT(FCheckPattern(pexpr));

	CMemoryPool *pmp = pxfctxt->Pmp();

	const ULONG ulArity = pexpr->Arity();
	SPQOS_ASSERT(ulArity >= 3);

	CExpressionArray *pdrspqexpr = SPQOS_NEW(pmp) CExpressionArray(pmp);
	for (ULONG ul = 0; ul < ulArity - 1; ul++)
	{
		CExpression *pexprChild = (*pexpr)[ul];
		pexprChild->AddRef();
		pdrspqexpr->Append(pexprChild);
	}

	CExpression *pexprScalar = (*pexpr)[ulArity - 1];
	CExpressionArray *pdrspqexprPreds =
		CPredicateUtils::PdrspqexprConjuncts(pmp, pexprScalar);

	// create a join order based on cardinality of intermediate results
	CJoinOrderGreedy jomc(pmp, pdrspqexpr, pdrspqexprPreds);
	CExpression *pexprResult = jomc.PexprExpand();

	if (NULL != pexprResult)
	{
		// normalize resulting expression
		CExpression *pexprNormalized =
			CNormalizer::PexprNormalize(pmp, pexprResult);
		pexprResult->Release();
		pxfres->Add(pexprNormalized);
	}
}

// EOF
