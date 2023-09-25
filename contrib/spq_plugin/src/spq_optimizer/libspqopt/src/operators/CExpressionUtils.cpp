//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal, Inc.
//
//	@filename:
//		CExpressionUtils.cpp
//
//	@doc:
//		Utility routines for transforming expressions
//
//	@owner:
//		,
//
//	@test:
//
//---------------------------------------------------------------------------

#include "spqopt/operators/CExpressionUtils.h"

#include "spqopt/base/CColRefSetIter.h"
#include "spqopt/base/CColRefTable.h"
#include "spqopt/base/CConstraintInterval.h"
#include "spqopt/base/CUtils.h"
#include "spqopt/exception.h"
#include "spqopt/mdcache/CMDAccessor.h"
#include "spqopt/operators/CLogicalJoin.h"
#include "spqopt/operators/CPredicateUtils.h"
#include "spqopt/operators/CScalarSubqueryExists.h"
#include "spqopt/operators/CScalarSubqueryNotExists.h"
#include "naucrates/md/IMDScalarOp.h"
#include "naucrates/md/IMDType.h"
#include "naucrates/statistics/CStatistics.h"

using namespace spqopt;

//---------------------------------------------------------------------------
//	@function:
//		CExpressionUtils::UnnestChild
//
//	@doc:
//		Unnest a given expression's child and append unnested nodes to
//		the given expression array
//
//---------------------------------------------------------------------------
void
CExpressionUtils::UnnestChild(
	CMemoryPool *mp,
	CExpression *pexpr,			 // parent node
	ULONG child_index,			 // child index
	BOOL fAnd,					 // is expression an AND node?
	BOOL fOr,					 // is expression an OR node?
	BOOL fHasNegatedChild,		 // does expression have NOT child nodes?
	CExpressionArray *pdrspqexpr	 // array to append results to
)
{
	SPQOS_ASSERT(NULL != mp);
	SPQOS_ASSERT(NULL != pexpr);
	SPQOS_ASSERT(child_index < pexpr->Arity());
	SPQOS_ASSERT(NULL != pdrspqexpr);

	CExpression *pexprChild = (*pexpr)[child_index];

	if ((fAnd && CPredicateUtils::FAnd(pexprChild)) ||
		(fOr && CPredicateUtils::FOr(pexprChild)))
	{
		// two cascaded AND nodes or two cascaded OR nodes, recursively
		// pull-up children
		AppendChildren(mp, pexprChild, pdrspqexpr);

		return;
	}

	if (fHasNegatedChild && CPredicateUtils::FNot(pexprChild) &&
		CPredicateUtils::FNot((*pexprChild)[0]))
	{
		// two cascaded Not nodes cancel each other
		CExpression *pexprNot = (*pexprChild)[0];
		pexprChild = (*pexprNot)[0];
	}
	CExpression *pexprUnnestedChild = PexprUnnest(mp, pexprChild);
	pdrspqexpr->Append(pexprUnnestedChild);
}

//---------------------------------------------------------------------------
//	@function:
//		CExpressionUtils::AppendChildren
//
//	@doc:
//		Append the unnested children of given expression to given array
//
//---------------------------------------------------------------------------
void
CExpressionUtils::AppendChildren(CMemoryPool *mp, CExpression *pexpr,
								 CExpressionArray *pdrspqexpr)
{
	SPQOS_ASSERT(NULL != mp);
	SPQOS_ASSERT(NULL != pexpr);
	SPQOS_ASSERT(NULL != pdrspqexpr);

	CExpressionArray *pdrspqexprChildren = PdrspqexprUnnestChildren(mp, pexpr);
	CUtils::AddRefAppend<CExpression, CleanupRelease>(pdrspqexpr,
													  pdrspqexprChildren);
	pdrspqexprChildren->Release();
}

