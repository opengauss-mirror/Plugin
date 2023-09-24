//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 EMC Corp.
//
//	@filename:
//		CXformLeftAntiSemiJoinNotIn2NLJoinNotIn.cpp
//
//	@doc:
//		Implementation of transform
//---------------------------------------------------------------------------

#include "spqopt/xforms/CXformLeftAntiSemiJoinNotIn2NLJoinNotIn.h"

#include "spqos/base.h"

#include "spqopt/operators/CLogicalLeftAntiSemiJoinNotIn.h"
#include "spqopt/operators/CPatternLeaf.h"
#include "spqopt/operators/CPhysicalLeftAntiSemiNLJoinNotIn.h"
#include "spqopt/xforms/CXformUtils.h"

using namespace spqopt;

//---------------------------------------------------------------------------
//	@function:
//		CXformLeftAntiSemiJoinNotIn2NLJoinNotIn::CXformLeftAntiSemiJoinNotIn2NLJoinNotIn
//
//	@doc:
//		ctor
//
//---------------------------------------------------------------------------
CXformLeftAntiSemiJoinNotIn2NLJoinNotIn::
	CXformLeftAntiSemiJoinNotIn2NLJoinNotIn(CMemoryPool *mp)
	:  // pattern
	  CXformImplementation(SPQOS_NEW(mp) CExpression(
		  mp, SPQOS_NEW(mp) CLogicalLeftAntiSemiJoinNotIn(mp),
		  SPQOS_NEW(mp)
			  CExpression(mp, SPQOS_NEW(mp) CPatternLeaf(mp)),  // left child
		  SPQOS_NEW(mp)
			  CExpression(mp, SPQOS_NEW(mp) CPatternLeaf(mp)),  // right child
		  SPQOS_NEW(mp)
			  CExpression(mp, SPQOS_NEW(mp) CPatternLeaf(mp)))  // predicate
	  )
{
}


//---------------------------------------------------------------------------
//	@function:
//		CXformLeftAntiSemiJoinNotIn2NLJoinNotIn::Exfp
//
//	@doc:
//		Compute xform promise for a given expression handle;
//
//---------------------------------------------------------------------------
CXform::EXformPromise
CXformLeftAntiSemiJoinNotIn2NLJoinNotIn::Exfp(CExpressionHandle &exprhdl) const
{
	return CXformUtils::ExfpLogicalJoin2PhysicalJoin(exprhdl);
}


//---------------------------------------------------------------------------
//	@function:
//		CXformLeftAntiSemiJoinNotIn2NLJoinNotIn::Transform
//
//	@doc:
//		actual transformation
//
//---------------------------------------------------------------------------
void
CXformLeftAntiSemiJoinNotIn2NLJoinNotIn::Transform(CXformContext *pxfctxt,
												   CXformResult *pxfres,
												   CExpression *pexpr) const
{
	SPQOS_ASSERT(NULL != pxfctxt);
	SPQOS_ASSERT(FPromising(pxfctxt->Pmp(), this, pexpr));
	SPQOS_ASSERT(FCheckPattern(pexpr));

	CXformUtils::ImplementNLJoin<CPhysicalLeftAntiSemiNLJoinNotIn>(
		pxfctxt, pxfres, pexpr);
}


// EOF
