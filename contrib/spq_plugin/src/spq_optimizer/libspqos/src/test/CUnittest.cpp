//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (c) 2004-2015 Pivotal Software, Inc.
//
//	@filename:
//		CUnittest.cpp
//
//	@doc:
//		Driver for unittests
//---------------------------------------------------------------------------

#include "spqos/test/CUnittest.h"

#include <stddef.h>

#include "spqos/base.h"
#include "spqos/common/CAutoTimer.h"
#include "spqos/common/clibwrapper.h"
#include "spqos/error/CErrorHandlerStandard.h"
#include "spqos/memory/CAutoMemoryPool.h"
#include "spqos/task/CTask.h"
#include "spqos/task/CWorker.h"

using namespace spqos;


// threshold of oversized memory pool
#define SPQOS_OVERSIZED_POOL_SIZE 500 * 1024 * 1024

// initialize static members
CUnittest *CUnittest::m_rgut = NULL;
ULONG CUnittest::m_ulTests = 0;
ULONG CUnittest::m_ulNested = 0;
void (*CUnittest::m_pfConfig)() = NULL;
void (*CUnittest::m_pfCleanup)() = NULL;


//---------------------------------------------------------------------------
//	@function:
//		CUnittest::CUnittest
//
//	@doc:
//		Constructor for exception-free test
//
//---------------------------------------------------------------------------
CUnittest::CUnittest(const CHAR *szTitle, ETestType ett,
					 SPQOS_RESULT (*pfunc)(void))
	: m_szTitle(szTitle),
	  m_ett(ett),
	  m_pfunc(pfunc),
	  m_pfuncSubtest(NULL),
	  m_ulSubtest(0),
	  m_fExcep(false),
	  m_ulMajor(CException::ExmaInvalid),
	  m_ulMinor(CException::ExmiInvalid)
{
}


//---------------------------------------------------------------------------
//	@function:
//		CUnittest::CUnittest
//
//	@doc:
//		Constructor for test which are expected to throw an exception
//
//---------------------------------------------------------------------------
CUnittest::CUnittest(const CHAR *szTitle, ETestType ett,
					 SPQOS_RESULT (*pfunc)(void), ULONG major, ULONG minor)
	: m_szTitle(szTitle),
	  m_ett(ett),
	  m_pfunc(pfunc),
	  m_pfuncSubtest(NULL),
	  m_ulSubtest(0),
	  m_fExcep(true),
	  m_ulMajor(major),
	  m_ulMinor(minor)
{
}

//---------------------------------------------------------------------------
//	@function:
//		CUnittest::CUnittest
//
//	@doc:
//		Constructor for subtest identified by ULONG id
//
//---------------------------------------------------------------------------
CUnittest::CUnittest(const CHAR *szTitle, ETestType ett,
					 SPQOS_RESULT (*pfuncSubtest)(ULONG), ULONG ulSubtest)
	: m_szTitle(szTitle),
	  m_ett(ett),
	  m_pfunc(NULL),
	  m_pfuncSubtest(pfuncSubtest),
	  m_ulSubtest(ulSubtest),
	  m_fExcep(false),
	  m_ulMajor(CException::ExmaInvalid),
	  m_ulMinor(CException::ExmiInvalid)
{
}


//---------------------------------------------------------------------------
//	@function:
//		CUnittest::CUnittest
//
//	@doc:
//		Copy constructor
//
//---------------------------------------------------------------------------
CUnittest::CUnittest(const CUnittest &ut)
	: m_szTitle(ut.m_szTitle),
	  m_ett(ut.m_ett),
	  m_pfunc(ut.m_pfunc),
	  m_pfuncSubtest(ut.m_pfuncSubtest),
	  m_ulSubtest(ut.m_ulSubtest),
	  m_fExcep(ut.m_fExcep),
	  m_ulMajor(ut.m_ulMajor),
	  m_ulMinor(ut.m_ulMinor)
{
}



//---------------------------------------------------------------------------
//	@function:
//		CUnittest::FThrows
//
//	@doc:
//		Is test expected to throw?
//
//---------------------------------------------------------------------------
BOOL
CUnittest::FThrows() const
{
	return m_fExcep;
}


//---------------------------------------------------------------------------
//	@function:
//		CUnittest::Equals
//
//	@doc:
//		Is given string equal to title of test?
//
//---------------------------------------------------------------------------
BOOL
CUnittest::Equals(CHAR *sz) const
{
	return 0 == clib::Strcmp(sz, m_szTitle);
}


//---------------------------------------------------------------------------
//	@function:
//		CUnittest::FThrows
//
//	@doc:
//		Is test expected to throw given exception?
//
//---------------------------------------------------------------------------
BOOL
CUnittest::FThrows(ULONG major, ULONG minor) const
{
	return (m_ulMajor == major && m_ulMinor == minor);
}


