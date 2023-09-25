//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		CColRefSetTest.h
//
//	@doc:
//	    Test for CColRefSet
//---------------------------------------------------------------------------
#ifndef SPQOS_CColRefSetTest_H
#define SPQOS_CColRefSetTest_H

#include "spqos/base.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CColRefSetTest
//
//	@doc:
//		Static unit tests for column reference set
//
//---------------------------------------------------------------------------
class CColRefSetTest
{
public:
	// unittests
	static SPQOS_RESULT EresUnittest();
	static SPQOS_RESULT EresUnittest_Basics();

};	// class CColRefSetTest
}  // namespace spqopt

#endif	// !SPQOS_CColRefSetTest_H


// EOF
