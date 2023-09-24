//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2008 Greenplum, Inc.
//
//	@filename:
//		CWStringTest.cpp
//
//	@doc:
//		Tests for CWStringBase, CWString, and CWStringConst
//---------------------------------------------------------------------------

#include "unittest/spqos/string/CWStringTest.h"

#include <locale.h>

#include "spqos/base.h"
#include "spqos/error/CAutoTrace.h"
#include "spqos/memory/CAutoMemoryPool.h"
#include "spqos/string/CWStringConst.h"
#include "spqos/string/CWStringDynamic.h"
#include "spqos/string/CWStringStatic.h"
#include "spqos/test/CUnittest.h"

using namespace spqos;

//---------------------------------------------------------------------------
//	@function:
//		CWStringTest::EresUnittest
//
//	@doc:
//		Driver for unittests
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CWStringTest::EresUnittest()
{
	CUnittest rgut[] = {
		SPQOS_UNITTEST_FUNC(CWStringTest::EresUnittest_Initialize),
		SPQOS_UNITTEST_FUNC(CWStringTest::EresUnittest_Equals),
		SPQOS_UNITTEST_FUNC(CWStringTest::EresUnittest_Append),
		SPQOS_UNITTEST_FUNC(CWStringTest::EresUnittest_AppendFormat),
		SPQOS_UNITTEST_FUNC(CWStringTest::EresUnittest_Copy),
		SPQOS_UNITTEST_FUNC(CWStringTest::EresUnittest_AppendEscape),
		SPQOS_UNITTEST_FUNC(CWStringTest::EresUnittest_AppendFormatLarge),
#ifndef SPQOS_Darwin
		SPQOS_UNITTEST_FUNC(
			CWStringTest::EresUnittest_AppendFormatInvalidLocale),
#endif
	};

	return CUnittest::EresExecute(rgut, SPQOS_ARRAY_SIZE(rgut));
}


//---------------------------------------------------------------------------
//	@function:
//		CWStringTest::EresUnittest_Append
//
//	@doc:
//		Test appending of strings
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CWStringTest::EresUnittest_Append()
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	CWStringDynamic *pstr1 =
		SPQOS_NEW(mp) CWStringDynamic(mp, SPQOS_WSZ_LIT("123"));
	CWStringDynamic *pstr2 =
		SPQOS_NEW(mp) CWStringDynamic(mp, SPQOS_WSZ_LIT("456"));
	CWStringDynamic *pstr3 = SPQOS_NEW(mp) CWStringDynamic(mp);

	WCHAR buffer1[8];
	WCHAR buffer2[8];
	WCHAR buffer3[8];

	CWStringStatic ss1(buffer1, SPQOS_ARRAY_SIZE(buffer1), SPQOS_WSZ_LIT("123"));
	CWStringStatic ss2(buffer2, SPQOS_ARRAY_SIZE(buffer2), SPQOS_WSZ_LIT("456"));
	CWStringStatic ss3(buffer3, SPQOS_ARRAY_SIZE(buffer3));

	pstr1->Append(pstr2);
	ss1.Append(&ss2);

#ifdef SPQOS_DEBUG
	CWStringConst cstr1(SPQOS_WSZ_LIT("123456"));
	CWStringConst cstr2(SPQOS_WSZ_LIT("1234564"));
#endif

	SPQOS_ASSERT(pstr1->Equals(&cstr1));
	SPQOS_ASSERT(ss1.Equals(&cstr1));

	// append an empty string
	pstr1->Append(pstr3);
	ss1.Append(&ss3);

	// string should be the same as before
	SPQOS_ASSERT(pstr1->Equals(&cstr1));
	SPQOS_ASSERT(ss1.Equals(&cstr1));

	// append to an empty string
	pstr3->Append(pstr1);
	ss3.Append(&ss1);

	SPQOS_ASSERT(pstr3->Equals(pstr1));
	SPQOS_ASSERT(ss3.Equals(&ss1));

	// check truncation
	ss3.Append(&ss2);
	SPQOS_ASSERT(ss3.Equals(&cstr2));

	// cleanup
	SPQOS_DELETE(pstr1);
	SPQOS_DELETE(pstr2);
	SPQOS_DELETE(pstr3);

	return SPQOS_OK;
}


