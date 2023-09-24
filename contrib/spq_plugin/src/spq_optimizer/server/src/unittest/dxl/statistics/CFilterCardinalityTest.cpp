//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2018 Pivotal, Inc.
//
//	@filename:
//		CFilterCardinalityTest.cpp
//
//	@doc:
//		Test for filter cardinality estimation
//---------------------------------------------------------------------------

#ifndef __STDC_CONSTANT_MACROS
#define __STDC_CONSTANT_MACROS
#endif

#include "unittest/dxl/statistics/CFilterCardinalityTest.h"

#include <stdint.h>

#include "spqos/io/COstreamString.h"
#include "spqos/string/CWStringDynamic.h"

#include "naucrates/dxl/CDXLUtils.h"
#include "naucrates/statistics/CFilterStatsProcessor.h"
#include "naucrates/statistics/CStatisticsUtils.h"

#include "unittest/base.h"
#include "unittest/dxl/statistics/CCardinalityTestUtils.h"
#include "unittest/dxl/statistics/CStatisticsTest.h"
#include "unittest/spqopt/CTestUtils.h"

using namespace spqopt;

// DXL files
const CHAR *szInputDXLFileName =
	"../data/dxl/statistics/Basic-Statistics-Input.xml";
const CHAR *szOutputDXLFileName =
	"../data/dxl/statistics/Basic-Statistics-Output.xml";

const CTestUtils::STestCase rgtcStatistics[] = {
	{"../data/dxl/statistics/Numeric-Input.xml",
	 "../data/dxl/statistics/Numeric-Output-LT-MinBoundary.xml"},
	{"../data/dxl/statistics/Numeric-Input.xml",
	 "../data/dxl/statistics/Numeric-Output-LTE-MinBoundary.xml"},
	{"../data/dxl/statistics/Numeric-Input.xml",
	 "../data/dxl/statistics/Numeric-Output-E-MinBoundary.xml"},
	{"../data/dxl/statistics/Numeric-Input.xml",
	 "../data/dxl/statistics/Numeric-Output-GT-MaxBoundary.xml"},
	{"../data/dxl/statistics/Numeric-Input.xml",
	 "../data/dxl/statistics/Numeric-Output-GTE-MaxBoundary.xml"},
	{"../data/dxl/statistics/Numeric-Input.xml",
	 "../data/dxl/statistics/Numeric-Output-E-MaxBoundary.xml"},

	{"../data/dxl/statistics/Numeric-Input2.xml",
	 "../data/dxl/statistics/Numeric-Output-2-LT-MinBoundary.xml"},
	{"../data/dxl/statistics/Numeric-Input2.xml",
	 "../data/dxl/statistics/Numeric-Output-2-LTE-MinBoundary.xml"},
	{"../data/dxl/statistics/Numeric-Input2.xml",
	 "../data/dxl/statistics/Numeric-Output-2-E-MinBoundary.xml"},
	{"../data/dxl/statistics/Numeric-Input2.xml",
	 "../data/dxl/statistics/Numeric-Output-2-GT-MinBoundary.xml"},
	{"../data/dxl/statistics/Numeric-Input2.xml",
	 "../data/dxl/statistics/Numeric-Output-2-GTE-MinBoundary.xml"},

	{"../data/dxl/statistics/Numeric-Input2.xml",
	 "../data/dxl/statistics/Numeric-Output-2-LT-MaxBoundary.xml"},
	{"../data/dxl/statistics/Numeric-Input2.xml",
	 "../data/dxl/statistics/Numeric-Output-2-LTE-MaxBoundary.xml"},
	{"../data/dxl/statistics/Numeric-Input2.xml",
	 "../data/dxl/statistics/Numeric-Output-2-GT-MaxBoundary.xml"},
	{"../data/dxl/statistics/Numeric-Input2.xml",
	 "../data/dxl/statistics/Numeric-Output-2-GTE-MaxBoundary.xml"},
	{"../data/dxl/statistics/Numeric-Input2.xml",
	 "../data/dxl/statistics/Numeric-Output-2-E-MaxBoundary.xml"},
};

// unittest for statistics objects
SPQOS_RESULT
CFilterCardinalityTest::EresUnittest()
{
	// tests that use shared optimization context
	CUnittest rgutSharedOptCtxt[] = {
		SPQOS_UNITTEST_FUNC(CFilterCardinalityTest::
							   EresUnittest_CStatisticsBasicsFromDXLNumeric),
		SPQOS_UNITTEST_FUNC(
			CFilterCardinalityTest::EresUnittest_CStatisticsFilter),
		SPQOS_UNITTEST_FUNC(
			CFilterCardinalityTest::EresUnittest_CStatisticsFilterArrayCmpAny),
		SPQOS_UNITTEST_FUNC(
			CFilterCardinalityTest::EresUnittest_CStatisticsFilterConj),
		SPQOS_UNITTEST_FUNC(
			CFilterCardinalityTest::EresUnittest_CStatisticsFilterDisj),
		SPQOS_UNITTEST_FUNC(
			CFilterCardinalityTest::EresUnittest_CStatisticsNestedPred),
		SPQOS_UNITTEST_FUNC(
			CFilterCardinalityTest::EresUnittest_CStatisticsBasicsFromDXL),
		SPQOS_UNITTEST_FUNC(
			CFilterCardinalityTest::EresUnittest_CStatisticsAccumulateCard)};

	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	// setup a file-based provider
	CMDProviderMemory *pmdp = CTestUtils::m_pmdpf;
	pmdp->AddRef();
	CMDAccessor mda(mp, CMDCache::Pcache(), CTestUtils::m_sysidDefault, pmdp);

	// install opt context in TLS
	CAutoOptCtxt aoc(mp, &mda, NULL /* pceeval */,
					 CTestUtils::GetCostModel(mp));

	return CUnittest::EresExecute(rgutSharedOptCtxt,
								  SPQOS_ARRAY_SIZE(rgutSharedOptCtxt));
}

// reads a DXL document, generates the statistics object, performs a
// filter operation on it, serializes it into a DXL document and
// compares the generated DXL document with the expected DXL document.
SPQOS_RESULT
CFilterCardinalityTest::EresUnittest_CStatistics(
	SStatsFilterSTestCase rgstatsdisjtc[], ULONG ulTestCases)
{
	// create memory pool
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	CWStringDynamic str(mp);
	COstreamString oss(&str);

	CMDAccessor *md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();

	for (ULONG ul = 0; ul < ulTestCases; ul++)
	{
		SStatsFilterSTestCase elem = rgstatsdisjtc[ul];

		// read input/output DXL file
		CHAR *szDXLInput = CDXLUtils::Read(mp, elem.m_szInputFile);
		CHAR *szDXLOutput = CDXLUtils::Read(mp, elem.m_szOutputFile);

		SPQOS_CHECK_ABORT;

		// parse the statistics objects
		CDXLStatsDerivedRelationArray *dxl_derived_rel_stats_array =
			CDXLUtils::ParseDXLToStatsDerivedRelArray(mp, szDXLInput, NULL);
		CStatisticsArray *pdrspqstatBefore =
			CDXLUtils::ParseDXLToOptimizerStatisticObjArray(
				mp, md_accessor, dxl_derived_rel_stats_array);
		dxl_derived_rel_stats_array->Release();
		SPQOS_ASSERT(NULL != pdrspqstatBefore);

		SPQOS_CHECK_ABORT;

		// generate the disjunctive predicate
		FnPstatspredDisj *pf = elem.m_pf;
		SPQOS_ASSERT(NULL != pf);
		CStatsPred *disjunctive_pred_stats = pf(mp);

		SPQOS_RESULT eres = EresUnittest_CStatisticsCompare(
			mp, md_accessor, pdrspqstatBefore, disjunctive_pred_stats,
			szDXLOutput);

		// clean up
		pdrspqstatBefore->Release();
		disjunctive_pred_stats->Release();
		SPQOS_DELETE_ARRAY(szDXLInput);
		SPQOS_DELETE_ARRAY(szDXLOutput);

		if (SPQOS_FAILED == eres)
		{
			return eres;
		}
	}

	return SPQOS_OK;
}

// reads a DXL document, generates the statistics object, performs a
// filter operation on it, serializes it into a DXL document and
// compares the generated DXL document with the expected DXL document.
SPQOS_RESULT
CFilterCardinalityTest::EresUnittest_CStatisticsFilter()
{
	SStatsFilterSTestCase rgstatstc[] = {
		{"../data/dxl/statistics/Select-Statistics-Input-Null-Bucket.xml",
		 "../data/dxl/statistics/Select-Statistics-Output-Null-Bucket.xml",
		 PstatspredNullableCols},
		{"../data/dxl/statistics/Select-Statistics-Input-Null-Bucket.xml",
		 "../data/dxl/statistics/Select-Statistics-Output-Null-Constant.xml",
		 PstatspredWithNullConstant},
		{"../data/dxl/statistics/Select-Statistics-Input-Null-Bucket.xml",
		 "../data/dxl/statistics/Select-Statistics-Output-NotNull-Constant.xml",
		 PstatspredNotNull},
	};

	const ULONG ulTestCases = SPQOS_ARRAY_SIZE(rgstatstc);

	return EresUnittest_CStatistics(rgstatstc, ulTestCases);
}

