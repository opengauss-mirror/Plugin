//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2015 Pivotal, Inc.
//
//	@filename:
//		CBitmapTest.h
//
//	@doc:
//		Test for optimizing queries that can use a bitmap index
//---------------------------------------------------------------------------
#ifndef SPQOPT_CBitmapTest_H
#define SPQOPT_CBitmapTest_H

#include "spqos/base.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CBitmapTest
//
//	@doc:
//		Unittests
//
//---------------------------------------------------------------------------
class CBitmapTest
{
private:
	// counter used to mark last successful test
	static ULONG m_ulBitmapTestCounter;

public:
	// unittests
	static SPQOS_RESULT EresUnittest();

	static SPQOS_RESULT EresUnittest_RunTests();

};	// class CBitmapTest
}  // namespace spqopt

#endif	// !SPQOPT_CBitmapTest_H

// EOF
