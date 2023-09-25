//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CDistributionSpecTest.cpp
//
//	@doc:
//		Tests for distribution specification
//---------------------------------------------------------------------------

#include "unittest/spqopt/base/CDistributionSpecTest.h"

#include "spqos/error/CAutoTrace.h"

#include "spqopt/base/CColRefSet.h"
#include "spqopt/base/CColumnFactory.h"
#include "spqopt/base/CDistributionSpecAny.h"
#include "spqopt/base/CDistributionSpecHashed.h"
#include "spqopt/base/CDistributionSpecRandom.h"
#include "spqopt/base/CDistributionSpecReplicated.h"
#include "spqopt/base/CDistributionSpecSingleton.h"
#include "spqopt/base/CDistributionSpecUniversal.h"
#include "spqopt/eval/CConstExprEvaluatorDefault.h"
#include "spqopt/mdcache/CMDAccessor.h"
#include "spqopt/mdcache/CMDCache.h"
#include "spqopt/metadata/CName.h"
#include "spqopt/operators/CExpressionHandle.h"
#include "naucrates/md/CMDProviderMemory.h"
#include "naucrates/md/IMDTypeInt4.h"

#include "unittest/base.h"
#include "unittest/spqopt/CTestUtils.h"


const CHAR *szMDFileName = "../data/dxl/metadata/md.xml";

//---------------------------------------------------------------------------
//	@function:
//		CDistributionSpecTest::EresUnittest
//
//	@doc:
//		Unittest for distribution spec classes
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CDistributionSpecTest::EresUnittest()
{
	CUnittest rgut[] = {
		SPQOS_UNITTEST_FUNC(CDistributionSpecTest::EresUnittest_Any),
		SPQOS_UNITTEST_FUNC(CDistributionSpecTest::EresUnittest_Singleton),
		SPQOS_UNITTEST_FUNC(CDistributionSpecTest::EresUnittest_Random),
		SPQOS_UNITTEST_FUNC(CDistributionSpecTest::EresUnittest_Replicated),
		SPQOS_UNITTEST_FUNC(CDistributionSpecTest::EresUnittest_Universal),
		SPQOS_UNITTEST_FUNC(CDistributionSpecTest::EresUnittest_Hashed),
#ifdef SPQOS_DEBUG
		SPQOS_UNITTEST_FUNC_ASSERT(
			CDistributionSpecTest::EresUnittest_NegativeAny),
		SPQOS_UNITTEST_FUNC_ASSERT(
			CDistributionSpecTest::EresUnittest_NegativeUniversal),
		SPQOS_UNITTEST_FUNC_ASSERT(
			CDistributionSpecTest::EresUnittest_NegativeRandom),
#endif	// SPQOS_DEBUG
	};

	return CUnittest::EresExecute(rgut, SPQOS_ARRAY_SIZE(rgut));
}

//---------------------------------------------------------------------------
//	@function:
//		CDistributionSpecTest::EresUnittest_Any
//
//	@doc:
//		Test for "any" distribution spec
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CDistributionSpecTest::EresUnittest_Any()
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	// any distribution
	CDistributionSpecAny *pdsany =
		SPQOS_NEW(mp) CDistributionSpecAny(COperator::EopSentinel);

	SPQOS_RTL_ASSERT(pdsany->FSatisfies(pdsany));
	SPQOS_RTL_ASSERT(pdsany->Matches(pdsany));

	CAutoTrace at(mp);
	at.Os() << std::endl;
	at.Os() << *pdsany << std::endl;

	pdsany->Release();

	return SPQOS_OK;
}

