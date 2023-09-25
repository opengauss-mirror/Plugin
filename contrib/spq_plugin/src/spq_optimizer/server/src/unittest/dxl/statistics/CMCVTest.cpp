//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2018 Pivotal Inc.
//
//	@filename:
//		CHistogramTest.cpp
//
//	@doc:
//		Testing merging most common values (MCV) and histograms
//---------------------------------------------------------------------------

#ifndef __STDC_CONSTANT_MACROS
#define __STDC_CONSTANT_MACROS
#endif

#include "unittest/dxl/statistics/CMCVTest.h"

#include <stdint.h>

#include "spqos/io/COstreamString.h"
#include "spqos/string/CWStringDynamic.h"

#include "naucrates/dxl/CDXLUtils.h"
#include "naucrates/statistics/CHistogram.h"
#include "naucrates/statistics/CPoint.h"
#include "naucrates/statistics/CStatistics.h"
#include "naucrates/statistics/CStatisticsUtils.h"

#include "unittest/base.h"
#include "unittest/dxl/statistics/CCardinalityTestUtils.h"
#include "unittest/spqopt/CTestUtils.h"

using namespace spqopt;

// DXL files
const CHAR *szMCVSortExpectedFileName =
	"../data/dxl/statistics/MCV-Sort-Output.xml";


