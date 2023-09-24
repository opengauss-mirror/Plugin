//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2016 Pivotal Software
//
//	@filename:
//		CWindowTest.h
//
//	@doc:
//		Test for Window functions
//---------------------------------------------------------------------------
#ifndef SPQOPT_CWindowTest_H
#define SPQOPT_CWindowTest_H

#include "spqos/base.h"

namespace spqopt
{
using namespace spqos;

class CWindowTest
{
public:
	// unittests
	static SPQOS_RESULT EresUnittest();
};	// class CWindowTest
}  // namespace spqopt

#endif	// !SPQOPT_CWindowTest_H

// EOF
