//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CXformSelect2Apply.cpp
//
//	@doc:
//		Implementation of Select to Apply transform
//---------------------------------------------------------------------------

#include "spqopt/xforms/CXformSelect2Apply.h"

#include "spqos/base.h"

#include "spqopt/operators/CLogicalSelect.h"
#include "spqopt/operators/CPatternLeaf.h"

using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CXformSelect2Apply::CXformSelect2Apply
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CXformSelect2Apply::CXformSelect2Apply(CMemoryPool *mp)
	:  // pattern
	  CXformSubqueryUnnest(SPQOS_NEW(mp) CExpression(
		  mp, SPQOS_NEW(mp) CLogicalSelect(mp),
		  SPQOS_NEW(mp) CExpression(
			  mp, SPQOS_NEW(mp) CPatternLeaf(mp)),  // relational child
		  SPQOS_NEW(mp)
			  CExpression(mp, SPQOS_NEW(mp) CPatternTree(mp))  // predicate tree
		  ))
{
}

// EOF
