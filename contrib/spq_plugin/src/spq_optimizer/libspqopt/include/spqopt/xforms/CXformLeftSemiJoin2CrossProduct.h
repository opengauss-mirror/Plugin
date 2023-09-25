//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 EMC Corp.
//
//	@filename:
//		CXformLeftSemiJoin2CrossProduct.h
//
//	@doc:
//		Transform left semi join to cross product
//---------------------------------------------------------------------------
#ifndef SPQOPT_CXformLeftSemiJoin2CrossProduct_H
#define SPQOPT_CXformLeftSemiJoin2CrossProduct_H

#include "spqos/base.h"

#include "spqopt/xforms/CXformExploration.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CXformLeftSemiJoin2CrossProduct
//
//	@doc:
//		Transform left semi join to cross product
//
//---------------------------------------------------------------------------
class CXformLeftSemiJoin2CrossProduct : public CXformExploration
{
private:
	// private copy ctor
	CXformLeftSemiJoin2CrossProduct(const CXformLeftSemiJoin2CrossProduct &);

public:
	// ctor
	explicit CXformLeftSemiJoin2CrossProduct(CMemoryPool *mp);

	// dtor
	virtual ~CXformLeftSemiJoin2CrossProduct()
	{
	}

	// ident accessors
	virtual EXformId
	Exfid() const
	{
		return ExfLeftSemiJoin2CrossProduct;
	}

	// return a string for xform name
	virtual const CHAR *
	SzId() const
	{
		return "CXformLeftSemiJoin2CrossProduct";
	}

	// compute xform promise for a given expression handle
	virtual EXformPromise Exfp(CExpressionHandle &exprhdl) const;

	// actual transform
	void Transform(CXformContext *pxfctxt, CXformResult *pxfres,
				   CExpression *pexpr) const;

};	// class CXformLeftSemiJoin2CrossProduct

}  // namespace spqopt


#endif	// !SPQOPT_CXformLeftSemiJoin2CrossProduct_H

// EOF
