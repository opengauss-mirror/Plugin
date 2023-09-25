//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CStatisticsTest.cpp
//
//	@doc:
//		Tests for CPoint
//---------------------------------------------------------------------------

#ifndef __STDC_CONSTANT_MACROS
#define __STDC_CONSTANT_MACROS
#endif

#include "unittest/dxl/statistics/CStatisticsTest.h"

#include <stdint.h>

#include "spqos/error/CAutoTrace.h"
#include "spqos/io/COstreamString.h"
#include "spqos/string/CWStringDynamic.h"

#include "spqopt/base/CQueryContext.h"
#include "spqopt/eval/CConstExprEvaluatorDefault.h"
#include "spqopt/operators/CLogicalInnerJoin.h"
#include "spqopt/operators/CScalarProjectElement.h"
#include "naucrates/base/CDatumBoolSPQDB.h"
#include "naucrates/base/CDatumGenericSPQDB.h"
#include "naucrates/base/CDatumInt4SPQDB.h"
#include "naucrates/dxl/CDXLUtils.h"
#include "naucrates/md/CMDTypeGenericSPQDB.h"
#include "naucrates/md/IMDType.h"
#include "naucrates/statistics/CBucket.h"
#include "naucrates/statistics/CFilterStatsProcessor.h"
#include "naucrates/statistics/CGroupByStatsProcessor.h"
#include "naucrates/statistics/CHistogram.h"
#include "naucrates/statistics/CLimitStatsProcessor.h"
#include "naucrates/statistics/CPoint.h"
#include "naucrates/statistics/CStatistics.h"
#include "naucrates/statistics/CStatisticsUtils.h"
#include "naucrates/statistics/CUnionAllStatsProcessor.h"

#include "unittest/base.h"
#include "unittest/dxl/statistics/CCardinalityTestUtils.h"
#include "unittest/spqopt/CTestUtils.h"

using namespace spqopt;

const CHAR *szQuerySelect = "../data/dxl/statistics/SelectQuery.xml";
const CHAR *szPlanSelect = "../data/dxl/statistics/SelectPlan.xml";

// unittest for statistics objects
SPQOS_RESULT
CStatisticsTest::EresUnittest()
{
	// tests that use shared optimization context
	CUnittest rgutSharedOptCtxt[] = {
		SPQOS_UNITTEST_FUNC(CStatisticsTest::EresUnittest_CStatisticsBasic),
		SPQOS_UNITTEST_FUNC(CStatisticsTest::EresUnittest_UnionAll),
		SPQOS_UNITTEST_FUNC(CStatisticsTest::EresUnittest_CStatisticsCopy),

		// TODO,  Mar 18 2013 temporarily disabling the test
		// SPQOS_UNITTEST_FUNC(CStatisticsTest::EresUnittest_CStatisticsSelectDerivation),
	};

	// tests that use separate optimization contexts
	CUnittest rgutSeparateOptCtxt[] = {
		SPQOS_UNITTEST_FUNC(
			CStatisticsTest::EresUnittest_GbAggWithRepeatedGbCols),
	};

	// run tests with shared optimization context first
	SPQOS_RESULT eres = SPQOS_FAILED;
	{
		CAutoMemoryPool amp;
		CMemoryPool *mp = amp.Pmp();

		// setup a file-based provider
		CMDProviderMemory *pmdp = CTestUtils::m_pmdpf;
		pmdp->AddRef();
		CMDAccessor mda(mp, CMDCache::Pcache(), CTestUtils::m_sysidDefault,
						pmdp);

		// install opt context in TLS
		CAutoOptCtxt aoc(mp, &mda, NULL /* pceeval */,
						 CTestUtils::GetCostModel(mp));

		eres = CUnittest::EresExecute(rgutSharedOptCtxt,
									  SPQOS_ARRAY_SIZE(rgutSharedOptCtxt));

		if (SPQOS_FAILED == eres)
		{
			return eres;
		}
	}

	// run tests with separate optimization contexts
	return CUnittest::EresExecute(rgutSeparateOptCtxt,
								  SPQOS_ARRAY_SIZE(rgutSeparateOptCtxt));
}

