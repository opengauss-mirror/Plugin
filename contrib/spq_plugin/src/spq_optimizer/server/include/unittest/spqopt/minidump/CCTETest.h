//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2015 Pivotal, Inc.
//
//	@filename:
//		CCTETest.h
//
//	@doc:
//		Test for optimizing queries with CTEs
//---------------------------------------------------------------------------
#ifndef SPQOPT_CCTETest_H
#define SPQOPT_CCTETest_H

#include "spqos/base.h"

namespace spqopt
{
//---------------------------------------------------------------------------
//	@class:
//		CCTETest
//
//	@doc:
//		Unittests
//
//---------------------------------------------------------------------------
class CCTETest
{
private:
	// counter used to mark last successful test
	static spqos::ULONG m_ulCTETestCounter;

public:
	// unittests
	static spqos::SPQOS_RESULT EresUnittest();

	static spqos::SPQOS_RESULT EresUnittest_RunTests();

};	// class CCTETest
}  // namespace spqopt

#endif	// !SPQOPT_CCTETest_H

// EOF
