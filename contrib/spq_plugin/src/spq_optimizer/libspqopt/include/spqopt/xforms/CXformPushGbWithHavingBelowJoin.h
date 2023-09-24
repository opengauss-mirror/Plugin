//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CXformPushGbWithHavingBelowJoin.h
//
//	@doc:
//		Push group by with having clause below join transform
//---------------------------------------------------------------------------
#ifndef SPQOPT_CXformPushGbWithHavingBelowJoin_H
#define SPQOPT_CXformPushGbWithHavingBelowJoin_H

#include "spqos/base.h"

#include "spqopt/xforms/CXformExploration.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CXformPushGbWithHavingBelowJoin
//
//	@doc:
//		Push group by with having clause below join transform
//
//---------------------------------------------------------------------------
class CXformPushGbWithHavingBelowJoin : public CXformExploration
{
private:
	// private copy ctor
	CXformPushGbWithHavingBelowJoin(const CXformPushGbWithHavingBelowJoin &);

public:
	// ctor
	explicit CXformPushGbWithHavingBelowJoin(CMemoryPool *mp);

	// dtor
	virtual ~CXformPushGbWithHavingBelowJoin()
	{
	}

	// ident accessors
	virtual EXformId
	Exfid() const
	{
		return ExfPushGbWithHavingBelowJoin;
	}

	virtual const CHAR *
	SzId() const
	{
		return "CXformPushGbWithHavingBelowJoin";
	}

	// compute xform promise for a given expression handle
	virtual EXformPromise Exfp(CExpressionHandle &exprhdl) const;

	// actual transform
	void Transform(CXformContext *pxfctxt, CXformResult *pxfres,
				   CExpression *pexpr) const;

};	// class CXformPushGbWithHavingBelowJoin

}  // namespace spqopt

#endif	// !SPQOPT_CXformPushGbWithHavingBelowJoin_H

// EOF
