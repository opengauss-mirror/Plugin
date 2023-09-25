//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CXformImplementSequenceProject.cpp
//
//	@doc:
//		Implementation of transform
//---------------------------------------------------------------------------

#include "spqopt/xforms/CXformImplementSequenceProject.h"

#include "spqos/base.h"

#include "spqopt/operators/CLogicalSequenceProject.h"
#include "spqopt/operators/CPatternLeaf.h"
#include "spqopt/operators/CPhysicalSequenceProject.h"


using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CXformImplementSequenceProject::CXformImplementSequenceProject
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CXformImplementSequenceProject::CXformImplementSequenceProject(CMemoryPool *mp)
	:  // pattern
	  CXformImplementation(SPQOS_NEW(mp) CExpression(
		  mp, SPQOS_NEW(mp) CLogicalSequenceProject(mp),
		  SPQOS_NEW(mp) CExpression(
			  mp, SPQOS_NEW(mp) CPatternLeaf(mp)),  // relational child
		  SPQOS_NEW(mp)
			  CExpression(mp, SPQOS_NEW(mp) CPatternLeaf(mp))  // scalar child
		  ))
{
}


//---------------------------------------------------------------------------
//	@function:
//		CXformImplementSequenceProject::Transform
//
//	@doc:
//		Actual transformation
//
//---------------------------------------------------------------------------
void
CXformImplementSequenceProject::Transform(CXformContext *pxfctxt,
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

	// extract members of logical sequence project operator
	CLogicalSequenceProject *popLogicalSequenceProject =
		CLogicalSequenceProject::PopConvert(pexpr->Pop());
	CDistributionSpec *pds = popLogicalSequenceProject->Pds();
	COrderSpecArray *pdrspqos = popLogicalSequenceProject->Pdrspqos();
	CWindowFrameArray *pdrspqwf = popLogicalSequenceProject->Pdrspqwf();
	pds->AddRef();
	pdrspqos->AddRef();
	pdrspqwf->AddRef();

	// assemble physical operator
	CExpression *pexprSequenceProject = SPQOS_NEW(mp) CExpression(
		mp, SPQOS_NEW(mp) CPhysicalSequenceProject(mp, pds, pdrspqos, pdrspqwf),
		pexprRelational, pexprScalar);

	// add alternative to results
	pxfres->Add(pexprSequenceProject);
}


// EOF
