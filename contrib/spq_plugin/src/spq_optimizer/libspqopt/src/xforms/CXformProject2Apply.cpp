//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CXformProject2Apply.cpp
//
//	@doc:
//		Implementation of Project to Apply transform
//---------------------------------------------------------------------------

#include "spqopt/xforms/CXformProject2Apply.h"

#include "spqos/base.h"

#include "spqopt/operators/CLogicalProject.h"
#include "spqopt/operators/CPatternLeaf.h"

using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CXformProject2Apply::CXformProject2Apply
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CXformProject2Apply::CXformProject2Apply(CMemoryPool *mp)
	:  // pattern
	  CXformSubqueryUnnest(SPQOS_NEW(mp) CExpression(
		  mp, SPQOS_NEW(mp) CLogicalProject(mp),
		  SPQOS_NEW(mp) CExpression(
			  mp, SPQOS_NEW(mp) CPatternLeaf(mp)),  // relational child
		  SPQOS_NEW(mp) CExpression(
			  mp, SPQOS_NEW(mp) CPatternTree(mp))  // scalar project list
		  ))
{
}


// EOF
