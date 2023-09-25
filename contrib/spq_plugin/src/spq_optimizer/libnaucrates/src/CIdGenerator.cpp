//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 Greenplum, Inc.
//
//	@filename:
//		CIdGenerator.cpp
//
//	@doc:
//		Implementing the ULONG Counter
//---------------------------------------------------------------------------

#include "naucrates/dxl/CIdGenerator.h"

#include "spqos/base.h"

using namespace spqdxl;
using namespace spqos;

CIdGenerator::CIdGenerator(ULONG start_id) : id(start_id)
{
}

//---------------------------------------------------------------------------
//	@function:
//		CIdGenerator::next_id
//
//	@doc:
//		Returns the next unique id
//
//---------------------------------------------------------------------------
ULONG
CIdGenerator::next_id()
{
	return id++;
}

//---------------------------------------------------------------------------
//	@function:
//		CIdGenerator::current_id
//
//	@doc:
//		Returns the current unique id used
//
//---------------------------------------------------------------------------
ULONG
CIdGenerator::current_id()
{
	return id;
}


// EOF
