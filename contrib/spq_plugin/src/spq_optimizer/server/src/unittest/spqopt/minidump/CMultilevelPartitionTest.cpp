//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2015 Pivotal, Inc.
//
//	@filename:
//		CMultilevelPartitionTest.cpp
//
//	@doc:
//		Test for optimizing queries on multilevel partitioned tables
//---------------------------------------------------------------------------

#include "unittest/spqopt/minidump/CMultilevelPartitionTest.h"

#include "spqos/base.h"
#include "spqos/memory/CAutoMemoryPool.h"
#include "spqos/task/CAutoTraceFlag.h"
#include "spqos/test/CUnittest.h"

#include "spqopt/exception.h"
#include "spqopt/minidump/CMinidumperUtils.h"

#include "unittest/spqopt/CTestUtils.h"

using namespace spqopt;

ULONG CMultilevelPartitionTest::m_ulMLPTTestCounter =
	0;	// start from first test

// minidump files
const CHAR *rgszMultilevel[] = {
	"../data/dxl/multilevel-partitioning/Multilevel-Casting-cast_boundary_value_to_date.mdp",
	"../data/dxl/multilevel-partitioning/Multilevel-Casting-cast_partition_column_to_text.mdp",
	"../data/dxl/multilevel-partitioning/Multilevel-Casting-no_casting.mdp",
	"../data/dxl/multilevel-partitioning/Multilevel-Casting-predicate-on-all-levels.mdp",
	"../data/dxl/multilevel-partitioning/Multilevel-Casting-predicate-on-non-leaf-levels.mdp",
	"../data/dxl/multilevel-partitioning/Multilevel-Casting-predicate-on-non-root-levels.mdp",
	"../data/dxl/multilevel-partitioning/Multilevel-FullScan.mdp",
	"../data/dxl/multilevel-partitioning/Multilevel-ConstPred-Level1-NoDefault.mdp",
	"../data/dxl/multilevel-partitioning/Multilevel-ConstPred-Level1-Default.mdp",
	"../data/dxl/multilevel-partitioning/Multilevel-ConstPred-Level2-NoDefault.mdp",
	"../data/dxl/multilevel-partitioning/Multilevel-ConstPred-Level2-Default.mdp",
	"../data/dxl/multilevel-partitioning/Multilevel-ConstPred-AllLevels-NoDefault.mdp",
	"../data/dxl/multilevel-partitioning/Multilevel-ConstPred-AllLevels-Default.mdp",
	"../data/dxl/multilevel-partitioning/Multilevel-JoinPred-Level1.mdp",
	"../data/dxl/multilevel-partitioning/Multilevel-JoinPred-Level2.mdp",
	"../data/dxl/multilevel-partitioning/Multilevel-JoinPred-AllLevels.mdp",
	"../data/dxl/multilevel-partitioning/Multilevel-Nary-Join.mdp",
};

//---------------------------------------------------------------------------
//	@function:
//		CMultilevelPartitionTest::EresUnittest
//
//	@doc:
//		Unittest for expressions
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CMultilevelPartitionTest::EresUnittest()
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
//		CMultilevelPartitionTest::EresUnittest_RunTests
//
//	@doc:
//		Run all Minidump-based tests with plan matching
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CMultilevelPartitionTest::EresUnittest_RunTests()
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	const ULONG ulTests = SPQOS_ARRAY_SIZE(rgszMultilevel);

	return CTestUtils::EresRunMinidumps(mp, rgszMultilevel, ulTests,
										&m_ulMLPTTestCounter,
										1,	   // ulSessionId
										1,	   // ulCmdId
										true,  // fMatchPlans
										true   // fTestSpacePruning
	);
}

// EOF
