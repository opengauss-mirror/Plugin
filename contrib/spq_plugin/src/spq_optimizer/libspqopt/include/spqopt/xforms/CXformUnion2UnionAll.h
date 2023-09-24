//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CXformUnion2UnionAll.h
//
//	@doc:
//		Transform logical union into an aggregate over a logical union all
//
//	@owner:
//
//
//	@test:
//
//---------------------------------------------------------------------------
#ifndef SPQOPT_CXformUnion2UnionAll_H
#define SPQOPT_CXformUnion2UnionAll_H

#include "spqos/base.h"

#include "spqopt/xforms/CXformExploration.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CXformUnion2UnionAll
//
//	@doc:
//		Transform logical union into an aggregate over a logical union all
//
//---------------------------------------------------------------------------
class CXformUnion2UnionAll : public CXformExploration
{
private:
	// private copy ctor
	CXformUnion2UnionAll(const CXformUnion2UnionAll &);

public:
	// ctor
	explicit CXformUnion2UnionAll(CMemoryPool *mp);

	// dtor
	virtual ~CXformUnion2UnionAll()
	{
	}

	// ident accessors
	virtual EXformId
	Exfid() const
	{
		return ExfUnion2UnionAll;
	}

	virtual const CHAR *
	SzId() const
	{
		return "CXformUnion2UnionAll";
	}

	// compute xform promise for a given expression handle
	virtual EXformPromise
	Exfp(CExpressionHandle &  // exprhdl
	) const
	{
		return CXform::ExfpHigh;
	}

	// actual transform
	void Transform(CXformContext *, CXformResult *, CExpression *) const;

};	// class CXformUnion2UnionAll

}  // namespace spqopt

#endif	// !SPQOPT_CXformUnion2UnionAll_H

// EOF
