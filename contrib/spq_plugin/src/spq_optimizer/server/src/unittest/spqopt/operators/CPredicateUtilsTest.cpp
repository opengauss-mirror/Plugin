//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CPredicateUtilsTest.cpp
//
//	@doc:
//		Test for predicate utilities
//---------------------------------------------------------------------------
#include "unittest/spqopt/operators/CPredicateUtilsTest.h"

#include "spqos/error/CAutoTrace.h"

#include "spqopt/base/CColRefSetIter.h"
#include "spqopt/base/CUtils.h"
#include "spqopt/eval/CConstExprEvaluatorDefault.h"
#include "spqopt/operators/CExpressionPreprocessor.h"
#include "spqopt/operators/CLogicalInnerJoin.h"
#include "spqopt/operators/CPredicateUtils.h"
#include "naucrates/md/CMDIdSPQDB.h"

#include "unittest/base.h"
#include "unittest/spqopt/CTestUtils.h"


//---------------------------------------------------------------------------
//	@function:
//		CPredicateUtilsTest::EresUnittest
//
//	@doc:
//		Unittest for predicate utilities
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CPredicateUtilsTest::EresUnittest()
{
	CUnittest rgut[] = {
		SPQOS_UNITTEST_FUNC(CPredicateUtilsTest::EresUnittest_Conjunctions),
		SPQOS_UNITTEST_FUNC(CPredicateUtilsTest::EresUnittest_Disjunctions),
		SPQOS_UNITTEST_FUNC(CPredicateUtilsTest::EresUnittest_PlainEqualities),
		SPQOS_UNITTEST_FUNC(CPredicateUtilsTest::EresUnittest_Implication),
	};

	return CUnittest::EresExecute(rgut, SPQOS_ARRAY_SIZE(rgut));
}


//---------------------------------------------------------------------------
//	@function:
//		CPredicateUtilsTest::EresUnittest_Conjunctions
//
//	@doc:
//		Test extraction and construction of conjuncts
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CPredicateUtilsTest::EresUnittest_Conjunctions()
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	// setup a file-based provider
	CMDProviderMemory *pmdp = CTestUtils::m_pmdpf;
	pmdp->AddRef();
	CMDAccessor mda(mp, CMDCache::Pcache(), CTestUtils::m_sysidDefault, pmdp);

	// install opt context in TLS
	CAutoOptCtxt aoc(mp, &mda, NULL, /* pceeval */
					 CTestUtils::GetCostModel(mp));

	// build conjunction
	CExpressionArray *pdrspqexpr = SPQOS_NEW(mp) CExpressionArray(mp);
	const ULONG ulConjs = 3;
	for (ULONG ul = 0; ul < ulConjs; ul++)
	{
		pdrspqexpr->Append(CUtils::PexprScalarConstBool(mp, true /*fValue*/));
	}
	CExpression *pexprConjunction =
		CUtils::PexprScalarBoolOp(mp, CScalarBoolOp::EboolopAnd, pdrspqexpr);

	// break into conjuncts
	CExpressionArray *pdrspqexprExtract =
		CPredicateUtils::PdrspqexprConjuncts(mp, pexprConjunction);
	SPQOS_ASSERT(pdrspqexprExtract->Size() == ulConjs);

	// collapse into single conjunct
	CExpression *pexpr =
		CPredicateUtils::PexprConjunction(mp, pdrspqexprExtract);
	SPQOS_ASSERT(NULL != pexpr);
	SPQOS_ASSERT(CUtils::FScalarConstTrue(pexpr));
	pexpr->Release();

	// collapse empty input array to conjunct
	CExpression *pexprSingleton =
		CPredicateUtils::PexprConjunction(mp, NULL /*pdrspqexpr*/);
	SPQOS_ASSERT(NULL != pexprSingleton);
	pexprSingleton->Release();

	pexprConjunction->Release();

	// conjunction on scalar comparisons
	CExpression *pexprGet = CTestUtils::PexprLogicalGet(mp);
	CColRefSet *pcrs = pexprGet->DeriveOutputColumns();
	CColRef *pcr1 = pcrs->PcrAny();
	CColRef *pcr2 = pcrs->PcrFirst();
	CExpression *pexprCmp1 =
		CUtils::PexprScalarCmp(mp, pcr1, pcr2, IMDType::EcmptEq);
	CExpression *pexprCmp2 = CUtils::PexprScalarCmp(
		mp, pcr1, CUtils::PexprScalarConstInt4(mp, 1 /*val*/),
		IMDType::EcmptEq);

	CExpression *pexprConj =
		CPredicateUtils::PexprConjunction(mp, pexprCmp1, pexprCmp2);
	pdrspqexprExtract = CPredicateUtils::PdrspqexprConjuncts(mp, pexprConj);
	SPQOS_ASSERT(2 == pdrspqexprExtract->Size());
	pdrspqexprExtract->Release();

	pexprCmp1->Release();
	pexprCmp2->Release();
	pexprConj->Release();
	pexprGet->Release();

	return SPQOS_OK;
}


