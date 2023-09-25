//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 - 2012 EMC Corp.
//
//	@filename:
//		CMaxCardTest.cpp
//
//	@doc:
//		Tests for max card computation
//---------------------------------------------------------------------------

#include "unittest/spqopt/base/CMaxCardTest.h"

#include "spqos/base.h"
#include "spqos/test/CUnittest.h"

#include "spqopt/base/CMaxCard.h"

#include "unittest/spqopt/CTestUtils.h"

using namespace spqopt;

//---------------------------------------------------------------------------
//	@function:
//		CMaxCardTest::EresUnittest
//
//	@doc:
//		Unittest for key collections
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CMaxCardTest::EresUnittest()
{
	CUnittest rgut[] = {
		SPQOS_UNITTEST_FUNC(CMaxCardTest::EresUnittest_Basics),
		SPQOS_UNITTEST_FUNC(CMaxCardTest::EresUnittest_RunMinidumpTests)};

	return CUnittest::EresExecute(rgut, SPQOS_ARRAY_SIZE(rgut));
}

//---------------------------------------------------------------------------
//	@function:
//		CMaxCardTest::EresUnittest_Basics
//
//	@doc:
//		Basic test for key collections
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CMaxCardTest::EresUnittest_Basics()
{
#ifdef SPQOS_DEBUG

	CMaxCard mcOne(1);
	CMaxCard mcTwo(1);
	SPQOS_ASSERT(mcOne == mcTwo);

	CMaxCard mcThree;
	SPQOS_ASSERT(!(mcOne == mcThree));

	CMaxCard mcFour(0);
	mcFour *= mcThree;
	SPQOS_ASSERT(0 == mcFour);

	mcFour += mcOne;
	SPQOS_ASSERT(1 == mcFour);

	mcFour *= mcThree;
	SPQOS_ASSERT(SPQOPT_MAX_CARD == mcFour);

	mcFour += mcThree;
	SPQOS_ASSERT(SPQOPT_MAX_CARD == mcFour);

#endif

	return SPQOS_OK;
}

SPQOS_RESULT
CMaxCardTest::EresUnittest_RunMinidumpTests()
{
	ULONG ulTestCounter = 0;
	// minidump files
	const CHAR *rgszFileNames[] = {
		"../data/dxl/minidump/FullOuterJoinMaxCardRightChild.mdp",
		"../data/dxl/minidump/FullOuterJoinMaxCardLeftChild.mdp",
		"../data/dxl/minidump/FullOuterJoinZeroMaxCard.mdp",
		"../data/dxl/minidump/FullOuterJoinLeftMultiplyRightMaxCard.mdp",
	};

	return CTestUtils::EresUnittest_RunTests(rgszFileNames, &ulTestCounter,
											 SPQOS_ARRAY_SIZE(rgszFileNames));
}

// EOF
