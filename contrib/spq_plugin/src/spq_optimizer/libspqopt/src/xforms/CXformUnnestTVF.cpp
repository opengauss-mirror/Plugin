//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 EMC Corp.
//
//	@filename:
//		CXformUnnestTVF.cpp
//
//	@doc:
//		Implementation of TVF unnesting xform
//---------------------------------------------------------------------------

#include "spqopt/xforms/CXformUnnestTVF.h"

#include "spqos/base.h"
#include "spqos/common/CHashMap.h"

#include "spqopt/operators/CExpressionHandle.h"
#include "spqopt/operators/CLogicalCTEAnchor.h"
#include "spqopt/operators/CLogicalCTEConsumer.h"
#include "spqopt/operators/CLogicalLeftOuterCorrelatedApply.h"
#include "spqopt/operators/CLogicalTVF.h"
#include "spqopt/operators/CPatternMultiTree.h"
#include "spqopt/operators/CScalarProjectElement.h"
#include "spqopt/xforms/CXformUtils.h"

using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CXformUnnestTVF::CXformUnnestTVF
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CXformUnnestTVF::CXformUnnestTVF(CMemoryPool *mp)
	: CXformExploration(
		  // pattern
		  SPQOS_NEW(mp) CExpression(
			  mp, SPQOS_NEW(mp) CLogicalTVF(mp),
			  SPQOS_NEW(mp) CExpression(
				  mp, SPQOS_NEW(mp) CPatternMultiTree(
						  mp))	// variable number of args, each is a deep tree
			  ))
{
}


//---------------------------------------------------------------------------
//	@function:
//		CXformUnnestTVF::Exfp
//
//	@doc:
//		Compute xform promise for a given expression handle
//
//---------------------------------------------------------------------------
CXform::EXformPromise
CXformUnnestTVF::Exfp(CExpressionHandle &exprhdl) const
{
	const ULONG arity = exprhdl.Arity();
	for (ULONG ul = 0; ul < arity; ul++)
	{
		if (exprhdl.DeriveHasSubquery(ul))
		{
			// xform is applicable if TVF argument is a subquery
			return CXform::ExfpHigh;
		}
	}

	return CXform::ExfpNone;
}


//---------------------------------------------------------------------------
//	@function:
//		CXformUnnestTVF::PdrspqcrSubqueries
//
//	@doc:
//		Return array of subquery column references in CTE consumer output
//		after mapping to consumer output
//
//---------------------------------------------------------------------------
CColRefArray *
CXformUnnestTVF::PdrspqcrSubqueries(CMemoryPool *mp, CExpression *pexprCTEProd,
								   CExpression *pexprCTECons)
{
	CExpression *pexprProject = (*pexprCTEProd)[0];
	SPQOS_ASSERT(COperator::EopLogicalProject == pexprProject->Pop()->Eopid());

	CColRefArray *pdrspqcrProdOutput =
		pexprCTEProd->DeriveOutputColumns()->Pdrspqcr(mp);
	CColRefArray *pdrspqcrConsOutput =
		pexprCTECons->DeriveOutputColumns()->Pdrspqcr(mp);
	SPQOS_ASSERT(pdrspqcrProdOutput->Size() == pdrspqcrConsOutput->Size());

	CColRefArray *colref_array = SPQOS_NEW(mp) CColRefArray(mp);
	const ULONG ulPrjElems = (*pexprProject)[1]->Arity();
	for (ULONG ulOuter = 0; ulOuter < ulPrjElems; ulOuter++)
	{
		CExpression *pexprPrjElem = (*(*pexprProject)[1])[ulOuter];
		if ((*pexprPrjElem)[0]->DeriveHasSubquery())
		{
			CColRef *pcrProducer =
				CScalarProjectElement::PopConvert(pexprPrjElem->Pop())->Pcr();
			CColRef *pcrConsumer = CUtils::PcrMap(
				pcrProducer, pdrspqcrProdOutput, pdrspqcrConsOutput);
			SPQOS_ASSERT(NULL != pcrConsumer);

			colref_array->Append(pcrConsumer);
		}
	}

	pdrspqcrProdOutput->Release();
	pdrspqcrConsOutput->Release();

	return colref_array;
}


//---------------------------------------------------------------------------
//	@function:
//		CXformUnnestTVF::PexprProjectSubqueries
//
//	@doc:
//		Collect subquery arguments and return a Project expression with
//		collected subqueries in project list
//
//---------------------------------------------------------------------------
CExpression *
CXformUnnestTVF::PexprProjectSubqueries(CMemoryPool *mp, CExpression *pexprTVF)
{
	SPQOS_ASSERT(COperator::EopLogicalTVF == pexprTVF->Pop()->Eopid());

	// collect subquery arguments
	CExpressionArray *pdrspqexprSubqueries = SPQOS_NEW(mp) CExpressionArray(mp);
	const ULONG arity = pexprTVF->Arity();
	for (ULONG ul = 0; ul < arity; ul++)
	{
		CExpression *pexprScalarChild = (*pexprTVF)[ul];
		if (pexprScalarChild->DeriveHasSubquery())
		{
			pexprScalarChild->AddRef();
			pdrspqexprSubqueries->Append(pexprScalarChild);
		}
	}
	SPQOS_ASSERT(0 < pdrspqexprSubqueries->Size());

	CExpression *pexprCTG = CUtils::PexprLogicalCTGDummy(mp);
	CExpression *pexprProject =
		CUtils::PexprAddProjection(mp, pexprCTG, pdrspqexprSubqueries);
	pdrspqexprSubqueries->Release();

	return pexprProject;
}


