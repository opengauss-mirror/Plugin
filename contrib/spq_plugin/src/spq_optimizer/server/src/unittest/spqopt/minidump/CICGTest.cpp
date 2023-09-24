//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CICGTest.cpp
//
//	@doc:
//		Test for installcheck-good bugs
//---------------------------------------------------------------------------

#include "unittest/spqopt/minidump/CICGTest.h"

#include "spqos/error/CAutoTrace.h"
#include "spqos/task/CAutoTraceFlag.h"

#include "spqopt/base/CAutoOptCtxt.h"
#include "spqopt/engine/CCTEConfig.h"
#include "spqopt/engine/CEnumeratorConfig.h"
#include "spqopt/engine/CStatisticsConfig.h"
#include "spqopt/exception.h"
#include "spqopt/mdcache/CMDCache.h"
#include "spqopt/minidump/CMinidumperUtils.h"
#include "spqopt/optimizer/COptimizerConfig.h"
#include "naucrates/dxl/operators/CDXLNode.h"
#include "naucrates/dxl/operators/CDXLPhysicalNLJoin.h"
#include "naucrates/exception.h"

#include "unittest/base.h"
#include "unittest/spqopt/CConstExprEvaluatorForDates.h"
#include "unittest/spqopt/CTestUtils.h"

using namespace spqdxl;

ULONG CICGTest::m_ulTestCounter = 0;  // start from first test
ULONG CICGTest::m_ulUnsupportedTestCounter =
	0;	// start from first unsupported test
ULONG CICGTest::m_ulTestCounterPreferHashJoinToIndexJoin = 0;
ULONG CICGTest::m_ulTestCounterPreferIndexJoinToHashJoin = 0;
ULONG CICGTest::m_ulNegativeIndexApplyTestCounter = 0;
ULONG CICGTest::m_ulTestCounterNoAdditionTraceFlag = 0;

// minidump files
const CHAR *rgszFileNames[] = {
	"../data/dxl/minidump/InsertIntoNonNullAfterDroppingColumn.mdp",
	"../data/dxl/minidump/OptimizerConfigWithSegmentsForCosting.mdp",
	"../data/dxl/minidump/QueryMismatchedDistribution.mdp",
	"../data/dxl/minidump/QueryMismatchedDistribution-DynamicIndexScan.mdp",
	"../data/dxl/minidump/3WayJoinOnMultiDistributionColumnsTables.mdp",
	"../data/dxl/minidump/3WayJoinOnMultiDistributionColumnsTablesNoMotion.mdp",
	"../data/dxl/minidump/4WayJoinInferredPredsRemovedWith2Motion.mdp",
	"../data/dxl/minidump/NoRedistributeOnAppend.mdp",
	"../data/dxl/minidump/ConstraintIntervalIncludesNull.mdp",
	"../data/dxl/minidump/ConstraintIntervalNotIncludesNull.mdp",
	"../data/dxl/minidump/ConstraintIntervalWithBoolNotIncludesNull.mdp",
	"../data/dxl/minidump/ConstraintIntervalWithBoolNotIncludesNull.mdp",
	"../data/dxl/minidump/ConstraintIntervalWithInIncludesNull.mdp",
	"../data/dxl/minidump/ConstraintIntervalWithInNotIncludesNull.mdp",
	"../data/dxl/minidump/ConstraintIntervalWithInIncludesNullArray.mdp",
	"../data/dxl/minidump/ConstraintIntervalWithNotIncludesNull.mdp",
	"../data/dxl/minidump/ConstraintIntervalWithMultiColumnsIncludeNull.mdp",
	"../data/dxl/minidump/ConstraintIntervalWithMultiColumnsNotIncludeNull.mdp",

#ifndef SPQOS_DEBUG
	"../data/dxl/minidump/HAWQ-TPCH-Stat-Derivation.mdp",
	"../data/dxl/minidump/HJN-Redistribute-One-Side.mdp",
	"../data/dxl/minidump/TPCH-Q5.mdp",
	"../data/dxl/minidump/TPCDS-39-InnerJoin-JoinEstimate.mdp",
	"../data/dxl/minidump/TPCH-Partitioned-256GB.mdp",
	// TODO:  - Jul 31st 2018; disabling it since new cost model picks up Indexed nested Loop Joi
	// however the comment on file says that it should not pick Indexed Nested Loop Join.
	// disabling it for now. Revisit this test when we upgrade scan cost model.
	// "../data/dxl/minidump/Tpcds-10TB-Q37-NoIndexJoin.mdp",

	// TODO:  - 06/29/2015: the row estimate for 32-bit rhel is off in the 6th decimel place
	"../data/dxl/minidump/retail_28.mdp",
	"../data/dxl/minidump/JoinNDVRemain.mdp",
	"../data/dxl/minidump/Least-Greatest.mdp",
#endif
};

