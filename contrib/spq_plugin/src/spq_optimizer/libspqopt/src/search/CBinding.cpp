//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		CBinding.cpp
//
//	@doc:
//		Implementation of Binding structure
//---------------------------------------------------------------------------

#include "spqopt/search/CBinding.h"

#include "spqos/base.h"

#include "spqopt/operators/CPattern.h"
#include "spqopt/search/CGroupProxy.h"
#include "spqopt/search/CMemo.h"

using namespace spqopt;

//---------------------------------------------------------------------------
//	@function:
//		CBinding::PgexprNext
//
//	@doc:
//		Move cursor within a group (initialize if NULL)
//
//---------------------------------------------------------------------------
CGroupExpression *
CBinding::PgexprNext(CGroup *pgroup, CGroupExpression *pgexpr) const
{
	CGroupProxy spq(pgroup);

	if (pgroup->FScalar())
	{
		// initialize
		if (NULL == pgexpr)
		{
			return spq.PgexprFirst();
		}

		return spq.PgexprNext(pgexpr);
	}

	// for non-scalar group, we only consider logical expressions in bindings
	return spq.PgexprNextLogical(pgexpr);
}


//---------------------------------------------------------------------------
//	@function:
//		CBinding::PexprExpandPattern
//
//	@doc:
//		Pattern operators which match more than one operator need to be
//		passed around;
//		Given the pattern determine if we need to re-use the pattern operators;
//
//---------------------------------------------------------------------------
CExpression *
CBinding::PexprExpandPattern(CExpression *pexprPattern, ULONG ulPos,
							 ULONG arity)
{
	SPQOS_ASSERT_IMP(pexprPattern->Pop()->FPattern(),
					!CPattern::PopConvert(pexprPattern->Pop())->FLeaf());

	// re-use tree pattern
	if (COperator::EopPatternTree == pexprPattern->Pop()->Eopid() ||
		COperator::EopPatternMultiTree == pexprPattern->Pop()->Eopid())
	{
		return pexprPattern;
	}

	// re-use first child if it is a multi-leaf/tree
	if (0 < pexprPattern->Arity() &&
		CPattern::FMultiNode((*pexprPattern)[0]->Pop()))
	{
		SPQOS_ASSERT(pexprPattern->Arity() <= 2);

		if (ulPos == arity - 1)
		{
			// special-case last child
			return (*pexprPattern)[pexprPattern->Arity() - 1];
		}

		// otherwise re-use multi-leaf/tree child
		return (*pexprPattern)[0];
	}
	SPQOS_ASSERT(pexprPattern->Arity() > ulPos);

	return (*pexprPattern)[ulPos];
}


//---------------------------------------------------------------------------
//	@function:
//		CBinding::PexprFinalize
//
//	@doc:
//		Assemble expression; substitute operator with pattern as necessary
//
//---------------------------------------------------------------------------
CExpression *
CBinding::PexprFinalize(CMemoryPool *mp, CGroupExpression *pgexpr,
						CExpressionArray *pdrspqexpr)
{
	COperator *pop = pgexpr->Pop();

	pop->AddRef();
	CExpression *pexpr = SPQOS_NEW(mp)
		CExpression(mp, pop, pgexpr, pdrspqexpr, NULL /*input_stats*/);

	return pexpr;
}


