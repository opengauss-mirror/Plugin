//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright 2012 EMC Corp.
//
//	@filename:
//		CNormalizer.cpp
//
//	@doc:
//		Implementation of expression tree normalizer
//---------------------------------------------------------------------------

#include "spqopt/operators/CNormalizer.h"

#include "spqos/base.h"
#include "spqos/memory/CAutoMemoryPool.h"

#include "spqopt/base/CUtils.h"
#include "spqopt/operators/CLogical.h"
#include "spqopt/operators/CLogicalConstTableGet.h"
#include "spqopt/operators/CLogicalInnerJoin.h"
#include "spqopt/operators/CLogicalLeftOuterCorrelatedApply.h"
#include "spqopt/operators/CLogicalLeftOuterJoin.h"
#include "spqopt/operators/CLogicalNAryJoin.h"
#include "spqopt/operators/CLogicalProject.h"
#include "spqopt/operators/CLogicalSelect.h"
#include "spqopt/operators/CLogicalSequenceProject.h"
#include "spqopt/operators/CLogicalSetOp.h"
#include "spqopt/operators/CPredicateUtils.h"
#include "spqopt/operators/CScalarNAryJoinPredList.h"
#include "spqopt/operators/CScalarProjectElement.h"
#include "spqopt/operators/CScalarProjectList.h"

using namespace spqopt;


// initialization of push through handler
const CNormalizer::SPushThru CNormalizer::m_rspqt[] = {
	{COperator::EopLogicalSelect, PushThruSelect},
	{COperator::EopLogicalProject, PushThruUnaryWithScalarChild},
	{COperator::EopLogicalSequenceProject, PushThruSeqPrj},
	{COperator::EopLogicalGbAgg, PushThruUnaryWithScalarChild},
	{COperator::EopLogicalCTEAnchor, PushThruUnaryWithoutScalarChild},
	{COperator::EopLogicalUnion, PushThruSetOp},
	{COperator::EopLogicalUnionAll, PushThruSetOp},
	{COperator::EopLogicalIntersect, PushThruSetOp},
	{COperator::EopLogicalIntersectAll, PushThruSetOp},
	{COperator::EopLogicalDifference, PushThruSetOp},
	{COperator::EopLogicalDifferenceAll, PushThruSetOp},
	{COperator::EopLogicalInnerJoin, PushThruJoin},
	{COperator::EopLogicalNAryJoin, PushThruJoin},
	{COperator::EopLogicalInnerApply, PushThruJoin},
	{COperator::EopLogicalInnerCorrelatedApply, PushThruJoin},
	{COperator::EopLogicalLeftOuterJoin, PushThruJoin},
	{COperator::EopLogicalLeftOuterApply, PushThruJoin},
	{COperator::EopLogicalLeftOuterCorrelatedApply, PushThruJoin},
	{COperator::EopLogicalLeftSemiApply, PushThruJoin},
	{COperator::EopLogicalLeftSemiApplyIn, PushThruJoin},
	{COperator::EopLogicalLeftSemiCorrelatedApplyIn, PushThruJoin},
	{COperator::EopLogicalLeftAntiSemiApply, PushThruJoin},
	{COperator::EopLogicalLeftAntiSemiApplyNotIn, PushThruJoin},
	{COperator::EopLogicalLeftAntiSemiCorrelatedApplyNotIn, PushThruJoin},
	{COperator::EopLogicalLeftSemiJoin, PushThruJoin},
};


//---------------------------------------------------------------------------
//	@function:
//		CNormalizer::FPushThruOuterChild
//
//	@doc:
//		Check if we should push predicates through expression's outer child
//
//
//---------------------------------------------------------------------------
BOOL
CNormalizer::FPushThruOuterChild(CExpression *pexprLogical)
{
	SPQOS_ASSERT(NULL != pexprLogical);

	COperator::EOperatorId op_id = pexprLogical->Pop()->Eopid();

	return COperator::EopLogicalLeftOuterJoin == op_id ||
		   COperator::EopLogicalLeftOuterApply == op_id ||
		   COperator::EopLogicalLeftOuterCorrelatedApply == op_id ||
		   CUtils::FLeftAntiSemiApply(pexprLogical->Pop()) ||
		   CUtils::FLeftSemiApply(pexprLogical->Pop());
}


//---------------------------------------------------------------------------
//	@function:
//		CNormalizer::FPushableThruSeqPrjChild
//
//	@doc:
//		Check if a predicate can be pushed through the child of a sequence
//		project expression
//
//---------------------------------------------------------------------------
BOOL
CNormalizer::FPushableThruSeqPrjChild(CExpression *pexprSeqPrj,
									  CExpression *pexprPred)
{
	SPQOS_ASSERT(NULL != pexprSeqPrj);
	SPQOS_ASSERT(NULL != pexprPred);
	SPQOS_ASSERT(CLogical::EopLogicalSequenceProject ==
				pexprSeqPrj->Pop()->Eopid());

	CDistributionSpec *pds =
		CLogicalSequenceProject::PopConvert(pexprSeqPrj->Pop())->Pds();

	BOOL fPushable = false;
	if (CDistributionSpec::EdtHashed == pds->Edt())
	{
		SPQOS_ASSERT(NULL ==
					CDistributionSpecHashed::PdsConvert(pds)->PdshashedEquiv());
		CAutoMemoryPool amp;
		CMemoryPool *mp = amp.Pmp();
		CColRefSet *pcrsUsed = pexprPred->DeriveUsedColumns();
		CColRefSet *pcrsPartCols = CUtils::PcrsExtractColumns(
			mp, CDistributionSpecHashed::PdsConvert(pds)->Pdrspqexpr());
		if (pcrsPartCols->ContainsAll(pcrsUsed))
		{
			// predicate is pushable if used columns are included in partition-by expression
			fPushable = true;
		}
		pcrsPartCols->Release();
	}

	return fPushable;
}


//---------------------------------------------------------------------------
//	@function:
//		CNormalizer::FPushable
//
//	@doc:
//		Check if a predicate can be pushed through a logical expression
//
//
//---------------------------------------------------------------------------
BOOL
CNormalizer::FPushable(CExpression *pexprLogical, CExpression *pexprPred)
{
	SPQOS_ASSERT(NULL != pexprLogical);
	SPQOS_ASSERT(NULL != pexprPred);

	// do not push through volatile functions below an aggregate
	// volatile functions can potentially give different results when they
	// are called so we don't want aggregate to use volatile function result
	// while it processes each row
	if (COperator::EopLogicalGbAgg == pexprLogical->Pop()->Eopid() &&
		(CPredicateUtils::FContainsVolatileFunction(pexprPred)))
	{
		return false;
	}


	CColRefSet *pcrsUsed = pexprPred->DeriveUsedColumns();
	CColRefSet *pcrsOutput = pexprLogical->DeriveOutputColumns();

	//	In case of a Union or UnionAll the predicate might get pushed
	//	to multiple children In such cases we will end up with duplicate
	//	CTEProducers having the same cte_id.
	return pcrsOutput->ContainsAll(pcrsUsed) &&
		   !CUtils::FHasCTEAnchor(pexprPred);
}


