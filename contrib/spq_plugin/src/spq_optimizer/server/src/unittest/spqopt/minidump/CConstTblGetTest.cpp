//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2018 Pivotal, Inc.
//---------------------------------------------------------------------------

#include "unittest/spqopt/minidump/CConstTblGetTest.h"

#include "spqos/base.h"
#include "spqos/memory/CAutoMemoryPool.h"
#include "spqos/task/CAutoTraceFlag.h"
#include "spqos/test/CUnittest.h"

#include "spqopt/exception.h"
#include "spqopt/minidump/CMinidumperUtils.h"

#include "unittest/spqopt/CTestUtils.h"


using namespace spqopt;

ULONG CConstTblGetTest::m_ulTestCounter = 0;  // start from first test

// minidump files
const CHAR *rgszCTGMdpFiles[] = {
	"../data/dxl/minidump/ConstTblGetUnderSubqWithOuterRef.mdp",
	"../data/dxl/minidump/ConstTblGetUnderSubqWithNoOuterRef.mdp",
	"../data/dxl/minidump/ConstTblGetUnderSubqUnderProjectNoOuterRef.mdp",
	"../data/dxl/minidump/ConstTblGetUnderSubqUnderProjectWithOuterRef.mdp",
	"../data/dxl/minidump/CTG-Filter.mdp",
	"../data/dxl/minidump/CTG-Join.mdp",
	"../data/dxl/minidump/Sequence-With-Universal-Outer.mdp",
	"../data/dxl/minidump/UseDistributionSatisfactionForUniversalInnerChild.mdp",
	"../data/dxl/minidump/Join_OuterChild_DistUniversal.mdp",
};


SPQOS_RESULT
CConstTblGetTest::EresUnittest()
{
	CUnittest rgut[] = {
		SPQOS_UNITTEST_FUNC(EresUnittest_RunTests),
	};

	SPQOS_RESULT eres = CUnittest::EresExecute(rgut, SPQOS_ARRAY_SIZE(rgut));

	// reset metadata cache
	CMDCache::Reset();

	return eres;
}

// Run all Minidump-based tests with plan matching
SPQOS_RESULT
CConstTblGetTest::EresUnittest_RunTests()
{
	return CTestUtils::EresUnittest_RunTests(rgszCTGMdpFiles, &m_ulTestCounter,
											 SPQOS_ARRAY_SIZE(rgszCTGMdpFiles));
}

// EOF
