//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 Pivotal, Inc.
//
//	@filename:
//		CXformMultiExternalGet2MultiExternalScan.h
//
//	@doc:
//		Transform MultiExternalGet to MultiExternalScan
//---------------------------------------------------------------------------
#ifndef SPQOPT_CXformMultiExternalGet2MultiExternalScan_H
#define SPQOPT_CXformMultiExternalGet2MultiExternalScan_H

#include "spqos/base.h"

#include "spqopt/xforms/CXformImplementation.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CXformMultiExternalGet2MultiExternalScan
//
//	@doc:
//		Transform MultiExternalGet to MultiExternalScan
//
//---------------------------------------------------------------------------
class CXformMultiExternalGet2MultiExternalScan : public CXformImplementation
{
private:
	// private copy ctor
	CXformMultiExternalGet2MultiExternalScan(
		const CXformMultiExternalGet2MultiExternalScan &);

public:
	// ctor
	explicit CXformMultiExternalGet2MultiExternalScan(CMemoryPool *);

	// dtor
	virtual ~CXformMultiExternalGet2MultiExternalScan()
	{
	}

	// ident accessors
	virtual EXformId
	Exfid() const
	{
		return ExfMultiExternalGet2MultiExternalScan;
	}

	// return a string for xform name
	virtual const CHAR *
	SzId() const
	{
		return "CXformMultiExternalGet2MultiExternalScan";
	}

	// compute xform promise for a given expression handle
	virtual EXformPromise Exfp(CExpressionHandle &exprhdl) const;

	// actual transform
	virtual void Transform(CXformContext *pxfctxt, CXformResult *pxfres,
						   CExpression *pexpr) const;

};	// class CXformMultiExternalGet2MultiExternalScan

}  // namespace spqopt

#endif	// !SPQOPT_CXformMultiExternalGet2MultiExternalScan_H

// EOF
