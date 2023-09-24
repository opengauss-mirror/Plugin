//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CXformLeftOuterApply2LeftOuterJoinNoCorrelations.cpp
//
//	@doc:
//		Implementation of transform
//---------------------------------------------------------------------------

#include "spqopt/xforms/CXformLeftOuterApply2LeftOuterJoinNoCorrelations.h"

#include "spqos/base.h"

#include "spqopt/base/CUtils.h"


using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CXformLeftOuterApply2LeftOuterJoinNoCorrelations::Exfp
//
//	@doc:
//		Compute xform promise for a given expression handle;
//		outer references are not allowed
//
//---------------------------------------------------------------------------
CXform::EXformPromise
CXformLeftOuterApply2LeftOuterJoinNoCorrelations::Exfp(
	CExpressionHandle &exprhdl) const
{
	// if there are no outer references, or if all outer refs do not reference outer
	// child, the transformation is applicable
	if (!exprhdl.HasOuterRefs(1 /*child_index*/) ||
		CUtils::FInnerUsesExternalColsOnly(exprhdl))
	{
		return CXform::ExfpHigh;
	}

	return CXform::ExfpNone;
}


//---------------------------------------------------------------------------
//	@function:
//		CXformLeftOuterApply2LeftOuterJoinNoCorrelations::Transform
//
//	@doc:
//		Actual transformation
//
//---------------------------------------------------------------------------
void
CXformLeftOuterApply2LeftOuterJoinNoCorrelations::Transform(
	CXformContext *pxfctxt, CXformResult *pxfres, CExpression *pexpr) const
{
	SPQOS_ASSERT(NULL != pxfctxt);
	SPQOS_ASSERT(FPromising(pxfctxt->Pmp(), this, pexpr));
	SPQOS_ASSERT(FCheckPattern(pexpr));

	CreateJoinAlternative(pxfctxt, pxfres, pexpr);
}


// EOF
