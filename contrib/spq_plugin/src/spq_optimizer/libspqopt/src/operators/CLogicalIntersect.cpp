//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CLogicalIntersect.cpp
//
//	@doc:
//		Implementation of Intersect operator
//---------------------------------------------------------------------------

#include "spqopt/operators/CLogicalIntersect.h"

#include "spqos/base.h"

#include "spqopt/base/CKeyCollection.h"
#include "spqopt/base/CUtils.h"
#include "spqopt/operators/CExpressionHandle.h"
#include "spqopt/operators/CLogicalGbAgg.h"
#include "spqopt/operators/CLogicalIntersectAll.h"

using namespace spqopt;

//---------------------------------------------------------------------------
//	@function:
//		CLogicalIntersect::CLogicalIntersect
//
//	@doc:
//		Ctor - for pattern
//
//---------------------------------------------------------------------------
CLogicalIntersect::CLogicalIntersect(CMemoryPool *mp) : CLogicalSetOp(mp)
{
	m_fPattern = true;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalIntersect::CLogicalIntersect
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CLogicalIntersect::CLogicalIntersect(CMemoryPool *mp,
									 CColRefArray *pdrspqcrOutput,
									 CColRef2dArray *pdrspqdrspqcrInput)
	: CLogicalSetOp(mp, pdrspqcrOutput, pdrspqdrspqcrInput)
{
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalIntersect::~CLogicalIntersect
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CLogicalIntersect::~CLogicalIntersect()
{
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalIntersect::DeriveMaxCard
//
//	@doc:
//		Derive max card
//
//---------------------------------------------------------------------------
CMaxCard
CLogicalIntersect::DeriveMaxCard(CMemoryPool *,	 // mp
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
//		CLogicalIntersect::PopCopyWithRemappedColumns
//
//	@doc:
//		Return a copy of the operator with remapped columns
//
//---------------------------------------------------------------------------
COperator *
CLogicalIntersect::PopCopyWithRemappedColumns(CMemoryPool *mp,
											  UlongToColRefMap *colref_mapping,
											  BOOL must_exist)
{
	CColRefArray *pdrspqcrOutput =
		CUtils::PdrspqcrRemap(mp, m_pdrspqcrOutput, colref_mapping, must_exist);
	CColRef2dArray *pdrspqdrspqcrInput = CUtils::PdrspqdrspqcrRemap(
		mp, m_pdrspqdrspqcrInput, colref_mapping, must_exist);

	return SPQOS_NEW(mp) CLogicalIntersect(mp, pdrspqcrOutput, pdrspqdrspqcrInput);
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalIntersect::PxfsCandidates
//
//	@doc:
//		Get candidate xforms
//
//---------------------------------------------------------------------------
CXformSet *
CLogicalIntersect::PxfsCandidates(CMemoryPool *mp) const
{
	CXformSet *xform_set = SPQOS_NEW(mp) CXformSet(mp);
	(void) xform_set->ExchangeSet(CXform::ExfIntersect2Join);
	return xform_set;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalIntersect::PstatsDerive
//
//	@doc:
//		Derive statistics
//
//---------------------------------------------------------------------------
IStatistics *
CLogicalIntersect::PstatsDerive(CMemoryPool *mp, CExpressionHandle &exprhdl,
								IStatisticsArray *	// not used
) const
{
	SPQOS_ASSERT(Esp(exprhdl) > EspNone);

	// intersect is transformed into a group by over an intersect all
	// we follow the same route to compute statistics

	CColRefSetArray *output_colrefsets = SPQOS_NEW(mp) CColRefSetArray(mp);
	const ULONG size = m_pdrspqdrspqcrInput->Size();
	for (ULONG ul = 0; ul < size; ul++)
	{
		CColRefSet *pcrs =
			SPQOS_NEW(mp) CColRefSet(mp, (*m_pdrspqdrspqcrInput)[ul]);
		output_colrefsets->Append(pcrs);
	}

	IStatistics *pstatsIntersectAll = CLogicalIntersectAll::PstatsDerive(
		mp, exprhdl, m_pdrspqdrspqcrInput, output_colrefsets);

	// computed columns
	ULongPtrArray *pdrspqulComputedCols = SPQOS_NEW(mp) ULongPtrArray(mp);

	IStatistics *stats = CLogicalGbAgg::PstatsDerive(
		mp, pstatsIntersectAll,
		(*m_pdrspqdrspqcrInput)[0],  // we group by the columns of the first child
		pdrspqulComputedCols,	   // no computed columns for set ops
		NULL					   // no keys, use all grouping cols
	);
	// clean up
	pdrspqulComputedCols->Release();
	pstatsIntersectAll->Release();
	output_colrefsets->Release();

	return stats;
}

// EOF
