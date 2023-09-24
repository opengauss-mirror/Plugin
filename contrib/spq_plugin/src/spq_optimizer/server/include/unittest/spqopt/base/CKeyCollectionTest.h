//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 - 2012 EMC Corp.
//
//	@filename:
//		CKeyCollectionTest.h
//
//	@doc:
//		Test for key collection functionality
//---------------------------------------------------------------------------
#ifndef SPQOS_CKeyCollectionTest_H
#define SPQOS_CKeyCollectionTest_H

#include "spqos/base.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CKeyCollectionTest
//
//	@doc:
//		Static unit tests for key collections
//
//---------------------------------------------------------------------------
class CKeyCollectionTest
{
public:
	// unittests
	static SPQOS_RESULT EresUnittest();
	static SPQOS_RESULT EresUnittest_Basics();
	static SPQOS_RESULT EresUnittest_Subsumes();
};	// class CKeyCollectionTest
}  // namespace spqopt

#endif	// !SPQOS_CKeyCollectionTest_H


// EOF
