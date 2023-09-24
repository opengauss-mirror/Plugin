//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2008 Greenplum, Inc.
//
//	@filename:
//		CBitVectorTest.cpp
//
//	@doc:
//		Tests for CBitVector
//---------------------------------------------------------------------------


#include "unittest/spqos/common/CBitVectorTest.h"

#include "spqos/base.h"
#include "spqos/common/CAutoRg.h"
#include "spqos/common/CBitVector.h"
#include "spqos/common/CRandom.h"
#include "spqos/common/clibwrapper.h"
#include "spqos/memory/CAutoMemoryPool.h"
#include "spqos/test/CUnittest.h"

using namespace spqos;


//---------------------------------------------------------------------------
//	@function:
//		CBitVector::EresUnittest
//
//	@doc:
//		Unittest for bit vectors
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CBitVectorTest::EresUnittest()
{
	CUnittest rgut[] = {
		SPQOS_UNITTEST_FUNC(CBitVectorTest::EresUnittest_Basics),
		SPQOS_UNITTEST_FUNC(CBitVectorTest::EresUnittest_SetOps),
		SPQOS_UNITTEST_FUNC(CBitVectorTest::EresUnittest_Cursor),
		SPQOS_UNITTEST_FUNC(CBitVectorTest::EresUnittest_Random)
#ifdef SPQOS_DEBUG
			,
		SPQOS_UNITTEST_FUNC_ASSERT(CBitVectorTest::EresUnittest_OutOfBounds)
#endif	// SPQOS_DEBUG
	};

	return CUnittest::EresExecute(rgut, SPQOS_ARRAY_SIZE(rgut));
}


//---------------------------------------------------------------------------
//	@function:
//		CBitVectorTest::EresUnittest_Basics
//
//	@doc:
//		Various basic operations
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CBitVectorTest::EresUnittest_Basics()
{
	// create memory pool
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	ULONG cSize = 129;

	CBitVector bv(mp, cSize);
	SPQOS_ASSERT(bv.IsEmpty());

	for (ULONG i = 0; i < cSize; i++)
	{
		BOOL fSet = bv.ExchangeSet(i);
		if (fSet)
		{
			return SPQOS_FAILED;
		}
		SPQOS_ASSERT(bv.Get(i));

		CBitVector bvCopy(mp, bv);
		for (ULONG j = 0; j <= i; j++)
		{
			BOOL fSetAlt = bvCopy.Get(j);
			SPQOS_ASSERT(fSetAlt);

			if (true != fSetAlt)
			{
				return SPQOS_FAILED;
			}

			// clear and check
			bvCopy.ExchangeClear(j);
			fSetAlt = bvCopy.Get(j);
			SPQOS_ASSERT(!fSetAlt);
		}

		SPQOS_ASSERT(bvCopy.CountSetBits() == 0);
	}

	SPQOS_ASSERT(bv.CountSetBits() == cSize);

	return SPQOS_OK;
}



//---------------------------------------------------------------------------
//	@function:
//		CBitVectorTest::EresUnittest_SetOps
//
//	@doc:
//		Set operation tests
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CBitVectorTest::EresUnittest_SetOps()
{
	// create memory pool
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	ULONG cSize = 129;
	CBitVector bvEmpty(mp, cSize);

	CBitVector bvEven(mp, cSize);
	for (ULONG i = 0; i < cSize; i += 2)
	{
		bvEven.ExchangeSet(i);
	}
	SPQOS_ASSERT(bvEven.ContainsAll(&bvEmpty));

	CBitVector bvOdd(mp, cSize);
	for (ULONG i = 1; i < cSize; i += 2)
	{
		bvOdd.ExchangeSet(i);
	}
	SPQOS_ASSERT(bvOdd.ContainsAll(&bvEmpty));
	SPQOS_ASSERT(bvOdd.IsDisjoint(&bvEven));

	SPQOS_ASSERT(!bvEven.ContainsAll(&bvOdd));
	SPQOS_ASSERT(!bvOdd.ContainsAll(&bvEven));

	CBitVector bv(mp, bvOdd);

	bv.Or(&bvEven);
	bv.And(&bvOdd);
	SPQOS_ASSERT(bv.Equals(&bvOdd));

	bv.Or(&bvEven);
	bv.And(&bvEven);
	SPQOS_ASSERT(bv.Equals(&bvEven));

	return SPQOS_OK;
}


//---------------------------------------------------------------------------
//	@function:
//		CBitVectorTest::EresUnittest_Cursor
//
//	@doc:
//		Unittest for cursoring
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CBitVectorTest::EresUnittest_Cursor()
{
	// create memory pool
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	CBitVector bv(mp, 129);
	for (ULONG i = 1; i < 20; i++)
	{
		bv.ExchangeSet(i * 3);
	}

	ULONG ulCursor = 0;
	bv.GetNextSetBit(0, ulCursor);
	while (bv.GetNextSetBit(ulCursor + 1, ulCursor))
	{
		SPQOS_ASSERT(ulCursor == ((ulCursor / 3) * 3));
	}

	return SPQOS_OK;
}


//---------------------------------------------------------------------------
//	@function:
//		CBitVectorTest::EresUnittest_Random
//
//	@doc:
//		Test with random bit vectors to avoid patterns
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CBitVectorTest::EresUnittest_Random()
{
	// create memory pool
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	// set up control vector
	ULONG cTotal = 10000;
	CHAR *rg = SPQOS_NEW_ARRAY(mp, CHAR, cTotal);

	CRandom rand;

	clib::Memset(rg, 0, cTotal);

	// set random chars in the control vector
	for (ULONG i = 0; i < cTotal * 0.2; i++)
	{
		ULONG index = rand.Next() % (cTotal - 1);
		SPQOS_ASSERT(index < cTotal);
		rg[index] = 1;
	}

	ULONG cElements = 0;
	CBitVector bv(mp, cTotal);
	for (ULONG i = 0; i < cTotal; i++)
	{
		if (1 == rg[i])
		{
			bv.ExchangeSet(i);
			cElements++;
		}
	}

	SPQOS_ASSERT(cElements == bv.CountSetBits());

	ULONG ulCursor = 0;
	while (bv.GetNextSetBit(ulCursor + 1, ulCursor))
	{
		SPQOS_ASSERT(1 == rg[ulCursor]);
		cElements--;
	}

	SPQOS_ASSERT(0 == cElements);
	SPQOS_DELETE_ARRAY(rg);

	return SPQOS_OK;
}


#ifdef SPQOS_DEBUG

//---------------------------------------------------------------------------
//	@function:
//		CBitVectorTest::EresUnittest_OutOfBounds
//
//	@doc:
//		Unittest for OOB access
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CBitVectorTest::EresUnittest_OutOfBounds()
{
	// create memory pool
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	CBitVector bv(mp, 129);

	// this must assert
	bv.ExchangeSet(130);

	return SPQOS_FAILED;
}

#endif	// SPQOS_DEBUG

// EOF
