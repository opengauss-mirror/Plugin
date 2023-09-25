//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2008 Greenplum, Inc.
//
//	@filename:
//		CRefCountTest.cpp
//
//	@doc:
//		Tests for CRefCount
//---------------------------------------------------------------------------

#include "unittest/spqos/common/CRefCountTest.h"

#include "spqos/base.h"
#include "spqos/common/CRefCount.h"
#include "spqos/memory/CAutoMemoryPool.h"
#include "spqos/task/CAutoTraceFlag.h"
#include "spqos/test/CUnittest.h"

using namespace spqos;

//---------------------------------------------------------------------------
//	@function:
//		CRefCountTest::EresUnittest
//
//	@doc:
//		Unittest for ref-counting
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CRefCountTest::EresUnittest()
{
	CUnittest rgut[] = {
		SPQOS_UNITTEST_FUNC(CRefCountTest::EresUnittest_CountUpAndDown),
		SPQOS_UNITTEST_FUNC(CRefCountTest::EresUnittest_DeletableObjects)

#ifdef SPQOS_DEBUG
			,
		SPQOS_UNITTEST_FUNC_ASSERT(CRefCountTest::EresUnittest_Stack),
		SPQOS_UNITTEST_FUNC_ASSERT(CRefCountTest::EresUnittest_Check)
#endif	// SPQOS_DEBUG
	};

	return CUnittest::EresExecute(rgut, SPQOS_ARRAY_SIZE(rgut));
}


//---------------------------------------------------------------------------
//	@function:
//		CRefCountTest::EresUnittest_CountUpAndDown
//
//	@doc:
//		Simple count up and down of ref counted object
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CRefCountTest::EresUnittest_CountUpAndDown()
{
	// create memory pool
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	// blank ref count object
	CRefCount *pref = SPQOS_NEW(mp) CRefCount;

	// add counts
	for (ULONG i = 0; i < 10; i++)
	{
		pref->AddRef();
	}

	// release all additional refs
	for (ULONG i = 0; i < 10; i++)
	{
		pref->Release();
	}

	// destruct the object
	pref->Release();

	return SPQOS_OK;
}

//---------------------------------------------------------------------------
//	@function:
//		CRefCountTest::EresUnittest_DeletableObjects
//
//	@doc:
//		Test deletable/undeletable objects
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CRefCountTest::EresUnittest_DeletableObjects()
{
	// create memory pool
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	CAutoTraceFlag atfOOM(EtraceSimulateOOM, false);

	CDeletableTest *pdt = SPQOS_NEW(mp) CDeletableTest;

	SPQOS_TRY
	{
		// trying to release object here throws InvalidDeletion exception
		pdt->Release();
	}
	SPQOS_CATCH_EX(ex)
	{
		if (!SPQOS_MATCH_EX(ex, CException::ExmaSystem,
						   CException::ExmiInvalidDeletion))
		{
			// unexpected exception -- rethrow it
			SPQOS_RETHROW(ex);
		}

		SPQOS_RESET_EX;
	}
	SPQOS_CATCH_END;

	pdt->AllowDeletion();

	// now deletion is allowed
	pdt->Release();

	return SPQOS_OK;
}


#ifdef SPQOS_DEBUG

//---------------------------------------------------------------------------
//	@function:
//		CRefCountTest::EresUnittest_Stack
//
//	@doc:
//		Put CRefCount on stack -- this must assert in destructor
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CRefCountTest::EresUnittest_Stack()
{
	CRefCount ref;

	// does not reach this line
	return SPQOS_FAILED;
}



//---------------------------------------------------------------------------
//	@function:
//		CRefCountTest::EresUnittest_Check
//
//	@doc:
//		Call AddRef on a deleted ref count; this test is quite experimental;
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CRefCountTest::EresUnittest_Check()
{
	// create memory pool
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	BYTE *rgb = SPQOS_NEW_ARRAY(mp, BYTE, 128);
	CRefCount *pref = (CRefCount *) rgb;

	SPQOS_DELETE_ARRAY(rgb);


	// must throw
	pref->AddRef();

	// does not reach this line
	return SPQOS_FAILED;
}

#endif	// SPQOS_DEBUG

// EOF
