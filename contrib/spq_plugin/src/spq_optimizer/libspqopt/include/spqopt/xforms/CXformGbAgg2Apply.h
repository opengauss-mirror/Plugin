//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CXformGbAgg2Apply.h
//
//	@doc:
//		Transform GbAgg to Apply
//---------------------------------------------------------------------------
#ifndef SPQOPT_CXformGbAgg2Apply_H
#define SPQOPT_CXformGbAgg2Apply_H

#include "spqos/base.h"

#include "spqopt/xforms/CXformSubqueryUnnest.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CXformGbAgg2Apply
//
//	@doc:
//		Transform GbAgg to Apply; this transformation is only applicable
//		to GbAgg expression with aggregate functions that have subquery
//		arguments
//
//---------------------------------------------------------------------------
class CXformGbAgg2Apply : public CXformSubqueryUnnest
{
private:
	// private copy ctor
	CXformGbAgg2Apply(const CXformGbAgg2Apply &);

public:
	// ctor
	explicit CXformGbAgg2Apply(CMemoryPool *mp);

	// dtor
	virtual ~CXformGbAgg2Apply()
	{
	}

	// ident accessors
	virtual EXformId
	Exfid() const
	{
		return ExfGbAgg2Apply;
	}

	// return a string for xform name
	virtual const CHAR *
	SzId() const
	{
		return "CXformGbAgg2Apply";
	}

	// compute xform promise for a given expression handle
	virtual EXformPromise Exfp(CExpressionHandle &exprhdl) const;

};	// class CXformGbAgg2Apply

}  // namespace spqopt

#endif	// !SPQOPT_CXformGbAgg2Apply_H

// EOF
