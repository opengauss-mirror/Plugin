//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		CColRefSetIterTest.cpp
//
//	@doc:
//		Test of ColRefSet iterator
//---------------------------------------------------------------------------

#include "unittest/spqopt/base/CColRefSetIterTest.h"

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
//		CColRefSetIterTest::EresUnittest
//
//	@doc:
//		Unittest for bit vectors
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CColRefSetIterTest::EresUnittest()
{
	CUnittest rgut[] = {
		SPQOS_UNITTEST_FUNC(CColRefSetIterTest::EresUnittest_Basics)};

	return CUnittest::EresExecute(rgut, SPQOS_ARRAY_SIZE(rgut));
}

//---------------------------------------------------------------------------
//	@function:
//		CColRefSetIterTest::EresUnittest_Basics
//
//	@doc:
//		Testing ctors/dtor; and colref decoding;
//		Other functionality already tested in vanilla CBitSetIter;
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CColRefSetIterTest::EresUnittest_Basics()
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	// Setup an MD cache with a file-based provider
	CMDProviderMemory *pmdp = CTestUtils::m_pmdpf;
	pmdp->AddRef();
	CMDAccessor mda(mp, CMDCache::Pcache());
	mda.RegisterProvider(CTestUtils::m_sysidDefault, pmdp);

	// install opt context in TLS
	CAutoOptCtxt aoc(mp, &mda, NULL /* pceeval */,
					 CTestUtils::GetCostModel(mp));

	// get column factory from optimizer context object
	CColumnFactory *col_factory = COptCtxt::PoctxtFromTLS()->Pcf();

	CColRefSet *pcrs = SPQOS_NEW(mp) CColRefSet(mp);
	CWStringConst strName(SPQOS_WSZ_LIT("Test Column"));
	CName name(&strName);

	// create a int4 datum
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

	ULONG count = 0;
	CColRefSetIter crsi(*pcrs);
	while (crsi.Advance())
	{
		SPQOS_ASSERT((BOOL) crsi);

		CColRef *colref = crsi.Pcr();
		SPQOS_ASSERT(colref->Name().Equals(name));

		// to avoid unused variable warnings
		(void) colref->Id();

		count++;
	}

	SPQOS_ASSERT(num_cols == count);
	SPQOS_ASSERT(!((BOOL) crsi));

	pcrs->Release();

	return SPQOS_OK;
}

// EOF
