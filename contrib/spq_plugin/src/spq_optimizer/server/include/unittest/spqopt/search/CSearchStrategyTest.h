//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CSearchStrategyTest.h
//
//	@doc:
//		Test for search strategy
//---------------------------------------------------------------------------
#ifndef SPQOPT_CSearchStrategyTest_H
#define SPQOPT_CSearchStrategyTest_H

#include "spqos/base.h"

#include "spqopt/operators/CExpression.h"
#include "spqopt/search/CSearchStage.h"

namespace spqopt
{
//---------------------------------------------------------------------------
//	@class:
//		CSearchStrategyTest
//
//	@doc:
//		unittest for search strategy
//
//---------------------------------------------------------------------------
class CSearchStrategyTest
{
private:
	// type definition for of expression generator
	typedef CExpression *(*Pfpexpr)(CMemoryPool *);

	// type definition for of optimize function
	typedef void (*PfnOptimize)(CMemoryPool *, CExpression *,
								CSearchStageArray *);

	// generate random search strategy
	static CSearchStageArray *PdrspqssRandom(CMemoryPool *mp);

	// run optimize function on given expression
	static void Optimize(CMemoryPool *mp, Pfpexpr pfnGenerator,
						 CSearchStageArray *search_stage_array,
						 PfnOptimize pfnOptimize);

	static void BuildMemo(CMemoryPool *mp, CExpression *pexprInput,
						  CSearchStageArray *search_stage_array);

public:
	// unittests driver
	static SPQOS_RESULT EresUnittest();

#ifdef SPQOS_DEBUG
	// test search strategy with recursive optimization
	static SPQOS_RESULT EresUnittest_RecursiveOptimize();
#endif	// SPQOS_DEBUG

	// test search strategy with multi-threaded optimization
	static SPQOS_RESULT EresUnittest_MultiThreadedOptimize();

	// test reading search strategy from XML file
	static SPQOS_RESULT EresUnittest_Parsing();

	// test search strategy that times out
	static SPQOS_RESULT EresUnittest_Timeout();

	// test exception handling when parsing search strategy
	static SPQOS_RESULT EresUnittest_ParsingWithException();

};	// CSearchStrategyTest

}  // namespace spqopt

#endif	// !SPQOPT_CSearchStrategyTest_H


// EOF
