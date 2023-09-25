//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2022 VMware, Inc. or its affiliates.
//
//	@filename:
//		COrderedAggPreprocessor.h
//
//	@doc:
//		Preprocessing routines of ordered-set aggregates
//---------------------------------------------------------------------------
#ifndef SPQOPT_COrderedAggPreprocessor_H
#define SPQOPT_COrderedAggPreprocessor_H

#include "spqos/base.h"

#include "spqopt/base/COrderSpec.h"
#include "spqopt/base/CWindowFrame.h"
#include "spqopt/operators/CExpression.h"

namespace spqopt
{
//---------------------------------------------------------------------------
//	@class:
//		COrderedAggPreprocessor
//
//	@doc:
//		Preprocessing routines of window functions
//
//---------------------------------------------------------------------------
class COrderedAggPreprocessor
{
private:
	// private copy ctor
	COrderedAggPreprocessor(const COrderedAggPreprocessor &);

	// iterate over project elements and split elements between Ordered Aggs
	// list, and Others list
	static void SplitPrjList(CMemoryPool *mp, CExpression *pexprInputAggPrj,
							 CExpressionArray **ppdrspqexprOrderedAggsPrEl,
							 CExpressionArray **ppdrspqexprOtherPrjElems);

	// split given InputAgg expression into:
	//	- A GbAgg expression containing ordered aggs split to spq_percentile agg, and
	//	- A GbAgg expression containing all other non-ordered agg functions
	static void SplitOrderedAggsPrj(CMemoryPool *mp,
									CExpression *pexprInputAggPrj,
									CExpression **ppexprGbAgg,
									CExpression **ppexprOutputSeqPrj);

	// create a CTE with two consumers using the child expression of Sequence
	// Project
	static void CreateCTE(CMemoryPool *mp, CExpression *pexprInputAggPrj,
						  CExpression **ppexprFirstConsumer,
						  CExpression **ppexprSecondConsumer);

	// create an expression for final agg function
	static CExpression *PexprFinalAgg(CMemoryPool *mp,
									  CExpression *pexprAggFunc,
									  CColRef *arg_col_ref,
									  CColRef *total_count_colref,
									  CColRef *peer_count_colref);

	// transform sequence project expression into an inner join expression
	static CExpression *PexprInputAggPrj2Join(CMemoryPool *mp,
											  CExpression *pexprInputAggPrj);

public:
	// main driver
	static CExpression *PexprPreprocess(CMemoryPool *mp, CExpression *pexpr);

};	// class COrderedAggPreprocessor
}  // namespace spqopt


#endif	// !SPQOPT_COrderedAggPreprocessor_H

// EOF
