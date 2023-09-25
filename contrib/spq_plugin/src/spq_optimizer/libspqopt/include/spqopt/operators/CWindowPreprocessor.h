//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2015 Pivotal Inc.
//
//	@filename:
//		CWindowPreprocessor.h
//
//	@doc:
//		Preprocessing routines of window functions
//---------------------------------------------------------------------------
#ifndef SPQOPT_CWindowPreprocessor_H
#define SPQOPT_CWindowPreprocessor_H

#include "spqos/base.h"

#include "spqopt/base/COrderSpec.h"
#include "spqopt/base/CWindowFrame.h"
#include "spqopt/operators/CExpression.h"

namespace spqopt
{
//---------------------------------------------------------------------------
//	@class:
//		CWindowPreprocessor
//
//	@doc:
//		Preprocessing routines of window functions
//
//---------------------------------------------------------------------------
class CWindowPreprocessor
{
private:
	// private copy ctor
	CWindowPreprocessor(const CWindowPreprocessor &);

	// iterate over project elements and split them elements between Distinct Aggs list, and Others list
	static void SplitPrjList(CMemoryPool *mp, CExpression *pexprSeqPrj,
							 CExpressionArray **ppdrspqexprDistinctAggsPrjElems,
							 CExpressionArray **ppdrspqexprOtherPrjElems,
							 spqopt::COrderSpecArray **ppdrspqosOther,
							 spqopt::CWindowFrameArray **ppdrspqwfOther);

	// split given SeqPrj expression into:
	//	- A GbAgg expression containing distinct Aggs, and
	//	- A SeqPrj expression containing all other window functions
	static void SplitSeqPrj(CMemoryPool *mp, CExpression *pexprSeqPrj,
							CExpression **ppexprGbAgg,
							CExpression **ppexprOutputSeqPrj);

	// create a CTE with two consumers using the child expression of Sequence Project
	static void CreateCTE(CMemoryPool *mp, CExpression *pexprSeqPrj,
						  CExpression **ppexprFirstConsumer,
						  CExpression **ppexprSecondConsumer);

	// extract grouping columns from given expression
	static CColRefArray *PdrspqcrGrpCols(CExpression *pexprJoinDQAs);

	// transform sequence project expression into an inner join expression
	static CExpression *PexprSeqPrj2Join(CMemoryPool *mp,
										 CExpression *pexprSeqPrj);

public:
	// main driver
	static CExpression *PexprPreprocess(CMemoryPool *mp, CExpression *pexpr);

};	// class CWindowPreprocessor
}  // namespace spqopt


#endif	// !SPQOPT_CWindowPreprocessor_H

// EOF
