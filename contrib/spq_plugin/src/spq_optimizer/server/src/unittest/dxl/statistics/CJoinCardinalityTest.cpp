//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2017 Pivotal, Inc.
//
//	@filename:
//		CJoinCardinalityTest.cpp
//
//	@doc:
//		Test for join cardinality estimation
//---------------------------------------------------------------------------

#ifndef __STDC_CONSTANT_MACROS
#define __STDC_CONSTANT_MACROS
#endif

#include "unittest/dxl/statistics/CJoinCardinalityTest.h"

#include <stdint.h>

#include "spqos/error/CAutoTrace.h"
#include "spqos/io/COstreamString.h"
#include "spqos/string/CWStringDynamic.h"

#include "naucrates/dxl/CDXLUtils.h"
#include "naucrates/statistics/CStatisticsUtils.h"

#include "unittest/base.h"
#include "unittest/dxl/statistics/CCardinalityTestUtils.h"
#include "unittest/spqopt/CTestUtils.h"

// unittest for join cardinality estimation
SPQOS_RESULT
CJoinCardinalityTest::EresUnittest()
{
	// tests that use shared optimization context
	CUnittest rgutSharedOptCtxt[] = {
		SPQOS_UNITTEST_FUNC(CJoinCardinalityTest::EresUnittest_Join),
		SPQOS_UNITTEST_FUNC(CJoinCardinalityTest::EresUnittest_JoinNDVRemain),
	};

	// run tests with shared optimization context first
	SPQOS_RESULT eres = SPQOS_FAILED;

	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	// setup a file-based provider
	CMDProviderMemory *pmdp = CTestUtils::m_pmdpf;
	pmdp->AddRef();
	CMDAccessor mda(mp, CMDCache::Pcache(), CTestUtils::m_sysidDefault, pmdp);

	// install opt context in TLS
	CAutoOptCtxt aoc(mp, &mda, NULL /* pceeval */,
					 CTestUtils::GetCostModel(mp));

	eres = CUnittest::EresExecute(rgutSharedOptCtxt,
								  SPQOS_ARRAY_SIZE(rgutSharedOptCtxt));

	return eres;
}

