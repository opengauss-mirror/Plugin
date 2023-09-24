//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CMiniDumperDXLTest.cpp
//
//	@doc:
//		Test for DXL-based minidumps
//---------------------------------------------------------------------------
#include "unittest/spqopt/minidump/CMiniDumperDXLTest.h"

#include <fstream>

#include "spqos/io/COstreamFile.h"
#include "spqos/io/COstreamString.h"
#include "spqos/task/CAutoTraceFlag.h"

#include "spqopt/base/CQueryContext.h"
#include "spqopt/engine/CEngine.h"
#include "spqopt/engine/CEnumeratorConfig.h"
#include "spqopt/engine/CStatisticsConfig.h"
#include "spqopt/eval/CConstExprEvaluatorDefault.h"
#include "spqopt/minidump/CDXLMinidump.h"
#include "spqopt/minidump/CMiniDumperDXL.h"
#include "spqopt/minidump/CMinidumperUtils.h"
#include "spqopt/minidump/CSerializableMDAccessor.h"
#include "spqopt/minidump/CSerializableOptimizerConfig.h"
#include "spqopt/minidump/CSerializablePlan.h"
#include "spqopt/minidump/CSerializableQuery.h"
#include "spqopt/minidump/CSerializableStackTrace.h"
#include "spqopt/optimizer/COptimizerConfig.h"
#include "spqopt/translate/CTranslatorDXLToExpr.h"
#include "spqopt/translate/CTranslatorExprToDXL.h"
#include "naucrates/base/CQueryToDXLResult.h"
#include "naucrates/dxl/CDXLUtils.h"

#include "unittest/base.h"
#include "unittest/spqopt/CTestUtils.h"
#include "unittest/spqopt/translate/CTranslatorExprToDXLTest.h"

static const CHAR *szQueryFile = "../data/dxl/minidump/Query.xml";

//---------------------------------------------------------------------------
//	@function:
//		CMiniDumperDXLTest::EresUnittest
//
//	@doc:
//		Unittest for DXL minidumps
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CMiniDumperDXLTest::EresUnittest()
{
	CUnittest rgut[] = {
		SPQOS_UNITTEST_FUNC(CMiniDumperDXLTest::EresUnittest_Basic),
		SPQOS_UNITTEST_FUNC(CMiniDumperDXLTest::EresUnittest_Load),
	};

	return CUnittest::EresExecute(rgut, SPQOS_ARRAY_SIZE(rgut));
}



