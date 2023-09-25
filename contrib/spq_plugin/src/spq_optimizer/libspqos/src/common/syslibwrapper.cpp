//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		syslibwrapper.cpp
//
//	@doc:
//		Wrapper for functions in system library
//
//---------------------------------------------------------------------------

#include "spqos/common/syslibwrapper.h"

#include <sys/stat.h>
#include <sys/time.h>
#include <syslog.h>

#include "spqos/assert.h"
#include "spqos/error/CException.h"
#include "spqos/utils.h"


using namespace spqos;


//---------------------------------------------------------------------------
//	@function:
//		syslib::GetTimeOfDay
//
//	@doc:
//		Get the date and time
//
//---------------------------------------------------------------------------
void
spqos::syslib::GetTimeOfDay(TIMEVAL *tv, TIMEZONE *tz)
{
	SPQOS_ASSERT(NULL != tv);

#ifdef SPQOS_DEBUG
	INT res =
#endif	// SPQOS_DEBUG
		gettimeofday(tv, tz);

	SPQOS_ASSERT(0 == res);
}


//---------------------------------------------------------------------------
//	@function:
//		syslib::GetRusage
//
//	@doc:
//		Get system and user time
//
//---------------------------------------------------------------------------
void
spqos::syslib::GetRusage(RUSAGE *usage)
{
	SPQOS_ASSERT(NULL != usage);

#ifdef SPQOS_DEBUG
	INT res =
#endif	// SPQOS_DEBUG
		getrusage(RUSAGE_SELF, usage);

	SPQOS_ASSERT(0 == res);
}


//---------------------------------------------------------------------------
//	@function:
//		syslib::OpenLog
//
//	@doc:
//		Open a connection to the system logger for a program
//
//---------------------------------------------------------------------------
void
spqos::syslib::OpenLog(const CHAR *ident, INT option, INT facility)
{
	openlog(ident, option, facility);
}


//---------------------------------------------------------------------------
//	@function:
//		syslib::SysLog
//
//	@doc:
//		Generate a log message
//
//---------------------------------------------------------------------------
void
spqos::syslib::SysLog(INT priority, const CHAR *format)
{
	syslog(priority, "%s", format);
}


//---------------------------------------------------------------------------
//	@function:
//		syslib::CloseLog
//
//	@doc:
//		Close the descriptor being used to write to the system logger
//
//---------------------------------------------------------------------------
void
spqos::syslib::CloseLog()
{
	closelog();
}

// EOF
