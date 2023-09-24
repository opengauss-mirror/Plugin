//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CXformJoinCommutativity.cpp
//
//	@doc:
//		Implementation of commutativity transform
//---------------------------------------------------------------------------

#include "spqopt/xforms/CXformJoinCommutativity.h"

#include "spqos/base.h"

#include "spqopt/metadata/CTableDescriptor.h"
#include "spqopt/operators/CLogicalInnerJoin.h"
#include "spqopt/operators/CPatternLeaf.h"

using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CXformJoinCommutativity::CXformJoinCommutativity
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CXformJoinCommutativity::CXformJoinCommutativity(CMemoryPool *mp)
	: CXformExploration(
		  // pattern
		  SPQOS_NEW(mp) CExpression(
			  mp, SPQOS_NEW(mp) CLogicalInnerJoin(mp),
			  SPQOS_NEW(mp)
				  CExpression(mp, SPQOS_NEW(mp) CPatternLeaf(mp)),  // left child
			  SPQOS_NEW(mp) CExpression(
				  mp, SPQOS_NEW(mp) CPatternLeaf(mp)),  // right child
			  SPQOS_NEW(mp)
				  CExpression(mp, SPQOS_NEW(mp) CPatternLeaf(mp)))  // predicate
	  )
{
}


//---------------------------------------------------------------------------
//	@function:
//		CXformJoinCommutativity::FCompatible
//
//	@doc:
//		Compatibility function for join commutativity
//
//---------------------------------------------------------------------------
BOOL
CXformJoinCommutativity::FCompatible(CXform::EXformId exfid)
{
	BOOL fCompatible = true;

	switch (exfid)
	{
		case CXform::ExfJoinCommutativity:
			fCompatible = false;
			break;
		default:
			fCompatible = true;
	}

	return fCompatible;
}


//---------------------------------------------------------------------------
//	@function:
//		CXformJoinCommutativity::Transform
//
//	@doc:
//		Actual transformation
//
//---------------------------------------------------------------------------
void
CXformJoinCommutativity::Transform(CXformContext *pxfctxt, CXformResult *pxfres,
								   CExpression *pexpr) const
{
	SPQOS_ASSERT(NULL != pxfctxt);
	SPQOS_ASSERT(FPromising(pxfctxt->Pmp(), this, pexpr));
	SPQOS_ASSERT(FCheckPattern(pexpr));

	CMemoryPool *mp = pxfctxt->Pmp();

	// extract components
	CExpression *pexprLeft = (*pexpr)[0];
	CExpression *pexprRight = (*pexpr)[1];
	CExpression *pexprScalar = (*pexpr)[2];

	// addref children
	pexprLeft->AddRef();
	pexprRight->AddRef();
	pexprScalar->AddRef();

	// assemble transformed expression
	CExpression *pexprAlt = CUtils::PexprLogicalJoin<CLogicalInnerJoin>(
		mp, pexprRight, pexprLeft, pexprScalar);

	// add alternative to transformation result
	pxfres->Add(pexprAlt);
}

// EOF
