//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CCorrelatedExecutionTest.cpp
//
//	@doc:
//		Test for correlated subqueries
//---------------------------------------------------------------------------
#include "unittest/spqopt/csq/CCorrelatedExecutionTest.h"

#include "spqos/io/COstreamString.h"
#include "spqos/string/CWStringDynamic.h"

#include "spqopt/base/CColRefSetIter.h"
#include "spqopt/base/CPrintPrefix.h"
#include "spqopt/base/CQueryContext.h"
#include "spqopt/base/CUtils.h"
#include "spqopt/engine/CEngine.h"
#include "spqopt/eval/CConstExprEvaluatorDefault.h"
#include "spqopt/exception.h"
#include "spqopt/metadata/CTableDescriptor.h"
#include "spqopt/operators/ops.h"
#include "spqopt/translate/CTranslatorDXLToExpr.h"
#include "spqopt/translate/CTranslatorExprToDXL.h"
#include "naucrates/dxl/CDXLUtils.h"

#include "unittest/base.h"
#include "unittest/spqopt/CTestUtils.h"

ULONG CCorrelatedExecutionTest::m_ulTestCounter = 0;  // start from first test

// files for testing of different queries with correlated subqueries
const CHAR *rgszPositiveTests[] = {
	"../data/dxl/csq_tests/dxl-q1.xml",	 "../data/dxl/csq_tests/dxl-q2.xml",
	"../data/dxl/csq_tests/dxl-q3.xml",	 "../data/dxl/csq_tests/dxl-q4.xml",
	"../data/dxl/csq_tests/dxl-q5.xml",	 "../data/dxl/csq_tests/dxl-q6.xml",
	"../data/dxl/csq_tests/dxl-q7.xml",	 "../data/dxl/csq_tests/dxl-q8.xml",
	"../data/dxl/csq_tests/dxl-q9.xml",	 "../data/dxl/csq_tests/dxl-q10.xml",
	"../data/dxl/csq_tests/dxl-q11.xml", "../data/dxl/csq_tests/dxl-q12.xml",
	"../data/dxl/csq_tests/dxl-q13.xml", "../data/dxl/csq_tests/dxl-q14.xml",
	"../data/dxl/csq_tests/dxl-q15.xml", "../data/dxl/csq_tests/dxl-q16.xml",
};

//---------------------------------------------------------------------------
//	@function:
//		CCorrelatedExecutionTest::EresUnittest
//
//	@doc:
//		Unittest for converting correlated Apply expressions to NL joins
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CCorrelatedExecutionTest::EresUnittest()
{
	CUnittest rgut[] = {
		SPQOS_UNITTEST_FUNC(
			CCorrelatedExecutionTest::EresUnittest_RunAllPositiveTests),
	};

	return CUnittest::EresExecute(rgut, SPQOS_ARRAY_SIZE(rgut));
}

//---------------------------------------------------------------------------
//	@function:
//		CCorrelatedExecutionTest::EresUnittest_RunAllPositiveTests
//
//	@doc:
//		Run all tests that are expected to pass without any exceptions
//		for parsing DXL documents into DXL trees.
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CCorrelatedExecutionTest::EresUnittest_RunAllPositiveTests()
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	// setup a file-based provider
	CMDProviderMemory *pmdp = CTestUtils::m_pmdpf;
	pmdp->AddRef();
	CMDAccessor mda(mp, CMDCache::Pcache(), CTestUtils::m_sysidDefault, pmdp);

	// install opt context in TLS
	CAutoOptCtxt aoc(mp, &mda, NULL, /* pceeval */
					 CTestUtils::GetCostModel(mp));

	// loop over all test files
	for (ULONG ul = m_ulTestCounter; ul < SPQOS_ARRAY_SIZE(rgszPositiveTests);
		 ul++)
	{
		// TODO:  06/15/2012; enable plan matching
		SPQOS_RESULT eres = CTestUtils::EresTranslate(mp, rgszPositiveTests[ul],
													 NULL /* plan file */,
													 true /*fIgnoreMismatch*/
		);

		m_ulTestCounter++;

		if (SPQOS_OK != eres)
		{
			return eres;
		}
	}

	// reset test counter
	m_ulTestCounter = 0;

	return SPQOS_OK;
}

// EOF
