//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CXformImplementLimit.h
//
//	@doc:
//		Transform Logical into Physical Limit
//---------------------------------------------------------------------------
#ifndef SPQOPT_CXformImplementLimit_H
#define SPQOPT_CXformImplementLimit_H

#include "spqos/base.h"

#include "spqopt/xforms/CXformImplementation.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CXformImplementLimit
//
//	@doc:
//		Transform Logical into Physical Limit
//
//---------------------------------------------------------------------------
class CXformImplementLimit : public CXformImplementation
{
private:
	// private copy ctor
	CXformImplementLimit(const CXformImplementLimit &);

public:
	// ctor
	explicit CXformImplementLimit(CMemoryPool *mp);

	// dtor
	virtual ~CXformImplementLimit()
	{
	}

	// ident accessors
	virtual EXformId
	Exfid() const
	{
		return ExfImplementLimit;
	}

	virtual const CHAR *
	SzId() const
	{
		return "CXformImplementLimit";
	}

	// compute xform promise for a given expression handle
	virtual EXformPromise Exfp(CExpressionHandle &exprhdl) const;

	// actual transform
	void Transform(CXformContext *, CXformResult *, CExpression *) const;

};	// class CXformImplementLimit

}  // namespace spqopt

#endif	// !SPQOPT_CXformImplementLimit_H

// EOF
