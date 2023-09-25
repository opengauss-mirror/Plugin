//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2017 Pivotal Inc.
//
//	@filename:
//		CScalarArrayCoerceExpr.h
//
//	@doc:
//		Scalar Array Coerce Expr operator,
//		the operator will apply type casting for each element in this array
//		using the given element coercion function.
//
//	@owner:
//
//	@test:
//
//
//---------------------------------------------------------------------------
#ifndef SPQOPT_CScalarArrayCoerceExpr_H
#define SPQOPT_CScalarArrayCoerceExpr_H

#include "spqos/base.h"

#include "spqopt/operators/CScalarCoerceBase.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CScalarArrayCoerceExpr
//
//	@doc:
//		Scalar Array Coerce Expr operator
//
//---------------------------------------------------------------------------
class CScalarArrayCoerceExpr : public CScalarCoerceBase
{
private:
	// catalog MDId of the element function
	IMDId *m_pmdidElementFunc;

	// conversion semantics flag to pass to func
	BOOL m_is_explicit;

	// private copy ctor
	CScalarArrayCoerceExpr(const CScalarArrayCoerceExpr &);

public:
	// ctor
	CScalarArrayCoerceExpr(CMemoryPool *mp, IMDId *element_func,
						   IMDId *result_type_mdid, INT type_modifier,
						   BOOL is_explicit, ECoercionForm dxl_coerce_format,
						   INT location);

	// dtor
	virtual ~CScalarArrayCoerceExpr();

	// return metadata id of element coerce function
	IMDId *PmdidElementFunc() const;

	BOOL IsExplicit() const;

	virtual EOperatorId Eopid() const;

	// return a string for operator name
	virtual const CHAR *SzId() const;

	// match function
	virtual BOOL Matches(COperator *pop) const;

	// sensitivity to order of inputs
	virtual BOOL FInputOrderSensitive() const;

	// conversion function
	static CScalarArrayCoerceExpr *PopConvert(COperator *pop);

};	// class CScalarArrayCoerceExpr

}  // namespace spqopt


#endif	// !SPQOPT_CScalarArrayCoerceExpr_H

// EOF
