//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2010 Greenplum, Inc.
//
//	@filename:
//		CDXLMemoryManagerTest.h
//
//	@doc:
//		Tests the memory manager to be plugged in Xerces parser.
//---------------------------------------------------------------------------


#ifndef SPQOPT_CDXLMemoryManagerTest_H
#define SPQOPT_CDXLMemoryManagerTest_H

#include "spqos/base.h"

#include "naucrates/dxl/operators/CDXLNode.h"
#include "naucrates/dxl/xml/CDXLMemoryManager.h"


namespace spqdxl
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CDXLMemoryManagerTest
//
//	@doc:
//		Static unit tests
//
//---------------------------------------------------------------------------

class CDXLMemoryManagerTest
{
public:
	// unittests
	static SPQOS_RESULT EresUnittest();
	static SPQOS_RESULT EresUnittest_Basic();

};	// class CDXLMemoryManagerTest
}  // namespace spqdxl

#endif	// !SPQOPT_CDXLMemoryManagerTest_H

// EOF
