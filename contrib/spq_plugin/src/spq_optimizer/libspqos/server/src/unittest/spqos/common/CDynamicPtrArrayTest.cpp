//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2008 Greenplum, Inc.
//
//	@filename:
//		CDynamicPtrArrayTest.cpp
//
//	@doc:
//		Test for CDynamicPtrArray
//---------------------------------------------------------------------------

#include "unittest/spqos/common/CDynamicPtrArrayTest.h"

#include "spqos/base.h"
#include "spqos/common/CDynamicPtrArray.h"
#include "spqos/memory/CAutoMemoryPool.h"
#include "spqos/test/CUnittest.h"

using namespace spqos;

//---------------------------------------------------------------------------
//	@function:
//		CDynamicPtrArrayTest::EresUnittest
//
//	@doc:
//		Unittest for ref-counting
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CDynamicPtrArrayTest::EresUnittest()
{
	CUnittest rgut[] = {
		SPQOS_UNITTEST_FUNC(CDynamicPtrArrayTest::EresUnittest_Basic),
		SPQOS_UNITTEST_FUNC(CDynamicPtrArrayTest::EresUnittest_Ownership),
		SPQOS_UNITTEST_FUNC(CDynamicPtrArrayTest::EresUnittest_ArrayAppend),
		SPQOS_UNITTEST_FUNC(
			CDynamicPtrArrayTest::EresUnittest_ArrayAppendExactFit),
		SPQOS_UNITTEST_FUNC(
			CDynamicPtrArrayTest::EresUnittest_PdrspqulSubsequenceIndexes),
	};

	return CUnittest::EresExecute(rgut, SPQOS_ARRAY_SIZE(rgut));
}


//---------------------------------------------------------------------------
//	@function:
//		CDynamicPtrArrayTest::EresUnittest_Basic
//
//	@doc:
//		Basic array allocation test
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CDynamicPtrArrayTest::EresUnittest_Basic()
{
	// create memory pool
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	// test with CHAR array

	CHAR rgsz[][9] = {"abc",  "def", "ghi", "qwe", "wer",
					  "wert", "dfg", "xcv", "zxc"};
	const CHAR *szMissingElem = "missing";

	CDynamicPtrArray<CHAR, CleanupNULL<CHAR> > *pdrg =
		SPQOS_NEW(mp) CDynamicPtrArray<CHAR, CleanupNULL<CHAR> >(mp, 2);

	// add elements incl trigger resize of array
	for (ULONG i = 0; i < 9; i++)
	{
		pdrg->Append(rgsz[i]);
		SPQOS_ASSERT(i + 1 == pdrg->Size());
		SPQOS_ASSERT(rgsz[i] == (*pdrg)[i]);
	}

	// lookup tests
#ifdef SPQOS_DEBUG
	const CHAR *szElem =
#endif	// SPQOS_DEBUG
		pdrg->Find(rgsz[0]);
	SPQOS_ASSERT(NULL != szElem);

#ifdef SPQOS_DEBUG
	ULONG ulPos =
#endif	// SPQOS_DEBUG
		pdrg->IndexOf(rgsz[0]);
	SPQOS_ASSERT(0 == ulPos);

#ifdef SPQOS_DEBUG
	ULONG ulPosMissing =
#endif	// SPQOS_DEBUG
		pdrg->IndexOf(szMissingElem);
	SPQOS_ASSERT(spqos::ulong_max == ulPosMissing);
	// all elements were inserted in ascending order
	SPQOS_ASSERT(pdrg->IsSorted());

	pdrg->Release();


	// test with ULONG array

	typedef CDynamicPtrArray<ULONG, CleanupNULL<ULONG> > UlongArray;
	UlongArray *pdrgULONG = SPQOS_NEW(mp) UlongArray(mp, 1);
	ULONG c = 256;

	// add elements incl trigger resize of array
	for (ULONG_PTR ulpK = c; ulpK > 0; ulpK--)
	{
		ULONG *pul = (ULONG *) (ulpK - 1);
		pdrgULONG->Append(pul);
	}

	SPQOS_ASSERT(c == pdrgULONG->Size());

	// all elements were inserted in descending order
	SPQOS_ASSERT(!pdrgULONG->IsSorted());

	pdrgULONG->Sort();
	SPQOS_ASSERT(pdrgULONG->IsSorted());

	// test that all positions got copied and sorted properly
	for (ULONG_PTR ulpJ = 0; ulpJ < c; ulpJ++)
	{
		SPQOS_ASSERT((ULONG *) ulpJ == (*pdrgULONG)[(ULONG) ulpJ]);
	}
	pdrgULONG->Release();


	return SPQOS_OK;
}


