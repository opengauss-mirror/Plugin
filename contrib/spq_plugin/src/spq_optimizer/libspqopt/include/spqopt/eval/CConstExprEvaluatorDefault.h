//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal, Inc.
//
//	@filename:
//		CConstExprEvaluatorDefault.h
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

#ifndef SPQOPT_CConstExprEvaluatorDefault_H
#define SPQOPT_CConstExprEvaluatorDefault_H

#include "spqos/base.h"

#include "spqopt/eval/IConstExprEvaluator.h"

namespace spqopt
{
//---------------------------------------------------------------------------
//	@class:
//		CConstExprEvaluatorDefault
//
//	@doc:
//		Constant expression evaluator default implementation for the case when
//		no database instance is available
//
//---------------------------------------------------------------------------
class CConstExprEvaluatorDefault : public IConstExprEvaluator
{
private:
	// private copy ctor
	CConstExprEvaluatorDefault(const CConstExprEvaluatorDefault &);

public:
	// ctor
	CConstExprEvaluatorDefault() : IConstExprEvaluator()
	{
	}

	// dtor
	virtual ~CConstExprEvaluatorDefault();

	// Evaluate the given expression and return the result as a new expression
	virtual CExpression *PexprEval(CExpression *pexpr);

	// Returns true iff the evaluator can evaluate constant expressions
	virtual BOOL FCanEvalExpressions();
};
}  // namespace spqopt

#endif	// !SPQOPT_CConstExprEvaluatorSPQDB_H

// EOF
