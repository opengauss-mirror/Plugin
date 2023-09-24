//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 Pivotal, Inc.
//
//	@filename:
//		CXformPushGbBelowUnion.h
//
//	@doc:
//		Push grouping below Union operation
//---------------------------------------------------------------------------
#ifndef SPQOPT_CXformPushGbBelowUnion_H
#define SPQOPT_CXformPushGbBelowUnion_H

#include "spqos/base.h"

#include "spqopt/operators/CLogicalUnion.h"
#include "spqopt/xforms/CXformPushGbBelowSetOp.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CXformPushGbBelowUnion
//
//	@doc:
//		Push grouping below Union operation
//
//---------------------------------------------------------------------------
class CXformPushGbBelowUnion : public CXformPushGbBelowSetOp<CLogicalUnion>
{
private:
	// private copy ctor
	CXformPushGbBelowUnion(const CXformPushGbBelowUnion &);

public:
	// ctor
	explicit CXformPushGbBelowUnion(CMemoryPool *mp)
		: CXformPushGbBelowSetOp<CLogicalUnion>(mp)
	{
	}

	// dtor
	virtual ~CXformPushGbBelowUnion()
	{
	}

	// Compatibility function
	virtual BOOL
	FCompatible(CXform::EXformId exfid)
	{
		return ExfPushGbBelowUnion != exfid;
	}

	// ident accessors
	virtual EXformId
	Exfid() const
	{
		return ExfPushGbBelowUnion;
	}

	virtual const CHAR *
	SzId() const
	{
		return "CXformPushGbBelowUnion";
	}

};	// class CXformPushGbBelowUnion

}  // namespace spqopt

#endif	// !SPQOPT_CXformPushGbBelowUnion_H

// EOF
