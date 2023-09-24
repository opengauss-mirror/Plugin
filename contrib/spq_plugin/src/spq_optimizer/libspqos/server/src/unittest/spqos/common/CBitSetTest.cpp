//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		CBitSetTest.cpp
//
//	@doc:
//      Test for CBitSet
//---------------------------------------------------------------------------

#include "unittest/spqos/common/CBitSetTest.h"

#include "spqos/base.h"
#include "spqos/common/CBitSet.h"
#include "spqos/io/COstreamString.h"
#include "spqos/memory/CAutoMemoryPool.h"
#include "spqos/string/CWStringDynamic.h"
#include "spqos/test/CUnittest.h"

using namespace spqos;

//---------------------------------------------------------------------------
//	@function:
//		CBitSet::EresUnittest
//
//	@doc:
//		Unittest for bit vectors
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CBitSetTest::EresUnittest()
{
	CUnittest rgut[] = {
		SPQOS_UNITTEST_FUNC(CBitSetTest::EresUnittest_Basics),
		SPQOS_UNITTEST_FUNC(CBitSetTest::EresUnittest_Removal),
		SPQOS_UNITTEST_FUNC(CBitSetTest::EresUnittest_SetOps),
		SPQOS_UNITTEST_FUNC(CBitSetTest::EresUnittest_Performance)};

	return CUnittest::EresExecute(rgut, SPQOS_ARRAY_SIZE(rgut));
}

//---------------------------------------------------------------------------
//	@function:
//		CBitSetTest::EresUnittest_Basics
//
//	@doc:
//		Testing ctors/dtor
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CBitSetTest::EresUnittest_Basics()
{
	// create memory pool
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	ULONG vector_size = 32;
	CBitSet *pbs = SPQOS_NEW(mp) CBitSet(mp, vector_size);

	ULONG cInserts = 10;
	for (ULONG i = 0; i < cInserts; i += 2)
	{
		// forces addition of new link
		pbs->ExchangeSet(i * vector_size);
	}
	SPQOS_ASSERT(cInserts / 2 == pbs->Size());

	for (ULONG i = 1; i < cInserts; i += 2)
	{
		// new link between existing links
		pbs->ExchangeSet(i * vector_size);
	}
	SPQOS_ASSERT(cInserts == pbs->Size());

	CBitSet *pbsCopy = SPQOS_NEW(mp) CBitSet(mp, *pbs);
	SPQOS_ASSERT(pbsCopy->Equals(pbs));

	// delete old bitset to make sure we're not accidentally
	// using any of its memory
	pbs->Release();

	for (ULONG i = 0; i < cInserts; i++)
	{
		SPQOS_ASSERT(pbsCopy->Get(i * vector_size));
	}

	CWStringDynamic str(mp);
	COstreamString os(&str);

	os << *pbsCopy << std::endl;
	SPQOS_TRACE(str.GetBuffer());

	pbsCopy->Release();

	return SPQOS_OK;
}


//---------------------------------------------------------------------------
//	@function:
//		CBitSetTest::EresUnittest_Removal
//
//	@doc:
//		Cleanup test
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CBitSetTest::EresUnittest_Removal()
{
	// create memory pool
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	ULONG vector_size = 32;
	CBitSet *pbs = SPQOS_NEW(mp) CBitSet(mp, vector_size);
	CBitSet *pbsEmpty = SPQOS_NEW(mp) CBitSet(mp, vector_size);

	SPQOS_ASSERT(pbs->Equals(pbsEmpty));
	SPQOS_ASSERT(pbsEmpty->Equals(pbs));

	ULONG cInserts = 10;
	for (ULONG i = 0; i < cInserts; i++)
	{
		pbs->ExchangeSet(i * vector_size);

		SPQOS_ASSERT(i + 1 == pbs->Size());
	}

	for (ULONG i = 0; i < cInserts; i++)
	{
		// cleans up empty links
		pbs->ExchangeClear(i * vector_size);

		SPQOS_ASSERT(cInserts - i - 1 == pbs->Size());
	}

	SPQOS_ASSERT(pbs->Equals(pbsEmpty));
	SPQOS_ASSERT(pbsEmpty->Equals(pbs));

	pbs->Release();
	pbsEmpty->Release();

	return SPQOS_OK;
}


