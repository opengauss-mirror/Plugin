//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		CEnumSetTest.cpp
//
//	@doc:
//      Test for CEnumSet/CEnumSetIter
//---------------------------------------------------------------------------

#include "unittest/spqos/common/CEnumSetTest.h"

#include "spqos/base.h"
#include "spqos/common/CEnumSet.h"
#include "spqos/common/CEnumSetIter.h"
#include "spqos/memory/CAutoMemoryPool.h"
#include "spqos/test/CUnittest.h"
#include "spqos/types.h"

using namespace spqos;

//---------------------------------------------------------------------------
//	@function:
//		CEnumSetTest::EresUnittest
//
//	@doc:
//		Unittest for enum sets
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CEnumSetTest::EresUnittest()
{
	CUnittest rgut[] = {SPQOS_UNITTEST_FUNC(CEnumSetTest::EresUnittest_Basics)};

	return CUnittest::EresExecute(rgut, SPQOS_ARRAY_SIZE(rgut));
}


//---------------------------------------------------------------------------
//	@function:
//		CEnumSetTest::EresUnittest_Basics
//
//	@doc:
//		Testing ctors/dtor, accessors, iterator
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CEnumSetTest::EresUnittest_Basics()
{
	// create memory pool
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	typedef CEnumSet<eTest, eTestSentinel> CETestSet;
	typedef CEnumSetIter<eTest, eTestSentinel> CETestIter;

	CETestSet *enum_set = SPQOS_NEW(mp) CETestSet(mp);

	(void) enum_set->ExchangeSet(eTestOne);
	(void) enum_set->ExchangeSet(eTestTwo);

	SPQOS_ASSERT(enum_set->ExchangeClear(eTestTwo));
	SPQOS_ASSERT(!enum_set->ExchangeSet(eTestTwo));

	CETestIter type_info(*enum_set);
	while (type_info.Advance())
	{
		SPQOS_ASSERT((BOOL) type_info);
		SPQOS_ASSERT(eTestSentinel > type_info.TBit());
		SPQOS_ASSERT(enum_set->Get(type_info.TBit()));
	}

	enum_set->Release();

	return SPQOS_OK;
}

// EOF