//---------------------------------------------------------------------------
//	@function:
//		CNormalizer::PexprRecursiveNormalize
//
//	@doc:
//		Call normalizer recursively on children array
//
//
//---------------------------------------------------------------------------
CExpression *
CNormalizer::PexprRecursiveNormalize(CMemoryPool *mp, CExpression *pexpr)
{
	SPQOS_ASSERT(NULL != pexpr);

	const ULONG arity = pexpr->Arity();
	CExpressionArray *pdrspqexpr = SPQOS_NEW(mp) CExpressionArray(mp);
	for (ULONG ul = 0; ul < arity; ul++)
	{
		CExpression *pexprChild = PexprNormalize(mp, (*pexpr)[ul]);
		pdrspqexpr->Append(pexprChild);
	}

	COperator *pop = pexpr->Pop();
	pop->AddRef();

	return SPQOS_NEW(mp) CExpression(mp, pop, pdrspqexpr);
}


//---------------------------------------------------------------------------
//	@function:
//		CNormalizer::SplitConjunct
//
//	@doc:
//		Split the given conjunct into pushable and unpushable predicates
//
//
//---------------------------------------------------------------------------
void
CNormalizer::SplitConjunct(CMemoryPool *mp, CExpression *pexpr,
						   CExpression *pexprConj,
						   CExpressionArray **ppdrspqexprPushable,
						   CExpressionArray **ppdrspqexprUnpushable)
{
	SPQOS_ASSERT(pexpr->Pop()->FLogical());
	SPQOS_ASSERT(pexprConj->Pop()->FScalar());
	SPQOS_ASSERT(NULL != ppdrspqexprPushable);
	SPQOS_ASSERT(NULL != ppdrspqexprUnpushable);

	// collect pushable predicates from given conjunct
	*ppdrspqexprPushable = SPQOS_NEW(mp) CExpressionArray(mp);
	*ppdrspqexprUnpushable = SPQOS_NEW(mp) CExpressionArray(mp);
	CExpressionArray *pdrspqexprConjuncts =
		CPredicateUtils::PdrspqexprConjuncts(mp, pexprConj);
	const ULONG size = pdrspqexprConjuncts->Size();
	for (ULONG ul = 0; ul < size; ul++)
	{
		CExpression *pexprScalar = (*pdrspqexprConjuncts)[ul];
		pexprScalar->AddRef();
		if (FPushable(pexpr, pexprScalar))
		{
			(*ppdrspqexprPushable)->Append(pexprScalar);
		}
		else
		{
			(*ppdrspqexprUnpushable)->Append(pexprScalar);
		}
	}
	pdrspqexprConjuncts->Release();
}


//---------------------------------------------------------------------------
//	@function:
//		CNormalizer::PushThruOuterChild
//
//	@doc:
//		Push scalar expression through left outer join children;
//		this only handles the case of a SELECT on top of LEFT OUTER JOIN;
//		pushing down join predicates is handled in PushThruJoin();
//		here, we push predicates of the top SELECT node through LEFT OUTER JOIN's
//		outer child
//
//---------------------------------------------------------------------------
void
CNormalizer::PushThruOuterChild(CMemoryPool *mp, CExpression *pexpr,
								CExpression *pexprConj,
								CExpression **ppexprResult)
{
	SPQOS_ASSERT(NULL != pexpr);
	SPQOS_ASSERT(FPushThruOuterChild(pexpr));
	SPQOS_ASSERT(NULL != pexprConj);
	SPQOS_ASSERT(NULL != ppexprResult);

	if (0 == pexpr->Arity())
	{
		// end recursion early for leaf patterns extracted from memo
		pexpr->AddRef();
		pexprConj->AddRef();
		*ppexprResult = CUtils::PexprSafeSelect(mp, pexpr, pexprConj);

		return;
	}

	CExpression *pexprOuter = (*pexpr)[0];
	CExpression *pexprInner = (*pexpr)[1];
	CExpression *pexprPred = (*pexpr)[2];

	CExpressionArray *pdrspqexprPushable = NULL;
	CExpressionArray *pdrspqexprUnpushable = NULL;
	SplitConjunct(mp, pexprOuter, pexprConj, &pdrspqexprPushable,
				  &pdrspqexprUnpushable);

	if (0 < pdrspqexprPushable->Size())
	{
		pdrspqexprPushable->AddRef();
		CExpression *pexprNewConj =
			CPredicateUtils::PexprConjunction(mp, pdrspqexprPushable);

		// create a new select node on top of the outer child
		pexprOuter->AddRef();
		CExpression *pexprNewSelect = SPQOS_NEW(mp) CExpression(
			mp, SPQOS_NEW(mp) CLogicalSelect(mp), pexprOuter, pexprNewConj);

		// push predicate through the new select to create a new outer child
		CExpression *pexprNewOuter = NULL;
		PushThru(mp, pexprNewSelect, pexprNewConj, &pexprNewOuter);
		pexprNewSelect->Release();

		// create a new outer join using the new outer child and the new inner child
		COperator *pop = pexpr->Pop();
		pop->AddRef();
		pexprInner->AddRef();
		pexprPred->AddRef();
		CExpression *pexprNew = SPQOS_NEW(mp)
			CExpression(mp, pop, pexprNewOuter, pexprInner, pexprPred);

		// call push down predicates on the new outer join
		CExpression *pexprConstTrue =
			CUtils::PexprScalarConstBool(mp, true /*value*/);
		PushThru(mp, pexprNew, pexprConstTrue, ppexprResult);
		pexprConstTrue->Release();
		pexprNew->Release();
	}

	if (0 < pdrspqexprUnpushable->Size())
	{
		CExpression *pexprOuterJoin = pexpr;
		if (0 < pdrspqexprPushable->Size())
		{
			pexprOuterJoin = *ppexprResult;
			SPQOS_ASSERT(NULL != pexprOuterJoin);
		}

		// call push down on the outer join predicates
		CExpression *pexprNew = NULL;
		CExpression *pexprConstTrue =
			CUtils::PexprScalarConstBool(mp, true /*value*/);
		PushThru(mp, pexprOuterJoin, pexprConstTrue, &pexprNew);
		if (pexprOuterJoin != pexpr)
		{
			pexprOuterJoin->Release();
		}
		pexprConstTrue->Release();

		// create a SELECT on top of the new outer join
		pdrspqexprUnpushable->AddRef();
		*ppexprResult = PexprSelect(mp, pexprNew, pdrspqexprUnpushable);
	}

	pdrspqexprPushable->Release();
	pdrspqexprUnpushable->Release();
}


//---------------------------------------------------------------------------
//	@function:
//		CNormalizer::FSimplifySelectOnOuterJoin
//
//	@doc:
//		A SELECT on top of LOJ, where SELECT's predicate is NULL-filtering and
//		uses columns from LOJ's inner child, is simplified as Inner-Join
//
//		Example:
//
//			select * from (select * from R left join S on r1=s1) as foo where foo.s1>0;
//
//			is converted to:
//
//			select * from R inner join S on r1=s1 and s1>0;
//
//
//
//---------------------------------------------------------------------------
BOOL
CNormalizer::FSimplifySelectOnOuterJoin(
	CMemoryPool *mp, CExpression *pexprOuterJoin,
	CExpression *pexprPred,	 // selection predicate
	CExpression **ppexprResult)
{
	SPQOS_ASSERT(NULL != mp);
	SPQOS_ASSERT(COperator::EopLogicalLeftOuterJoin ==
				pexprOuterJoin->Pop()->Eopid());
	SPQOS_ASSERT(pexprPred->Pop()->FScalar());
	SPQOS_ASSERT(NULL != ppexprResult);

	if (0 == pexprOuterJoin->Arity())
	{
		// exit early for leaf patterns extracted from memo
		*ppexprResult = NULL;
		return false;
	}

	CExpression *pexprOuterJoinOuterChild = (*pexprOuterJoin)[0];
	CExpression *pexprOuterJoinInnerChild = (*pexprOuterJoin)[1];
	CExpression *pexprOuterJoinPred = (*pexprOuterJoin)[2];

	CColRefSet *pcrsOutput = pexprOuterJoinInnerChild->DeriveOutputColumns();
	if (!SPQOS_FTRACE(EopttraceDisableOuterJoin2InnerJoinRewrite) &&
		CPredicateUtils::FNullRejecting(mp, pexprPred, pcrsOutput))
	{
		// we have a predicate on top of LOJ that uses LOJ's inner child,
		// if the predicate filters-out nulls, we can add it to the join
		// predicate and turn LOJ into Inner-Join
		pexprOuterJoinOuterChild->AddRef();
		pexprOuterJoinInnerChild->AddRef();

		*ppexprResult = SPQOS_NEW(mp)
			CExpression(mp, SPQOS_NEW(mp) CLogicalInnerJoin(mp),
						pexprOuterJoinOuterChild, pexprOuterJoinInnerChild,
						CPredicateUtils::PexprConjunction(mp, pexprPred,
														  pexprOuterJoinPred));

		return true;
	}

	// failed to convert LOJ to inner-join
	return false;
}