struct UnSupportedTestCase
{
	// file name of minidump
	const CHAR *filename;

	// expected exception major
	ULONG major;

	// expected exception minor
	ULONG minor;
};

// unsupported minidump files
const struct UnSupportedTestCase unSupportedTestCases[] = {
	{"../data/dxl/minidump/OneSegmentGather.mdp", spqdxl::ExmaDXL,
	 spqdxl::ExmiExpr2DXLUnsupportedFeature},
	{"../data/dxl/minidump/CTEWithOuterReferences.mdp", spqopt::ExmaSPQOPT,
	 spqopt::ExmiUnsupportedOp},
	{"../data/dxl/minidump/CTEMisAlignedProducerConsumer.mdp", spqopt::ExmaSPQOPT,
	 spqopt::ExmiCTEProducerConsumerMisAligned}};

// negative index apply tests
const CHAR *rgszNegativeIndexApplyFileNames[] = {
	"../data/dxl/minidump/Negative-IndexApply1.mdp",
	"../data/dxl/minidump/Negative-IndexApply2.mdp",
};

// index join penalization tests
const CHAR *rgszPreferHashJoinVersusIndexJoin[] = {
	"../data/dxl/indexjoin/positive_04.mdp"};


//---------------------------------------------------------------------------
//	@function:
//		CICGTest::EresUnittest
//
//	@doc:
//		Unittest for expressions
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CICGTest::EresUnittest()
{
	CUnittest rgut[] = {
		// keep test for testing partially supported operators/xforms
		SPQOS_UNITTEST_FUNC(CICGTest::EresUnittest_RunUnsupportedMinidumpTests),
		SPQOS_UNITTEST_FUNC(CICGTest::EresUnittest_NegativeIndexApplyTests),

		SPQOS_UNITTEST_FUNC(CICGTest::EresUnittest_RunMinidumpTests),
		SPQOS_UNITTEST_FUNC(
			CICGTest::EresUnittest_RunTestsWithoutAdditionalTraceFlags),

#ifndef SPQOS_DEBUG
		// This test is slow in debug build because it has to free a lot of memory structures
		SPQOS_UNITTEST_FUNC(
			EresUnittest_PreferHashJoinVersusIndexJoinWhenRiskIsHigh)
#endif	// SPQOS_DEBUG
	};

	SPQOS_RESULT eres = CUnittest::EresExecute(rgut, SPQOS_ARRAY_SIZE(rgut));

	// reset metadata cache
	CMDCache::Reset();

	return eres;
}


//---------------------------------------------------------------------------
//	@function:
//		CICGTest::EresUnittest_RunMinidumpTests
//
//	@doc:
//		Run all Minidump-based tests
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CICGTest::EresUnittest_RunMinidumpTests()
{
	return CTestUtils::EresUnittest_RunTests(rgszFileNames, &m_ulTestCounter,
											 SPQOS_ARRAY_SIZE(rgszFileNames));
}

