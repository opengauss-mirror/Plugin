//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CHeapObject.h
//
//	@doc:
//		Class of all objects that must reside on the heap;
//---------------------------------------------------------------------------
#ifndef SPQOS_CHeapObject_H
#define SPQOS_CHeapObject_H

#include "spqos/types.h"

namespace spqos
{
//---------------------------------------------------------------------------
//	@class:
//		CHeapObject
//
//	@doc:
//		Constructor tests stack layout to ensure object is not allocated on stack;
//		constructor is protected to prevent direct instantiation of class;
//
//---------------------------------------------------------------------------
class CHeapObject
{
protected:
	CHeapObject();
};
}  // namespace spqos

#endif	// !SPQOS_CHeapObject_H

// EOF