// A SELECT on top of FOJ, where SELECT's predicate is NULL-filtering and uses
// columns from FOJ's outer child, is simplified as a SELECT on top of a
// Left-Join
// Example:
//   select * from lhs full join rhs on (lhs.a=rhs.a) where lhs.a = 5;
// is converted to:
//   select * from lhs left join rhs on (lhs.a=rhs.a) where lhs.a = 5;;
BOOL
CNormalizer::FSimplifySelectOnFullJoin(
	CMemoryPool *mp, CExpression *pexprFullJoin,
	CExpression *pexprPred,	 // selection predicate
	CExpression **ppexprResult)
{
	SPQOS_ASSERT(NULL != mp);
	SPQOS_ASSERT(COperator::EopLogicalFullOuterJoin ==
				pexprFullJoin->Pop()->Eopid());
	SPQOS_ASSERT(pexprPred->Pop()->FScalar());
	SPQOS_ASSERT(NULL != ppexprResult);

	if (0 == pexprFullJoin->Arity())
	{
		// exit early for leaf patterns extracted from memo
		*ppexprResult = NULL;
		return false;
	}

	CExpression *pexprLeftChild = (*pexprFullJoin)[0];
	CExpression *pexprRightChild = (*pexprFullJoin)[1];
	CExpression *pexprJoinPred = (*pexprFullJoin)[2];

	CColRefSet *pcrsOutputLeftChild = pexprLeftChild->DeriveOutputColumns();

	if (CPredicateUtils::FNullRejecting(mp, pexprPred, pcrsOutputLeftChild))
	{
		// we have a predicate on top of FOJ that uses FOJ's outer child,
		// if the predicate filters-out nulls, we can convert the FOJ to LOJ
		pexprLeftChild->AddRef();
		pexprRightChild->AddRef();
		pexprJoinPred->AddRef();
		pexprPred->AddRef();

		*ppexprResult = SPQOS_NEW(mp) CExpression(
			mp, SPQOS_NEW(mp) CLogicalSelect(mp),
			SPQOS_NEW(mp)
				CExpression(mp, SPQOS_NEW(mp) CLogicalLeftOuterJoin(mp),
							pexprLeftChild, pexprRightChild, pexprJoinPred),
			pexprPred);

		return true;
	}

	// failed to convert FOJ to LOJ
	return false;
}


//---------------------------------------------------------------------------
//	@function:
//		CNormalizer::PushThruSelect
//
//	@doc:
//		Push a conjunct through a select
//
//---------------------------------------------------------------------------
void
CNormalizer::PushThruSelect(CMemoryPool *mp, CExpression *pexprSelect,
							CExpression *pexprConj, CExpression **ppexprResult)
{
	SPQOS_ASSERT(NULL != pexprConj);
	SPQOS_ASSERT(NULL != ppexprResult);

	CExpression *pexprLogicalChild = (*pexprSelect)[0];
	CExpression *pexprScalarChild = (*pexprSelect)[1];
	CExpression *pexprPred =
		CPredicateUtils::PexprConjunction(mp, pexprScalarChild, pexprConj);

	if (CUtils::FScalarConstTrue(pexprPred))
	{
		pexprPred->Release();
		*ppexprResult = PexprNormalize(mp, pexprLogicalChild);

		return;
	}

	COperator::EOperatorId op_id = pexprLogicalChild->Pop()->Eopid();
	CExpression *pexprSimplified = NULL;
	if (COperator::EopLogicalLeftOuterJoin == op_id &&
		FSimplifySelectOnOuterJoin(mp, pexprLogicalChild, pexprPred,
								   &pexprSimplified))
	{
		// simplification succeeded, normalize resulting expression
		*ppexprResult = PexprNormalize(mp, pexprSimplified);
		pexprPred->Release();
		pexprSimplified->Release();

		return;
	}
	if (COperator::EopLogicalFullOuterJoin == op_id &&
		FSimplifySelectOnFullJoin(mp, pexprLogicalChild, pexprPred,
								  &pexprSimplified))
	{
		// simplification succeeded, normalize resulting expression
		*ppexprResult = PexprNormalize(mp, pexprSimplified);
		pexprPred->Release();
		pexprSimplified->Release();

		return;
	}

	if (FPushThruOuterChild(pexprLogicalChild))
	{
		PushThruOuterChild(mp, pexprLogicalChild, pexprPred, ppexprResult);
	}
	else
	{
		// logical child may not pass all predicates through, we need to collect
		// unpushable predicates, if any, into a top Select node
		CExpressionArray *pdrspqexprConjuncts =
			CPredicateUtils::PdrspqexprConjuncts(mp, pexprPred);
		CExpressionArray *pdrspqexprRemaining = NULL;
		CExpression *pexpr = NULL;
		PushThru(mp, pexprLogicalChild, pdrspqexprConjuncts, &pexpr,
				 &pdrspqexprRemaining);
		*ppexprResult = PexprSelect(mp, pexpr, pdrspqexprRemaining);
		pdrspqexprConjuncts->Release();
	}

	pexprPred->Release();
}


//---------------------------------------------------------------------------
//	@function:
//		CNormalizer::PexprSelect
//
//	@doc:
//		Return a Select expression, if needed, with a scalar condition made of
//		given array of conjuncts
//
//---------------------------------------------------------------------------
CExpression *
CNormalizer::PexprSelect(CMemoryPool *mp, CExpression *pexpr,
						 CExpressionArray *pdrspqexpr)
{
	SPQOS_ASSERT(NULL != pexpr);
	SPQOS_ASSERT(NULL != pdrspqexpr);

	if (0 == pdrspqexpr->Size())
	{
		// no predicate, return given expression
		pdrspqexpr->Release();
		return pexpr;
	}

	// result expression is a select over predicates
	CExpression *pexprConjunction =
		CPredicateUtils::PexprConjunction(mp, pdrspqexpr);
	CExpression *pexprSelect =
		CUtils::PexprSafeSelect(mp, pexpr, pexprConjunction);
	if (COperator::EopLogicalSelect != pexprSelect->Pop()->Eopid())
	{
		// Select node was pruned, return created expression
		return pexprSelect;
	}

	CExpression *pexprLogicalChild = (*pexprSelect)[0];
	COperator::EOperatorId eopidChild = pexprLogicalChild->Pop()->Eopid();

	// we have a Select on top of Outer Join expression, attempt simplifying expression into InnerJoin
	CExpression *pexprSimplified = NULL;
	if (COperator::EopLogicalLeftOuterJoin == eopidChild &&
		FSimplifySelectOnOuterJoin(mp, pexprLogicalChild, (*pexprSelect)[1],
								   &pexprSimplified))
	{
		// simplification succeeded, normalize resulting expression
		pexprSelect->Release();
		CExpression *pexprResult = PexprNormalize(mp, pexprSimplified);
		pexprSimplified->Release();

		return pexprResult;
	}
	else if (COperator::EopLogicalFullOuterJoin == eopidChild &&
			 FSimplifySelectOnFullJoin(mp, pexprLogicalChild, (*pexprSelect)[1],
									   &pexprSimplified))
	{
		// simplification succeeded, normalize resulting expression
		pexprSelect->Release();
		CExpression *pexprResult = PexprNormalize(mp, pexprSimplified);
		pexprSimplified->Release();

		return pexprResult;
	}

	// simplification failed, return created Select expression
	return pexprSelect;
}


