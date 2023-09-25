//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2008 Greenplum, Inc.
//
//	@filename:
//		CFSimulatorTest.cpp
//
//	@doc:
//		Tests for
//---------------------------------------------------------------------------

#include "unittest/spqos/error/CFSimulatorTest.h"

#include "spqos/base.h"
#include "spqos/error/CFSimulator.h"
#include "spqos/memory/CAutoMemoryPool.h"
#include "spqos/task/CAutoSuspendAbort.h"
#include "spqos/task/CAutoTraceFlag.h"
#include "spqos/test/CUnittest.h"

using namespace spqos;

#ifdef SPQOS_FPSIMULATOR

//---------------------------------------------------------------------------
//	@function:
//		CFSimulator::EresUnittest
//
//	@doc:
//		Driver for unittests
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CFSimulatorTest::EresUnittest()
{
	CUnittest rgut[] = {
		SPQOS_UNITTEST_FUNC(CFSimulatorTest::EresUnittest_BasicTracking)};

	// ignore this test for FP simulation and time slicing check
	if (CFSimulator::FSimulation())
	{
		return SPQOS_OK;
	}

	// set test flag in this scope
	CAutoTraceFlag atf(EtraceTest, true);

	SPQOS_RESULT eres = CUnittest::EresExecute(rgut, SPQOS_ARRAY_SIZE(rgut));

	return eres;
}


//---------------------------------------------------------------------------
//	@function:
//		CFSimulatorTest::EresUnittest_BasicTracking
//
//	@doc:
//		Register a single occurrance
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CFSimulatorTest::EresUnittest_BasicTracking()
{
	BOOL fThrown = false;
	static ULONG ul = 0;
	if (10 == ul)
	{
		return SPQOS_OK;
	}

	SPQOS_TRY
	{
		SPQOS_SIMULATE_FAILURE(EtraceTest, CException::ExmaSystem,
							  CException::ExmiOOM);
	}
	SPQOS_CATCH_EX(ex)
	{
		if (SPQOS_MATCH_EX(ex, CException::ExmaSystem, CException::ExmiOOM))
		{
			// suspend CFA
			CAutoSuspendAbort asa;

			SPQOS_TRACE_FORMAT("%d: Caught expected exception.", ul);

			fThrown = true;
		}

		SPQOS_RESET_EX;
	}
	SPQOS_CATCH_END;

	++ul;
	EresUnittest_BasicTracking();

	if (fThrown)
	{
		return SPQOS_OK;
	}

	return SPQOS_FAILED;
}

#endif	// SPQOS_FPSIMULATOR

// EOF
