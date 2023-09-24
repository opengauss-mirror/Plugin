//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CTestUtils.cpp
//
//	@doc:
//		Implementation of test utility functions
//---------------------------------------------------------------------------

#include "unittest/spqopt/CTestUtils.h"

#include <fstream>

#include "spqos/base.h"
#include "spqos/common/CAutoP.h"
#include "spqos/error/CMessage.h"
#include "spqos/io/COstreamString.h"
#include "spqos/memory/CAutoMemoryPool.h"
#include "spqos/string/CWStringDynamic.h"
#include "spqos/task/CAutoTraceFlag.h"
#include "spqos/test/CUnittest.h"

#include "spqopt/base/CAutoOptCtxt.h"
#include "spqopt/base/CCTEMap.h"
#include "spqopt/base/CColRefSetIter.h"
#include "spqopt/base/CConstraintInterval.h"
#include "spqopt/base/CDefaultComparator.h"
#include "spqopt/base/CDistributionSpecHashed.h"
#include "spqopt/base/CPrintPrefix.h"
#include "spqopt/base/CUtils.h"
#include "spqopt/engine/CEngine.h"
#include "spqopt/engine/CEnumeratorConfig.h"
#include "spqopt/engine/CStatisticsConfig.h"
#include "spqopt/eval/CConstExprEvaluatorDefault.h"
#include "spqopt/eval/IConstExprEvaluator.h"
#include "spqopt/exception.h"
#include "spqopt/mdcache/CMDCache.h"
#include "spqopt/metadata/CTableDescriptor.h"
#include "spqopt/minidump/CMinidumperUtils.h"
#include "spqopt/operators/CScalarSubqueryAll.h"
#include "spqopt/operators/CScalarSubqueryAny.h"
#include "spqopt/operators/ops.h"
#include "spqopt/optimizer/COptimizerConfig.h"
#include "spqopt/translate/CTranslatorDXLToExpr.h"
#include "spqopt/translate/CTranslatorExprToDXL.h"
#include "spqopt/xforms/CSubqueryHandler.h"
#include "spqopt/xforms/CXformUtils.h"
#include "naucrates/base/CDatumBoolSPQDB.h"
#include "naucrates/base/CDatumInt2SPQDB.h"
#include "naucrates/base/CDatumInt4SPQDB.h"
#include "naucrates/base/CDatumInt8SPQDB.h"
#include "naucrates/base/CQueryToDXLResult.h"
#include "naucrates/dxl/CDXLUtils.h"
#include "naucrates/dxl/operators/CDXLDatumGeneric.h"
#include "naucrates/dxl/operators/CDXLDatumStatsDoubleMappable.h"
#include "naucrates/dxl/operators/CDXLDatumStatsLintMappable.h"
#include "naucrates/md/CMDIdSPQDB.h"
#include "naucrates/md/CMDProviderMemory.h"
#include "naucrates/md/CMDTypeGenericSPQDB.h"
#include "naucrates/md/IMDScalarOp.h"
#include "naucrates/md/IMDTypeBool.h"
#include "naucrates/md/IMDTypeInt2.h"
#include "naucrates/md/IMDTypeInt4.h"
#include "naucrates/md/IMDTypeInt8.h"
#include "naucrates/statistics/CStatsPredUtils.h"

#include "unittest/base.h"
#include "unittest/spqopt/CSubqueryTestUtils.h"

#define SPQOPT_SEGMENT_COUNT 2  // number segments for testing

using namespace spqopt;

// static variable initialization
// default source system id
CSystemId CTestUtils::m_sysidDefault(IMDId::EmdidGeneral,
									 SPQOS_WSZ_STR_LENGTH("SPQDB"));


// XSD path
const CHAR *CTestUtils::m_szXSDPath =
	"http://greenplum.com/dxl/2010/12/ dxl.xsd";

// metadata file
const CHAR *CTestUtils::m_szMDFileName = "../data/dxl/metadata/md.xml";

// provider file
CMDProviderMemory *CTestUtils::m_pmdpf = NULL;

// local memory pool
CMemoryPool *CTestUtils::m_mp = NULL;

//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::CTestSetup::PmdpSetupFileBasedProvider
//
//	@doc:
//		set up a file based provider
//
//---------------------------------------------------------------------------
CMDProviderMemory *
CTestUtils::CTestSetup::PmdpSetupFileBasedProvider()
{
	CMDProviderMemory *pmdp = CTestUtils::m_pmdpf;
	pmdp->AddRef();
	return pmdp;
}

//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::CTestSetup::CTestSetup
//
//	@doc:
//		ctor
//
//---------------------------------------------------------------------------
CTestUtils::CTestSetup::CTestSetup()
	: m_amp(),
	  m_mda(m_amp.Pmp(), CMDCache::Pcache(), CTestUtils::m_sysidDefault,
			PmdpSetupFileBasedProvider()),
	  // install opt context in TLS
	  m_aoc(m_amp.Pmp(), &m_mda, NULL, /* pceeval */
			CTestUtils::GetCostModel(m_amp.Pmp()))
{
}

//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::InitProviderFile
//
//	@doc:
//		Initialize provider file;
//		called before unittests start
//
//---------------------------------------------------------------------------
void
CTestUtils::InitProviderFile(CMemoryPool *mp)
{
	SPQOS_ASSERT(NULL == m_mp);
	SPQOS_ASSERT(NULL != mp);

	m_mp = mp;
	m_pmdpf = SPQOS_NEW(m_mp) CMDProviderMemory(m_mp, m_szMDFileName);
}


//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::DestroyMDProvider
//
//	@doc:
//		Destroy metadata file;
//		called after all unittests complete
//
//---------------------------------------------------------------------------
void
CTestUtils::DestroyMDProvider()
{
	SPQOS_ASSERT(NULL != m_mp);

	CRefCount::SafeRelease(m_pmdpf);

	// release local memory pool
	CMemoryPoolManager::GetMemoryPoolMgr()->Destroy(m_mp);
}


//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PtabdescPlainWithColNameFormat
//
//	@doc:
//		Generate a plain table descriptor, where the column names are generated
//		using a format string containing %d
//
//---------------------------------------------------------------------------


CTableDescriptor *
CTestUtils::PtabdescPlainWithColNameFormat(
	CMemoryPool *mp, ULONG num_cols, IMDId *mdid, const WCHAR *wszColNameFormat,
	const CName &nameTable,
	BOOL is_nullable  // define nullable columns
)
{
	SPQOS_ASSERT(0 < num_cols);

	CMDAccessor *md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();

	const IMDTypeInt4 *pmdtypeint4 =
		md_accessor->PtMDType<IMDTypeInt4>(CTestUtils::m_sysidDefault);
	CWStringDynamic *str_name = SPQOS_NEW(mp) CWStringDynamic(mp);
	CTableDescriptor *ptabdesc = SPQOS_NEW(mp) CTableDescriptor(
		mp, mdid, nameTable,
		false,	// convert_hash_to_random
		IMDRelation::EreldistrRandom, IMDRelation::ErelstorageHeap,
		0  // ulExecuteAsUser
	);

	for (ULONG i = 0; i < num_cols; i++)
	{
		str_name->Reset();
		str_name->AppendFormat(wszColNameFormat, i);

		// create a shallow constant string to embed in a name
		CWStringConst strName(str_name->GetBuffer());
		CName nameColumnInt(&strName);

		CColumnDescriptor *pcoldescInt = SPQOS_NEW(mp)
			CColumnDescriptor(mp, pmdtypeint4, default_type_modifier,
							  nameColumnInt, i + 1, is_nullable);
		ptabdesc->AddColumn(pcoldescInt);
	}

	SPQOS_DELETE(str_name);

	return ptabdesc;
}


//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PtabdescPlain
//
//	@doc:
//		Generate a plain table descriptor
//
//---------------------------------------------------------------------------
CTableDescriptor *
CTestUtils::PtabdescPlain(CMemoryPool *mp, ULONG num_cols, IMDId *mdid,
						  const CName &nameTable,
						  BOOL is_nullable	// define nullable columns
)
{
	return PtabdescPlainWithColNameFormat(mp, num_cols, mdid,
										  SPQOS_WSZ_LIT("column_%04d"),
										  nameTable, is_nullable);
}

//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PtabdescCreate
//
//	@doc:
//		Generate a table descriptor
//
//---------------------------------------------------------------------------
CTableDescriptor *
CTestUtils::PtabdescCreate(CMemoryPool *mp, ULONG num_cols, IMDId *mdid,
						   const CName &nameTable, BOOL fPartitioned)
{
	CTableDescriptor *ptabdesc = PtabdescPlain(mp, num_cols, mdid, nameTable);

	if (fPartitioned)
	{
		ptabdesc->AddPartitionColumn(0);
	}

	// create a keyset containing the first column
	CBitSet *pbs = SPQOS_NEW(mp) CBitSet(mp, num_cols);
	pbs->ExchangeSet(0);
#ifdef SPQOS_DEBUG
	BOOL fSuccess =
#endif	// SPQOS_DEBUG
		ptabdesc->FAddKeySet(pbs);
	SPQOS_ASSERT(fSuccess);

	return ptabdesc;
}

//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PexprLogicalGet
//
//	@doc:
//		Generate a get expression
//
//---------------------------------------------------------------------------
CExpression *
CTestUtils::PexprLogicalGet(CMemoryPool *mp, CTableDescriptor *ptabdesc,
							const CWStringConst *pstrTableAlias)
{
	SPQOS_ASSERT(NULL != ptabdesc);

	CLogicalGet *pop = SPQOS_NEW(mp) CLogicalGet(
		mp, SPQOS_NEW(mp) CName(mp, CName(pstrTableAlias)), ptabdesc);

	CExpression *result = SPQOS_NEW(mp) CExpression(mp, pop);

	CColRefArray *arr = pop->PdrspqcrOutput();
	for (ULONG ul = 0; ul < arr->Size(); ul++)
	{
		CColRef *ref = (*arr)[ul];
		ref->MarkAsUsed();
	}

	return result;
}

//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PexprLogicalGetNullable
//
//	@doc:
//		Generate a get expression over table with nullable columns
//
//---------------------------------------------------------------------------
CExpression *
CTestUtils::PexprLogicalGetNullable(CMemoryPool *mp, OID oidTable,
									const CWStringConst *str_table_name,
									const CWStringConst *pstrTableAlias)
{
	CWStringConst strName(str_table_name->GetBuffer());
	CMDIdSPQDB *mdid = SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidRel, oidTable, 1, 1);
	CTableDescriptor *ptabdesc = CTestUtils::PtabdescPlain(
		mp, 3, mdid, CName(&strName), true /*is_nullable*/);
	CWStringConst strAlias(pstrTableAlias->GetBuffer());

	return PexprLogicalGet(mp, ptabdesc, &strAlias);
}


//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PexprLogicalGet
//
//	@doc:
//		Generate a get expression
//
//---------------------------------------------------------------------------
CExpression *
CTestUtils::PexprLogicalGet(CMemoryPool *mp, CWStringConst *str_table_name,
							CWStringConst *pstrTableAlias, ULONG ulTableId)
{
	CTableDescriptor *ptabdesc =
		PtabdescCreate(mp, SPQOPT_TEST_REL_WIDTH,
					   SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidRel, ulTableId, 1, 1),
					   CName(str_table_name));

	CWStringConst strAlias(pstrTableAlias->GetBuffer());
	return PexprLogicalGet(mp, ptabdesc, &strAlias);
}

//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PexprLogicalGet
//
//	@doc:
//		Generate a randomized get expression
//
//---------------------------------------------------------------------------
CExpression *
CTestUtils::PexprLogicalGet(CMemoryPool *mp)
{
	CWStringConst strName(SPQOS_WSZ_LIT("BaseTable"));
	CMDIdSPQDB *mdid =
		SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidRel, SPQOPT_MDCACHE_TEST_OID, 1, 1);
	CTableDescriptor *ptabdesc = PtabdescCreate(mp, 3, mdid, CName(&strName));
	CWStringConst strAlias(SPQOS_WSZ_LIT("BaseTableAlias"));

	return PexprLogicalGet(mp, ptabdesc, &strAlias);
}

//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PexprLogicalExternalGet
//
//	@doc:
//		Generate a randomized external get expression
//
//---------------------------------------------------------------------------
CExpression *
CTestUtils::PexprLogicalExternalGet(CMemoryPool *mp)
{
	CWStringConst strName(SPQOS_WSZ_LIT("ExternalTable"));
	CMDIdSPQDB *mdid =
		SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidRel, SPQOPT_MDCACHE_TEST_OID, 1, 1);
	CTableDescriptor *ptabdesc = PtabdescCreate(mp, 3, mdid, CName(&strName));
	CWStringConst strAlias(SPQOS_WSZ_LIT("ExternalTableAlias"));

	return SPQOS_NEW(mp)
		CExpression(mp, SPQOS_NEW(mp) CLogicalExternalGet(
							mp, SPQOS_NEW(mp) CName(mp, &strAlias), ptabdesc));
}

//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PexprLogicalGetPartitioned
//
//	@doc:
//		Generate a randomized get expression for a partitioned table
//
//---------------------------------------------------------------------------
CExpression *
CTestUtils::PexprLogicalGetPartitioned(CMemoryPool *mp)
{
	ULONG ulAttributes = 2;
	CWStringConst strName(SPQOS_WSZ_LIT("PartTable"));
	CMDIdSPQDB *mdid = SPQOS_NEW(mp)
		CMDIdSPQDB(IMDId::EmdidRel, SPQOPT_MDCACHE_TEST_OID_PARTITIONED);
	CTableDescriptor *ptabdesc = PtabdescCreate(
		mp, ulAttributes, mdid, CName(&strName), true /*fPartitioned*/);
	CWStringConst strAlias(SPQOS_WSZ_LIT("PartTableAlias"));

	return PexprLogicalGet(mp, ptabdesc, &strAlias);
}

//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PexprLogicalDynamicGetWithIndexes
//
//	@doc:
//		Generate a randomized get expression for a partitioned table
//		with indexes
//
//---------------------------------------------------------------------------
CExpression *
CTestUtils::PexprLogicalDynamicGetWithIndexes(CMemoryPool *mp)
{
	ULONG ulAttributes = 2;
	CWStringConst strName(SPQOS_WSZ_LIT("P1"));
	CMDIdSPQDB *mdid = SPQOS_NEW(mp) CMDIdSPQDB(
		IMDId::EmdidRel, SPQOPT_MDCACHE_TEST_OID_PARTITIONED_WITH_INDEXES);
	CTableDescriptor *ptabdesc = PtabdescCreate(
		mp, ulAttributes, mdid, CName(&strName), true /*fPartitioned*/);
	CWStringConst strAlias(SPQOS_WSZ_LIT("P1Alias"));

	return SPQOS_NEW(mp) CExpression(
		mp, SPQOS_NEW(mp) CLogicalDynamicGet(
				mp, SPQOS_NEW(mp) CName(mp, CName(&strAlias)), ptabdesc,
				0  // ulPartIndex
				));
}


//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PexprLogicalSelect
//
//	@doc:
//		Generate a Select expression with a random equality predicate
//
//---------------------------------------------------------------------------
CExpression *
CTestUtils::PexprLogicalSelect(CMemoryPool *mp, CExpression *pexpr)
{
	SPQOS_ASSERT(NULL != pexpr);

	// get any two columns
	CColRefSet *pcrs = pexpr->DeriveOutputColumns();
	CColRef *pcrLeft = pcrs->PcrAny();
	CColRef *pcrRight = pcrs->PcrAny();
	CExpression *pexprPredicate =
		CUtils::PexprScalarEqCmp(mp, pcrLeft, pcrRight);

	return CUtils::PexprLogicalSelect(mp, pexpr, pexprPredicate);
}

//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PexprLogicalSelect
//
//	@doc:
//		Generate select expression
//
//---------------------------------------------------------------------------
CExpression *
CTestUtils::PexprLogicalSelect(CMemoryPool *mp, CWStringConst *str_table_name,
							   CWStringConst *pstrTableAlias, ULONG ulTableId)
{
	CExpression *pexprGet =
		PexprLogicalGet(mp, str_table_name, pstrTableAlias, ulTableId);
	return PexprLogicalSelect(mp, pexprGet);
}

//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PexprLogicalSelect
//
//	@doc:
//		Generate randomized Select expression
//
//---------------------------------------------------------------------------
CExpression *
CTestUtils::PexprLogicalSelect(CMemoryPool *mp)
{
	return PexprLogicalSelect(mp, PexprLogicalGet(mp));
}

//---------------------------------------------------------------------------
//	@function:
// 		CTestUtils::PexprLogicalSelectWithContradiction
//
//	@doc:
//		Generate a select expression with a contradiction
//
//---------------------------------------------------------------------------
CExpression *
CTestUtils::PexprLogicalSelectWithContradiction(CMemoryPool *mp)
{
	CExpression *pexpr = PexprLogicalSelect(mp, PexprLogicalGet(mp));
	// get any column
	CColRefSet *pcrs = pexpr->DeriveOutputColumns();
	CColRef *colref = pcrs->PcrAny();

	CExpression *pexprConstFirst = CUtils::PexprScalarConstInt4(mp, 3 /*val*/);
	CExpression *pexprPredFirst =
		CUtils::PexprScalarEqCmp(mp, colref, pexprConstFirst);

	CExpression *pexprConstSecond = CUtils::PexprScalarConstInt4(mp, 5 /*val*/);
	CExpression *pexprPredSecond =
		CUtils::PexprScalarEqCmp(mp, colref, pexprConstSecond);

	CExpression *pexprPredicate =
		CPredicateUtils::PexprConjunction(mp, pexprPredFirst, pexprPredSecond);
	pexprPredFirst->Release();
	pexprPredSecond->Release();

	return CUtils::PexprLogicalSelect(mp, pexpr, pexprPredicate);
}

//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PexprLogicalSelectPartitioned
//
//	@doc:
//		Generate a randomized select expression over a partitioned table
//
//---------------------------------------------------------------------------
CExpression *
CTestUtils::PexprLogicalSelectPartitioned(CMemoryPool *mp)
{
	CExpression *pexprGet = PexprLogicalGetPartitioned(mp);

	// extract first partition key
	CLogicalGet *popGet = CLogicalGet::PopConvert(pexprGet->Pop());
	const CColRef2dArray *pdrspqdrspqcr = popGet->PdrspqdrspqcrPartColumns();

	SPQOS_ASSERT(pdrspqdrspqcr != NULL);
	SPQOS_ASSERT(0 < pdrspqdrspqcr->Size());
	CColRefArray *colref_array = (*pdrspqdrspqcr)[0];
	SPQOS_ASSERT(1 == colref_array->Size());
	CColRef *pcrPartKey = (*colref_array)[0];

	// construct a comparison pk = 5
	INT val = 5;
	CExpression *pexprScalar =
		CUtils::PexprScalarEqCmp(mp, CUtils::PexprScalarIdent(mp, pcrPartKey),
								 CUtils::PexprScalarConstInt4(mp, val));

	return CUtils::PexprLogicalSelect(mp, pexprGet, pexprScalar);
}

