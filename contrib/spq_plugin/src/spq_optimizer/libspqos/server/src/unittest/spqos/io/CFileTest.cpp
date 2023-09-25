//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CFileTest.cpp
//
//	@doc:
//		Tests for CFileWriter and CFileReader
//---------------------------------------------------------------------------

#include "unittest/spqos/io/CFileTest.h"

#include "spqos/assert.h"
#include "spqos/error/CMessage.h"
#include "spqos/io/CFileReader.h"
#include "spqos/io/CFileWriter.h"
#include "spqos/io/ioutils.h"
#include "spqos/string/CStringStatic.h"
#include "spqos/task/CAutoTraceFlag.h"
#include "spqos/test/CUnittest.h"

using namespace spqos;


//---------------------------------------------------------------------------
//	@function:
//		CFileTest::EresUnittest
//
//	@doc:
//		Driver for unittests
//---------------------------------------------------------------------------
SPQOS_RESULT
CFileTest::EresUnittest()
{
	CUnittest rgut[] = {
		SPQOS_UNITTEST_FUNC(CFileTest::EresUnittest_Invalid),
		SPQOS_UNITTEST_FUNC(CFileTest::EresUnittest_FileContent),
		SPQOS_UNITTEST_FUNC(CFileTest::EresUnittest_InconsistentSize),
	};

	return CUnittest::EresExecute(rgut, SPQOS_ARRAY_SIZE(rgut));
}


//---------------------------------------------------------------------------
//	@function:
//		CFileTest::EresUnittest_Invalid
//
//	@doc:
//		Test for open files with invalid operations
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CFileTest::EresUnittest_Invalid()
{
	CHAR szTmpDir[SPQOS_FILE_NAME_BUF_SIZE];
	CHAR szTmpFile[SPQOS_FILE_NAME_BUF_SIZE];

	const ULONG ulWrPerms = S_IWUSR | S_IRGRP | S_IROTH;
	const ULONG ulRdPerms = S_IRUSR | S_IRGRP | S_IROTH;


	// make a unique temporary file name
	Unittest_MkTmpFile(szTmpDir, szTmpFile);

	SPQOS_TRY
	{
		// open a nonexistent file by CFileReader,
		// it will catch an exception
		CFileReader rd1;
		Unittest_CheckError(&rd1, &CFileReader::Open, (const CHAR *) szTmpFile,
							ulRdPerms, CException::ExmiIOError);

		// create, or truncate the file only permit to write
		CFileWriter wr;
		wr.Open(szTmpFile, ulWrPerms);

		// close file
		wr.Close();

		// open a write only file by CFileReader,
		// it will catch an exception
		CFileReader rd2;
		Unittest_CheckError(&rd2, &CFileReader::Open, (const CHAR *) szTmpFile,
							ulRdPerms, CException::ExmiIOError);
	}
	SPQOS_CATCH_EX(ex)
	{
		// delete tmp file and dir
		Unittest_DeleteTmpDir(szTmpDir, szTmpFile);

		SPQOS_RETHROW(ex);
	}
	SPQOS_CATCH_END;

	// delete tmp file and dir
	Unittest_DeleteTmpDir(szTmpDir, szTmpFile);

	return SPQOS_OK;
}


//---------------------------------------------------------------------------
//	@function:
//		CFileTest::EresUnittest_FileContent
//
//	@doc:
//		Test for write data, and read the written data
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CFileTest::EresUnittest_FileContent()
{
	CHAR szTmpDir[SPQOS_FILE_NAME_BUF_SIZE];
	CHAR szTmpFile[SPQOS_FILE_NAME_BUF_SIZE];

	const ULONG ulWrPerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
	const ULONG ulRdPerms = S_IRUSR | S_IRGRP | S_IROTH;
	const CHAR szData[] = "Test file content for CFileTest_file\n";
	const INT iLineNum = 100;
	const ULONG ulLineLength = SPQOS_ARRAY_SIZE(szData);

	CFileReader rd;
	CFileWriterInternal wr;

	// make a unique temporary file name
	Unittest_MkTmpFile(szTmpDir, szTmpFile);

	SPQOS_TRY
	{
		// open file for writing by CFileWriter
		wr.Open(szTmpFile, ulWrPerms);

		// write file line by line
		for (INT i = 0; i < iLineNum; i++)
		{
			wr.Write((BYTE *) szData, ulLineLength);
		}

		// test file size
		SPQOS_ASSERT((wr.FileSize() == (ulLineLength * iLineNum)) &&
					(wr.UllSizeInternal() == wr.FileSize()));

		// close file
		wr.Close();

		// read data written to file above line by line
		rd.Open(szTmpFile, ulRdPerms);

		CHAR szRdBuf[ulLineLength];
		CHAR szRdData[ulLineLength];
#ifdef SPQOS_DEBUG
		ULONG_PTR ulpRdLen = 0;
#endif	// SPQOS_DEBUG
		CStringStatic strRdData(szRdData, SPQOS_ARRAY_SIZE(szRdData));

		// read file content line by line,
		// and test if it is equal to what is written
		for (INT i = 0; i < iLineNum; i++)
		{
#ifdef SPQOS_DEBUG
			ulpRdLen = rd.ReadBytesToBuffer((BYTE *) szRdBuf, ulLineLength);
#endif	// SPQOS_DEBUG
			strRdData.AppendBuffer((const CHAR *) szRdBuf);

			SPQOS_ASSERT(strRdData.Equals((CHAR *) szData));
			SPQOS_ASSERT(ulpRdLen == ulLineLength);

			strRdData.Reset();
		}

		SPQOS_ASSERT(rd.FileReadSize() == (ulLineLength * iLineNum));

		// when the EOF is reached, read again will return zero
		SPQOS_ASSERT(0 == rd.ReadBytesToBuffer((BYTE *) szRdBuf, ulLineLength));

		//close file
		rd.Close();
	}
	SPQOS_CATCH_EX(ex)
	{
		// delete tmp file and dir
		Unittest_DeleteTmpDir(szTmpDir, szTmpFile);

		SPQOS_RETHROW(ex);
	}
	SPQOS_CATCH_END;

	// delete temporary file and dir
	Unittest_DeleteTmpDir(szTmpDir, szTmpFile);

	return SPQOS_OK;
}