//---------------------------------------------------------------------------
//	@function:
//		CDistributionSpecTest::EresUnittest_Random
//
//	@doc:
//		Test for forced random distribution spec
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CDistributionSpecTest::EresUnittest_Random()
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	// setup a file-based provider
	CMDProviderMemory *pmdp = CTestUtils::m_pmdpf;
	pmdp->AddRef();
	CMDAccessor mda(mp, CMDCache::Pcache(), CTestUtils::m_sysidDefault, pmdp);

	// install opt context in TLS
	CAutoOptCtxt aoc(mp, &mda, NULL /*pceeval*/, CTestUtils::GetCostModel(mp));
	COptCtxt *poptctxt = COptCtxt::PoctxtFromTLS();

	// basic tests with random distribution
	poptctxt->MarkDMLQuery(true /*fDMLQuery*/);
	CDistributionSpecRandom *pdsRandomDuplicateSensitive =
		SPQOS_NEW(mp) CDistributionSpecRandom();

	poptctxt->MarkDMLQuery(false /*fDMLQuery*/);
	CDistributionSpecRandom *pdsRandomNonDuplicateSensitive =
		SPQOS_NEW(mp) CDistributionSpecRandom();

	SPQOS_RTL_ASSERT(
		pdsRandomDuplicateSensitive->FSatisfies(pdsRandomDuplicateSensitive));
	SPQOS_RTL_ASSERT(
		pdsRandomDuplicateSensitive->Matches(pdsRandomDuplicateSensitive));
	SPQOS_RTL_ASSERT(pdsRandomDuplicateSensitive->FSatisfies(
		pdsRandomNonDuplicateSensitive));
	SPQOS_RTL_ASSERT(!pdsRandomNonDuplicateSensitive->FSatisfies(
		pdsRandomDuplicateSensitive));

	// random and universal
	CDistributionSpecUniversal *pdsUniversal =
		SPQOS_NEW(mp) CDistributionSpecUniversal();
	SPQOS_RTL_ASSERT(pdsUniversal->FSatisfies(pdsRandomNonDuplicateSensitive));
	SPQOS_RTL_ASSERT(!pdsUniversal->FSatisfies(pdsRandomDuplicateSensitive));

	// random and any
	CDistributionSpecAny *pdsany =
		SPQOS_NEW(mp) CDistributionSpecAny(COperator::EopSentinel);

	SPQOS_RTL_ASSERT(!pdsany->FSatisfies(pdsRandomDuplicateSensitive));
	SPQOS_RTL_ASSERT(pdsRandomDuplicateSensitive->FSatisfies(pdsany));

	CAutoTrace at(mp);
	at.Os() << std::endl;
	at.Os() << *pdsRandomDuplicateSensitive << std::endl;

	pdsRandomDuplicateSensitive->Release();
	pdsRandomNonDuplicateSensitive->Release();
	pdsUniversal->Release();
	pdsany->Release();

	return SPQOS_OK;
}

//---------------------------------------------------------------------------
//	@function:
//		CDistributionSpecTest::EresUnittest_Replicated
//
//	@doc:
//		Test for replicated distribution spec
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CDistributionSpecTest::EresUnittest_Replicated()
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	// setup a file-based provider
	CMDProviderMemory *pmdp = CTestUtils::m_pmdpf;
	pmdp->AddRef();
	CMDAccessor mda(mp, CMDCache::Pcache(), CTestUtils::m_sysidDefault, pmdp);

	// install opt context in TLS
	CAutoOptCtxt aoc(mp, &mda, NULL /*pceeval*/, CTestUtils::GetCostModel(mp));

	// basic tests with replicated distributions
	CDistributionSpecReplicated *pdsreplicated = SPQOS_NEW(mp)
		CDistributionSpecReplicated(CDistributionSpec::EdtStrictReplicated);

	SPQOS_RTL_ASSERT(pdsreplicated->FSatisfies(pdsreplicated));
	SPQOS_RTL_ASSERT(pdsreplicated->Matches(pdsreplicated));

	// replicated and random
	CDistributionSpecRandom *pdsrandom = SPQOS_NEW(mp) CDistributionSpecRandom();

	SPQOS_RTL_ASSERT(!pdsrandom->FSatisfies(pdsreplicated));
	SPQOS_RTL_ASSERT(pdsreplicated->FSatisfies(pdsrandom));

	// replicated and any
	CDistributionSpecAny *pdsany =
		SPQOS_NEW(mp) CDistributionSpecAny(COperator::EopSentinel);

	SPQOS_RTL_ASSERT(!pdsany->FSatisfies(pdsreplicated));
	SPQOS_RTL_ASSERT(pdsreplicated->FSatisfies(pdsany));

	CAutoTrace at(mp);
	at.Os() << std::endl;
	at.Os() << *pdsreplicated << std::endl;

	pdsreplicated->Release();
	pdsrandom->Release();
	pdsany->Release();

	return SPQOS_OK;
}

