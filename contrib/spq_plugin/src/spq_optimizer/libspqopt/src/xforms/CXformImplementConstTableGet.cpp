//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CXformImplementConstTableGet.cpp
//
//	@doc:
//		Implementation of transform
//---------------------------------------------------------------------------

#include "spqopt/xforms/CXformImplementConstTableGet.h"

#include "spqos/base.h"

#include "spqopt/operators/CLogicalConstTableGet.h"
#include "spqopt/operators/CPhysicalConstTableGet.h"

using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CXformImplementConstTableGet::CXformImplementConstTableGet
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CXformImplementConstTableGet::CXformImplementConstTableGet(CMemoryPool *mp)
	: CXformImplementation(
		  // pattern
		  SPQOS_NEW(mp) CExpression(mp, SPQOS_NEW(mp) CLogicalConstTableGet(mp)))
{
}


//---------------------------------------------------------------------------
//	@function:
//		CXformImplementConstTableGet::Transform
//
//	@doc:
//		Actual transformation
//
//---------------------------------------------------------------------------
void
CXformImplementConstTableGet::Transform(CXformContext *pxfctxt,
										CXformResult *pxfres,
										CExpression *pexpr) const
{
	SPQOS_ASSERT(NULL != pxfctxt);
	SPQOS_ASSERT(FPromising(pxfctxt->Pmp(), this, pexpr));
	SPQOS_ASSERT(FCheckPattern(pexpr));

	CLogicalConstTableGet *popConstTableGet =
		CLogicalConstTableGet::PopConvert(pexpr->Pop());
	CMemoryPool *mp = pxfctxt->Pmp();

	// create/extract components for alternative
	CColumnDescriptorArray *pdrspqcoldesc = popConstTableGet->Pdrspqcoldesc();
	pdrspqcoldesc->AddRef();

	IDatum2dArray *pdrspqdrspqdatum = popConstTableGet->Pdrspqdrspqdatum();
	pdrspqdrspqdatum->AddRef();

	CColRefArray *pdrspqcrOutput = popConstTableGet->PdrspqcrOutput();
	pdrspqcrOutput->AddRef();

	// create alternative expression
	CExpression *pexprAlt = SPQOS_NEW(mp)
		CExpression(mp, SPQOS_NEW(mp) CPhysicalConstTableGet(
							mp, pdrspqcoldesc, pdrspqdrspqdatum, pdrspqcrOutput));

	// add alternative to transformation result
	pxfres->Add(pexprAlt);
}


// EOF
