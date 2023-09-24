//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (c) 2004-2015 Pivotal Software, Inc.
//
//	@filename:
//		CWorkerPoolManager.cpp
//
//	@doc:
//		Central scheduler;
//		* maintains global worker-local-storage
//		* keeps track of all worker pools
//---------------------------------------------------------------------------

#include "knl/knl_session.h"

#include "spqos/memory/CMemoryPoolManager.h"

#include "spqos/base.h"
#include "spqos/common/CAutoP.h"
#include "spqos/common/clibwrapper.h"
#include "spqos/error/CAutoTrace.h"
#include "spqos/error/CFSimulator.h"	 // for SPQOS_FPSIMULATOR
#include "spqos/memory/CMemoryPool.h"
#include "spqos/memory/CMemoryPoolTracker.h"
#include "spqos/memory/CMemoryVisitorPrint.h"
#include "spqos/task/CAutoSuspendAbort.h"
#include "spqos/task/CAutoTraceFlag.h"


using namespace spqos;
using namespace spqos::clib;

// ctor
CMemoryPoolManager::CMemoryPoolManager(CMemoryPool *internal,
									   EMemoryPoolType memory_pool_type)
	: m_internal_memory_pool(internal),
	  m_allow_global_new(true),
	  m_ht_all_pools(NULL),
	  m_memory_pool_type(memory_pool_type)
{
	SPQOS_ASSERT(NULL != internal);
	SPQOS_ASSERT(SPQOS_OFFSET(CMemoryPool, m_link) ==
				SPQOS_OFFSET(CMemoryPoolTracker, m_link));
}

// Set up CMemoryPoolManager's internals.
// This must be done here instead of the constructor because CMemoryPoolManager
// uses virtual methods for determining the correct CMemoryPool for
// allocations; and these virtual methods must not be called in the
// constructor, or else it will use the base class allocator instead of the one
// defined in derived-class virtual function.
void
CMemoryPoolManager::Setup()
{
	m_ht_all_pools = SPQOS_NEW(m_internal_memory_pool)
		CSyncHashtable<CMemoryPool, ULONG_PTR>();
	m_ht_all_pools->Init(
		m_internal_memory_pool, SPQOS_MEMORY_POOL_HT_SIZE,
		SPQOS_OFFSET(CMemoryPool, m_link), SPQOS_OFFSET(CMemoryPool, m_hash_key),
		&(CMemoryPool::m_invalid), HashULongPtr, EqualULongPtr);

	// create pool used in allocations made using global new operator
	m_global_memory_pool = CreateMemoryPool();
}

// Initialize global memory pool manager using CMemoryPoolTracker
SPQOS_RESULT
CMemoryPoolManager::Init()
{
	if (NULL == u_sess->spq_cxt.m_memory_pool_mgr)
	{
		return SetupGlobalMemoryPoolManager<CMemoryPoolManager,
											CMemoryPoolTracker>();
	}

	return SPQOS_OK;
}

CMemoryPoolManager *
CMemoryPoolManager::GetMemoryPoolMgr()
{
	return u_sess->spq_cxt.m_memory_pool_mgr;
}

CMemoryPoolManager **
CMemoryPoolManager::GetMemoryPoolMgrPtr()
{
	return &(u_sess->spq_cxt.m_memory_pool_mgr);
}

CMemoryPool *
CMemoryPoolManager::CreateMemoryPool()
{
	CMemoryPool *mp = NewMemoryPool();

	// accessor scope
	{
		// HERE BE DRAGONS
		// See comment in CCache::InsertEntry
		const ULONG_PTR hashKey = mp->GetHashKey();
		MemoryPoolKeyAccessor acc(*m_ht_all_pools, hashKey);
		acc.Insert(mp);
	}

	return mp;
}


// Allocate a new NewMemoryPool
CMemoryPool *
CMemoryPoolManager::NewMemoryPool()
{
	return SPQOS_NEW(m_internal_memory_pool) CMemoryPoolTracker();
}


// Release given memory pool
void
CMemoryPoolManager::Destroy(CMemoryPool *mp)
{
	SPQOS_ASSERT(NULL != mp);

	// accessor scope
	{
		// HERE BE DRAGONS
		// See comment in CCache::InsertEntry
		const ULONG_PTR hashKey = mp->GetHashKey();
		MemoryPoolKeyAccessor acc(*m_ht_all_pools, hashKey);
		acc.Remove(mp);
	}

	mp->TearDown();

	SPQOS_DELETE(mp);
}


