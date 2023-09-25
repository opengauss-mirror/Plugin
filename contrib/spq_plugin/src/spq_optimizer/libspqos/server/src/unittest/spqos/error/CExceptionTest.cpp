//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2008 Greenplum, Inc.
//
//	@filename:
//		CExceptionTest.cpp
//
//	@doc:
//		Tests for CException
//---------------------------------------------------------------------------

#include "unittest/spqos/error/CExceptionTest.h"

#include "spqos/assert.h"
#include "spqos/error/CErrorContext.h"
#include "spqos/error/CException.h"
#include "spqos/task/CWorkerPoolManager.h"
#include "spqos/test/CUnittest.h"

using namespace spqos;

//---------------------------------------------------------------------------
//	@function:
//		CExceptionTest::EresUnittest
//
//	@doc:
//		Function for raising assert exceptions; again, encapsulated in a function
//		to facilitate debugging
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CExceptionTest::EresUnittest()
{
	CUnittest rgut[] = {
		SPQOS_UNITTEST_FUNC(CExceptionTest::EresUnittest_BasicThrow),
		SPQOS_UNITTEST_FUNC_THROW(CExceptionTest::EresUnittest_StackOverflow,
								 CException::ExmaSystem,
								 CException::ExmiOutOfStack),

		SPQOS_UNITTEST_FUNC_THROW(CExceptionTest::EresUnittest_AdditionOverflow,
								 CException::ExmaSystem,
								 CException::ExmiOverflow),

		SPQOS_UNITTEST_FUNC_THROW(
			CExceptionTest::EresUnittest_MultiplicationOverflow,
			CException::ExmaSystem, CException::ExmiOverflow),

#ifdef SPQOS_DEBUG
		SPQOS_UNITTEST_FUNC_THROW(CExceptionTest::EresUnittest_BasicRethrow,
								 CException::ExmaSystem, CException::ExmiOOM),
		SPQOS_UNITTEST_FUNC_ASSERT(CExceptionTest::EresUnittest_Assert),
		SPQOS_UNITTEST_FUNC_ASSERT(CExceptionTest::EresUnittest_AssertImp),
		SPQOS_UNITTEST_FUNC_ASSERT(CExceptionTest::EresUnittest_AssertIffLHS),
		SPQOS_UNITTEST_FUNC_ASSERT(CExceptionTest::EresUnittest_AssertIffRHS)
#endif	// SPQOS_DEBUG
	};

	return CUnittest::EresExecute(rgut, SPQOS_ARRAY_SIZE(rgut));
}


//---------------------------------------------------------------------------
//	@function:
//		CExceptionTest::EresUnittest_BasicThrow
//
//	@doc:
//		Basic raising of exception and catching
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CExceptionTest::EresUnittest_BasicThrow()
{
	SPQOS_TRY
	{
		SPQOS_RAISE(CException::ExmaSystem, CException::ExmiOOM);
		return SPQOS_FAILED;
	}
	SPQOS_CATCH_EX(ex)
	{
		if (SPQOS_MATCH_EX(ex, CException::ExmaSystem, CException::ExmiOOM))
		{
			SPQOS_RESET_EX;
			return SPQOS_OK;
		}
	}
	SPQOS_CATCH_END;

	return SPQOS_FAILED;
}

//---------------------------------------------------------------------------
//	@function:
//		CExceptionTest::EresUnittest_StackOverflow
//
//	@doc:
//		Test stack overflow
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CExceptionTest::EresUnittest_StackOverflow()
{
	// character data to bloat the stack frame somewhat
	CHAR szTestData[5][1024] = {"5 KB data", "to bloat", "the", "stack frame"};

	// stack checker will throw after a few recursions
	IWorker::Self()->CheckStackSize();

	// infinite recursion
	CExceptionTest::EresUnittest_StackOverflow();

	SPQOS_ASSERT(!"Must not return from recursion");
	SPQOS_TRACE_FORMAT("%s", szTestData[0]);

	return SPQOS_FAILED;
}


