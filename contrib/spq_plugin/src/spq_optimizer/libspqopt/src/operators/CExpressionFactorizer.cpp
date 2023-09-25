//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal, Inc.
//
//	@filename:
//		CExpressionFactorizer.cpp
//
//	@doc:
//		,
//
//	@owner:
//		Utility functions for expression factorization
//
//	@test:
//
//---------------------------------------------------------------------------

#include "spqopt/operators/CExpressionFactorizer.h"

#include "spqopt/base/CColRefSetIter.h"
#include "spqopt/base/CColRefTable.h"
#include "spqopt/base/CConstraintInterval.h"
#include "spqopt/base/CFunctionProp.h"
#include "spqopt/base/CUtils.h"
#include "spqopt/exception.h"
#include "spqopt/mdcache/CMDAccessor.h"
#include "spqopt/operators/CExpressionUtils.h"
#include "spqopt/operators/CPredicateUtils.h"
#include "naucrates/md/IMDFunction.h"
#include "naucrates/md/IMDScalarOp.h"
#include "naucrates/md/IMDType.h"
#include "naucrates/statistics/CStatistics.h"

using namespace spqopt;

//---------------------------------------------------------------------------
//	@function:
//		CExpressionFactorizer::PexprProcessDisjDescendents
//
//	@doc:
//		Visitor-like function to process descendents that are OR operators.
//
//
//---------------------------------------------------------------------------
CExpression *
CExpressionFactorizer::PexprProcessDisjDescendents(
	CMemoryPool *mp, CExpression *pexpr,
	CExpression *pexprLowestLogicalAncestor, PexprProcessDisj pfnpepdFunction)
{
	// protect against stack overflow during recursion
	SPQOS_CHECK_STACK_SIZE;
	SPQOS_ASSERT(NULL != mp);
	SPQOS_ASSERT(NULL != pexpr);

	CExpression *pexprLogicalAncestor = pexprLowestLogicalAncestor;
	if (pexpr->Pop()->FLogical())
	{
		pexprLogicalAncestor = pexpr;
	}

	if (CPredicateUtils::FOr(pexpr))
	{
		return (*pfnpepdFunction)(mp, pexpr, pexprLogicalAncestor);
	}

	// recursively process children
	const ULONG arity = pexpr->Arity();
	CExpressionArray *pdrspqexprChildren = SPQOS_NEW(mp) CExpressionArray(mp);
	for (ULONG ul = 0; ul < arity; ul++)
	{
		CExpression *pexprChild = PexprProcessDisjDescendents(
			mp, (*pexpr)[ul], pexprLogicalAncestor, pfnpepdFunction);
		pdrspqexprChildren->Append(pexprChild);
	}

	COperator *pop = pexpr->Pop();
	pop->AddRef();
	return SPQOS_NEW(mp) CExpression(mp, pop, pdrspqexprChildren);
}

//---------------------------------------------------------------------------
//	@function:
//		CExpressionFactorizer::AddFactor
//
//	@doc:
//		Helper for adding a given expression either to the given factors
//		array or to a residuals array
//
//
//---------------------------------------------------------------------------
void
CExpressionFactorizer::AddFactor(CMemoryPool *mp, CExpression *pexpr,
								 CExpressionArray *pdrspqexprFactors,
								 CExpressionArray *pdrspqexprResidual,
								 ExprMap *pexprmapFactors,
								 ULONG
#ifdef SPQOS_DEBUG
									 ulDisjuncts
#endif	// SPQOS_DEBUG
)
{
	ULONG *pul = pexprmapFactors->Find(pexpr);
	SPQOS_ASSERT_IMP(NULL != pul, ulDisjuncts == *pul);

	if (NULL != pul)
	{
		// check if factor already exist in factors array
		BOOL fFound = false;
		const ULONG size = pdrspqexprFactors->Size();
		for (ULONG ul = 0; !fFound && ul < size; ul++)
		{
			fFound = CUtils::Equals(pexpr, (*pdrspqexprFactors)[ul]);
		}

		if (!fFound)
		{
			pexpr->AddRef();
			pdrspqexprFactors->Append(pexpr);
		}

		// replace factor with constant True in the residuals array
		pdrspqexprResidual->Append(
			CPredicateUtils::PexprConjunction(mp, NULL /*pdrspqexpr*/));
	}
	else
	{
		pexpr->AddRef();
		pdrspqexprResidual->Append(pexpr);
	}
}


