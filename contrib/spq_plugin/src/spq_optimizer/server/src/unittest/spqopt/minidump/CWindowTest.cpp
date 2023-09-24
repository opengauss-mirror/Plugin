//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2016 Pivotal Software
//---------------------------------------------------------------------------
#include "unittest/spqopt/minidump/CWindowTest.h"

#include "unittest/spqopt/CTestUtils.h"



//---------------------------------------------------------------------------
//	@function:
//		CWindowTest::EresUnittest
//
//	@doc:
//		Unittest for Window functions
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CWindowTest::EresUnittest()
{
	ULONG ulTestCounter = 0;
	const CHAR *rgszWindowFunctionFileNames[] = {
		"../data/dxl/minidump/NoBroadcastUnderGatherForWindowFunction.mdp",
		"../data/dxl/minidump/WinFuncWithSubqArgs.mdp",
		"../data/dxl/minidump/WinFunc-Simple.mdp",
		"../data/dxl/minidump/WindowFrame-SingleEdged.mdp",
		"../data/dxl/minidump/Join-WinFunc-Preds.mdp",
		"../data/dxl/minidump/Lead-Lag-WinFuncs.mdp",
		"../data/dxl/minidump/Preds-Over-WinFunc1.mdp",
		"../data/dxl/minidump/Preds-Over-WinFunc2.mdp",
		"../data/dxl/minidump/Preds-Over-WinFunc3.mdp",
		"../data/dxl/minidump/Preds-Over-WinFunc4.mdp",
		"../data/dxl/minidump/Preds-Over-WinFunc5.mdp",
		"../data/dxl/minidump/window-count-spqdb6.mdp",
	};

	return CTestUtils::EresUnittest_RunTestsWithoutAdditionalTraceFlags(
		rgszWindowFunctionFileNames, &ulTestCounter,
		SPQOS_ARRAY_SIZE(rgszWindowFunctionFileNames), true, true);
}
// EOF
