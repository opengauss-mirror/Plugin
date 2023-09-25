//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 Pivotal Inc.
//
//	@filename:
//		CXformSplitDQA.cpp
//
//	@doc:
//		Implementation of the splitting of an aggregate into a three levels -- namely,
//		local, intermediate and global, aggregate
//---------------------------------------------------------------------------

#include "spqopt/xforms/CXformSplitDQA.h"

#include "spqos/base.h"

#include "spqopt/base/CColumnFactory.h"
#include "spqopt/base/CUtils.h"
#include "spqopt/operators/CLogicalGbAgg.h"
#include "spqopt/operators/COperator.h"
#include "spqopt/translate/CTranslatorDXLToExpr.h"
#include "spqopt/xforms/CXformUtils.h"
#include "naucrates/md/IMDAggregate.h"

using namespace spqmd;
using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CXformSplitDQA::CXformSplitDQA
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CXformSplitDQA::CXformSplitDQA(CMemoryPool *mp)
	: CXformExploration(
		  // pattern
		  SPQOS_NEW(mp) CExpression(
			  mp, SPQOS_NEW(mp) CLogicalGbAgg(mp),
			  SPQOS_NEW(mp) CExpression(
				  mp, SPQOS_NEW(mp) CPatternLeaf(mp)),  // relational child
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
CXformSplitDQA::Exfp(CExpressionHandle &exprhdl) const
{
	// do not split aggregate if it is not a global aggregate,  has no distinct aggs, has MDQAs, has outer references,
	// or return types of Agg functions are ambiguous
	if (!CLogicalGbAgg::PopConvert(exprhdl.Pop())->FGlobal() ||
		0 == exprhdl.DeriveTotalDistinctAggs(1) ||
		exprhdl.DeriveHasMultipleDistinctAggs(1) ||
		0 < exprhdl.DeriveOuterReferences()->Size() ||
		NULL == exprhdl.PexprScalarExactChild(1) ||
		CXformUtils::FHasAmbiguousType(
			exprhdl.PexprScalarExactChild(1 /*child_index*/),
			COptCtxt::PoctxtFromTLS()->Pmda()))
	{
		return CXform::ExfpNone;
	}

	return CXform::ExfpHigh;
}

//---------------------------------------------------------------------------
//	@function:
//		CXformSplitDQA::Transform
//
//	@doc:
//		Actual transformation to expand a global aggregate into local,
//		intermediate and global aggregates
//
//---------------------------------------------------------------------------
void
CXformSplitDQA::Transform(CXformContext *pxfctxt, CXformResult *pxfres,
						  CExpression *pexpr) const
{
	SPQOS_ASSERT(NULL != pxfctxt);
	SPQOS_ASSERT(NULL != pxfres);
	SPQOS_ASSERT(FPromising(pxfctxt->Pmp(), this, pexpr));
	SPQOS_ASSERT(FCheckPattern(pexpr));

	CColumnFactory *col_factory = COptCtxt::PoctxtFromTLS()->Pcf();
	CMDAccessor *md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();
	CMemoryPool *mp = pxfctxt->Pmp();

	// extract components
	CExpression *pexprRelational = (*pexpr)[0];
	CExpression *pexprProjectList = (*pexpr)[1];

	ExprToColRefMap *phmexprcr = SPQOS_NEW(mp) ExprToColRefMap(mp);
	CExpressionArray *pdrspqexprChildPrEl = SPQOS_NEW(mp) CExpressionArray(mp);
	CColRefArray *pdrspqcrArgDQA = NULL;

	ExtractDistinctCols(mp, col_factory, md_accessor, pexprProjectList,
						pdrspqexprChildPrEl, phmexprcr, &pdrspqcrArgDQA);

	if (NULL == pdrspqcrArgDQA)
	{
		// failed to extract distinct columns
		phmexprcr->Release();
		pdrspqexprChildPrEl->Release();

		return;
	}


	if (0 < pdrspqexprChildPrEl->Size())
	{
		pexprRelational->AddRef();

		// computed columns referred to in the DQA
		CExpression *pexprChildProject = CUtils::PexprLogicalProject(
			mp, pexprRelational,
			SPQOS_NEW(mp) CExpression(mp, SPQOS_NEW(mp) CScalarProjectList(mp),
									 pdrspqexprChildPrEl),
			true /*fNewComputedCol*/
		);
		pexprRelational = pexprChildProject;
	}

	CColRefArray *grouping_colref_array =
		CLogicalGbAgg::PopConvert(pexpr->Pop())->Pdrspqcr();
	BOOL fScalarDQA =
		(grouping_colref_array == NULL || grouping_colref_array->Size() == 0);

	// multi-stage for both scalar and non-scalar aggregates.
	CExpression *pexprThreeStageDQA =
		PexprSplitHelper(mp, col_factory, md_accessor, pexpr, pexprRelational,
						 phmexprcr, pdrspqcrArgDQA,
						 fScalarDQA ? CLogicalGbAgg::EasThreeStageScalarDQA
									: CLogicalGbAgg::EasOthers);

	pxfres->Add(pexprThreeStageDQA);

	// generate two-stage agg
	// this transform is useful for cases where distinct column is same as distributed column.
	// for a query like "select count(distinct a) from bar;"
	// we generate a two stage agg where the aggregate operator gives us the distinct values.
	// CScalarProjectList for the Local agg below is empty on purpose.

	//		+--CLogicalGbAgg( Global ) Grp Cols: [][Global]
	//		|--CLogicalGbAgg( Local ) Grp Cols: ["a" (0)][Local],
	//		|  |--CLogicalGet "bar" ("bar"),
	//		|  +--CScalarProjectList
	//		+--CScalarProjectList
	//			+--CScalarProjectElement "count" (9)
	//				+--CScalarAggFunc (count , Distinct: false , Aggregate Stage: Global)
	//					+--CScalarIdent "a" (0)

	CExpression *pexprTwoStageScalarDQA = PexprSplitHelper(
		mp, col_factory, md_accessor, pexpr, pexprRelational, phmexprcr,
		pdrspqcrArgDQA, CLogicalGbAgg::EasTwoStageScalarDQA);
	pxfres->Add(pexprTwoStageScalarDQA);

	// generate local DQA, global agg for both scalar and non-scalar agg cases.
	// for a query like "select count(distinct a) from bar;"
	// we generate a two-stage agg where the projectlist with AggFunc is pushed to
	// both local and global agg. We use the AggFunc to get our distinct values.

	//	+--CLogicalGbAgg( Global ) Grp Cols: []
	//	|--CLogicalGbAgg( Local ) Grp Cols: []
	//	|  |--CLogicalGet "bar" ("bar"),
	//	|  +--CScalarProjectList
	//	|     +--CScalarProjectElement "ColRef_0010" (10)
	//	|        +--CScalarAggFunc (count , Distinct: true , Aggregate Stage: Local)
	//	|           +--CScalarIdent "a" (0)
	//	+--CScalarProjectList
	//		+--CScalarProjectElement "count" (9)
	//			+--CScalarAggFunc (count , Distinct: false , Aggregate Stage: Global)
	//				+--CScalarIdent "ColRef_0010" (10)

	CExpression *pexprTwoStageDQA = PexprSplitIntoLocalDQAGlobalAgg(
		mp, col_factory, md_accessor, pexpr, pexprRelational, phmexprcr,
		pdrspqcrArgDQA,
		fScalarDQA ? CLogicalGbAgg::EasTwoStageScalarDQA
				   : CLogicalGbAgg::EasOthers);

	pxfres->Add(pexprTwoStageDQA);


	pdrspqcrArgDQA->Release();

	// clean up
	if (0 < pdrspqexprChildPrEl->Size())
	{
		pexprRelational->Release();
	}
	else
	{
		pdrspqexprChildPrEl->Release();
	}

	phmexprcr->Release();
}


//---------------------------------------------------------------------------
//	@function:
//		CXformSplitDQA::PexprSplitIntoLocalDQAGlobalAgg
//
//	@doc:
// 		Split the DQA into a local DQA and global agg function
//
//---------------------------------------------------------------------------
CExpression *
CXformSplitDQA::PexprSplitIntoLocalDQAGlobalAgg(
	CMemoryPool *mp, CColumnFactory *col_factory, CMDAccessor *md_accessor,
	CExpression *pexpr, CExpression *pexprRelational,
	ExprToColRefMap *phmexprcr, CColRefArray *pdrspqcrArgDQA,
	CLogicalGbAgg::EAggStage aggStage)
{
	CExpression *pexprPrL = (*pexpr)[1];

	CLogicalGbAgg *popAgg = CLogicalGbAgg::PopConvert(pexpr->Pop());
	CColRefArray *pdrspqcrGlobal = popAgg->Pdrspqcr();

	// array of project elements for the local, intermediate and global aggregate operator
	CExpressionArray *pdrspqexprPrElFirstStage =
		SPQOS_NEW(mp) CExpressionArray(mp);
	CExpressionArray *pdrspqexprPrElLastStage =
		SPQOS_NEW(mp) CExpressionArray(mp);

	const ULONG arity = pexprPrL->Arity();
	for (ULONG ul = 0; ul < arity; ul++)
	{
		CExpression *pexprPrEl = (*pexprPrL)[ul];
		CScalarProjectElement *popScPrEl =
			CScalarProjectElement::PopConvert(pexprPrEl->Pop());

		// get the scalar aggregate function
		CExpression *pexprAggFunc = (*pexprPrEl)[0];
		CScalarAggFunc *popScAggFunc =
			CScalarAggFunc::PopConvert(pexprAggFunc->Pop());

		if (popScAggFunc->IsDistinct())
		{
			// create a new local DQA version of the original global DQA
			popScAggFunc->MDId()->AddRef();
			CScalarAggFunc *popScAggFuncLocal = CUtils::PopAggFunc(
				mp, popScAggFunc->MDId(),
				SPQOS_NEW(mp)
					CWStringConst(mp, popScAggFunc->PstrAggFunc()->GetBuffer()),
				true /* is_distinct */, EaggfuncstageLocal /*eaggfuncstage*/,
				true /* fSplit */, NULL /* pmdidResolvedReturnType */,
				EaggfunckindNormal);

			// CScalarValuesList
			CExpression *pexprArg = (*(*pexprAggFunc)[0])[0];
			CColRef *pcrDistinctCol = phmexprcr->Find(pexprArg);
			SPQOS_ASSERT(NULL != pcrDistinctCol);
			CExpressionArray *pdrspqexprArgsLocal =
				SPQOS_NEW(mp) CExpressionArray(mp);
			CExpressionArray *pdrspqexprArgs = SPQOS_NEW(mp) CExpressionArray(mp);
			pdrspqexprArgs->Append(CUtils::PexprScalarIdent(mp, pcrDistinctCol));

			// agg args
			pdrspqexprArgsLocal->Append(SPQOS_NEW(mp) CExpression(
				mp, SPQOS_NEW(mp) CScalarValuesList(mp), pdrspqexprArgs));

			for (ULONG i = 1; i < pexprAggFunc->Arity(); i++)
			{
				CExpression *expr = (*pexprAggFunc)[i];
				expr->AddRef();
				pdrspqexprArgsLocal->Append(expr);
			}

			const IMDAggregate *pmdagg =
				md_accessor->RetrieveAgg(popScAggFunc->MDId());
			const IMDType *pmdtype = md_accessor->RetrieveType(
				pmdagg->GetIntermediateResultTypeMdid());
			CColRef *pcrLocal =
				col_factory->PcrCreate(pmdtype, default_type_modifier);

			CExpression *pexprPrElLocal = CUtils::PexprScalarProjectElement(
				mp, pcrLocal,
				SPQOS_NEW(mp)
					CExpression(mp, popScAggFuncLocal, pdrspqexprArgsLocal));

			pdrspqexprPrElFirstStage->Append(pexprPrElLocal);

			// create a new "non-distinct" global aggregate version of the original DQA
			popScAggFunc->MDId()->AddRef();
			CScalarAggFunc *popScAggFuncGlobal = CUtils::PopAggFunc(
				mp, popScAggFunc->MDId(),
				SPQOS_NEW(mp)
					CWStringConst(mp, popScAggFunc->PstrAggFunc()->GetBuffer()),
				false /* is_distinct */, EaggfuncstageGlobal /*eaggfuncstage*/,
				true /* fSplit */, NULL /* pmdidResolvedReturnType */,
				EaggfunckindNormal);

			CExpressionArray *pdrspqexprArgsGlobal =
				SPQOS_NEW(mp) CExpressionArray(mp);
			CExpressionArray *pdrspqexprArgsGlobalArgs =
				SPQOS_NEW(mp) CExpressionArray(mp);
			pdrspqexprArgsGlobalArgs->Append(
				CUtils::PexprScalarIdent(mp, pcrLocal));

			// agg args
			pdrspqexprArgsGlobal->Append(
				SPQOS_NEW(mp) CExpression(mp, SPQOS_NEW(mp) CScalarValuesList(mp),
										 pdrspqexprArgsGlobalArgs));

			for (ULONG i = 1; i < pexprAggFunc->Arity(); i++)
			{
				CExpression *expr = (*pexprAggFunc)[i];
				expr->AddRef();
				pdrspqexprArgsGlobal->Append(expr);
			}

			CExpression *pexprPrElGlobal = CUtils::PexprScalarProjectElement(
				mp, popScPrEl->Pcr(),
				SPQOS_NEW(mp)
					CExpression(mp, popScAggFuncGlobal, pdrspqexprArgsGlobal));

			pdrspqexprPrElLastStage->Append(pexprPrElGlobal);
		}
		else
		{
			// split regular aggregate function into multi-level aggregate functions
			PopulatePrLMultiPhaseAgg(
				mp, col_factory, md_accessor, pexprPrEl,
				pdrspqexprPrElFirstStage, NULL, /* pdrspqexprPrElSecondStage*/
				pdrspqexprPrElLastStage, true   /* fSplit2LevelsOnly */
			);
		}
	}

	CExpression *pexprGlobal = PexprMultiLevelAggregation(
		mp, pexprRelational, pdrspqexprPrElFirstStage,
		NULL, /* pdrspqexprPrElSecondStage */
		pdrspqexprPrElLastStage, pdrspqcrArgDQA, pdrspqcrGlobal,
		true /* fSplit2LevelsOnly */, false /* fAddDistinctColToLocalGb */,
		aggStage);

	return pexprGlobal;
}

//---------------------------------------------------------------------------
//	@function:
//		CXformSplitDQA::PexprSplitHelper
//
//	@doc:
//		Helper function to Split DQA into 2-stage or 3-stage aggregation.
//
//		If fSpillTo2Level is FALSE, split the 'group by' operator into 3-stage aggregation,
//		e.g., first, second and last aggregates.
//
//		If fSpillTo2Level is TRUE, split the 'group by' operator into 2-stage aggregation,
//		e.g., first, last aggregates. (second aggregate function becomes empty.)
//
//		In both scenarios, add the new aggregate functions to the project list of the
//		corresponding group by operator at each stage of the multi-stage aggregation.
//
//---------------------------------------------------------------------------
CExpression *
CXformSplitDQA::PexprSplitHelper(CMemoryPool *mp, CColumnFactory *col_factory,
								 CMDAccessor *md_accessor, CExpression *pexpr,
								 CExpression *pexprRelational,
								 ExprToColRefMap *phmexprcr,
								 CColRefArray *pdrspqcrArgDQA,
								 CLogicalGbAgg::EAggStage aggStage)
{
	CExpression *pexprPrL = (*pexpr)[1];

	CLogicalGbAgg *popAgg = CLogicalGbAgg::PopConvert(pexpr->Pop());
	CColRefArray *pdrspqcrGlobal = popAgg->Pdrspqcr();

	// array of project elements for the local (first), intermediate
	// (second) and global (third) aggregate operator
	CExpressionArray *pdrspqexprPrElFirstStage =
		SPQOS_NEW(mp) CExpressionArray(mp);
	CExpressionArray *pdrspqexprPrElSecondStage =
		SPQOS_NEW(mp) CExpressionArray(mp);
	CExpressionArray *pdrspqexprPrElLastStage =
		SPQOS_NEW(mp) CExpressionArray(mp);
	BOOL fSpillTo2Level = (aggStage == CLogicalGbAgg::EasTwoStageScalarDQA);



	const ULONG arity = pexprPrL->Arity();
	for (ULONG ul = 0; ul < arity; ul++)
	{
		CExpression *pexprPrEl = (*pexprPrL)[ul];
		CScalarProjectElement *popScPrEl =
			CScalarProjectElement::PopConvert(pexprPrEl->Pop());

		// get the scalar aggregate function
		CExpression *pexprAggFunc = (*pexprPrEl)[0];
		CScalarAggFunc *popScAggFunc =
			CScalarAggFunc::PopConvert(pexprAggFunc->Pop());

		if (popScAggFunc->IsDistinct())
		{
			// create a new "non-distinct" version of the original aggregate function
			popScAggFunc->MDId()->AddRef();
			CScalarAggFunc *popScAggFuncNew = CUtils::PopAggFunc(
				mp, popScAggFunc->MDId(),
				SPQOS_NEW(mp)
					CWStringConst(mp, popScAggFunc->PstrAggFunc()->GetBuffer()),
				false /* is_distinct */, EaggfuncstageGlobal /*eaggfuncstage*/,
				false /* fSplit */, NULL /* pmdidResolvedReturnType */,
				EaggfunckindNormal);

			CExpressionArray *pdrspqexprChildren =
				SPQOS_NEW(mp) CExpressionArray(mp);

			CExpressionArray *pdrspqexprArgs = SPQOS_NEW(mp) CExpressionArray(mp);
			for (ULONG ul = 0; ul < (*pexprAggFunc)[0]->Arity(); ul++)
			{
				CExpression *pexprArg = (*(*pexprAggFunc)[0])[ul];
				CColRef *pcrDistinctCol = phmexprcr->Find(pexprArg);
				SPQOS_ASSERT(NULL != pcrDistinctCol);

				pdrspqexprArgs->Append(
					CUtils::PexprScalarIdent(mp, pcrDistinctCol));
			}

			// agg args
			pdrspqexprChildren->Append(SPQOS_NEW(mp) CExpression(
				mp, SPQOS_NEW(mp) CScalarValuesList(mp), pdrspqexprArgs));

			// agg direct args
			pdrspqexprChildren->Append(
				SPQOS_NEW(mp) CExpression(mp, SPQOS_NEW(mp) CScalarValuesList(mp),
										 SPQOS_NEW(mp) CExpressionArray(mp)));

			// agg order
			pdrspqexprChildren->Append(
				SPQOS_NEW(mp) CExpression(mp, SPQOS_NEW(mp) CScalarValuesList(mp),
										 SPQOS_NEW(mp) CExpressionArray(mp)));

			// agg distinct
			CExpressionArray *pdrspqexprDirectArgs =
				SPQOS_NEW(mp) CExpressionArray(mp);
			for (ULONG ul = 0;
				 ul < (*pexprAggFunc)[EaggfuncIndexDistinct]->Arity(); ul++)
			{
				CExpression *pexprDirectArg =
					(*(*pexprAggFunc)[EaggfuncIndexDistinct])[ul];
				pexprDirectArg->AddRef();
				pdrspqexprDirectArgs->Append(pexprDirectArg);
			}
			pdrspqexprChildren->Append(SPQOS_NEW(mp) CExpression(
				mp, SPQOS_NEW(mp) CScalarValuesList(mp), pdrspqexprDirectArgs));

			CExpression *pexprPrElGlobal = CUtils::PexprScalarProjectElement(
				mp, popScPrEl->Pcr(),
				SPQOS_NEW(mp)
					CExpression(mp, popScAggFuncNew, pdrspqexprChildren));

			pdrspqexprPrElLastStage->Append(pexprPrElGlobal);
		}
		else
		{
			// split the regular aggregate function into multi-level aggregate functions
			PopulatePrLMultiPhaseAgg(mp, col_factory, md_accessor, pexprPrEl,
									 pdrspqexprPrElFirstStage,
									 pdrspqexprPrElSecondStage,
									 pdrspqexprPrElLastStage, fSpillTo2Level);
		}
	}

	CExpression *pexprGlobal = PexprMultiLevelAggregation(
		mp, pexprRelational, pdrspqexprPrElFirstStage, pdrspqexprPrElSecondStage,
		pdrspqexprPrElLastStage, pdrspqcrArgDQA, pdrspqcrGlobal, fSpillTo2Level,
		true /* fAddDistinctColToLocalGb */, aggStage /* fTwoStageScalarDQA */
	);

	// clean-up the secondStage if spill to 2 level
	if (fSpillTo2Level)
	{
		pdrspqexprPrElSecondStage->Release();
	}

	return pexprGlobal;
}

//---------------------------------------------------------------------------
//	@function:
//		CXformSplitDQA::PexprPrElAgg
//
//	@doc:
//		Create an aggregate function  of a particular level and a project
//		project element to hold it
//
//---------------------------------------------------------------------------
CExpression *
CXformSplitDQA::PexprPrElAgg(CMemoryPool *mp, CExpression *pexprAggFunc,
							 EAggfuncStage eaggfuncstage,
							 CColRef *pcrPreviousStage, CColRef *pcrCurrStage)
{
	SPQOS_ASSERT(NULL != pexprAggFunc);
	SPQOS_ASSERT(NULL != pcrCurrStage);
	SPQOS_ASSERT(EaggfuncstageSentinel != eaggfuncstage);

	CScalarAggFunc *popScAggFunc =
		CScalarAggFunc::PopConvert(pexprAggFunc->Pop());
	SPQOS_ASSERT(!popScAggFunc->IsDistinct());

	// project element of global aggregation
	CExpressionArray *pdrspqexprArg = NULL;
	if (EaggfuncstageLocal == eaggfuncstage)
	{
		CExpressionArray *pdrspqexprAggOrig = pexprAggFunc->PdrgPexpr();
		pdrspqexprAggOrig->AddRef();
		pdrspqexprArg = pdrspqexprAggOrig;
	}
	else
	{
		CExpressionArray *pdrspqargs = SPQOS_NEW(mp) CExpressionArray(mp);
		pdrspqargs->Append(CUtils::PexprScalarIdent(mp, pcrPreviousStage));

		pdrspqexprArg = SPQOS_NEW(mp) CExpressionArray(mp);

		// agg args
		pdrspqexprArg->Append(SPQOS_NEW(mp) CExpression(
			mp, SPQOS_NEW(mp) CScalarValuesList(mp), pdrspqargs));

		// agg direct args
		pdrspqexprArg->Append(
			SPQOS_NEW(mp) CExpression(mp, SPQOS_NEW(mp) CScalarValuesList(mp),
									 SPQOS_NEW(mp) CExpressionArray(mp)));
		// agg order
		pdrspqexprArg->Append(
			SPQOS_NEW(mp) CExpression(mp, SPQOS_NEW(mp) CScalarValuesList(mp),
									 SPQOS_NEW(mp) CExpressionArray(mp)));
		// agg distinct
		pdrspqexprArg->Append(
			SPQOS_NEW(mp) CExpression(mp, SPQOS_NEW(mp) CScalarValuesList(mp),
									 SPQOS_NEW(mp) CExpressionArray(mp)));
	}

	popScAggFunc->MDId()->AddRef();
	CScalarAggFunc *popScAggFuncNew = CUtils::PopAggFunc(
		mp, popScAggFunc->MDId(),
		SPQOS_NEW(mp)
			CWStringConst(mp, popScAggFunc->PstrAggFunc()->GetBuffer()),
		false, /*fdistinct */
		eaggfuncstage, true /* fSplit */, NULL /* pmdidResolvedReturnType */,
		EaggfunckindNormal);

	return CUtils::PexprScalarProjectElement(
		mp, pcrCurrStage,
		SPQOS_NEW(mp) CExpression(mp, popScAggFuncNew, pdrspqexprArg));
}


//---------------------------------------------------------------------------
//	@function:
//		CXformSplitDQA::PopulatePrLMultiPhaseAgg
//
//	@doc:
// 		Given a scalar aggregate generate the local, intermediate and global
// 		aggregate function. Then add it to the project list of the corresponding
// 		aggregate operator at each stage of the multi-stage aggregation
//
//---------------------------------------------------------------------------
void
CXformSplitDQA::PopulatePrLMultiPhaseAgg(
	CMemoryPool *mp, CColumnFactory *col_factory, CMDAccessor *md_accessor,
	CExpression *pexprPrEl, CExpressionArray *pdrspqexprPrElFirstStage,
	CExpressionArray *pdrspqexprPrElSecondStage,
	CExpressionArray *pdrspqexprPrElLastStage, BOOL fSplit2LevelsOnly)
{
	SPQOS_ASSERT(NULL != pexprPrEl);
	SPQOS_ASSERT(NULL != pdrspqexprPrElFirstStage);
	SPQOS_ASSERT_IMP(NULL == pdrspqexprPrElSecondStage, fSplit2LevelsOnly);
	SPQOS_ASSERT(NULL != pdrspqexprPrElLastStage);

	// get the components of the project element (agg func)
	CScalarProjectElement *popScPrEl =
		CScalarProjectElement::PopConvert(pexprPrEl->Pop());
	CExpression *pexprAggFunc = (*pexprPrEl)[0];
	CScalarAggFunc *popScAggFunc =
		CScalarAggFunc::PopConvert(pexprAggFunc->Pop());

	const IMDAggregate *pmdagg = md_accessor->RetrieveAgg(popScAggFunc->MDId());
	const IMDType *pmdtype =
		md_accessor->RetrieveType(pmdagg->GetIntermediateResultTypeMdid());

	// create new column reference for the first stage (local) project element
	CColRef *pcrLocal = col_factory->PcrCreate(pmdtype, default_type_modifier);

	CExpression *pexprPrElFirstStage =
		PexprPrElAgg(mp, pexprAggFunc, EaggfuncstageLocal,
					 NULL /*pcrPreviousStage*/, pcrLocal);
	pdrspqexprPrElFirstStage->Append(pexprPrElFirstStage);

	// column reference for the second stage project elements
	CColRef *pcrSecondStage = NULL;
	EAggfuncStage eaggfuncstage = EaggfuncstageIntermediate;
	if (fSplit2LevelsOnly)
	{
		eaggfuncstage = EaggfuncstageGlobal;
		pcrSecondStage = popScPrEl->Pcr();
	}
	else
	{
		// create a new column reference for the second stage (intermediate) project element
		pcrSecondStage = col_factory->PcrCreate(pmdtype, default_type_modifier);
	}

	CExpression *pexprPrElSecondStage =
		PexprPrElAgg(mp, pexprAggFunc, eaggfuncstage, pcrLocal, pcrSecondStage);
	if (fSplit2LevelsOnly)
	{
		pdrspqexprPrElLastStage->Append(pexprPrElSecondStage);
		return;
	}

	pdrspqexprPrElSecondStage->Append(pexprPrElSecondStage);

	// column reference for the third stage project elements
	CColRef *pcrGlobal = popScPrEl->Pcr();
	CExpression *pexprPrElGlobal = PexprPrElAgg(
		mp, pexprAggFunc, EaggfuncstageGlobal, pcrSecondStage, pcrGlobal);

	pdrspqexprPrElLastStage->Append(pexprPrElGlobal);
}


//---------------------------------------------------------------------------
//	@function:
//		CXformSplitDQA::PcrAggFuncArgument
//
//	@doc:
//		Return the column reference of the argument to the aggregate function.
//		If the argument is a computed column then create a new project element
//		in the child's project list, else just return its column reference
//
//---------------------------------------------------------------------------
CColRef *
CXformSplitDQA::PcrAggFuncArgument(CMemoryPool *mp, CMDAccessor *md_accessor,
								   CColumnFactory *col_factory,
								   CExpression *pexprArg,
								   CExpressionArray *pdrspqexprChildPrEl)
{
	SPQOS_ASSERT(NULL != pexprArg);
	SPQOS_ASSERT(NULL != pdrspqexprChildPrEl);

	if (COperator::EopScalarIdent == pexprArg->Pop()->Eopid())
	{
		return (const_cast<CColRef *>(
			CScalarIdent::PopConvert(pexprArg->Pop())->Pcr()));
	}

	CScalar *popScalar = CScalar::PopConvert(pexprArg->Pop());
	// computed argument to the input
	const IMDType *pmdtype = md_accessor->RetrieveType(popScalar->MdidType());
	CColRef *pcrAdditionalGrpCol =
		col_factory->PcrCreate(pmdtype, popScalar->TypeModifier());

	pexprArg->AddRef();
	CExpression *pexprPrElNew =
		CUtils::PexprScalarProjectElement(mp, pcrAdditionalGrpCol, pexprArg);

	pdrspqexprChildPrEl->Append(pexprPrElNew);

	return pcrAdditionalGrpCol;
}


//---------------------------------------------------------------------------
//	@function:
//		CXformSplitDQA::PexprMultiLevelAggregation
//
//	@doc:
//		Generate an expression with multi-level aggregation
//
//---------------------------------------------------------------------------
CExpression *
CXformSplitDQA::PexprMultiLevelAggregation(
	CMemoryPool *mp, CExpression *pexprRelational,
	CExpressionArray *pdrspqexprPrElFirstStage,
	CExpressionArray *pdrspqexprPrElSecondStage,
	CExpressionArray *pdrspqexprPrElThirdStage, CColRefArray *pdrspqcrArgDQA,
	CColRefArray *pdrspqcrLastStage, BOOL fSplit2LevelsOnly,
	BOOL fAddDistinctColToLocalGb, CLogicalGbAgg::EAggStage aggStage)
{
	SPQOS_ASSERT(NULL != pexprRelational);
	SPQOS_ASSERT(NULL != pdrspqexprPrElFirstStage);
	SPQOS_ASSERT(NULL != pdrspqexprPrElThirdStage);
	SPQOS_ASSERT(NULL != pdrspqcrArgDQA);

	SPQOS_ASSERT_IMP(!fAddDistinctColToLocalGb, fSplit2LevelsOnly);

	CColRefArray *pdrspqcrLocal = CUtils::PdrspqcrExactCopy(mp, pdrspqcrLastStage);
	const ULONG length = pdrspqcrArgDQA->Size();
	SPQOS_ASSERT(0 < length);

	if (fAddDistinctColToLocalGb)
	{
		// add the distinct column to the group by at the first stage of
		// the multi-level aggregation
		CColRefSet *pcrs = SPQOS_NEW(mp) CColRefSet(mp, pdrspqcrLocal);
		for (ULONG ul = 0; ul < length; ul++)
		{
			CColRef *colref = (*pdrspqcrArgDQA)[ul];
			if (!pcrs->FMember(colref))
			{
				pdrspqcrLocal->Append(colref);
				pcrs->Include(colref);
			}
		}
		pcrs->Release();
	}

	CLogicalGbAgg *popFirstStage = NULL;
	CLogicalGbAgg *popSecondStage = NULL;
	CExpressionArray *pdrspqexprLastStage = pdrspqexprPrElSecondStage;
	if (fSplit2LevelsOnly)
	{
		// the local aggregate is responsible for removing duplicates
		pdrspqcrArgDQA->AddRef();
		popFirstStage = SPQOS_NEW(mp) CLogicalGbAgg(
			mp, pdrspqcrLocal, COperator::EgbaggtypeLocal,
			false /* fGeneratesDuplicates */, pdrspqcrArgDQA, aggStage);
		pdrspqcrLastStage->AddRef();
		popSecondStage = SPQOS_NEW(mp) CLogicalGbAgg(
			mp, pdrspqcrLastStage, COperator::EgbaggtypeGlobal, /* egbaggtype */
			aggStage);
		pdrspqexprLastStage = pdrspqexprPrElThirdStage;
	}
	else
	{
		popFirstStage = SPQOS_NEW(mp) CLogicalGbAgg(
			mp, pdrspqcrLocal, COperator::EgbaggtypeLocal, /* egbaggtype */
			aggStage);
		pdrspqcrLocal->AddRef();
		pdrspqcrArgDQA->AddRef();
		popSecondStage = SPQOS_NEW(mp)
			CLogicalGbAgg(mp, pdrspqcrLocal, COperator::EgbaggtypeIntermediate,
						  false, /* fGeneratesDuplicates */
						  pdrspqcrArgDQA, aggStage);
	}

	pexprRelational->AddRef();
	CExpression *pexprFirstStage = SPQOS_NEW(mp) CExpression(
		mp, popFirstStage, pexprRelational,
		SPQOS_NEW(mp) CExpression(mp, SPQOS_NEW(mp) CScalarProjectList(mp),
								 pdrspqexprPrElFirstStage));

	CExpression *pexprSecondStage = SPQOS_NEW(mp) CExpression(
		mp, popSecondStage, pexprFirstStage,
		SPQOS_NEW(mp) CExpression(mp, SPQOS_NEW(mp) CScalarProjectList(mp),
								 pdrspqexprLastStage));

	if (fSplit2LevelsOnly)
	{
		return pexprSecondStage;
	}

	pdrspqcrLastStage->AddRef();
	CLogicalGbAgg *popGlobalThreeStage = SPQOS_NEW(mp) CLogicalGbAgg(
		mp, pdrspqcrLastStage, COperator::EgbaggtypeGlobal, /* egbaggtype */
		aggStage);
	return SPQOS_NEW(mp) CExpression(
		mp, popGlobalThreeStage, pexprSecondStage,
		SPQOS_NEW(mp) CExpression(mp, SPQOS_NEW(mp) CScalarProjectList(mp),
								 pdrspqexprPrElThirdStage));
}


//---------------------------------------------------------------------------
//	@function:
//		CXformSplitDQA::ExtractDistinctCols
//
//	@doc:
//		Extract arguments of distinct aggs
//
//---------------------------------------------------------------------------
void
CXformSplitDQA::ExtractDistinctCols(
	CMemoryPool *mp, CColumnFactory *col_factory, CMDAccessor *md_accessor,
	CExpression *pexpr, CExpressionArray *pdrspqexprChildPrEl,
	ExprToColRefMap *phmexprcr,
	CColRefArray **ppdrspqcrArgDQA  // output: array of distinct aggs arguments
)
{
	SPQOS_ASSERT(NULL != pdrspqexprChildPrEl);
	SPQOS_ASSERT(NULL != ppdrspqcrArgDQA);
	SPQOS_ASSERT(NULL != phmexprcr);

	const ULONG arity = pexpr->Arity();
	BOOL hasNonSplittableAgg = false;

	// use a set to deduplicate distinct aggs arguments
	CColRefSet *pcrsArgDQA = SPQOS_NEW(mp) CColRefSet(mp);
	ULONG ulDistinct = 0;
	for (ULONG ul = 0; ul < arity; ul++)
	{
		CExpression *pexprPrEl = (*pexpr)[ul];

		// get the scalar child of the project element
		CExpression *pexprAggFunc = (*pexprPrEl)[0];
		CScalarAggFunc *popScAggFunc =
			CScalarAggFunc::PopConvert(pexprAggFunc->Pop());
		hasNonSplittableAgg =
			!md_accessor->RetrieveAgg(popScAggFunc->MDId())->IsSplittable();

		// if an agg fucntion is missing a combine function, then such an agg is
		// called non splittable. Non splittable aggs cannot participate in multi-phase DQAs
		// We do not track missing combine functions per DQA so we cannot have some
		// as single phase and some as multiple phases.
		if (hasNonSplittableAgg)
		{
			break;
		}

		if (popScAggFunc->IsDistinct())
		{
			// CScalarValuesList
			for (ULONG ul = 0; ul < (*pexprAggFunc)[EaggfuncIndexArgs]->Arity();
				 ul++)
			{
				CExpression *pexprArg =
					(*(*pexprAggFunc)[EaggfuncIndexArgs])[ul];
				SPQOS_ASSERT(NULL != pexprArg);
				CColRef *pcrDistinctCol = phmexprcr->Find(pexprArg);
				if (NULL == pcrDistinctCol)
				{
					ulDistinct++;

					// get the column reference of the DQA argument
					pcrDistinctCol =
						PcrAggFuncArgument(mp, md_accessor, col_factory,
										   pexprArg, pdrspqexprChildPrEl);

					// insert into the map between the expression representing the DQA argument
					// and its column reference
					pexprArg->AddRef();
					BOOL fInserted SPQOS_ASSERTS_ONLY =
						phmexprcr->Insert(pexprArg, pcrDistinctCol);
					SPQOS_ASSERT(fInserted);

					// add the distinct column to the set of distinct columns
					pcrsArgDQA->Include(pcrDistinctCol);
				}
			}
		}
	}

	if (1 == ulDistinct && !hasNonSplittableAgg)
	{
		*ppdrspqcrArgDQA = pcrsArgDQA->Pdrspqcr(mp);
	}
	else
	{
		// failed to find a single DQA, or agg is defined as non-splittable
		*ppdrspqcrArgDQA = NULL;
	}
	pcrsArgDQA->Release();
}

// EOF