//---------------------------------------------------------------------------
//	@function:
//		CPredicateUtilsTest::EresUnittest_Disjunctions
//
//	@doc:
//		Test extraction and construction of disjuncts
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CPredicateUtilsTest::EresUnittest_Disjunctions()
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	// setup a file-based provider
	CMDProviderMemory *pmdp = CTestUtils::m_pmdpf;
	pmdp->AddRef();
	CMDAccessor mda(mp, CMDCache::Pcache(), CTestUtils::m_sysidDefault, pmdp);

	// install opt context in TLS
	CAutoOptCtxt aoc(mp, &mda, NULL, /* pceeval */
					 CTestUtils::GetCostModel(mp));

	// build disjunction
	CExpressionArray *pdrspqexpr = SPQOS_NEW(mp) CExpressionArray(mp);
	const ULONG ulDisjs = 3;
	for (ULONG ul = 0; ul < ulDisjs; ul++)
	{
		pdrspqexpr->Append(CUtils::PexprScalarConstBool(mp, false /*fValue*/));
	}
	CExpression *pexprDisjunction =
		CUtils::PexprScalarBoolOp(mp, CScalarBoolOp::EboolopOr, pdrspqexpr);

	// break into disjuncts
	CExpressionArray *pdrspqexprExtract =
		CPredicateUtils::PdrspqexprDisjuncts(mp, pexprDisjunction);
	SPQOS_ASSERT(pdrspqexprExtract->Size() == ulDisjs);

	// collapse into single disjunct
	CExpression *pexpr =
		CPredicateUtils::PexprDisjunction(mp, pdrspqexprExtract);
	SPQOS_ASSERT(NULL != pexpr);
	SPQOS_ASSERT(CUtils::FScalarConstFalse(pexpr));
	pexpr->Release();

	// collapse empty input array to disjunct
	CExpression *pexprSingleton =
		CPredicateUtils::PexprDisjunction(mp, NULL /*pdrspqexpr*/);
	SPQOS_ASSERT(NULL != pexprSingleton);
	pexprSingleton->Release();

	pexprDisjunction->Release();

	// disjunction on scalar comparisons
	CExpression *pexprGet = CTestUtils::PexprLogicalGet(mp);
	CColRefSet *pcrs = pexprGet->DeriveOutputColumns();
	CColRefSetIter crsi(*pcrs);

#ifdef SPQOS_DEBUG
	BOOL fAdvance =
#endif
		crsi.Advance();
	SPQOS_ASSERT(fAdvance);
	CColRef *pcr1 = crsi.Pcr();

#ifdef SPQOS_DEBUG
	fAdvance =
#endif
		crsi.Advance();
	SPQOS_ASSERT(fAdvance);
	CColRef *pcr2 = crsi.Pcr();

#ifdef SPQOS_DEBUG
	fAdvance =
