//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal, Inc.
//
//	@filename:
//		CXformImplementDynamicBitmapTableGet.cpp
//
//	@doc:
//		Implement DynamicBitmapTableGet
//
//	@owner:
//
//
//	@test:
//
//---------------------------------------------------------------------------

#include "spqopt/xforms/CXformImplementDynamicBitmapTableGet.h"

#include "spqopt/metadata/CTableDescriptor.h"
#include "spqopt/operators/CLogicalDynamicBitmapTableGet.h"
#include "spqopt/operators/CPatternLeaf.h"
#include "spqopt/operators/CPhysicalDynamicBitmapTableScan.h"

using namespace spqopt;
using namespace spqos;

//---------------------------------------------------------------------------
//	@function:
//		CXformImplementDynamicBitmapTableGet::CXformImplementDynamicBitmapTableGet
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CXformImplementDynamicBitmapTableGet::CXformImplementDynamicBitmapTableGet(
	CMemoryPool *mp)
	:  // pattern
	  CXformImplementation(SPQOS_NEW(mp) CExpression(
		  mp, SPQOS_NEW(mp) CLogicalDynamicBitmapTableGet(mp),
		  SPQOS_NEW(mp)
			  CExpression(mp, SPQOS_NEW(mp) CPatternLeaf(mp)),  // predicate tree
		  SPQOS_NEW(mp) CExpression(
			  mp, SPQOS_NEW(mp) CPatternLeaf(mp))  // bitmap index expression
		  ))
{
}

// compute xform promise for a given expression handle
CXform::EXformPromise
CXformImplementDynamicBitmapTableGet::Exfp(CExpressionHandle &exprhdl) const
{
	if (exprhdl.DeriveHasSubquery(0) || exprhdl.DeriveHasSubquery(1))
	{
		return CXform::ExfpNone;
	}
	return CXform::ExfpHigh;
}

//---------------------------------------------------------------------------
//	@function:
//		CXformImplementDynamicBitmapTableGet::Transform
//
//	@doc:
//		Actual transformation
//
//---------------------------------------------------------------------------
void
CXformImplementDynamicBitmapTableGet::Transform(CXformContext *pxfctxt,
												CXformResult *pxfres,
												CExpression *pexpr) const
{
	SPQOS_ASSERT(NULL != pxfctxt);
	SPQOS_ASSERT(FPromising(pxfctxt->Pmp(), this, pexpr));
	SPQOS_ASSERT(FCheckPattern(pexpr));

	CMemoryPool *mp = pxfctxt->Pmp();
	CLogicalDynamicBitmapTableGet *popLogical =
		CLogicalDynamicBitmapTableGet::PopConvert(pexpr->Pop());

	CTableDescriptor *ptabdesc = popLogical->Ptabdesc();
	ptabdesc->AddRef();

	CName *pname = SPQOS_NEW(mp) CName(mp, popLogical->Name());

	CColRefArray *pdrspqcrOutput = popLogical->PdrspqcrOutput();

	SPQOS_ASSERT(NULL != pdrspqcrOutput);
	pdrspqcrOutput->AddRef();

	CColRef2dArray *pdrspqdrspqcrPart = popLogical->PdrspqdrspqcrPart();
	pdrspqdrspqcrPart->AddRef();

	CPartConstraint *ppartcnstr = popLogical->Ppartcnstr();
	ppartcnstr->AddRef();

	CPartConstraint *ppartcnstrRel = popLogical->PpartcnstrRel();
	ppartcnstrRel->AddRef();

	CPhysicalDynamicBitmapTableScan *popPhysical =
		SPQOS_NEW(mp) CPhysicalDynamicBitmapTableScan(
			mp, popLogical->IsPartial(), ptabdesc, pexpr->Pop()->UlOpId(),
			pname, popLogical->ScanId(), pdrspqcrOutput, pdrspqdrspqcrPart,
			popLogical->UlSecondaryScanId(), ppartcnstr, ppartcnstrRel);

	CExpression *pexprCondition = (*pexpr)[0];
	CExpression *pexprIndexPath = (*pexpr)[1];
	pexprCondition->AddRef();
	pexprIndexPath->AddRef();

	CExpression *pexprPhysical = SPQOS_NEW(mp)
		CExpression(mp, popPhysical, pexprCondition, pexprIndexPath);
	pxfres->Add(pexprPhysical);
}

// EOF