//---------------------------------------------------------------------------
//	@function:
//		CUnittest::EresExecLoop
//
//	@doc:
//		Execute a single test -- top-level wrapper;
//		Retries individual complex tests if exception simulation is enabled
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CUnittest::EresExecLoop(const CUnittest &ut)
{
	while (true)
	{
		SPQOS_TRY
		{
			return EresExecTest(ut);
		}
		SPQOS_CATCH_EX(ex)
		{
			// check for exception simulation
			if (ITask::Self()->IsTraceSet(EtraceSimulateOOM))
			{
				SPQOS_ASSERT(SPQOS_MATCH_EX(ex, CException::ExmaSystem,
										  CException::ExmiOOM));
			}
			else if (ITask::Self()->IsTraceSet(EtraceSimulateAbort))
			{
				SPQOS_ASSERT(SPQOS_MATCH_EX(ex, CException::ExmaSystem,
										  CException::ExmiAbort));
			}
			else if (ITask::Self()->IsTraceSet(EtraceSimulateIOError))
			{
				SPQOS_ASSERT(SPQOS_MATCH_EX(ex, CException::ExmaSystem,
										  CException::ExmiIOError));
			}
			else if (ITask::Self()->IsTraceSet(EtraceSimulateNetError))
			{
				SPQOS_ASSERT(SPQOS_MATCH_EX(ex, CException::ExmaSystem,
										  CException::ExmiNetError));
			}
			else
			{
				// unexpected exception
				SPQOS_RETHROW(ex);
			}

			// reset & retry
			SPQOS_RESET_EX;
		}
		SPQOS_CATCH_END;
	}

	SPQOS_ASSERT(!"Unexpected end of loop");
	return SPQOS_FAILED;
}


//---------------------------------------------------------------------------
//	@function:
//		CUnittest::EresExecTest
//
//	@doc:
//		Execute a single test
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CUnittest::EresExecTest(const CUnittest &ut)
{
	SPQOS_RESULT eres = SPQOS_FAILED;

	CErrorHandlerStandard errhdl;
	SPQOS_TRY_HDL(&errhdl)
	{
		// reset cancellation flag
		CTask::Self()->ResetCancel();

		eres = ut.m_pfunc != NULL ? ut.m_pfunc()
								  : ut.m_pfuncSubtest(ut.m_ulSubtest);

		// check if this was expected to throw
		if (ut.FThrows())
		{
			// should have thrown
			return SPQOS_FAILED;
		}

		SPQOS_CHECK_ABORT;

		return eres;
	}
	SPQOS_CATCH_EX(ex)
	{
		// if time slice was exceeded, mark test as failed
		if (SPQOS_MATCH_EX(ex, CException::ExmaSystem,
						  CException::ExmiAbortTimeout))
		{
			SPQOS_RESET_EX;
			return SPQOS_FAILED;
		}

		// check if exception was expected
		if (ut.FThrows(ex.Major(), ex.Minor()))
		{
			SPQOS_RESET_EX;
			return SPQOS_OK;
		}

		// check if exception was injected by simulation
		if (FSimulated(ex))
		{
			SPQOS_RETHROW(ex);
		}

		SPQOS_RESET_EX;
	}
	SPQOS_CATCH_END;

	return SPQOS_FAILED;
}


//---------------------------------------------------------------------------
//	@function:
//		CUnittest::FSimulated
//
//	@doc:
//		Check if exception was injected by simulation
//
//---------------------------------------------------------------------------
BOOL
CUnittest::FSimulated(CException ex)
{
	ITask *ptsk = ITask::Self();
	SPQOS_ASSERT(NULL != ptsk);

	return (ptsk->IsTraceSet(EtraceSimulateOOM) &&
			SPQOS_MATCH_EX(ex, CException::ExmaSystem, CException::ExmiOOM)) ||

		   (ptsk->IsTraceSet(EtraceSimulateAbort) &&
			SPQOS_MATCH_EX(ex, CException::ExmaSystem, CException::ExmiAbort)) ||

		   (ptsk->IsTraceSet(EtraceSimulateIOError) &&
			SPQOS_MATCH_EX(ex, CException::ExmaSystem,
						  CException::ExmiIOError)) ||

		   (ptsk->IsTraceSet(EtraceSimulateNetError) &&
			SPQOS_MATCH_EX(ex, CException::ExmaSystem,
						  CException::ExmiNetError));
}

//---------------------------------------------------------------------------
//	@function:
//		CUnittest::EresExecute
//
//	@doc:
//		Execute a set of test given as an array of CUnittest objects
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CUnittest::EresExecute(const CUnittest *rgut, const ULONG cSize)
{
	SPQOS_RESULT eres = SPQOS_OK;

	for (ULONG i = 0; i < cSize; i++)
	{
		SPQOS_RESULT eresPart = SPQOS_FAILED;
		const CUnittest &ut = rgut[i];

		{  // scope for timer
			CAutoTimer timer(ut.m_szTitle, true /*fPrint*/);
			eresPart = EresExecLoop(ut);
		}

		SPQOS_TRACE_FORMAT("Unittest %s...%s.", ut.m_szTitle,
						  (SPQOS_OK == eresPart ? "OK" : "*** FAILED ***"));

#ifdef SPQOS_DEBUG
		{
			CAutoMemoryPool amp;
			CMemoryPoolManager::GetMemoryPoolMgr()->PrintOverSizedPools(
				amp.Pmp(), SPQOS_OVERSIZED_POOL_SIZE);
		}
#endif	// SPQOS_DEBUG

		// invalidate result summary if any part fails
		if (SPQOS_OK != eresPart)
		{
			eres = SPQOS_FAILED;
		}
	}

	return eres;
}


