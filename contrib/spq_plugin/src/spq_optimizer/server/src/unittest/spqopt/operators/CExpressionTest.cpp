//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		CExpressionTest.cpp
//
//	@doc:
//		Test for CExpression
//---------------------------------------------------------------------------
#include "unittest/spqopt/operators/CExpressionTest.h"

#include "spqos/error/CAutoTrace.h"
#include "spqos/io/COstreamString.h"
#include "spqos/string/CWStringDynamic.h"

#include "spqopt/base/CCTEReq.h"
#include "spqopt/base/CDrvdPropCtxtPlan.h"
#include "spqopt/base/CUtils.h"
#include "spqopt/engine/CEngine.h"
#include "spqopt/eval/CConstExprEvaluatorDefault.h"
#include "spqopt/mdcache/CAutoMDAccessor.h"
#include "spqopt/operators/CLogicalDynamicGetBase.h"
#include "spqopt/operators/ops.h"
#include "naucrates/base/CDatumInt8SPQDB.h"
#include "naucrates/md/CMDIdSPQDB.h"
#include "naucrates/md/IMDScalarOp.h"

#include "unittest/base.h"
#include "unittest/spqopt/CTestUtils.h"

using namespace spqos;

//---------------------------------------------------------------------------
//	@function:
//		CExpressionTest::EresUnittest
//
//	@doc:
//		Create required properties which are empty, except for required column set, given by 'pcrs'.
// 		Caller takes ownership of returned pointer.
//
//---------------------------------------------------------------------------
CReqdPropPlan *
CExpressionTest::PrppCreateRequiredProperties(CMemoryPool *mp, CColRefSet *pcrs)
{
	COrderSpec *pos = SPQOS_NEW(mp) COrderSpec(mp);
	CDistributionSpec *pds = SPQOS_NEW(mp)
		CDistributionSpecSingleton(CDistributionSpecSingleton::EstMaster);
	CRewindabilitySpec *prs = SPQOS_NEW(mp) CRewindabilitySpec(
		CRewindabilitySpec::ErtNone, CRewindabilitySpec::EmhtNoMotion);
	CEnfdOrder *peo = SPQOS_NEW(mp) CEnfdOrder(pos, CEnfdOrder::EomSatisfy);
	CEnfdDistribution *ped =
		SPQOS_NEW(mp) CEnfdDistribution(pds, CEnfdDistribution::EdmSatisfy);
	CEnfdRewindability *per =
		SPQOS_NEW(mp) CEnfdRewindability(prs, CEnfdRewindability::ErmSatisfy);
	CCTEReq *pcter = SPQOS_NEW(mp) CCTEReq(mp);
	return SPQOS_NEW(mp) CReqdPropPlan(pcrs, peo, ped, per, pcter);
}

//---------------------------------------------------------------------------
//	@function:
//		CExpressionTest::EresUnittest
//
//	@doc:
//		Creates a logical GroupBy and a Get as its child. The columns in the
//		Get follow the format wszColNameFormat.
//		Caller takes ownership of returned pointer.
//
//---------------------------------------------------------------------------
CExpression *
CExpressionTest::PexprCreateGbyWithColumnFormat(CMemoryPool *mp,
												const WCHAR *wszColNameFormat)
{
	CWStringConst strRelName(SPQOS_WSZ_LIT("Rel1"));
	CMDIdSPQDB *rel_mdid =
		SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidRel, SPQOPT_TEST_REL_OID1, 1, 1);
	CTableDescriptor *ptabdesc = CTestUtils::PtabdescPlainWithColNameFormat(
		mp, 3, rel_mdid, wszColNameFormat, CName(&strRelName), false);
	CWStringConst strRelAlias(SPQOS_WSZ_LIT("Rel1"));
	CExpression *pexprGet =
		CTestUtils::PexprLogicalGet(mp, ptabdesc, &strRelAlias);
	return CTestUtils::PexprLogicalGbAggWithInput(mp, pexprGet);
}

//---------------------------------------------------------------------------
//	@function:
//		CExpressionTest::EresUnittest
//
//	@doc:
//		Unittest for expressions
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CExpressionTest::EresUnittest()
{
	CUnittest rgut[] = {
		SPQOS_UNITTEST_FUNC(CExpressionTest::EresUnittest_SimpleOps),
		SPQOS_UNITTEST_FUNC(CExpressionTest::EresUnittest_Union),
		SPQOS_UNITTEST_FUNC(CExpressionTest::EresUnittest_BitmapGet),
		SPQOS_UNITTEST_FUNC(CExpressionTest::EresUnittest_Const),
#ifdef SPQOS_DEBUG
		SPQOS_UNITTEST_FUNC(CExpressionTest::EresUnittest_ComparisonTypes),
#endif	// SPQOS_DEBUG
		SPQOS_UNITTEST_FUNC(CExpressionTest::EresUnittest_FValidPlan),
		SPQOS_UNITTEST_FUNC(
			CExpressionTest::EresUnittest_FValidPlan_InvalidOrder),
		SPQOS_UNITTEST_FUNC(
			CExpressionTest::EresUnittest_FValidPlan_InvalidDistribution),
		SPQOS_UNITTEST_FUNC(
			CExpressionTest::EresUnittest_FValidPlan_InvalidRewindability),
		SPQOS_UNITTEST_FUNC(
			CExpressionTest::EresUnittest_FValidPlan_InvalidCTEs),

#ifdef SPQOS_DEBUG
		SPQOS_UNITTEST_FUNC(CExpressionTest::EresUnittest_FValidPlanError),
#endif	// SPQOS_DEBUG
		SPQOS_UNITTEST_FUNC(EresUnittest_ReqdCols),
#ifdef SPQOS_DEBUG
		SPQOS_UNITTEST_FUNC_ASSERT(CExpressionTest::EresUnittest_InvalidSetOp),
#endif	// SPQOS_DEBUG
	};

	return CUnittest::EresExecute(rgut, SPQOS_ARRAY_SIZE(rgut));
}

//---------------------------------------------------------------------------
//	@function:
//		CExpressionTest::EresUnittest_SimpleOps
//
//	@doc:
//		Basic tree builder test
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CExpressionTest::EresUnittest_SimpleOps()
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	// setup a file-based provider
	CMDProviderMemory *pmdp = CTestUtils::m_pmdpf;
	pmdp->AddRef();
	CMDAccessor mda(mp, CMDCache::Pcache(), CTestUtils::m_sysidDefault, pmdp);

	typedef CExpression *(*Pfpexpr)(CMemoryPool *);

	Pfpexpr rspqf[] = {
		CTestUtils::PexprLogicalGet,
		CTestUtils::PexprLogicalExternalGet,
		CTestUtils::PexprLogicalGetPartitioned,
		CTestUtils::PexprLogicalSelect,
		CTestUtils::PexprLogicalSelectCmpToConst,
		CTestUtils::PexprLogicalSelectArrayCmp,
		CTestUtils::PexprLogicalSelectPartitioned,
		CTestUtils::PexprLogicalJoin<CLogicalInnerJoin>,
		CTestUtils::PexprLogicalJoin<CLogicalLeftOuterJoin>,
		CTestUtils::PexprLogicalJoin<CLogicalLeftSemiJoin>,
		CTestUtils::PexprLogicalJoin<CLogicalLeftAntiSemiJoin>,
		CTestUtils::PexprLogicalJoin<CLogicalLeftAntiSemiJoinNotIn>,
		CTestUtils::PexprLogicalGbAgg,
		CTestUtils::PexprLogicalGbAggOverJoin,
		CTestUtils::PexprLogicalGbAggWithSum,
		CTestUtils::PexprLogicalLimit,
		CTestUtils::PexprLogicalNAryJoin,
		CTestUtils::PexprLogicalProject,
		CTestUtils::PexprConstTableGet5,
		CTestUtils::PexprLogicalDynamicGet,
		CTestUtils::PexprLogicalSequence,
		CTestUtils::PexprLogicalTVFTwoArgs,
		CTestUtils::PexprLogicalAssert,
		PexprComplexJoinTree};

