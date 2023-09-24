//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		ITimer.h
//
//	@doc:
//		A timer which records time between construction and the ElapsedMS call;
//---------------------------------------------------------------------------
#ifndef SPQOS_ITimer_H
#define SPQOS_ITimer_H

#include "spqos/types.h"
#include "spqos/utils.h"

namespace spqos
{
//---------------------------------------------------------------------------
//	@class:
//		ITimer
//
//	@doc:
//		Timer interface;
//
//---------------------------------------------------------------------------
class ITimer
{
private:
	// private copy ctor
	ITimer(const ITimer &);

public:
	// ctor
	ITimer()
	{
	}

	// dtor
	virtual ~ITimer()
	{
	}

	// retrieve elapsed time in micro-seconds
	virtual ULONG ElapsedUS() const = 0;

	// retrieve elapsed time in milli-seconds
	ULONG
	ElapsedMS() const
	{
		return ElapsedUS() / SPQOS_USEC_IN_MSEC;
	}

	// restart timer
	virtual void Restart() = 0;

};	// class ITimer

}  // namespace spqos

#endif	// !SPQOS_ITimer_H

// EOF
