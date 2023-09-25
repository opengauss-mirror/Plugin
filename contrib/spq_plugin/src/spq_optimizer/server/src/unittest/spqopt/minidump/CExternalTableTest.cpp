//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CExternalTableTest.cpp
//
//	@doc:
//		Test for external tables
//---------------------------------------------------------------------------

#include "unittest/spqopt/minidump/CExternalTableTest.h"

#include "spqos/task/CAutoTraceFlag.h"

#include "spqopt/base/CAutoOptCtxt.h"
#include "spqopt/engine/CEnumeratorConfig.h"
#include "spqopt/engine/CStatisticsConfig.h"
#include "spqopt/exception.h"
#include "spqopt/mdcache/CMDCache.h"
#include "spqopt/minidump/CMinidumperUtils.h"
#include "spqopt/optimizer/COptimizerConfig.h"
#include "naucrates/dxl/operators/CDXLNode.h"
#include "naucrates/dxl/operators/CDXLPhysicalNLJoin.h"
#include "naucrates/exception.h"

#include "unittest/base.h"
#include "unittest/spqopt/CConstExprEvaluatorForDates.h"
#include "unittest/spqopt/CTestUtils.h"

using namespace spqdxl;

ULONG CExternalTableTest::m_ulTestCounter = 0;	// start from first test

// minidump files
const CHAR *rgszExternalTableFileNames[] = {
	"../data/dxl/minidump/ExternalTable1.mdp",
	"../data/dxl/minidump/ExternalTable2.mdp",
	"../data/dxl/minidump/ExternalTable3.mdp",
	"../data/dxl/minidump/ExternalTable4.mdp",
	"../data/dxl/minidump/ExternalTableWithFilter.mdp",
	"../data/dxl/minidump/CTAS-with-randomly-distributed-external-table.mdp",
	"../data/dxl/minidump/CTAS-with-hashed-distributed-external-table.mdp",
	"../data/dxl/minidump/AggonExternalTableNoMotion.mdp",
	"../data/dxl/minidump/ExternalPartitionTableNoPredicate.mdp",
	"../data/dxl/minidump/ExternalPartitionTableSelectOnPartKey.mdp",
};


//---------------------------------------------------------------------------
//	@function:
//		CExternalTableTest::EresUnittest
//
//	@doc:
//		Unittest for expressions
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CExternalTableTest::EresUnittest()
{
	CUnittest rgut[] = {
		SPQOS_UNITTEST_FUNC(CExternalTableTest::EresUnittest_RunMinidumpTests),
	};

	SPQOS_RESULT eres = CUnittest::EresExecute(rgut, SPQOS_ARRAY_SIZE(rgut));

	// reset metadata cache
	CMDCache::Reset();

	return eres;
}

//---------------------------------------------------------------------------
//	@function:
//		CExternalTableTest::EresUnittest_RunMinidumpTests
//
//	@doc:
//		Run all Minidump-based tests
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CExternalTableTest::EresUnittest_RunMinidumpTests()
{
	return CTestUtils::EresUnittest_RunTests(
		rgszExternalTableFileNames, &m_ulTestCounter,
		SPQOS_ARRAY_SIZE(rgszExternalTableFileNames));
}

// EOF
