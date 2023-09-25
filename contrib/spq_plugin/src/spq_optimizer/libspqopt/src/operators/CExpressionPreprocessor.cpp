//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright 2012 EMC Corp.
//
//	@filename:
//		CExpressionPreprocessor.cpp
//
//	@doc:
//		Expression tree preprocessing routines, needed to prepare an input
//		logical expression to be optimized
//---------------------------------------------------------------------------

#include "spqopt/operators/CExpressionPreprocessor.h"

#include "spqos/base.h"
#include "spqos/common/CAutoRef.h"
#include "spqos/common/CAutoTimer.h"

#include "spqopt/base/CColRefSetIter.h"
#include "spqopt/base/CColRefTable.h"
#include "spqopt/base/CConstraintInterval.h"
#include "spqopt/base/CUtils.h"
#include "spqopt/exception.h"
#include "spqopt/mdcache/CMDAccessor.h"
#include "spqopt/operators/CExpressionFactorizer.h"
#include "spqopt/operators/CExpressionUtils.h"
#include "spqopt/operators/CLogicalCTEAnchor.h"
#include "spqopt/operators/CLogicalCTEConsumer.h"
#include "spqopt/operators/CLogicalCTEProducer.h"
#include "spqopt/operators/CLogicalConstTableGet.h"
#include "spqopt/operators/CLogicalGbAgg.h"
#include "spqopt/operators/CLogicalInnerJoin.h"
#include "spqopt/operators/CLogicalLimit.h"
#include "spqopt/operators/CLogicalNAryJoin.h"
#include "spqopt/operators/CLogicalProject.h"
#include "spqopt/operators/CLogicalSelect.h"
#include "spqopt/operators/CLogicalSequenceProject.h"
#include "spqopt/operators/CLogicalSetOp.h"
#include "spqopt/operators/CLogicalUnion.h"
#include "spqopt/operators/CLogicalUnionAll.h"
#include "spqopt/operators/CNormalizer.h"
#include "spqopt/operators/COrderedAggPreprocessor.h"
#include "spqopt/operators/CPredicateUtils.h"
#include "spqopt/operators/CScalarCmp.h"
#include "spqopt/operators/CScalarIdent.h"
#include "spqopt/operators/CScalarNAryJoinPredList.h"
#include "spqopt/operators/CScalarProjectElement.h"
#include "spqopt/operators/CScalarProjectList.h"
#include "spqopt/operators/CScalarSubquery.h"
#include "spqopt/operators/CScalarSubqueryAll.h"
#include "spqopt/operators/CScalarSubqueryAny.h"
#include "spqopt/operators/CScalarSubqueryExists.h"
#include "spqopt/operators/CScalarSubqueryQuantified.h"
#include "spqopt/operators/CWindowPreprocessor.h"
#include "spqopt/optimizer/COptimizerConfig.h"
#include "spqopt/xforms/CXform.h"
#include "naucrates/md/IMDScalarOp.h"
#include "naucrates/md/IMDType.h"
#include "naucrates/statistics/CStatistics.h"
#include "naucrates/traceflags/traceflags.h"

using namespace spqopt;

// maximum number of equality predicates to be derived from existing equalities
#define SPQOPT_MAX_DERIVED_PREDS 50

// eliminate self comparisons in the given expression
CExpression *
CExpressionPreprocessor::PexprEliminateSelfComparison(CMemoryPool *mp,
													  CExpression *pexpr)
{
	// protect against stack overflow during recursion
	SPQOS_CHECK_STACK_SIZE;
	SPQOS_ASSERT(NULL != mp);
	SPQOS_ASSERT(NULL != pexpr);

	if (CUtils::FScalarCmp(pexpr))
	{
		return CPredicateUtils::PexprEliminateSelfComparison(mp, pexpr);
	}

	// recursively process children
	const ULONG arity = pexpr->Arity();
	CExpressionArray *pdrspqexprChildren = SPQOS_NEW(mp) CExpressionArray(mp);
	for (ULONG ul = 0; ul < arity; ul++)
	{
		CExpression *pexprChild =
			PexprEliminateSelfComparison(mp, (*pexpr)[ul]);
		pdrspqexprChildren->Append(pexprChild);
	}

	COperator *pop = pexpr->Pop();
	pop->AddRef();

	return SPQOS_NEW(mp) CExpression(mp, pop, pdrspqexprChildren);
}

// remove superfluous equality operations
CExpression *
CExpressionPreprocessor::PexprPruneSuperfluousEquality(CMemoryPool *mp,
													   CExpression *pexpr)
{
	// protect against stack overflow during recursion
	SPQOS_CHECK_STACK_SIZE;
	SPQOS_ASSERT(NULL != mp);
	SPQOS_ASSERT(NULL != pexpr);

	if (pexpr->Pop()->FScalar())
	{
		return CPredicateUtils::PexprPruneSuperfluosEquality(mp, pexpr);
	}

	// recursively process children
	const ULONG arity = pexpr->Arity();
	CExpressionArray *pdrspqexprChildren = SPQOS_NEW(mp) CExpressionArray(mp);
	for (ULONG ul = 0; ul < arity; ul++)
	{
		CExpression *pexprChild =
			PexprPruneSuperfluousEquality(mp, (*pexpr)[ul]);
		pdrspqexprChildren->Append(pexprChild);
	}

	COperator *pop = pexpr->Pop();
	pop->AddRef();
	return SPQOS_NEW(mp) CExpression(mp, pop, pdrspqexprChildren);
}

// an existential subquery whose inner expression is a GbAgg
// with no grouping columns is replaced with a Boolean constant
//
//		Example:
//
//			exists(select sum(i) from X) --> True
//			not exists(select sum(i) from X) --> False
CExpression *
CExpressionPreprocessor::PexprTrimExistentialSubqueries(CMemoryPool *mp,
														CExpression *pexpr)
{
	// protect against stack overflow during recursion
	SPQOS_CHECK_STACK_SIZE;
	SPQOS_ASSERT(NULL != mp);
	SPQOS_ASSERT(NULL != pexpr);

	COperator *pop = pexpr->Pop();
	if (CUtils::FExistentialSubquery(pop))
	{
		CExpression *pexprInner = (*pexpr)[0];
		if (COperator::EopLogicalGbAgg == pexprInner->Pop()->Eopid() &&
			0 ==
				CLogicalGbAgg::PopConvert(pexprInner->Pop())->Pdrspqcr()->Size())
		{
			SPQOS_ASSERT(0 < (*pexprInner)[1]->Arity() &&
						"Project list of GbAgg is expected to be non-empty");
			BOOL fValue = true;
			if (COperator::EopScalarSubqueryNotExists == pop->Eopid())
			{
				fValue = false;
			}
			return CUtils::PexprScalarConstBool(mp, fValue);
		}
	}

	// recursively process children
	const ULONG arity = pexpr->Arity();
	CExpressionArray *pdrspqexprChildren = SPQOS_NEW(mp) CExpressionArray(mp);
	for (ULONG ul = 0; ul < arity; ul++)
	{
		CExpression *pexprChild =
			PexprTrimExistentialSubqueries(mp, (*pexpr)[ul]);
		pdrspqexprChildren->Append(pexprChild);
	}

	if (CPredicateUtils::FAnd(pexpr))
	{
		return CPredicateUtils::PexprConjunction(mp, pdrspqexprChildren);
	}

	if (CPredicateUtils::FOr(pexpr))
	{
		return CPredicateUtils::PexprDisjunction(mp, pdrspqexprChildren);
	}

	pop->AddRef();
	return SPQOS_NEW(mp) CExpression(mp, pop, pdrspqexprChildren);
}



// a quantified subquery with maxcard 1 is simplified as a scalar subquery
//
// Example:
//		a = ANY (select sum(i) from X) --> a = (select sum(i) from X)
//		a <> ALL (select sum(i) from X) --> a <> (select sum(i) from X)
CExpression *
CExpressionPreprocessor::PexprSimplifyQuantifiedSubqueries(CMemoryPool *mp,
														   CExpression *pexpr)
{
	// protect against stack overflow during recursion
	SPQOS_CHECK_STACK_SIZE;
	SPQOS_ASSERT(NULL != mp);
	SPQOS_ASSERT(NULL != pexpr);

	COperator *pop = pexpr->Pop();
	if (CUtils::FQuantifiedSubquery(pop) &&
		1 == (*pexpr)[0]->DeriveMaxCard().Ull())
	{
		CExpression *pexprInner = (*pexpr)[0];

		// skip intermediate unary nodes
		CExpression *pexprChild = pexprInner;
		COperator *popChild = pexprChild->Pop();
		while (NULL != pexprChild && CUtils::FLogicalUnary(popChild))
		{
			pexprChild = (*pexprChild)[0];
			popChild = pexprChild->Pop();
		}

		// inspect next node
		BOOL fGbAggWithoutGrpCols =
			COperator::EopLogicalGbAgg == popChild->Eopid() &&
			0 == CLogicalGbAgg::PopConvert(popChild)->Pdrspqcr()->Size();

		BOOL fOneRowConstTable =
			COperator::EopLogicalConstTableGet == popChild->Eopid() &&
			1 == CLogicalConstTableGet::PopConvert(popChild)
					 ->Pdrspqdrspqdatum()
					 ->Size();

		if (fGbAggWithoutGrpCols || fOneRowConstTable)
		{
			// quantified subquery with max card 1
			CExpression *pexprScalar = (*pexpr)[1];
			CScalarSubqueryQuantified *popSubqQuantified =
				CScalarSubqueryQuantified::PopConvert(pexpr->Pop());
			const CColRef *colref = popSubqQuantified->Pcr();
			pexprInner->AddRef();
			CExpression *pexprSubquery = SPQOS_NEW(mp) CExpression(
				mp,
				SPQOS_NEW(mp)
					CScalarSubquery(mp, colref, false /*fGeneratedByExist*/,
									true /*fGeneratedByQuantified*/),
				pexprInner);

			CMDAccessor *md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();
			IMDId *mdid = popSubqQuantified->MdIdOp();
			const CWStringConst *str =
				md_accessor->RetrieveScOp(mdid)->Mdname().GetMDName();
			mdid->AddRef();
			pexprScalar->AddRef();

			return CUtils::PexprScalarCmp(mp, pexprScalar, pexprSubquery, *str,
										  mdid);
		}
	}

	// recursively process children
	const ULONG arity = pexpr->Arity();
	CExpressionArray *pdrspqexprChildren = SPQOS_NEW(mp) CExpressionArray(mp);
	for (ULONG ul = 0; ul < arity; ul++)
	{
		CExpression *pexprChild =
			PexprSimplifyQuantifiedSubqueries(mp, (*pexpr)[ul]);
		pdrspqexprChildren->Append(pexprChild);
	}

	pop->AddRef();
	return SPQOS_NEW(mp) CExpression(mp, pop, pdrspqexprChildren);
}

// preliminary unnesting of scalar subqueries
// Example:
// 		Input:   SELECT k, (SELECT (SELECT Y.i FROM Y WHERE Y.j=X.j)) from X
//		Output:  SELECT k, (SELECT Y.i FROM Y WHERE Y.j=X.j) from X
CExpression *
CExpressionPreprocessor::PexprUnnestScalarSubqueries(CMemoryPool *mp,
													 CExpression *pexpr)
{
	// protect against stack overflow during recursion
	SPQOS_CHECK_STACK_SIZE;
	SPQOS_ASSERT(NULL != mp);
	SPQOS_ASSERT(NULL != pexpr);

	COperator *pop = pexpr->Pop();
	// look for a Project Element with a scalar subquery below it
	if (CUtils::FProjElemWithScalarSubq(pexpr))
	{
		// recursively process scalar subquery
		CExpression *pexprSubq = PexprUnnestScalarSubqueries(mp, (*pexpr)[0]);

		// if the scalar subquery is replaced by the CScalarIdent in the previous
		// recursive call we simply return the CScalarIdent and stop preprocessing
		// at this stage.
		// +--CScalarProjectList
		//	  +--CScalarProjectElement "?column?" (2)
		//	     +--CScalarIdent "column1" (1)
		if (COperator::EopScalarIdent == pexprSubq->Pop()->Eopid())
		{
			pop->AddRef();
			return SPQOS_NEW(mp) CExpression(mp, pop, pexprSubq);
		}

		// check if subquery is defined as a Project on Const Table
		CExpression *pexprSubqChild = (*pexprSubq)[0];
		if (CUtils::FProjectConstTableWithOneScalarSubq(pexprSubqChild))
		{
			CExpression *pexprConstTable = (*pexprSubqChild)[0];
			CExpression *pexprPrjList = (*pexprSubqChild)[1];
			SPQOS_ASSERT(1 == pexprPrjList->Arity());

			CExpression *pexprPrjElem = (*pexprPrjList)[0];
			CExpression *pexprInnerSubq = (*pexprPrjElem)[0];
			SPQOS_ASSERT(COperator::EopScalarSubquery ==
						pexprInnerSubq->Pop()->Eopid());

			// make sure that inner subquery has no outer references to Const Table
			// since Const Table will be eliminated in output expression
			CColRefSet *pcrsConstTableOutput =
				pexprConstTable->DeriveOutputColumns();
			CColRefSet *outer_refs =
				(*pexprInnerSubq)[0]->DeriveOuterReferences();
			if (0 == outer_refs->Size() ||
				outer_refs->IsDisjoint(pcrsConstTableOutput))
			{
				// recursively process inner subquery
				CExpression *pexprUnnestedSubq =
					PexprUnnestScalarSubqueries(mp, pexprInnerSubq);

				// the original subquery is processed and can be removed now
				pexprSubq->Release();

				// build the new Project Element after eliminating outer subquery
				pop->AddRef();
				return SPQOS_NEW(mp) CExpression(mp, pop, pexprUnnestedSubq);
			}
		}

		// otherwise, return a Project Element with the processed outer subquery
		pop->AddRef();
		return SPQOS_NEW(mp) CExpression(mp, pop, pexprSubq);
	}

	else if (CUtils::FScalarSubqWithConstTblGet(pexpr))
	{
		const CColRef *pcrSubq =
			CScalarSubquery::PopConvert(pexpr->Pop())->Pcr();
		CColRefSet *pcrsConstTableOutput = (*pexpr)[0]->DeriveOutputColumns();

		// if the subquery has outer ref, we do not make use of the output columns of constant table get.
		// In this scenairo, we replace the entire scalar subquery with a CScalarIdent with the outer reference.
		// Otherwise, the subquery remains unchanged.
		// Input:
		//   +--CScalarSubquery["b" (8)]
		//      +--CLogicalConstTableGet Columns: ["" (16)] Values: [(1)]
		// Output:
		//   +--CScalarIdent "b" (8)
		if (!pcrsConstTableOutput->FMember(pcrSubq))
		{
			CScalarSubquery *pScalarSubquery =
				CScalarSubquery::PopConvert(pexpr->Pop());
			return CUtils::PexprScalarIdent(mp, pScalarSubquery->Pcr());
		}
	}

	// recursively process children
	const ULONG arity = pexpr->Arity();
	CExpressionArray *pdrspqexprChildren = SPQOS_NEW(mp) CExpressionArray(mp);
	for (ULONG ul = 0; ul < arity; ul++)
	{
		CExpression *pexprChild = PexprUnnestScalarSubqueries(mp, (*pexpr)[ul]);
		pdrspqexprChildren->Append(pexprChild);
	}

	pop->AddRef();
	return SPQOS_NEW(mp) CExpression(mp, pop, pdrspqexprChildren);
}