// testing statistical operations on Union All;
SPQOS_RESULT
CStatisticsTest::EresUnittest_UnionAll()
{
	// create memory pool
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();
	CMDAccessor *md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();

	SStatsUnionAllSTestCase rgstatsunionalltc[] = {
		{"../data/dxl/statistics/UnionAll-Input-1.xml",
		 "../data/dxl/statistics/UnionAll-Output-1.xml"},
	};

	const ULONG ulTestCases = SPQOS_ARRAY_SIZE(rgstatsunionalltc);
	for (ULONG i = 0; i < ulTestCases; i++)
	{
		SStatsUnionAllSTestCase elem = rgstatsunionalltc[i];

		// read input/output DXL file
		CHAR *szDXLInput = CDXLUtils::Read(mp, elem.m_szInputFile);
		CHAR *szDXLOutput = CDXLUtils::Read(mp, elem.m_szOutputFile);

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

		ULongPtrArray *pdrspqulColIdOutput = Pdrspqul(mp, 1);
		ULongPtrArray *pdrspqulColIdInput1 = Pdrspqul(mp, 1);
		ULongPtrArray *pdrspqulColIdInput2 = Pdrspqul(mp, 2);

		CStatistics *pstatsOutput =
			CUnionAllStatsProcessor::CreateStatsForUnionAll(
				mp, pstats1, pstats2, pdrspqulColIdOutput, pdrspqulColIdInput1,
				pdrspqulColIdInput2);

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

// gbAgg test when grouping on repeated columns
SPQOS_RESULT
CStatisticsTest::EresUnittest_GbAggWithRepeatedGbCols()
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	// setup a file-based provider
	CMDProviderMemory *pmdp = CTestUtils::m_pmdpf;
	pmdp->AddRef();
	CMDAccessor mda(mp, CMDCache::Pcache(), CTestUtils::m_sysidDefault, pmdp);

	// install opt context in TLS
	CAutoOptCtxt aoc(mp, &mda, NULL /* pceeval */,
					 CTestUtils::GetCostModel(mp));

	CExpression *pexpr = CTestUtils::PexprLogicalJoin<CLogicalInnerJoin>(mp);
	CColRefSet *colrefs = pexpr->DeriveOutputColumns();

	// create first GbAgg expression: GbAgg on top of given expression
	CColRefArray *pdrspqcr1 = SPQOS_NEW(mp) CColRefArray(mp);
	pdrspqcr1->Append(colrefs->PcrFirst());
	CExpression *pexprGbAgg1 = CUtils::PexprLogicalGbAggGlobal(
		mp, pdrspqcr1, pexpr,
		SPQOS_NEW(mp) CExpression(mp, SPQOS_NEW(mp) CScalarProjectList(mp)));

	// create second GbAgg expression: GbAgg with repeated base column on top of given expression
	CColRefArray *pdrspqcr2 = SPQOS_NEW(mp) CColRefArray(mp);
	pdrspqcr2->Append(colrefs->PcrFirst());
	pdrspqcr2->Append(colrefs->PcrFirst());
	pexpr->AddRef();
	CExpression *pexprGbAgg2 = CUtils::PexprLogicalGbAggGlobal(
		mp, pdrspqcr2, pexpr,
		SPQOS_NEW(mp) CExpression(mp, SPQOS_NEW(mp) CScalarProjectList(mp)));

	// create third GbAgg expression: GbAgg with a repeated projected base column on top of given expression
	pexpr->AddRef();
	CExpression *pexprPrj = CUtils::PexprAddProjection(
		mp, pexpr, CUtils::PexprScalarIdent(mp, colrefs->PcrFirst()));
	CColRef *pcrComputed =
		CScalarProjectElement::PopConvert((*(*pexprPrj)[1])[0]->Pop())->Pcr();
	CColRefArray *pdrspqcr3 = SPQOS_NEW(mp) CColRefArray(mp);
	pdrspqcr3->Append(colrefs->PcrFirst());
	pdrspqcr3->Append(pcrComputed);
	CExpression *pexprGbAgg3 = CUtils::PexprLogicalGbAggGlobal(
		mp, pdrspqcr3, pexprPrj,
		SPQOS_NEW(mp) CExpression(mp, SPQOS_NEW(mp) CScalarProjectList(mp)));

	// derive stats on different GbAgg expressions
	CReqdPropRelational *prprel =
		SPQOS_NEW(mp) CReqdPropRelational(SPQOS_NEW(mp) CColRefSet(mp));
	(void) pexprGbAgg1->PstatsDerive(prprel, NULL /* stats_ctxt */);
	(void) pexprGbAgg2->PstatsDerive(prprel, NULL /* stats_ctxt */);
	(void) pexprGbAgg3->PstatsDerive(prprel, NULL /* stats_ctxt */);

	BOOL fRows1EqualRows2 =
		(pexprGbAgg1->Pstats()->Rows() == pexprGbAgg2->Pstats()->Rows());
	BOOL fRows2EqualRows3 =
		(pexprGbAgg2->Pstats()->Rows() == pexprGbAgg3->Pstats()->Rows());

	{
		CAutoTrace at(mp);
		at.Os() << std::endl
				<< "pexprGbAgg1:" << std::endl
				<< *pexprGbAgg1 << std::endl;
		at.Os() << std::endl
				<< "pexprGbAgg2:" << std::endl
				<< *pexprGbAgg2 << std::endl;
		at.Os() << std::endl
				<< "pexprGbAgg3:" << std::endl
				<< *pexprGbAgg3 << std::endl;
	}

	// cleanup
	pexprGbAgg1->Release();
	pexprGbAgg2->Release();
	pexprGbAgg3->Release();
	prprel->Release();

	if (fRows1EqualRows2 && fRows2EqualRows3)
	{
		return SPQOS_OK;
	}

	return SPQOS_FAILED;
}

// generates example int histogram corresponding to dimension table
CHistogram *
CStatisticsTest::PhistExampleInt4Dim(CMemoryPool *mp)
{
	// generate histogram of the form [0, 10), [10, 20), [20, 30) ... [80, 90)
	CBucketArray *histogram_buckets = SPQOS_NEW(mp) CBucketArray(mp);
	for (ULONG idx = 0; idx < 9; idx++)
	{
		INT iLower = INT(idx * 10);
		INT iUpper = iLower + INT(10);
		CDouble frequency(0.1);
		CDouble distinct(10.0);
		CBucket *bucket = CCardinalityTestUtils::PbucketIntegerClosedLowerBound(
			mp, iLower, iUpper, frequency, distinct);
		histogram_buckets->Append(bucket);
	}

	return SPQOS_NEW(mp) CHistogram(mp, histogram_buckets);
}

// create a table descriptor with two columns having the given names.
CTableDescriptor *
CStatisticsTest::PtabdescTwoColumnSource(CMemoryPool *mp,
										 const CName &nameTable,
										 const IMDTypeInt4 *pmdtype,
										 const CWStringConst &strColA,
										 const CWStringConst &strColB)
{
	CTableDescriptor *ptabdesc = SPQOS_NEW(mp) CTableDescriptor(
		mp, SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidRel, SPQOPT_TEST_REL_OID1, 1, 1),
		nameTable,
		false,	// convert_hash_to_random
		IMDRelation::EreldistrRandom, IMDRelation::ErelstorageHeap,
		0  // ulExecuteAsUser
	);

	for (ULONG i = 0; i < 2; i++)
	{
		// create a shallow constant string to embed in a name
		const CWStringConst *str_name = &strColA;
		if (0 < i)
		{
			str_name = &strColB;
		}
		CName nameColumn(str_name);

		CColumnDescriptor *pcoldesc = SPQOS_NEW(mp)
			CColumnDescriptor(mp, pmdtype, default_type_modifier, nameColumn,
							  i + 1, false /*is_nullable*/
			);
		ptabdesc->AddColumn(pcoldesc);
	}

	return ptabdesc;
}