//---------------------------------------------------------------------------
//	@function:
//		CWStringTest::EresUnittest_AppendFormat
//
//	@doc:
//		Test formatting strings
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CWStringTest::EresUnittest_AppendFormat()
{
	CAutoMemoryPool amp(CAutoMemoryPool::ElcExc);
	CMemoryPool *mp = amp.Pmp();

	CWStringDynamic *pstr1 =
		SPQOS_NEW(mp) CWStringDynamic(mp, SPQOS_WSZ_LIT("Hello"));

	WCHAR buffer1[16];
	WCHAR buffer2[12];

	CWStringStatic ss1(buffer1, SPQOS_ARRAY_SIZE(buffer1),
					   SPQOS_WSZ_LIT("Hello"));
	CWStringStatic ss2(buffer2, SPQOS_ARRAY_SIZE(buffer2),
					   SPQOS_WSZ_LIT("Hello"));

	pstr1->AppendFormat(SPQOS_WSZ_LIT(" world %d"), 123);
	ss1.AppendFormat(SPQOS_WSZ_LIT(" world %d"), 123);
	ss2.AppendFormat(SPQOS_WSZ_LIT(" world %d"), 123);

	CWStringConst cstr1(SPQOS_WSZ_LIT("Hello world 123"));
#ifdef SPQOS_DEBUG
	CWStringConst cstr2(SPQOS_WSZ_LIT("Hello world"));
#endif

	SPQOS_ASSERT(pstr1->Equals(&cstr1));
	SPQOS_ASSERT(ss1.Equals(&cstr1));
	SPQOS_ASSERT(ss2.Equals(&cstr2));

	SPQOS_RESULT eres = SPQOS_OK;

	// cleanup
	SPQOS_DELETE(pstr1);

	return eres;
}



// This tests the behavior of AppendFormat for unicode characters when the locale settings of the system are incompatible
// AppendFormat uses a C library function, vswprintf(), whose behavior is platform specific.
// vswprintf() returns with error on non-Darwin platforms when the locale is incompatible with unicode string.
// Hence do not run this test on Darwin platform.
#ifndef SPQOS_Darwin
SPQOS_RESULT
CWStringTest::EresUnittest_AppendFormatInvalidLocale()
{
	CAutoMemoryPool amp(CAutoMemoryPool::ElcExc);
	CMemoryPool *mp = amp.Pmp();

	CHAR *oldLocale = setlocale(LC_CTYPE, NULL);
	CWStringDynamic *pstr1 = SPQOS_NEW(mp) CWStringDynamic(mp);

	SPQOS_RESULT eres = SPQOS_OK;

	setlocale(LC_CTYPE, "C");
	SPQOS_TRY
	{
		pstr1->AppendFormat(SPQOS_WSZ_LIT("%s"), (CHAR *) "ÃË", 123);

		eres = SPQOS_FAILED;
	}
	SPQOS_CATCH_EX(ex)
	{
		SPQOS_ASSERT(SPQOS_MATCH_EX(ex, CException::ExmaSystem,
								  CException::ExmiIllegalByteSequence));

		SPQOS_RESET_EX;
	}
	SPQOS_CATCH_END;

	// cleanup
	setlocale(LC_CTYPE, oldLocale);
	SPQOS_DELETE(pstr1);

	return eres;
}
#endif

//---------------------------------------------------------------------------
//	@function:
//		CWStringTest::EresUnittest_AppendFormatLarge
//
//	@doc:
//		Test formatting large strings
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CWStringTest::EresUnittest_AppendFormatLarge()
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	CWStringDynamic *pstr1 =
		SPQOS_NEW(mp) CWStringDynamic(mp, SPQOS_WSZ_LIT("Hello"));
	CWStringConst cstr1(SPQOS_WSZ_LIT(" World "));

	const ULONG ulLengthInit = pstr1->Length();
	const ULONG ulLengthAppend = cstr1.Length();
	const ULONG ulIters = 1000;
	const ULONG ulExpected = ulLengthInit + ulLengthAppend * ulIters;

	for (ULONG ul = 0; ul < ulIters; ul++)
	{
		pstr1->AppendFormat(SPQOS_WSZ_LIT("%ls"), cstr1.GetBuffer());
	}

	{
		CAutoTrace at(mp);
		at.Os() << std::endl << "Formatted string size:" << pstr1->Length();
		at.Os() << std::endl
				<< "Expected string size:" << ulExpected << std::endl;
	}

	SPQOS_ASSERT(pstr1->Length() == ulExpected);

	// cleanup
	SPQOS_DELETE(pstr1);

	// append small string
	CWStringDynamic *pstr2 =
		SPQOS_NEW(mp) CWStringDynamic(mp, SPQOS_WSZ_LIT("Hello"));
	pstr2->AppendCharArray(" World");
	SPQOS_TRACE(pstr2->GetBuffer());

	// cleanup
	SPQOS_DELETE(pstr2);

	// append large string
	CWStringDynamic *pstr3 =
		SPQOS_NEW(mp) CWStringDynamic(mp, SPQOS_WSZ_LIT("Hello "));