// an intermediate limit is removed if it has neither row count nor offset
CExpression *
CExpressionPreprocessor::PexprRemoveSuperfluousLimit(CMemoryPool *mp,
													 CExpression *pexpr)
{
	// protect against stack overflow during recursion
	SPQOS_CHECK_STACK_SIZE;
	SPQOS_ASSERT(NULL != mp);
	SPQOS_ASSERT(NULL != pexpr);

	COperator *pop = pexpr->Pop();
	// if current operator is a logical limit with zero offset, and no specified
	// row count, skip to limit's logical child
	if (COperator::EopLogicalLimit == pop->Eopid() &&
		CUtils::FHasZeroOffset(pexpr) &&
		!CLogicalLimit::PopConvert(pop)->FHasCount())
	{
		CLogicalLimit *popLgLimit = CLogicalLimit::PopConvert(pop);
        if (SPQOS_FTRACE(EopttraceRemoveSuperfluousOrder) &&
            ((!popLgLimit->IsTopLimitUnderDMLorCTAS() &&
              SPQOS_FTRACE(EopttraceRemoveOrderBelowDML)) ||
             popLgLimit->IsTopLimitUnderDMLorCTAS()))
        {
            return PexprRemoveSuperfluousLimit(mp, (*pexpr)[0]);
        }
	}

	// recursively process children
	const ULONG arity = pexpr->Arity();
	CExpressionArray *pdrspqexprChildren = SPQOS_NEW(mp) CExpressionArray(mp);

	for (ULONG ul = 0; ul < arity; ul++)
	{
		CExpression *pexprChild = PexprRemoveSuperfluousLimit(mp, (*pexpr)[ul]);

		pdrspqexprChildren->Append(pexprChild);
	}

	pop->AddRef();
	return SPQOS_NEW(mp) CExpression(mp, pop, pdrspqexprChildren);
}

// distinct is removed from a DQA if it has a max or min agg
// e.g. select max(distinct(a)) from tbl -> select max(a) from tbl
CExpression *
CExpressionPreprocessor::PexprRemoveSuperfluousDistinctInDQA(CMemoryPool *mp,
															 CExpression *pexpr)
{
	// protect against stack overflow during recursion
	SPQOS_CHECK_STACK_SIZE;
	SPQOS_ASSERT(NULL != mp);
	SPQOS_ASSERT(NULL != pexpr);

	COperator *pop = pexpr->Pop();
	if (COperator::EopLogicalGbAgg == pop->Eopid())
	{
		const CExpression *const pexprProjectList = (*pexpr)[1];
		SPQOS_ASSERT(COperator::EopScalarProjectList ==
					pexprProjectList->Pop()->Eopid());
		const ULONG arity = pexprProjectList->Arity();

		CMDAccessor *md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();

		for (ULONG ul = 0; ul < arity; ul++)
		{
			CExpression *const pexprPrjElem = (*pexprProjectList)[ul];
			if (COperator::EopScalarAggFunc ==
				(*pexprPrjElem)[0]->Pop()->Eopid())
			{
				CScalarAggFunc *popAggFunc =
					CScalarAggFunc::PopConvert((*pexprPrjElem)[0]->Pop());
				IMDId *agg_child_mdid =
					CScalar::PopConvert((*pexprPrjElem)[0]->Pop())->MdidType();
				const IMDType *agg_child_type =
					md_accessor->RetrieveType(agg_child_mdid);

				if (popAggFunc->IsDistinct() &&
					popAggFunc->IsMinMax(agg_child_type))
				{
					popAggFunc->SetIsDistinct(false);
				}
			}
		}
	}


	// recursively process children
	const ULONG arity = pexpr->Arity();
	CExpressionArray *pdrspqexprChildren = SPQOS_NEW(mp) CExpressionArray(mp);

	for (ULONG ul = 0; ul < arity; ul++)
	{
		CExpression *pexprChild =
			PexprRemoveSuperfluousDistinctInDQA(mp, (*pexpr)[ul]);

		pdrspqexprChildren->Append(pexprChild);
	}

	pop->AddRef();
	return SPQOS_NEW(mp) CExpression(mp, pop, pdrspqexprChildren);
}

//	Remove outer references from order spec inside limit, grouping columns
//	in GbAgg, and Partition/Order columns in window operators. Also handle
//	cases where we would end up with an empty groupby list and project list,
//	which is not supported.
//
//	Example, for the schema: t(a, b), s(i, j)
//	The query:
//			select * from t where a < all (select i from s order by j, b limit 1);
//		should be equivalent to:
//			select * from t where a < all (select i from s order by j limit 1);
//		after removing the outer reference (b) from the order by clause of the
//		subquery (all tuples in the subquery have the same value for the outer ref)
//
//		Similarly,
//			select * from t where a in (select count(i) from s group by j, b);
//		is equivalent to:
//			select * from t where a in (select count(i) from s group by j);
//
//		Similarly,
//			select * from t where a in (select row_number() over (partition by t.a order by t.b) from s);
//		is equivalent to:
//			select * from t where a in (select row_number() over () from s);
CExpression *
CExpressionPreprocessor::PexprRemoveSuperfluousOuterRefs(CMemoryPool *mp,
														 CExpression *pexpr)
{
	// protect against stack overflow during recursion
	SPQOS_CHECK_STACK_SIZE;
	SPQOS_ASSERT(NULL != mp);
	SPQOS_ASSERT(NULL != pexpr);

	// operator, possibly altered below if we need to change the operator
	COperator *pop = pexpr->Pop();
	// expression, possibly altered below if we need to change the children
	CExpression *newExpr = pexpr;

	COperator::EOperatorId op_id = pop->Eopid();
	BOOL fHasOuterRefs = (pop->FLogical() && CUtils::HasOuterRefs(pexpr));

	pop->AddRef();
	if (fHasOuterRefs)
	{
		// special handling for three operator types: Limit, GrbyAgg, Sequence
		if (COperator::EopLogicalLimit == op_id)
		{
			CColRefSet *outer_refs = pexpr->DeriveOuterReferences();

			CLogicalLimit *popLimit = CLogicalLimit::PopConvert(pop);
			COrderSpec *pos = popLimit->Pos();
			COrderSpec *posNew = pos->PosExcludeColumns(mp, outer_refs);

			pop->Release();
			pop = SPQOS_NEW(mp) CLogicalLimit(
				mp, posNew, popLimit->FGlobal(), popLimit->FHasCount(),
				popLimit->IsTopLimitUnderDMLorCTAS());
		}
		else if (COperator::EopLogicalGbAgg == op_id)
		{
			CColRefSet *outer_refs = pexpr->DeriveOuterReferences();

			CLogicalGbAgg *popAgg = CLogicalGbAgg::PopConvert(pop);
			CColRefArray *colref_array = CUtils::PdrspqcrExcludeColumns(
				mp, popAgg->Pdrspqcr(), outer_refs);

			CExpression *pExprProjList = (*pexpr)[1];

			// It's only valid to remove the outer reference if:
			// the projection list is NOT empty
			// or
			// the outer references are NOT the ONLY Group By column
			//
			// For example:
			// -- Cannot remove t.b from groupby, because this will produce an invalid plan
			// -- with both groupby list and project list empty, in this case we need to add
			// -- a project node below the GrbyAgg
			// select a from t where c in (select distinct t.b from s)
			//
			// -- remove t.b from groupby is ok, because there is at least one agg function: count()
			// select a from t where c in (select count(s.j) from s group by t.b)
			//
			// -- remove t.b from groupby is ok, because there is other groupby column s.j
			// select a from t where c in (select s.j from s group by t.b, s.j)
			//
			// -- remove t.b from groupby is ok, because outer reference is a
			// -- constant for each invocation of subquery
			// select a from t where c in (select count(s.j) from s group by s.i, t.b)
			//

			if (0 < pExprProjList->Arity() || 0 < colref_array->Size())
			{
				// remove outer refs from the groupby columns list
				CColRefArray *pdrspqcrMinimal = popAgg->PdrspqcrMinimal();
				if (NULL != pdrspqcrMinimal)
				{
					pdrspqcrMinimal = CUtils::PdrspqcrExcludeColumns(
						mp, pdrspqcrMinimal, outer_refs);
				}

				CColRefArray *pdrspqcrArgDQA = popAgg->PdrspqcrArgDQA();
				if (NULL != pdrspqcrArgDQA)
				{
					pdrspqcrArgDQA->AddRef();
				}

				pop->Release();
				pop = SPQOS_NEW(mp) CLogicalGbAgg(
					mp, colref_array, pdrspqcrMinimal, popAgg->Egbaggtype(),
					popAgg->FGeneratesDuplicates(), pdrspqcrArgDQA);
			}
			else
			{
				// grouping_cols has outer references that can't be removed, because
				// that would make both pExprProjList and grouping_cols empty, which is not allowed.
				// The solution in this case is to add a project node below that will simply echo
				// the outer reference, and to use that newly produced ColRef as groupby column.
				CExpression *child = (*pexpr)[0];
				CExpressionArray *grouping_cols_arr =
					CUtils::PdrspqexprScalarIdents(mp, popAgg->Pdrspqcr());

				SPQOS_ASSERT(0 < grouping_cols_arr->Size());
				child->AddRef();

				// add a project node on top of our child
				CExpression *projectExpr = CUtils::PexprAddProjection(
					mp, child, grouping_cols_arr,
					false  // don't add to hash table,
						   // this is done at the end
						   // of preprocessing
				);
				grouping_cols_arr->Release();

				// build a children array for the new GrbyAgg expression
				CExpressionArray *new_children =
					SPQOS_NEW(mp) CExpressionArray(mp);
				new_children->Append(projectExpr);
				for (ULONG ul = 1; ul < pexpr->PdrgPexpr()->Size(); ul++)
				{
					new_children->Append((*pexpr->PdrgPexpr())[ul]);
					(*pexpr->PdrgPexpr())[ul]->AddRef();
				}

				// build a new CLogicalGbAgg operator, with a new grouping columns list
				CColRefArray *new_grouping_cols = SPQOS_NEW(mp) CColRefArray(mp);
				CExpression *new_projected_cols = (*projectExpr)[1];
				for (ULONG ul = 0; ul < new_projected_cols->Arity(); ul++)
				{
					new_grouping_cols->Append(
						CUtils::PcrFromProjElem((*new_projected_cols)[ul]));
				}
				SPQOS_ASSERT(NULL == popAgg->PdrspqcrArgDQA());
				pop = SPQOS_NEW(mp) CLogicalGbAgg(mp, new_grouping_cols, NULL,
												 popAgg->Egbaggtype(),
												 popAgg->FGeneratesDuplicates(),
												 NULL  // no DQA cols
				);
				// release the previous pop
				popAgg->Release();
				popAgg = NULL;

				// finally, put it all together, our new GrbyAgg now has a project node below
				// it that will turn the outer reference into a produced ColRef that is used
				// as a groupby column
				pop->AddRef();
				newExpr = SPQOS_NEW(mp) CExpression(mp, pop, new_children);
				// clean up
				colref_array->Release();
			}
		}
		else if (COperator::EopLogicalSequenceProject == op_id)
		{
			CExpressionHandle exprhdl(mp);
			exprhdl.Attach(pexpr);
			exprhdl.DeriveProps(NULL /*pdpctxt*/);
			CLogicalSequenceProject *popSequenceProject =
				CLogicalSequenceProject::PopConvert(pop);
			if (popSequenceProject->FHasLocalReferencesTo(
					exprhdl.DeriveOuterReferences()))
			{
				COperator *popNew =
					popSequenceProject->PopRemoveLocalOuterRefs(mp, exprhdl);
				pop->Release();
				pop = popNew;
			}
		}
	}

	// recursively process children
	const ULONG arity = newExpr->Arity();
	CExpressionArray *pdrspqexprChildren = SPQOS_NEW(mp) CExpressionArray(mp);
	for (ULONG ul = 0; ul < arity; ul++)
	{
		CExpression *pexprChild =
			PexprRemoveSuperfluousOuterRefs(mp, (*newExpr)[ul]);
		pdrspqexprChildren->Append(pexprChild);
	}
	// Adjust colref for ScalarSubqueryAll with CLogicalGbAgg having outer refs
	if (COperator::EopScalarSubqueryAll == (pop)->Eopid())
	{
		CExpression *pexprChild = (*pdrspqexprChildren)[0];
		if (COperator::EopLogicalGbAgg == (pexprChild->Pop())->Eopid() &&
			CUtils::HasOuterRefs(pexprChild))
		{
			// Since grouping_cols with outer references now has an added
			// project node below with newly produced ColRef, these newly
			// produced ColRef need to be referenced in CScalarSubqueryAll
			// The expression tree looks like below for an example query,
			// select t.b from t where t.b not in (select distinct t.b where t.c>1 ):
			// +--CScalarSubqueryAll(<>) <prev ColRef> -> <Update to adjusted ColRef>
			// |--CLogicalGbAgg
			// |  |--CLogicalProject
			// |  |  |--...
			// |  |  +--CScalarProjectList
			// |  |     +--CScalarProjectElement <new ColRef>
			// |  |        +--...
			// |  +--...
			// +--...
			CExpression *projectExpr = (*pexprChild)[0];
			if (COperator::EopLogicalProject == (projectExpr->Pop())->Eopid())
			{
				CExpression *projectListExpr = (*projectExpr)[1];
				CExpression *projectElemExpr = (*projectListExpr)[0];
				CColRef *pcrOuter = CUtils::PcrFromProjElem(projectElemExpr);
				CScalarSubqueryAll *prevPop =
					CScalarSubqueryAll::PopConvert(pop);
				IMDId *mdid = prevPop->MdIdOp();
				const CWStringConst *str = prevPop->PstrOp();
				mdid->AddRef();
				pop = SPQOS_NEW(mp) CScalarSubqueryAll(
					mp, mdid, SPQOS_NEW(mp) CWStringConst(mp, str->GetBuffer()),
					pcrOuter);
				prevPop->Release();
			}
		}
	}

	if (newExpr != pexpr)
	{
		newExpr->Release();
	}
	return SPQOS_NEW(mp) CExpression(mp, pop, pdrspqexprChildren);
}

// generate a ScalarBoolOp expression or simply return the only expression
// in the array if there is only one.
CExpression *
CExpressionPreprocessor::PexprScalarBoolOpConvert2In(
	CMemoryPool *mp, CScalarBoolOp::EBoolOperator eboolop,
	CExpressionArray *pdrspqexpr)
{
	SPQOS_ASSERT(NULL != pdrspqexpr);
	SPQOS_ASSERT(0 < pdrspqexpr->Size());

	if (1 == pdrspqexpr->Size())
	{
		// if there is one child, do not wrap it in a bool op
		CExpression *pexpr = (*pdrspqexpr)[0];
		pexpr->AddRef();
		pdrspqexpr->Release();
		return pexpr;
	}

	return SPQOS_NEW(mp)
		CExpression(mp, SPQOS_NEW(mp) CScalarBoolOp(mp, eboolop), pdrspqexpr);
}

