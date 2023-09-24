//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2008 Greenplum, Inc.
//
//	@filename:
//		CAutoPTest.h
//
//	@doc:
//      Unit test for CAutoP
//---------------------------------------------------------------------------
#ifndef SPQOS_CAutoPTest_H
#define SPQOS_CAutoPTest_H

#include "spqos/base.h"

namespace spqos
{
//---------------------------------------------------------------------------
//	@class:
//		CAutoPTest
//
//	@doc:
//		Static unit tests for auto pointer
//
//---------------------------------------------------------------------------
class CAutoPTest
{
public:
	class CElem
	{
	public:
		ULONG m_ul;

	};	// class CElem


	// unittests
	static SPQOS_RESULT EresUnittest();
	static SPQOS_RESULT EresUnittest_Basics();

};	// class CAutoPTest

}  // namespace spqos

#endif	// !SPQOS_CAutoPTest_H

// EOF
