//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 EMC Corp.
//
//	@filename:
//		CEnumeratorTest.cpp
//
//	@doc:
//		Test for CEngine
//---------------------------------------------------------------------------
#include "unittest/spqopt/engine/CEnumeratorTest.h"

#include "spqos/base.h"
#include "spqos/error/CAutoTrace.h"
#include "spqos/memory/CAutoMemoryPool.h"
#include "spqos/task/CAutoTraceFlag.h"
#include "spqos/test/CUnittest.h"

#include "spqopt/exception.h"

#include "unittest/spqopt/CTestUtils.h"

using namespace spqopt;


ULONG CEnumeratorTest::m_ulSamplingTestCounter = 0;	 // start from first test
ULONG CEnumeratorTest::m_ulUnsatisfiedRequiredPropertiesTestCounter =
	0;	// start from first test
ULONG CEnumeratorTest::m_ulCompatibleDistributionsTestCounter =
	0;	// start from first test

#ifdef SPQOS_DEBUG
ULONG CEnumeratorTest::m_ulSamplingTestNoMotions = 0;  // start from first test
ULONG CEnumeratorTest::m_ulSamplingTestNoMotionBroadcast =
	0;	// start from first test
ULONG CEnumeratorTest::m_ulSamplingTestNoMotionRedistribute =
	0;	// start from first test
ULONG CEnumeratorTest::m_ulSamplingTestNoMotionGather =
	0;												 // start from first test
ULONG CEnumeratorTest::m_ulSamplingTestNoSort = 0;	 // start from first test
ULONG CEnumeratorTest::m_ulSamplingTestNoSpool = 0;	 // start from first test
ULONG CEnumeratorTest::m_ulSamplingTestNoPartPropagation =
	0;	// start from first test
ULONG CEnumeratorTest::m_ulSamplingTestNoOneStageAgg = 0;
#endif	// SPQOS_DEBUG

// minidump files
const CHAR *rgszSamplePlans[] = {
	"../data/dxl/tpch/q1.mdp",
	"../data/dxl/tpch/q3.mdp",
#ifndef SPQOS_DEBUG
	// run the following queries in optimized build to keep small test execution time
	//		"../data/dxl/tpch/q2.mdp",
	"../data/dxl/tpch/q4.mdp",
	"../data/dxl/tpch/q5.mdp",
	"../data/dxl/tpch/q6.mdp",
	"../data/dxl/tpch/q7.mdp",
	"../data/dxl/tpch/q8.mdp",
	// TODO:  - Jan 26, 2013; enable q9 after figuring out why it does not finish
	//		"../data/dxl/tpch/q9.mdp",
	"../data/dxl/tpch/q10.mdp",
	"../data/dxl/tpch/q11.mdp",
	"../data/dxl/tpch/q12.mdp",
	"../data/dxl/tpch/q13.mdp",
	"../data/dxl/tpch/q14.mdp",
	"../data/dxl/tpch/q15.mdp",
	"../data/dxl/tpch/q16.mdp",
	"../data/dxl/tpch/q17.mdp",
	//		"../data/dxl/tpch/q18.mdp",
	//		"../data/dxl/tpch/q19.mdp",
	"../data/dxl/tpch/q20.mdp",
	"../data/dxl/tpch/q21.mdp",
	"../data/dxl/tpch/q22.mdp",
//		re-enable after regenerating minidump
//		"../data/dxl/minidump/LargeJoins.mdp", // a test for joining all tables
#endif	// SPQOS_DEBUG
};

// minidump files that should not generate plans with unsatisfied required properties
const CHAR *rgszCompatibleDistributions[] = {
	// SINGLETON and UNIVERSAL distributions should be compatible as join children
	"../data/dxl/minidump/JoinWithSingletonAndUniversalBranches.mdp",
	// SINGLETON and SINGLETON distributions should be compatible as join children
	"../data/dxl/minidump/JoinWithSingletonAndSingletonBranches.mdp",
};


// minidump files raising an exception because required properties are not satisfied
const CHAR *rgszUnsatisfiedRequiredPropertiesPlans[] = {
	"../data/dxl/minidump/InvalidUpdatePlan.mdp",
	"../data/dxl/minidump/InvalidDeleteGather.mdp",
	"../data/dxl/minidump/InvalidPlan_CTE-2-all-plans.mdp",
	"../data/dxl/minidump/InvalidPlan_MotionGatherFromMasterToMaster.mdp",
	"../data/dxl/minidump/InvalidPlan_MotionGatherFromMasterToMaster-ScalarDQA.mdp",
	"../data/dxl/minidump/InvalidPlan_IncompatibleDistributionOnJoinBranches.mdp",
};

