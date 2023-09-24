//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		COstreamFileTest.h
//
//	@doc:
//		Test for COstreamFile
//---------------------------------------------------------------------------
#ifndef SPQOS_COstreamFileTest_H
#define SPQOS_COstreamFileTest_H

#include "spqos/base.h"

namespace spqos
{
//---------------------------------------------------------------------------
//	@class:
//		COstreamFileTest
//
//	@doc:
//		Static unit tests for messages
//
//---------------------------------------------------------------------------
class COstreamFileTest
{
private:
	// write to output file stream
	static void Unittest_WriteFileStream(const CHAR *szFile);

	// check file stream for correctness
	static void Unittest_CheckOutputFile(const CHAR *szFile);

	// delete temporary file and containing directory
	static void Unittest_DeleteTmpFile(const CHAR *szDir, const CHAR *szFile);

public:
	// unittests
	static SPQOS_RESULT EresUnittest();
	static SPQOS_RESULT EresUnittest_Basic();
};
}  // namespace spqos

#endif	// !SPQOS_COstreamFileTest_H

// EOF
