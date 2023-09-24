//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CColRefComputed.cpp
//
//	@doc:
//		Implementation of column reference class for computed columns
//---------------------------------------------------------------------------

#include "spqopt/base/CColRefComputed.h"

#include "spqos/base.h"

using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CColRefComputed::CColRefComputed
//
//	@doc:
//		ctor
//		takes ownership of string; verify string is properly formatted
//
//---------------------------------------------------------------------------
CColRefComputed::CColRefComputed(const IMDType *pmdtype, INT type_modifier,
								 ULONG id, const CName *pname)
	: CColRef(pmdtype, type_modifier, id, pname)
{
	SPQOS_ASSERT(NULL != pmdtype);
	SPQOS_ASSERT(pmdtype->MDId()->IsValid());
	SPQOS_ASSERT(NULL != pname);
}


//---------------------------------------------------------------------------
//	@function:
//		CColRefComputed::~CColRefComputed
//
//	@doc:
//		ctor
//
//---------------------------------------------------------------------------
CColRefComputed::~CColRefComputed()
{
}


// EOF
