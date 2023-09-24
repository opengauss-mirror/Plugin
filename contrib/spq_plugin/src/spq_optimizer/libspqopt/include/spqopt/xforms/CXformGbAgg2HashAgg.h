//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		CXformGbAgg2HashAgg.h
//
//	@doc:
//		Transform GbAgg to HashAgg
//---------------------------------------------------------------------------
#ifndef SPQOPT_CXformGbAgg2HashAgg_H
#define SPQOPT_CXformGbAgg2HashAgg_H

#include "spqos/base.h"

#include "spqopt/xforms/CXformImplementation.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CXformGbAgg2HashAgg
//
//	@doc:
//		Transform Get to TableScan
//
//---------------------------------------------------------------------------
class CXformGbAgg2HashAgg : public CXformImplementation
{
private:
	// private copy ctor
	CXformGbAgg2HashAgg(const CXformGbAgg2HashAgg &);

protected:
	// check if the transformation is applicable
	BOOL FApplicable(CExpression *pexpr) const;

public:
	// ctor
	CXformGbAgg2HashAgg(CMemoryPool *mp);

	// ctor
	explicit CXformGbAgg2HashAgg(CExpression *pexprPattern);

	// dtor
	virtual ~CXformGbAgg2HashAgg()
	{
	}

	// ident accessors
	virtual EXformId
	Exfid() const
	{
		return ExfGbAgg2HashAgg;
	}

	// return a string for xform name
	virtual const CHAR *
	SzId() const
	{
		return "CXformGbAgg2HashAgg";
	}

	// compute xform promise for a given expression handle
	virtual EXformPromise Exfp(CExpressionHandle &exprhdl) const;

	// actual transform
	void Transform(CXformContext *pxfctxt, CXformResult *pxfres,
				   CExpression *pexpr) const;

};	// class CXformGbAgg2HashAgg

}  // namespace spqopt


#endif	// !SPQOPT_CXformGbAgg2HashAgg_H

// EOF
