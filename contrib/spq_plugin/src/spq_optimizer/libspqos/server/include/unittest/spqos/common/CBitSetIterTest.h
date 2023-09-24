//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		CBitSetIterTest.h
//
//	@doc:
//		Test for CBitSetIter
//---------------------------------------------------------------------------
#ifndef SPQOS_CBitSetIterTest_H
#define SPQOS_CBitSetIterTest_H

#include "spqos/base.h"

namespace spqos
{
//---------------------------------------------------------------------------
//	@class:
//		CBitSetIterTest
//
//	@doc:
//		Static unit tests for bit set
//
//---------------------------------------------------------------------------
class CBitSetIterTest
{
public:
	// unittests
	static SPQOS_RESULT EresUnittest();
	static SPQOS_RESULT EresUnittest_Basics();

#ifdef SPQOS_DEBUG
	static SPQOS_RESULT EresUnittest_Uninitialized();
	static SPQOS_RESULT EresUnittest_Overrun();
#endif	// SPQOS_DEBUG

};	// class CBitSetIterTest
}  // namespace spqos

#endif	// !SPQOS_CBitSetIterTest_H

// EOF
