//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2016 Pivotal, Inc.
//
//	@filename:
//		CPruneColumnsTest.cpp
//
//	@doc:
//		Test for optimizing queries where intermediate columns are pruned
//---------------------------------------------------------------------------

#include "unittest/spqopt/minidump/CPruneColumnsTest.h"

#include "spqos/base.h"
#include "spqos/memory/CAutoMemoryPool.h"
#include "spqos/task/CAutoTraceFlag.h"
#include "spqos/test/CUnittest.h"

#include "spqopt/exception.h"
#include "spqopt/minidump/CMinidumperUtils.h"

#include "unittest/spqopt/CTestUtils.h"


using namespace spqopt;

ULONG CPruneColumnsTest::m_ulPruneColumnsTestCounter =
	0;	// start from first test

// minidump files
const CHAR *rgszPruneColumnsFileNames[] = {
	"../data/dxl/minidump/RemoveUnusedProjElementsInGbAgg.mdp",
	"../data/dxl/minidump/RemoveUnusedProjElements.mdp",
	"../data/dxl/minidump/CPruneColumnsTest/PruneIntermediateUnusedColumns.mdp",  // prune all unused columns
	"../data/dxl/minidump/CPruneColumnsTest/AggTopOfSingleSetRetFuncs.mdp",	 // no pruning done
	"../data/dxl/minidump/CPruneColumnsTest/AggTopOfSetRetFuncsAndUnusedScalar.mdp",  // partial pruning
	"../data/dxl/minidump/CPruneColumnsTest/AggTopOfMultipleSetRetFuncs.mdp",  // no pruning done
	"../data/dxl/minidump/CPruneColumnsTest/AggTopOfMultipleSetRetFuncsAndUnusedScalar.mdp",  // partial prune
	"../data/dxl/minidump/CPruneColumnsTest/AggTopOfSetRefFuncsOnTopTbl.mdp",  // no pruning done
	"../data/dxl/minidump/CPruneColumnsTest/AllColsUsed.mdp",  // no pruning done
	"../data/dxl/minidump/CPruneColumnsTest/UsedSetRetFuncAndUnusedScalarFunc.mdp",	 // partial pruning
	"../data/dxl/minidump/CPruneColumnsTest/UnusedSetRetFuncAndUsedScalarFunc.mdp",	 // partial pruning ---> BUG
	"../data/dxl/minidump/CPruneColumnsTest/MultiLevelSubqueryWithSetRetFuncs.mdp",	 // expect error in optimizer and planner: "Expected no more than one row to be returned by expression"
	"../data/dxl/minidump/CPruneColumnsTest/MultiLevelSubqueryWithSetRetFuncsAndScalarFuncs.mdp",  // if pruning subquery is handled then we should prune some columns
};


//---------------------------------------------------------------------------
//	@function:
//		CPruneColumnsTest::EresUnittest
//
//	@doc:
//		Unittest for expressions
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CPruneColumnsTest::EresUnittest()
{
	CUnittest rgut[] = {
		SPQOS_UNITTEST_FUNC(EresUnittest_RunTests),
	};

	SPQOS_RESULT eres = CUnittest::EresExecute(rgut, SPQOS_ARRAY_SIZE(rgut));

	// reset metadata cache
	CMDCache::Reset();

	return eres;
}

//---------------------------------------------------------------------------
//	@function:
//		CPruneColumnsTest::EresUnittest_RunTests
//
//	@doc:
//		Run all Minidump-based tests with plan matching
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CPruneColumnsTest::EresUnittest_RunTests()
{
	return CTestUtils::EresUnittest_RunTests(
		rgszPruneColumnsFileNames, &m_ulPruneColumnsTestCounter,
		SPQOS_ARRAY_SIZE(rgszPruneColumnsFileNames));
}

// EOF
