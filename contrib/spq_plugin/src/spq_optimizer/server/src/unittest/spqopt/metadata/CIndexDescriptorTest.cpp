//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CIndexDescriptorTest.cpp
//
//	@doc:
//		Test for CIndexDescriptor
//---------------------------------------------------------------------------
#include "unittest/spqopt/metadata/CIndexDescriptorTest.h"

#include "spqos/io/COstreamString.h"
#include "spqos/string/CWStringDynamic.h"

#include "spqopt/base/CQueryContext.h"
#include "spqopt/base/CUtils.h"
#include "spqopt/eval/CConstExprEvaluatorDefault.h"
#include "spqopt/metadata/CTableDescriptor.h"
#include "naucrates/md/CMDIdSPQDB.h"
#include "naucrates/md/CMDProviderMemory.h"
#include "naucrates/md/IMDIndex.h"

#include "unittest/base.h"
#include "unittest/spqopt/CTestUtils.h"


//---------------------------------------------------------------------------
//	@function:
//		CIndexDescriptorTest::EresUnittest
//
//	@doc:
//		Unittest for metadata names
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CIndexDescriptorTest::EresUnittest()
{
	CUnittest rgut[] = {
		SPQOS_UNITTEST_FUNC(CIndexDescriptorTest::EresUnittest_Basic)};

	return CUnittest::EresExecute(rgut, SPQOS_ARRAY_SIZE(rgut));
}

//---------------------------------------------------------------------------
//	@function:
//		CIndexDescriptorTest::EresUnittest_Basic
//
//	@doc:
//		Basic naming, key columns and index columns printing test
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CIndexDescriptorTest::EresUnittest_Basic()
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

	// get the index associated with the table
	const IMDRelation *pmdrel = mda.RetrieveRel(ptabdesc->MDId());
	SPQOS_ASSERT(0 < pmdrel->IndexCount());

	// create an index descriptor
	IMDId *pmdidIndex = pmdrel->IndexMDidAt(0);	 // get the first index
	const IMDIndex *pmdindex = mda.RetrieveIndex(pmdidIndex);
	CIndexDescriptor *pindexdesc =
		CIndexDescriptor::Pindexdesc(mp, ptabdesc, pmdindex);

#ifdef SPQOS_DEBUG
	CWStringDynamic str(mp);
	COstreamString oss(&str);
	pindexdesc->OsPrint(oss);

	SPQOS_TRACE(str.GetBuffer());
#endif	// SPQOS_DEBUG

	// clean up
	ptabdesc->Release();
	pindexdesc->Release();

	return SPQOS_OK;
}

// EOF
