//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CXformImplementLimit.cpp
//
//	@doc:
//		Implementation of limit operator
//---------------------------------------------------------------------------

#include "spqopt/xforms/CXformImplementLimit.h"

#include "spqos/base.h"

#include "spqopt/operators/CExpressionHandle.h"
#include "spqopt/operators/CLogicalLimit.h"
#include "spqopt/operators/CPatternLeaf.h"
#include "spqopt/operators/CPhysicalLimit.h"

using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CXformImplementLimit::CXformImplementLimit
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CXformImplementLimit::CXformImplementLimit(CMemoryPool *mp)
	:  // pattern
	  CXformImplementation(SPQOS_NEW(mp) CExpression(
		  mp, SPQOS_NEW(mp) CLogicalLimit(mp),
		  SPQOS_NEW(mp) CExpression(
			  mp, SPQOS_NEW(mp) CPatternLeaf(mp)),  // relational child
		  SPQOS_NEW(mp) CExpression(
			  mp, SPQOS_NEW(mp) CPatternLeaf(mp)),  // scalar child for offset
		  SPQOS_NEW(mp) CExpression(
			  mp,
			  SPQOS_NEW(mp) CPatternLeaf(mp))  // scalar child for number of rows

		  ))
{
}

CXform::EXformPromise
CXformImplementLimit::Exfp(CExpressionHandle &exprhdl) const
{
	// Although it is valid SQL for the limit/offset to be a subquery, Orca does
	// not support it
	if (exprhdl.DeriveHasSubquery(1) || exprhdl.DeriveHasSubquery(2))
	{
		return CXform::ExfpNone;
	}
	return CXform::ExfpHigh;
}

//---------------------------------------------------------------------------
//	@function:
//		CXformImplementLimit::Transform
//
//	@doc:
//		Actual transformation
//
//---------------------------------------------------------------------------
void
CXformImplementLimit::Transform(CXformContext *pxfctxt, CXformResult *pxfres,
								CExpression *pexpr) const
{
	SPQOS_ASSERT(NULL != pxfctxt);
	SPQOS_ASSERT(FPromising(pxfctxt->Pmp(), this, pexpr));
	SPQOS_ASSERT(FCheckPattern(pexpr));

	CMemoryPool *mp = pxfctxt->Pmp();

	// extract components
	CLogicalLimit *popLimit = CLogicalLimit::PopConvert(pexpr->Pop());
	CExpression *pexprRelational = (*pexpr)[0];
	CExpression *pexprScalarStart = (*pexpr)[1];
	CExpression *pexprScalarRows = (*pexpr)[2];
	COrderSpec *pos = popLimit->Pos();

	// addref all components
	pexprRelational->AddRef();
	pexprScalarStart->AddRef();
	pexprScalarRows->AddRef();
	popLimit->Pos()->AddRef();

	// assemble physical operator
	CExpression *pexprLimit = SPQOS_NEW(mp) CExpression(
		mp,
		SPQOS_NEW(mp)
			CPhysicalLimit(mp, pos, popLimit->FGlobal(), popLimit->FHasCount(),
						   popLimit->IsTopLimitUnderDMLorCTAS()),
		pexprRelational, pexprScalarStart, pexprScalarRows);

	// add alternative to results
	pxfres->Add(pexprLimit);
}


// EOF
