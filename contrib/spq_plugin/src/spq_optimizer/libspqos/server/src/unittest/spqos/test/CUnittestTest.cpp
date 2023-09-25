//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2015 Pivotal Software, Inc.
//
//	@filename:
//		CUnittestTest.cpp
//
//	@doc:
//		Test for CUnittest with subtests
//---------------------------------------------------------------------------

#include "unittest/spqos/test/CUnittestTest.h"

#include "spqos/base.h"
#include "spqos/types.h"

namespace spqos
{
//---------------------------------------------------------------------------
//	@function:
//		CUnittestTest::EresSubtest
//
//	@doc:
//		Driver for trivial subtest.
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CUnittestTest::EresSubtest(ULONG ulSubtest)
{
	if (ulSubtest * 1 == ulSubtest)
	{
		return SPQOS_OK;
	}
	else
	{
		return SPQOS_FAILED;
	}
}

}  // namespace spqos

// EOF