//---------------------------------------------------------------------------
//	@function:
//		CFileTest::EresUnittest_InconsistentSize
//
//	@doc:
//		Test for write and read file with given length
//		not equal to buffer size
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CFileTest::EresUnittest_InconsistentSize()
{
	CHAR szTmpDir[SPQOS_FILE_NAME_BUF_SIZE];
	CHAR szTmpFile[SPQOS_FILE_NAME_BUF_SIZE];

	// make a unique temporary file name
	Unittest_MkTmpFile(szTmpDir, szTmpFile);

	SPQOS_TRY
	{
		// write data with inconsistent size with write buffer
		Unittest_WriteInconsistentSize(szTmpFile);

		// read data with inconsistent size with read buffer
		Unittest_ReadInconsistentSize(szTmpFile);
	}
	SPQOS_CATCH_EX(ex)
	{
		// delete tmp file and dir
		Unittest_DeleteTmpDir(szTmpDir, szTmpFile);

		SPQOS_RETHROW(ex);
	}
	SPQOS_CATCH_END;

	// delete tmp file and dir
	Unittest_DeleteTmpDir(szTmpDir, szTmpFile);

	return SPQOS_OK;
}


//---------------------------------------------------------------------------
//	@function:
//		CFileTest::Unittest_MkTmpFile
//
//	@doc:
//		Make a unique temporary dir and file
//
//---------------------------------------------------------------------------
void
CFileTest::Unittest_MkTmpFile(CHAR *szTmpDir, CHAR *szTmpFile)
{
	SPQOS_ASSERT(NULL != szTmpDir);
	SPQOS_ASSERT(NULL != szTmpFile);

	CStringStatic strTmpDir(szTmpDir, SPQOS_FILE_NAME_BUF_SIZE);
	CStringStatic strTmpFile(szTmpFile, SPQOS_FILE_NAME_BUF_SIZE);

	const CHAR szDir[SPQOS_FILE_NAME_BUF_SIZE] = "/tmp/CFileTest.XXXXXX";
	const CHAR szFile[SPQOS_FILE_NAME_BUF_SIZE] = "/CFileTest_file.txt";

	// create unique temporary directory name under /tmp
	strTmpDir.AppendFormat(szDir);
	ioutils::CreateTempDir(szTmpDir);

	// unique temporary file name
	strTmpFile.AppendFormat(szTmpDir);
	strTmpFile.AppendFormat(szFile);
}


//---------------------------------------------------------------------------
//	@function:
//		CFileTest::Unittest_DeleteTmpDir
//
//	@doc:
//		Delete test files and directory
//
//---------------------------------------------------------------------------
void
CFileTest::Unittest_DeleteTmpDir(const CHAR *szDir, const CHAR *szFile)
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


//---------------------------------------------------------------------------
//	@function:
//		CFileTest::Unittest_CheckError
//
//	@doc:
//		Check error for write, read and open
//
//---------------------------------------------------------------------------
template <typename T, typename R, typename ARG1, typename ARG2>
void
CFileTest::Unittest_CheckError(T *pt, R (T::*pfunc)(ARG1, ARG2), ARG1 argFirst,
							   ARG2 argSec, CException::ExMinor exmi)
{
	SPQOS_ASSERT(NULL != pt);
	SPQOS_ASSERT(NULL != pfunc);

	SPQOS_TRY
	{
		(pt->*pfunc)(argFirst, argSec);
	}
	SPQOS_CATCH_EX(ex)
	{
		if (!SPQOS_MATCH_EX(ex, CException::ExmaSystem, (ULONG) exmi))
		{
			SPQOS_RETHROW(ex);
		}

		SPQOS_RESET_EX;
	}
	SPQOS_CATCH_END;
}


