//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		CColRefSetIterTest.h
//
//	@doc:
//		Tests for CColRefSetIter
//---------------------------------------------------------------------------
#ifndef SPQOS_CColRefSetIterTest_H
#define SPQOS_CColRefSetIterTest_H

#include "spqos/base.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CColRefSetIterTest
//
//	@doc:
//		Static unit tests for col ref set
//
//---------------------------------------------------------------------------
class CColRefSetIterTest
{
public:
	// unittests
	static SPQOS_RESULT EresUnittest();
	static SPQOS_RESULT EresUnittest_Basics();

};	// class CColRefSetIterTest
}  // namespace spqopt

#endif	// !SPQOS_CColRefSetIterTest_H


// EOF
