//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2008 Greenplum, Inc.
//
//	@filename:
//		CBitVectorTest.h
//
//	@doc:
//		Unit test for CBitVector
//---------------------------------------------------------------------------
#ifndef SPQOS_CBitVectorTest_H
#define SPQOS_CBitVectorTest_H

#include "spqos/base.h"

namespace spqos
{
//---------------------------------------------------------------------------
//	@class:
//		CBitVectorTest
//
//	@doc:
//		Static unit tests for bit vector
//
//---------------------------------------------------------------------------
class CBitVectorTest
{
public:
	// unittests
	static SPQOS_RESULT EresUnittest();
	static SPQOS_RESULT EresUnittest_Basics();
	static SPQOS_RESULT EresUnittest_SetOps();
	static SPQOS_RESULT EresUnittest_Cursor();
	static SPQOS_RESULT EresUnittest_Random();

#ifdef SPQOS_DEBUG
	static SPQOS_RESULT EresUnittest_OutOfBounds();
#endif	// SPQOS_DEBUG

};	// class CBitVectorTest
}  // namespace spqos

#endif	// !SPQOS_CBitVectorTest_H

// EOF
