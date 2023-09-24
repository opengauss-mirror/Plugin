//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		CXformTest.h
//
//	@doc:
//		Test for CXForm
//---------------------------------------------------------------------------
#ifndef SPQOPT_CXformTest_H
#define SPQOPT_CXformTest_H

#include "spqos/base.h"

#include "spqopt/mdcache/CMDAccessor.h"
#include "spqopt/operators/CExpression.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CXformTest
//
//	@doc:
//		Unittests
//
//---------------------------------------------------------------------------
class CXformTest
{
private:
	// path to metadata test file
	static const CHAR *m_szMDFilePath;

	// accessor to metadata cache
	static CMDAccessor *m_pmda;

	// generate a random join tree
	static CExpression *PexprJoinTree(CMemoryPool *mp);

	// generate random star join tree
	static CExpression *PexprStarJoinTree(CMemoryPool *mp, ULONG ulTabs);

	// application of different xforms for the given expression
	static void ApplyExprXforms(CMemoryPool *mp, IOstream &os,
								CExpression *pexpr);

public:
	// test driver
	static SPQOS_RESULT EresUnittest();

	// test application of different xforms
	static SPQOS_RESULT EresUnittest_ApplyXforms();

	// test application of cte-related xforms
	static SPQOS_RESULT EresUnittest_ApplyXforms_CTE();

#ifdef SPQOS_DEBUG
	// test name -> xform mapping
	static SPQOS_RESULT EresUnittest_Mapping();
#endif	// SPQOS_DEBUG

};	// class CXformTest
}  // namespace spqopt

#endif	// !SPQOPT_CXformTest_H

// EOF