SPQOS_RESULT
CICGTest::EresUnittest_RunTestsWithoutAdditionalTraceFlags()
{
	const CHAR *rgszFileNames[] = {
		"../data/dxl/minidump/Union-On-HJNs.mdp",
		"../data/dxl/minidump/Tpcds-NonPart-Q70a.mdp"};
	return CTestUtils::EresUnittest_RunTestsWithoutAdditionalTraceFlags(
		rgszFileNames, &m_ulTestCounterNoAdditionTraceFlag,
		SPQOS_ARRAY_SIZE(rgszFileNames), true, true);
}


//---------------------------------------------------------------------------
//	@function:
//		CICGTest::EresUnittest_RunUnsupportedMinidumpTests
//
//	@doc:
//		Run all unsupported Minidump-based tests
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CICGTest::EresUnittest_RunUnsupportedMinidumpTests()
{
	// enable (Redistribute, Broadcast) hash join plans
	CAutoTraceFlag atf1(EopttraceEnableRedistributeBroadcastHashJoin,
						true /*value*/);

	CAutoTraceFlag atf2(
		EopttraceDisableXformBase + CXform::ExfDynamicGet2DynamicTableScan,
		true);

	CAutoMemoryPool amp(CAutoMemoryPool::ElcNone);
	CMemoryPool *mp = amp.Pmp();

	SPQOS_RESULT eres = SPQOS_OK;
	const ULONG ulTests = SPQOS_ARRAY_SIZE(unSupportedTestCases);
	for (ULONG ul = m_ulUnsupportedTestCounter; ul < ulTests; ul++)
	{
		const CHAR *filename = unSupportedTestCases[ul].filename;
		CDXLMinidump *pdxlmd = CMinidumperUtils::PdxlmdLoad(mp, filename);
		bool unmatchedException = false;
		ULONG unmatchedExceptionMajor = 0;
		ULONG unmatchedExceptionMinor = 0;

		SPQOS_TRY
		{
			ICostModel *pcm = CTestUtils::GetCostModel(mp);

			COptimizerConfig *optimizer_config = pdxlmd->GetOptimizerConfig();
			CDXLNode *pdxlnPlan = CMinidumperUtils::PdxlnExecuteMinidump(
				mp, filename,
				optimizer_config->GetCostModel()->UlHosts() /*ulSegments*/,
				1 /*ulSessionId*/, 1,  /*ulCmdId*/
				optimizer_config, NULL /*pceeval*/
			);


			SPQOS_CHECK_ABORT;
			pdxlnPlan->Release();
			pcm->Release();

			// test should have thrown
			eres = SPQOS_FAILED;
			break;
		}
		SPQOS_CATCH_EX(ex)
		{
			unmatchedExceptionMajor = ex.Major();
			unmatchedExceptionMinor = ex.Minor();

			// verify expected exception
			if (unSupportedTestCases[ul].major == unmatchedExceptionMajor &&
				unSupportedTestCases[ul].minor == unmatchedExceptionMinor)
			{
				eres = SPQOS_OK;
			}
			else
			{
				unmatchedException = true;
				eres = SPQOS_FAILED;
			}
			SPQOS_RESET_EX;
		}
		SPQOS_CATCH_END;

		SPQOS_DELETE(pdxlmd);
		m_ulUnsupportedTestCounter++;

		if (SPQOS_FAILED == eres && unmatchedException)
		{
			CAutoTrace at(mp);
			at.Os() << "Test failed due to unmatched exceptions." << std::endl;
			at.Os() << " Expected result: " << unSupportedTestCases[ul].major
					<< "." << unSupportedTestCases[ul].minor << std::endl;
			at.Os() << " Actual result: " << unmatchedExceptionMajor << "."
					<< unmatchedExceptionMinor << std::endl;
		}
	}

	if (SPQOS_OK == eres)
	{
		m_ulUnsupportedTestCounter = 0;
	}

	return eres;
}


