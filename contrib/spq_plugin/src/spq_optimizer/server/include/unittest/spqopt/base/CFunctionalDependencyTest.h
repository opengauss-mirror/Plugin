//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CFunctionalDependencyTest.h
//
//	@doc:
//		Test for functional dependencies
//---------------------------------------------------------------------------
#ifndef SPQOPT_CFunctionalDependencyTest_H
#define SPQOPT_CFunctionalDependencyTest_H

#include "spqos/base.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CFunctionalDependencyTest
//
//	@doc:
//		Static unit tests for functional dependencies
//
//---------------------------------------------------------------------------
class CFunctionalDependencyTest
{
public:
	// unittests
	static SPQOS_RESULT EresUnittest();
	static SPQOS_RESULT EresUnittest_Basics();

};	// class CFunctionalDependencyTest
}  // namespace spqopt

#endif	// !SPQOPT_CFunctionalDependencyTest_H


// EOF
