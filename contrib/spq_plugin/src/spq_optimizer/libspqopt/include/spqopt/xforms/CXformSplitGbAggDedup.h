//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 Pivotal, Inc.
//
//	@filename:
//		CXformSplitGbAggDedup.h
//
//	@doc:
//		Split a dedup aggregate into a pair of local and global dedup aggregates
//---------------------------------------------------------------------------
#ifndef SPQOPT_CXformSplitGbAggDedup_H
#define SPQOPT_CXformSplitGbAggDedup_H

#include "spqos/base.h"

#include "spqopt/xforms/CXformSplitGbAgg.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CXformSplitGbAggDedup
//
//	@doc:
//		Split a dedup aggregate operator into pair of local and global aggregates
//
//---------------------------------------------------------------------------
class CXformSplitGbAggDedup : public CXformSplitGbAgg
{
private:
	// private copy ctor
	CXformSplitGbAggDedup(const CXformSplitGbAggDedup &);

public:
	// ctor
	explicit CXformSplitGbAggDedup(CMemoryPool *mp);

	// dtor
	virtual ~CXformSplitGbAggDedup()
	{
	}

	// ident accessors
	virtual EXformId
	Exfid() const
	{
		return ExfSplitGbAggDedup;
	}

	// return a string for xform name
	virtual const CHAR *
	SzId() const
	{
		return "CXformSplitGbAggDedup";
	}

	// Compatibility function for splitting aggregates
	virtual BOOL
	FCompatible(CXform::EXformId exfid)
	{
		return (CXform::ExfSplitGbAggDedup != exfid);
	}

	// actual transform
	virtual void Transform(CXformContext *pxfctxt, CXformResult *pxfres,
						   CExpression *pexpr) const;

};	// class CXformSplitGbAggDedup

}  // namespace spqopt

#endif	// !SPQOPT_CXformSplitGbAggDedup_H

// EOF
