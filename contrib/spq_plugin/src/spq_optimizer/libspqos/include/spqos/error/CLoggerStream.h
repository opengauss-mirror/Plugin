//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2010 Greenplum, Inc.
//
//	@filename:
//		CLoggerStream.h
//
//	@doc:
//		Implementation of logging interface over stream
//---------------------------------------------------------------------------
#ifndef SPQOS_CLoggerStream_H
#define SPQOS_CLoggerStream_H

#include "spqos/error/CLogger.h"
#include "spqos/io/IOstream.h"

namespace spqos
{
//---------------------------------------------------------------------------
//	@class:
//		CLoggerStream
//
//	@doc:
//		Stream logging.
//
//---------------------------------------------------------------------------

class CLoggerStream : public CLogger
{
private:
	// log stream
	IOstream &m_os;

	// write string to stream
	void
	Write(const WCHAR *log_entry,
		  ULONG	 // severity
	)
	{
		m_os = m_os << log_entry;
	}

	// no copy ctor
	CLoggerStream(const CLoggerStream &);

public:
	// ctor
	CLoggerStream(IOstream &os);

	// dtor
	virtual ~CLoggerStream();

	// wrapper for stdout
	static CLoggerStream m_stdout_stream_logger;

	// wrapper for stderr
	static CLoggerStream m_stderr_stream_logger;

};	// class CLoggerStream
}  // namespace spqos

#endif	// !SPQOS_CLoggerStream_H

// EOF