//---------------------------------------------------------------------------
//	@function:
//		CDistributionSpecTest::EresUnittest_Singleton
//
//	@doc:
//		Test for singleton distribution spec
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CDistributionSpecTest::EresUnittest_Singleton()
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	// setup a file-based provider
	CMDProviderMemory *pmdp = CTestUtils::m_pmdpf;
	pmdp->AddRef();
	CMDAccessor mda(mp, CMDCache::Pcache(), CTestUtils::m_sysidDefault, pmdp);

	// install opt context in TLS
	CAutoOptCtxt aoc(mp, &mda, NULL /*pceeval*/, CTestUtils::GetCostModel(mp));

	// basic tests with singleton distributions
	CDistributionSpecSingleton *pdssSegment = SPQOS_NEW(mp)
		CDistributionSpecSingleton(CDistributionSpecSingleton::EstSegment);
	CDistributionSpecSingleton *pdssMaster = SPQOS_NEW(mp)
		CDistributionSpecSingleton(CDistributionSpecSingleton::EstMaster);

	SPQOS_RTL_ASSERT(pdssMaster->FSatisfies(pdssMaster));
	SPQOS_RTL_ASSERT(pdssMaster->Matches(pdssMaster));

	SPQOS_RTL_ASSERT(pdssSegment->FSatisfies(pdssSegment));
	SPQOS_RTL_ASSERT(pdssSegment->Matches(pdssSegment));

	SPQOS_RTL_ASSERT(!pdssMaster->FSatisfies(pdssSegment) &&
					!pdssSegment->FSatisfies(pdssMaster));

	// singleton and replicated
	CDistributionSpecReplicated *pdsreplicated = SPQOS_NEW(mp)
		CDistributionSpecReplicated(CDistributionSpec::EdtStrictReplicated);

	SPQOS_RTL_ASSERT(pdsreplicated->FSatisfies(pdssSegment));
	SPQOS_RTL_ASSERT(!pdsreplicated->FSatisfies(pdssMaster));

	SPQOS_RTL_ASSERT(!pdssSegment->FSatisfies(pdsreplicated));

	// singleton and random
	CDistributionSpecRandom *pdsrandom = SPQOS_NEW(mp) CDistributionSpecRandom();

	SPQOS_RTL_ASSERT(!pdsrandom->FSatisfies(pdssSegment));
	SPQOS_RTL_ASSERT(!pdsrandom->FSatisfies(pdssMaster));
	SPQOS_RTL_ASSERT(!pdssSegment->FSatisfies(pdsrandom));
	SPQOS_RTL_ASSERT(!pdssMaster->FSatisfies(pdsrandom));

	// singleton and any
	CDistributionSpecAny *pdsany =
		SPQOS_NEW(mp) CDistributionSpecAny(COperator::EopSentinel);

	SPQOS_RTL_ASSERT(!pdsany->FSatisfies(pdssSegment));
	SPQOS_RTL_ASSERT(pdssSegment->FSatisfies(pdsany));

	CAutoTrace at(mp);
	at.Os() << std::endl;
	at.Os() << *pdssMaster << std::endl;
	at.Os() << *pdssSegment << std::endl;

	pdssMaster->Release();
	pdssSegment->Release();
	pdsreplicated->Release();
	pdsrandom->Release();
	pdsany->Release();

	return SPQOS_OK;
}

//---------------------------------------------------------------------------
//	@function:
//		CDistributionSpecTest::EresUnittest_Universal
//
//	@doc:
//		Test for universal distribution spec
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CDistributionSpecTest::EresUnittest_Universal()
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	// setup a file-based provider
	CMDProviderMemory *pmdp = CTestUtils::m_pmdpf;
	pmdp->AddRef();
	CMDAccessor mda(mp, CMDCache::Pcache(), CTestUtils::m_sysidDefault, pmdp);

	// install opt context in TLS
	CAutoOptCtxt aoc(mp, &mda, NULL /*pceeval*/, CTestUtils::GetCostModel(mp));

	// basic tests with universal distributions
	CDistributionSpecUniversal *pdsuniversal =
		SPQOS_NEW(mp) CDistributionSpecUniversal();

	SPQOS_RTL_ASSERT(pdsuniversal->FSatisfies(pdsuniversal));
	SPQOS_RTL_ASSERT(pdsuniversal->Matches(pdsuniversal));

	// universal and singleton
	CDistributionSpecSingleton *pdssSegment = SPQOS_NEW(mp)
		CDistributionSpecSingleton(CDistributionSpecSingleton::EstSegment);

	SPQOS_RTL_ASSERT(pdsuniversal->FSatisfies(pdssSegment));

	// universal and replicated
	CDistributionSpecReplicated *pdsreplicated = SPQOS_NEW(mp)
		CDistributionSpecReplicated(CDistributionSpec::EdtStrictReplicated);

	SPQOS_RTL_ASSERT(pdsuniversal->FSatisfies(pdsreplicated));

	// universal and random
	CDistributionSpecRandom *pdsrandom = SPQOS_NEW(mp) CDistributionSpecRandom();

	SPQOS_RTL_ASSERT(pdsuniversal->FSatisfies(pdsrandom));

	// universal and any
	CDistributionSpecAny *pdsany =
		SPQOS_NEW(mp) CDistributionSpecAny(COperator::EopSentinel);

	SPQOS_RTL_ASSERT(pdsuniversal->FSatisfies(pdsany));

	CAutoTrace at(mp);
	at.Os() << std::endl;
	at.Os() << *pdsuniversal << std::endl;

	pdsuniversal->Release();
	pdssSegment->Release();
	pdsreplicated->Release();
	pdsrandom->Release();
	pdsany->Release();

	return SPQOS_OK;
}

