//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 - 2011 EMC Corp.
//
//	@filename:
//		CEngineTest.cpp
//
//	@doc:
//		Test for CEngine
//---------------------------------------------------------------------------
#include "unittest/spqopt/engine/CEngineTest.h"

#include "spqos/error/CAutoTrace.h"
#include "spqos/task/CAutoTraceFlag.h"

#include "spqopt/base/CColRefSetIter.h"
#include "spqopt/base/CUtils.h"
#include "spqopt/engine/CEngine.h"
#include "spqopt/eval/CConstExprEvaluatorDefault.h"
#include "spqopt/mdcache/CMDCache.h"
#include "spqopt/operators/ops.h"
#include "spqopt/search/CGroup.h"
#include "spqopt/search/CGroupProxy.h"

#include "unittest/base.h"
#include "unittest/spqopt/CSubqueryTestUtils.h"

ULONG CEngineTest::m_ulTestCounter = 0;		 // start from first test
ULONG CEngineTest::m_ulTestCounterSubq = 0;	 // start from first test

//---------------------------------------------------------------------------
//	@function:
//		CEngineTest::EresUnittest
//
//	@doc:
//		Unittest for engine
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CEngineTest::EresUnittest()
{
	CUnittest rgut[] = {
		SPQOS_UNITTEST_FUNC(EresUnittest_Basic),
#ifdef SPQOS_DEBUG
		SPQOS_UNITTEST_FUNC(EresUnittest_BuildMemo),
		SPQOS_UNITTEST_FUNC(EresUnittest_AppendStats),
		SPQOS_UNITTEST_FUNC(EresUnittest_BuildMemoWithSubqueries),
		SPQOS_UNITTEST_FUNC(EresUnittest_BuildMemoWithGrouping),
		SPQOS_UNITTEST_FUNC(EresUnittest_BuildMemoWithTVF),
		SPQOS_UNITTEST_FUNC(EresUnittest_BuildMemoWithPartitioning),
		SPQOS_UNITTEST_FUNC(EresUnittest_BuildMemoWithWindowing),
		SPQOS_UNITTEST_FUNC(EresUnittest_BuildMemoLargeJoins),
		SPQOS_UNITTEST_FUNC(EresUnittest_BuildMemoWithCTE),
#endif	// SPQOS_DEBUG
	};

	return CUnittest::EresExecute(rgut, SPQOS_ARRAY_SIZE(rgut));
}


//---------------------------------------------------------------------------
//	@function:
//		CEngineTest::EresUnittest_Basic
//
//	@doc:
//		Basic test
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CEngineTest::EresUnittest_Basic()
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	CAutoTraceFlag atf1(EopttracePrintOptimizationStatistics, true);
	CAutoTraceFlag atf2(EopttracePrintMemoAfterExploration, true);
	CAutoTraceFlag atf3(EopttracePrintMemoAfterImplementation, true);
	CAutoTraceFlag atf4(EopttracePrintMemoAfterOptimization, true);
	CAutoTraceFlag atf6(EopttracePrintXform, true);
	CAutoTraceFlag atf7(EopttracePrintGroupProperties, true);
	CAutoTraceFlag atf8(EopttracePrintOptimizationContext, true);
	CAutoTraceFlag atf9(EopttracePrintXformPattern, true);

	// setup a file-based provider
	CMDProviderMemory *pmdp = CTestUtils::m_pmdpf;
	pmdp->AddRef();
	CMDAccessor mda(mp, CMDCache::Pcache(), CTestUtils::m_sysidDefault, pmdp);

	// install opt context in TLS
	CAutoOptCtxt aoc(mp, &mda, NULL, /* pceeval */
					 CTestUtils::GetCostModel(mp));

	CEngine eng(mp);

	// generate  join expression
	CExpression *pexpr = CTestUtils::PexprLogicalJoin<CLogicalInnerJoin>(mp);

	// generate query context
	CQueryContext *pqc = CTestUtils::PqcGenerate(mp, pexpr);

	// Initialize engine
	eng.Init(pqc, NULL /*search_stage_array*/);

	// optimize query
	eng.Optimize();

	// extract plan
	CExpression *pexprPlan = eng.PexprExtractPlan();
	SPQOS_ASSERT(NULL != pexprPlan);

	// clean up
	pexpr->Release();
	pexprPlan->Release();
	SPQOS_DELETE(pqc);

	return SPQOS_OK;
}