//---------------------------------------------------------------------------
//	@function:
//		CNormalizer::PushThruUnaryWithoutScalarChild
//
//	@doc:
//		Push a conjunct through unary operator without scalar child
//
//---------------------------------------------------------------------------
void
CNormalizer::PushThruUnaryWithoutScalarChild(CMemoryPool *mp,
											 CExpression *pexprLogical,
											 CExpression *pexprConj,
											 CExpression **ppexprResult)
{
	SPQOS_ASSERT(NULL != pexprLogical);
	SPQOS_ASSERT(1 == pexprLogical->Arity());
	SPQOS_ASSERT(NULL != pexprConj);
	SPQOS_ASSERT(NULL != ppexprResult);

	// break scalar expression to conjuncts
	CExpressionArray *pdrspqexprConjuncts =
		CPredicateUtils::PdrspqexprConjuncts(mp, pexprConj);

	// get logical child
	CExpression *pexprLogicalChild = (*pexprLogical)[0];

	// push conjuncts through the logical child
	CExpression *pexprNewLogicalChild = NULL;
	CExpressionArray *pdrspqexprUnpushable = NULL;
	PushThru(mp, pexprLogicalChild, pdrspqexprConjuncts, &pexprNewLogicalChild,
			 &pdrspqexprUnpushable);
	pdrspqexprConjuncts->Release();

	// create a new logical expression based on recursion results
	COperator *pop = pexprLogical->Pop();
	pop->AddRef();
	CExpression *pexprNewLogical =
		SPQOS_NEW(mp) CExpression(mp, pop, pexprNewLogicalChild);
	*ppexprResult = PexprSelect(mp, pexprNewLogical, pdrspqexprUnpushable);
}


//---------------------------------------------------------------------------
//	@function:
//		CNormalizer::PushThruUnaryWithScalarChild
//
//	@doc:
//		Push a conjunct through a unary operator with scalar child
//
//---------------------------------------------------------------------------
void
CNormalizer::PushThruUnaryWithScalarChild(CMemoryPool *mp,
										  CExpression *pexprLogical,
										  CExpression *pexprConj,
										  CExpression **ppexprResult)
{
	SPQOS_ASSERT(NULL != pexprLogical);
	SPQOS_ASSERT(2 == pexprLogical->Arity());
	SPQOS_ASSERT(NULL != pexprConj);
	SPQOS_ASSERT(NULL != ppexprResult);

	// get logical and scalar children
	CExpression *pexprLogicalChild = (*pexprLogical)[0];
	CExpression *pexprScalarChild = (*pexprLogical)[1];

	// push conjuncts through the logical child
	CExpression *pexprNewLogicalChild = NULL;
	CExpressionArray *pdrspqexprUnpushable = NULL;

	// break scalar expression to conjuncts
	CExpressionArray *pdrspqexprConjuncts =
		CPredicateUtils::PdrspqexprConjuncts(mp, pexprConj);

	PushThru(mp, pexprLogicalChild, pdrspqexprConjuncts, &pexprNewLogicalChild,
			 &pdrspqexprUnpushable);
	pdrspqexprConjuncts->Release();

	// create a new logical expression based on recursion results
	COperator *pop = pexprLogical->Pop();
	pop->AddRef();
	pexprScalarChild->AddRef();
	CExpression *pexprNewLogical = SPQOS_NEW(mp)
		CExpression(mp, pop, pexprNewLogicalChild, pexprScalarChild);
	*ppexprResult = PexprSelect(mp, pexprNewLogical, pdrspqexprUnpushable);
}

//---------------------------------------------------------------------------
//	@function:
//		CNormalizer::SplitConjunctForSeqPrj
//
//	@doc:
//		Split the given conjunct into pushable and unpushable predicates
//		for a sequence project expression
//
//---------------------------------------------------------------------------
void
CNormalizer::SplitConjunctForSeqPrj(CMemoryPool *mp, CExpression *pexprSeqPrj,
									CExpression *pexprConj,
									CExpressionArray **ppdrspqexprPushable,
									CExpressionArray **ppdrspqexprUnpushable)
{
	SPQOS_ASSERT(NULL != pexprSeqPrj);
	SPQOS_ASSERT(NULL != pexprConj);
	SPQOS_ASSERT(NULL != ppdrspqexprPushable);
	SPQOS_ASSERT(NULL != ppdrspqexprUnpushable);

	*ppdrspqexprPushable = SPQOS_NEW(mp) CExpressionArray(mp);
	*ppdrspqexprUnpushable = SPQOS_NEW(mp) CExpressionArray(mp);
	CExpressionArray *pdrspqexprPreds =
		CPredicateUtils::PdrspqexprConjuncts(mp, pexprConj);
	const ULONG ulPreds = pdrspqexprPreds->Size();
	for (ULONG ul = 0; ul < ulPreds; ul++)
	{
		CExpression *pexprPred = (*pdrspqexprPreds)[ul];
		pexprPred->AddRef();
		if (FPushableThruSeqPrjChild(pexprSeqPrj, pexprPred))
		{
			(*ppdrspqexprPushable)->Append(pexprPred);
		}
		else
		{
			(*ppdrspqexprUnpushable)->Append(pexprPred);
		}
	}
	pdrspqexprPreds->Release();
}


//---------------------------------------------------------------------------
//	@function:
//		CNormalizer::PushThruSeqPrj
//
//	@doc:
//		Push a conjunct through a sequence project expression
//
//---------------------------------------------------------------------------
void
CNormalizer::PushThruSeqPrj(CMemoryPool *mp, CExpression *pexprSeqPrj,
							CExpression *pexprConj, CExpression **ppexprResult)
{
	SPQOS_ASSERT(NULL != pexprSeqPrj);
	SPQOS_ASSERT(CLogical::EopLogicalSequenceProject ==
				pexprSeqPrj->Pop()->Eopid());
	SPQOS_ASSERT(NULL != pexprConj);
	SPQOS_ASSERT(NULL != ppexprResult);

	// get logical and scalar children
	CExpression *pexprLogicalChild = (*pexprSeqPrj)[0];
	CExpression *pexprScalarChild = (*pexprSeqPrj)[1];

	// break scalar expression to pushable and unpushable conjuncts
	CExpressionArray *pdrspqexprPushable = NULL;
	CExpressionArray *pdrspqexprUnpushable = NULL;
	SplitConjunctForSeqPrj(mp, pexprSeqPrj, pexprConj, &pdrspqexprPushable,
						   &pdrspqexprUnpushable);

	CExpression *pexprNewLogicalChild = NULL;
	if (0 < pdrspqexprPushable->Size())
	{
		CExpression *pexprPushableConj =
			CPredicateUtils::PexprConjunction(mp, pdrspqexprPushable);
		PushThru(mp, pexprLogicalChild, pexprPushableConj,
				 &pexprNewLogicalChild);
		pexprPushableConj->Release();
	}
	else
	{
		// no pushable predicates on top of sequence project,
		// we still need to process child recursively to push-down child's own predicates
		pdrspqexprPushable->Release();
		pexprNewLogicalChild = PexprNormalize(mp, pexprLogicalChild);
	}

	// create a new logical expression based on recursion results
	COperator *pop = pexprSeqPrj->Pop();
	pop->AddRef();
	pexprScalarChild->AddRef();
	CExpression *pexprNewLogical = SPQOS_NEW(mp)
		CExpression(mp, pop, pexprNewLogicalChild, pexprScalarChild);

	// create a select node for remaining predicates, if any
	*ppexprResult = PexprSelect(mp, pexprNewLogical, pdrspqexprUnpushable);
}


