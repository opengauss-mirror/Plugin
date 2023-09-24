//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2008 - 2010 Greenplum, Inc.
//
//	@filename:
//		CErrorHandlerTest.h
//
//	@doc:
//		Test for CMessage
//---------------------------------------------------------------------------
#ifndef SPQOS_CErrorHandlerTest_H
#define SPQOS_CErrorHandlerTest_H

#include "spqos/assert.h"
#include "spqos/types.h"

namespace spqos
{
//---------------------------------------------------------------------------
//	@class:
//		CErrorHandlerTest
//
//	@doc:
//		Static unit tests for error handler base class
//
//---------------------------------------------------------------------------
class CErrorHandlerTest
{
public:
	// unittests
	static SPQOS_RESULT EresUnittest();
	static SPQOS_RESULT EresUnittest_Basic();

#ifdef SPQOS_DEBUG
	static SPQOS_RESULT EresUnittest_BadRethrow();
	static SPQOS_RESULT EresUnittest_BadReset();
	static SPQOS_RESULT EresUnittest_Unhandled();
#endif	// SPQOS_DEBUG
};
}  // namespace spqos

#endif	// !SPQOS_CErrorHandlerTest_H

// EOF
