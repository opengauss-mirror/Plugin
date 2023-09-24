//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2008 Greenplum, Inc.
//
//	@filename:
//		CSyncHashtableTest.h
//
//	@doc:
//      Test for CSyncHashtable
//---------------------------------------------------------------------------
#ifndef SPQOS_CSyncHashtableTest_H
#define SPQOS_CSyncHashtableTest_H

#include "spqos/base.h"
#include "spqos/common/CList.h"
#include "spqos/common/CSyncHashtable.h"
#include "spqos/common/CSyncHashtableAccessByKey.h"
#include "spqos/common/CSyncHashtableIter.h"



namespace spqos
{
//---------------------------------------------------------------------------
//	@class:
//		CSyncHashtableTest
//
//	@doc:
//		Wrapper class to avoid compiler confusion about template parameters
//		Contains also unittests for accessor class
//
//---------------------------------------------------------------------------
class CSyncHashtableTest
{
	// prototypes
	struct SElem;

private:
	// types used by testing functions
	typedef CSyncHashtable<SElem, ULONG> SElemHashtable;

	typedef CSyncHashtableAccessByKey<SElem, ULONG> SElemHashtableAccessor;

	typedef CSyncHashtableIter<SElem, ULONG> SElemHashtableIter;

	typedef CSyncHashtableAccessByIter<SElem, ULONG> SElemHashtableIterAccessor;

	// function pointer to a hash table task
	typedef void *(*pfuncHashtableTask)(void *);

	//---------------------------------------------------------------------------
	//	@class:
	//		SElem
	//
	//	@doc:
	//		Local class for hashtable tests;
	//
	//---------------------------------------------------------------------------
	struct SElem
	{
	private:
		// element's unique id
		ULONG m_id;

	public:
		// generic link
		SLink m_link;

		// hash key
		ULONG m_ulKey;

		// invalid key
		static const ULONG m_ulInvalid;

		// invalid element
		static const SElem m_elemInvalid;

		// simple hash function
		static ULONG
		HashValue(const ULONG &ul)
		{
			return ul;
		}

		static ULONG
		HashValue(const SElem &elem)
		{
			return elem.m_id;
		}

		// equality for object-based comparison
		BOOL
		operator==(const SElem &elem) const
		{
			return elem.m_id == m_id;
		}

		// key equality function for hashtable
		static BOOL
		FEqualKeys(const ULONG &ulkey, const ULONG &ulkeyOther)
		{
			return ulkey == ulkeyOther;
		}

		// element equality function for hashtable
		static BOOL
		Equals(const SElem &elem, const SElem &elemOther)
		{
			return elem == elemOther;
		}

		// ctor
		SElem(ULONG id, ULONG ulKey) : m_id(id), m_ulKey(ulKey)
		{
		}

		// copy ctor
		SElem(const SElem &elem)
		{
			m_id = elem.m_id;
			m_ulKey = elem.m_ulKey;
		}

#ifdef SPQOS_DEBUG
		static BOOL
		IsValid(const ULONG &ulKey)
		{
			return !FEqualKeys(m_ulInvalid, ulKey);
		}
#endif	// SPQOS_DEBUG

		// dummy ctor
		SElem()
		{
		}

		// Id accessor
		ULONG
		Id() const
		{
			return m_id;
		}

	};	// struct SElem

public:
	// actual unittests
	static SPQOS_RESULT EresUnittest();
	static SPQOS_RESULT EresUnittest_Basics();
	static SPQOS_RESULT EresUnittest_Accessor();
	static SPQOS_RESULT EresUnittest_ComplexEquality();
	static SPQOS_RESULT EresUnittest_SameKeyIteration();
	static SPQOS_RESULT EresUnittest_NonConcurrentIteration();
};
}  // namespace spqos

#endif	// !SPQOS_CSyncHashtableTest_H

// EOF