//---------------------------------------------------------------------------
//	@function:
//		CNormalizer::PushThruSetOp
//
//	@doc:
//		Push a conjunct through set operation
//
//---------------------------------------------------------------------------
void
CNormalizer::PushThruSetOp(CMemoryPool *mp, CExpression *pexprSetOp,
						   CExpression *pexprConj, CExpression **ppexprResult)
{
	SPQOS_ASSERT(NULL != pexprSetOp);
	SPQOS_ASSERT(CUtils::FLogicalSetOp(pexprSetOp->Pop()));
	SPQOS_ASSERT(NULL != pexprConj);
	SPQOS_ASSERT(NULL != ppexprResult);

	CLogicalSetOp *popSetOp = CLogicalSetOp::PopConvert(pexprSetOp->Pop());
	CColRefArray *pdrspqcrOutput = popSetOp->PdrspqcrOutput();
	CColRefSet *pcrsOutput = SPQOS_NEW(mp) CColRefSet(mp, pdrspqcrOutput);
	CColRef2dArray *pdrspqdrspqcrInput = popSetOp->PdrspqdrspqcrInput();
	CExpressionArray *pdrspqexprNewChildren = SPQOS_NEW(mp) CExpressionArray(mp);
	const ULONG arity = pexprSetOp->Arity();
	for (ULONG ul = 0; ul < arity; ul++)
	{
		CExpression *pexprChild = (*pexprSetOp)[ul];
		CColRefArray *pdrspqcrChild = (*pdrspqdrspqcrInput)[ul];
		CColRefSet *pcrsChild = SPQOS_NEW(mp) CColRefSet(mp, pdrspqcrChild);

		pexprConj->AddRef();
		CExpression *pexprRemappedConj = pexprConj;
		if (!pcrsChild->Equals(pcrsOutput))
		{
			// child columns are different from SetOp output columns,
			// we need to fix conjunct by mapping output columns to child columns,
			// columns that are not in the output of SetOp child need also to be re-mapped
			// to new columns,
			//
			// for example, if the conjunct looks like 'x > (select max(y) from T)'
			// and the SetOp child produces only column x, we need to create a new
			// conjunct that looks like 'x1 > (select max(y1) from T)'
			// where x1 is a copy of x, and y1 is a copy of y
			//
			// this is achieved by passing (must_exist = True) flag below, which enforces
			// creating column copies for columns not already in the given map
			UlongToColRefMap *colref_mapping =
				CUtils::PhmulcrMapping(mp, pdrspqcrOutput, pdrspqcrChild);
			pexprRemappedConj->Release();
			pexprRemappedConj = pexprConj->PexprCopyWithRemappedColumns(
				mp, colref_mapping, true /*must_exist*/);
			colref_mapping->Release();
		}

		CExpression *pexprNewChild = NULL;
		PushThru(mp, pexprChild, pexprRemappedConj, &pexprNewChild);
		pdrspqexprNewChildren->Append(pexprNewChild);

		pexprRemappedConj->Release();
		pcrsChild->Release();
	}

	pcrsOutput->Release();
	popSetOp->AddRef();
	*ppexprResult =
		SPQOS_NEW(mp) CExpression(mp, popSetOp, pdrspqexprNewChildren);
}


