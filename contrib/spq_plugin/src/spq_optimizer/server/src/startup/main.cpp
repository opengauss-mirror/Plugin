//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2008 Greenplum, Inc.
//
//	@filename:
//		main.cpp
//
//	@doc:
//		Startup routines for optimizer
//---------------------------------------------------------------------------

#include "spqos/_api.h"
#include "spqos/common/CMainArgs.h"
#include "spqos/memory/CAutoMemoryPool.h"
#include "spqos/test/CFSimulatorTestExt.h"
#include "spqos/test/CUnittest.h"
#include "spqos/types.h"

#include "spqopt/engine/CEnumeratorConfig.h"
#include "spqopt/engine/CStatisticsConfig.h"
#include "spqopt/init.h"
#include "spqopt/mdcache/CMDCache.h"
#include "spqopt/minidump/CMinidumperUtils.h"
#include "spqopt/optimizer/COptimizerConfig.h"
#include "spqopt/xforms/CXformFactory.h"
#include "naucrates/init.h"

// test headers

#include "unittest/base.h"
#include "unittest/dxl/CDXLMemoryManagerTest.h"
#include "unittest/dxl/CDXLUtilsTest.h"
#include "unittest/dxl/CParseHandlerCostModelTest.h"
#include "unittest/dxl/CParseHandlerManagerTest.h"
#include "unittest/dxl/CParseHandlerOptimizerConfigSerializeTest.h"
#include "unittest/dxl/CParseHandlerTest.h"
#include "unittest/dxl/CXMLSerializerTest.h"
#include "unittest/dxl/base/CDatumTest.h"
#include "unittest/dxl/statistics/CBucketTest.h"
#include "unittest/dxl/statistics/CFilterCardinalityTest.h"
#include "unittest/dxl/statistics/CHistogramTest.h"
#include "unittest/dxl/statistics/CJoinCardinalityTest.h"
#include "unittest/dxl/statistics/CMCVTest.h"
#include "unittest/dxl/statistics/CPointTest.h"
#include "unittest/dxl/statistics/CStatisticsTest.h"
#include "unittest/spqopt/CTestUtils.h"
#include "unittest/spqopt/base/CColRefSetIterTest.h"
#include "unittest/spqopt/base/CColRefSetTest.h"
#include "unittest/spqopt/base/CColumnFactoryTest.h"
#include "unittest/spqopt/base/CConstraintTest.h"
#include "unittest/spqopt/base/CDistributionSpecTest.h"
#include "unittest/spqopt/base/CEquivalenceClassesTest.h"
#include "unittest/spqopt/base/CFunctionalDependencyTest.h"
#include "unittest/spqopt/base/CGroupTest.h"
#include "unittest/spqopt/base/CKeyCollectionTest.h"
#include "unittest/spqopt/base/CMaxCardTest.h"
#include "unittest/spqopt/base/COrderSpecTest.h"
#include "unittest/spqopt/base/CRangeTest.h"
#include "unittest/spqopt/base/CStateMachineTest.h"
#include "unittest/spqopt/cost/CCostTest.h"
#include "unittest/spqopt/csq/CCorrelatedExecutionTest.h"
#include "unittest/spqopt/engine/CBindingTest.h"
#include "unittest/spqopt/engine/CEngineTest.h"
#include "unittest/spqopt/engine/CEnumeratorTest.h"
#include "unittest/spqopt/eval/CConstExprEvaluatorDXLTest.h"
#include "unittest/spqopt/eval/CConstExprEvaluatorDefaultTest.h"
#include "unittest/spqopt/mdcache/CMDAccessorTest.h"
#include "unittest/spqopt/mdcache/CMDProviderTest.h"
#include "unittest/spqopt/metadata/CColumnDescriptorTest.h"
#include "unittest/spqopt/metadata/CIndexDescriptorTest.h"
#include "unittest/spqopt/metadata/CNameTest.h"
#include "unittest/spqopt/metadata/CPartConstraintTest.h"
#include "unittest/spqopt/metadata/CTableDescriptorTest.h"
#include "unittest/spqopt/minidump/CAggTest.h"
#include "unittest/spqopt/minidump/CArrayExpansionTest.h"
#include "unittest/spqopt/minidump/CBitmapTest.h"
#include "unittest/spqopt/minidump/CCTETest.h"
#include "unittest/spqopt/minidump/CCastTest.h"
#include "unittest/spqopt/minidump/CCollapseProjectTest.h"
#include "unittest/spqopt/minidump/CConstTblGetTest.h"
#include "unittest/spqopt/minidump/CDMLTest.h"
#include "unittest/spqopt/minidump/CDirectDispatchTest.h"
#include "unittest/spqopt/minidump/CEscapeMechanismTest.h"
#include "unittest/spqopt/minidump/CExternalTableTest.h"
#include "unittest/spqopt/minidump/CICGTest.h"
#include "unittest/spqopt/minidump/CJoinOrderDPTest.h"
#include "unittest/spqopt/minidump/CMiniDumperDXLTest.h"
#include "unittest/spqopt/minidump/CMinidumpWithConstExprEvaluatorTest.h"
#include "unittest/spqopt/minidump/CMissingStatsTest.h"
#include "unittest/spqopt/minidump/CMultilevelPartitionTest.h"
#include "unittest/spqopt/minidump/CPhysicalParallelUnionAllTest.h"
#include "unittest/spqopt/minidump/CPruneColumnsTest.h"
#include "unittest/spqopt/minidump/CPullUpProjectElementTest.h"
#include "unittest/spqopt/minidump/CSubqueryTest.h"
#include "unittest/spqopt/minidump/CTVFTest.h"
#include "unittest/spqopt/minidump/CWindowTest.h"
#include "unittest/spqopt/minidump/MinidumpTestHeaders.h"  // auto generated header file
#include "unittest/spqopt/operators/CContradictionTest.h"
#include "unittest/spqopt/operators/CExpressionPreprocessorTest.h"
#include "unittest/spqopt/operators/CExpressionTest.h"
#include "unittest/spqopt/operators/CPredicateUtilsTest.h"
#include "unittest/spqopt/operators/CScalarIsDistinctFromTest.h"
#include "unittest/spqopt/search/COptimizationJobsTest.h"
#include "unittest/spqopt/search/CSearchStrategyTest.h"
#include "unittest/spqopt/search/CTreeMapTest.h"
#include "unittest/spqopt/translate/CTranslatorDXLToExprTest.h"
#include "unittest/spqopt/translate/CTranslatorExprToDXLTest.h"
#include "unittest/spqopt/xforms/CDecorrelatorTest.h"
#include "unittest/spqopt/xforms/CJoinOrderTest.h"
#include "unittest/spqopt/xforms/CSubqueryHandlerTest.h"
#include "unittest/spqopt/xforms/CXformFactoryTest.h"
#include "unittest/spqopt/xforms/CXformRightOuterJoin2HashJoinTest.h"
#include "unittest/spqopt/xforms/CXformTest.h"