//---------------------------------------------------------------------------
//	@function:
//		CDistributionSpecTest::EresUnittest_Hashed
//
//	@doc:
//		Test for hashed distribution spec
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CDistributionSpecTest::EresUnittest_Hashed()
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	CMDProviderMemory *pmdp = CTestUtils::m_pmdpf;
	pmdp->AddRef();
	CMDAccessor mda(mp, CMDCache::Pcache(), CTestUtils::m_sysidDefault, pmdp);

	// install opt context
	CAutoOptCtxt aoc(mp, &mda, NULL, /* pceeval */
					 CTestUtils::GetCostModel(mp));

	// get column factory from optimizer context object
	COptCtxt *poptctxt = COptCtxt::PoctxtFromTLS();
	CColumnFactory *col_factory = poptctxt->Pcf();

	CWStringConst strA(SPQOS_WSZ_LIT("A"));
	CWStringConst strB(SPQOS_WSZ_LIT("B"));
	CWStringConst strC(SPQOS_WSZ_LIT("C"));
	CWStringConst strD(SPQOS_WSZ_LIT("D"));
	CWStringConst strE(SPQOS_WSZ_LIT("E"));
	CWStringConst strF(SPQOS_WSZ_LIT("F"));
	CWStringConst strG(SPQOS_WSZ_LIT("G"));

	CName nameA(&strA);
	CName nameB(&strB);
	CName nameC(&strC);
	CName nameD(&strD);
	CName nameE(&strE);
	CName nameF(&strF);
	CName nameG(&strG);

	const IMDTypeInt4 *pmdtypeint4 =
		mda.PtMDType<IMDTypeInt4>(CTestUtils::m_sysidDefault);

	CColRef *pcrA =
		col_factory->PcrCreate(pmdtypeint4, default_type_modifier, nameA);
	CColRef *pcrB =
		col_factory->PcrCreate(pmdtypeint4, default_type_modifier, nameB);
	CColRef *pcrC =
		col_factory->PcrCreate(pmdtypeint4, default_type_modifier, nameC);
	CColRef *pcrD =
		col_factory->PcrCreate(pmdtypeint4, default_type_modifier, nameD);
	CColRef *pcrE =
		col_factory->PcrCreate(pmdtypeint4, default_type_modifier, nameE);
	CColRef *pcrF =
		col_factory->PcrCreate(pmdtypeint4, default_type_modifier, nameF);
	CColRef *pcrG =
		col_factory->PcrCreate(pmdtypeint4, default_type_modifier, nameG);

	CExpression *pexprScalarA = CUtils::PexprScalarIdent(mp, pcrA);
	CExpression *pexprScalarB = CUtils::PexprScalarIdent(mp, pcrB);
	CExpression *pexprScalarC = CUtils::PexprScalarIdent(mp, pcrC);
	CExpression *pexprScalarD = CUtils::PexprScalarIdent(mp, pcrD);
	CExpression *pexprScalarE = CUtils::PexprScalarIdent(mp, pcrE);
	CExpression *pexprScalarF = CUtils::PexprScalarIdent(mp, pcrF);
	CExpression *pexprScalarG = CUtils::PexprScalarIdent(mp, pcrG);

	CExpressionArray *prspqexpr1 = SPQOS_NEW(mp) CExpressionArray(mp);
	prspqexpr1->Append(pexprScalarA);
	prspqexpr1->Append(pexprScalarB);

	CExpressionArray *prspqexpr2 = SPQOS_NEW(mp) CExpressionArray(mp);
	pexprScalarA->AddRef();
	prspqexpr2->Append(pexprScalarA);

	CExpressionArray *prspqexpr3 = SPQOS_NEW(mp) CExpressionArray(mp);
	pexprScalarB->AddRef();
	prspqexpr3->Append(pexprScalarB);

	CExpressionArray *prspqexpr4 = SPQOS_NEW(mp) CExpressionArray(mp);
	prspqexpr4->Append(pexprScalarC);

	CExpressionArray *prspqexpr5 = SPQOS_NEW(mp) CExpressionArray(mp);
	prspqexpr5->Append(pexprScalarD);

	CExpressionArray *prspqexpr6 = SPQOS_NEW(mp) CExpressionArray(mp);
	prspqexpr6->Append(pexprScalarE);

	CExpressionArray *prspqexpr7 = SPQOS_NEW(mp) CExpressionArray(mp);
	prspqexpr7->Append(pexprScalarF);

	CExpressionArray *prspqexpr8 = SPQOS_NEW(mp) CExpressionArray(mp);
	prspqexpr8->Append(pexprScalarG);

	// basic hash distribution tests

	// pdshashed1: HD{A,B}, true, duplicate insensitive
	poptctxt->MarkDMLQuery(false /*fDMLQuery*/);
	CDistributionSpecHashed *pdshashed1 = SPQOS_NEW(mp)
		CDistributionSpecHashed(prspqexpr1, true /* fNullsCollocated */);
	SPQOS_RTL_ASSERT(pdshashed1->FSatisfies(pdshashed1));
	SPQOS_RTL_ASSERT(pdshashed1->Matches(pdshashed1));

	// pdshashed2: HD{A}, true, duplicate sensitive
	poptctxt->MarkDMLQuery(true /*fDMLQuery*/);
	CDistributionSpecHashed *pdshashed2 = SPQOS_NEW(mp)
		CDistributionSpecHashed(prspqexpr2, true /* fNullsCollocated */);
	SPQOS_RTL_ASSERT(pdshashed2->FSatisfies(pdshashed2));
	SPQOS_RTL_ASSERT(pdshashed2->Matches(pdshashed2));

	// pdshashed3: HD{A}, false
	prspqexpr2->AddRef();
	CDistributionSpecHashed *pdshashed3 = SPQOS_NEW(mp)
		CDistributionSpecHashed(prspqexpr2, false /* fNullsCollocated */);
	SPQOS_RTL_ASSERT(pdshashed3->FSatisfies(pdshashed3));
	SPQOS_RTL_ASSERT(pdshashed3->Matches(pdshashed3));

	// Test satisfaction
	// ({A,B}, true, true) does not satisfy ({A}, true, false)
	SPQOS_RTL_ASSERT(!pdshashed1->FMatchSubset(pdshashed2));
	SPQOS_RTL_ASSERT(!pdshashed1->FSatisfies(pdshashed2));

	// ({A}, true) satisfies ({A, B}, true)
	SPQOS_RTL_ASSERT(pdshashed2->FMatchSubset(pdshashed1));
	SPQOS_RTL_ASSERT(pdshashed2->FSatisfies(pdshashed1));

	// ({A}, true) satisfies ({A}, false)
	SPQOS_RTL_ASSERT(pdshashed2->FMatchSubset(pdshashed3));
	SPQOS_RTL_ASSERT(pdshashed2->FSatisfies(pdshashed3));

	// ({A}, false) does not satisfy ({A}, true)
	SPQOS_RTL_ASSERT(!pdshashed1->FMatchSubset(pdshashed3));
	SPQOS_RTL_ASSERT(!pdshashed1->FSatisfies(pdshashed3));

	// pdshashed4: HD{B}, true
	poptctxt->MarkDMLQuery(true /*fDMLQuery*/);
	CDistributionSpecHashed *pdshashed4 = SPQOS_NEW(mp)
		CDistributionSpecHashed(prspqexpr3, true /* fNullsCollocated */);

	// pdshashed5: HD{C}, false
	CDistributionSpecHashed *pdshashed5 = SPQOS_NEW(mp)
		CDistributionSpecHashed(prspqexpr4, false /* fNullsCollocated */);

	// pdshashed6: HD{D}, false
	CDistributionSpecHashed *pdshashed6 = SPQOS_NEW(mp)
		CDistributionSpecHashed(prspqexpr5, false /* fNullsCollocated */);

	// Test Combine
	// Combine {A} true, {C} false
	// pdshashed7: {C} false -> {A} true
	CDistributionSpecHashed *pdshashed7 = pdshashed2->Combine(mp, pdshashed5);

	SPQOS_RTL_ASSERT(false == pdshashed7->FNullsColocated());
	SPQOS_RTL_ASSERT(true == pdshashed7->PdshashedEquiv()->FNullsColocated());

	// Combine {D} false, {B} true
	// pdshashed8: {B} true -> {D} false
	CDistributionSpecHashed *pdshashed8 = pdshashed6->Combine(mp, pdshashed4);

	SPQOS_RTL_ASSERT(true == pdshashed8->FNullsColocated());
	SPQOS_RTL_ASSERT(false == pdshashed8->PdshashedEquiv()->FNullsColocated());

	// Combine {C} false -> {A} true, {B} true -> {D} false
	// pdshashed9: {D} false -> {B} true -> {A} true -> {C} false
	CDistributionSpecHashed *pdshashed9 = pdshashed7->Combine(mp, pdshashed8);

	SPQOS_RTL_ASSERT(pdshashed9->FMatchHashedDistribution(pdshashed6));
	SPQOS_RTL_ASSERT(
		pdshashed9->PdshashedEquiv()->FMatchHashedDistribution(pdshashed4));
	SPQOS_RTL_ASSERT(pdshashed9->PdshashedEquiv()
						->PdshashedEquiv()
						->FMatchHashedDistribution(pdshashed2));
	SPQOS_RTL_ASSERT(pdshashed9->PdshashedEquiv()
						->PdshashedEquiv()
						->PdshashedEquiv()
						->FMatchHashedDistribution(pdshashed5));

	// Test Copy
	// Copy {C} false -> {A} true
	// pdshashed10: {C} false -> {A} true
	CDistributionSpecHashed *pdshashed10 = pdshashed7->Copy(mp);

	SPQOS_RTL_ASSERT(false == pdshashed10->FNullsColocated());
	SPQOS_RTL_ASSERT(true == pdshashed10->PdshashedEquiv()->FNullsColocated());

	// Copy {C} false -> {A} true
	// Overwrite nulls colocation -> true
	// pdshashed11: {C} false -> {A} true
	CDistributionSpecHashed *pdshashed11 = pdshashed7->Copy(mp, true);

	SPQOS_RTL_ASSERT(true == pdshashed11->FNullsColocated());
	SPQOS_RTL_ASSERT(true == pdshashed11->PdshashedEquiv()->FNullsColocated());

	// Copy {B} true -> {D} false
	// Overwrite nulls colocation -> false
	// pdshashed12: {B} false -> {D} false
	CDistributionSpecHashed *pdshashed12 = pdshashed8->Copy(mp, false);

	SPQOS_RTL_ASSERT(false == pdshashed12->FNullsColocated());
	SPQOS_RTL_ASSERT(false == pdshashed12->PdshashedEquiv()->FNullsColocated());

	// Test 3+ node Combine
	// This is to ensure Combine doesn't just swap the head and tail node,
	// and it indeed reverses the linked list order
	// pdshashed13: {E}, true
	CDistributionSpecHashed *pdshashed13 = SPQOS_NEW(mp)
		CDistributionSpecHashed(prspqexpr6, true /* fNullsCollocated */);

	// pdshashed14: {F}, true
	CDistributionSpecHashed *pdshashed14 = SPQOS_NEW(mp)
		CDistributionSpecHashed(prspqexpr7, true /* fNullsCollocated */);

	// pdshashed15: {G}, true
	CDistributionSpecHashed *pdshashed15 = SPQOS_NEW(mp)
		CDistributionSpecHashed(prspqexpr8, true /* fNullsCollocated */);

	// Combine {E} true, {F} true
	// pdshashed16: {F} true -> {E} true
	CDistributionSpecHashed *pdshashed16 =
		pdshashed13->Combine(mp, pdshashed14);

	// Combine {G} true, {F} true -> {E} true
	// pdshashed17: {E} true -> {F} true -> {G} true
	CDistributionSpecHashed *pdshashed17 =
		pdshashed15->Combine(mp, pdshashed16);

	// Combine {D} false -> {B} true -> {A} true -> {C} false, {E} true -> {F} true -> {G} true
	// pdshashed18: {G} true -> {F} true -> {E} true -> {C} false -> {A} true -> {B} true -> {D} false
	CDistributionSpecHashed *pdshashed18 = pdshashed9->Combine(mp, pdshashed17);
	SPQOS_RTL_ASSERT(pdshashed18->FMatchHashedDistribution(pdshashed15));
	SPQOS_RTL_ASSERT(
		pdshashed18->PdshashedEquiv()->FMatchHashedDistribution(pdshashed14));
	SPQOS_RTL_ASSERT(pdshashed18->PdshashedEquiv()
						->PdshashedEquiv()
						->FMatchHashedDistribution(pdshashed13));
	SPQOS_RTL_ASSERT(pdshashed18->PdshashedEquiv()
						->PdshashedEquiv()
						->PdshashedEquiv()
						->FMatchHashedDistribution(pdshashed5));
	SPQOS_RTL_ASSERT(pdshashed18->PdshashedEquiv()
						->PdshashedEquiv()
						->PdshashedEquiv()
						->PdshashedEquiv()
						->FMatchHashedDistribution(pdshashed2));
	SPQOS_RTL_ASSERT(pdshashed18->PdshashedEquiv()
						->PdshashedEquiv()
						->PdshashedEquiv()
						->PdshashedEquiv()
						->PdshashedEquiv()
						->FMatchHashedDistribution(pdshashed4));
	SPQOS_RTL_ASSERT(pdshashed18->PdshashedEquiv()
						->PdshashedEquiv()
						->PdshashedEquiv()
						->PdshashedEquiv()
						->PdshashedEquiv()
						->PdshashedEquiv()
						->FMatchHashedDistribution(pdshashed6));

	// Summary
	// pdshashed1: {A, B}, true
	// pdshashed2: {A} true
	// pdshashed3: {A} false
	// pdshashed4: {B} true
	// pdshashed5: {C} false
	// pdshashed6: {D} false
	// pdshashed7: {C} false -> {A} true
	// pdshashed8: {B} true -> {D} false
	// pdshashed9: {D} false -> {B} true -> {A} true -> {C} false
	// pdshashed10: {C} false -> {A} true
	// pdshashed11: {C} false -> {A} true
	// pdshashed12: {B} false -> {D} false
	// pdshashed13: {E} true
	// pdshashed14: {F} true
	// pdshashed15: {G} true
	// pdshashed16: {F} true -> {E} true
	// pdshashed17: {E} true -> {F} true -> {G} true
	// pdshashed18: {G} true -> {F} true -> {E} true -> {C} false -> {A} true -> {B} true -> {D} false

	// hashed and universal
	CDistributionSpecUniversal *pdsuniversal =
		SPQOS_NEW(mp) CDistributionSpecUniversal();

	SPQOS_RTL_ASSERT(pdsuniversal->FSatisfies(pdshashed1));

	// hashed and singleton
	CDistributionSpecSingleton *pdssSegment = SPQOS_NEW(mp)
		CDistributionSpecSingleton(CDistributionSpecSingleton::EstSegment);

	SPQOS_RTL_ASSERT(!pdshashed1->FSatisfies(pdssSegment));
	SPQOS_RTL_ASSERT(!pdssSegment->Matches(pdshashed1));

	// hashed and replicated
	CDistributionSpecReplicated *pdsreplicated = SPQOS_NEW(mp)
		CDistributionSpecReplicated(CDistributionSpec::EdtStrictReplicated);

	SPQOS_RTL_ASSERT(pdsreplicated->FSatisfies(pdshashed1));
	SPQOS_RTL_ASSERT(!pdshashed1->FSatisfies(pdsreplicated));

	// hashed and random
	CDistributionSpecRandom *pdsrandom = SPQOS_NEW(mp) CDistributionSpecRandom();

	SPQOS_RTL_ASSERT(!pdsrandom->FSatisfies(pdshashed1));
	SPQOS_RTL_ASSERT(!pdshashed1->FSatisfies(pdsrandom));

	// hashed and any
	CDistributionSpecAny *pdsany =
		SPQOS_NEW(mp) CDistributionSpecAny(COperator::EopSentinel);

	SPQOS_RTL_ASSERT(!pdsany->FSatisfies(pdshashed1));
	SPQOS_RTL_ASSERT(pdshashed1->FSatisfies(pdsany));

	CAutoTrace at(mp);
	at.Os() << std::endl;
	at.Os() << *pdshashed1 << std::endl;
	at.Os() << *pdshashed2 << std::endl;
	at.Os() << *pdshashed3 << std::endl;
	at.Os() << *pdshashed4 << std::endl;
	at.Os() << *pdshashed5 << std::endl;
	at.Os() << *pdshashed6 << std::endl;
	at.Os() << *pdshashed7 << std::endl;
	at.Os() << *pdshashed8 << std::endl;
	at.Os() << *pdshashed9 << std::endl;
	at.Os() << *pdshashed10 << std::endl;
	at.Os() << *pdshashed11 << std::endl;
	at.Os() << *pdshashed12 << std::endl;
	at.Os() << *pdshashed13 << std::endl;
	at.Os() << *pdshashed14 << std::endl;
	at.Os() << *pdshashed15 << std::endl;
	at.Os() << *pdshashed16 << std::endl;
	at.Os() << *pdshashed17 << std::endl;
	at.Os() << *pdshashed18 << std::endl;

	pdshashed1->Release();
	pdshashed2->Release();
	pdshashed3->Release();
	pdshashed4->Release();
	pdshashed5->Release();
	pdshashed6->Release();
	pdshashed7->Release();
	pdshashed8->Release();
	pdshashed9->Release();
	pdshashed10->Release();
	pdshashed11->Release();
	pdshashed12->Release();
	pdshashed13->Release();
	pdshashed14->Release();
	pdshashed15->Release();
	pdshashed16->Release();
	pdshashed17->Release();
	pdshashed18->Release();

	pdssSegment->Release();
	pdsreplicated->Release();
	pdsrandom->Release();
	pdsany->Release();
	pdsuniversal->Release();

	return SPQOS_OK;
}

