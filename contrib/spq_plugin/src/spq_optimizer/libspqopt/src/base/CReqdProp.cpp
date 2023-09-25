//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 Greenplum, Inc.
//
//	@filename:
//		CReqdProp.cpp
//
//	@doc:
//		Implementation of required properties
//---------------------------------------------------------------------------

#include "spqopt/base/CReqdProp.h"

#include "spqos/base.h"

#include "spqopt/operators/COperator.h"

#ifdef SPQOS_DEBUG
#include "spqos/error/CAutoTrace.h"

#include "spqopt/base/COptCtxt.h"
#endif	// SPQOS_DEBUG

using namespace spqopt;

FORCE_GENERATE_DBGSTR(CReqdProp);

//---------------------------------------------------------------------------
//	@function:
//		CReqdProp::CReqdProp
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CReqdProp::CReqdProp()
{
}


//---------------------------------------------------------------------------
//	@function:
//		CReqdProp::~CReqdProp
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CReqdProp::~CReqdProp()
{
}


// EOF