//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PexprLogicalAssert
//
//	@doc:
//		Generate randomized Assert expression
//
//---------------------------------------------------------------------------
CExpression *
CTestUtils::PexprLogicalAssert(CMemoryPool *mp)
{
	CExpression *pexprGet = PexprLogicalGet(mp);

	// get any two columns
	CColRefSet *pcrs = pexprGet->DeriveOutputColumns();
	CColRef *pcrLeft = pcrs->PcrAny();
	CColRef *pcrRight = pcrs->PcrAny();
	CExpression *pexprPredicate =
		CUtils::PexprScalarEqCmp(mp, pcrLeft, pcrRight);

	CWStringConst *pstrErrMsg = CXformUtils::PstrErrorMessage(
		mp, spqos::CException::ExmaSQL, spqos::CException::ExmiSQLTest,
		SPQOS_WSZ_LIT("Test msg"));
	CExpression *pexprAssertConstraint = SPQOS_NEW(mp)
		CExpression(mp, SPQOS_NEW(mp) CScalarAssertConstraint(mp, pstrErrMsg),
					pexprPredicate);
	CExpression *pexprAssertPredicate = SPQOS_NEW(mp)
		CExpression(mp, SPQOS_NEW(mp) CScalarAssertConstraintList(mp),
					pexprAssertConstraint);

	CLogicalAssert *popAssert = SPQOS_NEW(mp) CLogicalAssert(
		mp, SPQOS_NEW(mp) CException(spqos::CException::ExmaSQL,
									spqos::CException::ExmiSQLTest));

	return SPQOS_NEW(mp)
		CExpression(mp, popAssert, pexprGet, pexprAssertPredicate);
}

//---------------------------------------------------------------------------
//		CTestUtils::Pexpr3WayJoinPartitioned
//
//	@doc:
//		Generate a 3-way join including a partitioned table
//
//---------------------------------------------------------------------------
CExpression *
CTestUtils::Pexpr3WayJoinPartitioned(CMemoryPool *mp)
{
	return PexprLogicalJoin<CLogicalInnerJoin>(
		mp, PexprLogicalGet(mp),
		PexprJoinPartitionedOuter<CLogicalInnerJoin>(mp));
}

//---------------------------------------------------------------------------
//		CTestUtils::Pexpr4WayJoinPartitioned
//
//	@doc:
//		Generate a 4-way join including a partitioned table
//
//---------------------------------------------------------------------------
CExpression *
CTestUtils::Pexpr4WayJoinPartitioned(CMemoryPool *mp)
{
	return PexprLogicalJoin<CLogicalInnerJoin>(mp, PexprLogicalGet(mp),
											   Pexpr3WayJoinPartitioned(mp));
}


//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PexprLogicalSelectWithNestedAnd
//
//	@doc:
//		Generate a random select expression with nested AND tree
//
//---------------------------------------------------------------------------
CExpression *
CTestUtils::PexprLogicalSelectWithNestedAnd(CMemoryPool *mp)
{
	SPQOS_ASSERT(NULL != mp);

	CExpression *pexprGet = PexprLogicalGet(mp);
	CExpression *pexprPred =
		PexprScalarNestedPreds(mp, pexprGet, CScalarBoolOp::EboolopAnd);

	return CUtils::PexprLogicalSelect(mp, pexprGet, pexprPred);
}

//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PexprLogicalSelectWithNestedOr
//
//	@doc:
//		Generate a random select expression with nested OR tree
//
//---------------------------------------------------------------------------
CExpression *
CTestUtils::PexprLogicalSelectWithNestedOr(CMemoryPool *mp)
{
	SPQOS_ASSERT(NULL != mp);

	CExpression *pexprGet = PexprLogicalGet(mp);
	CExpression *pexprPred =
		PexprScalarNestedPreds(mp, pexprGet, CScalarBoolOp::EboolopOr);

	return CUtils::PexprLogicalSelect(mp, pexprGet, pexprPred);
}

//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PexprLogicalSelectWithEvenNestedNot
//
//	@doc:
//		Generate a random select expression with an even number of
//		nested NOT nodes
//
//---------------------------------------------------------------------------
CExpression *
CTestUtils::PexprLogicalSelectWithEvenNestedNot(CMemoryPool *mp)
{
	SPQOS_ASSERT(NULL != mp);

	CExpression *pexprGet = PexprLogicalGet(mp);
	CExpression *pexprPred =
		PexprScalarNestedPreds(mp, pexprGet, CScalarBoolOp::EboolopNot);

	return CUtils::PexprLogicalSelect(mp, pexprGet, pexprPred);
}


//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PexprScIdentCmpScIdent
//
//	@doc:
//		Generate a scalar expression comparing scalar identifiers
//
//---------------------------------------------------------------------------
CExpression *
CTestUtils::PexprScIdentCmpScIdent(CMemoryPool *mp, CExpression *pexprLeft,
								   CExpression *pexprRight,
								   IMDType::ECmpType cmp_type)
{
	SPQOS_ASSERT(NULL != pexprLeft);
	SPQOS_ASSERT(NULL != pexprRight);
	SPQOS_ASSERT(cmp_type <= IMDType::EcmptOther);

	CColRefSet *pcrsLeft = pexprLeft->DeriveOutputColumns();
	CColRef *pcrLeft = pcrsLeft->PcrAny();

	CColRefSet *pcrsRight = pexprRight->DeriveOutputColumns();
	CColRef *pcrRight = pcrsRight->PcrAny();

	CExpression *pexprPred =
		CUtils::PexprScalarCmp(mp, pcrLeft, pcrRight, cmp_type);

	return pexprPred;
}


//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PexprScIdentCmpConst
//
//	@doc:
//		Generate a scalar expression comparing scalar identifier to a constant
//
//---------------------------------------------------------------------------
CExpression *
CTestUtils::PexprScIdentCmpConst(CMemoryPool *mp, CExpression *pexpr,
								 IMDType::ECmpType cmp_type, ULONG ulVal)
{
	SPQOS_ASSERT(NULL != pexpr);

	CColRefSet *pcrs = pexpr->DeriveOutputColumns();
	CColRef *pcrLeft = pcrs->PcrAny();
	CExpression *pexprUl = CUtils::PexprScalarConstInt4(mp, ulVal);

	CExpression *pexprPred =
		CUtils::PexprScalarCmp(mp, pcrLeft, pexprUl, cmp_type);

	return pexprPred;
}


//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PexprLogicalSelectCmpToConst
//
//	@doc:
//		Generate a Select expression with an equality predicate on the first
//		column and a constant
//
//---------------------------------------------------------------------------
CExpression *
CTestUtils::PexprLogicalSelectCmpToConst(CMemoryPool *mp)
{
	// generate a get expression
	CExpression *pexpr = PexprLogicalGet(mp);
	CExpression *pexprPred = PexprScIdentCmpConst(
		mp, pexpr, IMDType::EcmptEq /* cmp_type */, 10 /* ulVal */);

	return CUtils::PexprLogicalSelect(mp, pexpr, pexprPred);
}


//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PexprLogicalSelectArrayCmp
//
//	@doc:
//		Generate a Select expression with an array compare
//
//---------------------------------------------------------------------------
CExpression *
CTestUtils::PexprLogicalSelectArrayCmp(CMemoryPool *mp)
{
	return PexprLogicalSelectArrayCmp(mp, CScalarArrayCmp::EarrcmpAny,
									  IMDType::EcmptEq);
}

//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PexprLogicalSelectArrayCmp
//
//	@doc:
//		Generate a Select expression with an array compare. Takes an enum for
//		the type of array comparison (ANY or ALL) and an enum for the comparator
//		type (=, !=, <, etc).
//
//---------------------------------------------------------------------------
CExpression *
CTestUtils::PexprLogicalSelectArrayCmp(CMemoryPool *mp,
									   CScalarArrayCmp::EArrCmpType earrcmptype,
									   IMDType::ECmpType ecmptype)
{
	const ULONG ulArraySize = 5;
	IntPtrArray *pdrspqiVals = SPQOS_NEW(mp) IntPtrArray(mp);
	for (ULONG val = 0; val < ulArraySize; val++)
	{
		pdrspqiVals->Append(SPQOS_NEW(mp) INT(val));
	}
	CExpression *pexprSelect =
		PexprLogicalSelectArrayCmp(mp, earrcmptype, ecmptype, pdrspqiVals);
	pdrspqiVals->Release();
	return pexprSelect;
}

//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PexprLogicalSelectArrayCmp
//
//	@doc:
//		Generate a Select expression with an array compare. Takes an enum for
//		the type of array comparison (ANY or ALL) and an enum for the comparator
//		type (=, !=, <, etc). The array will be populated with the given integer
//		values.
//
//---------------------------------------------------------------------------
CExpression *
CTestUtils::PexprLogicalSelectArrayCmp(CMemoryPool *mp,
									   CScalarArrayCmp::EArrCmpType earrcmptype,
									   IMDType::ECmpType ecmptype,
									   const IntPtrArray *pdrspqiVals)
{
	SPQOS_ASSERT(CScalarArrayCmp::EarrcmpSentinel > earrcmptype);
	SPQOS_ASSERT(IMDType::EcmptOther > ecmptype);

	// generate a get expression
	CExpression *pexprGet = PexprLogicalGet(mp);

	// get the first column
	CColRefSet *pcrs = pexprGet->DeriveOutputColumns();
	CColRef *colref = pcrs->PcrAny();

	// construct an array of integers
	CExpressionArray *pdrspqexprArrayElems = SPQOS_NEW(mp) CExpressionArray(mp);

	const ULONG ulValsLength = pdrspqiVals->Size();
	for (ULONG ul = 0; ul < ulValsLength; ul++)
	{
		CExpression *pexprArrayElem =
			CUtils::PexprScalarConstInt4(mp, *(*pdrspqiVals)[ul]);
		pdrspqexprArrayElems->Append(pexprArrayElem);
	}

	return CUtils::PexprLogicalSelect(
		mp, pexprGet,
		CUtils::PexprScalarArrayCmp(mp, earrcmptype, ecmptype,
									pdrspqexprArrayElems, colref));
}

//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PexprLogicalSelectWithOddNestedNot
//
//	@doc:
//		Generate a random select expression with an odd number of
//		nested NOT nodes
//
//---------------------------------------------------------------------------
CExpression *
CTestUtils::PexprLogicalSelectWithOddNestedNot(CMemoryPool *mp)
{
	SPQOS_ASSERT(NULL != mp);

	CExpression *pexprGet = PexprLogicalGet(mp);
	CExpression *pexprPred =
		PexprScalarNestedPreds(mp, pexprGet, CScalarBoolOp::EboolopNot);
	CExpressionArray *pdrspqexpr = SPQOS_NEW(mp) CExpressionArray(mp);
	pdrspqexpr->Append(pexprPred);
	CExpression *pexprNot =
		CUtils::PexprScalarBoolOp(mp, CScalarBoolOp::EboolopNot, pdrspqexpr);

	return CUtils::PexprLogicalSelect(mp, pexprGet, pexprNot);
}

//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PexprLogicalSelectWithNestedAndOrNot
//
//	@doc:
//		Generate a random select expression with nested AND-OR-NOT predicate
//
//---------------------------------------------------------------------------
CExpression *
CTestUtils::PexprLogicalSelectWithNestedAndOrNot(CMemoryPool *mp)
{
	SPQOS_ASSERT(NULL != mp);

	CExpression *pexprGet = PexprLogicalGet(mp);
	CExpression *pexprPredAnd =
		PexprScalarNestedPreds(mp, pexprGet, CScalarBoolOp::EboolopAnd);
	CExpression *pexprPredOr =
		PexprScalarNestedPreds(mp, pexprGet, CScalarBoolOp::EboolopOr);
	CExpression *pexprPredNot =
		PexprScalarNestedPreds(mp, pexprGet, CScalarBoolOp::EboolopNot);

	CExpressionArray *pdrspqexpr = SPQOS_NEW(mp) CExpressionArray(mp);
	pdrspqexpr->Append(pexprPredAnd);
	pdrspqexpr->Append(pexprPredOr);
	pdrspqexpr->Append(pexprPredNot);
	CExpression *pexprOr =
		CUtils::PexprScalarBoolOp(mp, CScalarBoolOp::EboolopOr, pdrspqexpr);
	CExpression *pexprPred = CUtils::PexprNegate(mp, pexprOr);

	return CUtils::PexprLogicalSelect(mp, pexprGet, pexprPred);
}

//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PexprLogicalSubqueryWithConstTableGet
//
//	@doc:
//		Generate Select expression with Any subquery predicate over a const
//		table get
//
//---------------------------------------------------------------------------
CExpression *
CTestUtils::PexprLogicalSubqueryWithConstTableGet(CMemoryPool *mp,
												  COperator::EOperatorId op_id)
{
	SPQOS_ASSERT(NULL != mp);
	SPQOS_ASSERT(COperator::EopScalarSubqueryAny == op_id ||
				COperator::EopScalarSubqueryAll == op_id);

	CWStringConst strNameR(SPQOS_WSZ_LIT("Rel1"));
	CMDIdSPQDB *pmdidR =
		SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidRel, SPQOPT_TEST_REL_OID1, 1, 1);
	CTableDescriptor *ptabdescR =
		PtabdescCreate(mp, 3 /*num_cols*/, pmdidR, CName(&strNameR));

	CExpression *pexprOuter = PexprLogicalGet(mp, ptabdescR, &strNameR);
	CExpression *pexprConstTableGet =
		PexprConstTableGet(mp, 3 /* ulElements */);

	// get random columns from inner expression
	CColRefSet *pcrs = pexprConstTableGet->DeriveOutputColumns();
	const CColRef *pcrInner = pcrs->PcrAny();

	// get random columns from outer expression
	pcrs = pexprOuter->DeriveOutputColumns();
	const CColRef *pcrOuter = pcrs->PcrAny();

	const CWStringConst *str = SPQOS_NEW(mp) CWStringConst(SPQOS_WSZ_LIT("="));

	CExpression *pexprSubquery = NULL;
	if (COperator::EopScalarSubqueryAny == op_id)
	{
		// construct ANY subquery expression
		pexprSubquery = SPQOS_NEW(mp) CExpression(
			mp,
			SPQOS_NEW(mp) CScalarSubqueryAny(
				mp,
				SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_INT4_EQ_OP),
				str, pcrInner),
			pexprConstTableGet, CUtils::PexprScalarIdent(mp, pcrOuter));
	}
	else
	{
		// construct ALL subquery expression
		pexprSubquery = SPQOS_NEW(mp) CExpression(
			mp,
			SPQOS_NEW(mp) CScalarSubqueryAll(
				mp,
				SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_INT4_EQ_OP),
				str, pcrInner),
			pexprConstTableGet, CUtils::PexprScalarIdent(mp, pcrOuter));
	}

	return CUtils::PexprLogicalSelect(mp, pexprOuter, pexprSubquery);
}

//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PexprLogicalSelectWithConstAnySubquery
//
//	@doc:
//		Generate a random select expression with constant ANY subquery
//
//---------------------------------------------------------------------------
CExpression *
CTestUtils::PexprLogicalSelectWithConstAnySubquery(CMemoryPool *mp)
{
	return PexprLogicalSubqueryWithConstTableGet(
		mp, COperator::EopScalarSubqueryAny);
}

//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PexprLogicalSelectWithConstAllSubquery
//
//	@doc:
//		Generate a random select expression with constant ALL subquery
//
//---------------------------------------------------------------------------
CExpression *
CTestUtils::PexprLogicalSelectWithConstAllSubquery(CMemoryPool *mp)
{
	return PexprLogicalSubqueryWithConstTableGet(
		mp, COperator::EopScalarSubqueryAll);
}

//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PexprLogicalSelectCorrelated
//
//	@doc:
//		Generate correlated select; wrapper
//
//---------------------------------------------------------------------------
CExpression *
CTestUtils::PexprLogicalSelectCorrelated(CMemoryPool *mp)
{
	CExpression *pexprOuter = PexprLogicalGet(mp);
	CColRefSet *outer_refs = pexprOuter->DeriveOutputColumns();

	CExpression *pexpr = PexprLogicalSelectCorrelated(mp, outer_refs, 8);

	pexprOuter->Release();

	return pexpr;
}


//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PexprLogicalSelectOnOuterJoin
//
//	@doc:
//		Generate a select on top of outer join expression
//
//---------------------------------------------------------------------------
CExpression *
CTestUtils::PexprLogicalSelectOnOuterJoin(CMemoryPool *mp)
{
	// generate a pair of get expressions
	CExpression *pexprOuter = NULL;
	CExpression *pexprInner = NULL;
	CSubqueryTestUtils::GenerateGetExpressions(mp, &pexprOuter, &pexprInner);

	const CColRef *pcrOuter = pexprOuter->DeriveOutputColumns()->PcrAny();
	const CColRef *pcrInner = pexprInner->DeriveOutputColumns()->PcrAny();
	CExpression *pexprOuterJoinPred =
		CUtils::PexprScalarEqCmp(mp, pcrOuter, pcrInner);
	CExpression *pexprOuterJoin =
		SPQOS_NEW(mp) CExpression(mp, SPQOS_NEW(mp) CLogicalLeftOuterJoin(mp),
								 pexprOuter, pexprInner, pexprOuterJoinPred);
	CExpression *pexprPred = CUtils::PexprScalarEqCmp(
		mp, pcrInner, CUtils::PexprScalarConstInt4(mp, 5 /*val*/));

	return CUtils::PexprLogicalSelect(mp, pexprOuterJoin, pexprPred);
}