//---------------------------------------------------------------------------
//	@function:
//		CBinding::PexprExtract
//
//	@doc:
//		Extract a binding according to a given pattern;
//		Keep root node fixed;
//
//---------------------------------------------------------------------------
CExpression *
CBinding::PexprExtract(CMemoryPool *mp, CGroupExpression *pgexpr,
					   CExpression *pexprPattern, CExpression *pexprLast)
{
	SPQOS_CHECK_ABORT;

	if (!pexprPattern->FMatchPattern(pgexpr))
	{
		// shallow matching fails
		return NULL;
	}

	// the previously extracted pattern must have the same root
	SPQOS_ASSERT_IMP(NULL != pexprLast, pexprLast->Pgexpr() == pgexpr);

	COperator *popPattern = pexprPattern->Pop();
	if (popPattern->FPattern() && CPattern::PopConvert(popPattern)->FLeaf())
	{
		// return immediately; no deep extraction for leaf patterns
		pgexpr->Pop()->AddRef();
		return SPQOS_NEW(mp) CExpression(mp, pgexpr->Pop(), pgexpr);
	}

	// for a scalar operator, there is always only one group expression in it's
	// group. scalar operators are required to derive the scalar properties only
	// and no xforms are applied to them (i.e no PxfsCandidates in scalar op)
	// specifically which will generate equivalent scalar operators in the same group.
	// so, if a scalar op been extracted once, there is no need to explore
	// all the child bindings, as the scalar properites will remain the same.
	if (NULL != pexprLast && pgexpr->Pgroup()->FScalar())
	{
		// there is only 1 group expression in the scalar group
		SPQOS_ASSERT(1 == pgexpr->Pgroup()->UlGExprs());

		// the last operator and the current group expression will be same
		// for a scalar operator, as there is only one group expression in
		// the group
		SPQOS_ASSERT(pexprLast->Pop()->Eopid() == pgexpr->Pop()->Eopid());
		return NULL;
	}

	CExpressionArray *pdrspqexpr = NULL;
	ULONG arity = pgexpr->Arity();
	if (0 == arity && NULL != pexprLast)
	{
		// no more bindings
		return NULL;
	}
	else
	{
		// attempt binding to children
		pdrspqexpr = SPQOS_NEW(mp) CExpressionArray(mp);
		if (!FExtractChildren(mp, pgexpr, pexprPattern, pexprLast, pdrspqexpr))
		{
			pdrspqexpr->Release();
			return NULL;
		}
	}

	CExpression *pexpr = PexprFinalize(mp, pgexpr, pdrspqexpr);
	SPQOS_ASSERT(NULL != pexpr);

	return pexpr;
}


//---------------------------------------------------------------------------
//	@function:
//		CBinding::FInitChildCursors
//
//	@doc:
//		Initialize cursors of child expressions
//
//---------------------------------------------------------------------------
BOOL
CBinding::FInitChildCursors(CMemoryPool *mp, CGroupExpression *pgexpr,
							CExpression *pexprPattern,
							CExpressionArray *pdrspqexpr)
{
	SPQOS_ASSERT(NULL != pexprPattern);
	SPQOS_ASSERT(NULL != pdrspqexpr);

	const ULONG arity = pgexpr->Arity();

	// grab first expression from each cursor
	for (ULONG ul = 0; ul < arity; ul++)
	{
		CGroup *pgroup = (*pgexpr)[ul];
		CExpression *pexprPatternChild =
			PexprExpandPattern(pexprPattern, ul, arity);
		CExpression *pexprNewChild = PexprExtract(mp, pgroup, pexprPatternChild,
												  NULL /*pexprLastChild*/);

		if (NULL == pexprNewChild)
		{
			// failure means we have no more expressions
			return false;
		}

		pdrspqexpr->Append(pexprNewChild);
	}

	return true;
}


//---------------------------------------------------------------------------
//	@function:
//		CBinding::FAdvanceChildCursors
//
//	@doc:
//		Advance cursors of child expressions and populate the given array
//		with the next child expressions
//
//---------------------------------------------------------------------------
BOOL
CBinding::FAdvanceChildCursors(CMemoryPool *mp, CGroupExpression *pgexpr,
							   CExpression *pexprPattern,
							   CExpression *pexprLast,
							   CExpressionArray *pdrspqexpr)
{
	SPQOS_ASSERT(NULL != pexprPattern);
	SPQOS_ASSERT(NULL != pdrspqexpr);

	const ULONG arity = pgexpr->Arity();
	if (NULL == pexprLast)
	{
		// first call, initialize cursors
		return FInitChildCursors(mp, pgexpr, pexprPattern, pdrspqexpr);
	}

	// could we advance a child's cursor?
	BOOL fCursorAdvanced = false;

	// number of exhausted cursors
	ULONG ulExhaustedCursors = 0;

	for (ULONG ul = 0; ul < arity; ul++)
	{
		CGroup *pgroup = (*pgexpr)[ul];
		CExpression *pexprPatternChild =
			PexprExpandPattern(pexprPattern, ul, arity);
		CExpression *pexprNewChild = NULL;

		if (fCursorAdvanced)
		{
			// re-use last extracted child expression
			(*pexprLast)[ul]->AddRef();
			pexprNewChild = (*pexprLast)[ul];
		}
		else
		{
			CExpression *pexprLastChild = (*pexprLast)[ul];
			SPQOS_ASSERT(pgroup == pexprLastChild->Pgexpr()->Pgroup());

			// advance current cursor
			pexprNewChild =
				PexprExtract(mp, pgroup, pexprPatternChild, pexprLastChild);

			if (NULL == pexprNewChild)
			{
				// cursor is exhausted, we need to reset it
				pexprNewChild = PexprExtract(mp, pgroup, pexprPatternChild,
											 NULL /*pexprLastChild*/);
				ulExhaustedCursors++;
			}
			else
			{
				// advancing current cursor has succeeded
				fCursorAdvanced = true;
			}
		}
		SPQOS_ASSERT(NULL != pexprNewChild);

		pdrspqexpr->Append(pexprNewChild);
	}

	SPQOS_ASSERT(ulExhaustedCursors <= arity);


	return ulExhaustedCursors < arity;
}


