//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CXformInnerApply2InnerJoinNoCorrelations.h
//
//	@doc:
//		Turn inner Apply into Inner Join when Apply's inner child has no
//		correlations
//---------------------------------------------------------------------------
#ifndef SPQOPT_CXformInnerApply2InnerJoinNoCorrelations_H
#define SPQOPT_CXformInnerApply2InnerJoinNoCorrelations_H

#include "spqos/base.h"

#include "spqopt/operators/CLogicalInnerApply.h"
#include "spqopt/operators/CLogicalInnerJoin.h"
#include "spqopt/xforms/CXformApply2Join.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CXformInnerApply2InnerJoinNoCorrelations
//
//	@doc:
//		Transform inner apply into inner join
//
//---------------------------------------------------------------------------
class CXformInnerApply2InnerJoinNoCorrelations
	: public CXformApply2Join<CLogicalInnerApply, CLogicalInnerJoin>
{
private:
	// private copy ctor
	CXformInnerApply2InnerJoinNoCorrelations(
		const CXformInnerApply2InnerJoinNoCorrelations &);

public:
	// ctor
	explicit CXformInnerApply2InnerJoinNoCorrelations(CMemoryPool *mp)
		: CXformApply2Join<CLogicalInnerApply, CLogicalInnerJoin>(mp)
	{
	}

	// dtor
	virtual ~CXformInnerApply2InnerJoinNoCorrelations()
	{
	}

	// ident accessors
	virtual EXformId
	Exfid() const
	{
		return ExfInnerApply2InnerJoinNoCorrelations;
	}

	virtual const CHAR *
	SzId() const
	{
		return "CXformInnerApply2InnerJoinNoCorrelations";
	}

	// compute xform promise for a given expression handle
	virtual EXformPromise Exfp(CExpressionHandle &exprhdl) const;

	// actual transform
	void Transform(CXformContext *pxfctxt, CXformResult *pxfres,
				   CExpression *pexpr) const;

};	// class CXformInnerApply2InnerJoinNoCorrelations

}  // namespace spqopt

#endif	// !SPQOPT_CXformInnerApply2InnerJoinNoCorrelations_H

// EOF