#ifdef SPQOS_DEBUG
	// misestimation risk for the roots of the plans in rspqf
	ULONG rgulRisk[] = {1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2,
						2, 3, 2, 1, 2, 1, 1, 1, 1, 1, 1, 6};
#endif	// SPQOS_DEBUG

	for (ULONG i = 0; i < SPQOS_ARRAY_SIZE(rspqf); i++)
	{
		// install opt context in TLS
		CAutoOptCtxt aoc(mp, &mda, NULL, /* pceeval */
						 CTestUtils::GetCostModel(mp));

		// generate simple expression
		CExpression *pexpr = rspqf[i](mp);

		CLogicalGet *popGet = dynamic_cast<CLogicalGet *>(pexpr->Pop());
		CLogicalDynamicGetBase *popDynGet =
			dynamic_cast<CLogicalDynamicGetBase *>(pexpr->Pop());
		CColRefArray *colrefs = NULL;

		if (NULL != popGet)
		{
			colrefs = popGet->PdrspqcrOutput();
		}
		else if (NULL != popDynGet)
		{
			colrefs = popDynGet->PdrspqcrOutput();
		}

		if (NULL != colrefs)
		{
			for (ULONG ul = 0; ul < colrefs->Size(); ul++)
			{
				(*colrefs)[ul]->MarkAsUsed();
			}
		}

		// self-match
		SPQOS_ASSERT(pexpr->FMatchDebug(pexpr));

		// debug print
		CWStringDynamic str(mp, SPQOS_WSZ_LIT("\n"));
		COstreamString oss(&str);

		oss << "EXPR:" << std::endl << *pexpr << std::endl;
		SPQOS_TRACE(str.GetBuffer());
		str.Reset();

#ifdef SPQOS_DEBUG

		oss << std::endl << "DERIVED PROPS:" << std::endl;
		SPQOS_TRACE(str.GetBuffer());
		str.Reset();
		pexpr->DbgPrint();

		// copy expression
		CColRef *pcrOld = pexpr->DeriveOutputColumns()->PcrAny();
		CColRef *new_colref =
			COptCtxt::PoctxtFromTLS()->Pcf()->PcrCreate(pcrOld);
		UlongToColRefMap *colref_mapping = SPQOS_NEW(mp) UlongToColRefMap(mp);

		BOOL result = colref_mapping->Insert(SPQOS_NEW(mp) ULONG(pcrOld->Id()),
											 new_colref);
		SPQOS_ASSERT(result);
		CExpression *pexprCopy = pexpr->PexprCopyWithRemappedColumns(
			mp, colref_mapping, true /*must_exist*/);
		colref_mapping->Release();
		oss << std::endl
			<< "COPIED EXPRESSION (AFTER MAPPING " << *pcrOld << " TO "
			<< *new_colref << "):" << std::endl;
		SPQOS_TRACE(str.GetBuffer());
		str.Reset();
		pexprCopy->DbgPrint();
		pexprCopy->Release();

		// derive stats on expression
		CReqdPropRelational *prprel =
			SPQOS_NEW(mp) CReqdPropRelational(SPQOS_NEW(mp) CColRefSet(mp));
		IStatisticsArray *stats_ctxt = SPQOS_NEW(mp) IStatisticsArray(mp);
		IStatistics *stats = pexpr->PstatsDerive(prprel, stats_ctxt);
		SPQOS_ASSERT(NULL != stats);

		oss << "Expected risk: " << rgulRisk[i] << std::endl;
		oss << std::endl << "STATS:" << *stats << std::endl;
		SPQOS_TRACE(str.GetBuffer());

		SPQOS_ASSERT(rgulRisk[i] == stats->StatsEstimationRisk());

		prprel->Release();
		stats_ctxt->Release();
#endif	// SPQOS_DEBUG

		// cleanup
		pexpr->Release();
	}

	return SPQOS_OK;
}

//---------------------------------------------------------------------------
//		CExpressionTest::EresUnittest_Union
//
//	@doc:
//		Basic tree builder test w/ Unions
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CExpressionTest::EresUnittest_Union()
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

	// build union tree of depth 2
	CExpression *pexpr = CTestUtils::PexprLogicalUnion(mp, 2);

	// debug print
	CWStringDynamic str(mp);
	COstreamString oss(&str);
	pexpr->OsPrint(oss);

	SPQOS_TRACE(str.GetBuffer());

	// derive properties on expression
	(void) pexpr->PdpDerive();

#ifdef SPQOS_DEBUG
	CReqdPropRelational *prprel =
		SPQOS_NEW(mp) CReqdPropRelational(SPQOS_NEW(mp) CColRefSet(mp));
	IStatisticsArray *stats_ctxt = SPQOS_NEW(mp) IStatisticsArray(mp);
	IStatistics *stats = pexpr->PstatsDerive(prprel, stats_ctxt);
	SPQOS_ASSERT(NULL != stats);

	// We expect a risk of 3 because every Union increments the risk.
	SPQOS_ASSERT(3 == stats->StatsEstimationRisk());
	stats_ctxt->Release();
	prprel->Release();
#endif	// SPQOS_DEBUG

	// cleanup
	pexpr->Release();

	return SPQOS_OK;
}