#ifdef SPQOS_DEBUG
	ULONG ulStartLength = pstr3->Length();
#endif	// SPQOS_DEBUG
	const ULONG ulAppendLength = 50000;
	CHAR *sz = SPQOS_NEW_ARRAY(mp, CHAR, ulAppendLength + 1);
	for (ULONG ul = 0; ul < ulAppendLength; ul++)
	{
		sz[ul] = 'W';
	}
	sz[ulAppendLength] = '\0';

	// append a large string
	pstr3->AppendCharArray(sz);
	SPQOS_ASSERT(ulAppendLength + ulStartLength == pstr3->Length());
	SPQOS_DELETE_ARRAY(sz);

	// do another append of a small string
	pstr3->AppendCharArray(" World");

	pstr3->AppendWideCharArray(SPQOS_WSZ_LIT(" WIDE WORLD"));
	SPQOS_TRACE(pstr3->GetBuffer());
	SPQOS_TRACE(SPQOS_WSZ_LIT("\n"));

	// cleanup
	SPQOS_DELETE(pstr3);

	WCHAR w_str[25];
	CWStringStatic *pstr4 = SPQOS_NEW(mp)
		CWStringStatic(w_str, SPQOS_ARRAY_SIZE(w_str), SPQOS_WSZ_LIT("Hello"));
	pstr4->AppendCharArray(" World");
	pstr4->AppendWideCharArray(SPQOS_WSZ_LIT(" WIDE WORLD"));

	// another append should be truncated since we will overflow string
	pstr4->AppendWideCharArray(L" 1234567");
	SPQOS_TRACE(pstr4->GetBuffer());
	SPQOS_TRACE(SPQOS_WSZ_LIT("\n"));

	SPQOS_ASSERT(pstr4->GetBuffer()[pstr4->Length() - 1] == L'1');

	// cleanup
	SPQOS_DELETE(pstr4);

	return SPQOS_OK;
}


//---------------------------------------------------------------------------
//	@function:
//		CWStringTest::EresUnittest_Initialize
//
//	@doc:
//		Test string construction
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CWStringTest::EresUnittest_Initialize()
{
#ifdef SPQOS_DEBUG  // run this test in debug mode only
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	CWStringDynamic *pstr1 =
		SPQOS_NEW(mp) CWStringDynamic(mp, SPQOS_WSZ_LIT("123"));

	CWStringConst cstr1(SPQOS_WSZ_LIT("123"));
	SPQOS_ASSERT(pstr1->Equals(&cstr1));

	// empty string initialization
	CWStringDynamic *pstr2 = SPQOS_NEW(mp) CWStringDynamic(mp);
	WCHAR buffer[16];
	CWStringStatic ss(buffer, SPQOS_ARRAY_SIZE(buffer));

	CWStringConst cstr2(SPQOS_WSZ_LIT(""));
	SPQOS_ASSERT(pstr2->Equals(&cstr2));
	SPQOS_ASSERT(ss.Equals(&cstr2));
	SPQOS_ASSERT(0 == pstr2->Length());
	SPQOS_ASSERT(0 == ss.Length());

	// constant string initialization
	CWStringConst *pcstr1 = SPQOS_NEW(mp) CWStringConst(SPQOS_WSZ_LIT("123"));
	SPQOS_ASSERT(pcstr1->Equals(&cstr1));

	// cleanup
	SPQOS_DELETE(pstr1);
	SPQOS_DELETE(pstr2);
	SPQOS_DELETE(pcstr1);

#endif	// #ifdef SPQOS_DEBUG
	return SPQOS_OK;
}

