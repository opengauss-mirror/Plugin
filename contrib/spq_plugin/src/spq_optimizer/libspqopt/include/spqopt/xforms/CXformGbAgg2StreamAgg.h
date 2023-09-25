//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CXformGbAgg2StreamAgg.h
//
//	@doc:
//		Transform GbAgg to StreamAgg
//---------------------------------------------------------------------------
#ifndef SPQOPT_CXformGbAgg2StreamAgg_H
#define SPQOPT_CXformGbAgg2StreamAgg_H

#include "spqos/base.h"

#include "spqopt/xforms/CXformImplementation.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CXformGbAgg2StreamAgg
//
//	@doc:
//		Transform GbAgg to Stream Agg
//
//---------------------------------------------------------------------------
class CXformGbAgg2StreamAgg : public CXformImplementation
{
private:
	// private copy ctor
	CXformGbAgg2StreamAgg(const CXformGbAgg2StreamAgg &);

public:
	// ctor
	CXformGbAgg2StreamAgg(CMemoryPool *mp);

	// ctor
	explicit CXformGbAgg2StreamAgg(CExpression *pexprPattern);

	// dtor
	virtual ~CXformGbAgg2StreamAgg()
	{
	}

	// ident accessors
	virtual EXformId
	Exfid() const
	{
		return ExfGbAgg2StreamAgg;
	}

	// return a string for xform name
	virtual const CHAR *
	SzId() const
	{
		return "CXformGbAgg2StreamAgg";
	}

	// compute xform promise for a given expression handle
	virtual EXformPromise Exfp(CExpressionHandle &exprhdl) const;

	// actual transform
	void Transform(CXformContext *pxfctxt, CXformResult *pxfres,
				   CExpression *pexpr) const;

};	// class CXformGbAgg2StreamAgg

}  // namespace spqopt


#endif	// !SPQOPT_CXformGbAgg2StreamAgg_H

// EOF
