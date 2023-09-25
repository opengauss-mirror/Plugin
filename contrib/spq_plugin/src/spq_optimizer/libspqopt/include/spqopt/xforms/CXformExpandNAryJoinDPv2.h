//---------------------------------------------------------------------------
// Greenplum Database
// Copyright (C) 2019 Pivotal Inc.
//
//	@filename:
//		CXformExpandNAryJoinDPv2.h
//
//	@doc:
//		Expand n-ary join into series of binary joins using dynamic
//		programming
//---------------------------------------------------------------------------
#ifndef SPQOPT_CXformExpandNAryJoinDPv2_H
#define SPQOPT_CXformExpandNAryJoinDPv2_H

#include "spqos/base.h"

#include "spqopt/xforms/CXformExploration.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CXformExpandNAryJoinDPv2
//
//	@doc:
//		Expand n-ary join into series of binary joins using dynamic
//		programming
//
//---------------------------------------------------------------------------
class CXformExpandNAryJoinDPv2 : public CXformExploration
{
private:
	// private copy ctor
	CXformExpandNAryJoinDPv2(const CXformExpandNAryJoinDPv2 &);

public:
	// ctor
	explicit CXformExpandNAryJoinDPv2(CMemoryPool *mp);

	// dtor
	virtual ~CXformExpandNAryJoinDPv2()
	{
	}

	// ident accessors
	virtual EXformId
	Exfid() const
	{
		return ExfExpandNAryJoinDPv2;
	}

	// return a string for xform name
	virtual const CHAR *
	SzId() const
	{
		return "CXformExpandNAryJoinDPv2";
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

};	// class CXformExpandNAryJoinDPv2

}  // namespace spqopt


#endif	// !SPQOPT_CXformExpandNAryJoinDPv2_H

// EOF
