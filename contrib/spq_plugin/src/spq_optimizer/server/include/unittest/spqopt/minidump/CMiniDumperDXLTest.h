//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CMiniDumperDXLTest.h
//
//	@doc:
//		Test for DXL-based minidumoers
//---------------------------------------------------------------------------
#ifndef SPQOPT_CMiniDumperDXLTest_H
#define SPQOPT_CMiniDumperDXLTest_H

#include "spqos/base.h"


namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CMiniDumperDXLTest
//
//	@doc:
//		Unittests
//
//---------------------------------------------------------------------------
class CMiniDumperDXLTest
{
public:
	// unittests
	static SPQOS_RESULT EresUnittest();
	static SPQOS_RESULT EresUnittest_Basic();
	static SPQOS_RESULT EresUnittest_Load();

};	// class CMiniDumperDXLTest
}  // namespace spqopt

#endif	// !SPQOPT_CMiniDumperDXLTest_H

// EOF
