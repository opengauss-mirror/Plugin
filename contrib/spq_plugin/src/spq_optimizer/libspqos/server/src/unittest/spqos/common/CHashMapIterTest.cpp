//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CHashMapIterTest.cpp
//
//	@doc:
//		Test for CHashMapIter
//---------------------------------------------------------------------------

#include "unittest/spqos/common/CHashMapIterTest.h"

#include "spqos/base.h"
#include "spqos/common/CAutoRef.h"
#include "spqos/common/CHashMapIter.h"
#include "spqos/memory/CAutoMemoryPool.h"
#include "spqos/test/CUnittest.h"

using namespace spqos;

//---------------------------------------------------------------------------
//	@function:
//		CHashMapIterTest::EresUnittest
//
//	@doc:
//		Unittest for basic hash map iterator
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CHashMapIterTest::EresUnittest()
{
	CUnittest rgut[] = {
		SPQOS_UNITTEST_FUNC(CHashMapIterTest::EresUnittest_Basic),
	};

	return CUnittest::EresExecute(rgut, SPQOS_ARRAY_SIZE(rgut));
}


//---------------------------------------------------------------------------
//	@function:
//		CHashMapIterTest::EresUnittest_Basic
//
//	@doc:
//		Basic iterator test
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CHashMapIterTest::EresUnittest_Basic()
{
	// create memory pool
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	// test data
	ULONG rgul[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
	const ULONG ulCnt = SPQOS_ARRAY_SIZE(rgul);

	typedef CHashMap<ULONG, ULONG, HashPtr<ULONG>, spqos::Equals<ULONG>,
					 CleanupNULL<ULONG>, CleanupNULL<ULONG> >
		Map;

	typedef CHashMapIter<ULONG, ULONG, HashPtr<ULONG>, spqos::Equals<ULONG>,
						 CleanupNULL<ULONG>, CleanupNULL<ULONG> >
		MapIter;


	// using N - 2 slots guarantees collisions
	Map *pm = SPQOS_NEW(mp) Map(mp, ulCnt - 2);

#ifdef SPQOS_DEBUG

	// iteration over empty map
	MapIter miEmpty(pm);
	SPQOS_ASSERT(!miEmpty.Advance());

#endif	// SPQOS_DEBUG

	typedef CDynamicPtrArray<const ULONG, CleanupNULL> ULongPtrArray;
	CAutoRef<ULongPtrArray> pdrspqulKeys(SPQOS_NEW(mp) ULongPtrArray(mp)),
		pdrspqulValues(SPQOS_NEW(mp) ULongPtrArray(mp));
	// load map and iterate over it after each step
	for (ULONG ul = 0; ul < ulCnt; ++ul)
	{
		(void) pm->Insert(&rgul[ul], &rgul[ul]);
		pdrspqulKeys->Append(&rgul[ul]);
		pdrspqulValues->Append(&rgul[ul]);

		CAutoRef<ULongPtrArray> pdrspqulIterKeys(SPQOS_NEW(mp) ULongPtrArray(mp)),
			pdrspqulIterValues(SPQOS_NEW(mp) ULongPtrArray(mp));

		// iterate over full map
		MapIter mi(pm);
		while (mi.Advance())
		{
			pdrspqulIterKeys->Append(mi.Key());
			pdrspqulIterValues->Append(mi.Value());
		}

		pdrspqulIterKeys->Sort();
		pdrspqulIterValues->Sort();

		SPQOS_ASSERT(pdrspqulKeys->Equals(pdrspqulIterKeys.Value()));
		SPQOS_ASSERT(pdrspqulValues->Equals(pdrspqulIterValues.Value()));
	}

	pm->Release();

	return SPQOS_OK;
}


// EOF
