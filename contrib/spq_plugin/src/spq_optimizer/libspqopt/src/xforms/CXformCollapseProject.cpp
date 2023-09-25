//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2015 Pivotal Inc.
//
//	@filename:
//		CXformCollapseProject.cpp
//
//	@doc:
//		Implementation of the transform that collapses two cascaded project nodes
//---------------------------------------------------------------------------

#include "spqopt/xforms/CXformCollapseProject.h"

#include "spqos/base.h"

#include "spqopt/operators/CLogicalProject.h"
#include "spqopt/operators/CPatternLeaf.h"
#include "spqopt/operators/CPatternTree.h"

using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CXformCollapseProject::CXformCollapseProject
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CXformCollapseProject::CXformCollapseProject(CMemoryPool *mp)
	: CXformExploration(
		  // pattern
		  SPQOS_NEW(mp) CExpression(
			  mp, SPQOS_NEW(mp) CLogicalProject(mp),
			  SPQOS_NEW(mp) CExpression(
				  mp, SPQOS_NEW(mp) CLogicalProject(mp),
				  SPQOS_NEW(mp) CExpression(
					  mp, SPQOS_NEW(mp) CPatternLeaf(mp)),  // relational child
				  SPQOS_NEW(mp) CExpression(
					  mp, SPQOS_NEW(mp) CPatternTree(mp))  // scalar project list
				  ),
			  SPQOS_NEW(mp) CExpression(
				  mp, SPQOS_NEW(mp) CPatternTree(mp))  // scalar project list
			  ))
{
}



//---------------------------------------------------------------------------
//	@function:
//		CXformSplitDQA::Exfp
//
//	@doc:
//		Compute xform promise for a given expression handle;
//
//---------------------------------------------------------------------------
CXform::EXformPromise
CXformCollapseProject::Exfp(CExpressionHandle &	 //exprhdl
) const
{
	return CXform::ExfpHigh;
}


//---------------------------------------------------------------------------
//	@function:
//		CXformCollapseProject::Transform
//
//	@doc:
//		Actual transformation to collapse projects
//
//---------------------------------------------------------------------------
void
CXformCollapseProject::Transform(CXformContext *pxfctxt, CXformResult *pxfres,
								 CExpression *pexpr) const
{
	SPQOS_ASSERT(NULL != pxfctxt);
	SPQOS_ASSERT(NULL != pxfres);
	SPQOS_ASSERT(FCheckPattern(pexpr));

	CMemoryPool *mp = pxfctxt->Pmp();

	CExpression *pexprCollapsed = CUtils::PexprCollapseProjects(mp, pexpr);

	if (NULL != pexprCollapsed)
	{
		pxfres->Add(pexprCollapsed);
	}
}

// EOF
