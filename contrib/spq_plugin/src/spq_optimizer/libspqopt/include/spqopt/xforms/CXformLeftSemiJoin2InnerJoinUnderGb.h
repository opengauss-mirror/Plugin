//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 Pivotal, Inc.
//
//	@filename:
//		CXformLeftSemiJoin2InnerJoinUnderGb.h
//
//	@doc:
//		Transform left semi join to inner join under a groupby
//---------------------------------------------------------------------------
#ifndef SPQOPT_CXformLeftSemiJoin2InnerJoinUnderGb_H
#define SPQOPT_CXformLeftSemiJoin2InnerJoinUnderGb_H

#include "spqos/base.h"

#include "spqopt/xforms/CXformExploration.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CXformLeftSemiJoin2InnerJoinUnderGb
//
//	@doc:
//		Transform left semi join to inner join under a groupby
//
//---------------------------------------------------------------------------
class CXformLeftSemiJoin2InnerJoinUnderGb : public CXformExploration
{
private:
	// private copy ctor
	CXformLeftSemiJoin2InnerJoinUnderGb(
		const CXformLeftSemiJoin2InnerJoinUnderGb &);

public:
	// ctor
	explicit CXformLeftSemiJoin2InnerJoinUnderGb(CMemoryPool *mp);

	// dtor
	virtual ~CXformLeftSemiJoin2InnerJoinUnderGb()
	{
	}

	// ident accessors
	virtual EXformId
	Exfid() const
	{
		return ExfLeftSemiJoin2InnerJoinUnderGb;
	}

	// return a string for xform name
	virtual const CHAR *
	SzId() const
	{
		return "CXformLeftSemiJoin2InnerJoinUnderGb";
	}

	// compute xform promise for a given expression handle
	virtual EXformPromise Exfp(CExpressionHandle &exprhdl) const;

	// actual transform
	virtual void Transform(CXformContext *pxfctxt, CXformResult *pxfres,
						   CExpression *pexpr) const;

};	// class CXformLeftSemiJoin2InnerJoinUnderGb

}  // namespace spqopt


#endif	// !SPQOPT_CXformLeftSemiJoin2InnerJoinUnderGb_H

// EOF
