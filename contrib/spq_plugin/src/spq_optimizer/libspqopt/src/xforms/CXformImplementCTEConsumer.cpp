//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CXformImplementCTEConsumer.cpp
//
//	@doc:
//		Implementation of transform
//---------------------------------------------------------------------------

#include "spqopt/xforms/CXformImplementCTEConsumer.h"

#include "spqos/base.h"

#include "spqopt/operators/CLogicalCTEConsumer.h"
#include "spqopt/operators/CPhysicalCTEConsumer.h"

using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CXformImplementCTEConsumer::CXformImplementCTEConsumer
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CXformImplementCTEConsumer::CXformImplementCTEConsumer(CMemoryPool *mp)
	: CXformImplementation(
		  // pattern
		  SPQOS_NEW(mp) CExpression(mp, SPQOS_NEW(mp) CLogicalCTEConsumer(mp)))
{
}

//---------------------------------------------------------------------------
//	@function:
//		CXformImplementCTEConsumer::Exfp
//
//	@doc:
//		Compute promise of xform
//
//---------------------------------------------------------------------------
CXform::EXformPromise
CXformImplementCTEConsumer::Exfp(CExpressionHandle &  // exprhdl
) const
{
	return CXform::ExfpHigh;
}


//---------------------------------------------------------------------------
//	@function:
//		CXformImplementCTEConsumer::Transform
//
//	@doc:
//		Actual transformation
//
//---------------------------------------------------------------------------
void
CXformImplementCTEConsumer::Transform(CXformContext *pxfctxt,
									  CXformResult *pxfres,
									  CExpression *pexpr) const
{
	SPQOS_ASSERT(NULL != pxfctxt);
	SPQOS_ASSERT(FPromising(pxfctxt->Pmp(), this, pexpr));
	SPQOS_ASSERT(FCheckPattern(pexpr));

	CLogicalCTEConsumer *popCTEConsumer =
		CLogicalCTEConsumer::PopConvert(pexpr->Pop());
	CMemoryPool *mp = pxfctxt->Pmp();

	// extract components for alternative
	ULONG id = popCTEConsumer->UlCTEId();

	CColRefArray *colref_array = popCTEConsumer->Pdrspqcr();
	colref_array->AddRef();

	UlongToColRefMap *colref_mapping = popCTEConsumer->Phmulcr();
	SPQOS_ASSERT(NULL != colref_mapping);
	colref_mapping->AddRef();

	// create physical CTE Consumer
	CExpression *pexprAlt =
		SPQOS_NEW(mp) CExpression(mp, SPQOS_NEW(mp) CPhysicalCTEConsumer(
										 mp, id, colref_array, colref_mapping));

	// add alternative to transformation result
	pxfres->Add(pexprAlt);
}

// EOF