//---------------------------------------------------------------------------
//	@function:
//		CEngineTest::EresOptimize
//
//	@doc:
//		Helper for optimizing deep join trees;
//		generate an array of join expressions for the given relation, if the
//		bit corresponding to an expression is set, the optimizer is invoked
//		for that expression
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CEngineTest::EresOptimize(
	FnOptimize *pfopt,
	CWStringConst *str,	 // array of relation names
	ULONG *pul,			 // array of relation OIDs
	ULONG ulRels,		 // number of array entries
	CBitSet *
		pbs	 // if a bit is set, the corresponding join expression will be optimized
)
{
	SPQOS_ASSERT(NULL != pfopt);
	SPQOS_ASSERT(NULL != str);
	SPQOS_ASSERT(NULL != pul);
	SPQOS_ASSERT(NULL != pbs);

	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	// setup a file-based provider
	CMDProviderMemory *pmdp = CTestUtils::m_pmdpf;
	pmdp->AddRef();
	CMDAccessor mda(mp, CMDCache::Pcache());
	mda.RegisterProvider(CTestUtils::m_sysidDefault, pmdp);
	// scope for optimization context
	{
		CAutoOptCtxt aoc(mp, &mda, NULL, /* pceeval */
						 CTestUtils::GetCostModel(mp));

		// generate cross product expressions
		CExpressionJoinsArray *pdrspqexprCrossProducts =
			CTestUtils::PdrspqexprJoins(mp, str, pul, ulRels,
									   true /*fCrossProduct*/);

		// generate join expressions
		CExpressionJoinsArray *pdrspqexpr = CTestUtils::PdrspqexprJoins(
			mp, str, pul, ulRels, false /*fCrossProduct*/);

		// build memo for each expression
		for (ULONG ul = m_ulTestCounter; ul < ulRels; ul++)
		{
			if (pbs->Get(ul))
			{
				pfopt(mp, (*pdrspqexprCrossProducts)[ul],
					  NULL /*search_stage_array*/);
				SPQOS_CHECK_ABORT;

				pfopt(mp, (*pdrspqexpr)[ul], NULL /*search_stage_array*/);
				SPQOS_CHECK_ABORT;

				m_ulTestCounter++;
			}
		}

		// reset counter
		m_ulTestCounter = 0;

		(*pdrspqexprCrossProducts)[ulRels - 1]->Release();
		CRefCount::SafeRelease(pdrspqexprCrossProducts);

		(*pdrspqexpr)[ulRels - 1]->Release();
		CRefCount::SafeRelease(pdrspqexpr);
	}

	return SPQOS_OK;
}


#ifdef SPQOS_DEBUG
//---------------------------------------------------------------------------
//	@function:
//		CEngineTest::EresUnittest_BuildMemo
//
//	@doc:
//		Test of building memo from different input expressions
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CEngineTest::EresUnittest_BuildMemo()
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	// array of relation names
	CWStringConst rgscRel[] = {
		SPQOS_WSZ_LIT("Rel1"), SPQOS_WSZ_LIT("Rel2"), SPQOS_WSZ_LIT("Rel3"),
		SPQOS_WSZ_LIT("Rel4"), SPQOS_WSZ_LIT("Rel5"),
	};

	// array of relation IDs
	ULONG rgulRel[] = {
		SPQOPT_TEST_REL_OID1, SPQOPT_TEST_REL_OID2, SPQOPT_TEST_REL_OID3,
		SPQOPT_TEST_REL_OID4, SPQOPT_TEST_REL_OID5,
	};

	CBitSet *pbs = SPQOS_NEW(mp) CBitSet(mp);
	const ULONG ulRels = SPQOS_ARRAY_SIZE(rgscRel);
	for (ULONG ul = 0; ul < ulRels; ul++)
	{
		(void) pbs->ExchangeSet(ul);
	}

	SPQOS_RESULT eres =
		EresOptimize(BuildMemoRecursive, rgscRel, rgulRel, ulRels, pbs);
	pbs->Release();

	return eres;
}


