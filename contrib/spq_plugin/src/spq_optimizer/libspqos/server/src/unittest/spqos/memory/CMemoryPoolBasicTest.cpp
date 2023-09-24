//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2008 Greenplum, Inc.
//
//	@filename:
//		CMemoryPoolBasicTest.cpp
//
//	@doc:
//		Tests for CMemoryPoolBasicTest
//---------------------------------------------------------------------------

#include "unittest/spqos/memory/CMemoryPoolBasicTest.h"

#include "spqos/assert.h"
#include "spqos/common/CAutoTimer.h"
#include "spqos/common/clibwrapper.h"
#include "spqos/error/CErrorHandlerStandard.h"
#include "spqos/error/CException.h"
#include "spqos/io/COstreamString.h"
#include "spqos/memory/CAutoMemoryPool.h"
#include "spqos/memory/CMemoryVisitorPrint.h"
#include "spqos/string/CWStringDynamic.h"
#include "spqos/task/CAutoTaskProxy.h"
#include "spqos/task/CAutoTraceFlag.h"
#include "spqos/task/CWorkerPoolManager.h"
#include "spqos/test/CUnittest.h"

#define SPQOS_MEM_TEST_ALLOC_SMALL (8)
#define SPQOS_MEM_TEST_ALLOC_LARGE (256)

using namespace spqos;

//---------------------------------------------------------------------------
//	@function:
//		CMemoryPoolBasicTest::EresUnittest
//
//	@doc:
//		Basic tests for memory management abstraction
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CMemoryPoolBasicTest::EresUnittest()
{
	CUnittest rgut[] = {
#ifdef SPQOS_DEBUG
		SPQOS_UNITTEST_FUNC(CMemoryPoolBasicTest::EresUnittest_Print),
#endif	// SPQOS_DEBUG
		SPQOS_UNITTEST_FUNC(CMemoryPoolBasicTest::EresUnittest_TestTracker)};

	CAutoTraceFlag atf(EtraceTestMemoryPools, true /*value*/);

	return CUnittest::EresExecute(rgut, SPQOS_ARRAY_SIZE(rgut));
}


#ifdef SPQOS_DEBUG

//---------------------------------------------------------------------------
//	@function:
//		CMemoryPoolBasicTest::EresUnittest_Print
//
//	@doc:
//		Print memory pools
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CMemoryPoolBasicTest::EresUnittest_Print()
{
	CAutoTraceFlag atfStackTrace(EtracePrintMemoryLeakStackTrace, true);

	const ULONG ulBufferSize = 4096;
	WCHAR wsz[ulBufferSize];
	CWStringStatic str(wsz, SPQOS_ARRAY_SIZE(wsz));
	COstreamString os(&str);

	(void) CMemoryPoolManager::GetMemoryPoolMgr()->OsPrint(os);
	SPQOS_TRACE(str.GetBuffer());

	return SPQOS_OK;
}

#endif	// SPQOS_DEBUG


//---------------------------------------------------------------------------
//	@function:
//		CMemoryPoolBasicTest::EresUnittest_TestTracker
//
//	@doc:
//		Run tests for pool tracking allocations
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CMemoryPoolBasicTest::EresUnittest_TestTracker()
{
	return EresTestType();
}


//---------------------------------------------------------------------------
//	@function:
//		CMemoryPoolBasicTest::EresTestType
//
//	@doc:
//		Run tests per pool type
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CMemoryPoolBasicTest::EresTestType()
{
	if (SPQOS_OK != EresNewDelete() ||
		SPQOS_OK != EresTestExpectedError(EresThrowingCtor, CException::ExmiOOM)

#ifdef SPQOS_DEBUG
		|| SPQOS_OK != EresTestExpectedError(EresLeak, CException::ExmiAssert) ||
		SPQOS_OK !=
			EresTestExpectedError(EresLeakByException, CException::ExmiAssert)
#endif	// SPQOS_DEBUG
	)
	{
		return SPQOS_FAILED;
	}

	return SPQOS_OK;
}


//---------------------------------------------------------------------------
//	@function:
//		CMemoryPoolBasicTest::EresTestExpectedError
//
//	@doc:
//		Run test that is expected to raise an exception
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CMemoryPoolBasicTest::EresTestExpectedError(SPQOS_RESULT (*pfunc)(), ULONG minor)
{
	CErrorHandlerStandard errhdl;
	SPQOS_TRY_HDL(&errhdl)
	{
		pfunc();
	}
	SPQOS_CATCH_EX(ex)
	{
		if (CException::ExmaSystem == ex.Major() && minor == ex.Minor())
		{
			SPQOS_RESET_EX;

			return SPQOS_OK;
		}

		SPQOS_RETHROW(ex);
	}
	SPQOS_CATCH_END;

	return SPQOS_FAILED;
}


