//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2016 Pivotal Software
//
//	@filename:
//		CArrayExpansionTest.cpp
//
//	@doc:
//		Test for array expansion in WHERE clause
//---------------------------------------------------------------------------
#include "unittest/spqopt/minidump/CArrayExpansionTest.h"

#include "unittest/spqopt/CTestUtils.h"



//---------------------------------------------------------------------------
//	@function:
//		CArrayExpansionTest::EresUnittest
//
//	@doc:
//		Unittest for array expansion in WHERE clause
//
//---------------------------------------------------------------------------
spqos::SPQOS_RESULT
CArrayExpansionTest::EresUnittest()
{
	ULONG ulTestCounter = 0;
	const CHAR *rgszFileNames[] = {
		"../data/dxl/minidump/ArrayCmpInList.mdp",
		"../data/dxl/minidump/CArrayExpansionTest/JoinWithInListNoExpand.mdp",
		"../data/dxl/minidump/CArrayExpansionTest/JoinWithInListExpand.mdp"};

	return CTestUtils::EresUnittest_RunTestsWithoutAdditionalTraceFlags(
		rgszFileNames, &ulTestCounter, SPQOS_ARRAY_SIZE(rgszFileNames), true,
		true);
}
// EOF
