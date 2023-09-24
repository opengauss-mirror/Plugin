//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2019 Pivotal, Inc.
//
//	@filename:
//		CMemoryPoolPalloc.h
//
//	@doc:
//		CMemoryPool implementation that uses PostgreSQL memory
//		contexts.
//
//---------------------------------------------------------------------------

#ifndef SPQDXL_CMemoryPoolPalloc_H
#define SPQDXL_CMemoryPoolPalloc_H

#include "spqos/base.h"
#include "spqos/memory/CMemoryPool.h"

namespace spqos
{
// Memory pool that maps to a Postgres MemoryContext.
class CMemoryPoolPalloc : public CMemoryPool
{
private:
	MemoryContext m_cxt;

	// When destroying arrays, we need to call the destructor of each element
	// To do this, we need the size of the allocation, which we then divide by the
	// the size of the element to get number of elements to iterate through.
	// This struct is only used for array allocations (SPQOS_NEW_ARRAY())
	struct SArrayAllocHeader
	{
		ULONG m_user_size;
	};

public:
	// ctor
	CMemoryPoolPalloc();

	// allocate memory
	void *NewImpl(const ULONG bytes, const CHAR *file, const ULONG line,
				  CMemoryPool::EAllocationType eat);

	// free memory
	static void DeleteImpl(void *ptr, CMemoryPool::EAllocationType eat);

	// prepare the memory pool to be deleted
	void TearDown();

	// return total allocated size include management overhead
	ULLONG TotalAllocatedSize() const;

	// get user requested size of allocation
	static ULONG UserSizeOfAlloc(const void *ptr);
};
}  // namespace spqos

#endif	// !SPQDXL_CMemoryPoolPalloc_H

// EOF