//---------------------------------------------------------------------------
//	@function:
//		CWStringTest::EresUnittest_Equals
//
//	@doc:
//		Test checking for equality of strings
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CWStringTest::EresUnittest_Equals()
{
#ifdef SPQOS_DEBUG
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	// dynamic strings
	CWStringDynamic *str1 =
		SPQOS_NEW(mp) CWStringDynamic(mp, SPQOS_WSZ_LIT("123"));
	CWStringDynamic *str2 =
		SPQOS_NEW(mp) CWStringDynamic(mp, SPQOS_WSZ_LIT("123"));
	CWStringDynamic *str3 =
		SPQOS_NEW(mp) CWStringDynamic(mp, SPQOS_WSZ_LIT("12"));

	SPQOS_ASSERT(str1->Equals(str2));
	SPQOS_ASSERT(!str1->Equals(str3));
	SPQOS_ASSERT(!str3->Equals(str1));

	// static strings
	WCHAR buffer1[8];
	WCHAR buffer2[8];
	WCHAR buffer3[8];

	CWStringStatic ss1(buffer1, SPQOS_ARRAY_SIZE(buffer1), SPQOS_WSZ_LIT("123"));
	CWStringStatic ss2(buffer2, SPQOS_ARRAY_SIZE(buffer2), SPQOS_WSZ_LIT("123"));
	CWStringStatic ss3(buffer3, SPQOS_ARRAY_SIZE(buffer3), SPQOS_WSZ_LIT("12"));

	SPQOS_ASSERT(ss1.Equals(&ss2));
	SPQOS_ASSERT(!ss1.Equals(&ss3));
	SPQOS_ASSERT(!ss3.Equals(&ss1));

	// Const strings
	CWStringConst *cstr1 = SPQOS_NEW(mp) CWStringConst(SPQOS_WSZ_LIT("123"));
	CWStringConst *cstr2 = SPQOS_NEW(mp) CWStringConst(SPQOS_WSZ_LIT("12"));
	SPQOS_ASSERT(!cstr1->Equals(cstr2));
	SPQOS_ASSERT(cstr1->Equals(str1));

	// cleanup
	SPQOS_DELETE(str1);
	SPQOS_DELETE(str2);
	SPQOS_DELETE(str3);
	SPQOS_DELETE(cstr1);
	SPQOS_DELETE(cstr2);

#endif	// #ifdef SPQOS_DEBUG

	return SPQOS_OK;
}

//---------------------------------------------------------------------------
//	@function:
//		CWStringTest::EresUnittest_Copy
//
//	@doc:
//		Test deep copying of strings
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CWStringTest::EresUnittest_Copy()
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	CWStringDynamic *pstr1 =
		SPQOS_NEW(mp) CWStringDynamic(mp, SPQOS_WSZ_LIT("Hello"));

	CWStringConst *pcstr1 = pstr1->Copy(mp);

	SPQOS_ASSERT(pstr1->Equals(pcstr1));

	// character buffers should be different
	SPQOS_ASSERT(pstr1->GetBuffer() != pcstr1->GetBuffer());

	// cleanup
	SPQOS_DELETE(pstr1);

	SPQOS_ASSERT(NULL != pcstr1->GetBuffer());
	SPQOS_DELETE(pcstr1);

	return SPQOS_OK;
}

//---------------------------------------------------------------------------
//	@function:
//		CWStringTest::EresUnittest_AppendEscape
//
//	@doc:
//		Test replacing character with string
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CWStringTest::EresUnittest_AppendEscape()
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	const WCHAR *w_str = SPQOS_WSZ_LIT("Helloe ");
	CWStringConst cstr(w_str);

	CWStringDynamic strd(mp, w_str);

	WCHAR buffer1[32];
	WCHAR buffer2[9];

	CWStringStatic strs1(buffer1, SPQOS_ARRAY_SIZE(buffer1), w_str);
	CWStringStatic strs2(buffer2, SPQOS_ARRAY_SIZE(buffer2), w_str);

	SPQOS_ASSERT(1 == strd.Find('e'));
	SPQOS_ASSERT(1 == strs1.Find('e'));
	SPQOS_ASSERT(1 == strs2.Find('e'));

	SPQOS_ASSERT(2 == strd.Find('l'));
	SPQOS_ASSERT(6 == strs1.Find(' '));
	SPQOS_ASSERT(-1 == strs2.Find('a'));

	strd.Reset();
	strs1.Reset();
	strs2.Reset();

	strd.AppendEscape(&cstr, 'e', SPQOS_WSZ_LIT("yyy"));
	strs1.AppendEscape(&cstr, 'e', SPQOS_WSZ_LIT("yyy"));
	strs2.AppendEscape(&cstr, 'e', SPQOS_WSZ_LIT("yyy"));

