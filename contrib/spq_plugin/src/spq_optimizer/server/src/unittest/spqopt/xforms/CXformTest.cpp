//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		CXformTest.cpp
//
//	@doc:
//		Test for CXForm
//---------------------------------------------------------------------------
#include "unittest/spqopt/xforms/CXformTest.h"

#include "spqos/io/COstreamString.h"
#include "spqos/string/CWStringDynamic.h"

#include "spqopt/base/CQueryContext.h"
#include "spqopt/eval/CConstExprEvaluatorDefault.h"
#include "spqopt/operators/ops.h"
#include "spqopt/xforms/CXform.h"
#include "spqopt/xforms/xforms.h"

#include "unittest/base.h"
#include "unittest/spqopt/CTestUtils.h"
#include "unittest/spqopt/translate/CTranslatorExprToDXLTest.h"
#include "unittest/spqopt/xforms/CDecorrelatorTest.h"


//---------------------------------------------------------------------------
//	@function:
//		CXformTest::EresUnittest
//
//	@doc:
//		Unittest driver
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CXformTest::EresUnittest()
{
	CUnittest rgut[] = {
		SPQOS_UNITTEST_FUNC(CXformTest::EresUnittest_ApplyXforms),
		SPQOS_UNITTEST_FUNC(CXformTest::EresUnittest_ApplyXforms_CTE),
#ifdef SPQOS_DEBUG
		SPQOS_UNITTEST_FUNC(CXformTest::EresUnittest_Mapping),
#endif	// SPQOS_DEBUG
	};

	return CUnittest::EresExecute(rgut, SPQOS_ARRAY_SIZE(rgut));
}


//---------------------------------------------------------------------------
//	@function:
//		CXformTest::EresUnittest_ApplyXforms
//
//	@doc:
//		Test application of different xforms
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CXformTest::EresUnittest_ApplyXforms()
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	typedef CExpression *(*Pfpexpr)(CMemoryPool *);
	Pfpexpr rspqf[] = {
		CTestUtils::PexprLogicalApplyWithOuterRef<CLogicalInnerApply>,
		CTestUtils::PexprLogicalApply<CLogicalLeftSemiApply>,
		CTestUtils::PexprLogicalApply<CLogicalLeftAntiSemiApply>,
		CTestUtils::PexprLogicalApply<CLogicalLeftAntiSemiApplyNotIn>,
		CTestUtils::PexprLogicalApplyWithOuterRef<CLogicalLeftOuterApply>,
		CTestUtils::PexprLogicalGet,
		CTestUtils::PexprLogicalExternalGet,
		CTestUtils::PexprLogicalSelect,
		CTestUtils::PexprLogicalLimit,
		CTestUtils::PexprLogicalJoin<CLogicalInnerJoin>,
		CTestUtils::PexprLogicalJoin<CLogicalLeftOuterJoin>,
		CTestUtils::PexprLogicalJoin<CLogicalLeftSemiJoin>,
		CTestUtils::PexprLogicalJoin<CLogicalLeftAntiSemiJoin>,
		CTestUtils::PexprLogicalJoin<CLogicalLeftAntiSemiJoinNotIn>,
		CTestUtils::PexprLogicalGbAgg,
		CTestUtils::PexprLogicalGbAggOverJoin,
		CTestUtils::PexprLogicalGbAggWithSum,
		CTestUtils::PexprLogicalGbAggDedupOverInnerJoin,
		CTestUtils::PexprLogicalNAryJoin,
		CTestUtils::PexprLeftOuterJoinOnNAryJoin,
		CTestUtils::PexprLogicalProject,
		CTestUtils::PexprLogicalSequence,
		CTestUtils::PexprLogicalGetPartitioned,
		CTestUtils::PexprLogicalSelectPartitioned,
		CTestUtils::PexprLogicalDynamicGet,
		CTestUtils::PexprJoinPartitionedInner<CLogicalInnerJoin>,
		CTestUtils::PexprLogicalSelectCmpToConst,
		CTestUtils::PexprLogicalTVFTwoArgs,
		CTestUtils::PexprLogicalTVFNoArgs,
		CTestUtils::PexprLogicalInsert,
		CTestUtils::PexprLogicalDelete,
		CTestUtils::PexprLogicalUpdate,
		CTestUtils::PexprLogicalAssert,
		CTestUtils::PexprLogicalJoin<CLogicalFullOuterJoin>,
		PexprJoinTree,
		CTestUtils::PexprLogicalJoinWithPartitionedAndIndexedInnerChild,
	};

	// setup a file-based provider
	CMDProviderMemory *pmdp = CTestUtils::m_pmdpf;
	pmdp->AddRef();
	CMDAccessor mda(mp, CMDCache::Pcache(), CTestUtils::m_sysidDefault, pmdp);

	// install opt context in TLS
	CAutoOptCtxt aoc(mp, &mda, NULL, /* pceeval */
					 CTestUtils::GetCostModel(mp));

	for (ULONG ul = 0; ul < SPQOS_ARRAY_SIZE(rspqf); ul++)
	{
		CWStringDynamic str(mp);
		COstreamString oss(&str);

		// generate simple expression
		CExpression *pexpr = rspqf[ul](mp);
		ApplyExprXforms(mp, oss, pexpr);

		SPQOS_TRACE(str.GetBuffer());
		pexpr->Release();
	}

	return SPQOS_OK;
}