//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PexprLogicalSelectCorrelated
//
//	@doc:
//		Generate correlated select, different predicates depending on nesting
//		level: correlated, exclusively external, non-correlated
//
//---------------------------------------------------------------------------
CExpression *
CTestUtils::PexprLogicalSelectCorrelated(CMemoryPool *mp,
										 CColRefSet *outer_refs, ULONG ulLevel)
{
	SPQOS_CHECK_STACK_SIZE;

	if (0 == ulLevel)
	{
		return PexprLogicalGet(mp);
	}

	CExpression *pexpr =
		PexprLogicalSelectCorrelated(mp, outer_refs, ulLevel - 1);

	CColRefSet *pcrs = pexpr->DeriveOutputColumns();
	SPQOS_ASSERT(outer_refs->IsDisjoint(pcrs));

	CExpressionArray *pdrspqexpr = SPQOS_NEW(mp) CExpressionArray(mp);

	switch (ulLevel % 6)
	{
		case 4:
			// outer only
			EqualityPredicate(mp, outer_refs, outer_refs, pdrspqexpr);

			// inner only
			EqualityPredicate(mp, pcrs, pcrs, pdrspqexpr);

			// regular correlation
			EqualityPredicate(mp, pcrs, outer_refs, pdrspqexpr);
			break;

		case 3:
			// outer only
			EqualityPredicate(mp, outer_refs, outer_refs, pdrspqexpr);
			break;

		case 2:
			// inner only
			EqualityPredicate(mp, pcrs, pcrs, pdrspqexpr);

			// regular correlation
			EqualityPredicate(mp, pcrs, outer_refs, pdrspqexpr);
			break;

		case 1:
			// inner only
			EqualityPredicate(mp, pcrs, pcrs, pdrspqexpr);
			break;

		case 0:
			// regular correlation
			EqualityPredicate(mp, pcrs, outer_refs, pdrspqexpr);
			break;
	}

	CExpression *pexprCorrelation =
		CPredicateUtils::PexprConjunction(mp, pdrspqexpr);

	// assemble select
	CExpression *pexprResult = SPQOS_NEW(mp) CExpression(
		mp, SPQOS_NEW(mp) CLogicalSelect(mp), pexpr, pexprCorrelation);

	return pexprResult;
}

//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PexprLogicalProject
//
//	@doc:
//		Generate a Project expression that re-maps a single column
//
//---------------------------------------------------------------------------
CExpression *
CTestUtils::PexprLogicalProject(CMemoryPool *mp, CExpression *pexpr,
								CColRef *colref, CColRef *new_colref)
{
	SPQOS_ASSERT(NULL != pexpr);
	SPQOS_ASSERT(NULL != colref);
	SPQOS_ASSERT(NULL != new_colref);

	return SPQOS_NEW(mp) CExpression(
		mp, SPQOS_NEW(mp) CLogicalProject(mp), pexpr,
		SPQOS_NEW(mp) CExpression(
			mp, SPQOS_NEW(mp) CScalarProjectList(mp),
			SPQOS_NEW(mp) CExpression(
				mp, SPQOS_NEW(mp) CScalarProjectElement(mp, new_colref),
				SPQOS_NEW(mp)
					CExpression(mp, SPQOS_NEW(mp) CScalarIdent(mp, colref)))));
}

//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PexprLogicalProject
//
//	@doc:
//		Generate a randomized project expression
//
//---------------------------------------------------------------------------
CExpression *
CTestUtils::PexprLogicalProject(CMemoryPool *mp)
{
	// generate a get expression
	CExpression *pexpr = PexprLogicalGet(mp);

	// get output columns
	CColRefSet *pcrs = pexpr->DeriveOutputColumns();

	CColumnFactory *col_factory = COptCtxt::PoctxtFromTLS()->Pcf();
	CColRef *colref = pcrs->PcrAny();

	// create new column to which we will an existing one remap to
	CColRef *new_colref = col_factory->PcrCreate(colref);

	return PexprLogicalProject(mp, pexpr, colref, new_colref);
}

//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PexprLogicalProjectGbAggCorrelated
//
//	@doc:
//		Generate correlated Project over GbAgg
//
//---------------------------------------------------------------------------
CExpression *
CTestUtils::PexprLogicalProjectGbAggCorrelated(CMemoryPool *mp)
{
	// create a GbAgg expression
	CExpression *pexprGbAgg = PexprLogicalGbAggCorrelated(mp);

	CExpression *pexprPrjList =
		SPQOS_NEW(mp) CExpression(mp, SPQOS_NEW(mp) CScalarProjectList(mp));
	CExpression *pexprProject = SPQOS_NEW(mp) CExpression(
		mp, SPQOS_NEW(mp) CLogicalProject(mp), pexprGbAgg, pexprPrjList);

	return pexprProject;
}

//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PexprLogicalJoinCorrelated
//
//	@doc:
//		Generate correlated join
//
//---------------------------------------------------------------------------
CExpression *
CTestUtils::PexprLogicalJoinCorrelated(CMemoryPool *mp)
{
	CExpression *pexprLeft = PexprLogicalSelectCorrelated(mp);
	CExpression *pexprRight = PexprLogicalSelectCorrelated(mp);

	CExpressionArray *pdrspqexpr = SPQOS_NEW(mp) CExpressionArray(mp);
	pdrspqexpr->Append(pexprLeft);
	pdrspqexpr->Append(pexprRight);

	CColRefSet *pcrsLeft = pexprLeft->DeriveOutputColumns();
	CColRefSet *pcrsRight = pexprRight->DeriveOutputColumns();

	EqualityPredicate(mp, pcrsLeft, pcrsRight, pdrspqexpr);

	return SPQOS_NEW(mp)
		CExpression(mp, SPQOS_NEW(mp) CLogicalInnerJoin(mp), pdrspqexpr);
}


//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PexprLogicalJoinCorrelated
//
//	@doc:
//		Generate join with a partitioned and indexed inner child
//
//---------------------------------------------------------------------------
CExpression *
CTestUtils::PexprLogicalJoinWithPartitionedAndIndexedInnerChild(CMemoryPool *mp)
{
	CExpression *pexprLeft = PexprLogicalGet(mp);
	CExpression *pexprRight = PexprLogicalDynamicGetWithIndexes(mp);

	CExpressionArray *pdrspqexpr = SPQOS_NEW(mp) CExpressionArray(mp);
	pdrspqexpr->Append(pexprLeft);
	pdrspqexpr->Append(pexprRight);

	CColRefSet *pcrsLeft = pexprLeft->DeriveOutputColumns();
	CColRefSet *pcrsRight = pexprRight->DeriveOutputColumns();

	EqualityPredicate(mp, pcrsLeft, pcrsRight, pdrspqexpr);

	return SPQOS_NEW(mp)
		CExpression(mp, SPQOS_NEW(mp) CLogicalInnerJoin(mp), pdrspqexpr);
}


//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PexprLogicalNAryJoin
//
//	@doc:
//		Generate randomized n-ary join expression
//
//---------------------------------------------------------------------------
CExpression *
CTestUtils::PexprLogicalNAryJoin(CMemoryPool *mp, CExpressionArray *pdrspqexpr)
{
	SPQOS_ASSERT(NULL != pdrspqexpr);
	SPQOS_ASSERT(2 < pdrspqexpr->Size());

	return SPQOS_NEW(mp)
		CExpression(mp, SPQOS_NEW(mp) CLogicalNAryJoin(mp), pdrspqexpr);
}

//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PexprLogicalNAryJoin
//
//	@doc:
//		Generate randomized n-ary join expression
//
//---------------------------------------------------------------------------
CExpression *
CTestUtils::PexprLogicalNAryJoin(CMemoryPool *mp)
{
	const ULONG ulRels = 4;

	CExpressionArray *pdrspqexpr = SPQOS_NEW(mp) CExpressionArray(mp);
	CExpressionArray *pdrspqexprConjuncts = SPQOS_NEW(mp) CExpressionArray(mp);

	CExpression *pexprInit = PexprLogicalGet(mp);
	pdrspqexpr->Append(pexprInit);

	// build pairwise joins, extract predicate and input, then discard join
	for (ULONG ul = 0; ul < ulRels; ul++)
	{
		CExpression *pexprNext = PexprLogicalGet(mp);
		pdrspqexpr->Append(pexprNext);

		pexprInit->AddRef();
		pexprNext->AddRef();

		CExpression *pexprJoin =
			PexprLogicalJoin<CLogicalInnerJoin>(mp, pexprInit, pexprNext);

		CExpression *pexprPredicate = (*pexprJoin)[2];

		pexprPredicate->AddRef();
		pdrspqexprConjuncts->Append(pexprPredicate);

		pexprJoin->Release();
	}

	// add predicate built of conjuncts to the input array
	pdrspqexpr->Append(CUtils::PexprScalarBoolOp(mp, CScalarBoolOp::EboolopAnd,
												pdrspqexprConjuncts));

	return PexprLogicalNAryJoin(mp, pdrspqexpr);
}


//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PexprLeftOuterJoinOnNAryJoin
//
//	@doc:
//		Generate left outer join on top of n-ary join expression
//
//---------------------------------------------------------------------------
CExpression *
CTestUtils::PexprLeftOuterJoinOnNAryJoin(CMemoryPool *mp)
{
	CExpression *pexprNAryJoin = PexprLogicalNAryJoin(mp);
	CExpression *pexprLOJInnerChild = PexprLogicalGet(mp);

	// get a random column from LOJ inner child output
	CColRef *pcrLeft1 = pexprLOJInnerChild->DeriveOutputColumns()->PcrAny();

	// get a random column from NAry join second child
	CColRef *pcrLeft2 = (*pexprNAryJoin)[1]->DeriveOutputColumns()->PcrAny();

	// get a random column from NAry join output
	CColRef *pcrRight = pexprNAryJoin->DeriveOutputColumns()->PcrAny();

	CExpression *pexprPred1 = CUtils::PexprScalarEqCmp(mp, pcrLeft1, pcrRight);
	CExpression *pexprPred2 = CUtils::PexprScalarEqCmp(mp, pcrLeft2, pcrRight);
	CExpression *pexprPred =
		CPredicateUtils::PexprConjunction(mp, pexprPred1, pexprPred2);
	pexprPred1->Release();
	pexprPred2->Release();

	return SPQOS_NEW(mp)
		CExpression(mp, SPQOS_NEW(mp) CLogicalLeftOuterJoin(mp), pexprNAryJoin,
					pexprLOJInnerChild, pexprPred);
}

CExpression *
CTestUtils::PexprRightOuterJoin(CMemoryPool *mp)
{
	CExpression *pexprInnerChild = PexprLogicalGet(mp);
	CExpression *pexprOuterChild = PexprLogicalGet(mp);

	// get a random column from inner child output
	CColRef *pcrLeft = pexprInnerChild->DeriveOutputColumns()->PcrAny();

	// get a random column from outer child output
	CColRef *pcrRight = pexprOuterChild->DeriveOutputColumns()->PcrAny();

	CExpression *pexprPred = CUtils::PexprScalarEqCmp(mp, pcrLeft, pcrRight);

	return SPQOS_NEW(mp)
		CExpression(mp, SPQOS_NEW(mp) CLogicalRightOuterJoin(mp),
					pexprOuterChild, pexprInnerChild, pexprPred);
}


//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PexprNAryJoinOnLeftOuterJoin
//
//	@doc:
//		Generate n-ary join expression on top of left outer join
//
//---------------------------------------------------------------------------
CExpression *
CTestUtils::PexprNAryJoinOnLeftOuterJoin(CMemoryPool *mp)
{
	CExpression *pexprNAryJoin = PexprLogicalNAryJoin(mp);
	CColRef *pcrNAryJoin = (*pexprNAryJoin)[0]->DeriveOutputColumns()->PcrAny();
	CExpression *pexprScalar = (*pexprNAryJoin)[pexprNAryJoin->Arity() - 1];

	// copy NAry-Join children
	CExpressionArray *pdrspqexpr = SPQOS_NEW(mp) CExpressionArray(mp);
	CUtils::AddRefAppend<CExpression>(pdrspqexpr, pexprNAryJoin->PdrgPexpr());

	// generate LOJ expression
	CExpression *pexprLOJOuterChild = PexprLogicalGet(mp);
	CExpression *pexprLOJInnerChild = PexprLogicalGet(mp);

	// get a random column from LOJ outer child output
	CColRef *pcrLeft = pexprLOJOuterChild->DeriveOutputColumns()->PcrAny();

	// get a random column from LOJ inner child output
	CColRef *pcrRight = pexprLOJInnerChild->DeriveOutputColumns()->PcrAny();

	CExpression *pexprPred = CUtils::PexprScalarEqCmp(mp, pcrLeft, pcrRight);
	CExpression *pexprLOJ = SPQOS_NEW(mp)
		CExpression(mp, SPQOS_NEW(mp) CLogicalLeftOuterJoin(mp),
					pexprLOJOuterChild, pexprLOJInnerChild, pexprPred);

	// replace NAry-Join scalar predicate with LOJ expression
	pdrspqexpr->Replace(pdrspqexpr->Size() - 1, pexprLOJ);

	// create new scalar predicate for NAry join
	CExpression *pexprNewPred =
		CUtils::PexprScalarEqCmp(mp, pcrNAryJoin, pcrRight);
	pdrspqexpr->Append(
		CPredicateUtils::PexprConjunction(mp, pexprScalar, pexprNewPred));
	pexprNAryJoin->Release();
	pexprNewPred->Release();

	return SPQOS_NEW(mp)
		CExpression(mp, SPQOS_NEW(mp) CLogicalNAryJoin(mp), pdrspqexpr);
}


//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PexprLogicalLimit
//
//	@doc:
//		Generate a limit expression
//
//---------------------------------------------------------------------------
CExpression *
CTestUtils::PexprLogicalLimit(CMemoryPool *mp, CExpression *pexpr, LINT iStart,
							  LINT iRows, BOOL fGlobal, BOOL fHasCount)
{
	SPQOS_ASSERT(NULL != pexpr);
	SPQOS_ASSERT(0 <= iStart);
	SPQOS_ASSERT(0 <= iRows);

	COrderSpec *pos = SPQOS_NEW(mp) COrderSpec(mp);
	return SPQOS_NEW(mp)
		CExpression(mp,
					SPQOS_NEW(mp) CLogicalLimit(mp, pos, fGlobal, fHasCount,
											   false /*fTopLimitUnderDML*/),
					pexpr, CUtils::PexprScalarConstInt8(mp, iStart),
					CUtils::PexprScalarConstInt8(mp, iRows));
}

//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PexprLogicalLimit
//
//	@doc:
//		Generate a randomized limit expression
//
//---------------------------------------------------------------------------
CExpression *
CTestUtils::PexprLogicalLimit(CMemoryPool *mp)
{
	return PexprLogicalLimit(mp, PexprLogicalGet(mp),
							 0,		// iStart
							 100,	// iRows
							 true,	// fGlobal
							 true	// fHasCount
	);
}


//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PexprLogicalGbAggWithSum
//
//	@doc:
//		Generate a randomized aggregate expression with sum agg function
//
//---------------------------------------------------------------------------
CExpression *
CTestUtils::PexprLogicalGbAggWithSum(CMemoryPool *mp)
{
	CExpression *pexprGb = PexprLogicalGbAgg(mp);

	// get a random column from Gb child
	CColRef *colref = (*pexprGb)[0]->DeriveOutputColumns()->PcrAny();

	// generate a SUM expression
	CExpression *pexprProjElem = PexprPrjElemWithSum(mp, colref);
	CExpression *pexprPrjList = SPQOS_NEW(mp)
		CExpression(mp, SPQOS_NEW(mp) CScalarProjectList(mp), pexprProjElem);

	(*pexprGb)[0]->AddRef();
	CColRefArray *colref_array =
		CLogicalGbAgg::PopConvert(pexprGb->Pop())->Pdrspqcr();
	colref_array->AddRef();
	CExpression *pexprGbWithSum = CUtils::PexprLogicalGbAggGlobal(
		mp, colref_array, (*pexprGb)[0], pexprPrjList);

	// release old Gb expression
	pexprGb->Release();

	return pexprGbWithSum;
}

//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PexprLogicalGbAggWithInput
//
//	@doc:
//		Generate a randomized aggregate expression
//
//---------------------------------------------------------------------------
CExpression *
CTestUtils::PexprLogicalGbAggWithInput(CMemoryPool *mp, CExpression *pexprInput)
{
	// get the first few columns
	CColRefSet *pcrs = pexprInput->DeriveOutputColumns();

	ULONG num_cols = std::min(3u, pcrs->Size());

	CColRefArray *colref_array = SPQOS_NEW(mp) CColRefArray(mp);
	CColRefSetIter crsi(*pcrs);
	for (ULONG i = 0; i < num_cols; i++)
	{
		(void) crsi.Advance();
		colref_array->Append(crsi.Pcr());
	}

	CExpression *pexprList =
		SPQOS_NEW(mp) CExpression(mp, SPQOS_NEW(mp) CScalarProjectList(mp));
	return CUtils::PexprLogicalGbAggGlobal(mp, colref_array, pexprInput,
										   pexprList);
}

//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PexprLogicalGbAgg
//
//	@doc:
//		Generate a randomized aggregate expression
//
//---------------------------------------------------------------------------
CExpression *
CTestUtils::PexprLogicalGbAgg(CMemoryPool *mp)
{
	return PexprLogicalGbAggWithInput(mp, PexprLogicalGet(mp));
}

//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PexprPrjElemWithSum
//
//	@doc:
//		Generate a project element with sum agg function
//
//---------------------------------------------------------------------------
CExpression *
CTestUtils::PexprPrjElemWithSum(CMemoryPool *mp, CColRef *colref)
{
	CMDAccessor *md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();

	// generate a SUM expression
	CMDIdSPQDB *pmdidSumAgg =
		SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_INT4_SUM_AGG);
	CWStringConst *pstrAggFunc =
		SPQOS_NEW(mp) CWStringConst(SPQOS_WSZ_LIT("sum"));
	CExpression *pexprScalarAgg = CUtils::PexprAggFunc(
		mp, pmdidSumAgg, pstrAggFunc, colref, false /*is_distinct*/,
		EaggfuncstageGlobal /*eaggfuncstage*/, false /*fSplit*/
	);

	// map a computed column to SUM expression
	CScalar *pop = CScalar::PopConvert(pexprScalarAgg->Pop());
	IMDId *mdid_type = pop->MdidType();
	const IMDType *pmdtype = md_accessor->RetrieveType(mdid_type);
	CWStringConst str(SPQOS_WSZ_LIT("sum_col"));
	CName name(mp, &str);
	CColRef *pcrComputed = COptCtxt::PoctxtFromTLS()->Pcf()->PcrCreate(
		pmdtype, pop->TypeModifier(), name);

	return CUtils::PexprScalarProjectElement(mp, pcrComputed, pexprScalarAgg);
}


