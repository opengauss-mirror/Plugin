//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2010 Greenplum, Inc.
//
//	@filename:
//		CHashMapTest.cpp
//
//	@doc:
//		Test for CHashMap
//---------------------------------------------------------------------------

#include "unittest/spqos/common/CHashMapTest.h"

#include "spqos/base.h"
#include "spqos/common/CHashMap.h"
#include "spqos/memory/CAutoMemoryPool.h"
#include "spqos/test/CUnittest.h"

using namespace spqos;

//---------------------------------------------------------------------------
//	@function:
//		CHashMapTest::EresUnittest
//
//	@doc:
//		Unittest for basic hash map
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CHashMapTest::EresUnittest()
{
	CUnittest rgut[] = {
		SPQOS_UNITTEST_FUNC(CHashMapTest::EresUnittest_Basic),
		SPQOS_UNITTEST_FUNC(CHashMapTest::EresUnittest_Ownership),
	};

	return CUnittest::EresExecute(rgut, SPQOS_ARRAY_SIZE(rgut));
}


//---------------------------------------------------------------------------
//	@function:
//		CHashMapTest::EresUnittest_Basic
//
//	@doc:
//		Basic insertion/lookup for hash table
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CHashMapTest::EresUnittest_Basic()
{
	// create memory pool
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	// test with CHAR array
	ULONG_PTR rgul[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
	CHAR rgsz[][5] = {"abc",  "def", "ghi", "qwe", "wer",
					  "wert", "dfg", "xcv", "zxc"};

	SPQOS_ASSERT(SPQOS_ARRAY_SIZE(rgul) == SPQOS_ARRAY_SIZE(rgsz));
	const ULONG ulCnt = SPQOS_ARRAY_SIZE(rgul);

	typedef CHashMap<ULONG_PTR, CHAR, HashPtr<ULONG_PTR>,
					 spqos::Equals<ULONG_PTR>, CleanupNULL<ULONG_PTR>,
					 CleanupNULL<CHAR> >
		UlongPtrToCharMap;

	UlongPtrToCharMap *phm = SPQOS_NEW(mp) UlongPtrToCharMap(mp, 128);
	for (ULONG i = 0; i < ulCnt; ++i)
	{
#ifdef SPQOS_DEBUG
		BOOL fSuccess =
#endif	// SPQOS_DEBUG
			phm->Insert(&rgul[i], (CHAR *) rgsz[i]);
		SPQOS_ASSERT(fSuccess);

		for (ULONG j = 0; j <= i; ++j)
		{
			SPQOS_ASSERT(rgsz[j] == phm->Find(&rgul[j]));
		}
	}
	SPQOS_ASSERT(ulCnt == phm->Size());

	// test replacing entry values of existing keys
	CHAR rgszNew[][10] = {"abc_",  "def_", "ghi_", "qwe_", "wer_",
						  "wert_", "dfg_", "xcv_", "zxc_"};
	for (ULONG i = 0; i < ulCnt; ++i)
	{
#ifdef SPQOS_DEBUG
		BOOL fSuccess =
#endif	// SPQOS_DEBUG
			phm->Replace(&rgul[i], rgszNew[i]);
		SPQOS_ASSERT(fSuccess);

#ifdef SPQOS_DEBUG
		fSuccess =
#endif	// SPQOS_DEBUG
			phm->Replace(&rgul[i], rgsz[i]);
		SPQOS_ASSERT(fSuccess);
	}
	SPQOS_ASSERT(ulCnt == phm->Size());

	// test replacing entry value of a non-existing key
	ULONG_PTR ulp = 0;
#ifdef SPQOS_DEBUG
	BOOL fSuccess =
#endif	// SPQOS_DEBUG
		phm->Replace(&ulp, rgsz[0]);
	SPQOS_ASSERT(!fSuccess);

	phm->Release();

	// test replacing values and triggering their release
	typedef CHashMap<ULONG, ULONG, HashValue<ULONG>, spqos::Equals<ULONG>,
					 CleanupDelete<ULONG>, CleanupDelete<ULONG> >
		UlongToUlongMap;
	UlongToUlongMap *phm2 = SPQOS_NEW(mp) UlongToUlongMap(mp, 128);

	ULONG *pulKey = SPQOS_NEW(mp) ULONG(1);
	ULONG *pulVal1 = SPQOS_NEW(mp) ULONG(2);
	ULONG *pulVal2 = SPQOS_NEW(mp) ULONG(3);

#ifdef SPQOS_DEBUG
	fSuccess =
#endif	// SPQOS_DEBUG
		phm2->Insert(pulKey, pulVal1);
	SPQOS_ASSERT(fSuccess);

#ifdef SPQOS_DEBUG
	ULONG *pulVal = phm2->Find(pulKey);
	SPQOS_ASSERT(*pulVal == 2);

	fSuccess =
#endif	// SPQOS_DEBUG
		phm2->Replace(pulKey, pulVal2);
	SPQOS_ASSERT(fSuccess);

#ifdef SPQOS_DEBUG
	pulVal = phm2->Find(pulKey);
	SPQOS_ASSERT(*pulVal == 3);
#endif	// SPQOS_DEBUG

	phm2->Release();

	return SPQOS_OK;
}


//---------------------------------------------------------------------------
//	@function:
//		CHashMapTest::EresUnittest_Ownership
//
//	@doc:
//		Basic hash map test with ownership
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CHashMapTest::EresUnittest_Ownership()
{
	// create memory pool
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	ULONG ulCnt = 256;

	typedef CHashMap<ULONG_PTR, CHAR, HashPtr<ULONG_PTR>,
					 spqos::Equals<ULONG_PTR>, CleanupDelete<ULONG_PTR>,
					 CleanupDeleteArray<CHAR> >
		UlongPtrToCharMap;

	UlongPtrToCharMap *phm = SPQOS_NEW(mp) UlongPtrToCharMap(mp, 32);
	for (ULONG i = 0; i < ulCnt; ++i)
	{
		ULONG_PTR *pulp = SPQOS_NEW(mp) ULONG_PTR(i);
		CHAR *sz = SPQOS_NEW_ARRAY(mp, CHAR, 3);

#ifdef SPQOS_DEBUG
		BOOL fSuccess =
#endif	// SPQOS_DEBUG
			phm->Insert(pulp, sz);

		SPQOS_ASSERT(fSuccess);
		SPQOS_ASSERT(sz == phm->Find(pulp));

		// can't insert existing keys
		SPQOS_ASSERT(!phm->Insert(pulp, sz));
	}

	phm->Release();

	return SPQOS_OK;
}

// EOF
