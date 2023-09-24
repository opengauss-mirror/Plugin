//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		COrderSpecTest.cpp
//
//	@doc:
//		Tests for order specification
//---------------------------------------------------------------------------
#include "unittest/spqopt/base/COrderSpecTest.h"

#include "spqopt/base/CColumnFactory.h"
#include "spqopt/base/COrderSpec.h"
#include "spqopt/base/CQueryContext.h"
#include "spqopt/eval/CConstExprEvaluatorDefault.h"
#include "naucrates/md/CMDProviderMemory.h"
#include "naucrates/md/IMDId.h"
#include "naucrates/md/IMDTypeInt4.h"

#include "unittest/base.h"
#include "unittest/spqopt/CTestUtils.h"
#include "unittest/spqopt/translate/CTranslatorExprToDXLTest.h"

//---------------------------------------------------------------------------
//	@function:
//		COrderSpecTest::EresUnittest
//
//	@doc:
//		Unittest for order spec classes
//
//---------------------------------------------------------------------------
SPQOS_RESULT
COrderSpecTest::EresUnittest()
{
	CUnittest rgut[] = {
		SPQOS_UNITTEST_FUNC(COrderSpecTest::EresUnittest_Basics)};

	return CUnittest::EresExecute(rgut, SPQOS_ARRAY_SIZE(rgut));
}

//---------------------------------------------------------------------------
//	@function:
//		COrderSpecTest::EresUnittest_Basics
//
//	@doc:
//		Basic order spec tests
//
//---------------------------------------------------------------------------
SPQOS_RESULT
COrderSpecTest::EresUnittest_Basics()
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	// Setup an MD cache with a file-based provider
	CMDProviderMemory *pmdp = CTestUtils::m_pmdpf;
	pmdp->AddRef();
	CMDAccessor mda(mp, CMDCache::Pcache(), CTestUtils::m_sysidDefault, pmdp);

	// install opt context in TLS
	CAutoOptCtxt aoc(mp, &mda, NULL, /* pceeval */
					 CTestUtils::GetCostModel(mp));

	// get column factory from optimizer context object
	CColumnFactory *col_factory = COptCtxt::PoctxtFromTLS()->Pcf();

	CWStringConst strName(SPQOS_WSZ_LIT("Test Column"));
	CName name(&strName);

	const IMDTypeInt4 *pmdtypeint4 =
		mda.PtMDType<IMDTypeInt4>(CTestUtils::m_sysidDefault);

	CColRef *pcr1 =
		col_factory->PcrCreate(pmdtypeint4, default_type_modifier, name);
	CColRef *pcr2 =
		col_factory->PcrCreate(pmdtypeint4, default_type_modifier, name);
	CColRef *pcr3 =
		col_factory->PcrCreate(pmdtypeint4, default_type_modifier, name);


	COrderSpec *pos1 = SPQOS_NEW(mp) COrderSpec(mp);

	IMDId *pmdidInt4LT = pmdtypeint4->GetMdidForCmpType(IMDType::EcmptL);
	pmdidInt4LT->AddRef();
	pmdidInt4LT->AddRef();

	pos1->Append(pmdidInt4LT, pcr1, COrderSpec::EntFirst);
	pos1->Append(pmdidInt4LT, pcr2, COrderSpec::EntLast);

	SPQOS_ASSERT(pos1->Matches(pos1));
	SPQOS_ASSERT(pos1->FSatisfies(pos1));

	COrderSpec *pos2 = SPQOS_NEW(mp) COrderSpec(mp);
	pmdidInt4LT->AddRef();
	pmdidInt4LT->AddRef();
	pmdidInt4LT->AddRef();

	pos2->Append(pmdidInt4LT, pcr1, COrderSpec::EntFirst);
	pos2->Append(pmdidInt4LT, pcr2, COrderSpec::EntLast);
	pos2->Append(pmdidInt4LT, pcr3, COrderSpec::EntAuto);

	(void) pos1->HashValue();
	(void) pos2->HashValue();

	SPQOS_ASSERT(pos2->Matches(pos2));
	SPQOS_ASSERT(pos2->FSatisfies(pos2));


	SPQOS_ASSERT(!pos1->Matches(pos2));
	SPQOS_ASSERT(!pos2->Matches(pos1));

	SPQOS_ASSERT(pos2->FSatisfies(pos1));
	SPQOS_ASSERT(!pos1->FSatisfies(pos2));

	// iterate over the components of the order spec
	for (ULONG ul = 0; ul < pos1->UlSortColumns(); ul++)
	{
#ifdef SPQOS_DEBUG
		const CColRef *colref =
#endif	// SPQOS_DEBUG
			pos1->Pcr(ul);

		SPQOS_ASSERT(NULL != colref);

#ifdef SPQOS_DEBUG
		const IMDId *mdid =
#endif	// SPQOS_DEBUG
			pos1->GetMdIdSortOp(ul);

		SPQOS_ASSERT(mdid->IsValid());

		(void) pos1->Ent(ul);
	}

	pos1->Release();
	pos2->Release();

	return SPQOS_OK;
}


// EOF