//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PexprLogicalGbAggOverJoin
//
//	@doc:
//		Generate a randomized group by over join expression
//
//---------------------------------------------------------------------------
CExpression *
CTestUtils::PexprLogicalGbAggOverJoin(CMemoryPool *mp)
{
	// generate a join expression
	CExpression *pexprJoin = PexprLogicalJoin<CLogicalInnerJoin>(mp);

	// include one grouping column
	CColRefArray *colref_array = SPQOS_NEW(mp) CColRefArray(mp);
	CColRef *colref = pexprJoin->DeriveOutputColumns()->PcrAny();
	colref_array->Append(colref);

	return CUtils::PexprLogicalGbAggGlobal(
		mp, colref_array, pexprJoin,
		SPQOS_NEW(mp) CExpression(mp, SPQOS_NEW(mp) CScalarProjectList(mp)));
}

//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PexprLogicalGbAggDedupOverInnerJoin
//
//	@doc:
//		Generate a dedup group by over inner join expression
//
//---------------------------------------------------------------------------
CExpression *
CTestUtils::PexprLogicalGbAggDedupOverInnerJoin(CMemoryPool *mp)
{
	// generate a join expression
	CExpression *pexprJoin = PexprLogicalJoin<CLogicalInnerJoin>(mp);

	// get join's outer child
	CExpression *pexprOuter = (*pexprJoin)[0];
	CColRefSet *pcrs = pexprOuter->DeriveOutputColumns();

	// grouping columns: all columns from outer child
	CColRefArray *pdrspqcrGrp = pcrs->Pdrspqcr(mp);

	// outer child keys: get a random column from its output columns
	CColRef *colref = pcrs->PcrAny();
	CColRefArray *pdrspqcrKeys = SPQOS_NEW(mp) CColRefArray(mp);
	pdrspqcrKeys->Append(colref);

	return SPQOS_NEW(mp) CExpression(
		mp,
		SPQOS_NEW(mp) CLogicalGbAggDeduplicate(
			mp, pdrspqcrGrp, COperator::EgbaggtypeGlobal /*egbaggtype*/,
			pdrspqcrKeys),
		pexprJoin,
		SPQOS_NEW(mp) CExpression(mp, SPQOS_NEW(mp) CScalarProjectList(mp)));
}

//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PexprLogicalGbAggCorrelated
//
//	@doc:
//		Generate correlated GbAgg
//
//---------------------------------------------------------------------------
CExpression *
CTestUtils::PexprLogicalGbAggCorrelated(CMemoryPool *mp)
{
	CExpression *pexpr = PexprLogicalSelectCorrelated(mp);

	// include one grouping column
	CColRefArray *colref_array = SPQOS_NEW(mp) CColRefArray(mp);
	CColRef *colref = pexpr->DeriveOutputColumns()->PcrAny();
	colref_array->Append(colref);

	CExpression *pexprList =
		SPQOS_NEW(mp) CExpression(mp, SPQOS_NEW(mp) CScalarProjectList(mp));
	return CUtils::PexprLogicalGbAggGlobal(mp, colref_array, pexpr, pexprList);
}

//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PexprConstTableGet
//
//	@doc:
//		Generate logical const table get expression with one column and
//		the given number of elements
//
//---------------------------------------------------------------------------
CExpression *
CTestUtils::PexprConstTableGet(CMemoryPool *mp, ULONG ulElements)
{
	SPQOS_ASSERT(0 < ulElements);

	CMDAccessor *md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();

	const IMDTypeInt4 *pmdtypeint4 =
		md_accessor->PtMDType<IMDTypeInt4>(CTestUtils::m_sysidDefault);

	// create an integer column descriptor
	CWStringConst strName(SPQOS_WSZ_LIT("A"));
	CName name(&strName);
	CColumnDescriptor *pcoldescInt =
		SPQOS_NEW(mp) CColumnDescriptor(mp, pmdtypeint4, default_type_modifier,
									   name, 1 /* attno */, false /*IsNullable*/
		);

	CColumnDescriptorArray *pdrspqcoldesc =
		SPQOS_NEW(mp) CColumnDescriptorArray(mp);
	pdrspqcoldesc->Append(pcoldescInt);

	// generate values
	IDatum2dArray *pdrspqdrspqdatum = SPQOS_NEW(mp) IDatum2dArray(mp);

	for (ULONG ul = 0; ul < ulElements; ul++)
	{
		IDatumInt4 *datum =
			pmdtypeint4->CreateInt4Datum(mp, ul, false /* is_null */);
		IDatumArray *pdrspqdatum = SPQOS_NEW(mp) IDatumArray(mp);
		pdrspqdatum->Append((IDatum *) datum);
		pdrspqdrspqdatum->Append(pdrspqdatum);
	}
	CLogicalConstTableGet *popConstTableGet =
		SPQOS_NEW(mp) CLogicalConstTableGet(mp, pdrspqcoldesc, pdrspqdrspqdatum);

	return SPQOS_NEW(mp) CExpression(mp, popConstTableGet);
}

//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PexprConstTableGet5
//
//	@doc:
//		Generate logical const table get expression with one column and 5 elements
//
//---------------------------------------------------------------------------
CExpression *
CTestUtils::PexprConstTableGet5(CMemoryPool *mp)
{
	return PexprConstTableGet(mp, 5 /* ulElements */);
}

//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PexprLogicalInsert
//
//	@doc:
//		Generate logical insert expression
//
//---------------------------------------------------------------------------
CExpression *
CTestUtils::PexprLogicalInsert(CMemoryPool *mp)
{
	CExpression *pexprCTG = PexprConstTableGet(mp, 1 /* ulElements */);

	CColRefSet *pcrs = pexprCTG->DeriveOutputColumns();
	CColRefArray *colref_array = pcrs->Pdrspqcr(mp);

	CWStringConst strName(SPQOS_WSZ_LIT("BaseTable"));
	CMDIdSPQDB *mdid =
		SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidRel, SPQOPT_MDCACHE_TEST_OID, 1, 1);
	CTableDescriptor *ptabdesc = PtabdescCreate(mp, 1, mdid, CName(&strName));

	return SPQOS_NEW(mp) CExpression(
		mp, SPQOS_NEW(mp) CLogicalInsert(mp, ptabdesc, colref_array), pexprCTG);
}

//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PexprLogicalDelete
//
//	@doc:
//		Generate logical delete expression
//
//---------------------------------------------------------------------------
CExpression *
CTestUtils::PexprLogicalDelete(CMemoryPool *mp)
{
	CWStringConst strName(SPQOS_WSZ_LIT("BaseTable"));
	CMDIdSPQDB *mdid =
		SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidRel, SPQOPT_MDCACHE_TEST_OID, 1, 1);
	CTableDescriptor *ptabdesc = PtabdescCreate(mp, 1, mdid, CName(&strName));
	CWStringConst strAlias(SPQOS_WSZ_LIT("BaseTableAlias"));

	CExpression *pexprGet = PexprLogicalGet(mp, ptabdesc, &strAlias);

	CColRefSet *pcrs = pexprGet->DeriveOutputColumns();
	CColRefArray *colref_array = pcrs->Pdrspqcr(mp);

	CColRef *colref = pcrs->PcrFirst();

	ptabdesc->AddRef();

	return SPQOS_NEW(mp) CExpression(
		mp,
		SPQOS_NEW(mp) CLogicalDelete(mp, ptabdesc, colref_array, colref, colref),
		pexprGet);
}

//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PexprLogicalUpdate
//
//	@doc:
//		Generate logical update expression
//
//---------------------------------------------------------------------------
CExpression *
CTestUtils::PexprLogicalUpdate(CMemoryPool *mp)
{
	CWStringConst strName(SPQOS_WSZ_LIT("BaseTable"));
	CMDIdSPQDB *mdid =
		SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidRel, SPQOPT_MDCACHE_TEST_OID, 1, 1);
	CTableDescriptor *ptabdesc = PtabdescCreate(mp, 1, mdid, CName(&strName));
	CWStringConst strAlias(SPQOS_WSZ_LIT("BaseTableAlias"));

	CExpression *pexprGet = PexprLogicalGet(mp, ptabdesc, &strAlias);

	CColRefSet *pcrs = pexprGet->DeriveOutputColumns();
	CColRefArray *pdrspqcrDelete = pcrs->Pdrspqcr(mp);
	CColRefArray *pdrspqcrInsert = pcrs->Pdrspqcr(mp);

	CColRef *colref = pcrs->PcrFirst();

	ptabdesc->AddRef();

	return SPQOS_NEW(mp) CExpression(
		mp,
		SPQOS_NEW(mp) CLogicalUpdate(mp, ptabdesc, pdrspqcrDelete, pdrspqcrInsert,
									colref, colref, NULL /*pcrTupleOid*/),
		pexprGet);
}

//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PexprLogicalDynamicGet
//
//	@doc:
//		Generate a dynamic get expression
//
//---------------------------------------------------------------------------
CExpression *
CTestUtils::PexprLogicalDynamicGet(CMemoryPool *mp, CTableDescriptor *ptabdesc,
								   const CWStringConst *pstrTableAlias,
								   ULONG ulPartIndex)
{
	SPQOS_ASSERT(NULL != ptabdesc);

	return SPQOS_NEW(mp)
		CExpression(mp, SPQOS_NEW(mp) CLogicalDynamicGet(
							mp, SPQOS_NEW(mp) CName(mp, CName(pstrTableAlias)),
							ptabdesc, ulPartIndex));
}

//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PexprLogicalDynamicGet
//
//	@doc:
//		Generate a dynamic get expression
//
//---------------------------------------------------------------------------
CExpression *
CTestUtils::PexprLogicalDynamicGet(CMemoryPool *mp)
{
	CWStringConst strName(SPQOS_WSZ_LIT("PartTable"));
	CMDIdSPQDB *mdid = SPQOS_NEW(mp)
		CMDIdSPQDB(IMDId::EmdidRel, SPQOPT_MDCACHE_TEST_OID_PARTITIONED, 1, 1);
	CTableDescriptor *ptabdesc =
		PtabdescCreate(mp, 3, mdid, CName(&strName), true /*fPartitioned*/);
	CWStringConst strAlias(SPQOS_WSZ_LIT("PartTableAlias"));

	return PexprLogicalDynamicGet(mp, ptabdesc, &strAlias,
								  SPQOPT_TEST_PART_INDEX);
}

//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PexprLogicalSelectWithEqPredicateOverDynamicGet
//
//	@doc:
//		Generate a select over dynamic get expression with a predicate on the
//		partition key
//
//---------------------------------------------------------------------------
CExpression *
CTestUtils::PexprLogicalSelectWithEqPredicateOverDynamicGet(CMemoryPool *mp)
{
	CWStringConst strName(SPQOS_WSZ_LIT("PartTable"));
	CMDIdSPQDB *mdid = SPQOS_NEW(mp)
		CMDIdSPQDB(IMDId::EmdidRel, SPQOPT_MDCACHE_TEST_OID_PARTITIONED, 1, 1);
	CTableDescriptor *ptabdesc =
		PtabdescCreate(mp, 3, mdid, CName(&strName), true /*fPartitioned*/);
	CWStringConst strAlias(SPQOS_WSZ_LIT("PartTableAlias"));

	CExpression *pexprDynamicGet =
		PexprLogicalDynamicGet(mp, ptabdesc, &strAlias, SPQOPT_TEST_PART_INDEX);

	// construct scalar comparison
	CLogicalDynamicGet *popDynamicGet =
		CLogicalDynamicGet::PopConvert(pexprDynamicGet->Pop());
	CColRef2dArray *pdrspqdrspqcr = popDynamicGet->PdrspqdrspqcrPart();
	CColRef *colref = CUtils::PcrExtractPartKey(pdrspqdrspqcr, 0 /*ulLevel*/);
	CExpression *pexprScalarIdent = CUtils::PexprScalarIdent(mp, colref);

	CExpression *pexprConst = CUtils::PexprScalarConstInt4(mp, 5 /*val*/);
	CExpression *pexprScalar =
		CUtils::PexprScalarEqCmp(mp, pexprScalarIdent, pexprConst);

	return CUtils::PexprLogicalSelect(mp, pexprDynamicGet, pexprScalar);
}

//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PexprLogicalSelectWithLTPredicateOverDynamicGet
//
//	@doc:
//		Generate a select over dynamic get expression with a predicate on the
//		partition key
//
//---------------------------------------------------------------------------
CExpression *
CTestUtils::PexprLogicalSelectWithLTPredicateOverDynamicGet(CMemoryPool *mp)
{
	CWStringConst strName(SPQOS_WSZ_LIT("PartTable"));
	CMDIdSPQDB *mdid = SPQOS_NEW(mp)
		CMDIdSPQDB(IMDId::EmdidRel, SPQOPT_MDCACHE_TEST_OID_PARTITIONED, 1, 1);
	CTableDescriptor *ptabdesc =
		PtabdescCreate(mp, 3, mdid, CName(&strName), true /*fPartitioned*/);
	CWStringConst strAlias(SPQOS_WSZ_LIT("PartTableAlias"));

	CExpression *pexprDynamicGet =
		PexprLogicalDynamicGet(mp, ptabdesc, &strAlias, SPQOPT_TEST_PART_INDEX);

	// construct scalar comparison
	CLogicalDynamicGet *popDynamicGet =
		CLogicalDynamicGet::PopConvert(pexprDynamicGet->Pop());
	CColRef2dArray *pdrspqdrspqcr = popDynamicGet->PdrspqdrspqcrPart();
	CColRef *colref = CUtils::PcrExtractPartKey(pdrspqdrspqcr, 0 /*ulLevel*/);
	CExpression *pexprScalarIdent = CUtils::PexprScalarIdent(mp, colref);

	CExpression *pexprConst = CUtils::PexprScalarConstInt4(mp, 5 /*val*/);
	CExpression *pexprScalar = CUtils::PexprScalarCmp(
		mp, pexprScalarIdent, pexprConst, IMDType::EcmptL);

	return CUtils::PexprLogicalSelect(mp, pexprDynamicGet, pexprScalar);
}

//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PexprLogicalTVFTwoArgs
//
//	@doc:
//		Generate a logical TVF expression with 2 constant arguments
//
//---------------------------------------------------------------------------
CExpression *
CTestUtils::PexprLogicalTVFTwoArgs(CMemoryPool *mp)
{
	return PexprLogicalTVF(mp, 2);
}

//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PexprLogicalTVFThreeArgs
//
//	@doc:
//		Generate a logical TVF expression with 3 constant arguments
//
//---------------------------------------------------------------------------
CExpression *
CTestUtils::PexprLogicalTVFThreeArgs(CMemoryPool *mp)
{
	return PexprLogicalTVF(mp, 3);
}

//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PexprLogicalTVFNoArgs
//
//	@doc:
//		Generate a logical TVF expression with no arguments
//
//---------------------------------------------------------------------------
CExpression *
CTestUtils::PexprLogicalTVFNoArgs(CMemoryPool *mp)
{
	return PexprLogicalTVF(mp, 0);
}

//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PexprLogicalTVF
//
//	@doc:
//		Generate a TVF expression
//
//---------------------------------------------------------------------------
CExpression *
CTestUtils::PexprLogicalTVF(CMemoryPool *mp, ULONG ulArgs)
{
	CMDAccessor *md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();

	const WCHAR *wszFuncName = SPQOS_WSZ_LIT("generate_series");

	IMDId *mdid =
		SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_INT8_GENERATE_SERIES);
	CWStringConst *str_func_name = SPQOS_NEW(mp) CWStringConst(mp, wszFuncName);

	const IMDTypeInt8 *pmdtypeint8 =
		md_accessor->PtMDType<IMDTypeInt8>(CTestUtils::m_sysidDefault);

	// create an integer column descriptor
	CWStringConst strName(SPQOS_WSZ_LIT("generate_series"));
	CName name(&strName);

	CColumnDescriptor *pcoldescInt = SPQOS_NEW(mp)
		CColumnDescriptor(mp, pmdtypeint8, default_type_modifier, name,
						  1 /* attno */, false /*IsNullable*/);

	CColumnDescriptorArray *pdrspqcoldesc =
		SPQOS_NEW(mp) CColumnDescriptorArray(mp);
	pdrspqcoldesc->Append(pcoldescInt);

	IMDId *mdid_return_type = pmdtypeint8->MDId();
	mdid_return_type->AddRef();

	CLogicalTVF *popTVF = SPQOS_NEW(mp)
		CLogicalTVF(mp, mdid, mdid_return_type, str_func_name, pdrspqcoldesc);

	if (0 == ulArgs)
	{
		return SPQOS_NEW(mp) CExpression(mp, popTVF);
	}

	CExpressionArray *pdrspqexprArgs = SPQOS_NEW(mp) CExpressionArray(mp);

	for (ULONG ul = 0; ul < ulArgs; ul++)
	{
		ULONG ulArg = 1;
		IDatum *datum = (IDatum *) pmdtypeint8->CreateInt8Datum(
			mp, ulArg, false /* is_null */);
		CScalarConst *popConst = SPQOS_NEW(mp) CScalarConst(mp, datum);
		pdrspqexprArgs->Append(SPQOS_NEW(mp) CExpression(mp, popConst));
	}

	return SPQOS_NEW(mp) CExpression(mp, popTVF, pdrspqexprArgs);
}

//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PexprLogicalCTEProducerOverSelect
//
//	@doc:
//		Generate a CTE producer on top of a select
//
//---------------------------------------------------------------------------
CExpression *
CTestUtils::PexprLogicalCTEProducerOverSelect(CMemoryPool *mp, ULONG ulCTEId)
{
	CExpression *pexprSelect = CTestUtils::PexprLogicalSelect(mp);
	CColRefArray *colref_array =
		pexprSelect->DeriveOutputColumns()->Pdrspqcr(mp);

	return SPQOS_NEW(mp) CExpression(
		mp, SPQOS_NEW(mp) CLogicalCTEProducer(mp, ulCTEId, colref_array),
		pexprSelect);
}

//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PexprLogicalCTEProducerOverSelect
//
//	@doc:
//		Generate a CTE producer on top of a select
//
//---------------------------------------------------------------------------
CExpression *
CTestUtils::PexprLogicalCTEProducerOverSelect(CMemoryPool *mp)
{
	return PexprLogicalCTEProducerOverSelect(mp, 0);
}

