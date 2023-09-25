//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 Pivotal, Inc.
//
//	@filename:
//		CXformSplitGbAggDedup.cpp
//
//	@doc:
//		Implementation of the splitting of a dedup aggregate into a pair of
//		local and global aggregates
//---------------------------------------------------------------------------

#include "spqopt/xforms/CXformSplitGbAggDedup.h"

#include "spqos/base.h"

#include "spqopt/base/CColRefComputed.h"
#include "spqopt/base/CUtils.h"
#include "spqopt/operators/CLogicalGbAggDeduplicate.h"
#include "spqopt/operators/COperator.h"
#include "spqopt/translate/CTranslatorDXLToExpr.h"
#include "naucrates/md/IMDAggregate.h"

using namespace spqmd;
using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CXformSplitGbAggDedup::CXformSplitGbAggDedup
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CXformSplitGbAggDedup::CXformSplitGbAggDedup(CMemoryPool *mp)
	: CXformSplitGbAgg(
		  // pattern
		  SPQOS_NEW(mp) CExpression(
			  mp, SPQOS_NEW(mp) CLogicalGbAggDeduplicate(mp),
			  SPQOS_NEW(mp) CExpression(
				  mp, SPQOS_NEW(mp) CPatternLeaf(mp)),  // relational child
			  SPQOS_NEW(mp) CExpression(
				  mp, SPQOS_NEW(mp) CPatternTree(mp))  // scalar project list
			  ))
{
}

//---------------------------------------------------------------------------
//	@function:
//		CXformSplitGbAggDedup::Transform
//
//	@doc:
//		Actual transformation to expand a global aggregate into a pair of
//		local and global aggregate
//
//---------------------------------------------------------------------------
void
CXformSplitGbAggDedup::Transform(CXformContext *pxfctxt, CXformResult *pxfres,
								 CExpression *pexpr) const
{
	SPQOS_ASSERT(NULL != pxfctxt);
	SPQOS_ASSERT(NULL != pxfres);
	SPQOS_ASSERT(FPromising(pxfctxt->Pmp(), this, pexpr));
	SPQOS_ASSERT(FCheckPattern(pexpr));

	CMemoryPool *mp = pxfctxt->Pmp();
	CLogicalGbAggDeduplicate *popAggDedup =
		CLogicalGbAggDeduplicate::PopConvert(pexpr->Pop());

	// extract components
	CExpression *pexprRelational = (*pexpr)[0];
	CExpression *pexprProjectList = (*pexpr)[1];

	// check if the transformation is applicable
	if (!FApplicable(pexprProjectList))
	{
		return;
	}

	pexprRelational->AddRef();

	CExpression *pexprProjectListLocal = NULL;
	CExpression *pexprProjectListGlobal = NULL;

	(void) PopulateLocalGlobalProjectList(
		mp, pexprProjectList, &pexprProjectListLocal, &pexprProjectListGlobal);
	SPQOS_ASSERT(NULL != pexprProjectListLocal && NULL != pexprProjectListLocal);

	CColRefArray *colref_array = popAggDedup->Pdrspqcr();
	colref_array->AddRef();
	colref_array->AddRef();

	CColRefArray *pdrspqcrMinimal = popAggDedup->PdrspqcrMinimal();
	if (NULL != pdrspqcrMinimal)
	{
		pdrspqcrMinimal->AddRef();
		pdrspqcrMinimal->AddRef();
	}

	CColRefArray *pdrspqcrKeys = popAggDedup->PdrspqcrKeys();
	pdrspqcrKeys->AddRef();
	pdrspqcrKeys->AddRef();

	CExpression *local_expr = SPQOS_NEW(mp)
		CExpression(mp,
					SPQOS_NEW(mp) CLogicalGbAggDeduplicate(
						mp, colref_array, pdrspqcrMinimal,
						COperator::EgbaggtypeLocal /*egbaggtype*/, pdrspqcrKeys),
					pexprRelational, pexprProjectListLocal);

	CExpression *pexprGlobal = SPQOS_NEW(mp) CExpression(
		mp,
		SPQOS_NEW(mp) CLogicalGbAggDeduplicate(
			mp, colref_array, pdrspqcrMinimal,
			COperator::EgbaggtypeGlobal /*egbaggtype*/, pdrspqcrKeys),
		local_expr, pexprProjectListGlobal);

	pxfres->Add(pexprGlobal);
}

// EOF
