//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2008 - 2010 Greenplum, Inc.
//
//	@filename:
//		CErrorHandlerStandardStandard.cpp
//
//	@doc:
//		Implements standard error handler
//---------------------------------------------------------------------------

#include "spqos/error/CErrorHandlerStandard.h"

#include "spqos/base.h"
#include "spqos/error/CErrorContext.h"
#include "spqos/error/CLogger.h"
#include "spqos/io/ioutils.h"
#include "spqos/string/CWStringStatic.h"
#include "spqos/task/CAutoSuspendAbort.h"
#include "spqos/task/CTask.h"

using namespace spqos;

//---------------------------------------------------------------------------
//	@function:
//		CErrorHandlerStandard::Process
//
//	@doc:
//		Process pending error context;
//
//---------------------------------------------------------------------------
void
CErrorHandlerStandard::Process(CException exception)
{
	CTask *task = CTask::Self();

	SPQOS_ASSERT(NULL != task && "No task in current context");

	IErrorContext *err_ctxt = task->GetErrCtxt();
	CLogger *log = dynamic_cast<CLogger *>(task->GetErrorLogger());

	SPQOS_ASSERT(err_ctxt->IsPending() && "No error to process");
	SPQOS_ASSERT(err_ctxt->GetException() == exception &&
				"Exception processed different from pending");

	// print error stack trace
	if (CException::ExmaSystem == exception.Major() && !err_ctxt->IsRethrown())
	{
		if ((CException::ExmiIOError == exception.Minor() ||
			 CException::ExmiNetError == exception.Minor()) &&
			0 < errno)
		{
			err_ctxt->AppendErrnoMsg();
		}

		if (ILogger::EeilMsgHeaderStack <= log->InfoLevel())
		{
			err_ctxt->AppendStackTrace();
		}
	}

	// scope for suspending cancellation
	{
		// suspend cancellation
		CAutoSuspendAbort asa;

		// log error message
		log->Log(err_ctxt->GetErrorMsg(), err_ctxt->GetSeverity(), __FILE__,
				 __LINE__);
	}
}

// EOF
