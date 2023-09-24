//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2010 Greenplum, Inc.
//
//	@filename:
//		ILogger.h
//
//	@doc:
//		Interface class for logging
//---------------------------------------------------------------------------
#ifndef SPQOS_ILogger_H
#define SPQOS_ILogger_H

#include "spqos/types.h"

#ifndef SPQOS_DEBUG
#define SPQOS_LOG_MESSAGE_BUFFER_SIZE (1024 * 128)
#else
// have a larger buffer size for debug builds (e.g. printing out MEMO)
#define SPQOS_LOG_MESSAGE_BUFFER_SIZE (8192 * 128)
#endif
#define SPQOS_LOG_TRACE_BUFFER_SIZE (1024 * 8)
#define SPQOS_LOG_ENTRY_BUFFER_SIZE (SPQOS_LOG_MESSAGE_BUFFER_SIZE + 256)
#define SPQOS_LOG_WRITE_RETRIES (10)

#define SPQOS_WARNING(...) ILogger::Warning(__FILE__, __LINE__, __VA_ARGS__)

#define SPQOS_TRACE(msg) \
	ILogger::Trace(__FILE__, __LINE__, false /*is_err*/, msg)

#define SPQOS_TRACE_ERR(msg) \
	ILogger::Trace(__FILE__, __LINE__, true /*is_err*/, msg)

#define SPQOS_TRACE_FORMAT(format, ...)                         \
	ILogger::TraceFormat(__FILE__, __LINE__, false /*is_err*/, \
						 SPQOS_WSZ_LIT(format), __VA_ARGS__)

#define SPQOS_TRACE_FORMAT_ERR(format, ...)                    \
	ILogger::TraceFormat(__FILE__, __LINE__, true /*is_err*/, \
						 SPQOS_WSZ_LIT(format), __VA_ARGS__)

namespace spqos
{
//---------------------------------------------------------------------------
//	@class:
//		ILogger
//
//	@doc:
//		Interface for abstracting logging primitives.
//
//---------------------------------------------------------------------------

class ILogger
{
	friend class CErrorHandlerStandard;

public:
	// enum indicating error logging information
	enum ErrorInfoLevel
	{
		EeilMsg,			// log error message only
		EeilMsgHeader,		// log error header and message
		EeilMsgHeaderStack	// log error header, message and stack trace
	};

private:
	// log message to current task's logger;
	// use stdout/stderr wrapping loggers outside worker framework;
	static void LogTask(const WCHAR *msg, ULONG severity, BOOL is_err,
						const CHAR *filename, ULONG line);

	// no copy ctor
	ILogger(const ILogger &);

protected:
	// write log message
	virtual void Write(const WCHAR *log_entry, ULONG severity) = 0;

public:
	// ctor
	ILogger();

	// dtor
	virtual ~ILogger();

	// error info level accessor
	virtual ErrorInfoLevel InfoLevel() const = 0;

	// set error info level
	virtual void SetErrorInfoLevel(ErrorInfoLevel info_level) = 0;

	// retrieve warning message from repository and log it to error log
	static void Warning(const CHAR *filename, ULONG line, ULONG major,
						ULONG minor, ...);

	// log trace message to current task's output or error log
	static void Trace(const CHAR *filename, ULONG line, BOOL is_err,
					  const WCHAR *msg);

	// format and log trace message to current task's output or error log
	static void TraceFormat(const CHAR *filename, ULONG line, BOOL is_err,
							const WCHAR *format, ...);

};	// class ILogger
}  // namespace spqos

#endif	// !SPQOS_ILogger_H

// EOF
