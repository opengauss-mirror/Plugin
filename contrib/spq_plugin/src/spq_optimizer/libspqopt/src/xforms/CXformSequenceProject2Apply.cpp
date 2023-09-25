//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CXformSequenceProject2Apply.cpp
//
//	@doc:
//		Implementation of Sequence Project to Apply transform
//---------------------------------------------------------------------------

#include "spqopt/xforms/CXformSequenceProject2Apply.h"

#include "spqos/base.h"

#include "spqopt/operators/CLogicalSequenceProject.h"
#include "spqopt/operators/CPatternLeaf.h"

using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CXformSequenceProject2Apply::CXformSequenceProject2Apply
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CXformSequenceProject2Apply::CXformSequenceProject2Apply(CMemoryPool *mp)
	:  // pattern
	  CXformSubqueryUnnest(SPQOS_NEW(mp) CExpression(
		  mp, SPQOS_NEW(mp) CLogicalSequenceProject(mp),
		  SPQOS_NEW(mp) CExpression(
			  mp, SPQOS_NEW(mp) CPatternLeaf(mp)),  // relational child
		  SPQOS_NEW(mp)
			  CExpression(mp, SPQOS_NEW(mp) CPatternTree(mp))  // project list
		  ))
{
}

// EOF
