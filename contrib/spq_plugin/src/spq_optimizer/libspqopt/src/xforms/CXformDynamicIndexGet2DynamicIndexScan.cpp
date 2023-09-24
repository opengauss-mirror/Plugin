//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CXformDynamicIndexGet2DynamicIndexScan.cpp
//
//	@doc:
//		Implementation of transform
//---------------------------------------------------------------------------

#include "spqopt/xforms/CXformDynamicIndexGet2DynamicIndexScan.h"

#include "spqos/base.h"

#include "spqopt/metadata/CPartConstraint.h"
#include "spqopt/metadata/CTableDescriptor.h"
#include "spqopt/operators/CLogicalDynamicIndexGet.h"
#include "spqopt/operators/CPatternLeaf.h"
#include "spqopt/operators/CPhysicalDynamicIndexScan.h"

using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CXformDynamicIndexGet2DynamicIndexScan::CXformDynamicIndexGet2DynamicIndexScan
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CXformDynamicIndexGet2DynamicIndexScan::CXformDynamicIndexGet2DynamicIndexScan(
	CMemoryPool *mp)
	: CXformImplementation(
		  // pattern
		  SPQOS_NEW(mp) CExpression(
			  mp, SPQOS_NEW(mp) CLogicalDynamicIndexGet(mp),
			  SPQOS_NEW(mp) CExpression(
				  mp, SPQOS_NEW(mp) CPatternLeaf(mp))  // index lookup predicate
			  ))
{
}

CXform::EXformPromise
CXformDynamicIndexGet2DynamicIndexScan::Exfp(CExpressionHandle &exprhdl) const
{
	if (exprhdl.DeriveHasSubquery(0))
	{
		return CXform::ExfpNone;
	}
	return CXform::ExfpHigh;
}

//---------------------------------------------------------------------------
//	@function:
//		CXformDynamicIndexGet2DynamicIndexScan::Transform
//
//	@doc:
//		Actual transformation
//
//---------------------------------------------------------------------------
void
CXformDynamicIndexGet2DynamicIndexScan::Transform(CXformContext *pxfctxt,
												  CXformResult *pxfres,
												  CExpression *pexpr) const
{
	SPQOS_ASSERT(NULL != pxfctxt);
	SPQOS_ASSERT(FPromising(pxfctxt->Pmp(), this, pexpr));
	SPQOS_ASSERT(FCheckPattern(pexpr));

	CLogicalDynamicIndexGet *popIndexGet =
		CLogicalDynamicIndexGet::PopConvert(pexpr->Pop());
	CMemoryPool *mp = pxfctxt->Pmp();

	// create/extract components for alternative
	CName *pname = SPQOS_NEW(mp) CName(mp, popIndexGet->Name());

	// extract components
	CExpression *pexprIndexCond = (*pexpr)[0];
	pexprIndexCond->AddRef();

	CTableDescriptor *ptabdesc = popIndexGet->Ptabdesc();
	ptabdesc->AddRef();

	CIndexDescriptor *pindexdesc = popIndexGet->Pindexdesc();
	pindexdesc->AddRef();

	CColRefArray *pdrspqcrOutput = popIndexGet->PdrspqcrOutput();
	SPQOS_ASSERT(NULL != pdrspqcrOutput);
	pdrspqcrOutput->AddRef();

	CColRef2dArray *pdrspqdrspqcrPart = popIndexGet->PdrspqdrspqcrPart();
	pdrspqdrspqcrPart->AddRef();

	CPartConstraint *ppartcnstr = popIndexGet->Ppartcnstr();
	ppartcnstr->AddRef();

	CPartConstraint *ppartcnstrRel = popIndexGet->PpartcnstrRel();
	ppartcnstrRel->AddRef();

	COrderSpec *pos = popIndexGet->Pos();
	pos->AddRef();

	// create alternative expression
	CExpression *pexprAlt = SPQOS_NEW(mp) CExpression(
		mp,
		SPQOS_NEW(mp) CPhysicalDynamicIndexScan(
			mp, popIndexGet->IsPartial(), pindexdesc, ptabdesc,
			pexpr->Pop()->UlOpId(), pname, pdrspqcrOutput, popIndexGet->ScanId(),
			pdrspqdrspqcrPart, popIndexGet->UlSecondaryScanId(), ppartcnstr,
			ppartcnstrRel, pos),
		pexprIndexCond);
	// add alternative to transformation result
	pxfres->Add(pexprAlt);
}


// EOF
