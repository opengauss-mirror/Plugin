//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		CColumnDescriptorTest.cpp
//
//	@doc:
//		Test for CColumnDescriptor
//---------------------------------------------------------------------------


#include "unittest/spqopt/metadata/CColumnDescriptorTest.h"

#include "spqos/base.h"
#include "spqos/memory/CAutoMemoryPool.h"
#include "spqos/test/CUnittest.h"

#include "spqopt/base/CAutoOptCtxt.h"
#include "spqopt/base/COptCtxt.h"
#include "spqopt/base/CQueryContext.h"
#include "spqopt/mdcache/CMDAccessor.h"
#include "spqopt/mdcache/CMDCache.h"
#include "spqopt/metadata/CColumnDescriptor.h"
#include "naucrates/md/CMDProviderMemory.h"
#include "naucrates/md/IMDTypeInt4.h"

#include "unittest/spqopt/CTestUtils.h"
#include "unittest/spqopt/translate/CTranslatorExprToDXLTest.h"

using namespace spqopt;

//---------------------------------------------------------------------------
//	@function:
//		CColumnDescriptorTest::EresUnittest
//
//	@doc:
//		Unittest column descriptors
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CColumnDescriptorTest::EresUnittest()
{
	CUnittest rgut[] = {
		SPQOS_UNITTEST_FUNC(CColumnDescriptorTest::EresUnittest_Basic)};

	return CUnittest::EresExecute(rgut, SPQOS_ARRAY_SIZE(rgut));
}


//---------------------------------------------------------------------------
//	@function:
//		CColumnDescriptorTest::EresUnittest_Basic
//
//	@doc:
//		basic naming
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CColumnDescriptorTest::EresUnittest_Basic()
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	// Setup an MD cache with a file-based provider
	CMDProviderMemory *pmdp = CTestUtils::m_pmdpf;
	pmdp->AddRef();

	CMDAccessor mda(mp, CMDCache::Pcache(), CTestUtils::m_sysidDefault, pmdp);

	const IMDTypeInt4 *pmdtypeint4 =
		mda.PtMDType<IMDTypeInt4>(CTestUtils::m_sysidDefault);

	CWStringConst strName(SPQOS_WSZ_LIT("column desc test"));
	CName name(&strName);
	CColumnDescriptor *pcdesc = SPQOS_NEW(mp) CColumnDescriptor(
		mp, pmdtypeint4, default_type_modifier, name, 1, false /*IsNullable*/);

	SPQOS_ASSERT(name.Equals(pcdesc->Name()));

	SPQOS_ASSERT(1 == pcdesc->AttrNum());

	pcdesc->Release();

	return SPQOS_OK;
}


// EOF
