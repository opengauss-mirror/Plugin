//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2008 Greenplum, Inc.
//
//	@filename:
//		CFSimulator.h
//
//	@doc:
//		Failpoint simulator framework; computes a hash value for current
//		call stack; if stack has not been seen before, stack repository
//		returns true which makes the call macro simulate a failure, i.e.
//		throw an exception.
//---------------------------------------------------------------------------
#ifndef SPQOS_CFSimulator_H
#define SPQOS_CFSimulator_H

#include "spqos/types.h"

#if SPQOS_FPSIMULATOR

#include "spqos/common/CBitVector.h"
#include "spqos/common/CList.h"
#include "spqos/common/CStackDescriptor.h"
#include "spqos/common/CSyncHashtable.h"
#include "spqos/common/CSyncHashtableAccessByKey.h"

// macro to trigger failure simulation; must be macro to get accurate
// file/line information
#define SPQOS_SIMULATE_FAILURE(trace, major, minor)       \
	do                                                   \
	{                                                    \
		if (ITask::Self()->IsTraceSet(trace) &&          \
			CFSimulator::FSim()->NewStack(major, minor)) \
		{                                                \
			SPQOS_RAISE(major, minor);                    \
		}                                                \
	} while (0)

// resolution of hash vector
#define SPQOS_FSIM_RESOLUTION 10000


namespace spqos
{
//---------------------------------------------------------------------------
//	@class:
//		CFSimulator
//
//	@doc:
//		Failpoint simulator; maintains hashtable of stack hashes
//
//---------------------------------------------------------------------------
class CFSimulator
{
private:
	//---------------------------------------------------------------------------
	//	@class:
	//		CStackTracker
	//
	//	@doc:
	//		Tracks all stacks for a given exception, i.e. contains one single
	//		bitvector;
	//
	//---------------------------------------------------------------------------
	class CStackTracker
	{
	public:
		//---------------------------------------------------------------------------
		//	@struct:
		//		StackKey
		//
		//	@doc:
		//		Wrapper around the two parts of an exception identification; provides
		//		equality operator for hashtable
		//
		//---------------------------------------------------------------------------
		struct SStackKey
		{
			// stack trackers are identified by the exceptions they manage
			ULONG m_major;
			ULONG m_minor;

			// ctor
			SStackKey(ULONG major, ULONG minor) : m_major(major), m_minor(minor)
			{
			}

			// simple comparison
			BOOL
			operator==(const SStackKey &key) const
			{
				return m_major == key.m_major && m_minor == key.m_minor;
			}

			// equality function -- needed for hashtable
			static BOOL
			Equals(const SStackKey &key, const SStackKey &other_key)
			{
				return key == other_key;
			}

			// basic hash function
			static ULONG
			HashValue(const SStackKey &key)
			{
				return key.m_major ^ key.m_minor;
			}

		};	// struct StackKey


		// ctor
		explicit CStackTracker(CMemoryPool *mp, ULONG resolution,
							   SStackKey key);

		// exchange/set function
		BOOL ExchangeSet(ULONG bit);

		// link element for hashtable
		SLink m_link;

		// identifier
		SStackKey m_key;

		// invalid key
		static const SStackKey m_invalid_key;

	private:
		// no copy ctor
		CStackTracker(const CStackTracker &);

		// bitvector to hold stack hashes
		CBitVector *m_bit_vector;

	};	// class CStackTracker



	// hidden copy ctor
	CFSimulator(const CFSimulator &);

	// memory pool
	CMemoryPool *m_mp;

	// resolution
	ULONG m_resolution;

	// short hands for stack repository and accessor
	typedef CSyncHashtable<CStackTracker, CStackTracker::SStackKey> CStackTable;

	typedef CSyncHashtableAccessByKey<CStackTracker, CStackTracker::SStackKey>
		CStackTableAccessor;

	// stack repository
	CStackTable m_stack;

	// insert new tracker
	void AddTracker(CStackTracker::SStackKey key);

public:
	// ctor
	CFSimulator(CMemoryPool *mp, ULONG resolution);

	// dtor
	~CFSimulator()
	{
	}

	// determine if stack is new
	BOOL NewStack(ULONG major, ULONG minor);

	// global instance
	static CFSimulator *m_fsim;

	// initializer for global f-simulator
	static SPQOS_RESULT Init();

#ifdef SPQOS_DEBUG
	// destroy simulator
	void Shutdown();
#endif	// SPQOS_DEBUG

	// accessor for global instance
	static CFSimulator *
	FSim()
	{
		return m_fsim;
	}

	// check if simulation is activated
	static BOOL
	FSimulation()
	{
		ITask *task = ITask::Self();
		return task->IsTraceSet(EtraceSimulateOOM) ||
			   task->IsTraceSet(EtraceSimulateAbort) ||
			   task->IsTraceSet(EtraceSimulateIOError) ||
			   task->IsTraceSet(EtraceSimulateNetError);
	}

};	// class CFSimulator
}  // namespace spqos

#else

#define SPQOS_SIMULATE_FAILURE(x, y) ;

#endif	// !SPQOS_FPSIMULATOR

#endif	// !SPQOS_CFSimulator_H

// EOF
