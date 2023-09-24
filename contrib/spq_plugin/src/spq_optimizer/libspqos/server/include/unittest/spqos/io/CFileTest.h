//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CFileTest.h
//
//	@doc:
//		Tests for CFileWriter and CFileReader
//---------------------------------------------------------------------------

#ifndef SPQOS_CFileTest_H
#define SPQOS_CFileTest_H

#include "spqos/base.h"
#include "spqos/io/CFileReader.h"
#include "spqos/io/CFileWriter.h"
#include "spqos/io/ioutils.h"

namespace spqos
{
//---------------------------------------------------------------------------
//	@class:
//		CFileTest
//
//	@doc:
//		Static unit tests for CFileWriter and CFileReader
//
//---------------------------------------------------------------------------
class CFileTest
{
public:
	// help CFileWriter get file size by descriptor
	class CFileWriterInternal : public CFileWriter
	{
	public:
		// get file size by descriptor
		ULLONG UllSizeInternal() const;

		// dtor
		virtual ~CFileWriterInternal();


	};	// class CFileWriterInternal

	// unittests
	static SPQOS_RESULT EresUnittest();
	static SPQOS_RESULT EresUnittest_FileContent();
	static SPQOS_RESULT EresUnittest_Invalid();
	static SPQOS_RESULT EresUnittest_InconsistentSize();

	static void Unittest_MkTmpFile(CHAR *szTmpDir, CHAR *szTmpFile);
	static void Unittest_DeleteTmpDir(const CHAR *szDir, const CHAR *szFile);

	template <typename T, typename R, typename ARG1, typename ARG2>
	static void Unittest_CheckError(T *pt, R (T::*pfunc)(ARG1, ARG2),
									ARG1 argFirst, ARG2 argSec,
									CException::ExMinor exmi);

	static void Unittest_WriteInconsistentSize(const CHAR *szTmpFile);
	static void Unittest_ReadInconsistentSize(const CHAR *szTmpFile);

};	// class CFileTest

}  // namespace spqos

#endif	// !SPQOS_CFileTest_H

// EOF
