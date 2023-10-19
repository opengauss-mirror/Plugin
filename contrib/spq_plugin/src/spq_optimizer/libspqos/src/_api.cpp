//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (c) 2004-2015 Pivotal Software, Inc.
//
//	@filename:
//		_api.cpp
//
//	@doc:
//		Implementation of SPQOS wrapper interface for SPQDB.
//---------------------------------------------------------------------------

#include "spqos/_api.h"

#include "spqos/base.h"
#include "spqos/common/CAutoP.h"
#include "spqos/common/CDebugCounter.h"
#include "spqos/common/CMainArgs.h"
#include "spqos/error/CFSimulator.h"
#include "spqos/error/CLoggerStream.h"
#include "spqos/error/CMessageRepository.h"
#include "spqos/io/COstreamString.h"
#include "spqos/memory/CAutoMemoryPool.h"
#include "spqos/memory/CCacheFactory.h"
#include "spqos/string/CWStringStatic.h"
#include "spqos/task/CAutoTaskProxy.h"
#include "spqos/task/CWorkerPoolManager.h"

#include "spqopt/exception.h"
#include "naucrates/exception.h"

using namespace spqos;


// refer spqopt/exception.cpp for explanation of errors
const ULONG expected_opt_fallback[] = {
	spqopt::
		ExmiInvalidPlanAlternative,	 // chosen plan id is outside range of possible plans
	spqopt::ExmiUnsupportedOp,				 // unsupported operator
	spqopt::ExmiUnsupportedPred,				 // unsupported predicate
	spqopt::ExmiUnsupportedCompositePartKey,	 // composite partitioning keys
	spqopt::ExmiUnsupportedNonDeterministicUpdate,  // non deterministic update
	spqopt::ExmiNoPlanFound,
	spqopt::ExmiUnsupportedOp,
	spqopt::ExmiUnexpectedOp,
	spqopt::ExmiUnsatisfiedRequiredProperties,
	spqopt::ExmiEvalUnsupportedScalarExpr,
	spqopt::ExmiCTEProducerConsumerMisAligned,
	spqopt::ExmiNoStats};

// array of DXL minor exception types that trigger expected fallback to the planner
// refer naucrates/exception.cpp for explanation of errors
const ULONG expected_dxl_fallback[] = {
	spqdxl::ExmiMDObjUnsupported,  // unsupported metadata object
	spqdxl::
		ExmiQuery2DXLUnsupportedFeature,  // unsupported feature during algebrization
	spqdxl::
		ExmiPlStmt2DXLConversion,  // unsupported feature during plan freezing
	spqdxl::
		ExmiDXL2PlStmtConversion,  // unsupported feature during planned statement translation
	spqdxl::ExmiDXL2ExprAttributeNotFound,
	spqdxl::ExmiOptimizerError,
	spqdxl::ExmiDXLMissingAttribute,
	spqdxl::ExmiDXLUnrecognizedOperator,
	spqdxl::ExmiDXLUnrecognizedCompOperator,
	spqdxl::ExmiDXLIncorrectNumberOfChildren,
	spqdxl::ExmiQuery2DXLMissingValue,
	spqdxl::ExmiQuery2DXLDuplicateRTE,
	spqdxl::ExmiMDCacheEntryNotFound,
	spqdxl::ExmiQuery2DXLError,
	spqdxl::ExmiInvalidComparisonTypeCode};

// array of DXL minor exception types that error out and NOT fallback to the planner
const ULONG expected_dxl_errors[] = {
	spqdxl::ExmiDXL2PlStmtExternalScanError,	 // external table error
	spqdxl::ExmiQuery2DXLNotNullViolation,	 // not null violation
};

BOOL
ShouldErrorOut(spqos::CException &exc)
{
	return spqdxl::ExmaDXL == exc.Major() &&
		   FoundException(exc, expected_dxl_errors,
						  SPQOS_ARRAY_SIZE(expected_dxl_errors));
}

spqos::BOOL
FoundException(spqos::CException &exc, const spqos::ULONG *exceptions,
			   spqos::ULONG size)
{
	SPQOS_ASSERT(NULL != exceptions);

	spqos::ULONG minor = exc.Minor();
	spqos::BOOL found = false;
	for (spqos::ULONG ul = 0; !found && ul < size; ul++)
	{
		found = (exceptions[ul] == minor);
	}

	return found;
}

spqos::BOOL
IsLoggableFailure(spqos::CException &exc)
{
	spqos::ULONG major = exc.Major();

	spqos::BOOL is_opt_failure_expected =
		spqopt::ExmaSPQOPT == major &&
		FoundException(exc, expected_opt_fallback,
					   SPQOS_ARRAY_SIZE(expected_opt_fallback));

	spqos::BOOL is_dxl_failure_expected =
		(spqdxl::ExmaDXL == major || spqdxl::ExmaMD == major) &&
		FoundException(exc, expected_dxl_fallback,
					   SPQOS_ARRAY_SIZE(expected_dxl_fallback));

	return (!is_opt_failure_expected && !is_dxl_failure_expected);
}


