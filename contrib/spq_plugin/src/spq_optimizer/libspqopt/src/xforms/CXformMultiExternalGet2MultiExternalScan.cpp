//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 Pivotal, Inc.
//
//	@filename:
//		CXformMultiExternalGet2MultiExternalScan.cpp
//
//	@doc:
//		Implementation of transform
//---------------------------------------------------------------------------

#include "spqopt/xforms/CXformMultiExternalGet2MultiExternalScan.h"

#include "spqos/base.h"

#include "spqopt/metadata/CTableDescriptor.h"
#include "spqopt/operators/CLogicalMultiExternalGet.h"
#include "spqopt/operators/CPhysicalMultiExternalScan.h"

using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CXformMultiExternalGet2MultiExternalScan::CXformMultiExternalGet2MultiExternalScan
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CXformMultiExternalGet2MultiExternalScan::
	CXformMultiExternalGet2MultiExternalScan(CMemoryPool *mp)
	: CXformImplementation(
		  // pattern
		  SPQOS_NEW(mp)
			  CExpression(mp, SPQOS_NEW(mp) CLogicalMultiExternalGet(mp)))
{
}

//---------------------------------------------------------------------------
//	@function:
//		CXformMultiExternalGet2MultiExternalScan::Exfp
//
//	@doc:
//		Compute promise of xform
//
//---------------------------------------------------------------------------
CXform::EXformPromise
CXformMultiExternalGet2MultiExternalScan::Exfp(CExpressionHandle &	//exprhdl
) const
{
	return CXform::ExfpHigh;
}

//---------------------------------------------------------------------------
//	@function:
//		CXformMultiExternalGet2MultiExternalScan::Transform
//
//	@doc:
//		Actual transformation
//
//---------------------------------------------------------------------------
void
CXformMultiExternalGet2MultiExternalScan::Transform(CXformContext *pxfctxt,
													CXformResult *pxfres,
													CExpression *pexpr) const
{
	SPQOS_ASSERT(NULL != pxfctxt);
	SPQOS_ASSERT(FPromising(pxfctxt->Pmp(), this, pexpr));
	SPQOS_ASSERT(FCheckPattern(pexpr));

	CLogicalMultiExternalGet *popGet =
		CLogicalMultiExternalGet::PopConvert(pexpr->Pop());
	CMemoryPool *mp = pxfctxt->Pmp();

	// extract components for alternative
	CName *pname = SPQOS_NEW(mp) CName(mp, popGet->Name());

	CColRefArray *pdrspqcrOutput = popGet->PdrspqcrOutput();
	SPQOS_ASSERT(NULL != pdrspqcrOutput);

	popGet->GetScanPartitionMdids()->AddRef();
	popGet->Ptabdesc()->AddRef();
	popGet->PdrspqdrspqcrPart()->AddRef();
	popGet->Ppartcnstr()->AddRef();
	popGet->PpartcnstrRel()->AddRef();
	pdrspqcrOutput->AddRef();

	// create alternative expression
	CExpression *pexprAlt = SPQOS_NEW(mp) CExpression(
		mp, SPQOS_NEW(mp) CPhysicalMultiExternalScan(
				mp, popGet->GetScanPartitionMdids(), popGet->IsPartial(),
				popGet->Ptabdesc(), popGet->UlOpId(), pname, popGet->ScanId(),
				pdrspqcrOutput, popGet->PdrspqdrspqcrPart(),
				popGet->UlSecondaryScanId(), popGet->Ppartcnstr(),
				popGet->PpartcnstrRel()));

	// add alternative to transformation result
	pxfres->Add(pexprAlt);
}

// EOF
