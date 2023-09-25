//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CAutoTrace.h
//
//	@doc:
//		Auto object for creating trace messages
//---------------------------------------------------------------------------
#ifndef SPQOS_CAutoTrace_H
#define SPQOS_CAutoTrace_H

#include "spqos/base.h"
#include "spqos/common/CStackObject.h"
#include "spqos/io/COstreamString.h"
#include "spqos/string/CWStringDynamic.h"

namespace spqos
{
//---------------------------------------------------------------------------
//	@class:
//		CAutoTrace
//
//	@doc:
//		Auto object for creating trace messages;
//		creates a stream over a dynamic string and uses it to print objects;
//		at destruction the string is written to the log as a trace msg;
//
//---------------------------------------------------------------------------
class CAutoTrace : public CStackObject
{
private:
	// dynamic string buffer
	CWStringDynamic m_wstr;

	// string stream
	COstreamString m_os;

	// private copy ctor
	CAutoTrace(const CAutoTrace &);

public:
	// ctor
	explicit CAutoTrace(CMemoryPool *mp);

	// dtor
	~CAutoTrace();

	// stream accessor
	IOstream &
	Os()
	{
		return m_os;
	}

};	// class CAutoTrace
}  // namespace spqos

#endif	// !SPQOS_CAutoTrace_H

// EOF
