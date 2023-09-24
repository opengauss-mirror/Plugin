//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2010 Greenplum, Inc.
//
//	@filename:
//		ITaskScheduler.h
//
//	@doc:
//		Interface class for task scheduling
//---------------------------------------------------------------------------
#ifndef SPQOS_ITaskScheduler_H
#define SPQOS_ITaskScheduler_H

#include "spqos/types.h"

namespace spqos
{
// prototypes
class CTask;
class CTaskId;

//---------------------------------------------------------------------------
//	@class:
//		ITaskScheduler
//
//	@doc:
//		Interface for abstracting task scheduling primitives.
//
//---------------------------------------------------------------------------

class ITaskScheduler
{
private:
	// private copy ctor
	ITaskScheduler(const ITaskScheduler &);

public:
	// dummy ctor
	ITaskScheduler()
	{
	}

	// dummy dtor
	virtual ~ITaskScheduler()
	{
	}

	// add task to waiting queue
	virtual void Enqueue(CTask *) = 0;

	// get next task to execute
	virtual CTask *Dequeue() = 0;

	// check if task is waiting to be scheduled and remove it
	virtual SPQOS_RESULT Cancel(CTask *task) = 0;

	// get number of waiting tasks
	virtual ULONG GetQueueSize() = 0;

	// check if task queue is empty
	virtual BOOL IsEmpty() const = 0;

};	// class ITaskScheduler
}  // namespace spqos

#endif /* SPQOS_ITaskScheduler_H */

// EOF