// create a filter on a column with null values
CStatsPred *
CFilterCardinalityTest::PstatspredNullableCols(CMemoryPool *mp)
{
	CStatsPredPtrArry *pdrspqstatspred = SPQOS_NEW(mp) CStatsPredPtrArry(mp);

	pdrspqstatspred->Append(SPQOS_NEW(mp) CStatsPredPoint(
		1, CStatsPred::EstatscmptLEq, CTestUtils::PpointInt4(mp, 1)));

	return SPQOS_NEW(mp) CStatsPredConj(pdrspqstatspred);
}

// create a point filter where the constant is null
CStatsPred *
CFilterCardinalityTest::PstatspredWithNullConstant(CMemoryPool *mp)
{
	CStatsPredPtrArry *pdrspqstatspred = SPQOS_NEW(mp) CStatsPredPtrArry(mp);

	pdrspqstatspred->Append(SPQOS_NEW(mp) CStatsPredPoint(
		1, CStatsPred::EstatscmptEq, CTestUtils::PpointInt4NullVal(mp)));

	return SPQOS_NEW(mp) CStatsPredConj(pdrspqstatspred);
}

// create an 'is not null' point filter
CStatsPred *
CFilterCardinalityTest::PstatspredNotNull(CMemoryPool *mp)
{
	CStatsPredPtrArry *pdrspqstatspred = SPQOS_NEW(mp) CStatsPredPtrArry(mp);

	pdrspqstatspred->Append(SPQOS_NEW(mp) CStatsPredPoint(
		1, CStatsPred::EstatscmptNEq, CTestUtils::PpointInt4NullVal(mp)));

	return SPQOS_NEW(mp) CStatsPredConj(pdrspqstatspred);
}

// testing ArryCmpAny predicates
SPQOS_RESULT
CFilterCardinalityTest::EresUnittest_CStatisticsFilterArrayCmpAny()
{
	SStatsFilterSTestCase rgstatsdisjtc[] = {
		{"../data/dxl/statistics/ArrayCmpAny-Input-1.xml",
		 "../data/dxl/statistics/ArrayCmpAny-Output-1.xml",
		 PstatspredArrayCmpAnySimple},
		{"../data/dxl/statistics/ArrayCmpAny-Input-1.xml",
		 "../data/dxl/statistics/ArrayCmpAny-Output-1.xml",
		 PstatspredArrayCmpAnyDuplicate}};

	const ULONG ulTestCases = SPQOS_ARRAY_SIZE(rgstatsdisjtc);

	return EresUnittest_CStatistics(rgstatsdisjtc, ulTestCases);
}

// create a 'col IN (...)' filter without duplicates
CStatsPred *
CFilterCardinalityTest::PstatspredArrayCmpAnySimple(CMemoryPool *mp)
{
	CStatsPredPtrArry *pdrspqstatspred = SPQOS_NEW(mp) CStatsPredPtrArry(mp);

	CPointArray *arr = SPQOS_NEW(mp) CPointArray(mp);
	arr->Append(CTestUtils::PpointInt4(mp, 1));
	arr->Append(CTestUtils::PpointInt4(mp, 2));
	arr->Append(CTestUtils::PpointInt4(mp, 15));

	pdrspqstatspred->Append(
		SPQOS_NEW(mp) CStatsPredArrayCmp(1, CStatsPred::EstatscmptEq, arr));

	return SPQOS_NEW(mp) CStatsPredConj(pdrspqstatspred);
}

// create a 'col IN (...)' filter with duplicates (unsorted)
CStatsPred *
CFilterCardinalityTest::PstatspredArrayCmpAnyDuplicate(CMemoryPool *mp)
{
	CStatsPredPtrArry *pdrspqstatspred = SPQOS_NEW(mp) CStatsPredPtrArry(mp);

	CPointArray *arr = SPQOS_NEW(mp) CPointArray(mp);
	arr->Append(CTestUtils::PpointInt4(mp, 1));
	arr->Append(CTestUtils::PpointInt4(mp, 1));
	arr->Append(CTestUtils::PpointInt4(mp, 15));
	arr->Append(CTestUtils::PpointInt4(mp, 2));
	arr->Append(CTestUtils::PpointInt4(mp, 1));
	arr->Append(CTestUtils::PpointInt4(mp, 1));
	arr->Append(CTestUtils::PpointInt4(mp, 2));
	arr->Append(CTestUtils::PpointInt4(mp, 15));
	arr->Append(CTestUtils::PpointInt4(mp, 15));
	arr->Append(CTestUtils::PpointInt4(mp, 15));

	pdrspqstatspred->Append(
		SPQOS_NEW(mp) CStatsPredArrayCmp(1, CStatsPred::EstatscmptEq, arr));

	return SPQOS_NEW(mp) CStatsPredConj(pdrspqstatspred);
}

// reads a DXL document, generates the statistics object, performs a
// filter operation on it, serializes it into a DXL document and
// compares the generated DXL document with the expected DXL document.
SPQOS_RESULT
CFilterCardinalityTest::EresUnittest_CStatisticsFilterDisj()
{
	SStatsFilterSTestCase rgstatsdisjtc[] = {
		{"../data/dxl/statistics/Disj-Input-1.xml",
		 "../data/dxl/statistics/Disj-Output-1.xml", PstatspredDisj1},
		{"../data/dxl/statistics/Disj-Input-1.xml",
		 "../data/dxl/statistics/Disj-Output-1.xml", PstatspredDisj2},
		{"../data/dxl/statistics/Disj-Input-1.xml",
		 "../data/dxl/statistics/Disj-Output-1.xml", PstatspredDisj3},
		{"../data/dxl/statistics/Disj-Input-2.xml",
		 "../data/dxl/statistics/Disj-Output-2-1.xml", PstatspredDisj4},
		{"../data/dxl/statistics/Disj-Input-2.xml",
		 "../data/dxl/statistics/Disj-Output-2-2.xml", PstatspredDisj5},
		{"../data/dxl/statistics/Disj-Input-2.xml",
		 "../data/dxl/statistics/Disj-Output-2-3.xml", PstatspredDisj6},
		{"../data/dxl/statistics/Disj-Input-2.xml",
		 "../data/dxl/statistics/Disj-Output-2-4.xml", PstatspredDisj7},
		{"../data/dxl/statistics/NestedPred-Input-10.xml",
		 "../data/dxl/statistics/Disj-Output-8.xml", PstatspredDisj8},
		{"../data/dxl/statistics/Disj-Input-2.xml",
		 "../data/dxl/statistics/Disj-Output-2-5.xml", PstatspredDisj9},
	};

	const ULONG ulTestCases = SPQOS_ARRAY_SIZE(rgstatsdisjtc);

	return EresUnittest_CStatistics(rgstatsdisjtc, ulTestCases);
}

// create an or filter (no duplicate)
CStatsPred *
CFilterCardinalityTest::PstatspredDisj1(CMemoryPool *mp)
{
	// predicate col_1 in (13, 25, 47, 49);
	INT rgiVal[] = {13, 25, 47, 49};
	const ULONG ulVals = SPQOS_ARRAY_SIZE(rgiVal);
	CStatsPredPtrArry *pdrspqstatspredDisj =
		PdrspqstatspredInteger(mp, 1, CStatsPred::EstatscmptEq, rgiVal, ulVals);

	return SPQOS_NEW(mp) CStatsPredDisj(pdrspqstatspredDisj);
}

// create an or filter (one duplicate constant)
CStatsPred *
CFilterCardinalityTest::PstatspredDisj2(CMemoryPool *mp)
{
	// predicate col_1 in (13, 13, 25, 47, 49);
	INT rgiVal[] = {13, 13, 25, 47, 49};
	const ULONG ulVals = SPQOS_ARRAY_SIZE(rgiVal);
	CStatsPredPtrArry *pdrspqstatspredDisj =
		PdrspqstatspredInteger(mp, 1, CStatsPred::EstatscmptEq, rgiVal, ulVals);

	return SPQOS_NEW(mp) CStatsPredDisj(pdrspqstatspredDisj);
}

