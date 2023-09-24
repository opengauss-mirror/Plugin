//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2015 Pivotal Inc.
//
//	@filename:
//		CXformGbAggWithMDQA2Join.cpp
//
//	@doc:
//		Transform a GbAgg with multiple distinct qualified aggregates (MDQAs)
//		to a join tree with single DQA leaves
//---------------------------------------------------------------------------

#include "spqopt/xforms/CXformGbAggWithMDQA2Join.h"

#include "spqos/base.h"
#include "spqos/memory/CAutoMemoryPool.h"

#include "spqopt/base/CKeyCollection.h"
#include "spqopt/base/CUtils.h"
#include "spqopt/operators/CLogicalCTEAnchor.h"
#include "spqopt/operators/CLogicalCTEConsumer.h"
#include "spqopt/operators/CLogicalGbAgg.h"
#include "spqopt/operators/COperator.h"
#include "spqopt/xforms/CXformUtils.h"

using namespace spqmd;
using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CXformGbAggWithMDQA2Join::CXformGbAggWithMDQA2Join
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CXformGbAggWithMDQA2Join::CXformGbAggWithMDQA2Join(CMemoryPool *mp)
	: CXformExploration(
		  // pattern
		  SPQOS_NEW(mp) CExpression(
			  mp, SPQOS_NEW(mp) CLogicalGbAgg(mp),
			  SPQOS_NEW(mp) CExpression(
				  mp, SPQOS_NEW(mp) CPatternTree(mp)),  // relational child
			  SPQOS_NEW(mp) CExpression(
				  mp, SPQOS_NEW(mp) CPatternTree(mp))  // scalar project list
			  ))
{
}


//---------------------------------------------------------------------------
//	@function:
//		CXformGbAggWithMDQA2Join::Exfp
//
//	@doc:
//		Compute xform promise for a given expression handle;
//
//---------------------------------------------------------------------------
CXform::EXformPromise
CXformGbAggWithMDQA2Join::Exfp(CExpressionHandle &exprhdl) const
{
	CAutoMemoryPool amp;

	CLogicalGbAgg *popAgg = CLogicalGbAgg::PopConvert(exprhdl.Pop());

	if (COperator::EgbaggtypeGlobal == popAgg->Egbaggtype() &&
		exprhdl.DeriveHasMultipleDistinctAggs(1))
	{
		return CXform::ExfpHigh;
	}

	return CXform::ExfpNone;
}


//---------------------------------------------------------------------------
//	@function:
//		CXformGbAggWithMDQA2Join::PexprMDQAs2Join
//
//	@doc:
//		Converts GbAgg with multiple distinct aggregates into a join of single
//		distinct aggregates,
//
//		distinct aggregates that share the same argument are grouped together
//		in one leaf of the generated join expression,
//
//		non-distinct aggregates are also grouped together in one leaf of the
//		generated join expression
//
//---------------------------------------------------------------------------
CExpression *
CXformGbAggWithMDQA2Join::PexprMDQAs2Join(CMemoryPool *mp, CExpression *pexpr)
{
	SPQOS_ASSERT(NULL != pexpr);
	SPQOS_ASSERT(COperator::EopLogicalGbAgg == pexpr->Pop()->Eopid());
	SPQOS_ASSERT((*pexpr)[1]->DeriveHasMultipleDistinctAggs());

	// extract components
	CExpression *pexprChild = (*pexpr)[0];

	CColRefSet *pcrsChildOutput = pexprChild->DeriveOutputColumns();
	CColRefArray *pdrspqcrChildOutput = pcrsChildOutput->Pdrspqcr(mp);

	// create a CTE producer based on child expression
	CCTEInfo *pcteinfo = COptCtxt::PoctxtFromTLS()->Pcteinfo();
	const ULONG ulCTEId = pcteinfo->next_id();
	(void) CXformUtils::PexprAddCTEProducer(mp, ulCTEId, pdrspqcrChildOutput,
											pexprChild);

	// create a CTE consumer with child output columns
	CExpression *pexprConsumer = SPQOS_NEW(mp) CExpression(
		mp, SPQOS_NEW(mp) CLogicalCTEConsumer(mp, ulCTEId, pdrspqcrChildOutput));
	pcteinfo->IncrementConsumers(ulCTEId);

	// finalize GbAgg expression by replacing its child with CTE consumer
	pexpr->Pop()->AddRef();
	(*pexpr)[1]->AddRef();
	CExpression *pexprGbAggWithConsumer =
		SPQOS_NEW(mp) CExpression(mp, pexpr->Pop(), pexprConsumer, (*pexpr)[1]);

	CExpression *pexprJoinDQAs =
		CXformUtils::PexprGbAggOnCTEConsumer2Join(mp, pexprGbAggWithConsumer);
	SPQOS_ASSERT(NULL != pexprJoinDQAs);

	pexprGbAggWithConsumer->Release();

	return SPQOS_NEW(mp) CExpression(
		mp, SPQOS_NEW(mp) CLogicalCTEAnchor(mp, ulCTEId), pexprJoinDQAs);
}