//---------------------------------------------------------------------------
//	@function:
//		CICGTest::EresUnittest_NegativeIndexApplyTests
//
//	@doc:
//		Negative IndexApply tests;
//		optimizer should not be able to generate a plan
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CICGTest::EresUnittest_NegativeIndexApplyTests()
{
	// enable (Redistribute, Broadcast) hash join plans
	CAutoTraceFlag atf(EopttraceEnableRedistributeBroadcastHashJoin,
					   true /*value*/);

	// disable physical scans and NLJ to force using index-apply
	CAutoTraceFlag atfDTS(
		EopttraceDisableXformBase + CXform::ExfDynamicGet2DynamicTableScan,
		true);
	CAutoTraceFlag atfTS(EopttraceDisableXformBase + CXform::ExfGet2TableScan,
						 true);
	CAutoTraceFlag atfNLJ(
		EopttraceDisableXformBase + CXform::ExfInnerJoin2NLJoin, true);

	CAutoMemoryPool amp(CAutoMemoryPool::ElcNone);
	CMemoryPool *mp = amp.Pmp();

	SPQOS_RESULT eres = SPQOS_OK;
	const ULONG ulTests = SPQOS_ARRAY_SIZE(rgszNegativeIndexApplyFileNames);
	for (ULONG ul = m_ulNegativeIndexApplyTestCounter; ul < ulTests; ul++)
	{
		SPQOS_TRY
		{
			ICostModel *pcm = CTestUtils::GetCostModel(mp);

			COptimizerConfig *optimizer_config = SPQOS_NEW(mp) COptimizerConfig(
				CEnumeratorConfig::GetEnumeratorCfg(mp, 0 /*plan_id*/),
				CStatisticsConfig::PstatsconfDefault(mp),
				CCTEConfig::PcteconfDefault(mp), pcm, CHint::PhintDefault(mp),
				CWindowOids::GetWindowOids(mp));
			CDXLNode *pdxlnPlan = CMinidumperUtils::PdxlnExecuteMinidump(
				mp, rgszNegativeIndexApplyFileNames[ul],
				SPQOPT_TEST_SEGMENTS /*ulSegments*/, 1 /*ulSessionId*/,
				1,					   /*ulCmdId*/
				optimizer_config, NULL /*pceeval*/
			);
			SPQOS_CHECK_ABORT;
			optimizer_config->Release();
			pdxlnPlan->Release();
			pcm->Release();

			// test should have thrown
			eres = SPQOS_FAILED;
			break;
		}
		SPQOS_CATCH_EX(ex)
		{
			if (SPQOS_MATCH_EX(ex, spqopt::ExmaSPQOPT, spqopt::ExmiNoPlanFound))
			{
				SPQOS_RESET_EX;
			}
			else
			{
				SPQOS_RETHROW(ex);
			}
		}
		SPQOS_CATCH_END;
		m_ulNegativeIndexApplyTestCounter++;
	}

	if (SPQOS_OK == eres)
	{
		m_ulNegativeIndexApplyTestCounter = 0;
	}

	return eres;
}

//---------------------------------------------------------------------------
//	@function:
//		CICGTest::FDXLSatisfiesPredicate
//
//	@doc:
//		Check if all the operators in the given dxl fragment satisfy the given predicate
//
//---------------------------------------------------------------------------
BOOL
CICGTest::FDXLOpSatisfiesPredicate(CDXLNode *pdxl, FnDXLOpPredicate fdop)
{
	using namespace spqdxl;

	CDXLOperator *dxl_op = pdxl->GetOperator();
	if (!fdop(dxl_op))
	{
		return false;
	}

	for (ULONG ul = 0; ul < pdxl->Arity(); ul++)
	{
		if (!FDXLOpSatisfiesPredicate((*pdxl)[ul], fdop))
		{
			return false;
		}
	}

	return true;
}


