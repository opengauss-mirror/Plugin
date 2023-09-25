//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		CStack.h
//
//	@doc:
//		Create stack of objects
//
//	@owner:
//		Siva
//
//	@test:
//
//
//---------------------------------------------------------------------------
#ifndef SPQOPT_CStack_H
#define SPQOPT_CStack_H

#include "spqos/base.h"
#include "spqos/common/CDynamicPtrArray.h"
#include "spqos/common/CRefCount.h"

namespace spqos
{
template <class T>
class CStack
{
private:
	// backing dynamic array store
	CDynamicPtrArray<T, CleanupNULL> *m_dynamic_ptr_array;

	// top of stack index
	ULONG m_size;

	// copy c'tor - not defined
	CStack(CStack &);

public:
	// c'tor
	CStack<T>(CMemoryPool *mp, ULONG min_size = 4) : m_size(0)
	{
		m_dynamic_ptr_array =
			SPQOS_NEW(mp) CDynamicPtrArray<T, CleanupNULL>(mp, min_size, 10);
	}

	// destructor
	virtual ~CStack()
	{
		m_dynamic_ptr_array->Release();
	}

	// push element onto stack
	void
	Push(T *obj)
	{
		SPQOS_ASSERT(m_dynamic_ptr_array != NULL && "Dynamic array missing");
		SPQOS_ASSERT(m_size <= m_dynamic_ptr_array->Size() &&
					"The top of stack cannot be beyond the underlying array");

		// if the stack was Popped before, reuse that space by replacing the element
		if (m_size < m_dynamic_ptr_array->Size())
		{
			m_dynamic_ptr_array->Replace(m_size, obj);
		}
		else
		{
			m_dynamic_ptr_array->Append(obj);
		}

		m_size++;
	}

	// pop top element
	T *
	Pop()
	{
		SPQOS_ASSERT(!this->IsEmpty() && "Cannot pop from empty stack");

		T *obj = (*m_dynamic_ptr_array)[m_size - 1];
		m_size--;
		return obj;
	}

	// peek at top element
	const T *
	Peek() const
	{
		SPQOS_ASSERT(!this->IsEmpty() && "Cannot peek into empty stack");

		const T *obj = (*m_dynamic_ptr_array)[m_size - 1];

		return obj;
	}

	// is stack empty?
	BOOL
	IsEmpty() const
	{
		return (m_size == 0);
	}
};

}  // namespace spqos

#endif	// !SPQOPT_CStack_H

// EOF
