//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		CXformImplementation.cpp
//
//	@doc:
//		Implementation of basic implementation transformation
//---------------------------------------------------------------------------

#include "spqopt/xforms/CXformImplementation.h"

#include "spqos/base.h"

using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CXformImplementation::CXformImplementation
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CXformImplementation::CXformImplementation(CExpression *pexpr) : CXform(pexpr)
{
	SPQOS_ASSERT(NULL != pexpr);
}


//---------------------------------------------------------------------------
//	@function:
//		CXformImplementation::~CXformImplementation
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CXformImplementation::~CXformImplementation()
{
}


// EOF
