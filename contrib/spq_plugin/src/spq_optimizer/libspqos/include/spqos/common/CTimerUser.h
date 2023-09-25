//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CTimerUser.h
//
//	@doc:
//		A timer which records elapsed user time;
//---------------------------------------------------------------------------
#ifndef SPQOS_CTimerUser_H
#define SPQOS_CTimerUser_H

#include "spqos/common/ITimer.h"

namespace spqos
{
//---------------------------------------------------------------------------
//	@class:
//		CTimerUser
//
//	@doc:
//		Records user time;
//
//---------------------------------------------------------------------------
class CTimerUser : public ITimer
{
private:
	// actual timer
	RUSAGE m_rusage;

public:
	// ctor
	CTimerUser()
	{
	}

	// retrieve elapsed user time in micro-seconds
	virtual ULONG ElapsedUS() const;

	// restart timer
	virtual void Restart();

};	// class CTimerUser
}  // namespace spqos

#endif	// !SPQOS_CTimerUser_H

// EOF