//---------------------------------------------------------------------------
//	@function:
//		CEngineTest::EresUnittest_AppendStats
//
//	@doc:
//		Test of appending stats during optimization
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CEngineTest::EresUnittest_AppendStats()
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	CAutoTraceFlag atf(EopttracePrintGroupProperties, true);

	// setup a file-based provider
	CMDProviderMemory *pmdp = CTestUtils::m_pmdpf;
	pmdp->AddRef();
	CMDAccessor mda(mp, CMDCache::Pcache(), CTestUtils::m_sysidDefault, pmdp);

	// install opt context in TLS
	CAutoOptCtxt aoc(mp, &mda, NULL, /* pceeval */
					 CTestUtils::GetCostModel(mp));

	CEngine eng(mp);

	// generate  join expression
	CExpression *pexpr = CTestUtils::PexprLogicalJoin<CLogicalInnerJoin>(mp);

	// generate query context
	CQueryContext *pqc = CTestUtils::PqcGenerate(mp, pexpr);

	// Initialize engine
	eng.Init(pqc, NULL /*search_stage_array*/);

	CGroupExpression *pgexpr = NULL;
	{
		CGroupProxy spq(eng.PgroupRoot());
		pgexpr = spq.PgexprFirst();
	}

	// derive stats with empty requirements
	{
		CAutoTrace at(mp);

		CExpressionHandle exprhdl(mp);
		exprhdl.Attach(pgexpr);
		exprhdl.DeriveStats(mp, mp, NULL /*prprel*/, NULL /*stats_ctxt*/);
		at.Os() << std::endl
				<< "MEMO AFTER FIRST STATS DERIVATION:" << std::endl;
	}
	eng.Trace();

	// create a non-empty set of output columns as requirements for stats derivation
	ULONG ulIndex = 0;
	CColRefSet *pcrs = SPQOS_NEW(mp) CColRefSet(mp);
	CColRefSetIter crsi(*pexpr->DeriveOutputColumns());
	while (crsi.Advance() && ulIndex < 3)
	{
		CColRef *colref = crsi.Pcr();
		if (ulIndex == 1 || ulIndex == 2)
		{
			pcrs->Include(colref);
		}
		ulIndex++;
	}

	CReqdPropRelational *prprel = SPQOS_NEW(mp) CReqdPropRelational(pcrs);

	// derive stats with non-empty requirements
	// missing stats should be appended to the already derived ones
	{
		CAutoTrace at(mp);

		CExpressionHandle exprhdl(mp);
		exprhdl.Attach(pgexpr);
		exprhdl.DeriveStats(mp, mp, prprel, NULL /*stats_ctxt*/);
		at.Os() << std::endl
				<< "MEMO AFTER SECOND STATS DERIVATION:" << std::endl;
	}
	eng.Trace();

	pexpr->Release();
	prprel->Release();
	SPQOS_DELETE(pqc);

	return SPQOS_OK;
}


//---------------------------------------------------------------------------
//	@function:
//		CEngineTest::EresUnittest_BuildMemoLargeJoins
//
//	@doc:
//		Test of building memo for a large number of joins
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CEngineTest::EresUnittest_BuildMemoLargeJoins()
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	// array of relation names
	CWStringConst rgscRel[] = {
		SPQOS_WSZ_LIT("Rel1"), SPQOS_WSZ_LIT("Rel2"), SPQOS_WSZ_LIT("Rel3"),
		SPQOS_WSZ_LIT("Rel4"), SPQOS_WSZ_LIT("Rel5"),
	};

	// array of relation IDs
	ULONG rgulRel[] = {
		SPQOPT_TEST_REL_OID1, SPQOPT_TEST_REL_OID2, SPQOPT_TEST_REL_OID3,
		SPQOPT_TEST_REL_OID4, SPQOPT_TEST_REL_OID5,
	};

	// only optimize the last join expression
	CBitSet *pbs = SPQOS_NEW(mp) CBitSet(mp);
	const ULONG ulRels = SPQOS_ARRAY_SIZE(rgscRel);
	(void) pbs->ExchangeSet(ulRels - 1);

	SPQOS_RESULT eres =
		EresOptimize(BuildMemoRecursive, rgscRel, rgulRel, ulRels, pbs);
	pbs->Release();

	return eres;
}


