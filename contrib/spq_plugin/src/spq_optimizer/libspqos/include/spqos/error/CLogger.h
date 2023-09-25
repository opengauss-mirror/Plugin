//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2010 Greenplum, Inc.
//
//	@filename:
//		CLogger.h
//
//	@doc:
//		Partial implementation of interface class for logging
//---------------------------------------------------------------------------
#ifndef SPQOS_CLogger_H
#define SPQOS_CLogger_H

#include "spqos/error/ILogger.h"
#include "spqos/string/CWStringStatic.h"

namespace spqos
{
//---------------------------------------------------------------------------
//	@class:
//		CLogger
//
//	@doc:
//		Partial implementation of interface for abstracting logging primitives.
//
//---------------------------------------------------------------------------
class CLogger : public ILogger
{
	friend class ILogger;
	friend class CErrorHandlerStandard;

private:
	// buffer used to construct the log entry
	WCHAR m_entry[SPQOS_LOG_ENTRY_BUFFER_SIZE];

	// buffer used to construct the log entry
	WCHAR m_msg[SPQOS_LOG_MESSAGE_BUFFER_SIZE];

	// buffer used to retrieve system error messages
	CHAR m_retrieved_msg[SPQOS_LOG_MESSAGE_BUFFER_SIZE];

	// entry buffer wrapper
	CWStringStatic m_entry_wrapper;

	// message buffer wrapper
	CWStringStatic m_msg_wrapper;

	// error logging information level
	ErrorInfoLevel m_info_level;

	// log message
	void Log(const WCHAR *msg, ULONG severity, const CHAR *filename,
			 ULONG line);

	// format log message
	void Format(const WCHAR *msg, ULONG severity, const CHAR *filename,
				ULONG line);

	// add date to message
	void AppendDate();

	// report logging failure
	void ReportFailure();

	// no copy ctor
	CLogger(const CLogger &);

protected:
	// accessor for system error buffer
	CHAR *
	Msg()
	{
		return m_retrieved_msg;
	}

public:
	// ctor
	explicit CLogger(ErrorInfoLevel info_level = ILogger::EeilMsgHeaderStack);

	// dtor
	virtual ~CLogger();

	// error level accessor
	virtual ErrorInfoLevel
	InfoLevel() const
	{
		return m_info_level;
	}

	// set error info level
	virtual void
	SetErrorInfoLevel(ErrorInfoLevel info_level)
	{
		m_info_level = info_level;
	}

};	// class CLogger
}  // namespace spqos

#endif	// !SPQOS_CLogger_H

// EOF
