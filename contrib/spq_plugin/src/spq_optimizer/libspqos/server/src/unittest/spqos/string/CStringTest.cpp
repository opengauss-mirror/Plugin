//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2010 Greenplum, Inc.
//
//	@filename:
//		CStringTest.cpp
//
//	@doc:
//		Tests for CStringStatic
//---------------------------------------------------------------------------

#include "unittest/spqos/string/CStringTest.h"

#include "spqos/base.h"
#include "spqos/string/CStringStatic.h"
#include "spqos/test/CUnittest.h"


using namespace spqos;

//---------------------------------------------------------------------------
//	@function:
//		CStringTest::EresUnittest
//
//	@doc:
//		Driver for unittests
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CStringTest::EresUnittest()
{
	CUnittest rgut[] = {
		SPQOS_UNITTEST_FUNC(CStringTest::EresUnittest_Equals),
		SPQOS_UNITTEST_FUNC(CStringTest::EresUnittest_Append),
		SPQOS_UNITTEST_FUNC(CStringTest::EresUnittest_AppendFormat),
	};

	return CUnittest::EresExecute(rgut, SPQOS_ARRAY_SIZE(rgut));
}


//---------------------------------------------------------------------------
//	@function:
//		CStringTest::EresUnittest_Append
//
//	@doc:
//		Test appending of strings
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CStringTest::EresUnittest_Append()
{
	CHAR buffer1[16];
	CHAR buffer2[8];
	CHAR buffer3[8];

	CStringStatic ss1(buffer1, SPQOS_ARRAY_SIZE(buffer1), "123");
	CStringStatic ss2(buffer2, SPQOS_ARRAY_SIZE(buffer2), "456");
	CStringStatic ss3(buffer3, SPQOS_ARRAY_SIZE(buffer3));

	ss1.Append(&ss2);

	SPQOS_ASSERT(ss1.Equals("123456"));

	// append an empty string
	ss1.Append(&ss3);

	// string should be the same as before
	SPQOS_ASSERT(ss1.Equals("123456"));

	// append to an empty string
	ss3.Append(&ss1);

	SPQOS_ASSERT(ss3.Equals("123456"));

	// check truncation
	ss3.Append(&ss2);
	SPQOS_ASSERT(ss3.Equals("1234564"));

	// test wide character string
	ss1.Reset();
	ss1.AppendConvert(SPQOS_WSZ_LIT("Wide \x1111 character string"));
	SPQOS_ASSERT(ss1.Equals("Wide . characte"));

	return SPQOS_OK;
}


//---------------------------------------------------------------------------
//	@function:
//		CStringTest::EresUnittest_AppendFormat
//
//	@doc:
//		Test formatting strings
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CStringTest::EresUnittest_AppendFormat()
{
	CHAR buffer1[16];
	CHAR buffer2[12];

	CStringStatic ss1(buffer1, SPQOS_ARRAY_SIZE(buffer1), "Hello");
	CStringStatic ss2(buffer2, SPQOS_ARRAY_SIZE(buffer2), "Hello");

	ss1.AppendFormat(" world %d", 123);
	ss2.AppendFormat(" world %d", 123);

	SPQOS_ASSERT(ss1.Equals("Hello world 123"));
	SPQOS_ASSERT(ss2.Equals("Hello world"));

	return SPQOS_OK;
}


//---------------------------------------------------------------------------
//	@function:
//		CStringTest::EresUnittest_Equals
//
//	@doc:
//		Test checking for equality of strings
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CStringTest::EresUnittest_Equals()
{
#ifdef SPQOS_DEBUG
	// static strings
	CHAR buffer1[8];
	CHAR buffer2[8];
	CHAR buffer3[8];

	CStringStatic ss1(buffer1, SPQOS_ARRAY_SIZE(buffer1), "123");
	CStringStatic ss2(buffer2, SPQOS_ARRAY_SIZE(buffer2), "123");
	CStringStatic ss3(buffer3, SPQOS_ARRAY_SIZE(buffer3), "12");

	SPQOS_ASSERT(ss1.Equals(ss2.Buffer()));
	SPQOS_ASSERT(!ss1.Equals(ss3.Buffer()));
	SPQOS_ASSERT(!ss3.Equals(ss1.Buffer()));
#endif	// #ifdef SPQOS_DEBUG

	return SPQOS_OK;
}

// EOF
