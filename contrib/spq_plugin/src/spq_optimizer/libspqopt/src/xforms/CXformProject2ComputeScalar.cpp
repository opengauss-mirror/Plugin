//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CXformProject2ComputeScalar.cpp
//
//	@doc:
//		Implementation of transform
//---------------------------------------------------------------------------

#include "spqopt/xforms/CXformProject2ComputeScalar.h"

#include "spqos/base.h"

#include "spqopt/operators/CLogicalProject.h"
#include "spqopt/operators/CPatternLeaf.h"
#include "spqopt/operators/CPhysicalComputeScalar.h"


using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CXformProject2ComputeScalar::CXformProject2ComputeScalar
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CXformProject2ComputeScalar::CXformProject2ComputeScalar(CMemoryPool *mp)
	:  // pattern
	  CXformImplementation(SPQOS_NEW(mp) CExpression(
		  mp, SPQOS_NEW(mp) CLogicalProject(mp),
		  SPQOS_NEW(mp) CExpression(
			  mp, SPQOS_NEW(mp) CPatternLeaf(mp)),  // relational child
		  SPQOS_NEW(mp)
			  CExpression(mp, SPQOS_NEW(mp) CPatternLeaf(mp))  // scalar child
		  ))
{
}


//---------------------------------------------------------------------------
//	@function:
//		CXformProject2ComputeScalar::Transform
//
//	@doc:
//		Actual transformation
//
//---------------------------------------------------------------------------
void
CXformProject2ComputeScalar::Transform(CXformContext *pxfctxt,
									   CXformResult *pxfres,
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
	CExpression *pexprComputeScalar =
		SPQOS_NEW(mp) CExpression(mp, SPQOS_NEW(mp) CPhysicalComputeScalar(mp),
								 pexprRelational, pexprScalar);

	// add alternative to results
	pxfres->Add(pexprComputeScalar);
}


// EOF