//---------------------------------------------------------------------------
//	@function:
//		CBitSetTest::EresUnittest_SetOps
//
//	@doc:
//		Test for set operations
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CBitSetTest::EresUnittest_SetOps()
{
	// create memory pool
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	ULONG vector_size = 32;
	ULONG cInserts = 10;

	CBitSet *pbs1 = SPQOS_NEW(mp) CBitSet(mp, vector_size);
	for (ULONG i = 0; i < cInserts; i += 2)
	{
		pbs1->ExchangeSet(i * vector_size);
	}

	CBitSet *pbs2 = SPQOS_NEW(mp) CBitSet(mp, vector_size);
	for (ULONG i = 1; i < cInserts; i += 2)
	{
		pbs2->ExchangeSet(i * vector_size);
	}
	CBitSet *pbs = SPQOS_NEW(mp) CBitSet(mp, vector_size);

	pbs->Union(pbs1);
	SPQOS_ASSERT(pbs->Equals(pbs1));

	pbs->Intersection(pbs1);
	SPQOS_ASSERT(pbs->Equals(pbs1));
	SPQOS_ASSERT(pbs->Equals(pbs));
	SPQOS_ASSERT(pbs1->Equals(pbs1));

	pbs->Union(pbs2);
	SPQOS_ASSERT(!pbs->Equals(pbs1) && !pbs->Equals(pbs2));
	SPQOS_ASSERT(pbs->ContainsAll(pbs1) && pbs->ContainsAll(pbs2));

	pbs->Difference(pbs2);
	SPQOS_ASSERT(pbs->Equals(pbs1));

	pbs1->Release();

	pbs->Union(pbs2);
	pbs->Intersection(pbs2);
	SPQOS_ASSERT(pbs->Equals(pbs2));
	SPQOS_ASSERT(pbs->ContainsAll(pbs2));

	SPQOS_ASSERT(pbs->Size() == pbs2->Size());

	pbs2->Release();

	pbs->Release();

	// Test that intersection properly deletes links
	CBitSet *pbs4 = SPQOS_NEW(mp) CBitSet(mp, vector_size);
	CBitSet *pbs5 = SPQOS_NEW(mp) CBitSet(mp, vector_size);
	pbs4->ExchangeSet(1);
	pbs5->ExchangeSet(10);
	pbs5->ExchangeSet(50);
	pbs4->Intersection(pbs5);
	pbs5->ExchangeClear(10);
	pbs5->ExchangeClear(50);
	SPQOS_ASSERT(pbs4->Equals(pbs5));

	pbs4->Release();
	pbs5->Release();
	return SPQOS_OK;
}


//---------------------------------------------------------------------------
//	@function:
//		CBitSetTest::EresUnittest_Performance
//
//	@doc:
//		Simple perf test -- simulates xform candidate sets
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CBitSetTest::EresUnittest_Performance()
{
	// create memory pool
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	ULONG vector_size = 512;
	CBitSet *pbsBase = SPQOS_NEW(mp) CBitSet(mp, vector_size);
	for (ULONG i = 0; i < vector_size; i++)
	{
		(void) pbsBase->ExchangeSet(i);
	}

	CBitSet *pbsTest = SPQOS_NEW(mp) CBitSet(mp, vector_size);
	for (ULONG j = 0; j < 100000; j++)
	{
		ULONG cRandomBits = 16;
		for (ULONG i = 0; i < cRandomBits;
			 i += ((vector_size - 1) / cRandomBits))
		{
			(void) pbsTest->ExchangeSet(i);
		}

		pbsTest->Intersection(pbsBase);
	}

	pbsTest->Release();
	pbsBase->Release();

	return SPQOS_OK;
}

// EOF
