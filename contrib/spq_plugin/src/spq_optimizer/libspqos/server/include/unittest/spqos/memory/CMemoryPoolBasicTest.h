//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2008-2010 Greenplum Inc.
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CMemoryPoolBasicTest.h
//
//	@doc:
//      Test for CMemoryPoolBasic
//
//	@owner:
//
//	@test:
//
//---------------------------------------------------------------------------
#ifndef SPQOS_CMemoryPoolBasicTest_H
#define SPQOS_CMemoryPoolBasicTest_H

#include "spqos/memory/CMemoryPool.h"
#include "spqos/memory/CMemoryPoolManager.h"

namespace spqos
{
class CMemoryPoolBasicTest
{
private:
	static SPQOS_RESULT EresTestType();
	static SPQOS_RESULT EresTestExpectedError(SPQOS_RESULT (*pfunc)(),
											 ULONG minor);

	static SPQOS_RESULT EresNewDelete();
	static SPQOS_RESULT EresThrowingCtor();
#ifdef SPQOS_DEBUG
	static SPQOS_RESULT EresLeak();
	static SPQOS_RESULT EresLeakByException();
#endif	// SPQOS_DEBUG

	static ULONG Size(ULONG offset);

public:
	// unittests
	static SPQOS_RESULT EresUnittest();
#ifdef SPQOS_DEBUG
	static SPQOS_RESULT EresUnittest_Print();
#endif	// SPQOS_DEBUG
	static SPQOS_RESULT EresUnittest_TestTracker();
	static SPQOS_RESULT EresUnittest_TestSlab();

};	// class CMemoryPoolBasicTest
}  // namespace spqos

#endif	// !SPQOS_CMemoryPoolBasicTest_H

// EOF