//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PexprCTETree
//
//	@doc:
//		Generate an expression with CTE producer and consumer
//
//---------------------------------------------------------------------------
CExpression *
CTestUtils::PexprCTETree(CMemoryPool *mp)
{
	ULONG ulCTEId = 0;
	CExpressionArray *pdrspqexpr = SPQOS_NEW(mp) CExpressionArray(mp);

	CExpression *pexprProducer = PexprLogicalCTEProducerOverSelect(mp, ulCTEId);
	COptCtxt::PoctxtFromTLS()->Pcteinfo()->AddCTEProducer(pexprProducer);

	pdrspqexpr->Append(pexprProducer);

	CColRefArray *pdrspqcrProducer =
		CLogicalCTEProducer::PopConvert(pexprProducer->Pop())->Pdrspqcr();
	CColRefArray *pdrspqcrConsumer = CUtils::PdrspqcrCopy(mp, pdrspqcrProducer);

	CExpression *pexprConsumer = SPQOS_NEW(mp) CExpression(
		mp, SPQOS_NEW(mp) CLogicalCTEConsumer(mp, ulCTEId, pdrspqcrConsumer));

	pdrspqexpr->Append(pexprConsumer);

	return PexprLogicalSequence(mp, pdrspqexpr);
}

//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PexprLogicalSequence
//
//	@doc:
//		Generate a dynamic get expression
//
//---------------------------------------------------------------------------
CExpression *
CTestUtils::PexprLogicalSequence(CMemoryPool *mp, CExpressionArray *pdrspqexpr)
{
	SPQOS_ASSERT(NULL != pdrspqexpr);

	return SPQOS_NEW(mp)
		CExpression(mp, SPQOS_NEW(mp) CLogicalSequence(mp), pdrspqexpr);
}

//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PexprLogicalSequence
//
//	@doc:
//		Generate a logical sequence expression
//
//---------------------------------------------------------------------------
CExpression *
CTestUtils::PexprLogicalSequence(CMemoryPool *mp)
{
	const ULONG ulRels = 2;
	const ULONG ulCTGElements = 2;

	CExpressionArray *pdrspqexpr = SPQOS_NEW(mp) CExpressionArray(mp);

	// build an array of get expressions
	for (ULONG ul = 0; ul < ulRels; ul++)
	{
		CExpression *pexpr = PexprLogicalGet(mp);
		pdrspqexpr->Append(pexpr);
	}

	CExpression *pexprCTG = PexprConstTableGet(mp, ulCTGElements);
	pdrspqexpr->Append(pexprCTG);

	CExpression *pexprDynamicGet = PexprLogicalDynamicGet(mp);
	pdrspqexpr->Append(pexprDynamicGet);

	return PexprLogicalSequence(mp, pdrspqexpr);
}

//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PexprLogicalUnion
//
//	@doc:
//		Generate tree of unions recursively
//
//---------------------------------------------------------------------------
CExpression *
CTestUtils::PexprLogicalUnion(CMemoryPool *mp, ULONG ulDepth)
{
	// stack check in recursion
	SPQOS_CHECK_STACK_SIZE;

	CExpression *pexpr = NULL;

	if (0 == ulDepth)
	{
		// terminal case, generate table
		pexpr = PexprLogicalGet(mp);
	}
	else
	{
		// recursive case, generate union w/ 3 children
		CExpressionArray *pdrspqexprInput = SPQOS_NEW(mp) CExpressionArray(mp, 3);
		CColRef2dArray *pdrspqdrspqcrInput = SPQOS_NEW(mp) CColRef2dArray(mp);

		for (ULONG i = 0; i < 3; i++)
		{
			CExpression *pexprInput = PexprLogicalUnion(mp, ulDepth - 1);
			COperator *pop = pexprInput->Pop();
			CColRefArray *colref_array = NULL;

			if (pop->Eopid() == COperator::EopLogicalGet)
			{
				CLogicalGet *popGet = CLogicalGet::PopConvert(pop);
				colref_array = popGet->PdrspqcrOutput();
			}
			else
			{
				SPQOS_ASSERT(COperator::EopLogicalUnion == pop->Eopid());
				CLogicalUnion *popUnion = CLogicalUnion::PopConvert(pop);
				colref_array = popUnion->PdrspqcrOutput();
			}
			pdrspqexprInput->Append(pexprInput);
			SPQOS_ASSERT(NULL != colref_array);

			colref_array->AddRef();
			pdrspqdrspqcrInput->Append(colref_array);
		}

		// remap columns of first input
		CColRefArray *pdrspqcrInput = (*pdrspqdrspqcrInput)[0];
		ULONG num_cols = pdrspqcrInput->Size();
		CColRefArray *pdrspqcrOutput = SPQOS_NEW(mp) CColRefArray(mp, num_cols);

		for (ULONG j = 0; j < num_cols; j++)
		{
			pdrspqcrOutput->Append((*pdrspqcrInput)[j]);
		}

		pexpr = SPQOS_NEW(mp) CExpression(
			mp, SPQOS_NEW(mp) CLogicalUnion(mp, pdrspqcrOutput, pdrspqdrspqcrInput),
			pdrspqexprInput);
	}

	return pexpr;
}

//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PexprLogicalSequenceProject
//
//	@doc:
//		Generate a logical sequence project expression
//
//---------------------------------------------------------------------------
CExpression *
CTestUtils::PexprLogicalSequenceProject(CMemoryPool *mp, OID oidFunc,
										CExpression *pexprInput)
{
	CMDAccessor *md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();
	CColumnFactory *col_factory = COptCtxt::PoctxtFromTLS()->Pcf();

	CExpressionArray *pdrspqexpr = SPQOS_NEW(mp) CExpressionArray(mp);
	pdrspqexpr->Append(CUtils::PexprScalarIdent(
		mp, pexprInput->DeriveOutputColumns()->PcrAny()));
	COrderSpecArray *pdrspqos = SPQOS_NEW(mp) COrderSpecArray(mp);
	CWindowFrameArray *pdrgwf = SPQOS_NEW(mp) CWindowFrameArray(mp);

	CLogicalSequenceProject *popSeqProj = SPQOS_NEW(mp) CLogicalSequenceProject(
		mp,
		SPQOS_NEW(mp)
			CDistributionSpecHashed(pdrspqexpr, true /*fNullsCollocated*/),
		pdrspqos, pdrgwf);

	IMDId *mdid = SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, oidFunc);
	const IMDFunction *pmdfunc = md_accessor->RetrieveFunc(mdid);

	IMDId *mdid_return_type = pmdfunc->GetResultTypeMdid();
	mdid_return_type->AddRef();

	CExpression *pexprWinFunc = SPQOS_NEW(mp) CExpression(
		mp, SPQOS_NEW(mp) CScalarWindowFunc(
				mp, mdid, mdid_return_type,
				SPQOS_NEW(mp) CWStringConst(
					mp, pmdfunc->Mdname().GetMDName()->GetBuffer()),
				CScalarWindowFunc::EwsImmediate, false /*is_distinct*/,
				false /*is_star_arg*/, false /*is_simple_agg*/
				));
	// window function call is not a cast and so does not need a type modifier
	CColRef *pcrComputed = col_factory->PcrCreate(
		md_accessor->RetrieveType(pmdfunc->GetResultTypeMdid()),
		default_type_modifier);

	CExpression *pexprPrjList = SPQOS_NEW(mp) CExpression(
		mp, SPQOS_NEW(mp) CScalarProjectList(mp),
		CUtils::PexprScalarProjectElement(mp, pcrComputed, pexprWinFunc));

	return SPQOS_NEW(mp) CExpression(mp, popSeqProj, pexprInput, pexprPrjList);
}


//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PexprOneWindowFunction
//
//	@doc:
//		Generate a random expression with one window function
//
//---------------------------------------------------------------------------
CExpression *
CTestUtils::PexprOneWindowFunction(CMemoryPool *mp)
{
	CExpression *pexprGet = PexprLogicalGet(mp);

	OID row_number_oid = COptCtxt::PoctxtFromTLS()
							 ->GetOptimizerConfig()
							 ->GetWindowOids()
							 ->OidRowNumber();

	return PexprLogicalSequenceProject(mp, row_number_oid, pexprGet);
}


//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PexprTwoWindowFunctions
//
//	@doc:
//		Generate a random expression with two window functions
//
//---------------------------------------------------------------------------
CExpression *
CTestUtils::PexprTwoWindowFunctions(CMemoryPool *mp)
{
	CExpression *pexprWinFunc = PexprOneWindowFunction(mp);

	OID rank_oid = COptCtxt::PoctxtFromTLS()
					   ->GetOptimizerConfig()
					   ->GetWindowOids()
					   ->OidRank();

	return PexprLogicalSequenceProject(mp, rank_oid, pexprWinFunc);
}


//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PdrspqexprJoins
//
//	@doc:
//		Generate an array of join expressions for the given relation names;
//		the first generated expression is a Select of the first relation;
//		every subsequent expression is a Join between a new relation and the
//		previous join expression
//
//---------------------------------------------------------------------------
CExpressionJoinsArray *
CTestUtils::PdrspqexprJoins(CMemoryPool *mp,
						   CWStringConst *pstrRel,	// array of relation names
						   ULONG *pulRel,			// array of relation oids
						   ULONG ulRels,			// number of relations
						   BOOL fCrossProduct)
{
	CExpressionJoinsArray *pdrspqexpr = SPQOS_NEW(mp) CExpressionJoinsArray(mp);
	for (ULONG i = 0; i < ulRels; i++)
	{
		CExpression *pexpr = NULL;
		if (fCrossProduct)
		{
			pexpr = PexprLogicalGet(mp, &pstrRel[i], &pstrRel[i], pulRel[i]);
		}
		else
		{
			pexpr = PexprLogicalSelect(mp, &pstrRel[i], &pstrRel[i], pulRel[i]);
		}


		if (0 == i)
		{
			// the first join is set to a Select/Get of the first relation
			pdrspqexpr->Append(pexpr);
		}
		else
		{
			CExpression *pexprJoin = NULL;
			if (fCrossProduct)
			{
				// create a cross product
				pexprJoin = CUtils::PexprLogicalJoin<CLogicalInnerJoin>(
					mp, (*pdrspqexpr)[i - 1], pexpr,
					CPredicateUtils::PexprConjunction(
						mp, NULL)  // generate a constant True
				);
			}
			else
			{
				// otherwise, we create a new join out of the last created
				// join and the current Select expression
				pexprJoin = PexprLogicalJoin<CLogicalInnerJoin>(
					mp, (*pdrspqexpr)[i - 1], pexpr);
			}

			pdrspqexpr->Append(pexprJoin);
		}

		SPQOS_CHECK_ABORT;
	}

	return pdrspqexpr;
}


//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PexprLogicalNAryJoin
//
//	@doc:
//		Generate an n-ary join expression using given array of relation names
//
//---------------------------------------------------------------------------
CExpression *
CTestUtils::PexprLogicalNAryJoin(CMemoryPool *mp, CWStringConst *pstrRel,
								 ULONG *pulRel, ULONG ulRels,
								 BOOL fCrossProduct)
{
	CExpressionArray *pdrspqexpr = SPQOS_NEW(mp) CExpressionArray(mp);
	for (ULONG ul = 0; ul < ulRels; ul++)
	{
		CExpression *pexpr =
			PexprLogicalGet(mp, &pstrRel[ul], &pstrRel[ul], pulRel[ul]);
		pdrspqexpr->Append(pexpr);
	}

	CExpressionArray *pdrspqexprPred = NULL;
	if (!fCrossProduct)
	{
		pdrspqexprPred = SPQOS_NEW(mp) CExpressionArray(mp);
		for (ULONG ul = 0; ul < ulRels - 1; ul++)
		{
			CExpression *pexprLeft = (*pdrspqexpr)[ul];
			CExpression *pexprRight = (*pdrspqexpr)[ul + 1];

			// get any two columns; one from each side
			CColRef *pcrLeft = pexprLeft->DeriveOutputColumns()->PcrAny();
			CColRef *pcrRight = pexprRight->DeriveOutputColumns()->PcrAny();
			pdrspqexprPred->Append(
				CUtils::PexprScalarEqCmp(mp, pcrLeft, pcrRight));
		}
	}
	pdrspqexpr->Append(CPredicateUtils::PexprConjunction(mp, pdrspqexprPred));

	return PexprLogicalNAryJoin(mp, pdrspqexpr);
}


//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PqcGenerate
//
//	@doc:
//		Generate a dummy context from an array of column references and
//		empty sort columns for testing
//
//---------------------------------------------------------------------------
CQueryContext *
CTestUtils::PqcGenerate(CMemoryPool *mp, CExpression *pexpr,
						CColRefArray *colref_array)
{
	// generate required columns
	CColRefSet *pcrs = SPQOS_NEW(mp) CColRefSet(mp);
	pcrs->Include(colref_array);

	COrderSpec *pos = SPQOS_NEW(mp) COrderSpec(mp);
	CDistributionSpec *pds = SPQOS_NEW(mp)
		CDistributionSpecSingleton(CDistributionSpecSingleton::EstMaster);

	CRewindabilitySpec *prs = SPQOS_NEW(mp) CRewindabilitySpec(
		CRewindabilitySpec::ErtNone, CRewindabilitySpec::EmhtNoMotion);

	CEnfdOrder *peo = SPQOS_NEW(mp) CEnfdOrder(pos, CEnfdOrder::EomSatisfy);

	// we require exact matching on distribution since final query results must be sent to master
	CEnfdDistribution *ped =
		SPQOS_NEW(mp) CEnfdDistribution(pds, CEnfdDistribution::EdmExact);

	CEnfdRewindability *per =
		SPQOS_NEW(mp) CEnfdRewindability(prs, CEnfdRewindability::ErmSatisfy);

	CCTEReq *pcter = COptCtxt::PoctxtFromTLS()->Pcteinfo()->PcterProducers(mp);

	CReqdPropPlan *prpp =
		SPQOS_NEW(mp) CReqdPropPlan(pcrs, peo, ped, per, pcter);

	colref_array->AddRef();
	CMDNameArray *pdrspqmdname = SPQOS_NEW(mp) CMDNameArray(mp);
	const ULONG length = colref_array->Size();
	for (ULONG ul = 0; ul < length; ul++)
	{
		CColRef *colref = (*colref_array)[ul];
		CMDName *mdname = SPQOS_NEW(mp) CMDName(mp, colref->Name().Pstr());
		pdrspqmdname->Append(mdname);
	}

	return SPQOS_NEW(mp) CQueryContext(mp, pexpr, prpp, colref_array,
									  pdrspqmdname, true /*fDeriveStats*/);
}

//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PqcGenerate
//
//	@doc:
//		Generate a dummy context with a subset of required column, one
//		sort column and one distribution column
//
//---------------------------------------------------------------------------
CQueryContext *
CTestUtils::PqcGenerate(CMemoryPool *mp, CExpression *pexpr)
{
	CColRefSet *pcrs = SPQOS_NEW(mp) CColRefSet(mp);
	pcrs->Include(pexpr->DeriveOutputColumns());

	// keep a subset of columns
	CColRefSet *pcrsOutput = SPQOS_NEW(mp) CColRefSet(mp);
	CColRefSetIter crsi(*pcrs);
	while (crsi.Advance())
	{
		CColRef *colref = crsi.Pcr();
		if (1 != colref->Id() % SPQOPT_TEST_REL_WIDTH)
		{
			pcrsOutput->Include(colref);
		}
	}
	pcrs->Release();

	// construct an ordered array of the output columns
	CColRefArray *colref_array = SPQOS_NEW(mp) CColRefArray(mp);
	CColRefSetIter crsiOutput(*pcrsOutput);
	while (crsiOutput.Advance())
	{
		CColRef *colref = crsiOutput.Pcr();
		colref_array->Append(colref);
	}

	// generate a sort order
	COrderSpec *pos = SPQOS_NEW(mp) COrderSpec(mp);
	pos->Append(SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_INT4_LT_OP),
				pcrsOutput->PcrAny(), COrderSpec::EntFirst);

	CDistributionSpec *pds = SPQOS_NEW(mp)
		CDistributionSpecSingleton(CDistributionSpecSingleton::EstMaster);

	CRewindabilitySpec *prs = SPQOS_NEW(mp) CRewindabilitySpec(
		CRewindabilitySpec::ErtNone, CRewindabilitySpec::EmhtNoMotion);

	CEnfdOrder *peo = SPQOS_NEW(mp) CEnfdOrder(pos, CEnfdOrder::EomSatisfy);

	// we require exact matching on distribution since final query results must be sent to master
	CEnfdDistribution *ped =
		SPQOS_NEW(mp) CEnfdDistribution(pds, CEnfdDistribution::EdmExact);

	CEnfdRewindability *per =
		SPQOS_NEW(mp) CEnfdRewindability(prs, CEnfdRewindability::ErmSatisfy);

	CCTEReq *pcter = COptCtxt::PoctxtFromTLS()->Pcteinfo()->PcterProducers(mp);

	CReqdPropPlan *prpp =
		SPQOS_NEW(mp) CReqdPropPlan(pcrsOutput, peo, ped, per, pcter);

	CMDNameArray *pdrspqmdname = SPQOS_NEW(mp) CMDNameArray(mp);
	const ULONG length = colref_array->Size();
	for (ULONG ul = 0; ul < length; ul++)
	{
		CColRef *colref = (*colref_array)[ul];
		CMDName *mdname = SPQOS_NEW(mp) CMDName(mp, colref->Name().Pstr());
		pdrspqmdname->Append(mdname);
	}

	return SPQOS_NEW(mp) CQueryContext(mp, pexpr, prpp, colref_array,
									  pdrspqmdname, true /*fDeriveStats*/);
}

//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PexprScalarNestedPreds
//
//	@doc:
//		Helper for generating a nested AND/OR/NOT tree
//
//---------------------------------------------------------------------------
CExpression *
CTestUtils::PexprScalarNestedPreds(CMemoryPool *mp, CExpression *pexpr,
								   CScalarBoolOp::EBoolOperator eboolop)
{
	SPQOS_ASSERT(NULL != mp);
	SPQOS_ASSERT(NULL != pexpr);
	SPQOS_ASSERT(CScalarBoolOp::EboolopAnd == eboolop ||
				CScalarBoolOp::EboolopOr == eboolop ||
				CScalarBoolOp::EboolopNot == eboolop);

	CColRefSet *pcrs = pexpr->DeriveOutputColumns();
	CColRef *pcrLeft = pcrs->PcrAny();
	CExpression *pexprConstActual = CUtils::PexprScalarConstInt4(mp, 3 /*val*/);

	CExpression *pexprPredActual =
		CUtils::PexprScalarEqCmp(mp, pcrLeft, pexprConstActual);
	CExpression *pexprPredExpected = NULL;

	if (CScalarBoolOp::EboolopNot != eboolop)
	{
		CExpression *pexprConstExpected =
			CUtils::PexprScalarConstInt4(mp, 5 /*val*/);
		pexprPredExpected =
			CUtils::PexprScalarEqCmp(mp, pcrLeft, pexprConstExpected);
	}

	CExpressionArray *pdrspqexprActual = SPQOS_NEW(mp) CExpressionArray(mp);
	pdrspqexprActual->Append(pexprPredActual);

	if (CScalarBoolOp::EboolopNot != eboolop)
	{
		pdrspqexprActual->Append(pexprPredExpected);
	}

	CExpression *pexprBoolOp =
		CUtils::PexprScalarBoolOp(mp, eboolop, pdrspqexprActual);
	CExpressionArray *pdrspqexprExpected = SPQOS_NEW(mp) CExpressionArray(mp);
	pdrspqexprExpected->Append(pexprBoolOp);

	if (CScalarBoolOp::EboolopNot != eboolop)
	{
		pexprPredExpected->AddRef();
		pdrspqexprExpected->Append(pexprPredExpected);
	}

	return CUtils::PexprScalarBoolOp(mp, eboolop, pdrspqexprExpected);
}