// bucket statistics test
SPQOS_RESULT
CStatisticsTest::EresUnittest_CStatisticsBucketTest()
{
	// create memory pool
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	CDouble frequency(0.1);
	CDouble distinct(10.0);
	CBucket *bucket1 = CCardinalityTestUtils::PbucketInteger(
		mp, 1, 5, true, true, frequency, distinct);
	CBucket *bucket2 = CCardinalityTestUtils::PbucketInteger(
		mp, 1, 5, false, false, frequency, distinct);

	SPQOS_RESULT eres = SPQOS_OK;

	if (0 == CBucket::CompareLowerBounds(bucket1, bucket2) ||
		(0 == CBucket::CompareUpperBounds(bucket1, bucket2)))
	{
		eres = SPQOS_FAILED;
	}

	delete bucket1;
	delete bucket2;

	return eres;
}

// basic statistics test
SPQOS_RESULT
CStatisticsTest::EresUnittest_CStatisticsBasic()
{
	// create memory pool
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	CColumnFactory *col_factory = COptCtxt::PoctxtFromTLS()->Pcf();

	const IMDTypeInt4 *pmdtypeint4 =
		COptCtxt::PoctxtFromTLS()->Pmda()->PtMDType<IMDTypeInt4>();

	CWStringConst strRelAlias(SPQOS_WSZ_LIT("Rel1"));
	CWStringConst strColA(SPQOS_WSZ_LIT("a"));
	CWStringConst strColB(SPQOS_WSZ_LIT("b"));
	CWStringConst strColC(SPQOS_WSZ_LIT("int4_10"));
	CTableDescriptor *ptabdesc = PtabdescTwoColumnSource(
		mp, CName(&strRelAlias), pmdtypeint4, strColA, strColB);
	CExpression *pexprGet =
		CTestUtils::PexprLogicalGet(mp, ptabdesc, &strRelAlias);

	if (NULL == col_factory->LookupColRef(1 /*id*/))
	{
		// create column references for grouping columns
		(void) col_factory->PcrCreate(
			pmdtypeint4, default_type_modifier, NULL, 0 /* attno */,
			false /*IsNullable*/, 1 /* id */, CName(&strColA),
			pexprGet->Pop()->UlOpId(), false /*IsDistCol*/
		);
	}

	if (NULL == col_factory->LookupColRef(2 /*id*/))
	{
		(void) col_factory->PcrCreate(
			pmdtypeint4, default_type_modifier, NULL, 1 /* attno */,
			false /*IsNullable*/, 2 /* id */, CName(&strColB),
			pexprGet->Pop()->UlOpId(), false /*IsDistCol*/
		);
	}

	if (NULL == col_factory->LookupColRef(10 /*id*/))
	{
		(void) col_factory->PcrCreate(
			pmdtypeint4, default_type_modifier, NULL, 2 /* attno */,
			false /*IsNullable*/, 10 /* id */, CName(&strColC),
			pexprGet->Pop()->UlOpId(), false /*IsDistCol*/
		);
	}

	// create hash map from colid -> histogram
	UlongToHistogramMap *col_histogram_mapping =
		SPQOS_NEW(mp) UlongToHistogramMap(mp);

	// generate bool histogram for column 1
	col_histogram_mapping->Insert(SPQOS_NEW(mp) ULONG(1),
								  CCardinalityTestUtils::PhistExampleBool(mp));

	// generate int histogram for column 2
	col_histogram_mapping->Insert(SPQOS_NEW(mp) ULONG(2),
								  CCardinalityTestUtils::PhistExampleInt4(mp));

	// array capturing columns for which width information is available
	UlongToDoubleMap *colid_width_mapping = SPQOS_NEW(mp) UlongToDoubleMap(mp);

	// width for boolean
	colid_width_mapping->Insert(SPQOS_NEW(mp) ULONG(1),
								SPQOS_NEW(mp) CDouble(1.0));

	// width for int
	colid_width_mapping->Insert(SPQOS_NEW(mp) ULONG(2),
								SPQOS_NEW(mp) CDouble(4.0));

	CStatistics *stats =
		SPQOS_NEW(mp) CStatistics(mp, col_histogram_mapping, colid_width_mapping,
								 1000.0 /* rows */, false /* is_empty */);
	stats->Rows();

	SPQOS_TRACE(SPQOS_WSZ_LIT("stats"));

	// before stats
	CCardinalityTestUtils::PrintStats(mp, stats);

	// create a filter: column 1: [25,45), column 2: [true, true)
	CStatsPredPtrArry *pdrspqstatspred = Pdrspqstatspred1(mp);

	CStatsPredConj *pred_stats = SPQOS_NEW(mp) CStatsPredConj(pdrspqstatspred);
	CStatistics *pstats1 = CFilterStatsProcessor::MakeStatsFilter(
		mp, stats, pred_stats, true /* do_cap_NDVs */);
	pstats1->Rows();

	SPQOS_TRACE(SPQOS_WSZ_LIT("pstats1 after filter"));

	// after stats
	CCardinalityTestUtils::PrintStats(mp, pstats1);

	// create another statistics structure with a single int4 column with id 10
	UlongToHistogramMap *phmulhist2 = SPQOS_NEW(mp) UlongToHistogramMap(mp);
	phmulhist2->Insert(SPQOS_NEW(mp) ULONG(10), PhistExampleInt4Dim(mp));

	UlongToDoubleMap *phmuldoubleWidth2 = SPQOS_NEW(mp) UlongToDoubleMap(mp);
	phmuldoubleWidth2->Insert(SPQOS_NEW(mp) ULONG(10),
							  SPQOS_NEW(mp) CDouble(4.0));

	CStatistics *pstats2 =
		SPQOS_NEW(mp) CStatistics(mp, phmulhist2, phmuldoubleWidth2,
								 100.0 /* rows */, false /* is_empty */);

	SPQOS_TRACE(SPQOS_WSZ_LIT("pstats2"));
	CCardinalityTestUtils::PrintStats(mp, pstats2);

	// join stats with pstats2
	CStatsPredJoin *pstatspredjoin =
		SPQOS_NEW(mp) CStatsPredJoin(2, CStatsPred::EstatscmptEq, 10);
	CStatsPredJoinArray *join_preds_stats =
		SPQOS_NEW(mp) CStatsPredJoinArray(mp);
	join_preds_stats->Append(pstatspredjoin);
	CStatistics *pstats3 =
		stats->CalcInnerJoinStats(mp, pstats2, join_preds_stats);

	SPQOS_TRACE(SPQOS_WSZ_LIT("pstats3 = stats JOIN pstats2 on (col2 = col10)"));
	// after stats
	CCardinalityTestUtils::PrintStats(mp, pstats3);

	// group by stats on columns 1 and 2
	ULongPtrArray *GCs = SPQOS_NEW(mp) ULongPtrArray(mp);
	GCs->Append(SPQOS_NEW(mp) ULONG(1));
	GCs->Append(SPQOS_NEW(mp) ULONG(2));

	ULongPtrArray *aggs = SPQOS_NEW(mp) ULongPtrArray(mp);
	CStatistics *pstats4 = CGroupByStatsProcessor::CalcGroupByStats(
		mp, stats, GCs, aggs, NULL /*keys*/);

	SPQOS_TRACE(SPQOS_WSZ_LIT("pstats4 = stats group by"));
	CCardinalityTestUtils::PrintStats(mp, pstats4);

	// LASJ stats
	CStatistics *pstats5 = stats->CalcLASJoinStats(
		mp, pstats2, join_preds_stats, true /* DoIgnoreLASJHistComputation */);

	SPQOS_TRACE(SPQOS_WSZ_LIT("pstats5 = stats LASJ pstats2 on (col2 = col10)"));
	CCardinalityTestUtils::PrintStats(mp, pstats5);

	// union all
	ULongPtrArray *colids = SPQOS_NEW(mp) ULongPtrArray(mp);
	colids->Append(SPQOS_NEW(mp) ULONG(1));
	colids->Append(SPQOS_NEW(mp) ULONG(2));
	colids->AddRef();
	colids->AddRef();
	colids->AddRef();

	CStatistics *pstats6 = CUnionAllStatsProcessor::CreateStatsForUnionAll(
		mp, stats, stats, colids, colids, colids);

	SPQOS_TRACE(SPQOS_WSZ_LIT("pstats6 = pstats1 union all pstats1"));
	CCardinalityTestUtils::PrintStats(mp, pstats6);

	CStatistics *pstats7 =
		CLimitStatsProcessor::CalcLimitStats(mp, stats, CDouble(4.0));

	SPQOS_TRACE(SPQOS_WSZ_LIT("pstats7 = stats limit 4"));
	CCardinalityTestUtils::PrintStats(mp, pstats7);

	stats->Release();
	pstats1->Release();
	pstats2->Release();
	pstats3->Release();
	pstats4->Release();
	pstats5->Release();
	pstats6->Release();
	pstats7->Release();
	pred_stats->Release();
	join_preds_stats->Release();
	GCs->Release();
	aggs->Release();
	colids->Release();
	pexprGet->Release();

	return SPQOS_OK;
}

