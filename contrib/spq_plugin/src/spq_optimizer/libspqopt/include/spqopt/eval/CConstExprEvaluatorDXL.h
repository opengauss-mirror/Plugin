//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal, Inc.
//
//	@filename:
//		CConstExprEvaluatorDXL.h
//
//	@doc:
//		Constant expression evaluator implementation that delegates to a DXL evaluator
//
//	@owner:
//
//
//	@test:
//
//---------------------------------------------------------------------------

#ifndef SPQOPT_CConstExprEvaluatorDXL_H
#define SPQOPT_CConstExprEvaluatorDXL_H

#include "spqos/base.h"

#include "spqopt/base/CColRef.h"
#include "spqopt/eval/IConstExprEvaluator.h"
#include "spqopt/translate/CTranslatorDXLToExpr.h"
#include "spqopt/translate/CTranslatorExprToDXL.h"
#include "naucrates/dxl/operators/CDXLNode.h"
#include "naucrates/md/CMDName.h"

// forward declaration
namespace spqos
{
class CMemoryPool;
}

namespace spqopt
{
class CExpression;
class CMDAccessor;
class IConstDXLNodeEvaluator;

//---------------------------------------------------------------------------
//	@class:
//		CConstExprEvaluatorDXL
//
//	@doc:
//		Constant expression evaluator implementation that delegates to a DXL evaluator
//
//---------------------------------------------------------------------------
class CConstExprEvaluatorDXL : public IConstExprEvaluator
{
private:
	// evaluates expressions represented as DXL, not owned
	IConstDXLNodeEvaluator *m_pconstdxleval;

	// translates CExpression's to DXL which can then be sent to the evaluator
	CTranslatorExprToDXL m_trexpr2dxl;

	// translates DXL coming from the evaluator back to CExpression
	CTranslatorDXLToExpr m_trdxl2expr;

	// private copy ctor
	CConstExprEvaluatorDXL(const CConstExprEvaluatorDXL &);

public:
	// ctor
	CConstExprEvaluatorDXL(CMemoryPool *mp, CMDAccessor *md_accessor,
						   IConstDXLNodeEvaluator *pconstdxleval);

	// dtor
	virtual ~CConstExprEvaluatorDXL();

	// evaluate the given expression and return the result as a new expression
	// caller takes ownership of returned expression
	virtual CExpression *PexprEval(CExpression *pexpr);

	// Returns true iff the evaluator can evaluate expressions
	virtual BOOL FCanEvalExpressions();
};
}  // namespace spqopt

#endif	// !SPQOPT_CConstExprEvaluatorDXL_H

// EOF
