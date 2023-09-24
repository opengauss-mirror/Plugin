//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CScalarSubqueryExists.h
//
//	@doc:
//		Scalar subquery EXISTS operator
//---------------------------------------------------------------------------
#ifndef SPQOPT_CScalarSubqueryExists_H
#define SPQOPT_CScalarSubqueryExists_H

#include "spqos/base.h"

#include "spqopt/operators/CScalarSubqueryExistential.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CScalarSubqueryExists
//
//	@doc:
//		Scalar subquery EXISTS.
//
//---------------------------------------------------------------------------
class CScalarSubqueryExists : public CScalarSubqueryExistential
{
private:
	// private copy ctor
	CScalarSubqueryExists(const CScalarSubqueryExists &);

public:
	// ctor
	CScalarSubqueryExists(CMemoryPool *mp) : CScalarSubqueryExistential(mp)
	{
	}

	// dtor
	virtual ~CScalarSubqueryExists()
	{
	}

	// ident accessors
	virtual EOperatorId
	Eopid() const
	{
		return EopScalarSubqueryExists;
	}

	// return a string for scalar subquery
	virtual const CHAR *
	SzId() const
	{
		return "CScalarSubqueryExists";
	}

	// conversion function
	static CScalarSubqueryExists *
	PopConvert(COperator *pop)
	{
		SPQOS_ASSERT(NULL != pop);
		SPQOS_ASSERT(EopScalarSubqueryExists == pop->Eopid());

		return reinterpret_cast<CScalarSubqueryExists *>(pop);
	}

};	// class CScalarSubqueryExists
}  // namespace spqopt

#endif	// !SPQOPT_CScalarSubqueryExists_H

// EOF