//---------------------------------------------------------------------------
//	@function:
//		CBinding::FExtractChildren
//
//	@doc:
//		For a given root, extract children into a dynamic array;
//		Allocates the array for the children as needed;
//
//---------------------------------------------------------------------------
BOOL
CBinding::FExtractChildren(CMemoryPool *mp, CGroupExpression *pgexpr,
						   CExpression *pexprPattern, CExpression *pexprLast,
						   CExpressionArray *pdrspqexpr)
{
	SPQOS_CHECK_STACK_SIZE;
	SPQOS_CHECK_ABORT;

	SPQOS_ASSERT(NULL != pexprPattern);
	SPQOS_ASSERT(NULL != pdrspqexpr);
	SPQOS_ASSERT_IMP(pexprPattern->Pop()->FPattern(),
					!CPattern::PopConvert(pexprPattern->Pop())->FLeaf());
	SPQOS_ASSERT(pexprPattern->FMatchPattern(pgexpr));

	ULONG arity = pgexpr->Arity();
	if (arity < pexprPattern->Arity())
	{
		// does not have enough children
		return false;
	}

	if (0 == arity)
	{
		SPQOS_ASSERT(0 == pexprPattern->Arity());
		return true;
	}


	return FAdvanceChildCursors(mp, pgexpr, pexprPattern, pexprLast, pdrspqexpr);
}


//---------------------------------------------------------------------------
//	@function:
//		CBinding::PexprExtract
//
//	@doc:
//		Extract a binding according to a given pattern;
//		If no appropriate child pattern can be matched advance the root node
//		until group is exhausted;
//
//---------------------------------------------------------------------------
CExpression *
CBinding::PexprExtract(CMemoryPool *mp, CGroup *pgroup,
					   CExpression *pexprPattern, CExpression *pexprLast)
{
	SPQOS_CHECK_STACK_SIZE;
	SPQOS_CHECK_ABORT;

	SPQOS_ASSERT(NULL != pgroup);
	SPQOS_ASSERT(NULL != pexprPattern);

	CGroupExpression *pgexpr = NULL;
	if (NULL != pexprLast)
	{
		pgexpr = pexprLast->Pgexpr();
	}
	else
	{
		// init cursor
		pgexpr = PgexprNext(pgroup, NULL);
	}
	SPQOS_ASSERT(NULL != pgexpr);

	COperator *popPattern = pexprPattern->Pop();
	if (popPattern->FPattern() && CPattern::PopConvert(popPattern)->FLeaf())
	{
		// for leaf patterns, we do not iterate on group expressions
		if (NULL != pexprLast)
		{
			// if a leaf was extracted before, then group is exhausted
			return NULL;
		}

		return PexprExtract(mp, pgexpr, pexprPattern, pexprLast);
	}

	// start position for next binding
	CExpression *pexprStart = pexprLast;
	do
	{
		if (pexprPattern->FMatchPattern(pgexpr))
		{
			CExpression *pexprResult =
				PexprExtract(mp, pgexpr, pexprPattern, pexprStart);
			if (NULL != pexprResult)
			{
				return pexprResult;
			}
		}

		// move cursor and reset start position
		pgexpr = PgexprNext(pgroup, pgexpr);
		pexprStart = NULL;

		SPQOS_CHECK_ABORT;
	} while (NULL != pgexpr);

	// group exhausted
	return NULL;
}


// EOF
