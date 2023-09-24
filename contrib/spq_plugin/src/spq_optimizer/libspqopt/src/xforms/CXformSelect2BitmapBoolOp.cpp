//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal, Inc.
//
//	@filename:
//		CXformSelect2BitmapBoolOp.cpp
//
//	@doc:
//		Transform select over table into a bitmap table get over bitmap bool op
//
//	@owner:
//
//
//	@test:
//
//---------------------------------------------------------------------------

#include "spqopt/xforms/CXformSelect2BitmapBoolOp.h"

#include "spqopt/operators/CLogicalGet.h"
#include "spqopt/operators/CLogicalSelect.h"
#include "spqopt/xforms/CXformUtils.h"

using namespace spqmd;
using namespace spqopt;

//---------------------------------------------------------------------------
//	@function:
//		CXformSelect2BitmapBoolOp::CXformSelect2BitmapBoolOp
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CXformSelect2BitmapBoolOp::CXformSelect2BitmapBoolOp(CMemoryPool *mp)
	: CXformExploration(SPQOS_NEW(mp) CExpression(
		  mp, SPQOS_NEW(mp) CLogicalSelect(mp),
		  SPQOS_NEW(mp)
			  CExpression(mp, SPQOS_NEW(mp) CLogicalGet(mp)),  // logical child
		  SPQOS_NEW(mp)
			  CExpression(mp, SPQOS_NEW(mp) CPatternTree(mp))  // predicate tree
		  ))
{
}

//---------------------------------------------------------------------------
//	@function:
//		CXformSelect2BitmapBoolOp::Exfp
//
//	@doc:
//		Compute xform promise for a given expression handle
//
//---------------------------------------------------------------------------
CXform::EXformPromise
CXformSelect2BitmapBoolOp::Exfp(CExpressionHandle &	 // exprhdl
) const
{
	return CXform::ExfpHigh;
}

//---------------------------------------------------------------------------
//	@function:
//		CXformSelect2BitmapBoolOp::Transform
//
//	@doc:
//		Actual transformation
//
//---------------------------------------------------------------------------
void
CXformSelect2BitmapBoolOp::Transform(CXformContext *pxfctxt,
									 CXformResult *pxfres,
									 CExpression *pexpr) const
{
	SPQOS_ASSERT(NULL != pxfctxt);
	SPQOS_ASSERT(FPromising(pxfctxt->Pmp(), this, pexpr));
	SPQOS_ASSERT(FCheckPattern(pexpr));

	CExpression *pexprResult =
		CXformUtils::PexprSelect2BitmapBoolOp(pxfctxt->Pmp(), pexpr);

	if (NULL != pexprResult)
	{
		pxfres->Add(pexprResult);
	}
}

// EOF