#ifdef SPQOS_DEBUG
//---------------------------------------------------------------------------
//	@function:
//		CDistributionSpecTest::EresUnittest_NegativeAny
//
//	@doc:
//		Negative test for ANY distribution spec
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CDistributionSpecTest::EresUnittest_NegativeAny()
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	// cannot add enforcers for ANY distribution
	CDistributionSpecAny *pdsany =
		SPQOS_NEW(mp) CDistributionSpecAny(COperator::EopSentinel);
	CExpressionHandle *pexprhdl = SPQOS_NEW(mp) CExpressionHandle(mp);
	pdsany->AppendEnforcers(NULL /*mp*/, *pexprhdl, NULL /*prpp*/,
							NULL /*pdrspqexpr*/, NULL /*pexpr*/);
	pdsany->Release();
	SPQOS_DELETE(pexprhdl);

	return SPQOS_FAILED;
}


//---------------------------------------------------------------------------
//	@function:
//		CDistributionSpecTest::EresUnittest_NegativeUniversal
//
//	@doc:
//		Negative test for universal distribution spec
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CDistributionSpecTest::EresUnittest_NegativeUniversal()
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	// cannot add enforcers for Universal distribution
	CDistributionSpecUniversal *pdsuniversal =
		SPQOS_NEW(mp) CDistributionSpecUniversal();
	CExpressionHandle *pexprhdl = SPQOS_NEW(mp) CExpressionHandle(mp);

	pdsuniversal->AppendEnforcers(NULL /*mp*/, *pexprhdl, NULL /*prpp*/,
								  NULL /*pdrspqexpr*/, NULL /*pexpr*/);
	pdsuniversal->Release();
	SPQOS_DELETE(pexprhdl);

	return SPQOS_FAILED;
}

