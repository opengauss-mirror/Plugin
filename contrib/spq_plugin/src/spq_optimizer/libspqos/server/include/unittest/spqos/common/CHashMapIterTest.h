//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CHashMapIterTest.h
//
//	@doc:
//		Test for CHashMapIter
//---------------------------------------------------------------------------
#ifndef SPQOS_CHashMapIterTest_H
#define SPQOS_CHashMapIterTest_H

#include "spqos/base.h"

namespace spqos
{
//---------------------------------------------------------------------------
//	@class:
//		CHashMapIterTest
//
//	@doc:
//		Static unit tests
//
//---------------------------------------------------------------------------
class CHashMapIterTest
{
public:
	// unittests
	static SPQOS_RESULT EresUnittest();
	static SPQOS_RESULT EresUnittest_Basic();

};	// class CHashMapIterTest
}  // namespace spqos

#endif	// !SPQOS_CHashMapIterTest_H

// EOF
