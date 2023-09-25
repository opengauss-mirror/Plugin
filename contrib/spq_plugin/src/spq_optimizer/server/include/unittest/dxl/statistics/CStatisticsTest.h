//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		CStatisticsTest.h
//
//	@doc:
//		Test for CPoint
//---------------------------------------------------------------------------
#ifndef SPQNAUCRATES_CStatisticsTest_H
#define SPQNAUCRATES_CStatisticsTest_H

#include "naucrates/statistics/CBucket.h"
#include "naucrates/statistics/CHistogram.h"
#include "naucrates/statistics/CPoint.h"
#include "naucrates/statistics/CStatistics.h"
#include "naucrates/statistics/CStatsPredDisj.h"

// fwd declarations
namespace spqopt
{
class CTableDescriptor;
}

namespace spqmd
{
class IMDTypeInt4;
}

namespace spqnaucrates
{
//---------------------------------------------------------------------------
//	@class:
//		CStatisticsTest
//
//	@doc:
//		Static unit tests for statistics objects
//
//---------------------------------------------------------------------------
class CStatisticsTest
{
private:
	// test case for union all evaluation
	struct SStatsUnionAllSTestCase
	{
		// input stats dxl file
		const CHAR *m_szInputFile;

		// output stats dxl file
		const CHAR *m_szOutputFile;
	};

	// create filter on int4 types
	static void StatsFilterInt4(CMemoryPool *mp, ULONG colid, INT iLower,
								INT iUpper, CStatsPredPtrArry *pgrspqstatspred);

	// create filter on boolean types
	static void StatsFilterBool(CMemoryPool *mp, ULONG colid, BOOL fValue,
								CStatsPredPtrArry *pgrspqstatspred);

	// create filter on numeric types
	static void StatsFilterNumeric(CMemoryPool *mp, ULONG colid,
								   CWStringDynamic *pstrLowerEncoded,
								   CWStringDynamic *pstrUpperEncoded,
								   CDouble dValLower, CDouble dValUpper,
								   CStatsPredPtrArry *pdrspqstatspred);

	// create filter on generic types
	static void StatsFilterGeneric(CMemoryPool *mp, ULONG colid, OID oid,
								   CWStringDynamic *pstrLowerEncoded,
								   CWStringDynamic *pstrUpperEncoded,
								   LINT lValLower, LINT lValUpper,
								   CStatsPredPtrArry *pgrspqstatspred);

	static CHistogram *PhistExampleInt4Dim(CMemoryPool *mp);

	// helper function that generates an array of ULONG pointers
	static ULongPtrArray *
	Pdrspqul(CMemoryPool *mp, ULONG ul1, ULONG ul2 = spqos::ulong_max)
	{
		ULongPtrArray *pdrspqul = SPQOS_NEW(mp) ULongPtrArray(mp);
		pdrspqul->Append(SPQOS_NEW(mp) ULONG(ul1));

		if (spqos::ulong_max != ul2)
		{
			pdrspqul->Append(SPQOS_NEW(mp) ULONG(ul2));
		}

		return pdrspqul;
	}

	// create a table descriptor with two columns having the given names
	static CTableDescriptor *PtabdescTwoColumnSource(
		CMemoryPool *mp, const CName &nameTable, const IMDTypeInt4 *pmdtype,
		const CWStringConst &strColA, const CWStringConst &strColB);

public:
	// example filter
	static CStatsPredPtrArry *Pdrspqstatspred1(CMemoryPool *mp);

	static CStatsPredPtrArry *Pdrspqstatspred2(CMemoryPool *mp);

	// unittests
	static SPQOS_RESULT EresUnittest();

	// union all tests
	static SPQOS_RESULT EresUnittest_UnionAll();

	// statistics basic tests
	static SPQOS_RESULT EresUnittest_CStatisticsBasic();

	// statistics basic tests
	static SPQOS_RESULT EresUnittest_CStatisticsBucketTest();

	// exercise stats derivation during optimization
	static SPQOS_RESULT EresUnittest_CStatisticsSelectDerivation();

	// GbAgg test when grouping on repeated columns
	static SPQOS_RESULT EresUnittest_GbAggWithRepeatedGbCols();

	// test that stats copy methods copy all fields
	static SPQOS_RESULT EresUnittest_CStatisticsCopy();


};	// class CStatisticsTest
}  // namespace spqnaucrates

#endif	// !SPQNAUCRATES_CStatisticsTest_H


// EOF
