//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2019 Pivotal Software, Inc.
//
#ifndef SPQOPT_CXformImplementFullOuterMergeJoin_H
#define SPQOPT_CXformImplementFullOuterMergeJoin_H

#include "spqos/base.h"

#include "spqopt/xforms/CXformExploration.h"

namespace spqopt
{
using namespace spqos;

// FIXME: This should derive from CXformImplementation, but there is an unrelated bug
// with ImplementMergeJoin() that causes us to generate an invalid plan in some
// cases if we change this without fixing the bug.
class CXformImplementFullOuterMergeJoin : public CXformExploration
{
private:
	// private copy ctor
	CXformImplementFullOuterMergeJoin(
		const CXformImplementFullOuterMergeJoin &);

public:
	// ctor
	explicit CXformImplementFullOuterMergeJoin(CMemoryPool *mp);

	// dtor
	virtual ~CXformImplementFullOuterMergeJoin()
	{
	}

	// ident accessors
	virtual EXformId
	Exfid() const
	{
		return ExfImplementFullOuterMergeJoin;
	}

	// return a string for xform name
	virtual const CHAR *
	SzId() const
	{
		return "CXformImplementFullOuterMergeJoin";
	}

	// compute xform promise for a given expression handle
	virtual EXformPromise Exfp(CExpressionHandle &exprhdl) const;

	// actual transform
	virtual void Transform(CXformContext *pxfctxt, CXformResult *pxfres,
						   CExpression *pexpr) const;

};	// class CXformImplementFullOuterMergeJoin
}  // namespace spqopt

#endif	// !SPQOPT_CXformImplementFullOuterMergeJoin_H

// EOF