//---------------------------------------------------------------------------
//	@function:
//		CExpressionFactorizer::PexprmapFactors
//
//	@doc:
//		Helper for building a factors map
//
//		Example:
//		input:  (A=B AND B=C AND B>0) OR (A=B AND B=C AND A>0)
//		output: [(A=B, 2), (B=C, 2)]
//
//---------------------------------------------------------------------------
CExpressionFactorizer::ExprMap *
CExpressionFactorizer::PexprmapFactors(CMemoryPool *mp, CExpression *pexpr)
{
	SPQOS_ASSERT(CPredicateUtils::FOr(pexpr) &&
				"input must be an OR expression");

	// a global (expression -> count) map
	ExprMap *pexprmapGlobal = SPQOS_NEW(mp) ExprMap(mp);

	// factors map
	ExprMap *pexprmapFactors = SPQOS_NEW(mp) ExprMap(mp);

	// iterate over child disjuncts;
	// if a disjunct is an AND tree, iterate over its children
	const ULONG ulDisjuncts = pexpr->Arity();
	for (ULONG ulOuter = 0; ulOuter < ulDisjuncts; ulOuter++)
	{
		CExpression *pexprDisj = (*pexpr)[ulOuter];

		CExpressionArray *pdrspqexpr = SPQOS_NEW(mp) CExpressionArray(mp);
		pexprDisj->AddRef();
		pdrspqexpr->Append(pexprDisj);

		if (CPredicateUtils::FAnd(pexprDisj))
		{
			pdrspqexpr->Release();
			pexprDisj->PdrgPexpr()->AddRef();
			pdrspqexpr = pexprDisj->PdrgPexpr();
		}

		const ULONG size = pdrspqexpr->Size();
		for (ULONG ulInner = 0; ulInner < size; ulInner++)
		{
			CExpression *pexprConj = (*pdrspqexpr)[ulInner];
			ULONG *pul = pexprmapGlobal->Find(pexprConj);
			if (NULL == pul)
			{
				pexprConj->AddRef();
				(void) pexprmapGlobal->Insert(pexprConj, SPQOS_NEW(mp) ULONG(1));
			}
			else
			{
				(*pul)++;
			}

			pul = pexprmapGlobal->Find(pexprConj);
			if (*pul == ulDisjuncts)
			{
				// reached the count of initial disjuncts, add expression to factors map
				pexprConj->AddRef();
				(void) pexprmapFactors->Insert(pexprConj,
											   SPQOS_NEW(mp) ULONG(ulDisjuncts));
			}
		}
		pdrspqexpr->Release();
	}
	pexprmapGlobal->Release();

	return pexprmapFactors;
}