//---------------------------------------------------------------------------
//	@function:
//		CExpressionUtils::PdrspqexprUnnestChildren
//
//	@doc:
//		Return an array of expression's children after unnesting nested
//		AND/OR/NOT subtrees
//
//---------------------------------------------------------------------------
CExpressionArray *
CExpressionUtils::PdrspqexprUnnestChildren(CMemoryPool *mp, CExpression *pexpr)
{
	// protect against stack overflow during recursion
	SPQOS_CHECK_STACK_SIZE;
	SPQOS_ASSERT(NULL != mp);
	SPQOS_ASSERT(NULL != pexpr);

	// compute flags for cases where we may have nested predicates
	BOOL fAnd = CPredicateUtils::FAnd(pexpr);
	BOOL fOr = CPredicateUtils::FOr(pexpr);
	BOOL fHasNegatedChild = CPredicateUtils::FHasNegatedChild(pexpr);

	CExpressionArray *pdrspqexpr = SPQOS_NEW(mp) CExpressionArray(mp);
	const ULONG arity = pexpr->Arity();
	for (ULONG ul = 0; ul < arity; ul++)
	{
		UnnestChild(mp, pexpr, ul, fAnd, fOr, fHasNegatedChild, pdrspqexpr);
	}

	return pdrspqexpr;
}

//---------------------------------------------------------------------------
//	@function:
//		CExpressionUtils::PexprUnnest
//
//	@doc:
//		Unnest AND/OR/NOT predicates
//
//---------------------------------------------------------------------------
CExpression *
CExpressionUtils::PexprUnnest(CMemoryPool *mp, CExpression *pexpr)
{
	// protect against stack overflow during recursion
	SPQOS_CHECK_STACK_SIZE;
	SPQOS_ASSERT(NULL != mp);
	SPQOS_ASSERT(NULL != pexpr);

	if (CPredicateUtils::FNot(pexpr))
	{
		CExpression *pexprChild = (*pexpr)[0];
		CExpression *pexprPushedNot = PexprPushNotOneLevel(mp, pexprChild);

		COperator *pop = pexprPushedNot->Pop();
		CExpressionArray *pdrspqexpr =
			PdrspqexprUnnestChildren(mp, pexprPushedNot);
		pop->AddRef();

		// clean up
		pexprPushedNot->Release();

		return SPQOS_NEW(mp) CExpression(mp, pop, pdrspqexpr);
	}

	COperator *pop = pexpr->Pop();
	CExpressionArray *pdrspqexpr = PdrspqexprUnnestChildren(mp, pexpr);
	pop->AddRef();

	return SPQOS_NEW(mp) CExpression(mp, pop, pdrspqexpr);
}


//---------------------------------------------------------------------------
//	@function:
//		CExpressionUtils::PexprPushNotOneLevel
//
//	@doc:
// 		Push not expression one level down the given expression. For example:
// 		1. AND of expressions into an OR a negation of these expression
// 		2. OR of expressions into an AND a negation of these expression
// 		3. EXISTS into NOT EXISTS and vice versa
//      4. Else, return NOT of given expression
//---------------------------------------------------------------------------
CExpression *
CExpressionUtils::PexprPushNotOneLevel(CMemoryPool *mp, CExpression *pexpr)
{
	SPQOS_ASSERT(NULL != pexpr);

	BOOL fAnd = CPredicateUtils::FAnd(pexpr);
	BOOL fOr = CPredicateUtils::FOr(pexpr);

	if (fAnd || fOr)
	{
		COperator *popNew = NULL;

		if (fOr)
		{
			popNew = SPQOS_NEW(mp) CScalarBoolOp(mp, CScalarBoolOp::EboolopAnd);
		}
		else
		{
			popNew = SPQOS_NEW(mp) CScalarBoolOp(mp, CScalarBoolOp::EboolopOr);
		}

		CExpressionArray *pdrspqexpr = SPQOS_NEW(mp) CExpressionArray(mp);
		const ULONG arity = pexpr->Arity();
		for (ULONG ul = 0; ul < arity; ul++)
		{
			CExpression *pexprChild = (*pexpr)[ul];
			pexprChild->AddRef();
			pdrspqexpr->Append(CUtils::PexprNegate(mp, pexprChild));
		}

		return SPQOS_NEW(mp) CExpression(mp, popNew, pdrspqexpr);
	}

	const COperator *pop = pexpr->Pop();
	if (COperator::EopScalarSubqueryExists == pop->Eopid())
	{
		pexpr->PdrgPexpr()->AddRef();
		return SPQOS_NEW(mp) CExpression(
			mp, SPQOS_NEW(mp) CScalarSubqueryNotExists(mp), pexpr->PdrgPexpr());
	}

	if (COperator::EopScalarSubqueryNotExists == pop->Eopid())
	{
		pexpr->PdrgPexpr()->AddRef();
		return SPQOS_NEW(mp) CExpression(
			mp, SPQOS_NEW(mp) CScalarSubqueryExists(mp), pexpr->PdrgPexpr());
	}

	// TODO: , Feb 4 2015, we currently only handling EXISTS/NOT EXISTS/AND/OR
	pexpr->AddRef();
	return SPQOS_NEW(mp) CExpression(
		mp, SPQOS_NEW(mp) CScalarBoolOp(mp, CScalarBoolOp::EboolopNot), pexpr);
}


