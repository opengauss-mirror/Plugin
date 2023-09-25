//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009-2010 Greenplum Inc.
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CAutoMemoryPool.cpp
//
//	@doc:
//		Implementation for auto memory pool that automatically releases
//  	the attached memory pool and performs leak checking
//
//	@owner:
//
//	@test:
//
//---------------------------------------------------------------------------

#include "spqos/memory/CAutoMemoryPool.h"

#include "spqos/assert.h"
#include "spqos/error/CErrorContext.h"
#include "spqos/error/CErrorHandler.h"
#include "spqos/memory/CMemoryPoolManager.h"
#include "spqos/task/CAutoSuspendAbort.h"
#include "spqos/task/ITask.h"
#include "spqos/types.h"
#include "spqos/utils.h"

using namespace spqos;


//---------------------------------------------------------------------------
//	@function:
//		CAutoMemoryPool::CAutoMemoryPool
//
//	@doc:
//		Create an auto-managed pool; the managed pool is allocated from
//  	the CMemoryPoolManager global instance
//
//---------------------------------------------------------------------------
CAutoMemoryPool::CAutoMemoryPool(ELeakCheck leak_check_type)
	: m_leak_check_type(leak_check_type)
{
	m_mp = CMemoryPoolManager::GetMemoryPoolMgr()->CreateMemoryPool();
}



//---------------------------------------------------------------------------
//	@function:
//		CAutoMemoryPool::Detach
//
//	@doc:
//		Detach function used when CAutoMemoryPool is used to guard a newly
//		created pool until it is safe, e.g., in constructors
//
//---------------------------------------------------------------------------
CMemoryPool *
CAutoMemoryPool::Detach()
{
	CMemoryPool *mp = m_mp;
	m_mp = NULL;

	return mp;
}


//---------------------------------------------------------------------------
//	@function:
//		CAutoMemoryPool::~CAutoMemoryPool
//
//	@doc:
//		Release the pool back to the manager and perform leak checks if
//		(1) strict leak checking indicated, or
//		(2) no checking while pending exception indicated and no pending exception
//
//---------------------------------------------------------------------------
CAutoMemoryPool::~CAutoMemoryPool()
{
	if (NULL == m_mp)
	{
		return;
	}

	// suspend cancellation
	CAutoSuspendAbort asa;

#ifdef SPQOS_DEBUG

	ITask *task = ITask::Self();

	// ElcExc must be used inside tasks only
	SPQOS_ASSERT_IMP(ElcExc == m_leak_check_type, NULL != task);

	SPQOS_TRY
	{
		if (ElcStrict == m_leak_check_type ||
			(ElcExc == m_leak_check_type && !task->GetErrCtxt()->IsPending()))
		{
			spqos::IOstream &os = spqos::oswcerr;

			// check for leaks, use this to trigger standard Assert handling
			m_mp->AssertEmpty(os);
		}

		// release pool
		CMemoryPoolManager::GetMemoryPoolMgr()->Destroy(m_mp);
	}
	SPQOS_CATCH_EX(ex)
	{
		SPQOS_ASSERT(
			SPQOS_MATCH_EX(ex, CException::ExmaSystem, CException::ExmiAssert));

		// release pool
		CMemoryPoolManager::GetMemoryPoolMgr()->Destroy(m_mp);

		SPQOS_RETHROW(ex);
	}
	SPQOS_CATCH_END;

#else  // SPQOS_DEBUG

	// hand in pool and return
	CMemoryPoolManager::GetMemoryPoolMgr()->Destroy(m_mp);

#endif	// SPQOS_DEBUG
}

// EOF
