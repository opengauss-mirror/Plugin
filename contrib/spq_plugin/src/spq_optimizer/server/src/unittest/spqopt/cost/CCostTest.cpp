//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CCostTest.cpp
//
//	@doc:
//		Tests for basic operations on cost
//---------------------------------------------------------------------------
#include "unittest/spqopt/cost/CCostTest.h"

#include "spqos/error/CAutoTrace.h"
#include "spqos/task/CAutoTraceFlag.h"

#include "spqdbcost/CCostModelSPQDB.h"
#include "spqopt/cost/CCost.h"
#include "spqopt/cost/ICostModelParams.h"
#include "spqopt/engine/CEngine.h"
#include "spqopt/eval/CConstExprEvaluatorDefault.h"
#include "spqopt/minidump/CMinidumperUtils.h"
#include "spqopt/operators/CLogicalInnerJoin.h"
#include "spqopt/optimizer/COptimizerConfig.h"
#include "naucrates/dxl/CDXLUtils.h"
#include "naucrates/dxl/parser/CParseHandlerDXL.h"

#include "unittest/base.h"
#include "unittest/spqopt/CTestUtils.h"

//---------------------------------------------------------------------------
//	@function:
//		CCostTest::EresUnittest
//
//	@doc:
//		Driver for unittests
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CCostTest::EresUnittest()
{
	CUnittest rgut[] = {
		SPQOS_UNITTEST_FUNC(CCostTest::EresUnittest_Bool),
		SPQOS_UNITTEST_FUNC(CCostTest::EresUnittest_Arithmetic),
		SPQOS_UNITTEST_FUNC(CCostTest::EresUnittest_Params),
		SPQOS_UNITTEST_FUNC(CCostTest::EresUnittest_Parsing),
		SPQOS_UNITTEST_FUNC(EresUnittest_SetParams),

		// TODO: : re-enable test after resolving exception throwing problem on OSX
		// SPQOS_UNITTEST_FUNC_THROW(CCostTest::EresUnittest_ParsingWithException, spqdxl::ExmaDXL, spqdxl::ExmiDXLUnexpectedTag),
	};

	return CUnittest::EresExecute(rgut, SPQOS_ARRAY_SIZE(rgut));
}


//---------------------------------------------------------------------------
//	@function:
//		CCostTest::EresUnittest_Arithmetic
//
//	@doc:
//		Test arithmetic operations
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CCostTest::EresUnittest_Arithmetic()
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	CCost cost1(2.5);
	CCost cost2(3.0);
	CCost cost3(5.49);
	CCost cost4(5.51);
	CCost cost5(7.49);
	CCost cost6(7.51);

	CCost costAdd(cost1 + cost2);
	CCost costMultiply(cost1 * cost2);

	SPQOS_ASSERT(costAdd > cost3);
	SPQOS_ASSERT(costAdd < cost4);

	SPQOS_ASSERT(costMultiply > cost5);
	SPQOS_ASSERT(costMultiply < cost6);

	CAutoTrace at(mp);
	IOstream &os(at.Os());

	os << "Arithmetic operations: " << std::endl
	   << cost1 << " + " << cost2 << " = " << costAdd << std::endl
	   << cost1 << " * " << cost2 << " = " << costMultiply << std::endl;

	return SPQOS_OK;
}


//---------------------------------------------------------------------------
//	@function:
//		CCostTest::EresUnittest_Bool
//
//	@doc:
//		Test comparison operations
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CCostTest::EresUnittest_Bool()
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	CCost cost1(2.5);
	CCost cost2(3.5);

	SPQOS_ASSERT(cost1 < cost2);
	SPQOS_ASSERT(cost2 > cost1);

	CAutoTrace at(mp);
	IOstream &os(at.Os());

	os << "Boolean operations: " << std::endl
	   << cost1 << " < " << cost2 << " = " << (cost1 < cost2) << std::endl
	   << cost2 << " > " << cost1 << " = " << (cost2 > cost1) << std::endl;

	return SPQOS_OK;
}

