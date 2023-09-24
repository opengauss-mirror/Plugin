//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CTreeMapTest.cpp
//
//	@doc:
//		Test of tree map facility
//---------------------------------------------------------------------------

#include "unittest/spqopt/search/CTreeMapTest.h"

#include "spqos/error/CAutoTrace.h"
#include "spqos/io/COstreamString.h"
#include "spqos/string/CWStringDynamic.h"
#include "spqos/task/CAutoTraceFlag.h"
#include "spqos/task/CTask.h"
#include "spqos/types.h"

#include "spqopt/base/CDrvdPropCtxtPlan.h"
#include "spqopt/engine/CEngine.h"
#include "spqopt/eval/CConstExprEvaluatorDefault.h"
#include "spqopt/exception.h"
#include "spqopt/operators/ops.h"

#include "unittest/base.h"
#include "unittest/spqopt/CTestUtils.h"


ULONG CTreeMapTest::m_ulTestCounter = 0;  // start from first test

// minidump files that fail during plan enumeration because of large plan space
const CHAR *rgszFailedPlanEnumerationTests[] = {
	"../data/dxl/minidump/106-way-join.mdp",
};


//---------------------------------------------------------------------------
// raw data for test
//---------------------------------------------------------------------------
const ULONG ulElems = 10;
static ULONG rgul[ulElems];


//---------------------------------------------------------------------------
//	@function:
//		CTreeMapTest::EresUnittest
//
//	@doc:
//		Unittest for state machine
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CTreeMapTest::EresUnittest()
{
	CUnittest rgut[] = {
		SPQOS_UNITTEST_FUNC(CTreeMapTest::EresUnittest_Basic),
		SPQOS_UNITTEST_FUNC(CTreeMapTest::EresUnittest_Count),
		SPQOS_UNITTEST_FUNC(CTreeMapTest::EresUnittest_Unrank),
		SPQOS_UNITTEST_FUNC(CTreeMapTest::EresUnittest_Memo),

#ifndef SPQOS_DEBUG
		SPQOS_UNITTEST_FUNC(EresUnittest_FailedPlanEnumerationTests),
#endif	// SPQOS_DEBUG

#ifdef SPQOS_DEBUG
		SPQOS_UNITTEST_FUNC_ASSERT(CTreeMapTest::EresUnittest_Cycle),
#endif	// SPQOS_DEBUG
	};

	return CUnittest::EresExecute(rgut, SPQOS_ARRAY_SIZE(rgut));
}


//---------------------------------------------------------------------------
//	@function:
//		CTreeMapTest::CNode::OsPrint
//
//	@doc:
//		Debug print function
//
//---------------------------------------------------------------------------
IOstream &
CTreeMapTest::CNode::OsPrintWithIndent(IOstream &os, ULONG ulIndent) const
{
	for (ULONG ul = 0; ul < ulIndent; ul++)
	{
		os << " ";
	}
	os << "node: " << m_ulData << std::endl;

	for (ULONG ulChild = 0; ulChild < m_pdrspqnd->Size(); ulChild++)
	{
		(void) (*m_pdrspqnd)[ulChild]->OsPrintWithIndent(os, ulIndent + 2);
	}

	return os;
}


//---------------------------------------------------------------------------
//	@function:
//		CTreeMapTest::CNode::CNode
//
//	@doc:
//		ctor
//
//---------------------------------------------------------------------------
CTreeMapTest::CNode::CNode(CMemoryPool *,  // mp
						   ULONG *pulData, CNodeArray *pdrspqnd)
	: m_ulData(spqos::ulong_max), m_pdrspqnd(pdrspqnd)
{
	if (NULL != pulData)
	{
		m_ulData = *pulData;
	}
}


//---------------------------------------------------------------------------
//	@function:
//		CTreeMapTest::CNode::~CNode
//
//	@doc:
//		ctor
//
//---------------------------------------------------------------------------
CTreeMapTest::CNode::~CNode()
{
	CRefCount::SafeRelease(m_pdrspqnd);
}


//---------------------------------------------------------------------------
//	@function:
//		CTreeMapTest::Pnd
//
//	@doc:
//		Constructor function for result tree
//
//---------------------------------------------------------------------------
CTreeMapTest::CNode *
CTreeMapTest::Pnd(CMemoryPool *mp, ULONG *pul, CNodeArray *pdrspqnd,
				  BOOL *fTestTrue)
{
	// The test passes 'true' to PrUnrank and the rehydrate function expects to find it here.
	SPQOS_ASSERT(NULL != fTestTrue);
	SPQOS_ASSERT(*fTestTrue && "Flag is expected to be true");
	*fTestTrue = true;
	return SPQOS_NEW(mp) CNode(mp, pul, pdrspqnd);
}


