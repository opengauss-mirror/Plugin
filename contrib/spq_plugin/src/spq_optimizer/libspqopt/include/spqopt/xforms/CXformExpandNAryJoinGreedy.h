//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2018 Pivotal Software Inc.
//
//	@filename:
//		CXformExpandNAryJoinGreedy.h
//
//	@doc:
//		Expand n-ary join into series of binary joins while minimizing
//		cardinality of intermediate results and delay cross joins to
//		the end
//---------------------------------------------------------------------------
#ifndef SPQOPT_CXformExpandNAryJoinGreedy_H
#define SPQOPT_CXformExpandNAryJoinGreedy_H

#include "spqos/base.h"

#include "spqopt/xforms/CXformExploration.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CXformExpandNAryJoinGreedy
//
//	@doc:
//		Expand n-ary join into series of binary joins while minimizing
//		cardinality of intermediate results and delay cross joins to
//		the end
//
//---------------------------------------------------------------------------
class CXformExpandNAryJoinGreedy : public CXformExploration
{
private:
	// private copy ctor
	CXformExpandNAryJoinGreedy(const CXformExpandNAryJoinGreedy &);

public:
	// ctor
	explicit CXformExpandNAryJoinGreedy(CMemoryPool *pmp);

	// dtor
	virtual ~CXformExpandNAryJoinGreedy()
	{
	}

	// ident accessors
	virtual EXformId
	Exfid() const
	{
		return ExfExpandNAryJoinGreedy;
	}

	// return a string for xform name
	virtual const CHAR *
	SzId() const
	{
		return "CXformExpandNAryJoinGreedy";
	}

	// compute xform promise for a given expression handle
	virtual EXformPromise Exfp(CExpressionHandle &exprhdl) const;

	// do stats need to be computed before applying xform?
	virtual BOOL
	FNeedsStats() const
	{
		return true;
	}

	// actual transform
	void Transform(CXformContext *pxfctxt, CXformResult *pxfres,
				   CExpression *pexpr) const;

	BOOL
	IsApplyOnce()
	{
		return true;
	}

};	// class CXformExpandNAryJoinGreedy

}  // namespace spqopt


#endif	// !SPQOPT_CXformExpandNAryJoinGreedy_H

// EOF
