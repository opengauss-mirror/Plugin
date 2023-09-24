//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal, Inc.
//
//	@filename:
//		CConstExprEvaluatorProxy.h
//
//	@doc:
//		Evaluator for constant expressions passed as DXL
//
//	@test:
//
//---------------------------------------------------------------------------

#ifndef SPQDXL_CConstExprEvaluator_H
#define SPQDXL_CConstExprEvaluator_H

#include "spqos/base.h"

#include "spqopt/eval/IConstDXLNodeEvaluator.h"
#include "spqopt/mdcache/CMDAccessor.h"
#include "spq_optimizer_util/translate/CMappingColIdVar.h"
#include "spq_optimizer_util/translate/CTranslatorDXLToScalar.h"

namespace spqdxl
{
class CDXLNode;

//---------------------------------------------------------------------------
//	@class:
//		CConstExprEvaluatorProxy
//
//	@doc:
//		Wrapper over SPQDB's expression evaluator that takes a constant expression,
//		given as DXL, tries to evaluate it and returns the result as DXL.
//
//		The metadata cache should have been initialized by the caller before
//		creating an instance of this class and should not be released before
//		the destructor of this class.
//
//---------------------------------------------------------------------------
class CConstExprEvaluatorProxy : public spqopt::IConstDXLNodeEvaluator
{
private:
	//---------------------------------------------------------------------------
	//	@class:
	//		CEmptyMappingColIdVar
	//
	//	@doc:
	//		Dummy class to implement an empty variable mapping. Variable lookups
	//		raise exceptions.
	//
	//---------------------------------------------------------------------------
	class CEmptyMappingColIdVar : public CMappingColIdVar
	{
	public:
		explicit CEmptyMappingColIdVar(CMemoryPool *mp) : CMappingColIdVar(mp)
		{
		}

		virtual ~CEmptyMappingColIdVar()
		{
		}

		virtual Var *VarFromDXLNodeScId(const CDXLScalarIdent *scalar_ident);
	};

	// memory pool, not owned
	CMemoryPool *m_mp;

	// empty mapping needed for the translator
	CEmptyMappingColIdVar m_emptymapcidvar;

	// pointer to metadata cache accessor
	CMDAccessor *m_md_accessor;

	// translator for the DXL input -> SPQDB Expr
	CTranslatorDXLToScalar m_dxl2scalar_translator;

public:
	// ctor
	CConstExprEvaluatorProxy(CMemoryPool *mp, CMDAccessor *md_accessor)
		: m_mp(mp),
		  m_emptymapcidvar(m_mp),
		  m_md_accessor(md_accessor),
		  m_dxl2scalar_translator(m_mp, m_md_accessor, 0)
	{
	}

	// dtor
	virtual ~CConstExprEvaluatorProxy()
	{
	}

	// evaluate given constant expressionand return the DXL representation of the result.
	// if the expression has variables, an error is thrown.
	// caller keeps ownership of 'expr_dxlnode' and takes ownership of the returned pointer
	virtual CDXLNode *EvaluateExpr(const CDXLNode *expr);

	// returns true iff the evaluator can evaluate constant expressions without subqueries
	virtual BOOL
	FCanEvalExpressions()
	{
		return true;
	}
};
}  // namespace spqdxl

#endif	// !SPQDXL_CConstExprEvaluator_H

// EOF
