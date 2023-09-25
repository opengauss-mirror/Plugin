//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2010 Greenplum, Inc.
//
//	@filename:
//		ILogger.cpp
//
//	@doc:
//		Worker abstraction, e.g. thread
//---------------------------------------------------------------------------

#include "spqos/error/ILogger.h"

#include "spqos/common/clibwrapper.h"
#include "spqos/common/syslibwrapper.h"
#include "spqos/error/CLoggerStream.h"
#include "spqos/error/CLoggerSyslog.h"
#include "spqos/error/CMessageRepository.h"
#include "spqos/string/CWStringConst.h"

using namespace spqos;


//---------------------------------------------------------------------------
//	@function:
//		ILogger::ILogger
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
ILogger::ILogger()
{
}


//---------------------------------------------------------------------------
//	@function:
//		ILogger::~ILogger
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
ILogger::~ILogger()
{
}


//---------------------------------------------------------------------------
//	@function:
//		ILogger::Warning
//
//	@doc:
//		Retrieve warning message from repository and log it to error log
//
//---------------------------------------------------------------------------
void
ILogger::Warning(const CHAR *filename, ULONG line, ULONG major, ULONG minor...)
{
	SPQOS_CHECK_ABORT;

	// get warning
	CException exc(major, minor, filename, line);

	ITask *task = ITask::Self();

	// get current task's locale
	ELocale locale = ElocEnUS_Utf8;
	if (NULL != task)
	{
		locale = task->Locale();
	}

	// retrieve warning message from repository
	CMessage *msg =
		CMessageRepository::GetMessageRepository()->LookupMessage(exc, locale);

	SPQOS_ASSERT(CException::ExsevWarning == msg->GetSeverity());

	WCHAR buffer[SPQOS_LOG_MESSAGE_BUFFER_SIZE];
	CWStringStatic str(buffer, SPQOS_ARRAY_SIZE(buffer));

	// format warning message
	{
		VA_LIST va_args;

		VA_START(va_args, minor);

		msg->Format(&str, va_args);

		VA_END(va_args);
	}

	LogTask(str.GetBuffer(), CException::ExsevWarning, true /*is_err*/,
			filename, line);
}


//---------------------------------------------------------------------------
//	@function:
//		ILogger::Trace
//
//	@doc:
//		Format and log debugging message to current task's output or error log
//
//---------------------------------------------------------------------------
void
ILogger::Trace(const CHAR *filename, ULONG line, BOOL is_err, const WCHAR *msg)
{
	SPQOS_CHECK_ABORT;

	LogTask(msg, CException::ExsevTrace, is_err, filename, line);
}


//---------------------------------------------------------------------------
//	@function:
//		ILogger::TraceFormat
//
//	@doc:
//		Format and log debugging message to current task's output or error log
//
//---------------------------------------------------------------------------
void
ILogger::TraceFormat(const CHAR *filename, ULONG line, BOOL is_err,
					 const WCHAR *format, ...)
{
	SPQOS_CHECK_ABORT;

	WCHAR buffer[SPQOS_LOG_TRACE_BUFFER_SIZE];
	CWStringStatic str(buffer, SPQOS_ARRAY_SIZE(buffer));

	VA_LIST va_args;

	// get arguments
	VA_START(va_args, format);

	str.AppendFormatVA(format, va_args);

	// reset arguments
	VA_END(va_args);

	LogTask(str.GetBuffer(), CException::ExsevTrace, is_err, filename, line);
}


//---------------------------------------------------------------------------
//	@function:
//		ILogger::LogTask
//
//	@doc:
//		Log message to current task's logger;
// 		Use stdout/stderr-wrapping loggers outside worker framework;
//
//---------------------------------------------------------------------------
void
ILogger::LogTask(const WCHAR *msg, ULONG severity, BOOL is_err,
				 const CHAR *filename, ULONG line)
{
	CLogger *log = NULL;

	if (is_err)
	{
		log = &CLoggerStream::m_stderr_stream_logger;
	}
	else
	{
		log = &CLoggerStream::m_stdout_stream_logger;
	}

	ITask *task = ITask::Self();
	if (NULL != task)
	{
		if (is_err)
		{
			log = dynamic_cast<CLogger *>(task->GetErrorLogger());
		}
		else
		{
			log = dynamic_cast<CLogger *>(task->GetOutputLogger());
		}
	}

	SPQOS_ASSERT(NULL != log);

	log->Log(msg, severity, filename, line);
}

// EOF
