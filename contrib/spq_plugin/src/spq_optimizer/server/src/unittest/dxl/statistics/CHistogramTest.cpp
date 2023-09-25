//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2018 Pivotal Inc.
//
//	@filename:
//		CHistogramTest.cpp
//
//	@doc:
//		Testing operations on histogram objects
//---------------------------------------------------------------------------

#ifndef __STDC_CONSTANT_MACROS
#define __STDC_CONSTANT_MACROS
#endif

#include "unittest/dxl/statistics/CHistogramTest.h"

#include <stdint.h>

#include "spqos/error/CAutoTrace.h"
#include "spqos/io/COstreamString.h"
#include "spqos/string/CWStringDynamic.h"

#include "naucrates/statistics/CHistogram.h"
#include "naucrates/statistics/CPoint.h"

#include "unittest/base.h"
#include "unittest/dxl/statistics/CCardinalityTestUtils.h"
#include "unittest/spqopt/CTestUtils.h"

using namespace spqopt;

// unittest for statistics objects
SPQOS_RESULT
CHistogramTest::EresUnittest()
{
	// tests that use shared optimization context
	CUnittest rgutSharedOptCtxt[] = {
		SPQOS_UNITTEST_FUNC(CHistogramTest::EresUnittest_CHistogramInt4),
		SPQOS_UNITTEST_FUNC(CHistogramTest::EresUnittest_CHistogramBool),
		SPQOS_UNITTEST_FUNC(CHistogramTest::EresUnittest_Skew),
		SPQOS_UNITTEST_FUNC(CHistogramTest::EresUnittest_CHistogramValid),
		SPQOS_UNITTEST_FUNC(CHistogramTest::EresUnittest_MergeUnion),
		SPQOS_UNITTEST_FUNC(
			CHistogramTest::EresUnittest_MergeUnionDoubleLessThanEpsilon)};


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

// histogram of int4
SPQOS_RESULT
CHistogramTest::EresUnittest_CHistogramInt4()
{
	// create memory pool
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	// original histogram
	CHistogram *histogram = CCardinalityTestUtils::PhistExampleInt4(mp);
	CCardinalityTestUtils::PrintHist(mp, "histogram", histogram);

	// test edge case of MakeBucketGreaterThan
	CPoint *ppoint0 = CTestUtils::PpointInt4(mp, 9);
	CHistogram *phist0 =
		histogram->MakeHistogramFilter(CStatsPred::EstatscmptG, ppoint0);
	CCardinalityTestUtils::PrintHist(mp, "phist0", phist0);
	SPQOS_RTL_ASSERT(phist0->GetNumBuckets() == 9);

	CPoint *point1 = CTestUtils::PpointInt4(mp, 35);
	CHistogram *histogram1 =
		histogram->MakeHistogramFilter(CStatsPred::EstatscmptL, point1);
	CCardinalityTestUtils::PrintHist(mp, "histogram1", histogram1);
	SPQOS_RTL_ASSERT(histogram1->GetNumBuckets() == 4);

	// edge case where point is equal to upper bound
	CPoint *point2 = CTestUtils::PpointInt4(mp, 50);
	CHistogram *histogram2 =
		histogram->MakeHistogramFilter(CStatsPred::EstatscmptL, point2);
	CCardinalityTestUtils::PrintHist(mp, "histogram2", histogram2);
	SPQOS_RTL_ASSERT(histogram2->GetNumBuckets() == 5);

	// equality check
	CPoint *point3 = CTestUtils::PpointInt4(mp, 100);
	CHistogram *phist3 =
		histogram->MakeHistogramFilter(CStatsPred::EstatscmptEq, point3);
	CCardinalityTestUtils::PrintHist(mp, "phist3", phist3);
	SPQOS_RTL_ASSERT(phist3->GetNumBuckets() == 1);

	// normalized output after filter
	CPoint *ppoint4 = CTestUtils::PpointInt4(mp, 100);
	CDouble scale_factor(0.0);
	CHistogram *phist4 = histogram->MakeHistogramFilterNormalize(
		CStatsPred::EstatscmptEq, ppoint4, &scale_factor);
	CCardinalityTestUtils::PrintHist(mp, "phist4", phist4);
	SPQOS_RTL_ASSERT(phist4->IsValid());

	// lasj
	CHistogram *phist5 =
		histogram->MakeLASJHistogram(CStatsPred::EstatscmptEq, histogram2);
	CCardinalityTestUtils::PrintHist(mp, "phist5", phist5);
	SPQOS_RTL_ASSERT(phist5->GetNumBuckets() == 5);

	// inequality check
	CHistogram *phist6 =
		histogram->MakeHistogramFilter(CStatsPred::EstatscmptNEq, point2);
	CCardinalityTestUtils::PrintHist(mp, "phist6", phist6);
	SPQOS_RTL_ASSERT(phist6->GetNumBuckets() == 10);

	// histogram with null fraction and remaining tuples
	CHistogram *phist7 = PhistExampleInt4Remain(mp);
	CCardinalityTestUtils::PrintHist(mp, "phist7", phist7);
	CPoint *ppoint5 = CTestUtils::PpointInt4(mp, 20);

	// equality check, hitting remaining tuples
	CHistogram *phist8 =
		phist7->MakeHistogramFilter(CStatsPred::EstatscmptEq, point3);
	SPQOS_RTL_ASSERT(fabs((phist8->GetFrequency() - 0.2).Get()) <
					CStatistics::Epsilon);
	SPQOS_RTL_ASSERT(fabs((phist8->GetNumDistinct() - 1.0).Get()) <
					CStatistics::Epsilon);

	// greater than, hitting remaining tuples
	CHistogram *phist9 =
		phist7->MakeHistogramFilter(CStatsPred::EstatscmptG, point1);
	CCardinalityTestUtils::PrintHist(mp, "phist9", phist9);
	SPQOS_RTL_ASSERT(fabs((phist9->GetFrequency() - 0.26).Get()) <
					CStatistics::Epsilon);
	SPQOS_RTL_ASSERT(fabs((phist9->GetNumDistinct() - 1.8).Get()) <
					CStatistics::Epsilon);

	// equality join, hitting remaining tuples
	CHistogram *phist10 =
		phist7->MakeJoinHistogram(CStatsPred::EstatscmptEq, phist7);
	SPQOS_RTL_ASSERT(phist10->GetNumBuckets() == 5);
	SPQOS_RTL_ASSERT(fabs((phist10->GetDistinctRemain() - 2.0).Get()) <
					CStatistics::Epsilon);
	SPQOS_RTL_ASSERT(fabs((phist10->GetFreqRemain() - 0.08).Get()) <
					CStatistics::Epsilon);

	// clean up
	ppoint0->Release();
	point1->Release();
	point2->Release();
	point3->Release();
	ppoint4->Release();
	ppoint5->Release();
	SPQOS_DELETE(histogram);
	SPQOS_DELETE(phist0);
	SPQOS_DELETE(histogram1);
	SPQOS_DELETE(histogram2);
	SPQOS_DELETE(phist3);
	SPQOS_DELETE(phist4);
	SPQOS_DELETE(phist5);
	SPQOS_DELETE(phist6);
	SPQOS_DELETE(phist7);
	SPQOS_DELETE(phist8);
	SPQOS_DELETE(phist9);
	SPQOS_DELETE(phist10);

	return SPQOS_OK;
}

// histogram on bool
SPQOS_RESULT
CHistogramTest::EresUnittest_CHistogramBool()
{
	// create memory pool
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	// generate histogram of the form [false, false), [true,true)
	CBucketArray *histogram_buckets = SPQOS_NEW(mp) CBucketArray(mp);
	CBucket *pbucketFalse =
		CCardinalityTestUtils::PbucketSingletonBoolVal(mp, false, 0.1);
	CBucket *pbucketTrue =
		CCardinalityTestUtils::PbucketSingletonBoolVal(mp, false, 0.9);
	histogram_buckets->Append(pbucketFalse);
	histogram_buckets->Append(pbucketTrue);
	CHistogram *histogram = SPQOS_NEW(mp) CHistogram(mp, histogram_buckets);

	// equality check
	CPoint *point1 = CTestUtils::PpointBool(mp, false);
	CDouble scale_factor(0.0);
	CHistogram *histogram1 = histogram->MakeHistogramFilterNormalize(
		CStatsPred::EstatscmptEq, point1, &scale_factor);
	CCardinalityTestUtils::PrintHist(mp, "histogram1", histogram1);
	SPQOS_RTL_ASSERT(histogram1->GetNumBuckets() == 1);

	// clean up
	point1->Release();
	SPQOS_DELETE(histogram);
	SPQOS_DELETE(histogram1);

	return SPQOS_OK;
}


// check for well-formed histogram. Expected to fail
SPQOS_RESULT
CHistogramTest::EresUnittest_CHistogramValid()
{
	// create memory pool
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	CBucketArray *histogram_buckets = SPQOS_NEW(mp) CBucketArray(mp);

	// generate histogram of the form [0, 10), [9, 20)
	CBucket *bucket1 = CCardinalityTestUtils::PbucketIntegerClosedLowerBound(
		mp, 0, 10, 0.1, 2.0);
	histogram_buckets->Append(bucket1);
	CBucket *bucket2 = CCardinalityTestUtils::PbucketIntegerClosedLowerBound(
		mp, 9, 20, 0.1, 2.0);
	histogram_buckets->Append(bucket2);

	// original histogram
	CHistogram *histogram = SPQOS_NEW(mp) CHistogram(mp, histogram_buckets);

	// create an auto object
	CAutoP<CHistogram> ahist;
	ahist = histogram;

	{
		CAutoTrace at(mp);
		at.Os() << std::endl << "Invalid Histogram" << std::endl;
		histogram->OsPrint(at.Os());
	}

	if (histogram->IsValid())
	{
		return SPQOS_FAILED;
	}

	return SPQOS_OK;
}

// generates example int histogram having tuples not covered by buckets,
// including null fraction and nDistinctRemain
CHistogram *
CHistogramTest::PhistExampleInt4Remain(CMemoryPool *mp)
{
	// generate histogram of the form [0, 0], [10, 10], [20, 20] ...
	CBucketArray *histogram_buckets = SPQOS_NEW(mp) CBucketArray(mp);
	for (ULONG idx = 0; idx < 5; idx++)
	{
		INT iLower = INT(idx * 10);
		INT iUpper = iLower;
		CDouble frequency(0.1);
		CDouble distinct(1.0);
		CBucket *bucket = CCardinalityTestUtils::PbucketIntegerClosedLowerBound(
			mp, iLower, iUpper, frequency, distinct);
		histogram_buckets->Append(bucket);
	}

	return SPQOS_NEW(mp)
		CHistogram(mp, histogram_buckets, true, 0.1 /*null_freq*/,
				   2.0 /*distinct_remaining*/, 0.4 /*freq_remaining*/);
}

// basis skew test
SPQOS_RESULT
CHistogramTest::EresUnittest_Skew()
{
	// create memory pool
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	CBucket *bucket1 = CCardinalityTestUtils::PbucketIntegerClosedLowerBound(
		mp, 1, 100, CDouble(0.6), CDouble(100.0));
	CBucket *bucket2 = CCardinalityTestUtils::PbucketIntegerClosedLowerBound(
		mp, 101, 200, CDouble(0.2), CDouble(100.0));
	CBucket *pbucket3 = CCardinalityTestUtils::PbucketIntegerClosedLowerBound(
		mp, 201, 300, CDouble(0.2), CDouble(100.0));
	CBucket *pbucket4 = CCardinalityTestUtils::PbucketIntegerClosedLowerBound(
		mp, 301, 400, CDouble(0.2), CDouble(100.0));
	CBucket *pbucket5 = CCardinalityTestUtils::PbucketIntegerClosedLowerBound(
		mp, 401, 500, CDouble(0.2), CDouble(100.0));
	CBucket *pbucket6 = CCardinalityTestUtils::PbucketIntegerClosedLowerBound(
		mp, 501, 600, CDouble(0.2), CDouble(100.0));
	CBucket *pbucket7 = CCardinalityTestUtils::PbucketIntegerClosedLowerBound(
		mp, 601, 700, CDouble(0.2), CDouble(100.0));
	CBucket *pbucket8 = CCardinalityTestUtils::PbucketIntegerClosedLowerBound(
		mp, 701, 800, CDouble(0.2), CDouble(100.0));

	CBucketArray *pdrspqpbucket1 = SPQOS_NEW(mp) CBucketArray(mp);
	pdrspqpbucket1->Append(bucket1);
	pdrspqpbucket1->Append(bucket2);
	pdrspqpbucket1->Append(pbucket3);
	CHistogram *histogram1 = SPQOS_NEW(mp) CHistogram(mp, pdrspqpbucket1);

	CBucketArray *pdrspqpbucket2 = SPQOS_NEW(mp) CBucketArray(mp);
	pdrspqpbucket2->Append(pbucket4);
	pdrspqpbucket2->Append(pbucket5);
	pdrspqpbucket2->Append(pbucket6);
	pdrspqpbucket2->Append(pbucket7);
	pdrspqpbucket2->Append(pbucket8);
	CHistogram *histogram2 = SPQOS_NEW(mp) CHistogram(mp, pdrspqpbucket2);
	SPQOS_ASSERT(histogram1->GetSkew() > histogram2->GetSkew());

	{
		CAutoTrace at(mp);
		histogram1->OsPrint(at.Os());
		histogram2->OsPrint(at.Os());
	}

	SPQOS_DELETE(histogram1);
	SPQOS_DELETE(histogram2);

	return SPQOS_OK;
}

// basic merge commutativity test
SPQOS_RESULT
CHistogramTest::EresUnittest_MergeUnion()
{
	// create memory pool
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	// 1000 rows
	CBucket *bucket1 = CCardinalityTestUtils::PbucketIntegerClosedLowerBound(
		mp, 1, 100, CDouble(0.6), CDouble(100.0));
	CBucket *bucket2 = CCardinalityTestUtils::PbucketIntegerClosedLowerBound(
		mp, 150, 200, CDouble(0.4), CDouble(100.0));

	// 600 rows
	CBucket *pbucket3 = CCardinalityTestUtils::PbucketIntegerClosedLowerBound(
		mp, 50, 150, CDouble(0.3), CDouble(100.0));
	CBucket *pbucket4 = CCardinalityTestUtils::PbucketIntegerClosedLowerBound(
		mp, 200, 400, CDouble(0.7), CDouble(100.0));

	CBucketArray *pdrspqpbucket1 = SPQOS_NEW(mp) CBucketArray(mp);
	pdrspqpbucket1->Append(bucket1);
	pdrspqpbucket1->Append(bucket2);
	CHistogram *histogram1 = SPQOS_NEW(mp) CHistogram(mp, pdrspqpbucket1);

	CBucketArray *pdrspqpbucket2 = SPQOS_NEW(mp) CBucketArray(mp);
	pdrspqpbucket2->Append(pbucket3);
	pdrspqpbucket2->Append(pbucket4);
	CHistogram *histogram2 = SPQOS_NEW(mp) CHistogram(mp, pdrspqpbucket2);

	CDouble output_rows1(0.0);
	CHistogram *result1 = histogram1->MakeUnionHistogramNormalize(
		1000, histogram2, 600, &output_rows1);


	CDouble output_rows2(0.0);
	CHistogram *result2 = histogram2->MakeUnionHistogramNormalize(
		600, histogram1, 1000, &output_rows2);

	{
		CAutoTrace at(mp);
		result1->OsPrint(at.Os());
		result2->OsPrint(at.Os());
		at.Os() << "Result 1: " << output_rows1 << std::endl;
		at.Os() << "Result 2: " << output_rows2 << std::endl;
	}

	SPQOS_ASSERT(output_rows1 == output_rows2);
	SPQOS_DELETE(histogram1);
	SPQOS_DELETE(histogram2);
	SPQOS_DELETE(result1);
	SPQOS_DELETE(result2);

	return SPQOS_OK;
}

// merge union test with double values differing by less than epsilon
SPQOS_RESULT
CHistogramTest::EresUnittest_MergeUnionDoubleLessThanEpsilon()
{
	// create memory pool
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	// [631.82140700000002, 631.82140700000002]
	CPoint *ppLower1 = CCardinalityTestUtils::PpointDouble(
		mp, SPQDB_FLOAT8, CDouble(631.82140700000002));
	CPoint *ppUpper1 = CCardinalityTestUtils::PpointDouble(
		mp, SPQDB_FLOAT8, CDouble(631.82140700000002));

	CBucket *bucket1 = SPQOS_NEW(mp)
		CBucket(ppLower1, ppUpper1, true /* is_lower_closed */,
				true /*is_upper_closed*/, CDouble(0.2), CDouble(50));

	// (631.82140500000003, 645.05197699999997)
	CPoint *ppLower2 = CCardinalityTestUtils::PpointDouble(
		mp, SPQDB_FLOAT8, CDouble(631.82140500000003));
	CPoint *ppUpper2 = CCardinalityTestUtils::PpointDouble(
		mp, SPQDB_FLOAT8, CDouble(645.05197699999997));
	CBucket *bucket2 = SPQOS_NEW(mp)
		CBucket(ppLower2, ppUpper2, false /* is_lower_closed */,
				false /*is_upper_closed*/, CDouble(0.2), CDouble(50));

	CBucketArray *pdrspqpbucket1 = SPQOS_NEW(mp) CBucketArray(mp);
	pdrspqpbucket1->Append(bucket1);
	CHistogram *histogram1 = SPQOS_NEW(mp) CHistogram(mp, pdrspqpbucket1);

	CBucketArray *pdrspqpbucket2 = SPQOS_NEW(mp) CBucketArray(mp);
	pdrspqpbucket2->Append(bucket2);
	CHistogram *histogram2 = SPQOS_NEW(mp) CHistogram(mp, pdrspqpbucket2);

	CDouble output_rows1(0.0);
	CHistogram *result1 = histogram1->MakeUnionHistogramNormalize(
		1000, histogram2, 600, &output_rows1);


	{
		CAutoTrace at(mp);
		result1->OsPrint(at.Os());
		at.Os() << "Result 1: " << output_rows1 << std::endl;
	}

	SPQOS_DELETE(histogram1);
	SPQOS_DELETE(histogram2);
	SPQOS_DELETE(result1);

	return SPQOS_OK;
}
// EOF
