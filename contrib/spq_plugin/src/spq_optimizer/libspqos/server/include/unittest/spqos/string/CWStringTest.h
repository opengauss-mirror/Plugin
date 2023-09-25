//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2008 Greenplum, Inc.
//
//	@filename:
//		CWStringTest.h
//
//	@doc:
//		Tests for the CWStringBase, CWStringConst, CWString classes
//---------------------------------------------------------------------------
#ifndef SPQOS_CWStringTest_H
#define SPQOS_CWStringTest_H

#include "spqos/base.h"

namespace spqos
{
//---------------------------------------------------------------------------
//	@class:
//		CWStringTest
//
//	@doc:
//		Unittests for strings
//
//---------------------------------------------------------------------------
class CWStringTest
{
public:
	// unittests
	static SPQOS_RESULT EresUnittest();
	static SPQOS_RESULT EresUnittest_Initialize();
	static SPQOS_RESULT EresUnittest_Equals();
	static SPQOS_RESULT EresUnittest_Append();
	static SPQOS_RESULT EresUnittest_AppendFormat();
	static SPQOS_RESULT EresUnittest_Copy();
	static SPQOS_RESULT EresUnittest_AppendEscape();
	static SPQOS_RESULT EresUnittest_AppendFormatLarge();
#ifndef SPQOS_Darwin
	static SPQOS_RESULT EresUnittest_AppendFormatInvalidLocale();
#endif
};	// class CWStringTest
}  // namespace spqos

#endif	// !SPQOS_CWStringTest_H

// EOF