//---------------------------------------------------------------------------
//	@function:
//		CNormalizer::PushThruJoin
//
//	@doc:
//		Push a conjunct through a join
//
//
//---------------------------------------------------------------------------
void
CNormalizer::PushThruJoin(CMemoryPool *mp, CExpression *pexprJoin,
						  CExpression *pexprConj, CExpression **ppexprResult)
{
	SPQOS_ASSERT(NULL != pexprConj);
	SPQOS_ASSERT(NULL != ppexprResult);

	COperator *pop = pexprJoin->Pop();
	CLogicalNAryJoin *popNAryJoin = CLogicalNAryJoin::PopConvert(pop);
	const ULONG arity = pexprJoin->Arity();
	BOOL fLASApply = CUtils::FLeftAntiSemiApply(pop);
	COperator::EOperatorId op_id = pop->Eopid();
	BOOL fOuterJoin = COperator::EopLogicalLeftOuterJoin == op_id ||
					  COperator::EopLogicalLeftOuterApply == op_id ||
					  COperator::EopLogicalLeftOuterCorrelatedApply == op_id;
	BOOL fMixedInnerOuterJoin =
		(popNAryJoin && popNAryJoin->HasOuterJoinChildren());

	if (fOuterJoin && !CUtils::FScalarConstTrue(pexprConj))
	{
		// whenever possible, push incoming predicate through outer join's outer child,
		// recursion will eventually reach the rest of PushThruJoin() to process join predicates
		PushThruOuterChild(mp, pexprJoin, pexprConj, ppexprResult);

		return;
	}

	// if we have an nary join with only inner joins and a false scalar condition,
	// simplify the expression to a constant false. Trying to normalize this would
	// improperly cause the scalar condition to be pulled into one of the predicates,
	// and leave the condition as a const false
	if (popNAryJoin && !fMixedInnerOuterJoin &&
		CUtils::FScalarConstFalse(pexprConj))
	{
		COperator *popCTG = SPQOS_NEW(mp) CLogicalConstTableGet(
			mp, pexprJoin->DeriveOutputColumns()->Pdrspqcr(mp),
			SPQOS_NEW(mp) IDatum2dArray(mp));
		*ppexprResult = SPQOS_NEW(mp) CExpression(mp, popCTG);

		return;
	}

	// combine conjunct with join predicate
	CExpression *pexprScalar = (*pexprJoin)[arity - 1];
	if (fMixedInnerOuterJoin)
	{
		SPQOS_ASSERT(COperator::EopScalarNAryJoinPredList ==
					pexprScalar->Pop()->Eopid());
		pexprScalar = (*pexprScalar)[0];

		if (COperator::EopScalarNAryJoinPredList == pexprConj->Pop()->Eopid())
		{
			pexprConj = (*pexprConj)[0];
		}
	}
	CExpression *pexprPred =
		CPredicateUtils::PexprConjunction(mp, pexprScalar, pexprConj);

	// break predicate to conjuncts
	CExpressionArray *pdrspqexprConjuncts =
		CPredicateUtils::PdrspqexprConjuncts(mp, pexprPred);
	pexprPred->Release();

	// push predicates through children and compute new child expressions
	CExpressionArray *pdrspqexprChildren = SPQOS_NEW(mp) CExpressionArray(mp);

	for (ULONG ul = 0; ul < arity - 1; ul++)
	{
		CExpression *pexprChild = (*pexprJoin)[ul];
		CExpression *pexprNewChild = NULL;
		if (fLASApply)
		{
			// do not push anti-semi-apply predicates to any of the children
			pexprNewChild = PexprNormalize(mp, pexprChild);
			pdrspqexprChildren->Append(pexprNewChild);
			continue;
		}

		if (0 == ul && fOuterJoin)
		{
			// do not push outer join predicates through outer child
			// otherwise, we will throw away outer child's tuples that should
			// be part of the join result
			pexprNewChild = PexprNormalize(mp, pexprChild);
			pdrspqexprChildren->Append(pexprNewChild);
			continue;
		}

		if (fMixedInnerOuterJoin && !popNAryJoin->IsInnerJoinChild(ul))
		{
			// this is similar to what PushThruOuterChild does, only push the
			// preds to those children that are using an inner join
			pexprNewChild = PexprNormalize(mp, pexprChild);
			pdrspqexprChildren->Append(pexprNewChild);
			continue;
		}

		CExpressionArray *pdrspqexprRemaining = NULL;
		PushThru(mp, pexprChild, pdrspqexprConjuncts, &pexprNewChild,
				 &pdrspqexprRemaining);
		pdrspqexprChildren->Append(pexprNewChild);

		pdrspqexprConjuncts->Release();
		pdrspqexprConjuncts = pdrspqexprRemaining;
	}

	// remaining conjuncts become the new join predicate
	CExpression *pexprNewScalar =
		CPredicateUtils::PexprConjunction(mp, pdrspqexprConjuncts);
	if (fMixedInnerOuterJoin)
	{
		CExpressionArray *naryJoinPredicates =
			SPQOS_NEW(mp) CExpressionArray(mp);
		naryJoinPredicates->Append(pexprNewScalar);
		CExpression *pexprScalar = (*pexprJoin)[arity - 1];
		ULONG scalar_arity = pexprScalar->Arity();

		for (ULONG count = 1; count < scalar_arity; count++)
		{
			CExpression *pexprChild = (*pexprScalar)[count];
			pexprChild->AddRef();
			naryJoinPredicates->Append(pexprChild);
		}

		CExpression *nAryJoinPredicateList = SPQOS_NEW(mp) CExpression(
			mp, SPQOS_NEW(mp) CScalarNAryJoinPredList(mp), naryJoinPredicates);
		pdrspqexprChildren->Append(nAryJoinPredicateList);
	}
	else
	{
		pdrspqexprChildren->Append(pexprNewScalar);
	}

	// create a new join expression
	pop->AddRef();
	CExpression *pexprJoinWithInferredPred =
		SPQOS_NEW(mp) CExpression(mp, pop, pdrspqexprChildren);
	CExpression *pexprJoinWithoutInferredPred = NULL;

	// remove inferred predicate from the join expression. inferred predicate can impact the cost
	// of the join node as the node will have to project more columns even though they are not
	// used by the above nodes. So, better to remove them from the join after all the inferred predicates
	// are pushed down.
	// We don't do this for CTE as removing inferred predicates from CTE with inlining enabled may
	// cause the relational properties of two group to be same and can result in group merges,
	// which can lead to circular derivations, we should fix the bug to avoid circular references
	// before we enable it for Inlined CTEs.
	if (CUtils::CanRemoveInferredPredicates(pop->Eopid()) &&
		!COptCtxt::PoctxtFromTLS()->Pcteinfo()->FEnableInlining())
	{
		// Subqueries should be un-nested first so that we can infer any predicates if possible,
		// if they are not un-nested, they don't have any inferred predicates to remove.
		// ORCA only infers predicates for subqueries after they are un-nested.
		BOOL has_subquery =
			CUtils::FHasSubqueryOrApply(pexprJoinWithInferredPred);
		if (!has_subquery)
		{
			pexprJoinWithoutInferredPred = CUtils::MakeJoinWithoutInferredPreds(
				mp, pexprJoinWithInferredPred);
			pexprJoinWithInferredPred->Release();
			*ppexprResult = pexprJoinWithoutInferredPred;
			return;
		}
	}
	*ppexprResult = pexprJoinWithInferredPred;
}

//---------------------------------------------------------------------------
//	@function:
//		CNormalizer::FChild
//
//	@doc:
//		Return true if second expression is a child of first expression
//
//---------------------------------------------------------------------------
BOOL
CNormalizer::FChild(CExpression *pexpr, CExpression *pexprChild)
{
	SPQOS_ASSERT(NULL != pexpr);
	SPQOS_ASSERT(NULL != pexprChild);

	BOOL fFound = false;
	const ULONG arity = pexpr->Arity();
	for (ULONG ul = 0; !fFound && ul < arity; ul++)
	{
		fFound = ((*pexpr)[ul] == pexprChild);
	}

	return fFound;
}


//---------------------------------------------------------------------------
//	@function:
//		CNormalizer::PushThru
//
//	@doc:
//		Hub for pushing a conjunct through a logical expression
//
//---------------------------------------------------------------------------
void
CNormalizer::PushThru(CMemoryPool *mp, CExpression *pexprLogical,
					  CExpression *pexprConj, CExpression **ppexprResult)
{
	SPQOS_CHECK_STACK_SIZE;
	SPQOS_ASSERT(NULL != pexprLogical);
	SPQOS_ASSERT(NULL != pexprConj);
	SPQOS_ASSERT(NULL != ppexprResult);

	// TODO: 01/13/2012 - ; predicate push down with set returning functions

	if (0 == pexprLogical->Arity())
	{
		// end recursion early for leaf patterns extracted from memo
		pexprLogical->AddRef();
		pexprConj->AddRef();
		*ppexprResult = CUtils::PexprSafeSelect(mp, pexprLogical, pexprConj);
		return;
	}

	FnPushThru *pfnpt = NULL;
	COperator::EOperatorId op_id = pexprLogical->Pop()->Eopid();
	const ULONG size = SPQOS_ARRAY_SIZE(m_rspqt);
	// find the push thru function corresponding to the given operator
	for (ULONG ul = 0; pfnpt == NULL && ul < size; ul++)
	{
		if (op_id == m_rspqt[ul].m_eopid)
		{
			pfnpt = m_rspqt[ul].m_pfnpt;
		}
	}

	// We must be careful when pushing left correlated outer apply
	bool is_pushable =
		COperator::EopLogicalLeftOuterCorrelatedApply !=
			pexprLogical->Pop()->Eopid() ||
		CLogicalLeftOuterCorrelatedApply::PopConvert(pexprLogical->Pop())
			->IsPredicatePushDownAllowed();

	if (NULL != pfnpt && is_pushable)
	{
		pfnpt(mp, pexprLogical, pexprConj, ppexprResult);
		return;
	}

	// can't push predicates through, start a new normalization path
	CExpression *pexprNormalized = PexprRecursiveNormalize(mp, pexprLogical);
	*ppexprResult = pexprNormalized;
	if (!FChild(pexprLogical, pexprConj))
	{
		// add select node on top of the result for the given predicate
		pexprConj->AddRef();
		*ppexprResult = CUtils::PexprSafeSelect(mp, pexprNormalized, pexprConj);
	}
}