//---------------------------------------------------------------------------
//	@function:
//		CExpressionUtils::PexprDedupChildren
//
//	@doc:
//		Remove duplicate AND/OR children
//
//---------------------------------------------------------------------------
CExpression *
CExpressionUtils::PexprDedupChildren(CMemoryPool *mp, CExpression *pexpr)
{
	// protect against stack overflow during recursion
	SPQOS_CHECK_STACK_SIZE;
	SPQOS_ASSERT(NULL != mp);
	SPQOS_ASSERT(NULL != pexpr);

	// recursively process children
	const ULONG arity = pexpr->Arity();
	CExpressionArray *pdrspqexprChildren = SPQOS_NEW(mp) CExpressionArray(mp);
	for (ULONG ul = 0; ul < arity; ul++)
	{
		CExpression *pexprChild = PexprDedupChildren(mp, (*pexpr)[ul]);
		pdrspqexprChildren->Append(pexprChild);
	}

	if (CPredicateUtils::FAnd(pexpr) || CPredicateUtils::FOr(pexpr))
	{
		CExpressionArray *pdrspqexprNewChildren =
			CUtils::PdrspqexprDedup(mp, pdrspqexprChildren);

		pdrspqexprChildren->Release();
		pdrspqexprChildren = pdrspqexprNewChildren;

		// Check if we end with one child, return that child
		if (1 == pdrspqexprChildren->Size())
		{
			CExpression *pexprChild = (*pdrspqexprChildren)[0];
			pexprChild->AddRef();
			pdrspqexprChildren->Release();

			return pexprChild;
		}
	}


	COperator *pop = pexpr->Pop();
	pop->AddRef();
	return SPQOS_NEW(mp) CExpression(mp, pop, pdrspqexprChildren);
}

// if the expression is a LogicalSelect and contains correlated EXISTS/ANY subqueries,
// extract those subqueries and return the derived constraint property from subqueries.
// this is useful when we try to infer and propagate predicates from subquery to outer
// relation. returns null if the expression doesn't satisfy aforementioned requirements.
CPropConstraint *
CExpressionUtils::GetPropConstraintFromSubquery(CMemoryPool *mp,
												CExpression *pexpr)
{
	SPQOS_ASSERT(NULL != pexpr);
	if (pexpr->Pop()->Eopid() != COperator::EopLogicalSelect)
	{
		return NULL;
	}

	CExpression *filter = (*pexpr)[1];
	CExpressionArray *subqueries = SPQOS_NEW(mp) CExpressionArray(mp);

	if (CUtils::FCorrelatedExistsAnySubquery(filter))
	{
		filter->AddRef();
		subqueries->Append(filter);
	}
	else if (CPredicateUtils::FAnd(filter))
	{
		const ULONG arity = filter->Arity();
		for (ULONG ul = 0; ul < arity; ul++)
		{
			CExpression *childFilter = (*filter)[ul];
			if (CUtils::FCorrelatedExistsAnySubquery(childFilter))
			{
				childFilter->AddRef();
				subqueries->Append(childFilter);
			}
		}
	}
	else
	{
		subqueries->Release();
		return NULL;
	}

	if (subqueries->Size() == 0)
	{
		subqueries->Release();
		return NULL;
	}

	CExpression *exprSubquery =
		CPredicateUtils::PexprConjunction(mp, subqueries);
	CColRefSetArray *colRefSetArray = NULL;
	CConstraint *pcnstr =
		CConstraint::PcnstrFromScalarExpr(mp, exprSubquery, &colRefSetArray);
	exprSubquery->Release();

	if (NULL == pcnstr)
	{
		CRefCount::SafeRelease(colRefSetArray);
		return NULL;
	}
	return SPQOS_NEW(mp) CPropConstraint(mp, colRefSetArray, pcnstr);
}
// EOF
