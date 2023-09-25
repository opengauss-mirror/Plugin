//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2008 Greenplum, Inc.
//
//	@filename:
//		CRefCount.h
//
//	@doc:
//		Base class for reference counting in the optimizer;
//		Ref-counting is single-threaded only;
//		Enforces allocation on the heap, i.e. no deletion unless the
//		count has really dropped to zero
//---------------------------------------------------------------------------
#ifndef SPQOS_CRefCount_H
#define SPQOS_CRefCount_H

#include "spqos/assert.h"
#include "spqos/common/CHeapObject.h"
#include "spqos/error/CException.h"
#include "spqos/task/ITask.h"
#include "spqos/types.h"
#include "spqos/utils.h"

// pattern used to mark deallocated memory, this must match
// SPQOS_MEM_FREED_PATTERN_CHAR in CMemoryPool.h
#define SPQOS_WIPED_MEM_PATTERN 0xCdCdCdCdCdCdCdCd

namespace spqos
{
//---------------------------------------------------------------------------
//	@class:
//		CRefCount
//
//	@doc:
//		Basic reference counting
//
//---------------------------------------------------------------------------
class CRefCount : public CHeapObject
{
private:
	// reference counter -- first in class to be in sync with Check()
	ULONG_PTR m_refs;

#ifdef SPQOS_DEBUG
	// sanity check to detect deleted memory
	void
	Check()
	{
		// assert that first member of class has not been wiped
		SPQOS_ASSERT(m_refs != SPQOS_WIPED_MEM_PATTERN);
	}
#endif	// SPQOS_DEBUG

	// private copy ctor
	CRefCount(const CRefCount &);

public:
	// ctor
	CRefCount() : m_refs(1)
	{
	}

	// dtor
	virtual ~CRefCount()
	{
		// enforce strict ref-counting unless we're in a pending exception,
		// e.g., a ctor has thrown
		SPQOS_ASSERT(NULL == ITask::Self() ||
					ITask::Self()->HasPendingExceptions() || 0 == m_refs);
	}

	// return ref-count
	ULONG_PTR
	RefCount() const
	{
		return m_refs;
	}

	// return true if calling object's destructor is allowed
	virtual BOOL
	Deletable() const
	{
		return true;
	}

	// count up
	void
	AddRef()
	{
#ifdef SPQOS_DEBUG
		Check();
#endif	// SPQOS_DEBUG
		m_refs++;
	}

	// count down
	void
	Release()
	{
#ifdef SPQOS_DEBUG
		Check();
#endif	// SPQOS_DEBUG
		m_refs--;

		if (0 == m_refs)
		{
			if (!Deletable())
			{
				// restore ref-count
				AddRef();

				// deletion is not allowed
				SPQOS_RAISE(CException::ExmaSystem,
						   CException::ExmiInvalidDeletion);
			}

			SPQOS_DELETE(this);
		}
	}

	// safe version of Release -- handles NULL pointers
	static void
	SafeRelease(CRefCount *rc)
	{
		if (NULL != rc)
		{
			rc->Release();
		}
	}

	// print function
	virtual IOstream &
	OsPrint(IOstream &os) const
	{
		return os;
	}


};	// class CRefCount
}  // namespace spqos

#endif	// !SPQOS_CRefCount_H

// EOF
