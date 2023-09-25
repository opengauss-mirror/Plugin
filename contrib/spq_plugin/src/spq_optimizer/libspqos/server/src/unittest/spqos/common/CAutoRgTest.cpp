//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2008 Greenplum, Inc.
//
//	@filename:
//		CAutoRgTest.cpp
//
//	@doc:
//		Tests for CAutoRg
//---------------------------------------------------------------------------

#include "unittest/spqos/common/CAutoRgTest.h"

#include "spqos/base.h"
#include "spqos/common/CAutoRg.h"
#include "spqos/memory/CAutoMemoryPool.h"
#include "spqos/test/CUnittest.h"

using namespace spqos;


//---------------------------------------------------------------------------
//	@function:
//		CAutoRg::EresUnittest
//
//	@doc:
//		Unittest for bit vectors
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CAutoRgTest::EresUnittest()
{
	CUnittest rgut[] = {SPQOS_UNITTEST_FUNC(CAutoRgTest::EresUnittest_Basics)};

	return CUnittest::EresExecute(rgut, SPQOS_ARRAY_SIZE(rgut));
}


//---------------------------------------------------------------------------
//	@function:
//		CAutoRgTest::EresUnittest_Basics
//
//	@doc:
//		Various basic operations
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CAutoRgTest::EresUnittest_Basics()
{
	// create memory pool
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	CAutoRg<CHAR> asz;
	CHAR *sz = SPQOS_NEW_ARRAY(mp, CHAR, 1234);
	asz = sz;

	CAutoRg<CHAR> asz2;
	CAutoRg<CHAR> asz3;
	CHAR *sz2 = SPQOS_NEW_ARRAY(mp, CHAR, 1234);

	asz2 = sz2;
	asz3 = asz2;

#ifdef SPQOS_DEBUG
	CHAR ch = asz3[0];
	SPQOS_ASSERT(ch == sz2[0]);
#endif	// SPQOS_DEBUG

	asz2 = NULL;
	SPQOS_DELETE_ARRAY(asz3.RgtReset());

	// ctor
	CAutoRg<CHAR> asz4(SPQOS_NEW_ARRAY(mp, CHAR, 1234));

	return SPQOS_OK;
}

// EOF