//	test join cardinality estimation over histograms with NDVRemain information
SPQOS_RESULT
CJoinCardinalityTest::EresUnittest_JoinNDVRemain()
{
	// create memory pool
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	SHistogramTestCase rghisttc[] = {
		{0, 0, false, 0},	  // empty histogram
		{10, 100, false, 0},  // distinct values only in buckets
		{0, 0, false, 1000},  // distinct values only in NDVRemain
		{5, 100, false,
		 500}  // distinct values spread in both buckets and NDVRemain
	};

	UlongToHistogramMap *col_histogram_mapping =
		SPQOS_NEW(mp) UlongToHistogramMap(mp);

	const ULONG ulHist = SPQOS_ARRAY_SIZE(rghisttc);
	for (ULONG ul1 = 0; ul1 < ulHist; ul1++)
	{
		SHistogramTestCase elem = rghisttc[ul1];

		ULONG num_of_buckets = elem.m_num_of_buckets;
		CDouble dNDVPerBucket = elem.m_dNDVPerBucket;
		BOOL fNullFreq = elem.m_fNullFreq;
		CDouble num_NDV_remain = elem.m_dNDVRemain;

		CHistogram *histogram = CCardinalityTestUtils::PhistInt4Remain(
			mp, num_of_buckets, dNDVPerBucket, fNullFreq, num_NDV_remain);
#ifdef SPQOS_DEBUG
		BOOL result =
#endif	// SPQOS_DEBUG
			col_histogram_mapping->Insert(SPQOS_NEW(mp) ULONG(ul1), histogram);
		SPQOS_ASSERT(result);
	}

	SStatsJoinNDVRemainTestCase rgjoinndvrtc[] = {
		// cases where we are joining with an empty histogram
		// first two columns refer to the histogram entries that are joining
		{0, 0, 0, CDouble(0.0), CDouble(0.0), CDouble(0.0)},
		{0, 1, 0, CDouble(0.0), CDouble(0.0), CDouble(0.0)},
		{0, 2, 0, CDouble(0.0), CDouble(0.0), CDouble(0.0)},
		{0, 3, 0, CDouble(0.0), CDouble(0.0), CDouble(0.0)},

		{1, 0, 0, CDouble(0.0), CDouble(0.0), CDouble(0.0)},
		{2, 0, 0, CDouble(0.0), CDouble(0.0), CDouble(0.0)},
		{3, 0, 0, CDouble(0.0), CDouble(0.0), CDouble(0.0)},

		// cases where one or more input histogram has only buckets and no remaining NDV information
		{1, 1, 10, CDouble(1000.00), CDouble(0.0), CDouble(0.0)},
		{1, 3, 5, CDouble(500.00), CDouble(500.0), CDouble(0.333333)},
		{3, 1, 5, CDouble(500.00), CDouble(500.0), CDouble(0.333333)},

		// cases where for one or more input histogram has only remaining NDV information and no buckets
		{1, 2, 0, CDouble(0.0), CDouble(1000.0), CDouble(1.0)},
		{2, 1, 0, CDouble(0.0), CDouble(1000.0), CDouble(1.0)},
		{2, 2, 0, CDouble(0.0), CDouble(1000.0), CDouble(1.0)},
		{2, 3, 0, CDouble(0.0), CDouble(1000.0), CDouble(1.0)},
		{3, 2, 0, CDouble(0.0), CDouble(1000.0), CDouble(1.0)},

		// cases where both buckets and NDV remain information available for both inputs
		{3, 3, 5, CDouble(500.0), CDouble(500.0), CDouble(0.5)},
	};

	SPQOS_RESULT eres = SPQOS_OK;
	const ULONG ulTestCases = SPQOS_ARRAY_SIZE(rgjoinndvrtc);
	for (ULONG ul2 = 0; ul2 < ulTestCases && (SPQOS_FAILED != eres); ul2++)
	{
		SStatsJoinNDVRemainTestCase elem = rgjoinndvrtc[ul2];
		ULONG colid1 = elem.m_ulCol1;
		ULONG colid2 = elem.m_ulCol2;
		CHistogram *histogram1 = col_histogram_mapping->Find(&colid1);
		CHistogram *histogram2 = col_histogram_mapping->Find(&colid2);

		CHistogram *join_histogram =
			histogram1->MakeJoinHistogram(CStatsPred::EstatscmptEq, histogram2);

		{
			CAutoTrace at(mp);
			at.Os() << std::endl << "Input Histogram 1" << std::endl;
			histogram1->OsPrint(at.Os());
			at.Os() << "Input Histogram 2" << std::endl;
			histogram2->OsPrint(at.Os());
			at.Os() << "Join Histogram" << std::endl;
			join_histogram->OsPrint(at.Os());

			join_histogram->NormalizeHistogram();

			at.Os() << std::endl << "Normalized Join Histogram" << std::endl;
			join_histogram->OsPrint(at.Os());
		}

		ULONG ulBucketsJoin = elem.m_ulBucketsJoin;
		CDouble dNDVBucketsJoin = elem.m_dNDVBucketsJoin;
		CDouble dNDVRemainJoin = elem.m_dNDVRemainJoin;
		CDouble dFreqRemainJoin = elem.m_dFreqRemainJoin;

		CDouble dDiffNDVJoin(fabs(
			(dNDVBucketsJoin -
			 CStatisticsUtils::GetNumDistinct(join_histogram->GetBuckets()))
				.Get()));
		CDouble dDiffNDVRemainJoin(
			fabs((dNDVRemainJoin - join_histogram->GetDistinctRemain()).Get()));
		CDouble dDiffFreqRemainJoin(
			fabs((dFreqRemainJoin - join_histogram->GetFreqRemain()).Get()));

		if (join_histogram->GetNumBuckets() != ulBucketsJoin ||
			(dDiffNDVJoin > CStatistics::Epsilon) ||
			(dDiffNDVRemainJoin > CStatistics::Epsilon) ||
			(dDiffFreqRemainJoin > CStatistics::Epsilon))
		{
			eres = SPQOS_FAILED;
		}

		SPQOS_DELETE(join_histogram);
	}
	// clean up
	col_histogram_mapping->Release();

	return eres;
}

