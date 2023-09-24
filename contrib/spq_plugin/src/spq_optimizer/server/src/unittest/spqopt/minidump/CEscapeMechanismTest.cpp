//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2016 Pivotal, Inc.
//
//	@filename:
//		CEscapeMechanismTest.cpp
//
//	@doc:
//		Test for optimizing queries for exploring fewer alternatives and
//		run optimization process faster
//---------------------------------------------------------------------------

#include "unittest/spqopt/minidump/CEscapeMechanismTest.h"

#include "spqos/base.h"
#include "spqos/memory/CAutoMemoryPool.h"
#include "spqos/task/CAutoTraceFlag.h"
#include "spqos/test/CUnittest.h"

#include "spqopt/exception.h"
#include "spqopt/minidump/CMinidumperUtils.h"

#include "unittest/spqopt/CTestUtils.h"


using namespace spqopt;

ULONG CEscapeMechanismTest::m_ulEscapeMechanismTestCounter =
	0;	// start from first test

// minidump files
const CHAR *rgszEscapeMechanismFileNames[] = {
	"../data/dxl/minidump/JoinArityAssociativityCommutativityAtLimit.mdp",
	"../data/dxl/minidump/JoinArityAssociativityCommutativityAboveLimit.mdp",
	"../data/dxl/minidump/JoinArityAssociativityCommutativityBelowLimit.mdp",
};


//---------------------------------------------------------------------------
//	@function:
//		CEscapeMechanismTest::EresUnittest
//
//	@doc:
//		Unittest for expressions
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CEscapeMechanismTest::EresUnittest()
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
//		CEscapeMechanismTest::EresUnittest_RunTests
//
//	@doc:
//		Run all Minidump-based tests with plan matching
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CEscapeMechanismTest::EresUnittest_RunTests()
{
	return CTestUtils::EresUnittest_RunTestsWithoutAdditionalTraceFlags(
		rgszEscapeMechanismFileNames, &m_ulEscapeMechanismTestCounter,
		SPQOS_ARRAY_SIZE(rgszEscapeMechanismFileNames), true, /* fMatchPlans */
		true /* fTestSpacePruning */
	);
}

// EOF
