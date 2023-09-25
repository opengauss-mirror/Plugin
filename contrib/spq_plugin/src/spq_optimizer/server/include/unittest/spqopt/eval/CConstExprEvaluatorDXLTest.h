//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal, Inc.
//
//	@filename:
//		CConstExprEvaluatorDXLTest.h
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

#ifndef SPQOPT_CConstExprEvaluatorDXLTest_H
#define SPQOPT_CConstExprEvaluatorDXLTest_H

#include "spqos/base.h"

#include "spqopt/eval/IConstDXLNodeEvaluator.h"

// forward decl
namespace spqdxl
{
class CDXLNode;
}

namespace spqopt
{
using namespace spqos;

// forward decl
class CMDAccessor;

//---------------------------------------------------------------------------
//	@class:
//		CConstExprEvaluatorDXLTest
//
//	@doc:
//		Unit tests for CConstExprEvaluatorDXL
//
//---------------------------------------------------------------------------
class CConstExprEvaluatorDXLTest
{
private:
	class CDummyConstDXLNodeEvaluator : public IConstDXLNodeEvaluator
	{
	private:
		// memory pool
		CMemoryPool *m_mp;

		// metadata accessor
		CMDAccessor *m_pmda;

		// dummy value to return
		INT m_val;

		// private copy ctor
		CDummyConstDXLNodeEvaluator(const CDummyConstDXLNodeEvaluator &);

	public:
		// ctor
		CDummyConstDXLNodeEvaluator(CMemoryPool *mp, CMDAccessor *md_accessor,
									INT val)
			: m_mp(mp), m_pmda(md_accessor), m_val(val)
		{
		}

		// dtor
		virtual ~CDummyConstDXLNodeEvaluator()
		{
		}

		// evaluate the given DXL node representing an expression and returns a dummy value as DXL
		virtual spqdxl::CDXLNode *EvaluateExpr(const spqdxl::CDXLNode *pdxlnExpr);

		// can evaluate expressions
		virtual BOOL
		FCanEvalExpressions()
		{
			return true;
		}
	};

	// value  which the dummy constant evaluator should produce
	static const INT m_iDefaultEvalValue;

public:
	// run unittests
	static SPQOS_RESULT EresUnittest();

	// test that evaluation fails for a non scalar input
	static SPQOS_RESULT EresUnittest_NonScalar();

	// test that evaluation fails for a scalar with a nested subquery
	static SPQOS_RESULT EresUnittest_NestedSubquery();

	// test that evaluation fails for a scalar with variables
	static SPQOS_RESULT EresUnittest_ScalarContainingVariables();
};
}  // namespace spqopt

#endif	// !SPQOPT_CConstExprEvaluatorDXLTest_H

// EOF
