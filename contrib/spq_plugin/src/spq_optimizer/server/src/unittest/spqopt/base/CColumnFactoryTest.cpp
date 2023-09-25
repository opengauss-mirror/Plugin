//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2008, 2009 Greenplum, Inc.
//
//	@filename:
//		CColumnFactoryTest.cpp
//
//	@doc:
//		Test for CColumnFactory
//---------------------------------------------------------------------------

#include "unittest/spqopt/base/CColumnFactoryTest.h"

#include "spqos/base.h"
#include "spqos/memory/CAutoMemoryPool.h"
#include "spqos/test/CUnittest.h"

#include "spqopt/base/CColumnFactory.h"
#include "spqopt/base/COptCtxt.h"
#include "spqopt/base/CQueryContext.h"
#include "spqopt/mdcache/CMDAccessor.h"
#include "spqopt/mdcache/CMDCache.h"
#include "naucrates/md/CMDProviderMemory.h"
#include "naucrates/md/IMDTypeInt4.h"

#include "unittest/spqopt/CTestUtils.h"
#include "unittest/spqopt/translate/CTranslatorExprToDXLTest.h"

using namespace spqopt;

//---------------------------------------------------------------------------
//	@function:
//		CColumnFactoryTest::EresUnittest
//
//	@doc:
//		Unittest for column factory
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CColumnFactoryTest::EresUnittest()
{
	CUnittest rgut[] = {
		SPQOS_UNITTEST_FUNC(CColumnFactoryTest::EresUnittest_Basic)};

	return CUnittest::EresExecute(rgut, SPQOS_ARRAY_SIZE(rgut));
}


//---------------------------------------------------------------------------
//	@function:
//		CColumnFactoryTest::EresUnittest_Basic
//
//	@doc:
//		Basic array allocation test
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CColumnFactoryTest::EresUnittest_Basic()
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	CMDProviderMemory *pmdp = CTestUtils::m_pmdpf;
	pmdp->AddRef();
	CMDAccessor mda(mp, CMDCache::Pcache());
	mda.RegisterProvider(CTestUtils::m_sysidDefault, pmdp);

	const IMDTypeInt4 *pmdtypeint4 = mda.PtMDType<IMDTypeInt4>();

	CColumnFactory cf;

	// typed colref
	CColRef *pcrOne = cf.PcrCreate(pmdtypeint4, default_type_modifier);
	SPQOS_ASSERT(pcrOne == cf.LookupColRef(pcrOne->m_id));
	cf.Destroy(pcrOne);

	// typed/named colref
	CWStringConst strName(SPQOS_WSZ_LIT("C_CustKey"));
	CColRef *pcrTwo =
		cf.PcrCreate(pmdtypeint4, default_type_modifier, CName(&strName));
	SPQOS_ASSERT(pcrTwo == cf.LookupColRef(pcrTwo->m_id));

	// clone previous colref
	CColRef *pcrThree = cf.PcrCreate(pcrTwo);
	SPQOS_ASSERT(pcrThree != cf.LookupColRef(pcrTwo->m_id));
	SPQOS_ASSERT(!pcrThree->Name().Equals(pcrTwo->Name()));
	cf.Destroy(pcrThree);

	cf.Destroy(pcrTwo);

	return SPQOS_OK;
}


// EOF
