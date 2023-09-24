//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2008 Greenplum, Inc.
//
//	@filename:
//		CExceptionTest.h
//
//	@doc:
//		Test for CException
//---------------------------------------------------------------------------
#ifndef SPQOS_CExceptionTest_H
#define SPQOS_CExceptionTest_H

#include "spqos/assert.h"
#include "spqos/types.h"

namespace spqos
{
//---------------------------------------------------------------------------
//	@class:
//		CExceptionTest
//
//	@doc:
//		Static unit tests for exceptions
//
//---------------------------------------------------------------------------
class CExceptionTest
{
public:
	// unittests
	static SPQOS_RESULT EresUnittest();

	static SPQOS_RESULT EresUnittest_BasicThrow();
	static SPQOS_RESULT EresUnittest_BasicRethrow();
	static SPQOS_RESULT EresUnittest_StackOverflow();
	static SPQOS_RESULT EresUnittest_AdditionOverflow();
	static SPQOS_RESULT EresUnittest_MultiplicationOverflow();

#ifdef SPQOS_DEBUG
	static SPQOS_RESULT EresUnittest_Assert();
	static SPQOS_RESULT EresUnittest_AssertImp();
	static SPQOS_RESULT EresUnittest_AssertIffLHS();
	static SPQOS_RESULT EresUnittest_AssertIffRHS();
#endif	// SPQOS_DEBUG
};
}  // namespace spqos

#endif	// !SPQOS_CExceptionTest_H

// EOF
