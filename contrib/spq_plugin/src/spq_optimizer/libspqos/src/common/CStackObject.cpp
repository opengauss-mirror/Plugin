//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CStackObject.cpp
//
//	@doc:
//		Implementation of classes of all objects that must reside on the stack;
//		There used to be an assertion for that here, but it was too fragile.
//---------------------------------------------------------------------------

#include "spqos/common/CStackObject.h"

#include "spqos/utils.h"

using namespace spqos;


//---------------------------------------------------------------------------
//	@function:
//		CStackObject::CStackObject
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CStackObject::CStackObject()
{
}


// EOF