//	create an or filter (multiple duplicate constants)
CStatsPred *
CFilterCardinalityTest::PstatspredDisj3(CMemoryPool *mp)
{
	// predicate col_1 in (13, 25, 47, 47, 47, 49, 13);
	INT rgiVal[] = {13, 25, 47, 47, 47, 49, 13};
	const ULONG ulVals = SPQOS_ARRAY_SIZE(rgiVal);
	CStatsPredPtrArry *pdrspqstatspredDisj =
		PdrspqstatspredInteger(mp, 1, CStatsPred::EstatscmptEq, rgiVal, ulVals);

	return SPQOS_NEW(mp) CStatsPredDisj(pdrspqstatspredDisj);
}

// create an or filter
CStatsPred *
CFilterCardinalityTest::PstatspredDisj4(CMemoryPool *mp)
{
	// the predicate is (x <= 5 or x <= 10 or x <= 13) (domain [0 -- 20])
	INT rgiVal[] = {5, 10, 13};
	const ULONG ulVals = SPQOS_ARRAY_SIZE(rgiVal);
	CStatsPredPtrArry *pdrspqstatspredDisj =
		PdrspqstatspredInteger(mp, 1, CStatsPred::EstatscmptLEq, rgiVal, ulVals);

	return SPQOS_NEW(mp) CStatsPredDisj(pdrspqstatspredDisj);
}

//	create an or filter (multiple LEQ)
CStatsPred *
CFilterCardinalityTest::PstatspredDisj5(CMemoryPool *mp)
{
	// the predicate is (x >= 5 or x >= 13) (domain [0 -- 20])
	INT rgiVal[] = {5, 13};
	const ULONG ulVals = SPQOS_ARRAY_SIZE(rgiVal);
	CStatsPredPtrArry *pdrspqstatspredDisj =
		PdrspqstatspredInteger(mp, 1, CStatsPred::EstatscmptGEq, rgiVal, ulVals);

	return SPQOS_NEW(mp) CStatsPredDisj(pdrspqstatspredDisj);
}

//	create an or filter
CStatsPred *
CFilterCardinalityTest::PstatspredDisj6(CMemoryPool *mp)
{
	// the predicate is (x > 10 or x < 5) (domain [0 -- 20])
	CStatsPredPtrArry *pdrspqstatspredDisj = SPQOS_NEW(mp) CStatsPredPtrArry(mp);

	pdrspqstatspredDisj->Append(SPQOS_NEW(mp) CStatsPredPoint(
		1, CStatsPred::EstatscmptG, CTestUtils::PpointInt4(mp, 10)));
	pdrspqstatspredDisj->Append(SPQOS_NEW(mp) CStatsPredPoint(
		1, CStatsPred::EstatscmptL, CTestUtils::PpointInt4(mp, 5)));

	return SPQOS_NEW(mp) CStatsPredDisj(pdrspqstatspredDisj);
}

// create an or filter
CStatsPred *
CFilterCardinalityTest::PstatspredDisj7(CMemoryPool *mp)
{
	// the predicate is (x <= 15 or x >= 5 or x > = 10) (domain [0 -- 20])
	INT rgiVal[] = {5, 10};
	const ULONG ulVals = SPQOS_ARRAY_SIZE(rgiVal);
	CStatsPredPtrArry *pdrspqstatspredDisj =
		PdrspqstatspredInteger(mp, 1, CStatsPred::EstatscmptGEq, rgiVal, ulVals);
	pdrspqstatspredDisj->Append(SPQOS_NEW(mp) CStatsPredPoint(
		1, CStatsPred::EstatscmptLEq, CTestUtils::PpointInt4(mp, 15)));

	return SPQOS_NEW(mp) CStatsPredDisj(pdrspqstatspredDisj);
}

// create disjunctive predicate on same columns
CStatsPred *
CFilterCardinalityTest::PstatspredDisj8(CMemoryPool *mp)
{
	// predicate is b = 2001 OR b == 2002
	INT rgiVal[] = {2001, 2002};
	const ULONG ulVals = SPQOS_ARRAY_SIZE(rgiVal);
	CStatsPredPtrArry *pdrspqstatspredDisj =
		PdrspqstatspredInteger(mp, 61, CStatsPred::EstatscmptEq, rgiVal, ulVals);

	return SPQOS_NEW(mp) CStatsPredDisj(pdrspqstatspredDisj);
}

//	create an or filter (multiple LEQ)
CStatsPred *
CFilterCardinalityTest::PstatspredDisj9(CMemoryPool *mp)
{
	// the predicate is (x <= 3 or x <= 10) (domain [0 -- 20])
	INT rgiVal[] = {3, 10};
	const ULONG ulVals = SPQOS_ARRAY_SIZE(rgiVal);
	CStatsPredPtrArry *pdrspqstatspredDisj =
		PdrspqstatspredInteger(mp, 1, CStatsPred::EstatscmptLEq, rgiVal, ulVals);

	return SPQOS_NEW(mp) CStatsPredDisj(pdrspqstatspredDisj);
}

// reads a DXL document, generates the statistics object, performs a
// filter operation on it, serializes it into a DXL document and
// compares the generated DXL document with the expected DXL document.
SPQOS_RESULT
CFilterCardinalityTest::EresUnittest_CStatisticsFilterConj()
{
	SStatsFilterSTestCase rgstatsdisjtc[] = {
		{"../data/dxl/statistics/NestedPred-Input-9.xml",
		 "../data/dxl/statistics/NestedPred-Output-9.xml", PstatspredConj},
	};

	const ULONG ulTestCases = SPQOS_ARRAY_SIZE(rgstatsdisjtc);

	return EresUnittest_CStatistics(rgstatsdisjtc, ulTestCases);
}

// create conjunctive predicate
CStatsPred *
CFilterCardinalityTest::PstatspredConj(CMemoryPool *mp)
{
	CWStringDynamic *pstrW =
		SPQOS_NEW(mp) CWStringDynamic(mp, SPQOS_WSZ_LIT("AAAABXc="));

	// predicate is a == 'w' AND b == 2001 AND c > 0
	CStatsPredPtrArry *pdrspqstatspredConj3 = SPQOS_NEW(mp) CStatsPredPtrArry(mp);
	pdrspqstatspredConj3->Append(SPQOS_NEW(mp) CStatsPredPoint(
		594, CStatsPred::EstatscmptEq,
		CCardinalityTestUtils::PpointGeneric(mp, SPQDB_TEXT, pstrW, 160621100)));
	pdrspqstatspredConj3->Append(SPQOS_NEW(mp) CStatsPredPoint(
		592, CStatsPred::EstatscmptEq, CTestUtils::PpointInt4(mp, 2001)));
	pdrspqstatspredConj3->Append(SPQOS_NEW(mp) CStatsPredPoint(
		593, CStatsPred::EstatscmptG, CTestUtils::PpointInt4(mp, 0)));

	SPQOS_DELETE(pstrW);

	return SPQOS_NEW(mp) CStatsPredConj(pdrspqstatspredConj3);
}

// reads a DXL document, generates the statistics object, performs a
// filter operation on it, serializes it into a DXL document and
// compares the generated DXL document with the expected DXL document.
SPQOS_RESULT
CFilterCardinalityTest::EresUnittest_CStatisticsNestedPred()
{
	SStatsFilterSTestCase rgstatsdisjtc[] = {
		{"../data/dxl/statistics/NestedPred-Input-1.xml",
		 "../data/dxl/statistics/NestedPred-Output-1.xml",
		 PstatspredNestedPredDiffCol1},
		{"../data/dxl/statistics/NestedPred-Input-1.xml",
		 "../data/dxl/statistics/NestedPred-Output-1.xml",
		 PstatspredNestedPredDiffCol2},
		{"../data/dxl/statistics/NestedPred-Input-2.xml",
		 "../data/dxl/statistics/NestedPred-Output-2.xml",
		 PstatspredNestedPredCommonCol1},
		{"../data/dxl/statistics/NestedPred-Input-1.xml",
		 "../data/dxl/statistics/NestedPred-Output-3.xml",
		 PstatspredNestedSharedCol},
		{"../data/dxl/statistics/NestedPred-Input-3.xml",
		 "../data/dxl/statistics/NestedPred-Output-4.xml",
		 PstatspredDisjOverConjSameCol1},
		{"../data/dxl/statistics/NestedPred-Input-3.xml",
		 "../data/dxl/statistics/NestedPred-Input-3.xml",
		 PstatspredDisjOverConjSameCol2},
		{"../data/dxl/statistics/NestedPred-Input-1.xml",
		 "../data/dxl/statistics/NestedPred-Output-5.xml",
		 PstatspredDisjOverConjDifferentCol1},
		{"../data/dxl/statistics/NestedPred-Input-1.xml",
		 "../data/dxl/statistics/NestedPred-Output-6.xml",
		 PstatspredDisjOverConjMultipleIdenticalCols},
		{"../data/dxl/statistics/NestedPred-Input-2.xml",
		 "../data/dxl/statistics/NestedPred-Output-7.xml",
		 PstatspredNestedPredCommonCol2},
		{"../data/dxl/statistics/NestedPred-Input-8.xml",
		 "../data/dxl/statistics/NestedPred-Output-8.xml",
		 PstatspredDisjOverConjSameCol3},
		{"../data/dxl/statistics/NestedPred-Input-10.xml",
		 "../data/dxl/statistics/NestedPred-Output-10.xml",
		 PstatspredDisjOverConjSameCol4},
	};

	const ULONG ulTestCases = SPQOS_ARRAY_SIZE(rgstatsdisjtc);

	return EresUnittest_CStatistics(rgstatsdisjtc, ulTestCases);
}

