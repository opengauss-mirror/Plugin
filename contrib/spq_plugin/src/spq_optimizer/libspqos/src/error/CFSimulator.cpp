//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2008 Greenplum, Inc.
//
//	@filename:
//		CFSimulator.cpp
//
//	@doc:
//		Failpoint simulator; maintains a hashtable of bitvectors which encode
//		stacks. Stack walker determines computes a hash value for call stack
//		when checking for a failpoint: if stack has been seen before, skip,
//		otherwise raise exception.
//---------------------------------------------------------------------------

#include "spqos/error/CFSimulator.h"

#include "spqos/base.h"

#ifdef SPQOS_FPSIMULATOR

#include "spqos/common/CAutoP.h"
#include "spqos/memory/CAutoMemoryPool.h"
#include "spqos/memory/CMemoryPoolManager.h"
#include "spqos/task/CAutoTraceFlag.h"

using namespace spqos;

// global instance of simulator
CFSimulator *CFSimulator::m_fsim = NULL;


// invalid stack key
const CFSimulator::CStackTracker::SStackKey
	CFSimulator::CStackTracker::m_invalid_key(CException::ExmaInvalid,
											  CException::ExmiInvalid);

//---------------------------------------------------------------------------
//	@function:
//		CFSimulator::AddTracker
//
//	@doc:
//		Attempt inserting a new tracker for a given exception; we can get
//		overtaken while allocating a tracker before inserting it; in this case
//		back out and delete tracker;
//
//---------------------------------------------------------------------------
void
CFSimulator::AddTracker(CStackTracker::SStackKey key)
{
	// disable OOM simulation in this scope
	CAutoTraceFlag atf(EtraceSimulateOOM, false);

	// allocate new tracker
	CStackTracker *new_stack_tracker =
		SPQOS_NEW(m_mp) CStackTracker(m_mp, m_resolution, key);

	// assume somebody overtook
	BOOL overtaken = true;

	// scope for accessor
	{
		CStackTableAccessor acc(m_stack, key);
		CStackTracker *stack_tracker = acc.Find();

		if (NULL == stack_tracker)
		{
			overtaken = false;
			acc.Insert(new_stack_tracker);
		}

		// must have tracker now
		SPQOS_ASSERT(NULL != acc.Find());
	}

	// clean up as necessary
	if (overtaken)
	{
		SPQOS_DELETE(new_stack_tracker);
	}
}


//---------------------------------------------------------------------------
//	@function:
//		CFSimulator::NewStack
//
//	@doc:
//		Determine if stack is unknown so far and, if so, add to repository
//
//---------------------------------------------------------------------------
BOOL
CFSimulator::NewStack(ULONG major, ULONG minor)
{
	// hash stack
	CStackDescriptor stack_desc;
	stack_desc.BackTrace();
	ULONG hash = stack_desc.HashValue();

	CStackTracker::SStackKey key(major, minor);

	// attempt direct lookup; if we don't have a tracker yet, we
	// need to retry exactly once
	for (ULONG i = 0; i < 2; i++)
	{
		// scope for hashtable access
		{
			CStackTableAccessor acc(m_stack, key);
			CStackTracker *stack_tracker = acc.Find();

			// always true once a tracker has been initialized
			if (NULL != stack_tracker)
			{
				return false == stack_tracker->ExchangeSet(hash % m_resolution);
			}
		}

		// very first time we call in here: fall through and add new tracker
		this->AddTracker(key);
	}

	SPQOS_ASSERT(!"Unexpected exit from loop");
	return false;
}


//---------------------------------------------------------------------------
//	@function:
//		CFSimulator::Init
//
//	@doc:
//		Initialize global instance
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CFSimulator::Init()
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	CFSimulator::m_fsim = SPQOS_NEW(mp) CFSimulator(mp, SPQOS_FSIM_RESOLUTION);

	// detach safety
	(void) amp.Detach();

	return SPQOS_OK;
}


#ifdef SPQOS_DEBUG
//---------------------------------------------------------------------------
//	@function:
//		CFSimulator::Shutdown
//
//	@doc:
//		Destroy singleton instance
//
//---------------------------------------------------------------------------
void
CFSimulator::Shutdown()
{
	CMemoryPool *mp = m_mp;
	SPQOS_DELETE(CFSimulator::m_fsim);
	CFSimulator::m_fsim = NULL;

	CMemoryPoolManager::GetMemoryPoolMgr()->Destroy(mp);
}
#endif	// SPQOS_DEBUG


//---------------------------------------------------------------------------
//	@function:
//		CFSimulator::CStackTracker::CStackTracker
//
//	@doc:
//		ctor
//
//---------------------------------------------------------------------------
CFSimulator::CStackTracker::CStackTracker(CMemoryPool *mp, ULONG resolution,
										  SStackKey key)
	: m_key(key), m_bit_vector(NULL)
{
	// allocate bit vector
	m_bit_vector = SPQOS_NEW(mp) CBitVector(mp, resolution);
}


//---------------------------------------------------------------------------
//	@function:
//		CFSimulator::CStackTracker::ExchangeSet
//
//	@doc:
//		Test and set a given bit in the bit vector
//
//---------------------------------------------------------------------------
BOOL
CFSimulator::CStackTracker::ExchangeSet(ULONG bit)
{
	return m_bit_vector->ExchangeSet(bit);
}


//---------------------------------------------------------------------------
//	@function:
//		CFSimulator::CFSimulator
//
//	@doc:
//		ctor
//
//---------------------------------------------------------------------------
CFSimulator::CFSimulator(CMemoryPool *mp, ULONG resolution)
	: m_mp(mp), m_resolution(resolution)
{
	// setup init table
	m_stack.Init(
		m_mp, 1024, SPQOS_OFFSET(CStackTracker, m_link),
		SPQOS_OFFSET(CStackTracker, m_key), &(CStackTracker::m_invalid_key),
		CStackTracker::SStackKey::HashValue, CStackTracker::SStackKey::Equals);
}

#endif	// SPQOS_FPSIMULATOR

// EOF