// create a filter clause
CStatsPredPtrArry *
CStatisticsTest::Pdrspqstatspred1(CMemoryPool *mp)
{
	CStatsPredPtrArry *pdrspqstatspred = SPQOS_NEW(mp) CStatsPredPtrArry(mp);

	// col1 = true
	StatsFilterBool(mp, 1, true, pdrspqstatspred);

	// col2 >= 25 and col2 < 35
	StatsFilterInt4(mp, 2, 25, 35, pdrspqstatspred);

	return pdrspqstatspred;
}

// create a filter clause
CStatsPredPtrArry *
CStatisticsTest::Pdrspqstatspred2(CMemoryPool *mp)
{
	// contain for filters
	CStatsPredPtrArry *pdrspqstatspred = SPQOS_NEW(mp) CStatsPredPtrArry(mp);

	// create int4 filter column 2: [5,15)::int4
	StatsFilterInt4(mp, 2, 5, 15, pdrspqstatspred);

	// create numeric filter column 3: [1.0, 2.0)::numeric
	CWStringDynamic *pstrLowerNumeric =
		SPQOS_NEW(mp) CWStringDynamic(mp, SPQOS_WSZ_LIT("AAAACgAAAQABAA=="));
	CWStringDynamic *pstrUpperNumeric =
		SPQOS_NEW(mp) CWStringDynamic(mp, SPQOS_WSZ_LIT("AAAACgAAAQACAA=="));

	StatsFilterNumeric(mp, 3, pstrLowerNumeric, pstrUpperNumeric, CDouble(1.0),
					   CDouble(2.0), pdrspqstatspred);

	SPQOS_DELETE(pstrLowerNumeric);
	SPQOS_DELETE(pstrUpperNumeric);

	// create a date filter column 4: ['01-01-2012', '01-21-2012')::date
	CWStringDynamic *pstrLowerDate =
		SPQOS_NEW(mp) CWStringDynamic(mp, SPQOS_WSZ_LIT("HxEAAA=="));
	CWStringDynamic *pstrUpperDate =
		SPQOS_NEW(mp) CWStringDynamic(mp, SPQOS_WSZ_LIT("LREAAA=="));
	LINT lLowerDate = LINT(4383);
	LINT lUpperDate = LINT(4397);
	StatsFilterGeneric(mp, 4, SPQDB_DATE, pstrLowerDate, pstrUpperDate,
					   lLowerDate, lUpperDate, pdrspqstatspred);

	SPQOS_DELETE(pstrLowerDate);
	SPQOS_DELETE(pstrUpperDate);

	// create timestamp filter column 5: ['01-01-2012 00:01:00', '01-01-2012 10:00:00')::timestamp
	CWStringDynamic *pstrLowerTS =
		SPQOS_NEW(mp) CWStringDynamic(mp, SPQOS_WSZ_LIT("ACcI7mpYAQA="));
	CWStringDynamic *pstrUpperTS =
		SPQOS_NEW(mp) CWStringDynamic(mp, SPQOS_WSZ_LIT("AAg5THNYAQA="));
	LINT lLowerTS = LINT(INT64_C(378691260000000));	 // microseconds
	LINT lUpperTS = LINT(INT64_C(378727200000000));	 // microseconds

	StatsFilterGeneric(mp, 5, SPQDB_TIMESTAMP, pstrLowerTS, pstrUpperTS,
					   lLowerTS, lUpperTS, pdrspqstatspred);

	SPQOS_DELETE(pstrLowerTS);
	SPQOS_DELETE(pstrUpperTS);

	return pdrspqstatspred;
}

