//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		CXformImplementSequence.cpp
//
//	@doc:
//		Implementation of transform
//---------------------------------------------------------------------------

#include "spqopt/xforms/CXformImplementSequence.h"

#include "spqos/base.h"

#include "spqopt/metadata/CTableDescriptor.h"
#include "spqopt/operators/CLogicalSequence.h"
#include "spqopt/operators/CPatternMultiLeaf.h"
#include "spqopt/operators/CPhysicalSequence.h"

using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CXformImplementSequence::CXformImplementSequence
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CXformImplementSequence::CXformImplementSequence(CMemoryPool *mp)
	: CXformImplementation(
		  // pattern
		  SPQOS_NEW(mp) CExpression(
			  mp, SPQOS_NEW(mp) CLogicalSequence(mp),
			  SPQOS_NEW(mp) CExpression(mp, SPQOS_NEW(mp) CPatternMultiLeaf(mp))))
{
}


//---------------------------------------------------------------------------
//	@function:
//		CXformImplementSequence::Transform
//
//	@doc:
//		Actual transformation
//
//---------------------------------------------------------------------------
void
CXformImplementSequence::Transform(CXformContext *pxfctxt, CXformResult *pxfres,
								   CExpression *pexpr) const
{
	SPQOS_ASSERT(NULL != pxfctxt);
	SPQOS_ASSERT(FPromising(pxfctxt->Pmp(), this, pexpr));
	SPQOS_ASSERT(FCheckPattern(pexpr));

	CMemoryPool *mp = pxfctxt->Pmp();

	CExpressionArray *pdrspqexpr = pexpr->PdrgPexpr();
	pdrspqexpr->AddRef();

	// create alternative expression
	CExpression *pexprAlt = SPQOS_NEW(mp)
		CExpression(mp, SPQOS_NEW(mp) CPhysicalSequence(mp), pdrspqexpr);
	// add alternative to transformation result
	pxfres->Add(pexprAlt);
}


// EOF
