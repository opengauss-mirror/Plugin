//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2015 Pivotal, Inc.
//
//	@filename:
//		CBitmapTest.cpp
//
//	@doc:
//		Test for optimizing queries that can use a bitmap index
//---------------------------------------------------------------------------

#include "unittest/spqopt/minidump/CBitmapTest.h"

#include "spqos/base.h"
#include "spqos/memory/CAutoMemoryPool.h"
#include "spqos/task/CAutoTraceFlag.h"
#include "spqos/test/CUnittest.h"

#include "spqopt/exception.h"
#include "spqopt/minidump/CMinidumperUtils.h"

#include "unittest/spqopt/CTestUtils.h"

using namespace spqopt;

ULONG CBitmapTest::m_ulBitmapTestCounter = 0;  // start from first test

// minidump files
const CHAR *rgszBitmapFileNames[] = {
	"../data/dxl/minidump/BitmapIndexScan.mdp",
	"../data/dxl/minidump/BitmapIndexScanCost.mdp",
	"../data/dxl/minidump/DynamicBitmapIndexScan.mdp",
	"../data/dxl/minidump/BitmapTableScan-AO.mdp",
	"../data/dxl/minidump/BitmapTableScan-Basic.mdp",
	"../data/dxl/minidump/BitmapTableScan-ColumnOnRightSide.mdp",
	"../data/dxl/minidump/BitmapTableScan-AndCondition.mdp",
	"../data/dxl/minidump/BitmapIndexProbeMergeFilters.mdp",
	"../data/dxl/minidump/DynamicBitmapTableScan-Basic.mdp",
	"../data/dxl/minidump/DynamicBitmapTableScan-Heterogeneous.mdp",
	"../data/dxl/minidump/DynamicBitmapTableScan-UUID.mdp",
	"../data/dxl/minidump/BitmapBoolOr.mdp",
	"../data/dxl/minidump/BitmapBoolOr-BoolColumn.mdp",
	"../data/dxl/minidump/BitmapBoolAnd.mdp",
	"../data/dxl/minidump/BitmapBoolOp-DeepTree.mdp",
	"../data/dxl/minidump/BitmapBoolOp-DeepTree2.mdp",
	"../data/dxl/minidump/BitmapBoolOp-DeepTree3.mdp",
	"../data/dxl/minidump/DynamicBitmapBoolOp.mdp",
	"../data/dxl/minidump/BitmapIndexScan-WithUnsupportedOperatorFilter.mdp",
	"../data/dxl/minidump/BitmapTableScan-AO-Btree.mdp",
	"../data/dxl/minidump/BitmapIndexApply-Basic-SelfJoin.mdp",
	"../data/dxl/minidump/BitmapIndexApply-Basic-TwoTables.mdp",
	"../data/dxl/minidump/BitmapIndexApply-Complex-Condition.mdp",
	"../data/dxl/minidump/BitmapIndexApply-PartTable.mdp",
	"../data/dxl/minidump/BitmapIndexApply-InnerSelect-Basic.mdp",
	"../data/dxl/minidump/BitmapIndexApply-InnerSelect-PartTable.mdp",
	"../data/dxl/minidump/PredicateWithConjunctsAndDisjuncts.mdp",
	"../data/dxl/minidump/PredicateWithConjunctsOfDisjuncts.mdp",
	"../data/dxl/minidump/PredicateWithLongConjunction.mdp",
	"../data/dxl/minidump/MultipleIndexPredicate.mdp",
	"../data/dxl/minidump/BitmapIndexScanChooseIndex.mdp",
	"../data/dxl/minidump/DynamicBitmapTableScan-PartAO-SemiJoin.mdp",
};

//---------------------------------------------------------------------------
//	@function:
//		CBitmapTest::EresUnittest
//
//	@doc:
//		Unittest for expressions
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CBitmapTest::EresUnittest()
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
//		CBitmapTest::EresUnittest_RunTests
//
//	@doc:
//		Run all Minidump-based tests with plan matching
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CBitmapTest::EresUnittest_RunTests()
{
	return CTestUtils::EresUnittest_RunTests(
		rgszBitmapFileNames, &m_ulBitmapTestCounter,
		SPQOS_ARRAY_SIZE(rgszBitmapFileNames));
}

// EOF
