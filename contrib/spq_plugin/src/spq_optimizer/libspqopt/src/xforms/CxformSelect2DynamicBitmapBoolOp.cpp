//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal, Inc.
//
//	@filename:
//		CXformSelect2DynamicBitmapBoolOp.cpp
//
//	@doc:
//		Transform select over partitioned table into a dynamic bitmap table get
//		over bitmap bool op
//
//	@owner:
//
//
//	@test:
//
//---------------------------------------------------------------------------

#include "spqopt/xforms/CXformSelect2DynamicBitmapBoolOp.h"

#include "spqopt/operators/CLogicalDynamicGet.h"
#include "spqopt/operators/CLogicalSelect.h"
#include "spqopt/xforms/CXformUtils.h"

using namespace spqmd;
using namespace spqopt;

//---------------------------------------------------------------------------
//	@function:
//		CXformSelect2DynamicBitmapBoolOp::CXformSelect2DynamicBitmapBoolOp
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CXformSelect2DynamicBitmapBoolOp::CXformSelect2DynamicBitmapBoolOp(
	CMemoryPool *mp)
	: CXformExploration(SPQOS_NEW(mp) CExpression(
		  mp, SPQOS_NEW(mp) CLogicalSelect(mp),
		  SPQOS_NEW(mp) CExpression(
			  mp, SPQOS_NEW(mp) CLogicalDynamicGet(mp)),	 // logical child
		  SPQOS_NEW(mp)
			  CExpression(mp, SPQOS_NEW(mp) CPatternTree(mp))  // predicate tree
		  ))
{
}

//---------------------------------------------------------------------------
//	@function:
//		CXformSelect2DynamicBitmapBoolOp::Exfp
//
//	@doc:
//		Compute xform promise for a given expression handle
//
//---------------------------------------------------------------------------
CXform::EXformPromise
CXformSelect2DynamicBitmapBoolOp::Exfp(CExpressionHandle &	// exprhdl
) const
{
	return CXform::ExfpHigh;
}

//---------------------------------------------------------------------------
//	@function:
//		CXformSelect2DynamicBitmapBoolOp::Transform
//
//	@doc:
//		Actual transformation
//
//---------------------------------------------------------------------------
void
CXformSelect2DynamicBitmapBoolOp::Transform(CXformContext *pxfctxt,
											CXformResult *pxfres,
											CExpression *pexpr) const
{
	SPQOS_ASSERT(NULL != pxfctxt);
	SPQOS_ASSERT(FPromising(pxfctxt->Pmp(), this, pexpr));
	SPQOS_ASSERT(FCheckPattern(pexpr));

	CMemoryPool *mp = pxfctxt->Pmp();
	CExpression *pexprResult = CXformUtils::PexprSelect2BitmapBoolOp(mp, pexpr);

	if (NULL != pexprResult)
	{
		// create a redundant SELECT on top of DynamicIndexGet to be able to use predicate in partition elimination
		CExpression *pexprRedundantSelect =
			CXformUtils::PexprRedundantSelectForDynamicIndex(mp, pexprResult);
		pexprResult->Release();

		pxfres->Add(pexprRedundantSelect);
	}
}

// EOF
