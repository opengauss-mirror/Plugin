//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CXformSubqJoin2Apply.cpp
//
//	@doc:
//		Implementation of Inner Join to Apply transform
//---------------------------------------------------------------------------

#include "spqopt/xforms/CXformSubqJoin2Apply.h"

#include "spqos/base.h"

#include "spqopt/operators/CLogicalInnerJoin.h"
#include "spqopt/operators/CLogicalNAryJoin.h"
#include "spqopt/operators/CNormalizer.h"
#include "spqopt/operators/CPatternLeaf.h"
#include "spqopt/operators/CPredicateUtils.h"
#include "spqopt/operators/CScalarProjectElement.h"
#include "spqopt/xforms/CSubqueryHandler.h"
#include "spqopt/xforms/CXformUtils.h"


using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CXformSubqJoin2Apply::CXformSubqJoin2Apply
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CXformSubqJoin2Apply::CXformSubqJoin2Apply(CMemoryPool *mp)
	:  // pattern
	  CXformSubqueryUnnest(SPQOS_NEW(mp) CExpression(
		  mp, SPQOS_NEW(mp) CLogicalInnerJoin(mp),
		  SPQOS_NEW(mp) CExpression(
			  mp, SPQOS_NEW(mp) CPatternLeaf(mp)),  // relational child
		  SPQOS_NEW(mp) CExpression(
			  mp, SPQOS_NEW(mp) CPatternLeaf(mp)),  // relational child
		  SPQOS_NEW(mp)
			  CExpression(mp, SPQOS_NEW(mp) CPatternTree(mp))  // predicate tree
		  ))
{
}

//---------------------------------------------------------------------------
//	@function:
//		CXformSubqJoin2Apply::Exfp
//
//	@doc:
//		Compute xform promise for a given expression handle;
// 		if subqueries exist in the scalar predicate, we must have an
// 		equivalent logical Apply expression created during exploration;
// 		no need for generating a Join expression here
//
//---------------------------------------------------------------------------
CXform::EXformPromise
CXformSubqJoin2Apply::Exfp(CExpressionHandle &exprhdl) const
{
	if (exprhdl.DeriveHasSubquery(exprhdl.Arity() - 1))
	{
		return CXform::ExfpHigh;
	}

	return CXform::ExfpNone;
}

//---------------------------------------------------------------------------
//	@function:
//		CXformSubqJoin2Apply::CollectSubqueries
//
//	@doc:
//		Collect subqueries that exclusively use columns from one join child
//
//---------------------------------------------------------------------------
void
CXformSubqJoin2Apply::CollectSubqueries(
	CMemoryPool *mp, CExpression *pexpr, CColRefSetArray *pdrspqcrs,
	CExpressionArrays
		*pdrspqdrspqexprSubqs	 // array-of-arrays indexed on join child index.
	//  i^{th} entry is an array corresponding to subqueries collected for join child #i
)
{
	SPQOS_CHECK_STACK_SIZE;
	SPQOS_ASSERT(NULL != pexpr);
	SPQOS_ASSERT(NULL != pdrspqcrs);
	SPQOS_ASSERT(NULL != pdrspqdrspqexprSubqs);

	COperator *pop = pexpr->Pop();
	if (CUtils::FSubquery(pop))
	{
		// extract outer references below subquery
		CColRefSet *outer_refs = SPQOS_NEW(mp)
			CColRefSet(mp, *((*pexpr)[0]->DeriveOuterReferences()));

		// add columns used by subquery
		outer_refs->Union(pexpr->DeriveUsedColumns());

		ULONG child_index = spqos::ulong_max;
		const ULONG size = pdrspqcrs->Size();
		for (ULONG ul = 0; ul < size; ul++)
		{
			CColRefSet *pcrsOutput = (*pdrspqcrs)[ul];
			if (pcrsOutput->ContainsAll(outer_refs))
			{
				// outer columns all come from the same join child, break here
				child_index = ul;
				break;
			}
		}

		if (spqos::ulong_max != child_index)
		{
			pexpr->AddRef();
			(*pdrspqdrspqexprSubqs)[child_index]->Append(pexpr);
		}

		outer_refs->Release();
		return;
	}

	// recursively process children
	const ULONG arity = pexpr->Arity();
	for (ULONG ul = 0; ul < arity; ul++)
	{
		CExpression *pexprChild = (*pexpr)[ul];
		CollectSubqueries(mp, pexprChild, pdrspqcrs, pdrspqdrspqexprSubqs);
	}
}


