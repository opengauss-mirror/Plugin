//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		CBitSetTest.h
//
//	@doc:
//		Test for CBitSet
//---------------------------------------------------------------------------
#ifndef SPQOS_CBitSetTest_H
#define SPQOS_CBitSetTest_H

#include "spqos/base.h"

namespace spqos
{
//---------------------------------------------------------------------------
//	@class:
//		CBitSetTest
//
//	@doc:
//		Static unit tests for bit set
//
//---------------------------------------------------------------------------
class CBitSetTest
{
public:
	// unittests
	static SPQOS_RESULT EresUnittest();
	static SPQOS_RESULT EresUnittest_Basics();
	static SPQOS_RESULT EresUnittest_Removal();
	static SPQOS_RESULT EresUnittest_SetOps();
	static SPQOS_RESULT EresUnittest_Performance();

};	// class CBitSetTest
}  // namespace spqos

#endif	// !SPQOS_CBitSetTest_H

// EOF