//---------------------------------------------------------------------------
//		CExpressionTest::EresUnittest_BitmapScan
//
//	@doc:
//		Basic tree builder test with bitmap index and table get.
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CExpressionTest::EresUnittest_BitmapGet()
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

	CWStringConst strRelName(SPQOS_WSZ_LIT("MyTable"));
	CWStringConst strRelAlias(SPQOS_WSZ_LIT("T"));
	CMDIdSPQDB *rel_mdid =
		SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidRel, SPQOPT_MDCACHE_TEST_OID, 1, 1);
	const WCHAR *wszColNameFormat = SPQOS_WSZ_LIT("column_%04d");
	CTableDescriptor *ptabdesc = CTestUtils::PtabdescPlainWithColNameFormat(
		mp, 3, rel_mdid, wszColNameFormat, CName(&strRelName), false);

	// get the index associated with the table
	const IMDRelation *pmdrel = mda.RetrieveRel(ptabdesc->MDId());
	SPQOS_ASSERT(0 < pmdrel->IndexCount());

	// create an index descriptor
	IMDId *pmdidIndex = pmdrel->IndexMDidAt(0 /*pos*/);
	const IMDIndex *pmdindex = mda.RetrieveIndex(pmdidIndex);

	CColumnFactory *col_factory = COptCtxt::PoctxtFromTLS()->Pcf();

	const ULONG num_cols = pmdrel->ColumnCount();
	SPQOS_ASSERT(2 < num_cols);

	// create an index on the first column
	const IMDColumn *pmdcol = pmdrel->GetMdCol(0);
	const IMDType *pmdtype = mda.RetrieveType(pmdcol->MdidType());
	CColRef *pcrFirst = col_factory->PcrCreate(pmdtype, pmdcol->TypeModifier());

	CExpression *pexprIndexCond = CUtils::PexprScalarEqCmp(
		mp, pcrFirst, CUtils::PexprScalarConstInt4(mp, 20 /*val*/));

	CMDIdSPQDB *mdid = SPQOS_NEW(mp) CMDIdSPQDB(CMDIdSPQDB::m_mdid_unknown);
	CIndexDescriptor *pindexdesc =
		CIndexDescriptor::Pindexdesc(mp, ptabdesc, pmdindex);
	CExpression *pexprBitmapIndex = SPQOS_NEW(mp) CExpression(
		mp, SPQOS_NEW(mp) CScalarBitmapIndexProbe(mp, pindexdesc, mdid),
		pexprIndexCond);

	CColRefArray *pdrspqcrTable = SPQOS_NEW(mp) CColRefArray(mp);
	for (ULONG ul = 0; ul < num_cols; ++ul)
	{
		const IMDColumn *pmdcol = pmdrel->GetMdCol(ul);
		const IMDType *pmdtype = mda.RetrieveType(pmdcol->MdidType());
		CColRef *colref =
			col_factory->PcrCreate(pmdtype, pmdcol->TypeModifier());
		pdrspqcrTable->Append(colref);
	}

	CExpression *pexprTableCond = CUtils::PexprScalarEqCmp(
		mp, pcrFirst, CUtils::PexprScalarConstInt4(mp, 20 /*val*/));

	CExpression *pexprBitmapTableGet = SPQOS_NEW(mp) CExpression(
		mp,
		SPQOS_NEW(mp) CLogicalBitmapTableGet(
			mp, ptabdesc,
			spqos::ulong_max,  // pgexprOrigin
			SPQOS_NEW(mp) CName(mp, CName(&strRelAlias)), pdrspqcrTable),
		pexprTableCond, pexprBitmapIndex);

	// debug print
	CWStringDynamic str(mp);
	COstreamString oss(&str);
	pexprBitmapTableGet->OsPrint(oss);

	CWStringConst strExpectedDebugPrint(SPQOS_WSZ_LIT(
		"+--CLogicalBitmapTableGet , Table Name: (\"MyTable\"), Columns: [\"ColRef_0001\" (1), \"ColRef_0002\" (2), \"ColRef_0003\" (3)]\n"
		"   |--CScalarCmp (=)\n"
		"   |  |--CScalarIdent \"ColRef_0000\" (0)\n"
		"   |  +--CScalarConst (20)\n"
		"   +--CScalarBitmapIndexProbe   Bitmap Index Name: (\"T_a\")\n"
		"      +--CScalarCmp (=)\n"
		"         |--CScalarIdent \"ColRef_0000\" (0)\n"
		"         +--CScalarConst (20)\n"));

	SPQOS_ASSERT(str.Equals(&strExpectedDebugPrint));

	// derive properties on expression
	(void) pexprBitmapTableGet->PdpDerive();

	// test matching of bitmap index probe expressions
	CMDIdSPQDB *pmdid2 = SPQOS_NEW(mp) CMDIdSPQDB(CMDIdSPQDB::m_mdid_unknown);
	CIndexDescriptor *pindexdesc2 =
		CIndexDescriptor::Pindexdesc(mp, ptabdesc, pmdindex);
	CExpression *pexprIndexCond2 = CUtils::PexprScalarEqCmp(
		mp, pcrFirst, CUtils::PexprScalarConstInt4(mp, 20 /*val*/));
	CExpression *pexprBitmapIndex2 = SPQOS_NEW(mp) CExpression(
		mp, SPQOS_NEW(mp) CScalarBitmapIndexProbe(mp, pindexdesc2, pmdid2),
		pexprIndexCond2);
	CWStringDynamic strIndex2(mp);
	COstreamString ossIndex2(&strIndex2);
	pexprBitmapIndex2->OsPrint(ossIndex2);
	CWStringConst strExpectedDebugPrintIndex2(SPQOS_WSZ_LIT(
		// clang-format off
		"+--CScalarBitmapIndexProbe   Bitmap Index Name: (\"T_a\")\n"
		"   +--CScalarCmp (=)\n"
		"      |--CScalarIdent \"ColRef_0000\" (0)\n"
		"      +--CScalarConst (20)\n"
		// clang-format on
		));

	SPQOS_ASSERT(strIndex2.Equals(&strExpectedDebugPrintIndex2));
	SPQOS_ASSERT(pexprBitmapIndex2->Matches(pexprBitmapIndex));

	mdid->AddRef();
	pexprBitmapIndex->AddRef();
	pexprBitmapIndex2->AddRef();
	CExpression *pexprBitmapBoolOp1 = SPQOS_NEW(mp) CExpression(
		mp,
		SPQOS_NEW(mp)
			CScalarBitmapBoolOp(mp, CScalarBitmapBoolOp::EbitmapboolAnd, mdid),
		pexprBitmapIndex, pexprBitmapIndex2);

	mdid->AddRef();
	pexprBitmapIndex->AddRef();
	CExpression *pexprBitmapBoolOp2 = SPQOS_NEW(mp) CExpression(
		mp,
		SPQOS_NEW(mp)
			CScalarBitmapBoolOp(mp, CScalarBitmapBoolOp::EbitmapboolAnd, mdid),
		pexprBitmapIndex, pexprBitmapIndex2);
	SPQOS_ASSERT(pexprBitmapBoolOp2->Matches(pexprBitmapBoolOp1));

	// cleanup
	pexprBitmapBoolOp2->Release();
	pexprBitmapBoolOp1->Release();
	pexprBitmapTableGet->Release();

	return SPQOS_OK;
}

