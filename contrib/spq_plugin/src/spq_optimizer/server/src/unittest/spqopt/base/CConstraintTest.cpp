//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CConstraintTest.cpp
//
//	@doc:
//		Test for constraint
//---------------------------------------------------------------------------

#ifndef __STDC_CONSTANT_MACROS
#define __STDC_CONSTANT_MACROS
#endif

#include "unittest/spqopt/base/CConstraintTest.h"

#include <stdint.h>

#include "spqos/task/CAutoTraceFlag.h"

#include "spqopt/base/CDefaultComparator.h"
#include "spqopt/base/CUtils.h"
#include "spqopt/eval/CConstExprEvaluatorDefault.h"
#include "spqopt/operators/CPredicateUtils.h"
#include "naucrates/base/CDatumInt8SPQDB.h"
#include "naucrates/md/CMDIdSPQDB.h"
#include "naucrates/md/IMDScalarOp.h"

#include "unittest/base.h"
#include "unittest/spqopt/CConstExprEvaluatorForDates.h"

// number of microseconds in one day
const LINT CConstraintTest::lMicrosecondsPerDay =
	24 * 60 * 60 * INT64_C(1000000);

// date for '01-01-2012'
const LINT CConstraintTest::lInternalRepresentationFor2012_01_01 =
	LINT(4383) * CConstraintTest::lMicrosecondsPerDay;

// date for '01-21-2012'
const LINT CConstraintTest::lInternalRepresentationFor2012_01_21 =
	LINT(5003) * CConstraintTest::lMicrosecondsPerDay;

// date for '01-02-2012'
const LINT CConstraintTest::lInternalRepresentationFor2012_01_02 =
	LINT(4384) * CConstraintTest::lMicrosecondsPerDay;

// date for '01-22-2012'
const LINT CConstraintTest::lInternalRepresentationFor2012_01_22 =
	LINT(5004) * CConstraintTest::lMicrosecondsPerDay;

// byte representation for '01-01-2012'
const WCHAR *CConstraintTest::wszInternalRepresentationFor2012_01_01 =
	SPQOS_WSZ_LIT("HxEAAA==");

// byte representation for '01-21-2012'
const WCHAR *CConstraintTest::wszInternalRepresentationFor2012_01_21 =
	SPQOS_WSZ_LIT("MxEAAA==");

// byte representation for '01-02-2012'
const WCHAR *CConstraintTest::wszInternalRepresentationFor2012_01_02 =
	SPQOS_WSZ_LIT("IBEAAA==");

// byte representation for '01-22-2012'
const WCHAR *CConstraintTest::wszInternalRepresentationFor2012_01_22 =
	SPQOS_WSZ_LIT("MhEAAA==");

static SPQOS_RESULT EresUnittest_CConstraintIntervalFromArrayExprIncludesNull();

//---------------------------------------------------------------------------
//	@function:
//		CConstraintTest::EresUnittest
//
//	@doc:
//		Unittest for ranges
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CConstraintTest::EresUnittest()
{
	CUnittest rgut[] = {
		SPQOS_UNITTEST_FUNC(
			EresUnittest_CConstraintIntervalFromArrayExprIncludesNull),
		SPQOS_UNITTEST_FUNC(CConstraintTest::EresUnittest_CInterval),
		SPQOS_UNITTEST_FUNC(
			CConstraintTest::EresUnittest_CIntervalFromScalarExpr),
		SPQOS_UNITTEST_FUNC(CConstraintTest::EresUnittest_CConjunction),
		SPQOS_UNITTEST_FUNC(CConstraintTest::EresUnittest_CDisjunction),
		SPQOS_UNITTEST_FUNC(CConstraintTest::EresUnittest_CNegation),
		SPQOS_UNITTEST_FUNC(
			CConstraintTest::EresUnittest_CConstraintFromScalarExpr),
		SPQOS_UNITTEST_FUNC(
			CConstraintTest::EresUnittest_CConstraintIntervalConvertsTo),
		SPQOS_UNITTEST_FUNC(
			CConstraintTest::EresUnittest_CConstraintIntervalPexpr),
		SPQOS_UNITTEST_FUNC(
			CConstraintTest::EresUnittest_CConstraintIntervalFromArrayExpr),
#ifdef SPQOS_DEBUG
		SPQOS_UNITTEST_FUNC_THROW(CConstraintTest::EresUnittest_NegativeTests,
								 spqos::CException::ExmaSystem,
								 spqos::CException::ExmiAssert),
#endif	// SPQOS_DEBUG
		SPQOS_UNITTEST_FUNC(CConstraintTest::EresUnittest_ConstraintsOnDates),
	};

	return CUnittest::EresExecute(rgut, SPQOS_ARRAY_SIZE(rgut));
}

//---------------------------------------------------------------------------
//	@function:
//		CConstraintTest::EresUnittest_CInterval
//
//	@doc:
//		Interval tests
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CConstraintTest::EresUnittest_CInterval()
{
	// create memory pool
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	// setup a file-based provider
	CMDProviderMemory *pmdp = CTestUtils::m_pmdpf;
	pmdp->AddRef();
	CMDAccessor mda(mp, CMDCache::Pcache(), CTestUtils::m_sysidDefault, pmdp);

	CConstExprEvaluatorForDates *pceeval =
		SPQOS_NEW(mp) CConstExprEvaluatorForDates(mp);

	// install opt context in TLS
	CAutoOptCtxt aoc(mp, &mda, pceeval, CTestUtils::GetCostModel(mp));
	SPQOS_ASSERT(NULL != COptCtxt::PoctxtFromTLS()->Pcomp());

	IMDTypeInt8 *pmdtypeint8 =
		(IMDTypeInt8 *) mda.PtMDType<IMDTypeInt8>(CTestUtils::m_sysidDefault);
	IMDId *mdid = pmdtypeint8->MDId();

	CExpression *pexprGet = CTestUtils::PexprLogicalGet(mp);
	CColRefSet *pcrs = pexprGet->DeriveOutputColumns();
	CColRef *colref = pcrs->PcrAny();

	// first interval
	CConstraintInterval *pciFirst = PciFirstInterval(mp, mdid, colref);
	PrintConstraint(mp, pciFirst);

	// second interval
	CConstraintInterval *pciSecond = PciSecondInterval(mp, mdid, colref);
	PrintConstraint(mp, pciSecond);

	// intersection
	CConstraintInterval *pciIntersect = pciFirst->PciIntersect(mp, pciSecond);
	PrintConstraint(mp, pciIntersect);

	// union
	CConstraintInterval *pciUnion = pciFirst->PciUnion(mp, pciSecond);
	PrintConstraint(mp, pciUnion);

	// diff 1
	CConstraintInterval *pciDiff1 = pciFirst->PciDifference(mp, pciSecond);
	PrintConstraint(mp, pciDiff1);

	// diff 2
	CConstraintInterval *pciDiff2 = pciSecond->PciDifference(mp, pciFirst);
	PrintConstraint(mp, pciDiff2);

	// complement
	CConstraintInterval *pciComp = pciFirst->PciComplement(mp);
	PrintConstraint(mp, pciComp);

	// containment
	SPQOS_ASSERT(!pciFirst->Contains(pciSecond));
	SPQOS_ASSERT(pciFirst->Contains(pciDiff1));
	SPQOS_ASSERT(!pciSecond->Contains(pciFirst));
	SPQOS_ASSERT(pciSecond->Contains(pciDiff2));

	// equality
	CConstraintInterval *pciThird = PciFirstInterval(mp, mdid, colref);
	pciThird->AddRef();
	CConstraintInterval *pciFourth = pciThird;
	SPQOS_ASSERT(!pciFirst->Equals(pciSecond));
	SPQOS_ASSERT(!pciFirst->Equals(pciDiff1));
	SPQOS_ASSERT(!pciSecond->Equals(pciDiff2));
	SPQOS_ASSERT(pciFirst->Equals(pciThird));
	SPQOS_ASSERT(pciFourth->Equals(pciThird));

	pciFirst->Release();
	pciSecond->Release();
	pciThird->Release();
	pciFourth->Release();
	pciIntersect->Release();
	pciUnion->Release();
	pciDiff1->Release();
	pciDiff2->Release();
	pciComp->Release();

	pexprGet->Release();

	return SPQOS_OK;
}

