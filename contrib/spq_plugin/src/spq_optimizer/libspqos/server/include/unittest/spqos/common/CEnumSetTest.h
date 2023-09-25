//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		CEnumSetTest.h
//
//	@doc:
//		Test for enum sets
//---------------------------------------------------------------------------
#ifndef SPQOS_CEnumSetTest_H
#define SPQOS_CEnumSetTest_H

#include "spqos/types.h"

namespace spqos
{
//---------------------------------------------------------------------------
//	@class:
//		CEnumSetTest
//
//	@doc:
//		Static unit tests for enum set
//
//---------------------------------------------------------------------------
class CEnumSetTest
{
public:
	enum eTest
	{
		eTestOne,
		eTestTwo,

		eTestSentinel
	};

	// unittests
	static SPQOS_RESULT EresUnittest();
	static SPQOS_RESULT EresUnittest_Basics();

};	// class CEnumSetTest
}  // namespace spqos

#endif	// !SPQOS_CEnumSetTest_H

// EOF