//---------------------------------------------------------------------------
//	@function:
//		CExpressionTest::EresUnittest_Const
//
//	@doc:
//		Test of scalar constant expressions
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CExpressionTest::EresUnittest_Const()
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

	BOOL value = true;
	CExpression *pexprTrue = CUtils::PexprScalarConstBool(mp, value);

	value = false;
	CExpression *pexprFalse = CUtils::PexprScalarConstBool(mp, value);

	ULONG ulVal = 123456;
	CExpression *pexprUl = CUtils::PexprScalarConstInt4(mp, ulVal);
	CExpression *pexprUl2nd = CUtils::PexprScalarConstInt4(mp, ulVal);

	ulVal = 1;
	CExpression *pexprUlOne = CUtils::PexprScalarConstInt4(mp, ulVal);

	CWStringDynamic str(mp);
	COstreamString oss(&str);
	oss << std::endl;
	pexprTrue->OsPrint(oss);
	pexprFalse->OsPrint(oss);
	pexprUl->OsPrint(oss);
	pexprUl2nd->OsPrint(oss);
	pexprUlOne->OsPrint(oss);

#ifdef SPQOS_DEBUG
	CScalarConst *pscalarconstTrue = CScalarConst::PopConvert(pexprTrue->Pop());
	CScalarConst *pscalarconstFalse =
		CScalarConst::PopConvert(pexprFalse->Pop());
	CScalarConst *pscalarconstUl = CScalarConst::PopConvert(pexprUl->Pop());
	CScalarConst *pscalarconstUl2nd =
		CScalarConst::PopConvert(pexprUl2nd->Pop());
	CScalarConst *pscalarconstUlOne =
		CScalarConst::PopConvert(pexprUlOne->Pop());
#endif	// SPQOS_DEBUG

	SPQOS_ASSERT(pscalarconstUl->HashValue() == pscalarconstUl2nd->HashValue());
	SPQOS_ASSERT(!pscalarconstTrue->Matches(pscalarconstFalse));
	SPQOS_ASSERT(!pscalarconstTrue->Matches(pscalarconstUlOne));

	pexprTrue->Release();
	pexprFalse->Release();
	pexprUl->Release();
	pexprUl2nd->Release();
	pexprUlOne->Release();

	SPQOS_TRACE(str.GetBuffer());

	return SPQOS_OK;
}


#ifdef SPQOS_DEBUG
//---------------------------------------------------------------------------
//	@function:
//		CExpressionTest::EresUnittest_ComparisonTypes
//
//	@doc:
//		Test of scalar comparison types
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CExpressionTest::EresUnittest_ComparisonTypes()
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	// setup a file-based provider
	CMDProviderMemory *pmdp = CTestUtils::m_pmdpf;
	pmdp->AddRef();
	CMDAccessor mda(mp, CMDCache::Pcache());
	mda.RegisterProvider(CTestUtils::m_sysidDefault, pmdp);

	// install opt context in TLS
	CAutoOptCtxt aoc(mp, &mda, NULL, /* pceeval */
					 CTestUtils::GetCostModel(mp));

	const IMDType *pmdtype = mda.PtMDType<IMDTypeInt4>();

	SPQOS_ASSERT(
		IMDType::EcmptEq ==
		CUtils::ParseCmpType(pmdtype->GetMdidForCmpType(IMDType::EcmptEq)));
	SPQOS_ASSERT(
		IMDType::EcmptL ==
		CUtils::ParseCmpType(pmdtype->GetMdidForCmpType(IMDType::EcmptL)));
	SPQOS_ASSERT(
		IMDType::EcmptG ==
		CUtils::ParseCmpType(pmdtype->GetMdidForCmpType(IMDType::EcmptG)));

	const IMDScalarOp *pmdscopEq =
		mda.RetrieveScOp(pmdtype->GetMdidForCmpType(IMDType::EcmptEq));
	const IMDScalarOp *pmdscopLT =
		mda.RetrieveScOp(pmdtype->GetMdidForCmpType(IMDType::EcmptL));
	const IMDScalarOp *pmdscopGT =
		mda.RetrieveScOp(pmdtype->GetMdidForCmpType(IMDType::EcmptG));

	SPQOS_ASSERT(IMDType::EcmptNEq ==
				CUtils::ParseCmpType(pmdscopEq->GetInverseOpMdid()));
	SPQOS_ASSERT(IMDType::EcmptLEq ==
				CUtils::ParseCmpType(pmdscopGT->GetInverseOpMdid()));
	SPQOS_ASSERT(IMDType::EcmptGEq ==
				CUtils::ParseCmpType(pmdscopLT->GetInverseOpMdid()));

	return SPQOS_OK;
}
#endif	// SPQOS_DEBUG

//---------------------------------------------------------------------------
//	@function:
//		CExpressionTest::SetupPlanForFValidPlanTest
//
//	@doc:
//		Helper function for the FValidPlan tests
//
//---------------------------------------------------------------------------
void
CExpressionTest::SetupPlanForFValidPlanTest(CMemoryPool *mp,
											CExpression **ppexprGby,
											CColRefSet **ppcrs,
											CExpression **ppexprPlan,
											CReqdPropPlan **pprpp)
{
	*ppexprGby =
		PexprCreateGbyWithColumnFormat(mp, SPQOS_WSZ_LIT("Test Column%d"));

	// Create a column requirement using the first output column of the group by.
	CColRefSet *pcrsGby = (*ppexprGby)->DeriveOutputColumns();
	*ppcrs = SPQOS_NEW(mp) CColRefSet(mp);
	(*ppcrs)->Include(pcrsGby->PcrFirst());

	*pprpp = PrppCreateRequiredProperties(mp, *ppcrs);
	CExpressionHandle exprhdl(mp);
	exprhdl.Attach(*ppexprGby);
	exprhdl.InitReqdProps(*pprpp);

	// Optimize the logical plan under default required properties, which are always satisfied.
	CEngine eng(mp);
	CAutoP<CQueryContext> pqc;
	pqc = CTestUtils::PqcGenerate(mp, *ppexprGby);
	eng.Init(pqc.Value(), NULL /*search_stage_array*/);
	eng.Optimize();
	*ppexprPlan = eng.PexprExtractPlan();
}