//		Create nested AND and OR predicates where the AND and OR predicates
//		are on different columns
CStatsPred *
CFilterCardinalityTest::PstatspredNestedPredDiffCol1(CMemoryPool *mp)
{
	CStatsPredPtrArry *pdrspqstatspredConj = SPQOS_NEW(mp) CStatsPredPtrArry(mp);

	// predicate col_1 <> 3
	pdrspqstatspredConj->Append(SPQOS_NEW(mp) CStatsPredPoint(
		1, CStatsPred::EstatscmptNEq, CTestUtils::PpointInt4(mp, 3)));

	// predicate col_2 in (15, 20, 22, 24, 31, 39, 42, 46);
	INT rgiVal[] = {15, 20, 22, 24, 31, 39, 42, 46};
	const ULONG ulVals = SPQOS_ARRAY_SIZE(rgiVal);
	CStatsPredPtrArry *pdrspqstatspredDisj =
		PdrspqstatspredInteger(mp, 2, CStatsPred::EstatscmptEq, rgiVal, ulVals);

	CStatsPredDisj *disjunctive_pred_stats =
		SPQOS_NEW(mp) CStatsPredDisj(pdrspqstatspredDisj);
	pdrspqstatspredConj->Append(disjunctive_pred_stats);

	return SPQOS_NEW(mp) CStatsPredConj(pdrspqstatspredConj);
}

// create nested AND and OR predicates where the AND and OR predicates
// are on different columns. note: the order of the predicates in
// reversed as in PstatspredNestedPredDiffCol1
CStatsPred *
CFilterCardinalityTest::PstatspredNestedPredDiffCol2(CMemoryPool *mp)
{
	CStatsPredPtrArry *pdrspqstatspredConj = SPQOS_NEW(mp) CStatsPredPtrArry(mp);

	// predicate col_2 in (15, 20, 22, 24, 31, 39, 42, 46);
	INT rgiVal[] = {15, 20, 22, 24, 31, 39, 42, 46};
	const ULONG ulVals = SPQOS_ARRAY_SIZE(rgiVal);
	CStatsPredPtrArry *pdrspqstatspredDisj =
		PdrspqstatspredInteger(mp, 2, CStatsPred::EstatscmptEq, rgiVal, ulVals);

	CStatsPredDisj *disjunctive_pred_stats =
		SPQOS_NEW(mp) CStatsPredDisj(pdrspqstatspredDisj);
	pdrspqstatspredConj->Append(disjunctive_pred_stats);

	// predicate col_1 <> 3
	pdrspqstatspredConj->Append(SPQOS_NEW(mp) CStatsPredPoint(
		1, CStatsPred::EstatscmptNEq, CTestUtils::PpointInt4(mp, 3)));

	return SPQOS_NEW(mp) CStatsPredConj(pdrspqstatspredConj);
}

// create nested AND and OR predicates where the AND and OR predicates
// are on the same columns
CStatsPred *
CFilterCardinalityTest::PstatspredNestedPredCommonCol1(CMemoryPool *mp)
{
	// predicate is col_2 in (39, 31, 24, 22, 46, 20, 42, 15) AND col_2 == 2
	CStatsPredPtrArry *pdrspqstatspredConj = SPQOS_NEW(mp) CStatsPredPtrArry(mp);

	// predicate col_2 in (15, 20, 22, 24, 31, 39, 42, 46);
	INT rgiVal[] = {15, 20, 22, 24, 31, 39, 42, 46};
	const ULONG ulVals = SPQOS_ARRAY_SIZE(rgiVal);
	CStatsPredPtrArry *pdrspqstatspredDisj =
		PdrspqstatspredInteger(mp, 2, CStatsPred::EstatscmptEq, rgiVal, ulVals);

	CStatsPredDisj *disjunctive_pred_stats =
		SPQOS_NEW(mp) CStatsPredDisj(pdrspqstatspredDisj);
	pdrspqstatspredConj->Append(disjunctive_pred_stats);

	// predicate col_2 == 2
	pdrspqstatspredConj->Append(SPQOS_NEW(mp) CStatsPredPoint(
		2, CStatsPred::EstatscmptEq, CTestUtils::PpointInt4(mp, 2)));

	return SPQOS_NEW(mp) CStatsPredConj(pdrspqstatspredConj);
}

// create nested AND and OR predicates where the AND and OR predicates
// are on the same columns
CStatsPred *
CFilterCardinalityTest::PstatspredNestedPredCommonCol2(CMemoryPool *mp)
{
	// predicate is col_2 in (2, 39, 31, 24, 22, 46, 20, 42, 15) AND col_2 == 2
	CStatsPredPtrArry *pdrspqstatspredConj = SPQOS_NEW(mp) CStatsPredPtrArry(mp);

	// IN predicate: col_2 in (2, 39, 31, 24, 22, 46, 20, 42, 15);
	INT rgiVal[] = {2, 15, 20, 22, 24, 31, 39, 42, 46};
	const ULONG ulVals = SPQOS_ARRAY_SIZE(rgiVal);
	CStatsPredPtrArry *pdrspqstatspredDisj =
		PdrspqstatspredInteger(mp, 2, CStatsPred::EstatscmptEq, rgiVal, ulVals);

	CStatsPredDisj *disjunctive_pred_stats =
		SPQOS_NEW(mp) CStatsPredDisj(pdrspqstatspredDisj);
	pdrspqstatspredConj->Append(disjunctive_pred_stats);

	// predicate col_2 == 2
	pdrspqstatspredConj->Append(SPQOS_NEW(mp) CStatsPredPoint(
		2, CStatsPred::EstatscmptEq, CTestUtils::PpointInt4(mp, 2)));

	return SPQOS_NEW(mp) CStatsPredConj(pdrspqstatspredConj);
}

// create nested AND and OR predicates where the AND and OR predicates
// share common columns
CStatsPred *
CFilterCardinalityTest::PstatspredNestedSharedCol(CMemoryPool *mp)
{
	CStatsPredPtrArry *pdrspqstatspredConj = SPQOS_NEW(mp) CStatsPredPtrArry(mp);

	// predicate col_1 <> 3
	pdrspqstatspredConj->Append(SPQOS_NEW(mp) CStatsPredPoint(
		1, CStatsPred::EstatscmptNEq, CTestUtils::PpointInt4(mp, 3)));

	// predicate col_2 in (15, 20, 22, 24, 31, 39, 42, 46) OR (col_1 == 4));

	INT rgiVal[] = {15, 20, 22, 24, 31, 39, 42, 46};
	const ULONG ulVals = SPQOS_ARRAY_SIZE(rgiVal);
	CStatsPredPtrArry *pdrspqstatspredDisj =
		PdrspqstatspredInteger(mp, 2, CStatsPred::EstatscmptEq, rgiVal, ulVals);

	pdrspqstatspredDisj->Append(SPQOS_NEW(mp) CStatsPredPoint(
		1, CStatsPred::EstatscmptEq, CTestUtils::PpointInt4(mp, 4)));

	CStatsPredDisj *disjunctive_pred_stats =
		SPQOS_NEW(mp) CStatsPredDisj(pdrspqstatspredDisj);
	pdrspqstatspredConj->Append(disjunctive_pred_stats);

	return SPQOS_NEW(mp) CStatsPredConj(pdrspqstatspredConj);
}

// create nested AND and OR predicates where the AND and OR predicates share common columns
CStatsPred *
CFilterCardinalityTest::PstatspredDisjOverConjSameCol1(CMemoryPool *mp)
{
	CStatsPredPtrArry *pdrspqstatspredConj = SPQOS_NEW(mp) CStatsPredPtrArry(mp);

	// predicate col_1 = 3 AND col_1 >=3
	pdrspqstatspredConj->Append(SPQOS_NEW(mp) CStatsPredPoint(
		1, CStatsPred::EstatscmptEq, CTestUtils::PpointInt4(mp, 3)));
	pdrspqstatspredConj->Append(SPQOS_NEW(mp) CStatsPredPoint(
		1, CStatsPred::EstatscmptGEq, CTestUtils::PpointInt4(mp, 3)));

	CStatsPredConj *conjunctive_pred_stats =
		SPQOS_NEW(mp) CStatsPredConj(pdrspqstatspredConj);

	// predicate (col_1 = 1);
	CStatsPredPtrArry *pdrspqstatspredDisj = SPQOS_NEW(mp) CStatsPredPtrArry(mp);
	pdrspqstatspredDisj->Append(SPQOS_NEW(mp) CStatsPredPoint(
		1, CStatsPred::EstatscmptEq, CTestUtils::PpointInt4(mp, 1)));
	pdrspqstatspredDisj->Append(conjunctive_pred_stats);

	return SPQOS_NEW(mp) CStatsPredDisj(pdrspqstatspredDisj);
}

