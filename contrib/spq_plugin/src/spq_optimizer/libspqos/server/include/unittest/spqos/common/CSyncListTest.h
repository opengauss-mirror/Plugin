//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CSyncListTest.h
//
//	@doc:
//		Tests for CSyncList
//---------------------------------------------------------------------------
#ifndef SPQOS_CSyncListTest_H
#define SPQOS_CSyncListTest_H

#include "spqos/common/CSyncList.h"
#include "spqos/common/CSyncPool.h"
#include "spqos/task/CAutoTaskProxy.h"
#include "spqos/types.h"

namespace spqos
{
//---------------------------------------------------------------------------
//	@class:
//		CSyncListTest
//
//	@doc:
//		Wrapper class for CSyncList template to avoid compiler confusion
//		regarding instantiation with sample parameters;
//
//---------------------------------------------------------------------------
class CSyncListTest
{
private:
	// list element;
	struct SElem
	{
		// object id
		ULONG m_id;

		// generic link for list
		SLink m_link;

		// ctor
		SElem() : m_id(0)
		{
		}
	};

	// collection of parameters for parallel tasks
	struct SArg
	{
		// pointer to list
		CSyncList<SElem> *m_plist;

		// pool of elements to insert
		CSyncPool<SElem> *m_psp;

		// number of tasks
		ULONG m_ulCount;

		// ctor
		SArg(CSyncList<SElem> *pstack, CSyncPool<SElem> *psp, ULONG count)
			: m_plist(pstack), m_psp(psp), m_ulCount(count)
		{
		}

		// ctor
		SArg() : m_plist(NULL), m_psp(NULL), m_ulCount(0)
		{
		}
	};

public:
	// unittests
	static SPQOS_RESULT EresUnittest();
	static SPQOS_RESULT EresUnittest_Basics();

};	// class CSyncListTest
}  // namespace spqos


#endif	// !SPQOS_CSyncListTest_H

// EOF
