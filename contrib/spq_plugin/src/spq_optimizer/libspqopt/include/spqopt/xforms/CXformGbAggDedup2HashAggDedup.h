//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 Pivotal, Inc.
//
//	@filename:
//		CXformGbAggDedup2HashAggDedup.h
//
//	@doc:
//		Transform GbAggDeduplicate to HashAggDeduplicate
//---------------------------------------------------------------------------
#ifndef SPQOPT_CXformGbAggDedup2HashAggDedup_H
#define SPQOPT_CXformGbAggDedup2HashAggDedup_H

#include "spqos/base.h"

#include "spqopt/xforms/CXformGbAgg2HashAgg.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CXformGbAggDedup2HashAggDedup
//
//	@doc:
//		Transform GbAggDeduplicate to HashAggDeduplicate
//
//---------------------------------------------------------------------------
class CXformGbAggDedup2HashAggDedup : public CXformGbAgg2HashAgg
{
private:
	// private copy ctor
	CXformGbAggDedup2HashAggDedup(const CXformGbAggDedup2HashAggDedup &);

public:
	// ctor
	CXformGbAggDedup2HashAggDedup(CMemoryPool *mp);

	// dtor
	virtual ~CXformGbAggDedup2HashAggDedup()
	{
	}

	// ident accessors
	virtual EXformId
	Exfid() const
	{
		return ExfGbAggDedup2HashAggDedup;
	}

	// return a string for xform name
	virtual const CHAR *
	SzId() const
	{
		return "CXformGbAggDedup2HashAggDedup";
	}

	// actual transform
	virtual void Transform(CXformContext *pxfctxt, CXformResult *pxfres,
						   CExpression *pexpr) const;

};	// class CXformGbAggDedup2HashAggDedup

}  // namespace spqopt


#endif	// !SPQOPT_CXformGbAggDedup2HashAggDedup_H

// EOF