// create nested AND and OR predicates where the AND and OR predicates share common columns
CStatsPred *
CFilterCardinalityTest::PstatspredDisjOverConjSameCol2(CMemoryPool *mp)
{
	CStatsPredPtrArry *pdrspqstatspredConj = SPQOS_NEW(mp) CStatsPredPtrArry(mp);

	// predicate col_1 <= 5 AND col_1 >=1
	pdrspqstatspredConj->Append(SPQOS_NEW(mp) CStatsPredPoint(
		1, CStatsPred::EstatscmptLEq, CTestUtils::PpointInt4(mp, 5)));
	pdrspqstatspredConj->Append(SPQOS_NEW(mp) CStatsPredPoint(
		1, CStatsPred::EstatscmptGEq, CTestUtils::PpointInt4(mp, 1)));

	CStatsPredConj *conjunctive_pred_stats =
		SPQOS_NEW(mp) CStatsPredConj(pdrspqstatspredConj);

	// predicate (col_1 = 1);
	CStatsPredPtrArry *pdrspqstatspredDisj = SPQOS_NEW(mp) CStatsPredPtrArry(mp);
	pdrspqstatspredDisj->Append(SPQOS_NEW(mp) CStatsPredPoint(
		1, CStatsPred::EstatscmptEq, CTestUtils::PpointInt4(mp, 1)));
	pdrspqstatspredDisj->Append(conjunctive_pred_stats);

	return SPQOS_NEW(mp) CStatsPredDisj(pdrspqstatspredDisj);
}

// create disjunctive predicate over conjunctions on same columns
CStatsPred *
CFilterCardinalityTest::PstatspredDisjOverConjSameCol3(CMemoryPool *mp)
{
	CStatsPredPtrArry *pdrspqstatspredDisj = SPQOS_NEW(mp) CStatsPredPtrArry(mp);

	CWStringDynamic *pstrS =
		SPQOS_NEW(mp) CWStringDynamic(mp, SPQOS_WSZ_LIT("AAAABXM="));
	CWStringDynamic *pstrW =
		SPQOS_NEW(mp) CWStringDynamic(mp, SPQOS_WSZ_LIT("AAAABXc="));

	// predicate is a == 's' AND b == 2001
	CStatsPredPtrArry *pdrspqstatspredConj1 = SPQOS_NEW(mp) CStatsPredPtrArry(mp);
	pdrspqstatspredConj1->Append(SPQOS_NEW(mp) CStatsPredPoint(
		142, CStatsPred::EstatscmptEq,
		CCardinalityTestUtils::PpointGeneric(mp, SPQDB_TEXT, pstrS, 160588332)));
	pdrspqstatspredConj1->Append(SPQOS_NEW(mp) CStatsPredPoint(
		113, CStatsPred::EstatscmptEq, CTestUtils::PpointInt4(mp, 2001)));
	pdrspqstatspredDisj->Append(SPQOS_NEW(mp)
								   CStatsPredConj(pdrspqstatspredConj1));

	// predicate is a == 's' AND b == 2002
	CStatsPredPtrArry *pdrspqstatspredConj2 = SPQOS_NEW(mp) CStatsPredPtrArry(mp);
	pdrspqstatspredConj2->Append(SPQOS_NEW(mp) CStatsPredPoint(
		142, CStatsPred::EstatscmptEq,
		CCardinalityTestUtils::PpointGeneric(mp, SPQDB_TEXT, pstrS, 160588332)));
	pdrspqstatspredConj2->Append(SPQOS_NEW(mp) CStatsPredPoint(
		113, CStatsPred::EstatscmptEq, CTestUtils::PpointInt4(mp, 2002)));
	pdrspqstatspredDisj->Append(SPQOS_NEW(mp)
								   CStatsPredConj(pdrspqstatspredConj2));

	// predicate is a == 'w' AND b == 2001
	CStatsPredPtrArry *pdrspqstatspredConj3 = SPQOS_NEW(mp) CStatsPredPtrArry(mp);
	pdrspqstatspredConj3->Append(SPQOS_NEW(mp) CStatsPredPoint(
		142, CStatsPred::EstatscmptEq,
		CCardinalityTestUtils::PpointGeneric(mp, SPQDB_TEXT, pstrW, 160621100)));
	pdrspqstatspredConj3->Append(SPQOS_NEW(mp) CStatsPredPoint(
		113, CStatsPred::EstatscmptEq, CTestUtils::PpointInt4(mp, 2001)));
	pdrspqstatspredDisj->Append(SPQOS_NEW(mp)
								   CStatsPredConj(pdrspqstatspredConj3));

	// predicate is a == 'w' AND b == 2002
	CStatsPredPtrArry *pdrspqstatspredConj4 = SPQOS_NEW(mp) CStatsPredPtrArry(mp);
	pdrspqstatspredConj4->Append(SPQOS_NEW(mp) CStatsPredPoint(
		142, CStatsPred::EstatscmptEq,
		CCardinalityTestUtils::PpointGeneric(mp, SPQDB_TEXT, pstrW, 160621100)));
	pdrspqstatspredConj4->Append(SPQOS_NEW(mp) CStatsPredPoint(
		113, CStatsPred::EstatscmptEq, CTestUtils::PpointInt4(mp, 2002)));
	pdrspqstatspredDisj->Append(SPQOS_NEW(mp)
								   CStatsPredConj(pdrspqstatspredConj4));

	SPQOS_DELETE(pstrS);
	SPQOS_DELETE(pstrW);

	return SPQOS_NEW(mp) CStatsPredDisj(pdrspqstatspredDisj);
}

// create disjunctive predicate over conjunctions on same columns
CStatsPred *
CFilterCardinalityTest::PstatspredDisjOverConjSameCol4(CMemoryPool *mp)
{
	CStatsPredPtrArry *pdrspqstatspredDisj = SPQOS_NEW(mp) CStatsPredPtrArry(mp);

	CWStringDynamic *pstrS =
		SPQOS_NEW(mp) CWStringDynamic(mp, SPQOS_WSZ_LIT("AAAABXM="));
	CWStringDynamic *pstrW =
		SPQOS_NEW(mp) CWStringDynamic(mp, SPQOS_WSZ_LIT("AAAABXc="));

	// predicate is a == 's' AND b == 2001 AND c > 0
	CStatsPredPtrArry *pdrspqstatspredConj1 = SPQOS_NEW(mp) CStatsPredPtrArry(mp);
	pdrspqstatspredConj1->Append(SPQOS_NEW(mp) CStatsPredPoint(
		91, CStatsPred::EstatscmptEq,
		CCardinalityTestUtils::PpointGeneric(mp, SPQDB_TEXT, pstrS, 160588332)));
	pdrspqstatspredConj1->Append(SPQOS_NEW(mp) CStatsPredPoint(
		61, CStatsPred::EstatscmptEq, CTestUtils::PpointInt4(mp, 2001)));
	pdrspqstatspredConj1->Append(SPQOS_NEW(mp) CStatsPredPoint(
		90, CStatsPred::EstatscmptG, CTestUtils::PpointInt4(mp, 0)));
	pdrspqstatspredDisj->Append(SPQOS_NEW(mp)
								   CStatsPredConj(pdrspqstatspredConj1));

	// predicate is a == 's' AND b == 2002
	CStatsPredPtrArry *pdrspqstatspredConj2 = SPQOS_NEW(mp) CStatsPredPtrArry(mp);
	pdrspqstatspredConj2->Append(SPQOS_NEW(mp) CStatsPredPoint(
		91, CStatsPred::EstatscmptEq,
		CCardinalityTestUtils::PpointGeneric(mp, SPQDB_TEXT, pstrS, 160588332)));
	pdrspqstatspredConj2->Append(SPQOS_NEW(mp) CStatsPredPoint(
		61, CStatsPred::EstatscmptEq, CTestUtils::PpointInt4(mp, 2002)));
	pdrspqstatspredDisj->Append(SPQOS_NEW(mp)
								   CStatsPredConj(pdrspqstatspredConj2));

	// predicate is a == 'w' AND b == 2001 AND c > 0
	CStatsPredPtrArry *pdrspqstatspredConj3 = SPQOS_NEW(mp) CStatsPredPtrArry(mp);
	pdrspqstatspredConj3->Append(SPQOS_NEW(mp) CStatsPredPoint(
		91, CStatsPred::EstatscmptEq,
		CCardinalityTestUtils::PpointGeneric(mp, SPQDB_TEXT, pstrW, 160621100)));
	pdrspqstatspredConj3->Append(SPQOS_NEW(mp) CStatsPredPoint(
		61, CStatsPred::EstatscmptEq, CTestUtils::PpointInt4(mp, 2001)));
	pdrspqstatspredConj1->Append(SPQOS_NEW(mp) CStatsPredPoint(
		90, CStatsPred::EstatscmptG, CTestUtils::PpointInt4(mp, 0)));
	pdrspqstatspredDisj->Append(SPQOS_NEW(mp)
								   CStatsPredConj(pdrspqstatspredConj3));

	// predicate is a == 'w' AND b == 2002
	CStatsPredPtrArry *pdrspqstatspredConj4 = SPQOS_NEW(mp) CStatsPredPtrArry(mp);
	pdrspqstatspredConj4->Append(SPQOS_NEW(mp) CStatsPredPoint(
		91, CStatsPred::EstatscmptEq,
		CCardinalityTestUtils::PpointGeneric(mp, SPQDB_TEXT, pstrW, 160621100)));
	pdrspqstatspredConj4->Append(SPQOS_NEW(mp) CStatsPredPoint(
		61, CStatsPred::EstatscmptEq, CTestUtils::PpointInt4(mp, 2002)));
	pdrspqstatspredDisj->Append(SPQOS_NEW(mp)
								   CStatsPredConj(pdrspqstatspredConj4));

	SPQOS_DELETE(pstrS);
	SPQOS_DELETE(pstrW);

	return SPQOS_NEW(mp) CStatsPredDisj(pdrspqstatspredDisj);
}

