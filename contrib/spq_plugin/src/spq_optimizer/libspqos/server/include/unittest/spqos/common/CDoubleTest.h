//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CDoubleTest.h
//
//	@doc:
//		Tests for the floating-point wrapper class.
//---------------------------------------------------------------------------
#ifndef SPQOS_CDoubleTest_H
#define SPQOS_CDoubleTest_H

#include "spqos/base.h"

namespace spqos
{
//---------------------------------------------------------------------------
//	@class:
//		CDoubleTest
//
//	@doc:
//		Unittests for floating-point class
//
//---------------------------------------------------------------------------
class CDoubleTest
{
public:
	// unittests
	static SPQOS_RESULT EresUnittest();
	static SPQOS_RESULT EresUnittest_Arithmetic();
	static SPQOS_RESULT EresUnittest_Bool();
	static SPQOS_RESULT EresUnittest_Convert();
	static SPQOS_RESULT EresUnittest_Limits();

};	// class CDoubleTest
}  // namespace spqos

#endif	// !SPQOS_CDoubleTest_H

// EOF
