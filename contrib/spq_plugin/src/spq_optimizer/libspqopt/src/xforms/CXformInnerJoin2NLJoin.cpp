//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		CXformInnerJoin2NLJoin.cpp
//
//	@doc:
//		Implementation of transform
//---------------------------------------------------------------------------

#include "spqopt/xforms/CXformInnerJoin2NLJoin.h"

#include "spqos/base.h"

#include "spqopt/operators/CLogicalInnerJoin.h"
#include "spqopt/operators/CPatternLeaf.h"
#include "spqopt/operators/CPhysicalInnerNLJoin.h"
#include "spqopt/xforms/CXformInnerJoin2HashJoin.h"
#include "spqopt/xforms/CXformUtils.h"

using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CXformInnerJoin2NLJoin::CXformInnerJoin2NLJoin
//
//	@doc:
//		ctor
//
//---------------------------------------------------------------------------
CXformInnerJoin2NLJoin::CXformInnerJoin2NLJoin(CMemoryPool *mp)
	:  // pattern
	  CXformImplementation(SPQOS_NEW(mp) CExpression(
		  mp, SPQOS_NEW(mp) CLogicalInnerJoin(mp),
		  SPQOS_NEW(mp)
			  CExpression(mp, SPQOS_NEW(mp) CPatternLeaf(mp)),  // left child
		  SPQOS_NEW(mp)
			  CExpression(mp, SPQOS_NEW(mp) CPatternLeaf(mp)),  // right child
		  SPQOS_NEW(mp)
			  CExpression(mp, SPQOS_NEW(mp) CPatternLeaf(mp))  // predicate
		  ))
{
}


//---------------------------------------------------------------------------
//	@function:
//		CXformInnerJoin2NLJoin::Exfp
//
//	@doc:
//		Compute xform promise for a given expression handle;
//
//---------------------------------------------------------------------------
CXform::EXformPromise
CXformInnerJoin2NLJoin::Exfp(CExpressionHandle &) const
{
	return CXform::ExfpNone;
}


//---------------------------------------------------------------------------
//	@function:
//		CXformInnerJoin2NLJoin::Transform
//
//	@doc:
//		actual transformation
//		Deprecated in favor of CXformImplementInnerJoin.
//
//---------------------------------------------------------------------------
void
CXformInnerJoin2NLJoin::Transform(CXformContext *, CXformResult *,
								  CExpression *) const
{
}


// EOF
