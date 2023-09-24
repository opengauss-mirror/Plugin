//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CBindingTest.h
//
//	@doc:
//		Test for checking bindings extracted for an expression
//---------------------------------------------------------------------------
#ifndef SPQOPT_CBindingTest_H
#define SPQOPT_CBindingTest_H

#include "spqos/base.h"


namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CBindingTest
//
//	@doc:
//		Unittests
//
//---------------------------------------------------------------------------
class CBindingTest
{
public:
	// unittests
	static SPQOS_RESULT EresUnittest();
	static SPQOS_RESULT EresUnittest_Basic();

};	// class CBindingTest
}  // namespace spqopt

#endif	// !SPQOPT_CBindingTest_H

// EOF
