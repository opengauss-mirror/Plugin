//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CJoinOrderTest.h
//
//	@doc:
//		Tests for join ordering
//---------------------------------------------------------------------------
#ifndef SPQOPT_CJoinOrderTest_H
#define SPQOPT_CJoinOrderTest_H

#include "spqos/base.h"

#include "spqopt/operators/CExpression.h"

namespace spqopt
{
//---------------------------------------------------------------------------
//	@class:
//		CJoinOrderTest
//
//	@doc:
//		Tests for join ordering
//
//---------------------------------------------------------------------------
class CJoinOrderTest
{
private:
	// counter used to mark last successful test
	static ULONG m_ulTestCounter;

public:
	// unittests
	static SPQOS_RESULT EresUnittest();
	static SPQOS_RESULT EresUnittest_ExpandMinCard();
	static SPQOS_RESULT EresUnittest_RunTests();

};	// class CJoinOrderTest
}  // namespace spqopt


#endif	// !SPQOPT_CJoinOrderTest_H

// EOF