//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PexprFindFirstExpressionWithOpId
//
//	@doc:
//		DFS of expression tree to find and return a pointer to the expression
//		containing the given operator type. NULL if not found
//
//---------------------------------------------------------------------------
CExpression *
CTestUtils::PexprFindFirstExpressionWithOpId(CExpression *pexpr,
											 COperator::EOperatorId op_id)
{
	SPQOS_ASSERT(NULL != pexpr);
	if (op_id == pexpr->Pop()->Eopid())
	{
		return pexpr;
	}

	ULONG arity = pexpr->Arity();
	for (ULONG ul = 0; ul < arity; ul++)
	{
		CExpression *pexprFound =
			PexprFindFirstExpressionWithOpId((*pexpr)[ul], op_id);
		if (NULL != pexprFound)
		{
			return pexprFound;
		}
	}

	return NULL;
}

//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::EqualityPredicate
//
//	@doc:
//		Generate equality predicate given column sets
//
//---------------------------------------------------------------------------
void
CTestUtils::EqualityPredicate(CMemoryPool *mp, CColRefSet *pcrsLeft,
							  CColRefSet *pcrsRight,
							  CExpressionArray *pdrspqexpr)
{
	CColRef *pcrLeft = pcrsLeft->PcrAny();
	CColRef *pcrRight = pcrsRight->PcrAny();

	// generate correlated predicate
	CExpression *pexprCorrelation =
		CUtils::PexprScalarEqCmp(mp, pcrLeft, pcrRight);

	pdrspqexpr->Append(pexprCorrelation);
}


//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PpointInt2
//
//	@doc:
//		Create an INT2 point
//
//---------------------------------------------------------------------------
CPoint *
CTestUtils::PpointInt2(CMemoryPool *mp, INT i)
{
	CDatumInt2SPQDB *datum =
		SPQOS_NEW(mp) CDatumInt2SPQDB(m_sysidDefault, (SINT) i);
	CPoint *point = SPQOS_NEW(mp) CPoint(datum);
	return point;
}


//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PpointInt4
//
//	@doc:
//		Create an INT4 point
//
//---------------------------------------------------------------------------
CPoint *
CTestUtils::PpointInt4(CMemoryPool *mp, INT i)
{
	CDatumInt4SPQDB *datum = SPQOS_NEW(mp) CDatumInt4SPQDB(m_sysidDefault, i);
	CPoint *point = SPQOS_NEW(mp) CPoint(datum);
	return point;
}


//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PpointInt4NullVal
//
//	@doc:
//		Create an INT4 point with null value
//
//---------------------------------------------------------------------------
CPoint *
CTestUtils::PpointInt4NullVal(CMemoryPool *mp)
{
	CDatumInt4SPQDB *datum =
		SPQOS_NEW(mp) CDatumInt4SPQDB(m_sysidDefault, 0, true /* is_null */);
	CPoint *point = SPQOS_NEW(mp) CPoint(datum);
	return point;
}


//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PpointInt8
//
//	@doc:
//		Create an INT8 point
//
//---------------------------------------------------------------------------
CPoint *
CTestUtils::PpointInt8(CMemoryPool *mp, INT i)
{
	CDatumInt8SPQDB *datum =
		SPQOS_NEW(mp) CDatumInt8SPQDB(m_sysidDefault, (LINT) i);
	CPoint *point = SPQOS_NEW(mp) CPoint(datum);
	return point;
}

//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PpointBool
//
//	@doc:
//		Create a point
//
//---------------------------------------------------------------------------
CPoint *
CTestUtils::PpointBool(CMemoryPool *mp, BOOL value)
{
	CDatumBoolSPQDB *datum = SPQOS_NEW(mp) CDatumBoolSPQDB(m_sysidDefault, value);
	CPoint *point = SPQOS_NEW(mp) CPoint(datum);
	return point;
}


//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PexprReadQuery
//
//	@doc:
//		Return query logical expression from minidump
//
//---------------------------------------------------------------------------
CExpression *
CTestUtils::PexprReadQuery(CMemoryPool *mp, const CHAR *szQueryFileName)
{
	CHAR *szQueryDXL = CDXLUtils::Read(mp, szQueryFileName);

	// parse the DXL query tree from the given DXL document
	CQueryToDXLResult *ptroutput =
		CDXLUtils::ParseQueryToQueryDXLTree(mp, szQueryDXL, NULL);

	// get md accessor
	CMDAccessor *md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();
	SPQOS_ASSERT(NULL != md_accessor);

	// translate DXL tree into CExpression
	CTranslatorDXLToExpr trdxl2expr(mp, md_accessor);
	CExpression *pexprQuery = trdxl2expr.PexprTranslateQuery(
		ptroutput->CreateDXLNode(), ptroutput->GetOutputColumnsDXLArray(),
		ptroutput->GetCTEProducerDXLArray());

	SPQOS_DELETE(ptroutput);
	SPQOS_DELETE_ARRAY(szQueryDXL);

	return pexprQuery;
}


//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::EresTranslate
//
//	@doc:
//		Rehydrate a query from the given file, optimize it, translate back to
//		DXL and compare the resulting plan to the given DXL document
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CTestUtils::EresTranslate(CMemoryPool *mp, const CHAR *szQueryFileName,
						  const CHAR *szPlanFileName, BOOL fIgnoreMismatch)
{
	// debug print
	CWStringDynamic str(mp);
	COstreamString oss(&str);

	SPQOS_TRACE(str.GetBuffer());

	// read the dxl document
	CHAR *szQueryDXL = CDXLUtils::Read(mp, szQueryFileName);

	// parse the DXL query tree from the given DXL document
	CQueryToDXLResult *ptroutput =
		CDXLUtils::ParseQueryToQueryDXLTree(mp, szQueryDXL, NULL);

	// get md accessor
	CMDAccessor *md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();
	SPQOS_ASSERT(NULL != md_accessor);

	// translate DXL tree into CExpression
	CTranslatorDXLToExpr ptrdxl2expr(mp, md_accessor);
	CExpression *pexprQuery = ptrdxl2expr.PexprTranslateQuery(
		ptroutput->CreateDXLNode(), ptroutput->GetOutputColumnsDXLArray(),
		ptroutput->GetCTEProducerDXLArray());

	CQueryContext *pqc = CQueryContext::PqcGenerate(
		mp, pexprQuery, ptrdxl2expr.PdrspqulOutputColRefs(),
		ptrdxl2expr.Pdrspqmdname(), true /*fDeriveStats*/
	);

#ifdef SPQOS_DEBUG
	pqc->OsPrint(oss);
#endif	//SPQOS_DEBUG

	spqopt::CEngine eng(mp);
	eng.Init(pqc, NULL /*search_stage_array*/);

#ifdef SPQOS_DEBUG
	eng.RecursiveOptimize();
#else
	eng.Optimize();
#endif	//SPQOS_DEBUG

	spqopt::CExpression *pexprPlan = eng.PexprExtractPlan();
	SPQOS_ASSERT(NULL != pexprPlan);

	(void) pexprPlan->PrppCompute(mp, pqc->Prpp());
	pexprPlan->OsPrint(oss);

	// translate plan back to DXL
	CTranslatorExprToDXL ptrexpr2dxl(mp, md_accessor, PdrspqiSegments(mp));
	CDXLNode *pdxlnPlan = ptrexpr2dxl.PdxlnTranslate(pexprPlan, pqc->PdrgPcr(),
													 pqc->Pdrspqmdname());
	SPQOS_ASSERT(NULL != pdxlnPlan);

	COptimizerConfig *optimizer_config =
		COptCtxt::PoctxtFromTLS()->GetOptimizerConfig();

	CWStringDynamic strTranslatedPlan(mp);
	COstreamString osTranslatedPlan(&strTranslatedPlan);

	CDXLUtils::SerializePlan(
		mp, osTranslatedPlan, pdxlnPlan,
		optimizer_config->GetEnumeratorCfg()->GetPlanId(),
		optimizer_config->GetEnumeratorCfg()->GetPlanSpaceSize(),
		true /*serialize_header_footer*/, true /*indentation*/);

	SPQOS_TRACE(str.GetBuffer());
	SPQOS_RESULT eres = SPQOS_OK;
	if (NULL != szPlanFileName)
	{
		// parse the DXL plan tree from the given DXL file
		CHAR *szExpectedPlan = CDXLUtils::Read(mp, szPlanFileName);

		CWStringDynamic strExpectedPlan(mp);
		strExpectedPlan.AppendFormat(SPQOS_WSZ_LIT("%s"), szExpectedPlan);

		eres = EresCompare(oss, &strTranslatedPlan, &strExpectedPlan,
						   fIgnoreMismatch);
		SPQOS_DELETE_ARRAY(szExpectedPlan);
	}

	// cleanup
	pexprQuery->Release();
	pexprPlan->Release();
	pdxlnPlan->Release();
	SPQOS_DELETE_ARRAY(szQueryDXL);
	SPQOS_DELETE(ptroutput);
	SPQOS_DELETE(pqc);
	return eres;
}

//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::EresCompare
//
//	@doc:
//		Compare expected and actual output
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CTestUtils::EresCompare(IOstream &os, CWStringDynamic *pstrActual,
						CWStringDynamic *pstrExpected, BOOL fIgnoreMismatch)
{
	if (!pstrExpected->Equals(pstrActual))
	{
		os << "Output does not match expected DXL document" << std::endl;
		os << "Actual: " << std::endl;
		os << pstrActual->GetBuffer() << std::endl;

		os << "Expected: " << std::endl;
		os << pstrExpected->GetBuffer() << std::endl;

		if (fIgnoreMismatch)
		{
			return SPQOS_OK;
		}
		return SPQOS_FAILED;
	}
	else
	{
		os << "Output matches expected DXL document" << std::endl;
		return SPQOS_OK;
	}
}

CHAR *
CTestUtils::ExtractFilenameFromPath(CHAR *file_path)
{
	CHAR *filename = NULL;
	CHAR *token = strtok(file_path, "/");

	while (token != NULL)
	{
		filename = token;
		token = strtok(NULL, "/");
	}

	return filename;
}

void
CTestUtils::CreateExpectedAndActualFile(CMemoryPool *mp, const CHAR *file_name,
										CWStringDynamic *strExpected,
										CWStringDynamic *strActual)
{
	CHAR *filepath = SPQOS_NEW_ARRAY(mp, CHAR, strlen(file_name));
	clib::Strncpy(filepath, file_name, strlen(file_name));
	filepath[strlen(file_name) - 1] = '\0';
	CWStringDynamic expected_file(mp);
	CWStringDynamic actual_file(mp);
	CHAR *basename = ExtractFilenameFromPath(filepath);

	expected_file.AppendFormat(SPQOS_WSZ_LIT("../server/%s_expected"), basename);
	actual_file.AppendFormat(SPQOS_WSZ_LIT("../server/%s_actual"), basename);

	CAutoP<std::wofstream> wos_expected;
	CAutoP<COstreamBasic> os_expected;

	CAutoP<std::wofstream> wos_actual;
	CAutoP<COstreamBasic> os_actual;

	wos_expected = SPQOS_NEW(mp)
		std::wofstream(CUtils::CreateMultiByteCharStringFromWCString(
			m_mp, const_cast<WCHAR *>(expected_file.GetBuffer())));
	os_expected = SPQOS_NEW(mp) COstreamBasic(wos_expected.Value());

	wos_actual = SPQOS_NEW(mp)
		std::wofstream(CUtils::CreateMultiByteCharStringFromWCString(
			m_mp, const_cast<WCHAR *>(actual_file.GetBuffer())));
	os_actual = SPQOS_NEW(mp) COstreamBasic(wos_actual.Value());

	*os_expected << strExpected->GetBuffer() << std::endl;
	*os_actual << strActual->GetBuffer() << std::endl;

	SPQOS_DELETE_ARRAY(filepath);
}


//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::FPlanMatch
//
//	@doc:
//		Match given two plans using string comparison
//
//---------------------------------------------------------------------------
BOOL
CTestUtils::FPlanMatch(CMemoryPool *mp, IOstream &os,
					   const CDXLNode *pdxlnActual, ULLONG ullPlanIdActual,
					   ULLONG ullPlanSpaceSizeActual,
					   const CDXLNode *pdxlnExpected, ULLONG ullPlanIdExpected,
					   ULLONG ullPlanSpaceSizeExpected, const CHAR *file_name)
{
	if (NULL == pdxlnActual && NULL == pdxlnExpected)
	{
		CAutoTrace at(mp);
		at.Os() << "Both plans are NULL." << std::endl;

		return true;
	}

	if (NULL != pdxlnActual && NULL == pdxlnExpected)
	{
		CAutoTrace at(mp);
		at.Os() << "Plan comparison *** FAILED ***" << std::endl;
		at.Os() << "Expected plan is NULL. Actual: " << std::endl;
		CDXLUtils::SerializePlan(
			mp, at.Os(), pdxlnActual, ullPlanIdActual, ullPlanSpaceSizeActual,
			false /*serialize_document_header_footer*/, true /*indentation*/);
		at.Os() << std::endl;

		return false;
	}

	if (NULL == pdxlnActual && NULL != pdxlnExpected)
	{
		CAutoTrace at(mp);
		at.Os() << "Plan comparison *** FAILED ***" << std::endl;
		at.Os() << "Actual plan is NULL. Expected: " << std::endl;
		CDXLUtils::SerializePlan(mp, at.Os(), pdxlnExpected, ullPlanIdExpected,
								 ullPlanSpaceSizeExpected,
								 false /*serialize_document_header_footer*/,
								 true /*indentation*/);
		at.Os() << std::endl;

		return false;
	}

	SPQOS_ASSERT(NULL != pdxlnActual);
	SPQOS_ASSERT(NULL != pdxlnExpected);

	// plan id's and space sizes are already compared before this point,
	// overwrite PlanId's and space sizes with zeros to pass string comparison on plan body
	CWStringDynamic strActual(mp);
	COstreamString osActual(&strActual);
	CDXLUtils::SerializePlan(mp, osActual, pdxlnActual, 0 /*ullPlanIdActual*/,
							 0 /*ullPlanSpaceSizeActual*/,
							 false /*serialize_document_header_footer*/,
							 true /*indentation*/);
	SPQOS_CHECK_ABORT;

	CWStringDynamic strExpected(mp);
	COstreamString osExpected(&strExpected);
	CDXLUtils::SerializePlan(
		mp, osExpected, pdxlnExpected, 0 /*ullPlanIdExpected*/,
		0 /*ullPlanSpaceSizeExpected*/,
		false /*serialize_document_header_footer*/, true /*indentation*/);
	SPQOS_CHECK_ABORT;

	BOOL result = strActual.Equals(&strExpected);

	if (!result)
	{
		// serialize plans again to restore id's and space size before printing error message
		CWStringDynamic strActual(mp);
		COstreamString osActual(&strActual);
		CDXLUtils::SerializePlan(
			mp, osActual, pdxlnActual, ullPlanIdActual, ullPlanSpaceSizeActual,
			false /*serialize_document_header_footer*/, true /*indentation*/);
		SPQOS_CHECK_ABORT;

		CWStringDynamic strExpected(mp);
		COstreamString osExpected(&strExpected);
		CDXLUtils::SerializePlan(mp, osExpected, pdxlnExpected,
								 ullPlanIdExpected, ullPlanSpaceSizeExpected,
								 false /*serialize_document_header_footer*/,
								 true /*indentation*/);
		SPQOS_CHECK_ABORT;

		{
			CAutoTrace at(mp);

			at.Os() << "Plan comparison *** FAILED ***" << std::endl;
			at.Os() << "Actual: " << std::endl;
			at.Os() << strActual.GetBuffer() << std::endl;
		}

		os << "Expected: " << std::endl;
		os << strExpected.GetBuffer() << std::endl;

		CreateExpectedAndActualFile(mp, file_name, &strExpected, &strActual);
	}

	SPQOS_CHECK_ABORT;
	return result;
}

//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::FPlanCompare
//
//	@doc:
//		Compare two plans based on their string representation
//
//---------------------------------------------------------------------------
BOOL
CTestUtils::FPlanCompare(CMemoryPool *mp, IOstream &os,
						 const CDXLNode *pdxlnActual, ULLONG ullPlanIdActual,
						 ULLONG ullPlanSpaceSizeActual,
						 const CDXLNode *pdxlnExpected,
						 ULLONG ullPlanIdExpected,
						 ULLONG ullPlanSpaceSizeExpected, BOOL fMatchPlans,
						 INT iCmpSpaceSize, const CHAR *file_name)
{
	BOOL fPlanSpaceUnchanged = true;

	if (!fMatchPlans)
	{
		return true;
	}

	CAutoTrace at(mp);

	if (ullPlanIdActual != ullPlanIdExpected)
	{
		at.Os() << "Plan Id mismatch." << std::endl
				<< "\tActual Id: " << ullPlanIdActual << std::endl
				<< "\tExpected Id: " << ullPlanIdExpected << std::endl;

		return false;
	}

	// check plan space size required comparison
	if ((0 == iCmpSpaceSize &&
		 ullPlanSpaceSizeActual !=
			 ullPlanSpaceSizeExpected) ||  // required comparison is equality
		(-1 == iCmpSpaceSize &&
		 ullPlanSpaceSizeActual >
			 ullPlanSpaceSizeExpected) ||  // required comparison is (Actual <= Expected)
		(1 == iCmpSpaceSize &&
		 ullPlanSpaceSizeActual <
			 ullPlanSpaceSizeExpected)	// required comparison is (Actual >= Expected)
	)
	{
		at.Os() << "Plan space size comparison *** FAILED ***" << std::endl
				<< "Required comparison: " << iCmpSpaceSize << std::endl
				<< "\tActual size: " << ullPlanSpaceSizeActual << std::endl
				<< "\tExpected size: " << ullPlanSpaceSizeExpected << std::endl;

		fPlanSpaceUnchanged = false;
	}

	// perform deep matching on plan bodies
	return FPlanMatch(mp, os, pdxlnActual, ullPlanIdActual,
					  ullPlanSpaceSizeActual, pdxlnExpected, ullPlanIdExpected,
					  ullPlanSpaceSizeExpected, file_name) &&
		   fPlanSpaceUnchanged;
}

