//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CSearchStrategyTest.cpp
//
//	@doc:
//		Test for search strategy
//---------------------------------------------------------------------------
#include "unittest/spqopt/search/CSearchStrategyTest.h"

#include "spqos/error/CAutoTrace.h"
#include "spqos/task/CAutoTraceFlag.h"

#include "spqopt/engine/CEngine.h"
#include "spqopt/eval/CConstExprEvaluatorDefault.h"
#include "spqopt/exception.h"
#include "spqopt/search/CSearchStage.h"
#include "spqopt/xforms/CXformFactory.h"
#include "naucrates/dxl/CDXLUtils.h"
#include "naucrates/dxl/parser/CParseHandlerDXL.h"

#include "unittest/base.h"
#include "unittest/spqopt/CTestUtils.h"
#include "unittest/spqopt/engine/CEngineTest.h"

//---------------------------------------------------------------------------
//	@function:
//		CSearchStrategyTest::EresUnittest
//
//	@doc:
//		Unittest for scheduler
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CSearchStrategyTest::EresUnittest()
{
	CUnittest rgut[] = {
#ifdef SPQOS_DEBUG
		SPQOS_UNITTEST_FUNC(CSearchStrategyTest::EresUnittest_RecursiveOptimize),
#endif	// SPQOS_DEBUG
		SPQOS_UNITTEST_FUNC(
			CSearchStrategyTest::EresUnittest_MultiThreadedOptimize),
		SPQOS_UNITTEST_FUNC(CSearchStrategyTest::EresUnittest_Parsing),
		SPQOS_UNITTEST_FUNC_THROW(CSearchStrategyTest::EresUnittest_Timeout,
								 spqopt::ExmaSPQOPT, spqopt::ExmiNoPlanFound),
		SPQOS_UNITTEST_FUNC_THROW(
			CSearchStrategyTest::EresUnittest_ParsingWithException,
			spqdxl::ExmaDXL, spqdxl::ExmiDXLXercesParseError),
	};

	return CUnittest::EresExecute(rgut, SPQOS_ARRAY_SIZE(rgut));
}


//---------------------------------------------------------------------------
//	@function:
//		CSearchStrategyTest::Optimize
//
//	@doc:
//		Run optimize function on given expression
//
//---------------------------------------------------------------------------
void
CSearchStrategyTest::Optimize(CMemoryPool *mp, Pfpexpr pfnGenerator,
							  CSearchStageArray *search_stage_array,
							  PfnOptimize pfnOptimize)
{
	// setup a file-based provider
	CMDProviderMemory *pmdp = CTestUtils::m_pmdpf;
	pmdp->AddRef();
	CMDAccessor mda(mp, CMDCache::Pcache());
	mda.RegisterProvider(CTestUtils::m_sysidDefault, pmdp);

	// install opt context in TLS
	{
		CAutoOptCtxt aoc(mp, &mda, NULL, /* pceeval */
						 CTestUtils::GetCostModel(mp));
		CExpression *pexpr = pfnGenerator(mp);
		pfnOptimize(mp, pexpr, search_stage_array);
		pexpr->Release();
	}
}


#ifdef SPQOS_DEBUG
//---------------------------------------------------------------------------
//	@function:
//		CSearchStrategyTest::EresUnittest_RecursiveOptimize
//
//	@doc:
//		Test search strategy with recursive optimization
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CSearchStrategyTest::EresUnittest_RecursiveOptimize()
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();
	Optimize(mp, CTestUtils::PexprLogicalSelectOnOuterJoin, PdrspqssRandom(mp),
			 CEngineTest::BuildMemoRecursive);

	return SPQOS_OK;
}
#endif	// SPQOS_DEBUG


//---------------------------------------------------------------------------
//	@function:
//		CSearchStrategyTest::EresUnittest_MultiThreadedOptimize
//
//	@doc:
//		Test search strategy with multi-threaded optimization
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CSearchStrategyTest::EresUnittest_MultiThreadedOptimize()
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();
	Optimize(mp, CTestUtils::PexprLogicalSelectOnOuterJoin, PdrspqssRandom(mp),
			 BuildMemo);

	return SPQOS_OK;
}


//---------------------------------------------------------------------------
//	@function:
//		CSearchStrategyTest::EresUnittest_Parsing
//
//	@doc:
//		Test search strategy with multi-threaded optimization
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CSearchStrategyTest::EresUnittest_Parsing()
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();
	CParseHandlerDXL *pphDXL = CDXLUtils::GetParseHandlerForDXLFile(
		mp, "../data/dxl/search/strategy0.xml", NULL);
	CSearchStageArray *search_stage_array = pphDXL->GetSearchStageArray();
	const ULONG size = search_stage_array->Size();
	for (ULONG ul = 0; ul < size; ul++)
	{
		CAutoTrace at(mp);
		(*search_stage_array)[ul]->OsPrint(at.Os());
	}
	search_stage_array->AddRef();
	Optimize(mp, CTestUtils::PexprLogicalSelectOnOuterJoin, search_stage_array,
			 BuildMemo);

	SPQOS_DELETE(pphDXL);

	return SPQOS_OK;
}



