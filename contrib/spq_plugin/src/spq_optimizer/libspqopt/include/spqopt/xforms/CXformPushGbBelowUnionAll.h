//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 Pivotal, Inc.
//
//	@filename:
//		CXformPushGbBelowUnionAll.h
//
//	@doc:
//		Push grouping below UnionAll operation
//---------------------------------------------------------------------------
#ifndef SPQOPT_CXformPushGbBelowUnionAll_H
#define SPQOPT_CXformPushGbBelowUnionAll_H

#include "spqos/base.h"

#include "spqopt/operators/CLogicalUnionAll.h"
#include "spqopt/xforms/CXformPushGbBelowSetOp.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CXformPushGbBelowUnionAll
//
//	@doc:
//		Push grouping below UnionAll operation
//
//---------------------------------------------------------------------------
class CXformPushGbBelowUnionAll
	: public CXformPushGbBelowSetOp<CLogicalUnionAll>
{
private:
	// private copy ctor
	CXformPushGbBelowUnionAll(const CXformPushGbBelowUnionAll &);

public:
	// ctor
	explicit CXformPushGbBelowUnionAll(CMemoryPool *mp)
		: CXformPushGbBelowSetOp<CLogicalUnionAll>(mp)
	{
	}

	// dtor
	virtual ~CXformPushGbBelowUnionAll()
	{
	}

	// Compatibility function
	virtual BOOL
	FCompatible(CXform::EXformId exfid)
	{
		return ExfPushGbBelowUnionAll != exfid;
	}

	// ident accessors
	virtual EXformId
	Exfid() const
	{
		return ExfPushGbBelowUnionAll;
	}

	virtual const CHAR *
	SzId() const
	{
		return "CXformPushGbBelowUnionAll";
	}

};	// class CXformPushGbBelowUnionAll

}  // namespace spqopt

#endif	// !SPQOPT_CXformPushGbBelowUnionAll_H

// EOF
