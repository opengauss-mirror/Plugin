//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CContradictionTest.h
//
//	@doc:
//		Test for contradiction detection
//---------------------------------------------------------------------------
#ifndef SPQOPT_CContradictionTest_H
#define SPQOPT_CContradictionTest_H

#include "spqos/base.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CContradictionTest
//
//	@doc:
//		Unittests
//
//---------------------------------------------------------------------------
class CContradictionTest
{
public:
	// unittests
	static SPQOS_RESULT EresUnittest();
	static SPQOS_RESULT EresUnittest_Constraint();

};	// class CContradictionTest
}  // namespace spqopt

#endif	// !SPQOPT_CContradictionTest_H

// EOF
