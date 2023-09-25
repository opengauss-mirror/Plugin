//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CTreeMapTest.h
//
//	@doc:
//		Test for tree map implementation
//---------------------------------------------------------------------------
#ifndef SPQOPT_CTreeMapTest_H
#define SPQOPT_CTreeMapTest_H

#include "spqos/base.h"

#include "spqopt/search/CTreeMap.h"

namespace spqopt
{
//---------------------------------------------------------------------------
//	@class:
//		CTreeMapTest
//
//	@doc:
//		unittest for tree map
//
//---------------------------------------------------------------------------
class CTreeMapTest
{
	// fwd declaration
	class CNode;
	typedef CDynamicPtrArray<CNode, CleanupRelease<CNode> > CNodeArray;

	// struct for resulting trees
	class CNode : public CRefCount
	{
	private:
		// element number
		ULONG m_ulData;

		// children
		CNodeArray *m_pdrspqnd;

		// private copy ctor
		CNode(const CNode &);

	public:
		// ctor
		CNode(CMemoryPool *mp, ULONG *pulData, CNodeArray *pdrspqnd);

		// dtor
		~CNode();

		// debug print
		IOstream &OsPrintWithIndent(IOstream &os, ULONG ulIndent = 0) const;
	};


private:
	// counter used to mark last successful test
	static ULONG m_ulTestCounter;

	// factory function for result object
	static CNode *Pnd(CMemoryPool *mp, ULONG *pul, CNodeArray *pdrspqnd,
					  BOOL *fTestTrue);

	// shorthand for tests
	typedef CTreeMap<ULONG, CNode, BOOL, HashValue<ULONG>, Equals<ULONG> >
		TestMap;

	// helper to generate loaded the tree map
	static TestMap *PtmapLoad(CMemoryPool *mp);

public:
	// unittests
	static SPQOS_RESULT EresUnittest();
	static SPQOS_RESULT EresUnittest_Basic();
	static SPQOS_RESULT EresUnittest_Count();
	static SPQOS_RESULT EresUnittest_Unrank();
	static SPQOS_RESULT EresUnittest_Memo();

#ifndef SPQOS_DEBUG
	// this test is run in optimized build because of long optimization time
	static SPQOS_RESULT EresUnittest_FailedPlanEnumerationTests();
#endif	// SPQOS_DEBUG



#ifdef SPQOS_DEBUG
	static SPQOS_RESULT EresUnittest_Cycle();
#endif	// SPQOS_DEBUG

};	// CTreeMapTest

}  // namespace spqopt

#endif	// !SPQOPT_CTreeMapTest_H


// EOF
