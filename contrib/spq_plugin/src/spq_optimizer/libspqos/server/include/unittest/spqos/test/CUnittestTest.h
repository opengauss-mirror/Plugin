//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2015 Pivotal Software, Inc.
//
//	@filename:
//		CUnittestTest.h
//
//	@doc:
//		Test for CUnittest with subtests
//---------------------------------------------------------------------------
#ifndef SPQOS_CUnittestTest_H
#define SPQOS_CUnittestTest_H

#include "spqos/base.h"
#include "spqos/types.h"


namespace spqos
{
//---------------------------------------------------------------------------
//	@class:
//		CUnittestTest
//
//	@doc:
//		Unit test with parameter denoting subtest
//
//---------------------------------------------------------------------------
class CUnittestTest
{
public:
	// unittests
	static SPQOS_RESULT EresSubtest(ULONG ulSubtest);

};	// CAutoMutexTest
}  // namespace spqos

#endif	// !SPQOS_CUnittestTest_H

// EOF
