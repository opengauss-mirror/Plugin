//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CXformSelect2Filter.cpp
//
//	@doc:
//		Implementation of transform
//---------------------------------------------------------------------------

#include "spqopt/xforms/CXformSelect2Filter.h"

#include "spqos/base.h"

#include "spqopt/operators/CLogicalSelect.h"
#include "spqopt/operators/CPatternLeaf.h"
#include "spqopt/operators/CPhysicalFilter.h"

using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CXformSelect2Filter::CXformSelect2Filter
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CXformSelect2Filter::CXformSelect2Filter(CMemoryPool *mp)
	:  // pattern
	  CXformImplementation(SPQOS_NEW(mp) CExpression(
		  mp, SPQOS_NEW(mp) CLogicalSelect(mp),
		  SPQOS_NEW(mp) CExpression(
			  mp, SPQOS_NEW(mp) CPatternLeaf(mp)),  // relational child
		  SPQOS_NEW(mp)
			  CExpression(mp, SPQOS_NEW(mp) CPatternLeaf(mp))  // predicate
		  ))
{
}


//---------------------------------------------------------------------------
//	@function:
//		CXformSelect2Filter::Exfp
//
//	@doc:
//		Compute xform promise level for a given expression handle;
// 		if scalar predicate has a subquery, then we must have an
// 		equivalent logical Apply expression created during exploration;
// 		no need for generating a Filter expression here
//
//---------------------------------------------------------------------------
CXform::EXformPromise
CXformSelect2Filter::Exfp(CExpressionHandle &exprhdl) const
{
	if (exprhdl.DeriveHasSubquery(1))
	{
		return CXform::ExfpNone;
	}

	return CXform::ExfpHigh;
}


//---------------------------------------------------------------------------
//	@function:
//		CXformSelect2Filter::Transform
//
//	@doc:
//		Actual transformation
//
//---------------------------------------------------------------------------
void
CXformSelect2Filter::Transform(CXformContext *pxfctxt, CXformResult *pxfres,
							   CExpression *pexpr) const
{
	SPQOS_ASSERT(NULL != pxfctxt);
	SPQOS_ASSERT(FPromising(pxfctxt->Pmp(), this, pexpr));
	SPQOS_ASSERT(FCheckPattern(pexpr));

	CMemoryPool *mp = pxfctxt->Pmp();

	// extract components
	CExpression *pexprRelational = (*pexpr)[0];
	CExpression *pexprScalar = (*pexpr)[1];

	// addref all children
	pexprRelational->AddRef();
	pexprScalar->AddRef();

	// assemble physical operator
	CExpression *pexprFilter = SPQOS_NEW(mp) CExpression(
		mp, SPQOS_NEW(mp) CPhysicalFilter(mp), pexprRelational, pexprScalar);

	// add alternative to results
	pxfres->Add(pexprFilter);
}


// EOF
