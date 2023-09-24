//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2010 Greenplum, Inc.
//
//	@filename:
//		CLoggerTest
//
//	@doc:
//		Unit test for logger classes.
//---------------------------------------------------------------------------

#include "unittest/spqos/error/CLoggerTest.h"

#include "spqos/error/CLoggerStream.h"
#include "spqos/error/CLoggerSyslog.h"
#include "spqos/error/ILogger.h"
#include "spqos/io/ioutils.h"
#include "spqos/memory/CAutoMemoryPool.h"
#include "spqos/task/CAutoTaskProxy.h"
#include "spqos/task/CAutoTraceFlag.h"
#include "spqos/test/CUnittest.h"

using namespace spqos;


//---------------------------------------------------------------------------
//	@function:
//		CLoggerTest::EresUnittest
//
//	@doc:
//		Driver for unittests
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CLoggerTest::EresUnittest()
{
	CUnittest rgut[] = {
		SPQOS_UNITTEST_FUNC(CLoggerTest::EresUnittest_Basic),
		SPQOS_UNITTEST_FUNC(CLoggerTest::EresUnittest_LoggerSyslog),
	};

	SPQOS_RESULT eres = CUnittest::EresExecute(rgut, SPQOS_ARRAY_SIZE(rgut));

	return eres;
}


//---------------------------------------------------------------------------
//	@function:
//		CLoggerTest::EresUnittest_Basic
//
//	@doc:
//		Basic test for logging
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CLoggerTest::EresUnittest_Basic()
{
	// log trace messages
	SPQOS_TRACE(SPQOS_WSZ_LIT("Log trace message as built string"));
	SPQOS_TRACE_FORMAT("Log trace message as %s %s", "formatted", "string");

	{
		// disable Abort simulation;
		// simulation leads to self-deadlock while trying to log injection
		CAutoTraceFlag atfSet(EtraceSimulateAbort, false);

		// log warning message
		SPQOS_WARNING(CException::ExmaSystem, CException::ExmiDummyWarning,
					 "Foo");
	}

	return SPQOS_OK;
}


//---------------------------------------------------------------------------
//	@function:
//		CLoggerTest::EresUnittest_LoggerSyslog
//
//	@doc:
//		Log to file
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CLoggerTest::EresUnittest_LoggerSyslog()
{
	SPQOS_SYSLOG_ALERT("This is test message 1 from SPQOS to syslog");
	SPQOS_SYSLOG_ALERT("This is test message 2 from SPQOS to syslog");
	SPQOS_SYSLOG_ALERT("This is test message 3 from SPQOS to syslog");

	return SPQOS_OK;
}


// EOF
