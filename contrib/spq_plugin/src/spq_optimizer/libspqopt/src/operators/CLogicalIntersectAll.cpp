//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CLogicalIntersectAll.cpp
//
//	@doc:
//		Implementation of Intersect all operator
//---------------------------------------------------------------------------

#include "spqopt/operators/CLogicalIntersectAll.h"

#include "spqos/base.h"

#include "spqopt/base/CKeyCollection.h"
#include "spqopt/base/CUtils.h"
#include "spqopt/operators/CExpressionHandle.h"
#include "spqopt/operators/CLogicalLeftSemiJoin.h"
#include "naucrates/statistics/CStatsPredUtils.h"

using namespace spqopt;

//---------------------------------------------------------------------------
//	@function:
//		CLogicalIntersectAll::CLogicalIntersectAll
//
//	@doc:
//		Ctor - for pattern
//
//---------------------------------------------------------------------------
CLogicalIntersectAll::CLogicalIntersectAll(CMemoryPool *mp) : CLogicalSetOp(mp)
{
	m_fPattern = true;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalIntersectAll::CLogicalIntersectAll
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CLogicalIntersectAll::CLogicalIntersectAll(CMemoryPool *mp,
										   CColRefArray *pdrspqcrOutput,
										   CColRef2dArray *pdrspqdrspqcrInput)
	: CLogicalSetOp(mp, pdrspqcrOutput, pdrspqdrspqcrInput)
{
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalIntersectAll::~CLogicalIntersectAll
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CLogicalIntersectAll::~CLogicalIntersectAll()
{
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalIntersectAll::DeriveMaxCard
//
//	@doc:
//		Derive max card
//
//---------------------------------------------------------------------------
CMaxCard
CLogicalIntersectAll::DeriveMaxCard(CMemoryPool *,	// mp
									CExpressionHandle &exprhdl) const
{
	// contradictions produce no rows
	if (exprhdl.DerivePropertyConstraint()->FContradiction())
	{
		return CMaxCard(0 /*ull*/);
	}

	CMaxCard maxcardL = exprhdl.DeriveMaxCard(0);
	CMaxCard maxcardR = exprhdl.DeriveMaxCard(1);

	if (maxcardL <= maxcardR)
	{
		return maxcardL;
	}

	return maxcardR;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalIntersectAll::PopCopyWithRemappedColumns
//
//	@doc:
//		Return a copy of the operator with remapped columns
//
//---------------------------------------------------------------------------
COperator *
CLogicalIntersectAll::PopCopyWithRemappedColumns(
	CMemoryPool *mp, UlongToColRefMap *colref_mapping, BOOL must_exist)
{
	CColRefArray *pdrspqcrOutput =
		CUtils::PdrspqcrRemap(mp, m_pdrspqcrOutput, colref_mapping, must_exist);
	CColRef2dArray *pdrspqdrspqcrInput = CUtils::PdrspqdrspqcrRemap(
		mp, m_pdrspqdrspqcrInput, colref_mapping, must_exist);

	return SPQOS_NEW(mp)
		CLogicalIntersectAll(mp, pdrspqcrOutput, pdrspqdrspqcrInput);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalIntersectAll::PkcDeriveKeys
//
//	@doc:
//		Derive key collection
//
//---------------------------------------------------------------------------
CKeyCollection *
CLogicalIntersectAll::DeriveKeyCollection(CMemoryPool *,	   //mp,
										  CExpressionHandle &  //exprhdl
) const
{
	// TODO: Add the keys from outer and inner child
	return NULL;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalIntersectAll::PxfsCandidates
//
//	@doc:
//		Get candidate xforms
//
//---------------------------------------------------------------------------
CXformSet *
CLogicalIntersectAll::PxfsCandidates(CMemoryPool *mp) const
{
	CXformSet *xform_set = SPQOS_NEW(mp) CXformSet(mp);
	(void) xform_set->ExchangeSet(CXform::ExfIntersectAll2LeftSemiJoin);

	return xform_set;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalIntersectAll::PstatsDerive
//
//	@doc:
//		Derive statistics
//
//---------------------------------------------------------------------------
IStatistics *
CLogicalIntersectAll::PstatsDerive(
	CMemoryPool *mp, CExpressionHandle &exprhdl,
	CColRef2dArray *pdrspqdrspqcrInput,
	CColRefSetArray *output_colrefsets	// output of relational children
)
{
	SPQOS_ASSERT(2 == exprhdl.Arity());

	IStatistics *outer_stats = exprhdl.Pstats(0);
	IStatistics *inner_side_stats = exprhdl.Pstats(1);

	// construct the scalar condition similar to transform that turns an "intersect all" into a "left semi join"
	// over a window operation on the individual input (for row_number)

	// TODO:  Jan 8th 2012, add the stats for window operation
	CExpression *pexprScCond = CUtils::PexprConjINDFCond(mp, pdrspqdrspqcrInput);
	CColRefSet *outer_refs = exprhdl.DeriveOuterReferences();
	CStatsPredJoinArray *join_preds_stats =
		CStatsPredUtils::ExtractJoinStatsFromExpr(mp, exprhdl, pexprScCond,
												  output_colrefsets, outer_refs,
												  true	// is a semi-join
		);
	IStatistics *pstatsSemiJoin = CLogicalLeftSemiJoin::PstatsDerive(
		mp, join_preds_stats, outer_stats, inner_side_stats);

	// clean up
	pexprScCond->Release();
	join_preds_stats->Release();

	return pstatsSemiJoin;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalIntersectAll::PstatsDerive
//
//	@doc:
//		Derive statistics
//
//---------------------------------------------------------------------------
IStatistics *
CLogicalIntersectAll::PstatsDerive(CMemoryPool *mp, CExpressionHandle &exprhdl,
								   IStatisticsArray *  // not used
) const
{
	SPQOS_ASSERT(Esp(exprhdl) > EspNone);

	CColRefSetArray *output_colrefsets = SPQOS_NEW(mp) CColRefSetArray(mp);
	const ULONG size = m_pdrspqdrspqcrInput->Size();
	for (ULONG ul = 0; ul < size; ul++)
	{
		CColRefSet *pcrs =
			SPQOS_NEW(mp) CColRefSet(mp, (*m_pdrspqdrspqcrInput)[ul]);
		output_colrefsets->Append(pcrs);
	}
	IStatistics *stats =
		PstatsDerive(mp, exprhdl, m_pdrspqdrspqcrInput, output_colrefsets);

	// clean up
	output_colrefsets->Release();

	return stats;
}

// EOF
