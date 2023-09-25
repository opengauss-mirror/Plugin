//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2019 Pivotal, Inc.
//
//	@filename:
//		CMemoryPoolPalloc.cpp
//
//	@doc:
//		CMemoryPool implementation that uses PostgreSQL memory
//		contexts.
//
//---------------------------------------------------------------------------

#include "postgres.h"

#include "utils/memutils.h"

#include "spqos/memory/CMemoryPool.h"

#include "spq_optimizer_util/spq_wrappers.h"
#include "spq_optimizer_util/utils/CMemoryPoolPalloc.h"

using namespace spqos;

// ctor
CMemoryPoolPalloc::CMemoryPoolPalloc() : m_cxt(NULL)
{
	m_cxt = spqdb::SPQDBAllocSetContextCreate();
}

void *
CMemoryPoolPalloc::NewImpl(const ULONG bytes, const CHAR *, const ULONG,
						   CMemoryPool::EAllocationType eat)
{
	// if it's a singleton allocation, allocate requested memory
	if (CMemoryPool::EatSingleton == eat)
	{
		return spqdb::SPQDBMemoryContextAlloc(m_cxt, bytes);
	}
	// if it's an array allocation, allocate header + requested memory
	else
	{
		ULONG alloc_size = SPQOS_MEM_ALIGNED_STRUCT_SIZE(SArrayAllocHeader) +
						   SPQOS_MEM_ALIGNED_SIZE(bytes);

		void *ptr = spqdb::SPQDBMemoryContextAlloc(m_cxt, alloc_size);

		if (NULL == ptr)
		{
			return NULL;
		}

		SArrayAllocHeader *header = static_cast<SArrayAllocHeader *>(ptr);

		header->m_user_size = bytes;
		return static_cast<BYTE *>(ptr) +
			   SPQOS_MEM_ALIGNED_STRUCT_SIZE(SArrayAllocHeader);
	}
}

void
CMemoryPoolPalloc::DeleteImpl(void *ptr, CMemoryPool::EAllocationType eat)
{
	if (CMemoryPool::EatSingleton == eat)
	{
		spqdb::SPQDBFree(ptr);
	}
	else
	{
		void *header = static_cast<BYTE *>(ptr) -
					   SPQOS_MEM_ALIGNED_STRUCT_SIZE(SArrayAllocHeader);
		spqdb::SPQDBFree(header);
	}
}

// Prepare the memory pool to be deleted
void
CMemoryPoolPalloc::TearDown()
{
	spqdb::SPQDBMemoryContextDelete(m_cxt);
}

// Total allocated size including management overheads
ULLONG
CMemoryPoolPalloc::TotalAllocatedSize() const
{
	//return MemoryContextGetCurrentSpace(m_cxt);
	return 0;
}

// get user requested size of array allocation. Note: this is ONLY called for arrays
ULONG
CMemoryPoolPalloc::UserSizeOfAlloc(const void *ptr)
{
	SPQOS_ASSERT(ptr != NULL);
	void *void_header = static_cast<BYTE *>(const_cast<void *>(ptr)) -
						SPQOS_MEM_ALIGNED_STRUCT_SIZE(SArrayAllocHeader);
	const SArrayAllocHeader *header =
		static_cast<SArrayAllocHeader *>(void_header);
	return header->m_user_size;
}


// EOF
