//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		CNameTest.h
//
//	@doc:
//      Test for CName
//---------------------------------------------------------------------------
#ifndef SPQOPT_CNameTest_H
#define SPQOPT_CNameTest_H

#include "spqos/base.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CNameTest
//
//	@doc:
//		Static unit tests
//
//---------------------------------------------------------------------------
class CNameTest
{
public:
	// unittests
	static SPQOS_RESULT EresUnittest();
	static SPQOS_RESULT EresUnittest_Basic();
	static SPQOS_RESULT EresUnittest_Ownership();

};	// class CNameTest
}  // namespace spqopt

#endif	// !SPQOPT_CNameTest_H

// EOF