// create a stats filter on integer range
void
CStatisticsTest::StatsFilterInt4(CMemoryPool *mp, ULONG colid, INT iLower,
								 INT iUpper, CStatsPredPtrArry *pdrspqstatspred)
{
	CStatsPredPoint *pstatspred1 = SPQOS_NEW(mp) CStatsPredPoint(
		colid, CStatsPred::EstatscmptGEq, CTestUtils::PpointInt4(mp, iLower));

	CStatsPredPoint *pstatspred2 = SPQOS_NEW(mp) CStatsPredPoint(
		colid, CStatsPred::EstatscmptL, CTestUtils::PpointInt4(mp, iUpper));

	pdrspqstatspred->Append(pstatspred1);
	pdrspqstatspred->Append(pstatspred2);
}

// create a stats filter on boolean
void
CStatisticsTest::StatsFilterBool(CMemoryPool *mp, ULONG colid, BOOL fValue,
								 CStatsPredPtrArry *pdrspqstatspred)
{
	CStatsPredPoint *pstatspred1 = SPQOS_NEW(mp) CStatsPredPoint(
		colid, CStatsPred::EstatscmptEq, CTestUtils::PpointBool(mp, fValue));

	pdrspqstatspred->Append(pstatspred1);
}

// create a stats filter on numeric types
void
CStatisticsTest::StatsFilterNumeric(CMemoryPool *mp, ULONG colid,
									CWStringDynamic *pstrLowerEncoded,
									CWStringDynamic *pstrUpperEncoded,
									CDouble dValLower, CDouble dValUpper,
									CStatsPredPtrArry *pdrspqstatspred)
{
	CStatsPredPoint *pstatspred1 = SPQOS_NEW(mp) CStatsPredPoint(
		colid, CStatsPred::EstatscmptGEq,
		CCardinalityTestUtils::PpointNumeric(mp, pstrLowerEncoded, dValLower));

	CStatsPredPoint *pstatspred2 = SPQOS_NEW(mp) CStatsPredPoint(
		colid, CStatsPred::EstatscmptL,
		CCardinalityTestUtils::PpointNumeric(mp, pstrUpperEncoded, dValUpper));

	pdrspqstatspred->Append(pstatspred1);
	pdrspqstatspred->Append(pstatspred2);
}

