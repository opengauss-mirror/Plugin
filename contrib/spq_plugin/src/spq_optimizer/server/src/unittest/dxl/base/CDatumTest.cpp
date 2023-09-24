//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CDatumTest.cpp
//
//	@doc:
//		Tests for datum classes
//---------------------------------------------------------------------------
#include "unittest/dxl/base/CDatumTest.h"

#include "spqos/error/CAutoTrace.h"
#include "spqos/io/COstreamString.h"
#include "spqos/string/CWStringDynamic.h"

#include "spqopt/base/CQueryContext.h"
#include "spqopt/eval/CConstExprEvaluatorDefault.h"
#include "spqopt/mdcache/CMDCache.h"
#include "naucrates/base/CDatumBoolSPQDB.h"
#include "naucrates/base/CDatumGenericSPQDB.h"
#include "naucrates/base/CDatumInt2SPQDB.h"
#include "naucrates/base/CDatumInt4SPQDB.h"
#include "naucrates/base/CDatumInt8SPQDB.h"
#include "naucrates/base/CDatumOidSPQDB.h"
#include "naucrates/dxl/spqdb_types.h"
#include "naucrates/md/CMDIdSPQDB.h"
#include "naucrates/md/CMDTypeGenericSPQDB.h"
#include "naucrates/md/IMDType.h"

#include "unittest/base.h"
#include "unittest/spqopt/CTestUtils.h"
#include "unittest/spqopt/translate/CTranslatorExprToDXLTest.h"

//---------------------------------------------------------------------------
//	@function:
//		CDatumTest::EresUnittest
//
//	@doc:
//		Unittest for datum classes
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CDatumTest::EresUnittest()
{
	CUnittest rgut[] = {
		SPQOS_UNITTEST_FUNC(CDatumTest::EresUnittest_Basics),
		SPQOS_UNITTEST_FUNC(CDatumTest::StatsComparisonDoubleLessThan),
		SPQOS_UNITTEST_FUNC(CDatumTest::StatsComparisonDoubleEqualWithinEpsilon),
		SPQOS_UNITTEST_FUNC(CDatumTest::StatsComparisonIntLessThan),
		SPQOS_UNITTEST_FUNC(CDatumTest::StatsComparisonIntEqual)};

	return CUnittest::EresExecute(rgut, SPQOS_ARRAY_SIZE(rgut));
}


//---------------------------------------------------------------------------
//	@function:
//		CDatumTest::EresUnittest_Basics
//
//	@doc:
//		Basic datum tests; verify correct creation
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CDatumTest::EresUnittest_Basics()
{
	// create memory pool
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	// setup a file-based provider
	CMDProviderMemory *pmdp = CTestUtils::m_pmdpf;
	pmdp->AddRef();
	CMDAccessor mda(mp, CMDCache::Pcache(), CTestUtils::m_sysidDefault, pmdp);

	// install opt context in TLS
	CAutoOptCtxt aoc(mp, &mda, NULL, /* pceeval */
					 CTestUtils::GetCostModel(mp));

	typedef IDatum *(*Pfpdatum)(CMemoryPool *, BOOL);

	Pfpdatum rspqf[] = {
		CreateInt2Datum, CreateInt4Datum, CreateInt8Datum,
		CreateBoolDatum, CreateOidDatum,  CreateGenericDatum,
	};

	BOOL rgf[] = {true, false};

	const ULONG ulFuncs = SPQOS_ARRAY_SIZE(rspqf);
	const ULONG ulOptions = SPQOS_ARRAY_SIZE(rgf);

	for (ULONG ul1 = 0; ul1 < ulFuncs; ul1++)
	{
		for (ULONG ul2 = 0; ul2 < ulOptions; ul2++)
		{
			CAutoTrace at(mp);
			IOstream &os(at.Os());

			// generate datum
			BOOL is_null = rgf[ul2];
			IDatum *datum = rspqf[ul1](mp, is_null);
			IDatum *pdatumCopy = datum->MakeCopy(mp);

			SPQOS_ASSERT(datum->Matches(pdatumCopy));

			const CWStringConst *pstrDatum = datum->GetStrRepr(mp);

#ifdef SPQOS_DEBUG
			os << std::endl;
			(void) datum->OsPrint(os);
			os << std::endl << pstrDatum->GetBuffer() << std::endl;
#endif	// SPQOS_DEBUG

			os << "Datum type: " << datum->GetDatumType() << std::endl;

			if (datum->StatsMappable())
			{
				if (datum->IsDatumMappableToLINT())
				{
					os << "LINT stats value: " << datum->GetLINTMapping()
					   << std::endl;
				}

				if (datum->IsDatumMappableToDouble())
				{
					os << "Double stats value: " << datum->GetDoubleMapping()
					   << std::endl;
				}
			}

			// cleanup
			datum->Release();
			pdatumCopy->Release();
			SPQOS_DELETE(pstrDatum);
		}
	}

	return SPQOS_OK;
}


