//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 Greenplum, Inc.
//
//	@filename:
//		CMDProviderTest.h
//
//	@doc:
//		Tests the metadata provider.
//---------------------------------------------------------------------------


#ifndef SPQOPT_CMDProviderTest_H
#define SPQOPT_CMDProviderTest_H

#include "spqos/base.h"

#include "naucrates/md/IMDProvider.h"

namespace spqdxl
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CMDProviderTest
//
//	@doc:
//		Static unit tests
//
//---------------------------------------------------------------------------

class CMDProviderTest
{
private:
	// test lookup of MD objects with given MD provider
	static void TestMDLookup(CMemoryPool *mp, IMDProvider *pmdp);

public:
	// file for the file-based MD provider
	static const CHAR *file_name;

	// unittests
	static SPQOS_RESULT EresUnittest();
	static SPQOS_RESULT EresUnittest_Basic();
	static SPQOS_RESULT EresUnittest_Stats();
	static SPQOS_RESULT EresUnittest_Negative();


};	// class CMDProviderTest
}  // namespace spqdxl

#endif	// !SPQOPT_CMDProviderTest_H

// EOF
