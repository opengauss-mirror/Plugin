//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2016 Pivotal, Inc.
//
//	@filename:
//		CPruneColumnsTest.h
//
//	@doc:
//		Test for optimizing queries where intermediate columns are pruned
//---------------------------------------------------------------------------
#ifndef SPQOPT_CPruneColumnsTest_H
#define SPQOPT_CPruneColumnsTest_H

#include "spqos/base.h"

namespace spqopt
{
//---------------------------------------------------------------------------
//	@class:
//		CPruneColumnsTest
//
//	@doc:
//		Unittests
//
//---------------------------------------------------------------------------
class CPruneColumnsTest
{
private:
	// counter used to mark last successful test
	static spqos::ULONG m_ulPruneColumnsTestCounter;

public:
	// unittests
	static spqos::SPQOS_RESULT EresUnittest();

	static spqos::SPQOS_RESULT EresUnittest_RunTests();

};	// class CPruneColumnsTest
}  // namespace spqopt

#endif	// !SPQOPT_CPruneColumnsTest_H

// EOF
