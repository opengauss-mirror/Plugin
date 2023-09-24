//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		CAutoRefTest.h
//
//	@doc:
//		Unit Test for CAutoRef
//---------------------------------------------------------------------------
#ifndef SPQOS_CAutoRefTest_H
#define SPQOS_CAutoRefTest_H

#include "spqos/base.h"
#include "spqos/common/CAutoP.h"
#include "spqos/common/CRefCount.h"

namespace spqos
{
//---------------------------------------------------------------------------
//	@class:
//		CAutoRefTest
//
//	@doc:
//		Static unit tests for auto pointer
//
//---------------------------------------------------------------------------
class CAutoRefTest
{
public:
	class CElem : public CRefCount
	{
	public:
		CElem(ULONG ul) : m_ul(ul)
		{
		}

		ULONG m_ul;

	};	// class CElem


	// unittests
	static SPQOS_RESULT EresUnittest();
	static SPQOS_RESULT EresUnittest_Basics();

};	// class CAutoRefTest

}  // namespace spqos

#endif	// !SPQOS_CAutoRefTest_H

// EOF