// create nested AND and OR predicates where the AND and OR predicates share common columns
CStatsPred *
CFilterCardinalityTest::PstatspredDisjOverConjDifferentCol1(CMemoryPool *mp)
{
	CStatsPredPtrArry *pdrspqstatspredConj = SPQOS_NEW(mp) CStatsPredPtrArry(mp);

	// predicate col_1 = 3 AND col_2 >=3
	pdrspqstatspredConj->Append(SPQOS_NEW(mp) CStatsPredPoint(
		1, CStatsPred::EstatscmptEq, CTestUtils::PpointInt4(mp, 3)));
	pdrspqstatspredConj->Append(SPQOS_NEW(mp) CStatsPredPoint(
		2, CStatsPred::EstatscmptGEq, CTestUtils::PpointInt4(mp, 3)));

	CStatsPredConj *conjunctive_pred_stats =
		SPQOS_NEW(mp) CStatsPredConj(pdrspqstatspredConj);

	// predicate (col_1 = 1);
	CStatsPredPtrArry *pdrspqstatspredDisj = SPQOS_NEW(mp) CStatsPredPtrArry(mp);
	pdrspqstatspredDisj->Append(SPQOS_NEW(mp) CStatsPredPoint(
		1, CStatsPred::EstatscmptEq, CTestUtils::PpointInt4(mp, 1)));
	pdrspqstatspredDisj->Append(conjunctive_pred_stats);

	return SPQOS_NEW(mp) CStatsPredDisj(pdrspqstatspredDisj);
}

// create nested AND and OR predicates where the AND and OR predicates
CStatsPred *
CFilterCardinalityTest::PstatspredDisjOverConjMultipleIdenticalCols(
	CMemoryPool *mp)
{
	CStatsPredPtrArry *pdrspqstatspredConj1 = SPQOS_NEW(mp) CStatsPredPtrArry(mp);

	// predicate col_1 = 1 AND col_2 = 1
	pdrspqstatspredConj1->Append(SPQOS_NEW(mp) CStatsPredPoint(
		1, CStatsPred::EstatscmptEq, CTestUtils::PpointInt4(mp, 1)));
	pdrspqstatspredConj1->Append(SPQOS_NEW(mp) CStatsPredPoint(
		2, CStatsPred::EstatscmptEq, CTestUtils::PpointInt4(mp, 1)));

	CStatsPredConj *pstatspredConj1 =
		SPQOS_NEW(mp) CStatsPredConj(pdrspqstatspredConj1);

	CStatsPredPtrArry *pdrspqstatspredConj2 = SPQOS_NEW(mp) CStatsPredPtrArry(mp);

	// predicate col_1 = 2 AND col_2 = 2
	pdrspqstatspredConj2->Append(SPQOS_NEW(mp) CStatsPredPoint(
		1, CStatsPred::EstatscmptEq, CTestUtils::PpointInt4(mp, 2)));
	pdrspqstatspredConj2->Append(SPQOS_NEW(mp) CStatsPredPoint(
		2, CStatsPred::EstatscmptEq, CTestUtils::PpointInt4(mp, 2)));

	CStatsPredConj *pstatspredConj2 =
		SPQOS_NEW(mp) CStatsPredConj(pdrspqstatspredConj2);
	CStatsPredPtrArry *pdrspqstatspredDisj = SPQOS_NEW(mp) CStatsPredPtrArry(mp);

	pdrspqstatspredDisj->Append(pstatspredConj1);
	pdrspqstatspredDisj->Append(pstatspredConj2);

	return SPQOS_NEW(mp) CStatsPredDisj(pdrspqstatspredDisj);
}

// reads a DXL document, generates the statistics object, performs a
// filter operation on it, serializes it into a DXL document and
// compares the generated DXL document with the expected DXL document.
SPQOS_RESULT
CFilterCardinalityTest::EresUnittest_CStatisticsBasicsFromDXLNumeric()
{
	// create memory pool
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	SStatsCmpValElem rgStatsCmpValElem[] = {
		{CStatsPred::EstatscmptL, SPQOS_WSZ_LIT("AAAACgAAAgABAA=="),
		 CDouble(1.0)},
		{CStatsPred::EstatscmptLEq, SPQOS_WSZ_LIT("AAAACgAAAgABAA=="),
		 CDouble(1.0)},
		{CStatsPred::EstatscmptEq, SPQOS_WSZ_LIT("AAAACgAAAgABAA=="),
		 CDouble(1.0)},

		{CStatsPred::EstatscmptG, SPQOS_WSZ_LIT("AAAACgAAAgAyAA=="),
		 CDouble(50.0)},
		{CStatsPred::EstatscmptGEq, SPQOS_WSZ_LIT("AAAACgAAAgAyAA=="),
		 CDouble(50.0)},
		{CStatsPred::EstatscmptEq, SPQOS_WSZ_LIT("AAAACgAAAgAyAA=="),
		 CDouble(50.0)},

		{CStatsPred::EstatscmptL, SPQOS_WSZ_LIT("AAAACgAAAgABAA=="),
		 CDouble(1.0)},
		{CStatsPred::EstatscmptLEq, SPQOS_WSZ_LIT("AAAACgAAAgABAA=="),
		 CDouble(1.0)},
		{CStatsPred::EstatscmptEq, SPQOS_WSZ_LIT("AAAACgAAAgABAA=="),
		 CDouble(1.0)},
		{CStatsPred::EstatscmptG, SPQOS_WSZ_LIT("AAAACgAAAgABAA=="),
		 CDouble(1.0)},
		{CStatsPred::EstatscmptGEq, SPQOS_WSZ_LIT("AAAACgAAAgABAA=="),
		 CDouble(1.0)},

		{CStatsPred::EstatscmptL, SPQOS_WSZ_LIT("AAAACgAAAgAyAA=="),
		 CDouble(50.0)},
		{CStatsPred::EstatscmptLEq, SPQOS_WSZ_LIT("AAAACgAAAgAyAA=="),
		 CDouble(50.0)},
		{CStatsPred::EstatscmptG, SPQOS_WSZ_LIT("AAAACgAAAgAyAA=="),
		 CDouble(50.0)},
		{CStatsPred::EstatscmptGEq, SPQOS_WSZ_LIT("AAAACgAAAgAyAA=="),
		 CDouble(50.0)},
		{CStatsPred::EstatscmptEq, SPQOS_WSZ_LIT("AAAACgAAAgAyAA=="),
		 CDouble(50.0)},
	};

	const ULONG length = SPQOS_ARRAY_SIZE(rgStatsCmpValElem);
	SPQOS_ASSERT(length == SPQOS_ARRAY_SIZE(rgtcStatistics));
	for (ULONG ul = 0; ul < length; ul++)
	{
		// read input DXL file
		CHAR *szDXLInput = CDXLUtils::Read(mp, rgtcStatistics[ul].szInputFile);
		// read output DXL file
		CHAR *szDXLOutput =
			CDXLUtils::Read(mp, rgtcStatistics[ul].szOutputFile);

		SPQOS_CHECK_ABORT;

		CMDAccessor *md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();
		// parse the statistics objects
		CDXLStatsDerivedRelationArray *dxl_derived_rel_stats_array =
			CDXLUtils::ParseDXLToStatsDerivedRelArray(mp, szDXLInput, NULL);
		CStatisticsArray *pdrspqstatBefore =
			CDXLUtils::ParseDXLToOptimizerStatisticObjArray(
				mp, md_accessor, dxl_derived_rel_stats_array);
		dxl_derived_rel_stats_array->Release();

		SPQOS_ASSERT(NULL != pdrspqstatBefore);

		SPQOS_CHECK_ABORT;

		SStatsCmpValElem statsCmpValElem = rgStatsCmpValElem[ul];

		CStatsPredPtrArry *pdrspqstatspred =
			PdrspqpredfilterNumeric(mp, 1 /*colid*/, statsCmpValElem);
		CStatsPredConj *pred_stats =
			SPQOS_NEW(mp) CStatsPredConj(pdrspqstatspred);
		SPQOS_RESULT eres = EresUnittest_CStatisticsCompare(
			mp, md_accessor, pdrspqstatBefore, pred_stats, szDXLOutput,
			true /*fApplyTwice*/
		);

		// clean up
		pdrspqstatBefore->Release();
		pred_stats->Release();
		SPQOS_DELETE_ARRAY(szDXLInput);
		SPQOS_DELETE_ARRAY(szDXLOutput);

		if (SPQOS_OK != eres)
		{
			return eres;
		}
	}

	return SPQOS_OK;
}

