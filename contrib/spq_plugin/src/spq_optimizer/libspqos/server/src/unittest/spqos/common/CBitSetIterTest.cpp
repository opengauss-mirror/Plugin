//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		CBitSetIterTest.cpp
//
//	@doc:
//		Test of bitset iterator
//---------------------------------------------------------------------------

#include "unittest/spqos/common/CBitSetIterTest.h"

#include "spqos/base.h"
#include "spqos/common/CAutoRef.h"
#include "spqos/common/CBitSetIter.h"
#include "spqos/memory/CAutoMemoryPool.h"
#include "spqos/test/CUnittest.h"

using namespace spqos;

//---------------------------------------------------------------------------
//	@function:
//		CBitSetIter::EresUnittest
//
//	@doc:
//		Unittest for bit vectors
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CBitSetIterTest::EresUnittest()
{
	CUnittest rgut[] = {
		SPQOS_UNITTEST_FUNC(CBitSetIterTest::EresUnittest_Basics),

#ifdef SPQOS_DEBUG
		SPQOS_UNITTEST_FUNC_ASSERT(CBitSetIterTest::EresUnittest_Uninitialized),
		SPQOS_UNITTEST_FUNC_ASSERT(CBitSetIterTest::EresUnittest_Overrun)
#endif	// SPQOS_DEBUG
	};

	return CUnittest::EresExecute(rgut, SPQOS_ARRAY_SIZE(rgut));
}

//---------------------------------------------------------------------------
//	@function:
//		CBitSetIterTest::EresUnittest_Basics
//
//	@doc:
//		Testing ctors/dtor
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CBitSetIterTest::EresUnittest_Basics()
{
	// create memory pool
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	ULONG vector_size = 32;
	CBitSet *pbs = SPQOS_NEW(mp) CBitSet(mp, vector_size);

	ULONG cInserts = 10;
	for (ULONG i = 0; i < cInserts; i++)
	{
		// forces addition of new link
		pbs->ExchangeSet(i * vector_size);
	}

	ULONG cCount = 0;
	CBitSetIter bsi(*pbs);
	while (bsi.Advance())
	{
		SPQOS_ASSERT(bsi.Bit() == (bsi.Bit() / vector_size) * vector_size);
		SPQOS_ASSERT((BOOL) bsi);

		cCount++;
	}
	SPQOS_ASSERT(cInserts == cCount);

	SPQOS_ASSERT(!((BOOL) bsi));

	pbs->Release();

	return SPQOS_OK;
}


#ifdef SPQOS_DEBUG
//---------------------------------------------------------------------------
//	@function:
//		CBitSetIterTest::EresUnittest_Uninitialized
//
//	@doc:
//		Test for uninitialized access
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CBitSetIterTest::EresUnittest_Uninitialized()
{
	// create memory pool
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	ULONG vector_size = 32;

	CAutoRef<CBitSet> a_pbs;
	CBitSet *pbs = SPQOS_NEW(mp) CBitSet(mp, vector_size);
	a_pbs = pbs;

	CBitSetIter bsi(*pbs);

	// this throws
	bsi.Bit();

	return SPQOS_FAILED;
}


//---------------------------------------------------------------------------
//	@function:
//		CBitSetIterTest::EresUnittest_Overrun
//
//	@doc:
//		Test for calling Advance on exhausted iter
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CBitSetIterTest::EresUnittest_Overrun()
{
	// create memory pool
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	ULONG vector_size = 32;

	CAutoRef<CBitSet> a_pbs;
	CBitSet *pbs = SPQOS_NEW(mp) CBitSet(mp, vector_size);
	a_pbs = pbs;

	CBitSetIter bsi(*pbs);

	while (bsi.Advance())
	{
	}

	// this throws
	bsi.Advance();

	return SPQOS_FAILED;
}

#endif	// SPQOS_DEBUG

// EOF
