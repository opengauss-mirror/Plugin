//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CXformLeftAntiSemiJoin2HashJoin.cpp
//
//	@doc:
//		Implementation of transform
//---------------------------------------------------------------------------

#include "spqopt/xforms/CXformLeftAntiSemiJoin2HashJoin.h"

#include "spqos/base.h"

#include "spqopt/operators/CLogicalLeftAntiSemiJoin.h"
#include "spqopt/operators/CPatternLeaf.h"
#include "spqopt/operators/CPatternTree.h"
#include "spqopt/operators/CPhysicalLeftAntiSemiHashJoin.h"
#include "spqopt/operators/CPredicateUtils.h"
#include "spqopt/xforms/CXformUtils.h"

using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CXformLeftAntiSemiJoin2HashJoin::CXformLeftAntiSemiJoin2HashJoin
//
//	@doc:
//		ctor
//
//---------------------------------------------------------------------------
CXformLeftAntiSemiJoin2HashJoin::CXformLeftAntiSemiJoin2HashJoin(
	CMemoryPool *mp)
	:  // pattern
	  CXformImplementation(SPQOS_NEW(mp) CExpression(
		  mp, SPQOS_NEW(mp) CLogicalLeftAntiSemiJoin(mp),
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
//		CXformLeftAntiSemiJoin2HashJoin::Exfp
//
//	@doc:
//		Compute xform promise for a given expression handle;
//
//---------------------------------------------------------------------------
CXform::EXformPromise
CXformLeftAntiSemiJoin2HashJoin::Exfp(CExpressionHandle &exprhdl) const
{
	return CXformUtils::ExfpLogicalJoin2PhysicalJoin(exprhdl);
}


//---------------------------------------------------------------------------
//	@function:
//		CXformLeftAntiSemiJoin2HashJoin::Transform
//
//	@doc:
//		actual transformation
//
//---------------------------------------------------------------------------
void
CXformLeftAntiSemiJoin2HashJoin::Transform(CXformContext *pxfctxt,
										   CXformResult *pxfres,
										   CExpression *pexpr) const
{
	SPQOS_ASSERT(NULL != pxfctxt);
	SPQOS_ASSERT(FPromising(pxfctxt->Pmp(), this, pexpr));
	SPQOS_ASSERT(FCheckPattern(pexpr));

	CXformUtils::ImplementHashJoin<CPhysicalLeftAntiSemiHashJoin>(
		pxfctxt, pxfres, pexpr);

	if (pxfres->Pdrspqexpr()->Size() == 0)
	{
		CExpression *pexprProcessed = NULL;
		if (CXformUtils::FProcessSPQDBAntiSemiHashJoin(pxfctxt->Pmp(), pexpr,
													  &pexprProcessed))
		{
			// try again after simplifying join predicate
			CXformUtils::ImplementHashJoin<CPhysicalLeftAntiSemiHashJoin>(
				pxfctxt, pxfres, pexprProcessed);
			pexprProcessed->Release();
		}
	}
}


// EOF
