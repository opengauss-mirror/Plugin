//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		CColRefSetTest.cpp
//
//	@doc:
//		Tests for CColRefSet
//---------------------------------------------------------------------------
#include "unittest/spqopt/base/CColRefSetTest.h"

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


//---------------------------------------------------------------------------
//	@function:
//		CColRefSetTest::EresUnittest
//
//	@doc:
//		Unittest for bit vectors
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CColRefSetTest::EresUnittest()
{
	CUnittest rgut[] = {
		SPQOS_UNITTEST_FUNC(CColRefSetTest::EresUnittest_Basics)};

	return CUnittest::EresExecute(rgut, SPQOS_ARRAY_SIZE(rgut));
}

//---------------------------------------------------------------------------
//	@function:
//		CColRefSetTest::EresUnittest_Basics
//
//	@doc:
//		Very basic tests; setops tested in context of CBitSet already
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CColRefSetTest::EresUnittest_Basics()
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

	pcrsTwo->Release();
	pcrs->Release();

	return SPQOS_OK;
}


// EOF
