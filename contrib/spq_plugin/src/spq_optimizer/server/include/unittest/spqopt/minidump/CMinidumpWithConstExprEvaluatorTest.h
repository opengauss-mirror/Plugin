//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal, Inc.
//
//	@filename:
//		CMinidumpWithConstExprEvaluatorTest.h
//
//	@doc:
//		Tests minidumps with constant expression evaluator turned on
//
//	@owner:
//
//
//	@test:
//
//---------------------------------------------------------------------------

#ifndef SPQOPT_CMinidumpWithConstExprEvaluatorTest_H
#define SPQOPT_CMinidumpWithConstExprEvaluatorTest_H

#include "spqos/base.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CMinidumpWithConstExprEvaluatorTest
//
//	@doc:
//		Unittests
//
//---------------------------------------------------------------------------
class CMinidumpWithConstExprEvaluatorTest
{
private:
	// counter used to mark last successful test
	static ULONG m_ulTestCounter;

public:
	// unittests
	static SPQOS_RESULT EresUnittest();

	static SPQOS_RESULT EresUnittest_RunMinidumpTestsWithConstExprEvaluatorOn();

};	// class CMinidumpWithConstExprEvaluatorTest
}  // namespace spqopt

#endif	// !SPQOPT_CMinidumpWithConstExprEvaluatorTest_H

// EOF
