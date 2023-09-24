//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CExpressionPreprocessorTest.h
//
//	@doc:
//		Test for expression preprocessing
//---------------------------------------------------------------------------
#ifndef SPQOPT_CExpressionPreprocessorTest_H
#define SPQOPT_CExpressionPreprocessorTest_H

#include "spqos/base.h"

#include "spqopt/operators/CExpression.h"
#include "spqopt/operators/CScalarBoolOp.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CExpressionPreprocessorTest
//
//	@doc:
//		Unittests
//
//---------------------------------------------------------------------------
class CExpressionPreprocessorTest
{
private:
	// shorthand for functions for generating the expression for unnest test
	typedef CExpression *(FnPexprUnnestTestCase)(CMemoryPool *mp,
												 CExpression *pexpr);

	// unnest scalar subqueries test cases
	struct SUnnestSubqueriesTestCase
	{
		// generator function of first expression
		FnPexprUnnestTestCase *m_pfFst;

		// generator function of second expression
		FnPexprUnnestTestCase *m_pfSnd;

		// AND/OR boolean operator
		CScalarBoolOp::EBoolOperator m_eboolop;

		// operator that should be present after unnesting
		COperator::EOperatorId m_eopidPresent;

		// negate the children of or/and
		BOOL m_fNegateChildren;
	};	// SUnnestSubqueriesTestCase

	// count number of scalar subqueries
	static ULONG UlScalarSubqs(CExpression *pexpr);

	// check if a given expression has a subquery exists node
	static BOOL FHasSubqueryExists(CExpression *pexpr);

	// check if a given expression has a subquery not exitst node
	static BOOL FHasSubqueryNotExists(CExpression *pexpr);

	// check if a given expression has an ALL subquery
	static BOOL FHasSubqueryAll(CExpression *pexpr);

	// check if a given expression has an ANY subquery
	static BOOL FHasSubqueryAny(CExpression *pexpr);

	// check the type of the subquery
	static SPQOS_RESULT EresCheckSubqueryType(
		CExpression *pexpr, COperator::EOperatorId eopidPresent);

	static
		// check the type of the existential subquery
		SPQOS_RESULT
		EresCheckExistsSubqueryType(CExpression *pexpr,
									COperator::EOperatorId eopidPresent);

	// check the type of the quantified subquery
	static SPQOS_RESULT EresCheckQuantifiedSubqueryType(
		CExpression *pexpr, COperator::EOperatorId eopidPresent);

#ifdef SPQOS_DEBUG
	// check if a given expression has no Outer Join nodes
	static BOOL FHasNoOuterJoin(CExpression *pexpr);

	// check if a given expression has outer references in any node
	static BOOL HasOuterRefs(CExpression *pexpr);

	// check if a given expression has Sequence Project nodes
	static BOOL FHasSeqPrj(CExpression *pexpr);

	// check if a given expression has IS DISTINCT FROM nodes
	static BOOL FHasIDF(CExpression *pexpr);

#endif	// SPQOS_DEBUG

	// create a conjunction of comparisons using the given columns
	static CExpression *PexprCreateConjunction(CMemoryPool *mp,
											   CColRefArray *colref_array);

	// Helper for preprocessing window functions with outer references
	static void PreprocessWinFuncWithOuterRefs(const CHAR *szFilePath,
											   BOOL fAllowWinFuncOuterRefs);

	// Helper for preprocessing window functions with distinct aggs
	static void PreprocessWinFuncWithDistinctAggs(CMemoryPool *mp,
												  const CHAR *szFilePath,
												  BOOL fAllowSeqPrj,
												  BOOL fAllowIDF);

	// Helper for preprocessing outer joins
	static void PreprocessOuterJoin(const CHAR *szFilePath,
									BOOL fAllowOuterJoin);

	// helper function for testing collapse of Inner Joins
	static SPQOS_RESULT EresUnittest_CollapseInnerJoinHelper(
		CMemoryPool *mp, COperator *popJoin, CExpression *rspqexpr[],
		CDrvdPropRelational *rspqdprel[]);

	// helper function for testing cascaded inner/outer joins
	static CExpression *PexprJoinHelper(CMemoryPool *mp, CExpression *pexprLOJ,
										BOOL fCascadedLOJ,
										BOOL fIntermediateInnerjoin);

	// helper function for testing window functions with outer join
	static CExpression *PexprWindowFuncWithLOJHelper(
		CMemoryPool *mp, CExpression *pexprLOJ, CColRef *pcrPartitionBy,
		BOOL fAddWindowFunction, BOOL fOuterChildPred, BOOL fCascadedLOJ,
		BOOL fPredBelowWindow);

	// helper function for testing Select with outer join
	static CExpression *PexprSelectWithLOJHelper(CMemoryPool *mp,
												 CExpression *pexprLOJ,
												 CColRef *colref,
												 BOOL fOuterChildPred,
												 BOOL fCascadedLOJ);

	// helper function for comparing expressions resulting from preprocessing window functions with outer join
	static SPQOS_RESULT EresCompareExpressions(CMemoryPool *mp,
											  CWStringDynamic *rgstr[],
											  ULONG size);

	// test case generator for outer joins
	static SPQOS_RESULT EresTestLOJ(BOOL fAddWindowFunction);

	// helper to create an expression with a predicate containing an array and other comparisons
	static CExpression *PexprCreateConvertableArray(CMemoryPool *mp,
													BOOL fCreateInStatement);

public:
	// unittests
	static SPQOS_RESULT EresUnittest();
	static SPQOS_RESULT EresUnittest_PreProcess();
	static SPQOS_RESULT EresUnittest_InferPredsOnLOJ();
	static SPQOS_RESULT EresUnittest_PreProcessWindowFunc();
	static SPQOS_RESULT EresUnittest_PreProcessWindowFuncWithLOJ();
	static SPQOS_RESULT EresUnittest_PreProcessWindowFuncWithOuterRefs();
	static SPQOS_RESULT EresUnittest_PreProcessWindowFuncWithDistinctAggs();
	static SPQOS_RESULT EresUnittest_PreProcessNestedScalarSubqueries();
	static SPQOS_RESULT EresUnittest_UnnestSubqueries();
	static SPQOS_RESULT EresUnittest_PreProcessOuterJoin();
	static SPQOS_RESULT EresUnittest_PreProcessOuterJoinMinidumps();
	static SPQOS_RESULT EresUnittest_PreProcessOrPrefilters();
	static SPQOS_RESULT EresUnittest_PreProcessOrPrefiltersPartialPush();
	static SPQOS_RESULT EresUnittest_CollapseInnerJoin();
	static SPQOS_RESULT EresUnittest_PreProcessConvert2InPredicate();
	static SPQOS_RESULT
	EresUnittest_PreProcessConvert2InPredicateDeepExpressionTree();
	static SPQOS_RESULT EresUnittest_PreProcessConvertArrayWithEquals();

};	// class CExpressionPreprocessorTest
}  // namespace spqopt

#endif	// !SPQOPT_CExpressionPreprocessorTest_H

// EOF
