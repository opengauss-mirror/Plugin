//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		CTaskLocalStorageTest.cpp
//
//	@doc:
//		Tests for CTaskLocalStorage
//---------------------------------------------------------------------------

#include "unittest/spqos/task/CTaskLocalStorageTest.h"

#include "spqos/common/CSyncHashtableAccessByKey.h"
#include "spqos/task/CAutoTraceFlag.h"
#include "spqos/task/CTask.h"
#include "spqos/task/CTaskLocalStorage.h"
#include "spqos/task/CTaskLocalStorageObject.h"
#include "spqos/task/CTraceFlagIter.h"
#include "spqos/test/CUnittest.h"

using namespace spqos;


//---------------------------------------------------------------------------
//	@function:
//		CTaskLocalStorageTest::EresUnittest
//
//	@doc:
//		Unittest for TLS
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CTaskLocalStorageTest::EresUnittest()
{
	CUnittest rgut[] = {
		SPQOS_UNITTEST_FUNC(CTaskLocalStorageTest::EresUnittest_Basics),
		SPQOS_UNITTEST_FUNC(CTaskLocalStorageTest::EresUnittest_TraceFlags),
	};

	return CUnittest::EresExecute(rgut, SPQOS_ARRAY_SIZE(rgut));
}


//---------------------------------------------------------------------------
//	@function:
//		CTaskLocalStorageTest::EresUnittest_Basics
//
//	@doc:
//		Simple store/retrieve test for TLS
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CTaskLocalStorageTest::EresUnittest_Basics()
{
	CTestObject tobj;

	// store object in TLS
	ITask::Self()->GetTls().Store(&tobj);

	// assert identiy when looking it up
	SPQOS_ASSERT(&tobj ==
				ITask::Self()->GetTls().Get(CTaskLocalStorage::EtlsidxTest));

	// clean out TLS
	ITask::Self()->GetTls().Remove(&tobj);

	return SPQOS_OK;
}


//---------------------------------------------------------------------------
//	@function:
//		CTaskLocalStorageTest::EresUnittest_TraceFlags
//
//	@doc:
//		Test trace flag set, retrieve and iterate
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CTaskLocalStorageTest::EresUnittest_TraceFlags()
{
	SPQOS_ASSERT(!SPQOS_FTRACE(EtraceTest));

	SPQOS_SET_TRACE(EtraceTest);

	SPQOS_ASSERT(SPQOS_FTRACE(EtraceTest));

	// test auto trace flag
	{
		CAutoTraceFlag atf(EtraceTest, false /*value*/);

		SPQOS_ASSERT(!SPQOS_FTRACE(EtraceTest));
	}
	SPQOS_ASSERT(SPQOS_FTRACE(EtraceTest));

#ifdef SPQOS_DEBUG
	// test trace flag iterator
	CTraceFlagIter tfi;
	BOOL fFound = false;
	while (tfi.Advance())
	{
		SPQOS_ASSERT_IMP(!fFound, EtraceTest == tfi.Bit());
		fFound = true;
	}
#endif	// SPQOS_DEBUG

	SPQOS_ASSERT(SPQOS_FTRACE(EtraceTest));
	SPQOS_UNSET_TRACE(EtraceTest);
	SPQOS_ASSERT(!SPQOS_FTRACE(EtraceTest));

	return SPQOS_OK;
}
// EOF
