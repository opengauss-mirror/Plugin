//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2008 - 2010 Greenplum, Inc.
//
//	@filename:
//		CMessageRepositoryTest.h
//
//	@doc:
//		Test for CMessageTable
//---------------------------------------------------------------------------
#ifndef SPQOS_CMessageRepositoryTest_H
#define SPQOS_CMessageRepositoryTest_H

#include "spqos/assert.h"
#include "spqos/types.h"

namespace spqos
{
//---------------------------------------------------------------------------
//	@class:
//		CMessageRepositoryTest
//
//	@doc:
//		Static unit tests for message table
//
//---------------------------------------------------------------------------
class CMessageRepositoryTest
{
public:
	// unittests
	static SPQOS_RESULT EresUnittest();
	static SPQOS_RESULT EresUnittest_Basic();
};
}  // namespace spqos

#endif	// !SPQOS_CMessageRepositoryTest_H

// EOF
