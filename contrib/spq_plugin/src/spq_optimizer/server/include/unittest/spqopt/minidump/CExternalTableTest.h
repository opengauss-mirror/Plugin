//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CExternalTableTest.h
//
//	@doc:
//		Test for external tables
//---------------------------------------------------------------------------
#ifndef SPQOPT_CExternalTableTest_H
#define SPQOPT_CExternalTableTest_H

#include "spqos/base.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CExternalTableTest
//
//	@doc:
//		Unittests
//
//---------------------------------------------------------------------------
class CExternalTableTest
{
private:
	// counter used to mark last successful test
	static ULONG m_ulTestCounter;

public:
	// unittests
	static SPQOS_RESULT EresUnittest();

	static SPQOS_RESULT EresUnittest_RunMinidumpTests();
};	// class CExternalTableTest
}  // namespace spqopt

#endif	// !SPQOPT_CExternalTableTest_H

// EOF