//---------------------------------------------------------------------------
//	@function:
//		CConstraintTest::EresUnittest_CConjunction
//
//	@doc:
//		Conjunction test
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CConstraintTest::EresUnittest_CConjunction()
{
	// create memory pool
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	// setup a file-based provider
	CMDProviderMemory *pmdp = CTestUtils::m_pmdpf;
	pmdp->AddRef();
	CMDAccessor mda(mp, CMDCache::Pcache(), CTestUtils::m_sysidDefault, pmdp);

	CConstExprEvaluatorForDates *pceeval =
		SPQOS_NEW(mp) CConstExprEvaluatorForDates(mp);

	// install opt context in TLS
	CAutoOptCtxt aoc(mp, &mda, pceeval, CTestUtils::GetCostModel(mp));
	SPQOS_ASSERT(NULL != COptCtxt::PoctxtFromTLS()->Pcomp());

	IMDTypeInt8 *pmdtypeint8 =
		(IMDTypeInt8 *) mda.PtMDType<IMDTypeInt8>(CTestUtils::m_sysidDefault);
	IMDId *mdid = pmdtypeint8->MDId();

	CExpression *pexprGet1 = CTestUtils::PexprLogicalGet(mp);
	CColRefSet *pcrs1 = pexprGet1->DeriveOutputColumns();
	CColRef *pcr1 = pcrs1->PcrAny();

	CExpression *pexprGet2 = CTestUtils::PexprLogicalGet(mp);
	CColRefSet *pcrs2 = pexprGet2->DeriveOutputColumns();
	CColRef *pcr2 = pcrs2->PcrAny();

	CConstraintConjunction *pcconj1 = Pcstconjunction(mp, mdid, pcr1);
	PrintConstraint(mp, pcconj1);
	SPQOS_ASSERT(!pcconj1->FContradiction());

	CConstraintConjunction *pcconj2 = Pcstconjunction(mp, mdid, pcr2);
	PrintConstraint(mp, pcconj2);

	CConstraintArray *pdrspqcst = SPQOS_NEW(mp) CConstraintArray(mp);
	pcconj1->AddRef();
	pcconj2->AddRef();
	pdrspqcst->Append(pcconj1);
	pdrspqcst->Append(pcconj2);

	CConstraintConjunction *pcconjTop =
		SPQOS_NEW(mp) CConstraintConjunction(mp, pdrspqcst);
	PrintConstraint(mp, pcconjTop);

	// containment
	SPQOS_ASSERT(!pcconj1->Contains(pcconj2));
	SPQOS_ASSERT(!pcconj2->Contains(pcconj1));
	SPQOS_ASSERT(pcconj1->Contains(pcconjTop));
	SPQOS_ASSERT(!pcconjTop->Contains(pcconj1));
	SPQOS_ASSERT(pcconj2->Contains(pcconjTop));
	SPQOS_ASSERT(!pcconjTop->Contains(pcconj2));

	// equality
	SPQOS_ASSERT(!pcconj1->Equals(pcconjTop));
	SPQOS_ASSERT(!pcconjTop->Equals(pcconj2));

	pcconjTop->Release();
	pcconj1->Release();
	pcconj2->Release();

	pexprGet1->Release();
	pexprGet2->Release();

	return SPQOS_OK;
}


//---------------------------------------------------------------------------
//	@function:
//		CConstraintTest::Pcstconjunction
//
//	@doc:
//		Build a conjunction
//
//---------------------------------------------------------------------------
CConstraintConjunction *
CConstraintTest::Pcstconjunction(CMemoryPool *mp, IMDId *mdid, CColRef *colref)
{
	// first interval
	CConstraintInterval *pciFirst = PciFirstInterval(mp, mdid, colref);

	// second interval
	CConstraintInterval *pciSecond = PciSecondInterval(mp, mdid, colref);

	CConstraintArray *pdrspqcst = SPQOS_NEW(mp) CConstraintArray(mp);
	pdrspqcst->Append(pciFirst);
	pdrspqcst->Append(pciSecond);

	return SPQOS_NEW(mp) CConstraintConjunction(mp, pdrspqcst);
}

//---------------------------------------------------------------------------
//	@function:
//		CConstraintTest::Pcstdisjunction
//
//	@doc:
//		Build a disjunction
//
//---------------------------------------------------------------------------
CConstraintDisjunction *
CConstraintTest::Pcstdisjunction(CMemoryPool *mp, IMDId *mdid, CColRef *colref)
{
	// first interval
	CConstraintInterval *pciFirst = PciFirstInterval(mp, mdid, colref);

	// second interval
	CConstraintInterval *pciSecond = PciSecondInterval(mp, mdid, colref);

	CConstraintArray *pdrspqcst = SPQOS_NEW(mp) CConstraintArray(mp);
	pdrspqcst->Append(pciFirst);
	pdrspqcst->Append(pciSecond);

	return SPQOS_NEW(mp) CConstraintDisjunction(mp, pdrspqcst);
}

//---------------------------------------------------------------------------
//	@function:
//		CConstraintTest::EresUnittest_CDisjunction
//
//	@doc:
//		Conjunction test
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CConstraintTest::EresUnittest_CDisjunction()
{
	// create memory pool
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	// setup a file-based provider
	CMDProviderMemory *pmdp = CTestUtils::m_pmdpf;
	pmdp->AddRef();
	CMDAccessor mda(mp, CMDCache::Pcache(), CTestUtils::m_sysidDefault, pmdp);

	CConstExprEvaluatorForDates *pceeval =
		SPQOS_NEW(mp) CConstExprEvaluatorForDates(mp);

	// install opt context in TLS
	CAutoOptCtxt aoc(mp, &mda, pceeval, CTestUtils::GetCostModel(mp));
	SPQOS_ASSERT(NULL != COptCtxt::PoctxtFromTLS()->Pcomp());

	IMDTypeInt8 *pmdtypeint8 =
		(IMDTypeInt8 *) mda.PtMDType<IMDTypeInt8>(CTestUtils::m_sysidDefault);
	IMDId *mdid = pmdtypeint8->MDId();

	CExpression *pexprGet1 = CTestUtils::PexprLogicalGet(mp);
	CColRefSet *pcrs1 = pexprGet1->DeriveOutputColumns();
	CColRef *pcr1 = pcrs1->PcrAny();

	CExpression *pexprGet2 = CTestUtils::PexprLogicalGet(mp);
	CColRefSet *pcrs2 = pexprGet2->DeriveOutputColumns();
	CColRef *pcr2 = pcrs2->PcrAny();

	CConstraintDisjunction *pcdisj1 = Pcstdisjunction(mp, mdid, pcr1);
	PrintConstraint(mp, pcdisj1);
	SPQOS_ASSERT(!pcdisj1->FContradiction());

	CConstraintDisjunction *pcdisj2 = Pcstdisjunction(mp, mdid, pcr2);
	PrintConstraint(mp, pcdisj2);

	CConstraintArray *pdrspqcst = SPQOS_NEW(mp) CConstraintArray(mp);
	pcdisj1->AddRef();
	pcdisj2->AddRef();
	pdrspqcst->Append(pcdisj1);
	pdrspqcst->Append(pcdisj2);

	CConstraintDisjunction *pcdisjTop =
		SPQOS_NEW(mp) CConstraintDisjunction(mp, pdrspqcst);
	PrintConstraint(mp, pcdisjTop);

	// containment
	SPQOS_ASSERT(!pcdisj1->Contains(pcdisj2));
	SPQOS_ASSERT(!pcdisj2->Contains(pcdisj1));
	SPQOS_ASSERT(!pcdisj1->Contains(pcdisjTop));
	SPQOS_ASSERT(pcdisjTop->Contains(pcdisj1));
	SPQOS_ASSERT(!pcdisj2->Contains(pcdisjTop));
	SPQOS_ASSERT(pcdisjTop->Contains(pcdisj2));

	// equality
	SPQOS_ASSERT(!pcdisj1->Equals(pcdisjTop));
	SPQOS_ASSERT(!pcdisjTop->Equals(pcdisj2));

	pcdisjTop->Release();
	pcdisj1->Release();
	pcdisj2->Release();

	pexprGet1->Release();
	pexprGet2->Release();

	return SPQOS_OK;
}

