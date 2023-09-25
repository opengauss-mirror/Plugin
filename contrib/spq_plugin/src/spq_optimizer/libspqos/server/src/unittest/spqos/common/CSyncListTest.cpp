//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CSyncListTest.cpp
//
//	@doc:
//		Tests for CSyncList
//---------------------------------------------------------------------------

#include "unittest/spqos/common/CSyncListTest.h"

#include "spqos/base.h"
#include "spqos/error/CAutoTrace.h"
#include "spqos/memory/CAutoMemoryPool.h"
#include "spqos/task/CAutoTaskProxy.h"
#include "spqos/task/CWorkerPoolManager.h"
#include "spqos/test/CUnittest.h"

#define SPQOS_SLIST_SIZE 10

using namespace spqos;

//---------------------------------------------------------------------------
//	@function:
//		CSyncListTest::EresUnittest
//
//	@doc:
//		Unittest for sync list
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CSyncListTest::EresUnittest()
{
	CUnittest rgut[] = {SPQOS_UNITTEST_FUNC(CSyncListTest::EresUnittest_Basics)};

	return CUnittest::EresExecute(rgut, SPQOS_ARRAY_SIZE(rgut));
}


//---------------------------------------------------------------------------
//	@function:
//		CSyncListTest::EresUnittest_Basics
//
//	@doc:
//		Various sync list operations
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CSyncListTest::EresUnittest_Basics()
{
	CSyncList<SElem> list;
	list.Init(SPQOS_OFFSET(SElem, m_link));

	SElem rgelem[SPQOS_SLIST_SIZE];

	// insert all elements
	for (ULONG i = 0; i < SPQOS_ARRAY_SIZE(rgelem); i++)
	{
		list.Push(&rgelem[i]);

		SPQOS_ASSERT(SPQOS_OK == list.Find(&rgelem[i]));
	}

#ifdef SPQOS_DEBUG
	// scope for auto trace
	{
		CAutoMemoryPool amp;
		CMemoryPool *mp = amp.Pmp();

		CAutoTrace trace(mp);
		IOstream &os(trace.Os());

		os << SPQOS_WSZ_LIT("Sync list contents:") << std::endl;
		list.OsPrint(os);
	}
#endif	// SPQOS_DEBUG

	// pop elements until empty
	for (ULONG i = 0; i < SPQOS_ARRAY_SIZE(rgelem); i++)
	{
#ifdef SPQOS_DEBUG
		SElem *pe =
#endif	// SPQOS_DEBUG
			list.Pop();

		SPQOS_ASSERT(pe == &rgelem[SPQOS_ARRAY_SIZE(rgelem) - i - 1]);
	}
	SPQOS_ASSERT(NULL == list.Pop());

	// insert all elements in reverse order
	for (ULONG i = SPQOS_ARRAY_SIZE(rgelem); i > 0; i--)
	{
		list.Push(&rgelem[i - 1]);

		SPQOS_ASSERT(SPQOS_OK == list.Find(&rgelem[i - 1]));
	}

	// pop elements until empty
	for (ULONG i = 0; i < SPQOS_ARRAY_SIZE(rgelem); i++)
	{
#ifdef SPQOS_DEBUG
		SElem *pe =
#endif	// SPQOS_DEBUG
			list.Pop();

		SPQOS_ASSERT(pe == &rgelem[i]);
	}
	SPQOS_ASSERT(NULL == list.Pop());

	return SPQOS_OK;
}


// EOF
