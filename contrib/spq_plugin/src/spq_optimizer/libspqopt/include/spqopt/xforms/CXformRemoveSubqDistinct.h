//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2017 Pivotal Software Inc.
//
//	@filename:
//		CXformRemoveSubqDistinct.h
//
//	@doc:
//		Transform that removes distinct clause from subquery
//---------------------------------------------------------------------------
#ifndef SPQOPT_CXformRemoveSubqDistinct_H
#define SPQOPT_CXformRemoveSubqDistinct_H

#include "spqos/base.h"

#include "spqopt/xforms/CXformSimplifySubquery.h"

namespace spqopt
{
using namespace spqos;

class CXformRemoveSubqDistinct : public CXformExploration
{
private:
	// private copy ctor
	CXformRemoveSubqDistinct(const CXformRemoveSubqDistinct &);

public:
	// ctor
	explicit CXformRemoveSubqDistinct(CMemoryPool *mp);

	// dtor
	virtual ~CXformRemoveSubqDistinct()
	{
	}

	// ident accessors
	virtual EXformId
	Exfid() const
	{
		return ExfRemoveSubqDistinct;
	}

	// return a string for xform name
	virtual const CHAR *
	SzId() const
	{
		return "CXformRemoveSubqDistinct";
	}

	// compute xform promise for a given expression handle
	virtual EXformPromise Exfp(CExpressionHandle &exprhdl) const;

	// actual transform
	virtual void Transform(CXformContext *pxfctxt, CXformResult *pxfres,
						   CExpression *pexpr) const;

};	// class CXformRemoveSubqDistinct

}  // namespace spqopt

#endif	// !SPQOPT_CXformRemoveSubqDistinct_H

// EOF
