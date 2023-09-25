//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2008 - 2010 Greenplum, Inc.
//
//	@filename:
//		CErrorHandlerTest.cpp
//
//	@doc:
//		Tests for error handler
//---------------------------------------------------------------------------

#include "unittest/spqos/error/CErrorHandlerTest.h"

#include "spqos/base.h"
#include "spqos/error/CErrorHandlerStandard.h"
#include "spqos/test/CUnittest.h"


using namespace spqos;

//---------------------------------------------------------------------------
//	@function:
//		CErrorHandlerTest::EresUnittest
//
//	@doc:
//		Function for raising assert exceptions; again, encapsulated in a function
//		to facilitate debugging
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CErrorHandlerTest::EresUnittest()
{
	CUnittest rgut[] = {
		SPQOS_UNITTEST_FUNC(CErrorHandlerTest::EresUnittest_Basic)
#ifdef SPQOS_DEBUG
			,
		SPQOS_UNITTEST_FUNC_ASSERT(CErrorHandlerTest::EresUnittest_BadRethrow),
		SPQOS_UNITTEST_FUNC_ASSERT(CErrorHandlerTest::EresUnittest_BadReset),
		SPQOS_UNITTEST_FUNC_ASSERT(CErrorHandlerTest::EresUnittest_Unhandled)
#endif	// SPQOS_DEBUG
	};

	return CUnittest::EresExecute(rgut, SPQOS_ARRAY_SIZE(rgut));
}


//---------------------------------------------------------------------------
//	@function:
//		CErrorHandlerTest::EresUnittest_Basic
//
//	@doc:
//		Basic handling of an error
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CErrorHandlerTest::EresUnittest_Basic()
{
	CErrorHandlerStandard errhdl;
	SPQOS_TRY_HDL(&errhdl)
	{
		// raise an OOM exception
		SPQOS_OOM_CHECK(NULL);
	}
	SPQOS_CATCH_EX(ex)
	{
		// make sure we catch an OOM
		SPQOS_ASSERT(
			SPQOS_MATCH_EX(ex, CException::ExmaSystem, CException::ExmiOOM));

		SPQOS_RESET_EX;
	}
	SPQOS_CATCH_END;

	return SPQOS_OK;
}

#ifdef SPQOS_DEBUG
//---------------------------------------------------------------------------
//	@function:
//		CErrorHandlerTest::EresUnittest_BadRethrow
//
//	@doc:
//		catch, reset, and attempt rethrow
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CErrorHandlerTest::EresUnittest_BadRethrow()
{
	SPQOS_TRY
	{
		// raise an OOM exception
		SPQOS_OOM_CHECK(NULL);
	}
	SPQOS_CATCH_EX(ex)
	{
		// make sure we catch an OOM
		SPQOS_ASSERT(
			SPQOS_MATCH_EX(ex, CException::ExmaSystem, CException::ExmiOOM));

		// reset error context -- ignore, don't handle
		SPQOS_RESET_EX;

		// this asserts because we've reset already
		SPQOS_RETHROW(ex);
	}
	SPQOS_CATCH_END;

	return SPQOS_FAILED;
}



//---------------------------------------------------------------------------
//	@function:
//		CErrorHandlerTest::EresUnittest_BadReset
//
//	@doc:
//		catch and reset twice
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CErrorHandlerTest::EresUnittest_BadReset()
{
	SPQOS_TRY
	{
		// raise an OOM exception
		SPQOS_OOM_CHECK(NULL);
	}
	SPQOS_CATCH_EX(ex)
	{
		// make sure we catch an OOM
		SPQOS_ASSERT(
			SPQOS_MATCH_EX(ex, CException::ExmaSystem, CException::ExmiOOM));

		// reset error context
		SPQOS_RESET_EX;

		// reset error context again -- this throws
		SPQOS_RESET_EX;
	}
	SPQOS_CATCH_END;

	return SPQOS_FAILED;
}


//---------------------------------------------------------------------------
//	@function:
//		CErrorHandlerTest::EresUnittest_Unhandled
//
//	@doc:
//		try to rethrow with a pending error
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CErrorHandlerTest::EresUnittest_Unhandled()
{
	SPQOS_TRY
	{
		// raise an OOM exception
		SPQOS_OOM_CHECK(NULL);
	}
	SPQOS_CATCH_EX(ex)
	{
		// make sure we catch an OOM
		SPQOS_ASSERT(
			SPQOS_MATCH_EX(ex, CException::ExmaSystem, CException::ExmiOOM));

		// do not reset or rethrow here...
	}
	SPQOS_CATCH_END;

	// try raising another OOM exception -- this must assert
	SPQOS_OOM_CHECK(NULL);

	return SPQOS_FAILED;
}

#endif	// SPQOS_DEBUG

// EOF
