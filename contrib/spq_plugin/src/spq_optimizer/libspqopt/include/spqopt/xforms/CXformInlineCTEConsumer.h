//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CXformInlineCTEConsumer.h
//
//	@doc:
//		Transform logical CTE consumer to a copy of the expression under its
//		corresponding producer
//---------------------------------------------------------------------------
#ifndef SPQOPT_CXformInlineCTEConsumer_H
#define SPQOPT_CXformInlineCTEConsumer_H

#include "spqos/base.h"

#include "spqopt/xforms/CXformExploration.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CXformInlineCTEConsumer
//
//	@doc:
//		Transform logical CTE consumer to a copy of the expression under its
//		corresponding producer
//
//---------------------------------------------------------------------------
class CXformInlineCTEConsumer : public CXformExploration
{
private:
	// private copy ctor
	CXformInlineCTEConsumer(const CXformInlineCTEConsumer &);

public:
	// ctor
	explicit CXformInlineCTEConsumer(CMemoryPool *mp);

	// dtor
	virtual ~CXformInlineCTEConsumer()
	{
	}

	// ident accessors
	virtual EXformId
	Exfid() const
	{
		return ExfInlineCTEConsumer;
	}

	// return a string for xform name
	virtual const CHAR *
	SzId() const
	{
		return "CXformInlineCTEConsumer";
	}

	// compute xform promise for a given expression handle
	virtual EXformPromise Exfp(CExpressionHandle &exprhdl) const;

	// actual transform
	virtual void Transform(CXformContext *pxfctxt, CXformResult *pxfres,
						   CExpression *pexpr) const;

};	// class CXformInlineCTEConsumer
}  // namespace spqopt

#endif	// !SPQOPT_CXformInlineCTEConsumer_H

// EOF
