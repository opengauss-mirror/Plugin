//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CXformLeftOuterApply2LeftOuterJoinNoCorrelations.h
//
//	@doc:
//		Turn inner Apply into Inner Join when Apply's inner child has no
//		correlations
//---------------------------------------------------------------------------
#ifndef SPQOPT_CXformLeftOuterApply2LeftOuterJoinNoCorrelations_H
#define SPQOPT_CXformLeftOuterApply2LeftOuterJoinNoCorrelations_H

#include "spqos/base.h"

#include "spqopt/operators/CLogicalLeftOuterApply.h"
#include "spqopt/operators/CLogicalLeftOuterJoin.h"
#include "spqopt/xforms/CXformApply2Join.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CXformLeftOuterApply2LeftOuterJoinNoCorrelations
//
//	@doc:
//		Transform left outer apply into left outer join
//
//---------------------------------------------------------------------------
class CXformLeftOuterApply2LeftOuterJoinNoCorrelations
	: public CXformApply2Join<CLogicalLeftOuterApply, CLogicalLeftOuterJoin>
{
private:
	// private copy ctor
	CXformLeftOuterApply2LeftOuterJoinNoCorrelations(
		const CXformLeftOuterApply2LeftOuterJoinNoCorrelations &);

public:
	// ctor
	explicit CXformLeftOuterApply2LeftOuterJoinNoCorrelations(CMemoryPool *mp)
		: CXformApply2Join<CLogicalLeftOuterApply, CLogicalLeftOuterJoin>(mp)
	{
	}

	// dtor
	virtual ~CXformLeftOuterApply2LeftOuterJoinNoCorrelations()
	{
	}

	// ident accessors
	virtual EXformId
	Exfid() const
	{
		return ExfLeftOuterApply2LeftOuterJoinNoCorrelations;
	}

	virtual const CHAR *
	SzId() const
	{
		return "CXformLeftOuterApply2LeftOuterJoinNoCorrelations";
	}

	// compute xform promise for a given expression handle
	virtual EXformPromise Exfp(CExpressionHandle &exprhdl) const;

	// actual transform
	void Transform(CXformContext *pxfctxt, CXformResult *pxfres,
				   CExpression *pexpr) const;

};	// class CXformLeftOuterApply2LeftOuterJoinNoCorrelations

}  // namespace spqopt

#endif	// !SPQOPT_CXformLeftOuterApply2LeftOuterJoinNoCorrelations_H

// EOF
