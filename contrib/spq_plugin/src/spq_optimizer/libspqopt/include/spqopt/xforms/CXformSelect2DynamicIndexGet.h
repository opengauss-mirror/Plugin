//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CXformSelect2DynamicIndexGet.h
//
//	@doc:
//		Transform select over partitioned table into a dynamic index get
//---------------------------------------------------------------------------
#ifndef SPQOPT_CXformSelect2DynamicIndexGet_H
#define SPQOPT_CXformSelect2DynamicIndexGet_H

#include "spqos/base.h"

#include "spqopt/operators/CLogicalGet.h"
#include "spqopt/xforms/CXformExploration.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CXformSelect2DynamicIndexGet
//
//	@doc:
//		Transform select over a partitioned table into a dynamic index get
//
//---------------------------------------------------------------------------
class CXformSelect2DynamicIndexGet : public CXformExploration
{
private:
	// private copy ctor
	CXformSelect2DynamicIndexGet(const CXformSelect2DynamicIndexGet &);

	// return the column reference set of included / key columns
	CColRefSet *GetColRefSet(CMemoryPool *mp, CLogicalGet *popGet,
							 const IMDIndex *pmdindex,
							 BOOL fIncludedColumns) const;

public:
	// ctor
	explicit CXformSelect2DynamicIndexGet(CMemoryPool *mp);

	// dtor
	virtual ~CXformSelect2DynamicIndexGet()
	{
	}

	// ident accessors
	virtual EXformId
	Exfid() const
	{
		return ExfSelect2DynamicIndexGet;
	}

	// xform name
	virtual const CHAR *
	SzId() const
	{
		return "CXformSelect2DynamicIndexGet";
	}

	// compute xform promise for a given expression handle
	virtual EXformPromise Exfp(CExpressionHandle &exprhdl) const;

	// actual transform
	void Transform(CXformContext *pxfctxt, CXformResult *pxfres,
				   CExpression *pexpr) const;


};	// class CXformSelect2DynamicIndexGet

}  // namespace spqopt

#endif	// !SPQOPT_CXformSelect2DynamicIndexGet_H

// EOF
