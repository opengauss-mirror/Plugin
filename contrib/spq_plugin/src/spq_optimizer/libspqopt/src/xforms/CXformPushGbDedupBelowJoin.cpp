//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 Pivotal, Inc.
//
//	@filename:
//		CXformPushGbDedupBelowJoin.cpp
//
//	@doc:
//		Implementation of pushing dedup group by below join transform
//---------------------------------------------------------------------------

#include "spqopt/xforms/CXformPushGbDedupBelowJoin.h"

#include "spqos/base.h"

#include "spqopt/operators/CLogicalGbAggDeduplicate.h"
#include "spqopt/operators/CLogicalInnerJoin.h"
#include "spqopt/operators/CPatternLeaf.h"
#include "spqopt/xforms/CXformUtils.h"


using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CXformPushGbDedupBelowJoin::CXformPushGbDedupBelowJoin
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CXformPushGbDedupBelowJoin::CXformPushGbDedupBelowJoin(CMemoryPool *mp)
	:  // pattern
	  CXformPushGbBelowJoin(SPQOS_NEW(mp) CExpression(
		  mp, SPQOS_NEW(mp) CLogicalGbAggDeduplicate(mp),
		  SPQOS_NEW(mp) CExpression(
			  mp, SPQOS_NEW(mp) CLogicalInnerJoin(mp),
			  SPQOS_NEW(mp) CExpression(
				  mp, SPQOS_NEW(mp) CPatternLeaf(mp)),  // join outer child
			  SPQOS_NEW(mp) CExpression(
				  mp, SPQOS_NEW(mp) CPatternLeaf(mp)),  // join inner child
			  SPQOS_NEW(mp) CExpression(
				  mp, SPQOS_NEW(mp) CPatternTree(mp))  // join predicate
			  ),
		  SPQOS_NEW(mp) CExpression(
			  mp, SPQOS_NEW(mp) CPatternTree(mp))  // scalar project list
		  ))
{
}

// EOF
