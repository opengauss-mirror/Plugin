//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal, Inc.
//
//	@filename:
//		CExpressionUtils.h
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

#ifndef SPQOPT_CExpressionUtils_H
#define SPQOPT_CExpressionUtils_H

#include "spqos/base.h"

#include "spqopt/operators/CExpression.h"

namespace spqopt
{
using namespace spqos;

// fwd declarations
class CExpression;

//---------------------------------------------------------------------------
//	@class:
//		CExpressionUtils
//
//	@doc:
//		Utility routines for transforming expressions
//
//---------------------------------------------------------------------------
class CExpressionUtils
{
private:
	// unnest a given expression's child and append unnested nodes to given array
	static void UnnestChild(CMemoryPool *mp, CExpression *pexpr,
							ULONG UlChildIndex, BOOL fAnd, BOOL fOr,
							BOOL fNotChildren, CExpressionArray *pdrspqexpr);

	// append the unnested children of given expression to given array
	static void AppendChildren(CMemoryPool *mp, CExpression *pexpr,
							   CExpressionArray *pdrspqexpr);

	// return an array of expression children after being unnested
	static CExpressionArray *PdrspqexprUnnestChildren(CMemoryPool *mp,
													 CExpression *pexpr);

	// push not expression one level down the given expression
	static CExpression *PexprPushNotOneLevel(CMemoryPool *mp,
											 CExpression *pexpr);

public:
	// remove duplicate AND/OR children
	static CExpression *PexprDedupChildren(CMemoryPool *mp, CExpression *pexpr);

	// unnest AND/OR/NOT predicates
	static CExpression *PexprUnnest(CMemoryPool *mp, CExpression *pexpr);

	// get constraints property from LogicalSelect operator with EXISTS/ANY subquery
	static CPropConstraint *GetPropConstraintFromSubquery(CMemoryPool *mp,
														  CExpression *pexpr);
};
}  // namespace spqopt

#endif	// !SPQOPT_CExpressionUtils_H

// EOF
