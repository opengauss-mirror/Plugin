//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2008-2011 Greenplum, Inc.
//
//	@filename:
//		CSchedulerContext.h
//
//	@doc:
//		Container for objects associated with scheduling context of a job
//---------------------------------------------------------------------------
#ifndef SPQOPT_CSchedulerContext_H
#define SPQOPT_CSchedulerContext_H

#include "spqos/base.h"

#define SPQOPT_SCHED_CTXT_MEM_POOL_SIZE (64 * 1024 * 1024)

namespace spqopt
{
using namespace spqos;

// prototypes
class CJobFactory;
class CScheduler;
class CEngine;

//---------------------------------------------------------------------------
//	@class:
//		CSchedulerContext
//
//	@doc:
//		Scheduling context
//
//---------------------------------------------------------------------------
class CSchedulerContext
{
private:
	// memory pool used by all workers
	CMemoryPool *m_pmpGlobal;

	// memory pool used by only by current worker (scratch space)
	CMemoryPool *m_pmpLocal;

	// job factory
	CJobFactory *m_pjf;

	// scheduler
	CScheduler *m_psched;

	// optimization engine
	CEngine *m_peng;

	// flag indicating if context has been initialized
	BOOL m_fInit;

	BOOL
	FInit() const
	{
		return m_fInit;
	}

	// no copy ctor
	CSchedulerContext(const CSchedulerContext &);

public:
	// ctor
	CSchedulerContext();

	// dtor
	~CSchedulerContext();

	// initialization
	void Init(CMemoryPool *pmpGlobal, CJobFactory *pjf, CScheduler *psched,
			  CEngine *peng);

	// global memory pool accessor
	CMemoryPool *
	GetGlobalMemoryPool() const
	{
		SPQOS_ASSERT(FInit() && "Scheduling context is not initialized");
		return m_pmpGlobal;
	}

	// local memory pool accessor
	CMemoryPool *
	PmpLocal() const
	{
		SPQOS_ASSERT(FInit() && "Scheduling context is not initialized");
		return m_pmpLocal;
	}

	// job factory accessor
	CJobFactory *
	Pjf() const
	{
		SPQOS_ASSERT(FInit() && "Scheduling context is not initialized");
		return m_pjf;
	}

	// scheduler accessor
	CScheduler *
	Psched() const
	{
		SPQOS_ASSERT(FInit() && "Scheduling context is not initialized");
		return m_psched;
	}

	// engine accessor
	CEngine *
	Peng() const
	{
		SPQOS_ASSERT(FInit() && "Scheduling context is not initialized");
		return m_peng;
	}

};	// class CSchedulerContext
}  // namespace spqopt

#endif	// !SPQOPT_CSchedulerContext_H


// EOF