//---------------------------------------------------------------------------
//	@function:
//		CConstraintTest::EresUnittest_CNegation
//
//	@doc:
//		Conjunction test
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CConstraintTest::EresUnittest_CNegation()
{
	// create memory pool
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	// setup a file-based provider
	CMDProviderMemory *pmdp = CTestUtils::m_pmdpf;
	pmdp->AddRef();
	CMDAccessor mda(mp, CMDCache::Pcache(), CTestUtils::m_sysidDefault, pmdp);

	CConstExprEvaluatorForDates *pceeval =
		SPQOS_NEW(mp) CConstExprEvaluatorForDates(mp);

	// install opt context in TLS
	CAutoOptCtxt aoc(mp, &mda, pceeval, CTestUtils::GetCostModel(mp));
	SPQOS_ASSERT(NULL != COptCtxt::PoctxtFromTLS()->Pcomp());

	IMDTypeInt8 *pmdtypeint8 =
		(IMDTypeInt8 *) mda.PtMDType<IMDTypeInt8>(CTestUtils::m_sysidDefault);
	IMDId *mdid = pmdtypeint8->MDId();

	CExpression *pexprGet = CTestUtils::PexprLogicalGet(mp);
	CColRefSet *pcrs = pexprGet->DeriveOutputColumns();
	CColRef *colref = pcrs->PcrAny();

	CConstraintInterval *pci = PciFirstInterval(mp, mdid, colref);
	PrintConstraint(mp, pci);

	pci->AddRef();
	CConstraintNegation *pcn1 = SPQOS_NEW(mp) CConstraintNegation(mp, pci);
	PrintConstraint(mp, pcn1);

	pcn1->AddRef();
	CConstraintNegation *pcn2 = SPQOS_NEW(mp) CConstraintNegation(mp, pcn1);
	PrintConstraint(mp, pcn2);

	// containment
	SPQOS_ASSERT(!pcn1->Contains(pci));
	SPQOS_ASSERT(!pci->Contains(pcn1));
	SPQOS_ASSERT(!pcn2->Contains(pcn1));
	SPQOS_ASSERT(!pcn1->Contains(pcn2));
	SPQOS_ASSERT(pci->Contains(pcn2));
	SPQOS_ASSERT(pcn2->Contains(pci));

	// equality
	SPQOS_ASSERT(!pcn1->Equals(pci));
	SPQOS_ASSERT(!pcn1->Equals(pcn2));
	SPQOS_ASSERT(pci->Equals(pcn2));

	pcn2->Release();
	pcn1->Release();
	pci->Release();

	pexprGet->Release();

	return SPQOS_OK;
}

//---------------------------------------------------------------------------
//	@function:
//		CConstraintTest::EresUnittest_CIntervalFromScalarExpr
//
//	@doc:
//		Interval from scalar tests
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CConstraintTest::EresUnittest_CIntervalFromScalarExpr()
{
	// create memory pool
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	// setup a file-based provider
	CMDProviderMemory *pmdp = CTestUtils::m_pmdpf;
	pmdp->AddRef();
	CMDAccessor mda(mp, CMDCache::Pcache(), CTestUtils::m_sysidDefault, pmdp);

	CConstExprEvaluatorForDates *pceeval =
		SPQOS_NEW(mp) CConstExprEvaluatorForDates(mp);

	// install opt context in TLS
	CAutoOptCtxt aoc(mp, &mda, pceeval, CTestUtils::GetCostModel(mp));
	SPQOS_ASSERT(NULL != COptCtxt::PoctxtFromTLS()->Pcomp());

	CExpression *pexprGet = CTestUtils::PexprLogicalGet(mp);
	CColRefSet *pcrs = pexprGet->DeriveOutputColumns();
	CColRef *colref = pcrs->PcrAny();

	// from ScalarCmp
	SPQOS_RESULT eres1 = EresUnittest_CIntervalFromScalarCmp(mp, &mda, colref);

	// from ScalarBool
	SPQOS_RESULT eres2 =
		EresUnittest_CIntervalFromScalarBoolOp(mp, &mda, colref);

	pexprGet->Release();
	if (SPQOS_OK == eres1 && SPQOS_OK == eres2)
	{
		return SPQOS_OK;
	}

	return SPQOS_FAILED;
}

//---------------------------------------------------------------------------
//	@function:
//		CConstraintTest::EresUnittest_CConstraintFromScalarExpr
//
//	@doc:
//		Constraint from scalar tests
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CConstraintTest::EresUnittest_CConstraintFromScalarExpr()
{
	// create memory pool
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	// setup a file-based provider
	CMDProviderMemory *pmdp = CTestUtils::m_pmdpf;
	pmdp->AddRef();
	CMDAccessor mda(mp, CMDCache::Pcache(), CTestUtils::m_sysidDefault, pmdp);

	CConstExprEvaluatorForDates *pceeval =
		SPQOS_NEW(mp) CConstExprEvaluatorForDates(mp);

	// install opt context in TLS
	CAutoOptCtxt aoc(mp, &mda, pceeval, CTestUtils::GetCostModel(mp));
	SPQOS_ASSERT(NULL != COptCtxt::PoctxtFromTLS()->Pcomp());

	CExpression *pexprGet1 = CTestUtils::PexprLogicalGet(mp);
	CColRefSet *pcrs1 = pexprGet1->DeriveOutputColumns();
	CColRef *pcr1 = pcrs1->PcrAny();

	CExpression *pexprGet2 = CTestUtils::PexprLogicalGet(mp);
	CColRefSet *pcrs2 = pexprGet2->DeriveOutputColumns();
	CColRef *pcr2 = pcrs2->PcrAny();

	CColRefSetArray *pdrspqcrs = NULL;

	// expression with 1 column
	CExpression *pexpr = PexprScalarCmp(mp, &mda, pcr1, IMDType::EcmptG, 15);
	CConstraint *pcnstr =
		CConstraint::PcnstrFromScalarExpr(mp, pexpr, &pdrspqcrs);
	PrintConstraint(mp, pcnstr);
	PrintEquivClasses(mp, pdrspqcrs);
	pdrspqcrs->Release();
	pdrspqcrs = NULL;
	pcnstr->Release();
	pexpr->Release();

	// expression with 2 columns
	CExpressionArray *pdrspqexpr = SPQOS_NEW(mp) CExpressionArray(mp);
	pdrspqexpr->Append(PexprScalarCmp(mp, &mda, pcr1, IMDType::EcmptG, 15));
	pdrspqexpr->Append(PexprScalarCmp(mp, &mda, pcr2, IMDType::EcmptL, 20));

	CExpression *pexprAnd =
		CUtils::PexprScalarBoolOp(mp, CScalarBoolOp::EboolopAnd, pdrspqexpr);
	(void) pexprAnd->PdpDerive();

	CConstraint *pcnstrAnd =
		CConstraint::PcnstrFromScalarExpr(mp, pexprAnd, &pdrspqcrs);
	PrintConstraint(mp, pcnstrAnd);
	PrintEquivClasses(mp, pdrspqcrs);
	pdrspqcrs->Release();
	pdrspqcrs = NULL;

	pcnstrAnd->Release();
	pexprAnd->Release();

	// equality predicate with 2 columns
	CExpression *pexprEq = CUtils::PexprScalarEqCmp(mp, pcr1, pcr2);
	(void) pexprEq->PdpDerive();
	CConstraint *pcnstrEq =
		CConstraint::PcnstrFromScalarExpr(mp, pexprEq, &pdrspqcrs);
	PrintConstraint(mp, pcnstrEq);
	PrintEquivClasses(mp, pdrspqcrs);

	pcnstrEq->Release();
	pexprEq->Release();

	pdrspqcrs->Release();

	pexprGet1->Release();
	pexprGet2->Release();

	return SPQOS_OK;
}

