//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal, Inc.
//
//	@filename:
//		CConstExprEvaluatorDefault.cpp
//
//	@doc:
//		Dummy implementation of the constant expression evaluator
//
//	@owner:
//
//
//	@test:
//
//---------------------------------------------------------------------------

#include "spqopt/eval/CConstExprEvaluatorDefault.h"

#include "spqopt/operators/CExpression.h"

using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CConstExprEvaluatorDefault::~CConstExprEvaluatorDefault
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CConstExprEvaluatorDefault::~CConstExprEvaluatorDefault()
{
}

//---------------------------------------------------------------------------
//	@function:
//		CConstExprEvaluatorDefault::PexprEval
//
//	@doc:
//		Returns the given expression after having increased its ref count
//
//---------------------------------------------------------------------------
CExpression *
CConstExprEvaluatorDefault::PexprEval(CExpression *pexpr)
{
	pexpr->AddRef();
	return pexpr;
}

//---------------------------------------------------------------------------
//	@function:
//		CConstExprEvaluatorDefault::FCanEvalFunctions
//
//	@doc:
//		Returns false, since this evaluator cannot call any functions
//
//---------------------------------------------------------------------------
BOOL
CConstExprEvaluatorDefault::FCanEvalExpressions()
{
	return false;
}

// EOF
