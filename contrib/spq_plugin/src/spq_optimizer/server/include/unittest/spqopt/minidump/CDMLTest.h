//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2015 Pivotal, Inc.
//
//	@filename:
//		CDMLTest.h
//
//	@doc:
//		Test for optimizing DML queries
//---------------------------------------------------------------------------
#ifndef SPQOPT_CDMLTest_H
#define SPQOPT_CDMLTest_H

#include "spqos/base.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CDMLTest
//
//	@doc:
//		Unittests
//
//---------------------------------------------------------------------------
class CDMLTest
{
private:
	// counter used to mark last successful test
	static ULONG m_ulDMLTestCounter;

public:
	// unittests
	static SPQOS_RESULT EresUnittest();

	static SPQOS_RESULT EresUnittest_RunTests();

};	// class CDMLTest
}  // namespace spqopt

#endif	// !SPQOPT_CDMLTest_H

// EOF