//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PdrspqiSegments
//
//	@doc:
//		Helper function for creating an array of segment ids for the target system
//
//---------------------------------------------------------------------------
IntPtrArray *
CTestUtils::PdrspqiSegments(CMemoryPool *mp)
{
	IntPtrArray *pdrspqiSegments = SPQOS_NEW(mp) IntPtrArray(mp);
	const ULONG ulSegments = SPQOPT_SEGMENT_COUNT;
	SPQOS_ASSERT(0 < ulSegments);

	for (ULONG ul = 0; ul < ulSegments; ul++)
	{
		pdrspqiSegments->Append(SPQOS_NEW(mp) INT(ul));
	}
	return pdrspqiSegments;
}

#ifdef SPQOS_DEBUG
//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::FFaultSimulation
//
//	@doc:
//		Check if we are in fault simulation mode
//
//---------------------------------------------------------------------------
BOOL
CTestUtils::FFaultSimulation()
{
	return SPQOS_FTRACE(EtraceSimulateAbort) ||
		   SPQOS_FTRACE(EtraceSimulateIOError) || SPQOS_FTRACE(EtraceSimulateOOM);
}
#endif	// SPQOS_DEBUG


//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::SzMinidumpFileName
//
//	@doc:
//		Generate minidump file name from passed file name
//
//---------------------------------------------------------------------------
CHAR *
CTestUtils::SzMinidumpFileName(CMemoryPool *mp, const CHAR *file_name)
{
	SPQOS_ASSERT(NULL != file_name);

	if (!SPQOS_FTRACE(EopttraceEnableSpacePruning))
	{
		return const_cast<CHAR *>(file_name);
	}

	CWStringDynamic *pstrMinidumpFileName = SPQOS_NEW(mp) CWStringDynamic(mp);
	COstreamString oss(pstrMinidumpFileName);
	oss << file_name << "-space-pruned";

	// convert wide char to regular char
	const WCHAR *wsz = pstrMinidumpFileName->GetBuffer();
	const ULONG ulInputLength = SPQOS_WSZ_LENGTH(wsz);
	const ULONG ulWCHARSize = SPQOS_SIZEOF(WCHAR);
	const ULONG ulMaxLength = (ulInputLength + 1) * ulWCHARSize;
	CHAR *sz = SPQOS_NEW_ARRAY(mp, CHAR, ulMaxLength);
	spqos::clib::Wcstombs(sz, const_cast<WCHAR *>(wsz), ulMaxLength);
	sz[ulMaxLength - 1] = '\0';

	SPQOS_DELETE(pstrMinidumpFileName);

	return sz;
}


//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::EresRunMinidump
//
//	@doc:
//		Run one minidump-based test using passed MD Accessor
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CTestUtils::EresRunMinidump(CMemoryPool *mp, CMDAccessor *md_accessor,
							const CHAR *file_name, ULONG *pulTestCounter,
							ULONG ulSessionId, ULONG ulCmdId, BOOL fMatchPlans,
							INT iCmpSpaceSize, IConstExprEvaluator *pceeval)
{
	SPQOS_ASSERT(NULL != md_accessor);

	SPQOS_RESULT eres = SPQOS_OK;

	{
		CAutoTrace at(mp);
		at.Os() << "executing " << file_name;
	}

	// load dump file
	CDXLMinidump *pdxlmd = CMinidumperUtils::PdxlmdLoad(mp, file_name);
	SPQOS_CHECK_ABORT;

	COptimizerConfig *optimizer_config = pdxlmd->GetOptimizerConfig();

	if (NULL == optimizer_config)
	{
		optimizer_config = COptimizerConfig::PoconfDefault(mp);
	}
	else
	{
		optimizer_config->AddRef();
	}

	ULONG ulSegments = UlSegments(optimizer_config);

	// allow sampler to throw invalid plan exception
	optimizer_config->GetEnumeratorCfg()->SetSampleValidPlans(
		false /*fSampleValidPlans*/);

	CDXLNode *pdxlnPlan = NULL;

	CHAR *szMinidumpFileName = SzMinidumpFileName(mp, file_name);

	pdxlnPlan = CMinidumperUtils::PdxlnExecuteMinidump(
		mp, md_accessor, pdxlmd, szMinidumpFileName, ulSegments, ulSessionId,
		ulCmdId, optimizer_config, pceeval);

	if (szMinidumpFileName != file_name)
	{
		// a new name was generated
		SPQOS_DELETE_ARRAY(szMinidumpFileName);
	}

	SPQOS_CHECK_ABORT;


	{
		CAutoTrace at(mp);
		if (!CTestUtils::FPlanCompare(
				mp, at.Os(), pdxlnPlan,
				optimizer_config->GetEnumeratorCfg()->GetPlanId(),
				optimizer_config->GetEnumeratorCfg()->GetPlanSpaceSize(),
				pdxlmd->PdxlnPlan(), pdxlmd->GetPlanId(),
				pdxlmd->GetPlanSpaceSize(), fMatchPlans, iCmpSpaceSize,
				file_name))
		{
			eres = SPQOS_FAILED;
		}
	}

	SPQOS_CHECK_ABORT;

	{
		CAutoTrace at(mp);
		at.Os() << std::endl;
	}

	// cleanup
	SPQOS_DELETE(pdxlmd);
	pdxlnPlan->Release();
	optimizer_config->Release();

	(*pulTestCounter)++;
	return eres;
}


//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::EresRunMinidumps
//
//	@doc:
//		Run minidump-based tests
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CTestUtils::EresRunMinidumps(CMemoryPool *,	 // pmpInput,
							 const CHAR *rgszFileNames[], ULONG ulTests,
							 ULONG *pulTestCounter, ULONG ulSessionId,
							 ULONG ulCmdId, BOOL fMatchPlans,
							 BOOL fTestSpacePruning,
							 const CHAR *,	// szMDFilePath,
							 IConstExprEvaluator *pceeval)
{
	SPQOS_RESULT eres = SPQOS_OK;
	BOOL fSuccess = true;

	for (ULONG ul = *pulTestCounter; ul < ulTests; ul++)
	{
		// each test uses a new memory pool to keep total memory consumption low
		CAutoMemoryPool amp;
		CMemoryPool *mp = amp.Pmp();

		if (fMatchPlans)
		{
			{
				CAutoTrace at(mp);
				at.Os() << "IsRunning test with EXHAUSTIVE SEARCH:";
			}

			eres = EresRunMinidumpsUsingOneMDFile(
				mp, rgszFileNames[ul], &rgszFileNames[ul], pulTestCounter,
				ulSessionId, ulCmdId, fMatchPlans,
				0,	// iCmpSpaceSize
				pceeval);

			if (SPQOS_FAILED == eres)
			{
				fSuccess = false;
			}
		}

		if (SPQOS_OK == eres && fTestSpacePruning)
		{
			{
				CAutoTrace at(mp);
				at.Os() << "IsRunning test with BRANCH-AND-BOUND SEARCH:";
			}

			// enable space pruning
			CAutoTraceFlag atf(EopttraceEnableSpacePruning, true /*value*/);

			eres = EresRunMinidumpsUsingOneMDFile(
				mp, rgszFileNames[ul], &rgszFileNames[ul], pulTestCounter,
				ulSessionId, ulCmdId, fMatchPlans,
				-1,	 // iCmpSpaceSize
				pceeval);

			if (SPQOS_FAILED == eres)
			{
				fSuccess = false;
			}
		}
	}

	*pulTestCounter = 0;

	// return SPQOS_OK if all the minidump tests passed.
	// the minidump test runner, EresRunMinidump(), only returns
	// SPQOS_FAILED in case of a failure, hence remaining error codes need
	// not be handled here
	return fSuccess ? SPQOS_OK : SPQOS_FAILED;
}


//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::EresRunMinidumpsUsingOneMDFile
//
//	@doc:
//		Run all minidumps based on one metadata file
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CTestUtils::EresRunMinidumpsUsingOneMDFile(
	CMemoryPool *mp, const CHAR *szMDFilePath, const CHAR *rgszFileNames[],
	ULONG *pulTestCounter, ULONG ulSessionId, ULONG ulCmdId, BOOL fMatchPlans,
	INT iCmpSpaceSize, IConstExprEvaluator *pceeval)
{
	SPQOS_ASSERT(NULL != rgszFileNames);
	SPQOS_ASSERT(NULL != szMDFilePath);

	// reset metadata cache
	CMDCache::Reset();

	// load metadata file
	CDXLMinidump *pdxlmd = CMinidumperUtils::PdxlmdLoad(mp, szMDFilePath);
	SPQOS_CHECK_ABORT;

	// set up MD providers
	CMDProviderMemory *pmdp = SPQOS_NEW(mp) CMDProviderMemory(mp, szMDFilePath);
	SPQOS_CHECK_ABORT;

	const CSystemIdArray *pdrspqsysid = pdxlmd->GetSysidPtrArray();
	CMDProviderArray *pdrspqmdp = SPQOS_NEW(mp) CMDProviderArray(mp);
	pdrspqmdp->Append(pmdp);

	for (ULONG ul = 1; ul < pdrspqsysid->Size(); ul++)
	{
		pmdp->AddRef();
		pdrspqmdp->Append(pmdp);
	}

	SPQOS_RESULT eres = SPQOS_OK;

	{  // scope for MD accessor
		CMDAccessor mda(mp, CMDCache::Pcache(), pdrspqsysid, pdrspqmdp);

		eres = EresRunMinidump(mp, &mda, rgszFileNames[0], pulTestCounter,
							   ulSessionId, ulCmdId, fMatchPlans, iCmpSpaceSize,
							   pceeval);

		*pulTestCounter = 0;
	}

	// cleanup
	pdrspqmdp->Release();
	SPQOS_DELETE(pdxlmd);
	SPQOS_CHECK_ABORT;

	return eres;
}


//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::EresSamplePlans
//
//	@doc:
//		Test plan sampling
//		to extract attribute 'X' value from xml file:
//		xpath distr.xml //dxl:Value/@X | grep 'X=' | sed 's/\"//g' | sed 's/X=//g' | tr ' ' '\n'
//
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CTestUtils::EresSamplePlans(const CHAR *rgszFileNames[], ULONG ulTests,
							ULONG *pulTestCounter, ULONG ulSessionId,
							ULONG ulCmdId)
{
	SPQOS_RESULT eres = SPQOS_OK;

	for (ULONG ul = *pulTestCounter; ul < ulTests; ul++)
	{
		// each test uses a new memory pool to keep total memory consumption low
		CAutoMemoryPool amp;
		CMemoryPool *mp = amp.Pmp();

		// reset metadata cache
		CMDCache::Reset();

		CAutoTraceFlag atf1(EopttraceEnumeratePlans, true);
		CAutoTraceFlag atf2(EopttraceSamplePlans, true);

		// load dump file
		CDXLMinidump *pdxlmd =
			CMinidumperUtils::PdxlmdLoad(mp, rgszFileNames[ul]);
		SPQOS_CHECK_ABORT;

		// set up MD providers
		CMDProviderMemory *pmdp =
			SPQOS_NEW(mp) CMDProviderMemory(mp, rgszFileNames[ul]);
		SPQOS_CHECK_ABORT;

		const CSystemIdArray *pdrspqsysid = pdxlmd->GetSysidPtrArray();
		CMDProviderArray *pdrspqmdp = SPQOS_NEW(mp) CMDProviderArray(mp);
		pdrspqmdp->Append(pmdp);

		for (ULONG ulSys = 1; ulSys < pdrspqsysid->Size(); ulSys++)
		{
			pmdp->AddRef();
			pdrspqmdp->Append(pmdp);
		}

		COptimizerConfig *optimizer_config = pdxlmd->GetOptimizerConfig();

		if (NULL == optimizer_config)
		{
			optimizer_config = SPQOS_NEW(mp) COptimizerConfig(
				SPQOS_NEW(mp)
					CEnumeratorConfig(mp, 0 /*plan_id*/, 1000 /*ullSamples*/),
				CStatisticsConfig::PstatsconfDefault(mp),
				CCTEConfig::PcteconfDefault(mp), ICostModel::PcmDefault(mp),
				CHint::PhintDefault(mp), CWindowOids::GetWindowOids(mp));
		}
		else
		{
			optimizer_config->AddRef();
		}

		ULONG ulSegments = UlSegments(optimizer_config);

		// allow sampler to throw invalid plan exception
		optimizer_config->GetEnumeratorCfg()->SetSampleValidPlans(
			false /*fSampleValidPlans*/);

		{
			// scope for MD accessor
			CMDAccessor mda(mp, CMDCache::Pcache(), pdrspqsysid, pdrspqmdp);

			CDXLNode *pdxlnPlan = CMinidumperUtils::PdxlnExecuteMinidump(
				mp, &mda, pdxlmd, rgszFileNames[ul], ulSegments, ulSessionId,
				ulCmdId, optimizer_config,
				NULL  // pceeval
			);

			SPQOS_CHECK_ABORT;

			{
				CAutoTrace at(mp);

				at.Os()
					<< "Generated "
					<< optimizer_config->GetEnumeratorCfg()->UlCreatedSamples()
					<< " samples ... " << std::endl;

				// print ids of sampled plans
				CWStringDynamic *str = CDXLUtils::SerializeSamplePlans(
					mp, optimizer_config->GetEnumeratorCfg(), true /*fIdent*/);
				at.Os() << str->GetBuffer();
				SPQOS_DELETE(str);

				// print fitted cost distribution
				at.Os() << "Cost Distribution: " << std::endl;
				const ULONG size =
					optimizer_config->GetEnumeratorCfg()->UlCostDistrSize();
				for (ULONG ul = 0; ul < size; ul++)
				{
					at.Os()
						<< optimizer_config->GetEnumeratorCfg()->DCostDistrX(ul)
						<< "\t"
						<< optimizer_config->GetEnumeratorCfg()->DCostDistrY(ul)
						<< std::endl;
				}

				// print serialized cost distribution
				str = CDXLUtils::SerializeCostDistr(
					mp, optimizer_config->GetEnumeratorCfg(), true /*fIdent*/);

				at.Os() << str->GetBuffer();
				SPQOS_DELETE(str);
			}

			// cleanup
			SPQOS_DELETE(pdxlmd);
			pdxlnPlan->Release();

		}  // end of MDAccessor scope


		// cleanup
		optimizer_config->Release();
		pdrspqmdp->Release();


		(*pulTestCounter)++;
	}

	*pulTestCounter = 0;
	return eres;
}


//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::EresCheckPlans
//
//	@doc:
//		Check all enumerated plans using given PlanChecker function
//
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CTestUtils::EresCheckPlans(const CHAR *rgszFileNames[], ULONG ulTests,
						   ULONG *pulTestCounter, ULONG ulSessionId,
						   ULONG ulCmdId, FnPlanChecker *pfpc)
{
	SPQOS_ASSERT(NULL != pfpc);

	SPQOS_RESULT eres = SPQOS_OK;

	for (ULONG ul = *pulTestCounter; ul < ulTests; ul++)
	{
		// each test uses a new memory pool to keep total memory consumption low
		CAutoMemoryPool amp;
		CMemoryPool *mp = amp.Pmp();

		// reset metadata cache
		CMDCache::Reset();

		CAutoTraceFlag atf1(EopttraceEnumeratePlans, true);

		// load dump file
		CDXLMinidump *pdxlmd =
			CMinidumperUtils::PdxlmdLoad(mp, rgszFileNames[ul]);
		SPQOS_CHECK_ABORT;

		// set up MD providers
		CMDProviderMemory *pmdp =
			SPQOS_NEW(mp) CMDProviderMemory(mp, rgszFileNames[ul]);
		SPQOS_CHECK_ABORT;

		const CSystemIdArray *pdrspqsysid = pdxlmd->GetSysidPtrArray();
		CMDProviderArray *pdrspqmdp = SPQOS_NEW(mp) CMDProviderArray(mp);
		pdrspqmdp->Append(pmdp);

		for (ULONG ulSys = 1; ulSys < pdrspqsysid->Size(); ulSys++)
		{
			pmdp->AddRef();
			pdrspqmdp->Append(pmdp);
		}

		COptimizerConfig *optimizer_config = pdxlmd->GetOptimizerConfig();

		if (NULL == optimizer_config)
		{
			optimizer_config = SPQOS_NEW(mp) COptimizerConfig(
				SPQOS_NEW(mp)
					CEnumeratorConfig(mp, 0 /*plan_id*/, 1000 /*ullSamples*/),
				CStatisticsConfig::PstatsconfDefault(mp),
				CCTEConfig::PcteconfDefault(mp), ICostModel::PcmDefault(mp),
				CHint::PhintDefault(mp), CWindowOids::GetWindowOids(mp));
		}
		else
		{
			optimizer_config->AddRef();
		}

		ULONG ulSegments = UlSegments(optimizer_config);

		// set plan checker
		optimizer_config->GetEnumeratorCfg()->SetPlanChecker(pfpc);

		// allow sampler to throw invalid plan exception
		optimizer_config->GetEnumeratorCfg()->SetSampleValidPlans(
			false /*fSampleValidPlans*/);

		{
			// scope for MD accessor
			CMDAccessor mda(mp, CMDCache::Pcache(), pdrspqsysid, pdrspqmdp);

			CDXLNode *pdxlnPlan = CMinidumperUtils::PdxlnExecuteMinidump(
				mp, &mda, pdxlmd, rgszFileNames[ul], ulSegments, ulSessionId,
				ulCmdId, optimizer_config,
				NULL  // pceeval
			);

			SPQOS_CHECK_ABORT;

			// cleanup
			SPQOS_DELETE(pdxlmd);
			pdxlnPlan->Release();

		}  // end of MDAcessor scope

		optimizer_config->Release();
		pdrspqmdp->Release();

		(*pulTestCounter)++;
	}

	*pulTestCounter = 0;
	return eres;
}