// checks if the given expression is likely to be simplified by the constraints
// framework during array conversion. eboolop is the CScalarBoolOp type
// of the expression which contains the argument expression
BOOL
CExpressionPreprocessor::FConvert2InIsConvertable(
	CExpression *pexpr, CScalarBoolOp::EBoolOperator eboolopParent)
{
	bool fConvertableExpression = false;
	if (CPredicateUtils::FCompareIdentToConst(pexpr))
	{
		fConvertableExpression |=
			IMDType::EcmptEq ==
				CScalarCmp::PopConvert(pexpr->Pop())->ParseCmpType() &&
			CScalarBoolOp::EboolopOr == eboolopParent;
		fConvertableExpression |=
			IMDType::EcmptNEq ==
				CScalarCmp::PopConvert(pexpr->Pop())->ParseCmpType() &&
			CScalarBoolOp::EboolopAnd == eboolopParent;
	}
	else if (CPredicateUtils::FCompareIdentToConstArray(pexpr) ||
			 CPredicateUtils::FCompareCastIdentToConstArray(pexpr))
	{
		fConvertableExpression = true;
	}

	if (fConvertableExpression)
	{
		SPQOS_ASSERT(0 < pexpr->Arity());
		CScalarIdent *pscid = NULL;
		if (CUtils::FScalarIdent((*pexpr)[0]))
		{
			pscid = CScalarIdent::PopConvert((*pexpr)[0]->Pop());
		}
		else
		{
			SPQOS_ASSERT(CScalarIdent::FCastedScId((*pexpr)[0]));
			pscid = CScalarIdent::PopConvert((*(*pexpr)[0])[0]->Pop());
		}
		if (!CUtils::FConstrainableType(pscid->MdidType()))
		{
			fConvertableExpression = false;
		}
	}

	return fConvertableExpression;
}

// converts series of AND or OR comparisons into array IN expressions. For
// example, x = 1 OR x = 2 will convert to x IN (1,2). This stage assumes
// the expression has been unnested using CExpressionUtils::PexprUnnest.
CExpression *
CExpressionPreprocessor::PexprConvert2In(
	CMemoryPool *mp,
	CExpression *pexpr	// does not take ownership
)
{
	// protect against stack overflow during recursion
	SPQOS_CHECK_STACK_SIZE;
	SPQOS_ASSERT(NULL != mp);
	SPQOS_ASSERT(NULL != pexpr);

	COperator *pop = pexpr->Pop();
	if (CPredicateUtils::FOr(pexpr) || CPredicateUtils::FAnd(pexpr))
	{
		// the bool op type of this node
		CScalarBoolOp::EBoolOperator eboolop =
			CScalarBoolOp::PopConvert(pop)->Eboolop();
		// derive constraints on all of the simple scalar children
		// and add them to a new AND or OR expression
		CExpressionArray *pdrspqexprCollapse = SPQOS_NEW(mp) CExpressionArray(mp);
		CExpressionArray *pdrspqexprRemainder =
			SPQOS_NEW(mp) CExpressionArray(mp);
		const ULONG arity = pexpr->Arity();
		for (ULONG ul = 0; ul < arity; ul++)
		{
			CExpression *pexprChild = (*pexpr)[ul];

			if (FConvert2InIsConvertable(pexprChild, eboolop))
			{
				pexprChild->AddRef();
				pdrspqexprCollapse->Append(pexprChild);
			}
			else
			{
				// recursively convert the remainder and add to the array
				pdrspqexprRemainder->Append(PexprConvert2In(mp, pexprChild));
			}
		}

		if (0 != pdrspqexprCollapse->Size())
		{
			// create the constraint, rederive the collapsed expression
			// add the new derived expr to remainder
			CColRefSetArray *colref_array = NULL;
			pop->AddRef();
			CAutoRef<CExpression> apexprPreCollapse(
				SPQOS_NEW(mp) CExpression(mp, pop, pdrspqexprCollapse));
			CAutoRef<CConstraint> apcnst(CConstraint::PcnstrFromScalarExpr(
				mp, apexprPreCollapse.Value(), &colref_array));

			SPQOS_ASSERT(NULL != apcnst.Value());
			CExpression *pexprPostCollapse = apcnst->PexprScalar(mp);

			pexprPostCollapse->AddRef();
			pdrspqexprRemainder->Append(pexprPostCollapse);
			CRefCount::SafeRelease(colref_array);
		}
		else
		{
			pdrspqexprCollapse->Release();
		}

		SPQOS_ASSERT(0 < pdrspqexprRemainder->Size());
		return PexprScalarBoolOpConvert2In(mp, eboolop, pdrspqexprRemainder);
	}

	CExpressionArray *pdrspqexpr = SPQOS_NEW(mp) CExpressionArray(mp);
	CExpressionArray *pdrgexprChildren = pexpr->PdrgPexpr();
	for (ULONG ul = 0; ul < pexpr->Arity(); ul++)
	{
		pdrspqexpr->Append(PexprConvert2In(mp, (*pdrgexprChildren)[ul]));
	}

	pop->AddRef();
	return SPQOS_NEW(mp) CExpression(mp, pop, pdrspqexpr);
}

// collapse cascaded inner and left outer joins into NAry-joins
CExpression *
CExpressionPreprocessor::PexprCollapseJoins(CMemoryPool *mp, CExpression *pexpr)
{
	// protect against stack overflow during recursion
	SPQOS_CHECK_STACK_SIZE;
	SPQOS_ASSERT(NULL != mp);
	SPQOS_ASSERT(NULL != pexpr);

	COperator *pop = pexpr->Pop();
	const ULONG arity = pexpr->Arity();

	if (CPredicateUtils::FInnerOrNAryJoin(pexpr) ||
		(SPQOS_FTRACE(EopttraceEnableLOJInNAryJoin) &&
		 CPredicateUtils::FLeftOuterJoin(pexpr)))
	{
		CExpressionArray *newChildNodes = SPQOS_NEW(mp) CExpressionArray(mp);
		ULongPtrArray *lojChildPredIndexes = SPQOS_NEW(mp) ULongPtrArray(mp);
		CExpressionArray *innerJoinPredicates =
			SPQOS_NEW(mp) CExpressionArray(mp);
		CExpressionArray *lojPredicates = SPQOS_NEW(mp) CExpressionArray(mp);

		CollectJoinChildrenRecursively(mp, pexpr, newChildNodes,
									   lojChildPredIndexes, innerJoinPredicates,
									   lojPredicates);

		if (lojPredicates->Size() > 0)
		{
			// each logical child must have an associated predicate index
			SPQOS_ASSERT(newChildNodes->Size() == lojChildPredIndexes->Size());

			// this NAry join involves LOJs; create a CScalarNAryJoinPredList to hold
			// the information which predicates are inner join preds and which ON predicates
			// are associated with the LOJs' right children
			CExpressionArray *naryJoinPredicates =
				SPQOS_NEW(mp) CExpressionArray(mp);

			// create a new CScalarNAryJoinPredList as the last child of the NAry join
			// the first child are all the inner join predicates
			naryJoinPredicates->Append(
				CPredicateUtils::PexprConjunction(mp, innerJoinPredicates));
			// the remaining children are the LOJ predicates, one by one
			for (ULONG ul = 0; ul < lojPredicates->Size(); ul++)
			{
				CExpression *predicate = (*lojPredicates)[ul];
				predicate->AddRef();
				naryJoinPredicates->Append(predicate);
			}

			CExpression *nAryJoinPredicateList = SPQOS_NEW(mp)
				CExpression(mp, SPQOS_NEW(mp) CScalarNAryJoinPredList(mp),
							naryJoinPredicates);
			newChildNodes->Append(nAryJoinPredicateList);

			// some sanity checks

			// Example:  t1 join t2 on p12 left outer join t3 on p23 join t4 on p24 left outer join t5 on p35
			// results from this call:
			//    newChildNodes:       [ t1, t2, t3, t4, t5 ]
			//    lojChildPredIndexes: [  0,  0,  1,  0,  2 ] (one entry per logical leaf node)
			//    innerjoinPredicates: [ p12, p24 ]  (all correspond to child pred index 0 (SPQOPT_ZERO_INNER_JOIN_PRED_INDEX))
			//    lojPredicates:       [ p23, p35 ]  (p23 corresponds to child pred index 1, p35 corresponds to child pred index 2)

			// the leftmost child must have a predicate index of
			// SPQOPT_ZERO_INNER_JOIN_PRED_INDEX, since it cannot be the right child of an LOJ
			SPQOS_ASSERT(SPQOPT_ZERO_INNER_JOIN_PRED_INDEX ==
						*(*lojChildPredIndexes)[0]);

#ifdef SPQOS_DEBUG
			// lojChildPredIndexes must contain the numbers 1 ... lojPredicates->Size()
			// in ascending order, each number exactly once, with optional additional
			// SPQOPT_ZERO_INNER_JOIN_PRED_INDEX (0) entries in-between entries
			ULONG highestNumberSeen = 0;

			for (ULONG ix = 1; ix < lojChildPredIndexes->Size(); ix++)
			{
				ULONG nextNumber = *((*lojChildPredIndexes)[ix]);

				if (nextNumber == highestNumberSeen + 1)
				{
					// child is right child of an LOJ
					highestNumberSeen = nextNumber;
				}
				else
				{
					// if we don't see the next number for a child, it must
					// be associated with the collective inner join predicates
					SPQOS_ASSERT(SPQOPT_ZERO_INNER_JOIN_PRED_INDEX == nextNumber);
				}
			}
			SPQOS_ASSERT(highestNumberSeen == lojPredicates->Size());
#endif
		}
		else
		{
			// no LOJs involved, just add the ANDed preds as the scalar child
			newChildNodes->Append(
				CPredicateUtils::PexprConjunction(mp, innerJoinPredicates));
			lojChildPredIndexes->Release();
			lojChildPredIndexes = NULL;
		}

		CExpression *pexprNAryJoin = SPQOS_NEW(mp) CExpression(
			mp, SPQOS_NEW(mp) CLogicalNAryJoin(mp, lojChildPredIndexes),
			newChildNodes);

		COptimizerConfig *optimizer_config =
			COptCtxt::PoctxtFromTLS()->GetOptimizerConfig();
		ULONG ulJoinArityLimit =
			optimizer_config->GetHint()
				->UlJoinArityForAssociativityCommutativity();

		// The last child of an n-ary join expression is the scalar expression
		if (pexprNAryJoin->Arity() - 1 > ulJoinArityLimit)
		{
			SPQOPT_DISABLE_XFORM(CXform::ExfJoinCommutativity);
			SPQOPT_DISABLE_XFORM(CXform::ExfJoinAssociativity);
		}

		lojPredicates->Release();
		return pexprNAryJoin;
	}
	// current operator is not an inner-join or supported LOJ, recursively process children
	CExpressionArray *pdrspqexprChildren = SPQOS_NEW(mp) CExpressionArray(mp);
	for (ULONG ul = 0; ul < arity; ul++)
	{
		CExpression *pexprChild = PexprCollapseJoins(mp, (*pexpr)[ul]);
		pdrspqexprChildren->Append(pexprChild);
	}

	pop->AddRef();
	return SPQOS_NEW(mp) CExpression(mp, pop, pdrspqexprChildren);
}

// collect the children of a join backbone into an array of logical leaf
// nodes (leaves of the backbone, that is) and arrays of predicates, such
// that we can still associate the correct ON predicates to the children
void
CExpressionPreprocessor::CollectJoinChildrenRecursively(
	CMemoryPool *mp, CExpression *pexpr, CExpressionArray *logicalLeafNodes,
	ULongPtrArray *lojChildPredIndexes, CExpressionArray *innerJoinPredicates,
	CExpressionArray *lojPredicates)
{
	// protect against stack overflow during recursion
	SPQOS_CHECK_STACK_SIZE;
	SPQOS_ASSERT(pexpr->Pop()->FLogical());

	if (CPredicateUtils::FInnerOrNAryJoin(pexpr))
	{
		const ULONG arity = pexpr->Arity();
		CExpression *pexprScalar = (*pexpr)[arity - 1];

		if (COperator::EopScalarNAryJoinPredList != pexprScalar->Pop()->Eopid())
		{
			for (ULONG ul = 0; ul < arity - 1; ul++)
			{
				CExpression *child = (*pexpr)[ul];
				CollectJoinChildrenRecursively(
					mp, child, logicalLeafNodes, lojChildPredIndexes,
					innerJoinPredicates, lojPredicates);
			}

			innerJoinPredicates->Append(PexprCollapseJoins(mp, pexprScalar));
		}
		else
		{
			// we have collapsed this join before and it already has some non-inner join info,
			// merge the existing and new lists
			CLogicalNAryJoin *naryJoin =
				CLogicalNAryJoin::PopConvert(pexpr->Pop());
			ULongPtrArray *naryJoinPredIndexes =
				naryJoin->GetLojChildPredIndexes();

			// add all the inner join predicates
			innerJoinPredicates->Append(
				PexprCollapseJoins(mp, (*pexprScalar)[0]));

			// loop over the logical children
			for (ULONG ul = 0; ul < arity - 1; ul++)
			{
				if (SPQOPT_ZERO_INNER_JOIN_PRED_INDEX ==
					*(*naryJoinPredIndexes)[ul])
				{
					// inner join child, collapse recursively
					CollectJoinChildrenRecursively(
						mp, (*pexpr)[ul], logicalLeafNodes, lojChildPredIndexes,
						innerJoinPredicates, lojPredicates);
				}
				else
				{
					// this is the right child of a non-inner join
					ULONG oldPredIndex = *(*naryJoinPredIndexes)[ul];
					CExpression *lojPred =
						PexprCollapseJoins(mp, (*pexprScalar)[oldPredIndex]);

					// don't collapse this child into our current join node
					logicalLeafNodes->Append(
						PexprCollapseJoins(mp, (*pexpr)[ul]));
					lojPredicates->Append(lojPred);

					ULONG newPredIndex = lojPredicates->Size();

					lojChildPredIndexes->Append(SPQOS_NEW(mp)
													ULONG(newPredIndex));
				}
			}
		}
	}
	else if (SPQOS_FTRACE(EopttraceEnableLOJInNAryJoin) &&
			 CPredicateUtils::FLeftOuterJoin(pexpr))
	{
		SPQOS_ASSERT(3 == pexpr->Arity());

		CExpression *leftChild = (*pexpr)[0];
		CExpression *rightChild = (*pexpr)[1];
		CExpression *pexprScalar = (*pexpr)[2];

		CollectJoinChildrenRecursively(mp, leftChild, logicalLeafNodes,
									   lojChildPredIndexes, innerJoinPredicates,
									   lojPredicates);

		// stop collecting join children at the right child of the LOJ,
		// just add the child, regardless of whether it is a join or not
		logicalLeafNodes->Append(PexprCollapseJoins(mp, rightChild));

		// create an entry in lojPredicates...
		lojPredicates->Append(PexprCollapseJoins(mp, pexprScalar));

		// ... and point to this new entry in lojChildPredIndexes
		ULONG *indexOfThisLOJInTheArray =
			SPQOS_NEW(mp) ULONG(lojPredicates->Size());
		lojChildPredIndexes->Append(indexOfThisLOJInTheArray);
	}
	else
	{
		// pexpr is not the right child of a supported LOJ and is not a supported join
		logicalLeafNodes->Append(PexprCollapseJoins(mp, pexpr));

		// this logical "leaf" node is a child of an inner join or it is the left child
		// of an LOJ, either way it is associated with the inner join predicates
		ULONG *innerJoinPredIndex =
			SPQOS_NEW(mp) ULONG(SPQOPT_ZERO_INNER_JOIN_PRED_INDEX);
		lojChildPredIndexes->Append(innerJoinPredIndex);
	}
}


