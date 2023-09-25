//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 EMC Corp.
//
//	@filename:
//		CXformLeftSemiApplyWithExternalCorrs2InnerJoin.cpp
//
//	@doc:
//		Implementation of transform;
//		external correlations are correlations in the inner child of LSA
//		that use columns not defined by the outer child of LSA
//---------------------------------------------------------------------------

#include "spqopt/xforms/CXformLeftSemiApplyWithExternalCorrs2InnerJoin.h"

#include "spqos/base.h"

#include "spqopt/operators/CScalarProjectList.h"

using namespace spqopt;


//
//---------------------------------------------------------------------------
//	@function:
//		CXformLeftSemiApplyWithExternalCorrs2InnerJoin::FSplitCorrelations
//
//	@doc:
//		Helper for splitting correlations into external and residual
//
//---------------------------------------------------------------------------
BOOL
CXformLeftSemiApplyWithExternalCorrs2InnerJoin::FSplitCorrelations(
	CMemoryPool *mp, CExpression *pexprOuter, CExpression *pexprInner,
	CExpressionArray *pdrspqexprAllCorr, CExpressionArray **ppdrspqexprExternal,
	CExpressionArray **ppdrspqexprResidual, CColRefSet **ppcrsInnerUsed)
{
	SPQOS_ASSERT(NULL != pexprOuter);
	SPQOS_ASSERT(NULL != pexprInner);
	SPQOS_ASSERT(NULL != pdrspqexprAllCorr);
	SPQOS_ASSERT(NULL != ppdrspqexprExternal);
	SPQOS_ASSERT(NULL != ppdrspqexprResidual);
	SPQOS_ASSERT(NULL != ppcrsInnerUsed);

	// collect output columns of all children
	CColRefSet *pcrsOuterOuput = pexprOuter->DeriveOutputColumns();
	CColRefSet *pcrsInnerOuput = pexprInner->DeriveOutputColumns();
	CColRefSet *pcrsChildren = SPQOS_NEW(mp) CColRefSet(mp, *pcrsOuterOuput);
	pcrsChildren->Union(pcrsInnerOuput);

	// split correlations into external correlations and residual correlations
	CExpressionArray *pdrspqexprExternal = SPQOS_NEW(mp) CExpressionArray(mp);
	CExpressionArray *pdrspqexprResidual = SPQOS_NEW(mp) CExpressionArray(mp);
	const ULONG ulCorrs = pdrspqexprAllCorr->Size();
	CColRefSet *pcrsUsed = SPQOS_NEW(mp)
		CColRefSet(mp);	 // set of inner columns used in external correlations
	BOOL fSuccess = true;
	for (ULONG ul = 0; fSuccess && ul < ulCorrs; ul++)
	{
		CExpression *pexprCurrent = (*pdrspqexprAllCorr)[ul];
		CColRefSet *pcrsCurrent =
			SPQOS_NEW(mp) CColRefSet(mp, *pexprCurrent->DeriveUsedColumns());
		if (pcrsCurrent->IsDisjoint(pcrsOuterOuput) ||
			pcrsCurrent->IsDisjoint(pcrsInnerOuput))
		{
			// add current correlation to external correlation
			pexprCurrent->AddRef();
			pdrspqexprExternal->Append(pexprCurrent);

			// filter used columns from external columns
			pcrsCurrent->Intersection(pcrsInnerOuput);
			pcrsUsed->Union(pcrsCurrent);
		}
		else if (pcrsChildren->ContainsAll(pcrsCurrent))
		{
			// add current correlation to regular correlations
			pexprCurrent->AddRef();
			pdrspqexprResidual->Append(pexprCurrent);
		}
		else
		{
			// a mixed correlation (containing both outer columns and external columns)
			fSuccess = false;
		}
		pcrsCurrent->Release();
	}
	pcrsChildren->Release();

	if (!fSuccess || 0 == pdrspqexprExternal->Size())
	{
		// failed to find external correlations
		pcrsUsed->Release();
		pdrspqexprExternal->Release();
		pdrspqexprResidual->Release();

		return false;
	}

	*ppcrsInnerUsed = pcrsUsed;
	*ppdrspqexprExternal = pdrspqexprExternal;
	*ppdrspqexprResidual = pdrspqexprResidual;

	return true;
}