//---------------------------------------------------------------------------
//	@function:
//		CDatumTest::CreateOidDatum
//
//	@doc:
//		Create an oid datum
//
//---------------------------------------------------------------------------
IDatum *
CDatumTest::CreateOidDatum(CMemoryPool *mp, BOOL is_null)
{
	return SPQOS_NEW(mp)
		CDatumOidSPQDB(CTestUtils::m_sysidDefault, 1 /*val*/, is_null);
}

//---------------------------------------------------------------------------
//	@function:
//		CDatumTest::CreateInt2Datum
//
//	@doc:
//		Create an int2 datum
//
//---------------------------------------------------------------------------
IDatum *
CDatumTest::CreateInt2Datum(CMemoryPool *mp, BOOL is_null)
{
	return SPQOS_NEW(mp)
		CDatumInt2SPQDB(CTestUtils::m_sysidDefault, 1 /*val*/, is_null);
}

//---------------------------------------------------------------------------
//	@function:
//		CDatumTest::CreateInt4Datum
//
//	@doc:
//		Create an int4 datum
//
//---------------------------------------------------------------------------
IDatum *
CDatumTest::CreateInt4Datum(CMemoryPool *mp, BOOL is_null)
{
	return SPQOS_NEW(mp)
		CDatumInt4SPQDB(CTestUtils::m_sysidDefault, 1 /*val*/, is_null);
}

//---------------------------------------------------------------------------
//	@function:
//		CDatumTest::CreateInt8Datum
//
//	@doc:
//		Create an int8 datum
//
//---------------------------------------------------------------------------
IDatum *
CDatumTest::CreateInt8Datum(CMemoryPool *mp, BOOL is_null)
{
	return SPQOS_NEW(mp)
		CDatumInt8SPQDB(CTestUtils::m_sysidDefault, 1 /*val*/, is_null);
}

//---------------------------------------------------------------------------
//	@function:
//		CDatumTest::CreateBoolDatum
//
//	@doc:
//		Create a bool datum
//
//---------------------------------------------------------------------------
IDatum *
CDatumTest::CreateBoolDatum(CMemoryPool *mp, BOOL is_null)
{
	return SPQOS_NEW(mp)
		CDatumBoolSPQDB(CTestUtils::m_sysidDefault, false /*value*/, is_null);
}

//---------------------------------------------------------------------------
//	@function:
//		CDatumTest::CreateGenericDatum
//
//	@doc:
//		Create a generic datum
//
//---------------------------------------------------------------------------
IDatum *
CDatumTest::CreateGenericDatum(CMemoryPool *mp, BOOL is_null)
{
	CMDIdSPQDB *pmdidChar =
		SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_CHAR);

	const CHAR *val = "test";
	return SPQOS_NEW(mp)
		CDatumGenericSPQDB(mp, pmdidChar, default_type_modifier, val,
						  5 /*length*/, is_null, 0 /*value*/, 0 /*value*/
		);
}

