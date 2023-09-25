//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2008 Greenplum, Inc.
//
//	@filename:
//		CSyncHashtableTest.cpp
//
//	@doc:
//      Tests for CSyncHashtableTest; spliced out into a separate
//		class CSyncHashtableTest to avoid template parameter confusion for the compiler
//---------------------------------------------------------------------------

#include "unittest/spqos/common/CSyncHashtableTest.h"

#include "spqos/base.h"
#include "spqos/common/CBitVector.h"
#include "spqos/memory/CAutoMemoryPool.h"
#include "spqos/task/CAutoTaskProxy.h"
#include "spqos/test/CUnittest.h"

using namespace spqos;

#define SPQOS_SHT_SMALL_BUCKETS 5
#define SPQOS_SHT_BIG_BUCKETS 100
#define SPQOS_SHT_ELEMENTS 10
#define SPQOS_SHT_LOOKUPS 500
#define SPQOS_SHT_INITIAL_ELEMENTS (1 + SPQOS_SHT_ELEMENTS / 2)
#define SPQOS_SHT_ELEMENT_DUPLICATES 5
#define SPQOS_SHT_THREADS 15


// invalid key
const ULONG CSyncHashtableTest::SElem::m_ulInvalid = spqos::ulong_max;

// invalid element
const CSyncHashtableTest::SElem CSyncHashtableTest::SElem::m_elemInvalid(
	spqos::ulong_max, spqos::ulong_max);

//---------------------------------------------------------------------------
//	@function:
//		CSyncHashtableTest::EresUnittest
//
//	@doc:
//		Unittest for sync'd hashtable
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CSyncHashtableTest::EresUnittest()
{
	CUnittest rgut[] = {
		SPQOS_UNITTEST_FUNC(CSyncHashtableTest::EresUnittest_Basics),
		SPQOS_UNITTEST_FUNC(CSyncHashtableTest::EresUnittest_Accessor),
		SPQOS_UNITTEST_FUNC(CSyncHashtableTest::EresUnittest_ComplexEquality),
		SPQOS_UNITTEST_FUNC(CSyncHashtableTest::EresUnittest_SameKeyIteration),
		SPQOS_UNITTEST_FUNC(
			CSyncHashtableTest::EresUnittest_NonConcurrentIteration)};

	return CUnittest::EresExecute(rgut, SPQOS_ARRAY_SIZE(rgut));
}


//---------------------------------------------------------------------------
//	@function:
//		CSyncHashtableTest::EresUnittest_Basics
//
//	@doc:
//		Various list operations
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CSyncHashtableTest::EresUnittest_Basics()
{
	// create memory pool
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	SElem *rgelem = SPQOS_NEW_ARRAY(mp, SElem, SPQOS_SHT_ELEMENTS);
	CSyncHashtable<SElem, ULONG> sht;
	sht.Init(mp, SPQOS_SHT_SMALL_BUCKETS, SPQOS_OFFSET(SElem, m_link),
			 SPQOS_OFFSET(SElem, m_ulKey), &(SElem::m_ulInvalid),
			 SElem::HashValue, SElem::FEqualKeys);

	for (ULONG i = 0; i < SPQOS_SHT_ELEMENTS; i++)
	{
		rgelem[i] = SElem(i, i);
		sht.Insert(&rgelem[i]);
	}

	SPQOS_DELETE_ARRAY(rgelem);

	return SPQOS_OK;
}


