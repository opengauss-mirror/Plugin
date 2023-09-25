//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 Greenplum, Inc.
//
//	@filename:
//		CMDAccessorTest.h
//
//	@doc:
//		Tests accessing objects from the metadata cache.
//---------------------------------------------------------------------------


#ifndef SPQOPT_CMDAccessorTest_H
#define SPQOPT_CMDAccessorTest_H

#include "spqos/base.h"

#include "spqopt/mdcache/CMDAccessor.h"
#include "spqopt/operators/CExpression.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CMDAccessorTest
//
//	@doc:
//		Static unit tests
//
//---------------------------------------------------------------------------

class CMDAccessorTest
{
private:
	// a task for looking up a single object from the MD cache
	static void *PvLookupSingleObj(void *pv);

	// a task for looking up multiple objects from the MD cache
	static void *PvLookupMultipleObj(void *pv);

	// task that creates a MD accessor and starts multiple threads which
	// lookup MD objects through that accessor
	static void *PvInitMDAAndLookup(void *pv);

	// cache task function pointer
	typedef void *(*TaskFuncPtr)(void *);

	// structure for passing parameters to task functions
	struct SMDCacheTaskParams
	{
		// memory pool
		CMemoryPool *m_mp;

		// MD accessor
		CMDAccessor *m_pmda;

		SMDCacheTaskParams(CMemoryPool *mp, CMDAccessor *md_accessor)
			: m_mp(mp), m_pmda(md_accessor)
		{
		}
	};

public:
	// unittests
	static SPQOS_RESULT EresUnittest();
	static SPQOS_RESULT EresUnittest_Basic();
	static SPQOS_RESULT EresUnittest_Datum();
	static SPQOS_RESULT EresUnittest_DatumGeneric();
	static SPQOS_RESULT EresUnittest_Navigate();
	static SPQOS_RESULT EresUnittest_Negative();
	static SPQOS_RESULT EresUnittest_Indexes();
	static SPQOS_RESULT EresUnittest_CheckConstraint();
	static SPQOS_RESULT EresUnittest_IndexPartConstraint();
	static SPQOS_RESULT EresUnittest_Cast();
	static SPQOS_RESULT EresUnittest_ScCmp();
	static SPQOS_RESULT EresUnittest_PrematureMDIdRelease();

};	// class CMDAccessorTest
}  // namespace spqopt

#endif	// !SPQOPT_CMDAccessorTest_H

// EOF