// collapse cascaded logical project operators
CExpression *
CExpressionPreprocessor::PexprCollapseProjects(CMemoryPool *mp,
											   CExpression *pexpr)
{
	// protect against stack overflow during recursion
	SPQOS_CHECK_STACK_SIZE;
	SPQOS_ASSERT(NULL != mp);
	SPQOS_ASSERT(NULL != pexpr);

	CExpressionArray *pdrspqexpr = SPQOS_NEW(mp) CExpressionArray(mp);

	const ULONG arity = pexpr->Arity();
	// recursively process children
	for (ULONG ul = 0; ul < arity; ul++)
	{
		CExpression *pexprChild = PexprCollapseProjects(mp, (*pexpr)[ul]);
		pdrspqexpr->Append(pexprChild);
	}

	COperator *pop = pexpr->Pop();
	pop->AddRef();

	CExpression *pexprNew = SPQOS_NEW(mp) CExpression(mp, pop, pdrspqexpr);
	CExpression *pexprCollapsed = CUtils::PexprCollapseProjects(mp, pexprNew);

	if (NULL == pexprCollapsed)
	{
		return pexprNew;
	}

	pexprNew->Release();

	return pexprCollapsed;
}

// insert dummy project element below scalar subquery when the (a) the scalar
// subquery is below a project and (b) output column is an outer reference
CExpression *
CExpressionPreprocessor::PexprProjBelowSubquery(CMemoryPool *mp,
												CExpression *pexpr,
												BOOL fUnderPrList)
{
	// protect against stack overflow during recursion
	SPQOS_CHECK_STACK_SIZE;
	SPQOS_ASSERT(NULL != mp);
	SPQOS_ASSERT(NULL != pexpr);

	/*
	 * Consider the following subquery:
	 * SELECT (SELECT foo.b from bar) FROM foo
	 * If bar is empty we should return null.
	 *
	 * For this query during DXL->Expr translation, the project element
	 * (SELECT b FROM bar) is represented as scalar subquery that returns
	 * an output column. To ensure that this scalar subquery under the project
	 * operator is returned when bar (or an arbitrary tree instead of bar)
	 * we insert a dummy project element that points to FOO.b under the
	 * scalar subquery. This dummy project element prevents its incorrect
	 * transformation into a non-correlated plan.
	 *
	 * One of the reasons we add this dummy project is to force the subquery
	 * handler transformation to not produce a de-correlated plan
	 * for queries such as this.
	 *
	 * We want to limit the of such introduction dummy projects only when the
	 * following conditions are all satisfied:
	 * a)  The scalar subquery is in the project element scalar tree
	 * Another use case: SELECT (SELECT foo.b from bar) + 1 FROM foo
	 * b) The output of the scalar subquery is the column from the outer expression.
	 * Consider the query: SELECT (SELECT foo.b + 5 from bar) FROM foo. In such cases,
	 * since the foo.b + 5 is a new computed column inside the subquery with its own
	 * project element, we do not need to add anything.
	 */
	BOOL fUnderPrListChild = fUnderPrList;
	COperator *pop = pexpr->Pop();

	if (pop->FLogical())
	{
		if (COperator::EopLogicalProject == pop->Eopid())
		{
			CExpression *pexprRel = (*pexpr)[0];
			CExpression *pexprRelNew =
				PexprProjBelowSubquery(mp, pexprRel, false /* fUnderPrList */);

			CExpression *pexprPrList = (*pexpr)[1];
			CExpression *pexprPrListNew = PexprProjBelowSubquery(
				mp, pexprPrList, true /* fUnderPrList */);

			return SPQOS_NEW(mp)
				CExpression(mp, SPQOS_NEW(mp) CLogicalProject(mp), pexprRelNew,
							pexprPrListNew);
		}

		fUnderPrListChild = false;
	}
	else if (COperator::EopScalarSubquery == pop->Eopid() && fUnderPrList)
	{
		CExpression *pexprRel = (*pexpr)[0];
		CExpression *pexprRelNew =
			PexprProjBelowSubquery(mp, pexprRel, false /* fUnderPrList */);

		const CColRefSet *prcsOutput = pexprRelNew->DeriveOutputColumns();
		const CColRef *pcrSubquery = CScalarSubquery::PopConvert(pop)->Pcr();
		if (NULL != prcsOutput && !prcsOutput->FMember(pcrSubquery))
		{
			CColumnFactory *col_factory = COptCtxt::PoctxtFromTLS()->Pcf();
			CColRef *pcrNewSubquery = col_factory->PcrCreate(
				pcrSubquery->RetrieveType(), pcrSubquery->TypeModifier());

			CExpression *pexprPrEl = CUtils::PexprScalarProjectElement(
				mp, pcrNewSubquery, CUtils::PexprScalarIdent(mp, pcrSubquery));
			CExpression *pexprProjList = SPQOS_NEW(mp)
				CExpression(mp, SPQOS_NEW(mp) CScalarProjectList(mp), pexprPrEl);
			CExpression *pexprProj =
				SPQOS_NEW(mp) CExpression(mp, SPQOS_NEW(mp) CLogicalProject(mp),
										 pexprRelNew, pexprProjList);

			CScalarSubquery *popSubq = SPQOS_NEW(mp)
				CScalarSubquery(mp, pcrNewSubquery, false /*fGeneratedByExist*/,
								false /*fGeneratedByQuantified*/);

			CExpression *pexprResult =
				SPQOS_NEW(mp) CExpression(mp, popSubq, pexprProj);
			return pexprResult;
		}

		pop->AddRef();
		return SPQOS_NEW(mp) CExpression(mp, pop, pexprRelNew);
	}

	CExpressionArray *pdrspqexpr = SPQOS_NEW(mp) CExpressionArray(mp);

	const ULONG arity = pexpr->Arity();
	for (ULONG ul = 0; ul < arity; ul++)
	{
		CExpression *pexprChild =
			PexprProjBelowSubquery(mp, (*pexpr)[ul], fUnderPrListChild);
		pdrspqexpr->Append(pexprChild);
	}

	pop->AddRef();
	return SPQOS_NEW(mp) CExpression(mp, pop, pdrspqexpr);
}

// collapse cascaded union/union all into an NAry union/union all operator
CExpression *
CExpressionPreprocessor::PexprCollapseUnionUnionAll(CMemoryPool *mp,
													CExpression *pexpr)
{
	// protect against stack overflow during recursion
	SPQOS_CHECK_STACK_SIZE;
	SPQOS_ASSERT(NULL != mp);
	SPQOS_ASSERT(NULL != pexpr);

	COperator *pop = pexpr->Pop();
	const ULONG arity = pexpr->Arity();

	CExpressionArray *pdrspqexpr = SPQOS_NEW(mp) CExpressionArray(mp);

	// recursively process children
	for (ULONG ul = 0; ul < arity; ul++)
	{
		CExpression *pexprChild = PexprCollapseUnionUnionAll(mp, (*pexpr)[ul]);
		pdrspqexpr->Append(pexprChild);
	}

	pop->AddRef();
	CExpression *pexprNew = SPQOS_NEW(mp) CExpression(mp, pop, pdrspqexpr);
	if (!CPredicateUtils::FUnionOrUnionAll(pexprNew))
	{
		return pexprNew;
	}

	// array of input children and its column references
	CExpressionArray *pdrspqexprNew = SPQOS_NEW(mp) CExpressionArray(mp);
	CColRef2dArray *pdrgdrspqcrOrig =
		CLogicalSetOp::PopConvert(pop)->PdrspqdrspqcrInput();
	CColRef2dArray *pdrgdrspqcrNew = SPQOS_NEW(mp) CColRef2dArray(mp);

	BOOL fCollapsed = false;
	for (ULONG ul = 0; ul < arity; ul++)
	{
		if (CPredicateUtils::FCollapsibleChildUnionUnionAll(pexprNew, ul))
		{
			fCollapsed = true;
			CPredicateUtils::CollectGrandChildrenUnionUnionAll(
				mp, pexprNew, ul, pdrspqexprNew, pdrgdrspqcrNew);
		}
		else
		{
			CExpression *pexprChild = (*pexprNew)[ul];
			pexprChild->AddRef();
			pdrspqexprNew->Append(pexprChild);

			CColRefArray *pdrspqcrInput = (*pdrgdrspqcrOrig)[ul];
			pdrspqcrInput->AddRef();
			pdrgdrspqcrNew->Append(pdrspqcrInput);
		}
	}

	if (!fCollapsed)
	{
		// clean up
		pdrgdrspqcrNew->Release();
		pdrspqexprNew->Release();

		return pexprNew;
	}

	COperator *popNew = NULL;
	CColRefArray *pdrspqcrOutput =
		CLogicalSetOp::PopConvert(pop)->PdrspqcrOutput();
	pdrspqcrOutput->AddRef();
	if (pop->Eopid() == COperator::EopLogicalUnion)
	{
		popNew = SPQOS_NEW(mp) CLogicalUnion(mp, pdrspqcrOutput, pdrgdrspqcrNew);
	}
	else
	{
		SPQOS_ASSERT(pop->Eopid() == COperator::EopLogicalUnionAll);
		popNew =
			SPQOS_NEW(mp) CLogicalUnionAll(mp, pdrspqcrOutput, pdrgdrspqcrNew);
	}

	// clean up
	pexprNew->Release();

	return SPQOS_NEW(mp) CExpression(mp, popNew, pdrspqexprNew);
}

// transform outer joins into inner joins
CExpression *
CExpressionPreprocessor::PexprOuterJoinToInnerJoin(CMemoryPool *mp,
												   CExpression *pexpr)
{
	// protect against stack overflow during recursion
	SPQOS_CHECK_STACK_SIZE;
	SPQOS_ASSERT(NULL != mp);
	SPQOS_ASSERT(NULL != pexpr);

	COperator *pop = pexpr->Pop();
	const ULONG arity = pexpr->Arity();

	if (COperator::EopLogicalSelect == pop->Eopid() &&
		COperator::EopLogicalLeftOuterJoin == (*pexpr)[0]->Pop()->Eopid())
	{
		// a Select on top of LOJ can be turned into InnerJoin by normalization
		return CNormalizer::PexprNormalize(mp, pexpr);
	}

	if (CPredicateUtils::FInnerOrNAryJoin(pexpr))
	{
		// the predicates of an inner join on top of outer join can be used to turn the child outer join into another inner join
		CExpression *pexprScalar = (*pexpr)[arity - 1];

		if (COperator::EopScalarNAryJoinPredList == pexprScalar->Pop()->Eopid())
		{
			// since we have ScalarNAryJoinPredList, it means we have already
			// converted all possible LOJs to Inner Joins and collapsed them
			pexpr->AddRef();
			return pexpr;
		}

		CExpressionArray *pdrspqexprChildren = SPQOS_NEW(mp) CExpressionArray(mp);
		for (ULONG ul = 0; ul < arity; ul++)
		{
			CExpression *pexprChild = (*pexpr)[ul];
			BOOL fNewChild = false;
			if (COperator::EopLogicalLeftOuterJoin ==
				pexprChild->Pop()->Eopid())
			{
				CColRefSet *pcrsLOJInnerOutput =
					(*pexprChild)[1]->DeriveOutputColumns();
				if (!SPQOS_FTRACE(EopttraceDisableOuterJoin2InnerJoinRewrite) &&
					CPredicateUtils::FNullRejecting(mp, pexprScalar,
													pcrsLOJInnerOutput))
				{
					CExpression *pexprNewOuter =
						PexprOuterJoinToInnerJoin(mp, (*pexprChild)[0]);
					CExpression *pexprNewInner =
						PexprOuterJoinToInnerJoin(mp, (*pexprChild)[1]);
					CExpression *pexprNewScalar =
						PexprOuterJoinToInnerJoin(mp, (*pexprChild)[2]);
					pexprChild = CUtils::PexprLogicalJoin<CLogicalInnerJoin>(
						mp, pexprNewOuter, pexprNewInner, pexprNewScalar);
					fNewChild = true;
				}
			}

			// Consider the following join tree:
			// +--CLogicalNAryJoin
			// |--CLogicalLeftOuterJoin
			// |  |--CLogicalLeftOuterJoin
			// |  |  |--CLogicalGet "t1"
			// |  |  |--CLogicalGet "t2"
			// |  |--CLogicalGet "t3"
			// |--CLogicalGet "t4"
			//
			// If the predicate between the CLogicalNAryJoin and first CLogicalLeftOuterJoin
			// is NULL rejecting, we convert the left join to an inner join and create a new
			// expression. Then the modified tree would be:
			//
			// +--CLogicalNAryJoin
			// |--CLogicalLeftOuterJoin
			// |  |--CLogicalGet "t1"
			// |  |--CLogicalGet "t2"
			// |--CLogicalGet "t3"
			// |--CLogicalGet "t4"
			//
			// Note that we can still convert the second CLogicalLeftOuterJoin into an inner join
			// if the predicate between the CLogicalNAryJoin is NULL rejecting. So we need to recurse
			// into the child and continue checking if we can convert the LOJs into inner joins.

			CExpression *pexprChildNew =
				PexprOuterJoinToInnerJoin(mp, pexprChild);
			if (fNewChild)
			{
				pexprChild->Release();
			}
			pdrspqexprChildren->Append(pexprChildNew);
		}

		return SPQOS_NEW(mp) CExpression(mp, SPQOS_NEW(mp) CLogicalNAryJoin(mp),
										pdrspqexprChildren);
	}

	// current operator is not an NAry-join, recursively process children
	CExpressionArray *pdrspqexprChildren = SPQOS_NEW(mp) CExpressionArray(mp);
	for (ULONG ul = 0; ul < arity; ul++)
	{
		CExpression *pexprChild = PexprOuterJoinToInnerJoin(mp, (*pexpr)[ul]);
		pdrspqexprChildren->Append(pexprChild);
	}

	pop->AddRef();
	return SPQOS_NEW(mp) CExpression(mp, pop, pdrspqexprChildren);
}

// generate n*(n-1)/2 equality predicates, up to SPQOPT_MAX_DERIVED_PREDS, between
// the n columns in the given equivalence class (set),
CExpression *
CExpressionPreprocessor::PexprConjEqualityPredicates(CMemoryPool *mp,
													 CColRefSet *pcrs)
{
	SPQOS_ASSERT(NULL != pcrs);

	CExpressionArray *pdrspqexpr = SPQOS_NEW(mp) CExpressionArray(mp);
	ULONG ulPreds = 0;
	CColRefSetIter crsiRight(*pcrs);
	while (crsiRight.Advance() && SPQOPT_MAX_DERIVED_PREDS > ulPreds)
	{
		CColRef *pcrRight = crsiRight.Pcr();

		CColRefSetIter crsiLeft(*pcrs);
		while (crsiLeft.Advance() && SPQOPT_MAX_DERIVED_PREDS > ulPreds)
		{
			CColRef *pcrLeft = crsiLeft.Pcr();
			if (pcrLeft == pcrRight)
			{
				break;
			}

			pdrspqexpr->Append(CUtils::PexprScalarEqCmp(mp, pcrLeft, pcrRight));
			ulPreds++;
		}
	}

	return CPredicateUtils::PexprConjunction(mp, pdrspqexpr);
}

// check if all columns in the given equivalent class come from one of the
// children of the given expression
BOOL
CExpressionPreprocessor::FEquivClassFromChild(CColRefSet *pcrs,
											  CExpression *pexpr)
{
	SPQOS_ASSERT(NULL != pcrs);
	SPQOS_ASSERT(NULL != pexpr);

	const ULONG ulChildren = pexpr->Arity();
	for (ULONG ul = 0; ul < ulChildren; ul++)
	{
		CExpression *pexprChild = (*pexpr)[ul];
		if (!pexprChild->Pop()->FLogical())
		{
			continue;
		}
		CColRefSetArray *pdrspqcrs =
			pexprChild->DerivePropertyConstraint()->PdrspqcrsEquivClasses();
		if (pcrs->FContained(pdrspqcrs))
		{
			return true;
		}
	}

	return false;
}

