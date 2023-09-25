//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2010 Greenplum, Inc.
//
//	@filename:
//		CLogger.cpp
//
//	@doc:
//		Partial implementation of interface class for logging
//---------------------------------------------------------------------------

#include "spqos/error/CLogger.h"

#include "spqos/common/clibwrapper.h"
#include "spqos/common/syslibwrapper.h"
#include "spqos/error/CLoggerStream.h"
#include "spqos/error/CLoggerSyslog.h"
#include "spqos/error/CMessageRepository.h"
#include "spqos/io/ioutils.h"
#include "spqos/string/CWStringConst.h"

using namespace spqos;


//---------------------------------------------------------------------------
//	@function:
//		CLogger::CLogger
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CLogger::CLogger(ErrorInfoLevel info_level)
	: ILogger(),
	  m_entry_wrapper(m_entry, SPQOS_ARRAY_SIZE(m_entry)),
	  m_msg_wrapper(m_msg, SPQOS_ARRAY_SIZE(m_msg)),
	  m_info_level(info_level)
{
}


//---------------------------------------------------------------------------
//	@function:
//		CLogger::~CLogger
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CLogger::~CLogger()
{
}


//---------------------------------------------------------------------------
//	@function:
//		CLogger::Log
//
//	@doc:
//		Log message
//
//---------------------------------------------------------------------------
void
CLogger::Log(const WCHAR *msg, ULONG severity, const CHAR *filename, ULONG line)
{
	// format log message
	Format(msg, severity, filename, line);

	for (ULONG i = 0; i < SPQOS_LOG_WRITE_RETRIES; i++)
	{
		SPQOS_CHECK_ABORT;

		BOOL pending_exceptions = ITask::Self()->HasPendingExceptions();

		// logging is exercised in catch blocks so it cannot throw;
		// the only propagated exception is Abort;
		SPQOS_TRY
		{
			// write message to log
			Write(m_entry_wrapper.GetBuffer(), severity);
			return;
		}
		SPQOS_CATCH_EX(ex)
		{
			// propagate assert failures
			if (SPQOS_MATCH_EX(ex, CException::ExmaSystem,
							  CException::ExmiAssert))
			{
				SPQOS_RETHROW(ex);
			}

			// ignore anything else but aborts
			if (SPQOS_MATCH_EX(ex, CException::ExmaSystem,
							  CException::ExmiAbort))
			{
				// reset any currently handled exception
				SPQOS_RESET_EX;

				SPQOS_ABORT;
			}

			if (!pending_exceptions)
			{
				SPQOS_RESET_EX;
			}
		}
		SPQOS_CATCH_END;
	}

	// alert admin for logging failure
	ReportFailure();
}


//---------------------------------------------------------------------------
//	@function:
//		CLogger::Format
//
//	@doc:
//		Format log message
//
//---------------------------------------------------------------------------
void
CLogger::Format(const WCHAR *msg, ULONG severity,
				const CHAR *,  // filename
				ULONG		   // line
)
{
	m_entry_wrapper.Reset();
	m_msg_wrapper.Reset();

	CWStringConst strc(msg);

	if (ILogger::EeilMsgHeader <= InfoLevel())
	{
		// LOG ENTRY FORMAT: [date],[thread id],[severity],[message],

		const CHAR *sev = CException::m_severity[severity];
		m_msg_wrapper.Append(&strc);

		AppendDate();

		// append thread id and severity
		m_entry_wrapper.AppendFormat(SPQOS_WSZ_LIT(",THD%03d,%s,\"%ls\",\n"),
									 0,	 //thread id
									 sev, m_msg_wrapper.GetBuffer());
	}
	else
	{
		m_entry_wrapper.Append(&strc);
	}
}


//---------------------------------------------------------------------------
//	@function:
//		CLogger::AppendDate
//
//	@doc:
//		Add date to message
//
//---------------------------------------------------------------------------
void
CLogger::AppendDate()
{
	TIMEVAL tv;
	TIME tm;

	// get local time
	syslib::GetTimeOfDay(&tv, NULL /*timezone*/);
#ifdef SPQOS_DEBUG
	TIME *t =
#endif	// SPQOS_DEBUG
		clib::Localtime_r(&tv.tv_sec, &tm);

	SPQOS_ASSERT(NULL != t && "Failed to get local time");

	// format: YYYY-MM-DD HH-MM-SS-UUUUUU TZ
	m_entry_wrapper.AppendFormat(
		SPQOS_WSZ_LIT("%04d-%02d-%02d %02d:%02d:%02d:%06d %s"),
		tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min,
		tm.tm_sec, tv.tv_usec, tm.tm_zone);
}


//---------------------------------------------------------------------------
//	@function:
//		CLogger::AppendDate
//
//	@doc:
//		Report logging failure
//
//---------------------------------------------------------------------------
void
CLogger::ReportFailure()
{
	// check if errno was set
	if (0 < errno)
	{
		// get error description
		clib::Strerror_r(errno, m_retrieved_msg,
						 SPQOS_ARRAY_SIZE(m_retrieved_msg));
		m_retrieved_msg[SPQOS_ARRAY_SIZE(m_retrieved_msg) - 1] = '\0';

		m_entry_wrapper.Reset();
		m_entry_wrapper.AppendFormat(SPQOS_WSZ_LIT("%s\n"), m_retrieved_msg);
		CLoggerSyslog::Alert(m_entry_wrapper.GetBuffer());
		return;
	}

	// send generic failure message
	CLoggerSyslog::Alert(SPQOS_WSZ_LIT(
		"Log write failure, check disc space and filesystem integrity"));
}

// EOF
