//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 Pivotal, Inc.
//
//	@filename:
//		CXformExternalGet2ExternalScan.h
//
//	@doc:
//		Transform ExternalGet to ExternalScan
//---------------------------------------------------------------------------
#ifndef SPQOPT_CXformExternalGet2ExternalScan_H
#define SPQOPT_CXformExternalGet2ExternalScan_H

#include "spqos/base.h"

#include "spqopt/xforms/CXformImplementation.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CXformExternalGet2ExternalScan
//
//	@doc:
//		Transform ExternalGet to ExternalScan
//
//---------------------------------------------------------------------------
class CXformExternalGet2ExternalScan : public CXformImplementation
{
private:
	// private copy ctor
	CXformExternalGet2ExternalScan(const CXformExternalGet2ExternalScan &);

public:
	// ctor
	explicit CXformExternalGet2ExternalScan(CMemoryPool *);

	// dtor
	virtual ~CXformExternalGet2ExternalScan()
	{
	}

	// ident accessors
	virtual EXformId
	Exfid() const
	{
		return ExfExternalGet2ExternalScan;
	}

	// return a string for xform name
	virtual const CHAR *
	SzId() const
	{
		return "CXformExternalGet2ExternalScan";
	}

	// compute xform promise for a given expression handle
	virtual EXformPromise Exfp(CExpressionHandle &exprhdl) const;

	// actual transform
	virtual void Transform(CXformContext *pxfctxt, CXformResult *pxfres,
						   CExpression *pexpr) const;

};	// class CXformExternalGet2ExternalScan

}  // namespace spqopt

#endif	// !SPQOPT_CXformExternalGet2ExternalScan_H

// EOF