//---------------------------------------------------------------------------
//	@function:
//		CConstraintTest::EresUnittest_CConstraintIntervalConvertsTo
//
//	@doc:
//		Tests CConstraintInterval::ConvertsToIn and
//		CConstraintInterval::ConvertsToNotIn
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CConstraintTest::EresUnittest_CConstraintIntervalConvertsTo()
{
	// create memory pool
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	// setup a file-based provider
	CMDProviderMemory *pmdp = CTestUtils::m_pmdpf;
	pmdp->AddRef();
	CMDAccessor mda(mp, CMDCache::Pcache(), CTestUtils::m_sysidDefault, pmdp);

	CConstExprEvaluatorForDates *pceeval =
		SPQOS_NEW(mp) CConstExprEvaluatorForDates(mp);

	// install opt context in TLS
	CAutoOptCtxt aoc(mp, &mda, pceeval, CTestUtils::GetCostModel(mp));
	SPQOS_ASSERT(NULL != COptCtxt::PoctxtFromTLS()->Pcomp());

	// create a range which should convert to an IN array expression
	const SRangeInfo rgRangeInfoIn[] = {
		{CRange::EriIncluded, -1000, CRange::EriIncluded, -1000},
		{CRange::EriIncluded, -5, CRange::EriIncluded, -5},
		{CRange::EriIncluded, 0, CRange::EriIncluded, 0}};

	// metadata id
	IMDTypeInt8 *pmdtypeint8 =
		(IMDTypeInt8 *) mda.PtMDType<IMDTypeInt8>(CTestUtils::m_sysidDefault);
	IMDId *mdid = pmdtypeint8->MDId();

	// get a column ref
	CExpression *pexprGet = CTestUtils::PexprLogicalGet(mp);
	CColRefSet *pcrs = pexprGet->DeriveOutputColumns();
	CColRef *colref = pcrs->PcrAny();

	// create constraint
	CRangeArray *pdrspqrng =
		Pdrspqrng(mp, mdid, rgRangeInfoIn, SPQOS_ARRAY_SIZE(rgRangeInfoIn));
	CConstraintInterval *pcnstin =
		SPQOS_NEW(mp) CConstraintInterval(mp, colref, pdrspqrng, true);

	PrintConstraint(mp, pcnstin);

	// should convert to in
	SPQOS_ASSERT(pcnstin->FConvertsToIn());
	SPQOS_ASSERT(!pcnstin->FConvertsToNotIn());

	CConstraintInterval *pcnstNotIn = pcnstin->PciComplement(mp);

	// should convert to a not in statement after taking the complement
	SPQOS_ASSERT(pcnstNotIn->FConvertsToNotIn());
	SPQOS_ASSERT(!pcnstNotIn->FConvertsToIn());

	pcnstin->Release();
	pcnstNotIn->Release();
	pexprGet->Release();

	return SPQOS_OK;
}


//---------------------------------------------------------------------------
//	@function:
//		CConstraintTest::EresUnittest_CConstraintIntervalPexpr
//
//	@doc:
//		Tests CConstraintInterval::PexprConstructArrayScalar
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CConstraintTest::EresUnittest_CConstraintIntervalPexpr()
{
	// create memory pool
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	// setup a file-based provider
	CMDProviderMemory *pmdp = CTestUtils::m_pmdpf;
	pmdp->AddRef();
	CMDAccessor mda(mp, CMDCache::Pcache(), CTestUtils::m_sysidDefault, pmdp);

	CConstExprEvaluatorForDates *pceeval =
		SPQOS_NEW(mp) CConstExprEvaluatorForDates(mp);

	// install opt context in TLS
	CAutoOptCtxt aoc(mp, &mda, pceeval, CTestUtils::GetCostModel(mp));
	SPQOS_RTL_ASSERT(NULL != COptCtxt::PoctxtFromTLS()->Pcomp());

	CAutoTraceFlag atf(EopttraceArrayConstraints, true);

	// create a range which should convert to an IN array expression
	const SRangeInfo rgRangeInfoIn[] = {
		{CRange::EriIncluded, -1000, CRange::EriIncluded, -1000},
		{CRange::EriIncluded, -5, CRange::EriIncluded, -5},
		{CRange::EriIncluded, 0, CRange::EriIncluded, 0}};

	// metadata id
	IMDTypeInt8 *pmdtypeint8 =
		(IMDTypeInt8 *) mda.PtMDType<IMDTypeInt8>(CTestUtils::m_sysidDefault);
	IMDId *mdid = pmdtypeint8->MDId();

	// get a column ref
	CExpression *pexprGet = CTestUtils::PexprLogicalGet(mp);
	CColRefSet *pcrs = pexprGet->DeriveOutputColumns();
	CColRef *colref = pcrs->PcrAny();

	CRangeArray *pdrspqrng = NULL;
	CConstraintInterval *pcnstin = NULL;
	CExpression *pexpr = NULL;
	CConstraintInterval *pcnstNotIn = NULL;

	// IN CONSTRAINT FOR SIMPLE INTERVAL (WITHOUT NULL)

	// create constraint
	pdrspqrng =
		Pdrspqrng(mp, mdid, rgRangeInfoIn, SPQOS_ARRAY_SIZE(rgRangeInfoIn));
	pcnstin = SPQOS_NEW(mp) CConstraintInterval(mp, colref, pdrspqrng, false);

	pexpr = pcnstin->PexprScalar(mp);  // pexpr is owned by the constraint
	PrintConstraint(mp, pcnstin);

	SPQOS_RTL_ASSERT(!pcnstin->FConvertsToNotIn());
	SPQOS_RTL_ASSERT(pcnstin->FConvertsToIn());
	SPQOS_RTL_ASSERT(CUtils::FScalarArrayCmp(pexpr));
	SPQOS_RTL_ASSERT(3 ==
					CUtils::UlCountOperator(pexpr, COperator::EopScalarConst));

	pcnstin->Release();


	// IN CONSTRAINT FOR SIMPLE INTERVAL WITH NULL

	// create constraint
	pdrspqrng =
		Pdrspqrng(mp, mdid, rgRangeInfoIn, SPQOS_ARRAY_SIZE(rgRangeInfoIn));
	pcnstin = SPQOS_NEW(mp) CConstraintInterval(mp, colref, pdrspqrng, true);

	pexpr = pcnstin->PexprScalar(mp);  // pexpr is owned by the constraint
	PrintConstraint(mp, pcnstin);

	SPQOS_RTL_ASSERT(!pcnstin->FConvertsToNotIn());
	SPQOS_RTL_ASSERT(pcnstin->FConvertsToIn());
	SPQOS_RTL_ASSERT(CUtils::FScalarBoolOp(pexpr, CScalarBoolOp::EboolopOr));
	SPQOS_RTL_ASSERT(CUtils::FScalarArrayCmp((*pexpr)[0]));
	SPQOS_RTL_ASSERT(
		3 == CUtils::UlCountOperator((*pexpr)[0], COperator::EopScalarConst));
	SPQOS_RTL_ASSERT(CUtils::FScalarNullTest((*pexpr)[1]));

	pcnstin->Release();


	// NOT IN CONSTRAINT FOR SIMPLE INTERVAL WITHOUT NULL

	// create constraint
	pdrspqrng =
		Pdrspqrng(mp, mdid, rgRangeInfoIn, SPQOS_ARRAY_SIZE(rgRangeInfoIn));
	pcnstin = SPQOS_NEW(mp) CConstraintInterval(mp, colref, pdrspqrng, true);

	pcnstNotIn = pcnstin->PciComplement(mp);
	pcnstin->Release();

	pexpr = pcnstNotIn->PexprScalar(mp);  // pexpr is owned by the constraint
	PrintConstraint(mp, pcnstNotIn);

	SPQOS_RTL_ASSERT(pcnstNotIn->FConvertsToNotIn());
	SPQOS_RTL_ASSERT(!pcnstNotIn->FConvertsToIn());
	SPQOS_RTL_ASSERT(CUtils::FScalarArrayCmp(pexpr));
	SPQOS_RTL_ASSERT(3 ==
					CUtils::UlCountOperator(pexpr, COperator::EopScalarConst));

	pcnstNotIn->Release();


	// NOT IN CONSTRAINT FOR SIMPLE INTERVAL WITH NULL

	// create constraint
	pdrspqrng =
		Pdrspqrng(mp, mdid, rgRangeInfoIn, SPQOS_ARRAY_SIZE(rgRangeInfoIn));
	pcnstin = SPQOS_NEW(mp) CConstraintInterval(mp, colref, pdrspqrng, false);

	pcnstNotIn = pcnstin->PciComplement(mp);
	pcnstin->Release();

	pexpr = pcnstNotIn->PexprScalar(mp);  // pexpr is owned by the constraint
	PrintConstraint(mp, pcnstNotIn);

	SPQOS_RTL_ASSERT(pcnstNotIn->FConvertsToNotIn());
	SPQOS_RTL_ASSERT(!pcnstNotIn->FConvertsToIn());
	SPQOS_RTL_ASSERT(CUtils::FScalarBoolOp(pexpr, CScalarBoolOp::EboolopOr));
	SPQOS_RTL_ASSERT(CUtils::FScalarArrayCmp((*pexpr)[0]));
	SPQOS_RTL_ASSERT(3 ==
					CUtils::UlCountOperator(pexpr, COperator::EopScalarConst));
	SPQOS_RTL_ASSERT(CUtils::FScalarNullTest((*pexpr)[1]));

	pcnstNotIn->Release();


	pexprGet->Release();

	return SPQOS_OK;
}

