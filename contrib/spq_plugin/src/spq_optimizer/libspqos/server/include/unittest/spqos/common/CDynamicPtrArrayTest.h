//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2008 Greenplum, Inc.
//
//	@filename:
//		CDynamicPtrArrayTest.h
//
//	@doc:
//		Test for CDynamicPtrArray
//---------------------------------------------------------------------------
#ifndef SPQOS_CDynamicPtrArrayTest_H
#define SPQOS_CDynamicPtrArrayTest_H

#include "spqos/base.h"

namespace spqos
{
//---------------------------------------------------------------------------
//	@class:
//		CDynamicPtrArrayTest
//
//	@doc:
//		Static unit tests
//
//---------------------------------------------------------------------------
class CDynamicPtrArrayTest
{
public:
	// unittests
	static SPQOS_RESULT EresUnittest();
	static SPQOS_RESULT EresUnittest_Basic();
	static SPQOS_RESULT EresUnittest_Ownership();
	static SPQOS_RESULT EresUnittest_ArrayAppend();
	static SPQOS_RESULT EresUnittest_ArrayAppendExactFit();
	static SPQOS_RESULT EresUnittest_PdrspqulSubsequenceIndexes();

	// destructor function for char's
	static void DestroyChar(char *);

};	// class CDynamicPtrArrayTest
}  // namespace spqos

#endif	// !SPQOS_CDynamicPtrArrayTest_H

// EOF