//---------------------------------------------------------------------------
//	@function:
//		CXformGbAggWithMDQA2Join::PexprExpandMDQAs
//
//	@doc:
//		Expand GbAgg with multiple distinct aggregates into a join of single
//		distinct aggregates,
//		return NULL if expansion is not done
//
//---------------------------------------------------------------------------
CExpression *
CXformGbAggWithMDQA2Join::PexprExpandMDQAs(CMemoryPool *mp, CExpression *pexpr)
{
	SPQOS_ASSERT(NULL != pexpr);
	SPQOS_ASSERT(COperator::EopLogicalGbAgg == pexpr->Pop()->Eopid());

	COperator *pop = pexpr->Pop();
	if (CLogicalGbAgg::PopConvert(pop)->FGlobal())
	{
		BOOL fHasMultipleDistinctAggs =
			(*pexpr)[1]->DeriveHasMultipleDistinctAggs();
		if (fHasMultipleDistinctAggs)
		{
			CExpression *pexprExpanded = PexprMDQAs2Join(mp, pexpr);

			// recursively process the resulting expression
			CExpression *pexprResult = PexprTransform(mp, pexprExpanded);
			pexprExpanded->Release();

			return pexprResult;
		}
	}

	return NULL;
}


//---------------------------------------------------------------------------
//	@function:
//		CXformGbAggWithMDQA2Join::PexprTransform
//
//	@doc:
//		Main transformation driver
//
//---------------------------------------------------------------------------
CExpression *
CXformGbAggWithMDQA2Join::PexprTransform(CMemoryPool *mp, CExpression *pexpr)
{
	// protect against stack overflow during recursion
	SPQOS_CHECK_STACK_SIZE;
	SPQOS_ASSERT(NULL != mp);
	SPQOS_ASSERT(NULL != pexpr);

	COperator *pop = pexpr->Pop();
	if (COperator::EopLogicalGbAgg == pop->Eopid())
	{
		CExpression *pexprResult = PexprExpandMDQAs(mp, pexpr);
		if (NULL != pexprResult)
		{
			return pexprResult;
		}
	}

	// recursively process child expressions
	const ULONG arity = pexpr->Arity();
	CExpressionArray *pdrspqexprChildren = SPQOS_NEW(mp) CExpressionArray(mp);
	for (ULONG ul = 0; ul < arity; ul++)
	{
		CExpression *pexprChild = PexprTransform(mp, (*pexpr)[ul]);
		pdrspqexprChildren->Append(pexprChild);
	}

	pop->AddRef();
	return SPQOS_NEW(mp) CExpression(mp, pop, pdrspqexprChildren);
}


//---------------------------------------------------------------------------
//	@function:
//		CXformGbAggWithMDQA2Join::Transform
//
//	@doc:
//		Actual transformation to expand multiple distinct qualified aggregates
//		(MDQAs) to a join tree with single DQA leaves
//
//---------------------------------------------------------------------------
void
CXformGbAggWithMDQA2Join::Transform(CXformContext *pxfctxt,
									CXformResult *pxfres,
									CExpression *pexpr) const
{
	SPQOS_ASSERT(NULL != pxfctxt);
	SPQOS_ASSERT(NULL != pxfres);
	SPQOS_ASSERT(FPromising(pxfctxt->Pmp(), this, pexpr));
	SPQOS_ASSERT(FCheckPattern(pexpr));

	CMemoryPool *mp = pxfctxt->Pmp();

	CExpression *pexprResult = PexprTransform(mp, pexpr);
	if (NULL != pexprResult)
	{
		pxfres->Add(pexprResult);
	}
}

BOOL
CXformGbAggWithMDQA2Join::IsApplyOnce()
{
	return true;
}
// EOF
