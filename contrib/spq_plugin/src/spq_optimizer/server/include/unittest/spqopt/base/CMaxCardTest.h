//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 - 2012 EMC Corp.
//
//	@filename:
//		CMaxCardTest.h
//
//	@doc:
//		Test for max card functionality
//---------------------------------------------------------------------------
#ifndef SPQOS_CMaxCardTest_H
#define SPQOS_CMaxCardTest_H

#include "spqos/base.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CMaxCardTest
//
//	@doc:
//		Static unit tests for max card computation
//
//---------------------------------------------------------------------------
class CMaxCardTest
{
public:
	// unittests
	static SPQOS_RESULT EresUnittest();
	static SPQOS_RESULT EresUnittest_Basics();
	static SPQOS_RESULT EresUnittest_RunMinidumpTests();

};	// class CMaxCardTest
}  // namespace spqopt

#endif	// !SPQOS_CMaxCardTest_H


// EOF
