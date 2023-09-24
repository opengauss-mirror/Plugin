//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal, Inc.
//
//	@filename:
//		CConstExprEvaluatorDXL.cpp
//
//	@doc:
//		Constant expression evaluator implementation that delegats to a DXL evaluator
//
//	@owner:
//
//
//	@test:
//
//---------------------------------------------------------------------------

#include "spqopt/eval/CConstExprEvaluatorDXL.h"

#include "spqopt/base/CDrvdPropScalar.h"
#include "spqopt/eval/IConstDXLNodeEvaluator.h"
#include "spqopt/exception.h"
#include "spqopt/mdcache/CMDAccessor.h"
#include "spqopt/operators/CExpression.h"

using namespace spqdxl;
using namespace spqmd;
using namespace spqopt;
using namespace spqos;


//---------------------------------------------------------------------------
//	@function:
//		CConstExprEvaluatorDXL::CConstExprEvaluatorDXL
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CConstExprEvaluatorDXL::CConstExprEvaluatorDXL(
	CMemoryPool *mp, CMDAccessor *md_accessor,
	IConstDXLNodeEvaluator *pconstdxleval)
	: m_pconstdxleval(pconstdxleval),
	  m_trexpr2dxl(mp, md_accessor, NULL /*pdrspqiSegments*/,
				   false /*fInitColumnFactory*/),
	  m_trdxl2expr(mp, md_accessor, false /*fInitColumnFactory*/)
{
}

//---------------------------------------------------------------------------
//	@function:
//		CConstExprEvaluatorDXL::~CConstExprEvaluatorDXL
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CConstExprEvaluatorDXL::~CConstExprEvaluatorDXL()
{
}

//---------------------------------------------------------------------------
//	@function:
//		CConstExprEvaluatorDXL::PexprEval
//
//	@doc:
//		Evaluate the given expression and return the result as a new expression.
//		Caller takes ownership of returned expression
//
//---------------------------------------------------------------------------
CExpression *
CConstExprEvaluatorDXL::PexprEval(CExpression *pexpr)
{
	SPQOS_ASSERT(NULL != pexpr);

	if (!CPredicateUtils::FCompareConstToConstIgnoreCast(pexpr))
	{
		SPQOS_RAISE(spqopt::ExmaSPQOPT, spqopt::ExmiEvalUnsupportedScalarExpr);
	}
	CDXLNode *pdxlnExpr = m_trexpr2dxl.PdxlnScalar(pexpr);
	CDXLNode *pdxlnResult = m_pconstdxleval->EvaluateExpr(pdxlnExpr);

	SPQOS_ASSERT(EdxloptypeScalar ==
				pdxlnResult->GetOperator()->GetDXLOperatorType());

	CExpression *pexprResult =
		m_trdxl2expr.PexprTranslateScalar(pdxlnResult, NULL /*colref_array*/);
	pdxlnResult->Release();
	pdxlnExpr->Release();

	return pexprResult;
}

//---------------------------------------------------------------------------
//	@function:
//		CConstExprEvaluatorDXL::FCanEvalExpressions
//
//	@doc:
//		Returns true, since this evaluator always attempts to evaluate the expression and compute a datum
//
//---------------------------------------------------------------------------
BOOL
CConstExprEvaluatorDXL::FCanEvalExpressions()
{
	return m_pconstdxleval->FCanEvalExpressions();
}



// EOF