//---------------------------------------------------------------------------
//	@function:
//		CExpressionFactorizer::PexprFactorizeDisj
//
//	@doc:
//		Factorize common expressions in an OR tree;
//		the result is a conjunction of factors and a residual Or tree
//
//		Example:
//		input:  [(A=B AND C>0) OR (A=B AND A>0) OR (A=B AND B>0)]
//		output: [(A=B) AND (C>0 OR A>0 OR B>0)]
//
//---------------------------------------------------------------------------
CExpression *
CExpressionFactorizer::PexprFactorizeDisj(CMemoryPool *mp, CExpression *pexpr,
										  CExpression *	 //pexprLogical
)
{
	SPQOS_ASSERT(CPredicateUtils::FOr(pexpr) &&
				"input must be an OR expression");

	// build factors map
	ExprMap *pexprmapFactors = PexprmapFactors(mp, pexpr);

	CExpressionArray *pdrspqexprResidual = SPQOS_NEW(mp) CExpressionArray(mp);
	CExpressionArray *pdrspqexprFactors = SPQOS_NEW(mp) CExpressionArray(mp);

	// iterate over child expressions and factorize them
	const ULONG ulDisjuncts = pexpr->Arity();
	for (ULONG ulOuter = 0; ulOuter < ulDisjuncts; ulOuter++)
	{
		CExpression *pexprDisj = (*pexpr)[ulOuter];
		if (CPredicateUtils::FAnd(pexprDisj))
		{
			CExpressionArray *pdrspqexprConjuncts =
				SPQOS_NEW(mp) CExpressionArray(mp);
			const ULONG size = pexprDisj->Arity();
			for (ULONG ulInner = 0; ulInner < size; ulInner++)
			{
				CExpression *pexprConj = (*pexprDisj)[ulInner];
				AddFactor(mp, pexprConj, pdrspqexprFactors, pdrspqexprConjuncts,
						  pexprmapFactors, ulDisjuncts);
			}

			if (0 < pdrspqexprConjuncts->Size())
			{
				pdrspqexprResidual->Append(
					CPredicateUtils::PexprConjunction(mp, pdrspqexprConjuncts));
			}
			else
			{
				pdrspqexprConjuncts->Release();
			}
		}
		else
		{
			AddFactor(mp, pexprDisj, pdrspqexprFactors, pdrspqexprResidual,
					  pexprmapFactors, ulDisjuncts);
		}
	}
	pexprmapFactors->Release();

	if (0 < pdrspqexprResidual->Size())
	{
		// residual becomes a new factor
		pdrspqexprFactors->Append(
			CPredicateUtils::PexprDisjunction(mp, pdrspqexprResidual));
	}
	else
	{
		// no residuals, release created array
		pdrspqexprResidual->Release();
	}

	// return a conjunction of all factors
	return CPredicateUtils::PexprConjunction(mp, pdrspqexprFactors);
}


//---------------------------------------------------------------------------
//	@function:
//		CExpressionFactorizer::PexprDiscoverFactors
//
//	@doc:
//		Discover common factors in scalar expressions
//
//
//---------------------------------------------------------------------------
CExpression *
CExpressionFactorizer::PexprDiscoverFactors(CMemoryPool *mp, CExpression *pexpr)
{
	return PexprProcessDisjDescendents(
		mp, pexpr, NULL /*pexprLowestLogicalAncestor*/, PexprFactorizeDisj);
}


//---------------------------------------------------------------------------
//	@function:
//		CExpressionFactorizer::PexprFactorize
//
//	@doc:
//		Factorize common scalar expressions
//
//
//---------------------------------------------------------------------------
CExpression *
CExpressionFactorizer::PexprFactorize(CMemoryPool *mp, CExpression *pexpr)
{
	SPQOS_ASSERT(NULL != mp);
	SPQOS_ASSERT(NULL != pexpr);

	CExpression *pexprFactorized = PexprDiscoverFactors(mp, pexpr);

	// factorization might reveal unnested AND/OR
	CExpression *pexprUnnested =
		CExpressionUtils::PexprUnnest(mp, pexprFactorized);
	pexprFactorized->Release();

	// eliminate duplicate AND/OR children
	CExpression *pexprDeduped =
		CExpressionUtils::PexprDedupChildren(mp, pexprUnnested);
	pexprUnnested->Release();

	return pexprDeduped;
}

//---------------------------------------------------------------------------
//	@function:
//		CExpressionFactorizer::PcrsUsedByPushableScalar
//
//	@doc:
//		If the given expression is a scalar that can be pushed, it returns
//		the set of used columns.
//
//---------------------------------------------------------------------------
CColRefSet *
CExpressionFactorizer::PcrsUsedByPushableScalar(CExpression *pexpr)
{
	if (!pexpr->Pop()->FScalar())
	{
		return NULL;
	}

	if (0 < pexpr->DeriveDefinedColumns()->Size() ||
		pexpr->DeriveHasSubquery() ||
		IMDFunction::EfsVolatile ==
			pexpr->DeriveScalarFunctionProperties()->Efs())
	{
		return NULL;
	}

	return pexpr->DeriveUsedColumns();
}