// Return total allocated size in bytes
ULLONG
CMemoryPoolManager::TotalAllocatedSize()
{
	ULLONG total_size = 0;
	MemoryPoolIter iter(*m_ht_all_pools);
	while (iter.Advance())
	{
		MemoryPoolIterAccessor acc(iter);
		CMemoryPool *mp = acc.Value();
		if (NULL != mp)
		{
			total_size = total_size + mp->TotalAllocatedSize();
		}
	}

	return total_size;
}

// free memory allocation
void
CMemoryPoolManager::DeleteImpl(void *ptr, CMemoryPool::EAllocationType eat)
{
	CMemoryPoolTracker::DeleteImpl(ptr, eat);
}

// get user requested size of allocation
ULONG
CMemoryPoolManager::UserSizeOfAlloc(const void *ptr)
{
	return CMemoryPoolTracker::UserSizeOfAlloc(ptr);
}

#ifdef SPQOS_DEBUG


// Print contents of all allocated memory pools
IOstream &
CMemoryPoolManager::OsPrint(IOstream &os)
{
	os << "Print memory pools: " << std::endl;

	MemoryPoolIter iter(*m_ht_all_pools);
	while (iter.Advance())
	{
		CMemoryPool *mp = NULL;
		{
			MemoryPoolIterAccessor acc(iter);
			mp = acc.Value();
		}

		if (NULL != mp)
		{
			os << *mp << std::endl;
		}
	}

	return os;
}


// Print memory pools with total allocated size above given threshold
void
CMemoryPoolManager::PrintOverSizedPools(
	CMemoryPool *trace,
	ULLONG size_threshold  // size threshold in bytes
)
{
	CAutoTraceFlag Abort(EtraceSimulateAbort, false);
	CAutoTraceFlag OOM(EtraceSimulateOOM, false);
	CAutoTraceFlag Net(EtraceSimulateNetError, false);
	CAutoTraceFlag IO(EtraceSimulateIOError, false);

	MemoryPoolIter iter(*m_ht_all_pools);
	while (iter.Advance())
	{
		MemoryPoolIterAccessor acc(iter);
		CMemoryPool *mp = acc.Value();

		if (NULL != mp)
		{
			ULLONG size = mp->TotalAllocatedSize();
			if (size > size_threshold)
			{
				CAutoTrace at(trace);
				at.Os() << std::endl
						<< "OVERSIZED MEMORY POOL: " << size << " bytes "
						<< std::endl;
			}
		}
	}
}
#endif	// SPQOS_DEBUG


// Destroy a memory pool at shutdown
void
CMemoryPoolManager::DestroyMemoryPoolAtShutdown(CMemoryPool *mp)
{
	SPQOS_ASSERT(NULL != mp);

#ifdef SPQOS_DEBUG
	spqos::oswcerr << "Leaked " << *mp << std::endl;
#endif	// SPQOS_DEBUG

	mp->TearDown();
	SPQOS_DELETE(mp);
}


// Clean-up memory pools
// Counterpart to CMemoryPoolManager::Setup()
void
CMemoryPoolManager::Cleanup()
{
#ifdef SPQOS_DEBUG
	if (0 < m_global_memory_pool->TotalAllocatedSize())
	{
		// allocations made by calling global new operator are not deleted
		spqos::oswcerr << "Memory leaks detected" << std::endl
					  << *m_global_memory_pool << std::endl;
	}
#endif	// SPQOS_DEBUG

	SPQOS_ASSERT(NULL != m_global_memory_pool);
	Destroy(m_global_memory_pool);

	// cleanup left-over memory pools;
	// any such pool means that we have a leak
	m_ht_all_pools->DestroyEntries(DestroyMemoryPoolAtShutdown);
	SPQOS_DELETE(m_ht_all_pools);
}


// Delete memory pools and release manager
void
CMemoryPoolManager::Shutdown()
{
	// cleanup remaining memory pools
	Cleanup();

	// save off pointers for explicit deletion
	CMemoryPool *internal = m_internal_memory_pool;

	::delete u_sess->spq_cxt.m_memory_pool_mgr;
	u_sess->spq_cxt.m_memory_pool_mgr = NULL;

#ifdef SPQOS_DEBUG
	internal->AssertEmpty(oswcerr);
#endif	// SPQOS_DEBUG

	Free(internal);
}

// EOF
