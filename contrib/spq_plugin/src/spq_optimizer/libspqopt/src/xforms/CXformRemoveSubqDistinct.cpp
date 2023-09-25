//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2017 Pivotal Software Inc.
//
//	@filename:
//		CXformRemoveSubqDistinct.cpp
//
//	@doc:
//		Implementation of the transform that removes distinct clause from subquery
//---------------------------------------------------------------------------

#include "spqopt/xforms/CXformRemoveSubqDistinct.h"

#include "spqos/base.h"

#include "spqopt/operators/CLogicalSelect.h"
#include "spqopt/operators/COperator.h"
#include "spqopt/operators/CPatternLeaf.h"
#include "spqopt/search/CGroupProxy.h"
#include "spqopt/xforms/CXformUtils.h"

using namespace spqopt;

CXformRemoveSubqDistinct::CXformRemoveSubqDistinct(CMemoryPool *mp)
	:  // pattern
	  CXformExploration(SPQOS_NEW(mp) CExpression(
		  mp, SPQOS_NEW(mp) CLogicalSelect(mp),
		  SPQOS_NEW(mp) CExpression(
			  mp, SPQOS_NEW(mp) CPatternLeaf(mp)),  // relational child
		  SPQOS_NEW(mp)
			  CExpression(mp, SPQOS_NEW(mp) CPatternTree(mp))  // predicate tree
		  ))
{
}

CXform::EXformPromise
CXformRemoveSubqDistinct::Exfp(CExpressionHandle &exprhdl) const
{
	// consider this transformation only if subqueries exist
	if (!exprhdl.DeriveHasSubquery(1))
	{
		return CXform::ExfpNone;
	}

	CGroupProxy spq((*exprhdl.Pgexpr())[1]);
	CGroupExpression *pexprScalar = spq.PgexprFirst();
	COperator *pop = pexprScalar->Pop();
	if (CUtils::FQuantifiedSubquery(pop) || CUtils::FExistentialSubquery(pop))
	{
		return CXform::ExfpHigh;
	}

	return CXform::ExfpNone;
}

// For quantified subqueries (IN / NOT IN), the following transformation will be applied:
// CLogicalSelect
// |--CLogicalGet "foo"
// +--CScalarSubqueryAny(=)["c" (9)]
//    |--CLogicalGbAgg( Global ) Grp Cols: ["c" (9)]
//    |  |--CLogicalGet "bar"
//    |  +--CScalarProjectList
//    +--CScalarIdent "a" (0)
//
// will produce
//
// CLogicalSelect
// |--CLogicalGet "foo"
// +--CScalarSubqueryAny(=)["c" (9)]
//    |--CLogicalGet "bar"
//    +--CScalarIdent "a" (0)
//
// For existential subqueries (EXISTS / NOT EXISTS), the following transformation will be applied:
// CLogicalSelect
// |--CLogicalGet "foo"
// +--CScalarSubqueryExists
//    +--CLogicalGbAgg( Global ) Grp Cols: ["c" (9)]
//       |--CLogicalGet "bar"
//       +--CScalarProjectList
//
// will produce
//
// CLogicalSelect
// |--CLogicalGet "foo"
// +--CScalarSubqueryExists
//    +--CLogicalGet "bar"
//
void
CXformRemoveSubqDistinct::Transform(CXformContext *pxfctxt,
									CXformResult *pxfres,
									CExpression *pexpr) const
{
	SPQOS_ASSERT(NULL != pxfctxt);
	SPQOS_ASSERT(NULL != pxfres);
	SPQOS_ASSERT(FCheckPattern(pexpr));

	CMemoryPool *mp = pxfctxt->Pmp();
	CExpression *pexprScalar = (*pexpr)[1];
	CExpression *pexprGbAgg = (*pexprScalar)[0];

	if (COperator::EopLogicalGbAgg == pexprGbAgg->Pop()->Eopid())
	{
		CExpression *pexprGbAggProjectList = (*pexprGbAgg)[1];
		// only consider removing distinct when there is no aggregation functions
		if (0 == pexprGbAggProjectList->Arity())
		{
			CExpression *pexprNewScalar = NULL;
			CExpression *pexprRelChild = (*pexprGbAgg)[0];
			pexprRelChild->AddRef();

			COperator *pop = pexprScalar->Pop();
			pop->AddRef();
			if (CUtils::FExistentialSubquery(pop))
			{
				// EXIST/NOT EXIST scalar subquery
				pexprNewScalar =
					SPQOS_NEW(mp) CExpression(mp, pop, pexprRelChild);
			}
			else
			{
				// IN/NOT IN scalar subquery
				CExpression *pexprScalarIdent = (*pexprScalar)[1];
				pexprScalarIdent->AddRef();
				pexprNewScalar = SPQOS_NEW(mp)
					CExpression(mp, pop, pexprRelChild, pexprScalarIdent);
			}

			pexpr->Pop()->AddRef();	 // logical select operator
			(*pexpr)[0]->AddRef();	 // relational child of logical select

			// new logical select expression
			CExpression *ppexprNew = SPQOS_NEW(mp)
				CExpression(mp, pexpr->Pop(), (*pexpr)[0], pexprNewScalar);
			pxfres->Add(ppexprNew);
		}
	}
}

// EOF