//---------------------------------------------------------------------------
//	@function:
//		CXformTest::EresUnittest_ApplyXforms_CTE
//
//	@doc:
//		Test application of CTE-related xforms
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CXformTest::EresUnittest_ApplyXforms_CTE()
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

	CExpressionArray *pdrspqexpr = SPQOS_NEW(mp) CExpressionArray(mp);

	// create producer
	ULONG ulCTEId = 0;
	CExpression *pexprProducer =
		CTestUtils::PexprLogicalCTEProducerOverSelect(mp, ulCTEId);
	COptCtxt::PoctxtFromTLS()->Pcteinfo()->AddCTEProducer(pexprProducer);

	pdrspqexpr->Append(pexprProducer);

	CColRefArray *pdrspqcrProducer =
		CLogicalCTEProducer::PopConvert(pexprProducer->Pop())->Pdrspqcr();
	CColRefArray *pdrspqcrConsumer = CUtils::PdrspqcrCopy(mp, pdrspqcrProducer);

	CExpression *pexprConsumer = SPQOS_NEW(mp) CExpression(
		mp, SPQOS_NEW(mp) CLogicalCTEConsumer(mp, ulCTEId, pdrspqcrConsumer));

	pdrspqexpr->Append(pexprConsumer);
	COptCtxt::PoctxtFromTLS()->Pcteinfo()->IncrementConsumers(ulCTEId);

	pexprConsumer->AddRef();
	CExpression *pexprSelect =
		CTestUtils::PexprLogicalSelect(mp, pexprConsumer);
	pdrspqexpr->Append(pexprSelect);

	pexprSelect->AddRef();
	CExpression *pexprAnchor = SPQOS_NEW(mp) CExpression(
		mp, SPQOS_NEW(mp) CLogicalCTEAnchor(mp, ulCTEId), pexprSelect);

	pdrspqexpr->Append(pexprAnchor);

	const ULONG length = pdrspqexpr->Size();
	for (ULONG ul = 0; ul < length; ul++)
	{
		CWStringDynamic str(mp);
		COstreamString oss(&str);

		ApplyExprXforms(mp, oss, (*pdrspqexpr)[ul]);

		SPQOS_TRACE(str.GetBuffer());
	}
	pdrspqexpr->Release();

	return SPQOS_OK;
}

