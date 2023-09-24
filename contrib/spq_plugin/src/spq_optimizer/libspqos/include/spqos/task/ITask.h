//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2008 - 2010 Greenplum, Inc.
//
//	@filename:
//		ITask.h
//
//	@doc:
//		Interface class for task abstraction
//---------------------------------------------------------------------------
#ifndef SPQOS_ITask_H
#define SPQOS_ITask_H

#include "spqos/task/IWorker.h"
#include "spqos/task/traceflags.h"
#include "spqos/types.h"

// trace flag macro definitions
#define SPQOS_FTRACE(x) ITask::Self()->IsTraceSet(x)
#define SPQOS_SET_TRACE(x) (void) ITask::Self()->SetTrace(x, true /*value*/)
#define SPQOS_UNSET_TRACE(x) (void) ITask::Self()->SetTrace(x, false /*value*/)

namespace spqos
{
// forward declarations
class ILogger;
class CMemoryPool;
class CTaskContext;
class CTaskLocalStorage;
class IErrorContext;


class ITask
{
private:
	// private copy ctor
	ITask(const ITask &);

public:
	// task status
	enum ETaskStatus
	{
		EtsInit,	   // task initialized but not scheduled
		EtsQueued,	   // task added to scheduler's queue
		EtsDequeued,   // task removed from scheduler's queue, ready to run
		EtsRunning,	   // task currently executing
		EtsCompleted,  // task completed executing with no error
		EtsError	   // exception encountered while task was executed
	};

	// ctor
	ITask()
	{
	}

	// dtor
	virtual ~ITask()
	{
	}

	// accessor for memory pool, e.g. used for allocating task parameters in
	virtual CMemoryPool *Pmp() const = 0;

	// TLS
	virtual CTaskLocalStorage &GetTls() = 0;

	// task context accessor
	virtual CTaskContext *GetTaskCtxt() const = 0;

	// basic output streams
	virtual ILogger *GetOutputLogger() const = 0;
	virtual ILogger *GetErrorLogger() const = 0;

	// manipulate traceflags
	virtual BOOL SetTrace(ULONG, BOOL) = 0;
	virtual BOOL IsTraceSet(ULONG) = 0;

	// current locale
	virtual ELocale Locale() const = 0;

	// error context
	virtual IErrorContext *GetErrCtxt() const = 0;

	// any pending exceptions?
	virtual BOOL HasPendingExceptions() const = 0;

	static ITask *Self();

};	// class ITask
}  // namespace spqos

#endif	// !SPQOS_ITask_H

// EOF