//---------------------------------------------------------------------------
//	@function:
//		CExpressionTest::EresUnittest_FValidPlan
//
//	@doc:
//		Test for CExpression::FValidPlan
// 		Test now just very basic cases. More complex cases are covered by minidump tests
// 		in CEnumeratorTest.
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CExpressionTest::EresUnittest_FValidPlan()
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
	const IMDType *pmdtype = mda.PtMDType<IMDTypeInt4>();

	// Create a group-by with a get child. Properties required contain one of the columns in the group by.
	// Test that the plan is valid.
	{
		CExpression *pexprGby = NULL;
		CColRefSet *pcrs = NULL;
		CExpression *pexprPlan = NULL;
		CReqdPropPlan *prpp = NULL;

		SetupPlanForFValidPlanTest(mp, &pexprGby, &pcrs, &pexprPlan, &prpp);
		CDrvdPropCtxtPlan *pdpctxtplan = SPQOS_NEW(mp) CDrvdPropCtxtPlan(mp);

		// Test that prpp is actually satisfied.
		SPQOS_ASSERT(pexprPlan->FValidPlan(prpp, pdpctxtplan));
		pdpctxtplan->Release();
		pexprPlan->Release();
		prpp->Release();
		pexprGby->Release();
	}
	// Create a group-by with a get child. Properties required contain one column that doesn't exist.
	// Test that the plan is NOT valid.
	{
		CExpression *pexprGby =
			PexprCreateGbyWithColumnFormat(mp, SPQOS_WSZ_LIT("Test Column%d"));

		(void) pexprGby->PdpDerive();
		CColRefSet *pcrsInvalid = SPQOS_NEW(mp) CColRefSet(mp);
		CColumnFactory *col_factory = COptCtxt::PoctxtFromTLS()->Pcf();

		// Creating the column reference with the column factory ensures that it's a brand new column.
		CColRef *pcrComputed =
			col_factory->PcrCreate(pmdtype, default_type_modifier);
		pcrsInvalid->Include(pcrComputed);

		CReqdPropPlan *prpp = PrppCreateRequiredProperties(mp, pcrsInvalid);
		CExpressionHandle exprhdl(mp);
		exprhdl.Attach(pexprGby);
		exprhdl.InitReqdProps(prpp);

		// Optimize the logical plan, but under default required properties, which are always satisfied.
		CEngine eng(mp);
		CAutoP<CQueryContext> pqc;
		pqc = CTestUtils::PqcGenerate(mp, pexprGby);
		eng.Init(pqc.Value(), NULL /*search_stage_array*/);
		eng.Optimize();
		CExpression *pexprPlan = eng.PexprExtractPlan();

		CDrvdPropCtxtPlan *pdpctxtplan = SPQOS_NEW(mp) CDrvdPropCtxtPlan(mp);

		// Test that prpp is actually unsatisfied.
		SPQOS_ASSERT(!pexprPlan->FValidPlan(prpp, pdpctxtplan));
		pdpctxtplan->Release();
		pexprPlan->Release();
		prpp->Release();
		pexprGby->Release();
	}

	return SPQOS_OK;
}

//---------------------------------------------------------------------------
//	@function:
//		CExpressionTest::EresUnittest_FValidPlan_InvalidOrder
//
//	@doc:
//		Test for CExpression::FValidPlan with incompatible order properties
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CExpressionTest::EresUnittest_FValidPlan_InvalidOrder()
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

	CExpression *pexprGby = NULL;
	CColRefSet *pcrs = NULL;
	CExpression *pexprPlan = NULL;
	CReqdPropPlan *prpp = NULL;

	SetupPlanForFValidPlanTest(mp, &pexprGby, &pcrs, &pexprPlan, &prpp);
	CDrvdPropCtxtPlan *pdpctxtplan = SPQOS_NEW(mp) CDrvdPropCtxtPlan(mp);

	// Create similar requirements, but
	// add an order requirement using a couple of output columns of a Get
	CExpression *pexprGet = CTestUtils::PexprLogicalGet(mp);
	CColRefSet *pcrsGet = pexprGet->DeriveOutputColumns();
	CColRefSet *pcrsGetCopy = SPQOS_NEW(mp) CColRefSet(mp, *pcrsGet);

	CColRefArray *pdrspqcrGet = pcrsGetCopy->Pdrspqcr(mp);
	SPQOS_ASSERT(2 <= pdrspqcrGet->Size());

	COrderSpec *pos = SPQOS_NEW(mp) COrderSpec(mp);
	const IMDType *pmdtypeint4 = mda.PtMDType<IMDTypeInt4>();
	IMDId *pmdidInt4LT = pmdtypeint4->GetMdidForCmpType(IMDType::EcmptL);
	pmdidInt4LT->AddRef();
	pos->Append(pmdidInt4LT, (*pdrspqcrGet)[1], COrderSpec::EntFirst);
	CEnfdOrder *peo = SPQOS_NEW(mp) CEnfdOrder(pos, CEnfdOrder::EomSatisfy);

	CDistributionSpec *pds = SPQOS_NEW(mp)
		CDistributionSpecSingleton(CDistributionSpecSingleton::EstMaster);
	CRewindabilitySpec *prs = SPQOS_NEW(mp) CRewindabilitySpec(
		CRewindabilitySpec::ErtNone, CRewindabilitySpec::EmhtNoMotion);
	CEnfdDistribution *ped =
		SPQOS_NEW(mp) CEnfdDistribution(pds, CEnfdDistribution::EdmExact);
	CEnfdRewindability *per =
		SPQOS_NEW(mp) CEnfdRewindability(prs, CEnfdRewindability::ErmSatisfy);
	CCTEReq *pcter = SPQOS_NEW(mp) CCTEReq(mp);
	CReqdPropPlan *prppIncompatibleOrder =
		SPQOS_NEW(mp) CReqdPropPlan(pcrsGetCopy, peo, ped, per, pcter);

	CPartInfo *ppartinfo = SPQOS_NEW(mp) CPartInfo(mp);
	prppIncompatibleOrder->InitReqdPartitionPropagation(mp, ppartinfo);

	// Test that the plan is not valid.
	SPQOS_ASSERT(!pexprPlan->FValidPlan(prppIncompatibleOrder, pdpctxtplan));
	pdpctxtplan->Release();
	prppIncompatibleOrder->Release();
	pdrspqcrGet->Release();
	ppartinfo->Release();
	prpp->Release();
	pexprGet->Release();
	pexprPlan->Release();
	pexprGby->Release();

	return SPQOS_OK;
}

//---------------------------------------------------------------------------
//	@function:
//		CExpressionTest::EresUnittest_FValidPlan_InvalidDistribution
//
//	@doc:
//		Test for CExpression::FValidPlan with incompatible distribution properties
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CExpressionTest::EresUnittest_FValidPlan_InvalidDistribution()
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

	CExpression *pexprGby = NULL;
	CColRefSet *pcrs = NULL;
	CExpression *pexprPlan = NULL;
	CReqdPropPlan *prpp = NULL;

	SetupPlanForFValidPlanTest(mp, &pexprGby, &pcrs, &pexprPlan, &prpp);
	CDrvdPropCtxtPlan *pdpctxtplan = SPQOS_NEW(mp) CDrvdPropCtxtPlan(mp);
	COrderSpec *pos = SPQOS_NEW(mp) COrderSpec(mp);
	CDistributionSpec *pds = SPQOS_NEW(mp) CDistributionSpecRandom();
	CRewindabilitySpec *prs = SPQOS_NEW(mp) CRewindabilitySpec(
		CRewindabilitySpec::ErtNone, CRewindabilitySpec::EmhtNoMotion);
	CEnfdOrder *peo = SPQOS_NEW(mp) CEnfdOrder(pos, CEnfdOrder::EomSatisfy);
	CEnfdDistribution *ped =
		SPQOS_NEW(mp) CEnfdDistribution(pds, CEnfdDistribution::EdmExact);
	CEnfdRewindability *per =
		SPQOS_NEW(mp) CEnfdRewindability(prs, CEnfdRewindability::ErmSatisfy);
	CCTEReq *pcter = SPQOS_NEW(mp) CCTEReq(mp);
	CColRefSet *pcrsCopy = SPQOS_NEW(mp) CColRefSet(mp, *pcrs);
	CReqdPropPlan *prppIncompatibleDistribution =
		SPQOS_NEW(mp) CReqdPropPlan(pcrsCopy, peo, ped, per, pcter);
	CPartInfo *ppartinfo = SPQOS_NEW(mp) CPartInfo(mp);
	prppIncompatibleDistribution->InitReqdPartitionPropagation(mp, ppartinfo);

	// Test that the plan is not valid.
	SPQOS_ASSERT(
		!pexprPlan->FValidPlan(prppIncompatibleDistribution, pdpctxtplan));
	pdpctxtplan->Release();
	pexprPlan->Release();
	prppIncompatibleDistribution->Release();
	prpp->Release();
	pexprGby->Release();
	ppartinfo->Release();

	return SPQOS_OK;
}