//---------------------------------------------------------------------------
//	@function:
//		CCostTest::TestParams
//
//	@doc:
//		Test cost model parameters
//
//---------------------------------------------------------------------------
void
CCostTest::TestParams(CMemoryPool *mp)
{
	CAutoTrace at(mp);
	IOstream &os(at.Os());

	ICostModelParams *pcp =
		((CCostModelSPQDB *) COptCtxt::PoctxtFromTLS()->GetCostModel())
			->GetCostModelParams();

	CDouble dSeqIOBandwidth =
		pcp->PcpLookup(CCostModelParamsSPQDB::EcpSeqIOBandwidth)->Get();
	CDouble dRandomIOBandwidth =
		pcp->PcpLookup(CCostModelParamsSPQDB::EcpRandomIOBandwidth)->Get();
	CDouble dTupProcBandwidth =
		pcp->PcpLookup(CCostModelParamsSPQDB::EcpTupProcBandwidth)->Get();
	CDouble dNetBandwidth =
		pcp->PcpLookup(CCostModelParamsSPQDB::EcpNetBandwidth)->Get();
	CDouble dSegments =
		pcp->PcpLookup(CCostModelParamsSPQDB::EcpSegments)->Get();
	CDouble dNLJFactor =
		pcp->PcpLookup(CCostModelParamsSPQDB::EcpNLJFactor)->Get();
	CDouble dHashFactor =
		pcp->PcpLookup(CCostModelParamsSPQDB::EcpHashFactor)->Get();
	CDouble dDefaultCost =
		pcp->PcpLookup(CCostModelParamsSPQDB::EcpDefaultCost)->Get();

	os << std::endl << "Lookup cost model params by id: " << std::endl;
	os << "Seq I/O bandwidth: " << dSeqIOBandwidth << std::endl;
	os << "Random I/O bandwidth: " << dRandomIOBandwidth << std::endl;
	os << "Tuple proc bandwidth: " << dTupProcBandwidth << std::endl;
	os << "Network bandwidth: " << dNetBandwidth << std::endl;
	os << "Segments: " << dSegments << std::endl;
	os << "NLJ Factor: " << dNLJFactor << std::endl;
	os << "Hash Factor: " << dHashFactor << std::endl;
	os << "Default Cost: " << dDefaultCost << std::endl;

	CDouble dSeqIOBandwidth1 = pcp->PcpLookup("SeqIOBandwidth")->Get();
	CDouble dRandomIOBandwidth1 = pcp->PcpLookup("RandomIOBandwidth")->Get();
	CDouble dTupProcBandwidth1 = pcp->PcpLookup("TupProcBandwidth")->Get();
	CDouble dNetBandwidth1 = pcp->PcpLookup("NetworkBandwidth")->Get();
	CDouble dSegments1 = pcp->PcpLookup("Segments")->Get();
	CDouble dNLJFactor1 = pcp->PcpLookup("NLJFactor")->Get();
	CDouble dHashFactor1 = pcp->PcpLookup("HashFactor")->Get();
	CDouble dDefaultCost1 = pcp->PcpLookup("DefaultCost")->Get();

	os << std::endl << "Lookup cost model params by name: " << std::endl;
	os << "Seq I/O bandwidth: " << dSeqIOBandwidth1 << std::endl;
	os << "Random I/O bandwidth: " << dRandomIOBandwidth1 << std::endl;
	os << "Tuple proc bandwidth: " << dTupProcBandwidth1 << std::endl;
	os << "Network bandwidth: " << dNetBandwidth1 << std::endl;
	os << "Segments: " << dSegments1 << std::endl;
	os << "NLJ Factor: " << dNLJFactor1 << std::endl;
	os << "Hash Factor: " << dHashFactor1 << std::endl;
	os << "Default Cost: " << dDefaultCost1 << std::endl;

	SPQOS_ASSERT(dSeqIOBandwidth == dSeqIOBandwidth1);
	SPQOS_ASSERT(dRandomIOBandwidth == dRandomIOBandwidth1);
	SPQOS_ASSERT(dTupProcBandwidth == dTupProcBandwidth1);
	SPQOS_ASSERT(dNetBandwidth == dNetBandwidth1);
	SPQOS_ASSERT(dSegments == dSegments1);
	SPQOS_ASSERT(dNLJFactor == dNLJFactor1);
	SPQOS_ASSERT(dHashFactor == dHashFactor1);
	SPQOS_ASSERT(dDefaultCost == dDefaultCost1);
}


//---------------------------------------------------------------------------
//	@function:
//		CCostTest::EresUnittest_Params
//
//	@doc:
//		Cost model parameters
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CCostTest::EresUnittest_Params()
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	// setup a file-based provider
	CMDProviderMemory *pmdp = CTestUtils::m_pmdpf;
	pmdp->AddRef();
	CMDAccessor mda(mp, CMDCache::Pcache(), CTestUtils::m_sysidDefault, pmdp);

	// install opt context in TLS
	CAutoOptCtxt aoc(mp, &mda, NULL, /* pceeval */
					 SPQOS_NEW(mp) CCostModelSPQDB(mp, SPQOPT_TEST_SEGMENTS));

	TestParams(mp);

	return SPQOS_OK;
}