//---------------------------------------------------------------------------
//	@function:
//		CICGTest::FIsNotIndexJoin
//
//	@doc:
//		Check if the given dxl operator is an Index Join
//
//---------------------------------------------------------------------------
BOOL
CICGTest::FIsNotIndexJoin(CDXLOperator *dxl_op)
{
	if (EdxlopPhysicalNLJoin == dxl_op->GetDXLOperator())
	{
		if (CDXLPhysicalNLJoin::PdxlConvert(dxl_op)->IsIndexNLJ())
		{
			return false;
		}
	}

	return true;
}

//---------------------------------------------------------------------------
//	@function:
//		CICGTest::FHasNoIndexJoin
//
//	@doc:
//		Check if the given dxl fragment contains an Index Join
//
//---------------------------------------------------------------------------
BOOL
CICGTest::FHasNoIndexJoin(CDXLNode *pdxl)
{
	return FDXLOpSatisfiesPredicate(pdxl, FIsNotIndexJoin);
}

//---------------------------------------------------------------------------
//	@function:
//		CICGTest::EresUnittest_PenalizeIndexJoinVersusHashJoin
//
//	@doc:
//		Test that hash join is preferred versus index join when estimation
//		risk is high
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CICGTest::EresUnittest_PreferHashJoinVersusIndexJoinWhenRiskIsHigh()
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	// enable (Redistribute, Broadcast) hash join plans
	CAutoTraceFlag atf(EopttraceEnableRedistributeBroadcastHashJoin,
					   true /*value*/);

	// When the risk threshold is infinite, we should pick index join
	ICostModelParamsArray *pdrspqcpUnlimited =
		SPQOS_NEW(mp) ICostModelParamsArray(mp);
	ICostModelParams::SCostParam *pcpUnlimited =
		SPQOS_NEW(mp) ICostModelParams::SCostParam(
			CCostModelParamsSPQDB::EcpIndexJoinAllowedRiskThreshold,
			spqos::ulong_max,  // dVal
			0,				  // dLowerBound
			spqos::ulong_max	  // dUpperBound
		);
	pdrspqcpUnlimited->Append(pcpUnlimited);
	SPQOS_RESULT eres = CTestUtils::EresCheckOptimizedPlan(
		rgszPreferHashJoinVersusIndexJoin,
		SPQOS_ARRAY_SIZE(rgszPreferHashJoinVersusIndexJoin),
		&m_ulTestCounterPreferIndexJoinToHashJoin,
		1,	// ulSessionId
		1,	// ulCmdId
		FHasIndexJoin, pdrspqcpUnlimited);
	pdrspqcpUnlimited->Release();

	if (SPQOS_OK != eres)
	{
		return eres;
	}

	// When the risk threshold is zero, we should not pick index join
	ICostModelParamsArray *pdrspqcpNoIndexJoin =
		SPQOS_NEW(mp) ICostModelParamsArray(mp);
	ICostModelParams::SCostParam *pcpNoIndexJoin =
		SPQOS_NEW(mp) ICostModelParams::SCostParam(
			CCostModelParamsSPQDB::EcpIndexJoinAllowedRiskThreshold,
			0,	// dVal
			0,	// dLowerBound
			0	// dUpperBound
		);
	pdrspqcpNoIndexJoin->Append(pcpNoIndexJoin);
	eres = CTestUtils::EresCheckOptimizedPlan(
		rgszPreferHashJoinVersusIndexJoin,
		SPQOS_ARRAY_SIZE(rgszPreferHashJoinVersusIndexJoin),
		&m_ulTestCounterPreferHashJoinToIndexJoin,
		1,	// ulSessionId
		1,	// ulCmdId
		FHasNoIndexJoin, pdrspqcpNoIndexJoin);
	pdrspqcpNoIndexJoin->Release();

	return eres;
}

// EOF
