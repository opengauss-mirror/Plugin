//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CLogicalUnionAll.cpp
//
//	@doc:
//		Implementation of UnionAll operator
//---------------------------------------------------------------------------

#include "spqopt/operators/CLogicalUnionAll.h"

#include "spqos/base.h"

#include "spqopt/base/CUtils.h"
#include "spqopt/operators/CExpressionHandle.h"
#include "naucrates/statistics/CUnionAllStatsProcessor.h"

using namespace spqopt;

//---------------------------------------------------------------------------
//	@function:
//		CLogicalUnionAll::CLogicalUnionAll
//
//	@doc:
//		Ctor - for pattern
//
//---------------------------------------------------------------------------
CLogicalUnionAll::CLogicalUnionAll(CMemoryPool *mp)
	: CLogicalUnion(mp), m_ulScanIdPartialIndex(0)
{
	m_fPattern = true;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalUnionAll::CLogicalUnionAll
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CLogicalUnionAll::CLogicalUnionAll(CMemoryPool *mp, CColRefArray *pdrspqcrOutput,
								   CColRef2dArray *pdrspqdrspqcrInput,
								   ULONG ulScanIdPartialIndex)
	: CLogicalUnion(mp, pdrspqcrOutput, pdrspqdrspqcrInput),
	  m_ulScanIdPartialIndex(ulScanIdPartialIndex)
{
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalUnionAll::~CLogicalUnionAll
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CLogicalUnionAll::~CLogicalUnionAll()
{
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalUnionAll::DeriveMaxCard
//
//	@doc:
//		Derive max card
//
//---------------------------------------------------------------------------
CMaxCard
CLogicalUnionAll::DeriveMaxCard(CMemoryPool *,	// mp
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
//		CLogicalUnionAll::PopCopyWithRemappedColumns
//
//	@doc:
//		Return a copy of the operator with remapped columns
//
//---------------------------------------------------------------------------
COperator *
CLogicalUnionAll::PopCopyWithRemappedColumns(CMemoryPool *mp,
											 UlongToColRefMap *colref_mapping,
											 BOOL must_exist)
{
	CColRefArray *pdrspqcrOutput =
		CUtils::PdrspqcrRemap(mp, m_pdrspqcrOutput, colref_mapping, must_exist);
	CColRef2dArray *pdrspqdrspqcrInput = CUtils::PdrspqdrspqcrRemap(
		mp, m_pdrspqdrspqcrInput, colref_mapping, must_exist);

	return SPQOS_NEW(mp) CLogicalUnionAll(mp, pdrspqcrOutput, pdrspqdrspqcrInput,
										 m_ulScanIdPartialIndex);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalUnionAll::PkcDeriveKeys
//
//	@doc:
//		Derive key collection
//
//---------------------------------------------------------------------------
CKeyCollection *
CLogicalUnionAll::DeriveKeyCollection(CMemoryPool *,	   //mp,
									  CExpressionHandle &  // exprhdl
) const
{
	return NULL;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalUnionAll::PxfsCandidates
//
//	@doc:
//		Get candidate xforms
//
//---------------------------------------------------------------------------
CXformSet *
CLogicalUnionAll::PxfsCandidates(CMemoryPool *mp) const
{
	CXformSet *xform_set = SPQOS_NEW(mp) CXformSet(mp);
	(void) xform_set->ExchangeSet(CXform::ExfImplementUnionAll);

	return xform_set;
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalUnionAll::PstatsDeriveUnionAll
//
//	@doc:
//		Derive statistics based on union all semantics
//
//---------------------------------------------------------------------------
IStatistics *
CLogicalUnionAll::PstatsDeriveUnionAll(CMemoryPool *mp,
									   CExpressionHandle &exprhdl)
{
	SPQOS_ASSERT(COperator::EopLogicalUnionAll == exprhdl.Pop()->Eopid() ||
				COperator::EopLogicalUnion == exprhdl.Pop()->Eopid());

	CColRefArray *pdrspqcrOutput =
		CLogicalSetOp::PopConvert(exprhdl.Pop())->PdrspqcrOutput();
	CColRef2dArray *pdrspqdrspqcrInput =
		CLogicalSetOp::PopConvert(exprhdl.Pop())->PdrspqdrspqcrInput();
	SPQOS_ASSERT(NULL != pdrspqcrOutput);
	SPQOS_ASSERT(NULL != pdrspqdrspqcrInput);

	IStatistics *result_stats = exprhdl.Pstats(0);
	result_stats->AddRef();
	const ULONG arity = exprhdl.Arity();
	for (ULONG ul = 1; ul < arity; ul++)
	{
		IStatistics *child_stats = exprhdl.Pstats(ul);
		CStatistics *stats = CUnionAllStatsProcessor::CreateStatsForUnionAll(
			mp, dynamic_cast<CStatistics *>(result_stats),
			dynamic_cast<CStatistics *>(child_stats),
			CColRef::Pdrspqul(mp, pdrspqcrOutput),
			CColRef::Pdrspqul(mp, (*pdrspqdrspqcrInput)[0]),
			CColRef::Pdrspqul(mp, (*pdrspqdrspqcrInput)[ul]));
		result_stats->Release();
		result_stats = stats;
	}

	return result_stats;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalUnionAll::PstatsDerive
//
//	@doc:
//		Derive statistics based on union all semantics
//
//---------------------------------------------------------------------------
IStatistics *
CLogicalUnionAll::PstatsDerive(CMemoryPool *mp, CExpressionHandle &exprhdl,
							   IStatisticsArray *  // not used
) const
{
	SPQOS_ASSERT(EspNone < Esp(exprhdl));

	return PstatsDeriveUnionAll(mp, exprhdl);
}

// EOF
