//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2017 Pivotal, Inc.
//
//	@filename:
//		CSubqueryTest.h
//
//	@doc:
//		Test for subquery optimization
//---------------------------------------------------------------------------
#ifndef SPQOPT_CSubqueryTest_H
#define SPQOPT_CSubqueryTest_H

#include "spqos/base.h"

namespace spqopt
{
class CSubqueryTest
{
private:
	// counter used to mark last successful test
	static spqos::ULONG m_ulSubQueryTestCounter;

public:
	// unittests
	static spqos::SPQOS_RESULT EresUnittest();

	static spqos::SPQOS_RESULT EresUnittest_RunTests();

};	// class CSubqueryTest
}  // namespace spqopt

#endif	// !SPQOPT_CSubqueryTest_H

// EOF
