//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal, Inc.
//
//	@filename:
//		IConstDXLNodeEvaluator.h
//
//	@doc:
//		Interface for evaluating constant expressions given as DXL
//
//	@owner:
//
//
//	@test:
//
//---------------------------------------------------------------------------

#ifndef SPQOPT_IConstDXLNodeEvaluator_H
#define SPQOPT_IConstDXLNodeEvaluator_H

#include "spqos/base.h"

// forward declaration
namespace spqdxl
{
class CDXLNode;
}

namespace spqopt
{
//---------------------------------------------------------------------------
//	@class:
//		IConstExprEvaluator
//
//	@doc:
//		Interface for evaluating constant expressions given as DXL
//
//---------------------------------------------------------------------------
class IConstDXLNodeEvaluator
{
public:
	// dtor
	virtual ~IConstDXLNodeEvaluator()
	{
	}

	// evaluate the given DXL node representing an expression and return the result as DXL.
	// caller takes ownership of returned DXL node
	virtual spqdxl::CDXLNode *EvaluateExpr(const spqdxl::CDXLNode *pdxlnExpr) = 0;

	// returns true iff the evaluator can evaluate constant expressions without subqueries
	virtual spqos::BOOL FCanEvalExpressions() = 0;
};
}  // namespace spqopt

#endif	// !SPQOPT_IConstDXLNodeEvaluator_H

// EOF
