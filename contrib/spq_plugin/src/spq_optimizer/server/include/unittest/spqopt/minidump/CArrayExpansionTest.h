//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2016 Pivotal Software
//
//	@filename:
//		CArrayExpansionTest.h
//
//	@doc:
//		Test for Array expansion in WHERE clause
//
//
//---------------------------------------------------------------------------
#ifndef SPQOPT_CArrayExpansionTest_H
#define SPQOPT_CArrayExpansionTest_H

#include "spqos/base.h"

namespace spqopt
{
class CArrayExpansionTest
{
public:
	// unittests
	static spqos::SPQOS_RESULT EresUnittest();
};	// class CArrayExpansionTest
}  // namespace spqopt

#endif	// !SPQOPT_CArrayExpansionTest_H

// EOF
