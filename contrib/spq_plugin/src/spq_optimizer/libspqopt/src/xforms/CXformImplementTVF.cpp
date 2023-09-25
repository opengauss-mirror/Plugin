//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CXformImplementTVF.cpp
//
//	@doc:
//		Implementation of transform
//---------------------------------------------------------------------------

#include "spqopt/xforms/CXformImplementTVF.h"

#include "spqos/base.h"

#include "spqopt/operators/CExpressionHandle.h"
#include "spqopt/operators/CLogicalTVF.h"
#include "spqopt/operators/CPatternMultiLeaf.h"
#include "spqopt/operators/CPhysicalTVF.h"

using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CXformImplementTVF::CXformImplementTVF
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CXformImplementTVF::CXformImplementTVF(CMemoryPool *mp)
	: CXformImplementation(
		  // pattern
		  SPQOS_NEW(mp) CExpression(
			  mp, SPQOS_NEW(mp) CLogicalTVF(mp),
			  SPQOS_NEW(mp) CExpression(mp, SPQOS_NEW(mp) CPatternMultiLeaf(mp))))
{
}

//---------------------------------------------------------------------------
//	@function:
//		CXformImplementTVF::CXformImplementTVF
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CXformImplementTVF::CXformImplementTVF(CExpression *pexpr)
	: CXformImplementation(pexpr)
{
}


//---------------------------------------------------------------------------
//	@function:
//		CXformImplementTVF::Exfp
//
//	@doc:
//		Compute xform promise for a given expression handle
//
//---------------------------------------------------------------------------
CXform::EXformPromise
CXformImplementTVF::Exfp(CExpressionHandle &exprhdl) const
{
	const ULONG arity = exprhdl.Arity();
	for (ULONG ul = 0; ul < arity; ul++)
	{
		if (exprhdl.DeriveHasSubquery(ul))
		{
			// xform is inapplicable if TVF argument is a subquery
			return CXform::ExfpNone;
		}
	}

	return CXform::ExfpHigh;
}


//---------------------------------------------------------------------------
//	@function:
//		CXformImplementTVF::Transform
//
//	@doc:
//		Actual transformation
//
//---------------------------------------------------------------------------
void
CXformImplementTVF::Transform(CXformContext *pxfctxt, CXformResult *pxfres,
							  CExpression *pexpr) const
{
	SPQOS_ASSERT(NULL != pxfctxt);
	SPQOS_ASSERT(FPromising(pxfctxt->Pmp(), this, pexpr));
	SPQOS_ASSERT(FCheckPattern(pexpr));

	CLogicalTVF *popTVF = CLogicalTVF::PopConvert(pexpr->Pop());
	CMemoryPool *mp = pxfctxt->Pmp();

	// create/extract components for alternative
	IMDId *mdid_func = popTVF->FuncMdId();
	mdid_func->AddRef();

	IMDId *mdid_return_type = popTVF->ReturnTypeMdId();
	mdid_return_type->AddRef();

	CWStringConst *str =
		SPQOS_NEW(mp) CWStringConst(popTVF->Pstr()->GetBuffer());

	CColumnDescriptorArray *pdrspqcoldesc = popTVF->Pdrspqcoldesc();
	pdrspqcoldesc->AddRef();

	CColRefArray *pdrspqcrOutput = popTVF->PdrspqcrOutput();
	CColRefSet *pcrs = SPQOS_NEW(mp) CColRefSet(mp);
	pcrs->Include(pdrspqcrOutput);

	CExpressionArray *pdrspqexpr = pexpr->PdrgPexpr();

	CPhysicalTVF *pphTVF = SPQOS_NEW(mp)
		CPhysicalTVF(mp, mdid_func, mdid_return_type, str, pdrspqcoldesc, pcrs);

	CExpression *pexprAlt = NULL;
	// create alternative expression
	if (NULL == pdrspqexpr || 0 == pdrspqexpr->Size())
	{
		pexprAlt = SPQOS_NEW(mp) CExpression(mp, pphTVF);
	}
	else
	{
		pdrspqexpr->AddRef();
		pexprAlt = SPQOS_NEW(mp) CExpression(mp, pphTVF, pdrspqexpr);
	}

	// add alternative to transformation result
	pxfres->Add(pexprAlt);
}


// EOF
