//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009-2010 Greenplum Inc.
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CMemoryPoolTracker.h
//
//	@doc:
//		Memory pool that allocates from malloc() and adds on
//		statistics and debugging
//
//	@owner:
//
//	@test:
//
//---------------------------------------------------------------------------
#ifndef SPQOS_CMemoryPoolTracker_H
#define SPQOS_CMemoryPoolTracker_H

#include "spqos/assert.h"
#include "spqos/common/CList.h"
#include "spqos/common/CStackDescriptor.h"
#include "spqos/memory/CMemoryPool.h"
#include "spqos/memory/CMemoryPoolStatistics.h"
#include "spqos/types.h"
#include "spqos/utils.h"

namespace spqos
{
// memory pool with statistics and debugging support
class CMemoryPoolTracker : public CMemoryPool
{
private:
	// Defines memory block header layout for all allocations;
	// does not include the pointer to the pool;
	struct SAllocHeader
	{
		// pointer to pool
		CMemoryPoolTracker *m_mp;

		// total allocation size (including headers)
		ULONG m_alloc_size;

		// user requested size
		ULONG m_user_size;

		// sequence number
		ULLONG m_serial;

		// file name
		const CHAR *m_filename;

		// line in file
		ULONG m_line;

#ifdef SPQOS_DEBUG
		// allocation stack
		CStackDescriptor m_stack_desc;
#endif	// SPQOS_DEBUG

		// link for allocation list
		SLink m_link;
	};

	// statistics
	CMemoryPoolStatistics m_memory_pool_statistics;

	// allocation sequence number
	ULONG m_alloc_sequence;

	// list of allocated (live) objects
	CList<SAllocHeader> m_allocations_list;

	// private copy ctor
	CMemoryPoolTracker(CMemoryPoolTracker &);

	// record a successful allocation
	void RecordAllocation(SAllocHeader *header);

	// record a successful free
	void RecordFree(SAllocHeader *header);

protected:
	// dtor
	virtual ~CMemoryPoolTracker();

public:
	// ctor
	CMemoryPoolTracker();

	// prepare the memory pool to be deleted
	virtual void TearDown();

	// allocate memory
	void *NewImpl(const ULONG bytes, const CHAR *file, const ULONG line,
				  CMemoryPool::EAllocationType eat);

	// free memory allocation
	static void DeleteImpl(void *ptr, EAllocationType eat);

	// get user requested size of allocation
	static ULONG UserSizeOfAlloc(const void *ptr);

	// return total allocated size
	virtual ULLONG
	TotalAllocatedSize() const
	{
		return m_memory_pool_statistics.TotalAllocatedSize();
	}

#ifdef SPQOS_DEBUG

	// check if the memory pool keeps track of live objects
	virtual BOOL
	SupportsLiveObjectWalk() const
	{
		return true;
	}

	// walk the live objects
	virtual void WalkLiveObjects(spqos::IMemoryVisitor *visitor);

#endif	// SPQOS_DEBUG
};
}  // namespace spqos

#endif	// !SPQOS_CMemoryPoolTracker_H

// EOF
