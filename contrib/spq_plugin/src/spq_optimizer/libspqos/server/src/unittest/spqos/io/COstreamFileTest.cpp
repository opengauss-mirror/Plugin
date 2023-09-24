//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		COstreamFileTest.cpp
//
//	@doc:
//		Tests for COstreamFile
//---------------------------------------------------------------------------

#include "unittest/spqos/io/COstreamFileTest.h"

#include "spqos/io/CFileReader.h"
#include "spqos/io/COstreamFile.h"
#include "spqos/io/ioutils.h"
#include "spqos/string/CStringStatic.h"
#include "spqos/string/CWStringConst.h"
#include "spqos/string/CWStringStatic.h"
#include "spqos/task/CAutoTraceFlag.h"
#include "spqos/test/CUnittest.h"

using namespace spqos;


//---------------------------------------------------------------------------
//	@function:
//		COstreamFileTest::EresUnittest
//
//	@doc:
//		Function for raising assert exceptions; again, encapsulated in a function
//		to facilitate debugging
//
//---------------------------------------------------------------------------
SPQOS_RESULT
COstreamFileTest::EresUnittest()
{
	CUnittest rgut[] = {
		SPQOS_UNITTEST_FUNC(COstreamFileTest::EresUnittest_Basic),
	};

	return CUnittest::EresExecute(rgut, SPQOS_ARRAY_SIZE(rgut));
}


//---------------------------------------------------------------------------
//	@function:
//		COstreamFileTest::EresUnittest_Basic
//
//	@doc:
//		Basic test for writing to output file stream;
//
//---------------------------------------------------------------------------
SPQOS_RESULT
COstreamFileTest::EresUnittest_Basic()
{
	// create temporary file in new directory under /tmp
	CHAR file_path[SPQOS_FILE_NAME_BUF_SIZE];
	CHAR szFile[SPQOS_FILE_NAME_BUF_SIZE];

	CStringStatic strPath(file_path, SPQOS_ARRAY_SIZE(file_path));
	CStringStatic strFile(szFile, SPQOS_ARRAY_SIZE(szFile));

	strPath.AppendBuffer("/tmp/spqos_test_stream.XXXXXX");

	// create dir
	(void) ioutils::CreateTempDir(file_path);

	strFile.Append(&strPath);
	strFile.AppendBuffer("/COstreamFileTest");

	SPQOS_TRY
	{
		Unittest_WriteFileStream(strFile.Buffer());

		Unittest_CheckOutputFile(strFile.Buffer());
	}
	SPQOS_CATCH_EX(ex)
	{
		Unittest_DeleteTmpFile(strPath.Buffer(), strFile.Buffer());

		SPQOS_RETHROW(ex);
	}
	SPQOS_CATCH_END;

	Unittest_DeleteTmpFile(strPath.Buffer(), strFile.Buffer());

	return SPQOS_OK;
}


//---------------------------------------------------------------------------
//	@function:
//		COstreamFileTest::Unittest_WriteFileStream
//
//	@doc:
//		Write to output file stream
//
//---------------------------------------------------------------------------
void
COstreamFileTest::Unittest_WriteFileStream(const CHAR *szFile)
{
	SPQOS_ASSERT(NULL != szFile);

	COstreamFile osf(szFile);

	const WCHAR wc = 'W';
	const CHAR c = 'C';
	const ULLONG ull = 102;
	const LINT li = -10;
	const WCHAR wsz[] = SPQOS_WSZ_LIT("some regular string");
	const INT hex = 0xdeadbeef;

	osf << wc << c << ull << li << wsz << COstream::EsmHex << hex;
}


//---------------------------------------------------------------------------
//	@function:
//		COstreamFileTest::Unittest_CheckOutputFile
//
//	@doc:
//		Check the contents of the file used by the output stream
//
//---------------------------------------------------------------------------
void
COstreamFileTest::Unittest_CheckOutputFile(const CHAR *szFile)
{
	SPQOS_ASSERT(NULL != szFile);

	CFileReader fr;
	fr.Open(szFile);

	const ULONG ulReadBufferSize = 1024;
	WCHAR wszReadBuffer[ulReadBufferSize];

#ifdef SPQOS_DEBUG
	ULONG_PTR ulpRead =
#endif	// SPQOS_DEBUG
		fr.ReadBytesToBuffer((BYTE *) wszReadBuffer,
							 SPQOS_ARRAY_SIZE(wszReadBuffer));

	CWStringConst strExpected(
		SPQOS_WSZ_LIT("WC102-10some regular stringdeadbeef"));

	SPQOS_ASSERT(ulpRead ==
				(ULONG_PTR) strExpected.Length() * SPQOS_SIZEOF(WCHAR));
	SPQOS_ASSERT(strExpected.Equals(&strExpected));
}


//---------------------------------------------------------------------------
//	@function:
//		COstreamFileTest::Unittest_DeleteTmpFile
//
//	@doc:
//		Delete temporary file;
//
//---------------------------------------------------------------------------
void
COstreamFileTest::Unittest_DeleteTmpFile(const CHAR *szDir, const CHAR *szFile)
{
	SPQOS_ASSERT(NULL != szDir);
	SPQOS_ASSERT(NULL != szFile);

	CAutoTraceFlag atf(EtraceSimulateIOError, false);

	if (ioutils::PathExists(szFile))
	{
		// delete temporary file
		ioutils::Unlink(szFile);
	}

	if (ioutils::PathExists(szDir))
	{
		// delete temporary dir
		ioutils::RemoveDir(szDir);
	}
}


// EOF