//---------------------------------------------------------------------------
//	@function:
//		CMemoryPoolBasicTest::EresNewDelete
//
//	@doc:
//		Basic tests for allocation and free-ing
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CMemoryPoolBasicTest::EresNewDelete()
{
	// create memory pool
	CAutoTimer at("NewDelete test", true /*fPrint*/);
	CAutoMemoryPool amp(CAutoMemoryPool::ElcExc);
	CMemoryPool *mp = amp.Pmp();

	WCHAR rgwszText[] = SPQOS_WSZ_LIT(
		"This is a lengthy test string. "
		"Nothing serious just to demonstrate that "
		"you can allocate using New and free using "
		"delete. That's all. Special characters anybody? "
		"Sure thing: \x07 \xAB \xFF!. End of string.");

	// use overloaded New operator
	WCHAR *wsz = SPQOS_NEW_ARRAY(mp, WCHAR, SPQOS_ARRAY_SIZE(rgwszText));
	(void) clib::Wmemcpy(wsz, rgwszText, SPQOS_ARRAY_SIZE(rgwszText));

#ifdef SPQOS_DEBUG

	WCHAR rgBuffer[8 * 1024];
	CWStringStatic str(rgBuffer, SPQOS_ARRAY_SIZE(rgBuffer));
	COstreamString os(&str);

	// dump allocations
	if (mp->SupportsLiveObjectWalk())
	{
		CMemoryVisitorPrint movp(os);
		mp->WalkLiveObjects(&movp);
	}
	else
	{
		os << "Memory dump unavailable";
	}
	os << std::endl;

	SPQOS_TRACE(str.GetBuffer());

#endif	// SPQOS_DEBUG

	SPQOS_DELETE_ARRAY(wsz);

	return SPQOS_OK;
}

//---------------------------------------------------------------------------
//	@function:
//		CMemoryPoolBasicTest::EresThrowingCtor
//
//	@doc:
//		Basic tests for exception in constructor
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CMemoryPoolBasicTest::EresThrowingCtor()
{
	CAutoTimer at("ThrowingCtor test", true /*fPrint*/);

	// create memory pool
	CAutoMemoryPool amp(CAutoMemoryPool::ElcExc);
	CMemoryPool *mp = amp.Pmp();

	// malicious test class
	class CMyTestClass
	{
	public:
		CMyTestClass()
		{
			// throw in ctor
			SPQOS_RAISE(CException::ExmaSystem, CException::ExmiOOM);
		}
	};

	// try instantiating the class
	SPQOS_NEW(mp) CMyTestClass();

	// doesn't reach this line
	return SPQOS_FAILED;
}


#ifdef SPQOS_DEBUG

//---------------------------------------------------------------------------
//	@function:
//		CMemoryPoolBasicTest::EresLeak
//
//	@doc:
//		Basic tests for leak checking
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CMemoryPoolBasicTest::EresLeak()
{
	CAutoTraceFlag atfDump(EtracePrintMemoryLeakDump, true);
	CAutoTraceFlag atfStackTrace(EtracePrintMemoryLeakStackTrace, true);
	CAutoTimer at("Leak test", true /*fPrint*/);

	// scope for pool
	{
		CAutoMemoryPool amp(CAutoMemoryPool::ElcStrict);
		CMemoryPool *mp = amp.Pmp();

		for (ULONG i = 0; i < 10; i++)
		{
			// use overloaded New operator
			ULONG *rgul = SPQOS_NEW_ARRAY(mp, ULONG, 10);
			rgul[2] = 1;

			if (i < 8)
			{
				SPQOS_DELETE_ARRAY(rgul);
			}
		}
	}

	return SPQOS_FAILED;
}


//---------------------------------------------------------------------------
//	@function:
//		CMemoryPoolBasicTest::EresLeakByException
//
//	@doc:
//		Basic test for ignored leaks under exception
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CMemoryPoolBasicTest::EresLeakByException()
{
	CAutoTraceFlag atfDump(EtracePrintMemoryLeakDump, true);
	CAutoTraceFlag atfStackTrace(EtracePrintMemoryLeakStackTrace, true);
	CAutoTimer at("LeakByException test", true /*fPrint*/);

	// scope for pool
	{
		// create memory pool
		CAutoMemoryPool amp(CAutoMemoryPool::ElcExc);
		CMemoryPool *mp = amp.Pmp();

		for (ULONG i = 0; i < 10; i++)
		{
			// use overloaded New operator
			ULONG *rgul = SPQOS_NEW_ARRAY(mp, ULONG, 3);
			rgul[2] = 1;
		}

		SPQOS_ASSERT(!"Trigger leak with exception");
	}

	return SPQOS_FAILED;
}

#endif	// SPQOS_DEBUG


//---------------------------------------------------------------------------
//	@function:
//		CMemoryPoolBasicTest::Size
//
//	@doc:
//		Pick allocation size based on offset
//
//---------------------------------------------------------------------------
ULONG
CMemoryPoolBasicTest::Size(ULONG offset)
{
	if (0 == (offset & 1))
	{
		return SPQOS_MEM_TEST_ALLOC_SMALL;
	}
	return SPQOS_MEM_TEST_ALLOC_LARGE;
}

// EOF
