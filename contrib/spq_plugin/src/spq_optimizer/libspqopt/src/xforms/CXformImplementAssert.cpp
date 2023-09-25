//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CXformImplementAssert.cpp
//
//	@doc:
//		Implementation of transform
//---------------------------------------------------------------------------

#include "spqopt/xforms/CXformImplementAssert.h"

#include "spqos/base.h"

#include "spqopt/operators/CLogicalAssert.h"
#include "spqopt/operators/CPatternLeaf.h"
#include "spqopt/operators/CPhysicalAssert.h"

using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CXformImplementAssert::CXformImplementAssert
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CXformImplementAssert::CXformImplementAssert(CMemoryPool *mp)
	:  // pattern
	  CXformImplementation(SPQOS_NEW(mp) CExpression(
		  mp, SPQOS_NEW(mp) CLogicalAssert(mp),
		  SPQOS_NEW(mp) CExpression(
			  mp, SPQOS_NEW(mp) CPatternLeaf(mp)),  // relational child
		  SPQOS_NEW(mp)
			  CExpression(mp, SPQOS_NEW(mp) CPatternLeaf(mp))  // predicate
		  ))
{
}


//---------------------------------------------------------------------------
//	@function:
//		CXformImplementAssert::Exfp
//
//	@doc:
//		Compute xform promise level for a given expression handle;
//
//---------------------------------------------------------------------------
CXform::EXformPromise
CXformImplementAssert::Exfp(CExpressionHandle &exprhdl) const
{
	if (exprhdl.DeriveHasSubquery(1))
	{
		return CXform::ExfpNone;
	}

	return CXform::ExfpHigh;
}


//---------------------------------------------------------------------------
//	@function:
//		CXformImplementAssert::Transform
//
//	@doc:
//		Actual transformation
//
//---------------------------------------------------------------------------
void
CXformImplementAssert::Transform(CXformContext *pxfctxt, CXformResult *pxfres,
								 CExpression *pexpr) const
{
	SPQOS_ASSERT(NULL != pxfctxt);
	SPQOS_ASSERT(FPromising(pxfctxt->Pmp(), this, pexpr));
	SPQOS_ASSERT(FCheckPattern(pexpr));

	CMemoryPool *mp = pxfctxt->Pmp();

	// extract components
	CLogicalAssert *popAssert = CLogicalAssert::PopConvert(pexpr->Pop());
	CExpression *pexprRelational = (*pexpr)[0];
	CExpression *pexprScalar = (*pexpr)[1];
	CException *pexc = popAssert->Pexc();

	// addref all children
	pexprRelational->AddRef();
	pexprScalar->AddRef();

	// assemble physical operator
	CPhysicalAssert *popPhysicalAssert = SPQOS_NEW(mp) CPhysicalAssert(
		mp, SPQOS_NEW(mp) CException(pexc->Major(), pexc->Minor(),
									pexc->Filename(), pexc->Line()));

	CExpression *pexprAssert = SPQOS_NEW(mp)
		CExpression(mp, popPhysicalAssert, pexprRelational, pexprScalar);

	// add alternative to results
	pxfres->Add(pexprAssert);
}


// EOF