//---------------------------------------------------------------------------
//	@function:
//		CConstraintTest::EresUnittest_CConstraintIntervalFromArrayExpr
//
//	@doc:
//		Tests CConstraintInterval::PcnstrIntervalFromScalarArrayCmp
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CConstraintTest::EresUnittest_CConstraintIntervalFromArrayExpr()
{
	// create memory pool
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	// setup a file-based provider
	CMDProviderMemory *pmdp = CTestUtils::m_pmdpf;
	pmdp->AddRef();
	CMDAccessor mda(mp, CMDCache::Pcache(), CTestUtils::m_sysidDefault, pmdp);

	CConstExprEvaluatorForDates *pceeval =
		SPQOS_NEW(mp) CConstExprEvaluatorForDates(mp);

	// install opt context in TLS
	CAutoOptCtxt aoc(mp, &mda, pceeval, CTestUtils::GetCostModel(mp));
	SPQOS_ASSERT(NULL != COptCtxt::PoctxtFromTLS()->Pcomp());

	CAutoTraceFlag atf(EopttraceArrayConstraints, true);

	// Create an IN array expression
	CExpression *pexpr = CTestUtils::PexprLogicalSelectArrayCmp(mp);
	// get a ref to the comparison column
	CColRef *colref = pexpr->DeriveOutputColumns()->PcrAny();

	// remove the array child
	CExpression *pexprArrayComp = (*pexpr->PdrgPexpr())[1];
	SPQOS_ASSERT(CUtils::FScalarArrayCmp(pexprArrayComp));

	CConstraintInterval *pcnstIn =
		CConstraintInterval::PciIntervalFromScalarExpr(mp, pexprArrayComp,
													   colref);
	SPQOS_ASSERT(CConstraint::EctInterval == pcnstIn->Ect());
	SPQOS_ASSERT(
		pcnstIn->Pdrspqrng()->Size() ==
		CUtils::UlCountOperator(pexprArrayComp, COperator::EopScalarConst));

	pcnstIn->Release();
	pexpr->Release();

	// test a NOT IN expression

	CExpression *pexprNotIn = CTestUtils::PexprLogicalSelectArrayCmp(
		mp, CScalarArrayCmp::EarrcmpAll, IMDType::EcmptNEq);
	CExpression *pexprArrayNotInComp = (*pexprNotIn->PdrgPexpr())[1];
	CColRef *pcrNot = pexprNotIn->DeriveOutputColumns()->PcrAny();
	CConstraintInterval *pcnstNotIn =
		CConstraintInterval::PciIntervalFromScalarExpr(mp, pexprArrayNotInComp,
													   pcrNot);
	SPQOS_ASSERT(CConstraint::EctInterval == pcnstNotIn->Ect());
	// a NOT IN range array should have one more element than the expression array consts
	SPQOS_ASSERT(pcnstNotIn->Pdrspqrng()->Size() ==
				1 + CUtils::UlCountOperator(pexprArrayNotInComp,
											COperator::EopScalarConst));

	pexprNotIn->Release();
	pcnstNotIn->Release();

	// create an IN expression with repeated values
	IntPtrArray *pdrspqi = SPQOS_NEW(mp) IntPtrArray(mp);
	INT aiValsRepeat[] = {5, 1, 2, 5, 3, 4, 5};
	ULONG aiValsLength = sizeof(aiValsRepeat) / sizeof(INT);
	for (ULONG ul = 0; ul < aiValsLength; ul++)
	{
		pdrspqi->Append(SPQOS_NEW(mp) INT(aiValsRepeat[ul]));
	}
	CExpression *pexprInRepeatsSelect = CTestUtils::PexprLogicalSelectArrayCmp(
		mp, CScalarArrayCmp::EarrcmpAny, IMDType::EcmptEq, pdrspqi);
	CColRef *pcrInRepeats =
		pexprInRepeatsSelect->DeriveOutputColumns()->PcrAny();
	CExpression *pexprArrayCmpRepeats = (*pexprInRepeatsSelect->PdrgPexpr())[1];
	// add 2 repeated values and one unique
	CConstraintInterval *pcnstInRepeats =
		CConstraintInterval::PciIntervalFromScalarExpr(mp, pexprArrayCmpRepeats,
													   pcrInRepeats);
	SPQOS_ASSERT(5 == pcnstInRepeats->Pdrspqrng()->Size());
	pexprInRepeatsSelect->Release();
	pcnstInRepeats->Release();

	// create a NOT IN expression with repeated values
	CExpression *pexprNotInRepeatsSelect =
		CTestUtils::PexprLogicalSelectArrayCmp(mp, CScalarArrayCmp::EarrcmpAll,
											   IMDType::EcmptNEq, pdrspqi);
	CColRef *pcrNotInRepeats =
		pexprNotInRepeatsSelect->DeriveOutputColumns()->PcrAny();
	CExpression *pexprNotInArrayCmpRepeats =
		(*pexprNotInRepeatsSelect->PdrgPexpr())[1];
	CConstraintInterval *pcnstNotInRepeats =
		CConstraintInterval::PciIntervalFromScalarExpr(
			mp, pexprNotInArrayCmpRepeats, pcrNotInRepeats);
	// a total of 5 unique ScalarConsts in the expression will result in 6 ranges
	SPQOS_ASSERT(6 == pcnstNotInRepeats->Pdrspqrng()->Size());
	pexprNotInRepeatsSelect->Release();
	pcnstNotInRepeats->Release();
	pdrspqi->Release();

	return SPQOS_OK;
}