// additional equality predicates are generated based on the equivalence
// classes in the constraint properties of the expression.
CExpression *
CExpressionPreprocessor::PexprAddEqualityPreds(CMemoryPool *mp,
											   CExpression *pexpr,
											   CColRefSet *pcrsProcessed)
{
	SPQOS_ASSERT(NULL != pcrsProcessed);
	SPQOS_ASSERT(NULL != pexpr);
	SPQOS_ASSERT(pexpr->Pop()->FLogical());

	const ULONG ulChildren = pexpr->Arity();
	CPropConstraint *ppc = pexpr->DerivePropertyConstraint();

	CExpression *pexprPred = NULL;
	COperator *pop = pexpr->Pop();
	if (CUtils::FLogicalDML(pop))
	{
		pexprPred = CUtils::PexprScalarConstBool(mp, true);
	}
	else
	{
		CExpressionArray *pdrspqexpr = SPQOS_NEW(mp) CExpressionArray(mp);
		CColRefSetArray *pdrspqcrs = ppc->PdrspqcrsEquivClasses();
		SPQOS_ASSERT(NULL != pdrspqcrs);
		const ULONG ulEquivClasses = pdrspqcrs->Size();
		for (ULONG ul = 0; ul < ulEquivClasses; ul++)
		{
			CColRefSet *pcrsEquivClass = (*pdrspqcrs)[ul];

			CColRefSet *pcrsEquality = SPQOS_NEW(mp) CColRefSet(mp);
			pcrsEquality->Include(pcrsEquivClass);
			pcrsEquality->Exclude(pcrsProcessed);

			// if equivalence class comes from any of the children, then skip it
			if (FEquivClassFromChild(pcrsEquality, pexpr))
			{
				pcrsEquality->Release();
				continue;
			}

			CExpression *pexprEquality =
				PexprConjEqualityPredicates(mp, pcrsEquality);
			pcrsProcessed->Include(pcrsEquality);
			pcrsEquality->Release();
			pdrspqexpr->Append(pexprEquality);
		}

		pexprPred = CPredicateUtils::PexprConjunction(mp, pdrspqexpr);
	}

	CExpressionArray *pdrspqexprChildren = SPQOS_NEW(mp) CExpressionArray(mp);
	for (ULONG ul = 0; ul < ulChildren; ul++)
	{
		CExpression *pexprChild = (*pexpr)[ul];
		if (pexprChild->Pop()->FLogical())
		{
			CExpression *pexprChildNew =
				PexprAddEqualityPreds(mp, pexprChild, pcrsProcessed);
			pdrspqexprChildren->Append(pexprChildNew);
		}
		else
		{
			pexprChild->AddRef();
			pdrspqexprChildren->Append(pexprChild);
		}
	}

	pop->AddRef();

	return CUtils::PexprSafeSelect(
		mp, SPQOS_NEW(mp) CExpression(mp, pop, pdrspqexprChildren), pexprPred);
}

// generate predicates for the given set of columns based on the given
// constraint property. Columns for which predicates are generated will be
// added to the set of processed columns
CExpression *
CExpressionPreprocessor::PexprScalarPredicates(
	CMemoryPool *mp, CPropConstraint *ppc,
	CPropConstraint *constraintsForOuterRefs,
	CPropConstraint *ppcFromFilterSubquery, CColRefSet *pcrsNotNull,
	CColRefSet *pcrs, CColRefSet *pcrsProcessed)
{
	CExpressionArray *pdrspqexpr = SPQOS_NEW(mp) CExpressionArray(mp);

	CColRefSetIter crsi(*pcrs);
	while (crsi.Advance())
	{
		CColRef *colref = crsi.Pcr();
		CColRefSet *crs = NULL;	 // equiv class column refset

		if (ppcFromFilterSubquery != NULL &&
			(crs = ppcFromFilterSubquery->PcrsEquivClass(colref)) != NULL)
		{
			// add predicates that are inferred from subquery
			CConstraint *pcnstr =
				ppcFromFilterSubquery->Pcnstr()->Pcnstr(mp, crs);
			CConstraint *pcnstrCol = pcnstr->PcnstrRemapForColumn(mp, colref);
			CExpression *pexprScalar = pcnstrCol->PexprScalar(mp);

			// do not add a NOT NULL predicate if column is not nullable or if it
			// already has another predicate on it
			if (!(CUtils::FScalarNotNull(pexprScalar) &&
				  (pcrsNotNull->FMember(colref) ||
				   (ppc->Pcnstr() != NULL &&
					ppc->Pcnstr()->FConstraint(colref)))))
			{
				pexprScalar->AddRef();
				pdrspqexpr->Append(pexprScalar);
			}

			pcrsProcessed->Include(colref);
			pcnstr->Release();
			pcnstrCol->Release();
		}

		CExpression *pexprScalar = ppc->PexprScalarMappedFromEquivCols(
			mp, colref, constraintsForOuterRefs);
		if (NULL == pexprScalar)
		{
			continue;
		}

		pcrsProcessed->Include(colref);

		// do not add a NOT NULL predicate if column is not nullable or if it
		// already has another predicate on it
		if (CUtils::FScalarNotNull(pexprScalar) &&
			(pcrsNotNull->FMember(colref) ||
			 ppc->Pcnstr()->FConstraint(colref)))
		{
			pexprScalar->Release();
			continue;
		}
		pdrspqexpr->Append(pexprScalar);
	}

	if (0 == pdrspqexpr->Size())
	{
		pdrspqexpr->Release();
		return NULL;
	}

	return CPredicateUtils::PexprConjunction(mp, pdrspqexpr);
}

// process scalar expressions for generating additional predicates based on
// derived constraints. This function is needed because scalar expressions
// can have relational children when there are subqueries
CExpression *
CExpressionPreprocessor::PexprFromConstraintsScalar(
	CMemoryPool *mp, CExpression *pexpr,
	CPropConstraint *constraintsForOuterRefs)
{
	SPQOS_ASSERT(NULL != pexpr);
	SPQOS_ASSERT(pexpr->Pop()->FScalar());

	if (!CUtils::FHasSubquery(pexpr))
	{
		pexpr->AddRef();
		return pexpr;
	}

	const ULONG ulChildren = pexpr->Arity();
	CExpressionArray *pdrspqexprChildren = SPQOS_NEW(mp) CExpressionArray(mp);

	for (ULONG ul = 0; ul < ulChildren; ul++)
	{
		CExpression *pexprChild = (*pexpr)[ul];
		if (pexprChild->Pop()->FScalar())
		{
			pexprChild = PexprFromConstraintsScalar(mp, pexprChild,
													constraintsForOuterRefs);
		}
		else
		{
			SPQOS_ASSERT(pexprChild->Pop()->FLogical());
			CColRefSet *pcrsProcessed = SPQOS_NEW(mp) CColRefSet(mp);
			pexprChild = PexprFromConstraints(mp, pexprChild, pcrsProcessed,
											  constraintsForOuterRefs);
			pcrsProcessed->Release();
		}

		pdrspqexprChildren->Append(pexprChild);
	}

	COperator *pop = pexpr->Pop();
	pop->AddRef();
	return SPQOS_NEW(mp) CExpression(mp, pop, pdrspqexprChildren);
}

// Imply new predicates on LOJ's inner child based on constraints derived
// from LOJ's outer child and join predicate
CExpression *
CExpressionPreprocessor::PexprWithImpliedPredsOnLOJInnerChild(
	CMemoryPool *mp, CExpression *pexprLOJ,
	BOOL *
		pfAddedPredicates  // output: set to True if new predicates are added to inner child
)
{
	SPQOS_ASSERT(NULL != pexprLOJ);
	SPQOS_ASSERT(NULL != pfAddedPredicates);
	SPQOS_ASSERT(COperator::EopLogicalLeftOuterJoin == pexprLOJ->Pop()->Eopid());

	CExpression *pexprOuter = (*pexprLOJ)[0];
	CExpression *pexprInner = (*pexprLOJ)[1];
	CExpression *pexprOuterJoinPred = (*pexprLOJ)[2];

	// merge children constraints with constraints derived from join's predicate
	CExpressionHandle exprhdl(mp);
	exprhdl.Attach(pexprLOJ);
	CPropConstraint *ppc =
		CLogical::PpcDeriveConstraintFromPredicates(mp, exprhdl);

	// use the computed constraint to derive a scalar predicate on the inner child
	CColRefSet *pcrsInnerOutput = pexprInner->DeriveOutputColumns();
	CColRefSet *pcrsInnerNotNull = pexprInner->DeriveNotNullColumns();

	// generate a scalar predicate from the computed constraint, restricted to LOJ inner child
	CColRefSet *pcrsProcessed = SPQOS_NEW(mp) CColRefSet(mp);
	CExpression *pexprPred = PexprScalarPredicates(
		mp, ppc, NULL /*no outer refs*/, NULL /*no constraints from subquery*/,
		pcrsInnerNotNull, pcrsInnerOutput, pcrsProcessed);
	pcrsProcessed->Release();
	ppc->Release();

	pexprInner->AddRef();
	if (NULL != pexprPred && !CUtils::FScalarConstTrue(pexprPred))
	{
		// if a new predicate was added, set the output flag to True
		*pfAddedPredicates = true;
		pexprPred->AddRef();
		CExpression *pexprSelect =
			CUtils::PexprLogicalSelect(mp, pexprInner, pexprPred);
		CExpression *pexprInnerNormalized =
			CNormalizer::PexprNormalize(mp, pexprSelect);
		pexprSelect->Release();
		pexprInner = pexprInnerNormalized;
	}
	CRefCount::SafeRelease(pexprPred);

	// recursively process inner child
	CExpression *pexprNewInner =
		PexprOuterJoinInferPredsFromOuterChildToInnerChild(mp, pexprInner,
														   pfAddedPredicates);
	pexprInner->Release();

	// recursively process outer child
	CExpression *pexprNewOuter =
		PexprOuterJoinInferPredsFromOuterChildToInnerChild(mp, pexprOuter,
														   pfAddedPredicates);

	pexprOuterJoinPred->AddRef();
	COperator *pop = pexprLOJ->Pop();
	pop->AddRef();

	return SPQOS_NEW(mp)
		CExpression(mp, pop, pexprNewOuter, pexprNewInner, pexprOuterJoinPred);
}

// Infer predicate from outer child to inner child of the outer join,
//
//	for LOJ expressions with predicates on outer child, e.g.,
//
//		+-LOJ(x=y)
//  		|---Select(x=5)
// 	    	|   	+----X
// 	   		+----Y
//
//	this function implies an equivalent predicate (y=5) on the inner child of LOJ:
//
//		+-LOJ(x=y)
//			|---Select(x=5)
//			|		+----X
//			+---Select(y=5)
//					+----Y
//
//	the correctness of this rewrite can be proven as follows:
//		- By removing all tuples from Y that do not satisfy (y=5), the LOJ
//		results, where x=y, are retained. The reason is that any such join result
//		must satisfy (x=5 ^ x=y) which implies that (y=5).
//
//		- LOJ results that correspond to tuples from X not joining with any tuple
//		from Y are also retained. The reason is that such join results can only be
//		produced if for all tuples in Y, we have (y!=5). By selecting Y tuples where (y=5),
//		if we end up with no Y tuples, the LOJ results will be generated by joining X with empty Y.
//		This is the same as joining with all tuples from Y with (y!=5). If we end up with
//		any tuple in Y satisfying (y=5), no LOJ results corresponding to X tuples not joining
//		with Y can be produced.
//
//	to implement this rewrite in a general form, we need to imply general constraints on
//	LOJ's inner child from constraints that exist on LOJ's outer child. The generated predicate
//	from this inference can only be inserted below LOJ (on top of the inner child), and cannot be
//	inserted on top of LOJ, otherwise we may wrongly convert LOJ to inner-join.
CExpression *
CExpressionPreprocessor::PexprOuterJoinInferPredsFromOuterChildToInnerChild(
	CMemoryPool *mp, CExpression *pexpr,
	BOOL *
		pfAddedPredicates  // output: set to True if new predicates are added to inner child
)
{
	SPQOS_ASSERT(NULL != pexpr);
	SPQOS_ASSERT(NULL != pfAddedPredicates);

	COperator *pop = pexpr->Pop();
	if (COperator::EopLogicalLeftOuterJoin == pop->Eopid())
	{
		return PexprWithImpliedPredsOnLOJInnerChild(mp, pexpr,
													pfAddedPredicates);
	}

	// not an outer join, process children recursively
	CExpressionArray *pdrspqexpr = SPQOS_NEW(mp) CExpressionArray(mp);

	const ULONG ulChildren = pexpr->Arity();
	for (ULONG ul = 0; ul < ulChildren; ul++)
	{
		CExpression *pexprChild =
			PexprOuterJoinInferPredsFromOuterChildToInnerChild(
				mp, (*pexpr)[ul], pfAddedPredicates);
		pdrspqexpr->Append(pexprChild);
	}

	pop->AddRef();
	return SPQOS_NEW(mp) CExpression(mp, pop, pdrspqexpr);
}

// additional predicates are generated based on the derived constraint
// properties of the expression. No predicates are generated for the columns
// in the already processed set. This set is expanded with more columns
// that get processed along the way
CExpression *
CExpressionPreprocessor::PexprFromConstraints(
	CMemoryPool *mp, CExpression *pexpr, CColRefSet *pcrsProcessed,
	CPropConstraint *constraintsForOuterRefs)
{
	SPQOS_ASSERT(NULL != pcrsProcessed);
	SPQOS_ASSERT(NULL != pexpr);
	SPQOS_ASSERT(pexpr->Pop()->FLogical());

	const ULONG ulChildren = pexpr->Arity();
	CPropConstraint *ppc = pexpr->DerivePropertyConstraint();
	CColRefSet *pcrsNotNull = pexpr->DeriveNotNullColumns();
	CPropConstraint *ppcFromFilterSubquery =
		CExpressionUtils::GetPropConstraintFromSubquery(mp, pexpr);
	CExpressionArray *pdrspqexprChildren = SPQOS_NEW(mp) CExpressionArray(mp);

	for (ULONG ul = 0; ul < ulChildren; ul++)
	{
		CExpression *pexprChild = (*pexpr)[ul];
		if (pexprChild->Pop()->FScalar())
		{
			// we could potentially combine constraintsForOuterRefs and ppc,
			// but for now just pass ppc, the constraints from the direct ancestor
			// of the subquery
			pexprChild = PexprFromConstraintsScalar(mp, pexprChild, ppc);
			pdrspqexprChildren->Append(pexprChild);
			continue;
		}

		// process child
		CExpression *pexprChildNew = PexprFromConstraints(
			mp, pexprChild, pcrsProcessed, constraintsForOuterRefs);

		CColRefSet *pcrsOutChild = SPQOS_NEW(mp) CColRefSet(mp);
		// output columns on which predicates must be inferred
		pcrsOutChild->Include(pexprChild->DeriveOutputColumns());

		// exclude column references on which predicates had been already inferred,
		// this avoids generating duplicate predicates on the parent node if a
		// predicate has already been placed on the child.
		pcrsOutChild->Exclude(pcrsProcessed);

		// generate predicates for the output columns of child
		CExpression *pexprPred = PexprScalarPredicates(
			mp, ppc, constraintsForOuterRefs, ppcFromFilterSubquery,
			pcrsNotNull, pcrsOutChild, pcrsProcessed);
		pcrsOutChild->Release();

		if (NULL != pexprPred)
		{
			pdrspqexprChildren->Append(
				CUtils::PexprSafeSelect(mp, pexprChildNew, pexprPred));
		}
		else
		{
			pdrspqexprChildren->Append(pexprChildNew);
		}
	}

	COperator *pop = pexpr->Pop();
	pop->AddRef();

	CExpression *pexprPred =
		SPQOS_NEW(mp) CExpression(mp, pop, pdrspqexprChildren);
	if (ppcFromFilterSubquery != NULL)
	{
		CExpression *pexprNormalized =
			CNormalizer::PexprNormalize(mp, pexprPred);
		pexprPred->Release();
		pexprPred = pexprNormalized;
		ppcFromFilterSubquery->Release();
	}

	return pexprPred;
}

