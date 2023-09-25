//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		CLogicalUnion.cpp
//
//	@doc:
//		Implementation of union operator
//---------------------------------------------------------------------------

#include "spqopt/operators/CLogicalUnion.h"

#include "spqos/base.h"

#include "spqopt/base/CKeyCollection.h"
#include "spqopt/base/CUtils.h"
#include "spqopt/operators/CExpressionHandle.h"
#include "spqopt/operators/CLogicalGbAgg.h"
#include "spqopt/operators/CLogicalUnionAll.h"

using namespace spqopt;

//---------------------------------------------------------------------------
//	@function:
//		CLogicalUnion::CLogicalUnion
//
//	@doc:
//		ctor - for pattern
//
//---------------------------------------------------------------------------
CLogicalUnion::CLogicalUnion(CMemoryPool *mp) : CLogicalSetOp(mp)
{
	m_fPattern = true;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalUnion::CLogicalUnion
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CLogicalUnion::CLogicalUnion(CMemoryPool *mp, CColRefArray *pdrspqcrOutput,
							 CColRef2dArray *pdrspqdrspqcrInput)
	: CLogicalSetOp(mp, pdrspqcrOutput, pdrspqdrspqcrInput)
{
#ifdef SPQOS_DEBUG
	CColRefArray *pdrspqcrInput = (*pdrspqdrspqcrInput)[0];
	const ULONG num_cols = pdrspqcrOutput->Size();
	SPQOS_ASSERT(num_cols == pdrspqcrInput->Size());

	// Ensure that the output columns are the same as first input
	for (ULONG ul = 0; ul < num_cols; ul++)
	{
		SPQOS_ASSERT((*pdrspqcrOutput)[ul] == (*pdrspqcrInput)[ul]);
	}

#endif	// SPQOS_DEBUG
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalUnion::~CLogicalUnion
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CLogicalUnion::~CLogicalUnion()
{
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalUnion::PopCopyWithRemappedColumns
//
//	@doc:
//		Return a copy of the operator with remapped columns
//
//---------------------------------------------------------------------------
COperator *
CLogicalUnion::PopCopyWithRemappedColumns(CMemoryPool *mp,
										  UlongToColRefMap *colref_mapping,
										  BOOL must_exist)
{
	CColRefArray *pdrspqcrOutput =
		CUtils::PdrspqcrRemap(mp, m_pdrspqcrOutput, colref_mapping, must_exist);
	CColRef2dArray *pdrspqdrspqcrInput = CUtils::PdrspqdrspqcrRemap(
		mp, m_pdrspqdrspqcrInput, colref_mapping, must_exist);

	return SPQOS_NEW(mp) CLogicalUnion(mp, pdrspqcrOutput, pdrspqdrspqcrInput);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalUnion::DeriveMaxCard
//
//	@doc:
//		Derive max card
//
//---------------------------------------------------------------------------
CMaxCard
CLogicalUnion::DeriveMaxCard(CMemoryPool *,	 // mp
							 CExpressionHandle &exprhdl) const
{
	const ULONG arity = exprhdl.Arity();

	CMaxCard maxcard = exprhdl.DeriveMaxCard(0);
	for (ULONG ul = 1; ul < arity; ul++)
	{
		maxcard += exprhdl.DeriveMaxCard(ul);
	}

	return maxcard;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalUnion::PxfsCandidates
//
//	@doc:
//		Get candidate xforms
//
//---------------------------------------------------------------------------
CXformSet *
CLogicalUnion::PxfsCandidates(CMemoryPool *mp) const
{
	CXformSet *xform_set = SPQOS_NEW(mp) CXformSet(mp);
	(void) xform_set->ExchangeSet(CXform::ExfUnion2UnionAll);
	return xform_set;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalUnion::PstatsDerive
//
//	@doc:
//		Derive statistics
//
//---------------------------------------------------------------------------
IStatistics *
CLogicalUnion::PstatsDerive(CMemoryPool *mp, CExpressionHandle &exprhdl,
							IStatisticsArray *	// not used
) const
{
	SPQOS_ASSERT(Esp(exprhdl) > EspNone);

	// union is transformed into a group by over an union all
	// we follow the same route to compute statistics
	IStatistics *pstatsUnionAll =
		CLogicalUnionAll::PstatsDeriveUnionAll(mp, exprhdl);

	// computed columns
	ULongPtrArray *pdrspqulComputedCols = SPQOS_NEW(mp) ULongPtrArray(mp);

	IStatistics *stats = CLogicalGbAgg::PstatsDerive(
		mp, pstatsUnionAll,
		m_pdrspqcrOutput,	  // we group by the output columns
		pdrspqulComputedCols,  // no computed columns for set ops
		NULL				  // no keys, use all grouping cols
	);

	// clean up
	pdrspqulComputedCols->Release();
	pstatsUnionAll->Release();

	return stats;
}


// EOF