//---------------------------------------------------------------------------
//	@function:
//		CTreeMapTest::PtmapLoad
//
//	@doc:
//		Create a semantically meaningful tree map by simulating a MEMO
//		layout
//
//---------------------------------------------------------------------------
CTreeMapTest::TestMap *
CTreeMapTest::PtmapLoad(CMemoryPool *mp)
{
	TestMap *ptmap = SPQOS_NEW(mp) TestMap(mp, &Pnd);

	// init raw data
	for (ULONG pos = 0; pos < ulElems; pos++)
	{
		rgul[pos] = pos;
	}


	// simulate the following MEMO with individual edge insertions
	struct SEdge
	{
		// number of parent node
		ULONG m_ulParent;

		// position of child
		ULONG m_ulPos;

		// number of child node
		ULONG m_ulChild;
	} rgedge[] = {
		// root group: Join [4,3], HashJoin[4,3]{8}, HashJoin[3,4]{9}
		{9, 1, 7},
		{9, 0, 6},
		{9, 0, 5},
		{8, 0, 7},
		{8, 1, 5},
		{8, 1, 6},

		// group 4: C, TabScan{7}

		// group 3: Join[1,2], HashJoin[1,2]{5}, SortMergeJoin[1,2]{6}
		{5, 0, 0},
		{5, 0, 1},
		{5, 1, 2},
		{5, 1, 3},
		{5, 1, 4},
		{6, 0, 1},
		{6, 1, 3},
		{6, 1, 4},

		// group 2: B, TabScan{2}, IndexScan{3}, Sort[2]{4}
		{4, 0, 2}

		// group 1: A, TabScan{0}, IndexScan{1}
	};

	for (ULONG ul = 0; ul < SPQOS_ARRAY_SIZE(rgedge); ul++)
	{
		SEdge &edge = rgedge[ul];
		ptmap->Insert(&rgul[edge.m_ulParent], edge.m_ulPos,
					  &rgul[edge.m_ulChild]);
	}

	return ptmap;
}


//---------------------------------------------------------------------------
//	@function:
//		CTreeMapTest::EresUnittest_Basic
//
//	@doc:
//		Basic test
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CTreeMapTest::EresUnittest_Basic()
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	TestMap *ptmap = NULL;

	// create blank map
	ptmap = SPQOS_NEW(mp) TestMap(mp, &Pnd);
	SPQOS_ASSERT(0 == ptmap->UllCount());
	SPQOS_DELETE(ptmap);

	// create map with test data
	ptmap = PtmapLoad(mp);
	SPQOS_DELETE(ptmap);

	return SPQOS_OK;
}


//---------------------------------------------------------------------------
//	@function:
//		CTreeMapTest::EresUnittest_Count
//
//	@doc:
//		Count and debug output of all counts at various nodes
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CTreeMapTest::EresUnittest_Count()
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	TestMap *ptmap = PtmapLoad(mp);

	// debug print
	CWStringDynamic str(mp);
	COstreamString oss(&str);

	ULLONG ullCount = ptmap->UllCount();
	oss << "total number of trees: " << ullCount << std::endl;

#ifdef SPQOS_DEBUG

	for (ULONG ul = 0; ul < ulElems; ul++)
	{
		oss << "node: " << ul << " count: " << ptmap->UllCount(&rgul[ul])
			<< std::endl;
	}

	(void) ptmap->OsPrint(oss);

#endif	// SPQOS_DEBUG

	SPQOS_TRACE(str.GetBuffer());
	SPQOS_DELETE(ptmap);

	return SPQOS_OK;
}


//---------------------------------------------------------------------------
//	@function:
//		CTreeMapTest::EresUnittest_Unrank
//
//	@doc:
//		Rehydrate all trees encoded in the map
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CTreeMapTest::EresUnittest_Unrank()
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	TestMap *ptmap = PtmapLoad(mp);

	// debug print
	CWStringDynamic str(mp);
	COstreamString oss(&str);

	ULLONG ullCount = ptmap->UllCount();

	for (ULONG ulRank = 0; ulRank < ullCount; ulRank++)
	{
		oss << "=== tree rank: " << ulRank << " ===" << std::endl;
		BOOL fFlag = true;
		CNode *pnd = ptmap->PrUnrank(mp, &fFlag, ulRank);
		(void) pnd->OsPrint(oss);

		pnd->Release();
	}

	SPQOS_TRACE(str.GetBuffer());
	SPQOS_DELETE(ptmap);

	return SPQOS_OK;
}


