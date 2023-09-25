//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CContradictionTest.cpp
//
//	@doc:
//		Tests for contradiction detection
//---------------------------------------------------------------------------
#include "unittest/spqopt/operators/CContradictionTest.h"

#include "spqos/io/COstreamString.h"
#include "spqos/string/CWStringDynamic.h"

#include "spqopt/eval/CConstExprEvaluatorDefault.h"
#include "spqopt/operators/ops.h"
#include "naucrates/md/IMDScalarOp.h"

#include "unittest/base.h"
#include "unittest/spqopt/CTestUtils.h"



//---------------------------------------------------------------------------
//	@function:
//		CContradictionTest::EresUnittest
//
//	@doc:
//		Unittest for contradiction detection
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CContradictionTest::EresUnittest()
{
	CUnittest rgut[] = {
		SPQOS_UNITTEST_FUNC(CContradictionTest::EresUnittest_Constraint),
	};

	return CUnittest::EresExecute(rgut, SPQOS_ARRAY_SIZE(rgut));
}

//---------------------------------------------------------------------------
//	@function:
//		CContradictionTest::EresUnittest_Constraint
//
//	@doc:
//		Tests for constraint property derivation and constraint push down
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CContradictionTest::EresUnittest_Constraint()
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	// setup a file-based provider
	CMDProviderMemory *pmdp = CTestUtils::m_pmdpf;
	pmdp->AddRef();
	CMDAccessor mda(mp, CMDCache::Pcache(), CTestUtils::m_sysidDefault, pmdp);


	typedef CExpression *(*Pfpexpr)(CMemoryPool *);

	Pfpexpr rspqf[] = {
		CTestUtils::PexprLogicalApplyWithOuterRef<CLogicalInnerApply>,
		CTestUtils::PexprLogicalApply<CLogicalLeftSemiApply>,
		CTestUtils::PexprLogicalApply<CLogicalLeftAntiSemiApply>,
		CTestUtils::PexprLogicalApplyWithOuterRef<CLogicalLeftOuterApply>,
		CTestUtils::PexprLogicalGet,
		CTestUtils::PexprLogicalGetPartitioned,
		CTestUtils::PexprLogicalSelect,
		CTestUtils::PexprLogicalSelectCmpToConst,
		CTestUtils::PexprLogicalSelectPartitioned,
		CTestUtils::PexprLogicalSelectWithContradiction,
		CTestUtils::PexprLogicalJoin<CLogicalInnerJoin>,
		CTestUtils::PexprLogicalJoin<CLogicalLeftOuterJoin>,
		CTestUtils::PexprLogicalJoin<CLogicalLeftSemiJoin>,
		CTestUtils::PexprLogicalJoin<CLogicalLeftAntiSemiJoin>,
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
	};

	for (ULONG i = 0; i < SPQOS_ARRAY_SIZE(rspqf); i++)
	{
		// install opt context in TLS
		CAutoOptCtxt aoc(mp, &mda, NULL, /* pceeval */
						 CTestUtils::GetCostModel(mp));

		// generate simple expression
		CExpression *pexpr = rspqf[i](mp);

		// self-match
		SPQOS_ASSERT(pexpr->FMatchDebug(pexpr));

		// debug print
		CWStringDynamic str(mp, SPQOS_WSZ_LIT("\n"));
		COstreamString oss(&str);

		oss << "EXPR:" << std::endl << *pexpr << std::endl;
		SPQOS_TRACE(str.GetBuffer());
		str.Reset();


#ifdef SPQOS_DEBUG
		// derive properties on expression
		(void) pexpr->PdpDerive();

		oss << std::endl << "DERIVED PROPS:" << std::endl;
		SPQOS_TRACE(str.GetBuffer());
		str.Reset();
		pexpr->DbgPrint();
#endif	// SPQOS_DEBUG

		CExpression *pexprPreprocessed =
			CExpressionPreprocessor::PexprPreprocess(mp, pexpr);
		oss << std::endl
			<< "PREPROCESSED EXPR:" << std::endl
			<< *pexprPreprocessed << std::endl;
		SPQOS_TRACE(str.GetBuffer());
		str.Reset();

		// cleanup
		pexprPreprocessed->Release();
		pexpr->Release();
	}

	return SPQOS_OK;
}

// EOF
