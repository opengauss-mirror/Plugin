//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2010 Greenplum, Inc.
//
//	@filename:
//		CStringTest.h
//
//	@doc:
//		Tests for the CStringStatic class
//---------------------------------------------------------------------------
#ifndef SPQOS_CStringTest_H
#define SPQOS_CStringTest_H

#include "spqos/base.h"

namespace spqos
{
//---------------------------------------------------------------------------
//	@class:
//		CStringTest
//
//	@doc:
//		Unittests for strings
//
//---------------------------------------------------------------------------
class CStringTest
{
public:
	// unittests
	static SPQOS_RESULT EresUnittest();
	static SPQOS_RESULT EresUnittest_Equals();
	static SPQOS_RESULT EresUnittest_Append();
	static SPQOS_RESULT EresUnittest_AppendFormat();
};	// class CStringTest
}  // namespace spqos

#endif	// !SPQOS_CStringTest_H

// EOF