//---------------------------------------------------------------------------
//	@function:
//		CMiniDumperDXLTest::EresUnittest_Basic
//
//	@doc:
//		Test minidumps in case of an exception
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CMiniDumperDXLTest::EresUnittest_Basic()
{
	CAutoMemoryPool amp(CAutoMemoryPool::ElcNone);
	CMemoryPool *mp = amp.Pmp();

	CWStringDynamic minidumpstr(mp);
	COstreamString oss(&minidumpstr);
	CMiniDumperDXL mdrs(mp);
	mdrs.Init(&oss);

	CHAR file_name[SPQOS_FILE_NAME_BUF_SIZE];

	SPQOS_TRY
	{
		CSerializableStackTrace serStackTrace;

		// read the dxl document
		CHAR *szQueryDXL = CDXLUtils::Read(mp, szQueryFile);

		// parse the DXL query tree from the given DXL document
		CQueryToDXLResult *ptroutput =
			CDXLUtils::ParseQueryToQueryDXLTree(mp, szQueryDXL, NULL);
		SPQOS_CHECK_ABORT;

		CSerializableQuery serQuery(mp, ptroutput->CreateDXLNode(),
									ptroutput->GetOutputColumnsDXLArray(),
									ptroutput->GetCTEProducerDXLArray());

		// setup a file-based provider
		CMDProviderMemory *pmdp = CTestUtils::m_pmdpf;
		pmdp->AddRef();

		// we need to use an auto pointer for the cache here to ensure
		// deleting memory of cached objects when we throw
		CAutoP<CMDAccessor::MDCache> apcache;
		apcache = CCacheFactory::CreateCache<spqopt::IMDCacheObject *,
											 spqopt::CMDKey *>(
			true,  // fUnique
			0 /* unlimited cache quota */, CMDKey::UlHashMDKey,
			CMDKey::FEqualMDKey);

		CMDAccessor::MDCache *pcache = apcache.Value();

		CMDAccessor mda(mp, pcache, CTestUtils::m_sysidDefault, pmdp);

		CSerializableMDAccessor serMDA(&mda);

		CAutoTraceFlag atfPrintQuery(EopttracePrintQuery, true);
		CAutoTraceFlag atfPrintPlan(EopttracePrintPlan, true);
		CAutoTraceFlag atfTest(EtraceTest, true);

		COptimizerConfig *optimizer_config = SPQOS_NEW(mp) COptimizerConfig(
			CEnumeratorConfig::GetEnumeratorCfg(mp, 0 /*plan_id*/),
			CStatisticsConfig::PstatsconfDefault(mp),
			CCTEConfig::PcteconfDefault(mp), ICostModel::PcmDefault(mp),
			CHint::PhintDefault(mp), CWindowOids::GetWindowOids(mp));

		// setup opt ctx
		CAutoOptCtxt aoc(mp, &mda, NULL, /* pceeval */
						 CTestUtils::GetCostModel(mp));

		// translate DXL Tree -> Expr Tree
		CTranslatorDXLToExpr *pdxltr =
			SPQOS_NEW(mp) CTranslatorDXLToExpr(mp, &mda);
		CExpression *pexprTranslated = pdxltr->PexprTranslateQuery(
			ptroutput->CreateDXLNode(), ptroutput->GetOutputColumnsDXLArray(),
			ptroutput->GetCTEProducerDXLArray());

		spqdxl::ULongPtrArray *pdrgul = pdxltr->PdrspqulOutputColRefs();
		spqmd::CMDNameArray *pdrspqmdname = pdxltr->Pdrspqmdname();

		ULONG ulSegments = SPQOPT_TEST_SEGMENTS;
		CQueryContext *pqc = CQueryContext::PqcGenerate(
			mp, pexprTranslated, pdrgul, pdrspqmdname, true /*fDeriveStats*/);

		// optimize logical expression tree into physical expression tree.

		CEngine eng(mp);

		CSerializableOptimizerConfig serOptConfig(mp, optimizer_config);

		eng.Init(pqc, NULL /*search_stage_array*/);
		eng.Optimize();

		CExpression *pexprPlan = eng.PexprExtractPlan();
		(void) pexprPlan->PrppCompute(mp, pqc->Prpp());

		// translate plan into DXL
		IntPtrArray *pdrspqiSegments = SPQOS_NEW(mp) IntPtrArray(mp);


		SPQOS_ASSERT(0 < ulSegments);

		for (ULONG ul = 0; ul < ulSegments; ul++)
		{
			pdrspqiSegments->Append(SPQOS_NEW(mp) INT(ul));
		}

		CTranslatorExprToDXL ptrexprtodxl(mp, &mda, pdrspqiSegments);
		CDXLNode *pdxlnPlan = ptrexprtodxl.PdxlnTranslate(
			pexprPlan, pqc->PdrgPcr(), pqc->Pdrspqmdname());
		SPQOS_ASSERT(NULL != pdxlnPlan);

		CSerializablePlan serPlan(
			mp, pdxlnPlan, optimizer_config->GetEnumeratorCfg()->GetPlanId(),
			optimizer_config->GetEnumeratorCfg()->GetPlanSpaceSize());
		SPQOS_CHECK_ABORT;

		// simulate an exception
		SPQOS_OOM_CHECK(NULL);
	}
	SPQOS_CATCH_EX(ex)
	{
		// unless we're simulating faults, the exception must be OOM
		SPQOS_ASSERT_IMP(!SPQOS_FTRACE(EtraceSimulateAbort) &&
							!SPQOS_FTRACE(EtraceSimulateIOError),
						CException::ExmaSystem == ex.Major() &&
							CException::ExmiOOM == ex.Minor());

		mdrs.Finalize();

		SPQOS_RESET_EX;

		CWStringDynamic str(mp);
		COstreamString oss(&str);
		oss << std::endl << "Minidump" << std::endl;
		oss << minidumpstr.GetBuffer();
		oss << std::endl;

		// dump the same to a temp file
		ULONG ulSessionId = 1;
		ULONG ulCommandId = 1;

		CMinidumperUtils::GenerateMinidumpFileName(
			file_name, SPQOS_FILE_NAME_BUF_SIZE, ulSessionId, ulCommandId,
			NULL /*szMinidumpFileName*/);

		std::wofstream osMinidump(file_name);
		osMinidump << minidumpstr.GetBuffer();

		oss << "Minidump file: " << file_name << std::endl;

		SPQOS_TRACE(str.GetBuffer());
	}
	SPQOS_CATCH_END;

	// TODO:  - Feb 11, 2013; enable after fixing problems with serializing
	// XML special characters (OPT-2996)
	//	// try to load minidump file
	//	CDXLMinidump *pdxlmd = CMinidumperUtils::PdxlmdLoad(mp, file_name);
	//	SPQOS_ASSERT(NULL != pdxlmd);
	//	delete pdxlmd;


	// delete temp file
	ioutils::Unlink(file_name);

	return SPQOS_OK;
}

//---------------------------------------------------------------------------
//	@function:
//		CMiniDumperDXLTest::EresUnittest_Load
//
//	@doc:
//		Load a minidump file
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CMiniDumperDXLTest::EresUnittest_Load()
{
	CAutoMemoryPool amp(CAutoMemoryPool::ElcExc);
	CMemoryPool *mp = amp.Pmp();

	const CHAR *rgszMinidumps[] = {
		"../data/dxl/minidump/Minidump.xml",
	};
	ULONG ulTestCounter = 0;

	SPQOS_RESULT eres = CTestUtils::EresRunMinidumps(mp, rgszMinidumps,
													1,	// ulTests
													&ulTestCounter,
													1,		// ulSessionId
													1,		// ulCmdId
													false,	// fMatchPlans
													false	// fTestSpacePruning
	);
	return eres;
}
// EOF
