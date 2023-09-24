//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CXformDynamicGet2DynamicTableScan.cpp
//
//	@doc:
//		Implementation of transform
//---------------------------------------------------------------------------

#include "spqopt/xforms/CXformDynamicGet2DynamicTableScan.h"

#include "spqos/base.h"

#include "spqopt/metadata/CTableDescriptor.h"
#include "spqopt/operators/CLogicalDynamicGet.h"
#include "spqopt/operators/CPhysicalDynamicTableScan.h"

using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CXformDynamicGet2DynamicTableScan::CXformDynamicGet2DynamicTableScan
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CXformDynamicGet2DynamicTableScan::CXformDynamicGet2DynamicTableScan(
	CMemoryPool *mp)
	: CXformImplementation(
		  // pattern
		  SPQOS_NEW(mp) CExpression(mp, SPQOS_NEW(mp) CLogicalDynamicGet(mp)))
{
}


//---------------------------------------------------------------------------
//	@function:
//		CXformDynamicGet2DynamicTableScan::Transform
//
//	@doc:
//		Actual transformation
//
//---------------------------------------------------------------------------
void
CXformDynamicGet2DynamicTableScan::Transform(CXformContext *pxfctxt,
											 CXformResult *pxfres,
											 CExpression *pexpr) const
{
	SPQOS_ASSERT(NULL != pxfctxt);
	SPQOS_ASSERT(FPromising(pxfctxt->Pmp(), this, pexpr));
	SPQOS_ASSERT(FCheckPattern(pexpr));

	CLogicalDynamicGet *popGet = CLogicalDynamicGet::PopConvert(pexpr->Pop());
	CMemoryPool *mp = pxfctxt->Pmp();

	// create/extract components for alternative
	CName *pname = SPQOS_NEW(mp) CName(mp, popGet->Name());

	CTableDescriptor *ptabdesc = popGet->Ptabdesc();
	ptabdesc->AddRef();

	CColRefArray *pdrspqcrOutput = popGet->PdrspqcrOutput();
	SPQOS_ASSERT(NULL != pdrspqcrOutput);

	pdrspqcrOutput->AddRef();

	CColRef2dArray *pdrspqdrspqcrPart = popGet->PdrspqdrspqcrPart();
	pdrspqdrspqcrPart->AddRef();

	popGet->Ppartcnstr()->AddRef();
	popGet->PpartcnstrRel()->AddRef();

	// create alternative expression
	CExpression *pexprAlt = SPQOS_NEW(mp)
		CExpression(mp, SPQOS_NEW(mp) CPhysicalDynamicTableScan(
							mp, popGet->IsPartial(), pname, ptabdesc,
							popGet->UlOpId(), popGet->ScanId(), pdrspqcrOutput,
							pdrspqdrspqcrPart, popGet->UlSecondaryScanId(),
							popGet->Ppartcnstr(), popGet->PpartcnstrRel()));
	// add alternative to transformation result
	pxfres->Add(pexprAlt);
}


// EOF
