//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2015 Pivotal, Inc.
//
//	@filename:
//		CAggTest.h
//
//	@doc:
//		Test for optimizing queries with aggregates
//---------------------------------------------------------------------------
#ifndef SPQOPT_CAggTest_H
#define SPQOPT_CAggTest_H

#include "spqos/base.h"

namespace spqopt
{
//---------------------------------------------------------------------------
//	@class:
//		CAggTest
//
//	@doc:
//		Unittests
//
//---------------------------------------------------------------------------
class CAggTest
{
private:
	// counter used to mark last successful test
	static spqos::ULONG m_ulAggTestCounter;

public:
	// unittests
	static spqos::SPQOS_RESULT EresUnittest();

	static spqos::SPQOS_RESULT EresUnittest_RunTests();

};	// class CAggTest
}  // namespace spqopt

#endif	// !SPQOPT_CAggTest_H

// EOF