//---------------------------------------------------------------------------
//	@function:
//		CExpressionTest::EresUnittest_FValidPlan_InvalidRewindability
//
//	@doc:
//		Test for CExpression::FValidPlan with incompatible rewindability properties
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CExpressionTest::EresUnittest_FValidPlan_InvalidRewindability()
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

	CExpression *pexprGby = NULL;
	CColRefSet *pcrs = NULL;
	CExpression *pexprPlan = NULL;
	CReqdPropPlan *prpp = NULL;

	SetupPlanForFValidPlanTest(mp, &pexprGby, &pcrs, &pexprPlan, &prpp);
	CDrvdPropCtxtPlan *pdpctxtplan = SPQOS_NEW(mp) CDrvdPropCtxtPlan(mp);

	COrderSpec *pos = SPQOS_NEW(mp) COrderSpec(mp);
	CDistributionSpec *pds = SPQOS_NEW(mp)
		CDistributionSpecSingleton(CDistributionSpecSingleton::EstMaster);
	CRewindabilitySpec *prs = SPQOS_NEW(mp) CRewindabilitySpec(
		CRewindabilitySpec::ErtRewindable, CRewindabilitySpec::EmhtNoMotion);
	CEnfdOrder *peo = SPQOS_NEW(mp) CEnfdOrder(pos, CEnfdOrder::EomSatisfy);
	CEnfdDistribution *ped =
		SPQOS_NEW(mp) CEnfdDistribution(pds, CEnfdDistribution::EdmExact);
	CEnfdRewindability *per =
		SPQOS_NEW(mp) CEnfdRewindability(prs, CEnfdRewindability::ErmSatisfy);
	CCTEReq *pcter = SPQOS_NEW(mp) CCTEReq(mp);
	CColRefSet *pcrsCopy = SPQOS_NEW(mp) CColRefSet(mp, *pcrs);
	CReqdPropPlan *prppIncompatibleRewindability =
		SPQOS_NEW(mp) CReqdPropPlan(pcrsCopy, peo, ped, per, pcter);
	CPartInfo *ppartinfo = SPQOS_NEW(mp) CPartInfo(mp);
	prppIncompatibleRewindability->InitReqdPartitionPropagation(mp, ppartinfo);

	// Test that the plan is not valid.
	SPQOS_ASSERT(
		!pexprPlan->FValidPlan(prppIncompatibleRewindability, pdpctxtplan));
	pdpctxtplan->Release();
	pexprPlan->Release();
	prppIncompatibleRewindability->Release();
	prpp->Release();
	pexprGby->Release();
	ppartinfo->Release();

	return SPQOS_OK;
}

//---------------------------------------------------------------------------
//	@function:
//		CExpressionTest::EresUnittest_FValidPlan_InvalidCTEs
//
//	@doc:
//		Test for CExpression::FValidPlan with incompatible CTE properties
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CExpressionTest::EresUnittest_FValidPlan_InvalidCTEs()
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

	CExpression *pexprGby = NULL;
	CColRefSet *pcrs = NULL;
	CExpression *pexprPlan = NULL;
	CReqdPropPlan *prpp = NULL;

	SetupPlanForFValidPlanTest(mp, &pexprGby, &pcrs, &pexprPlan, &prpp);
	CDrvdPropCtxtPlan *pdpctxtplan = SPQOS_NEW(mp) CDrvdPropCtxtPlan(mp);

	COrderSpec *pos = SPQOS_NEW(mp) COrderSpec(mp);
	CDistributionSpec *pds = SPQOS_NEW(mp)
		CDistributionSpecSingleton(CDistributionSpecSingleton::EstMaster);
	CRewindabilitySpec *prs = SPQOS_NEW(mp) CRewindabilitySpec(
		CRewindabilitySpec::ErtNone, CRewindabilitySpec::EmhtNoMotion);
	CEnfdOrder *peo = SPQOS_NEW(mp) CEnfdOrder(pos, CEnfdOrder::EomSatisfy);
	CEnfdDistribution *ped =
		SPQOS_NEW(mp) CEnfdDistribution(pds, CEnfdDistribution::EdmExact);
	CEnfdRewindability *per =
		SPQOS_NEW(mp) CEnfdRewindability(prs, CEnfdRewindability::ErmSatisfy);
	CCTEReq *pcter = SPQOS_NEW(mp) CCTEReq(mp);
	ULONG ulCTEId = 0;

	CExpression *pexprProducer =
		CTestUtils::PexprLogicalCTEProducerOverSelect(mp, ulCTEId);
	CDrvdPropPlan *pdpplan = CDrvdPropPlan::Pdpplan(pexprPlan->PdpDerive());
	pdpplan->AddRef();
	pcter->Insert(ulCTEId, CCTEMap::EctProducer /*ect*/, true /*fRequired*/,
				  pdpplan);
	CColRefSet *pcrsCopy = SPQOS_NEW(mp) CColRefSet(mp, *pcrs);
	CReqdPropPlan *prppIncompatibleCTE =
		SPQOS_NEW(mp) CReqdPropPlan(pcrsCopy, peo, ped, per, pcter);
	CPartInfo *ppartinfo = SPQOS_NEW(mp) CPartInfo(mp);
	prppIncompatibleCTE->InitReqdPartitionPropagation(mp, ppartinfo);

	// Test that the plan is not valid.
	SPQOS_ASSERT(!pexprPlan->FValidPlan(prppIncompatibleCTE, pdpctxtplan));
	pdpctxtplan->Release();
	pexprPlan->Release();
	prppIncompatibleCTE->Release();
	prpp->Release();
	pexprGby->Release();
	ppartinfo->Release();
	pexprProducer->Release();

	return SPQOS_OK;
}