//---------------------------------------------------------------------------
//	@function:
//		CExceptionTest::EresUnittest_AdditionOverflow
//
//	@doc:
//		Test addition overflow
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CExceptionTest::EresUnittest_AdditionOverflow()
{
	SPQOS_TRY
	{
		// additions that must pass
		(void) spqos::Add(spqos::ullong_max - 2, 1);
		(void) spqos::Add(spqos::ullong_max, 0);
		(void) spqos::Add(spqos::ullong_max - 2, 2);
	}
	SPQOS_CATCH_EX(ex)
	{
		// no exception is expected here
		return SPQOS_FAILED;
	}
	SPQOS_CATCH_END;

	// addition that throws overflow exception
	(void) spqos::Add(spqos::ullong_max, 1);

	SPQOS_ASSERT(!"Must not add numbers successfully");

	return SPQOS_FAILED;
}

//---------------------------------------------------------------------------
//	@function:
//		CExceptionTest::EresUnittest_MultiplicationOverflow
//
//	@doc:
//		Test multiplication overflow
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CExceptionTest::EresUnittest_MultiplicationOverflow()
{
	SPQOS_TRY
	{
		// multiplications that must pass
		(void) spqos::Multiply(spqos::ullong_max, 1);
		(void) spqos::Multiply(spqos::ullong_max, 0);
		(void) spqos::Multiply(spqos::ullong_max / 2, 2);
		(void) spqos::Multiply(spqos::ullong_max / 2 - 1, 2);
	}
	SPQOS_CATCH_EX(ex)
	{
		// no exception is expected here
		return SPQOS_FAILED;
	}
	SPQOS_CATCH_END;

	// multiplication that throws overflow exception
	(void) spqos::Multiply(spqos::ullong_max - 4, 2);

	SPQOS_ASSERT(!"Must not multiply numbers successfully");

	return SPQOS_FAILED;
}


#ifdef SPQOS_DEBUG
//---------------------------------------------------------------------------
//	@function:
//		CExceptionTest::EresUnittest_BasicRethrow
//
//	@doc:
//		Basic raising of exception and catching then rethrowing
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CExceptionTest::EresUnittest_BasicRethrow()
{
	SPQOS_TRY
	{
		SPQOS_RAISE(CException::ExmaSystem, CException::ExmiOOM);
		return SPQOS_FAILED;
	}
	SPQOS_CATCH_EX(ex)
	{
		if (SPQOS_MATCH_EX(ex, CException::ExmaSystem, CException::ExmiOOM))
		{
			SPQOS_RETHROW(ex);
			return SPQOS_FAILED;
		}
	}
	SPQOS_CATCH_END;

	return SPQOS_FAILED;
}


//---------------------------------------------------------------------------
//	@function:
//		CExceptionTest::EresUnittest_Assert
//
//	@doc:
//		Fail an assertion
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CExceptionTest::EresUnittest_Assert()
{
	SPQOS_ASSERT(2 * 2 == 5);

	return SPQOS_FAILED;
}


//---------------------------------------------------------------------------
//	@function:
//		CExceptionTest::EresUnittest_AssertImp
//
//	@doc:
//		Fail an implication assertion
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CExceptionTest::EresUnittest_AssertImp()
{
	// valid implications
	SPQOS_ASSERT_IMP(true, true);
	SPQOS_ASSERT_IMP(false, false);
	SPQOS_ASSERT_IMP(false, true);

	// incorrect implication
	SPQOS_ASSERT_IMP(true, false);

	return SPQOS_FAILED;
}


//---------------------------------------------------------------------------
//	@function:
//		CExceptionTest::EresUnittest_AssertIffLHS
//
//	@doc:
//		Fail an if-and-only-if assertion; LHS false
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CExceptionTest::EresUnittest_AssertIffLHS()
{
	// valid implications
	SPQOS_ASSERT_IFF(true, true);
	SPQOS_ASSERT_IFF(false, false);

	// failed assertion
	SPQOS_ASSERT_IFF(false, true);

	return SPQOS_FAILED;
}


//---------------------------------------------------------------------------
//	@function:
//		CExceptionTest::EresUnittest_AssertIffRHS
//
//	@doc:
//		Fail an if-and-only-if assertion; RHS false
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CExceptionTest::EresUnittest_AssertIffRHS()
{
	// valid implications
	SPQOS_ASSERT_IFF(true, true);
	SPQOS_ASSERT_IFF(false, false);

	// failed assertion
	SPQOS_ASSERT_IFF(true, false);

	return SPQOS_FAILED;
}

#endif	// SPQOS_DEBUG

// EOF