#endif
		crsi.Advance();
	SPQOS_ASSERT(fAdvance);
	CColRef *pcr3 = crsi.Pcr();

	CExpression *pexprCmp1 =
		CUtils::PexprScalarCmp(mp, pcr1, pcr2, IMDType::EcmptEq);
	CExpression *pexprCmp2 = CUtils::PexprScalarCmp(
		mp, pcr1, CUtils::PexprScalarConstInt4(mp, 1 /*val*/),
		IMDType::EcmptEq);

	{
		CExpression *pexprDisj =
			CPredicateUtils::PexprDisjunction(mp, pexprCmp1, pexprCmp2);
		pdrspqexprExtract = CPredicateUtils::PdrspqexprDisjuncts(mp, pexprDisj);
		SPQOS_ASSERT(2 == pdrspqexprExtract->Size());
		pdrspqexprExtract->Release();
		pexprDisj->Release();
	}


	{
		CExpressionArray *pdrspqexpr = SPQOS_NEW(mp) CExpressionArray(mp);
		CExpression *pexprCmp3 =
			CUtils::PexprScalarCmp(mp, pcr2, pcr1, IMDType::EcmptG);
		CExpression *pexprCmp4 = CUtils::PexprScalarCmp(
			mp, CUtils::PexprScalarConstInt4(mp, 200 /*val*/), pcr3,
			IMDType::EcmptL);
		pexprCmp1->AddRef();
		pexprCmp2->AddRef();

		pdrspqexpr->Append(pexprCmp3);
		pdrspqexpr->Append(pexprCmp4);
		pdrspqexpr->Append(pexprCmp1);
		pdrspqexpr->Append(pexprCmp2);

		CExpression *pexprDisj =
			CPredicateUtils::PexprDisjunction(mp, pdrspqexpr);
		pdrspqexprExtract = CPredicateUtils::PdrspqexprDisjuncts(mp, pexprDisj);
		SPQOS_ASSERT(4 == pdrspqexprExtract->Size());
		pdrspqexprExtract->Release();
		pexprDisj->Release();
	}

	pexprCmp1->Release();
	pexprCmp2->Release();
	pexprGet->Release();

	return SPQOS_OK;
}


//---------------------------------------------------------------------------
//	@function:
//		CPredicateUtilsTest::EresUnittest_PlainEqualities
//
//	@doc:
//		Test the extraction of equality predicates between scalar identifiers
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CPredicateUtilsTest::EresUnittest_PlainEqualities()
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	// setup a file-based provider
	CMDProviderMemory *pmdp = CTestUtils::m_pmdpf;
	pmdp->AddRef();
	CMDAccessor mda(mp, CMDCache::Pcache(), CTestUtils::m_sysidDefault, pmdp);

	// install opt context in TLS
	CAutoOptCtxt aoc(mp, &mda, NULL, /* pceeval */
					 CTestUtils::GetCostModel(mp));

	CExpression *pexprLeft = CTestUtils::PexprLogicalGet(mp);
	CExpression *pexprRight = CTestUtils::PexprLogicalGet(mp);

	CExpressionArray *pdrspqexprOriginal = SPQOS_NEW(mp) CExpressionArray(mp);

	CColRefSet *pcrsLeft = pexprLeft->DeriveOutputColumns();
	CColRefSet *pcrsRight = pexprRight->DeriveOutputColumns();

	CColRef *pcrLeft = pcrsLeft->PcrAny();
	CColRef *pcrRight = pcrsRight->PcrAny();

	// generate an equality predicate between two column reference
	CExpression *pexprScIdentEquality =
		CUtils::PexprScalarEqCmp(mp, pcrLeft, pcrRight);

	pexprScIdentEquality->AddRef();
	pdrspqexprOriginal->Append(pexprScIdentEquality);

	// generate a non-equality predicate between two column reference
	CExpression *pexprScIdentInequality = CUtils::PexprScalarCmp(
		mp, pcrLeft, pcrRight, CWStringConst(SPQOS_WSZ_LIT("<")),
		SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_INT4_LT_OP));

	pexprScIdentInequality->AddRef();
	pdrspqexprOriginal->Append(pexprScIdentInequality);

	// generate an equality predicate between a column reference and a constant value
	CExpression *pexprScalarConstInt4 =
		CUtils::PexprScalarConstInt4(mp, 10 /*fValue*/);
	CExpression *pexprScIdentConstEquality =
		CUtils::PexprScalarEqCmp(mp, pexprScalarConstInt4, pcrRight);

	pdrspqexprOriginal->Append(pexprScIdentConstEquality);

	SPQOS_ASSERT(3 == pdrspqexprOriginal->Size());

	CExpressionArray *pdrspqexprResult =
		CPredicateUtils::PdrspqexprPlainEqualities(mp, pdrspqexprOriginal);

	SPQOS_ASSERT(1 == pdrspqexprResult->Size());

	// clean up
	pdrspqexprOriginal->Release();
	pdrspqexprResult->Release();
	pexprLeft->Release();
	pexprRight->Release();
	pexprScIdentEquality->Release();
	pexprScIdentInequality->Release();

	return SPQOS_OK;
}