//---------------------------------------------------------------------------
//	@function:
//		CCostTest::EresUnittest_Parsing
//
//	@doc:
//		Test parsing cost params from external file
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CCostTest::EresUnittest_Parsing()
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();
	CParseHandlerDXL *pphDXL = CDXLUtils::GetParseHandlerForDXLFile(
		mp, "../data/dxl/cost/cost0.xml", NULL);
	ICostModelParams *pcp = pphDXL->GetCostModelParams();

	{
		CAutoTrace at(mp);
		at.Os() << " Parsed cost params: " << std::endl;
		pcp->OsPrint(at.Os());
	}
	SPQOS_DELETE(pphDXL);

	return SPQOS_OK;
}


//---------------------------------------------------------------------------
//	@function:
//		CSearchStrategyTest::EresUnittest_ParsingWithException
//
//	@doc:
//		Test exception handling when parsing cost params
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CCostTest::EresUnittest_ParsingWithException()
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();
	CParseHandlerDXL *pphDXL = CDXLUtils::GetParseHandlerForDXLFile(
		mp, "../data/dxl/cost/wrong-cost.xml", NULL);
	SPQOS_DELETE(pphDXL);

	return SPQOS_OK;
}


//---------------------------------------------------------------------------
//	@function:
//		CCostTest::EresUnittest_SetParams
//
//	@doc:
//		Test of setting cost model params
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CCostTest::EresUnittest_SetParams()
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	// setup a file-based provider
	CMDProviderMemory *pmdp = CTestUtils::m_pmdpf;
	pmdp->AddRef();
	CMDAccessor mda(mp, CMDCache::Pcache(), CTestUtils::m_sysidDefault, pmdp);

	ICostModel *pcm = SPQOS_NEW(mp) CCostModelSPQDB(mp, SPQOPT_TEST_SEGMENTS);

	// install opt context in TLS
	CAutoOptCtxt aoc(mp, &mda, NULL, /* pceeval */ pcm);

	// generate in-equality join expression
	CExpression *pexprOuter = CTestUtils::PexprLogicalGet(mp);
	const CColRef *pcrOuter = pexprOuter->DeriveOutputColumns()->PcrAny();
	CExpression *pexprInner = CTestUtils::PexprLogicalGet(mp);
	const CColRef *pcrInner = pexprInner->DeriveOutputColumns()->PcrAny();
	CExpression *pexprPred =
		CUtils::PexprScalarCmp(mp, pcrOuter, pcrInner, IMDType::EcmptNEq);
	CExpression *pexpr = CUtils::PexprLogicalJoin<CLogicalInnerJoin>(
		mp, pexprOuter, pexprInner, pexprPred);

	// optimize in-equality join based on default cost model params
	CExpression *pexprPlan1 = NULL;
	{
		CEngine eng(mp);

		// generate query context
		CQueryContext *pqc = CTestUtils::PqcGenerate(mp, pexpr);

		// Initialize engine
		eng.Init(pqc, NULL /*search_stage_array*/);

		// optimize query
		eng.Optimize();

		// extract plan
		pexprPlan1 = eng.PexprExtractPlan();
		SPQOS_ASSERT(NULL != pexprPlan1);

		SPQOS_DELETE(pqc);
	}

	// change NLJ cost factor
	ICostModelParams::SCostParam *pcp = pcm->GetCostModelParams()->PcpLookup(
		CCostModelParamsSPQDB::EcpNLJFactor);
	CDouble dNLJFactor = CDouble(2.0);
	CDouble dVal = pcp->Get() * dNLJFactor;
	pcm->GetCostModelParams()->SetParam(pcp->Id(), dVal, dVal - 0.5,
										dVal + 0.5);

	// optimize again after updating NLJ cost factor
	CExpression *pexprPlan2 = NULL;
	{
		CEngine eng(mp);

		// generate query context
		CQueryContext *pqc = CTestUtils::PqcGenerate(mp, pexpr);

		// Initialize engine
		eng.Init(pqc, NULL /*search_stage_array*/);

		// optimize query
		eng.Optimize();

		// extract plan
		pexprPlan2 = eng.PexprExtractPlan();
		SPQOS_ASSERT(NULL != pexprPlan2);

		SPQOS_DELETE(pqc);
	}

	{
		CAutoTrace at(mp);
		at.Os() << "\nPLAN1: \n" << *pexprPlan1;
		at.Os() << "\nNLJ Cost1: " << (*pexprPlan1)[0]->Cost();
		at.Os() << "\n\nPLAN2: \n" << *pexprPlan2;
		at.Os() << "\nNLJ Cost2: " << (*pexprPlan2)[0]->Cost();
	}
	SPQOS_ASSERT(
		(*pexprPlan2)[0]->Cost() >= (*pexprPlan1)[0]->Cost() * dNLJFactor &&
		"expected NLJ cost in PLAN2 to be larger than NLJ cost in PLAN1");

	// clean up
	pexpr->Release();
	pexprPlan1->Release();
	pexprPlan2->Release();

	return SPQOS_OK;
}

// EOF
