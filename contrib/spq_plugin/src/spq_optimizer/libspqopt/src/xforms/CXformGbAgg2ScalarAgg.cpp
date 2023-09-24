//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CXformGbAgg2ScalarAgg.cpp
//
//	@doc:
//		Implementation of transform
//---------------------------------------------------------------------------

#include "spqopt/xforms/CXformGbAgg2ScalarAgg.h"

#include "spqos/base.h"

#include "spqopt/operators/CLogicalGbAgg.h"
#include "spqopt/operators/CPatternLeaf.h"
#include "spqopt/operators/CPhysicalScalarAgg.h"
#include "spqopt/xforms/CXformGbAgg2HashAgg.h"
#include "spqopt/xforms/CXformUtils.h"

using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CXformGbAgg2ScalarAgg::CXformGbAgg2ScalarAgg
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CXformGbAgg2ScalarAgg::CXformGbAgg2ScalarAgg(CMemoryPool *mp)
	: CXformImplementation(
		  // pattern
		  SPQOS_NEW(mp) CExpression(
			  mp, SPQOS_NEW(mp) CLogicalGbAgg(mp),
			  SPQOS_NEW(mp) CExpression(mp, SPQOS_NEW(mp) CPatternLeaf(mp)),
			  SPQOS_NEW(mp) CExpression(mp, SPQOS_NEW(mp) CPatternLeaf(mp))))
{
}


//---------------------------------------------------------------------------
//	@function:
//		CXformGbAgg2ScalarAgg::Exfp
//
//	@doc:
//		Compute xform promise for a given expression handle;
//		grouping columns must be empty
//
//---------------------------------------------------------------------------
CXform::EXformPromise
CXformGbAgg2ScalarAgg::Exfp(CExpressionHandle &exprhdl) const
{
	if (0 < CLogicalGbAgg::PopConvert(exprhdl.Pop())->Pdrspqcr()->Size() ||
		exprhdl.DeriveHasSubquery(1))
	{
		// GbAgg has grouping columns, or agg functions use subquery arguments
		return CXform::ExfpNone;
	}

	return CXform::ExfpHigh;
}


//---------------------------------------------------------------------------
//	@function:
//		CXformGbAgg2ScalarAgg::Transform
//
//	@doc:
//		Actual transformation
//
//---------------------------------------------------------------------------
void
CXformGbAgg2ScalarAgg::Transform(CXformContext *pxfctxt, CXformResult *pxfres,
								 CExpression *pexpr) const
{
	SPQOS_ASSERT(NULL != pxfctxt);
	SPQOS_ASSERT(FPromising(pxfctxt->Pmp(), this, pexpr));
	SPQOS_ASSERT(FCheckPattern(pexpr));

	CLogicalGbAgg *popAgg = CLogicalGbAgg::PopConvert(pexpr->Pop());
	CMemoryPool *mp = pxfctxt->Pmp();
	CColRefArray *colref_array = popAgg->Pdrspqcr();
	colref_array->AddRef();

	// extract components
	CExpression *pexprRel = (*pexpr)[0];
	CExpression *pexprScalar = (*pexpr)[1];

	// addref children
	pexprRel->AddRef();
	pexprScalar->AddRef();

	CColRefArray *pdrspqcrArgDQA = popAgg->PdrspqcrArgDQA();
	if (pdrspqcrArgDQA != NULL && 0 != pdrspqcrArgDQA->Size())
	{
		pdrspqcrArgDQA->AddRef();
	}

	// create alternative expression
	CExpression *pexprAlt = SPQOS_NEW(mp) CExpression(
		mp,
		SPQOS_NEW(mp) CPhysicalScalarAgg(
			mp, colref_array, popAgg->PdrspqcrMinimal(), popAgg->Egbaggtype(),
			popAgg->FGeneratesDuplicates(), pdrspqcrArgDQA,
			CXformUtils::FMultiStageAgg(pexpr),
			CXformUtils::FAggGenBySplitDQAXform(pexpr), popAgg->AggStage(),
			!CXformUtils::FLocalAggCreatedByEagerAggXform(pexpr)),
		pexprRel, pexprScalar);

	// add alternative to transformation result
	pxfres->Add(pexprAlt);
}


// EOF
