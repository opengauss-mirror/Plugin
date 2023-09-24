//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009-2010 Greenplum Inc.
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CMemoryPoolTracker.cpp
//
//	@doc:
//		Implementation for memory pool that allocates from Malloc
//		and adds synchronization, statistics, debugging information
//		and memory tracing.
//
//---------------------------------------------------------------------------

#include "spqos/memory/CMemoryPoolTracker.h"

#include "spqos/assert.h"
#include "spqos/common/clibwrapper.h"
#include "spqos/memory/CMemoryPool.h"
#include "spqos/memory/CMemoryPoolManager.h"
#include "spqos/memory/IMemoryVisitor.h"
#include "spqos/types.h"
#include "spqos/utils.h"

using namespace spqos;

#define SPQOS_MEM_GUARD_SIZE (SPQOS_SIZEOF(BYTE))

#define SPQOS_MEM_ALLOC_HEADER_SIZE SPQOS_MEM_ALIGNED_STRUCT_SIZE(SAllocHeader)

#define SPQOS_MEM_BYTES_TOTAL(ulNumBytes) \
	(SPQOS_MEM_ALLOC_HEADER_SIZE +        \
	 SPQOS_MEM_ALIGNED_SIZE((ulNumBytes) + SPQOS_MEM_GUARD_SIZE))


// ctor
CMemoryPoolTracker::CMemoryPoolTracker() : CMemoryPool(), m_alloc_sequence(0)
{
	m_allocations_list.Init(SPQOS_OFFSET(SAllocHeader, m_link));
}


// dtor
CMemoryPoolTracker::~CMemoryPoolTracker()
{
	SPQOS_ASSERT(m_allocations_list.IsEmpty());
}

void
CMemoryPoolTracker::RecordAllocation(SAllocHeader *header)
{
	m_memory_pool_statistics.RecordAllocation(header->m_user_size,
											  header->m_alloc_size);
	m_allocations_list.Prepend(header);
}

void
CMemoryPoolTracker::RecordFree(SAllocHeader *header)
{
	m_memory_pool_statistics.RecordFree(header->m_user_size,
										header->m_alloc_size);
	m_allocations_list.Remove(header);
}


void *
CMemoryPoolTracker::NewImpl(const ULONG bytes, const CHAR *file,
							const ULONG line, CMemoryPool::EAllocationType eat)
{
	SPQOS_ASSERT(bytes <= SPQOS_MEM_ALLOC_MAX);
	SPQOS_ASSERT(bytes <= spqos::ulong_max);
	SPQOS_ASSERT_IMP(
		(NULL != CMemoryPoolManager::GetMemoryPoolMgr()) &&
			(this ==
			 CMemoryPoolManager::GetMemoryPoolMgr()->GetGlobalMemoryPool()),
		CMemoryPoolManager::GetMemoryPoolMgr()->IsGlobalNewAllowed() &&
			"Use of new operator without target memory pool is prohibited, use New(...) instead");

	ULONG alloc_size = SPQOS_MEM_BYTES_TOTAL(bytes);

	void *ptr = clib::Malloc(alloc_size);

	// check if allocation failed
	if (NULL == ptr)
	{
		return NULL;
	}

	SPQOS_OOM_CHECK(ptr);

	// successful allocation: update header information and any memory pool data
	SAllocHeader *header = static_cast<SAllocHeader *>(ptr);
	header->m_serial = m_alloc_sequence;
	++m_alloc_sequence;

	header->m_alloc_size = alloc_size;
	header->m_mp = this;
	header->m_filename = file;
	header->m_line = line;
	header->m_user_size = bytes;

	RecordAllocation(header);

	void *ptr_result = header + 1;

#ifdef SPQOS_DEBUG
	header->m_stack_desc.BackTrace();

	clib::Memset(ptr_result, SPQOS_MEM_INIT_PATTERN_CHAR, bytes);
#endif	// SPQOS_DEBUG

	// add a footer with the allocation type (singleton/array)
	BYTE *alloc_type = reinterpret_cast<BYTE *>(ptr_result) + bytes;
	*alloc_type = eat;

	return ptr_result;
}

// free memory allocation
void
CMemoryPoolTracker::DeleteImpl(void *ptr, EAllocationType eat)
{
	SAllocHeader *header = static_cast<SAllocHeader *>(ptr) - 1;

	ULONG user_size = header->m_user_size;
	BYTE *alloc_type = static_cast<BYTE *>(ptr) + user_size;

	// this assert ensures we aren't writing past allocated memory
	SPQOS_RTL_ASSERT(eat == EatUnknown || *alloc_type == eat);

	// update stats and allocation list
	SPQOS_ASSERT(NULL != header->m_mp);
	header->m_mp->RecordFree(header);

#ifdef SPQOS_DEBUG
	// mark user memory as unused in debug mode
	clib::Memset(ptr, SPQOS_MEM_FREED_PATTERN_CHAR, user_size);
#endif	// SPQOS_DEBUG

	clib::Free(header);
}

// get user requested size of allocation
ULONG
CMemoryPoolTracker::UserSizeOfAlloc(const void *ptr)
{
	const SAllocHeader *header = static_cast<const SAllocHeader *>(ptr) - 1;
	return header->m_user_size;
}


// Prepare the memory pool to be deleted;
// this function is called only once so locking is not required;
void
CMemoryPoolTracker::TearDown()
{
	while (!m_allocations_list.IsEmpty())
	{
		SAllocHeader *header = m_allocations_list.First();
		void *user_data = header + 1;
		DeleteImpl(user_data, EatUnknown);
	}
}


#ifdef SPQOS_DEBUG

void
CMemoryPoolTracker::WalkLiveObjects(spqos::IMemoryVisitor *visitor)
{
	SPQOS_ASSERT(NULL != visitor);

	SAllocHeader *header = m_allocations_list.First();
	while (NULL != header)
	{
		void *user = header + 1;

		visitor->Visit(user, header->m_user_size, header, header->m_alloc_size,
					   header->m_filename, header->m_line, header->m_serial,
#ifdef SPQOS_DEBUG
					   &header->m_stack_desc
#else
					   NULL
#endif	// SPQOS_DEBUG
		);

		header = m_allocations_list.Next(header);
	}
}


#endif	// SPQOS_DEBUG
