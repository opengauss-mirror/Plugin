//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CXformPushGbBelowJoin.h
//
//	@doc:
//		Push group by below join transform
//---------------------------------------------------------------------------
#ifndef SPQOPT_CXformPushGbBelowJoin_H
#define SPQOPT_CXformPushGbBelowJoin_H

#include "spqos/base.h"

#include "spqopt/xforms/CXformExploration.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CXformPushGbBelowJoin
//
//	@doc:
//		Push group by below join transform
//
//---------------------------------------------------------------------------
class CXformPushGbBelowJoin : public CXformExploration
{
private:
	// private copy ctor
	CXformPushGbBelowJoin(const CXformPushGbBelowJoin &);

public:
	// ctor
	explicit CXformPushGbBelowJoin(CMemoryPool *mp);

	// ctor
	explicit CXformPushGbBelowJoin(CExpression *pexprPattern);

	// dtor
	virtual ~CXformPushGbBelowJoin()
	{
	}

	// ident accessors
	virtual EXformId
	Exfid() const
	{
		return ExfPushGbBelowJoin;
	}

	virtual const CHAR *
	SzId() const
	{
		return "CXformPushGbBelowJoin";
	}

	// compute xform promise for a given expression handle
	virtual EXformPromise Exfp(CExpressionHandle &exprhdl) const;

	// actual transform
	void Transform(CXformContext *pxfctxt, CXformResult *pxfres,
				   CExpression *pexpr) const;

};	// class CXformPushGbBelowJoin

}  // namespace spqopt

#endif	// !SPQOPT_CXformPushGbBelowJoin_H

// EOF
