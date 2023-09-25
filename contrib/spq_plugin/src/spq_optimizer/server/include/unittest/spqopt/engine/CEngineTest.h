//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 - 2011 EMC Corp.
//
//	@filename:
//		CEngineTest.h
//
//	@doc:
//		Test for CEngine
//---------------------------------------------------------------------------
#ifndef SPQOPT_CEngineTest_H
#define SPQOPT_CEngineTest_H


#include "spqos/base.h"
#include "spqos/common/CDynamicPtrArray.h"

#include "spqopt/base/COptimizationContext.h"
#include "spqopt/operators/CExpression.h"
#include "spqopt/search/CSearchStage.h"

namespace spqopt
{
//---------------------------------------------------------------------------
//	@class:
//		CEngineTest
//
//	@doc:
//		Unittests
//
//---------------------------------------------------------------------------
class CEngineTest
{
private:
#ifdef SPQOS_DEBUG

	// type definition for of expression generator
	typedef CExpression *(*Pfpexpr)(CMemoryPool *);

	// helper for testing engine using an array of expression generators
	static SPQOS_RESULT EresTestEngine(Pfpexpr rspqf[], ULONG size);

#endif	// SPQOS_DEBUG

	// counter used to mark last successful test
	static ULONG m_ulTestCounter;

	// counter used to mark last successful test in subquery test
	static ULONG m_ulTestCounterSubq;

public:
	// type definition of optimizer test function
	typedef void(FnOptimize)(CMemoryPool *, CExpression *, CSearchStageArray *);

	// main driver
	static SPQOS_RESULT EresUnittest();

	// basic unittest
	static SPQOS_RESULT EresUnittest_Basic();

	// helper function for optimizing deep join trees
	static SPQOS_RESULT EresOptimize(
		FnOptimize *pfopt,	 // optimization function
		CWStringConst *str,	 // array of relation names
		ULONG *pul,			 // array of relation OIDs
		ULONG ulRels,		 // number of array entries
		CBitSet *
			pbs	 // if a bit is set, the corresponding join expression will be optimized
	);

#ifdef SPQOS_DEBUG

	// build memo by recursive optimization
	static void BuildMemoRecursive(CMemoryPool *mp, CExpression *pexprInput,
								   CSearchStageArray *search_stage_array);

	// test of recursive memo building
	static SPQOS_RESULT EresUnittest_BuildMemo();

	// test of appending stats during optimization
	static SPQOS_RESULT EresUnittest_AppendStats();

	// test of recursive memo building with a large number of joins
	static SPQOS_RESULT EresUnittest_BuildMemoLargeJoins();

	// test of building memo for expressions with subqueries
	static SPQOS_RESULT EresUnittest_BuildMemoWithSubqueries();

	// test of building memo for expressions with TVFs
	static SPQOS_RESULT EresUnittest_BuildMemoWithTVF();

	// test of building memo for expressions with grouping
	static SPQOS_RESULT EresUnittest_BuildMemoWithGrouping();

	// test of building memo for expressions with partitioning
	static SPQOS_RESULT EresUnittest_BuildMemoWithPartitioning();

	// test of building memo for expressions with partitioning
	static SPQOS_RESULT EresUnittest_BuildMemoWithWindowing();

	// test of building memo for expressions with CTEs
	static SPQOS_RESULT EresUnittest_BuildMemoWithCTE();

#endif	// SPQOS_DEBUG

};	// class CEngineTest
}  // namespace spqopt

#endif	// !SPQOPT_CEngineTest_H


// EOF
