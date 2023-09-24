//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		CXformGet2TableScan.cpp
//
//	@doc:
//		Implementation of transform
//---------------------------------------------------------------------------

#include "spqopt/xforms/CXformGet2TableScan.h"

#include "spqos/base.h"

#include "spqopt/metadata/CTableDescriptor.h"
#include "spqopt/operators/CExpressionHandle.h"
#include "spqopt/operators/CLogicalGet.h"
#include "spqopt/operators/CPhysicalTableScan.h"

using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CXformGet2TableScan::CXformGet2TableScan
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CXformGet2TableScan::CXformGet2TableScan(CMemoryPool *mp)
	: CXformImplementation(
		  // pattern
		  SPQOS_NEW(mp) CExpression(mp, SPQOS_NEW(mp) CLogicalGet(mp)))
{
}

//---------------------------------------------------------------------------
//	@function:
//		CXformGet2TableScan::Exfp
//
//	@doc:
//		Compute promise of xform
//
//---------------------------------------------------------------------------
CXform::EXformPromise
CXformGet2TableScan::Exfp(CExpressionHandle &exprhdl) const
{
	CLogicalGet *popGet = CLogicalGet::PopConvert(exprhdl.Pop());

	CTableDescriptor *ptabdesc = popGet->Ptabdesc();
	if (ptabdesc->IsPartitioned())
	{
		return CXform::ExfpNone;
	}

	return CXform::ExfpHigh;
}


//---------------------------------------------------------------------------
//	@function:
//		CXformGet2TableScan::Transform
//
//	@doc:
//		Actual transformation
//
//---------------------------------------------------------------------------
void
CXformGet2TableScan::Transform(CXformContext *pxfctxt, CXformResult *pxfres,
							   CExpression *pexpr) const
{
	SPQOS_ASSERT(NULL != pxfctxt);
	SPQOS_ASSERT(FPromising(pxfctxt->Pmp(), this, pexpr));
	SPQOS_ASSERT(FCheckPattern(pexpr));

	CLogicalGet *popGet = CLogicalGet::PopConvert(pexpr->Pop());
	CMemoryPool *mp = pxfctxt->Pmp();

	// create/extract components for alternative
	CName *pname = SPQOS_NEW(mp) CName(mp, popGet->Name());

	CTableDescriptor *ptabdesc = popGet->Ptabdesc();
	ptabdesc->AddRef();

	CColRefArray *pdrspqcrOutput = popGet->PdrspqcrOutput();
	SPQOS_ASSERT(NULL != pdrspqcrOutput);

	pdrspqcrOutput->AddRef();

	// create alternative expression
	CExpression *pexprAlt = SPQOS_NEW(mp) CExpression(
		mp,
		SPQOS_NEW(mp) CPhysicalTableScan(mp, pname, ptabdesc, pdrspqcrOutput));
	// add alternative to transformation result
	pxfres->Add(pexprAlt);
}


// EOF
