//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CXformExploration.cpp
//
//	@doc:
//		Implementation of basic exploration transformation
//---------------------------------------------------------------------------

#include "spqopt/xforms/CXformExploration.h"

#include "spqos/base.h"

using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CXformExploration::CXformExploration
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CXformExploration::CXformExploration(CExpression *pexpr) : CXform(pexpr)
{
	SPQOS_ASSERT(NULL != pexpr);
}


//---------------------------------------------------------------------------
//	@function:
//		CXformExploration::~CXformExploration
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CXformExploration::~CXformExploration()
{
}


// EOF
