//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2020 VMware, Inc.
//
//	@filename:
//		CXformIndexGet2IndexOnlyScan.h
//
//	@doc:
//		Transform Index Get to Index Only Scan
//---------------------------------------------------------------------------
#ifndef SPQOPT_CXformIndexGet2IndexOnlyScan_H
#define SPQOPT_CXformIndexGet2IndexOnlyScan_H

#include "spqos/base.h"

#include "spqopt/xforms/CXformImplementation.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CXformIndexGet2IndexOnlyScan
//
//	@doc:
//		Transform Index Get to Index Scan
//
//---------------------------------------------------------------------------
class CXformIndexGet2IndexOnlyScan : public CXformImplementation
{
private:
	// private copy ctor
	CXformIndexGet2IndexOnlyScan(const CXformIndexGet2IndexOnlyScan &);

public:
	// ctor
	explicit CXformIndexGet2IndexOnlyScan(CMemoryPool *);

	// dtor
	virtual ~CXformIndexGet2IndexOnlyScan()
	{
	}

	// ident accessors
	virtual EXformId
	Exfid() const
	{
		return ExfIndexGet2IndexOnlyScan;
	}

	// xform name
	virtual const CHAR *
	SzId() const
	{
		return "CXformIndexGet2IndexOnlyScan";
	}

	// compute xform promise for a given expression handle
	virtual EXformPromise Exfp(CExpressionHandle &	//exprhdl
	) const;

	// actual transform
	void Transform(CXformContext *pxfctxt, CXformResult *pxfres,
				   CExpression *pexpr) const;

};	// class CXformIndexGet2IndexOnlyScan

}  // namespace spqopt

#endif	// !SPQOPT_CXformIndexGet2IndexOnlyScan_H

// EOF