//---------------------------------------------------------------------------
//	@function:
//		CXformTest::ApplyExprXforms
//
//	@doc:
//		Apply different xforms for the given expression
//
//---------------------------------------------------------------------------
void
CXformTest::ApplyExprXforms(CMemoryPool *mp, IOstream &os, CExpression *pexpr)
{
	os << std::endl << "EXPR:" << std::endl;
	(void) pexpr->OsPrint(os);

	for (ULONG ulXformId = 0; ulXformId < CXform::ExfSentinel; ulXformId++)
	{
		if (CXformFactory::Pxff()->IsXformIdUsed((CXform::EXformId) ulXformId))
		{
			CXform *pxform =
				CXformFactory::Pxff()->Pxf((CXform::EXformId) ulXformId);
			os << std::endl << "XFORM " << pxform->SzId() << ":" << std::endl;

			CXformContext *pxfctxt = SPQOS_NEW(mp) CXformContext(mp);
			CXformResult *pxfres = SPQOS_NEW(mp) CXformResult(mp);

#ifdef SPQOS_DEBUG
			if (pxform->FCheckPattern(pexpr) &&
				CXform::FPromising(mp, pxform, pexpr))
			{
				if (CXform::ExfExpandNAryJoinMinCard == pxform->Exfid())
				{
					SPQOS_ASSERT(COperator::EopLogicalNAryJoin ==
								pexpr->Pop()->Eopid());

					// derive stats on NAry join expression
					CExpressionHandle exprhdl(mp);
					exprhdl.Attach(pexpr);
					exprhdl.DeriveStats(mp, mp, NULL /*prprel*/,
										NULL /*stats_ctxt*/);
				}

				pxform->Transform(pxfctxt, pxfres, pexpr);

				CExpression *pexprResult = pxfres->PexprNext();
				while (NULL != pexprResult)
				{
					SPQOS_ASSERT(pexprResult->FMatchDebug(pexprResult));

					pexprResult = pxfres->PexprNext();
				}
				(void) pxfres->OsPrint(os);
			}
#endif	// SPQOS_DEBUG

			pxfres->Release();
			pxfctxt->Release();
		}
	}
}

//---------------------------------------------------------------------------
//	@function:
//		CXformTest::PexprStarJoinTree
//
//	@doc:
//		Generate a randomized star join tree
//
//---------------------------------------------------------------------------
CExpression *
CXformTest::PexprStarJoinTree(CMemoryPool *mp, ULONG ulTabs)
{
	CExpression *pexprLeft = CTestUtils::PexprLogicalGet(mp);

	for (ULONG ul = 1; ul < ulTabs; ul++)
	{
		CColRef *pcrLeft = pexprLeft->DeriveOutputColumns()->PcrAny();
		CExpression *pexprRight = CTestUtils::PexprLogicalGet(mp);
		CColRef *pcrRight = pexprRight->DeriveOutputColumns()->PcrAny();

		CExpression *pexprPred =
			CUtils::PexprScalarEqCmp(mp, pcrLeft, pcrRight);

		pexprLeft = CUtils::PexprLogicalJoin<CLogicalInnerJoin>(
			mp, pexprLeft, pexprRight, pexprPred);
	}

	return pexprLeft;
}


//---------------------------------------------------------------------------
//	@function:
//		CXformTest::PexprJoinTree
//
//	@doc:
//		Generate a randomized star join tree
//
//---------------------------------------------------------------------------
CExpression *
CXformTest::PexprJoinTree(CMemoryPool *mp)
{
	return PexprStarJoinTree(mp, 3);
}

#ifdef SPQOS_DEBUG
//---------------------------------------------------------------------------
//	@function:
//		CXformTest::EresUnittest_Mapping
//
//	@doc:
//		Test name -> xform mapping
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CXformTest::EresUnittest_Mapping()
{
	for (ULONG ul = 0; ul < CXform::ExfSentinel; ul++)
	{
		if (CXformFactory::Pxff()->IsXformIdUsed((CXform::EXformId) ul))
		{
			CXform::EXformId exfid = (CXform::EXformId) ul;
			CXform *pxform = CXformFactory::Pxff()->Pxf(exfid);
			CXform *pxformMapped = CXformFactory::Pxff()->Pxf(pxform->SzId());
			SPQOS_ASSERT(pxform == pxformMapped);
		}
	}

	return SPQOS_OK;
}
#endif	// SPQOS_DEBUG


// EOF
