//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2008-2010 Greenplum Inc.
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CMemoryPool.h
//
//	@doc:
//		Abstraction of memory pool management. Memory pool types are derived
//		from this class as drop-in replacements. This acts as an abstract class,
//		concrete memory pools such as CMemoryPoolTracker and CMemoryPoolPalloc are
//		derived from this.
//		Some things to note:
//		1. When allocating memory, we have the mp pointer that we are allocating into.
//			However, when deleting memory, we no longer have that pointer. How we free
//			this memory is dependent on the memory pool used, which we get from the
//			memory pool manager.
//		2. When deleting an array, we need to iterate through each element of the array.
//			To calculate this, we calculate the length by calling UserSizeOfAlloc(). This
//			is only done for allocations of type EatArray and thus we do not store the
//			allocation length for non-array allocations.
//
//---------------------------------------------------------------------------
#ifndef SPQOS_CMemoryPool_H
#define SPQOS_CMemoryPool_H

#include "spqos/assert.h"
#include "spqos/common/CLink.h"
#include "spqos/common/CStackDescriptor.h"
#include "spqos/error/CException.h"
#include "spqos/types.h"


// 8-byte alignment
#define SPQOS_MEM_ARCH (8)

#define SPQOS_MEM_ALIGNED_SIZE(x) \
	(SPQOS_MEM_ARCH * (((x) / SPQOS_MEM_ARCH) + ((x) % SPQOS_MEM_ARCH ? 1 : 0)))

// padded size for a given data structure
#define SPQOS_MEM_ALIGNED_STRUCT_SIZE(x) SPQOS_MEM_ALIGNED_SIZE(SPQOS_SIZEOF(x))

// sanity check: char and ulong always fits into the basic unit of alignment
SPQOS_CPL_ASSERT(SPQOS_MEM_ALIGNED_STRUCT_SIZE(spqos::CHAR) == SPQOS_MEM_ARCH);
SPQOS_CPL_ASSERT(SPQOS_MEM_ALIGNED_STRUCT_SIZE(spqos::ULONG) == SPQOS_MEM_ARCH);

// static pattern to init memory
#define SPQOS_MEM_INIT_PATTERN_CHAR (0xCC)
// pattern used to mark deallocated memory, this must match
// SPQOS_WIPED_MEM_PATTERN defined in CRefCount.h
#define SPQOS_MEM_FREED_PATTERN_CHAR (0xCD)

// max allocation per request: 1GB
#define SPQOS_MEM_ALLOC_MAX (0x40000000)

#define SPQOS_MEM_OFFSET_POS(p, ullOffset) ((void *) ((BYTE *) (p) + ullOffset))


namespace spqos
{
// prototypes
class IMemoryVisitor;
//---------------------------------------------------------------------------
//	@class:
//		CMemoryPool
//
//	@doc:
//		Interface class for memory pool
//
//---------------------------------------------------------------------------
class CMemoryPool
{
	// manager accesses internal functionality
	friend class CMemoryPoolManager;

private:
	// hash key is only set by pool manager
	ULONG_PTR m_hash_key;

#ifdef SPQOS_DEBUG
	// stack where pool is created
	CStackDescriptor m_stack_desc;
#endif	// SPQOS_DEBUG

	// link structure to manage pools
	SLink m_link;

protected:
	// invalid memory pool key
	static const ULONG_PTR m_invalid;

public:
	enum EAllocationType
	{
		EatUnknown = 0x00,
		EatSingleton = 0x7f,
		EatArray = 0x7e
	};

	// dtor
	virtual ~CMemoryPool()
	{
	}

	// prepare the memory pool to be deleted
	virtual void TearDown() = 0;

	// hash key accessor
	virtual ULONG_PTR
	GetHashKey() const
	{
		return m_hash_key;
	}

	// implementation of placement new with memory pool
	virtual void *NewImpl(const ULONG bytes, const CHAR *file, const ULONG line,
						  CMemoryPool::EAllocationType eat) = 0;

	// implementation of array-new with memory pool
	template <typename T>
	T *
	NewArrayImpl(SIZE_T num_elements, const CHAR *filename, ULONG line)
	{
		T *array = static_cast<T *>(
			NewImpl(sizeof(T) * num_elements, filename, line, EatArray));
		for (SIZE_T idx = 0; idx < num_elements; ++idx)
		{
			try
			{
				new (array + idx) T();
			}
			catch (...)
			{
				// If any element's constructor throws, deconstruct
				// previous objects and reclaim memory before rethrowing.
				for (SIZE_T destroy_idx = idx - 1; destroy_idx < idx;
					 --destroy_idx)
				{
					array[destroy_idx].~T();
				}
				DeleteImpl(array, EatArray);
				throw;
			}
		}
		return array;
	}

	// return total allocated size
	virtual ULLONG
	TotalAllocatedSize() const
	{
		SPQOS_ASSERT(!"not supported");
		return 0;
	}

	// requested size of allocation
	static ULONG UserSizeOfAlloc(const void *ptr);

	// free allocation
	static void DeleteImpl(void *ptr, EAllocationType eat);

    // requested size of allocation
    static ULONG UserDXLSizeOfAlloc(const void *ptr);

    // free allocation
    static void DeleteDXLImpl(void *ptr, EAllocationType eat);

#ifdef SPQOS_DEBUG

	// check if the memory pool keeps track of live objects
	virtual BOOL
	SupportsLiveObjectWalk() const
	{
		return false;
	}

	// walk the live objects, calling pVisitor.visit() for each one
	virtual void
	WalkLiveObjects(IMemoryVisitor *)
	{
		SPQOS_ASSERT(!"not supported");
	}

