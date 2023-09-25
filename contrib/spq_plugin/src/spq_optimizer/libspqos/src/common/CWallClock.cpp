//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp
//
//	@filename:
//		CWallClock.cpp
//
//	@doc:
//		Implementation of wall clock timer
//---------------------------------------------------------------------------

#include "spqos/common/CWallClock.h"

#include "spqos/base.h"
#include "spqos/common/syslibwrapper.h"

using namespace spqos;


//---------------------------------------------------------------------------
//	@function:
//		CWallClock::ElapsedUS
//
//	@doc:
//		Wall-clock time in micro-seconds since object construction
//
//---------------------------------------------------------------------------
ULONG
CWallClock::ElapsedUS() const
{
	timeval time;
	syslib::GetTimeOfDay(&time, NULL /*timezone*/);

	ULONG diff = (ULONG)(((time.tv_sec - m_time.tv_sec) * SPQOS_USEC_IN_SEC) +
						 (time.tv_usec - m_time.tv_usec));

	return diff;
}


//---------------------------------------------------------------------------
//	@function:
//		CWallClock::Restart
//
//	@doc:
//		Restart timer
//
//---------------------------------------------------------------------------
void
CWallClock::Restart()
{
	syslib::GetTimeOfDay(&m_time, NULL /*timezone*/);
}


// EOF
