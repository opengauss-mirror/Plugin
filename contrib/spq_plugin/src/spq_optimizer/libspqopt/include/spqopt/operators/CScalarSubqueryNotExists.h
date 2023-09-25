//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CScalarSubqueryNotExists.h
//
//	@doc:
//		Scalar subquery NOT EXISTS operator
//---------------------------------------------------------------------------
#ifndef SPQOPT_CScalarSubqueryNotExists_H
#define SPQOPT_CScalarSubqueryNotExists_H

#include "spqos/base.h"

#include "spqopt/operators/CScalarSubqueryExistential.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CScalarSubqueryNotExists
//
//	@doc:
//		Scalar subquery NOT EXISTS.
//
//---------------------------------------------------------------------------
class CScalarSubqueryNotExists : public CScalarSubqueryExistential
{
private:
	// private copy ctor
	CScalarSubqueryNotExists(const CScalarSubqueryNotExists &);

public:
	// ctor
	CScalarSubqueryNotExists(CMemoryPool *mp) : CScalarSubqueryExistential(mp)
	{
	}

	// dtor
	virtual ~CScalarSubqueryNotExists()
	{
	}

	// ident accessors
	virtual EOperatorId
	Eopid() const
	{
		return EopScalarSubqueryNotExists;
	}

	// return a string for scalar subquery
	virtual const CHAR *
	SzId() const
	{
		return "CScalarSubqueryNotExists";
	}

	// conversion function
	static CScalarSubqueryNotExists *
	PopConvert(COperator *pop)
	{
		SPQOS_ASSERT(NULL != pop);
		SPQOS_ASSERT(EopScalarSubqueryNotExists == pop->Eopid());

		return reinterpret_cast<CScalarSubqueryNotExists *>(pop);
	}

};	// class CScalarSubqueryNotExists
}  // namespace spqopt

#endif	// !SPQOPT_CScalarSubqueryNotExists_H

// EOF
