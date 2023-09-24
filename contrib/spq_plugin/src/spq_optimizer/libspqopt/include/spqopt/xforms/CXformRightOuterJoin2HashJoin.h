//---------------------------------------------------------------------------
//	Greenplum Database
//  Copyright (c) 2020 VMware, Inc.
//
//	@filename:
//		CXformRightOuterJoin2HashJoin.h
//
//	@doc:
//		Transform left outer join to left outer hash join
//---------------------------------------------------------------------------
#ifndef SPQOPT_CXformRightOuterJoin2HashJoin_H
#define SPQOPT_CXformRightOuterJoin2HashJoin_H

#include "spqos/base.h"

#include "spqopt/xforms/CXformImplementation.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CXformRightOuterJoin2HashJoin
//
//	@doc:
//		Transform left outer join to left outer hash join
//
//---------------------------------------------------------------------------
class CXformRightOuterJoin2HashJoin : public CXformImplementation
{
private:
	// private copy ctor
	CXformRightOuterJoin2HashJoin(const CXformRightOuterJoin2HashJoin &);


public:
	// ctor
	explicit CXformRightOuterJoin2HashJoin(CMemoryPool *mp);

	// dtor
	virtual ~CXformRightOuterJoin2HashJoin()
	{
	}

	// ident accessors
	virtual EXformId
	Exfid() const
	{
		return ExfRightOuterJoin2HashJoin;
	}

	// return a string for xform name
	virtual const CHAR *
	SzId() const
	{
		return "CXformRightOuterJoin2HashJoin";
	}

	// compute xform promise for a given expression handle
	virtual EXformPromise Exfp(CExpressionHandle &exprhdl) const;


	// actual transform
	void Transform(CXformContext *pxfctxt, CXformResult *pxfres,
				   CExpression *pexpr) const;

};	// class CXformRightOuterJoin2HashJoin

}  // namespace spqopt


#endif	// !SPQOPT_CXformRightOuterJoin2HashJoin_H

// EOF
