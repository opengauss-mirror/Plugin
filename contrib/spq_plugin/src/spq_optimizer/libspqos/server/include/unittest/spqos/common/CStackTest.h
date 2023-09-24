//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2008 Greenplum, Inc.
//
//	@filename:
//		CStackTest.h
//
//	@doc:
//		Test for CStack
//---------------------------------------------------------------------------
#ifndef SPQOS_CStackTest_H
#define SPQOS_CStackTest_H

#include "spqos/base.h"

namespace spqos
{
//---------------------------------------------------------------------------
//	@class:
//		CStackTest
//
//	@doc:
//		Static unit tests
//
//---------------------------------------------------------------------------
class CStackTest
{
public:
	// unittests
	static SPQOS_RESULT EresUnittest();
	static SPQOS_RESULT EresUnittest_Basic();
	static SPQOS_RESULT EresUnittest_PushPop();

#ifdef SPQOS_DEBUG
	static SPQOS_RESULT EresUnittest_Pop();
#endif

	// destructor function for char's
	static void DestroyChar(char *);

};	// class CStackTest
}  // namespace spqos

#endif	// !SPQOS_CStackTest_H

// EOF
