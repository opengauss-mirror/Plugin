//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal, Inc.
//
//	@filename:
//		CConstExprEvaluatorDXLTest.cpp
//
//	@doc:
//		Unit tests for CConstExprEvaluatorDXL
//
//	@owner:
//
//
//	@test:
//
//---------------------------------------------------------------------------

#include "unittest/spqopt/eval/CConstExprEvaluatorDXLTest.h"

#include "spqopt/base/CAutoOptCtxt.h"
#include "spqopt/base/CUtils.h"
#include "spqopt/eval/CConstExprEvaluatorDXL.h"
#include "spqopt/exception.h"
#include "spqopt/mdcache/CMDAccessor.h"
#include "spqopt/operators/CExpression.h"
#include "naucrates/dxl/operators/CDXLDatumInt4.h"
#include "naucrates/dxl/operators/CDXLNode.h"
#include "naucrates/dxl/operators/CDXLScalarConstValue.h"

#include "unittest/base.h"
#include "unittest/spqopt/CTestUtils.h"

using namespace spqdxl;
using namespace spqnaucrates;
using namespace spqopt;

// Value  which the dummy constant evaluator should produce.
const INT CConstExprEvaluatorDXLTest::m_iDefaultEvalValue = 300;

//---------------------------------------------------------------------------
//	@function:
//		CConstExprEvaluatorDXLTest::CDummyConstDXLNodeEvaluator::EvaluateExpr
//
//	@doc:
//		Evaluate the given DXL node representing an expression and returns a dummy
//		value as DXL. Caller must release it.
//
//---------------------------------------------------------------------------
spqdxl::CDXLNode *
CConstExprEvaluatorDXLTest::CDummyConstDXLNodeEvaluator::EvaluateExpr(
	const spqdxl::CDXLNode * /*pdxlnExpr*/
)
{
	const IMDTypeInt4 *pmdtypeint4 = m_pmda->PtMDType<IMDTypeInt4>();
	pmdtypeint4->MDId()->AddRef();

	CDXLDatumInt4 *dxl_datum = SPQOS_NEW(m_mp) CDXLDatumInt4(
		m_mp, pmdtypeint4->MDId(), false /*is_const_null*/, m_val);
	CDXLScalarConstValue *pdxlnConst =
		SPQOS_NEW(m_mp) CDXLScalarConstValue(m_mp, dxl_datum);

	return SPQOS_NEW(m_mp) CDXLNode(m_mp, pdxlnConst);
}

//---------------------------------------------------------------------------
//	@function:
//		CConstExprEvaluatorDXLTest::EresUnittest
//
//	@doc:
//		Executes all unit tests for CConstExprEvaluatorDXL
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CConstExprEvaluatorDXLTest::EresUnittest()
{
	{
		CUnittest rgut[] = {
			SPQOS_UNITTEST_FUNC_THROW(
				CConstExprEvaluatorDXLTest::EresUnittest_NonScalar,
				spqdxl::ExmaSPQOPT, spqdxl::ExmiEvalUnsupportedScalarExpr),
			SPQOS_UNITTEST_FUNC_THROW(
				CConstExprEvaluatorDXLTest::EresUnittest_NestedSubquery,
				spqdxl::ExmaSPQOPT, spqdxl::ExmiEvalUnsupportedScalarExpr),
			SPQOS_UNITTEST_FUNC_THROW(CConstExprEvaluatorDXLTest::
										 EresUnittest_ScalarContainingVariables,
									 spqdxl::ExmaSPQOPT,
									 spqdxl::ExmiEvalUnsupportedScalarExpr),
		};

		return CUnittest::EresExecute(rgut, SPQOS_ARRAY_SIZE(rgut));
	}
}

