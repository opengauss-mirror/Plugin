//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2015 Pivotal, Inc.
//
//	@filename:
//		CMissingStatsTest.cpp
//
//	@doc:
//		Test for optimizing queries with aggregates
//---------------------------------------------------------------------------

#include "unittest/spqopt/minidump/CMissingStatsTest.h"

#include "spqos/base.h"
#include "spqos/error/CAutoTrace.h"
#include "spqos/memory/CAutoMemoryPool.h"
#include "spqos/task/CAutoTraceFlag.h"
#include "spqos/test/CUnittest.h"

#include "spqopt/engine/CCTEConfig.h"
#include "spqopt/engine/CEnumeratorConfig.h"
#include "spqopt/engine/CStatisticsConfig.h"
#include "spqopt/exception.h"
#include "spqopt/minidump/CMinidumperUtils.h"
#include "spqopt/optimizer/COptimizerConfig.h"

#include "unittest/spqopt/CTestUtils.h"

using namespace spqopt;

ULONG CMissingStatsTest::m_ulMissingStatsTestCounter =
	0;	// start from first test

//---------------------------------------------------------------------------
//	@function:
//		CMissingStatsTest::EresUnittest
//
//	@doc:
//		Unittest for expressions
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CMissingStatsTest::EresUnittest()
{
	CUnittest rgut[] = {
		SPQOS_UNITTEST_FUNC(EresUnittest_RunTests),
	};

	SPQOS_RESULT eres = CUnittest::EresExecute(rgut, SPQOS_ARRAY_SIZE(rgut));

	// reset metadata cache
	CMDCache::Reset();

	return eres;
}

//---------------------------------------------------------------------------
//	@function:
//		CMissingStatsTest::EresUnittest_RunTests
//
//	@doc:
//		Run all Minidump-based tests with plan matching
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CMissingStatsTest::EresUnittest_RunTests()
{
	SMissingStatsTestCase rgtc[] = {
		{"../data/dxl/minidump/MissingStats.mdp", 2},
		{"../data/dxl/minidump/NoMissingStatsAfterDroppedCol.mdp", 0},
		{"../data/dxl/minidump/NoMissingStats.mdp", 0},
		{"../data/dxl/minidump/NoMissingStatsForEmptyTable.mdp", 0},
		{"../data/dxl/minidump/NoMissingStatsAskingForSystemColFOJ.mdp", 0},
	};

	CAutoMemoryPool amp(CAutoMemoryPool::ElcNone);
	CMemoryPool *mp = amp.Pmp();

	SPQOS_RESULT eres = SPQOS_OK;
	const ULONG ulTests = SPQOS_ARRAY_SIZE(rgtc);
	for (ULONG ul = m_ulMissingStatsTestCounter;
		 ((ul < ulTests) && (SPQOS_OK == eres)); ul++)
	{
		ICostModel *pcm = CTestUtils::GetCostModel(mp);
		CAutoTraceFlag atf1(EopttracePrintColsWithMissingStats, true /*value*/);

		COptimizerConfig *optimizer_config = SPQOS_NEW(mp) COptimizerConfig(
			CEnumeratorConfig::GetEnumeratorCfg(mp, 0 /*plan_id*/),
			CStatisticsConfig::PstatsconfDefault(mp),
			CCTEConfig::PcteconfDefault(mp), pcm, CHint::PhintDefault(mp),
			CWindowOids::GetWindowOids(mp));
		SMissingStatsTestCase testCase = rgtc[ul];

		CDXLNode *pdxlnPlan = CMinidumperUtils::PdxlnExecuteMinidump(
			mp, testCase.m_szInputFile, SPQOPT_TEST_SEGMENTS /*ulSegments*/,
			1 /*ulSessionId*/, 1,  /*ulCmdId*/
			optimizer_config, NULL /*pceeval*/
		);

		CStatisticsConfig *stats_config = optimizer_config->GetStatsConf();

		IMdIdArray *pdrgmdidCol = SPQOS_NEW(mp) IMdIdArray(mp);
		stats_config->CollectMissingStatsColumns(pdrgmdidCol);
		ULONG ulMissingStats = pdrgmdidCol->Size();

		if (ulMissingStats != testCase.m_ulExpectedMissingStats)
		{
			// for debug traces
			CWStringDynamic str(mp);
			COstreamString oss(&str);

			// print objects
			oss << std::endl;
			oss << "Expected Number of Missing Columns: "
				<< testCase.m_ulExpectedMissingStats;

			oss << std::endl;
			oss << "Number of Missing Columns: " << ulMissingStats;
			oss << std::endl;

			SPQOS_TRACE(str.GetBuffer());
			eres = SPQOS_FAILED;
		}

		SPQOS_CHECK_ABORT;
		optimizer_config->Release();
		pdxlnPlan->Release();

		m_ulMissingStatsTestCounter++;
	}

	if (SPQOS_OK == eres)
	{
		m_ulMissingStatsTestCounter = 0;
	}

	return eres;
}

// EOF