// create a stats filter on other types
void
CStatisticsTest::StatsFilterGeneric(CMemoryPool *mp, ULONG colid, OID oid,
									CWStringDynamic *pstrLowerEncoded,
									CWStringDynamic *pstrUpperEncoded,
									LINT lValueLower, LINT lValueUpper,
									CStatsPredPtrArry *pdrspqstatspred)
{
	CStatsPredPoint *pstatspred1 = SPQOS_NEW(mp)
		CStatsPredPoint(colid, CStatsPred::EstatscmptGEq,
						CCardinalityTestUtils::PpointGeneric(
							mp, oid, pstrLowerEncoded, lValueLower));

	CStatsPredPoint *pstatspred2 = SPQOS_NEW(mp)
		CStatsPredPoint(colid, CStatsPred::EstatscmptL,
						CCardinalityTestUtils::PpointGeneric(
							mp, oid, pstrUpperEncoded, lValueUpper));

	pdrspqstatspred->Append(pstatspred1);
	pdrspqstatspred->Append(pstatspred2);
}

// derivation over select query
SPQOS_RESULT
CStatisticsTest::EresUnittest_CStatisticsSelectDerivation()
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	return CTestUtils::EresTranslate(
		mp, szQuerySelect, szPlanSelect,
		true  // ignore mismatch in output dxl due to column id differences
	);
}

