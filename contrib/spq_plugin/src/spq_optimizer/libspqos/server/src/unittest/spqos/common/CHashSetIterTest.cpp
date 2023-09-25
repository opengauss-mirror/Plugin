//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2017 Pivotal Software, Inc

#include "unittest/spqos/common/CHashSetIterTest.h"

#include "spqos/base.h"
#include "spqos/common/CAutoRef.h"
#include "spqos/common/CHashSetIter.h"
#include "spqos/memory/CAutoMemoryPool.h"
#include "spqos/test/CUnittest.h"

using namespace spqos;

// Unittest for basic hash set iterator
SPQOS_RESULT
CHashSetIterTest::EresUnittest()
{
	CUnittest rgut[] = {
		SPQOS_UNITTEST_FUNC(CHashSetIterTest::EresUnittest_Basic),
	};

	return CUnittest::EresExecute(rgut, SPQOS_ARRAY_SIZE(rgut));
}

// Basic iterator test
SPQOS_RESULT
CHashSetIterTest::EresUnittest_Basic()
{
	// create memory pool
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	// test data
	ULONG rgul[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
	const ULONG ulCnt = SPQOS_ARRAY_SIZE(rgul);

	typedef CHashSet<ULONG, HashValue<ULONG>, spqos::Equals<ULONG>,
					 CleanupNULL<ULONG> >
		UlongHashSet;

	typedef CHashSetIter<ULONG, HashValue<ULONG>, spqos::Equals<ULONG>,
						 CleanupNULL<ULONG> >
		UlongHashSetIter;

	// using N - 2 slots guarantees collisions
	UlongHashSet *ps = SPQOS_NEW(mp) UlongHashSet(mp, ulCnt - 2);

#ifdef SPQOS_DEBUG

	// iteration over empty map
	UlongHashSetIter siEmpty(ps);
	SPQOS_ASSERT(!siEmpty.Advance());

#endif	// SPQOS_DEBUG

	typedef CDynamicPtrArray<const ULONG, CleanupNULL> ULongPtrArray;
	CAutoRef<ULongPtrArray> pdrspqulValues(SPQOS_NEW(mp) ULongPtrArray(mp));
	// load map and iterate over it after each step
	for (ULONG ul = 0; ul < ulCnt; ++ul)
	{
		(void) ps->Insert(&rgul[ul]);
		pdrspqulValues->Append(&rgul[ul]);

		CAutoRef<ULongPtrArray> pdrspqulIterValues(SPQOS_NEW(mp)
													  ULongPtrArray(mp));

		// iterate over full set
		UlongHashSetIter si(ps);
		while (si.Advance())
		{
			pdrspqulIterValues->Append(si.Get());
		}

		pdrspqulIterValues->Sort();

		SPQOS_ASSERT(pdrspqulValues->Equals(pdrspqulIterValues.Value()));
	}

	ps->Release();

	return SPQOS_OK;
}


// EOF
