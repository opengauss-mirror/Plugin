//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2010 Greenplum, Inc.
//
//	@filename:
//		CLoggerTest.h
//
//	@doc:
//      Unit test for logger classes.
//---------------------------------------------------------------------------
#ifndef SPQOS_CLoggerTest_H
#define SPQOS_CLoggerTest_H

#include "spqos/base.h"

namespace spqos
{
//---------------------------------------------------------------------------
//	@class:
//		CLoggerTest
//
//	@doc:
//		Unittests for log functionality
//
//---------------------------------------------------------------------------
class CLoggerTest
{
public:
	// unittests
	static SPQOS_RESULT EresUnittest();
	static SPQOS_RESULT EresUnittest_Basic();
	static SPQOS_RESULT EresUnittest_LoggerSyslog();

};	// CLoggerTest
}  // namespace spqos

#endif	// !SPQOS_CLoggerTest_H

// EOF