//---------------------------------------------------------------------------
//	@function:
//		CNormalizer::PushThru
//
//	@doc:
//		Push an array of conjuncts through a logical expression;
//		compute an array of unpushable conjuncts
//
//---------------------------------------------------------------------------
void
CNormalizer::PushThru(CMemoryPool *mp, CExpression *pexprLogical,
					  CExpressionArray *pdrspqexprConjuncts,
					  CExpression **ppexprResult,
					  CExpressionArray **ppdrspqexprRemaining)
{
	SPQOS_ASSERT(NULL != pexprLogical);
	SPQOS_ASSERT(NULL != pdrspqexprConjuncts);
	SPQOS_ASSERT(NULL != ppexprResult);

	CExpressionArray *pdrspqexprPushable = SPQOS_NEW(mp) CExpressionArray(mp);
	CExpressionArray *pdrspqexprUnpushable = SPQOS_NEW(mp) CExpressionArray(mp);

	const ULONG size = pdrspqexprConjuncts->Size();
	for (ULONG ul = 0; ul < size; ul++)
	{
		CExpression *pexprConj = (*pdrspqexprConjuncts)[ul];
		pexprConj->AddRef();

		if (FPushable(pexprLogical, pexprConj))
		{
			pdrspqexprPushable->Append(pexprConj);
		}
		else
		{
			pdrspqexprUnpushable->Append(pexprConj);
		}
	}

	// push through a conjunction of all pushable predicates
	CExpression *pexprPred =
		CPredicateUtils::PexprConjunction(mp, pdrspqexprPushable);
	if (FPushThruOuterChild(pexprLogical))
	{
		PushThruOuterChild(mp, pexprLogical, pexprPred, ppexprResult);
	}
	else
	{
		PushThru(mp, pexprLogical, pexprPred, ppexprResult);
	}
	pexprPred->Release();

	*ppdrspqexprRemaining = pdrspqexprUnpushable;
}


//---------------------------------------------------------------------------
//	@function:
//		CNormalizer::PexprNormalize
//
//	@doc:
//		Main driver
//
//---------------------------------------------------------------------------
CExpression *
CNormalizer::PexprNormalize(CMemoryPool *mp, CExpression *pexpr)
{
	SPQOS_CHECK_STACK_SIZE;
	SPQOS_ASSERT(NULL != pexpr);

	if (0 == pexpr->Arity())
	{
		// end recursion early for leaf patterns extracted from memo
		pexpr->AddRef();
		return pexpr;
	}

	CExpression *pexprResult = NULL;
	COperator *pop = pexpr->Pop();
	if (pop->FLogical() && CLogical::PopConvert(pop)->FSelectionOp())
	{
		if (FPushThruOuterChild(pexpr))
		{
			CExpression *pexprConstTrue =
				CUtils::PexprScalarConstBool(mp, true /*value*/);
			PushThru(mp, pexpr, pexprConstTrue, &pexprResult);
			pexprConstTrue->Release();
		}
		else
		{
			// add-ref all children except scalar predicate
			const ULONG arity = pexpr->Arity();
			CExpressionArray *pdrspqexpr = SPQOS_NEW(mp) CExpressionArray(mp);
			for (ULONG ul = 0; ul < arity - 1; ul++)
			{
				CExpression *pexprChild = (*pexpr)[ul];
				pexprChild->AddRef();
				pdrspqexpr->Append(pexprChild);
			}

			// normalize scalar predicate and construct a new expression
			CExpression *pexprPred = (*pexpr)[pexpr->Arity() - 1];
			CExpression *pexprPredNormalized =
				PexprRecursiveNormalize(mp, pexprPred);
			pdrspqexpr->Append(pexprPredNormalized);
			COperator *pop = pexpr->Pop();
			pop->AddRef();
			CExpression *pexprNew =
				SPQOS_NEW(mp) CExpression(mp, pop, pdrspqexpr);

			// push normalized predicate through
			PushThru(mp, pexprNew, pexprPredNormalized, &pexprResult);
			pexprNew->Release();
		}
	}
	else
	{
		pexprResult = PexprRecursiveNormalize(mp, pexpr);
	}
	SPQOS_ASSERT(NULL != pexprResult);

	return pexprResult;
}

//---------------------------------------------------------------------------
//	@function:
//		CNormalizer::PexprPullUpAndCombineProjects
//
//	@doc:
//		Pulls up logical projects as far as possible, and combines consecutive
//		projects if possible
//
//---------------------------------------------------------------------------
CExpression *
CNormalizer::PexprPullUpAndCombineProjects(
	CMemoryPool *mp, CExpression *pexpr,
	BOOL *pfSuccess	 // output to indicate whether anything was pulled up
)
{
	SPQOS_ASSERT(NULL != pexpr);
	SPQOS_ASSERT(NULL != pfSuccess);

	COperator *pop = pexpr->Pop();
	const ULONG arity = pexpr->Arity();
	if (!pop->FLogical() || 0 == arity)
	{
		pexpr->AddRef();
		return pexpr;
	}

	CExpressionArray *pdrspqexprChildren = SPQOS_NEW(mp) CExpressionArray(mp);
	CExpressionArray *pdrspqexprPrElPullUp = SPQOS_NEW(mp) CExpressionArray(mp);
	CExpressionHandle exprhdl(mp);
	exprhdl.Attach(pexpr);

	CColRefSet *pcrsOutput = pexpr->DeriveOutputColumns();

	// extract the columns used by the scalar expression and the operator itself (for grouping, sorting, etc.)
	CColRefSet *pcrsUsed = exprhdl.PcrsUsedColumns(mp);
	for (ULONG ul = 0; ul < arity; ul++)
	{
		CExpression *pexprChild =
			PexprPullUpAndCombineProjects(mp, (*pexpr)[ul], pfSuccess);
		if (pop->FLogical() &&
			CLogical::PopConvert(pop)->FCanPullProjectionsUp(ul) &&
			COperator::EopLogicalProject == pexprChild->Pop()->Eopid())
		{
			// this child is a project - see if any project elements can be pulled up
			CExpression *pexprNewChild = PexprPullUpProjectElements(
				mp, pexprChild, pcrsUsed, pcrsOutput, &pdrspqexprPrElPullUp);

			pexprChild->Release();
			pexprChild = pexprNewChild;
		}

		pdrspqexprChildren->Append(pexprChild);
	}

	pcrsUsed->Release();
	pop->AddRef();

	if (0 < pdrspqexprPrElPullUp->Size() &&
		COperator::EopLogicalProject == pop->Eopid())
	{
		// some project elements have been pulled up and the original expression
		// was a project - combine its project list with the pulled up project elements
		SPQOS_ASSERT(2 == pdrspqexprChildren->Size());
		*pfSuccess = true;
		CExpression *pexprRelational = (*pdrspqexprChildren)[0];
		CExpression *pexprPrLOld = (*pdrspqexprChildren)[1];
		pexprRelational->AddRef();

		CUtils::AddRefAppend(pdrspqexprPrElPullUp, pexprPrLOld->PdrgPexpr());
		pdrspqexprChildren->Release();
		CExpression *pexprPrjList = SPQOS_NEW(mp) CExpression(
			mp, SPQOS_NEW(mp) CScalarProjectList(mp), pdrspqexprPrElPullUp);

#ifdef SPQOS_DEBUG
		CColRefSet *availableCRs = SPQOS_NEW(mp) CColRefSet(mp);

		availableCRs->Include(pexprRelational->DeriveOutputColumns());
		availableCRs->Include(pexpr->DeriveOuterReferences());
		// check that the new project node has all the values it needs
		SPQOS_ASSERT(
			availableCRs->ContainsAll(pexprPrjList->DeriveUsedColumns()));
		availableCRs->Release();
#endif

		return SPQOS_NEW(mp) CExpression(mp, pop, pexprRelational, pexprPrjList);
	}

	CExpression *pexprOutput =
		SPQOS_NEW(mp) CExpression(mp, pop, pdrspqexprChildren);

	if (0 == pdrspqexprPrElPullUp->Size())
	{
		// no project elements were pulled up
		pdrspqexprPrElPullUp->Release();
		return pexprOutput;
	}

	// some project elements were pulled - add a project on top of output expression
	*pfSuccess = true;
	CExpression *pexprPrjList = SPQOS_NEW(mp) CExpression(
		mp, SPQOS_NEW(mp) CScalarProjectList(mp), pdrspqexprPrElPullUp);
	SPQOS_ASSERT(pexprOutput->DeriveOutputColumns()->ContainsAll(
		pexprPrjList->DeriveUsedColumns()));

	return SPQOS_NEW(mp) CExpression(mp, SPQOS_NEW(mp) CLogicalProject(mp),
									pexprOutput, pexprPrjList);
}

