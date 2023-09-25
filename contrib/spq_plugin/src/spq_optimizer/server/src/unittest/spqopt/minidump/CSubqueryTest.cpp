//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2017 Pivotal, Inc.
//
//	@filename:
//		CSubqueryTest.cpp
//
//	@doc:
//		Test for subquery optimization
//---------------------------------------------------------------------------

#include "unittest/spqopt/minidump/CSubqueryTest.h"

#include "spqos/base.h"
#include "spqos/memory/CAutoMemoryPool.h"
#include "spqos/task/CAutoTraceFlag.h"
#include "spqos/test/CUnittest.h"

#include "spqopt/exception.h"
#include "spqopt/minidump/CMinidumperUtils.h"

#include "unittest/spqopt/CTestUtils.h"


using namespace spqopt;

ULONG CSubqueryTest::m_ulSubQueryTestCounter = 0;  // start from first test

// minidump files
const CHAR *rgszSubqueryFileNames[] = {
	"../data/dxl/minidump/SubqExists-With-External-Corrs.mdp",
	"../data/dxl/minidump/SubqExists-Without-External-Corrs.mdp",
	"../data/dxl/minidump/Exists-SuperfluousEquality.mdp",
	"../data/dxl/minidump/NotExists-SuperfluousEquality.mdp",
	"../data/dxl/minidump/SimplifyExistsSubquery2Limit.mdp",
	"../data/dxl/minidump/NotExists-SuperflousOuterRefWithGbAgg.mdp",
	"../data/dxl/minidump/Remove-Distinct-From-Subquery.mdp",
	"../data/dxl/minidump/MultiLevel-NOT-IN-Subquery.mdp",
	"../data/dxl/minidump/Coalesce-With-Subquery.mdp",
	"../data/dxl/minidump/NullIf-With-Subquery.mdp",
	"../data/dxl/minidump/Switch-With-Subquery.mdp",
	"../data/dxl/minidump/MultiLevelDecorrelationWithSemiJoins.mdp",
	"../data/dxl/minidump/UnnestSQJoins.mdp",
	"../data/dxl/minidump/MultiLevel-CorrelatedExec.mdp",
	"../data/dxl/minidump/OneLevel-CorrelatedExec.mdp",
	"../data/dxl/minidump/MultiLevel-IN-Subquery.mdp",
	"../data/dxl/minidump/Stats-For-Select-With-Outer-Refs.mdp",
	"../data/dxl/minidump/Subq-On-OuterRef.mdp",
	"../data/dxl/minidump/Subq-With-OuterRefCol.mdp",
	"../data/dxl/minidump/Subq-NoParams.mdp",
	"../data/dxl/minidump/Subq-JoinWithOuterRef.mdp",
	"../data/dxl/minidump/SubqAll-To-ScalarSubq.mdp",
	"../data/dxl/minidump/SubqAll-Limit1.mdp",
	"../data/dxl/minidump/ProjectUnderSubq.mdp",
	"../data/dxl/minidump/MultipleSubqueriesInSelectClause.mdp",
	"../data/dxl/minidump/AssertMaxOneRow.mdp",
	"../data/dxl/minidump/AssertOneRowWithCorrelation.mdp",
	"../data/dxl/minidump/OrderByOuterRef.mdp",
	"../data/dxl/minidump/ScSubqueryWithOuterRef.mdp",
	"../data/dxl/minidump/ExprOnScSubqueryWithOuterRef.mdp",
	"../data/dxl/minidump/SubqEnforceSubplan.mdp",
	"../data/dxl/minidump/SubqAll-InsideScalarExpression.mdp",
	"../data/dxl/minidump/SubqAny-InsideScalarExpression.mdp",
	"../data/dxl/minidump/ExistentialSubquriesInsideScalarExpression.mdp",
	"../data/dxl/minidump/SubqueryInsideScalarIf.mdp",
	"../data/dxl/minidump/SubqueryNullTest.mdp",
	"../data/dxl/minidump/ExistsSubqInsideExpr.mdp",
	"../data/dxl/minidump/AnySubq-With-NonScalarSubqueryChild-1.mdp",
	"../data/dxl/minidump/AnySubq-With-NonScalarSubqueryChild-2.mdp",
	"../data/dxl/minidump/AnySubq-With-NonScalarSubqueryChild-3.mdp",
	"../data/dxl/minidump/InSubqueryWithNotNullCol.mdp",
	"../data/dxl/minidump/AnySubqueryWithVolatileComp.mdp",
	"../data/dxl/minidump/AnySubqueryWithVolatileFunc.mdp",
	"../data/dxl/minidump/SubqueryOuterRefTVF.mdp",
	"../data/dxl/minidump/SubqueryOuterRefLimit.mdp",
	"../data/dxl/minidump/Subquery-ExistsAllAggregates.mdp",
	"../data/dxl/minidump/Subquery-ExistsAllAggregatesWithDisjuncts.mdp",
	"../data/dxl/minidump/Subquery-AnyAllAggregates.mdp",
	"../data/dxl/minidump/Join-With-Subq-Preds-2.mdp",
};


// unittest for expressions
SPQOS_RESULT
CSubqueryTest::EresUnittest()
{
	CUnittest rgut[] = {
		SPQOS_UNITTEST_FUNC(EresUnittest_RunTests),
	};

	SPQOS_RESULT eres = CUnittest::EresExecute(rgut, SPQOS_ARRAY_SIZE(rgut));

	// reset metadata cache
	CMDCache::Reset();

	return eres;
}

// run all Minidump-based tests with plan matching
SPQOS_RESULT
CSubqueryTest::EresUnittest_RunTests()
{
	return CTestUtils::EresUnittest_RunTests(
		rgszSubqueryFileNames, &m_ulSubQueryTestCounter,
		SPQOS_ARRAY_SIZE(rgszSubqueryFileNames));
}

// EOF