//	join buckets tests
SPQOS_RESULT
CJoinCardinalityTest::EresUnittest_Join()
{
	// create memory pool
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();
	CMDAccessor *md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();

	SStatsJoinSTestCase rgstatsjointc[] = {
		{"../data/dxl/statistics/Join-Statistics-Input.xml",
		 "../data/dxl/statistics/Join-Statistics-Output.xml", false,
		 PdrspqstatspredjoinMultiplePredicates},
		{"../data/dxl/statistics/Join-Statistics-Input-Null-Bucket.xml",
		 "../data/dxl/statistics/Join-Statistics-Output-Null-Bucket.xml", false,
		 PdrspqstatspredjoinNullableCols},
		{"../data/dxl/statistics/LOJ-Input.xml",
		 "../data/dxl/statistics/LOJ-Output.xml", true,
		 PdrspqstatspredjoinNullableCols},
		{"../data/dxl/statistics/Join-Statistics-Input-Only-Nulls.xml",
		 "../data/dxl/statistics/Join-Statistics-Output-Only-Nulls.xml", false,
		 PdrspqstatspredjoinNullableCols},
		{"../data/dxl/statistics/Join-Statistics-Input-Only-Nulls.xml",
		 "../data/dxl/statistics/Join-Statistics-Output-LOJ-Only-Nulls.xml",
		 true, PdrspqstatspredjoinNullableCols},
		{"../data/dxl/statistics/Join-Statistics-DDistinct-Input.xml",
		 "../data/dxl/statistics/Join-Statistics-DDistinct-Output.xml", false,
		 PdrspqstatspredjoinSingleJoinPredicate},
	};

	CColumnFactory *col_factory = COptCtxt::PoctxtFromTLS()->Pcf();
	const IMDTypeInt4 *pmdtypeint4 =
		COptCtxt::PoctxtFromTLS()->Pmda()->PtMDType<IMDTypeInt4>();

	ULongPtrArray *cols = SPQOS_NEW(mp) ULongPtrArray(mp);
	cols->Append(SPQOS_NEW(mp) ULONG(0));
	cols->Append(SPQOS_NEW(mp) ULONG(1));
	cols->Append(SPQOS_NEW(mp) ULONG(2));
	cols->Append(SPQOS_NEW(mp) ULONG(8));
	cols->Append(SPQOS_NEW(mp) ULONG(16));
	cols->Append(SPQOS_NEW(mp) ULONG(31));
	cols->Append(SPQOS_NEW(mp) ULONG(32));
	cols->Append(SPQOS_NEW(mp) ULONG(53));
	cols->Append(SPQOS_NEW(mp) ULONG(54));

	for (ULONG ul = 0; ul < cols->Size(); ul++)
	{
		ULONG id = *((*cols)[ul]);
		if (NULL == col_factory->LookupColRef(id))
		{
			// for this test the col name doesn't matter
			CWStringConst str(SPQOS_WSZ_LIT("col"));
			// create column references for grouping columns
			(void) col_factory->PcrCreate(
				pmdtypeint4, default_type_modifier, NULL, ul /* attno */,
				false /*IsNullable*/, id, CName(&str), false /*IsDistCol*/, 0);
		}
	}
	cols->Release();

	const ULONG ulTestCases = SPQOS_ARRAY_SIZE(rgstatsjointc);
	for (ULONG ul = 0; ul < ulTestCases; ul++)
	{
		SStatsJoinSTestCase elem = rgstatsjointc[ul];

		// read input/output DXL file
		CHAR *szDXLInput = CDXLUtils::Read(mp, elem.m_szInputFile);
		CHAR *szDXLOutput = CDXLUtils::Read(mp, elem.m_szOutputFile);
		BOOL left_outer_join = elem.m_fLeftOuterJoin;

		SPQOS_CHECK_ABORT;

		// parse the input statistics objects
		CDXLStatsDerivedRelationArray *dxl_derived_rel_stats_array =
			CDXLUtils::ParseDXLToStatsDerivedRelArray(mp, szDXLInput, NULL);
		CStatisticsArray *pdrspqstatBefore =
			CDXLUtils::ParseDXLToOptimizerStatisticObjArray(
				mp, md_accessor, dxl_derived_rel_stats_array);
		dxl_derived_rel_stats_array->Release();

		SPQOS_ASSERT(NULL != pdrspqstatBefore);
		SPQOS_ASSERT(2 == pdrspqstatBefore->Size());
		CStatistics *pstats1 = (*pdrspqstatBefore)[0];
		CStatistics *pstats2 = (*pdrspqstatBefore)[1];

		SPQOS_CHECK_ABORT;

		// generate the join conditions
		FnPdrspqstatjoin *pf = elem.m_pf;
		SPQOS_ASSERT(NULL != pf);
		CStatsPredJoinArray *join_preds_stats = pf(mp);

		// calculate the output stats
		CStatistics *pstatsOutput = NULL;
		if (left_outer_join)
		{
			pstatsOutput =
				pstats1->CalcLOJoinStats(mp, pstats2, join_preds_stats);
		}
		else
		{
			pstatsOutput =
				pstats1->CalcInnerJoinStats(mp, pstats2, join_preds_stats);
		}
		SPQOS_ASSERT(NULL != pstatsOutput);

		CStatisticsArray *pdrspqstatOutput = SPQOS_NEW(mp) CStatisticsArray(mp);
		pdrspqstatOutput->Append(pstatsOutput);

		// serialize and compare against expected stats
		CWStringDynamic *pstrOutput = CDXLUtils::SerializeStatistics(
			mp, md_accessor, pdrspqstatOutput, true /*serialize_header_footer*/,
			true /*indentation*/
		);
		CWStringDynamic dstrExpected(mp);
		dstrExpected.AppendFormat(SPQOS_WSZ_LIT("%s"), szDXLOutput);

		SPQOS_RESULT eres = SPQOS_OK;
		CWStringDynamic str(mp);
		COstreamString oss(&str);

		// compare the two dxls
		if (!pstrOutput->Equals(&dstrExpected))
		{
			oss << "Output does not match expected DXL document" << std::endl;
			oss << "Actual: " << std::endl;
			oss << pstrOutput->GetBuffer() << std::endl;
			oss << "Expected: " << std::endl;
			oss << dstrExpected.GetBuffer() << std::endl;
			SPQOS_TRACE(str.GetBuffer());

			eres = SPQOS_FAILED;
		}

		// clean up
		pdrspqstatBefore->Release();
		pdrspqstatOutput->Release();
		join_preds_stats->Release();

		SPQOS_DELETE_ARRAY(szDXLInput);
		SPQOS_DELETE_ARRAY(szDXLOutput);
		SPQOS_DELETE(pstrOutput);

		if (SPQOS_FAILED == eres)
		{
			return eres;
		}
	}

	return SPQOS_OK;
}

