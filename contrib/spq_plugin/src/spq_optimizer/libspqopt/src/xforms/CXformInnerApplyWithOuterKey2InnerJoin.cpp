//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CXformInnerApplyWithOuterKey2InnerJoin.cpp
//
//	@doc:
//		Implementation of transform
//---------------------------------------------------------------------------

#include "spqopt/xforms/CXformInnerApplyWithOuterKey2InnerJoin.h"

#include "spqos/base.h"

#include "spqopt/base/CKeyCollection.h"
#include "spqopt/operators/CLogicalGbAgg.h"
#include "spqopt/operators/CLogicalInnerApply.h"
#include "spqopt/operators/CLogicalInnerJoin.h"
#include "spqopt/operators/CPatternLeaf.h"
#include "spqopt/operators/CPredicateUtils.h"

using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CXformInnerApplyWithOuterKey2InnerJoin::CXformInnerApplyWithOuterKey2InnerJoin
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CXformInnerApplyWithOuterKey2InnerJoin::CXformInnerApplyWithOuterKey2InnerJoin(
	CMemoryPool *mp)
	: CXformExploration(
		  // pattern
		  SPQOS_NEW(mp) CExpression(
			  mp, SPQOS_NEW(mp) CLogicalInnerApply(mp),
			  SPQOS_NEW(mp)
				  CExpression(mp, SPQOS_NEW(mp) CPatternLeaf(mp)),  // left child
			  SPQOS_NEW(mp) CExpression(
				  mp, SPQOS_NEW(mp) CLogicalGbAgg(mp),
				  SPQOS_NEW(mp) CExpression(
					  mp,
					  SPQOS_NEW(mp) CPatternTree(mp)),  // relational child of Gb
				  SPQOS_NEW(mp) CExpression(
					  mp, SPQOS_NEW(mp)
							  CPatternLeaf(mp))	 // scalar project list of Gb
				  ),							 // right child
			  SPQOS_NEW(mp) CExpression(
				  mp, SPQOS_NEW(mp) CPatternLeaf(mp))  // Apply predicate
			  ))
{
}



//---------------------------------------------------------------------------
//	@function:
//		CXformInnerApplyWithOuterKey2InnerJoin::Exfp
//
//	@doc:
//		Compute xform promise for a given expression handle;
//
//---------------------------------------------------------------------------
CXform::EXformPromise
CXformInnerApplyWithOuterKey2InnerJoin::Exfp(CExpressionHandle &exprhdl) const
{
	// check if outer child has key and inner child has outer references
	if (NULL == exprhdl.DeriveKeyCollection(0) ||
		0 == exprhdl.DeriveOuterReferences(1)->Size())
	{
		return ExfpNone;
	}

	return ExfpHigh;
}


//---------------------------------------------------------------------------
//	@function:
//		CXformInnerApplyWithOuterKey2InnerJoin::Transform
//
//	@doc:
//		Actual transformation
//
//---------------------------------------------------------------------------
void
CXformInnerApplyWithOuterKey2InnerJoin::Transform(CXformContext *pxfctxt,
												  CXformResult *pxfres,
												  CExpression *pexpr) const
{
	SPQOS_ASSERT(NULL != pxfctxt);
	SPQOS_ASSERT(FPromising(pxfctxt->Pmp(), this, pexpr));
	SPQOS_ASSERT(FCheckPattern(pexpr));

	CMemoryPool *mp = pxfctxt->Pmp();

	// extract components
	CExpression *pexprOuter = (*pexpr)[0];
	CExpression *pexprGb = (*pexpr)[1];
	CExpression *pexprScalar = (*pexpr)[2];

	if (0 < CLogicalGbAgg::PopConvert(pexprGb->Pop())->Pdrspqcr()->Size())
	{
		// xform is not applicable if inner Gb has grouping columns
		return;
	}

	if (CUtils::FHasSubqueryOrApply((*pexprGb)[0]))
	{
		// Subquery/Apply must be unnested before reaching here
		return;
	}

	// decorrelate Gb's relational child
	(*pexprGb)[0]->ResetDerivedProperties();
	CExpression *pexprInner = NULL;
	CExpressionArray *pdrspqexpr = SPQOS_NEW(mp) CExpressionArray(mp);
	if (!CDecorrelator::FProcess(mp, (*pexprGb)[0], false /*fEqualityOnly*/,
								 &pexprInner, pdrspqexpr,
								 pexprOuter->DeriveOutputColumns()))
	{
		pdrspqexpr->Release();
		return;
	}

	SPQOS_ASSERT(NULL != pexprInner);
	CExpression *pexprPredicate =
		CPredicateUtils::PexprConjunction(mp, pdrspqexpr);

	// join outer child with Gb's decorrelated child
	pexprOuter->AddRef();
	CExpression *pexprInnerJoin =
		SPQOS_NEW(mp) CExpression(mp, SPQOS_NEW(mp) CLogicalInnerJoin(mp),
								 pexprOuter, pexprInner, pexprPredicate);

	// create grouping columns from the output of outer child
	CColRefArray *pdrspqcrKey = NULL;
	CColRefArray *colref_array =
		CUtils::PdrspqcrGroupingKey(mp, pexprOuter, &pdrspqcrKey);
	pdrspqcrKey->Release();	// key is not used here

	CLogicalGbAgg *popGbAgg = SPQOS_NEW(mp) CLogicalGbAgg(
		mp, colref_array, COperator::EgbaggtypeGlobal /*egbaggtype*/);
	CExpression *pexprPrjList = (*pexprGb)[1];
	pexprPrjList->AddRef();
	CExpression *pexprNewGb =
		SPQOS_NEW(mp) CExpression(mp, popGbAgg, pexprInnerJoin, pexprPrjList);

	// add Apply predicate in a top Select node
	pexprScalar->AddRef();
	CExpression *pexprSelect =
		CUtils::PexprLogicalSelect(mp, pexprNewGb, pexprScalar);

	pxfres->Add(pexprSelect);
}


// EOF
