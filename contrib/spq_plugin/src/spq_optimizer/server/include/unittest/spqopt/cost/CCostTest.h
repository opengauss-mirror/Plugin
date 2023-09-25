//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CCostTest.h
//
//	@doc:
//		Basic tests for costing
//---------------------------------------------------------------------------
#ifndef SPQOPT_CCostTest_H
#define SPQOPT_CCostTest_H

#include "spqos/base.h"


namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CCostTest
//
//	@doc:
//		Unittests for costing
//
//---------------------------------------------------------------------------
class CCostTest
{
private:
	// test cost model parameters
	static void TestParams(CMemoryPool *mp);

public:
	// unittests
	static SPQOS_RESULT EresUnittest();
	static SPQOS_RESULT EresUnittest_Arithmetic();
	static SPQOS_RESULT EresUnittest_Bool();
	static SPQOS_RESULT EresUnittest_Params();
	static SPQOS_RESULT EresUnittest_Parsing();
	static SPQOS_RESULT EresUnittest_ParsingWithException();
	static SPQOS_RESULT EresUnittest_SetParams();

};	// class CCostTest
}  // namespace spqopt

#endif	// !SPQOPT_CCostTest_H

// EOF