//---------------------------------------------------------------------------
//	@function:
//		CDynamicPtrArrayTest::EresUnittest_Ownership
//
//	@doc:
//		Basic array test with ownership
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CDynamicPtrArrayTest::EresUnittest_Ownership()
{
	// create memory pool
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	// test with ULONGs

	typedef CDynamicPtrArray<ULONG, CleanupDelete<ULONG> > UlongArray;
	UlongArray *pdrgULONG = SPQOS_NEW(mp) UlongArray(mp, 1);

	// add elements incl trigger resize of array
	for (ULONG k = 0; k < 256; k++)
	{
		ULONG *pul = SPQOS_NEW(mp) ULONG;
		pdrgULONG->Append(pul);
		SPQOS_ASSERT(k + 1 == pdrgULONG->Size());
		SPQOS_ASSERT(pul == (*pdrgULONG)[k]);
	}
	pdrgULONG->Release();

	// test with CHAR array

	typedef CDynamicPtrArray<CHAR, CleanupDeleteArray<CHAR> > CharArray;
	CharArray *pdrgCHAR = SPQOS_NEW(mp) CharArray(mp, 2);

	// add elements incl trigger resize of array
	for (ULONG i = 0; i < 3; i++)
	{
		CHAR *sz = SPQOS_NEW_ARRAY(mp, CHAR, 5);
		pdrgCHAR->Append(sz);
		SPQOS_ASSERT(i + 1 == pdrgCHAR->Size());
		SPQOS_ASSERT(sz == (*pdrgCHAR)[i]);
	}

	pdrgCHAR->Clear();
	SPQOS_ASSERT(0 == pdrgCHAR->Size());

	pdrgCHAR->Release();

	return SPQOS_OK;
}


//---------------------------------------------------------------------------
//	@function:
//		CDynamicPtrArrayTest::EresUnittest_ArrayAppend
//
//	@doc:
//		Appending arrays
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CDynamicPtrArrayTest::EresUnittest_ArrayAppend()
{
	// create memory pool
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	typedef CDynamicPtrArray<ULONG, CleanupNULL<ULONG> > UlongArray;

	ULONG cVal = 0;

	// array with 1 element
	UlongArray *pdrgULONG1 = SPQOS_NEW(mp) UlongArray(mp, 1);
	pdrgULONG1->Append(&cVal);
	SPQOS_ASSERT(1 == pdrgULONG1->Size());

	// array with x elements
	ULONG cX = 1000;
	UlongArray *pdrgULONG2 = SPQOS_NEW(mp) UlongArray(mp, 1);
	for (ULONG i = 0; i < cX; i++)
	{
		pdrgULONG2->Append(&cX);
	}
	SPQOS_ASSERT(cX == pdrgULONG2->Size());

	// add one to another
	pdrgULONG1->AppendArray(pdrgULONG2);
	SPQOS_ASSERT(cX + 1 == pdrgULONG1->Size());
	for (ULONG j = 0; j < pdrgULONG2->Size(); j++)
	{
		SPQOS_ASSERT((*pdrgULONG1)[j + 1] == (*pdrgULONG2)[j]);
	}

	pdrgULONG1->Release();
	pdrgULONG2->Release();

	return SPQOS_OK;
}



