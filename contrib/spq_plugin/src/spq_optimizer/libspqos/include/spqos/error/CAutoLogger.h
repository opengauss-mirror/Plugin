//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CAutoLogger.h
//
//	@doc:
//		Auto object for replacing the logger for output/error.
//---------------------------------------------------------------------------
#ifndef SPQOS_CAutoLogger_H
#define SPQOS_CAutoLogger_H

#include "spqos/base.h"
#include "spqos/task/CTaskContext.h"

namespace spqos
{
//---------------------------------------------------------------------------
//	@class:
//		CAutoLogger
//
//	@doc:
//		Auto object for replacing the logger for outpur/error.
//
//---------------------------------------------------------------------------
class CAutoLogger : public CStackObject
{
private:
	// old logger
	ILogger *m_old_logger;

	// flag indicating if logger is used for error logging
	BOOL m_error;

	// private copy ctor
	CAutoLogger(const CAutoLogger &);

public:
	// ctor
	CAutoLogger(ILogger *logger, BOOL error)
		: m_old_logger(NULL), m_error(error)
	{
		SPQOS_ASSERT(NULL != logger);

		ITask *task = ITask::Self();
		SPQOS_ASSERT(NULL != task);

		if (m_error)
		{
			m_old_logger = task->GetErrorLogger();
			task->GetTaskCtxt()->SetLogErr(logger);
		}
		else
		{
			m_old_logger = task->GetOutputLogger();
			task->GetTaskCtxt()->SetLogOut(logger);
		}
	}

	// dtor
	~CAutoLogger()
	{
		ITask *task = ITask::Self();
		SPQOS_ASSERT(NULL != task);

		if (m_error)
		{
			task->GetTaskCtxt()->SetLogErr(m_old_logger);
		}
		else
		{
			task->GetTaskCtxt()->SetLogOut(m_old_logger);
		}
	}

};	// class CAutoLogger
}  // namespace spqos

#endif	// !SPQOS_CAutoLogger_H

// EOF
