//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (c) 2004-2015 Pivotal Software, Inc.
//
//	@filename:
//		main.cpp
//
//	@doc:
//		Startup routines for optimizer
//---------------------------------------------------------------------------

#include "spqos/_api.h"
#include "spqos/common/CMainArgs.h"
#include "spqos/test/CFSimulatorTestExt.h"
#include "spqos/test/CUnittest.h"
#include "spqos/types.h"


// test headers

#include "unittest/spqos/common/CAutoPTest.h"
#include "unittest/spqos/common/CAutoRefTest.h"
#include "unittest/spqos/common/CAutoRgTest.h"
#include "unittest/spqos/common/CBitSetIterTest.h"
#include "unittest/spqos/common/CBitSetTest.h"
#include "unittest/spqos/common/CBitVectorTest.h"
#include "unittest/spqos/common/CDoubleTest.h"
#include "unittest/spqos/common/CDynamicPtrArrayTest.h"
#include "unittest/spqos/common/CEnumSetTest.h"
#include "unittest/spqos/common/CHashMapIterTest.h"
#include "unittest/spqos/common/CHashMapTest.h"
#include "unittest/spqos/common/CHashSetIterTest.h"
#include "unittest/spqos/common/CHashSetTest.h"
#include "unittest/spqos/common/CListTest.h"
#include "unittest/spqos/common/CRefCountTest.h"
#include "unittest/spqos/common/CStackTest.h"
#include "unittest/spqos/common/CSyncHashtableTest.h"
#include "unittest/spqos/common/CSyncListTest.h"
#include "unittest/spqos/error/CErrorHandlerTest.h"
#include "unittest/spqos/error/CExceptionTest.h"
#include "unittest/spqos/error/CFSimulatorTest.h"
#include "unittest/spqos/error/CLoggerTest.h"
#include "unittest/spqos/error/CMessageRepositoryTest.h"
#include "unittest/spqos/error/CMessageTableTest.h"
#include "unittest/spqos/error/CMessageTest.h"
#include "unittest/spqos/error/CMiniDumperTest.h"
#include "unittest/spqos/io/CFileTest.h"
#include "unittest/spqos/io/COstreamBasicTest.h"
#include "unittest/spqos/io/COstreamFileTest.h"
#include "unittest/spqos/io/COstreamStringTest.h"
#include "unittest/spqos/memory/CCacheTest.h"
#include "unittest/spqos/memory/CMemoryPoolBasicTest.h"
#include "unittest/spqos/string/CStringTest.h"
#include "unittest/spqos/string/CWStringTest.h"
#include "unittest/spqos/task/CTaskLocalStorageTest.h"
#include "unittest/spqos/test/CUnittestTest.h"


using namespace spqos;

// static array of all known unittest routines
static spqos::CUnittest rgut[] = {
	// common
	SPQOS_UNITTEST_STD(CAutoPTest),
	SPQOS_UNITTEST_STD(CAutoRefTest),
	SPQOS_UNITTEST_STD(CAutoRgTest),
	SPQOS_UNITTEST_STD(CBitSetIterTest),
	SPQOS_UNITTEST_STD(CBitSetTest),
	SPQOS_UNITTEST_STD(CBitVectorTest),
	SPQOS_UNITTEST_STD(CDynamicPtrArrayTest),
	SPQOS_UNITTEST_STD(CEnumSetTest),
	SPQOS_UNITTEST_STD(CDoubleTest),
	SPQOS_UNITTEST_STD(CHashMapTest),
	SPQOS_UNITTEST_STD(CHashMapIterTest),
	SPQOS_UNITTEST_STD(CHashSetTest),
	SPQOS_UNITTEST_STD(CHashSetIterTest),
	SPQOS_UNITTEST_STD(CRefCountTest),
	SPQOS_UNITTEST_STD(CListTest),
	SPQOS_UNITTEST_STD(CStackTest),
	SPQOS_UNITTEST_STD(CSyncHashtableTest),
	SPQOS_UNITTEST_STD(CSyncListTest),

	// error
	SPQOS_UNITTEST_STD(CErrorHandlerTest),
	SPQOS_UNITTEST_STD(CExceptionTest),
	SPQOS_UNITTEST_STD(CLoggerTest),
	SPQOS_UNITTEST_STD(CMessageTest),
	SPQOS_UNITTEST_STD(CMessageTableTest),
	SPQOS_UNITTEST_STD(CMessageRepositoryTest),
	SPQOS_UNITTEST_STD(CMiniDumperTest),

	// io
	SPQOS_UNITTEST_STD(COstreamBasicTest),
	SPQOS_UNITTEST_STD(COstreamStringTest),
	SPQOS_UNITTEST_STD(COstreamFileTest),
	SPQOS_UNITTEST_STD(CFileTest),

	// memory
	SPQOS_UNITTEST_STD(CMemoryPoolBasicTest),
	SPQOS_UNITTEST_STD(CCacheTest),

	// string
	SPQOS_UNITTEST_STD(CWStringTest),
	SPQOS_UNITTEST_STD(CStringTest),

	// task
	SPQOS_UNITTEST_STD(CTaskLocalStorageTest),

	// test
	SPQOS_UNITTEST_STD_SUBTEST(CUnittestTest, 0),
	SPQOS_UNITTEST_STD_SUBTEST(CUnittestTest, 1),
	SPQOS_UNITTEST_STD_SUBTEST(CUnittestTest, 2),


#ifdef SPQOS_FPSIMULATOR
	// simulation
	SPQOS_UNITTEST_STD(CFSimulatorTest),
	SPQOS_UNITTEST_EXT(CFSimulatorTestExt),
#endif	// SPQOS_FPSIMULATOR
};

// static variable counting the number of failed tests; PvExec overwrites with
// the actual count of failed tests
static ULONG tests_failed = 0;

//---------------------------------------------------------------------------
//	@function:
//		PvExec
//
//	@doc:
//		Function driving execution.
//
//---------------------------------------------------------------------------
static void *
PvExec(void *pv)
{
	CMainArgs *pma = (CMainArgs *) pv;
	tests_failed = CUnittest::Driver(pma);

	return NULL;
}

//---------------------------------------------------------------------------
//	@function:
//		main
//
//	@doc:
//		Entry point for stand-alone optimizer binary; ignore arguments for the
//		time being
//
//---------------------------------------------------------------------------
INT
main(INT iArgs, const CHAR **rgszArgs)
{
	// setup args for unittest params
	CMainArgs ma(iArgs, rgszArgs, "cuU:xT:");

	struct spqos_init_params init_params = {NULL};
	spqos_init(&init_params);

	SPQOS_ASSERT(iArgs >= 0);

	// initialize unittest framework
	CUnittest::Init(rgut, SPQOS_ARRAY_SIZE(rgut), NULL, NULL);

	spqos_exec_params params;
	params.func = PvExec;
	params.arg = &ma;
	params.result = NULL;
	params.stack_start = &params;
	params.error_buffer = NULL;
	params.error_buffer_size = -1;
	params.abort_requested = NULL;

	if (spqos_exec(&params) || (tests_failed != 0))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


// EOF
