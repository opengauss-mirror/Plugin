//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CXformLeftSemiApply2LeftSemiJoin.h
//
//	@doc:
//		Turn LS apply into LS join
//---------------------------------------------------------------------------
#ifndef SPQOPT_CXformLeftSemiApply2LeftSemiJoin_H
#define SPQOPT_CXformLeftSemiApply2LeftSemiJoin_H

#include "spqos/base.h"

#include "spqopt/operators/CLogicalLeftSemiApply.h"
#include "spqopt/operators/CLogicalLeftSemiJoin.h"
#include "spqopt/xforms/CXformApply2Join.h"


namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CXformLeftSemiApply2LeftSemiJoin
//
//	@doc:
//		Transform Apply into Join by decorrelating the inner side
//
//---------------------------------------------------------------------------
class CXformLeftSemiApply2LeftSemiJoin
	: public CXformApply2Join<CLogicalLeftSemiApply, CLogicalLeftSemiJoin>
{
private:
	// private copy ctor
	CXformLeftSemiApply2LeftSemiJoin(const CXformLeftSemiApply2LeftSemiJoin &);

public:
	// ctor
	explicit CXformLeftSemiApply2LeftSemiJoin(CMemoryPool *mp)
		: CXformApply2Join<CLogicalLeftSemiApply, CLogicalLeftSemiJoin>(
			  mp, true /*fDeepTree*/)
	{
	}

	// ctor with a passed pattern
	CXformLeftSemiApply2LeftSemiJoin(CMemoryPool *mp, CExpression *pexprPattern)
		: CXformApply2Join<CLogicalLeftSemiApply, CLogicalLeftSemiJoin>(
			  mp, pexprPattern)
	{
	}

	// dtor
	virtual ~CXformLeftSemiApply2LeftSemiJoin()
	{
	}

	// ident accessors
	virtual EXformId
	Exfid() const
	{
		return ExfLeftSemiApply2LeftSemiJoin;
	}

	virtual const CHAR *
	SzId() const
	{
		return "CXformLeftSemiApply2LeftSemiJoin";
	}

	// compute xform promise for a given expression handle
	virtual EXformPromise Exfp(CExpressionHandle &exprhdl) const;

	// actual transform
	virtual void Transform(CXformContext *pxfctxt, CXformResult *pxfres,
						   CExpression *pexpr) const;


};	// class CXformLeftSemiApply2LeftSemiJoin

}  // namespace spqopt

#endif	// !SPQOPT_CXformLeftSemiApply2LeftSemiJoin_H

// EOF
