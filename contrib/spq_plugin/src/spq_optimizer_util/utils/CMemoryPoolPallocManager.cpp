//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2019 Pivotal, Inc.
//
//	@filename:
//		CMemoryPoolPallocManager.cpp
//
//	@doc:
//		MemoryPoolManager implementation that creates
//		CMemoryPoolPalloc memory pools
//
//---------------------------------------------------------------------------

#include "postgres.h"

#include "utils/memutils.h"

#include "spq_optimizer_util/utils/CMemoryPoolPalloc.h"
#include "spq_optimizer_util/utils/CMemoryPoolPallocManager.h"

using namespace spqos;

// ctor
CMemoryPoolPallocManager::CMemoryPoolPallocManager(CMemoryPool *internal,
												   EMemoryPoolType)
	: CMemoryPoolManager(internal, EMemoryPoolExternal)
{
}

// create new memory pool
CMemoryPool *
CMemoryPoolPallocManager::NewMemoryPool()
{
	return SPQOS_NEW(GetInternalMemoryPool()) CMemoryPoolPalloc();
}

void
CMemoryPoolPallocManager::DeleteImpl(void *ptr,
									 CMemoryPool::EAllocationType eat)
{
	CMemoryPoolPalloc::DeleteImpl(ptr, eat);
}

// get user requested size of allocation
ULONG
CMemoryPoolPallocManager::UserSizeOfAlloc(const void *ptr)
{
	return CMemoryPoolPalloc::UserSizeOfAlloc(ptr);
}

SPQOS_RESULT
CMemoryPoolPallocManager::Init()
{
	if (u_sess->spq_cxt.m_memory_pool_mgr == nullptr) {
		return CMemoryPoolManager::SetupGlobalMemoryPoolManager<
			CMemoryPoolPallocManager, CMemoryPoolPalloc>();
	}
	return SPQOS_OK;
}

// EOF