#ifdef SPQOS_DEBUG
// minidump files for checking generated plan
const CHAR *rgszCheckPlansNoMotions[] = {
	"../data/dxl/minidump/NoMotionsPlan.mdp",
};

const CHAR *rgszCheckPlansNoMotionBroadcast[] = {
	"../data/dxl/minidump/JoinPlan.mdp",
};

const CHAR *rgszCheckPlansNoMotionRedistribute[] = {
	"../data/dxl/minidump/JoinPlan.mdp",
	"../data/dxl/minidump/JoinPlanWithRedistribute.mdp",
};

const CHAR *rgszCheckPlansNoMotionGather[] = {
	"../data/dxl/minidump/NoMotionsPlan.mdp",
};

const CHAR *rgszCheckPlansNoSort[] = {
	"../data/dxl/minidump/NoSortPlan.mdp",
};

const CHAR *rgszCheckPlansNoSpool[] = {
	"../data/dxl/minidump/JoinPlan.mdp",
};

const CHAR *rgszCheckPlansNoPartPropagation[] = {
	"../data/dxl/minidump/NoPartPropagationPlan.mdp",
};

const CHAR *rgszCheckPlansNoOneStageAgg[] = {
	"../data/dxl/tpch/q1.mdp",
};
#endif	// SPQOS_DEBUG

//---------------------------------------------------------------------------
//	@function:
//		CEnumeratorTest::EresUnittest
//
//	@doc:
//		Unittest for plan enumerator
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CEnumeratorTest::EresUnittest()
{
	CUnittest rgut[] = {
		SPQOS_UNITTEST_FUNC(EresUnittest_RunUnsatisfiedRequiredPropertiesTests),
		SPQOS_UNITTEST_FUNC(EresUnittest_Sampling),
		SPQOS_UNITTEST_FUNC(EresUnittest_RunCompatibleDistributions),
#ifdef SPQOS_DEBUG
		SPQOS_UNITTEST_FUNC(EresUnittest_CheckNoMotions),
		SPQOS_UNITTEST_FUNC(EresUnittest_CheckNoMotionBroadcast),
		SPQOS_UNITTEST_FUNC(EresUnittest_CheckNoMotionRedistribute),
		SPQOS_UNITTEST_FUNC(EresUnittest_CheckNoMotionGather),
		SPQOS_UNITTEST_FUNC(EresUnittest_CheckNoSort),
		SPQOS_UNITTEST_FUNC(EresUnittest_CheckNoSpool),
		SPQOS_UNITTEST_FUNC(EresUnittest_CheckNoPartPropagation),
		SPQOS_UNITTEST_FUNC(EresUnittest_CheckNoOneStageAgg),
#endif	// SPQOS_DEBUG
	};

	SPQOS_RESULT eres = CUnittest::EresExecute(rgut, SPQOS_ARRAY_SIZE(rgut));

	// reset metadata cache
	CMDCache::Reset();

	return eres;
}

#ifdef SPQOS_DEBUG
//---------------------------------------------------------------------------
//	@function:
//		CEnumeratorTest::FHasNoMotions
//
//	@doc:
//		Check if a given expression has no Motion nodes
//
//---------------------------------------------------------------------------
BOOL
CEnumeratorTest::FHasNoMotions(CExpression *pexpr)
{
	COperator::EOperatorId rgeopid[] = {
		COperator::EopPhysicalMotionGather,
		COperator::EopPhysicalMotionHashDistribute,
		COperator::EopPhysicalMotionBroadcast,
		COperator::EopPhysicalMotionRandom,
		COperator::EopPhysicalMotionRoutedDistribute};

	return !CUtils::FHasOp(pexpr, rgeopid, SPQOS_ARRAY_SIZE(rgeopid));
}


//---------------------------------------------------------------------------
//	@function:
//		CEnumeratorTest::FHasNoMotionBroadcast
//
//	@doc:
//		Check if a given expression has no Motion Broadcast nodes
//
//---------------------------------------------------------------------------
BOOL
CEnumeratorTest::FHasNoMotionBroadcast(CExpression *pexpr)
{
	COperator::EOperatorId rgeopid[] = {
		COperator::EopPhysicalMotionBroadcast,
	};

	return !CUtils::FHasOp(pexpr, rgeopid, SPQOS_ARRAY_SIZE(rgeopid));
}


