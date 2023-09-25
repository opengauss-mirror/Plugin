//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 EMC Corp.
//
//	@filename:
//		CXformExpandNAryJoinDP.cpp
//
//	@doc:
//		Implementation of n-ary join expansion using dynamic programming
//---------------------------------------------------------------------------

#include "spqopt/xforms/CXformExpandNAryJoinDP.h"

#include "spqos/base.h"

#include "spqopt/base/CUtils.h"
#include "spqopt/engine/CHint.h"
#include "spqopt/operators/CLogicalNAryJoin.h"
#include "spqopt/operators/CNormalizer.h"
#include "spqopt/operators/CPatternMultiLeaf.h"
#include "spqopt/operators/CPatternMultiTree.h"
#include "spqopt/operators/CPredicateUtils.h"
#include "spqopt/optimizer/COptimizerConfig.h"
#include "spqopt/xforms/CJoinOrderDP.h"
#include "spqopt/xforms/CXformUtils.h"



using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CXformExpandNAryJoinDP::CXformExpandNAryJoinDP
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CXformExpandNAryJoinDP::CXformExpandNAryJoinDP(CMemoryPool *mp)
	: CXformExploration(
		  // pattern
		  SPQOS_NEW(mp) CExpression(
			  mp, SPQOS_NEW(mp) CLogicalNAryJoin(mp),
			  SPQOS_NEW(mp) CExpression(mp, SPQOS_NEW(mp) CPatternMultiLeaf(mp)),
			  SPQOS_NEW(mp) CExpression(mp, SPQOS_NEW(mp) CPatternTree(mp))))
{
}


//---------------------------------------------------------------------------
//	@function:
//		CXformExpandNAryJoinDP::Exfp
//
//	@doc:
//		Compute xform promise for a given expression handle
//
//---------------------------------------------------------------------------
CXform::EXformPromise
CXformExpandNAryJoinDP::Exfp(CExpressionHandle &exprhdl) const
{
	COptimizerConfig *optimizer_config =
		COptCtxt::PoctxtFromTLS()->GetOptimizerConfig();
	const CHint *phint = optimizer_config->GetHint();

	const ULONG arity = exprhdl.Arity();

	// since the last child of the join operator is a scalar child
	// defining the join predicate, ignore it.
	const ULONG ulRelChild = arity - 1;

	if (ulRelChild > phint->UlJoinOrderDPLimit())
	{
		return CXform::ExfpNone;
	}

	return CXformUtils::ExfpExpandJoinOrder(exprhdl, this);
}


//---------------------------------------------------------------------------
//	@function:
//		CXformExpandNAryJoinDP::Transform
//
//	@doc:
//		Actual transformation of n-ary join to cluster of inner joins using
//		dynamic programming
//
//---------------------------------------------------------------------------
void
CXformExpandNAryJoinDP::Transform(CXformContext *pxfctxt, CXformResult *pxfres,
								  CExpression *pexpr) const
{
	SPQOS_ASSERT(NULL != pxfctxt);
	SPQOS_ASSERT(NULL != pxfres);
	SPQOS_ASSERT(FPromising(pxfctxt->Pmp(), this, pexpr));
	SPQOS_ASSERT(FCheckPattern(pexpr));

	CMemoryPool *mp = pxfctxt->Pmp();

	const ULONG arity = pexpr->Arity();
	SPQOS_ASSERT(arity >= 3);

	CExpressionArray *pdrspqexpr = SPQOS_NEW(mp) CExpressionArray(mp);
	for (ULONG ul = 0; ul < arity - 1; ul++)
	{
		CExpression *pexprChild = (*pexpr)[ul];
		pexprChild->AddRef();
		pdrspqexpr->Append(pexprChild);
	}

	CExpression *pexprScalar = (*pexpr)[arity - 1];
	CExpressionArray *pdrspqexprPreds =
		CPredicateUtils::PdrspqexprConjuncts(mp, pexprScalar);

	// create join order using dynamic programming
	CJoinOrderDP jodp(mp, pdrspqexpr, pdrspqexprPreds);
	CExpression *pexprResult = jodp.PexprExpand();

	if (NULL != pexprResult)
	{
		// normalize resulting expression
		CExpression *pexprNormalized =
			CNormalizer::PexprNormalize(mp, pexprResult);
		pexprResult->Release();
		pxfres->Add(pexprNormalized);

		const ULONG UlTopKJoinOrders = jodp.PdrspqexprTopK()->Size();
		for (ULONG ul = 0; ul < UlTopKJoinOrders; ul++)
		{
			CExpression *pexprJoinOrder = (*jodp.PdrspqexprTopK())[ul];
			if (pexprJoinOrder != pexprResult)
			{
				// We should consider normalizing this expression before inserting it, as we do for pexprResult
				pexprJoinOrder->AddRef();
				pxfres->Add(pexprJoinOrder);
			}
		}
	}
}

// EOF
