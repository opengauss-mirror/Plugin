//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CLogicalDifferenceAll.cpp
//
//	@doc:
//		Implementation of DifferenceAll operator
//---------------------------------------------------------------------------

#include "spqopt/operators/CLogicalDifferenceAll.h"

#include "spqos/base.h"

#include "spqopt/base/CKeyCollection.h"
#include "spqopt/base/CUtils.h"
#include "spqopt/operators/CExpressionHandle.h"
#include "naucrates/statistics/CStatsPredUtils.h"

using namespace spqopt;

//---------------------------------------------------------------------------
//	@function:
//		CLogicalDifferenceAll::CLogicalDifferenceAll
//
//	@doc:
//		Ctor - for pattern
//
//---------------------------------------------------------------------------
CLogicalDifferenceAll::CLogicalDifferenceAll(CMemoryPool *mp)
	: CLogicalSetOp(mp)
{
	m_fPattern = true;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalDifferenceAll::CLogicalDifferenceAll
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CLogicalDifferenceAll::CLogicalDifferenceAll(CMemoryPool *mp,
											 CColRefArray *pdrspqcrOutput,
											 CColRef2dArray *pdrspqdrspqcrInput)
	: CLogicalSetOp(mp, pdrspqcrOutput, pdrspqdrspqcrInput)
{
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalDifferenceAll::~CLogicalDifferenceAll
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CLogicalDifferenceAll::~CLogicalDifferenceAll()
{
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalDifferenceAll::DeriveMaxCard
//
//	@doc:
//		Derive max card
//
//---------------------------------------------------------------------------
CMaxCard
CLogicalDifferenceAll::DeriveMaxCard(CMemoryPool *,	 // mp
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
//		CLogicalDifferenceAll::PopCopyWithRemappedColumns
//
//	@doc:
//		Return a copy of the operator with remapped columns
//
//---------------------------------------------------------------------------
COperator *
CLogicalDifferenceAll::PopCopyWithRemappedColumns(
	CMemoryPool *mp, UlongToColRefMap *colref_mapping, BOOL must_exist)
{
	CColRefArray *pdrspqcrOutput =
		CUtils::PdrspqcrRemap(mp, m_pdrspqcrOutput, colref_mapping, must_exist);
	CColRef2dArray *pdrspqdrspqcrInput = CUtils::PdrspqdrspqcrRemap(
		mp, m_pdrspqdrspqcrInput, colref_mapping, must_exist);

	return SPQOS_NEW(mp)
		CLogicalDifferenceAll(mp, pdrspqcrOutput, pdrspqdrspqcrInput);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalDifferenceAll::PkcDeriveKeys
//
//	@doc:
//		Derive key collection
//
//---------------------------------------------------------------------------
CKeyCollection *
CLogicalDifferenceAll::DeriveKeyCollection(CMemoryPool *,		// mp,
										   CExpressionHandle &	//exprhdl
) const
{
	// TODO: Add keys on columns contributing to the setop from the outer child
	return NULL;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalDifferenceAll::PstatsDerive
//
//	@doc:
//		Derive statistics
//
//---------------------------------------------------------------------------
IStatistics *
CLogicalDifferenceAll::PstatsDerive(CMemoryPool *mp, CExpressionHandle &exprhdl,
									IStatisticsArray *	// not used
) const
{
	SPQOS_ASSERT(Esp(exprhdl) > EspNone);

	// difference all is transformed into a LASJ,
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
									  true /* DoIgnoreLASJHistComputation*/
		);

	// clean up
	pexprScCond->Release();
	join_preds_stats->Release();
	output_colrefsets->Release();

	return LASJ_stats;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalDifferenceAll::PxfsCandidates
//
//	@doc:
//		Get candidate xforms
//
//---------------------------------------------------------------------------
CXformSet *
CLogicalDifferenceAll::PxfsCandidates(CMemoryPool *mp) const
{
	CXformSet *xform_set = SPQOS_NEW(mp) CXformSet(mp);
	(void) xform_set->ExchangeSet(CXform::ExfDifferenceAll2LeftAntiSemiJoin);
	return xform_set;
}

// EOF
