//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal, Inc.
//
//	@filename:
//		CXformImplementBitmapTableGet.cpp
//
//	@doc:
//		Implement BitmapTableGet
//
//	@owner:
//
//
//	@test:
//
//---------------------------------------------------------------------------

#include "spqopt/xforms/CXformImplementBitmapTableGet.h"

#include "spqopt/metadata/CTableDescriptor.h"
#include "spqopt/operators/CLogicalBitmapTableGet.h"
#include "spqopt/operators/CPatternLeaf.h"
#include "spqopt/operators/CPhysicalBitmapTableScan.h"

using namespace spqopt;
using namespace spqos;

//---------------------------------------------------------------------------
//	@function:
//		CXformImplementBitmapTableGet::CXformImplementBitmapTableGet
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CXformImplementBitmapTableGet::CXformImplementBitmapTableGet(CMemoryPool *mp)
	:  // pattern
	  CXformImplementation(SPQOS_NEW(mp) CExpression(
		  mp, SPQOS_NEW(mp) CLogicalBitmapTableGet(mp),
		  SPQOS_NEW(mp)
			  CExpression(mp, SPQOS_NEW(mp) CPatternLeaf(mp)),  // predicate tree
		  SPQOS_NEW(mp) CExpression(
			  mp, SPQOS_NEW(mp) CPatternLeaf(mp))  // bitmap index expression
		  ))
{
}

CXform::EXformPromise
CXformImplementBitmapTableGet::Exfp(CExpressionHandle &exprhdl) const
{
	if (exprhdl.DeriveHasSubquery(0) || exprhdl.DeriveHasSubquery(1))
	{
		return CXform::ExfpNone;
	}


	return CXform::ExfpHigh;
}

//---------------------------------------------------------------------------
//	@function:
//		CXformImplementBitmapTableGet::Transform
//
//	@doc:
//		Actual transformation
//
//---------------------------------------------------------------------------
void
CXformImplementBitmapTableGet::Transform(CXformContext *pxfctxt,
										 CXformResult *pxfres,
										 CExpression *pexpr) const
{
	SPQOS_ASSERT(NULL != pxfctxt);
	SPQOS_ASSERT(FPromising(pxfctxt->Pmp(), this, pexpr));
	SPQOS_ASSERT(FCheckPattern(pexpr));

	CMemoryPool *mp = pxfctxt->Pmp();
	CLogicalBitmapTableGet *popLogical =
		CLogicalBitmapTableGet::PopConvert(pexpr->Pop());

	CTableDescriptor *ptabdesc = popLogical->Ptabdesc();
	ptabdesc->AddRef();

	CColRefArray *pdrspqcrOutput = popLogical->PdrspqcrOutput();
	pdrspqcrOutput->AddRef();

	CPhysicalBitmapTableScan *popPhysical = SPQOS_NEW(mp)
		CPhysicalBitmapTableScan(mp, ptabdesc, pexpr->Pop()->UlOpId(),
								 SPQOS_NEW(mp)
									 CName(mp, *popLogical->PnameTableAlias()),
								 pdrspqcrOutput);

	CExpression *pexprCondition = (*pexpr)[0];
	CExpression *pexprIndexPath = (*pexpr)[1];
	pexprCondition->AddRef();
	pexprIndexPath->AddRef();

	CExpression *pexprPhysical = SPQOS_NEW(mp)
		CExpression(mp, popPhysical, pexprCondition, pexprIndexPath);
	pxfres->Add(pexprPhysical);
}

// EOF