//---------------------------------------------------------------------------
//	@function:
//		CEngineTest::BuildMemoRecursive
//
//	@doc:
//		Build memo recursively from a given input expression
//
//---------------------------------------------------------------------------
void
CEngineTest::BuildMemoRecursive(CMemoryPool *mp, CExpression *pexprInput,
								CSearchStageArray *search_stage_array)
{
	CQueryContext *pqc = CTestUtils::PqcGenerate(mp, pexprInput);

	CAutoTrace at(mp);
	IOstream &os(at.Os());

	os << std::endl << std::endl;
	os << "QUERY CONTEXT:" << std::endl;
	(void) pqc->OsPrint(os);

	// enable space pruning
	CAutoTraceFlag atf(EopttraceEnableSpacePruning, true /*value*/);

	CEngine eng(mp);
	eng.Init(pqc, search_stage_array);
	eng.PrintRoot();
	SPQOS_CHECK_ABORT;

	eng.RecursiveOptimize();
	SPQOS_CHECK_ABORT;

	CExpression *pexprPlan = eng.PexprExtractPlan();
	SPQOS_ASSERT(NULL != pexprPlan);

	(void) pexprPlan->PrppCompute(mp, pqc->Prpp());

	os << std::endl << std::endl;
	os << "OUTPUT PLAN:" << std::endl;
	(void) pexprPlan->OsPrint(os);
	os << std::endl << std::endl;

	eng.PrintOptCtxts();
	pexprPlan->Release();
	SPQOS_DELETE(pqc);

	SPQOS_CHECK_ABORT;
}


//---------------------------------------------------------------------------
//	@function:
//		CEngineTest::EresTestEngine
//
//	@doc:
//		Helper for testing engine using an array of expression generators
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CEngineTest::EresTestEngine(Pfpexpr rspqf[], ULONG size)
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	// setup a file-based provider
	CMDProviderMemory *pmdp = CTestUtils::m_pmdpf;
	pmdp->AddRef();
	CMDAccessor mda(mp, CMDCache::Pcache());
	mda.RegisterProvider(CTestUtils::m_sysidDefault, pmdp);

	for (ULONG ul = m_ulTestCounter; ul < size; ul++)
	{
		// install opt context in TLS
		CAutoOptCtxt aoc(mp, &mda, NULL, /* pceeval */
						 CTestUtils::GetCostModel(mp));

		CExpression *pexpr = rspqf[ul](mp);
		BuildMemoRecursive(mp, pexpr, NULL /*search_stage_array*/);
		pexpr->Release();

		m_ulTestCounter++;
	}

	// reset counter
	m_ulTestCounter = 0;

	return SPQOS_OK;
}


