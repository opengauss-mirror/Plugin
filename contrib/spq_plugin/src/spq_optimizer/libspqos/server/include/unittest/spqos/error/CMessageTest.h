//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2008 - 2010 Greenplum, Inc.
//
//	@filename:
//		CMessageTest.h
//
//	@doc:
//		Test for CMessage
//---------------------------------------------------------------------------
#ifndef SPQOS_CMessageTest_H
#define SPQOS_CMessageTest_H

#include "spqos/assert.h"
#include "spqos/types.h"

namespace spqos
{
//---------------------------------------------------------------------------
//	@class:
//		CMessageTest
//
//	@doc:
//		Static unit tests for messages
//
//---------------------------------------------------------------------------
class CMessageTest
{
public:
	// unittests
	static SPQOS_RESULT EresUnittest();
	static SPQOS_RESULT EresUnittest_BasicWrapper();
	static SPQOS_RESULT EresUnittest_Basic(const void *, ...);
};
}  // namespace spqos

#endif	// !SPQOS_CMessageTest_H

// EOF
