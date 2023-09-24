//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 EMC Corp.
//
//	@filename:
//		CXformCollapseGbAgg.h
//
//	@doc:
//		Collapse two cascaded GbAgg operators into a single GbAgg
//
//---------------------------------------------------------------------------
#ifndef SPQOPT_CXformCollapseGbAgg_H
#define SPQOPT_CXformCollapseGbAgg_H

#include "spqos/base.h"

#include "spqopt/xforms/CXformExploration.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CXformCollapseGbAgg
//
//	@doc:
//		Collapse two cascaded GbAgg operators into a single GbAgg
//
//---------------------------------------------------------------------------
class CXformCollapseGbAgg : public CXformExploration
{
private:
	// private copy ctor
	CXformCollapseGbAgg(const CXformCollapseGbAgg &);

public:
	// ctor
	explicit CXformCollapseGbAgg(CMemoryPool *mp);

	// dtor
	virtual ~CXformCollapseGbAgg()
	{
	}

	// ident accessors
	virtual EXformId
	Exfid() const
	{
		return ExfCollapseGbAgg;
	}

	// return a string for xform name
	virtual const CHAR *
	SzId() const
	{
		return "CXformCollapseGbAgg";
	}

	// compute xform promise for a given expression handle
	virtual EXformPromise Exfp(CExpressionHandle &exprhdl) const;

	// actual transform
	void Transform(CXformContext *, CXformResult *, CExpression *) const;

};	// class CXformCollapseGbAgg

}  // namespace spqopt

#endif	// !SPQOPT_CXformCollapseGbAgg_H

// EOF
