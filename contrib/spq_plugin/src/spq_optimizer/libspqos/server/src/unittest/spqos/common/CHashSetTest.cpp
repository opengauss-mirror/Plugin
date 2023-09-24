//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2015 Pivotal Inc.
//
//	@filename:
//		CHashSetTest.cpp
//
//	@doc:
//		Test for CHashSet
//
//	@owner:
//		solimm1
//
//	@test:
//
//
//---------------------------------------------------------------------------

#include "unittest/spqos/common/CHashSetTest.h"

#include "spqos/base.h"
#include "spqos/common/CHashSet.h"
#include "spqos/memory/CAutoMemoryPool.h"
#include "spqos/test/CUnittest.h"

using namespace spqos;

//---------------------------------------------------------------------------
//	@function:
//		CHashSetTest::EresUnittest
//
//	@doc:
//		Unittest for basic hash set
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CHashSetTest::EresUnittest()
{
	CUnittest rgut[] = {
		SPQOS_UNITTEST_FUNC(CHashSetTest::EresUnittest_Basic),
		SPQOS_UNITTEST_FUNC(CHashSetTest::EresUnittest_Ownership),
	};

	return CUnittest::EresExecute(rgut, SPQOS_ARRAY_SIZE(rgut));
}


//---------------------------------------------------------------------------
//	@function:
//		CHashSetTest::EresUnittest_Basic
//
//	@doc:
//		Basic insertion/lookup test
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CHashSetTest::EresUnittest_Basic()
{
	// create memory pool
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	// test with ULONG array
	ULONG_PTR rgul[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

	const ULONG ulCnt = SPQOS_ARRAY_SIZE(rgul);

	typedef CHashSet<ULONG_PTR, HashPtr<ULONG_PTR>, Equals<ULONG_PTR>,
					 CleanupNULL<ULONG_PTR> >
		UlongPtrHashSet;

	UlongPtrHashSet *phs = SPQOS_NEW(mp) UlongPtrHashSet(mp, 128);
	for (ULONG ul = 0; ul < ulCnt; ul++)
	{
#ifdef SPQOS_DEBUG
		BOOL fSuccess =
#endif	// SPQOS_DEBUG
			phs->Insert(&rgul[ul]);
		SPQOS_ASSERT(fSuccess);
	}
	SPQOS_ASSERT(ulCnt == phs->Size());

	for (ULONG ul = 0; ul < ulCnt; ul++)
	{
		SPQOS_ASSERT(phs->Contains(&rgul[ul]));
	}

	phs->Release();

	return SPQOS_OK;
}


//---------------------------------------------------------------------------
//	@function:
//		CHashSetTest::EresUnittest_Ownership
//
//	@doc:
//		Basic hash set test with ownership
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CHashSetTest::EresUnittest_Ownership()
{
	// create memory pool
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	ULONG ulCnt = 256;

	typedef CHashSet<ULONG_PTR, HashPtr<ULONG_PTR>, Equals<ULONG_PTR>,
					 CleanupDelete<ULONG_PTR> >
		UlongPtrHashSet;

	UlongPtrHashSet *phs = SPQOS_NEW(mp) UlongPtrHashSet(mp, 32);
	for (ULONG ul = 0; ul < ulCnt; ul++)
	{
		ULONG_PTR *pulp = SPQOS_NEW(mp) ULONG_PTR(ul);

#ifdef SPQOS_DEBUG
		BOOL fSuccess =
#endif	// SPQOS_DEBUG
			phs->Insert(pulp);

		SPQOS_ASSERT(fSuccess);
		SPQOS_ASSERT(phs->Contains(pulp));

		// can't insert existing keys
		SPQOS_ASSERT(!phs->Insert(pulp));
	}

	phs->Release();

	return SPQOS_OK;
}

// EOF