using namespace spqos;
using namespace spqopt;
using namespace spqdxl;
using namespace spqnaucrates;
using namespace spqdbcost;

// static array of all known unittest routines
static spqos::CUnittest rgut[] = {
#include "unittest/spqopt/minidump/MinidumpTestArray.inl"  // auto generated inlining file

	// naucrates
	SPQOS_UNITTEST_STD(CCostTest),
	SPQOS_UNITTEST_STD(CDatumTest),
	SPQOS_UNITTEST_STD(CDXLMemoryManagerTest),
	SPQOS_UNITTEST_STD(CDXLUtilsTest),
	SPQOS_UNITTEST_STD(CMDAccessorTest),
	SPQOS_UNITTEST_STD(CMDProviderTest),
	SPQOS_UNITTEST_STD(CMiniDumperDXLTest),
	SPQOS_UNITTEST_STD(CExpressionPreprocessorTest),
	SPQOS_UNITTEST_STD(CWindowTest),
	SPQOS_UNITTEST_STD(CICGTest),
	SPQOS_UNITTEST_STD(CMultilevelPartitionTest),
	SPQOS_UNITTEST_STD(CDMLTest),
	SPQOS_UNITTEST_STD(CDirectDispatchTest),
	SPQOS_UNITTEST_STD(CTVFTest),
	SPQOS_UNITTEST_STD(CAggTest),
	SPQOS_UNITTEST_STD(CSubqueryTest),
	SPQOS_UNITTEST_STD(CCollapseProjectTest),
	SPQOS_UNITTEST_STD(CPruneColumnsTest),
	SPQOS_UNITTEST_STD(CPhysicalParallelUnionAllTest),
	SPQOS_UNITTEST_STD(CMissingStatsTest),
	SPQOS_UNITTEST_STD(CBitmapTest),
	SPQOS_UNITTEST_STD(CCTETest),
	SPQOS_UNITTEST_STD(CExternalTableTest),
	SPQOS_UNITTEST_STD(CEscapeMechanismTest),

	SPQOS_UNITTEST_STD(CMinidumpWithConstExprEvaluatorTest),
	SPQOS_UNITTEST_STD(CParseHandlerManagerTest),
	SPQOS_UNITTEST_STD(CParseHandlerTest),
	SPQOS_UNITTEST_STD(CParseHandlerCostModelTest),
	SPQOS_UNITTEST_STD(CParseHandlerOptimizerConfigSerializeTest),
	SPQOS_UNITTEST_STD(CStatisticsTest),
	SPQOS_UNITTEST_STD(CFilterCardinalityTest),
	SPQOS_UNITTEST_STD(CPointTest),
	SPQOS_UNITTEST_STD(CBucketTest),
	SPQOS_UNITTEST_STD(CHistogramTest),
	SPQOS_UNITTEST_STD(CMCVTest),
	SPQOS_UNITTEST_STD(CJoinCardinalityTest),
	SPQOS_UNITTEST_STD(CTranslatorDXLToExprTest),
	SPQOS_UNITTEST_STD(CTranslatorExprToDXLTest),
	SPQOS_UNITTEST_STD(CXMLSerializerTest),

	// opt
	SPQOS_UNITTEST_STD(CArrayExpansionTest),
	SPQOS_UNITTEST_STD(CJoinOrderDPTest),
	SPQOS_UNITTEST_STD(CPullUpProjectElementTest),
	SPQOS_UNITTEST_STD(CColumnDescriptorTest),
	SPQOS_UNITTEST_STD(CColumnFactoryTest),
	SPQOS_UNITTEST_STD(CColRefSetIterTest),
	SPQOS_UNITTEST_STD(CColRefSetTest),
	SPQOS_UNITTEST_STD(CConstraintTest),
	SPQOS_UNITTEST_STD(CContradictionTest),
	SPQOS_UNITTEST_STD(CCorrelatedExecutionTest),
	SPQOS_UNITTEST_STD(CDecorrelatorTest),
	SPQOS_UNITTEST_STD(CDistributionSpecTest),
	SPQOS_UNITTEST_STD(CCastTest),
	SPQOS_UNITTEST_STD(CConstTblGetTest),

	SPQOS_UNITTEST_STD(CSubqueryHandlerTest),
	SPQOS_UNITTEST_STD(CXformRightOuterJoin2HashJoinTest),
	SPQOS_UNITTEST_STD(CBindingTest),
	SPQOS_UNITTEST_STD(CEngineTest),
	SPQOS_UNITTEST_STD(CEquivalenceClassesTest),
	SPQOS_UNITTEST_STD(CExpressionTest),
	SPQOS_UNITTEST_STD(CJoinOrderTest),
	SPQOS_UNITTEST_STD(CKeyCollectionTest),
	SPQOS_UNITTEST_STD(CMaxCardTest),
	SPQOS_UNITTEST_STD(CFunctionalDependencyTest),
	SPQOS_UNITTEST_STD(CNameTest),
	SPQOS_UNITTEST_STD(COrderSpecTest),
	SPQOS_UNITTEST_STD(CRangeTest),
	SPQOS_UNITTEST_STD(CGroupTest),
	SPQOS_UNITTEST_STD(CPredicateUtilsTest),
	SPQOS_UNITTEST_STD(CScalarIsDistinctFromTest),
	SPQOS_UNITTEST_STD(CPartConstraintTest),
	SPQOS_UNITTEST_STD(CSearchStrategyTest),
	SPQOS_UNITTEST_STD(COptimizationJobsTest),
	SPQOS_UNITTEST_STD(CStateMachineTest),
	SPQOS_UNITTEST_STD(CTableDescriptorTest),
	SPQOS_UNITTEST_STD(CIndexDescriptorTest),
	SPQOS_UNITTEST_STD(CTreeMapTest),
	SPQOS_UNITTEST_STD(CXformFactoryTest),
	SPQOS_UNITTEST_STD(CXformTest),
	SPQOS_UNITTEST_STD(CConstExprEvaluatorDefaultTest),
	SPQOS_UNITTEST_STD(CConstExprEvaluatorDXLTest),
// disable CEnumeratorTest until it is fixed
//	SPQOS_UNITTEST_STD(CEnumeratorTest),
// extended tests
#ifdef SPQOS_FPSIMULATOR
	SPQOS_UNITTEST_EXT(CFSimulatorTestExt),
#endif	// SPQOS_FPSIMULATOR
};

