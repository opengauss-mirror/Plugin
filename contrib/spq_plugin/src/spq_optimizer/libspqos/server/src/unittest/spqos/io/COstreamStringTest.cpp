//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2008 - 2010 Greenplum, Inc.
//
//	@filename:
//		COstreamStringTest.cpp
//
//	@doc:
//		Tests for COstreamString
//---------------------------------------------------------------------------

#include "unittest/spqos/io/COstreamStringTest.h"

#include "spqos/assert.h"
#include "spqos/error/CMessage.h"
#include "spqos/io/COstreamString.h"
#include "spqos/memory/CAutoMemoryPool.h"
#include "spqos/string/CWStringConst.h"
#include "spqos/string/CWStringDynamic.h"
#include "spqos/test/CUnittest.h"

using namespace spqos;

//---------------------------------------------------------------------------
//	@function:
//		COstreamStringTest::EresUnittest
//
//	@doc:
//		Function for raising assert exceptions; again, encapsulated in a function
//		to facilitate debugging
//
//---------------------------------------------------------------------------
SPQOS_RESULT
COstreamStringTest::EresUnittest()
{
	CUnittest rgut[] = {
		SPQOS_UNITTEST_FUNC(COstreamStringTest::EresUnittest_Basic),
#if defined(SPQOS_DEBUG) && defined(__GLIBCXX__)
		SPQOS_UNITTEST_FUNC_ASSERT(COstreamStringTest::EresUnittest_EndlAssert),
#endif
	};

	return CUnittest::EresExecute(rgut, SPQOS_ARRAY_SIZE(rgut));
}


//---------------------------------------------------------------------------
//	@function:
//		COstreamStringTest::EresUnittest_Basic
//
//	@doc:
//		test for non-string/non-number types;
//
//---------------------------------------------------------------------------
SPQOS_RESULT
COstreamStringTest::EresUnittest_Basic()
{
	// create memory pool of 128KB
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	CWStringDynamic str(mp);

	// define basic stream over wide char out stream
	COstreamString osb(&str);

	// non-string, non-number types
	WCHAR wc = 'W';
	CHAR c = 'C';
	ULONG ul = 102;
	INT i = -10;
	WCHAR wc_array[] = SPQOS_WSZ_LIT("some regular string");
	INT hex = 0xdeadbeef;

	osb << wc << c << ul << i << wc_array << COstream::EsmHex << hex;

	CWStringConst sexp(SPQOS_WSZ_LIT("WC102-10some regular stringdeadbeef"));

	SPQOS_ASSERT(str.Equals(&sexp) &&
				"Constructed string does not match expected output");

	return SPQOS_OK;
}

#if defined(SPQOS_DEBUG) && defined(__GLIBCXX__)
//---------------------------------------------------------------------------
//	@function:
//		COstreamStringTest::EresUnittest_EndlAssert
//
//	@doc:
//		Only allow std::endl
//
//---------------------------------------------------------------------------
SPQOS_RESULT
COstreamStringTest::EresUnittest_EndlAssert()
{
	// create memory pool of 1KB
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	CWStringDynamic str(mp);

	// define basic stream over wide char out stream
	COstreamString osb(&str);

	osb << std::ends;

	return SPQOS_FAILED;
}
#endif

// EOF