//---------------------------------------------------------------------------
//	@function:
//		CEnumeratorTest::FHasNoMotionRedistribute
//
//	@doc:
//		Check if a given expression has no Motion Redistribute nodes
//
//---------------------------------------------------------------------------
BOOL
CEnumeratorTest::FHasNoMotionRedistribute(CExpression *pexpr)
{
	COperator::EOperatorId rgeopid[] = {
		COperator::EopPhysicalMotionHashDistribute,
		COperator::EopPhysicalMotionRandom,
		COperator::EopPhysicalMotionRoutedDistribute};

	return !CUtils::FHasOp(pexpr, rgeopid, SPQOS_ARRAY_SIZE(rgeopid));
}

//---------------------------------------------------------------------------
//	@function:
//		CEnumeratorTest::FHasNoMotionGather
//
//	@doc:
//		Check if a given expression has no Motion Gather nodes
//
//---------------------------------------------------------------------------
BOOL
CEnumeratorTest::FHasNoMotionGather(CExpression *pexpr)
{
	COperator::EOperatorId rgeopid[] = {
		COperator::EopPhysicalMotionGather,
	};

	return !CUtils::FHasOp(pexpr, rgeopid, SPQOS_ARRAY_SIZE(rgeopid));
}

//---------------------------------------------------------------------------
//	@function:
//		CEnumeratorTest::FHasNoSort
//
//	@doc:
//		Check if a given expression has no Sort nodes
//
//---------------------------------------------------------------------------
BOOL
CEnumeratorTest::FHasNoSort(CExpression *pexpr)
{
	COperator::EOperatorId rgeopid[] = {
		COperator::EopPhysicalSort,
	};

	return !CUtils::FHasOp(pexpr, rgeopid, SPQOS_ARRAY_SIZE(rgeopid));
}


//---------------------------------------------------------------------------
//	@function:
//		CEnumeratorTest::FHasNoSpool
//
//	@doc:
//		Check if a given expression has no Spool nodes
//
//---------------------------------------------------------------------------
BOOL
CEnumeratorTest::FHasNoSpool(CExpression *pexpr)
{
	COperator::EOperatorId rgeopid[] = {
		COperator::EopPhysicalSpool,
	};

	return !CUtils::FHasOp(pexpr, rgeopid, SPQOS_ARRAY_SIZE(rgeopid));
}



//---------------------------------------------------------------------------
//	@function:
//		CEnumeratorTest::FHasNoPartPropagation
//
//	@doc:
//		Check if a given expression has no Part Propagation nodes
//
//---------------------------------------------------------------------------
BOOL
CEnumeratorTest::FHasNoPartPropagation(CExpression *pexpr)
{
	COperator::EOperatorId rgeopid[] = {
		COperator::EopPhysicalPartitionSelector};

	return !CUtils::FHasOp(pexpr, rgeopid, SPQOS_ARRAY_SIZE(rgeopid));
}

//---------------------------------------------------------------------------
//	@function:
//		CEnumeratorTest::FHasNoOneStageAgg
//
//	@doc:
//		Check if a given expression has no one stage agg nodes
//
//---------------------------------------------------------------------------
BOOL
CEnumeratorTest::FHasNoOneStageAgg(CExpression *pexpr)
{
	return !CUtils::FHasOneStagePhysicalAgg(pexpr);
}

#endif	// SPQOS_DEBUG


//---------------------------------------------------------------------------
//	@function:
//		CEnumeratorTest::EresUnittest_Sampling
//
//	@doc:
//		Plan sampling test
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CEnumeratorTest::EresUnittest_Sampling()
{
	SPQOS_RESULT eres = SPQOS_OK;
	SPQOS_TRY
	{
		eres = CTestUtils::EresSamplePlans(
			rgszSamplePlans, SPQOS_ARRAY_SIZE(rgszSamplePlans),
			&m_ulSamplingTestCounter, 1,  // ulSessionId
			1							  // ulCmdId
		);
	}
	SPQOS_CATCH_EX(ex)
	{
		if (!SPQOS_MATCH_EX(ex, spqopt::ExmaSPQOPT,
						   spqopt::ExmiUnsatisfiedRequiredProperties))
		{
			SPQOS_RETHROW(ex);
		}
		else
		{
			// Some enumerated plans may be invalid and throw this error.
			eres = SPQOS_OK;
			SPQOS_RESET_EX;
		}
	}
	SPQOS_CATCH_END;
	return eres;
}