//---------------------------------------------------------------------------
//	@function:
//		CConstraintTest::EresUnittest_CConstraintIntervalFromArrayExprIncludesNull
//
//	@doc:
//		Tests CConstraintInterval::PcnstrIntervalFromScalarArrayCmp in cases
//		where NULL is in the scalar array expression
//
//---------------------------------------------------------------------------
SPQOS_RESULT
EresUnittest_CConstraintIntervalFromArrayExprIncludesNull()
{
	// create memory pool
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	// setup a file-based provider
	CMDProviderMemory *pmdp = CTestUtils::m_pmdpf;
	pmdp->AddRef();
	CMDAccessor mda(mp, CMDCache::Pcache(), CTestUtils::m_sysidDefault, pmdp);

	CConstExprEvaluatorForDates *pceeval =
		SPQOS_NEW(mp) CConstExprEvaluatorForDates(mp);

	// install opt context in TLS
	CAutoOptCtxt aoc(mp, &mda, pceeval, CTestUtils::GetCostModel(mp));
	SPQOS_ASSERT(NULL != COptCtxt::PoctxtFromTLS()->Pcomp());

	CAutoTraceFlag atf(EopttraceArrayConstraints, true);

	// test for includes NULL
	// create an IN expression with repeated values
	IntPtrArray *pdrspqi = SPQOS_NEW(mp) IntPtrArray(mp);
	INT rngiValues[] = {1, 2};
	ULONG ulValsLength = SPQOS_ARRAY_SIZE(rngiValues);
	for (ULONG ul = 0; ul < ulValsLength; ul++)
	{
		pdrspqi->Append(SPQOS_NEW(mp) INT(rngiValues[ul]));
	}
	CExpression *pexprIn = CTestUtils::PexprLogicalSelectArrayCmp(
		mp, CScalarArrayCmp::EarrcmpAny, IMDType::EcmptEq, pdrspqi);

	CExpression *pexprArrayChild = (*(*pexprIn)[1])[1];
	// create a int4 datum
	const IMDTypeInt4 *pmdtypeint4 = mda.PtMDType<IMDTypeInt4>();
	IDatumInt4 *pdatumNull = pmdtypeint4->CreateInt4Datum(mp, 0, true);

	CExpression *pexprConstNull = SPQOS_NEW(mp)
		CExpression(mp, SPQOS_NEW(mp) CScalarConst(mp, (IDatum *) pdatumNull));
	pexprArrayChild->PdrgPexpr()->Append(pexprConstNull);

	CColRef *colref = pexprIn->DeriveOutputColumns()->PcrAny();
	CConstraintInterval *pci = CConstraintInterval::PciIntervalFromScalarExpr(
		mp, (*pexprIn)[1], colref);
	SPQOS_RTL_ASSERT(!pci->FIncludesNull());
	SPQOS_RTL_ASSERT(2 == pci->Pdrspqrng()->Size());
	pexprIn->Release();
	pci->Release();
	pdrspqi->Release();

	return SPQOS_OK;
}

//---------------------------------------------------------------------------
//	@function:
//		CConstraintTest::EresUnittest_CIntervalFromScalarCmp
//
//	@doc:
//		Interval from scalar comparison
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CConstraintTest::EresUnittest_CIntervalFromScalarCmp(CMemoryPool *mp,
													 CMDAccessor *md_accessor,
													 CColRef *colref)
{
	IMDType::ECmpType rgecmpt[] = {
		IMDType::EcmptEq,  IMDType::EcmptNEq, IMDType::EcmptL,
		IMDType::EcmptLEq, IMDType::EcmptG,	  IMDType::EcmptGEq,
	};

	for (ULONG ul = 0; ul < SPQOS_ARRAY_SIZE(rgecmpt); ul++)
	{
		CExpression *pexprScCmp =
			PexprScalarCmp(mp, md_accessor, colref, rgecmpt[ul], 4);
		CConstraintInterval *pci =
			CConstraintInterval::PciIntervalFromScalarExpr(mp, pexprScCmp,
														   colref);
		PrintConstraint(mp, pci);

		pci->Release();
		pexprScCmp->Release();
	}

	return SPQOS_OK;
}

//---------------------------------------------------------------------------
//	@function:
//		CConstraintTest::EresUnittest_CIntervalFromScalarBoolOp
//
//	@doc:
//		Interval from scalar bool op
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CConstraintTest::EresUnittest_CIntervalFromScalarBoolOp(
	CMemoryPool *mp, CMDAccessor *md_accessor, CColRef *colref)
{
	// AND
	CExpressionArray *pdrspqexpr = SPQOS_NEW(mp) CExpressionArray(mp);
	pdrspqexpr->Append(
		PexprScalarCmp(mp, md_accessor, colref, IMDType::EcmptL, 5));
	pdrspqexpr->Append(
		PexprScalarCmp(mp, md_accessor, colref, IMDType::EcmptGEq, 0));

	CExpression *pexpr =
		CUtils::PexprScalarBoolOp(mp, CScalarBoolOp::EboolopAnd, pdrspqexpr);
	(void) pexpr->PdpDerive();

	CConstraintInterval *pciAnd =
		CConstraintInterval::PciIntervalFromScalarExpr(mp, pexpr, colref);
	PrintConstraint(mp, pciAnd);

	pciAnd->Release();
	(void) pexpr->Release();

	// OR
	pdrspqexpr = SPQOS_NEW(mp) CExpressionArray(mp);
	pdrspqexpr->Append(
		PexprScalarCmp(mp, md_accessor, colref, IMDType::EcmptL, 5));
	pdrspqexpr->Append(
		PexprScalarCmp(mp, md_accessor, colref, IMDType::EcmptGEq, 10));

	pexpr = CUtils::PexprScalarBoolOp(mp, CScalarBoolOp::EboolopOr, pdrspqexpr);
	(void) pexpr->PdpDerive();

	CConstraintInterval *pciOr =
		CConstraintInterval::PciIntervalFromScalarExpr(mp, pexpr, colref);
	PrintConstraint(mp, pciOr);

	pciOr->Release();
	pexpr->Release();

	// NOT
	pdrspqexpr = SPQOS_NEW(mp) CExpressionArray(mp);
	pdrspqexpr->Append(
		CUtils::PexprIsNull(mp, CUtils::PexprScalarIdent(mp, colref)));

	pexpr = CUtils::PexprScalarBoolOp(mp, CScalarBoolOp::EboolopNot, pdrspqexpr);
	(void) pexpr->PdpDerive();

	CConstraintInterval *pciNot =
		CConstraintInterval::PciIntervalFromScalarExpr(mp, pexpr, colref);
	PrintConstraint(mp, pciNot);

	pciNot->Release();
	pexpr->Release();

	return SPQOS_OK;
}