//---------------------------------------------------------------------------
//	@function:
//		CEngineTest::EresUnittest_BuildMemoWithSubqueries
//
//	@doc:
//		Test of building memo for expressions with subqueries
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CEngineTest::EresUnittest_BuildMemoWithSubqueries()
{
	typedef CExpression *(*Pfpexpr)(CMemoryPool *, BOOL);
	Pfpexpr rspqf[] = {
		CSubqueryTestUtils::PexprSelectWithAllAggSubquery,
		CSubqueryTestUtils::PexprSelectWithAggSubquery,
		CSubqueryTestUtils::PexprSelectWithAggSubqueryConstComparison,
		CSubqueryTestUtils::PexprProjectWithAggSubquery,
		CSubqueryTestUtils::PexprSelectWithAnySubquery,
		CSubqueryTestUtils::PexprProjectWithAnySubquery,
		CSubqueryTestUtils::PexprSelectWithAllSubquery,
		CSubqueryTestUtils::PexprProjectWithAllSubquery,
		CSubqueryTestUtils::PexprSelectWithExistsSubquery,
		CSubqueryTestUtils::PexprProjectWithExistsSubquery,
		CSubqueryTestUtils::PexprSelectWithNotExistsSubquery,
		CSubqueryTestUtils::PexprProjectWithNotExistsSubquery,
		CSubqueryTestUtils::PexprSelectWithNestedCmpSubquery,
		CSubqueryTestUtils::PexprSelectWithCmpSubqueries,
		CSubqueryTestUtils::PexprSelectWithSubqueryConjuncts,
		CSubqueryTestUtils::PexprProjectWithSubqueries,
		CSubqueryTestUtils::PexprSelectWithAggSubqueryOverJoin,
		CSubqueryTestUtils::PexprSelectWithNestedSubquery,
		CSubqueryTestUtils::PexprSelectWithNestedAnySubqueries,
		CSubqueryTestUtils::PexprSelectWithNestedAllSubqueries,
		CSubqueryTestUtils::PexprSelectWith2LevelsCorrSubquery,
		CSubqueryTestUtils::PexprSubqueriesInNullTestContext,
		CSubqueryTestUtils::PexprSubqueriesInDifferentContexts,
		CSubqueryTestUtils::PexprSelectWithTrimmableExists,
		CSubqueryTestUtils::PexprSelectWithTrimmableNotExists,
		CSubqueryTestUtils::PexprSelectWithSubqueryDisjuncts,
		CSubqueryTestUtils::PexprUndecorrelatableAnySubquery,
		CSubqueryTestUtils::PexprUndecorrelatableAllSubquery,
		CSubqueryTestUtils::PexprUndecorrelatableExistsSubquery,
		CSubqueryTestUtils::PexprUndecorrelatableNotExistsSubquery,
		CSubqueryTestUtils::PexprUndecorrelatableScalarSubquery,
		CSubqueryTestUtils::PexprSelectWithAnySubqueryOverWindow,
		CSubqueryTestUtils::PexprSelectWithAllSubqueryOverWindow,
	};

	BOOL fCorrelated = true;

	// we get two expressions using each generator
	const ULONG size = 2 * SPQOS_ARRAY_SIZE(rspqf);
	for (ULONG ul = m_ulTestCounterSubq; ul < size; ul++)
	{
		CAutoMemoryPool amp;
		CMemoryPool *mp = amp.Pmp();

		// setup a file-based provider
		CMDProviderMemory *pmdp = CTestUtils::m_pmdpf;
		pmdp->AddRef();
		CMDAccessor mda(mp, CMDCache::Pcache());
		mda.RegisterProvider(CTestUtils::m_sysidDefault, pmdp);

		{
			// install opt context in TLS
			CAutoOptCtxt aoc(mp, &mda, NULL, /* pceeval */
							 CTestUtils::GetCostModel(mp));

			ULONG ulIndex = ul / 2;
			CExpression *pexpr = rspqf[ulIndex](mp, fCorrelated);
			BuildMemoRecursive(mp, pexpr, NULL /*search_stage_array*/);
			pexpr->Release();
		}

		fCorrelated = !fCorrelated;
		m_ulTestCounterSubq++;
	}

	m_ulTestCounterSubq = 0;

	return SPQOS_OK;
}


//---------------------------------------------------------------------------
//	@function:
//		CEngineTest::EresUnittest_BuildMemoWithTVF
//
//	@doc:
//		Test of building memo for expressions with table-valued functions
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CEngineTest::EresUnittest_BuildMemoWithTVF()
{
	Pfpexpr rspqf[] = {
		CTestUtils::PexprLogicalTVFTwoArgs,
		CTestUtils::PexprLogicalTVFThreeArgs,
		CTestUtils::PexprLogicalTVFNoArgs,
	};

	return EresTestEngine(rspqf, SPQOS_ARRAY_SIZE(rspqf));
}

