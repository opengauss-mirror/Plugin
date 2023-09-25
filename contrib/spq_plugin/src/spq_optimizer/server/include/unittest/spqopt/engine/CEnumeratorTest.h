//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 EMC Corp.
//
//	@filename:
//		CEnumeratorTest.h
//
//	@doc:
//		Test for plan enumeration and sampling
//---------------------------------------------------------------------------
#ifndef SPQOPT_CEnumeratorTest_H
#define SPQOPT_CEnumeratorTest_H

#include "spqos/base.h"

#include "spqopt/engine/CEnumeratorConfig.h"
#include "spqopt/operators/CExpression.h"
#include "naucrates/traceflags/traceflags.h"

using namespace spqos;

namespace spqopt
{
//---------------------------------------------------------------------------
//	@class:
//		CEnumeratorTest
//
//	@doc:
//		Plan enumeration and sampling tests
//
//---------------------------------------------------------------------------
class CEnumeratorTest
{
private:
	// counter used to mark last successful sampling test
	static ULONG m_ulSamplingTestCounter;

	// counter used to mark last successful test for unsatisfied required properties
	static ULONG m_ulUnsatisfiedRequiredPropertiesTestCounter;

	// counter for last successful test for compatible distributions
	static ULONG m_ulCompatibleDistributionsTestCounter;

	// counter for last successful test for testing all enumerated plans
	static ULONG m_ulTestAllPlansTestCounter;

#ifdef SPQOS_DEBUG
	// counter used to mark last successful plan checking test for no motions
	static ULONG m_ulSamplingTestNoMotions;

	// counter used to mark last successful plan checking test for no Motion Broadcast
	static ULONG m_ulSamplingTestNoMotionBroadcast;

	// counter used to mark last successful plan checking test for no Motion Redistribute
	static ULONG m_ulSamplingTestNoMotionRedistribute;

	// counter used to mark last successful plan checking test for no Motion Gather
	static ULONG m_ulSamplingTestNoMotionGather;

	// counter used to mark last successful plan checking test for no sort
	static ULONG m_ulSamplingTestNoSort;

	// counter used to mark last successful plan checking test for no spool
	static ULONG m_ulSamplingTestNoSpool;

	// counter used to mark last successful plan checking test for no part propagation
	static ULONG m_ulSamplingTestNoPartPropagation;

	// counter used to mark last successful plan checking test for no one stage agg
	static ULONG m_ulSamplingTestNoOneStageAgg;

	// check if a given expression has no Motion nodes
	static BOOL FHasNoMotions(CExpression *pexpr);

	// check if a given expression has no MotionBroadcast nodes
	static BOOL FHasNoMotionBroadcast(CExpression *pexpr);

	// check if a given expression has no motion redistribute nodes
	static BOOL FHasNoMotionRedistribute(CExpression *pexpr);

	// check if a given expression has no Motion Gather nodes
	static BOOL FHasNoMotionGather(CExpression *pexpr);

	// check if a given expression has no Sort nodes
	static BOOL FHasNoSort(CExpression *pexpr);

	// check if a given expression has no Spool nodes
	static BOOL FHasNoSpool(CExpression *pexpr);

	// check if a given expression has no Part Propagation nodes
	static BOOL FHasNoPartPropagation(CExpression *pexpr);

	// check if a given expression has no one stage agg nodes
	static BOOL FHasNoOneStageAgg(CExpression *pexpr);

	// test plan checking
	static SPQOS_RESULT EresUnittest_CheckPlans(EOptTraceFlag eopttrace,
											   FnPlanChecker *pfpc,
											   ULONG *pulTestCounter,
											   const CHAR *rgszCheckPlans[],
											   ULONG ulTests);

#endif	// SPQOS_DEBUG

public:
	// main driver
	static SPQOS_RESULT EresUnittest();

	// test plan sampling
	static SPQOS_RESULT EresUnittest_Sampling();

	// test all enumerated plans to check if any plan matches the minidump
	static SPQOS_RESULT EresUnittest_TestAllPlans();

	// test expected errors
	static SPQOS_RESULT EresUnittest_RunUnsatisfiedRequiredPropertiesTests();

	// test that valid distributions are not rejected
	static SPQOS_RESULT EresUnittest_RunCompatibleDistributions();

#ifdef SPQOS_DEBUG
	// test plan checking for no motions
	static SPQOS_RESULT EresUnittest_CheckNoMotions();

	// test plan checking for no motion broadcast
	static SPQOS_RESULT EresUnittest_CheckNoMotionBroadcast();

	// test plan checking for no motion broadcast
	static SPQOS_RESULT EresUnittest_CheckNoMotionRedistribute();

	// test plan checking for no motion gather
	static SPQOS_RESULT EresUnittest_CheckNoMotionGather();

	// test plan checking for no sort
	static SPQOS_RESULT EresUnittest_CheckNoSort();

	// test plan checking for no spool
	static SPQOS_RESULT EresUnittest_CheckNoSpool();

	// test plan checking for no part propagation
	static SPQOS_RESULT EresUnittest_CheckNoPartPropagation();

	// test plan checking for no one stage agg
	static SPQOS_RESULT EresUnittest_CheckNoOneStageAgg();

#endif	// SPQOS_DEBUG


};	// class CEnumeratorTest
}  // namespace spqopt

#endif	// !SPQOPT_CEnumeratorTest_H


// EOF
