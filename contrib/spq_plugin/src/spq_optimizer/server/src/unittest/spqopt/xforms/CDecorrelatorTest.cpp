//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CDecorrelatorTest.cpp
//
//	@doc:
//		Test for decorrelation
//---------------------------------------------------------------------------
#include "unittest/spqopt/xforms/CDecorrelatorTest.h"

#include "spqos/io/COstreamString.h"
#include "spqos/string/CWStringDynamic.h"

#include "spqopt/base/CQueryContext.h"
#include "spqopt/base/CUtils.h"
#include "spqopt/eval/CConstExprEvaluatorDefault.h"
#include "spqopt/operators/CPredicateUtils.h"
#include "spqopt/operators/ops.h"
#include "spqopt/xforms/CDecorrelator.h"

#include "unittest/base.h"
#include "unittest/spqopt/CTestUtils.h"


//---------------------------------------------------------------------------
//	@function:
//		CDecorrelatorTest::EresUnittest
//
//	@doc:
//		Unittest for predicate utilities
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CDecorrelatorTest::EresUnittest()
{
	CUnittest rgut[] = {
		SPQOS_UNITTEST_FUNC(CDecorrelatorTest::EresUnittest_Decorrelate),
	};

	return CUnittest::EresExecute(rgut, SPQOS_ARRAY_SIZE(rgut));
}


//---------------------------------------------------------------------------
//	@function:
//		CDecorrelatorTest::EresUnittest_Decorrelate
//
//	@doc:
//		Driver for decorrelation tests
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CDecorrelatorTest::EresUnittest_Decorrelate()
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	// setup a file-based provider
	CMDProviderMemory *pmdp = CTestUtils::m_pmdpf;
	pmdp->AddRef();
	CMDAccessor mda(mp, CMDCache::Pcache(), CTestUtils::m_sysidDefault, pmdp);

	// test cases
	typedef CExpression *(*Pfpexpr)(CMemoryPool *);
	Pfpexpr rspqf[] = {CTestUtils::PexprLogicalGbAggCorrelated,
					  CTestUtils::PexprLogicalSelectCorrelated,
					  CTestUtils::PexprLogicalJoinCorrelated,
					  CTestUtils::PexprLogicalProjectGbAggCorrelated};

	for (ULONG ulCase = 0; ulCase < SPQOS_ARRAY_SIZE(rspqf); ulCase++)
	{
		// install opt context in TLS
		CAutoOptCtxt aoc(mp, &mda, NULL, /* pceeval */
						 CTestUtils::GetCostModel(mp));

		// generate expression
		CExpression *pexpr = rspqf[ulCase](mp);

		CWStringDynamic str(mp);
		COstreamString oss(&str);
		oss << std::endl << "INPUT:" << std::endl << *pexpr << std::endl;
		SPQOS_TRACE(str.GetBuffer());
		str.Reset();

		CExpression *pexprResult = NULL;
		CExpressionArray *pdrspqexpr = SPQOS_NEW(mp) CExpressionArray(mp);
		CColRefSet *outerRefs = pexpr->DeriveOuterReferences();
#ifdef SPQOS_DEBUG
		BOOL fSuccess =
#endif	// SPQOS_DEBUG
			CDecorrelator::FProcess(mp, pexpr, false /*fEqualityOnly*/,
									&pexprResult, pdrspqexpr, outerRefs);
		SPQOS_ASSERT(fSuccess);

		// convert residuals into one single conjunct
		CExpression *pexprResidual =
			CPredicateUtils::PexprConjunction(mp, pdrspqexpr);

		oss << std::endl
			<< "RESIDUAL RELATIONAL:" << std::endl
			<< *pexprResult << std::endl;
		oss << std::endl
			<< "RESIDUAL SCALAR:" << std::endl
			<< *pexprResidual << std::endl;

		SPQOS_TRACE(str.GetBuffer());
		str.Reset();

		pexprResult->Release();
		pexprResidual->Release();
		pexpr->Release();
	}

	return SPQOS_OK;
}

// EOF
