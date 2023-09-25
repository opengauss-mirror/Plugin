//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2018 Pivotal Inc.
//
//	@filename:
//		CPointTest.h
//
//	@doc:
//		Testing operations on points used to define histogram buckets
//---------------------------------------------------------------------------
#ifndef SPQNAUCRATES_CPointTest_H
#define SPQNAUCRATES_CPointTest_H

#include "spqos/base.h"

namespace spqnaucrates
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CPointTest
//
//	@doc:
//		Static unit tests for point
//
//---------------------------------------------------------------------------
class CPointTest
{
public:
	// unittests
	static SPQOS_RESULT EresUnittest();

	// point related tests
	static SPQOS_RESULT EresUnittest_CPointInt4();

	static SPQOS_RESULT EresUnittest_CPointBool();

};	// class CPointTest
}  // namespace spqnaucrates

#endif	// !SPQNAUCRATES_CPointTest_H


// EOF
