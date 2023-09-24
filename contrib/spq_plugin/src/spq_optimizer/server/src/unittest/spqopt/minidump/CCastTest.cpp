//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2018 Pivotal, Inc.
//---------------------------------------------------------------------------

#include "unittest/spqopt/minidump/CCastTest.h"

#include "spqos/base.h"
#include "spqos/memory/CAutoMemoryPool.h"
#include "spqos/task/CAutoTraceFlag.h"
#include "spqos/test/CUnittest.h"

#include "spqopt/exception.h"
#include "spqopt/minidump/CMinidumperUtils.h"

#include "unittest/spqopt/CTestUtils.h"


using namespace spqopt;

ULONG CCastTest::m_ulTestCounter = 0;  // start from first test

// minidump files
const CHAR *rgszCastMdpFiles[] = {
	"../data/dxl/minidump/InferPredicatesForLimit.mdp",
	"../data/dxl/minidump/ArrayCoerceExpr.mdp",
	"../data/dxl/minidump/SelectOnCastedCol.mdp",
	"../data/dxl/minidump/JOIN-Pred-Cast-Int4.mdp",
	"../data/dxl/minidump/JOIN-Pred-Cast-Varchar.mdp",
	"../data/dxl/minidump/JOIN-int4-Eq-double.mdp",
	"../data/dxl/minidump/JOIN-cast2text-int4-Eq-cast2text-double.mdp",
	"../data/dxl/minidump/JOIN-int4-Eq-int2.mdp",
	"../data/dxl/minidump/CastOnSubquery.mdp",
	"../data/dxl/minidump/CoerceToDomain.mdp",
	"../data/dxl/minidump/CoerceViaIO.mdp",
	"../data/dxl/minidump/ArrayCoerceCast.mdp",
	"../data/dxl/minidump/SimpleArrayCoerceCast.mdp",
	"../data/dxl/minidump/EstimateJoinRowsForCastPredicates.mdp",
	"../data/dxl/minidump/HashJoinOnRelabeledColumns.mdp",
	"../data/dxl/minidump/Correlation-With-Casting-1.mdp",
	"../data/dxl/minidump/Correlation-With-Casting-2.mdp",
	"../data/dxl/minidump/Date-TimeStamp-HashJoin.mdp",
	"../data/dxl/minidump/TimeStamp-Date-HashJoin.mdp",
};


SPQOS_RESULT
CCastTest::EresUnittest()
{
	CUnittest rgut[] = {
		SPQOS_UNITTEST_FUNC(EresUnittest_RunTests),
	};

	SPQOS_RESULT eres = CUnittest::EresExecute(rgut, SPQOS_ARRAY_SIZE(rgut));

	// reset metadata cache
	CMDCache::Reset();

	return eres;
}

// Run all Minidump-based tests with plan matching
SPQOS_RESULT
CCastTest::EresUnittest_RunTests()
{
	return CTestUtils::EresUnittest_RunTests(rgszCastMdpFiles, &m_ulTestCounter,
											 SPQOS_ARRAY_SIZE(rgszCastMdpFiles));
}

// EOF