// generate an array of filter given a column identifier, comparison type,
// and array of integer point
CStatsPredPtrArry *
CFilterCardinalityTest::PdrspqstatspredInteger(
	CMemoryPool *mp, ULONG colid, CStatsPred::EStatsCmpType stats_cmp_type,
	INT *piVals, ULONG ulVals)
{
	SPQOS_ASSERT(0 < ulVals);

	CStatsPredPtrArry *pdrspqstatspred = SPQOS_NEW(mp) CStatsPredPtrArry(mp);
	for (ULONG ul = 0; ul < ulVals; ul++)
	{
		pdrspqstatspred->Append(SPQOS_NEW(mp) CStatsPredPoint(
			colid, stats_cmp_type, CTestUtils::PpointInt4(mp, piVals[ul])));
	}

	return pdrspqstatspred;
}

// generate a numeric filter on the column specified and the literal value
CStatsPredPtrArry *
CFilterCardinalityTest::PdrspqpredfilterNumeric(CMemoryPool *mp, ULONG colid,
											   SStatsCmpValElem statsCmpValElem)
{
	// create a filter
	CStatsPredPtrArry *pdrspqstatspred = SPQOS_NEW(mp) CStatsPredPtrArry(mp);
	CWStringDynamic *pstrNumeric =
		SPQOS_NEW(mp) CWStringDynamic(mp, statsCmpValElem.m_wsz);
	CStatsPredPoint *pred_stats = SPQOS_NEW(mp)
		CStatsPredPoint(colid, statsCmpValElem.m_stats_cmp_type,
						CCardinalityTestUtils::PpointNumeric(
							mp, pstrNumeric, statsCmpValElem.m_value));
	pdrspqstatspred->Append(pred_stats);
	SPQOS_DELETE(pstrNumeric);

	return pdrspqstatspred;
}

// reads a DXL document, generates the statistics object, performs a
// filter operation on it, serializes it into a DXL document and
// compares the generated DXL document with the expected DXL document.
SPQOS_RESULT
CFilterCardinalityTest::EresUnittest_CStatisticsBasicsFromDXL()
{
	// create memory pool
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	CWStringDynamic str(mp);
	COstreamString oss(&str);

	// read input DXL file
	CHAR *szDXLInput = CDXLUtils::Read(mp, szInputDXLFileName);
	// read output DXL file
	CHAR *szDXLOutput = CDXLUtils::Read(mp, szOutputDXLFileName);

	SPQOS_CHECK_ABORT;

	CMDAccessor *md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();

	// parse the statistics objects
	CDXLStatsDerivedRelationArray *dxl_derived_rel_stats_array =
		CDXLUtils::ParseDXLToStatsDerivedRelArray(mp, szDXLInput, NULL);
	CStatisticsArray *pdrspqstatsBefore =
		CDXLUtils::ParseDXLToOptimizerStatisticObjArray(
			mp, md_accessor, dxl_derived_rel_stats_array);
	dxl_derived_rel_stats_array->Release();
	SPQOS_ASSERT(NULL != pdrspqstatsBefore);

	SPQOS_CHECK_ABORT;

	// create a filter
	CStatsPredConj *pred_stats =
		SPQOS_NEW(mp) CStatsPredConj(CStatisticsTest::Pdrspqstatspred2(mp));
	SPQOS_RESULT eres = EresUnittest_CStatisticsCompare(
		mp, md_accessor, pdrspqstatsBefore, pred_stats, szDXLOutput);

	// clean up
	pdrspqstatsBefore->Release();
	pred_stats->Release();
	SPQOS_DELETE_ARRAY(szDXLInput);
	SPQOS_DELETE_ARRAY(szDXLOutput);

	return eres;
}

// performs a filter operation on it, serializes it into a DXL document
// and compares the generated DXL document with the expected DXL document
SPQOS_RESULT
CFilterCardinalityTest::EresUnittest_CStatisticsCompare(
	CMemoryPool *mp, CMDAccessor *md_accessor,
	CStatisticsArray *pdrspqstatBefore, CStatsPred *pred_stats,
	const CHAR *szDXLOutput, BOOL fApplyTwice)
{
	CWStringDynamic str(mp);
	COstreamString oss(&str);

	CStatistics *input_stats = (*pdrspqstatBefore)[0];

	SPQOS_TRACE(SPQOS_WSZ_LIT("Statistics before"));
	CCardinalityTestUtils::PrintStats(mp, input_stats);

	CStatistics *pstatsOutput = CFilterStatsProcessor::MakeStatsFilter(
		mp, input_stats, pred_stats, true /* do_cap_NDVs */);

	SPQOS_TRACE(SPQOS_WSZ_LIT("Statistics after"));
	CCardinalityTestUtils::PrintStats(mp, pstatsOutput);

	// output array of stats objects
	CStatisticsArray *pdrspqstatOutput = SPQOS_NEW(mp) CStatisticsArray(mp);
	pdrspqstatOutput->Append(pstatsOutput);

	oss << "Serializing Input Statistics Objects (Before Filter)" << std::endl;
	CWStringDynamic *pstrInput = CDXLUtils::SerializeStatistics(
		mp, md_accessor, pdrspqstatBefore, true /*serialize_header_footer*/,
		true /*indentation*/
	);
	SPQOS_TRACE(pstrInput->GetBuffer());
	SPQOS_DELETE(pstrInput);

	oss << "Serializing Output Statistics Objects (After Filter)" << std::endl;
	CWStringDynamic *pstrOutput = CDXLUtils::SerializeStatistics(
		mp, md_accessor, pdrspqstatOutput, true /*serialize_header_footer*/,
		true /*indentation*/
	);
	SPQOS_TRACE(pstrOutput->GetBuffer());

	CWStringDynamic dstrExpected(mp);
	dstrExpected.AppendFormat(SPQOS_WSZ_LIT("%s"), szDXLOutput);

	SPQOS_RESULT eres = CTestUtils::EresCompare(
		oss, pstrOutput, &dstrExpected, false /* ignore mismatch */
	);

	if (fApplyTwice && SPQOS_OK == eres)
	{
		CStatistics *pstatsOutput2 = CFilterStatsProcessor::MakeStatsFilter(
			mp, pstatsOutput, pred_stats, true /* do_cap_NDVs */);
		pstatsOutput2->Rows();
		SPQOS_TRACE(SPQOS_WSZ_LIT("Statistics after another filter"));
		CCardinalityTestUtils::PrintStats(mp, pstatsOutput2);

		// output array of stats objects
		CStatisticsArray *pdrspqstatOutput2 = SPQOS_NEW(mp) CStatisticsArray(mp);
		pdrspqstatOutput2->Append(pstatsOutput2);

		CWStringDynamic *pstrOutput2 = CDXLUtils::SerializeStatistics(
			mp, md_accessor, pdrspqstatOutput2, true /*serialize_header_footer*/,
			true /*indentation*/
		);
		eres = CTestUtils::EresCompare(
			oss, pstrOutput2, &dstrExpected, false /* ignore mismatch */
		);

		pdrspqstatOutput2->Release();
		SPQOS_DELETE(pstrOutput2);
	}

	pdrspqstatOutput->Release();
	SPQOS_DELETE(pstrOutput);

	return eres;
}