//---------------------------------------------------------------------------
//	@function:
//		CSyncHashtableTest::EresUnittest_Accessor
//
//	@doc:
//		Various hashtable operations via accessor class
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CSyncHashtableTest::EresUnittest_Accessor()
{
	// create memory pool
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	SElem *rgelem = SPQOS_NEW_ARRAY(mp, SElem, SPQOS_SHT_ELEMENTS);

	CSyncHashtable<SElem, ULONG> rgsht[2];

	rgsht[0].Init(mp, SPQOS_SHT_SMALL_BUCKETS, SPQOS_OFFSET(SElem, m_link),
				  SPQOS_OFFSET(SElem, m_ulKey), &(SElem::m_ulInvalid),
				  SElem::HashValue, SElem::FEqualKeys);

	rgsht[1].Init(mp, SPQOS_SHT_BIG_BUCKETS, SPQOS_OFFSET(SElem, m_link),
				  SPQOS_OFFSET(SElem, m_ulKey), &(SElem::m_ulInvalid),
				  SElem::HashValue, SElem::FEqualKeys);

	for (ULONG i = 0; i < SPQOS_SHT_ELEMENTS; i++)
	{
		rgelem[i] = SElem(i, i);

		// distribute elements over both hashtables
		rgsht[i % 2].Insert(&rgelem[i]);
	}

	for (ULONG i = 0; i < SPQOS_SHT_ELEMENTS; i++)
	{
		SElem *pelem = &rgelem[i];
		ULONG ulKey = pelem->m_ulKey;

		CSyncHashtableAccessByKey<SElem, ULONG> shtacc0(rgsht[0], ulKey);

		CSyncHashtableAccessByKey<SElem, ULONG> shtacc1(rgsht[1], ulKey);

		if (NULL == shtacc0.Find())
		{
			// must be in the other hashtable
			SPQOS_ASSERT(pelem == shtacc1.Find());

			// move to other hashtable
			shtacc1.Remove(pelem);
			shtacc0.Insert(pelem);
		}
	}

	// check that all elements have been moved over to the first hashtable
	for (ULONG i = 0; i < SPQOS_SHT_ELEMENTS; i++)
	{
		SElem *pelem = &rgelem[i];
		ULONG ulKey = pelem->m_ulKey;

		CSyncHashtableAccessByKey<SElem, ULONG> shtacc0(rgsht[0], ulKey);

		CSyncHashtableAccessByKey<SElem, ULONG> shtacc1(rgsht[1], ulKey);

		SPQOS_ASSERT(NULL == shtacc1.Find());
		SPQOS_ASSERT(pelem == shtacc0.Find());
	}

	SPQOS_DELETE_ARRAY(rgelem);

	return SPQOS_OK;
}


//---------------------------------------------------------------------------
//	@function:
//		CSyncHashtableTest::EresUnittest_ComplexEquality
//
//	@doc:
//		Test where key is the entire object rather than a member;
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CSyncHashtableTest::EresUnittest_ComplexEquality()
{
	// create memory pool
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	SElem *rgelem = SPQOS_NEW_ARRAY(mp, SElem, SPQOS_SHT_ELEMENTS);

	CSyncHashtable<SElem, SElem> sht;
	sht.Init(mp, SPQOS_SHT_SMALL_BUCKETS, SPQOS_OFFSET(SElem, m_link),
			 0 /*cKeyOffset*/, &(SElem::m_elemInvalid), SElem::HashValue,
			 SElem::Equals);

	for (ULONG i = 0; i < SPQOS_SHT_ELEMENTS; i++)
	{
		rgelem[i] = SElem(SPQOS_SHT_ELEMENTS + i, i);

		sht.Insert(&rgelem[i]);
	}

	for (ULONG j = 0; j < SPQOS_SHT_ELEMENTS; j++)
	{
		SElem elem(SPQOS_SHT_ELEMENTS + j, j);
		CSyncHashtableAccessByKey<SElem, SElem> shtacc(sht, elem);

#ifdef SPQOS_DEBUG
		SElem *pelem = shtacc.Find();
		SPQOS_ASSERT(NULL != pelem && pelem != &elem);
		SPQOS_ASSERT(pelem->Id() == SPQOS_SHT_ELEMENTS + j);
#endif	// SPQOS_DEBUG
	}

	SPQOS_DELETE_ARRAY(rgelem);

	return SPQOS_OK;
}