//---------------------------------------------------------------------------
//	@function:
//		CDatumTest::StatsComparisonDouble
//
//	@doc:
//		Compare DOUBle statistics that are within a small epsilon and ensure
//      StatsAreEqual and StatsAreLessThan do not overlap
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CDatumTest::StatsComparisonDoubleEqualWithinEpsilon()
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

	// create accesssor
	CMDAccessor *md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();

	IMDId *mdid1 = SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_FLOAT8);
	IDatum *datum1 = CTestUtils::CreateDoubleDatum(mp, md_accessor, mdid1,
												   CDouble(631.82140500000003));

	IMDId *mdid2 = SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_FLOAT8);
	IDatum *datum2 = CTestUtils::CreateDoubleDatum(mp, md_accessor, mdid2,
												   CDouble(631.82140700000002));

	BOOL isEqual = datum1->StatsAreEqual(datum2);
	BOOL isLessThan = datum1->StatsAreLessThan(datum2);
	datum1->Release();
	datum2->Release();

	if (isEqual || !isLessThan)
	{
		return SPQOS_OK;
	}

	return SPQOS_FAILED;
}

//---------------------------------------------------------------------------
//	@function:
//		CDatumTest::StatsComparisonDouble
//
//	@doc:
//		Compare DOUBle statistics and ensure StatsAreEqual and StatsAreLessThan do not overlap
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CDatumTest::StatsComparisonDoubleLessThan()
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

	// create accesssor
	CMDAccessor *md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();

	IMDId *mdid1 = SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_FLOAT8);
	IDatum *datum1 = CTestUtils::CreateDoubleDatum(mp, md_accessor, mdid1,
												   CDouble(99.82140500000003));

	IMDId *mdid2 = SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_FLOAT8);
	IDatum *datum2 = CTestUtils::CreateDoubleDatum(mp, md_accessor, mdid2,
												   CDouble(100.92140700000002));

	BOOL isEqual = datum1->StatsAreEqual(datum2);
	BOOL isLessThan = datum1->StatsAreLessThan(datum2);
	datum1->Release();
	datum2->Release();

	if (!isEqual && isLessThan)
	{
		return SPQOS_OK;
	}

	return SPQOS_FAILED;
}

//---------------------------------------------------------------------------
//	@function:
//		CDatumTest::StatsComparisonIntLessThan
//
//	@doc:
//		Compare LINT statistics and ensure StatsAreEqual and StatsAreLessThan do not overlap
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CDatumTest::StatsComparisonIntLessThan()
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

	IDatum *datum1 = SPQOS_NEW(mp) CDatumInt4SPQDB(CTestUtils::m_sysidDefault,
												 100 /*val*/, false /*isnull*/);
	IDatum *datum2 = SPQOS_NEW(mp) CDatumInt4SPQDB(CTestUtils::m_sysidDefault,
												 101 /*val*/, false /*isnull*/);
	BOOL isEqual = datum1->StatsAreEqual(datum2);
	BOOL isLessThan = datum1->StatsAreLessThan(datum2);
	datum1->Release();
	datum2->Release();

	if (!isEqual && isLessThan)
	{
		return SPQOS_OK;
	}

	return SPQOS_FAILED;
}

//---------------------------------------------------------------------------
//	@function:
//		CDatumTest::StatsComparisonIntEqual
//
//	@doc:
//		Compare LINT statistics and ensure StatsAreEqual and StatsAreLessThan do not overlap
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CDatumTest::StatsComparisonIntEqual()
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

	IDatum *datum1 = SPQOS_NEW(mp) CDatumInt4SPQDB(CTestUtils::m_sysidDefault,
												 101 /*val*/, false /*isnull*/);
	IDatum *datum2 = SPQOS_NEW(mp) CDatumInt4SPQDB(CTestUtils::m_sysidDefault,
												 101 /*val*/, false /*isnull*/);
	BOOL isEqual = datum1->StatsAreEqual(datum2);
	BOOL isLessThan = datum1->StatsAreLessThan(datum2);
	datum1->Release();
	datum2->Release();

	if (isEqual && !isLessThan)
	{
		return SPQOS_OK;
	}

	return SPQOS_FAILED;
}
// EOF