//---------------------------------------------------------------------------
//	@function:
//		CPredicateUtilsTest::EresUnittest_Implication
//
//	@doc:
//		Test removal of implied predicates
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CPredicateUtilsTest::EresUnittest_Implication()
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	// setup a file-based provider
	CMDProviderMemory *pmdp = CTestUtils::m_pmdpf;
	pmdp->AddRef();
	CMDAccessor mda(mp, CMDCache::Pcache(), CTestUtils::m_sysidDefault, pmdp);

	// install opt context in TLS
	CAutoOptCtxt aoc(mp, &mda, NULL, /* pceeval */
					 CTestUtils::GetCostModel(mp));

	// generate a two cascaded joins
	CWStringConst strName1(SPQOS_WSZ_LIT("Rel1"));
	CMDIdSPQDB *pmdid1 =
		SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidRel, SPQOPT_TEST_REL_OID1, 1, 1);
	CTableDescriptor *ptabdesc1 =
		CTestUtils::PtabdescCreate(mp, 3, pmdid1, CName(&strName1));
	CWStringConst strAlias1(SPQOS_WSZ_LIT("Rel1"));
	CExpression *pexprRel1 =
		CTestUtils::PexprLogicalGet(mp, ptabdesc1, &strAlias1);

	CWStringConst strName2(SPQOS_WSZ_LIT("Rel2"));
	CMDIdSPQDB *pmdid2 =
		SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidRel, SPQOPT_TEST_REL_OID2, 1, 1);
	CTableDescriptor *ptabdesc2 =
		CTestUtils::PtabdescCreate(mp, 3, pmdid2, CName(&strName2));
	CWStringConst strAlias2(SPQOS_WSZ_LIT("Rel2"));
	CExpression *pexprRel2 =
		CTestUtils::PexprLogicalGet(mp, ptabdesc2, &strAlias2);

	CWStringConst strName3(SPQOS_WSZ_LIT("Rel3"));
	CMDIdSPQDB *pmdid3 =
		SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidRel, SPQOPT_TEST_REL_OID3, 1, 1);
	CTableDescriptor *ptabdesc3 =
		CTestUtils::PtabdescCreate(mp, 3, pmdid3, CName(&strName3));
	CWStringConst strAlias3(SPQOS_WSZ_LIT("Rel3"));
	CExpression *pexprRel3 =
		CTestUtils::PexprLogicalGet(mp, ptabdesc3, &strAlias3);

	CExpression *pexprJoin1 = CTestUtils::PexprLogicalJoin<CLogicalInnerJoin>(
		mp, pexprRel1, pexprRel2);
	CExpression *pexprJoin2 = CTestUtils::PexprLogicalJoin<CLogicalInnerJoin>(
		mp, pexprJoin1, pexprRel3);

	{
		CAutoTrace at(mp);
		at.Os() << "Original expression:" << std::endl
				<< *pexprJoin2 << std::endl;
	}

	// imply new predicates by deriving constraints
	CExpression *pexprConstraints =
		CExpressionPreprocessor::PexprAddPredicatesFromConstraints(mp,
																   pexprJoin2);

	{
		CAutoTrace at(mp);
		at.Os() << "Expression with implied predicates:" << std::endl
				<< *pexprConstraints << std::endl;
		;
	}

	// minimize join predicates by removing implied conjuncts
	CExpressionHandle exprhdl(mp);
	exprhdl.Attach(pexprConstraints);
	CExpression *pexprMinimizedPred =
		CPredicateUtils::PexprRemoveImpliedConjuncts(mp, (*pexprConstraints)[2],
													 exprhdl);

	{
		CAutoTrace at(mp);
		at.Os() << "Minimized join predicate:" << std::endl
				<< *pexprMinimizedPred << std::endl;
	}

	CExpressionArray *pdrspqexprOriginalConjuncts =
		CPredicateUtils::PdrspqexprConjuncts(mp, (*pexprConstraints)[2]);
	CExpressionArray *pdrspqexprNewConjuncts =
		CPredicateUtils::PdrspqexprConjuncts(mp, pexprMinimizedPred);

	SPQOS_ASSERT(pdrspqexprNewConjuncts->Size() <
				pdrspqexprOriginalConjuncts->Size());

	// clean up
	pdrspqexprOriginalConjuncts->Release();
	pdrspqexprNewConjuncts->Release();
	pexprJoin2->Release();
	pexprConstraints->Release();
	pexprMinimizedPred->Release();

	return SPQOS_OK;
}
// EOF
