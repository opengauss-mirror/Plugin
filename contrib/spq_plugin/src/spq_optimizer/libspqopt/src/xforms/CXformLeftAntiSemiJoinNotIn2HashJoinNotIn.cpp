//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 EMC Corp.
//
//	@filename:
//		CXformLeftAntiSemiJoinNotIn2HashJoinNotIn.cpp
//
//	@doc:
//		Implementation of transform
//---------------------------------------------------------------------------

#include "spqopt/xforms/CXformLeftAntiSemiJoinNotIn2HashJoinNotIn.h"

#include "spqos/base.h"

#include "spqopt/operators/CLogicalLeftAntiSemiJoinNotIn.h"
#include "spqopt/operators/CPatternLeaf.h"
#include "spqopt/operators/CPhysicalLeftAntiSemiHashJoinNotIn.h"
#include "spqopt/operators/CPredicateUtils.h"
#include "spqopt/xforms/CXformUtils.h"

using namespace spqopt;

//---------------------------------------------------------------------------
//	@function:
//		CXformLeftAntiSemiJoinNotIn2HashJoinNotIn::CXformLeftAntiSemiJoinNotIn2HashJoinNotIn
//
//	@doc:
//		ctor
//
//---------------------------------------------------------------------------
CXformLeftAntiSemiJoinNotIn2HashJoinNotIn::
	CXformLeftAntiSemiJoinNotIn2HashJoinNotIn(CMemoryPool *mp)
	:  // pattern
	  CXformImplementation(SPQOS_NEW(mp) CExpression(
		  mp, SPQOS_NEW(mp) CLogicalLeftAntiSemiJoinNotIn(mp),
		  SPQOS_NEW(mp)
			  CExpression(mp, SPQOS_NEW(mp) CPatternLeaf(mp)),  // left child
		  SPQOS_NEW(mp)
			  CExpression(mp, SPQOS_NEW(mp) CPatternLeaf(mp)),  // right child
		  SPQOS_NEW(mp)
			  CExpression(mp, SPQOS_NEW(mp) CPatternTree(mp))  // predicate
		  ))
{
}

//---------------------------------------------------------------------------
//	@function:
//		CXformLeftAntiSemiJoinNotIn2HashJoinNotIn::Exfp
//
//	@doc:
//		Compute xform promise for a given expression handle;
//
//---------------------------------------------------------------------------
CXform::EXformPromise
CXformLeftAntiSemiJoinNotIn2HashJoinNotIn::Exfp(
	CExpressionHandle &exprhdl) const
{
	return CXformUtils::ExfpLogicalJoin2PhysicalJoin(exprhdl);
}

//---------------------------------------------------------------------------
//	@function:
//		CXformLeftAntiSemiJoinNotIn2HashJoinNotIn::Transform
//
//	@doc:
//		actual transformation
//
//---------------------------------------------------------------------------
void
CXformLeftAntiSemiJoinNotIn2HashJoinNotIn::Transform(CXformContext *pxfctxt,
													 CXformResult *pxfres,
													 CExpression *pexpr) const
{
	SPQOS_ASSERT(NULL != pxfctxt);
	SPQOS_ASSERT(FPromising(pxfctxt->Pmp(), this, pexpr));
	SPQOS_ASSERT(FCheckPattern(pexpr));

	CXformUtils::ImplementHashJoin<CPhysicalLeftAntiSemiHashJoinNotIn>(
		pxfctxt, pxfres, pexpr);

	if (pxfres->Pdrspqexpr()->Size() == 0)
	{
		CExpression *pexprProcessed = NULL;
		if (CXformUtils::FProcessSPQDBAntiSemiHashJoin(pxfctxt->Pmp(), pexpr,
													  &pexprProcessed))
		{
			// try again after simplifying join predicate
			CXformUtils::ImplementHashJoin<CPhysicalLeftAntiSemiHashJoinNotIn>(
				pxfctxt, pxfres, pexprProcessed);
			pexprProcessed->Release();
		}
	}
}

// EOF