#ifdef SPQOS_DEBUG
	CWStringConst cstr1(SPQOS_WSZ_LIT("Hyyylloyyy "));
	CWStringConst cstr2(SPQOS_WSZ_LIT("Hyyylloy"));
#endif	// SPQOS_DEBUG

	SPQOS_ASSERT(strd.Equals(&cstr1));
	SPQOS_ASSERT(strs1.Equals(&cstr1));
	SPQOS_ASSERT(strs2.Equals(&cstr2));

	strd.AppendEscape(&cstr, 'a', SPQOS_WSZ_LIT("yyy"));
	strs1.AppendEscape(&cstr, 'a', SPQOS_WSZ_LIT("yyy"));
	strs2.AppendEscape(&cstr, 'a', SPQOS_WSZ_LIT("yyy"));

#ifdef SPQOS_DEBUG
	CWStringConst cstr3(SPQOS_WSZ_LIT("Hyyylloyyy Helloe "));
#endif	// SPQOS_DEBUG

	// should be the same
	SPQOS_ASSERT(strd.Equals(&cstr3));
	SPQOS_ASSERT(strs1.Equals(&cstr3));
	SPQOS_ASSERT(strs2.Equals(&cstr2));

	// check escaped characters
	const WCHAR *wszEscape1 = SPQOS_WSZ_LIT("   \\\" ");
	const WCHAR *wszEscape2 = SPQOS_WSZ_LIT("   \\\\\" ");
	const WCHAR *wszEscape3 = SPQOS_WSZ_LIT("   \\\\\\\" ");
	const WCHAR *wszEscape4 = SPQOS_WSZ_LIT("\\\\\\\" ");
	const WCHAR *wszEscape5 = SPQOS_WSZ_LIT("\\\\\\\\\" ");
	const WCHAR *wszEscape6 = SPQOS_WSZ_LIT("\\\\\\\\\\\" ");

	CWStringConst cstrEscape1(wszEscape1);
	CWStringConst cstrEscape2(wszEscape2);
	CWStringConst cstrEscape3(wszEscape3);
	CWStringConst cstrEscape4(wszEscape4);
	CWStringConst cstrEscape5(wszEscape5);
	CWStringConst cstrEscape6(wszEscape6);

	strd.Reset();
	strs1.Reset();
	strd.AppendEscape(&cstrEscape1, '"', SPQOS_WSZ_LIT("\\\""));
	strs1.AppendEscape(&cstrEscape1, '"', SPQOS_WSZ_LIT("\\\""));

	// escape character is skipped
	SPQOS_ASSERT(strd.Equals(&cstrEscape1));
	SPQOS_ASSERT(strs1.Equals(&cstrEscape1));

	strd.Reset();
	strs1.Reset();
	strd.AppendEscape(&cstrEscape2, '"', SPQOS_WSZ_LIT("\\\""));
	strs1.AppendEscape(&cstrEscape2, '"', SPQOS_WSZ_LIT("\\\""));

	// escape character is added
	SPQOS_ASSERT(strd.Equals(&cstrEscape3));
	SPQOS_ASSERT(strs1.Equals(&cstrEscape3));

	strd.Reset();
	strs1.Reset();
	strd.AppendEscape(&cstrEscape4, '"', SPQOS_WSZ_LIT("\\\""));
	strs1.AppendEscape(&cstrEscape4, '"', SPQOS_WSZ_LIT("\\\""));

	// escape character is skipped
	SPQOS_ASSERT(strd.Equals(&cstrEscape4));
	SPQOS_ASSERT(strs1.Equals(&cstrEscape4));

	strd.Reset();
	strs1.Reset();
	strd.AppendEscape(&cstrEscape5, '"', SPQOS_WSZ_LIT("\\\""));
	strs1.AppendEscape(&cstrEscape5, '"', SPQOS_WSZ_LIT("\\\""));

	// escape character is added
	SPQOS_ASSERT(strd.Equals(&cstrEscape6));
	SPQOS_ASSERT(strs1.Equals(&cstrEscape6));

	return SPQOS_OK;
}

// EOF
