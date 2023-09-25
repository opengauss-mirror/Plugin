//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2008 - 2010 Greenplum, Inc.
//
//	@filename:
//		CAutoTimer.cpp
//
//	@doc:
//		Implementation of wrapper around wall clock timer
//---------------------------------------------------------------------------

#include "spqos/common/CAutoTimer.h"

#include "spqos/base.h"
#include "spqos/task/CAutoSuspendAbort.h"

using namespace spqos;

//---------------------------------------------------------------------------
//	@function:
//		CAutoTimer::CAutoTimer
//
//	@doc:
//		ctor
//
//---------------------------------------------------------------------------
CAutoTimer::CAutoTimer(const CHAR *sz, BOOL fPrint)
	: m_timer_text_label(sz), m_print_text_label(fPrint)
{
	SPQOS_ASSERT(NULL != sz);
}


//---------------------------------------------------------------------------
//	@function:
//		CAutoTimer::~CAutoTimer
//
//	@doc:
//		Destructor prints time difference and label
//
//---------------------------------------------------------------------------
CAutoTimer::~CAutoTimer() throw()
{
	if (m_print_text_label)
	{
		// suspend cancellation - destructors should not throw
		CAutoSuspendAbort asa;

		ULONG ulElapsedTimeMS = m_clock.ElapsedMS();

		SPQOS_TRACE_FORMAT("timer:%s: %dms", m_timer_text_label,
						  ulElapsedTimeMS);
	}
}

// EOF
