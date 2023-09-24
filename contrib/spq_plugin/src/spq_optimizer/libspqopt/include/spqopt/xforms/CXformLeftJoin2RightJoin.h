//---------------------------------------------------------------------------
//	Greenplum Database
//  Copyright (c) 2020 VMware, Inc.
//
//	@filename:
//		CXformLeftJoin2RightJoin.h
//
//	@doc:
//		Transform left outer join to right outer join
//---------------------------------------------------------------------------
#ifndef SPQOPT_CXformLeftJoin2RightJoin_H
#define SPQOPT_CXformLeftJoin2RightJoin_H

#include "spqos/base.h"

#include "spqopt/xforms/CXformExploration.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CXformLeftJoin2RightJoin
//
//	@doc:
//		Transform left outer join to right outer join
//
//---------------------------------------------------------------------------
class CXformLeftJoin2RightJoin : public CXformExploration
{
private:
	// private copy ctor
	CXformLeftJoin2RightJoin(const CXformLeftJoin2RightJoin &);

public:
	// ctor
	explicit CXformLeftJoin2RightJoin(CMemoryPool *mp);

	// dtor
	virtual ~CXformLeftJoin2RightJoin()
	{
	}

	// ident accessors
	virtual EXformId
	Exfid() const
	{
		return ExfLeftJoin2RightJoin;
	}

	// return a string for xform name
	virtual const CHAR *
	SzId() const
	{
		return "CXformLeftJoin2RightJoin";
	}

	// compute xform promise for a given expression handle
	virtual EXformPromise Exfp(CExpressionHandle &exprhdl) const;

	// actual transform
	void Transform(CXformContext *pxfctxt, CXformResult *pxfres,
				   CExpression *pexpr) const;

};	// class CXformLeftJoin2RightJoin

}  // namespace spqopt


#endif	// !SPQOPT_CXformLeftJoin2RightJoin_H

// EOF
