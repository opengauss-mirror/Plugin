//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 Pivotal, Inc.
//
//	@filename:
//		CXformGbAggDedup2StreamAggDedup.h
//
//	@doc:
//		Transform GbAggDeduplicate to StreamAggDeduplicate
//---------------------------------------------------------------------------
#ifndef SPQOPT_CXformGbAggDedup2StreamAggDedup_H
#define SPQOPT_CXformGbAggDedup2StreamAggDedup_H

#include "spqos/base.h"

#include "spqopt/xforms/CXformGbAgg2StreamAgg.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CXformGbAggDedup2StreamAggDedup
//
//	@doc:
//		Transform GbAggDeduplicate to StreamAggDeduplicate
//
//---------------------------------------------------------------------------
class CXformGbAggDedup2StreamAggDedup : public CXformGbAgg2StreamAgg
{
private:
	// private copy ctor
	CXformGbAggDedup2StreamAggDedup(const CXformGbAggDedup2StreamAggDedup &);

public:
	// ctor
	CXformGbAggDedup2StreamAggDedup(CMemoryPool *mp);

	// dtor
	virtual ~CXformGbAggDedup2StreamAggDedup()
	{
	}

	// ident accessors
	virtual EXformId
	Exfid() const
	{
		return ExfGbAggDedup2StreamAggDedup;
	}

	// return a string for xform name
	virtual const CHAR *
	SzId() const
	{
		return "CXformGbAggDedup2StreamAggDedup";
	}

	// actual transform
	virtual void Transform(CXformContext *pxfctxt, CXformResult *pxfres,
						   CExpression *pexpr) const;

};	// class CXformGbAggDedup2StreamAggDedup

}  // namespace spqopt


#endif	// !SPQOPT_CXformGbAggDedup2StreamAggDedup_H

// EOF
