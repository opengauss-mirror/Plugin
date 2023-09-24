//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CXformImplementInnerJoin.cpp
//
//	@doc:
//		Transform inner join to inner Hash Join or Inner Nested Loop Join
//		(if a Hash Join is not possible)
//---------------------------------------------------------------------------

#include "spqopt/xforms/CXformImplementInnerJoin.h"

#include "spqos/base.h"

#include "spqopt/operators/CLogicalInnerJoin.h"
#include "spqopt/operators/CPatternLeaf.h"
#include "spqopt/operators/CPhysicalInnerHashJoin.h"
#include "spqopt/operators/CPhysicalInnerNLJoin.h"
#include "spqopt/xforms/CXformUtils.h"

using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CXformImplementInnerJoin::CXformImplementInnerJoin
//
//	@doc:
//		ctor
//
//---------------------------------------------------------------------------
CXformImplementInnerJoin::CXformImplementInnerJoin(CMemoryPool *mp)
	:  // pattern
	  CXformImplementation(SPQOS_NEW(mp) CExpression(
		  mp, SPQOS_NEW(mp) CLogicalInnerJoin(mp),
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
//		CXformImplementInnerJoin::Exfp
//
//	@doc:
//		Compute xform promise for a given expression handle;
//
//---------------------------------------------------------------------------
CXform::EXformPromise
CXformImplementInnerJoin::Exfp(CExpressionHandle &exprhdl) const
{
	return CXformUtils::ExfpLogicalJoin2PhysicalJoin(exprhdl);
}


//---------------------------------------------------------------------------
//	@function:
//		CXformImplementInnerJoin::Transform
//
//	@doc:
//		actual transformation
//
//---------------------------------------------------------------------------
void
CXformImplementInnerJoin::Transform(CXformContext *pxfctxt,
									CXformResult *pxfres,
									CExpression *pexpr) const
{
	SPQOS_ASSERT(NULL != pxfctxt);
	SPQOS_ASSERT(FPromising(pxfctxt->Pmp(), this, pexpr));
	SPQOS_ASSERT(FCheckPattern(pexpr));

	SPQOS_ASSERT(pxfres->Size() == 0);

	if (SPQOPT_FENABLED_XFORM(ExfInnerJoin2HashJoin))
	{
		CXformUtils::ImplementHashJoin<CPhysicalInnerHashJoin>(pxfctxt, pxfres,
															   pexpr);
	}

	if ((SPQOS_FTRACE(EopttraceForceComprehensiveJoinImplementation) ||
		 pxfres->Size() == 0) &&
		SPQOPT_FENABLED_XFORM(ExfInnerJoin2NLJoin))
	{
		CXformUtils::ImplementNLJoin<CPhysicalInnerNLJoin>(pxfctxt, pxfres,
														   pexpr);
	}
}

// EOF