//---------------------------------------------------------------------------
//	@function:
//		CExpressionFactorizer::FOpSourceIdOrComputedColumn
//
//	@doc:
//		If the given expression is a non volatile scalar expression using table columns
//		created by the same operator (or using the same computed column)
//		return true and set ulOpSourceId to the id of the operator that created those
//		columns (set *ppcrComputedColumn to the computed column used),
//		otherwise return false.
//
//---------------------------------------------------------------------------
BOOL
CExpressionFactorizer::FOpSourceIdOrComputedColumn(CExpression *pexpr,
												   ULONG *ulOpSourceId,
												   CColRef **ppcrComputedColumn)
{
	SPQOS_ASSERT(NULL != pexpr);
	SPQOS_ASSERT(NULL != ulOpSourceId);
	SPQOS_ASSERT(NULL != ppcrComputedColumn);

	*ulOpSourceId = spqos::ulong_max;
	*ppcrComputedColumn = NULL;

	CColRefSet *pcrsUsed = PcrsUsedByPushableScalar(pexpr);
	if (NULL == pcrsUsed || 0 == pcrsUsed->Size())
	{
		return false;
	}

	ULONG ulComputedOpSourceId = spqos::ulong_max;
	CColRefSetIter crsi(*pcrsUsed);
	while (crsi.Advance())
	{
		CColRef *colref = crsi.Pcr();
		if (CColRef::EcrtTable != colref->Ecrt())
		{
			if (NULL == *ppcrComputedColumn)
			{
				*ppcrComputedColumn = colref;
			}
			else if (colref != *ppcrComputedColumn)
			{
				return false;
			}

			continue;
		}
		else if (NULL != *ppcrComputedColumn)
		{
			// don't allow a mix of computed columns and table columns
			return false;
		}

		const CColRefTable *pcrTable = CColRefTable::PcrConvert(colref);
		if (spqos::ulong_max == ulComputedOpSourceId)
		{
			ulComputedOpSourceId = pcrTable->UlSourceOpId();
		}
		else if (ulComputedOpSourceId != pcrTable->UlSourceOpId())
		{
			// expression refers to columns coming from different operators
			return false;
		}
	}
	*ulOpSourceId = ulComputedOpSourceId;

	return true;
}

//---------------------------------------------------------------------------
//	@function:
//		CExpressionFactorizer::PdrgPdrspqexprDisjunctArrayForSourceId
//
//	@doc:
//		Find the array of expression arrays corresponding to the given
//		operator source id in the given source to array position map
//		or construct a new array and add it to the map.
//
//---------------------------------------------------------------------------
CExpressionArrays *
CExpressionFactorizer::PdrgPdrspqexprDisjunctArrayForSourceId(
	CMemoryPool *mp, SourceToArrayPosMap *psrc2array, BOOL fAllowNewSources,
	ULONG ulOpSourceId)
{
	SPQOS_ASSERT(NULL != psrc2array);
	CExpressionArrays *pdrspqdrspqexpr = psrc2array->Find(&ulOpSourceId);

	// if there is no entry, we start recording expressions that will become disjuncts
	// corresponding to the source operator we are considering
	if (NULL == pdrspqdrspqexpr)
	{
		// checking this flag allows us to disable adding new entries: if a source operator
		// does not appear in the first disjunct, there is no need to add it later since it
		// will not cover the entire disjunction
		if (!fAllowNewSources)
		{
			return NULL;
		}
		pdrspqdrspqexpr = SPQOS_NEW(mp) CExpressionArrays(mp);
#ifdef SPQOS_DEBUG
		BOOL fInserted =
#endif	// SPQOS_DEBUG
			psrc2array->Insert(SPQOS_NEW(mp) ULONG(ulOpSourceId), pdrspqdrspqexpr);
		SPQOS_ASSERT(fInserted);
	}

	return pdrspqdrspqexpr;
}

