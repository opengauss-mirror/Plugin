//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CXformImplementTVFNoArgs.cpp
//
//	@doc:
//		Implementation of transform
//---------------------------------------------------------------------------

#include "spqopt/xforms/CXformImplementTVFNoArgs.h"

#include "spqos/base.h"

#include "spqopt/operators/CLogicalTVF.h"

using namespace spqopt;

//---------------------------------------------------------------------------
//	@function:
//		CXformImplementTVFNoArgs::CXformImplementTVFNoArgs
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CXformImplementTVFNoArgs::CXformImplementTVFNoArgs(CMemoryPool *mp)
	: CXformImplementTVF(
		  // pattern
		  SPQOS_NEW(mp) CExpression(mp, SPQOS_NEW(mp) CLogicalTVF(mp)))
{
}

// EOF
