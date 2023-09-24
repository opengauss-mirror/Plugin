//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp
//
//	@filename:
//		CXformDelete2DML.h
//
//	@doc:
//		Transform Logical Delete to Logical DML
//---------------------------------------------------------------------------
#ifndef SPQOPT_CXformDelete2DML_H
#define SPQOPT_CXformDelete2DML_H

#include "spqos/base.h"

#include "spqopt/xforms/CXformExploration.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CXformDelete2DML
//
//	@doc:
//		Transform Logical Delete to Logical DML
//
//---------------------------------------------------------------------------
class CXformDelete2DML : public CXformExploration
{
private:
	// private copy ctor
	CXformDelete2DML(const CXformDelete2DML &);

public:
	// ctor
	explicit CXformDelete2DML(CMemoryPool *mp);

	// dtor
	virtual ~CXformDelete2DML()
	{
	}

	// ident accessors
	virtual EXformId
	Exfid() const
	{
		return ExfDelete2DML;
	}

	// return a string for xform name
	virtual const CHAR *
	SzId() const
	{
		return "CXformDelete2DML";
	}

	// compute xform promise for a given expression handle
	virtual EXformPromise Exfp(CExpressionHandle &exprhdl) const;

	// actual transform
	virtual void Transform(CXformContext *pxfctxt, CXformResult *pxfres,
						   CExpression *pexpr) const;

};	// class CXformDelete2DML
}  // namespace spqopt

#endif	// !SPQOPT_CXformDelete2DML_H

// EOF
