//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CXformGbAgg2StreamAgg.cpp
//
//	@doc:
//		Implementation of transform
//---------------------------------------------------------------------------

#include "spqopt/xforms/CXformGbAgg2StreamAgg.h"

#include "spqos/base.h"

#include "spqopt/operators/CLogicalGbAgg.h"
#include "spqopt/operators/CPatternLeaf.h"
#include "spqopt/operators/CPhysicalStreamAgg.h"
#include "spqopt/xforms/CXformGbAgg2HashAgg.h"
#include "spqopt/xforms/CXformUtils.h"
using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CXformGbAgg2StreamAgg::CXformGbAgg2StreamAgg
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CXformGbAgg2StreamAgg::CXformGbAgg2StreamAgg(CMemoryPool *mp)
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
//		CXformGbAgg2StreamAgg::CXformGbAgg2StreamAgg
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CXformGbAgg2StreamAgg::CXformGbAgg2StreamAgg(CExpression *pexprPattern)
	: CXformImplementation(pexprPattern)
{
}

//---------------------------------------------------------------------------
//	@function:
//		CXformGbAgg2StreamAgg::Exfp
//
//	@doc:
//		Compute xform promise for a given expression handle;
//		grouping columns must be non-empty
//
//---------------------------------------------------------------------------
CXform::EXformPromise
CXformGbAgg2StreamAgg::Exfp(CExpressionHandle &exprhdl) const
{
	CLogicalGbAgg *popAgg = CLogicalGbAgg::PopConvert(exprhdl.Pop());
	if (0 == popAgg->Pdrspqcr()->Size() ||
		!CUtils::FComparisonPossible(popAgg->Pdrspqcr(), IMDType::EcmptL) ||
		exprhdl.DeriveHasSubquery(1))
	{
		// no grouping columns, or no sort operators are available for grouping columns, or
		// agg functions use subquery arguments
		return CXform::ExfpNone;
	}

	return CXform::ExfpHigh;
}


//---------------------------------------------------------------------------
//	@function:
//		CXformGbAgg2StreamAgg::Transform
//
//	@doc:
//		Actual transformation
//
//---------------------------------------------------------------------------
void
CXformGbAgg2StreamAgg::Transform(CXformContext *pxfctxt, CXformResult *pxfres,
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
		SPQOS_NEW(mp) CPhysicalStreamAgg(
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
