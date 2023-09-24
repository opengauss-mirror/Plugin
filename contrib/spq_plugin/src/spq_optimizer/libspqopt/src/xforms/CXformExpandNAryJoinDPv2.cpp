//---------------------------------------------------------------------------
// Greenplum Database
// Copyright (C) 2019 Pivotal Inc.
//
//	@filename:
//		CXformExpandNAryJoinDPv2.cpp
//
//	@doc:
//		Implementation of n-ary join expansion using dynamic programming
//---------------------------------------------------------------------------

#include "spqopt/xforms/CXformExpandNAryJoinDPv2.h"

#include "spqos/base.h"

#include "spqopt/base/CUtils.h"
#include "spqopt/engine/CHint.h"
#include "spqopt/operators/CLogicalNAryJoin.h"
#include "spqopt/operators/CNormalizer.h"
#include "spqopt/operators/CPatternMultiLeaf.h"
#include "spqopt/operators/CPatternTree.h"
#include "spqopt/operators/CPredicateUtils.h"
#include "spqopt/operators/CScalarNAryJoinPredList.h"
#include "spqopt/optimizer/COptimizerConfig.h"
#include "spqopt/xforms/CJoinOrderDPv2.h"
#include "spqopt/xforms/CXformUtils.h"



using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CXformExpandNAryJoinDPv2::CXformExpandNAryJoinDPv2
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CXformExpandNAryJoinDPv2::CXformExpandNAryJoinDPv2(CMemoryPool *mp)
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
//		CXformExpandNAryJoinDPv2::Exfp
//
//	@doc:
//		Compute xform promise for a given expression handle
//
//---------------------------------------------------------------------------
CXform::EXformPromise
CXformExpandNAryJoinDPv2::Exfp(CExpressionHandle &exprhdl) const
{
	return CXformUtils::ExfpExpandJoinOrder(exprhdl, this);
}


//---------------------------------------------------------------------------
//	@function:
//		CXformExpandNAryJoinDPv2::Transform
//
//	@doc:
//		Actual transformation of n-ary join to cluster of inner joins using
//		dynamic programming
//
//---------------------------------------------------------------------------
void
CXformExpandNAryJoinDPv2::Transform(CXformContext *pxfctxt,
									CXformResult *pxfres,
									CExpression *pexpr) const
{
	SPQOS_ASSERT(NULL != pxfctxt);
	SPQOS_ASSERT(NULL != pxfres);
	SPQOS_ASSERT(FPromising(pxfctxt->Pmp(), this, pexpr));
	SPQOS_ASSERT(FCheckPattern(pexpr));

	CMemoryPool *mp = pxfctxt->Pmp();

	const ULONG arity = pexpr->Arity();
	SPQOS_ASSERT(arity >= 3);

	// Make an expression array with all the atoms (the logical children)
	CExpressionArray *pdrspqexpr = SPQOS_NEW(mp) CExpressionArray(mp);
	for (ULONG ul = 0; ul < arity - 1; ul++)
	{
		CExpression *pexprChild = (*pexpr)[ul];
		pexprChild->AddRef();
		pdrspqexpr->Append(pexprChild);
	}

	// Make an expression array with all the join predicates, one entry for
	// every conjunct (ANDed condition),
	// plus an array of all the non-inner join predicates, together with
	// a lookup table for each child whether it is a non-inner join
	CLogicalNAryJoin *naryJoin = CLogicalNAryJoin::PopConvert(pexpr->Pop());
	CExpression *pexprScalar = (*pexpr)[arity - 1];
	CExpressionArray *innerJoinPreds = NULL;
	CExpressionArray *onPreds = SPQOS_NEW(mp) CExpressionArray(mp);
	ULongPtrArray *childPredIndexes = NULL;

	if (NULL != CScalarNAryJoinPredList::PopConvert(pexprScalar->Pop()))
	{
		innerJoinPreds =
			CPredicateUtils::PdrspqexprConjuncts(mp, (*pexprScalar)[0]);

		for (ULONG ul = 1; ul < pexprScalar->Arity(); ul++)
		{
			(*pexprScalar)[ul]->AddRef();
			onPreds->Append((*pexprScalar)[ul]);
		}

		childPredIndexes = naryJoin->GetLojChildPredIndexes();
		SPQOS_ASSERT(NULL != childPredIndexes);
		childPredIndexes->AddRef();
	}
	else
	{
		innerJoinPreds = CPredicateUtils::PdrspqexprConjuncts(mp, pexprScalar);
	}

	CColRefSet *outerRefs = pexpr->DeriveOuterReferences();

	outerRefs->AddRef();

	// create join order using dynamic programming v2, record topk results in jodp
	CJoinOrderDPv2 jodp(mp, pdrspqexpr, innerJoinPreds, onPreds,
						childPredIndexes, outerRefs);
	jodp.PexprExpand();

	// Retrieve top K join orders from jodp and add as alternatives
	CExpression *nextJoinOrder = NULL;

	while (NULL != (nextJoinOrder = jodp.GetNextOfTopK()))
	{
		CExpression *pexprNormalized =
			CNormalizer::PexprNormalize(mp, nextJoinOrder);

		nextJoinOrder->Release();
		pxfres->Add(pexprNormalized);
	}
}

// EOF
