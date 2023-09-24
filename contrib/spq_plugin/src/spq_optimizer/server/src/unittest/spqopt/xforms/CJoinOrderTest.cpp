//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CJoinOrderTest.cpp
//
//	@doc:
//		Test for join ordering
//---------------------------------------------------------------------------
#include "unittest/spqopt/xforms/CJoinOrderTest.h"

#include "spqos/error/CAutoTrace.h"
#include "spqos/io/COstreamString.h"
#include "spqos/test/CUnittest.h"

#include "spqopt/base/CQueryContext.h"
#include "spqopt/base/CUtils.h"
#include "spqopt/eval/CConstExprEvaluatorDefault.h"
#include "spqopt/operators/CPredicateUtils.h"
#include "spqopt/operators/ops.h"
#include "spqopt/xforms/CJoinOrder.h"
#include "spqopt/xforms/CJoinOrderMinCard.h"

#include "unittest/base.h"
#include "unittest/spqopt/CTestUtils.h"

ULONG CJoinOrderTest::m_ulTestCounter = 0;	// start from first test

// minidump files
const CHAR *rgszJoinOrderFileNames[] = {
	"../data/dxl/minidump/JoinOptimizationLevelGreedyNonPartTblInnerJoin.mdp",
	"../data/dxl/minidump/JoinOptimizationLevelQueryNonPartTblInnerJoin.mdp"};

//---------------------------------------------------------------------------
//	@function:
//		CJoinOrderTest::EresUnittest
//
//	@doc:
//		Unittest for predicate utilities
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CJoinOrderTest::EresUnittest()
{
	CUnittest rgut[] = {SPQOS_UNITTEST_FUNC(EresUnittest_ExpandMinCard),
						SPQOS_UNITTEST_FUNC(EresUnittest_RunTests)};

	return CUnittest::EresExecute(rgut, SPQOS_ARRAY_SIZE(rgut));
}


//---------------------------------------------------------------------------
//	@function:
//		CJoinOrderTest::EresUnittest_ExpandMinCard
//
//	@doc:
//		Expansion expansion based on cardinality of intermediate results
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CJoinOrderTest::EresUnittest_ExpandMinCard()
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	// array of relation names
	CWStringConst rgscRel[] = {
		SPQOS_WSZ_LIT("Rel10"), SPQOS_WSZ_LIT("Rel3"),  SPQOS_WSZ_LIT("Rel4"),
		SPQOS_WSZ_LIT("Rel6"),  SPQOS_WSZ_LIT("Rel7"),  SPQOS_WSZ_LIT("Rel8"),
		SPQOS_WSZ_LIT("Rel12"), SPQOS_WSZ_LIT("Rel13"), SPQOS_WSZ_LIT("Rel5"),
		SPQOS_WSZ_LIT("Rel14"), SPQOS_WSZ_LIT("Rel15"), SPQOS_WSZ_LIT("Rel1"),
		SPQOS_WSZ_LIT("Rel11"), SPQOS_WSZ_LIT("Rel2"),  SPQOS_WSZ_LIT("Rel9"),
	};

	// array of relation IDs
	ULONG rgulRel[] = {
		SPQOPT_TEST_REL_OID10, SPQOPT_TEST_REL_OID3,	SPQOPT_TEST_REL_OID4,
		SPQOPT_TEST_REL_OID6,  SPQOPT_TEST_REL_OID7,	SPQOPT_TEST_REL_OID8,
		SPQOPT_TEST_REL_OID12, SPQOPT_TEST_REL_OID13, SPQOPT_TEST_REL_OID5,
		SPQOPT_TEST_REL_OID14, SPQOPT_TEST_REL_OID15, SPQOPT_TEST_REL_OID1,
		SPQOPT_TEST_REL_OID11, SPQOPT_TEST_REL_OID2,	SPQOPT_TEST_REL_OID9,
	};

	const ULONG ulRels = SPQOS_ARRAY_SIZE(rgscRel);
	SPQOS_ASSERT(SPQOS_ARRAY_SIZE(rgulRel) == ulRels);

	// setup a file-based provider
	CMDProviderMemory *pmdp = CTestUtils::m_pmdpf;
	pmdp->AddRef();
	CMDAccessor mda(mp, CMDCache::Pcache());
	mda.RegisterProvider(CTestUtils::m_sysidDefault, pmdp);

	{
		// install opt context in TLS
		CAutoOptCtxt aoc(mp, &mda, NULL, /* pceeval */
						 CTestUtils::GetCostModel(mp));

		CExpression *pexprNAryJoin = CTestUtils::PexprLogicalNAryJoin(
			mp, rgscRel, rgulRel, ulRels, false /*fCrossProduct*/);

		// derive stats on input expression
		CExpressionHandle exprhdl(mp);
		exprhdl.Attach(pexprNAryJoin);
		exprhdl.DeriveStats(mp, mp, NULL /*prprel*/, NULL /*stats_ctxt*/);

		CExpressionArray *pdrspqexpr = SPQOS_NEW(mp) CExpressionArray(mp);
		for (ULONG ul = 0; ul < ulRels; ul++)
		{
			CExpression *pexprChild = (*pexprNAryJoin)[ul];
			pexprChild->AddRef();
			pdrspqexpr->Append(pexprChild);
		}
		CExpressionArray *pdrspqexprPred =
			CPredicateUtils::PdrspqexprConjuncts(mp, (*pexprNAryJoin)[ulRels]);
		pdrspqexpr->AddRef();
		pdrspqexprPred->AddRef();
		CJoinOrderMinCard jomc(mp, pdrspqexpr, pdrspqexprPred);
		CExpression *pexprResult = jomc.PexprExpand();
		{
			CAutoTrace at(mp);
			at.Os() << std::endl
					<< "INPUT:" << std::endl
					<< *pexprNAryJoin << std::endl;
			at.Os() << std::endl
					<< "OUTPUT:" << std::endl
					<< *pexprResult << std::endl;
		}
		pexprResult->Release();
		pexprNAryJoin->Release();
		pdrspqexpr->Release();
		pdrspqexprPred->Release();
	}

	return SPQOS_OK;
}

//	run all Minidump-based tests with plan matching
SPQOS_RESULT
CJoinOrderTest::EresUnittest_RunTests()
{
	return CTestUtils::EresUnittest_RunTests(
		rgszJoinOrderFileNames, &m_ulTestCounter,
		SPQOS_ARRAY_SIZE(rgszJoinOrderFileNames));
}
// EOF
