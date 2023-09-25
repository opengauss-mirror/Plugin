//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2017 Pivotal, Inc.
//
//	@filename:
//		CCollapseProjectTest.cpp
//
//	@doc:
//		Test for optimizing queries with multiple project nodes
//---------------------------------------------------------------------------

#include "unittest/spqopt/minidump/CCollapseProjectTest.h"

#include "spqos/base.h"
#include "spqos/memory/CAutoMemoryPool.h"
#include "spqos/task/CAutoTraceFlag.h"
#include "spqos/test/CUnittest.h"

#include "spqopt/exception.h"
#include "spqopt/minidump/CMinidumperUtils.h"

#include "unittest/spqopt/CTestUtils.h"


using namespace spqopt;

ULONG CCollapseProjectTest::m_ulCollapseProjectTestCounter =
	0;	// start from first test

// minidump files
const CHAR *rgszCollapseProjectFileNames[] = {
	"../data/dxl/minidump/MultipleSetReturningFunction-1.mdp",	// project list has multiple set returning functions but one of them cannot be collapsed
	"../data/dxl/minidump/MultipleSetReturningFunction-2.mdp",	// project list has multiple set returning functions that can be collapsed
	"../data/dxl/minidump/MultipleSetReturningFunction-3.mdp",	// both child's and parent's project list has collapsible set returning functions, but we should not
	"../data/dxl/minidump/DMLCollapseProject.mdp",
	"../data/dxl/minidump/CollapseCascadeProjects2of2.mdp",
	"../data/dxl/minidump/CollapseCascadeProjects2of3.mdp",
	"../data/dxl/minidump/CannotCollapseCascadeProjects.mdp",
	"../data/dxl/minidump/CollapseProject-SetReturning.mdp",
	"../data/dxl/minidump/CollapseProject-SetReturning-CTE.mdp",

};


//---------------------------------------------------------------------------
//	@function:
//		CCollapseProjectTest::EresUnittest
//
//	@doc:
//		Unittest for expressions
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CCollapseProjectTest::EresUnittest()
{
	CUnittest rgut[] = {
		SPQOS_UNITTEST_FUNC(EresUnittest_RunTests),
	};

	SPQOS_RESULT eres = CUnittest::EresExecute(rgut, SPQOS_ARRAY_SIZE(rgut));

	// reset metadata cache
	CMDCache::Reset();

	return eres;
}

//---------------------------------------------------------------------------
//	@function:
//		CCollapseProjectTest::EresUnittest_RunTests
//
//	@doc:
//		Run all Minidump-based tests with plan matching
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CCollapseProjectTest::EresUnittest_RunTests()
{
	return CTestUtils::EresUnittest_RunTests(
		rgszCollapseProjectFileNames, &m_ulCollapseProjectTestCounter,
		SPQOS_ARRAY_SIZE(rgszCollapseProjectFileNames));
}

// EOF
