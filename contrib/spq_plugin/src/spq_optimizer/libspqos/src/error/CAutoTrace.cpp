//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CAutoTrace.cpp
//
//	@doc:
//		Implementation of auto object for creating trace messages
//---------------------------------------------------------------------------

#include "spqos/error/CAutoTrace.h"

#include "spqos/task/ITask.h"

using namespace spqos;

//---------------------------------------------------------------------------
//	@function:
//		CAutoTrace::CAutoTrace
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CAutoTrace::CAutoTrace(CMemoryPool *mp) : m_wstr(mp), m_os(&m_wstr)
{
}


//---------------------------------------------------------------------------
//	@function:
//		CAutoTrace::~CAutoTrace
//
//	@doc:
//		Destructor prints trace message
//
//---------------------------------------------------------------------------
CAutoTrace::~CAutoTrace()
{
	if (0 < m_wstr.Length() && !ITask::Self()->GetErrCtxt()->IsPending())
	{
		SPQOS_TRACE(m_wstr.GetBuffer());
	}
}

// EOF