//---------------------------------------------------------------------------
//	@function:
//		CExpressionFactorizer::PdrgPdrspqexprDisjunctArrayForColumn
//
//	@doc:
// 		Find the array of expression arrays corresponding to the given
// 		column in the given column to array position map
// 		or construct a new array and add it to the map.
//
//---------------------------------------------------------------------------
CExpressionArrays *
CExpressionFactorizer::PdrgPdrspqexprDisjunctArrayForColumn(
	CMemoryPool *mp, ColumnToArrayPosMap *pcol2array, BOOL fAllowNewSources,
	CColRef *colref)
{
	SPQOS_ASSERT(NULL != pcol2array);
	CExpressionArrays *pdrspqdrspqexpr = pcol2array->Find(colref);

	// if there is no entry, we start recording expressions that will become disjuncts
	// corresponding to the computed column we are considering
	if (NULL == pdrspqdrspqexpr)
	{
		// checking this flag allows us to disable adding new entries: if a column
		// does not appear in the first disjunct, there is no need to add it later since it
		// will not cover the entire disjunction
		if (!fAllowNewSources)
		{
			return NULL;
		}
		pdrspqdrspqexpr = SPQOS_NEW(mp) CExpressionArrays(mp);
#ifdef SPQOS_DEBUG
		BOOL fInserted =
#endif	// SPQOS_DEBUG
			pcol2array->Insert(colref, pdrspqdrspqexpr);
		SPQOS_ASSERT(fInserted);
	}

	return pdrspqdrspqexpr;
}

//---------------------------------------------------------------------------
//	@function:
//		CExpressionFactorizer::RecordComparison
//
//	@doc:
//		If the given expression is a table column to constant comparison,
//		record it in the 'psrc2array' map.
//		If 'fAllowNewSources' is false, no new entries can be created in the
//		map. 'ulPosition' indicates the position in the entry where to add
//		the expression.
//
//---------------------------------------------------------------------------
void
CExpressionFactorizer::StoreBaseOpToColumnExpr(
	CMemoryPool *mp, CExpression *pexpr, SourceToArrayPosMap *psrc2array,
	ColumnToArrayPosMap *pcol2array, const CColRefSet *pcrsProducedByChildren,
	BOOL fAllowNewSources, ULONG ulPosition)
{
	ULONG ulOpSourceId;
	CColRef *pcrComputed = NULL;
	if (!FOpSourceIdOrComputedColumn(pexpr, &ulOpSourceId, &pcrComputed))
	{
		return;
	}

	CExpressionArrays *pdrspqdrspqexpr = NULL;

	if (spqos::ulong_max != ulOpSourceId)
	{
		pdrspqdrspqexpr = PdrgPdrspqexprDisjunctArrayForSourceId(
			mp, psrc2array, fAllowNewSources, ulOpSourceId);
	}
	else
	{
		SPQOS_ASSERT(NULL != pcrComputed);
		if (NULL != pcrsProducedByChildren &&
			pcrsProducedByChildren->FMember(pcrComputed))
		{
			// do not create filters for columns produced by the scalar tree of
			// a logical operator immediately under the current logical operator
			return;
		}

		pdrspqdrspqexpr = PdrgPdrspqexprDisjunctArrayForColumn(
			mp, pcol2array, fAllowNewSources, pcrComputed);
	}

	if (NULL == pdrspqdrspqexpr)
	{
		return;
	}

	CExpressionArray *pdrspqexpr = NULL;
	// there are only two cases we need to consider
	// the first one is that we found the current source operator in all previous disjuncts
	// and now we are starting a new sub-array for a new disjunct
	if (ulPosition == pdrspqdrspqexpr->Size())
	{
		pdrspqexpr = SPQOS_NEW(mp) CExpressionArray(mp);
		pdrspqdrspqexpr->Append(pdrspqexpr);
	}
	// the second case is that we found additional conjuncts for the current source operator
	// inside the current disjunct
	else if (ulPosition == pdrspqdrspqexpr->Size() - 1)
	{
		pdrspqexpr = (*pdrspqdrspqexpr)[ulPosition];
	}
	// otherwise, this source operator is not covered by all disjuncts, so there is no need to
	// keep recording it since it will not lead to a valid pre-filter
	else
	{
		return;
	}

	pexpr->AddRef();
	pdrspqexpr->Append(pexpr);
}

