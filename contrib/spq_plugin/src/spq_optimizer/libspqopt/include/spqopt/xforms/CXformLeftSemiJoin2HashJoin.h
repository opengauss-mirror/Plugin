//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CXformLeftSemiJoin2HashJoin.h
//
//	@doc:
//		Transform left semi join to left semi hash join
//---------------------------------------------------------------------------
#ifndef SPQOPT_CXformLeftSemiJoin2HashJoin_H
#define SPQOPT_CXformLeftSemiJoin2HashJoin_H

#include "spqos/base.h"

#include "spqopt/xforms/CXformImplementation.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CXformLeftSemiJoin2HashJoin
//
//	@doc:
//		Transform left semi join to left semi hash join
//
//---------------------------------------------------------------------------
class CXformLeftSemiJoin2HashJoin : public CXformImplementation
{
private:
	// private copy ctor
	CXformLeftSemiJoin2HashJoin(const CXformLeftSemiJoin2HashJoin &);

public:
	// ctor
	explicit CXformLeftSemiJoin2HashJoin(CMemoryPool *mp);

	// dtor
	virtual ~CXformLeftSemiJoin2HashJoin()
	{
	}

	// ident accessors
	virtual EXformId
	Exfid() const
	{
		return ExfLeftSemiJoin2HashJoin;
	}

	// return a string for xform name
	virtual const CHAR *
	SzId() const
	{
		return "CXformLeftSemiJoin2HashJoin";
	}

	// compute xform promise for a given expression handle
	virtual EXformPromise Exfp(CExpressionHandle &exprhdl) const;

	// actual transform
	void Transform(CXformContext *pxfctxt, CXformResult *pxfres,
				   CExpression *pexpr) const;

};	// class CXformLeftSemiJoin2HashJoin

}  // namespace spqopt


#endif	// !SPQOPT_CXformLeftSemiJoin2HashJoin_H

// EOF
