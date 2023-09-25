//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2008 - 2010 Greenplum, Inc.
//
//	@filename:
//		COstreamBasicTest.h
//
//	@doc:
//		Test for COstreamBasic
//---------------------------------------------------------------------------
#ifndef SPQOS_COstreamBasicTest_H
#define SPQOS_COstreamBasicTest_H

#include "spqos/assert.h"
#include "spqos/types.h"

namespace spqos
{
//---------------------------------------------------------------------------
//	@class:
//		COstreamBasicTest
//
//	@doc:
//		Static unit tests for messages
//
//---------------------------------------------------------------------------
class COstreamBasicTest
{
public:
	// unittests
	static SPQOS_RESULT EresUnittest();
	static SPQOS_RESULT EresUnittest_Basic();
	static SPQOS_RESULT EresUnittest_Strings();
	static SPQOS_RESULT EresUnittest_Numbers();
};
}  // namespace spqos

#endif	// !SPQOS_COstreamBasicTest_H

// EOF
