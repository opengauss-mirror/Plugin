//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2008 - 2010 Greenplum, Inc.
//
//	@filename:
//		COstreamStringTest.h
//
//	@doc:
//		Test for COstreamString
//---------------------------------------------------------------------------
#ifndef SPQOS_COstreamStringTest_H
#define SPQOS_COstreamStringTest_H

#include "spqos/base.h"

namespace spqos
{
//---------------------------------------------------------------------------
//	@class:
//		COstreamStringTest
//
//	@doc:
//		Static unit tests for messages
//
//---------------------------------------------------------------------------
class COstreamStringTest
{
public:
	// unittests
	static SPQOS_RESULT EresUnittest();
	static SPQOS_RESULT EresUnittest_Basic();
#ifdef SPQOS_DEBUG
	static SPQOS_RESULT EresUnittest_EndlAssert();
#endif
};
}  // namespace spqos

#endif	// !SPQOS_COstreamStringTest_H

// EOF