//---------------------------------------------------------------------------
//	@function:
//		CDistributionSpecTest::EresUnittest_NegativeRandom
//
//	@doc:
//		Negative test for random distribution spec
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CDistributionSpecTest::EresUnittest_NegativeRandom()
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	// setup a file-based provider
	CMDProviderMemory *pmdp = CTestUtils::m_pmdpf;
	pmdp->AddRef();
	CMDAccessor mda(mp, CMDCache::Pcache(), CTestUtils::m_sysidDefault, pmdp);

	// install opt context in TLS
	CAutoOptCtxt aoc(mp, &mda, NULL /*pceeval*/, CTestUtils::GetCostModel(mp));

	// cannot add enforcers for Random distribution
	CDistributionSpecRandom *pdsrandom = SPQOS_NEW(mp) CDistributionSpecRandom();
	CExpressionHandle *pexprhdl = SPQOS_NEW(mp) CExpressionHandle(mp);

	pdsrandom->AppendEnforcers(NULL /*mp*/, *pexprhdl, NULL /*prpp*/,
							   NULL /*pdrspqexpr*/, NULL /*pexpr*/);
	pdsrandom->Release();
	SPQOS_DELETE(pexprhdl);

	return SPQOS_FAILED;
}
#endif	// SPQOS_DEBUG

// EOF
