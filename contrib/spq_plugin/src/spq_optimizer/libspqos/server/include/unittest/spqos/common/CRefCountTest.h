//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2008 Greenplum, Inc.
//
//	@filename:
//		CRefCount.h
//
//	@doc:
//      Test for CRefCount
//---------------------------------------------------------------------------
#ifndef SPQOS_CRefCountTest_H
#define SPQOS_CRefCountTest_H

#include "spqos/base.h"
#include "spqos/common/CRefCount.h"

namespace spqos
{
//---------------------------------------------------------------------------
//	@class:
//		CRefCountTest
//
//	@doc:
//		Static unit tests
//
//---------------------------------------------------------------------------
class CRefCountTest
{
private:
	//---------------------------------------------------------------------------
	//	@class:
	//		CDeletableTest
	//
	//	@doc:
	//		Local class for testing deletable/undeletable objects
	//
	//---------------------------------------------------------------------------
	class CDeletableTest : public CRefCount
	{
	private:
		// is calling object's destructor allowed?
		BOOL m_fDeletable;

	public:
		// ctor
		CDeletableTest()
			: m_fDeletable(false)  // by default, object is not deletable
		{
		}

		// return true if calling object's destructor is allowed
		virtual BOOL
		Deletable() const
		{
			return m_fDeletable;
		}

		// allow calling object's destructor
		void
		AllowDeletion()
		{
			m_fDeletable = true;
		}

	};	// class CDeletableTest


public:
	// unittests
	static SPQOS_RESULT EresUnittest();
	static SPQOS_RESULT EresUnittest_CountUpAndDown();
	static SPQOS_RESULT EresUnittest_DeletableObjects();

#ifdef SPQOS_DEBUG
	static SPQOS_RESULT EresUnittest_Stack();
	static SPQOS_RESULT EresUnittest_Check();
#endif	// SPQOS_DEBUG
};
}  // namespace spqos

#endif	// !SPQOS_CRefCountTest_H

// EOF