//---------------------------------------------------------------------------
//	@function:
//		CXformLeftSemiApplyWithExternalCorrs2InnerJoin::FDecorrelate
//
//	@doc:
//		Helper for collecting correlations from LSA expression
//
//---------------------------------------------------------------------------
BOOL
CXformLeftSemiApplyWithExternalCorrs2InnerJoin::FDecorrelate(
	CMemoryPool *mp, CExpression *pexpr, CExpression **ppexprInnerNew,
	CExpressionArray **ppdrspqexprCorr)
{
	SPQOS_ASSERT(NULL != pexpr);
	SPQOS_ASSERT(COperator::EopLogicalLeftSemiApply == pexpr->Pop()->Eopid() ||
				COperator::EopLogicalLeftSemiApplyIn == pexpr->Pop()->Eopid());

	SPQOS_ASSERT(NULL != ppexprInnerNew);
	SPQOS_ASSERT(NULL != ppdrspqexprCorr);

	// extract children
	CExpression *pexprOuter = (*pexpr)[0];
	CExpression *pexprInner = (*pexpr)[1];
	CExpression *pexprScalar = (*pexpr)[2];

	// collect all correlations from inner child
	pexprInner->ResetDerivedProperties();
	CExpressionArray *pdrspqexpr = SPQOS_NEW(mp) CExpressionArray(mp);
	if (!CDecorrelator::FProcess(mp, pexprInner, true /* fEqualityOnly */,
								 ppexprInnerNew, pdrspqexpr,
								 pexprOuter->DeriveOutputColumns()))
	{
		// decorrelation failed
		pdrspqexpr->Release();
		CRefCount::SafeRelease(*ppexprInnerNew);

		return false;
	}

	// add all original scalar conjuncts to correlations
	CExpressionArray *pdrspqexprOriginal =
		CPredicateUtils::PdrspqexprConjuncts(mp, pexprScalar);
	CUtils::AddRefAppend<CExpression>(pdrspqexpr, pdrspqexprOriginal);
	pdrspqexprOriginal->Release();

	*ppdrspqexprCorr = pdrspqexpr;

	return true;
}


