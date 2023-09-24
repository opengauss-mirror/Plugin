//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		COrderSpecTest.h
//
//	@doc:
//		Test for order spec
//---------------------------------------------------------------------------
#ifndef SPQOS_COrderSpecTest_H
#define SPQOS_COrderSpecTest_H

#include "spqos/base.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		COrderSpecTest
//
//	@doc:
//		Static unit tests for order specs
//
//---------------------------------------------------------------------------
class COrderSpecTest
{
public:
	// unittests
	static SPQOS_RESULT EresUnittest();
	static SPQOS_RESULT EresUnittest_Basics();

};	// class COrderSpecTest
}  // namespace spqopt

#endif	// !SPQOS_COrderSpecTest_H


// EOF
