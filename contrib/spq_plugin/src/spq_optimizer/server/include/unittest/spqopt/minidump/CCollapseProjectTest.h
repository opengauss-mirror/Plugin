//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2017 Pivotal, Inc.
//
//	@filename:
//		CCollapseProjectTest.h
//
//	@doc:
//		Test for optimizing queries with multiple project nodes
//---------------------------------------------------------------------------
#ifndef SPQOPT_CCollapseProjectTest_H
#define SPQOPT_CCollapseProjectTest_H

#include "spqos/base.h"

namespace spqopt
{
//---------------------------------------------------------------------------
//	@class:
//		CCollapseProjectTest
//
//	@doc:
//		Unittests
//
//---------------------------------------------------------------------------
class CCollapseProjectTest
{
private:
	// counter used to mark last successful test
	static spqos::ULONG m_ulCollapseProjectTestCounter;

public:
	// unittests
	static spqos::SPQOS_RESULT EresUnittest();

	static spqos::SPQOS_RESULT EresUnittest_RunTests();

};	// class CCollapseProjectTest
}  // namespace spqopt

#endif	// !SPQOPT_CCollapseProjectTest_H

// EOF