// eliminate subtrees that have a zero output cardinality, replacing them
// with a const table get with the same output schema and zero tuples
CExpression *
CExpressionPreprocessor::PexprPruneEmptySubtrees(CMemoryPool *mp,
												 CExpression *pexpr)
{
	SPQOS_ASSERT(NULL != pexpr);

	COperator *pop = pexpr->Pop();
	if (pop->FLogical() && !CUtils::FLogicalDML(pop))
	{
		// if maxcard = 0: return a const table get with same output columns and zero tuples
		if (0 == pexpr->DeriveMaxCard())
		{
			// output columns
			CColRefArray *colref_array =
				pexpr->DeriveOutputColumns()->Pdrspqcr(mp);

			// empty output data
			IDatum2dArray *pdrspqdrspqdatum = SPQOS_NEW(mp) IDatum2dArray(mp);

			COperator *popCTG = SPQOS_NEW(mp)
				CLogicalConstTableGet(mp, colref_array, pdrspqdrspqdatum);
			return SPQOS_NEW(mp) CExpression(mp, popCTG);
		}
	}

	// process children
	CExpressionArray *pdrspqexpr = SPQOS_NEW(mp) CExpressionArray(mp);

	const ULONG ulChildren = pexpr->Arity();
	for (ULONG ul = 0; ul < ulChildren; ul++)
	{
		CExpression *pexprChild = PexprPruneEmptySubtrees(mp, (*pexpr)[ul]);
		pdrspqexpr->Append(pexprChild);
	}

	pop->AddRef();
	return SPQOS_NEW(mp) CExpression(mp, pop, pdrspqexpr);
}

// eliminate CTE Anchors for CTEs that have zero consumers
CExpression *
CExpressionPreprocessor::PexprRemoveUnusedCTEs(CMemoryPool *mp,
											   CExpression *pexpr)
{
	SPQOS_ASSERT(NULL != pexpr);

	COperator *pop = pexpr->Pop();
	if (COperator::EopLogicalCTEAnchor == pop->Eopid())
	{
		ULONG id = CLogicalCTEAnchor::PopConvert(pop)->Id();
		if (!COptCtxt::PoctxtFromTLS()->Pcteinfo()->FUsed(id))
		{
			SPQOS_ASSERT(1 == pexpr->Arity());
			return PexprRemoveUnusedCTEs(mp, (*pexpr)[0]);
		}
	}

	// process children
	CExpressionArray *pdrspqexpr = SPQOS_NEW(mp) CExpressionArray(mp);

	const ULONG ulChildren = pexpr->Arity();
	for (ULONG ul = 0; ul < ulChildren; ul++)
	{
		CExpression *pexprChild = PexprRemoveUnusedCTEs(mp, (*pexpr)[ul]);
		pdrspqexpr->Append(pexprChild);
	}

	pop->AddRef();
	return SPQOS_NEW(mp) CExpression(mp, pop, pdrspqexpr);
}

// for all consumers of the same CTE, collect all selection predicates
// on top of these consumers, if any, and store them in hash map
void
CExpressionPreprocessor::CollectCTEPredicates(CMemoryPool *mp,
											  CExpression *pexpr,
											  CTEPredsMap *phm)
{
	SPQOS_CHECK_STACK_SIZE;

	if (COperator::EopLogicalSelect == pexpr->Pop()->Eopid() &&
		COperator::EopLogicalCTEConsumer == (*pexpr)[0]->Pop()->Eopid() &&
		0 == pexpr->DeriveOuterReferences()
				 ->Size()  // no outer references in selection predicate
	)
	{
		CExpression *pexprScalar = (*pexpr)[1];
		if (!pexprScalar->DeriveHasSubquery())
		{
			CExpression *pexprChild = (*pexpr)[0];
			CLogicalCTEConsumer *popConsumer =
				CLogicalCTEConsumer::PopConvert(pexprChild->Pop());
			ULONG ulCTEId = popConsumer->UlCTEId();
			CExpression *pexprProducer =
				COptCtxt::PoctxtFromTLS()->Pcteinfo()->PexprCTEProducer(
					ulCTEId);
			SPQOS_ASSERT(NULL != pexprProducer);

			CLogicalCTEProducer *popProducer =
				CLogicalCTEProducer::PopConvert(pexprProducer->Pop());
			UlongToColRefMap *colref_mapping = CUtils::PhmulcrMapping(
				mp, popConsumer->Pdrspqcr(), popProducer->Pdrspqcr());
			CExpression *pexprRemappedScalar =
				pexprScalar->PexprCopyWithRemappedColumns(mp, colref_mapping,
														  true /*must_exist*/);
			colref_mapping->Release();

			CExpressionArray *pdrspqexpr = phm->Find(&ulCTEId);
			if (NULL == pdrspqexpr)
			{
				pdrspqexpr = SPQOS_NEW(mp) CExpressionArray(mp);
#ifdef SPQOS_DEBUG
				BOOL fInserted =
#endif	// SPQOS_DEBUG
					phm->Insert(SPQOS_NEW(mp) ULONG(ulCTEId), pdrspqexpr);
				SPQOS_ASSERT(fInserted);
			}
			pdrspqexpr->Append(pexprRemappedScalar);
		}
	}

	// process children recursively
	const ULONG ulChildren = pexpr->Arity();
	for (ULONG ul = 0; ul < ulChildren; ul++)
	{
		CollectCTEPredicates(mp, (*pexpr)[ul], phm);
	}
}

// add CTE predicates collected from consumers to producer expressions
void
CExpressionPreprocessor::AddPredsToCTEProducers(CMemoryPool *mp,
												CExpression *pexpr)
{
	CTEPredsMap *phm = SPQOS_NEW(mp) CTEPredsMap(mp);
	CollectCTEPredicates(mp, pexpr, phm);

	CCTEInfo *pcteinfo = COptCtxt::PoctxtFromTLS()->Pcteinfo();
	CTEPredsMapIter mi(phm);
	while (mi.Advance())
	{
		ULONG ulCTEId = *(mi.Key());
		CExpression *pexprProducer = pcteinfo->PexprCTEProducer(ulCTEId);
		SPQOS_ASSERT(NULL != pexprProducer);

		ULONG ulConsumers = pcteinfo->UlConsumers(ulCTEId);
		CExpressionArray *pdrspqexpr =
			const_cast<CExpressionArray *>(mi.Value());

		// skip the propagation of predicate contains volatile function e.g. random() (value change within a scan)
		if (CPredicateUtils::FContainsVolatileFunction(pdrspqexpr))
		{
			continue;
		}

		if (0 < ulConsumers && pdrspqexpr->Size() == ulConsumers)
		{
			// add new predicate to CTE producer only if all consumers have selection predicates,
			// for example, in the following query
			// 'with v as (select * from A) select * from v where a > 5 union select * from v where b > 5'
			// we add the new predicate '(a > 5 or b > 5)' to CTE producer expression,
			// while for the following query
			// 'with v as (select * from A) select * from v where a > 5 union select * from v'
			// we do not add any new predicates to CTE producer expression

			pdrspqexpr->AddRef();
			CExpression *pexprPred =
				CPredicateUtils::PexprDisjunction(mp, pdrspqexpr);
			(*pexprProducer)[0]->AddRef();
			CExpression *pexprSelect =
				CUtils::PexprLogicalSelect(mp, (*pexprProducer)[0], pexprPred);
			COperator *pop = pexprProducer->Pop();
			pop->AddRef();
			CExpression *pexprNewProducer =
				SPQOS_NEW(mp) CExpression(mp, pop, pexprSelect);

			pcteinfo->ReplaceCTEProducer(pexprNewProducer);
			pexprNewProducer->Release();
		}
	}

	phm->Release();
}

// derive constraints on given expression tree, and add new predicates by implication
CExpression *
CExpressionPreprocessor::PexprAddPredicatesFromConstraints(CMemoryPool *mp,
														   CExpression *pexpr)
{
	// normalize the tree, push down predicates (since we infer predicates bottom-up,
	// we want the predicates/constraints to be at the lowest possible point in the tree)
	CExpression *pexprNormalized = CNormalizer::PexprNormalize(mp, pexpr);

	// walk the tree and generate additional predicates from constraint properties
	// based on equivalence classes, e.g. constraint a=1 and equiv class {a,b} adds pred b=1
	CColRefSet *pcrsProcessed = SPQOS_NEW(mp) CColRefSet(mp);
	CExpression *pexprConstraints =
		PexprFromConstraints(mp, pexprNormalized, pcrsProcessed, NULL);
	SPQOS_CHECK_ABORT;
	pexprNormalized->Release();
	pcrsProcessed->Release();

	// walk the tree again and generate equality predicates for columns in
	// equivalence classes, e.g. {cr1,cr2,cr3} results in cr1=cr2 and cr1=cr3 and cr2=cr3
	pcrsProcessed = SPQOS_NEW(mp) CColRefSet(mp);
	CExpression *pexprAddEqualityPreds =
		PexprAddEqualityPreds(mp, pexprConstraints, pcrsProcessed);

	// normalize the tree, push down predicates
	CExpression *pexprEqualityNormalized =
		CNormalizer::PexprNormalize(mp, pexprAddEqualityPreds);
	SPQOS_CHECK_ABORT;
	pcrsProcessed->Release();
	pexprConstraints->Release();
	pexprAddEqualityPreds->Release();

	// remove generated duplicate predicates
	CExpression *pexprDeduped =
		CExpressionUtils::PexprDedupChildren(mp, pexprEqualityNormalized);
	pexprEqualityNormalized->Release();

	return pexprDeduped;
}

// driver for inferring predicates from constraints
CExpression *
CExpressionPreprocessor::PexprInferPredicates(CMemoryPool *mp,
											  CExpression *pexpr)
{
	SPQOS_ASSERT(NULL != pexpr);

	// generate new predicates from constraint properties and normalize the result
	CExpression *pexprWithPreds = PexprAddPredicatesFromConstraints(mp, pexpr);

	// infer predicates from outer child to inner child of outer join
	BOOL fNewPreds = false;
	CExpression *pexprInferredPreds =
		PexprOuterJoinInferPredsFromOuterChildToInnerChild(mp, pexprWithPreds,
														   &fNewPreds);
	pexprWithPreds->Release();
	pexprWithPreds = pexprInferredPreds;

	if (fNewPreds)
	{
		// if we succeeded in generating new predicates below outer join, we need to
		// re-derive constraints to generate any other potential predicates
		pexprWithPreds =
			PexprAddPredicatesFromConstraints(mp, pexprInferredPreds);
		pexprInferredPreds->Release();
	}

	return pexprWithPreds;
}

//	Workhorse for pruning unused computed columns
//
//	The required columns passed by the query is passed to this pre-processing
//	stage and the list of columns are copied to a new list. This driver function
//	calls the PexprPruneUnusedComputedColsRecursive function with the copied
//	required column set. The original required columns set is not modified by
//	this preprocessor.
//
// 	Extra copy of the required columns set is avoided in each recursive call by
//	creating a one-time copy and passing it by reference for all the recursive
//	calls.
//
//	The functional behavior of the PruneUnusedComputedCols changed slightly
//	because we do not delete the required column set at the end of every
//	call but pass it to the next and consecutive recursive calls. However,
//	it is safe to add required columns by each operator we traverse, because non
//	of the required columns from other child of a tree will appear on the project
//	list of the other children.
//
// Therefore, the added columns to the required columns which is caused by
// the recursive call and passing by reference will not have a bad affect
// on the overall result.
CExpression *
CExpressionPreprocessor::PexprPruneUnusedComputedCols(CMemoryPool *mp,
													  CExpression *pexpr,
													  CColRefSet *pcrsReqd)
{
	SPQOS_ASSERT(NULL != pexpr);

	if (NULL == pcrsReqd ||
		SPQOS_FTRACE(EopttraceDisablePruneUnusedComputedColumns))
	{
		pexpr->AddRef();
		return pexpr;
	}
	CColRefSet *pcrsReqdNew = SPQOS_NEW(mp) CColRefSet(mp);
	pcrsReqdNew->Include(pcrsReqd);

	CExpression *pExprNew =
		PexprPruneUnusedComputedColsRecursive(mp, pexpr, pcrsReqdNew);
	pcrsReqdNew->Release();
	return pExprNew;
}

// Workhorse for pruning unused computed columns
CExpression *
CExpressionPreprocessor::PexprPruneUnusedComputedColsRecursive(
	CMemoryPool *mp, CExpression *pexpr, CColRefSet *pcrsReqd)
{
	SPQOS_ASSERT(NULL != pexpr);

	COperator *pop = pexpr->Pop();

	// leave subquery alone
	if (CUtils::FSubquery(pop))
	{
		pexpr->AddRef();
		return pexpr;
	}

	if (COperator::EopLogicalProject == pop->Eopid() ||
		COperator::EopLogicalGbAgg == pop->Eopid())
	{
		CExpression *pexprProjList = (*pexpr)[1];
		CColRefSet *pcrsDefined = pexprProjList->DeriveDefinedColumns();
		CColRefSet *pcrsSetReturningFunction =
			pexprProjList->DeriveSetReturningFunctionColumns();

		pcrsReqd->Include(CLogical::PopConvert(pop)->PcrsLocalUsed());
		// columns containing set-returning functions are needed for correct query results
		pcrsReqd->Union(pcrsSetReturningFunction);

		CColRefSet *pcrsUnusedLocal = SPQOS_NEW(mp) CColRefSet(mp);
		pcrsUnusedLocal->Include(pcrsDefined);
		pcrsUnusedLocal->Difference(pcrsReqd);

		if (0 < pcrsUnusedLocal->Size())  // need to prune
		{
			// actual construction of new operators without unnecessary project elements
			CExpression *pexprResult = PexprPruneProjListProjectOrGbAgg(
				mp, pexpr, pcrsUnusedLocal, pcrsDefined, pcrsReqd);
			pcrsUnusedLocal->Release();
			return pexprResult;
		}
		pcrsUnusedLocal->Release();
	}

	if (pop->FLogical())
	{
		// for logical operators, collect the used columns
		// this includes columns used by the operator itself and its scalar children
		CExpressionHandle exprhdl(mp);
		exprhdl.Attach(pexpr);
		CColRefSet *pcrsLogicalUsed = exprhdl.PcrsUsedColumns(mp);
		pcrsReqd->Include(pcrsLogicalUsed);
		pcrsLogicalUsed->Release();
	}

	// process children
	CExpressionArray *pdrspqexpr = SPQOS_NEW(mp) CExpressionArray(mp);
	const ULONG ulChildren = pexpr->Arity();

	for (ULONG ul = 0; ul < ulChildren; ul++)
	{
		CExpression *pexprChild =
			PexprPruneUnusedComputedColsRecursive(mp, (*pexpr)[ul], pcrsReqd);
		pdrspqexpr->Append(pexprChild);
	}

	pop->AddRef();

	return SPQOS_NEW(mp) CExpression(mp, pop, pdrspqexpr);
}