//---------------------------------------------------------------------------
//	@function:
//		CSearchStrategyTest::EresUnittest_Timeout
//
//	@doc:
//		Test search strategy that times out
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CSearchStrategyTest::EresUnittest_Timeout()
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();
	CAutoTraceFlag atf(EopttracePrintOptimizationStatistics, true);
	CParseHandlerDXL *pphDXL = CDXLUtils::GetParseHandlerForDXLFile(
		mp, "../data/dxl/search/timeout-strategy.xml", NULL);
	CSearchStageArray *search_stage_array = pphDXL->GetSearchStageArray();
	search_stage_array->AddRef();
	Optimize(mp, CTestUtils::PexprLogicalNAryJoin, search_stage_array,
			 BuildMemo);

	SPQOS_DELETE(pphDXL);

	return SPQOS_OK;
}


//---------------------------------------------------------------------------
//	@function:
//		CSearchStrategyTest::EresUnittest_ParsingWithException
//
//	@doc:
//		Test exception handling when search strategy
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CSearchStrategyTest::EresUnittest_ParsingWithException()
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();
	CParseHandlerDXL *pphDXL = CDXLUtils::GetParseHandlerForDXLFile(
		mp, "../data/dxl/search/wrong-strategy.xml", NULL);
	SPQOS_DELETE(pphDXL);

	return SPQOS_OK;
}


//---------------------------------------------------------------------------
//	@function:
//		CSearchStrategyTest::PdrspqssRandom
//
//	@doc:
//		Generate a search strategy with random xform allocation
//
//---------------------------------------------------------------------------
CSearchStageArray *
CSearchStrategyTest::PdrspqssRandom(CMemoryPool *mp)
{
	CSearchStageArray *search_stage_array = SPQOS_NEW(mp) CSearchStageArray(mp);
	CXformSet *pxfsFst = SPQOS_NEW(mp) CXformSet(mp);
	CXformSet *pxfsSnd = SPQOS_NEW(mp) CXformSet(mp);

	// first xforms set contains essential rules to produce simple equality join plan
	(void) pxfsFst->ExchangeSet(CXform::ExfGet2TableScan);
	(void) pxfsFst->ExchangeSet(CXform::ExfSelect2Filter);
	(void) pxfsFst->ExchangeSet(CXform::ExfInnerJoin2HashJoin);

	// second xforms set contains all other rules
	pxfsSnd->Union(CXformFactory::Pxff()->PxfsExploration());
	pxfsSnd->Union(CXformFactory::Pxff()->PxfsImplementation());
	pxfsSnd->Difference(pxfsFst);

	search_stage_array->Append(SPQOS_NEW(mp) CSearchStage(
		pxfsFst, 1000 /*ulTimeThreshold*/, CCost(10E4) /*costThreshold*/));
	search_stage_array->Append(SPQOS_NEW(mp) CSearchStage(
		pxfsSnd, 10000 /*ulTimeThreshold*/, CCost(10E8) /*costThreshold*/));

	return search_stage_array;
}

//---------------------------------------------------------------------------
//	@function:
//		CSearchStrategyTest::BuildMemo
//
//	@doc:
//		Run the optimizer
//
//---------------------------------------------------------------------------
void
CSearchStrategyTest::BuildMemo(CMemoryPool *mp, CExpression *pexprInput,
							   CSearchStageArray *search_stage_array)
{
	CQueryContext *pqc = CTestUtils::PqcGenerate(mp, pexprInput);
	SPQOS_CHECK_ABORT;

	// enable space pruning
	CAutoTraceFlag atf(EopttraceEnableSpacePruning, true /*value*/);

	CWStringDynamic str(mp);
	COstreamString oss(&str);
	oss << std::endl << std::endl;
	oss << "INPUT EXPRESSION:" << std::endl;
	(void) pexprInput->OsPrint(oss);

	CEngine eng(mp);
	eng.Init(pqc, search_stage_array);
	eng.Optimize();

	CExpression *pexprPlan = eng.PexprExtractPlan();

	(void) pexprPlan->PrppCompute(mp, pqc->Prpp());

	oss << std::endl << "OUTPUT PLAN:" << std::endl;
	(void) pexprPlan->OsPrint(oss);
	oss << std::endl << std::endl;

	SPQOS_TRACE(str.GetBuffer());
	pexprPlan->Release();
	SPQOS_DELETE(pqc);

	SPQOS_CHECK_ABORT;
}

// EOF
