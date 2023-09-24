//---------------------------------------------------------------------------
//	Pivotal Software, Inc
//	Copyright (C) 2017 Pivotal Software, Inc
//---------------------------------------------------------------------------
#include "unittest/spqopt/base/CEquivalenceClassesTest.h"

#include "spqopt/base/CColRefSet.h"
#include "spqopt/base/CColRefSetIter.h"
#include "spqopt/base/CColumnFactory.h"
#include "spqopt/base/CQueryContext.h"
#include "spqopt/eval/CConstExprEvaluatorDefault.h"
#include "spqopt/mdcache/CMDCache.h"
#include "naucrates/md/CMDProviderMemory.h"
#include "naucrates/md/IMDTypeInt4.h"

#include "unittest/base.h"
#include "unittest/spqopt/CTestUtils.h"
#include "unittest/spqopt/translate/CTranslatorExprToDXLTest.h"


// Unittest for bit vectors
SPQOS_RESULT
CEquivalenceClassesTest::EresUnittest()
{
	CUnittest rgut[] = {
		SPQOS_UNITTEST_FUNC(CEquivalenceClassesTest::
							   EresUnittest_NotDisjointEquivalanceClasses),
		SPQOS_UNITTEST_FUNC(
			CEquivalenceClassesTest::EresUnittest_IntersectEquivalanceClasses)};

	return CUnittest::EresExecute(rgut, SPQOS_ARRAY_SIZE(rgut));
}

// Check disjoint equivalence classes are detected
SPQOS_RESULT
CEquivalenceClassesTest::EresUnittest_NotDisjointEquivalanceClasses()
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	CColRefSet *pcrs = SPQOS_NEW(mp) CColRefSet(mp);

	// Setup an MD cache with a file-based provider
	CMDProviderMemory *pmdp = CTestUtils::m_pmdpf;
	pmdp->AddRef();
	CMDAccessor mda(mp, CMDCache::Pcache());
	mda.RegisterProvider(CTestUtils::m_sysidDefault, pmdp);

	// install opt context in TLS
	CAutoOptCtxt aoc(mp, &mda, NULL, /* pceeval */
					 CTestUtils::GetCostModel(mp));

	// get column factory from optimizer context object
	CColumnFactory *col_factory = COptCtxt::PoctxtFromTLS()->Pcf();

	CWStringConst strName(SPQOS_WSZ_LIT("Test Column"));
	CName name(&strName);

	const IMDTypeInt4 *pmdtypeint4 = mda.PtMDType<IMDTypeInt4>();

	ULONG num_cols = 10;
	for (ULONG i = 0; i < num_cols; i++)
	{
		CColRef *colref =
			col_factory->PcrCreate(pmdtypeint4, default_type_modifier, name);
		pcrs->Include(colref);

		SPQOS_ASSERT(pcrs->FMember(colref));
	}

	SPQOS_ASSERT(pcrs->Size() == num_cols);

	CColRefSet *pcrsTwo = SPQOS_NEW(mp) CColRefSet(mp, *pcrs);
	SPQOS_ASSERT(pcrsTwo->Size() == num_cols);

	CColRefSet *pcrsThree = SPQOS_NEW(mp) CColRefSet(mp);
	SPQOS_ASSERT(pcrsThree->Size() == 0);
	CColRef *pcrThree =
		col_factory->PcrCreate(pmdtypeint4, default_type_modifier, name);
	pcrsThree->Include(pcrThree);
	SPQOS_ASSERT(pcrsThree->Size() == 1);

	CColRefSetArray *pdrspqcrs = SPQOS_NEW(mp) CColRefSetArray(mp);
	pcrs->AddRef();
	pcrsTwo->AddRef();
	pdrspqcrs->Append(pcrs);
	pdrspqcrs->Append(pcrsTwo);
	SPQOS_ASSERT(!CUtils::FEquivalanceClassesDisjoint(mp, pdrspqcrs));

	CColRefSetArray *pdrspqcrsTwo = SPQOS_NEW(mp) CColRefSetArray(mp);
	pcrs->AddRef();
	pcrsThree->AddRef();
	pdrspqcrsTwo->Append(pcrs);
	pdrspqcrsTwo->Append(pcrsThree);
	SPQOS_ASSERT(CUtils::FEquivalanceClassesDisjoint(mp, pdrspqcrsTwo));

	pcrsThree->Release();
	pcrsTwo->Release();
	pcrs->Release();
	pdrspqcrs->Release();
	pdrspqcrsTwo->Release();

	return SPQOS_OK;
}

// Check disjoint equivalence classes are detected
SPQOS_RESULT
CEquivalenceClassesTest::EresUnittest_IntersectEquivalanceClasses()
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	CColRefSet *pcrs = SPQOS_NEW(mp) CColRefSet(mp);

	// Setup an MD cache with a file-based provider
	CMDProviderMemory *pmdp = CTestUtils::m_pmdpf;
	pmdp->AddRef();
	CMDAccessor mda(mp, CMDCache::Pcache());
	mda.RegisterProvider(CTestUtils::m_sysidDefault, pmdp);

	// install opt context in TLS
	CAutoOptCtxt aoc(mp, &mda, NULL, /* pceeval */
					 CTestUtils::GetCostModel(mp));

	// get column factory from optimizer context object
	CColumnFactory *col_factory = COptCtxt::PoctxtFromTLS()->Pcf();

	CWStringConst strName(SPQOS_WSZ_LIT("Test Column"));
	CName name(&strName);

	const IMDTypeInt4 *pmdtypeint4 = mda.PtMDType<IMDTypeInt4>();

	ULONG num_cols = 10;
	for (ULONG i = 0; i < num_cols; i++)
	{
		CColRef *colref =
			col_factory->PcrCreate(pmdtypeint4, default_type_modifier, name);
		pcrs->Include(colref);

		SPQOS_ASSERT(pcrs->FMember(colref));
	}

	SPQOS_ASSERT(pcrs->Size() == num_cols);

	// Generate equivalence classes
	INT setBoundaryFirst[] = {2, 5, 7};
	CColRefSetArray *pdrspqFirst =
		CTestUtils::createEquivalenceClasses(mp, pcrs, setBoundaryFirst);

	INT setBoundarySecond[] = {1, 4, 5, 6};
	CColRefSetArray *pdrspqSecond =
		CTestUtils::createEquivalenceClasses(mp, pcrs, setBoundarySecond);

	INT setBoundaryExpected[] = {1, 2, 4, 5, 6, 7};
	CColRefSetArray *pdrspqIntersectExpectedOp =
		CTestUtils::createEquivalenceClasses(mp, pcrs, setBoundaryExpected);

	CColRefSetArray *pdrspqResult =
		CUtils::PdrspqcrsIntersectEquivClasses(mp, pdrspqFirst, pdrspqSecond);
	SPQOS_ASSERT(CUtils::FEquivalanceClassesDisjoint(mp, pdrspqResult));
	SPQOS_ASSERT(CUtils::FEquivalanceClassesEqual(mp, pdrspqResult,
												 pdrspqIntersectExpectedOp));

	pcrs->Release();
	pdrspqFirst->Release();
	pdrspqResult->Release();
	pdrspqSecond->Release();
	pdrspqIntersectExpectedOp->Release();

	return SPQOS_OK;
}
// EOF
