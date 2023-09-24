//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CLogicalDifference.cpp
//
//	@doc:
//		Implementation of Difference operator
//---------------------------------------------------------------------------

#include "spqopt/operators/CLogicalDifference.h"

#include "spqos/base.h"

#include "spqopt/base/CKeyCollection.h"
#include "spqopt/base/CUtils.h"
#include "spqopt/operators/CExpressionHandle.h"
#include "spqopt/operators/CLogicalGbAgg.h"
#include "naucrates/statistics/CStatsPredUtils.h"

using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CLogicalDifference::CLogicalDifference
//
//	@doc:
//		Ctor - for pattern
//
//---------------------------------------------------------------------------
CLogicalDifference::CLogicalDifference(CMemoryPool *mp) : CLogicalSetOp(mp)
{
	m_fPattern = true;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalDifference::CLogicalDifference
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CLogicalDifference::CLogicalDifference(CMemoryPool *mp,
									   CColRefArray *pdrspqcrOutput,
									   CColRef2dArray *pdrspqdrspqcrInput)
	: CLogicalSetOp(mp, pdrspqcrOutput, pdrspqdrspqcrInput)
{
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalDifference::~CLogicalDifference
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CLogicalDifference::~CLogicalDifference()
{
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalDifference::DeriveMaxCard
//
//	@doc:
//		Derive max card
//
//---------------------------------------------------------------------------
CMaxCard
CLogicalDifference::DeriveMaxCard(CMemoryPool *,  // mp
								  CExpressionHandle &exprhdl) const
{
	// contradictions produce no rows
	if (exprhdl.DerivePropertyConstraint()->FContradiction())
	{
		return CMaxCard(0 /*ull*/);
	}

	return exprhdl.DeriveMaxCard(0);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalDifference::PopCopyWithRemappedColumns
//
//	@doc:
//		Return a copy of the operator with remapped columns
//
//---------------------------------------------------------------------------
COperator *
CLogicalDifference::PopCopyWithRemappedColumns(CMemoryPool *mp,
											   UlongToColRefMap *colref_mapping,
											   BOOL must_exist)
{
	CColRefArray *pdrspqcrOutput =
		CUtils::PdrspqcrRemap(mp, m_pdrspqcrOutput, colref_mapping, must_exist);
	CColRef2dArray *pdrspqdrspqcrInput = CUtils::PdrspqdrspqcrRemap(
		mp, m_pdrspqdrspqcrInput, colref_mapping, must_exist);

	return SPQOS_NEW(mp) CLogicalDifference(mp, pdrspqcrOutput, pdrspqdrspqcrInput);
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalDifference::PxfsCandidates
//
//	@doc:
//		Get candidate xforms
//
//---------------------------------------------------------------------------
CXformSet *
CLogicalDifference::PxfsCandidates(CMemoryPool *mp) const
{
	CXformSet *xform_set = SPQOS_NEW(mp) CXformSet(mp);
	(void) xform_set->ExchangeSet(CXform::ExfDifference2LeftAntiSemiJoin);
	return xform_set;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalDifference::PstatsDerive
//
//	@doc:
//		Derive statistics
//
//---------------------------------------------------------------------------
IStatistics *
CLogicalDifference::PstatsDerive(CMemoryPool *mp, CExpressionHandle &exprhdl,
								 IStatisticsArray *	 // not used
) const
{
	SPQOS_ASSERT(Esp(exprhdl) > EspNone);

	// difference is transformed into an aggregate over a LASJ,
	// we follow the same route to compute statistics
	CColRefSetArray *output_colrefsets = SPQOS_NEW(mp) CColRefSetArray(mp);
	const ULONG size = m_pdrspqdrspqcrInput->Size();
	for (ULONG ul = 0; ul < size; ul++)
	{
		CColRefSet *pcrs =
			SPQOS_NEW(mp) CColRefSet(mp, (*m_pdrspqdrspqcrInput)[ul]);
		output_colrefsets->Append(pcrs);
	}

	IStatistics *outer_stats = exprhdl.Pstats(0);
	IStatistics *inner_side_stats = exprhdl.Pstats(1);

	// construct the scalar condition for the LASJ
	CExpression *pexprScCond =
		CUtils::PexprConjINDFCond(mp, m_pdrspqdrspqcrInput);

	// compute the statistics for LASJ
	CColRefSet *outer_refs = exprhdl.DeriveOuterReferences();
	CStatsPredJoinArray *join_preds_stats =
		CStatsPredUtils::ExtractJoinStatsFromExpr(mp, exprhdl, pexprScCond,
												  output_colrefsets, outer_refs,
												  true	// is an LASJ
		);
	IStatistics *LASJ_stats =
		outer_stats->CalcLASJoinStats(mp, inner_side_stats, join_preds_stats,
									  true /* DoIgnoreLASJHistComputation */
		);

	// clean up
	pexprScCond->Release();
	join_preds_stats->Release();

	// computed columns
	ULongPtrArray *pdrspqulComputedCols = SPQOS_NEW(mp) ULongPtrArray(mp);
	IStatistics *stats = CLogicalGbAgg::PstatsDerive(
		mp, LASJ_stats,
		(*m_pdrspqdrspqcrInput)[0],  // we group by the columns of the first child
		pdrspqulComputedCols,	   // no computed columns for set ops
		NULL					   // no keys, use all grouping cols
	);

	// clean up
	pdrspqulComputedCols->Release();
	LASJ_stats->Release();
	output_colrefsets->Release();

	return stats;
}

// EOF