//---------------------------------------------------------------------------
//	@function:
//		CExpressionFactorizer::PexprAddInferredFilters
//
//	@doc:
//		Create a conjunction of the given expression and inferred filters constructed out
//		of the given map.
//
//---------------------------------------------------------------------------
CExpression *
CExpressionFactorizer::PexprAddInferredFilters(CMemoryPool *mp,
											   CExpression *pexpr,
											   SourceToArrayPosMap *psrc2array,
											   ColumnToArrayPosMap *pcol2array)
{
	SPQOS_ASSERT(NULL != pexpr);
	SPQOS_ASSERT(CPredicateUtils::FOr(pexpr));
	SPQOS_ASSERT(NULL != psrc2array);

	SourceToArrayPosMapIter src2arrayIter(psrc2array);
	CExpressionArray *pdrspqexprPrefilters = SPQOS_NEW(mp) CExpressionArray(mp);
	pexpr->AddRef();
	pdrspqexprPrefilters->Append(pexpr);
	const ULONG ulDisjChildren = pexpr->Arity();

	while (src2arrayIter.Advance())
	{
		AddInferredFiltersFromArray(mp, src2arrayIter.Value(), ulDisjChildren,
									pdrspqexprPrefilters);
	}

	ColumnToArrayPosMapIter col2arrayIter(pcol2array);
	while (col2arrayIter.Advance())
	{
		AddInferredFiltersFromArray(mp, col2arrayIter.Value(), ulDisjChildren,
									pdrspqexprPrefilters);
	}

	return CPredicateUtils::PexprConjunction(mp, pdrspqexprPrefilters);
}

//---------------------------------------------------------------------------
//	@function:
//		CExpressionFactorizer::PexprAddInferredFilters
//
//	@doc:
// 		Construct a filter based on the expressions from 'pdrspqdrspqexpr'
// 		and add to the array 'pdrspqexprPrefilters'.
//
//---------------------------------------------------------------------------
void
CExpressionFactorizer::AddInferredFiltersFromArray(
	CMemoryPool *mp, const CExpressionArrays *pdrspqdrspqexpr,
	ULONG ulDisjChildrenLength, CExpressionArray *pdrspqexprInferredFilters)
{
	const ULONG ulEntryLength =
		(pdrspqdrspqexpr == NULL) ? 0 : pdrspqdrspqexpr->Size();
	if (ulEntryLength == ulDisjChildrenLength)
	{
		CExpressionArray *pdrspqexprDisjuncts =
			SPQOS_NEW(mp) CExpressionArray(mp);
		for (ULONG ul = 0; ul < ulEntryLength; ++ul)
		{
			(*pdrspqdrspqexpr)[ul]->AddRef();
			CExpression *pexprConj =
				CPredicateUtils::PexprConjunction(mp, (*pdrspqdrspqexpr)[ul]);
			pdrspqexprDisjuncts->Append(pexprConj);
		}
		if (0 < pdrspqexprDisjuncts->Size())
		{
			pdrspqexprInferredFilters->Append(
				CPredicateUtils::PexprDisjunction(mp, pdrspqexprDisjuncts));
		}
	}
}

//---------------------------------------------------------------------------
//	@function:
//		CExpressionFactorizer::PcrsColumnsProducedByChildren
//
//	@doc:
//		Returns the set of columns produced by the scalar trees of the given expression's
//		children
//
//---------------------------------------------------------------------------
CColRefSet *
CExpressionFactorizer::PcrsColumnsProducedByChildren(CMemoryPool *mp,
													 CExpression *pexpr)
{
	SPQOS_ASSERT(NULL != pexpr);
	const ULONG arity = pexpr->Arity();
	CColRefSet *pcrs = SPQOS_NEW(mp) CColRefSet(mp);
	for (ULONG ulTop = 0; ulTop < arity; ulTop++)
	{
		CExpression *pexprChild = (*pexpr)[ulTop];
		const ULONG ulChildArity = pexprChild->Arity();
		for (ULONG ulBelowChild = 0; ulBelowChild < ulChildArity;
			 ulBelowChild++)
		{
			CExpression *pexprGrandChild = (*pexprChild)[ulBelowChild];
			if (pexprGrandChild->Pop()->FScalar())
			{
				CColRefSet *pcrsChildDefined =
					pexprGrandChild->DeriveDefinedColumns();
				pcrs->Include(pcrsChildDefined);
			}
		}
	}

	return pcrs;
}