//	helper method to generate a single join predicate
CStatsPredJoinArray *
CJoinCardinalityTest::PdrspqstatspredjoinSingleJoinPredicate(CMemoryPool *mp)
{
	CStatsPredJoinArray *join_preds_stats =
		SPQOS_NEW(mp) CStatsPredJoinArray(mp);
	join_preds_stats->Append(
		SPQOS_NEW(mp) CStatsPredJoin(0, CStatsPred::EstatscmptEq, 8));

	return join_preds_stats;
}

//	helper method to generate generate multiple join predicates
CStatsPredJoinArray *
CJoinCardinalityTest::PdrspqstatspredjoinMultiplePredicates(CMemoryPool *mp)
{
	CStatsPredJoinArray *join_preds_stats =
		SPQOS_NEW(mp) CStatsPredJoinArray(mp);
	join_preds_stats->Append(
		SPQOS_NEW(mp) CStatsPredJoin(16, CStatsPred::EstatscmptEq, 32));
	join_preds_stats->Append(
		SPQOS_NEW(mp) CStatsPredJoin(0, CStatsPred::EstatscmptEq, 31));
	join_preds_stats->Append(
		SPQOS_NEW(mp) CStatsPredJoin(54, CStatsPred::EstatscmptEq, 32));
	join_preds_stats->Append(
		SPQOS_NEW(mp) CStatsPredJoin(53, CStatsPred::EstatscmptEq, 31));

	return join_preds_stats;
}

// helper method to generate join predicate over columns that contain null values
CStatsPredJoinArray *
CJoinCardinalityTest::PdrspqstatspredjoinNullableCols(CMemoryPool *mp)
{
	CStatsPredJoinArray *join_preds_stats =
		SPQOS_NEW(mp) CStatsPredJoinArray(mp);
	join_preds_stats->Append(
		SPQOS_NEW(mp) CStatsPredJoin(1, CStatsPred::EstatscmptEq, 2));

	return join_preds_stats;
}

// EOF