//---------------------------------------------------------------------------
//	@function:
//		spqos_init
//
//	@doc:
//		Initialize SPQOS memory pool, worker pool and message repository
//
//---------------------------------------------------------------------------
void
spqos_init(struct spqos_init_params *params)
{
	CWorker::abort_requested_by_system = params->abort_requested;

	if (SPQOS_OK != spqos::CMemoryPoolManager::Init())
	{
		return;
	}

	if (SPQOS_OK != spqos::CWorkerPoolManager::Init())
	{
		CMemoryPoolManager::GetMemoryPoolMgr()->Shutdown();
		return;
	}

	if (SPQOS_OK != spqos::CMessageRepository::Init())
	{
		CWorkerPoolManager::WorkerPoolManager()->Shutdown();
		CMemoryPoolManager::GetMemoryPoolMgr()->Shutdown();
		return;
	}

	if (SPQOS_OK != spqos::CCacheFactory::Init())
	{
		return;
	}

#ifdef SPQOS_FPSIMULATOR
	if (SPQOS_OK != spqos::CFSimulator::Init())
	{
		CMessageRepository::GetMessageRepository()->Shutdown();
		CWorkerPoolManager::WorkerPoolManager()->Shutdown();
		CMemoryPoolManager::GetMemoryPoolMgr()->Shutdown();
	}
#endif	// SPQOS_FPSIMULATOR

#ifdef SPQOS_DEBUG_COUNTERS
	CDebugCounter::Init();
#endif
}

//---------------------------------------------------------------------------
//	@function:
//		spqos_exec
//
//	@doc:
//		Execute function as a SPQOS task using current thread;
//		return 0 for successful completion, 1 for error;
//
//---------------------------------------------------------------------------
int
spqos_exec(spqos_exec_params *params)
{
	// check if passed parameters are valid
	if (NULL == params || NULL == params->func)
	{
		return 1;
	}

	try
	{
		CWorkerPoolManager *pwpm = CWorkerPoolManager::WorkerPoolManager();

		// check if worker pool is initialized
		if (NULL == pwpm)
		{
			return 1;
		}

		// if no stack start address is passed, use address in current stack frame
		void *pvStackStart = params->stack_start;
		if (NULL == pvStackStart)
		{
			pvStackStart = &pwpm;
		}

		// put worker to stack - main thread has id '0'
		CWorker wrkr(SPQOS_WORKER_STACK_SIZE, (ULONG_PTR) pvStackStart);

		// scope for memory pool
		{
			// setup task memory
			CAutoMemoryPool amp(CAutoMemoryPool::ElcStrict);
			CMemoryPool *mp = amp.Pmp();

			// scope for ATP
			{
				// task handler for this process
				CAutoTaskProxy atp(mp, pwpm, true /*fPropagateError*/);

				CTask *ptsk = atp.Create(params->func, params->arg,
										 params->abort_requested);

				// init TLS
				ptsk->GetTls().Reset(mp);

				CAutoP<CWStringStatic> apwstr;
				CAutoP<COstreamString> aposs;
				CAutoP<CLoggerStream> aplogger;

				// use passed buffer for logging
				if (NULL != params->error_buffer)
				{
					SPQOS_ASSERT(0 < params->error_buffer_size);

					apwstr = SPQOS_NEW(mp) CWStringStatic(
						(WCHAR *) params->error_buffer,
						params->error_buffer_size / SPQOS_SIZEOF(WCHAR));
					aposs = SPQOS_NEW(mp) COstreamString(apwstr.Value());
					aplogger = SPQOS_NEW(mp) CLoggerStream(*aposs.Value());

					CTaskContext *ptskctxt = ptsk->GetTaskCtxt();
					ptskctxt->SetLogOut(aplogger.Value());
					ptskctxt->SetLogErr(aplogger.Value());
				}

				// execute function
				atp.Execute(ptsk);

				// export task result
				params->result = ptsk->GetRes();

				// check for errors during execution
				if (CTask::EtsError == ptsk->GetStatus())
				{
					return 1;
				}
			}
		}
	}
	catch (CException ex)
	{
		throw ex;
	}
	catch (...)
	{
		// unexpected failure
		SPQOS_RAISE(CException::ExmaUnhandled, CException::ExmiUnhandled);
	}

	return 0;
}


//---------------------------------------------------------------------------
//	@function:
//		spqos_terminate
//
//	@doc:
//		Shutdown SPQOS memory pool, worker pool and message repository
//
//---------------------------------------------------------------------------
void
spqos_terminate()
{
#ifdef SPQOS_DEBUG_COUNTERS
	CDebugCounter::Shutdown();
#endif
#ifdef SPQOS_FPSIMULATOR
	CFSimulator::FSim()->Shutdown();
#endif	// SPQOS_FPSIMULATOR
    CMessageRepository::GetMessageRepository()->Shutdown();
    CWorkerPoolManager::WorkerPoolManager()->Shutdown();
    CCacheFactory::GetFactory()->Shutdown();
    CMemoryPoolManager::GetMemoryPoolMgr()->Shutdown();
}

// EOF
