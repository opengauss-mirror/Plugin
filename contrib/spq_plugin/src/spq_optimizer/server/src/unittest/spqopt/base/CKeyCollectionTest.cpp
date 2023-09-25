//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 - 2012 EMC Corp.
//
//	@filename:
//		CKeyCollectionTest.cpp
//
//	@doc:
//		Tests for CKeyCollectionTest
//---------------------------------------------------------------------------
#include "unittest/spqopt/base/CKeyCollectionTest.h"

#include "spqopt/base/CKeyCollection.h"
#include "spqopt/eval/CConstExprEvaluatorDefault.h"
#include "spqopt/mdcache/CMDAccessor.h"
#include "spqopt/mdcache/CMDCache.h"
#include "spqopt/metadata/CName.h"
#include "naucrates/md/CMDProviderMemory.h"
#include "naucrates/md/IMDTypeInt4.h"

#include "unittest/base.h"
#include "unittest/spqopt/CTestUtils.h"


using namespace spqopt;

//---------------------------------------------------------------------------
//	@function:
//		CKeyCollectionTest::EresUnittest
//
//	@doc:
//		Unittest for key collections
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CKeyCollectionTest::EresUnittest()
{
	CUnittest rgut[] = {
		SPQOS_UNITTEST_FUNC(CKeyCollectionTest::EresUnittest_Basics),
		SPQOS_UNITTEST_FUNC(CKeyCollectionTest::EresUnittest_Subsumes)};

	return CUnittest::EresExecute(rgut, SPQOS_ARRAY_SIZE(rgut));
}

//---------------------------------------------------------------------------
//	@function:
//		CKeyCollectionTest::EresUnittest_Basics
//
//	@doc:
//		Basic test for key collections
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CKeyCollectionTest::EresUnittest_Basics()
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

	// create test set
	CWStringConst strName(SPQOS_WSZ_LIT("Test Column"));
	CName name(&strName);
	const IMDTypeInt4 *pmdtypeint4 = mda.PtMDType<IMDTypeInt4>();

	CKeyCollection *pkc = SPQOS_NEW(mp) CKeyCollection(mp);

	const ULONG num_cols = 10;
	for (ULONG i = 0; i < num_cols; i++)
	{
		CColRef *colref =
			col_factory->PcrCreate(pmdtypeint4, default_type_modifier, name);
		pcrs->Include(colref);
	}

	pkc->Add(pcrs);
	SPQOS_ASSERT(pkc->FKey(pcrs));

	CColRefArray *colref_array = pkc->PdrspqcrKey(mp);
	SPQOS_ASSERT(pkc->FKey(mp, colref_array));

	pcrs->Include(colref_array);
	SPQOS_ASSERT(pkc->FKey(pcrs));

	colref_array->Release();

	pkc->Release();

	return SPQOS_OK;
}

//---------------------------------------------------------------------------
//	@function:
//		CKeyCollectionTest::EresUnittest_Subsumes
//
//	@doc:
//		Basic test for triming key collections
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CKeyCollectionTest::EresUnittest_Subsumes()
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

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

	// create test set
	CWStringConst strName(SPQOS_WSZ_LIT("Test Column"));
	CName name(&strName);
	const IMDTypeInt4 *pmdtypeint4 = mda.PtMDType<IMDTypeInt4>();

	CKeyCollection *pkc = SPQOS_NEW(mp) CKeyCollection(mp);

	CColRefSet *pcrs0 = SPQOS_NEW(mp) CColRefSet(mp);
	CColRefSet *pcrs1 = SPQOS_NEW(mp) CColRefSet(mp);
	CColRefSet *pcrs2 = SPQOS_NEW(mp) CColRefSet(mp);

	const ULONG num_cols = 10;
	const ULONG ulLen1 = 3;
	for (ULONG ul = 0; ul < num_cols; ul++)
	{
		CColRef *colref =
			col_factory->PcrCreate(pmdtypeint4, default_type_modifier, name);
		pcrs0->Include(colref);

		if (ul < ulLen1)
		{
			pcrs1->Include(colref);
		}

		if (ul == 0)
		{
			pcrs2->Include(colref);
		}
	}

	pkc->Add(pcrs0);
	pkc->Add(pcrs1);
	pkc->Add(pcrs2);

	SPQOS_ASSERT(pkc->FKey(pcrs2));

	// get the second key
	CColRefArray *colref_array = pkc->PdrspqcrKey(mp, 1);
	SPQOS_ASSERT(ulLen1 == colref_array->Size());
	SPQOS_ASSERT(pkc->FKey(mp, colref_array));

	// get the subsumed key
	CColRefArray *pdrspqcrSubsumed = pkc->PdrspqcrTrim(mp, colref_array);
	SPQOS_ASSERT(colref_array->Size() >= pdrspqcrSubsumed->Size());

	CColRefSet *pcrsSubsumed = SPQOS_NEW(mp) CColRefSet(mp);
	pcrsSubsumed->Include(colref_array);

#ifdef SPQOS_DEBUG
	const ULONG ulLenSubsumed = pdrspqcrSubsumed->Size();
	for (ULONG ul = 0; ul < ulLenSubsumed; ul++)
	{
		CColRef *colref = (*pdrspqcrSubsumed)[ul];
		SPQOS_ASSERT(pcrsSubsumed->FMember(colref));
	}
#endif	// SPQOS_DEBUG

	pcrsSubsumed->Release();
	colref_array->Release();
	pdrspqcrSubsumed->Release();
	pkc->Release();

	return SPQOS_OK;
}
// EOF
