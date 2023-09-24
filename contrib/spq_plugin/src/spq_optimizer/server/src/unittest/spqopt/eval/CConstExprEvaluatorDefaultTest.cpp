//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal, Inc.
//
//	@filename:
//		CConstExprEvaluatorDefaultTest.cpp
//
//	@doc:
//		Unit tests for CConstExprEvaluatorDefault
//
//	@owner:
//
//
//	@test:
//
//---------------------------------------------------------------------------

#include "unittest/spqopt/eval/CConstExprEvaluatorDefaultTest.h"

#include "spqopt/base/CAutoOptCtxt.h"
#include "spqopt/base/CUtils.h"
#include "spqopt/eval/CConstExprEvaluatorDefault.h"
#include "spqopt/mdcache/CMDAccessor.h"
#include "spqopt/operators/ops.h"
#include "naucrates/md/CMDProviderMemory.h"

#include "unittest/base.h"
#include "unittest/spqopt/CTestUtils.h"

using namespace spqos;
using namespace spqopt;

//---------------------------------------------------------------------------
//	@function:
//		CConstExprEvaluatorDefaultTest::EresUnittest
//
//	@doc:
//		Executes all unit tests for CConstExprEvaluatorDefault
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CConstExprEvaluatorDefaultTest::EresUnittest()
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	CConstExprEvaluatorDefault *pceevaldefault =
		SPQOS_NEW(mp) CConstExprEvaluatorDefault();
	SPQOS_ASSERT(!pceevaldefault->FCanEvalExpressions());

	// setup a file-based provider
	CMDProviderMemory *pmdp = CTestUtils::m_pmdpf;
	pmdp->AddRef();
	CMDAccessor mda(mp, CMDCache::Pcache(), CTestUtils::m_sysidDefault, pmdp);

	// install opt context in TLS
	CAutoOptCtxt aoc(mp, &mda, NULL, /* pceeval */
					 CTestUtils::GetCostModel(mp));

	// Test evaluation of an integer constant
	{
		ULONG ulVal = 123456;
		CExpression *pexprUl = CUtils::PexprScalarConstInt4(mp, ulVal);
#ifdef SPQOS_DEBUG
		CExpression *pexprUlResult = pceevaldefault->PexprEval(pexprUl);
		CScalarConst *pscalarconstUl = CScalarConst::PopConvert(pexprUl->Pop());
		CScalarConst *pscalarconstUlResult =
			CScalarConst::PopConvert(pexprUlResult->Pop());
		SPQOS_ASSERT(pscalarconstUl->Matches(pscalarconstUlResult));
		pexprUlResult->Release();
#endif	// SPQOS_DEBUG
		pexprUl->Release();
	}

	// Test evaluation of a null test expression
	{
		ULONG ulVal = 123456;
		CExpression *pexprUl = CUtils::PexprScalarConstInt4(mp, ulVal);
		CExpression *pexprIsNull = CUtils::PexprIsNull(mp, pexprUl);
#ifdef SPQOS_DEBUG
		CExpression *pexprResult = pceevaldefault->PexprEval(pexprIsNull);
		CScalarNullTest *pscalarnulltest =
			CScalarNullTest::PopConvert(pexprIsNull->Pop());
		SPQOS_ASSERT(pscalarnulltest->Matches(pexprResult->Pop()));
		pexprResult->Release();
#endif	// SPQOS_DEBUG
		pexprIsNull->Release();
	}
	pceevaldefault->Release();

	return SPQOS_OK;
}

// EOF
