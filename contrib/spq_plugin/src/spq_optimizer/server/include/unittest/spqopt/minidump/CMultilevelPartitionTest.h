//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2015 Pivotal, Inc.
//
//	@filename:
//		CMultilevelPartitionTest.h
//
//	@doc:
//		Test for optimizing queries on multilevel partitioned tables
//---------------------------------------------------------------------------
#ifndef SPQOPT_CMultilevelPartitionTest_H
#define SPQOPT_CMultilevelPartitionTest_H

#include "spqos/base.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CMultilevelPartitionTest
//
//	@doc:
//		Unittests
//
//---------------------------------------------------------------------------
class CMultilevelPartitionTest
{
private:
	// counter used to mark last successful test
	static ULONG m_ulMLPTTestCounter;

public:
	// unittests
	static SPQOS_RESULT EresUnittest();

	static SPQOS_RESULT EresUnittest_RunTests();

};	// class CMultilevelPartitionTest
}  // namespace spqopt

#endif	// !SPQOPT_CMultilevelPartitionTest_H

// EOF
