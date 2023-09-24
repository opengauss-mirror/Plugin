//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2015 Pivotal, Inc.
//
//	@filename:
//		CTVFTest.cpp
//
//	@doc:
//		Test for optimizing queries with TVF
//---------------------------------------------------------------------------

#include "unittest/spqopt/minidump/CTVFTest.h"

#include "spqos/base.h"
#include "spqos/memory/CAutoMemoryPool.h"
#include "spqos/task/CAutoTraceFlag.h"
#include "spqos/test/CUnittest.h"

#include "spqopt/exception.h"
#include "spqopt/minidump/CMinidumperUtils.h"

#include "unittest/spqopt/CTestUtils.h"

using namespace spqopt;

ULONG CTVFTest::m_ulTVFTestCounter = 0;	 // start from first test

// minidump files
const CHAR *rgszTVFFileNames[] = {
	"../data/dxl/minidump/TVF.mdp",
	"../data/dxl/minidump/TVFAnyelement.mdp",
	"../data/dxl/minidump/TVFVolatileJoin.mdp",
	"../data/dxl/minidump/TVFRandom.mdp",
	"../data/dxl/minidump/TVFGenerateSeries.mdp",
	"../data/dxl/minidump/TVF-With-Deep-Subq-Args.mdp",
	"../data/dxl/minidump/TVFCorrelatedExecution.mdp",
	"../data/dxl/minidump/CSQ-VolatileTVF.mdp",
};

//---------------------------------------------------------------------------
//	@function:
//		CTVFTest::EresUnittest
//
//	@doc:
//		Unittest for expressions
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CTVFTest::EresUnittest()
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
//		CTVFTest::EresUnittest_RunTests
//
//	@doc:
//		Run all Minidump-based tests with plan matching
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CTVFTest::EresUnittest_RunTests()
{
	return CTestUtils::EresUnittest_RunTests(rgszTVFFileNames,
											 &m_ulTVFTestCounter,
											 SPQOS_ARRAY_SIZE(rgszTVFFileNames));
}

// EOF
