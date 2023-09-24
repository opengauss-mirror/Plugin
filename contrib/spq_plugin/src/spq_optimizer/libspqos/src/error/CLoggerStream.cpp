//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2010 Greenplum, Inc.
//
//	@filename:
//		CLoggerStream.cpp
//
//	@doc:
//		Implementation of stream logging
//---------------------------------------------------------------------------

#include "spqos/error/CLoggerStream.h"

#include "spqos/utils.h"

using namespace spqos;

CLoggerStream CLoggerStream::m_stdout_stream_logger(oswcout);
CLoggerStream CLoggerStream::m_stderr_stream_logger(oswcerr);


//---------------------------------------------------------------------------
//	@function:
//		CLoggerStream::CLoggerStream
//
//	@doc:
//
//---------------------------------------------------------------------------
CLoggerStream::CLoggerStream(IOstream &os) : CLogger(), m_os(os)
{
}

//---------------------------------------------------------------------------
//	@function:
//		CLoggerStream::~CLoggerStream
//
//	@doc:
//
//---------------------------------------------------------------------------
CLoggerStream::~CLoggerStream()
{
}


// EOF
