//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2008 - 2010 Greenplum, Inc.
//
//	@filename:
//		CMessageTableTest.h
//
//	@doc:
//		Test for CMessageTable
//---------------------------------------------------------------------------
#ifndef SPQOS_CMessageTableTest_H
#define SPQOS_CMessageTableTest_H

#include "spqos/assert.h"
#include "spqos/types.h"

namespace spqos
{
//---------------------------------------------------------------------------
//	@class:
//		CMessageTableTest
//
//	@doc:
//		Static unit tests for message table
//
//---------------------------------------------------------------------------
class CMessageTableTest
{
public:
	// unittests
	static SPQOS_RESULT EresUnittest();
	static SPQOS_RESULT EresUnittest_Basic();
};
}  // namespace spqos

#endif	// !SPQOS_CMessageTableTest_H

// EOF
