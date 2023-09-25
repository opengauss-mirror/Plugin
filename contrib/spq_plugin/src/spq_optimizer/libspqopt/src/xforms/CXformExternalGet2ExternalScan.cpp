//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 Pivotal, Inc.
//
//	@filename:
//		CXformExternalGet2ExternalScan.cpp
//
//	@doc:
//		Implementation of transform
//---------------------------------------------------------------------------

#include "spqopt/xforms/CXformExternalGet2ExternalScan.h"

#include "spqos/base.h"

#include "spqopt/metadata/CTableDescriptor.h"
#include "spqopt/operators/CLogicalExternalGet.h"
#include "spqopt/operators/CPhysicalExternalScan.h"

using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CXformExternalGet2ExternalScan::CXformExternalGet2ExternalScan
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CXformExternalGet2ExternalScan::CXformExternalGet2ExternalScan(CMemoryPool *mp)
	: CXformImplementation(
		  // pattern
		  SPQOS_NEW(mp) CExpression(mp, SPQOS_NEW(mp) CLogicalExternalGet(mp)))
{
}

//---------------------------------------------------------------------------
//	@function:
//		CXformExternalGet2ExternalScan::Exfp
//
//	@doc:
//		Compute promise of xform
//
//---------------------------------------------------------------------------
CXform::EXformPromise
CXformExternalGet2ExternalScan::Exfp(CExpressionHandle &  //exprhdl
) const
{
	return CXform::ExfpHigh;
}

//---------------------------------------------------------------------------
//	@function:
//		CXformExternalGet2ExternalScan::Transform
//
//	@doc:
//		Actual transformation
//
//---------------------------------------------------------------------------
void
CXformExternalGet2ExternalScan::Transform(CXformContext *pxfctxt,
										  CXformResult *pxfres,
										  CExpression *pexpr) const
{
	SPQOS_ASSERT(NULL != pxfctxt);
	SPQOS_ASSERT(FPromising(pxfctxt->Pmp(), this, pexpr));
	SPQOS_ASSERT(FCheckPattern(pexpr));

	CLogicalExternalGet *popGet = CLogicalExternalGet::PopConvert(pexpr->Pop());
	CMemoryPool *mp = pxfctxt->Pmp();

	// extract components for alternative
	CName *pname = SPQOS_NEW(mp) CName(mp, popGet->Name());

	CTableDescriptor *ptabdesc = popGet->Ptabdesc();
	ptabdesc->AddRef();

	CColRefArray *pdrspqcrOutput = popGet->PdrspqcrOutput();
	SPQOS_ASSERT(NULL != pdrspqcrOutput);

	pdrspqcrOutput->AddRef();

	// create alternative expression
	CExpression *pexprAlt = SPQOS_NEW(mp) CExpression(
		mp,
		SPQOS_NEW(mp) CPhysicalExternalScan(mp, pname, ptabdesc, pdrspqcrOutput));

	// add alternative to transformation result
	pxfres->Add(pexprAlt);
}

// EOF