//---------------------------------------------------------------------------
//	@function:
//		CXformUnnestTVF::Transform
//
//	@doc:
//		Actual transformation
//		for queries of the form 'SELECT * FROM func(1, (select 5))'
//		we generate a correlated CTE expression to execute subquery args
//		in different subplans, the resulting expression looks like this
//
//		+--CLogicalCTEAnchor (0)
//		   +--CLogicalLeftOuterCorrelatedApply
//			  |--CLogicalTVF (func) Columns: ["a" (0), "b" (1)]
//			  |  |--CScalarConst (1)     <-- constant arg
//			  |  +--CScalarIdent "ColRef_0005" (11)    <-- subquery arg replaced by column
//			  |--CLogicalCTEConsumer (0), Columns: ["ColRef_0005" (11)]
//			  +--CScalarConst (1)
//
//		where CTE(0) is a Project expression on subquery args
//
//
//---------------------------------------------------------------------------
void
CXformUnnestTVF::Transform(CXformContext *pxfctxt, CXformResult *pxfres,
						   CExpression *pexpr) const
{
	SPQOS_ASSERT(NULL != pxfctxt);
	SPQOS_ASSERT(FPromising(pxfctxt->Pmp(), this, pexpr));
	SPQOS_ASSERT(FCheckPattern(pexpr));

	CMemoryPool *mp = pxfctxt->Pmp();

	// create a project expression on subquery arguments
	CExpression *pexprProject = PexprProjectSubqueries(mp, pexpr);

	// create a CTE producer on top of the project
	CCTEInfo *pcteinfo = COptCtxt::PoctxtFromTLS()->Pcteinfo();
	const ULONG ulCTEId = pcteinfo->next_id();

	// construct CTE producer output from subquery columns
	CColRefArray *pdrspqcrOutput = SPQOS_NEW(mp) CColRefArray(mp);
	const ULONG ulPrjElems = (*pexprProject)[1]->Arity();
	for (ULONG ulOuter = 0; ulOuter < ulPrjElems; ulOuter++)
	{
		CExpression *pexprPrjElem = (*(*pexprProject)[1])[ulOuter];
		if ((*pexprPrjElem)[0]->DeriveHasSubquery())
		{
			CColRef *pcrSubq =
				CScalarProjectElement::PopConvert(pexprPrjElem->Pop())->Pcr();
			pdrspqcrOutput->Append(pcrSubq);
		}
	}

	CExpression *pexprCTEProd = CXformUtils::PexprAddCTEProducer(
		mp, ulCTEId, pdrspqcrOutput, pexprProject);
	pdrspqcrOutput->Release();
	pexprProject->Release();

	// create CTE consumer
	CColRefArray *pdrspqcrProducerOutput =
		pexprCTEProd->DeriveOutputColumns()->Pdrspqcr(mp);
	CColRefArray *pdrspqcrConsumerOutput =
		CUtils::PdrspqcrCopy(mp, pdrspqcrProducerOutput);
	CLogicalCTEConsumer *popConsumer =
		SPQOS_NEW(mp) CLogicalCTEConsumer(mp, ulCTEId, pdrspqcrConsumerOutput);
	CExpression *pexprCTECons = SPQOS_NEW(mp) CExpression(mp, popConsumer);
	pcteinfo->IncrementConsumers(ulCTEId);
	pdrspqcrProducerOutput->Release();

	// find columns corresponding to subqueries in consumer's output
	CColRefArray *pdrspqcrSubqueries =
		PdrspqcrSubqueries(mp, pexprCTEProd, pexprCTECons);

	// create new function arguments by replacing subqueries with columns in CTE consumer output
	CExpressionArray *pdrspqexprNewArgs = SPQOS_NEW(mp) CExpressionArray(mp);
	ULONG ulIndex = 0;
	const ULONG arity = pexpr->Arity();
	for (ULONG ul = 0; ul < arity; ul++)
	{
		CExpression *pexprScalarChild = (*pexpr)[ul];
		if (pexprScalarChild->DeriveHasSubquery())
		{
			CColRef *colref = (*pdrspqcrSubqueries)[ulIndex];
			pdrspqexprNewArgs->Append(CUtils::PexprScalarIdent(mp, colref));
			ulIndex++;
		}
		else
		{
			(*pexpr)[ul]->AddRef();
			pdrspqexprNewArgs->Append((*pexpr)[ul]);
		}
	}

	// finally, create correlated apply expression
	CLogicalTVF *popTVF = CLogicalTVF::PopConvert(pexpr->Pop());
	popTVF->AddRef();
	CExpression *pexprCorrApply =
		CUtils::PexprLogicalApply<CLogicalLeftOuterCorrelatedApply>(
			mp, SPQOS_NEW(mp) CExpression(mp, popTVF, pdrspqexprNewArgs),
			pexprCTECons, pdrspqcrSubqueries, COperator::EopScalarSubquery,
			CPredicateUtils::PexprConjunction(
				mp, NULL /*pdrspqexpr*/)	 // scalar expression is const True
		);

	CExpression *pexprAlt = SPQOS_NEW(mp) CExpression(
		mp, SPQOS_NEW(mp) CLogicalCTEAnchor(mp, ulCTEId), pexprCorrApply);

	// add alternative to transformation result
	pxfres->Add(pexprAlt);
}


// EOF