//---------------------------------------------------------------------------
//	@function:
//		ConfigureTests
//
//	@doc:
//		Configurations needed before running unittests
//
//---------------------------------------------------------------------------
void
ConfigureTests()
{
	// initialize DXL support
	InitDXL();

	CMDCache::Init();

	// load metadata objects into provider file
	{
		CAutoMemoryPool amp;
		CMemoryPool *mp = amp.Pmp();
		CTestUtils::InitProviderFile(mp);

		// detach safety
		(void) amp.Detach();
	}

#ifdef SPQOS_DEBUG
	// reset xforms factory to exercise xforms ctors and dtors
	CXformFactory::Pxff()->Shutdown();
	SPQOS_RESULT eres = CXformFactory::Init();

	SPQOS_ASSERT(SPQOS_OK == eres);
#endif	// SPQOS_DEBUG
}


//---------------------------------------------------------------------------
//	@function:
//		Cleanup
//
//	@doc:
//		Cleanup after unittests are done
//
//---------------------------------------------------------------------------
void
Cleanup()
{
	CMDCache::Shutdown();
	CTestUtils::DestroyMDProvider();
}

// static variable counting the number of failed tests; PvExec overwrites with
// the actual count of failed tests
static ULONG tests_failed = 0;

//---------------------------------------------------------------------------
//	@function:
//		PvExec
//
//	@doc:
//		Function driving execution.
//
//---------------------------------------------------------------------------
static void *
PvExec(void *pv)
{
	CMainArgs *pma = (CMainArgs *) pv;
	CBitVector bv(ITask::Self()->Pmp(), CUnittest::UlTests());

	CHAR ch = '\0';

	CHAR *file_name = NULL;
	BOOL fMinidump = false;
	BOOL fUnittest = false;
	ULLONG ullPlanId = 0;

	while (pma->Getopt(&ch))
	{
		CHAR *szTestName = NULL;

		switch (ch)
		{
			case 'U':
				szTestName = optarg;
				// fallthru
			case 'u':
				CUnittest::FindTest(bv, CUnittest::EttStandard, szTestName);
				fUnittest = true;
				break;

			case 'x':
				CUnittest::FindTest(bv, CUnittest::EttExtended,
									NULL /*szTestName*/);
				fUnittest = true;
				break;

			case 'T':
				CUnittest::SetTraceFlag(optarg);
				break;

			case 'i':
				ullPlanId = CUnittest::UllParsePlanId(optarg);
				SPQOS_SET_TRACE(EopttraceEnumeratePlans);
				break;

			case 'd':
				fMinidump = true;
				file_name = optarg;
				break;

			default:
				// ignore other parameters
				break;
		}
	}

	if (fMinidump && fUnittest)
	{
		SPQOS_TRACE(SPQOS_WSZ_LIT(
			"Cannot specify -d and -U/-u options at the same time"));
		return NULL;
	}

	if (fMinidump)
	{
		// initialize DXL support
		InitDXL();

		CMDCache::Init();

		CAutoMemoryPool amp;
		CMemoryPool *mp = amp.Pmp();

		// load dump file
		CDXLMinidump *pdxlmd = CMinidumperUtils::PdxlmdLoad(mp, file_name);
		SPQOS_CHECK_ABORT;

		COptimizerConfig *optimizer_config = pdxlmd->GetOptimizerConfig();

		if (NULL == optimizer_config)
		{
			optimizer_config = COptimizerConfig::PoconfDefault(mp);
		}
		else
		{
			optimizer_config->AddRef();
		}

		if (ullPlanId != 0)
		{
			optimizer_config->GetEnumeratorCfg()->SetPlanId(ullPlanId);
		}

		ULONG ulSegments = CTestUtils::UlSegments(optimizer_config);

		CDXLNode *pdxlnPlan = CMinidumperUtils::PdxlnExecuteMinidump(
			mp, file_name, ulSegments, 1 /*ulSessionId*/, 1 /*ulCmdId*/,
			optimizer_config, NULL /*pceeval*/
		);

		SPQOS_DELETE(pdxlmd);
		optimizer_config->Release();
		pdxlnPlan->Release();
		CMDCache::Shutdown();
	}
	else
	{
		SPQOS_ASSERT(fUnittest);
		tests_failed = CUnittest::Driver(&bv);
	}

	return NULL;
}


//---------------------------------------------------------------------------
//	@function:
//		main
//
//	@doc:
//		Entry point for stand-alone optimizer binary; ignore arguments for the
//		time being
//
//---------------------------------------------------------------------------
INT
main(INT iArgs, const CHAR **rgszArgs)
{
	// Use default allocator
	struct spqos_init_params spqos_params = {NULL};

	spqos_init(&spqos_params);
	spqdxl_init();
	spqopt_init();

	SPQOS_ASSERT(iArgs >= 0);

	// setup args for unittest params
	CMainArgs ma(iArgs, rgszArgs, "uU:d:xT:i:");

	// initialize unittest framework
	CUnittest::Init(rgut, SPQOS_ARRAY_SIZE(rgut), ConfigureTests, Cleanup);

	spqos_exec_params params;
	params.func = PvExec;
	params.arg = &ma;
	params.stack_start = &params;
	params.error_buffer = NULL;
	params.error_buffer_size = -1;
	params.abort_requested = NULL;

	if (spqos_exec(&params) || (tests_failed != 0))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


// EOF
