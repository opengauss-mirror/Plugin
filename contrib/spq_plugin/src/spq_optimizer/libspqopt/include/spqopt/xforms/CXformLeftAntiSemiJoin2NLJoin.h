//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CXformLeftAntiSemiJoin2NLJoin.h
//
//	@doc:
//		Transform left anti semi join to left anti semi NLJ
//---------------------------------------------------------------------------
#ifndef SPQOPT_CXformLeftAntiSemiJoin2NLJoin_H
#define SPQOPT_CXformLeftAntiSemiJoin2NLJoin_H

#include "spqos/base.h"

#include "spqopt/xforms/CXformImplementation.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CXformLeftAntiSemiJoin2NLJoin
//
//	@doc:
//		Transform left anti semi join to left anti semi NLJ
//
//---------------------------------------------------------------------------
class CXformLeftAntiSemiJoin2NLJoin : public CXformImplementation
{
private:
	// private copy ctor
	CXformLeftAntiSemiJoin2NLJoin(const CXformLeftAntiSemiJoin2NLJoin &);


public:
	// ctor
	explicit CXformLeftAntiSemiJoin2NLJoin(CMemoryPool *mp);

	// dtor
	virtual ~CXformLeftAntiSemiJoin2NLJoin()
	{
	}

	// ident accessors
	virtual EXformId
	Exfid() const
	{
		return ExfLeftAntiSemiJoin2NLJoin;
	}

	// return a string for xform name
	virtual const CHAR *
	SzId() const
	{
		return "CXformLeftAntiSemiJoin2NLJoin";
	}

	// compute xform promise for a given expression handle
	virtual EXformPromise Exfp(CExpressionHandle &exprhdl) const;

	// actual transform
	void Transform(CXformContext *pxfctxt, CXformResult *pxfres,
				   CExpression *pexpr) const;

};	// class CXformLeftAntiSemiJoin2NLJoin

}  // namespace spqopt


#endif	// !SPQOPT_CXformLeftAntiSemiJoin2NLJoin_H

// EOF