//---------------------------------------------------------------------------
//	@function:
//		CTreeMapTest::EresUnittest_Memo
//
//	@doc:
//		Test loading map from actual memo
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CTreeMapTest::EresUnittest_Memo()
{
	SPQOS_SET_TRACE(EtraceDisablePrintMemoryLeak);

	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	// setup a file-based provider
	CMDProviderMemory *pmdp = CTestUtils::m_pmdpf;
	pmdp->AddRef();
	CMDAccessor mda(mp, CMDCache::Pcache(), CTestUtils::m_sysidDefault, pmdp);

	CEngine *peng = NULL;
	CExpression *pexpr = NULL;
	CQueryContext *pqc = NULL;
	CExpression *pexprPlan = NULL;
	{
		// install opt context in TLS
		CAutoOptCtxt aoc(mp, &mda, NULL, /* pceeval */
						 CTestUtils::GetCostModel(mp));

		CAutoTraceFlag atf(EopttraceEnumeratePlans, true);

		peng = SPQOS_NEW(mp) CEngine(mp);

		// generate join expression
		pexpr = CTestUtils::PexprLogicalJoin<CLogicalInnerJoin>(mp);

		// generate query context
		pqc = CTestUtils::PqcGenerate(mp, pexpr);

		// Initialize engine
		peng->Init(pqc, NULL /*search_stage_array*/);

		// optimize query
		peng->Optimize();

		// extract plan
		pexprPlan = peng->PexprExtractPlan();
		SPQOS_ASSERT(NULL != pexprPlan);

		peng->Trace();
		{
			CAutoTrace at(mp);
			ULLONG ullCount = peng->Pmemotmap()->UllCount();
#ifdef SPQOS_DEBUG
			// test resetting map and re-creating it
			peng->ResetTreeMap();
			ULLONG ullCount2 = peng->Pmemotmap()->UllCount();
			SPQOS_ASSERT(ullCount == ullCount2);
#endif	// SPQOS_DEBUG

			for (ULONG ulRank = 0; ulRank < ullCount; ulRank++)
			{
				CDrvdPropCtxtPlan *pdpctxtplan =
					SPQOS_NEW(mp) CDrvdPropCtxtPlan(mp, false /*fUpdateCTEMap*/);
				CExpression *pexprAlt = NULL;
				SPQOS_TRY
				{
					pexprAlt =
						peng->Pmemotmap()->PrUnrank(mp, pdpctxtplan, ulRank);
					at.Os() << std::endl
							<< "ALTERNATIVE [" << ulRank << "]:" << std::endl
							<< *pexprAlt << std::endl;
				}
				SPQOS_CATCH_EX(ex)
				{
					if (!SPQOS_MATCH_EX(
							ex, spqopt::ExmaSPQOPT,
							spqopt::ExmiUnsatisfiedRequiredProperties))
					{
						SPQOS_RETHROW(ex);
					}
					IErrorContext *perrctxt = CTask::Self()->GetErrCtxt();
					at.Os() << perrctxt->GetErrorMsg() << std::endl;
					SPQOS_RESET_EX;
				}
				SPQOS_CATCH_END;
				CRefCount::SafeRelease(pexprAlt);
				CRefCount::SafeRelease(pdpctxtplan);
			}
		}
	}

	// clean up
	CRefCount::SafeRelease(pexprPlan);
	SPQOS_DELETE(pqc);
	CRefCount::SafeRelease(pexpr);
	SPQOS_DELETE(peng);

	return SPQOS_OK;
}


#ifndef SPQOS_DEBUG
//---------------------------------------------------------------------------
//	@function:
//		CTreeMapTest::EresUnittest_FailedPlanEnumerationTests
//
//	@doc:
//		Run Minidump-based tests that fail during plan enumeration because
//		of large plan space
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CTreeMapTest::EresUnittest_FailedPlanEnumerationTests()
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	BOOL fMatchPlans = false;
	BOOL fTestSpacePruning = false;
	fMatchPlans = true;
	fTestSpacePruning = true;

	// enable plan enumeration only if we match plans
	CAutoTraceFlag atf1(EopttraceEnumeratePlans, fMatchPlans);

	// enable stats derivation for DPE
	CAutoTraceFlag atf2(EopttraceDeriveStatsForDPE, true /*value*/);

	const ULONG ulTests = SPQOS_ARRAY_SIZE(rgszFailedPlanEnumerationTests);
	SPQOS_RESULT eres = SPQOS_OK;
	for (ULONG ul = 0; eres == SPQOS_OK && ul < ulTests; ul++)
	{
		SPQOS_TRY
		{
			eres = CTestUtils::EresRunMinidumps(
				mp, &rgszFailedPlanEnumerationTests[ul],
				1,	// ulTests
				&m_ulTestCounter,
				1,	// ulSessionId
				1,	// ulCmdId
				fMatchPlans, fTestSpacePruning);
		}
		SPQOS_CATCH_EX(ex)
		{
			if (SPQOS_MATCH_EX(ex, CException::ExmaSystem,
							  CException::ExmiOverflow))
			{
				eres = SPQOS_OK;
				SPQOS_RESET_EX;
			}
			else
			{
				SPQOS_RETHROW(ex);

				eres = SPQOS_FAILED;
			}
		}
		SPQOS_CATCH_END;
	}

	return eres;
}
#endif	// SPQOS_DEBUG



#ifdef SPQOS_DEBUG
//---------------------------------------------------------------------------
//	@function:
//		CTreeMapTest::EresUnittest_Cycle
//
//	@doc:
//		Introduce cycle in graph; counting must assert
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CTreeMapTest::EresUnittest_Cycle()
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	TestMap *ptmap = SPQOS_NEW(mp) TestMap(mp, &Pnd);

	CAutoP<TestMap> a_ptmap;
	a_ptmap = ptmap;

	// build cycle
	ptmap->Insert(&rgul[0], 0, &rgul[1]);
	ptmap->Insert(&rgul[1], 0, &rgul[2]);
	ptmap->Insert(&rgul[2], 0, &rgul[1]);

	(void) ptmap->UllCount();

	return SPQOS_FAILED;
}

#endif	// SPQOS_FAILED

// EOF