//---------------------------------------------------------------------------
//	@function:
//		CUnittest::UlFindTest
//
//	@doc:
//		Find a test with given attributes and add it the list;
//		If no name passed (NULL), match only on attribute;
//
//---------------------------------------------------------------------------
void
CUnittest::FindTest(CBitVector &bv, ETestType ett, CHAR *szTestName)
{
	for (ULONG i = 0; i < CUnittest::m_ulTests; i++)
	{
		CUnittest &ut = CUnittest::m_rgut[i];

		if ((ut.Ett() == ett &&
			 (NULL == szTestName || ut.Equals(szTestName))) ||
			(NULL != szTestName && ut.Equals(szTestName)))
		{
			(void) bv.ExchangeSet(i);
		}
	}

	if (bv.IsEmpty())
	{
		SPQOS_TRACE_FORMAT("'%s' is not a valid test case.", szTestName);
	}
}


//---------------------------------------------------------------------------
//	@function:
//		CUnittest::SetTraceFlag
//
//	@doc:
//		Parse and set a trace flag
//
//---------------------------------------------------------------------------
void
CUnittest::SetTraceFlag(const CHAR *szTrace)
{
	CHAR *pcEnd = NULL;
	LINT lTrace = clib::Strtol(szTrace, &pcEnd, 0 /*iBase*/);

	SPQOS_SET_TRACE((ULONG) lTrace);
}

// Parse plan id
ULLONG
CUnittest::UllParsePlanId(const CHAR *szPlanId)
{
	CHAR *pcEnd = NULL;
	LINT ullPlanId = clib::Strtol(szPlanId, &pcEnd, 0 /*iBase*/);
	return ullPlanId;
}

//---------------------------------------------------------------------------
//	@function:
//		CUnittest::EresExecute
//
//	@doc:
//		Execute requested unittests
//
//---------------------------------------------------------------------------
ULONG
CUnittest::Driver(CBitVector *pbv)
{
	CAutoConfig ac(m_pfConfig, m_pfCleanup, m_ulNested);
	ULONG ulOk = 0;

	// scope of timer
	{
		spqos::CAutoTimer timer("total test run time", true /*fPrint*/);

		for (ULONG i = 0; i < CUnittest::m_ulTests; i++)
		{
			if (pbv->Get(i))
			{
				CUnittest &ut = CUnittest::m_rgut[i];
				SPQOS_RESULT eres = EresExecute(&ut, 1 /*size*/);
				SPQOS_ASSERT((SPQOS_OK == eres || SPQOS_FAILED == eres) &&
							"Unexpected result from unittest");

				if (SPQOS_OK == eres)
				{
					++ulOk;
				}

#ifdef SPQOS_DEBUG
				{
					CAutoMemoryPool amp;
					CMemoryPoolManager::GetMemoryPoolMgr()->PrintOverSizedPools(
						amp.Pmp(), SPQOS_OVERSIZED_POOL_SIZE);
				}
#endif	// SPQOS_DEBUG
			}
		}
	}

	SPQOS_TRACE_FORMAT("Tests succeeded: %d", ulOk);
	SPQOS_TRACE_FORMAT("Tests failed:    %d", pbv->CountSetBits() - ulOk);

	return pbv->CountSetBits() - ulOk;
}


//---------------------------------------------------------------------------
//	@function:
//		CUnittest::EresExecute
//
//	@doc:
//		Execute unittests by parsing input arguments
//
//---------------------------------------------------------------------------
ULONG
CUnittest::Driver(CMainArgs *pma)
{
	CBitVector bv(ITask::Self()->Pmp(), CUnittest::UlTests());

	CHAR ch = '\0';
	while (pma->Getopt(&ch))
	{
		CHAR *szTestName = NULL;

		switch (ch)
		{
			case 'U':
				szTestName = optarg;
				// fallthru
			case 'u':
				FindTest(bv, EttStandard, szTestName);
				break;

			case 'x':
				FindTest(bv, EttExtended, NULL /*szTestName*/);
				break;

			case 'T':
				SetTraceFlag(optarg);

			default:
				// ignore other parameters
				break;
		}
	}

	return Driver(&bv);
}


//---------------------------------------------------------------------------
//	@function:
//		CUnittest::Init
//
//	@doc:
//		Initialize unittest array
//
//---------------------------------------------------------------------------
void
CUnittest::Init(CUnittest *rgut, ULONG ulUtCnt, void (*pfConfig)(),
				void (*pfCleanup)())
{
	SPQOS_ASSERT(0 == m_ulTests &&
				"Unittest array has already been initialized");

	m_rgut = rgut;
	m_ulTests = ulUtCnt;
	m_pfConfig = pfConfig;
	m_pfCleanup = pfCleanup;
}

// EOF