	// dump memory pool to given stream
	virtual IOstream &OsPrint(IOstream &os);

	// check if a memory pool is empty
	virtual void AssertEmpty(IOstream &os);

#endif	// SPQOS_DEBUG

};	// class CMemoryPool
	// Overloading placement variant of singleton new operator. Used to allocate
	// arbitrary objects from an CMemoryPool. This does not affect the ordinary
	// built-in 'new', and is used only when placement-new is invoked with the
	// specific type signature defined below.

#ifdef SPQOS_DEBUG

//---------------------------------------------------------------------------
//	@function:
//		CMemoryPool::operator <<
//
//	@doc:
//		Print function for memory pools
//
//---------------------------------------------------------------------------
inline IOstream &
operator<<(IOstream &os, CMemoryPool &mp)
{
	return mp.OsPrint(os);
}
#endif	// SPQOS_DEBUG

namespace delete_detail
{
// All-static helper class. Base version deletes unqualified pointers / arrays.
template <typename T>
class CDeleter
{
public:
	static void
	Delete(T *object)
	{
		if (NULL == object)
		{
			return;
		}

		// Invoke destructor, then free memory.
		object->~T();
		CMemoryPool::DeleteImpl(object, CMemoryPool::EatSingleton);
	}

	static void
	DeleteArray(T *object_array)
	{
		if (NULL == object_array)
		{
			return;
		}

		// Invoke destructor on each array element in reverse
		// order from construction.
		const SIZE_T num_elements =
			CMemoryPool::UserSizeOfAlloc(object_array) / sizeof(T);
		for (SIZE_T idx = num_elements - 1; idx < num_elements; --idx)
		{
			object_array[idx].~T();
		}

		// Free memory.
		CMemoryPool::DeleteImpl(object_array, CMemoryPool::EatArray);
	}

    static void
        DeleteDXLArray(T *object_array)
    {
        if (NULL == object_array)
        {
            return;
        }

        // Invoke destructor on each array element in reverse
        // order from construction.
        const SIZE_T num_elements =
            CMemoryPool::UserDXLSizeOfAlloc(object_array) / sizeof(T);
        for (SIZE_T idx = num_elements - 1; idx < num_elements; --idx)
        {
            object_array[idx].~T();
        }

        // Free memory.
        CMemoryPool::DeleteDXLImpl(object_array, CMemoryPool::EatArray);
    }
};

// Specialization for const-qualified types.
template <typename T>
class CDeleter<const T>
{
public:
	static void
	Delete(const T *object)
	{
		CDeleter<T>::Delete(const_cast<T *>(object));
	}

	static void
	DeleteArray(const T *object_array)
	{
		CDeleter<T>::DeleteArray(const_cast<T *>(object_array));
	}

    static void
        DeleteDXLArray(const T *object_array)
    {
        CDeleter<T>::DeleteDXLArray(const_cast<T *>(object_array));
    }
};
}  // namespace delete_detail
}  // namespace spqos

// Overloading placement variant of singleton new operator. Used to allocate
// arbitrary objects from an CMemoryPool. This does not affect the ordinary
// built-in 'new', and is used only when placement-new is invoked with the
// specific type signature defined below.
inline void *
operator new(spqos::SIZE_T size, spqos::CMemoryPool *mp,
			 const spqos::CHAR *filename, spqos::ULONG line)
{
	return mp->NewImpl(size, filename, line, spqos::CMemoryPool::EatSingleton);
}

// Corresponding placement variant of delete operator. Note that, for delete
// statements in general, the compiler can not determine which overloaded
// version of new was used to allocate memory originally, and the global
// non-placement version is used. This placement version of 'delete' is used
// *only* when a constructor throws an exception, and the version of 'new' is
// known to be the one declared above.
inline void
operator delete(void *ptr, spqos::CMemoryPool *, const spqos::CHAR *, spqos::ULONG)
{
	// Reclaim memory after constructor throws exception.
	spqos::CMemoryPool::DeleteImpl(ptr, spqos::CMemoryPool::EatSingleton);
}

// Placement new-style macro to do 'new' with a memory pool. Anything allocated
// with this *must* be deleted by SPQOS_DELETE, *not* the ordinary delete
// operator.
#define SPQOS_NEW(mp) new (mp, __FILE__, __LINE__)

// Replacement for array-new. Conceptually equivalent to
// 'new(mp) datatype[count]'. Any arrays allocated with this *must* be deleted
// by SPQOS_DELETE_ARRAY, *not* the ordinary delete[] operator.
//
// NOTE: Unlike singleton new, we do not overload the built-in new operator for
// arrays, because when we do so the C++ compiler adds its own book-keeping
// information to the allocation in a non-portable way such that we can not
// recover SPQOS' own book-keeping information reliably.
#define SPQOS_NEW_ARRAY(mp, datatype, count) \
	mp->NewArrayImpl<datatype>(count, __FILE__, __LINE__)

// Delete a singleton object allocated by SPQOS_NEW().
template <typename T>
void
SPQOS_DELETE(T *object)
{
	::spqos::delete_detail::CDeleter<T>::Delete(object);
}

// Delete an array allocated by SPQOS_NEW_ARRAY().
template <typename T>
void
SPQOS_DELETE_ARRAY(T *object_array)
{
	::spqos::delete_detail::CDeleter<T>::DeleteArray(object_array);
}

// Delete an array allocated by SPQOS_NEW_DXL_ARRAY().
template <typename T>
void
    SPQOS_DELETE_DXL_ARRAY(T *object_array)
{
    ::spqos::delete_detail::CDeleter<T>::DeleteDXLArray(object_array);
}
#endif	// !SPQOS_CMemoryPool_H

// EOF
