//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 EMC Corp.
//
//	@filename:
//		COptimizationJobsTest.h
//
//	@doc:
//		Test for jobs created during optimization
//---------------------------------------------------------------------------
#ifndef SPQOPT_COptimizationJobsTest_H
#define SPQOPT_COptimizationJobsTest_H

#include "spqos/base.h"


namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		COptimizationJobsTest
//
//	@doc:
//		unittest for optimization jobs
//
//---------------------------------------------------------------------------
class COptimizationJobsTest
{
public:
	// unittests driver
	static SPQOS_RESULT EresUnittest();

	// test of optimization jobs state machines
	static SPQOS_RESULT EresUnittest_StateMachine();

};	// COptimizationJobsTest

}  // namespace spqopt

#endif	// !SPQOPT_COptimizationJobsTest_H


// EOF