// test that stats copy methods copy all fields
SPQOS_RESULT
CStatisticsTest::EresUnittest_CStatisticsCopy()
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	// create another statistics structure with a single int4 column with id 10
	UlongToHistogramMap *phmulhist = SPQOS_NEW(mp) UlongToHistogramMap(mp);
	phmulhist->Insert(SPQOS_NEW(mp) ULONG(10), PhistExampleInt4Dim(mp));

	UlongToDoubleMap *phmuldoubleWidth = SPQOS_NEW(mp) UlongToDoubleMap(mp);
	phmuldoubleWidth->Insert(SPQOS_NEW(mp) ULONG(10), SPQOS_NEW(mp) CDouble(4.0));

	CStatistics *pstats = SPQOS_NEW(mp) CStatistics(
		mp, phmulhist, phmuldoubleWidth, 100.0 /* rows */, false /* is_empty */,
		ULONG(5) /* relpages */, ULONG(10) /* relallvisible */,
		CDouble(100.0) /* rebinds */, ULONG(3) /* num predicates */);

	IStatistics *stats_copy = pstats->CopyStats(mp);

	UlongToColRefMap *colref_mapping = SPQOS_NEW(mp) UlongToColRefMap(mp);
	IStatistics *stats_copy_remap =
		pstats->CopyStatsWithRemap(mp, colref_mapping, false);

	SPQOS_RESULT eres = SPQOS_OK;
	if (pstats->Width() != stats_copy->Width() ||
		pstats->NumRebinds() != stats_copy->NumRebinds() ||
		pstats->RelPages() != stats_copy->RelPages() ||
		pstats->RelAllVisible() != stats_copy->RelAllVisible() ||
		pstats->GetNumberOfPredicates() != stats_copy->GetNumberOfPredicates())
	{
		eres = SPQOS_FAILED;
	}

	if (pstats->Width() != stats_copy_remap->Width() ||
		pstats->NumRebinds() != stats_copy_remap->NumRebinds() ||
		pstats->RelPages() != stats_copy_remap->RelPages() ||
		pstats->RelAllVisible() != stats_copy_remap->RelAllVisible() ||
		pstats->GetNumberOfPredicates() !=
			stats_copy_remap->GetNumberOfPredicates())
	{
		eres = SPQOS_FAILED;
	}
	stats_copy->Release();
	stats_copy_remap->Release();
	pstats->Release();
	colref_mapping->Release();

	return eres;
}

// EOF
