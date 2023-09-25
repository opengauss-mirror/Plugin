//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp
//
//	@filename:
//		CXformUpdate2DML.h
//
//	@doc:
//		Transform Logical Update to Logical DML
//---------------------------------------------------------------------------
#ifndef SPQOPT_CXformUpdate2DML_H
#define SPQOPT_CXformUpdate2DML_H

#include "spqos/base.h"

#include "spqopt/xforms/CXformExploration.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CXformUpdate2DML
//
//	@doc:
//		Transform Logical Update to Logical DML
//
//---------------------------------------------------------------------------
class CXformUpdate2DML : public CXformExploration
{
private:
	// private copy ctor
	CXformUpdate2DML(const CXformUpdate2DML &);

public:
	// ctor
	explicit CXformUpdate2DML(CMemoryPool *mp);

	// dtor
	virtual ~CXformUpdate2DML()
	{
	}

	// ident accessors
	virtual EXformId
	Exfid() const
	{
		return ExfUpdate2DML;
	}

	// return a string for xform name
	virtual const CHAR *
	SzId() const
	{
		return "CXformUpdate2DML";
	}

	// compute xform promise for a given expression handle
	virtual EXformPromise Exfp(CExpressionHandle &exprhdl) const;

	// actual transform
	virtual void Transform(CXformContext *pxfctxt, CXformResult *pxfres,
						   CExpression *pexpr) const;

};	// class CXformUpdate2DML
}  // namespace spqopt

#endif	// !SPQOPT_CXformUpdate2DML_H

// EOF