// unittest for statistics objects
SPQOS_RESULT
CMCVTest::EresUnittest()
{
	// tests that use shared optimization context
	CUnittest rgutSharedOptCtxt[] = {
		SPQOS_UNITTEST_FUNC(CMCVTest::EresUnittest_SortInt4MCVs),
		SPQOS_UNITTEST_FUNC(CMCVTest::EresUnittest_MergeHistMCV),
	};

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

// test sorting of Int4 MCVs and their associated frequencies
SPQOS_RESULT
CMCVTest::EresUnittest_SortInt4MCVs()
{
	// create memory pool
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	CMDAccessor *md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();
	CMDIdSPQDB *mdid = SPQOS_NEW(mp) CMDIdSPQDB(CMDIdSPQDB::m_mdid_int4);
	const IMDType *pmdtype = md_accessor->RetrieveType(mdid);

	// create three integer MCVs
	CPoint *point1 = CTestUtils::PpointInt4(mp, 5);
	CPoint *point2 = CTestUtils::PpointInt4(mp, 1);
	CPoint *point3 = CTestUtils::PpointInt4(mp, 10);
	IDatumArray *pdrspqdatumMCV = SPQOS_NEW(mp) IDatumArray(mp);
	IDatum *datum1 = point1->GetDatum();
	IDatum *datum2 = point2->GetDatum();
	IDatum *datum3 = point3->GetDatum();
	datum1->AddRef();
	datum2->AddRef();
	datum3->AddRef();
	pdrspqdatumMCV->Append(datum1);
	pdrspqdatumMCV->Append(datum2);
	pdrspqdatumMCV->Append(datum3);

	// create corresponding frequencies
	CDoubleArray *pdrspqdFreq = SPQOS_NEW(mp) CDoubleArray(mp);
	// in SPQDB, MCVs are stored in descending frequencies
	pdrspqdFreq->Append(SPQOS_NEW(mp) CDouble(0.4));
	pdrspqdFreq->Append(SPQOS_NEW(mp) CDouble(0.2));
	pdrspqdFreq->Append(SPQOS_NEW(mp) CDouble(0.1));

	// exercise MCV sorting function
	CHistogram *phistMCV = CStatisticsUtils::TransformMCVToHist(
		mp, pmdtype, pdrspqdatumMCV, pdrspqdFreq, pdrspqdatumMCV->Size());

	// create hash map from colid -> histogram
	UlongToHistogramMap *col_histogram_mapping =
		SPQOS_NEW(mp) UlongToHistogramMap(mp);

	// generate int histogram for column 1
	col_histogram_mapping->Insert(SPQOS_NEW(mp) ULONG(1), phistMCV);

	// column width for int4
	UlongToDoubleMap *colid_width_mapping = SPQOS_NEW(mp) UlongToDoubleMap(mp);
	colid_width_mapping->Insert(SPQOS_NEW(mp) ULONG(1),
								SPQOS_NEW(mp) CDouble(4.0));

	CStatistics *stats =
		SPQOS_NEW(mp) CStatistics(mp, col_histogram_mapping, colid_width_mapping,
								 1000.0 /* rows */, false /* is_empty */
		);

	// put stats object in an array in order to serialize
	CStatisticsArray *pdrspqstats = SPQOS_NEW(mp) CStatisticsArray(mp);
	pdrspqstats->Append(stats);

	// serialize stats object
	CWStringDynamic *pstrOutput =
		CDXLUtils::SerializeStatistics(mp, md_accessor, pdrspqstats, true, true);
	SPQOS_TRACE(pstrOutput->GetBuffer());

	// get expected output
	CWStringDynamic str(mp);
	COstreamString oss(&str);
	CHAR *szDXLExpected = CDXLUtils::Read(mp, szMCVSortExpectedFileName);
	CWStringDynamic dstrExpected(mp);
	dstrExpected.AppendFormat(SPQOS_WSZ_LIT("%s"), szDXLExpected);

	SPQOS_RESULT eres =
		CTestUtils::EresCompare(oss, pstrOutput, &dstrExpected,
								false  // mismatch will not be ignored
		);

	// cleanup
	SPQOS_DELETE(pstrOutput);
	SPQOS_DELETE_ARRAY(szDXLExpected);
	pdrspqdatumMCV->Release();
	pdrspqdFreq->Release();
	pdrspqstats->Release();
	point1->Release();
	point2->Release();
	point3->Release();
	mdid->Release();

	return eres;
}

// test merging MCVs and histogram
SPQOS_RESULT
CMCVTest::EresUnittest_MergeHistMCV()
{
	SMergeTestElem rgMergeTestElem[] = {
		{"../data/dxl/statistics/Merge-Input-MCV-Int.xml",
		 "../data/dxl/statistics/Merge-Input-Histogram-Int.xml",
		 "../data/dxl/statistics/Merge-Output-Int.xml"},

		{"../data/dxl/statistics/Merge-Input-MCV-Numeric.xml",
		 "../data/dxl/statistics/Merge-Input-Histogram-Numeric.xml",
		 "../data/dxl/statistics/Merge-Output-Numeric.xml"}};

	// create memory pool
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	ULONG length = SPQOS_ARRAY_SIZE(rgMergeTestElem);
	for (ULONG ul = 0; ul < length; ul++)
	{
		// read input MCVs DXL file
		CHAR *szDXLInputMCV =
			CDXLUtils::Read(mp, rgMergeTestElem[ul].szInputMCVFile);
		// read input histogram DXL file
		CHAR *szDXLInputHist =
			CDXLUtils::Read(mp, rgMergeTestElem[ul].szInputHistFile);

		SPQOS_CHECK_ABORT;

		CMDAccessor *md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();

		// parse the stats objects
		CDXLStatsDerivedRelationArray *pdrspqdxlstatsderrelMCV =
			CDXLUtils::ParseDXLToStatsDerivedRelArray(mp, szDXLInputMCV, NULL);
		CDXLStatsDerivedRelationArray *pdrspqdxlstatsderrelHist =
			CDXLUtils::ParseDXLToStatsDerivedRelArray(mp, szDXLInputHist, NULL);

		SPQOS_CHECK_ABORT;

		CDXLStatsDerivedRelation *pdxlstatsderrelMCV =
			(*pdrspqdxlstatsderrelMCV)[0];
		const CDXLStatsDerivedColumnArray *pdrspqdxlstatsdercolMCV =
			pdxlstatsderrelMCV->GetDXLStatsDerivedColArray();
		CDXLStatsDerivedColumn *pdxlstatsdercolMCV =
			(*pdrspqdxlstatsdercolMCV)[0];
		CBucketArray *pdrspqpbucketMCV = CDXLUtils::ParseDXLToBucketsArray(
			mp, md_accessor, pdxlstatsdercolMCV);
		CHistogram *phistMCV = SPQOS_NEW(mp) CHistogram(mp, pdrspqpbucketMCV);

		CDXLStatsDerivedRelation *pdxlstatsderrelHist =
			(*pdrspqdxlstatsderrelHist)[0];
		const CDXLStatsDerivedColumnArray *pdrspqdxlstatsdercolHist =
			pdxlstatsderrelHist->GetDXLStatsDerivedColArray();
		CDXLStatsDerivedColumn *pdxlstatsdercolHist =
			(*pdrspqdxlstatsdercolHist)[0];
		CBucketArray *pdrspqpbucketHist = CDXLUtils::ParseDXLToBucketsArray(
			mp, md_accessor, pdxlstatsdercolHist);
		CHistogram *phistHist = SPQOS_NEW(mp) CHistogram(mp, pdrspqpbucketHist);

		SPQOS_CHECK_ABORT;

		// exercise the merge
		CHistogram *phistMerged =
			CStatisticsUtils::MergeMCVHist(mp, phistMCV, phistHist);

		// create hash map from colid -> histogram
		UlongToHistogramMap *col_histogram_mapping =
			SPQOS_NEW(mp) UlongToHistogramMap(mp);

		// generate int histogram for column 1
		ULONG colid = pdxlstatsdercolMCV->GetColId();
		col_histogram_mapping->Insert(SPQOS_NEW(mp) ULONG(colid), phistMerged);

		// column width for int4
		UlongToDoubleMap *colid_width_mapping =
			SPQOS_NEW(mp) UlongToDoubleMap(mp);
		CDouble width = pdxlstatsdercolMCV->Width();
		colid_width_mapping->Insert(SPQOS_NEW(mp) ULONG(colid),
									SPQOS_NEW(mp) CDouble(width));

		CStatistics *stats = SPQOS_NEW(mp) CStatistics(
			mp, col_histogram_mapping, colid_width_mapping,
			pdxlstatsderrelMCV->Rows(), pdxlstatsderrelMCV->IsEmpty());

		// put stats object in an array in order to serialize
		CStatisticsArray *pdrspqstats = SPQOS_NEW(mp) CStatisticsArray(mp);
		pdrspqstats->Append(stats);

		// serialize stats object
		CWStringDynamic *pstrOutput = CDXLUtils::SerializeStatistics(
			mp, md_accessor, pdrspqstats, true, true);
		SPQOS_TRACE(pstrOutput->GetBuffer());

		// get expected output
		CWStringDynamic str(mp);
		COstreamString oss(&str);
		CHAR *szDXLExpected =
			CDXLUtils::Read(mp, rgMergeTestElem[ul].szMergedFile);
		CWStringDynamic dstrExpected(mp);
		dstrExpected.AppendFormat(SPQOS_WSZ_LIT("%s"), szDXLExpected);

		SPQOS_RESULT eres =
			CTestUtils::EresCompare(oss, pstrOutput, &dstrExpected,
									false  // mismatch will not be ignored
			);

		// cleanup
		SPQOS_DELETE_ARRAY(szDXLInputMCV);
		SPQOS_DELETE_ARRAY(szDXLInputHist);
		SPQOS_DELETE_ARRAY(szDXLExpected);
		SPQOS_DELETE(pstrOutput);
		pdrspqdxlstatsderrelMCV->Release();
		pdrspqdxlstatsderrelHist->Release();
		SPQOS_DELETE(phistMCV);
		SPQOS_DELETE(phistHist);
		pdrspqstats->Release();

		if (SPQOS_OK != eres)
		{
			return eres;
		}
	}

	return SPQOS_OK;
}

// EOF
