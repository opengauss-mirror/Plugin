//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2016 Pivotal, Inc.
//
//	@filename:
//		CEscapeMechanismTest.h
//
//	@doc:
//		Test for optimizing queries for exploring fewer alternatives and
//		run optimization process faster
//---------------------------------------------------------------------------
#ifndef SPQOPT_CEscapeMechanismTest_H
#define SPQOPT_CEscapeMechanismTest_H

#include "spqos/base.h"

namespace spqopt
{
//---------------------------------------------------------------------------
//	@class:
//		CEscapeMechanismTest
//
//	@doc:
//		Unittests
//
//---------------------------------------------------------------------------
class CEscapeMechanismTest
{
private:
	// counter used to mark last successful test
	static spqos::ULONG m_ulEscapeMechanismTestCounter;

public:
	// unittests
	static spqos::SPQOS_RESULT EresUnittest();

	static spqos::SPQOS_RESULT EresUnittest_RunTests();

};	// class CEscapeMechanismTest
}  // namespace spqopt

#endif	// !SPQOPT_CEscapeMechanismTest_H

// EOF