// test for accumulating cardinality in disjunctive and conjunctive predicates
SPQOS_RESULT
CFilterCardinalityTest::EresUnittest_CStatisticsAccumulateCard()
{
	// create memory pool
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	// create hash map from colid -> histogram
	UlongToHistogramMap *col_histogram_mapping =
		SPQOS_NEW(mp) UlongToHistogramMap(mp);

	// array capturing columns for which width information is available
	UlongToDoubleMap *colid_width_mapping = SPQOS_NEW(mp) UlongToDoubleMap(mp);

	const ULONG num_cols = 3;
	for (ULONG ul = 0; ul < num_cols; ul++)
	{
		// generate histogram of the form [0, 10), [10, 20), [20, 30), [80, 90), [100,100]
		col_histogram_mapping->Insert(
			SPQOS_NEW(mp) ULONG(ul),
			CCardinalityTestUtils::PhistExampleInt4(mp));

		// width for int
		colid_width_mapping->Insert(SPQOS_NEW(mp) ULONG(ul),
									SPQOS_NEW(mp) CDouble(4.0));
	}

	CStatistics *stats = SPQOS_NEW(mp)
		CStatistics(mp, col_histogram_mapping, colid_width_mapping,
					CDouble(1000.0) /* rows */, false /* is_empty() */
		);
	SPQOS_TRACE(SPQOS_WSZ_LIT("\nOriginal Stats:\n"));
	CCardinalityTestUtils::PrintStats(mp, stats);

	// (1)
	// create disjunctive filter
	CStatsPredPtrArry *pdrspqstatspred = SPQOS_NEW(mp) CStatsPredPtrArry(mp);
	pdrspqstatspred->Append(SPQOS_NEW(mp) CStatsPredPoint(
		0, CStatsPred::EstatscmptEq, CTestUtils::PpointInt4(mp, 5)));
	pdrspqstatspred->Append(SPQOS_NEW(mp) CStatsPredPoint(
		1, CStatsPred::EstatscmptEq, CTestUtils::PpointInt4(mp, 200)));
	pdrspqstatspred->Append(SPQOS_NEW(mp) CStatsPredPoint(
		2, CStatsPred::EstatscmptEq, CTestUtils::PpointInt4(mp, 200)));
	CStatsPredDisj *disjunctive_pred_stats =
		SPQOS_NEW(mp) CStatsPredDisj(pdrspqstatspred);

	// apply filter and print resulting stats
	CStatistics *pstats1 = CFilterStatsProcessor::MakeStatsFilter(
		mp, stats, disjunctive_pred_stats, true /* do_cap_NDVs */);
	CDouble num_rows1 = pstats1->Rows();
	SPQOS_TRACE(SPQOS_WSZ_LIT(
		"\n\nStats after disjunctive filter [Col0=5 OR Col1=200 OR Col2=200]:\n"));
	CCardinalityTestUtils::PrintStats(mp, pstats1);

	disjunctive_pred_stats->Release();

	// (2)
	// create point filter
	CStatsPredPtrArry *pdrspqstatspred1 = SPQOS_NEW(mp) CStatsPredPtrArry(mp);
	pdrspqstatspred1->Append(SPQOS_NEW(mp) CStatsPredPoint(
		0, CStatsPred::EstatscmptEq, CTestUtils::PpointInt4(mp, 5)));
	CStatsPredConj *pstatspredConj1 =
		SPQOS_NEW(mp) CStatsPredConj(pdrspqstatspred1);

	// apply filter and print resulting stats
	CStatistics *pstats2 = CFilterStatsProcessor::MakeStatsFilter(
		mp, stats, pstatspredConj1, true /* do_cap_NDVs */);
	CDouble num_rows2 = pstats2->Rows();
	SPQOS_TRACE(SPQOS_WSZ_LIT("\n\nStats after point filter [Col0=5]:\n"));
	CCardinalityTestUtils::PrintStats(mp, pstats2);

	pstatspredConj1->Release();

	SPQOS_RTL_ASSERT(
		num_rows1 - num_rows2 < 10 &&
		"Disjunctive filter and point filter have very different row estimates");

	// (3)
	// create conjunctive filter
	CStatsPredPtrArry *pdrspqstatspred2 = SPQOS_NEW(mp) CStatsPredPtrArry(mp);
	pdrspqstatspred2->Append(SPQOS_NEW(mp) CStatsPredPoint(
		0, CStatsPred::EstatscmptEq, CTestUtils::PpointInt4(mp, 5)));
	pdrspqstatspred2->Append(SPQOS_NEW(mp) CStatsPredPoint(
		1, CStatsPred::EstatscmptEq, CTestUtils::PpointInt4(mp, 200)));
	pdrspqstatspred2->Append(SPQOS_NEW(mp) CStatsPredPoint(
		2, CStatsPred::EstatscmptEq, CTestUtils::PpointInt4(mp, 200)));

	CStatsPredConj *pstatspredConj2 =
		SPQOS_NEW(mp) CStatsPredConj(pdrspqstatspred2);

	// apply filter and print resulting stats
	CStatistics *pstats3 = CFilterStatsProcessor::MakeStatsFilter(
		mp, stats, pstatspredConj2, true /* do_cap_NDVs */);
	CDouble dRows3 = pstats3->Rows();
	SPQOS_TRACE(SPQOS_WSZ_LIT(
		"\n\nStats after conjunctive filter [Col0=5 AND Col1=200 AND Col2=200]:\n"));
	CCardinalityTestUtils::PrintStats(mp, pstats3);

	pstatspredConj2->Release();
	SPQOS_RTL_ASSERT(
		dRows3 < num_rows2 &&
		"Conjunctive filter passes more rows than than point filter");

	// (4)
	// create selective disjunctive filter that pass no rows
	CStatsPredPtrArry *pdrspqstatspred3 = SPQOS_NEW(mp) CStatsPredPtrArry(mp);
	pdrspqstatspred3->Append(SPQOS_NEW(mp) CStatsPredPoint(
		1, CStatsPred::EstatscmptEq, CTestUtils::PpointInt4(mp, 200)));
	pdrspqstatspred3->Append(SPQOS_NEW(mp) CStatsPredPoint(
		2, CStatsPred::EstatscmptEq, CTestUtils::PpointInt4(mp, 200)));
	CStatsPredDisj *pstatspredDisj1 =
		SPQOS_NEW(mp) CStatsPredDisj(pdrspqstatspred3);

	// apply filter and print resulting stats
	CStatistics *pstats4 = CFilterStatsProcessor::MakeStatsFilter(
		mp, stats, pstatspredDisj1, true /* do_cap_NDVs */);
	CDouble dRows4 = pstats4->Rows();
	SPQOS_TRACE(SPQOS_WSZ_LIT(
		"\n\nStats after disjunctive filter [Col1=200 OR Col2=200]:\n"));
	CCardinalityTestUtils::PrintStats(mp, pstats4);

	pstatspredDisj1->Release();

	SPQOS_RTL_ASSERT(
		dRows4 < num_rows2 &&
		"Selective disjunctive filter passes more rows than than point filter");

	// (5)
	// create selective conjunctive filter that pass no rows
	CStatsPredPtrArry *pdrspqstatspred4 = SPQOS_NEW(mp) CStatsPredPtrArry(mp);
	pdrspqstatspred4->Append(SPQOS_NEW(mp) CStatsPredPoint(
		0, CStatsPred::EstatscmptEq, CTestUtils::PpointInt4(mp, 5)));
	pdrspqstatspred4->Append(SPQOS_NEW(mp) CStatsPredPoint(
		1, CStatsPred::EstatscmptEq, CTestUtils::PpointInt4(mp, 200)));
	CStatsPredConj *pstatspredConj3 =
		SPQOS_NEW(mp) CStatsPredConj(pdrspqstatspred4);

	// apply filter and print resulting stats
	CStatistics *pstats5 = CFilterStatsProcessor::MakeStatsFilter(
		mp, stats, pstatspredConj3, true /* do_cap_NDVs */);
	CDouble dRows5 = pstats5->Rows();
	SPQOS_TRACE(SPQOS_WSZ_LIT(
		"\n\nStats after conjunctive filter [Col0=5 AND Col1=200]:\n"));
	CCardinalityTestUtils::PrintStats(mp, pstats5);

	pstatspredConj3->Release();

	SPQOS_RTL_ASSERT(
		dRows5 < num_rows2 &&
		"Selective conjunctive filter passes more rows than than point filter");

	// clean up
	stats->Release();
	pstats1->Release();
	pstats2->Release();
	pstats3->Release();
	pstats4->Release();
	pstats5->Release();

	return SPQOS_OK;
}

// EOF