//---------------------------------------------------------------------------
//	@function:
//		CEnumeratorTest::EresUnittest_RunUnsatisfiedRequiredPropertiesTests
//
//	@doc:
//		Run plans that fail because required properties are unsatisfied.
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CEnumeratorTest::EresUnittest_RunUnsatisfiedRequiredPropertiesTests()
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	const ULONG ulTests =
		SPQOS_ARRAY_SIZE(rgszUnsatisfiedRequiredPropertiesPlans);

	SPQOS_RESULT eres = SPQOS_OK;
	for (ULONG ul = 0; ul < ulTests; ++ul)
	{
		SPQOS_TRY
		{
			(void) CTestUtils::EresSamplePlans(
				&rgszUnsatisfiedRequiredPropertiesPlans[ul],
				1,	// ulTests
				&m_ulUnsatisfiedRequiredPropertiesTestCounter,
				1,	// ulSessionId
				1	// ulCmdId
			);
			{
				CAutoTrace at(mp);
				at.Os() << std::endl
						<< "Minidump "
						<< rgszUnsatisfiedRequiredPropertiesPlans[ul];
				at.Os()
					<< " did not raise UnsatisfiedRequiredProperties, as expected"
					<< std::endl;
			}
			eres = SPQOS_FAILED;	 // Should never get here.
		}
		SPQOS_CATCH_EX(ex)
		{
			if (!SPQOS_MATCH_EX(ex, spqopt::ExmaSPQOPT,
							   spqopt::ExmiUnsatisfiedRequiredProperties))
			{
				SPQOS_RETHROW(ex);
			}
			else
			{
				SPQOS_RESET_EX;
			}
		}
		SPQOS_CATCH_END;
	}
	return eres;
}


//---------------------------------------------------------------------------
//	@function:
//		CEnumeratorTest::EresUnittest_RunCompatibleDistributions
//
//	@doc:
//		Run plans testing that various distributions are compatible and
//		do not raise any UnsatisfiedRequiredProperties exceptions.
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CEnumeratorTest::EresUnittest_RunCompatibleDistributions()
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();
	const ULONG ulTests = SPQOS_ARRAY_SIZE(rgszCompatibleDistributions);

	return CTestUtils::EresRunMinidumps(mp, rgszCompatibleDistributions,
										ulTests,
										&m_ulCompatibleDistributionsTestCounter,
										1,	   // ulSessionId
										1,	   // ulCmdId
										true,  // fMatchPlans
										false  // fTestSpacePruning
	);
}

#ifdef SPQOS_DEBUG
//---------------------------------------------------------------------------
//	@function:
//		CEnumeratorTest::EresUnittest_CheckPlans
//
//	@doc:
//		Test plan checking
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CEnumeratorTest::EresUnittest_CheckPlans(EOptTraceFlag eopttrace,
										 FnPlanChecker *pfpc,
										 ULONG *pulTestCounter,
										 const CHAR *rgszCheckPlans[],
										 ULONG ulTests)
{
	SPQOS_ASSERT(NULL != pfpc);
	SPQOS_ASSERT(NULL != pulTestCounter);
	SPQOS_ASSERT(NULL != rgszCheckPlans);

	SPQOS_RESULT eres = SPQOS_OK;
	SPQOS_TRY
	{
		CAutoTraceFlag atf(eopttrace, true /*fSet*/);

		eres =
			CTestUtils::EresCheckPlans(rgszCheckPlans, ulTests, pulTestCounter,
									   1,	 // ulSessionId
									   1,	 // ulCmdId
									   pfpc	 // plan checking function
			);
	}
	SPQOS_CATCH_EX(ex)
	{
		if (!SPQOS_MATCH_EX(ex, spqopt::ExmaSPQOPT,
						   spqopt::ExmiUnsatisfiedRequiredProperties))
		{
			SPQOS_RETHROW(ex);
		}
		else
		{
			// Some enumerated plans may be invalid and throw this error.
			eres = SPQOS_OK;
			SPQOS_RESET_EX;
		}
	}

	SPQOS_CATCH_END;
	return eres;
}


//---------------------------------------------------------------------------
//	@function:
//		CEnumeratorTest::EresUnittest_CheckNoMotions
//
//	@doc:
//		Test plan checking for no motions
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CEnumeratorTest::EresUnittest_CheckNoMotions()
{
	return EresUnittest_CheckPlans(
		EopttraceDisableMotions, FHasNoMotions, &m_ulSamplingTestNoMotions,
		rgszCheckPlansNoMotions, SPQOS_ARRAY_SIZE(rgszCheckPlansNoMotions));
}

//---------------------------------------------------------------------------
//	@function:
//		CEnumeratorTest::EresUnittest_CheckNoMotionBroadcast
//
//	@doc:
//		Test plan checking for no MotionBroadcast
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CEnumeratorTest::EresUnittest_CheckNoMotionBroadcast()
{
	return EresUnittest_CheckPlans(
		EopttraceDisableMotionBroadcast, FHasNoMotionBroadcast,
		&m_ulSamplingTestNoMotionBroadcast, rgszCheckPlansNoMotionBroadcast,
		SPQOS_ARRAY_SIZE(rgszCheckPlansNoMotionBroadcast));
}