//---------------------------------------------------------------------------
//	@function:
//		CFileTest::Unittest_WriteInconsistentSize
//
//	@doc:
//		Write data with inconsistent size with write buffer
//
//---------------------------------------------------------------------------
void
CFileTest::Unittest_WriteInconsistentSize(const CHAR *szTmpFile)
{
	SPQOS_ASSERT(NULL != szTmpFile);

	const ULONG ulWdPerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
	const BYTE szData[] = "Test file content for CFileTest_file\n";
	const ULONG length = SPQOS_ARRAY_SIZE(szData);
	const ULONG ulShortfall = 5;

	ULONG ulExpectSize = length - ulShortfall;

	CFileWriterInternal wr;

	wr.Open(szTmpFile, ulWdPerms);

	// try to write data with length smaller than buffer size
	wr.Write(szData, ulExpectSize);

	CFileWriterInternal wrInternal;

	// test file size
	SPQOS_ASSERT(ulExpectSize == wr.FileSize());
	SPQOS_ASSERT(ulExpectSize == wr.UllSizeInternal());

	// try to write data with length larger than buffer size
	ulExpectSize += (length + ulShortfall);
	wr.Write(szData, length + ulShortfall);

	// test file size
	SPQOS_ASSERT(ulExpectSize == wr.FileSize());
	SPQOS_ASSERT(wr.UllSizeInternal() == wr.FileSize());

	wr.Close();
}


//---------------------------------------------------------------------------
//	@function:
//		CFileTest::Unittest_ReadInconsistentSize
//
//	@doc:
//		Read data with inconsistent size with read buffer
//
//---------------------------------------------------------------------------
void
CFileTest::Unittest_ReadInconsistentSize(const CHAR *szTmpFile)
{
	SPQOS_ASSERT(NULL != szTmpFile);

	const ULONG ulWrPerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
	const ULONG ulRdPerms = S_IRUSR | S_IRGRP | S_IROTH;
	const BYTE szData[] = "Test file content for CFileTest_file\n";
	const ULONG length = SPQOS_ARRAY_SIZE(szData);
	const ULONG ulShortfall = 5;

	CFileWriter wr;

	// truncate file to zero, an then write specific data for reading
	wr.Open(szTmpFile, ulWrPerms);

	// write two lines
	wr.Write(szData, length);
	wr.Write(szData, length);

	// close file
	wr.Close();

	CFileReader rd;
	// read buffer is oversized, because below we attempt to read past the end of
	// the file
	CHAR szRdBuf[length + ulShortfall];

	// open file by reader
	rd.Open(szTmpFile, ulRdPerms);

	// read data with length smaller than read buffer size
#ifdef SPQOS_DEBUG
	ULONG_PTR ulpRdSize =
		rd.ReadBytesToBuffer((BYTE *) szRdBuf, length - ulShortfall);
#endif	// SPQOS_DEBUG
	szRdBuf[length - ulShortfall] = CHAR_EOS;

	CHAR strBuf[length];
	CStringStatic strRdData(strBuf, SPQOS_ARRAY_SIZE(strBuf));
	strRdData.AppendBuffer((const CHAR *) szRdBuf);

	CHAR szExpectData[] = "Test file content for CFileTest_f";
#ifdef SPQOS_DEBUG
	ULONG ulExpectSize = length - ulShortfall;
#endif	// SPQOS_DEBUG

	// test read data, and read size
	SPQOS_ASSERT(ulExpectSize == ulpRdSize);
	SPQOS_ASSERT(ulExpectSize == rd.FileReadSize());

#ifdef SPQOS_DEBUG
	BOOL fEqual =
#endif	// SPQOS_DEBUG
		strRdData.Equals(szExpectData);

	SPQOS_ASSERT(fEqual);

	// close file
	rd.Close();

	// try to read more data than the read buffer size
	rd.Open(szTmpFile, ulRdPerms);
#ifdef SPQOS_DEBUG
	ulExpectSize = length + ulShortfall;
	ulpRdSize = rd.ReadBytesToBuffer((BYTE *) szRdBuf, ulExpectSize);
#endif	// SPQOS_DEBUG

	strRdData.Reset();
	strRdData.AppendBuffer((const CHAR *) szRdBuf);

	// test read size, and read data
	SPQOS_ASSERT(ulpRdSize == ulExpectSize);
	SPQOS_ASSERT(strRdData.Equals((const CHAR *) szData));

	// close file
	rd.Close();
}


//---------------------------------------------------------------------------
//	@function:
//		CFileTest::CFileWriterInternal::UllSizeInternal
//
//	@doc:
//		Get file size by descriptor
//
//---------------------------------------------------------------------------
ULLONG
CFileTest::CFileWriterInternal::UllSizeInternal() const
{
	return ioutils::FileSize(GetFileDescriptor());
}


//---------------------------------------------------------------------------
//	@function:
//		CFileTest::CFileWriterInternal:: ~CFileWriterInternal
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CFileTest::CFileWriterInternal::~CFileWriterInternal()
{
}

// EOF
