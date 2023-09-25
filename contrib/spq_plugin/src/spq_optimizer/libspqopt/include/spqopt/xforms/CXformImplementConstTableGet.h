//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CXformImplementConstTableGet.h
//
//	@doc:
//		Implement logical const table with a physical const table get
//---------------------------------------------------------------------------
#ifndef SPQOPT_CXformImplementConstTableGet_H
#define SPQOPT_CXformImplementConstTableGet_H

#include "spqos/base.h"

#include "spqopt/xforms/CXformImplementation.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CXformImplementConstTableGet
//
//	@doc:
//		Implement const table get
//
//---------------------------------------------------------------------------
class CXformImplementConstTableGet : public CXformImplementation
{
private:
	// private copy ctor
	CXformImplementConstTableGet(const CXformImplementConstTableGet &);

public:
	// ctor
	explicit CXformImplementConstTableGet(CMemoryPool *);

	// dtor
	virtual ~CXformImplementConstTableGet()
	{
	}

	// ident accessors
	virtual EXformId
	Exfid() const
	{
		return ExfImplementConstTableGet;
	}

	// return a string for xform name
	virtual const CHAR *
	SzId() const
	{
		return "CXformImplementConstTableGet";
	}

	// compute xform promise for a given expression handle
	virtual EXformPromise
	Exfp(CExpressionHandle &  // exprhdl
	) const
	{
		return CXform::ExfpHigh;
	}

	// actual transform
	void Transform(CXformContext *pxfctxt, CXformResult *pxfres,
				   CExpression *pexpr) const;

};	// class CXformImplementConstTableGet

}  // namespace spqopt


#endif	// !SPQOPT_CXformImplementConstTableGet_H

// EOF