//---------------------------------------------------------------------------
//	@function:
//		CEngineTest::EresUnittest_BuildMemoWithGrouping
//
//	@doc:
//		Test of building memo for expressions with grouping
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CEngineTest::EresUnittest_BuildMemoWithGrouping()
{
	Pfpexpr rspqf[] = {
		CTestUtils::PexprLogicalGbAgg, CTestUtils::PexprLogicalGbAggOverJoin,
		CTestUtils::PexprLogicalGbAggWithSum, CTestUtils::PexprLogicalNAryJoin};

	return EresTestEngine(rspqf, SPQOS_ARRAY_SIZE(rspqf));
}


//---------------------------------------------------------------------------
//	@function:
//		CEngineTest::EresUnittest_BuildMemoWithPartitioning
//
//	@doc:
//		Test of building memo for expressions with partitioning
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CEngineTest::EresUnittest_BuildMemoWithPartitioning()
{
	Pfpexpr rspqf[] = {
		CTestUtils::PexprLogicalDynamicGet,
		CTestUtils::PexprLogicalSelectWithEqPredicateOverDynamicGet,
		CTestUtils::PexprLogicalSelectWithLTPredicateOverDynamicGet,
		//		CTestUtils::PexprJoinPartitionedOuter<CLogicalInnerJoin>,
		//		CTestUtils::Pexpr3WayJoinPartitioned,
		//		CTestUtils::Pexpr4WayJoinPartitioned
	};

	return EresTestEngine(rspqf, SPQOS_ARRAY_SIZE(rspqf));
}


//---------------------------------------------------------------------------
//	@function:
//		CEngineTest::EresUnittest_BuildMemoWithWindowing
//
//	@doc:
//		Test of building memo for expressions with windowing operations
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CEngineTest::EresUnittest_BuildMemoWithWindowing()
{
	Pfpexpr rspqf[] = {
		CTestUtils::PexprOneWindowFunction,
		CTestUtils::PexprTwoWindowFunctions,
	};

	return EresTestEngine(rspqf, SPQOS_ARRAY_SIZE(rspqf));
}

//---------------------------------------------------------------------------
//	@function:
//		CEngineTest::EresUnittest_BuildMemoWithCTE
//
//	@doc:
//		Test of building memo for expressions with CTEs
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CEngineTest::EresUnittest_BuildMemoWithCTE()
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	// setup a file-based provider
	CMDProviderMemory *pmdp = CTestUtils::m_pmdpf;
	pmdp->AddRef();
	CMDAccessor mda(mp, CMDCache::Pcache());
	mda.RegisterProvider(CTestUtils::m_sysidDefault, pmdp);

	// install opt context in TLS
	CAutoOptCtxt aoc(mp, &mda, NULL, /* pceeval */
					 CTestUtils::GetCostModel(mp));

	CExpression *pexprCTE = CTestUtils::PexprCTETree(mp);
	CExpression *pexprGet = CTestUtils::PexprLogicalGet(mp);

	CColRefSet *pcrsLeft = pexprCTE->DeriveOutputColumns();
	CColRef *pcrLeft = pcrsLeft->PcrAny();

	CColRefSet *pcrsRight = pexprGet->DeriveOutputColumns();
	CColRef *pcrRight = pcrsRight->PcrAny();

	CExpression *pexprScalar = CUtils::PexprScalarEqCmp(mp, pcrLeft, pcrRight);

	CExpression *pexpr =
		SPQOS_NEW(mp) CExpression(mp, SPQOS_NEW(mp) CLogicalInnerJoin(mp),
								 pexprCTE, pexprGet, pexprScalar);

	BuildMemoRecursive(mp, pexpr, NULL /*search_stage_array*/);
	pexpr->Release();

	return SPQOS_OK;
}

#endif	// SPQOS_DEBUG

// EOF
