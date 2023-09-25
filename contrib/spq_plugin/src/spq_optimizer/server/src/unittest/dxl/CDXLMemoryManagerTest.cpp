//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2010 Greenplum, Inc.
//
//	@filename:
//		CDXLMemoryManagerTest.cpp
//
//	@doc:
//		Tests the memory manager to be plugged in Xerces.
//---------------------------------------------------------------------------

#include "unittest/dxl/CDXLMemoryManagerTest.h"

#include "spqos/base.h"
#include "spqos/memory/CAutoMemoryPool.h"
#include "spqos/test/CUnittest.h"

using namespace spqos;
using namespace spqdxl;

//---------------------------------------------------------------------------
//	@function:
//		CDXLMemoryManagerTest::EresUnittest
//
//	@doc:
//
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CDXLMemoryManagerTest::EresUnittest()
{
	CUnittest rgut[] = {
		SPQOS_UNITTEST_FUNC(CDXLMemoryManagerTest::EresUnittest_Basic)};

	return CUnittest::EresExecute(rgut, SPQOS_ARRAY_SIZE(rgut));
}

//---------------------------------------------------------------------------
//	@function:
//		CDXLMemoryManagerTest::EresUnittest_Basic
//
//	@doc:
//		Test for allocating and deallocating memory, as required by the Xerces parser
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CDXLMemoryManagerTest::EresUnittest_Basic()
{
	// create memory pool
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	CDXLMemoryManager *dxl_memory_manager = SPQOS_NEW(mp) CDXLMemoryManager(mp);
	void *pvMemory = dxl_memory_manager->allocate(5);

	SPQOS_ASSERT(NULL != pvMemory);

	dxl_memory_manager->deallocate(pvMemory);

	// cleanup
	SPQOS_DELETE(dxl_memory_manager);
	// pvMemory is deallocated through the memory manager, otherwise the test will throw
	// with a memory leak

	return SPQOS_OK;
}



// EOF
