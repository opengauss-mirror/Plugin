//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CScalarSubqueryAny.h
//
//	@doc:
//		Class for scalar subquery ANY operators
//---------------------------------------------------------------------------
#ifndef SPQOPT_CScalarSubqueryAny_H
#define SPQOPT_CScalarSubqueryAny_H

#include "spqos/base.h"

#include "spqopt/operators/CScalarSubqueryQuantified.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CScalarSubqueryAny
//
//	@doc:
//		Scalar subquery ANY.
//		A scalar subquery ANY expression has two children: relational and scalar.
//
//---------------------------------------------------------------------------
class CScalarSubqueryAny : public CScalarSubqueryQuantified
{
private:
	// private copy ctor
	CScalarSubqueryAny(const CScalarSubqueryAny &);

public:
	// ctor
	CScalarSubqueryAny(CMemoryPool *mp, IMDId *scalar_op_mdid,
					   const CWStringConst *pstrScalarOp,
					   const CColRef *colref);

	// dtor
	virtual ~CScalarSubqueryAny()
	{
	}

	// ident accessors
	virtual EOperatorId
	Eopid() const
	{
		return EopScalarSubqueryAny;
	}

	// return a string for scalar subquery
	virtual const CHAR *
	SzId() const
	{
		return "CScalarSubqueryAny";
	}

	// return a copy of the operator with remapped columns
	virtual COperator *PopCopyWithRemappedColumns(
		CMemoryPool *mp, UlongToColRefMap *colref_mapping, BOOL must_exist);

	// conversion function
	static CScalarSubqueryAny *
	PopConvert(COperator *pop)
	{
		SPQOS_ASSERT(NULL != pop);
		SPQOS_ASSERT(EopScalarSubqueryAny == pop->Eopid());

		return reinterpret_cast<CScalarSubqueryAny *>(pop);
	}

};	// class CScalarSubqueryAny
}  // namespace spqopt

#endif	// !SPQOPT_CScalarSubqueryAny_H

// EOF
