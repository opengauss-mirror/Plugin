//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal, Inc.
//
//	@filename:
//		CConstExprEvaluatorProxy.cpp
//
//	@doc:
//		Wrapper over SPQDB's expression evaluator that takes a constant expression,
//		given as DXL, evaluates it and returns the result as DXL. In case the expression
//		has variables, an exception is raised
//
//	@test:
//
//---------------------------------------------------------------------------

#include "postgres.h"

#include "executor/executor.h"
#include "spq_optimizer_util/spq_wrappers.h"
#include "spq_optimizer_util/translate/CTranslatorScalarToDXL.h"
#include "spq_optimizer_util/utils/CConstExprEvaluatorProxy.h"
#include "naucrates/dxl/operators/CDXLNode.h"
#include "naucrates/exception.h"

using namespace spqdxl;
using namespace spqmd;
using namespace spqos;


//---------------------------------------------------------------------------
//	@function:
//		CConstExprEvaluatorProxy::EmptyMappingColIdVar::PvarFromDXLNodeScId
//
//	@doc:
//		Raises an exception in case someone looks up a variable
//
//---------------------------------------------------------------------------
Var *
CConstExprEvaluatorProxy::CEmptyMappingColIdVar::VarFromDXLNodeScId(
	const CDXLScalarIdent * /*scalar_ident*/
)
{
	elog(LOG,
		 "Expression passed to CConstExprEvaluatorProxy contains variables. "
		 "Evaluation will fail and an exception will be thrown.");
	SPQOS_RAISE(spqdxl::ExmaSPQDB, spqdxl::ExmiSPQDBError);
	return NULL;
}

//---------------------------------------------------------------------------
//	@function:
//		CConstExprEvaluatorProxy::EvaluateExpr
//
//	@doc:
//		Evaluate 'expr', assumed to be a constant expression, and return the DXL representation
// 		of the result. Caller keeps ownership of 'expr' and takes ownership of the returned pointer.
//
//---------------------------------------------------------------------------
CDXLNode *
CConstExprEvaluatorProxy::EvaluateExpr(const CDXLNode *dxl_expr)
{
	// Translate DXL -> SPQDB Expr
	Expr *expr = m_dxl2scalar_translator.TranslateDXLToScalar(
		dxl_expr, &m_emptymapcidvar);
	SPQOS_ASSERT(NULL != expr);

	// Evaluate the expression
	Expr *result = spqdb::EvaluateExpr(expr, spqdb::ExprType((Node *) expr),
									  spqdb::ExprTypeMod((Node *) expr));

	if (!IsA(result, Const))
	{
#ifdef SPQOS_DEBUG
		elog(
			NOTICE,
			"Expression did not evaluate to Const, but to an expression of type %d",
			result->type);
#endif
		SPQOS_RAISE(spqdxl::ExmaConstExprEval, spqdxl::ExmiConstExprEvalNonConst);
	}

	Const *const_result = (Const *) result;
	CDXLDatum *datum_dxl = CTranslatorScalarToDXL::TranslateConstToDXL(
		m_mp, m_md_accessor, const_result);
	CDXLNode *dxl_result = SPQOS_NEW(m_mp)
		CDXLNode(m_mp, SPQOS_NEW(m_mp) CDXLScalarConstValue(m_mp, datum_dxl));
	spqdb::SPQDBFree(result);
	spqdb::SPQDBFree(expr);

	return dxl_result;
}

// EOF