//---------------------------------------------------------------------------
//	@function:
//		CConstraintTest::PexprScalarCmp
//
//	@doc:
//		Generate comparison expression
//
//---------------------------------------------------------------------------
CExpression *
CConstraintTest::PexprScalarCmp(CMemoryPool *mp, CMDAccessor *md_accessor,
								CColRef *colref, IMDType::ECmpType cmp_type,
								LINT val)
{
	CExpression *pexprConst = CUtils::PexprScalarConstInt8(mp, val);

	const IMDTypeInt8 *pmdtypeint8 = md_accessor->PtMDType<IMDTypeInt8>();
	IMDId *mdid_op = pmdtypeint8->GetMdidForCmpType(cmp_type);
	mdid_op->AddRef();

	const CMDName mdname = md_accessor->RetrieveScOp(mdid_op)->Mdname();

	CWStringConst strOpName(mdname.GetMDName()->GetBuffer());

	CExpression *pexpr =
		CUtils::PexprScalarCmp(mp, colref, pexprConst, strOpName, mdid_op);
	(void) pexpr->PdpDerive();
	return pexpr;
}

//---------------------------------------------------------------------------
//	@function:
//		CConstraintTest::PciFirstInterval
//
//	@doc:
//		Create an interval
//
//---------------------------------------------------------------------------
CConstraintInterval *
CConstraintTest::PciFirstInterval(CMemoryPool *mp, IMDId *mdid, CColRef *colref)
{
	const SRangeInfo rgRangeInfo[] = {
		{CRange::EriExcluded, -1000, CRange::EriIncluded, -20},
		{CRange::EriExcluded, -15, CRange::EriExcluded, -5},
		{CRange::EriIncluded, 0, CRange::EriExcluded, 5},
		{CRange::EriIncluded, 10, CRange::EriIncluded, 10},
		{CRange::EriExcluded, 20, CRange::EriExcluded, 1000},
	};

	CRangeArray *pdrspqrng =
		Pdrspqrng(mp, mdid, rgRangeInfo, SPQOS_ARRAY_SIZE(rgRangeInfo));

	return SPQOS_NEW(mp)
		CConstraintInterval(mp, colref, pdrspqrng, true /*is_null*/);
}

//---------------------------------------------------------------------------
//	@function:
//		CConstraintTest::PciSecondInterval
//
//	@doc:
//		Create an interval
//
//---------------------------------------------------------------------------
CConstraintInterval *
CConstraintTest::PciSecondInterval(CMemoryPool *mp, IMDId *mdid,
								   CColRef *colref)
{
	const SRangeInfo rgRangeInfo[] = {
		{CRange::EriExcluded, -30, CRange::EriExcluded, 1},
		{CRange::EriIncluded, 3, CRange::EriIncluded, 3},
		{CRange::EriExcluded, 10, CRange::EriExcluded, 25},
	};

	CRangeArray *pdrspqrng =
		Pdrspqrng(mp, mdid, rgRangeInfo, SPQOS_ARRAY_SIZE(rgRangeInfo));

	return SPQOS_NEW(mp)
		CConstraintInterval(mp, colref, pdrspqrng, false /*is_null*/);
}

//---------------------------------------------------------------------------
//	@function:
//		CConstraintTest::Pdrspqrng
//
//	@doc:
//		Construct an array of ranges to be used to create an interval
//
//---------------------------------------------------------------------------
CRangeArray *
CConstraintTest::Pdrspqrng(CMemoryPool *mp, IMDId *mdid,
						  const SRangeInfo rgRangeInfo[], ULONG ulRanges)
{
	CRangeArray *pdrspqrng = SPQOS_NEW(mp) CRangeArray(mp);

	for (ULONG ul = 0; ul < ulRanges; ul++)
	{
		SRangeInfo rnginfo = rgRangeInfo[ul];
		mdid->AddRef();
		CRange *prange = SPQOS_NEW(mp)
			CRange(mdid, COptCtxt::PoctxtFromTLS()->Pcomp(),
				   SPQOS_NEW(mp) CDatumInt8SPQDB(CTestUtils::m_sysidDefault,
											   (LINT) rnginfo.iLeft),
				   rnginfo.eriLeft,
				   SPQOS_NEW(mp) CDatumInt8SPQDB(CTestUtils::m_sysidDefault,
											   (LINT) rnginfo.iRight),
				   rnginfo.eriRight);
		pdrspqrng->Append(prange);
	}

	return pdrspqrng;
}


//---------------------------------------------------------------------------
//	@function:
//		CConstraintTest::PrintConstraint
//
//	@doc:
//		Print a constraint and its expression representation
//
//---------------------------------------------------------------------------
void
CConstraintTest::PrintConstraint(CMemoryPool *mp, CConstraint *pcnstr)
{
	CExpression *pexpr = pcnstr->PexprScalar(mp);

	// debug print
	CAutoTrace at(mp);
	at.Os() << std::endl;
	at.Os() << "CONSTRAINT:" << std::endl
			<< *pcnstr << std::endl
			<< "EXPR:" << std::endl
			<< *pexpr << std::endl;
}

//---------------------------------------------------------------------------
//	@function:
//		CConstraintTest::PrintEquivClasses
//
//	@doc:
//		Print equivalent classes
//
//---------------------------------------------------------------------------
void
CConstraintTest::PrintEquivClasses(CMemoryPool *mp, CColRefSetArray *pdrspqcrs,
								   BOOL fExpected)
{
	// debug print
	CAutoTrace at(mp);
	at.Os() << std::endl;
	if (fExpected)
	{
		at.Os() << "EXPECTED ";
	}
	else
	{
		at.Os() << "ACTUAL ";
	}
	at.Os() << "EQUIVALENCE CLASSES: [ ";

	if (NULL == pdrspqcrs)
	{
		at.Os() << "]" << std::endl;

		return;
	}

	for (ULONG ul = 0; ul < pdrspqcrs->Size(); ul++)
	{
		at.Os() << "{" << *(*pdrspqcrs)[ul] << "} ";
	}

	at.Os() << "]" << std::endl;
}

#ifdef SPQOS_DEBUG
//---------------------------------------------------------------------------
//	@function:
//		CConstraintTest::EresUnittest_NegativeTests
//
//	@doc:
//		Tests for unconstrainable types.
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CConstraintTest::EresUnittest_NegativeTests()
{
	// create memory pool
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	// setup a file-based provider
	CMDProviderMemory *pmdp = CTestUtils::m_pmdpf;
	pmdp->AddRef();

	// we need to use an auto pointer for the cache here to ensure
	// deleting memory of cached objects when we throw
	CAutoP<CMDAccessor::MDCache> apcache;
	apcache =
		CCacheFactory::CreateCache<spqopt::IMDCacheObject *, spqopt::CMDKey *>(
			true,  // fUnique
			0 /* unlimited cache quota */, CMDKey::UlHashMDKey,
			CMDKey::FEqualMDKey);

	CMDAccessor::MDCache *pcache = apcache.Value();

	CMDAccessor mda(mp, pcache, CTestUtils::m_sysidDefault, pmdp);

	CConstExprEvaluatorForDates *pceeval =
		SPQOS_NEW(mp) CConstExprEvaluatorForDates(mp);

	// install opt context in TLS
	CAutoOptCtxt aoc(mp, &mda, pceeval, CTestUtils::GetCostModel(mp));
	SPQOS_ASSERT(NULL != COptCtxt::PoctxtFromTLS()->Pcomp());

	const IMDType *pmdtype = mda.RetrieveType(&CMDIdSPQDB::m_mdid_text);
	CWStringConst str(SPQOS_WSZ_LIT("text_col"));
	CName name(mp, &str);
	CAutoP<CColRef> colref(COptCtxt::PoctxtFromTLS()->Pcf()->PcrCreate(
		pmdtype, default_type_modifier, name));

	// create a text interval: ['baz', 'foobar')
	CAutoP<CWStringDynamic> pstrLower1(
		SPQOS_NEW(mp) CWStringDynamic(mp, SPQOS_WSZ_LIT("AAAAB2Jheg==")));
	CAutoP<CWStringDynamic> pstrUpper1(
		SPQOS_NEW(mp) CWStringDynamic(mp, SPQOS_WSZ_LIT("AAAACmZvb2Jhcg==")));
	const LINT lLower1 = 571163436;
	const LINT lUpper1 = 322061118;

	// 'text' is not a constrainable type, so the interval construction should assert-fail
	CConstraintInterval *pciFirst = CTestUtils::PciGenericInterval(
		mp, &mda, CMDIdSPQDB::m_mdid_text, colref.Value(), pstrLower1.Value(),
		lLower1, CRange::EriIncluded, pstrUpper1.Value(), lUpper1,
		CRange::EriExcluded);
	PrintConstraint(mp, pciFirst);
	pciFirst->Release();

	return SPQOS_OK;
}
#endif	// SPQOS_DEBUG

