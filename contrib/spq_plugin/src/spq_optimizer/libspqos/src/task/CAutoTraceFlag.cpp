//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CAutoTraceFlag.cpp
//
//	@doc:
//		Auto object to toggle TF in scope
//---------------------------------------------------------------------------

#include "spqos/task/CAutoTraceFlag.h"

#include "spqos/base.h"
#include "spqos/error/CAutoTrace.h"

using namespace spqos;


//---------------------------------------------------------------------------
//	@function:
//		CAutoTraceFlag::CAutoTraceFlag
//
//	@doc:
//		ctor
//
//---------------------------------------------------------------------------
CAutoTraceFlag::CAutoTraceFlag(ULONG trace, BOOL orig)
	: m_trace(trace), m_orig(false)
{
	SPQOS_ASSERT(NULL != ITask::Self());
	m_orig = ITask::Self()->SetTrace(m_trace, orig);
}


//---------------------------------------------------------------------------
//	@function:
//		CAutoTraceFlag::~CAutoTraceFlag
//
//	@doc:
//		dtor
//
//---------------------------------------------------------------------------
CAutoTraceFlag::~CAutoTraceFlag()
{
	SPQOS_ASSERT(NULL != ITask::Self());

	// reset original value
	ITask::Self()->SetTrace(m_trace, m_orig);
}


// EOF
