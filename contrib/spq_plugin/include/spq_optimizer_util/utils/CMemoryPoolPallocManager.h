//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2019 Pivotal, Inc.
//
//	@filename:
//		CMemoryPoolPallocManager.h
//
//	@doc:
//		MemoryPoolManager implementation that creates
//		CMemoryPoolPalloc memory pools
//
//---------------------------------------------------------------------------

#ifndef SPQDXL_CMemoryPoolPallocManager_H
#define SPQDXL_CMemoryPoolPallocManager_H

#include "spqos/base.h"
#include "spqos/memory/CMemoryPoolManager.h"

namespace spqos
{
// memory pool manager that uses SPQDB memory contexts
class CMemoryPoolPallocManager : public CMemoryPoolManager
{
private:
	// private no copy ctor
	CMemoryPoolPallocManager(const CMemoryPoolPallocManager &);

public:
	// ctor
	CMemoryPoolPallocManager(CMemoryPool *internal,
							 EMemoryPoolType memory_pool_type);

	// allocate new memorypool
	virtual CMemoryPool *NewMemoryPool();

	// free allocation
	void DeleteImpl(void *ptr, CMemoryPool::EAllocationType eat);

	// get user requested size of allocation
	ULONG UserSizeOfAlloc(const void *ptr);


	static SPQOS_RESULT Init();
};
}  // namespace spqos

#endif	// !SPQDXL_CMemoryPoolPallocManager_H

// EOF