#ifdef SPQOS_DEBUG
//---------------------------------------------------------------------------
//	@function:
//		CExpressionTest::EresUnittest_FValidPlanError
//
//	@doc:
//		Tests that CExpression::FValidPlan fails with an assert exception in debug mode
//		for bad input.
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CExpressionTest::EresUnittest_FValidPlanError()
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
	const IMDType *pmdtype = mda.PtMDType<IMDTypeInt4>();

	SPQOS_RESULT eres = SPQOS_OK;
	// Test that in debug mode SPQOS_ASSERT fails for non-physical expressions.
	{
		CColRefSet *pcrsInvalid = SPQOS_NEW(mp) CColRefSet(mp);
		CColumnFactory *col_factory = COptCtxt::PoctxtFromTLS()->Pcf();
		CColRef *pcrComputed =
			col_factory->PcrCreate(pmdtype, default_type_modifier);
		pcrsInvalid->Include(pcrComputed);

		CReqdPropPlan *prpp = PrppCreateRequiredProperties(mp, pcrsInvalid);
		IDatum *datum = SPQOS_NEW(mp) spqnaucrates::CDatumInt8SPQDB(
			CTestUtils::m_sysidDefault, 1 /*val*/, false /*is_null*/);
		CExpression *pexpr =
			SPQOS_NEW(mp) CExpression(mp, SPQOS_NEW(mp) CScalarConst(mp, datum));

		CDrvdPropCtxtPlan *pdpctxtplan = SPQOS_NEW(mp) CDrvdPropCtxtPlan(mp);
		SPQOS_TRY
		{
			// FValidPlan should fail for expressions which are not physical.
			if (!pexpr->FValidPlan(prpp, pdpctxtplan))
			{
				eres = SPQOS_FAILED;
			}
			pdpctxtplan->Release();
		}
		SPQOS_CATCH_EX(ex)
		{
			pdpctxtplan->Release();
			if (!SPQOS_MATCH_EX(ex, CException::ExmaSystem,
							   CException::ExmiAssert))
			{
				SPQOS_RETHROW(ex);
			}
			else
			{
				SPQOS_RESET_EX;
			}
		}
		SPQOS_CATCH_END;

		pexpr->Release();
		prpp->Release();
	}

	return eres;
}
#endif	// SPQOS_DEBUG



//---------------------------------------------------------------------------
//	@function:
//		CExpressionTest::EresCheckCachedReqdCols
//
//	@doc:
//		Helper for checking cached required columns
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CExpressionTest::EresCheckCachedReqdCols(CMemoryPool *mp, CExpression *pexpr,
										 CReqdPropPlan *prppInput)
{
	if (pexpr->Pop()->FScalar())
	{
		// scalar operators have no required columns
		return SPQOS_OK;
	}

	SPQOS_CHECK_STACK_SIZE;
	SPQOS_ASSERT(NULL != mp);
	SPQOS_ASSERT(NULL != prppInput);

	CExpressionHandle exprhdl(mp);
	exprhdl.Attach(pexpr);

	// init required properties of expression
	exprhdl.InitReqdProps(prppInput);

	// create array of child derived properties
	CDrvdPropArray *pdrspqdp = SPQOS_NEW(mp) CDrvdPropArray(mp);

	SPQOS_RESULT eres = SPQOS_OK;
	const ULONG arity = pexpr->Arity();
	for (ULONG ul = 0; SPQOS_OK == eres && ul < arity; ul++)
	{
		CExpression *pexprChild = (*pexpr)[ul];
		if (pexprChild->Pop()->FScalar())
		{
			continue;
		}
		SPQOS_ASSERT(NULL != pexprChild->Prpp());

		// extract cached required columns of the n-th child
		CColRefSet *pcrsChildReqd = pexprChild->Prpp()->PcrsRequired();

		// compute required columns of the n-th child
		exprhdl.ComputeChildReqdCols(ul, pdrspqdp);

		// check if cached columns pointer is the same as computed columns pointer,
		// if this is not the case, then we have re-computed the same set of columns and test should fail
		if (pcrsChildReqd != exprhdl.Prpp(ul)->PcrsRequired())
		{
			CAutoTrace at(mp);
			at.Os() << "\nExpression: \n" << *pexprChild;
			at.Os() << "\nCached cols: " << pcrsChildReqd << " : "
					<< *pcrsChildReqd;
			at.Os() << "\nComputed cols: " << exprhdl.Prpp(ul)->PcrsRequired()
					<< " : " << *exprhdl.Prpp(ul)->PcrsRequired();

			eres = SPQOS_FAILED;
			continue;
		}

		// call the function recursively for child expression
		SPQOS_RESULT eres =
			EresCheckCachedReqdCols(mp, pexprChild, exprhdl.Prpp(ul));
		if (SPQOS_FAILED == eres)
		{
			eres = SPQOS_FAILED;
			continue;
		}

		// add plan props of current child to derived props array
		CDrvdProp *pdp = pexprChild->PdpDerive();
		pdp->AddRef();
		pdrspqdp->Append(pdp);
	}

	pdrspqdp->Release();

	return eres;
}


//---------------------------------------------------------------------------
//	@function:
//		CExpressionTest::EresComputeReqdCols
//
//	@doc:
//		Helper for testing required column computation
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CExpressionTest::EresComputeReqdCols(const CHAR *szFilePath)
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	// reset metadata cache
	CMDCache::Reset();

	// set up MD providers
	CMDProviderMemory *pmdp = SPQOS_NEW(mp) CMDProviderMemory(mp, szFilePath);
	SPQOS_CHECK_ABORT;

	SPQOS_RESULT eres = SPQOS_FAILED;
	{
		CAutoMDAccessor amda(mp, pmdp, CTestUtils::m_sysidDefault);
		CAutoOptCtxt aoc(mp, amda.Pmda(), NULL,
						 /* pceeval */ CTestUtils::GetCostModel(mp));

		// read query expression
		CExpression *pexpr = CTestUtils::PexprReadQuery(mp, szFilePath);

		// optimize query
		CEngine eng(mp);
		CQueryContext *pqc = CTestUtils::PqcGenerate(mp, pexpr);
		eng.Init(pqc, NULL /*search_stage_array*/);
		eng.Optimize();

		// extract plan and decorate it with required columns
		CExpression *pexprPlan = eng.PexprExtractPlan();
		(void) pexprPlan->PrppCompute(mp, pqc->Prpp());

		// attempt computing required columns again --
		// we make sure that we reuse cached required columns at each operator
		eres = EresCheckCachedReqdCols(mp, pexprPlan, pqc->Prpp());

		pexpr->Release();
		pexprPlan->Release();
		SPQOS_DELETE(pqc);
	}

	return eres;
}


//---------------------------------------------------------------------------
//	@function:
//		CExpressionTest::EresUnittest_ReqdCols
//
//	@doc:
//		Test for required columns computation
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CExpressionTest::EresUnittest_ReqdCols()
{
	const CHAR *rgszTests[] = {
		"../data/dxl/tpch/q1.mdp",
		"../data/dxl/tpch/q3.mdp",
		"../data/dxl/expressiontests/WinFunc-OuterRef-Partition-Query.xml",
		"../data/dxl/expressiontests/WinFunc-OuterRef-Partition-Order-Query.xml",
	};

	SPQOS_RESULT eres = SPQOS_OK;
	for (ULONG ul = 0; SPQOS_OK == eres && ul < SPQOS_ARRAY_SIZE(rgszTests); ul++)
	{
		const CHAR *szFilePath = rgszTests[ul];
		eres = EresComputeReqdCols(szFilePath);
	}

	return eres;
}