//---------------------------------------------------------------------------
//	@function:
//		CNormalizer::PexprPullUpProjectElements
//
//	@doc:
//		Pull up project elements from the given projection expression that do not
//		exist in the given used columns set. The pulled up project elements must only
//		use columns that are in the output columns of the parent operator. Returns
//		a new expression that does not have the pulled up project elements. These
//		project elements are appended to the given array.
//
//---------------------------------------------------------------------------
CExpression *
CNormalizer::PexprPullUpProjectElements(
	CMemoryPool *mp, CExpression *pexpr, CColRefSet *pcrsUsed,
	CColRefSet *pcrsOutput,
	CExpressionArray *
		*ppdrspqexprPrElPullUp  // output: the pulled-up project elements
)
{
	SPQOS_ASSERT(NULL != pexpr);
	SPQOS_ASSERT(COperator::EopLogicalProject == pexpr->Pop()->Eopid());
	SPQOS_ASSERT(NULL != pcrsUsed);
	SPQOS_ASSERT(NULL != ppdrspqexprPrElPullUp);
	SPQOS_ASSERT(NULL != *ppdrspqexprPrElPullUp);

	if (2 != pexpr->Arity())
	{
		// the project's children were not extracted as part of the pattern in this xform
		SPQOS_ASSERT(0 == pexpr->Arity());
		pexpr->AddRef();
		return pexpr;
	}

	CExpressionArray *pdrspqexprPrElNoPullUp = SPQOS_NEW(mp) CExpressionArray(mp);
	CExpression *pexprPrL = (*pexpr)[1];

	const ULONG ulProjElements = pexprPrL->Arity();
	for (ULONG ul = 0; ul < ulProjElements; ul++)
	{
		CExpression *pexprPrEl = (*pexprPrL)[ul];
		CScalarProjectElement *popPrEl =
			CScalarProjectElement::PopConvert(pexprPrEl->Pop());
		CColRef *pcrDefined = popPrEl->Pcr();
		CColRefSet *pcrsUsedByProjElem = pexprPrEl->DeriveUsedColumns();

		// a proj elem can be pulled up only if the defined column is not in
		// pcrsUsed and its used columns are in pcrOutput
		// NB we don't pull up projections that call set-returning functions
		pexprPrEl->AddRef();

		if (!pcrsUsed->FMember(pcrDefined) &&
			pcrsOutput->ContainsAll(pcrsUsedByProjElem) &&
			!pexprPrEl->DeriveHasNonScalarFunction())
		{
			(*ppdrspqexprPrElPullUp)->Append(pexprPrEl);
		}
		else
		{
			pdrspqexprPrElNoPullUp->Append(pexprPrEl);
		}
	}

	CExpression *pexprNew = (*pexpr)[0];
	pexprNew->AddRef();
	if (0 == pdrspqexprPrElNoPullUp->Size())
	{
		pdrspqexprPrElNoPullUp->Release();
	}
	else
	{
		// some project elements could not be pulled up - need a project here
		CExpression *pexprPrjList = SPQOS_NEW(mp) CExpression(
			mp, SPQOS_NEW(mp) CScalarProjectList(mp), pdrspqexprPrElNoPullUp);
		pexprNew = SPQOS_NEW(mp) CExpression(
			mp, SPQOS_NEW(mp) CLogicalProject(mp), pexprNew, pexprPrjList);
	}

	return pexprNew;
}

//---------------------------------------------------------------------------
//	@function:
//		CNormalizer::PexprPullUpProjections
//
//	@doc:
//		Pulls up logical projects as far as possible, and combines consecutive
//		projects if possible
//
//---------------------------------------------------------------------------
CExpression *
CNormalizer::PexprPullUpProjections(CMemoryPool *mp, CExpression *pexpr)
{
	SPQOS_ASSERT(NULL != pexpr);

	BOOL fPullUp = true;
	pexpr->AddRef();
	CExpression *pexprOutput = pexpr;

	while (fPullUp)
	{
		fPullUp = false;

		CExpression *pexprOutputNew =
			PexprPullUpAndCombineProjects(mp, pexprOutput, &fPullUp);
		pexprOutput->Release();
		pexprOutput = pexprOutputNew;
	}

	SPQOS_ASSERT(FLocalColsSubsetOfInputCols(mp, pexprOutput));

	return pexprOutput;
}


#ifdef SPQOS_DEBUG
//---------------------------------------------------------------------------
//     @function:
//             CNormalizer::FLocalColsSubsetOfInputCols
//
//     @doc:
//             Check if the columns used by the operator are a subset of its input columns
//
//---------------------------------------------------------------------------
BOOL
CNormalizer::FLocalColsSubsetOfInputCols(CMemoryPool *mp, CExpression *pexpr)
{
	SPQOS_ASSERT(NULL != pexpr);
	SPQOS_CHECK_STACK_SIZE;

	CExpressionHandle exprhdl(mp);
	if (NULL != pexpr->Pgexpr())
	{
		exprhdl.Attach(pexpr->Pgexpr());
	}
	else
	{
		exprhdl.Attach(pexpr);
	}
	exprhdl.DeriveProps(NULL /*pdpctxt*/);

	BOOL fValid = true;
	if (pexpr->Pop()->FLogical())
	{
		if (0 == exprhdl.UlNonScalarChildren())
		{
			return true;
		}

		CColRefSet *pcrsInput = SPQOS_NEW(mp) CColRefSet(mp);

		const ULONG arity = exprhdl.Arity();
		for (ULONG ul = 0; ul < arity; ul++)
		{
			if (!exprhdl.FScalarChild(ul))
			{
				pcrsInput->Include(exprhdl.DeriveOutputColumns(ul));
			}
		}

		// check if the operator's locally used columns are a subset of the input columns
		CColRefSet *pcrsUsedOp = exprhdl.PcrsUsedColumns(mp);
		pcrsUsedOp->Exclude(exprhdl.DeriveOuterReferences());

		fValid = pcrsInput->ContainsAll(pcrsUsedOp);

		// release
		pcrsInput->Release();
		pcrsUsedOp->Release();
	}

	// check if its children are valid
	const ULONG ulExprArity = pexpr->Arity();
	for (ULONG ulChildIdx = 0; ulChildIdx < ulExprArity && fValid; ulChildIdx++)
	{
		CExpression *pexprChild = (*pexpr)[ulChildIdx];
		fValid = FLocalColsSubsetOfInputCols(mp, pexprChild);
	}

	return fValid;
}

#endif	//SPQOS_DEBUG

// EOF
