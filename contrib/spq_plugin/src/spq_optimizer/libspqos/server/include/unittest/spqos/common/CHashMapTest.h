//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2010 Greenplum, Inc.
//
//	@filename:
//		CHashMapTest.h
//
//	@doc:
//		Test for CHashMap
//---------------------------------------------------------------------------
#ifndef SPQOS_CHashMapTest_H
#define SPQOS_CHashMapTest_H

#include "spqos/base.h"

namespace spqos
{
//---------------------------------------------------------------------------
//	@class:
//		CHashMapTest
//
//	@doc:
//		Static unit tests
//
//---------------------------------------------------------------------------
class CHashMapTest
{
public:
	// unittests
	static SPQOS_RESULT EresUnittest();
	static SPQOS_RESULT EresUnittest_Basic();
	static SPQOS_RESULT EresUnittest_Ownership();

};	// class CHashMapTest
}  // namespace spqos

#endif	// !SPQOS_CHashMapTest_H

// EOF
