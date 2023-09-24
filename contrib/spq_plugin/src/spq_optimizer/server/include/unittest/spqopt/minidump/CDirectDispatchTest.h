//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2015 Pivotal, Inc.
//
//	@filename:
//		CDirectDispatchTest.h
//
//	@doc:
//		Test for direct dispatch
//---------------------------------------------------------------------------
#ifndef SPQOPT_CDirectDispatchTest_H
#define SPQOPT_CDirectDispatchTest_H

#include "spqos/base.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CDirectDispatchTest
//
//	@doc:
//		Unittests
//
//---------------------------------------------------------------------------
class CDirectDispatchTest
{
private:
	// counter used to mark last successful test
	static ULONG m_ulDirectDispatchCounter;

public:
	// unittests
	static SPQOS_RESULT EresUnittest();

	static SPQOS_RESULT EresUnittest_RunTests();

};	// class CDirectDispatchTest
}  // namespace spqopt

#endif	// !SPQOPT_CDirectDispatchTest_H

// EOF
