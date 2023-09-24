//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CStackObject.h
//
//	@doc:
//		Class of all objects that must reside on the stack;
//		e.g., auto objects;
//---------------------------------------------------------------------------
#ifndef SPQOS_CStackObject_H
#define SPQOS_CStackObject_H

#include "spqos/types.h"

namespace spqos
{
//---------------------------------------------------------------------------
//	@class:
//		CStackObject
//
//	@doc:
//		Constructor tests stack layout to ensure object is allocated on stack;
//		constructor is protected to prevent direct instantiation of class;
//
//---------------------------------------------------------------------------
class CStackObject
{
protected:
	CStackObject();
};
}  // namespace spqos

#endif	// !SPQOS_CStackObject_H

// EOF
