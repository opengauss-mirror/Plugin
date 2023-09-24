//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 EMC Corp.
//
//	@filename:
//		CXformCollapseGbAgg.cpp
//
//	@doc:
//		Implementation of collapsing two cascaded group-by operators
//		into a single group-by
//---------------------------------------------------------------------------

#include "spqopt/xforms/CXformCollapseGbAgg.h"

#include "spqos/base.h"

#include "spqopt/base/CUtils.h"
#include "spqopt/operators/CLogicalGbAgg.h"
#include "spqopt/operators/COperator.h"
#include "spqopt/operators/CPatternLeaf.h"
#include "spqopt/operators/CPredicateUtils.h"

using namespace spqmd;
using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CXformCollapseGbAgg::CXformCollapseGbAgg
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CXformCollapseGbAgg::CXformCollapseGbAgg(CMemoryPool *mp)
	: CXformExploration(
		  // pattern
		  SPQOS_NEW(mp) CExpression(
			  mp, SPQOS_NEW(mp) CLogicalGbAgg(mp),
			  SPQOS_NEW(mp) CExpression(
				  mp, SPQOS_NEW(mp) CLogicalGbAgg(mp),
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
//		CXformCollapseGbAgg::Exfp
//
//	@doc:
//		Compute xform promise for a given expression handle;
//		GbAgg must be global and have non empty grouping columns
//
//---------------------------------------------------------------------------
CXform::EXformPromise
CXformCollapseGbAgg::Exfp(CExpressionHandle &exprhdl) const
{
	CLogicalGbAgg *popAgg = CLogicalGbAgg::PopConvert(exprhdl.Pop());
	if (!popAgg->FGlobal() || 0 == popAgg->Pdrspqcr()->Size())
	{
		return CXform::ExfpNone;
	}

	return CXform::ExfpHigh;
}


//---------------------------------------------------------------------------
//	@function:
//		CXformCollapseGbAgg::Transform
//
//	@doc:
//		Actual transformation to collapse two cascaded group by operators;
//		if the top Gb grouping columns are subset of bottom Gb grouping
//		columns AND both Gb operators do not define agg functions, we can
//		remove the bottom group by operator
//
//
//---------------------------------------------------------------------------
void
CXformCollapseGbAgg::Transform(CXformContext *pxfctxt, CXformResult *pxfres,
							   CExpression *pexpr) const
{
	SPQOS_ASSERT(NULL != pxfctxt);
	SPQOS_ASSERT(NULL != pxfres);
	SPQOS_ASSERT(FPromising(pxfctxt->Pmp(), this, pexpr));
	SPQOS_ASSERT(FCheckPattern(pexpr));

	CMemoryPool *mp = pxfctxt->Pmp();

	// extract components
	CLogicalGbAgg *popTopGbAgg = CLogicalGbAgg::PopConvert(pexpr->Pop());
	SPQOS_ASSERT(0 < popTopGbAgg->Pdrspqcr()->Size());
	SPQOS_ASSERT(popTopGbAgg->FGlobal());

	CExpression *pexprRelational = (*pexpr)[0];
	CExpression *pexprTopProjectList = (*pexpr)[1];

	CLogicalGbAgg *popBottomGbAgg =
		CLogicalGbAgg::PopConvert(pexprRelational->Pop());
	CExpression *pexprChild = (*pexprRelational)[0];
	CExpression *pexprBottomProjectList = (*pexprRelational)[1];

	if (!popBottomGbAgg->FGlobal())
	{
		// bottom GbAgg must be global to prevent xform from getting applied to splitted GbAggs
		return;
	}

	if (0 < pexprTopProjectList->Arity() || 0 < pexprBottomProjectList->Arity())
	{
		// exit if any of the Gb operators has an aggregate function
		return;
	}

#ifdef SPQOS_DEBUG
	// for two cascaded GbAgg ops with no agg functions, top grouping
	// columns must be a subset of bottom grouping columns
	CColRefSet *pcrsTopGrpCols =
		SPQOS_NEW(mp) CColRefSet(mp, popTopGbAgg->Pdrspqcr());
	CColRefSet *pcrsBottomGrpCols =
		SPQOS_NEW(mp) CColRefSet(mp, popBottomGbAgg->Pdrspqcr());
	SPQOS_ASSERT(pcrsBottomGrpCols->ContainsAll(pcrsTopGrpCols));

	pcrsTopGrpCols->Release();
	pcrsBottomGrpCols->Release();
#endif	// SPQOS_DEBUG

	pexprChild->AddRef();
	CExpression *pexprSelect = CUtils::PexprLogicalSelect(
		mp, pexprChild,
		CPredicateUtils::PexprConjunction(mp, NULL /*pdrspqexpr*/));

	popTopGbAgg->AddRef();
	pexprTopProjectList->AddRef();
	CExpression *pexprGbAggNew = SPQOS_NEW(mp)
		CExpression(mp, popTopGbAgg, pexprSelect, pexprTopProjectList);

	pxfres->Add(pexprGbAggNew);
}


// EOF
