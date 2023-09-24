//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2008 Greenplum, Inc.
//
//	@filename:
//		CListTest.cpp
//
//	@doc:
//		Tests for CList
//---------------------------------------------------------------------------

#include "unittest/spqos/common/CListTest.h"

#include "spqos/base.h"
#include "spqos/common/CList.h"
#include "spqos/memory/CAutoMemoryPool.h"
#include "spqos/test/CUnittest.h"

using namespace spqos;

//---------------------------------------------------------------------------
//	@function:
//		CListTest::EresUnittest
//
//	@doc:
//		Unittest for lists
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CListTest::EresUnittest()
{
	CUnittest rgut[] = {
		SPQOS_UNITTEST_FUNC(CListTest::EresUnittest_Basics),
		SPQOS_UNITTEST_FUNC(CListTest::EresUnittest_Navigate),
		SPQOS_UNITTEST_FUNC(CListTest::EresUnittest_Cursor),
	};

	return CUnittest::EresExecute(rgut, SPQOS_ARRAY_SIZE(rgut));
}


//---------------------------------------------------------------------------
//	@function:
//		CListTest::EresUnittest_Basics
//
//	@doc:
//		Various list operations
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CListTest::EresUnittest_Basics()
{
	// create memory pool
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	CList<SElem> listFwd;
	listFwd.Init(SPQOS_OFFSET(SElem, m_linkFwd));

	CList<SElem> listBwd;
	listBwd.Init(SPQOS_OFFSET(SElem, m_linkBwd));

	ULONG cSize = 10;
	SElem *rgelem = SPQOS_NEW_ARRAY(mp, SElem, cSize);

	SPQOS_ASSERT(0 == listFwd.Size());
	SPQOS_ASSERT(0 == listBwd.Size());

	// insert all elements
	for (ULONG i = 0; i < cSize; i++)
	{
		SPQOS_ASSERT(i == listFwd.Size());
		SPQOS_ASSERT(i == listBwd.Size());

		listFwd.Prepend(&rgelem[i]);
		listBwd.Append(&rgelem[i]);
	}

	SPQOS_ASSERT(cSize == listFwd.Size());
	SPQOS_ASSERT(cSize == listBwd.Size());

	// remove first/last element until empty
	for (ULONG i = 0; i < cSize; i++)
	{
		SPQOS_ASSERT(cSize - i == listFwd.Size());
		SPQOS_ASSERT(&rgelem[i] == listFwd.Last());
		listFwd.Remove(listFwd.Last());

		// make sure it's still in the other list
		SPQOS_ASSERT(SPQOS_OK == listBwd.Find(&rgelem[i]));
	}
	SPQOS_ASSERT(NULL == listFwd.First());
	SPQOS_ASSERT(0 == listFwd.Size());

	// insert all elements in reverse order,
	// i.e. list is in same order as array
	for (ULONG i = cSize; i > 0; i--)
	{
		SPQOS_ASSERT(cSize - i == listFwd.Size());
		listFwd.Prepend(&rgelem[i - 1]);
	}
	SPQOS_ASSERT(cSize == listFwd.Size());

	for (ULONG i = 0; i < cSize; i++)
	{
		listFwd.Remove(&rgelem[(cSize / 2 + i) % cSize]);
	}
	SPQOS_ASSERT(NULL == listFwd.First());
	SPQOS_ASSERT(NULL == listFwd.Last());
	SPQOS_ASSERT(0 == listFwd.Size());

	SPQOS_DELETE_ARRAY(rgelem);
	return SPQOS_OK;
}



//---------------------------------------------------------------------------
//	@function:
//		CListTest::EresUnittest_Navigate
//
//	@doc:
//		Various navigation operations
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CListTest::EresUnittest_Navigate()
{
	// create memory pool
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	CList<SElem> listFwd;
	listFwd.Init(SPQOS_OFFSET(SElem, m_linkFwd));

	CList<SElem> listBwd;
	listBwd.Init(SPQOS_OFFSET(SElem, m_linkBwd));

	ULONG cSize = 10;
	SElem *rgelem = SPQOS_NEW_ARRAY(mp, SElem, cSize);

	// insert all elements in reverse order,
	// i.e. list is in same order as array
	for (ULONG i = 0; i < cSize; i++)
	{
		listBwd.Prepend(&rgelem[i]);
		listFwd.Append(&rgelem[i]);
	}

	// use getnext to walk list
	SElem *pelem = listFwd.First();
	for (ULONG i = 0; i < cSize; i++)
	{
		SPQOS_ASSERT(pelem == &rgelem[i]);
		pelem = listFwd.Next(pelem);
	}
	SPQOS_ASSERT(NULL == pelem);

	// go to end of list -- then traverse backward
	pelem = listFwd.First();
	while (pelem && listFwd.Next(pelem))
	{
		pelem = listFwd.Next(pelem);
	}
	SPQOS_ASSERT(listFwd.Last() == pelem);

	for (ULONG i = cSize; i > 0; i--)
	{
		SPQOS_ASSERT(pelem == &rgelem[i - 1]);
		pelem = listFwd.Prev(pelem);
	}
	SPQOS_ASSERT(NULL == pelem);

	SPQOS_DELETE_ARRAY(rgelem);
	return SPQOS_OK;
}



//---------------------------------------------------------------------------
//	@function:
//		CListTest::EresUnittest_Cursor
//
//	@doc:
//		Various cursor-based inserts
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CListTest::EresUnittest_Cursor()
{
	// create memory pool
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	CList<SElem> list;
	list.Init(SPQOS_OFFSET(SElem, m_linkFwd));

	ULONG cSize = 5;
	SElem *rgelem = SPQOS_NEW_ARRAY(mp, SElem, cSize);

	list.Append(&rgelem[0]);

	list.Prepend(&rgelem[1], list.First());
	list.Append(&rgelem[2], list.Last());

	SPQOS_ASSERT(&rgelem[1] == list.First());
	SPQOS_ASSERT(&rgelem[2] == list.Last());

	list.Prepend(&rgelem[3], list.Last());
	list.Append(&rgelem[4], list.First());

	SPQOS_ASSERT(&rgelem[1] == list.First());
	SPQOS_ASSERT(&rgelem[2] == list.Last());

	SPQOS_DELETE_ARRAY(rgelem);
	return SPQOS_OK;
}

// EOF