//---------------------------------------------------------------------------
//	@function:
//		CEnumeratorTest::EresUnittest_CheckNoMotionRedistribute
//
//	@doc:
//		Test plan checking for no motion redistribute
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CEnumeratorTest::EresUnittest_CheckNoMotionRedistribute()
{
	// disable additional redistribute enforcers
	CAutoTraceFlag atf1(EopttraceDisableMotionRandom, true /*fSet*/);
	CAutoTraceFlag atf2(EopttraceDisableMotionRountedDistribute, true /*fSet*/);

	return EresUnittest_CheckPlans(
		EopttraceDisableMotionHashDistribute, FHasNoMotionRedistribute,
		&m_ulSamplingTestNoMotionRedistribute,
		rgszCheckPlansNoMotionRedistribute,
		SPQOS_ARRAY_SIZE(rgszCheckPlansNoMotionRedistribute));
}

//---------------------------------------------------------------------------
//	@function:
//		CEnumeratorTest::EresUnittest_CheckNoMotionGather
//
//	@doc:
//		Test plan checking for no Motion Gather
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CEnumeratorTest::EresUnittest_CheckNoMotionGather()
{
	return EresUnittest_CheckPlans(
		EopttraceDisableMotionGather, FHasNoMotionGather,
		&m_ulSamplingTestNoMotionGather, rgszCheckPlansNoMotionGather,
		SPQOS_ARRAY_SIZE(rgszCheckPlansNoMotionGather));
}

//---------------------------------------------------------------------------
//	@function:
//		CEnumeratorTest::EresUnittest_CheckNoSort
//
//	@doc:
//		Test plan checking for no sort nodes
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CEnumeratorTest::EresUnittest_CheckNoSort()
{
	return EresUnittest_CheckPlans(
		EopttraceDisableSort, FHasNoSort, &m_ulSamplingTestNoSort,
		rgszCheckPlansNoSort, SPQOS_ARRAY_SIZE(rgszCheckPlansNoSort));
}


//---------------------------------------------------------------------------
//	@function:
//		CEnumeratorTest::EresUnittest_CheckNoSpool
//
//	@doc:
//		Test plan checking for no spool nodes
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CEnumeratorTest::EresUnittest_CheckNoSpool()
{
	return EresUnittest_CheckPlans(
		EopttraceDisableSpool, FHasNoSpool, &m_ulSamplingTestNoSpool,
		rgszCheckPlansNoSpool, SPQOS_ARRAY_SIZE(rgszCheckPlansNoSpool));
}


//---------------------------------------------------------------------------
//	@function:
//		CEnumeratorTest::EresUnittest_CheckNoPartPropagation
//
//	@doc:
//		Test plan checking for no partition propagation nodes
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CEnumeratorTest::EresUnittest_CheckNoPartPropagation()
{
	SPQOS_RESULT eres = SPQOS_OK;
	SPQOS_TRY
	{
		eres = EresUnittest_CheckPlans(
			EopttraceDisablePartPropagation, FHasNoPartPropagation,
			&m_ulSamplingTestNoPartPropagation, rgszCheckPlansNoPartPropagation,
			SPQOS_ARRAY_SIZE(rgszCheckPlansNoPartPropagation));
	}
	SPQOS_CATCH_EX(ex)
	{
		if (!SPQOS_MATCH_EX(ex, spqopt::ExmaSPQOPT, spqopt::ExmiNoPlanFound))
		{
			SPQOS_RETHROW(ex);
		}
		else
		{
			// optimizer should find No Valid Plan if PartPropgataion is disabled
			eres = SPQOS_OK;
			SPQOS_RESET_EX;
		}
	}
	SPQOS_CATCH_END;

	return eres;
}


//---------------------------------------------------------------------------
//	@function:
//		CEnumeratorTest::EresUnittest_CheckNoOneStageAgg
//
//	@doc:
//		Test plan checking for no one atage agg nodes
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CEnumeratorTest::EresUnittest_CheckNoOneStageAgg()
{
	return EresUnittest_CheckPlans(
		EopttraceForceMultiStageAgg, FHasNoOneStageAgg,
		&m_ulSamplingTestNoOneStageAgg, rgszCheckPlansNoOneStageAgg,
		SPQOS_ARRAY_SIZE(rgszCheckPlansNoOneStageAgg));
}


#endif	// SPQOS_DEBUG

// EOF
