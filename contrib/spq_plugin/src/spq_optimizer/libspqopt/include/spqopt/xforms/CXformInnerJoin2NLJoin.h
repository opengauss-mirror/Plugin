//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		CXformInnerJoin2NLJoin.h
//
//	@doc:
//		Transform inner join to inner NLJ
//---------------------------------------------------------------------------
#ifndef SPQOPT_CXformInnerJoin2NLJoin_H
#define SPQOPT_CXformInnerJoin2NLJoin_H

#include "spqos/base.h"

#include "spqopt/xforms/CXformImplementation.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CXformInnerJoin2NLJoin
//
//	@doc:
//		Transform inner join to inner NLJ
//		Deprecated in favor of CXformImplementInnerJoin.
//
//---------------------------------------------------------------------------
class CXformInnerJoin2NLJoin : public CXformImplementation
{
private:
	// private copy ctor
	CXformInnerJoin2NLJoin(const CXformInnerJoin2NLJoin &);

public:
	// ctor
	explicit CXformInnerJoin2NLJoin(CMemoryPool *mp);

	// dtor
	virtual ~CXformInnerJoin2NLJoin()
	{
	}

	// ident accessors
	virtual EXformId
	Exfid() const
	{
		return ExfInnerJoin2NLJoin;
	}

	// return a string for xform name
	virtual const CHAR *
	SzId() const
	{
		return "CXformInnerJoin2NLJoin";
	}

	// compute xform promise for a given expression handle
	virtual EXformPromise Exfp(CExpressionHandle &exprhdl) const;

	// actual transform
	void Transform(CXformContext *pxfctxt, CXformResult *pxfres,
				   CExpression *pexpr) const;

};	// class CXformInnerJoin2NLJoin

}  // namespace spqopt


#endif	// !SPQOPT_CXformInnerJoin2NLJoin_H

// EOF