//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::UlSegments
//
//	@doc:
//		Return the number of segments, default return SPQOPT_TEST_SEGMENTS
//
//---------------------------------------------------------------------------
ULONG
CTestUtils::UlSegments(COptimizerConfig *optimizer_config)
{
	SPQOS_ASSERT(NULL != optimizer_config);
	ULONG ulSegments = SPQOPT_TEST_SEGMENTS;
	if (NULL != optimizer_config->GetCostModel())
	{
		ULONG ulSegs = optimizer_config->GetCostModel()->UlHosts();
		if (ulSegments < ulSegs)
		{
			ulSegments = ulSegs;
		}
	}

	return ulSegments;
}


//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::EresCheckOptimizedPlan
//
//	@doc:
//		Check the optimized plan using given DXLPlanChecker function. Does
//		not take ownership of the given pdrspqcp. The cost model configured
//		in the minidumps must be the calibrated one.
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CTestUtils::EresCheckOptimizedPlan(const CHAR *rgszFileNames[], ULONG ulTests,
								   ULONG *pulTestCounter, ULONG ulSessionId,
								   ULONG ulCmdId, FnDXLPlanChecker *pfdpc,
								   ICostModelParamsArray *pdrspqcp)
{
	SPQOS_ASSERT(NULL != pfdpc);

	SPQOS_RESULT eres = SPQOS_OK;

	for (ULONG ul = *pulTestCounter; ul < ulTests; ul++)
	{
		// each test uses a new memory pool to keep total memory consumption low
		CAutoMemoryPool amp;
		CMemoryPool *mp = amp.Pmp();

		// reset metadata cache
		CMDCache::Reset();

		CAutoTraceFlag atf1(EopttraceEnableSpacePruning, true /*value*/);

		// load dump file
		CDXLMinidump *pdxlmd =
			CMinidumperUtils::PdxlmdLoad(mp, rgszFileNames[ul]);
		SPQOS_CHECK_ABORT;

		// set up MD providers
		CMDProviderMemory *pmdp =
			SPQOS_NEW(mp) CMDProviderMemory(mp, rgszFileNames[ul]);
		SPQOS_CHECK_ABORT;

		const CSystemIdArray *pdrspqsysid = pdxlmd->GetSysidPtrArray();
		CMDProviderArray *pdrspqmdp = SPQOS_NEW(mp) CMDProviderArray(mp);
		pdrspqmdp->Append(pmdp);

		for (ULONG ulSys = 1; ulSys < pdrspqsysid->Size(); ulSys++)
		{
			pmdp->AddRef();
			pdrspqmdp->Append(pmdp);
		}

		COptimizerConfig *optimizer_config = pdxlmd->GetOptimizerConfig();
		SPQOS_ASSERT(NULL != optimizer_config);

		if (NULL != pdrspqcp)
		{
			optimizer_config->GetCostModel()->SetParams(pdrspqcp);
		}
		optimizer_config->AddRef();
		ULONG ulSegments = UlSegments(optimizer_config);

		// allow sampler to throw invalid plan exception
		optimizer_config->GetEnumeratorCfg()->SetSampleValidPlans(
			false /*fSampleValidPlans*/);

		{
			// scope for MD accessor
			CMDAccessor mda(mp, CMDCache::Pcache(), pdrspqsysid, pdrspqmdp);

			CDXLNode *pdxlnPlan = CMinidumperUtils::PdxlnExecuteMinidump(
				mp, &mda, pdxlmd, rgszFileNames[ul], ulSegments, ulSessionId,
				ulCmdId, optimizer_config,
				NULL  // pceeval
			);
			if (!pfdpc(pdxlnPlan))
			{
				eres = SPQOS_FAILED;
				{
					CAutoTrace at(mp);
					at.Os() << "Failed check for minidump " << rgszFileNames[ul]
							<< std::endl;
					CDXLUtils::SerializePlan(mp, at.Os(), pdxlnPlan,
											 0,		// plan_id
											 0,		// plan_space_size
											 true,	// serialize_header_footer
											 true	// indentation
					);
					at.Os() << std::endl;
				}
			}

			SPQOS_CHECK_ABORT;

			// cleanup
			SPQOS_DELETE(pdxlmd);
			pdxlnPlan->Release();

		}  // end of MDAcessor scope

		optimizer_config->Release();
		pdrspqmdp->Release();

		(*pulTestCounter)++;
	}

	*pulTestCounter = 0;
	return eres;
}


//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::CreateGenericDatum
//
//	@doc:
//		Create a datum with a given type, encoded value and int value.
//
//---------------------------------------------------------------------------
IDatum *
CTestUtils::CreateGenericDatum(CMemoryPool *mp, CMDAccessor *md_accessor,
							   IMDId *mdid_type,
							   CWStringDynamic *pstrEncodedValue, LINT value)
{
	SPQOS_ASSERT(NULL != md_accessor);

	SPQOS_ASSERT(!mdid_type->Equals(&CMDIdSPQDB::m_mdid_numeric));
	const IMDType *pmdtype = md_accessor->RetrieveType(mdid_type);
	ULONG ulbaSize = 0;
	BYTE *data =
		CDXLUtils::DecodeByteArrayFromString(mp, pstrEncodedValue, &ulbaSize);

	CDXLDatumGeneric *dxl_datum = NULL;
	if (CMDTypeGenericSPQDB::IsTimeRelatedTypeMappableToDouble(mdid_type))
	{
		dxl_datum = SPQOS_NEW(mp) CDXLDatumStatsDoubleMappable(
			mp, mdid_type, default_type_modifier, false /*is_const_null*/, data,
			ulbaSize, CDouble(value));
	}
	else if (pmdtype->IsTextRelated() ||
			 CMDTypeGenericSPQDB::IsTimeRelatedTypeMappableToLint(mdid_type))
	{
		dxl_datum = SPQOS_NEW(mp) CDXLDatumStatsLintMappable(
			mp, mdid_type, default_type_modifier, false /*is_const_null*/, data,
			ulbaSize, value);
	}
	else
	{
		dxl_datum = SPQOS_NEW(mp)
			CDXLDatumGeneric(mp, mdid_type, default_type_modifier,
							 false /*is_const_null*/, data, ulbaSize);
	}

	IDatum *datum = pmdtype->GetDatumForDXLDatum(mp, dxl_datum);
	dxl_datum->Release();

	return datum;
}

//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::CreateDoubleDatum
//
//	@doc:
//		Create a datum with a given type and double value
//
//---------------------------------------------------------------------------
IDatum *
CTestUtils::CreateDoubleDatum(CMemoryPool *mp, CMDAccessor *md_accessor,
							  IMDId *mdid_type, CDouble value)
{
	SPQOS_ASSERT(NULL != md_accessor);

	SPQOS_ASSERT(!mdid_type->Equals(&CMDIdSPQDB::m_mdid_numeric));
	const IMDType *pmdtype = md_accessor->RetrieveType(mdid_type);
	ULONG ulbaSize = 0;
	CWStringDynamic *pstrW =
		SPQOS_NEW(mp) CWStringDynamic(mp, SPQOS_WSZ_LIT("AAAABXc="));
	BYTE *data = CDXLUtils::DecodeByteArrayFromString(mp, pstrW, &ulbaSize);
	CDXLDatumGeneric *dxl_datum = NULL;

	dxl_datum = SPQOS_NEW(mp) CDXLDatumStatsDoubleMappable(
		mp, mdid_type, default_type_modifier, false /*is_const_null*/, data,
		ulbaSize, CDouble(value));


	IDatum *datum = pmdtype->GetDatumForDXLDatum(mp, dxl_datum);
	dxl_datum->Release();
	SPQOS_DELETE(pstrW);
	return datum;
}

//---------------------------------------------------------------------------
//      @function:
//              CConstraintTest::PciGenericInterval
//
//      @doc:
//              Create an interval for generic data types.
//              Does not take ownership of any argument.
//              Caller takes ownership of returned pointer.
//
//---------------------------------------------------------------------------
CConstraintInterval *
CTestUtils::PciGenericInterval(CMemoryPool *mp, CMDAccessor *md_accessor,
							   const CMDIdSPQDB &mdidType, CColRef *colref,
							   CWStringDynamic *pstrLower, LINT lLower,
							   CRange::ERangeInclusion eriLeft,
							   CWStringDynamic *pstrUpper, LINT lUpper,
							   CRange::ERangeInclusion eriRight)
{
	SPQOS_ASSERT(NULL != md_accessor);

	IDatum *pdatumLower = CTestUtils::CreateGenericDatum(
		mp, md_accessor, SPQOS_NEW(mp) CMDIdSPQDB(mdidType), pstrLower, lLower);
	IDatum *pdatumUpper = CTestUtils::CreateGenericDatum(
		mp, md_accessor, SPQOS_NEW(mp) CMDIdSPQDB(mdidType), pstrUpper, lUpper);

	CRangeArray *pdrspqrng = SPQOS_NEW(mp) CRangeArray(mp);
	CMDIdSPQDB *mdid = SPQOS_NEW(mp) CMDIdSPQDB(CMDIdSPQDB::m_mdid_date);
	CRange *prange =
		SPQOS_NEW(mp) CRange(mdid, COptCtxt::PoctxtFromTLS()->Pcomp(),
							pdatumLower, eriLeft, pdatumUpper, eriRight);
	pdrspqrng->Append(prange);

	return SPQOS_NEW(mp)
		CConstraintInterval(mp, colref, pdrspqrng, false /*is_null*/);
}
//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PexprScalarCmpIdentToConstant
//
//	@doc:
//		Helper for generating a scalar compare identifier to a constant
//
//---------------------------------------------------------------------------
CExpression *
CTestUtils::PexprScalarCmpIdentToConstant(CMemoryPool *mp, CExpression *pexpr)
{
	SPQOS_ASSERT(NULL != mp);
	SPQOS_ASSERT(NULL != pexpr);

	CColRefSet *pcrs = pexpr->DeriveOutputColumns();
	CColRef *pcrAny = pcrs->PcrAny();
	CExpression *pexprConst = CUtils::PexprScalarConstInt4(mp, 10 /* val */);

	return CUtils::PexprScalarEqCmp(mp, pcrAny, pexprConst);
}


//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PexprExistsSubquery
//
//	@doc:
//		Helper for generating an exists subquery
//
//---------------------------------------------------------------------------
CExpression *
CTestUtils::PexprExistsSubquery(CMemoryPool *mp, CExpression *pexprOuter)
{
	SPQOS_ASSERT(NULL != mp);
	SPQOS_ASSERT(NULL != pexprOuter);

	CExpression *pexprInner = CTestUtils::PexprLogicalGet(mp);

	return CSubqueryTestUtils::PexprSubqueryExistential(
		mp, COperator::EopScalarSubqueryExists, pexprOuter, pexprInner,
		false /* fCorrelated */
	);
}


//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PexpSubqueryAll
//
//	@doc:
//		Helper for generating an ALL subquery
//
//---------------------------------------------------------------------------
CExpression *
CTestUtils::PexpSubqueryAll(CMemoryPool *mp, CExpression *pexprOuter)
{
	SPQOS_ASSERT(NULL != mp);
	SPQOS_ASSERT(NULL != pexprOuter);

	CColRefSet *outer_refs = pexprOuter->DeriveOutputColumns();
	const CColRef *pcrOuter = outer_refs->PcrAny();


	CExpression *pexprInner = CTestUtils::PexprLogicalGet(mp);
	CColRefSet *pcrsInner = pexprInner->DeriveOutputColumns();
	const CColRef *pcrInner = pcrsInner->PcrAny();

	return SPQOS_NEW(mp) CExpression(
		mp,
		SPQOS_NEW(mp) CScalarSubqueryAll(
			mp, SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_INT4_EQ_OP),
			SPQOS_NEW(mp) CWStringConst(SPQOS_WSZ_LIT("=")), pcrInner),
		pexprInner, CUtils::PexprScalarIdent(mp, pcrOuter));
}


//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PexpSubqueryAny
//
//	@doc:
//		Helper for generating an ANY subquery
//
//---------------------------------------------------------------------------
CExpression *
CTestUtils::PexpSubqueryAny(CMemoryPool *mp, CExpression *pexprOuter)
{
	SPQOS_ASSERT(NULL != mp);
	SPQOS_ASSERT(NULL != pexprOuter);

	CColRefSet *outer_refs = pexprOuter->DeriveOutputColumns();
	const CColRef *pcrOuter = outer_refs->PcrAny();


	CExpression *pexprInner = CTestUtils::PexprLogicalGet(mp);
	CColRefSet *pcrsInner = pexprInner->DeriveOutputColumns();
	const CColRef *pcrInner = pcrsInner->PcrAny();

	return SPQOS_NEW(mp) CExpression(
		mp,
		SPQOS_NEW(mp) CScalarSubqueryAny(
			mp, SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_INT4_EQ_OP),
			SPQOS_NEW(mp) CWStringConst(SPQOS_WSZ_LIT("=")), pcrInner),
		pexprInner, CUtils::PexprScalarIdent(mp, pcrOuter));
}


//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PexprNotExistsSubquery
//
//	@doc:
//		Helper for generating a not exists subquery
//
//---------------------------------------------------------------------------
CExpression *
CTestUtils::PexprNotExistsSubquery(CMemoryPool *mp, CExpression *pexprOuter)
{
	SPQOS_ASSERT(NULL != mp);
	SPQOS_ASSERT(NULL != pexprOuter);

	CExpression *pexprInner = CTestUtils::PexprLogicalGet(mp);

	return CSubqueryTestUtils::PexprSubqueryExistential(
		mp, COperator::EopScalarSubqueryNotExists, pexprOuter, pexprInner,
		false /* fCorrelated */
	);
}


//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::FHasOp
//
//	@doc:
//		 Recursively traverses the subtree rooted at the given expression, and
//  	 return the first subexpression it encounters that has the given id
//
//---------------------------------------------------------------------------
const CExpression *
CTestUtils::PexprFirst(const CExpression *pexpr,
					   const COperator::EOperatorId op_id)
{
	SPQOS_CHECK_STACK_SIZE;
	SPQOS_ASSERT(NULL != pexpr);

	if (pexpr->Pop()->Eopid() == op_id)
	{
		return pexpr;
	}

	// recursively check children
	const ULONG arity = pexpr->Arity();
	for (ULONG ul = 0; ul < arity; ul++)
	{
		const CExpression *pexprFirst = PexprFirst((*pexpr)[ul], op_id);
		if (NULL != pexprFirst)
		{
			return pexprFirst;
		}
	}

	return NULL;
}


//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PexprAnd
//
//	@doc:
//		Generate a scalar AND expression
//
//---------------------------------------------------------------------------
CExpression *
CTestUtils::PexprAnd(CMemoryPool *mp, CExpression *pexprActual,
					 CExpression *pexprExpected)
{
	SPQOS_ASSERT(NULL != pexprActual);
	SPQOS_ASSERT(NULL != pexprExpected);

	CExpressionArray *pdrspqexpr = SPQOS_NEW(mp) CExpressionArray(mp);
	pdrspqexpr->Append(pexprActual);
	pdrspqexpr->Append(pexprExpected);

	return CUtils::PexprScalarBoolOp(mp, CScalarBoolOp::EboolopAnd, pdrspqexpr);
}


//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::PexprOr
//
//	@doc:
//		Generate a scalar OR expression
//
//---------------------------------------------------------------------------
CExpression *
CTestUtils::PexprOr(CMemoryPool *mp, CExpression *pexprActual,
					CExpression *pexprExpected)
{
	SPQOS_ASSERT(NULL != pexprActual);
	SPQOS_ASSERT(NULL != pexprExpected);

	CExpressionArray *pdrspqexpr = SPQOS_NEW(mp) CExpressionArray(mp);
	pdrspqexpr->Append(pexprActual);
	pdrspqexpr->Append(pexprExpected);

	return CUtils::PexprScalarBoolOp(mp, CScalarBoolOp::EboolopOr, pdrspqexpr);
}

//---------------------------------------------------------------------------
//	@function:
//		CTestUtils::EresUnittest_RunTests
//
//	@doc:
//		Run  Minidump-based tests in the given array of files
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CTestUtils::EresUnittest_RunTests(const CHAR **rgszFileNames,
								  ULONG *pulTestCounter, ULONG ulTests)
{
	BOOL fMatchPlans = false;
	BOOL fTestSpacePruning = false;
	fMatchPlans = true;
	fTestSpacePruning = true;
	// enable (Redistribute, Broadcast) hash join plans
	CAutoTraceFlag atf1(EopttraceEnableRedistributeBroadcastHashJoin,
						true /*value*/);

	// enable plan enumeration only if we match plans
	CAutoTraceFlag atf2(EopttraceEnumeratePlans, fMatchPlans);

	// enable stats derivation for DPE
	CAutoTraceFlag atf3(EopttraceDeriveStatsForDPE, true /*value*/);

	// prefer MDQA
	CAutoTraceFlag atf5(EopttraceForceExpandedMDQAs, true);

	SPQOS_RESULT eres = EresUnittest_RunTestsWithoutAdditionalTraceFlags(
		rgszFileNames, pulTestCounter, ulTests, fMatchPlans, fTestSpacePruning);
	return eres;
}


SPQOS_RESULT
CTestUtils::EresUnittest_RunTestsWithoutAdditionalTraceFlags(
	const CHAR **rgszFileNames, ULONG *pulTestCounter, ULONG ulTests,
	BOOL fMatchPlans, BOOL fTestSpacePruning)
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();


	SPQOS_RESULT eres =
		CTestUtils::EresRunMinidumps(mp, rgszFileNames, ulTests, pulTestCounter,
									 1,	 // ulSessionId
									 1,	 // ulCmdId
									 fMatchPlans, fTestSpacePruning);

	return eres;
}


// Create Equivalence Class based on the breakpoints
CColRefSetArray *
CTestUtils::createEquivalenceClasses(CMemoryPool *mp, CColRefSet *pcrs,
									 INT setBoundary[])
{
	INT i = 0;
	ULONG bpIndex = 0;

	CColRefSetArray *pdrgcrs = SPQOS_NEW(mp) CColRefSetArray(mp);

	CColRefSetIter crsi(*pcrs);
	CColRefSet *pcrsLoop = SPQOS_NEW(mp) CColRefSet(mp);

	while (crsi.Advance())
	{
		if (i == setBoundary[bpIndex])
		{
			pdrgcrs->Append(pcrsLoop);
			CColRefSet *pcrsLoop1 = SPQOS_NEW(mp) CColRefSet(mp);
			pcrsLoop = pcrsLoop1;
			bpIndex++;
		}

		CColRef *colref = crsi.Pcr();
		pcrsLoop->Include(colref);
		i++;
	}
	pdrgcrs->Append(pcrsLoop);
	return pdrgcrs;
}
// EOF
