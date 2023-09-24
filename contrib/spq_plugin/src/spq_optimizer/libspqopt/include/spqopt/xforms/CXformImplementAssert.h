//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CXformImplementAssert.h
//
//	@doc:
//		Implement Assert
//---------------------------------------------------------------------------
#ifndef SPQOPT_CXformImplementAssert_H
#define SPQOPT_CXformImplementAssert_H

#include "spqos/base.h"

#include "spqopt/xforms/CXformImplementation.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CXformImplementAssert
//
//	@doc:
//		Implement Assert
//
//---------------------------------------------------------------------------
class CXformImplementAssert : public CXformImplementation
{
private:
	// private copy ctor
	CXformImplementAssert(const CXformImplementAssert &);

public:
	// ctor
	explicit CXformImplementAssert(CMemoryPool *mp);

	// dtor
	virtual ~CXformImplementAssert()
	{
	}

	// ident accessors
	virtual EXformId
	Exfid() const
	{
		return ExfImplementAssert;
	}

	// xform name
	virtual const CHAR *
	SzId() const
	{
		return "CXformImplementAssert";
	}

	// compute xform promise for a given expression handle
	virtual EXformPromise Exfp(CExpressionHandle &exprhdl) const;

	// actual transform
	virtual void Transform(CXformContext *, CXformResult *,
						   CExpression *) const;

};	// class CXformImplementAssert

}  // namespace spqopt

#endif	// !SPQOPT_CXformImplementAssert_H

// EOF
