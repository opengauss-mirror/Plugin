//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2008 Greenplum, Inc.
//
//	@filename:
//		CAutoRgTest.h
//
//	@doc:
//		Test for basic auto range implementation
//---------------------------------------------------------------------------
#ifndef SPQOS_CAutoRgTest_H
#define SPQOS_CAutoRgTest_H

#include "spqos/base.h"

namespace spqos
{
//---------------------------------------------------------------------------
//	@class:
//		CAutoRgTest
//
//	@doc:
//		Static unit tests for auto range
//
//---------------------------------------------------------------------------
class CAutoRgTest
{
public:
	// unittests
	static SPQOS_RESULT EresUnittest();
	static SPQOS_RESULT EresUnittest_Basics();

};	// class CAutoRgTest

}  // namespace spqos

#endif	// !SPQOS_CAutoRgTest_H

// EOF
