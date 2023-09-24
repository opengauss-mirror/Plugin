//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CXformLeftSemiJoin2InnerJoin.cpp
//
//	@doc:
//		Implementation of transform
//---------------------------------------------------------------------------

#include "spqopt/xforms/CXformLeftSemiJoin2InnerJoin.h"

#include "spqos/base.h"
#include "spqos/memory/CAutoMemoryPool.h"

#include "spqopt/base/CColRefSetIter.h"
#include "spqopt/base/CColRefTable.h"
#include "spqopt/base/CKeyCollection.h"
#include "spqopt/operators/CLogicalGbAgg.h"
#include "spqopt/operators/CLogicalInnerJoin.h"
#include "spqopt/operators/CLogicalLeftSemiJoin.h"
#include "spqopt/operators/CPatternLeaf.h"
#include "spqopt/operators/CPredicateUtils.h"
#include "spqopt/operators/CScalarProjectList.h"



using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CXformLeftSemiJoin2InnerJoin::CXformLeftSemiJoin2InnerJoin
//
//	@doc:
//		ctor
//
//---------------------------------------------------------------------------
CXformLeftSemiJoin2InnerJoin::CXformLeftSemiJoin2InnerJoin(CMemoryPool *mp)
	:  // pattern
	  CXformExploration(SPQOS_NEW(mp) CExpression(
		  mp, SPQOS_NEW(mp) CLogicalLeftSemiJoin(mp),
		  SPQOS_NEW(mp)
			  CExpression(mp, SPQOS_NEW(mp) CPatternLeaf(mp)),  // left child
		  SPQOS_NEW(mp)
			  CExpression(mp, SPQOS_NEW(mp) CPatternLeaf(mp)),  // right child
		  SPQOS_NEW(mp)
			  CExpression(mp, SPQOS_NEW(mp) CPatternLeaf(mp))  // predicate
		  ))
{
}


//---------------------------------------------------------------------------
//	@function:
//		CXformLeftSemiJoin2InnerJoin::Exfp
//
//	@doc:
//		Compute xform promise for a given expression handle;
//
//---------------------------------------------------------------------------
CXform::EXformPromise
CXformLeftSemiJoin2InnerJoin::Exfp(CExpressionHandle &exprhdl) const
{
	if (exprhdl.HasOuterRefs() || exprhdl.DeriveHasSubquery(2))
	{
		return ExfpNone;
	}

	CColRefSet *pcrsInnerOutput =
		exprhdl.DeriveOutputColumns(1 /*child_index*/);
	CExpression *pexprScalar = exprhdl.PexprScalarExactChild(2 /*child_index*/);
	CAutoMemoryPool amp;

	// examine join predicate to determine xform applicability
	if (NULL == pexprScalar || !CPredicateUtils::FSimpleEqualityUsingCols(
								   amp.Pmp(), pexprScalar, pcrsInnerOutput))
	{
		return ExfpNone;
	}

	return ExfpHigh;
}


//---------------------------------------------------------------------------
//	@function:
//		CXformLeftSemiJoin2InnerJoin::Transform
//
//	@doc:
//		actual transformation
//
//---------------------------------------------------------------------------
void
CXformLeftSemiJoin2InnerJoin::Transform(CXformContext *pxfctxt,
										CXformResult *pxfres,
										CExpression *pexpr) const
{
	SPQOS_ASSERT(NULL != pxfctxt);
	SPQOS_ASSERT(FPromising(pxfctxt->Pmp(), this, pexpr));
	SPQOS_ASSERT(FCheckPattern(pexpr));

	CMemoryPool *mp = pxfctxt->Pmp();

	// extract components
	CExpression *pexprOuter = (*pexpr)[0];
	CExpression *pexprInner = (*pexpr)[1];
	CExpression *pexprScalar = (*pexpr)[2];

	pexprOuter->AddRef();
	pexprInner->AddRef();
	pexprScalar->AddRef();

	// construct grouping columns by collecting used columns in the join predicate
	// that come from join's inner child
	CColRefSet *pcrsOuterOutput = pexprOuter->DeriveOutputColumns();
	CColRefSet *pcrsUsed = pexprScalar->DeriveUsedColumns();
	CColRefSet *pcrsGb = SPQOS_NEW(mp) CColRefSet(mp);
	pcrsGb->Include(pcrsUsed);
	pcrsGb->Difference(pcrsOuterOutput);
	SPQOS_ASSERT(0 < pcrsGb->Size());

	CKeyCollection *pkc = pexprInner->DeriveKeyCollection();
	if (NULL == pkc ||
		(NULL != pkc && !pkc->FKey(pcrsGb, false /*fExactMatch*/)))
	{
		// grouping columns do not cover a key on the inner side,
		// we need to create a group by on inner side
		CColRefArray *colref_array = pcrsGb->Pdrspqcr(mp);
		CExpression *pexprGb = SPQOS_NEW(mp) CExpression(
			mp,
			SPQOS_NEW(mp) CLogicalGbAgg(
				mp, colref_array, COperator::EgbaggtypeGlobal /*egbaggtype*/),
			pexprInner,
			SPQOS_NEW(mp) CExpression(mp, SPQOS_NEW(mp) CScalarProjectList(mp)));
		pexprInner = pexprGb;
	}

	CExpression *pexprInnerJoin = CUtils::PexprLogicalJoin<CLogicalInnerJoin>(
		mp, pexprOuter, pexprInner, pexprScalar);

	pcrsGb->Release();
	pxfres->Add(pexprInnerJoin);
}


// EOF
