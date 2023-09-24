//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2015 Pivotal, Inc.
//
//	@filename:
//		CHashSetTest.h
//
//	@doc:
//		Test for CHashSet
//
//	@owner:
//		solimm1
//
//	@test:
//
//
//---------------------------------------------------------------------------
#ifndef SPQOS_CHashSetTest_H
#define SPQOS_CHashSetTest_H

#include "spqos/base.h"

namespace spqos
{
//---------------------------------------------------------------------------
//	@class:
//		CHashSetTest
//
//	@doc:
//		Static unit tests
//
//---------------------------------------------------------------------------
class CHashSetTest
{
public:
	// unittests
	static SPQOS_RESULT EresUnittest();
	static SPQOS_RESULT EresUnittest_Basic();
	static SPQOS_RESULT EresUnittest_Ownership();

};	// class CHashSetTest
}  // namespace spqos

#endif	// !SPQOS_CHashSetTest_H

// EOF
