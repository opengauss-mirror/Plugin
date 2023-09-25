//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal, Inc.
//
//	@filename:
//		CConstExprEvaluationDefaultTest.h
//
//	@doc:
//		Unit tests for CConstExprEvaluatorDefault
//
//	@owner:
//
//
//	@test:
//
//---------------------------------------------------------------------------

#ifndef SPQOPT_CConstExprEvaluatorDefaultTest_H
#define SPQOPT_CConstExprEvaluatorDefaultTest_H

#include "spqos/base.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CConstExprEvaluatorDefaultTest
//
//	@doc:
//		Unit tests for CConstExprEvaluatorDefault
//
//---------------------------------------------------------------------------
class CConstExprEvaluatorDefaultTest
{
public:
	// run unittests
	static SPQOS_RESULT EresUnittest();
};
}  // namespace spqopt

#endif	// !SPQOPT_CConstExprEvaluatorDefaultTest_H

// EOF