// Construct new Project or GroupBy operator without unused computed
// columns as project elements
CExpression *
CExpressionPreprocessor::PexprPruneProjListProjectOrGbAgg(
	CMemoryPool *mp, CExpression *pexpr, CColRefSet *pcrsUnused,
	CColRefSet *pcrsDefined, const CColRefSet *pcrsReqd)
{
	SPQOS_ASSERT(NULL != pexpr);
	SPQOS_ASSERT(NULL != pcrsUnused);
	SPQOS_ASSERT(NULL != pcrsDefined);
	SPQOS_ASSERT(NULL != pcrsReqd);

	CExpression *pexprResult = NULL;
	COperator *pop = pexpr->Pop();
	CColRefSet *pcrsReqdNew = SPQOS_NEW(mp) CColRefSet(mp);
	pcrsReqdNew->Include(pcrsReqd);

	SPQOS_ASSERT(COperator::EopLogicalProject == pop->Eopid() ||
				COperator::EopLogicalGbAgg == pop->Eopid());

	CExpression *pexprRelational = (*pexpr)[0];
	CExpression *pexprProjList = (*pexpr)[1];

	// recursively process the relational child
	CExpression *pexprRelationalNew = NULL;

	if (pcrsUnused->Size() == pcrsDefined->Size())
	{
		// the entire project list needs to be pruned
		if (COperator::EopLogicalProject == pop->Eopid())
		{
			pexprRelationalNew = PexprPruneUnusedComputedColsRecursive(
				mp, pexprRelational, pcrsReqdNew);
			pexprResult = pexprRelationalNew;
		}
		else
		{
			SPQOS_ASSERT(COperator::EopLogicalGbAgg == pop->Eopid());

			CExpression *pexprProjectListNew = NULL;
			CColRefArray *pdrspqcrGroupingCols =
				CLogicalGbAgg::PopConvert(pop)->Pdrspqcr();
			if (0 < pdrspqcrGroupingCols->Size())
			{
				// if grouping cols exist, we need to maintain the GbAgg with an empty project list
				pexprProjectListNew = SPQOS_NEW(mp)
					CExpression(mp, SPQOS_NEW(mp) CScalarProjectList(mp));
				pcrsReqdNew->Include(pdrspqcrGroupingCols);
			}
			else
			{
				// TODO:  10/15/2015: if there is no grouping cols, we could remove the entire GbAgg and plug in a ConstTableGet instead
				pexprProjList->AddRef();
				pexprProjectListNew = pexprProjList;
				CExpressionHandle exprhdl(mp);
				exprhdl.Attach(pexpr);
				CColRefSet *pcrsLogicalUsed = exprhdl.PcrsUsedColumns(mp);
				pcrsReqdNew->Include(pcrsLogicalUsed);
				pcrsLogicalUsed->Release();
			}
			pop->AddRef();
			pexprRelationalNew = PexprPruneUnusedComputedColsRecursive(
				mp, pexprRelational, pcrsReqdNew);
			pexprResult = SPQOS_NEW(mp)
				CExpression(mp, pop, pexprRelationalNew, pexprProjectListNew);
		}
	}
	else
	{
		// only remove part of the project elements
		CExpressionArray *pdrspqexprPrElRemain =
			SPQOS_NEW(mp) CExpressionArray(mp);
		const ULONG ulPrjEls = pexprProjList->Arity();
		CExpressionHandle exprhdl(mp);

		for (ULONG ul = 0; ul < ulPrjEls; ul++)
		{
			CExpression *pexprPrEl = (*pexprProjList)[ul];
			CScalarProjectElement *popPrEl =
				CScalarProjectElement::PopConvert(pexprPrEl->Pop());
			if (!pcrsUnused->FMember(popPrEl->Pcr()))
			{
				pexprPrEl->AddRef();
				pdrspqexprPrElRemain->Append(pexprPrEl);
				pcrsReqdNew->Include(pexprPrEl->DeriveUsedColumns());
			}
		}

		SPQOS_ASSERT(0 < pdrspqexprPrElRemain->Size());
		CExpression *pexprNewProjectList = SPQOS_NEW(mp) CExpression(
			mp, SPQOS_NEW(mp) CScalarProjectList(mp), pdrspqexprPrElRemain);
		pop->AddRef();
		pexprRelationalNew = PexprPruneUnusedComputedColsRecursive(
			mp, pexprRelational, pcrsReqdNew);
		pexprResult = SPQOS_NEW(mp)
			CExpression(mp, pop, pexprRelationalNew, pexprNewProjectList);
	}

	pcrsReqdNew->Release();
	return pexprResult;
}

// reorder the child for scalar comparision to ensure that left child is a scalar ident and right child is a scalar const if not
CExpression *
CExpressionPreprocessor::PexprReorderScalarCmpChildren(CMemoryPool *mp,
													   CExpression *pexpr)
{
	SPQOS_ASSERT(NULL != pexpr);

	COperator *pop = pexpr->Pop();
	if (CUtils::FScalarCmp(pexpr) ||
		COperator::EopScalarIsDistinctFrom == pexpr->Pop()->Eopid())
	{
		SPQOS_ASSERT(2 == pexpr->Arity());
		CExpression *pexprLeft = (*pexpr)[0];
		CExpression *pexprRight = (*pexpr)[1];

		if (CUtils::FScalarConst(pexprLeft) && CUtils::FScalarIdent(pexprRight))
		{
			CScalarCmp *popScalarCmpCommuted =
				(dynamic_cast<CScalarCmp *>(pop))->PopCommutedOp(mp, pop);
			if (popScalarCmpCommuted)
			{
				pexprLeft->AddRef();
				pexprRight->AddRef();
				return SPQOS_NEW(mp) CExpression(mp, popScalarCmpCommuted,
												pexprRight, pexprLeft);
			}
		}
	}

	// process children
	CExpressionArray *pdrspqexpr = SPQOS_NEW(mp) CExpressionArray(mp);
	const ULONG ulChildren = pexpr->Arity();

	for (ULONG ul = 0; ul < ulChildren; ul++)
	{
		CExpression *pexprChild =
			PexprReorderScalarCmpChildren(mp, (*pexpr)[ul]);
		pdrspqexpr->Append(pexprChild);
	}

	pop->AddRef();
	return SPQOS_NEW(mp) CExpression(mp, pop, pdrspqexpr);
}

// converts IN subquery to a predicate AND an EXISTS subquery
// Example Algebrized queries:
// 1. Without a Project List:
//		Input:
//		+--CScalarSubqueryAny(=)["c2" (0)]
//		|--CLogicalGet "foo" ("foo"), Columns: ["c1" (8) ...] Key sets: {[1,7]}
//		+--CScalarIdent "c2" (0)
//
//		Output:
//		+--CScalarBoolOp (EboolopAnd)
//		|--CScalarCmp (=)
//		|  |--CScalarIdent "c2" (0)
//		|  +--CScalarIdent "c2" (0)
//		+--CScalarSubqueryExists
//		   +--CLogicalGet "foo" ("foo"), Columns: ["c1" (8) ...] Key sets: {[1,7]}
//
// 2. With a Project List:
//		Input:
//		+--CScalarSubqueryAny(=)["?column?" (16)]
//		|--CLogicalProject
//		|  |--CLogicalGet "foo" ("foo"), Columns: ["c1" (8) ...] Key sets: {[1,7]}
//		|  +--CScalarProjectList
//		|     +--CScalarProjectElement "?column?" (16)
//		|        +--CScalarOp (+)
//		|           |--CScalarIdent "c2" (0)
//		|           +--CScalarConst (1)
//		+--CScalarIdent "c2" (0)
//
//		Output:
//		+--CScalarBoolOp (EboolopAnd)
//		|--CScalarCmp (=)
//		|  |--CScalarIdent "c2" (0)
//		|  +--CScalarOp (+)
//		|     |--CScalarIdent "c2" (0)
//		|     +--CScalarConst (1)
//		+--CScalarSubqueryExists
//		   +--CLogicalGet "foo" ("foo"), Columns: ["c1" (8) ...] Key sets: {[1,7]}
CExpression *
CExpressionPreprocessor::ConvertInToSimpleExists(CMemoryPool *mp,
												 CExpression *pexpr)
{
	SPQOS_ASSERT(COperator::EopScalarSubqueryAny == pexpr->Pop()->Eopid());

	COperator *pop = pexpr->Pop();
	CExpression *pexprRelational = (*pexpr)[0];

	// Example for below variables:
	//   SELECT * FROM bar WHERE
	//       bar.a in (SELECT bar.b FROM foo)      <- Input expression (pexpr)
	//        |                |
	//    pexprLeft        pexprRight

	// generate scalarOp expression by using column reference of the IN subquery's
	// inner child's column reference as well as the expression extracted above
	// from the project element
	CExpression *pexprLeft = (*pexpr)[1];
	if (CUtils::FSubquery(pexprLeft->Pop()))
	{
		// don't convert if inner child is a subquery
		// Example: SELECT * FROM bar WHERE (SELECT 1) IN (SELECT c2 FROM foo);
		return NULL;
	}

	// since Orca doesn't support IN subqueries of multiple columns such as
	// (a,a) in (select foo.a, foo.a from ...) ,
	// only extract the first expression under the first project element in the
	// project list and make it as the right operand to the scalar operation.
	CExpression *pexprRight = NULL;
	CExpression *pexprSubqOfExists = NULL;
	if (COperator::EopLogicalProject == pexprRelational->Pop()->Eopid())
	{
		pexprRight = CUtils::PNthProjectElementExpr(pexprRelational, 0);
		pexprRight->AddRef();
		pexprSubqOfExists = (*pexprRelational)[0];
	}
	else
	{
		pexprRight = CUtils::PexprScalarIdent(
			mp, CScalarSubqueryAny::PopConvert(pop)->Pcr());
		pexprSubqOfExists = pexprRelational;
	}

	CMDAccessor *md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();
	IMDId *mdid = CScalarSubqueryAny::PopConvert(pop)->MdIdOp();
	const CWStringConst *str =
		md_accessor->RetrieveScOp(mdid)->Mdname().GetMDName();

	mdid->AddRef();
	pexprLeft->AddRef();
	CExpression *pexprScalarOp =
		CUtils::PexprScalarCmp(mp, pexprLeft, pexprRight, *str, mdid);

	pexprSubqOfExists->AddRef();
	CExpression *pexprScalarSubqExists = SPQOS_NEW(mp) CExpression(
		mp, SPQOS_NEW(mp) CScalarSubqueryExists(mp), pexprSubqOfExists);

	// AND the generated predicate with the EXISTS subquery expression and return.
	CExpressionArray *pdrspqexprBoolOperands = SPQOS_NEW(mp) CExpressionArray(mp);

	pdrspqexprBoolOperands->Append(pexprScalarOp);
	pdrspqexprBoolOperands->Append(pexprScalarSubqExists);

	return CUtils::PexprScalarBoolOp(mp, CScalarBoolOp::EboolopAnd,
									 pdrspqexprBoolOperands);
}

// rewrite IN subquery to EXIST subquery with a predicate
// Example:
//		Input:   SELECT * FROM foo WHERE foo.a IN (SELECT foo.b+1 FROM bar);
//		Output:  SELECT * FROM foo WHERE foo.a=foo.b+1 AND EXISTS (SELECT * FROM bar);
CExpression *
CExpressionPreprocessor::PexprExistWithPredFromINSubq(CMemoryPool *mp,
													  CExpression *pexpr)
{
	// protect against stack overflow during recursion
	SPQOS_CHECK_STACK_SIZE;
	SPQOS_ASSERT(NULL != mp);
	SPQOS_ASSERT(NULL != pexpr);

	COperator *pop = pexpr->Pop();

	// recursively process children
	const ULONG arity = pexpr->Arity();
	pop->AddRef();

	CExpressionArray *pdrspqexprChildren = SPQOS_NEW(mp) CExpressionArray(mp);
	for (ULONG ul = 0; ul < arity; ul++)
	{
		CExpression *pexprChild =
			PexprExistWithPredFromINSubq(mp, (*pexpr)[ul]);
		pdrspqexprChildren->Append(pexprChild);
	}

	CExpression *pexprNew =
		SPQOS_NEW(mp) CExpression(mp, pop, pdrspqexprChildren);
	//Check if the inner is a SubqueryAny
	if (CUtils::FAnySubquery(pop))
	{
		CExpression *pexprLogicalProject = (*pexprNew)[0];
		// we do the conversion if the project list has an outer reference and
		// it does not include any column from the relational child.
		if (COperator::EopLogicalProject == pexprLogicalProject->Pop()->Eopid())
		{
			// bail out if subquery has an inner reference or does not have any outer reference
			if (!CUtils::HasOuterRefs(pexprLogicalProject) ||
				CUtils::FInnerRefInProjectList(pexprLogicalProject))
			{
				return pexprNew;
			}
		}
		else
		{
			// perform conversion if subquery does not output any of the columns from relational child
			const CColRef *pcrSubquery =
				CScalarSubqueryAny::PopConvert(pop)->Pcr();
			CColRefSet *pcrsRelationalChild =
				(*pexpr)[0]->DeriveOutputColumns();
			if (pcrsRelationalChild->FMember(pcrSubquery))
			{
				return pexprNew;
			}
		}

		CExpression *pexprNewConverted = ConvertInToSimpleExists(mp, pexprNew);

		if (NULL != pexprNewConverted)
		{
			pexprNew->Release();
			pexprNew = pexprNewConverted;
			;
		}
	}

	return pexprNew;
}

// Collapse a select over a project and update column reference.
CExpression *
CExpressionPreprocessor::CollapseSelectAndReplaceColref(CMemoryPool *mp,
														CExpression *pexpr,
														CColRef *pcolref,
														CExpression *pprojExpr)
{
	// remove the logical project
	//
	// Input:
	// +--CLogicalSelect (x = 'meh')
	//    +--CLogicalProject (col1...n, expr as x)
	//       +-- CLogicalNAryJoin
	// Output:
	// +--CLogicalSelect (expr = 'meh')
	//    +-- CLogicalNAryJoin
	if (pexpr->Pop()->Eopid() == COperator::EopLogicalSelect &&
		(*pexpr)[0]->Pop()->Eopid() == COperator::EopLogicalProject &&
		(*(*pexpr)[0])[0]->Pop()->Eopid() == COperator::EopLogicalNAryJoin)
	{
		(*(*pexpr)[0])[0]->AddRef();
		CExpression *pexprCollapsedSelect = SPQOS_NEW(mp)
			CExpression(mp, SPQOS_NEW(mp) CLogicalSelect(mp), (*(*pexpr)[0])[0],
						CollapseSelectAndReplaceColref(mp, (*pexpr)[1], pcolref,
													   pprojExpr));

		CExpression *pexprTransposed =
			PexprTransposeSelectAndProject(mp, pexprCollapsedSelect);
		pexprCollapsedSelect->Release();
		return pexprTransposed;
	}

	// replace reference
	if (pexpr->Pop()->Eopid() == COperator::EopScalarIdent &&
		CColRef::Equals(CScalarIdent::PopConvert(pexpr->Pop())->Pcr(), pcolref))
	{
		pprojExpr->AddRef();
		return pprojExpr;
	}

	// recurse to children
	CExpressionArray *pdrspqexprChildren = SPQOS_NEW(mp) CExpressionArray(mp);
	for (ULONG ul = 0; ul < pexpr->Arity(); ul++)
	{
		pdrspqexprChildren->Append(CollapseSelectAndReplaceColref(
			mp, (*pexpr)[ul], pcolref, pprojExpr));
	}

	COperator *pop = pexpr->Pop();
	pop->AddRef();
	return SPQOS_NEW(mp) CExpression(mp, pop, pdrspqexprChildren);
}