//---------------------------------------------------------------------------
//	@function:
//		CXformLeftSemiApplyWithExternalCorrs2InnerJoin::PexprDecorrelate
//
//	@doc:
//		Transform LSA into a Gb on top of an InnerJoin, where
//		the grouping columns of Gb are the key of LSA's outer child
//		in addition to columns from LSA's inner child that are used
//		in external correlation;
//		this transformation exposes LSA's inner child columns so that
//		correlations can be pulled-up
//
//		Example:
//
//		Input:
//			LS-Apply
//				|----Get(B)
//				+----Select (C.c = A.a)
//						+----Get(C)
//
//		Output:
//			Select (C.c = A.a)
//				+--Gb(B.key, c)
//					+--InnerJoin
//						|--Get(B)
//						+--Get(C)
//
//---------------------------------------------------------------------------
CExpression *
CXformLeftSemiApplyWithExternalCorrs2InnerJoin::PexprDecorrelate(
	CMemoryPool *mp, CExpression *pexpr)
{
	SPQOS_ASSERT(NULL != pexpr);
	SPQOS_ASSERT(COperator::EopLogicalLeftSemiApply == pexpr->Pop()->Eopid() ||
				COperator::EopLogicalLeftSemiApplyIn == pexpr->Pop()->Eopid());

	CExpressionHandle exprhdl(mp);
	exprhdl.Attach(pexpr);

	if (NULL == exprhdl.DeriveKeyCollection(0 /*child_index*/) ||
		!CUtils::FInnerUsesExternalCols(exprhdl))
	{
		// outer child must have a key and inner child must have external correlations
		return NULL;
	}

	CExpression *pexprInnerNew = NULL;
	CExpressionArray *pdrspqexprAllCorr = NULL;
	if (!FDecorrelate(mp, pexpr, &pexprInnerNew, &pdrspqexprAllCorr))
	{
		// decorrelation failed
		return NULL;
	}
	SPQOS_ASSERT(NULL != pdrspqexprAllCorr);

	CExpressionArray *pdrspqexprExternal = NULL;
	CExpressionArray *pdrspqexprResidual = NULL;
	CColRefSet *pcrsInnerUsed = NULL;
	if (!FSplitCorrelations(mp, (*pexpr)[0], pexprInnerNew, pdrspqexprAllCorr,
							&pdrspqexprExternal, &pdrspqexprResidual,
							&pcrsInnerUsed))
	{
		// splitting correlations failed
		pdrspqexprAllCorr->Release();
		CRefCount::SafeRelease(pexprInnerNew);

		return NULL;
	}
	SPQOS_ASSERT(pdrspqexprExternal->Size() + pdrspqexprResidual->Size() ==
				pdrspqexprAllCorr->Size());

	pdrspqexprAllCorr->Release();

	// create an inner join between outer child and decorrelated inner child
	CExpression *pexprOuter = (*pexpr)[0];
	pexprOuter->AddRef();
	CExpression *pexprInnerJoin = CUtils::PexprLogicalJoin<CLogicalInnerJoin>(
		mp, pexprOuter, pexprInnerNew,
		CPredicateUtils::PexprConjunction(mp, pdrspqexprResidual));

	// add a group by on outer columns + inner columns appearing in external correlations
	CColRefArray *pdrspqcrUsed = pcrsInnerUsed->Pdrspqcr(mp);
	pcrsInnerUsed->Release();

	CColRefArray *pdrspqcrKey = NULL;
	CColRefArray *pdrspqcrGrpCols =
		CUtils::PdrspqcrGroupingKey(mp, pexprOuter, &pdrspqcrKey);
	pdrspqcrKey->Release();	// key is not used here

	pdrspqcrGrpCols->AppendArray(pdrspqcrUsed);
	pdrspqcrUsed->Release();
	CExpression *pexprGb = CUtils::PexprLogicalGbAggGlobal(
		mp, pdrspqcrGrpCols, pexprInnerJoin,
		SPQOS_NEW(mp) CExpression(mp, SPQOS_NEW(mp) CScalarProjectList(mp)));

	// add a top filter for external correlations
	return CUtils::PexprLogicalSelect(
		mp, pexprGb, CPredicateUtils::PexprConjunction(mp, pdrspqexprExternal));
}


//---------------------------------------------------------------------------
//	@function:
//		CXformLeftSemiApplyWithExternalCorrs2InnerJoin::Exfp
//
//	@doc:
//		Compute xform promise for a given expression handle;
//
//---------------------------------------------------------------------------
CXform::EXformPromise
CXformLeftSemiApplyWithExternalCorrs2InnerJoin::Exfp(
	CExpressionHandle &exprhdl) const
{
	// expression must have outer references with external correlations
	if (exprhdl.HasOuterRefs(1 /*child_index*/) &&
		CUtils::FInnerUsesExternalCols(exprhdl))
	{
		return CXform::ExfpHigh;
	}

	return CXform::ExfpNone;
}


//---------------------------------------------------------------------------
//	@function:
//		CXformLeftSemiApplyWithExternalCorrs2InnerJoin::Transform
//
//	@doc:
//		Actual transformation
//
//
//---------------------------------------------------------------------------
void
CXformLeftSemiApplyWithExternalCorrs2InnerJoin::Transform(
	CXformContext *pxfctxt, CXformResult *pxfres, CExpression *pexpr) const
{
	SPQOS_ASSERT(NULL != pxfctxt);
	SPQOS_ASSERT(FPromising(pxfctxt->Pmp(), this, pexpr));
	SPQOS_ASSERT(FCheckPattern(pexpr));

	CMemoryPool *mp = pxfctxt->Pmp();
	CExpression *pexprResult = PexprDecorrelate(mp, pexpr);
	if (NULL != pexprResult)
	{
		pxfres->Add(pexprResult);
	}
}

// EOF
