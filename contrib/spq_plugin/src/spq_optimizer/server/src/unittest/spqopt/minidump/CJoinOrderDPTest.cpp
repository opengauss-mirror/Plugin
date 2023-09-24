//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2016 Pivotal Software
//
//	@filename:
//		CJoinOrderDPTest.cpp
//
//	@doc:
//		Testing guc for disabling dynamic join order algorithm
//---------------------------------------------------------------------------
#include "unittest/spqopt/minidump/CJoinOrderDPTest.h"

#include "unittest/spqopt/CTestUtils.h"



//---------------------------------------------------------------------------
//	@function:
//		CJoinOrderDPTest::EresUnittest
//
//	@doc:
//		Unittest for testing guc for disabling dynamic join order algorithm
//
//---------------------------------------------------------------------------
spqos::SPQOS_RESULT
CJoinOrderDPTest::EresUnittest()
{
	ULONG ulTestCounter = 0;
	const CHAR *rgszFileNames[] = {
		"../data/dxl/minidump/CJoinOrderDPTest/JoinOrderWithDP.mdp",
		"../data/dxl/minidump/CJoinOrderDPTest/JoinOrderWithOutDP.mdp",
		"../data/dxl/minidump/JoinOptimizationLevelQuery3WayHashJoinPartTbl.mdp"};

	return CTestUtils::EresUnittest_RunTestsWithoutAdditionalTraceFlags(
		rgszFileNames, &ulTestCounter, SPQOS_ARRAY_SIZE(rgszFileNames), true,
		true);
}
// EOF