//---------------------------------------------------------------------------
//	@function:
//		CXformSubqJoin2Apply::PexprReplaceSubqueries
//
//	@doc:
//		Replace subqueries with scalar identifiers based on given map
//
//---------------------------------------------------------------------------
CExpression *
CXformSubqJoin2Apply::PexprReplaceSubqueries(CMemoryPool *mp,
											 CExpression *pexprScalar,
											 ExprToColRefMap *phmexprcr)
{
	SPQOS_CHECK_STACK_SIZE;
	SPQOS_ASSERT(NULL != pexprScalar);
	SPQOS_ASSERT(NULL != phmexprcr);

	CColRef *colref = phmexprcr->Find(pexprScalar);
	if (NULL != colref)
	{
		// look-up succeeded on root operator, we return here
		return CUtils::PexprScalarIdent(mp, colref);
	}

	// recursively process children
	const ULONG arity = pexprScalar->Arity();
	CExpressionArray *pdrspqexprChildren = SPQOS_NEW(mp) CExpressionArray(mp);
	for (ULONG ul = 0; ul < arity; ul++)
	{
		CExpression *pexprChild =
			PexprReplaceSubqueries(mp, (*pexprScalar)[ul], phmexprcr);
		pdrspqexprChildren->Append(pexprChild);
	}

	COperator *pop = pexprScalar->Pop();
	pop->AddRef();

	return SPQOS_NEW(mp) CExpression(mp, pop, pdrspqexprChildren);
}


//---------------------------------------------------------------------------
//	@function:
//		CXformSubqJoin2Apply::PexprSubqueryPushdown
//
//	@doc:
//		Push down subquery below join
//
//---------------------------------------------------------------------------
CExpression *
CXformSubqJoin2Apply::PexprSubqueryPushDown(CMemoryPool *mp, CExpression *pexpr,
											BOOL fEnforceCorrelatedApply)
{
	SPQOS_ASSERT(NULL != pexpr);
	SPQOS_ASSERT(COperator::EopLogicalSelect == pexpr->Pop()->Eopid());

	CExpression *pexprJoin = (*pexpr)[0];
	const ULONG arity = pexprJoin->Arity();
	CExpression *pexprScalar = (*pexpr)[1];
	CExpression *join_pred_expr = (*pexprJoin)[arity - 1];
	CLogicalNAryJoin *naryLOJOp =
		CLogicalNAryJoin::PopConvertNAryLOJ(pexprJoin->Pop());

	// collect output columns of all logical children
	CColRefSetArray *pdrspqcrs = SPQOS_NEW(mp) CColRefSetArray(mp);
	CExpressionArrays *pdrspqdrspqexprSubqs = SPQOS_NEW(mp) CExpressionArrays(mp);
	for (ULONG ul = 0; ul < arity - 1; ul++)
	{
		CExpression *pexprChild = (*pexprJoin)[ul];
		CColRefSet *pcrsOutput = NULL;

		if ((NULL == naryLOJOp || naryLOJOp->IsInnerJoinChild(ul)))
		{
			// inner join child
			pcrsOutput = pexprChild->DeriveOutputColumns();
			pcrsOutput->AddRef();
		}
		else
		{
			// use an empty set for right children of LOJs, because we don't want to
			// push any subqueries down to those children (note that non-correlated
			// subqueries will be pushed to the leftmost child, which is never the
			// right child of an LOJ)
			pcrsOutput = SPQOS_NEW(mp) CColRefSet(mp);
		}
		pdrspqcrs->Append(pcrsOutput);

		pdrspqdrspqexprSubqs->Append(SPQOS_NEW(mp) CExpressionArray(mp));
	}

	// collect subqueries that exclusively use columns from each join child
	CollectSubqueries(mp, pexprScalar, pdrspqcrs, pdrspqdrspqexprSubqs);

	// create new join children by pushing subqueries to Project nodes on top
	// of corresponding join children
	CExpressionArray *pdrspqexprNewChildren = SPQOS_NEW(mp) CExpressionArray(mp);
	ExprToColRefMap *phmexprcr = SPQOS_NEW(mp) ExprToColRefMap(mp);
	for (ULONG ulChild = 0; ulChild < arity - 1; ulChild++)
	{
		CExpression *pexprChild = (*pexprJoin)[ulChild];
		pexprChild->AddRef();
		CExpression *pexprNewChild = pexprChild;

		CExpressionArray *pdrspqexprSubqs = (*pdrspqdrspqexprSubqs)[ulChild];
		const ULONG ulSubqs = pdrspqexprSubqs->Size();
		if (0 < ulSubqs)
		{
			// join child has pushable subqueries
			pexprNewChild =
				CUtils::PexprAddProjection(mp, pexprChild, pdrspqexprSubqs);
			CExpression *pexprPrjList = (*pexprNewChild)[1];

			// add pushed subqueries to map
			for (ULONG ulSubq = 0; ulSubq < ulSubqs; ulSubq++)
			{
				CExpression *pexprSubq = (*pdrspqexprSubqs)[ulSubq];
				pexprSubq->AddRef();
				CColRef *colref = CScalarProjectElement::PopConvert(
									  (*pexprPrjList)[ulSubq]->Pop())
									  ->Pcr();
#ifdef SPQOS_DEBUG
				BOOL fInserted =
#endif	// SPQOS_DEBUG
					phmexprcr->Insert(pexprSubq, colref);
				SPQOS_ASSERT(fInserted);
			}

			// unnest subqueries in newly created child
			CExpression *pexprUnnested =
				PexprSubqueryUnnest(mp, pexprNewChild, fEnforceCorrelatedApply);
			if (NULL != pexprUnnested)
			{
				pexprNewChild->Release();
				pexprNewChild = pexprUnnested;
			}
		}

		pdrspqexprNewChildren->Append(pexprNewChild);
	}

	join_pred_expr->AddRef();
	pdrspqexprNewChildren->Append(join_pred_expr);

	// replace subqueries in the original scalar expression with
	// scalar identifiers based on constructed map
	CExpression *pexprNewScalar =
		PexprReplaceSubqueries(mp, pexprScalar, phmexprcr);

	phmexprcr->Release();
	pdrspqcrs->Release();
	pdrspqdrspqexprSubqs->Release();

	// build the new join expression
	COperator *pop = pexprJoin->Pop();
	pop->AddRef();
	CExpression *pexprNewJoin =
		SPQOS_NEW(mp) CExpression(mp, pop, pdrspqexprNewChildren);

	// return a new Select expression
	pop = pexpr->Pop();
	pop->AddRef();
	return SPQOS_NEW(mp) CExpression(mp, pop, pexprNewJoin, pexprNewScalar);
}


