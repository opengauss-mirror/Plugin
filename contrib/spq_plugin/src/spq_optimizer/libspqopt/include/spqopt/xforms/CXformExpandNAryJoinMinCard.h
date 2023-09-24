//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CXformExpandNAryJoinMinCard.h
//
//	@doc:
//		Expand n-ary join into series of binary joins while minimizing
//		cardinality of intermediate results
//---------------------------------------------------------------------------
#ifndef SPQOPT_CXformExpandNAryJoinMinCard_H
#define SPQOPT_CXformExpandNAryJoinMinCard_H

#include "spqos/base.h"

#include "spqopt/xforms/CXformExploration.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CXformExpandNAryJoinMinCard
//
//	@doc:
//		Expand n-ary join into series of binary joins while minimizing
//		cardinality of intermediate results
//
//---------------------------------------------------------------------------
class CXformExpandNAryJoinMinCard : public CXformExploration
{
private:
	// private copy ctor
	CXformExpandNAryJoinMinCard(const CXformExpandNAryJoinMinCard &);

public:
	// ctor
	explicit CXformExpandNAryJoinMinCard(CMemoryPool *mp);

	// dtor
	virtual ~CXformExpandNAryJoinMinCard()
	{
	}

	// ident accessors
	virtual EXformId
	Exfid() const
	{
		return ExfExpandNAryJoinMinCard;
	}

	// return a string for xform name
	virtual const CHAR *
	SzId() const
	{
		return "CXformExpandNAryJoinMinCard";
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
};	// class CXformExpandNAryJoinMinCard

}  // namespace spqopt


#endif	// !SPQOPT_CXformExpandNAryJoinMinCard_H

// EOF
