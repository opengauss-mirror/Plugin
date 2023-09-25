//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CXformImplementRowTrigger.cpp
//
//	@doc:
//		Implementation of transform
//---------------------------------------------------------------------------

#include "spqopt/xforms/CXformImplementRowTrigger.h"

#include "spqos/base.h"

#include "spqopt/operators/CLogicalRowTrigger.h"
#include "spqopt/operators/CPatternLeaf.h"
#include "spqopt/operators/CPhysicalRowTrigger.h"

using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CXformImplementRowTrigger::CXformImplementRowTrigger
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CXformImplementRowTrigger::CXformImplementRowTrigger(CMemoryPool *mp)
	: CXformImplementation(
		  // pattern
		  SPQOS_NEW(mp) CExpression(
			  mp, SPQOS_NEW(mp) CLogicalRowTrigger(mp),
			  SPQOS_NEW(mp) CExpression(mp, SPQOS_NEW(mp) CPatternLeaf(mp))))
{
}

//---------------------------------------------------------------------------
//	@function:
//		CXformImplementRowTrigger::Exfp
//
//	@doc:
//		Compute promise of xform
//
//---------------------------------------------------------------------------
CXform::EXformPromise
CXformImplementRowTrigger::Exfp(CExpressionHandle &	 // exprhdl
) const
{
	return CXform::ExfpHigh;
}


//---------------------------------------------------------------------------
//	@function:
//		CXformImplementRowTrigger::Transform
//
//	@doc:
//		Actual transformation
//
//---------------------------------------------------------------------------
void
CXformImplementRowTrigger::Transform(CXformContext *pxfctxt,
									 CXformResult *pxfres,
									 CExpression *pexpr) const
{
	SPQOS_ASSERT(NULL != pxfctxt);
	SPQOS_ASSERT(FPromising(pxfctxt->Pmp(), this, pexpr));
	SPQOS_ASSERT(FCheckPattern(pexpr));

	CLogicalRowTrigger *popRowTrigger =
		CLogicalRowTrigger::PopConvert(pexpr->Pop());
	CMemoryPool *mp = pxfctxt->Pmp();

	// extract components for alternative
	IMDId *rel_mdid = popRowTrigger->GetRelMdId();
	rel_mdid->AddRef();

	INT type = popRowTrigger->GetType();

	CColRefArray *pdrspqcrOld = popRowTrigger->PdrspqcrOld();
	if (NULL != pdrspqcrOld)
	{
		pdrspqcrOld->AddRef();
	}

	CColRefArray *pdrspqcrNew = popRowTrigger->PdrspqcrNew();
	if (NULL != pdrspqcrNew)
	{
		pdrspqcrNew->AddRef();
	}

	// child of RowTrigger operator
	CExpression *pexprChild = (*pexpr)[0];
	pexprChild->AddRef();

	// create physical RowTrigger
	CExpression *pexprAlt = SPQOS_NEW(mp) CExpression(
		mp,
		SPQOS_NEW(mp)
			CPhysicalRowTrigger(mp, rel_mdid, type, pdrspqcrOld, pdrspqcrNew),
		pexprChild);
	// add alternative to transformation result
	pxfres->Add(pexprAlt);
}

// EOF