// Transpose a select over a project
//
// This preprocessing step enables additional opportunities for predicate push
// down.  In the following example, the select constraint references "b" (18)
// from the project list. As-is the normalizer cannot push the contraint further
// than the project.  This step enables further push down by transposing the
// project/select operators and fixing up corresponding column references.
//
// Example:
//
// Input:
// +--CLogicalSelect
//    |--CLogicalProject
//    |  |--CLogicalNAryJoin
//    |  |  |--CLogicalGet "foo" ("foo"), Columns: ["a" (0), "b" (1), ...
//    |  |  |--CLogicalGet "bar" ("bar"), Columns: ["c" (9), "d" (10), ...
//    |  |  +--CScalarCmp (=)
//    |  |     |--CScalarIdent "a" (0)
//    |  |     +--CScalarIdent "c" (9)
//    |  +--CScalarProjectList
//    |     +--CScalarProjectElement "b" (18)
//    |        +--CScalarFunc (varchar)
//    |           |--CScalarCast
//    |           |  +--CScalarIdent "b" (1)
//    |           |--CScalarConst (104)
//    |           +--CScalarConst (1)
//    +--CScalarCmp (=)
//       |--CScalarCast
//       |  +--CScalarIdent "b" (18)
//       +--CScalarConst (1828233457.000)
// Output:
// +--CLogicalProject
//    |--CLogicalSelect
//    |  |--CLogicalNAryJoin
//    |  |  |--CLogicalGet "foo" ("foo"), Columns: ["a" (0), "b" (1), ...
//    |  |  |--CLogicalGet "bar" ("bar"), Columns: ["c" (9), "d" (10), ...
//    |  |  +--CScalarCmp (=)
//    |  |     |--CScalarIdent "a" (0)
//    |  |     +--CScalarIdent "c" (9)
//    |  +--CScalarCmp (=)
//    |     |--CScalarCast
//    |     |  +--CScalarFunc (varchar)
//    |     |     |--CScalarCast
//    |     |     |  +--CScalarIdent "b" (1)
//    |     |     |--CScalarConst (104)
//    |     |     +--CScalarConst (1)
//    |     +--CScalarConst (1828233457.000)
//    +--CScalarProjectList
//       +--CScalarProjectElement "b" (18)
//          +--CScalarFunc (varchar)
//             |--CScalarCast
//             |  +--CScalarIdent "b" (1)
//             |--CScalarConst (104)
//             +--CScalarConst (1)
CExpression *
CExpressionPreprocessor::PexprTransposeSelectAndProject(CMemoryPool *mp,
														CExpression *pexpr)
{
	// protect against stack overflow during recursion
	SPQOS_CHECK_STACK_SIZE;
	SPQOS_ASSERT(NULL != mp);
	SPQOS_ASSERT(NULL != pexpr);

	// Transpose:
	//
	// Input:
	// +--CLogicalSelect (x = 'meh')
	//    +--CLogicalProject (col1...n, expr as x)
	//       +-- CLogicalNAryJoin
	// Output:
	// +--CLogicalProject (col1..n, expr as x)
	//    +--CLogicalSelect (expr = 'meh')
	//       +-- CLogicalNAryJoin
	if (pexpr->Pop()->Eopid() == COperator::EopLogicalSelect &&
		(*pexpr)[0]->Pop()->Eopid() == COperator::EopLogicalProject &&
		(*(*pexpr)[0])[0]->Pop()->Eopid() == COperator::EopLogicalNAryJoin)
	{
		CExpression *pproject = (*pexpr)[0];
		CExpression *pprojectList = (*pproject)[1];
		CExpression *pselectNew = pexpr;

		CExpressionArray *pdrspqexpr = SPQOS_NEW(mp) CExpressionArray(mp);
		for (ULONG ul = 0; ul < pprojectList->Arity(); ul++)
		{
			CExpression *pprojexpr =
				CUtils::PNthProjectElementExpr(pproject, ul);

			if (pprojexpr->DeriveHasNonScalarFunction() ||
				pprojexpr->DeriveHasSubquery())
			{
				// Bail if project expression contains a set-returning function
				// or subquery
				pdrspqexpr->Release();
				pexpr->AddRef();
				return pexpr;
			}

			CExpressionHandle exprhdl(mp);
			exprhdl.Attach(pprojexpr);
			exprhdl.DeriveProps(NULL /*pdpctxt*/);

			if (exprhdl.FChildrenHaveVolatileFunc())
			{
				// Bail if project expression contains a volatile function
				pdrspqexpr->Release();
				pexpr->AddRef();
				return pexpr;
			}

			// TODO: In order to support mixed pushable and non-pushable
			//       predicates we need to be able to deconstruct a select
			//       conjunction constraint into pushable and non-pushable
			//       parts.
			//
			//       NB: JoinOnViewWithMixOfPushableAndNonpushablePredicates.mdp
			CExpression *prevpselectNew = pselectNew;
			pselectNew = CollapseSelectAndReplaceColref(
				mp, prevpselectNew,
				CUtils::PNthProjectElement(pproject, ul)->Pcr(),
				CUtils::PNthProjectElementExpr(pproject, ul));
			if (pexpr != prevpselectNew)
			{
				prevpselectNew->Release();
			}
		}
		pdrspqexpr->Append(pselectNew);

		CExpressionArray *pdrspqprojelems = SPQOS_NEW(mp) CExpressionArray(mp);
		for (ULONG ul = 0; ul < pprojectList->Arity(); ul++)
		{
			(*pprojectList)[ul]->AddRef();
			pdrspqprojelems->Append((*pprojectList)[ul]);
		}
		pdrspqexpr->Append(SPQOS_NEW(mp) CExpression(
			mp, SPQOS_NEW(mp) CScalarProjectList(mp), pdrspqprojelems));

		return SPQOS_NEW(mp)
			CExpression(mp, SPQOS_NEW(mp) CLogicalProject(mp), pdrspqexpr);
	}
	else
	{
		CExpressionArray *pdrspqexprChildren = SPQOS_NEW(mp) CExpressionArray(mp);
		for (ULONG ul = 0; ul < pexpr->Arity(); ul++)
		{
			pdrspqexprChildren->Append(
				PexprTransposeSelectAndProject(mp, (*pexpr)[ul]));
		}

		COperator *pop = pexpr->Pop();
		pop->AddRef();
		return SPQOS_NEW(mp) CExpression(mp, pop, pdrspqexprChildren);
	}
}

// main driver, pre-processing of input logical expression
CExpression *
CExpressionPreprocessor::PexprPreprocess(
	CMemoryPool *mp, CExpression *pexpr,
	CColRefSet *
		pcrsOutputAndOrderCols	// query output cols and cols used in the order specs
)
{
	SPQOS_ASSERT(NULL != mp);
	SPQOS_ASSERT(NULL != pexpr);

	CAutoTimer at("\n[OPT]: Expression Preprocessing Time",
				  SPQOS_FTRACE(EopttracePrintOptimizationStatistics));

	// (1) remove unused CTE anchors
	CExpression *pexprNoUnusedCTEs = PexprRemoveUnusedCTEs(mp, pexpr);
	SPQOS_CHECK_ABORT;

	// (2.a) remove intermediate superfluous limit
	CExpression *pexprSimplifiedLimit =
		PexprRemoveSuperfluousLimit(mp, pexprNoUnusedCTEs);
	SPQOS_CHECK_ABORT;
	pexprNoUnusedCTEs->Release();

	// (2.b) remove intermediate superfluous distinct
	CExpression *pexprSimplifiedDistinct =
		PexprRemoveSuperfluousDistinctInDQA(mp, pexprSimplifiedLimit);
	SPQOS_CHECK_ABORT;
	pexprSimplifiedLimit->Release();

	// (3) trim unnecessary existential subqueries
	CExpression *pexprTrimmed =
		PexprTrimExistentialSubqueries(mp, pexprSimplifiedDistinct);
	SPQOS_CHECK_ABORT;
	pexprSimplifiedDistinct->Release();

	// (4) collapse cascaded union / union all
	CExpression *pexprNaryUnionUnionAll =
		PexprCollapseUnionUnionAll(mp, pexprTrimmed);
	SPQOS_CHECK_ABORT;
	pexprTrimmed->Release();

	// (5) remove superfluous outer references from the order spec in limits, grouping columns in GbAgg, and
	// Partition/Order columns in window operators
	CExpression *pexprOuterRefsEleminated =
		PexprRemoveSuperfluousOuterRefs(mp, pexprNaryUnionUnionAll);
	SPQOS_CHECK_ABORT;
	pexprNaryUnionUnionAll->Release();

	// (6) remove superfluous equality
	CExpression *pexprTrimmed2 =
		PexprPruneSuperfluousEquality(mp, pexprOuterRefsEleminated);
	SPQOS_CHECK_ABORT;
	pexprOuterRefsEleminated->Release();

	// (7) simplify quantified subqueries
	CExpression *pexprSubqSimplified =
		PexprSimplifyQuantifiedSubqueries(mp, pexprTrimmed2);
	SPQOS_CHECK_ABORT;
	pexprTrimmed2->Release();

	// (8) do preliminary unnesting of scalar subqueries
	CExpression *pexprSubqUnnested =
		PexprUnnestScalarSubqueries(mp, pexprSubqSimplified);
	SPQOS_CHECK_ABORT;
	pexprSubqSimplified->Release();

	// (9) unnest AND/OR/NOT predicates
	CExpression *pexprUnnested =
		CExpressionUtils::PexprUnnest(mp, pexprSubqUnnested);
	SPQOS_CHECK_ABORT;
	pexprSubqUnnested->Release();

	CExpression *pexprConvert2In = pexprUnnested;

	if (SPQOS_FTRACE(EopttraceArrayConstraints))
	{
		// (9.5) ensure predicates are array IN or NOT IN where applicable
		pexprConvert2In = PexprConvert2In(mp, pexprUnnested);
		SPQOS_CHECK_ABORT;
		pexprUnnested->Release();
	}

	// (10) infer predicates from constraints
	CExpression *pexprInferredPreds = PexprInferPredicates(mp, pexprConvert2In);
	SPQOS_CHECK_ABORT;
	pexprConvert2In->Release();

	// (11) eliminate self comparisons
	CExpression *pexprSelfCompEliminated =
		PexprEliminateSelfComparison(mp, pexprInferredPreds);
	SPQOS_CHECK_ABORT;
	pexprInferredPreds->Release();

	// (12) remove duplicate AND/OR children
	CExpression *pexprDeduped =
		CExpressionUtils::PexprDedupChildren(mp, pexprSelfCompEliminated);
	SPQOS_CHECK_ABORT;
	pexprSelfCompEliminated->Release();

	// (13) factorize common expressions
	CExpression *pexprFactorized =
		CExpressionFactorizer::PexprFactorize(mp, pexprDeduped);
	SPQOS_CHECK_ABORT;
	pexprDeduped->Release();

	// (14) infer filters out of components of disjunctive filters
	CExpression *pexprPrefiltersExtracted =
		CExpressionFactorizer::PexprExtractInferredFilters(mp, pexprFactorized);
	SPQOS_CHECK_ABORT;
	pexprFactorized->Release();

	// (15) pre-process ordered agg functions
	CExpression *pexprOrderedAggPreprocessed =
		COrderedAggPreprocessor::PexprPreprocess(mp, pexprPrefiltersExtracted);
	SPQOS_CHECK_ABORT;
	pexprPrefiltersExtracted->Release();

	// (16) pre-process window functions
	CExpression *pexprWindowPreprocessed =
		CWindowPreprocessor::PexprPreprocess(mp, pexprOrderedAggPreprocessed);
	SPQOS_CHECK_ABORT;
	pexprOrderedAggPreprocessed->Release();

	// (17) eliminate unused computed columns
	CExpression *pexprNoUnusedPrEl = PexprPruneUnusedComputedCols(
		mp, pexprWindowPreprocessed, pcrsOutputAndOrderCols);
	SPQOS_CHECK_ABORT;
	pexprWindowPreprocessed->Release();

	// (18) normalize expression
	CExpression *pexprNormalized1 =
		CNormalizer::PexprNormalize(mp, pexprNoUnusedPrEl);
	SPQOS_CHECK_ABORT;
	pexprNoUnusedPrEl->Release();

	// (19) transform outer join into inner join whenever possible
	CExpression *pexprLOJToIJ = PexprOuterJoinToInnerJoin(mp, pexprNormalized1);
	SPQOS_CHECK_ABORT;
	pexprNormalized1->Release();

	// (20) collapse cascaded inner and left outer joins
	CExpression *pexprCollapsed = PexprCollapseJoins(mp, pexprLOJToIJ);
	SPQOS_CHECK_ABORT;
	pexprLOJToIJ->Release();

	// (21) after transforming outer joins to inner joins, we may be able to generate more predicates from constraints
	CExpression *pexprWithPreds =
		PexprAddPredicatesFromConstraints(mp, pexprCollapsed);
	SPQOS_CHECK_ABORT;
	pexprCollapsed->Release();

	// (22) eliminate empty subtrees
	CExpression *pexprPruned = PexprPruneEmptySubtrees(mp, pexprWithPreds);
	SPQOS_CHECK_ABORT;
	pexprWithPreds->Release();

	// (23) collapse cascade of projects
	CExpression *pexprCollapsedProjects =
		PexprCollapseProjects(mp, pexprPruned);
	SPQOS_CHECK_ABORT;
	pexprPruned->Release();

	// (24) insert dummy project when the scalar subquery is under a project and returns an outer reference
	CExpression *pexprSubquery = PexprProjBelowSubquery(
		mp, pexprCollapsedProjects, false /* fUnderPrList */);
	SPQOS_CHECK_ABORT;
	pexprCollapsedProjects->Release();

	// (25) reorder the children of scalar cmp operator to ensure that left child is scalar ident and right child is scalar const
	CExpression *pexrReorderedScalarCmpChildren =
		PexprReorderScalarCmpChildren(mp, pexprSubquery);
	SPQOS_CHECK_ABORT;
	pexprSubquery->Release();

	// (26) rewrite IN subquery to EXIST subquery with a predicate
	CExpression *pexprExistWithPredFromINSubq =
		PexprExistWithPredFromINSubq(mp, pexrReorderedScalarCmpChildren);
	SPQOS_CHECK_ABORT;
	pexrReorderedScalarCmpChildren->Release();


	// (27) swap logical select over logical project
	CExpression *pexprTransposeSelectAndProject =
		PexprTransposeSelectAndProject(mp, pexprExistWithPredFromINSubq);
	pexprExistWithPredFromINSubq->Release();

	// (28) normalize expression again
	CExpression *pexprNormalized2 =
		CNormalizer::PexprNormalize(mp, pexprTransposeSelectAndProject);
	SPQOS_CHECK_ABORT;
	pexprTransposeSelectAndProject->Release();

	return pexprNormalized2;
}

// EOF
