//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 Pivotal Inc.
//
//	@filename:
//		CXformSplitDQA.h
//
//	@doc:
//		Split an aggregate into a three level aggregation
//---------------------------------------------------------------------------
#ifndef SPQOPT_CXformSplitDQA_H
#define SPQOPT_CXformSplitDQA_H

#include "spqos/base.h"

#include "spqopt/base/CUtils.h"
#include "spqopt/operators/CLogicalGbAgg.h"
#include "spqopt/xforms/CXformExploration.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CXformSplitDQA
//
//	@doc:
//		Split an aggregate into a three level aggregation
//
//---------------------------------------------------------------------------
class CXformSplitDQA : public CXformExploration
{
private:
	// hash map between expression and a column reference
	typedef CHashMap<CExpression, CColRef, CExpression::HashValue,
					 CUtils::Equals, CleanupRelease<CExpression>,
					 CleanupNULL<CColRef> >
		ExprToColRefMap;

	// private copy ctor
	CXformSplitDQA(const CXformSplitDQA &);

	// generate an expression with multi-level aggregation
	static CExpression *PexprMultiLevelAggregation(
		CMemoryPool *mp, CExpression *pexprRelational,
		CExpressionArray *pdrspqexprPrElFirstStage,
		CExpressionArray *pdrspqexprPrElSecondStage,
		CExpressionArray *pdrspqexprPrElThirdStage, CColRefArray *pdrspqcrArgDQA,
		CColRefArray *pdrspqcrLastStage, BOOL fSplit2LevelsOnly,
		BOOL fAddDistinctColToLocalGb, CLogicalGbAgg::EAggStage aggStage);

	// split DQA into a local DQA and global non-DQA aggregate function
	static CExpression *PexprSplitIntoLocalDQAGlobalAgg(
		CMemoryPool *mp, CColumnFactory *col_factory, CMDAccessor *md_accessor,
		CExpression *pexpr, CExpression *pexprRelational,
		ExprToColRefMap *phmexprcr, CColRefArray *pdrspqcrArgDQA,
		CLogicalGbAgg::EAggStage aggStage);

	// helper function to split DQA
	static CExpression *PexprSplitHelper(
		CMemoryPool *mp, CColumnFactory *col_factory, CMDAccessor *md_accessor,
		CExpression *pexpr, CExpression *pexprRelational,
		ExprToColRefMap *phmexprcr, CColRefArray *pdrspqcrArgDQA,
		CLogicalGbAgg::EAggStage aggStage);

	// given a scalar aggregate generate the local, intermediate and global
	// aggregate functions. Then add them to the project list of the
	// corresponding aggregate operator at each stage of the multi-stage
	// aggregation
	static void PopulatePrLMultiPhaseAgg(
		CMemoryPool *mp, CColumnFactory *col_factory, CMDAccessor *md_accessor,
		CExpression *pexprPrEl, CExpressionArray *pdrspqexprPrElFirstStage,
		CExpressionArray *pdrspqexprPrElSecondStage,
		CExpressionArray *pdrspqexprPrElLastStage, BOOL fSplit2LevelsOnly);

	// create project element for the aggregate function of a particular level
	static CExpression *PexprPrElAgg(CMemoryPool *mp, CExpression *pexprAggFunc,
									 EAggfuncStage eaggfuncstage,
									 CColRef *pcrPreviousStage,
									 CColRef *pcrGlobal);

	// extract arguments of distinct aggs
	static void ExtractDistinctCols(
		CMemoryPool *mp, CColumnFactory *col_factory, CMDAccessor *md_accessor,
		CExpression *pexpr, CExpressionArray *pdrspqexprChildPrEl,
		ExprToColRefMap *phmexprcr, CColRefArray **ppdrspqcrArgDQA);

	// return the column reference of the argument to the aggregate function
	static CColRef *PcrAggFuncArgument(CMemoryPool *mp,
									   CMDAccessor *md_accessor,
									   CColumnFactory *col_factory,
									   CExpression *pexprArg,
									   CExpressionArray *pdrspqexprChildPrEl);

public:
	// ctor
	explicit CXformSplitDQA(CMemoryPool *mp);

	// dtor
	virtual ~CXformSplitDQA()
	{
	}

	// ident accessors
	virtual EXformId
	Exfid() const
	{
		return ExfSplitDQA;
	}

	// return a string for xform name
	virtual const CHAR *
	SzId() const
	{
		return "CXformSplitDQA";
	}

	// Compatibility function for splitting aggregates
	virtual BOOL
	FCompatible(CXform::EXformId exfid)
	{
		return (CXform::ExfSplitDQA != exfid) &&
			   (CXform::ExfSplitGbAgg != exfid);
	}

	// compute xform promise for a given expression handle
	virtual EXformPromise Exfp(CExpressionHandle &exprhdl) const;

	// actual transform
	void Transform(CXformContext *, CXformResult *, CExpression *) const;

};	// class CXformSplitDQA

}  // namespace spqopt

#endif	// !SPQOPT_CXformSplitDQA_H

// EOF