//---------------------------------------------------------------------------
//	@function:
//		CXformSubqJoin2Apply::Transform
//
//	@doc:
//		Helper of transformation function
//
//---------------------------------------------------------------------------
void
CXformSubqJoin2Apply::Transform(CXformContext *pxfctxt, CXformResult *pxfres,
								CExpression *pexpr,
								BOOL fEnforceCorrelatedApply) const
{
	SPQOS_ASSERT(NULL != pxfctxt);
	SPQOS_ASSERT(FPromising(pxfctxt->Pmp(), this, pexpr));
	SPQOS_ASSERT(FCheckPattern(pexpr));

	CMemoryPool *mp = pxfctxt->Pmp();
	CExpression *pexprSelect =
		CXformUtils::PexprSeparateSubqueryPreds(mp, pexpr);

	if (NULL == pexprSelect)
	{
		// separating predicates failed, probably because the subquery was in the LOJ parts
		return;
	}

	// attempt pushing subqueries to join children,
	// this optimization may not always succeed since unnested subqueries below joins
	// could hide columns needed to evaluate join condition
	CExpression *pexprSubqsPushedDown =
		PexprSubqueryPushDown(mp, pexprSelect, fEnforceCorrelatedApply);

	// check if join columns in join condition are still accessible after subquery pushdown
	CExpression *pexprJoin = (*pexprSubqsPushedDown)[0];
	CExpression *pexprJoinCondition = (*pexprJoin)[pexprJoin->Arity() - 1];
	CColRefSet *pcrsUsed = pexprJoinCondition->DeriveUsedColumns();
	CColRefSet *pcrsJoinOutput = pexprJoin->DeriveOutputColumns();
	if (!pcrsJoinOutput->ContainsAll(pcrsUsed))
	{
		// discard expression after subquery push down
		pexprSubqsPushedDown->Release();
		pexprSelect->AddRef();
		pexprSubqsPushedDown = pexprSelect;
	}

	pexprSelect->Release();

	CExpression *pexprResult = NULL;
	BOOL fHasSubquery = (*pexprSubqsPushedDown)[1]->DeriveHasSubquery();
	if (fHasSubquery)
	{
		// unnest subqueries remaining in the top Select expression
		pexprResult = PexprSubqueryUnnest(mp, pexprSubqsPushedDown,
										  fEnforceCorrelatedApply);
		pexprSubqsPushedDown->Release();
	}
	else
	{
		pexprResult = pexprSubqsPushedDown;
	}

	if (NULL == pexprResult)
	{
		// unnesting failed, return here
		return;
	}

	// normalize resulting expression and add it to xform results container
	CExpression *pexprNormalized = CNormalizer::PexprNormalize(mp, pexprResult);
	pexprResult->Release();
	pxfres->Add(pexprNormalized);
}


// EOF
