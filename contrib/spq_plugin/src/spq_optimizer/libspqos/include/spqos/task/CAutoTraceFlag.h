//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2008 Greenplum, Inc.
//
//	@filename:
//		CAutoTraceFlag.h
//
//	@doc:
//		Auto wrapper to set/reset a traceflag for a scope
//---------------------------------------------------------------------------
#ifndef SPQOS_CAutoTraceFlag_H
#define SPQOS_CAutoTraceFlag_H

#include "spqos/base.h"
#include "spqos/common/CStackObject.h"
#include "spqos/task/ITask.h"
#include "spqos/task/traceflags.h"


namespace spqos
{
//---------------------------------------------------------------------------
//	@class:
//		CAutoTraceFlag
//
//	@doc:
//		Auto wrapper;
//
//---------------------------------------------------------------------------
class CAutoTraceFlag : public CStackObject
{
private:
	// traceflag id
	ULONG m_trace;

	// original value
	BOOL m_orig;

	// no copy ctor
	CAutoTraceFlag(const CAutoTraceFlag &);

public:
	// ctor
	CAutoTraceFlag(ULONG trace, BOOL orig);

	// dtor
	virtual ~CAutoTraceFlag();

};	// class CAutoTraceFlag

}  // namespace spqos


#endif	// !SPQOS_CAutoTraceFlag_H

// EOF
