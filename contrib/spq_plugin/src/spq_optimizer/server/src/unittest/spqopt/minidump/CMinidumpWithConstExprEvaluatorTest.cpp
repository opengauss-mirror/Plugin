//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal, Inc.
//
//	@filename:
//		CMinidumpWithConstExprEvaluatorTest.cpp
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

#include "unittest/spqopt/minidump/CMinidumpWithConstExprEvaluatorTest.h"

#include "spqos/task/CAutoTraceFlag.h"

#include "spqopt/base/CAutoOptCtxt.h"
#include "spqopt/engine/CCTEConfig.h"
#include "spqopt/engine/CEnumeratorConfig.h"
#include "spqopt/engine/CStatisticsConfig.h"
#include "spqopt/exception.h"
#include "spqopt/mdcache/CMDCache.h"
#include "spqopt/minidump/CMinidumperUtils.h"
#include "spqopt/optimizer/COptimizerConfig.h"

#include "unittest/base.h"
#include "unittest/spqopt/CConstExprEvaluatorForDates.h"
#include "unittest/spqopt/CTestUtils.h"

using namespace spqopt;
using namespace spqos;

// start from first test that uses constant expression evaluator
ULONG CMinidumpWithConstExprEvaluatorTest::m_ulTestCounter = 0;

// minidump files we run with constant expression evaluator on
const CHAR *rgszConstExprEvaluatorOnFileNames[] = {
	"../data/dxl/minidump/DynamicIndexScan-Homogenous-EnabledDateConstraint.mdp",
	"../data/dxl/minidump/DynamicIndexScan-Heterogenous-EnabledDateConstraint.mdp",
	"../data/dxl/minidump/RemoveImpliedPredOnBCCPredicates.mdp"};


//---------------------------------------------------------------------------
//	@function:
//		CMinidumpWithConstExprEvaluatorTest::EresUnittest
//
//	@doc:
//		Runs all unittests
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CMinidumpWithConstExprEvaluatorTest::EresUnittest()
{
	CUnittest rgut[] = {
		SPQOS_UNITTEST_FUNC(
			CMinidumpWithConstExprEvaluatorTest::
				EresUnittest_RunMinidumpTestsWithConstExprEvaluatorOn),
	};

	SPQOS_RESULT eres = CUnittest::EresExecute(rgut, SPQOS_ARRAY_SIZE(rgut));

	return eres;
}


//---------------------------------------------------------------------------
//	@function:
//		CMinidumpWithConstExprEvaluatorTest::EresUnittest_RunMinidumpTestsWithConstExprEvaluatorOn
//
//	@doc:
//		Run tests with constant expression evaluation enabled
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CMinidumpWithConstExprEvaluatorTest::
	EresUnittest_RunMinidumpTestsWithConstExprEvaluatorOn()
{
	CAutoTraceFlag atf(EopttraceEnableConstantExpressionEvaluation,
					   true /*value*/);

	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	IConstExprEvaluator *pceeval = SPQOS_NEW(mp) CConstExprEvaluatorForDates(mp);

	BOOL fMatchPlans = true;

	// enable plan enumeration only if we match plans
	CAutoTraceFlag atf1(EopttraceEnumeratePlans, fMatchPlans);

	// enable stats derivation for DPE
	CAutoTraceFlag atf2(EopttraceDeriveStatsForDPE, true /*value*/);

	const ULONG ulTests = SPQOS_ARRAY_SIZE(rgszConstExprEvaluatorOnFileNames);

	SPQOS_RESULT eres = CTestUtils::EresRunMinidumps(
		mp, rgszConstExprEvaluatorOnFileNames, ulTests, &m_ulTestCounter,
		1,	// ulSessionId
		1,	// ulCmdId
		fMatchPlans,
		false,	// fTestSpacePruning
		NULL,	// szMDFilePath
		pceeval);
	pceeval->Release();

	return eres;
}

// EOF
