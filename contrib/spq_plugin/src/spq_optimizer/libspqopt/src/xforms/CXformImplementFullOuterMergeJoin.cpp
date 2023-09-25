//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2019 Pivotal Software, Inc.

#include "spqopt/xforms/CXformImplementFullOuterMergeJoin.h"

#include "spqos/base.h"

#include "spqopt/operators/CLogicalFullOuterJoin.h"
#include "spqopt/operators/CPatternLeaf.h"
#include "spqopt/operators/CPhysicalFullMergeJoin.h"
#include "spqopt/xforms/CXformUtils.h"

using namespace spqopt;


// ctor
CXformImplementFullOuterMergeJoin::CXformImplementFullOuterMergeJoin(
	CMemoryPool *mp)
	: CXformExploration(
		  // pattern
		  SPQOS_NEW(mp) CExpression(
			  mp, SPQOS_NEW(mp) CLogicalFullOuterJoin(mp),
			  SPQOS_NEW(mp) CExpression(
				  mp, SPQOS_NEW(mp) CPatternLeaf(mp)),  // outer child
			  SPQOS_NEW(mp) CExpression(
				  mp, SPQOS_NEW(mp) CPatternLeaf(mp)),  // inner child
			  SPQOS_NEW(mp) CExpression(
				  mp, SPQOS_NEW(mp) CPatternTree(mp))  // scalar child
			  ))
{
}

CXform::EXformPromise
CXformImplementFullOuterMergeJoin::Exfp(CExpressionHandle &exprhdl) const
{
	if (exprhdl.DeriveHasSubquery(2))
	{
		return CXform::ExfpNone;
	}
	return CXform::ExfpHigh;
}

void
CXformImplementFullOuterMergeJoin::Transform(CXformContext *pxfctxt,
											 CXformResult *pxfres,
											 CExpression *pexpr) const
{
	SPQOS_ASSERT(NULL != pxfctxt);
	SPQOS_ASSERT(FPromising(pxfctxt->Pmp(), this, pexpr));
	SPQOS_ASSERT(FCheckPattern(pexpr));

	CXformUtils::ImplementMergeJoin<CPhysicalFullMergeJoin>(pxfctxt, pxfres,
															pexpr);
}

// EOF
