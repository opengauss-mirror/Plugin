//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2010 Greenplum, Inc.
//
//	@filename:
//		CParseHandlerManagerTest.h
//
//	@doc:
//		Tests parse handler manager, responsible for stacking up parse handlers
//		during DXL parsing.
//---------------------------------------------------------------------------


#ifndef SPQOPT_CParseHandlerManagerTest_H
#define SPQOPT_CParseHandlerManagerTest_H

#include "spqos/base.h"


namespace spqdxl
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CParseHandlerManagerTest
//
//	@doc:
//		Static unit tests
//
//---------------------------------------------------------------------------

class CParseHandlerManagerTest
{
public:
	// unittests
	static SPQOS_RESULT EresUnittest();
	static SPQOS_RESULT EresUnittest_Basic();

};	// class CParseHandlerManagerTest
}  // namespace spqdxl

#endif	// SPQOPT_CParseHandlerManagerTest_H

// EOF