//---------------------------------------------------------------------------
//	@function:
//		CDynamicPtrArrayTest::EresUnittest_ArrayAppendExactFit
//
//	@doc:
//		Appending arrays when there is enough memory in first
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CDynamicPtrArrayTest::EresUnittest_ArrayAppendExactFit()
{
	// create memory pool
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	typedef CDynamicPtrArray<ULONG, CleanupNULL<ULONG> > UlongArray;

	ULONG cVal = 0;

	// array with 1 element
	UlongArray *pdrgULONG1 = SPQOS_NEW(mp) UlongArray(mp, 10);
	pdrgULONG1->Append(&cVal);
	SPQOS_ASSERT(1 == pdrgULONG1->Size());

	// array with x elements
	ULONG cX = 9;
	UlongArray *pdrgULONG2 = SPQOS_NEW(mp) UlongArray(mp, 15);
	for (ULONG i = 0; i < cX; i++)
	{
		pdrgULONG2->Append(&cX);
	}
	SPQOS_ASSERT(cX == pdrgULONG2->Size());

	// add one to another
	pdrgULONG1->AppendArray(pdrgULONG2);
	SPQOS_ASSERT(cX + 1 == pdrgULONG1->Size());
	for (ULONG j = 0; j < pdrgULONG2->Size(); j++)
	{
		SPQOS_ASSERT((*pdrgULONG1)[j + 1] == (*pdrgULONG2)[j]);
	}

	UlongArray *pdrgULONG3 = SPQOS_NEW(mp) UlongArray(mp, 15);
	pdrgULONG1->AppendArray(pdrgULONG3);
	SPQOS_ASSERT(cX + 1 == pdrgULONG1->Size());

	pdrgULONG1->Release();
	pdrgULONG2->Release();
	pdrgULONG3->Release();

	return SPQOS_OK;
}

//---------------------------------------------------------------------------
//	@function:
//		CDynamicPtrArrayTest::EresUnittest_PdrspqulSubsequenceIndexes
//
//	@doc:
//		Finding the first occurrences of the elements of the first array
//		in the second one.
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CDynamicPtrArrayTest::EresUnittest_PdrspqulSubsequenceIndexes()
{
	typedef CDynamicPtrArray<ULONG, CleanupNULL<ULONG> > UlongArray;

	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	// the array containing elements to look up
	UlongArray *pdrgULONGLookup = SPQOS_NEW(mp) UlongArray(mp);

	// the array containing the target elements that will give the positions
	UlongArray *pdrgULONGTarget = SPQOS_NEW(mp) UlongArray(mp);

	ULONG *pul1 = SPQOS_NEW(mp) ULONG(10);
	ULONG *pul2 = SPQOS_NEW(mp) ULONG(20);
	ULONG *pul3 = SPQOS_NEW(mp) ULONG(30);

	pdrgULONGLookup->Append(pul1);
	pdrgULONGLookup->Append(pul2);
	pdrgULONGLookup->Append(pul3);
	pdrgULONGLookup->Append(pul3);

	// since target is empty, there are elements in lookup with no match, so the function
	// should return NULL
	SPQOS_ASSERT(NULL == pdrgULONGTarget->IndexesOfSubsequence(pdrgULONGLookup));

	pdrgULONGTarget->Append(pul1);
	pdrgULONGTarget->Append(pul3);
	pdrgULONGTarget->Append(pul3);
	pdrgULONGTarget->Append(pul3);
	pdrgULONGTarget->Append(pul2);

	ULongPtrArray *pdrspqulIndexes =
		pdrgULONGTarget->IndexesOfSubsequence(pdrgULONGLookup);

	SPQOS_ASSERT(NULL != pdrspqulIndexes);
	SPQOS_ASSERT(4 == pdrspqulIndexes->Size());
	SPQOS_ASSERT(0 == *(*pdrspqulIndexes)[0]);
	SPQOS_ASSERT(4 == *(*pdrspqulIndexes)[1]);
	SPQOS_ASSERT(1 == *(*pdrspqulIndexes)[2]);
	SPQOS_ASSERT(1 == *(*pdrspqulIndexes)[3]);

	SPQOS_DELETE(pul1);
	SPQOS_DELETE(pul2);
	SPQOS_DELETE(pul3);
	pdrspqulIndexes->Release();
	pdrgULONGTarget->Release();
	pdrgULONGLookup->Release();

	return SPQOS_OK;
}

// EOF