//---------------------------------------------------------------------------
//	@function:
//		CExpressionTest::EresUnittest_InvalidSetOp
//
//	@doc:
//		Test for invalid SetOp expression,
//		SetOp is expected to have no outer references in input columns,
//		when an outer reference needs to be fed to SetOp as input, we must
//		project it first and feed the projected column into SetOp
//
//		For example, this is an invalid SetOp expression since it consumes
//		an outer reference from inner child
//
//			+--CLogicalUnion [Output: "col0" (0)], [Input: ("col0" (0)), ("col6" (6))]
//			   |--CLogicalGet ("T1"), Columns: ["col0" (0)]
//			   +--CLogicalGet ("T2"), Columns: ["col3" (3)]
//
//
//		the valid expression should looks like this:
//
//			+--CLogicalUnion [Output: "col0" (0)], [Input: ("col0" (0)), ("col7" (7))]
//			   |--CLogicalGet ("T1"), Columns: ["col0" (0)]
//			   +--CLogicalProject
//			       |--CLogicalGet ("T2"), Columns: ["col3" (3)]
//			       +--CScalarProjectList
//			       		+--CScalarProjectElement  "col7" (7)
//			       		      +--CScalarIdent "col6" (6)
//
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CExpressionTest::EresUnittest_InvalidSetOp()
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	// Setup an MD cache with a file-based provider
	CMDProviderMemory *pmdp = CTestUtils::m_pmdpf;
	pmdp->AddRef();
	CMDAccessor mda(mp, CMDCache::Pcache(), CTestUtils::m_sysidDefault, pmdp);

	{
		CAutoOptCtxt aoc(mp, &mda, NULL /* pceeval */,
						 CTestUtils::GetCostModel(mp));

		// create two different Get expressions
		CWStringConst strName1(SPQOS_WSZ_LIT("T1"));
		CMDIdSPQDB *pmdid1 =
			SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidRel, SPQOPT_TEST_REL_OID1, 1, 1);
		CTableDescriptor *ptabdesc1 =
			CTestUtils::PtabdescCreate(mp, 3, pmdid1, CName(&strName1));
		CWStringConst strAlias1(SPQOS_WSZ_LIT("T1Alias"));
		CExpression *pexprGet1 =
			CTestUtils::PexprLogicalGet(mp, ptabdesc1, &strAlias1);
		CColRefSet *pcrsOutput1 = pexprGet1->DeriveOutputColumns();

		CWStringConst strName2(SPQOS_WSZ_LIT("T2"));
		CMDIdSPQDB *pmdid2 =
			SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidRel, SPQOPT_TEST_REL_OID2, 1, 1);
		CTableDescriptor *ptabdesc2 =
			CTestUtils::PtabdescCreate(mp, 3, pmdid2, CName(&strName2));
		CWStringConst strAlias2(SPQOS_WSZ_LIT("T2Alias"));
		CExpression *pexprGet2 =
			CTestUtils::PexprLogicalGet(mp, ptabdesc2, &strAlias2);

		// create output columns of SetOp from output columns of first Get expression
		CColRefArray *pdrspqcrOutput = SPQOS_NEW(mp) CColRefArray(mp);
		pdrspqcrOutput->Append(pcrsOutput1->PcrFirst());

		// create input columns of SetOp while including an outer reference in inner child
		CColRef2dArray *pdrspqdrspqcrInput = SPQOS_NEW(mp) CColRef2dArray(mp);

		CColRefArray *pdrspqcr1 = SPQOS_NEW(mp) CColRefArray(mp);
		pdrspqcr1->Append(pcrsOutput1->PcrFirst());
		pdrspqdrspqcrInput->Append(pdrspqcr1);

		CColRefArray *pdrspqcr2 = SPQOS_NEW(mp) CColRefArray(mp);
		CColRef *pcrOuterRef = COptCtxt::PoctxtFromTLS()->Pcf()->PcrCreate(
			pcrsOutput1->PcrFirst());
		pdrspqcr2->Append(pcrOuterRef);
		pdrspqdrspqcrInput->Append(pdrspqcr2);

		// create invalid SetOp expression
		CLogicalUnion *pop =
			SPQOS_NEW(mp) CLogicalUnion(mp, pdrspqcrOutput, pdrspqdrspqcrInput);
		CExpression *pexprSetOp =
			SPQOS_NEW(mp) CExpression(mp, pop, pexprGet1, pexprGet2);

		{
			CAutoTrace at(mp);
			at.Os() << "\nInvalid SetOp Expression: \n" << *pexprSetOp;
		}

		// deriving relational properties must fail
		(void) pexprSetOp->PdpDerive();

		pexprSetOp->Release();
	}

	return SPQOS_FAILED;
}

//---------------------------------------------------------------------------
//	@function:
//		CExpressionTest::PexprComplexJoinTree
//
//	@doc:
// 		Return an expression with several joins
//
//---------------------------------------------------------------------------
CExpression *
CExpressionTest::PexprComplexJoinTree(CMemoryPool *mp)
{
	// The plan will have this shape
	//
	//	+--CLogicalInnerJoin
	//	   |--CLogicalUnion
	//	   |  |--CLogicalLeftOuterJoin
	//	   |  |  |--CLogicalInnerJoin
	//	   |  |  |  |--CLogicalGet
	//	   |  |  |  +--CLogicalSelect
	//	   |  |  +--CLogicalInnerJoin
	//	   |  |     |--CLogicalGet
	//	   |  |     +--CLogicalDynamicGet
	//	   |  +--CLogicalLeftOuterJoin
	//	   |     |--CLogicalGet
	//	   |     +--CLogicalGet
	//	   +--CLogicalInnerJoin
	//	      |--CLogicalGet
	//	      +--CLogicalSelect

	CExpression *pexprInnerJoin1 =
		CTestUtils::PexprLogicalJoin<CLogicalInnerJoin>(
			mp, CTestUtils::PexprLogicalGet(mp),
			CTestUtils::PexprLogicalSelect(mp));
	CExpression *pexprJoinIndex =
		CTestUtils::PexprLogicalJoinWithPartitionedAndIndexedInnerChild(mp);
	CExpression *pexprLeftJoin =
		CTestUtils::PexprLogicalJoin<CLogicalLeftOuterJoin>(mp, pexprInnerJoin1,
															pexprJoinIndex);

	CExpression *pexprOuterJoin =
		CTestUtils::PexprLogicalJoin<CLogicalLeftOuterJoin>(mp);
	CExpression *pexprInnerJoin4 =
		CTestUtils::PexprLogicalJoin<CLogicalInnerJoin>(mp, pexprLeftJoin,
														pexprOuterJoin);

	CExpression *pexprInnerJoin5 =
		CTestUtils::PexprLogicalJoin<CLogicalInnerJoin>(
			mp, CTestUtils::PexprLogicalGet(mp),
			CTestUtils::PexprLogicalSelect(mp));
	CExpression *pexprTopJoin = CTestUtils::PexprLogicalJoin<CLogicalInnerJoin>(
		mp, pexprInnerJoin4, pexprInnerJoin5);

	return pexprTopJoin;
}

// EOF
