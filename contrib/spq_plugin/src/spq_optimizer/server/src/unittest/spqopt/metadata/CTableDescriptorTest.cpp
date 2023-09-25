//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		CTableDescriptorTest.cpp
//
//	@doc:
//		Test for CTableDescriptor
//---------------------------------------------------------------------------
#include "unittest/spqopt/metadata/CTableDescriptorTest.h"

#include "spqos/io/COstreamString.h"
#include "spqos/string/CWStringDynamic.h"

#include "spqopt/base/CColumnFactory.h"
#include "spqopt/base/CQueryContext.h"
#include "spqopt/base/CUtils.h"
#include "spqopt/eval/CConstExprEvaluatorDefault.h"
#include "spqopt/metadata/CTableDescriptor.h"
#include "naucrates/md/CMDIdSPQDB.h"
#include "naucrates/md/CMDProviderMemory.h"
#include "naucrates/md/IMDTypeInt4.h"

#include "unittest/base.h"
#include "unittest/spqopt/CTestUtils.h"
#include "unittest/spqopt/metadata/CColumnDescriptorTest.h"
#include "unittest/spqopt/translate/CTranslatorExprToDXLTest.h"

//---------------------------------------------------------------------------
//	@function:
//		CTableDescriptorTest::EresUnittest
//
//	@doc:
//		Unittest for metadata names
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CTableDescriptorTest::EresUnittest()
{
	CUnittest rgut[] = {
		SPQOS_UNITTEST_FUNC(CTableDescriptorTest::EresUnittest_Basic)};

	return CUnittest::EresExecute(rgut, SPQOS_ARRAY_SIZE(rgut));
}


//---------------------------------------------------------------------------
//	@function:
//		CTableDescriptorTest::EresUnittest_Basic
//
//	@doc:
//		basic naming, assignment thru copy constructors
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CTableDescriptorTest::EresUnittest_Basic()
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

	CWStringConst strName(SPQOS_WSZ_LIT("MyTable"));
	CMDIdSPQDB *mdid =
		SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidRel, SPQOPT_MDCACHE_TEST_OID, 1, 1);
	CTableDescriptor *ptabdesc =
		CTestUtils::PtabdescCreate(mp, 10, mdid, CName(&strName));

#ifdef SPQOS_DEBUG
	CWStringDynamic str(mp);
	COstreamString oss(&str);
	ptabdesc->OsPrint(oss);

	SPQOS_TRACE(str.GetBuffer());
#endif	// SPQOS_DEBUG

	ptabdesc->Release();

	return SPQOS_OK;
}

// EOF