//---------------------------------------------------------------------------
//	@function:
//		CSyncHashtableTest::EresUnittest_SameKeyIteration
//
//	@doc:
//		Test iteration over elements with the same key
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CSyncHashtableTest::EresUnittest_SameKeyIteration()
{
	// create memory pool
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	const ULONG size = SPQOS_SHT_ELEMENTS * SPQOS_SHT_ELEMENT_DUPLICATES;
	SElem *rgelem = SPQOS_NEW_ARRAY(mp, SElem, size);

	SElemHashtable sht;

	sht.Init(mp, SPQOS_SHT_SMALL_BUCKETS, SPQOS_OFFSET(SElem, m_link),
			 SPQOS_OFFSET(SElem, m_ulKey), &(SElem::m_ulInvalid),
			 SElem::HashValue, SElem::FEqualKeys);

	// insert a mix of elements with duplicate keys
	for (ULONG j = 0; j < SPQOS_SHT_ELEMENT_DUPLICATES; j++)
	{
		for (ULONG i = 0; i < SPQOS_SHT_ELEMENTS; i++)
		{
			ULONG ulIndex = SPQOS_SHT_ELEMENTS * j + i;
			rgelem[ulIndex] = SElem(ulIndex, i);
			sht.Insert(&rgelem[ulIndex]);
		}
	}

	// iterate over elements with the same key
	for (ULONG ulKey = 0; ulKey < SPQOS_SHT_ELEMENTS; ulKey++)
	{
		SElemHashtableAccessor shtacc(sht, ulKey);

		ULONG count = 0;
		SElem *pelem = shtacc.Find();
		while (NULL != pelem)
		{
			count++;
			pelem = shtacc.Next(pelem);
		}
		SPQOS_ASSERT(count == SPQOS_SHT_ELEMENT_DUPLICATES);
	}

	SPQOS_DELETE_ARRAY(rgelem);

	return SPQOS_OK;
}


//---------------------------------------------------------------------------
//	@function:
//		CSyncHashtableTest::EresUnittest_NonConcurrentIteration
//
//	@doc:
//		Test iteration by a single client over all hash table elements
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CSyncHashtableTest::EresUnittest_NonConcurrentIteration()
{
	// create memory pool
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	SElem *rgelem = SPQOS_NEW_ARRAY(mp, SElem, SPQOS_SHT_ELEMENTS);

	SElemHashtable sht;

	sht.Init(mp, SPQOS_SHT_SMALL_BUCKETS, SPQOS_OFFSET(SElem, m_link),
			 SPQOS_OFFSET(SElem, m_ulKey), &(SElem::m_ulInvalid),
			 SElem::HashValue, SElem::FEqualKeys);


	// iterate over empty hash table
	SElemHashtableIter shtitEmpty(sht);
	SPQOS_ASSERT(!shtitEmpty.Advance() &&
				"Iterator advanced in an empty hash table");


	// insert elements
	for (ULONG i = 0; i < SPQOS_SHT_ELEMENTS; i++)
	{
		rgelem[i] = SElem(i, i);
		sht.Insert(&rgelem[i]);
	}

	// iteration with no concurrency - each access must
	// produce a unique, valid and not NULL element
	SElemHashtableIter shtit(sht);
	ULONG count = 0;

#ifdef SPQOS_DEBUG
	// maintain a flag for visiting each element
	CBitVector bv(mp, SPQOS_SHT_ELEMENTS);
#endif	// SPQOS_DEBUG

	while (shtit.Advance())
	{
		SElemHashtableIterAccessor htitacc(shtit);

#ifdef SPQOS_DEBUG
		SElem *pelem =
#endif	// SPQOS_DEBUG
			htitacc.Value();

		SPQOS_ASSERT(NULL != pelem);

		SPQOS_ASSERT(SElem::IsValid(pelem->m_ulKey));

		// check if element has been visited before
		SPQOS_ASSERT(!bv.ExchangeSet(pelem->Id()) &&
					"Iterator returned duplicates");

		count++;
	}

	SPQOS_ASSERT(count == SPQOS_SHT_ELEMENTS);

	SPQOS_DELETE_ARRAY(rgelem);

	return SPQOS_OK;
}

// EOF
