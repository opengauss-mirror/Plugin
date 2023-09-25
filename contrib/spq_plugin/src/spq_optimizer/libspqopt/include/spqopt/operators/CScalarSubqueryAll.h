//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CScalarSubqueryAll.h
//
//	@doc:
//		Class for scalar subquery ALL operators
//---------------------------------------------------------------------------
#ifndef SPQOPT_CScalarSubqueryAll_H
#define SPQOPT_CScalarSubqueryAll_H

#include "spqos/base.h"

#include "spqopt/operators/CScalarSubqueryQuantified.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CScalarSubqueryAll
//
//	@doc:
//		Scalar subquery ALL
//		A scalar subquery ALL expression has two children: relational and scalar.
//
//---------------------------------------------------------------------------
class CScalarSubqueryAll : public CScalarSubqueryQuantified
{
private:
	// private copy ctor
	CScalarSubqueryAll(const CScalarSubqueryAll &);

public:
	// ctor
	CScalarSubqueryAll(CMemoryPool *mp, IMDId *scalar_op_mdid,
					   const CWStringConst *pstrScalarOp,
					   const CColRef *colref);

	// dtor
	virtual ~CScalarSubqueryAll()
	{
	}

	// ident accessors
	virtual EOperatorId
	Eopid() const
	{
		return EopScalarSubqueryAll;
	}

	// return a string for scalar subquery
	virtual const CHAR *
	SzId() const
	{
		return "CScalarSubqueryAll";
	}

	// return a copy of the operator with remapped columns
	virtual COperator *PopCopyWithRemappedColumns(
		CMemoryPool *mp, UlongToColRefMap *colref_mapping, BOOL must_exist);

	// conversion function
	static CScalarSubqueryAll *
	PopConvert(COperator *pop)
	{
		SPQOS_ASSERT(NULL != pop);
		SPQOS_ASSERT(EopScalarSubqueryAll == pop->Eopid());

		return reinterpret_cast<CScalarSubqueryAll *>(pop);
	}

};	// class CScalarSubqueryAll
}  // namespace spqopt

#endif	// !SPQOPT_CScalarSubqueryAll_H

// EOF
