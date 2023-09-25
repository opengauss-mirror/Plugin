//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		CAutoTimer.h
//
//	@doc:
//		A timer which records wall-time between construction and destruction;
//---------------------------------------------------------------------------
#ifndef SPQOS_CAutoTimer_H
#define SPQOS_CAutoTimer_H

#include "spqos/common/CStackObject.h"
#include "spqos/common/CWallClock.h"


namespace spqos
{
//---------------------------------------------------------------------------
//	@class:
//		CAutoTimer
//
//	@doc:
//		Wrapper around timer object; prints elapsed time when going out of
//		scope as indicated (ctor argument);
//
//---------------------------------------------------------------------------
class CAutoTimer : public CStackObject
{
private:
	// actual timer
	CWallClock m_clock;

	// label for timer output
	const CHAR *m_timer_text_label;

	// trigger printing at destruction time
	BOOL m_print_text_label;

	// private copy ctor
	CAutoTimer(const CAutoTimer &);

public:
	// ctor
	CAutoTimer(const CHAR *sz, BOOL fPrint);

	// dtor
	~CAutoTimer() throw();

};	// class CAutoTimer
}  // namespace spqos

#endif	// !SPQOS_CAutoTimer_H

// EOF
