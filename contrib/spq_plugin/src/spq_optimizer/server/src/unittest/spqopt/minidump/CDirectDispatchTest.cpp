//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2015 Pivotal, Inc.
//
//	@filename:
//		CDirectDispatchTest.cpp
//
//	@doc:
//		Test for direct dispatch
//---------------------------------------------------------------------------

#include "unittest/spqopt/minidump/CDirectDispatchTest.h"

#include "spqos/base.h"
#include "spqos/memory/CAutoMemoryPool.h"
#include "spqos/task/CAutoTraceFlag.h"
#include "spqos/test/CUnittest.h"

#include "spqopt/exception.h"
#include "spqopt/minidump/CMinidumperUtils.h"

#include "unittest/spqopt/CTestUtils.h"

using namespace spqopt;

ULONG CDirectDispatchTest::m_ulDirectDispatchCounter =
	0;	// start from first test

// minidump files
const CHAR *rgszDirectDispatchFileNames[] = {
	"../data/dxl/minidump/DirectDispatch-SingleCol.mdp",
	"../data/dxl/minidump/DirectDispatch-GpSegmentId.mdp",
	"../data/dxl/minidump/DirectDispatch-GpSegmentId-SingleCol-Conjunction.mdp",
	"../data/dxl/minidump/DirectDispatch-GpSegmentId-MultiCol-Conjunction.mdp",
	"../data/dxl/minidump/DirectDispatch-SingleCol-Disjunction.mdp",
	"../data/dxl/minidump/DirectDispatch-SingleCol-Disjunction-IsNull.mdp",
	"../data/dxl/minidump/DirectDispatch-SingleCol-Disjunction-Negative.mdp",
	"../data/dxl/minidump/DirectDispatch-MultiCol.mdp",
	"../data/dxl/minidump/DirectDispatch-MultiCol-Disjunction.mdp",
	"../data/dxl/minidump/DirectDispatch-MultiCol-Negative.mdp",
	"../data/dxl/minidump/DirectDispatch-IndexScan.mdp",
	"../data/dxl/minidump/DirectDispatch-DynamicIndexScan.mdp",
	"../data/dxl/minidump/InsertDirectedDispatchNullValue.mdp",
	"../data/dxl/minidump/DirectDispatch-RandDistTable.mdp",
	"../data/dxl/minidump/DirectDispatch-RandDistTable-Disjunction.mdp",
};

//---------------------------------------------------------------------------
//	@function:
//		CDirectDispatchTest::EresUnittest
//
//	@doc:
//		Unittest for expressions
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CDirectDispatchTest::EresUnittest()
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
//		CDirectDispatchTest::EresUnittest_RunTests
//
//	@doc:
//		Run all Minidump-based tests with plan matching
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CDirectDispatchTest::EresUnittest_RunTests()
{
	return CTestUtils::EresUnittest_RunTests(
		rgszDirectDispatchFileNames, &m_ulDirectDispatchCounter,
		SPQOS_ARRAY_SIZE(rgszDirectDispatchFileNames));
}

// EOF