//---------------------------------------------------------------------------
//	@function:
//		CConstExprEvaluatorDXLTest::EresUnittest_NonScalar
//
//	@doc:
//		Test that evaluation fails for a non scalar input.
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CConstExprEvaluatorDXLTest::EresUnittest_NonScalar()
{
	CTestUtils::CTestSetup testsetup;
	CMemoryPool *mp = testsetup.Pmp();
	CDummyConstDXLNodeEvaluator consteval(mp, testsetup.Pmda(),
										  m_iDefaultEvalValue);
	CConstExprEvaluatorDXL *pceeval =
		SPQOS_NEW(mp) CConstExprEvaluatorDXL(mp, testsetup.Pmda(), &consteval);

	CExpression *pexprGet = CTestUtils::PexprLogicalGet(testsetup.Pmp());

	// this call should raise an exception
	CExpression *pexprResult = pceeval->PexprEval(pexprGet);
	CRefCount::SafeRelease(pexprResult);
	pexprGet->Release();
	pceeval->Release();

	return SPQOS_OK;
}

//---------------------------------------------------------------------------
//	@function:
//		CConstExprEvaluatorDXLTest::EresUnittest_NestedSubquery
//
//	@doc:
//		Test that an error is raised for a scalar with a nested subquery.
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CConstExprEvaluatorDXLTest::EresUnittest_NestedSubquery()
{
	CTestUtils::CTestSetup testsetup;
	CMemoryPool *mp = testsetup.Pmp();
	CDummyConstDXLNodeEvaluator consteval(mp, testsetup.Pmda(),
										  m_iDefaultEvalValue);
	CConstExprEvaluatorDXL *pceeval =
		SPQOS_NEW(mp) CConstExprEvaluatorDXL(mp, testsetup.Pmda(), &consteval);

	CExpression *pexprSelect =
		CTestUtils::PexprLogicalSelectWithConstAnySubquery(testsetup.Pmp());
	CExpression *pexprPredicate = (*pexprSelect)[1];
	SPQOS_ASSERT(COperator::EopScalarSubqueryAny ==
				pexprPredicate->Pop()->Eopid());

	// this call should raise an exception
	CExpression *pexprResult = pceeval->PexprEval(pexprPredicate);
	CRefCount::SafeRelease(pexprResult);
	pexprSelect->Release();
	pceeval->Release();

	return SPQOS_OK;
}

//---------------------------------------------------------------------------
//	@function:
//		CConstExprEvaluatorDXLTest::EresUnittest_ScalarContainingVariables
//
//	@doc:
//		Test that an error is raised for a scalar containing variables.
//
//---------------------------------------------------------------------------
SPQOS_RESULT
CConstExprEvaluatorDXLTest::EresUnittest_ScalarContainingVariables()
{
	CTestUtils::CTestSetup testsetup;
	CMemoryPool *mp = testsetup.Pmp();
	CDummyConstDXLNodeEvaluator consteval(mp, testsetup.Pmda(),
										  m_iDefaultEvalValue);
	CConstExprEvaluatorDXL *pceeval =
		SPQOS_NEW(mp) CConstExprEvaluatorDXL(mp, testsetup.Pmda(), &consteval);

	const IMDTypeInt4 *pmdtypeint4 = testsetup.Pmda()->PtMDType<IMDTypeInt4>();
	CColumnFactory *col_factory = COptCtxt::PoctxtFromTLS()->Pcf();
	CColRef *pcrComputed =
		col_factory->PcrCreate(pmdtypeint4, default_type_modifier);

	// create a comparison, where one of the children is a variable
	CExpression *pexprFunCall = CUtils::PexprScalarEqCmp(
		testsetup.Pmp(),
		CUtils::PexprScalarConstInt4(testsetup.Pmp(), 200 /*val*/),
		CUtils::PexprScalarIdent(testsetup.Pmp(), pcrComputed));

	// this call should raise an exception
	CExpression *pexprResult = pceeval->PexprEval(pexprFunCall);
	CRefCount::SafeRelease(pexprResult);
	pexprFunCall->Release();
	pceeval->Release();

	return SPQOS_OK;
}

// EOF