//---------------------------------------------------------------------------
//	@function:
//		CConstraintTest::EresUnittest_ConstraintsOnDates
//	@doc:
//		Test constraints on date intervals.
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CConstraintTest::EresUnittest_ConstraintsOnDates()
{
	CAutoTraceFlag atf(EopttraceEnableConstantExpressionEvaluation,
					   true /*value*/);

	// create memory pool
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	// setup a file-based provider
	CMDProviderMemory *pmdp = CTestUtils::m_pmdpf;
	pmdp->AddRef();
	CMDAccessor mda(mp, CMDCache::Pcache(), CTestUtils::m_sysidDefault, pmdp);

	CConstExprEvaluatorForDates *pceeval =
		SPQOS_NEW(mp) CConstExprEvaluatorForDates(mp);

	// install opt context in TLS
	CAutoOptCtxt aoc(mp, &mda, pceeval, CTestUtils::GetCostModel(mp));
	SPQOS_ASSERT(NULL != COptCtxt::PoctxtFromTLS()->Pcomp());

	const IMDType *pmdtype = mda.RetrieveType(&CMDIdSPQDB::m_mdid_date);
	CWStringConst str(SPQOS_WSZ_LIT("date_col"));
	CName name(mp, &str);
	CAutoP<CColRef> colref(COptCtxt::PoctxtFromTLS()->Pcf()->PcrCreate(
		pmdtype, default_type_modifier, name));

	// create a date interval: ['01-01-2012', '01-21-2012')
	CWStringDynamic pstrLowerDate1(mp, wszInternalRepresentationFor2012_01_01);
	CWStringDynamic pstrUpperDate1(mp, wszInternalRepresentationFor2012_01_21);
	CConstraintInterval *pciFirst = CTestUtils::PciGenericInterval(
		mp, &mda, CMDIdSPQDB::m_mdid_date, colref.Value(), &pstrLowerDate1,
		lInternalRepresentationFor2012_01_01, CRange::EriIncluded,
		&pstrUpperDate1, lInternalRepresentationFor2012_01_21,
		CRange::EriExcluded);
	PrintConstraint(mp, pciFirst);

	// create a date interval: ['01-02-2012', '01-22-2012')
	CWStringDynamic pstrLowerDate2(mp, wszInternalRepresentationFor2012_01_02);
	CWStringDynamic pstrUpperDate2(mp, wszInternalRepresentationFor2012_01_22);
	CConstraintInterval *pciSecond = CTestUtils::PciGenericInterval(
		mp, &mda, CMDIdSPQDB::m_mdid_date, colref.Value(), &pstrLowerDate2,
		lInternalRepresentationFor2012_01_02, CRange::EriIncluded,
		&pstrUpperDate2, lInternalRepresentationFor2012_01_22,
		CRange::EriExcluded);
	PrintConstraint(mp, pciSecond);

	// intersection
	CConstraintInterval *pciIntersect = pciFirst->PciIntersect(mp, pciSecond);
	PrintConstraint(mp, pciIntersect);
	CConstraintInterval *pciIntersectExpected = CTestUtils::PciGenericInterval(
		mp, &mda, CMDIdSPQDB::m_mdid_date, colref.Value(), &pstrLowerDate2,
		lInternalRepresentationFor2012_01_02, CRange::EriIncluded,
		&pstrUpperDate1, lInternalRepresentationFor2012_01_21,
		CRange::EriExcluded);
	SPQOS_ASSERT(pciIntersectExpected->Equals(pciIntersect));

	// union
	CConstraintInterval *pciUnion = pciFirst->PciUnion(mp, pciSecond);
	PrintConstraint(mp, pciUnion);
	CConstraintInterval *pciUnionExpected = CTestUtils::PciGenericInterval(
		mp, &mda, CMDIdSPQDB::m_mdid_date, colref.Value(), &pstrLowerDate1,
		lInternalRepresentationFor2012_01_01, CRange::EriIncluded,
		&pstrUpperDate2, lInternalRepresentationFor2012_01_22,
		CRange::EriExcluded);
	SPQOS_ASSERT(pciUnionExpected->Equals(pciUnion));

	// difference between pciFirst and pciSecond
	CConstraintInterval *pciDiff1 = pciFirst->PciDifference(mp, pciSecond);
	PrintConstraint(mp, pciDiff1);
	CConstraintInterval *pciDiff1Expected = CTestUtils::PciGenericInterval(
		mp, &mda, CMDIdSPQDB::m_mdid_date, colref.Value(), &pstrLowerDate1,
		lInternalRepresentationFor2012_01_01, CRange::EriIncluded,
		&pstrLowerDate2, lInternalRepresentationFor2012_01_02,
		CRange::EriExcluded);
	SPQOS_ASSERT(pciDiff1Expected->Equals(pciDiff1));

	// difference between pciSecond and pciFirst
	CConstraintInterval *pciDiff2 = pciSecond->PciDifference(mp, pciFirst);
	PrintConstraint(mp, pciDiff2);
	CConstraintInterval *pciDiff2Expected = CTestUtils::PciGenericInterval(
		mp, &mda, CMDIdSPQDB::m_mdid_date, colref.Value(), &pstrUpperDate1,
		lInternalRepresentationFor2012_01_21, CRange::EriIncluded,
		&pstrUpperDate2, lInternalRepresentationFor2012_01_22,
		CRange::EriExcluded);
	SPQOS_ASSERT(pciDiff2Expected->Equals(pciDiff2));

	// containment
	SPQOS_ASSERT(!pciFirst->Contains(pciSecond));
	SPQOS_ASSERT(pciFirst->Contains(pciDiff1));
	SPQOS_ASSERT(!pciSecond->Contains(pciFirst));
	SPQOS_ASSERT(pciSecond->Contains(pciDiff2));
	SPQOS_ASSERT(pciFirst->Contains(pciFirst));
	SPQOS_ASSERT(pciSecond->Contains(pciSecond));

	// equality
	// create a third interval identical to the first
	CConstraintInterval *pciThird = CTestUtils::PciGenericInterval(
		mp, &mda, CMDIdSPQDB::m_mdid_date, colref.Value(), &pstrLowerDate1,
		lInternalRepresentationFor2012_01_01, CRange::EriIncluded,
		&pstrUpperDate1, lInternalRepresentationFor2012_01_21,
		CRange::EriExcluded);
	SPQOS_ASSERT(!pciFirst->Equals(pciSecond));
	SPQOS_ASSERT(!pciFirst->Equals(pciDiff1));
	SPQOS_ASSERT(!pciSecond->Equals(pciDiff2));
	SPQOS_ASSERT(pciFirst->Equals(pciThird));

	pciThird->Release();
	pciDiff2Expected->Release();
	pciDiff1Expected->Release();
	pciUnionExpected->Release();
	pciIntersectExpected->Release();
	pciDiff2->Release();
	pciDiff1->Release();
	pciUnion->Release();
	pciIntersect->Release();
	pciSecond->Release();
	pciFirst->Release();

	return SPQOS_OK;
}

// EOF
