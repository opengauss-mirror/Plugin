//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2015 Pivotal, Inc.
//
//	@filename:
//		CTVFTest.h
//
//	@doc:
//		Test for optimizing queries with TVF
//---------------------------------------------------------------------------
#ifndef SPQOPT_CTVFTest_H
#define SPQOPT_CTVFTest_H

#include "spqos/base.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CTVFTest
//
//	@doc:
//		Unittests
//
//---------------------------------------------------------------------------
class CTVFTest
{
private:
	// counter used to mark last successful test
	static ULONG m_ulTVFTestCounter;

public:
	// unittests
	static SPQOS_RESULT EresUnittest();

	static SPQOS_RESULT EresUnittest_RunTests();

};	// class CTVFTest
}  // namespace spqopt

#endif	// !SPQOPT_CTVFTest_H

// EOF