//---------------------------------------------------------------------------
//	@function:
//		CExpressionFactorizer::PexprExtractInferredFiltersFromDisj
//
//	@doc:
//		Compute disjunctive inferred filters that can be pushed to the column creators
//
//---------------------------------------------------------------------------
CExpression *
CExpressionFactorizer::PexprExtractInferredFiltersFromDisj(
	CMemoryPool *mp, CExpression *pexpr,
	CExpression *pexprLowestLogicalAncestor)
{
	SPQOS_ASSERT(NULL != mp);
	SPQOS_ASSERT(NULL != pexpr);
	SPQOS_ASSERT(CPredicateUtils::FOr(pexpr) &&
				"input must be an OR expression");
	SPQOS_ASSERT(NULL != pexprLowestLogicalAncestor);

	const ULONG arity = pexpr->Arity();
	SPQOS_ASSERT(2 <= arity);

	// for each source operator, create a map entry which, for every disjunct,
	// has the array of comparisons using that operator
	// we initialize the entries with operators appearing in the first disjunct
	SourceToArrayPosMap *psrc2array = SPQOS_NEW(mp) SourceToArrayPosMap(mp);

	// create a similar map for computed columns
	ColumnToArrayPosMap *pcol2array = SPQOS_NEW(mp) ColumnToArrayPosMap(mp);

	CColRefSet *pcrsProducedByChildren = NULL;
	if (COperator::EopLogicalSelect ==
		pexprLowestLogicalAncestor->Pop()->Eopid())
	{
		pcrsProducedByChildren =
			PcrsColumnsProducedByChildren(mp, pexprLowestLogicalAncestor);
	}

	for (ULONG ul = 0; ul < arity; ul++)
	{
		CExpression *pexprCurrent = (*pexpr)[ul];
		BOOL fFirst = 0 == ul;
		if (CPredicateUtils::FAnd(pexprCurrent))
		{
			const ULONG ulAndArity = pexprCurrent->Arity();
			for (ULONG ulAnd = 0; ulAnd < ulAndArity; ++ulAnd)
			{
				StoreBaseOpToColumnExpr(mp, (*pexprCurrent)[ulAnd], psrc2array,
										pcol2array, pcrsProducedByChildren,
										fFirst,	 // fAllowNewSources
										ul);
			}
		}
		else
		{
			StoreBaseOpToColumnExpr(mp, pexprCurrent, psrc2array, pcol2array,
									pcrsProducedByChildren,
									fFirst /*fAllowNewSources*/, ul);
		}

		if (fFirst && 0 == psrc2array->Size() && 0 == pcol2array->Size())
		{
			psrc2array->Release();
			pcol2array->Release();
			CRefCount::SafeRelease(pcrsProducedByChildren);
			pexpr->AddRef();
			return pexpr;
		}
		SPQOS_CHECK_ABORT;
	}
	CExpression *pexprWithPrefilters =
		PexprAddInferredFilters(mp, pexpr, psrc2array, pcol2array);
	psrc2array->Release();
	pcol2array->Release();
	CRefCount::SafeRelease(pcrsProducedByChildren);
	CExpression *pexprDeduped =
		CExpressionUtils::PexprDedupChildren(mp, pexprWithPrefilters);
	pexprWithPrefilters->Release();

	return pexprDeduped;
}

//---------------------------------------------------------------------------
//	@function:
//		CExpressionFactorizer::PexprExtractInferredFilters
//
//	@doc:
//		Compute disjunctive pre-filters that can be pushed to the column creators.
//		These inferred filters need to "cover" all the disjuncts with expressions
//		coming from the same source.
//
//		For instance, out of the predicate
//		((sale_type = 's'::text AND dyear = 2001 AND year_total > 0::numeric) OR
//		(sale_type = 's'::text AND dyear = 2002) OR
//		(sale_type = 'w'::text AND dmoy = 7 AND year_total > 0::numeric) OR
//		(sale_type = 'w'::text AND dyear = 2002 AND dmoy = 7))
//
//		we can infer the filter
//		dyear=2001 OR dyear=2002 OR dmoy=7 OR (dyear=2002 AND dmoy=7)
//
//		which can later be pushed down by the normalizer
//
//---------------------------------------------------------------------------
CExpression *
CExpressionFactorizer::PexprExtractInferredFilters(CMemoryPool *mp,
												   CExpression *pexpr)
{
	return PexprProcessDisjDescendents(mp, pexpr,
									   NULL /*pexprLowestLogicalAncestor*/,
									   PexprExtractInferredFiltersFromDisj);
}


// EOF
