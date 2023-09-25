//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CXformImplementSplit.cpp
//
//	@doc:
//		Implementation of transform
//---------------------------------------------------------------------------

#include "spqopt/xforms/CXformImplementSplit.h"

#include "spqos/base.h"

#include "spqopt/metadata/CTableDescriptor.h"
#include "spqopt/operators/CLogicalSplit.h"
#include "spqopt/operators/CPatternLeaf.h"
#include "spqopt/operators/CPhysicalSplit.h"

using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CXformImplementSplit::CXformImplementSplit
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CXformImplementSplit::CXformImplementSplit(CMemoryPool *mp)
	: CXformImplementation(
		  // pattern
		  SPQOS_NEW(mp) CExpression(
			  mp, SPQOS_NEW(mp) CLogicalSplit(mp),
			  SPQOS_NEW(mp) CExpression(mp, SPQOS_NEW(mp) CPatternLeaf(mp)),
			  SPQOS_NEW(mp) CExpression(mp, SPQOS_NEW(mp) CPatternLeaf(mp))))
{
}

//---------------------------------------------------------------------------
//	@function:
//		CXformImplementSplit::Exfp
//
//	@doc:
//		Compute promise of xform
//
//---------------------------------------------------------------------------
CXform::EXformPromise
CXformImplementSplit::Exfp(CExpressionHandle &exprhdl) const
{
	if (exprhdl.DeriveHasSubquery(1))
	{
		return CXform::ExfpNone;
	}
	return CXform::ExfpHigh;
}


//---------------------------------------------------------------------------
//	@function:
//		CXformImplementSplit::Transform
//
//	@doc:
//		Actual transformation
//
//---------------------------------------------------------------------------
void
CXformImplementSplit::Transform(CXformContext *pxfctxt, CXformResult *pxfres,
								CExpression *pexpr) const
{
	SPQOS_ASSERT(NULL != pxfctxt);
	SPQOS_ASSERT(FPromising(pxfctxt->Pmp(), this, pexpr));
	SPQOS_ASSERT(FCheckPattern(pexpr));

	CLogicalSplit *popSplit = CLogicalSplit::PopConvert(pexpr->Pop());
	CMemoryPool *mp = pxfctxt->Pmp();

	// extract components for alternative
	CColRefArray *pdrspqcrDelete = popSplit->PdrspqcrDelete();
	pdrspqcrDelete->AddRef();

	CColRefArray *pdrspqcrInsert = popSplit->PdrspqcrInsert();
	pdrspqcrInsert->AddRef();

	CColRef *pcrAction = popSplit->PcrAction();
	CColRef *pcrCtid = popSplit->PcrCtid();
	CColRef *pcrSegmentId = popSplit->PcrSegmentId();
	CColRef *pcrTupleOid = popSplit->PcrTupleOid();

	// child of Split operator
	CExpression *pexprChild = (*pexpr)[0];
	CExpression *pexprProjList = (*pexpr)[1];
	pexprChild->AddRef();
	pexprProjList->AddRef();

	// create physical Split
	CExpression *pexprAlt = SPQOS_NEW(mp) CExpression(
		mp,
		SPQOS_NEW(mp) CPhysicalSplit(mp, pdrspqcrDelete, pdrspqcrInsert, pcrCtid,
									pcrSegmentId, pcrAction, pcrTupleOid),
		pexprChild, pexprProjList);
	// add alternative to transformation result
	pxfres->Add(pexprAlt);
}

// EOF
