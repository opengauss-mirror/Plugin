//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2008 - 2010 Greenplum, Inc.
//
//	@filename:
//		CFSimulatorTestExt.cpp
//
//	@doc:
//		Extended FS tests
//---------------------------------------------------------------------------

#include "spqos/test/CFSimulatorTestExt.h"

#include "spqos/base.h"
#include "spqos/common/CMainArgs.h"
#include "spqos/error/CAutoTrace.h"
#include "spqos/error/CErrorHandlerStandard.h"
#include "spqos/error/CFSimulator.h"
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
CFSimulatorTestExt::EresUnittest()
{
	CUnittest rgut[] = {
		SPQOS_UNITTEST_FUNC(CFSimulatorTestExt::EresUnittest_OOM),
		SPQOS_UNITTEST_FUNC(CFSimulatorTestExt::EresUnittest_Abort),
		SPQOS_UNITTEST_FUNC(CFSimulatorTestExt::EresUnittest_IOError),
		SPQOS_UNITTEST_FUNC(CFSimulatorTestExt::EresUnittest_NetError),
	};

	SPQOS_RESULT eres = CUnittest::EresExecute(rgut, SPQOS_ARRAY_SIZE(rgut));

	return eres;
}


//---------------------------------------------------------------------------
//	@function:
//		CFSimulatorTestExt::EresUnittest_OOM
//
//	@doc:
//		Simulate an OOM failure
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CFSimulatorTestExt::EresUnittest_OOM()
{
	// enable OOM simulation
	CAutoTraceFlag atfSet(EtraceSimulateOOM, true);

	// run simulation
	return EresUnittest_SimulateException(CException::ExmaSystem,
										  CException::ExmiOOM);
}


//---------------------------------------------------------------------------
//	@function:
//		CFSimulatorTestExt::EresUnittest_Abort
//
//	@doc:
//		Simulate an Abort request
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CFSimulatorTestExt::EresUnittest_Abort()
{
	// enable Abort simulation
	CAutoTraceFlag atfSet(EtraceSimulateAbort, true);

	// run simulation
	return EresUnittest_SimulateException(CException::ExmaSystem,
										  CException::ExmiAbort);
}


//---------------------------------------------------------------------------
//	@function:
//		CFSimulatorTestExt::EresUnittest_IOError
//
//	@doc:
//		Simulate an I/O error
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CFSimulatorTestExt::EresUnittest_IOError()
{
	// enable I/O error simulation
	CAutoTraceFlag atfSet(EtraceSimulateIOError, true);

	// run simulation
	return EresUnittest_SimulateException(CException::ExmaSystem,
										  CException::ExmiIOError);
}


//---------------------------------------------------------------------------
//	@function:
//		CFSimulatorTestExt::EresUnittest_NetError
//
//	@doc:
//		Simulate an networking error
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CFSimulatorTestExt::EresUnittest_NetError()
{
	// enable networking error simulation
	CAutoTraceFlag atfSet(EtraceSimulateNetError, true);

	// run simulation
	return EresUnittest_SimulateException(CException::ExmaSystem,
										  CException::ExmiNetError);
}


//---------------------------------------------------------------------------
//	@function:
//		CFSimulatorTestExt::EresUnittest_SimulateException
//
//	@doc:
//		Simulate exceptions of given type
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CFSimulatorTestExt::EresUnittest_SimulateException(ULONG major, ULONG minor)
{
	// assemble -u option
	const CHAR *rgsz[] = {"", "-u"};

	while (true)
	{
		SPQOS_TRY
		{
			CMainArgs ma(SPQOS_ARRAY_SIZE(rgsz), rgsz, "u");
			CUnittest::Driver(&ma);

			// executed all tests w/o exception
			return SPQOS_OK;
		}
		SPQOS_CATCH_EX(ex)
		{
			SPQOS_RESET_EX;

			// retry every time we hit an OOM, else bail
			if (!SPQOS_MATCH_EX(ex, major, minor))
			{
				return SPQOS_FAILED;
			}
		}
		SPQOS_CATCH_END;
	}
}


#endif	// SPQOS_FPSIMULATOR

// EOF
