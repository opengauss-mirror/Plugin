//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		CAutoRefTest.cpp
//
//	@doc:
//		Tests for CAutoRef
//---------------------------------------------------------------------------

#include "unittest/spqos/common/CAutoRefTest.h"

#include "spqos/base.h"
#include "spqos/common/CAutoRef.h"
#include "spqos/memory/CAutoMemoryPool.h"
#include "spqos/test/CUnittest.h"

using namespace spqos;


//---------------------------------------------------------------------------
//	@function:
//		CAutoRef::EresUnittest
//
//	@doc:
//		Unittest for reference counted auto pointers
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CAutoRefTest::EresUnittest()
{
	CUnittest rgut[] = {SPQOS_UNITTEST_FUNC(CAutoRefTest::EresUnittest_Basics)};

	return CUnittest::EresExecute(rgut, SPQOS_ARRAY_SIZE(rgut));
}


//---------------------------------------------------------------------------
//	@function:
//		CAutoRefTest::EresUnittest_Basics
//
//	@doc:
//		Various basic operations
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CAutoRefTest::EresUnittest_Basics()
{
	// create memory pool
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	// assignment
	CAutoRef<CElem> aelem;
	CElem *pelem = SPQOS_NEW(mp) CElem(0);
	aelem = pelem;

	SPQOS_ASSERT(aelem->m_ul == pelem->m_ul);
	SPQOS_ASSERT(&aelem->m_ul == &pelem->m_ul);

#ifdef SPQOS_DEBUG
	CElem *pelem2 = &(*pelem);
	SPQOS_ASSERT(pelem2 == pelem);
#endif	// SPQOS_DEBUG

	// hand reference over to other auto ref count
	CAutoRef<CElem> aelem2;
	aelem2 = aelem.Reset();

	// c'tor
	CAutoRef<CElem> aelem3(SPQOS_NEW(mp) CElem(10));
	SPQOS_ASSERT(aelem3->m_ul == ULONG(10));

	return SPQOS_OK;
}

// EOF
