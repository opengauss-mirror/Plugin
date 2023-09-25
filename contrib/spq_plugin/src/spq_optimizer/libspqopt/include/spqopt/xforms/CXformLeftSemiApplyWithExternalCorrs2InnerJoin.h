//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 EMC Corp.
//
//	@filename:
//		CXformLeftSemiApplyWithExternalCorrs2InnerJoin.h
//
//	@doc:
//		Turn LS apply with external correlations apply into inner join;
//		external correlations are correlations in the inner child of LSA
//		that use columns not defined by the outer child of LSA
//---------------------------------------------------------------------------
#ifndef SPQOPT_CXformLeftSemiApplyWithExternalCorrs2InnerJoin_H
#define SPQOPT_CXformLeftSemiApplyWithExternalCorrs2InnerJoin_H

#include "spqos/base.h"

#include "spqopt/operators/CLogicalInnerJoin.h"
#include "spqopt/operators/CLogicalLeftSemiApply.h"
#include "spqopt/xforms/CXformApply2Join.h"
#include "spqopt/xforms/CXformUtils.h"


namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CXformLeftSemiApplyWithExternalCorrs2InnerJoin
//
//	@doc:
//		Transform Apply into Join by decorrelating the inner side
//
//---------------------------------------------------------------------------
class CXformLeftSemiApplyWithExternalCorrs2InnerJoin
	: public CXformApply2Join<CLogicalLeftSemiApply, CLogicalInnerJoin>
{
private:
	// private copy ctor
	CXformLeftSemiApplyWithExternalCorrs2InnerJoin(
		const CXformLeftSemiApplyWithExternalCorrs2InnerJoin &);

	// helper for splitting correlations into external and residual
	static BOOL FSplitCorrelations(CMemoryPool *mp, CExpression *pexprOuter,
								   CExpression *pexprInner,
								   CExpressionArray *pdrspqexprAllCorr,
								   CExpressionArray **ppdrspqexprExternal,
								   CExpressionArray **ppdrspqexprResidual,
								   CColRefSet **ppcrsInnerUsed);

	// helper for collecting correlations
	static BOOL FDecorrelate(CMemoryPool *mp, CExpression *pexpr,
							 CExpression **ppexprInnerNew,
							 CExpressionArray **ppdrspqexprCorr);

	// decorrelate semi apply with external correlations
	static CExpression *PexprDecorrelate(CMemoryPool *mp, CExpression *pexpr);

public:
	// ctor
	explicit CXformLeftSemiApplyWithExternalCorrs2InnerJoin(CMemoryPool *mp)
		: CXformApply2Join<CLogicalLeftSemiApply, CLogicalInnerJoin>(
			  mp, true /*fDeepTree*/)
	{
	}

	// ctor with a passed pattern
	CXformLeftSemiApplyWithExternalCorrs2InnerJoin(CMemoryPool *mp,
												   CExpression *pexprPattern)
		: CXformApply2Join<CLogicalLeftSemiApply, CLogicalInnerJoin>(
			  mp, pexprPattern)
	{
	}

	// dtor
	virtual ~CXformLeftSemiApplyWithExternalCorrs2InnerJoin()
	{
	}

	// ident accessors
	virtual EXformId
	Exfid() const
	{
		return ExfLeftSemiApplyWithExternalCorrs2InnerJoin;
	}

	virtual const CHAR *
	SzId() const
	{
		return "CXformLeftSemiApplyWithExternalCorrs2InnerJoin";
	}

	// compute xform promise for a given expression handle
	virtual EXformPromise Exfp(CExpressionHandle &exprhdl) const;

	// actual transform
	void Transform(CXformContext *pxfctxt, CXformResult *pxfres,
				   CExpression *pexpr) const;

};	// class CXformLeftSemiApplyWithExternalCorrs2InnerJoin

}  // namespace spqopt

#endif	// !SPQOPT_CXformLeftSemiApplyWithExternalCorrs2InnerJoin_H

// EOF
