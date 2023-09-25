//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2008 Greenplum, Inc.
//
//	@filename:
//		CAutoPTest.cpp
//
//	@doc:
//		Tests for CAutoP
//---------------------------------------------------------------------------

#include "unittest/spqos/common/CAutoPTest.h"

#include "spqos/base.h"
#include "spqos/common/CAutoP.h"
#include "spqos/memory/CAutoMemoryPool.h"
#include "spqos/test/CUnittest.h"

using namespace spqos;


//---------------------------------------------------------------------------
//	@function:
//		CAutoP::EresUnittest
//
//	@doc:
//		Unittest for auto pointers
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CAutoPTest::EresUnittest()
{
	CUnittest rgut[] = {SPQOS_UNITTEST_FUNC(CAutoPTest::EresUnittest_Basics)};

	return CUnittest::EresExecute(rgut, SPQOS_ARRAY_SIZE(rgut));
}


//---------------------------------------------------------------------------
//	@function:
//		CAutoPTest::EresUnittest_Basics
//
//	@doc:
//		Various basic operations
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CAutoPTest::EresUnittest_Basics()
{
	// create memory pool
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	// assignment
	CAutoP<CHAR> asz;
	CHAR *sz = SPQOS_NEW(mp) CHAR;
	asz = sz;

	CAutoP<CHAR> asz2;
	CAutoP<CHAR> asz3;
	CHAR *sz2 = SPQOS_NEW(mp) CHAR;

	*sz2 = '\0';
	asz2 = sz2;

	// default assignment
	asz3 = asz2;

	// accessor
#ifdef SPQOS_DEBUG
	CHAR *szBack = asz3.Value();
	SPQOS_ASSERT(szBack == sz2);
#endif	// SPQOS_DEBUG

	// deref
	SPQOS_ASSERT(*sz2 == *asz3);

	// wipe out asz2 to prevent double free
	asz2 = NULL;

	// unhooking of object
	SPQOS_DELETE(asz3.Reset());

	CElem *pelem = SPQOS_NEW(mp) CElem;
	pelem->m_ul = 3;

	CAutoP<CElem> aelem;
	aelem = pelem;

	// deref
	SPQOS_ASSERT(pelem->m_ul == aelem->m_ul);

	// c'tor
	CAutoP<CHAR> asz4(SPQOS_NEW(mp) CHAR);
	*(asz4.Value()) = 'a';

	return SPQOS_OK;
}

// EOF
