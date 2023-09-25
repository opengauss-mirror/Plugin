//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal Inc.
//
//	@filename:
//		CLogicalLeftSemiApplyIn.cpp
//
//	@doc:
//		Implementation of left-semi-apply operator with In semantics
//---------------------------------------------------------------------------

#include "spqopt/operators/CLogicalLeftSemiApplyIn.h"

#include "spqos/base.h"

using namespace spqopt;

//---------------------------------------------------------------------------
//	@function:
//		CLogicalLeftSemiApplyIn::PxfsCandidates
//
//	@doc:
//		Get candidate xforms
//
//---------------------------------------------------------------------------
CXformSet *
CLogicalLeftSemiApplyIn::PxfsCandidates(CMemoryPool *mp) const
{
	CXformSet *xform_set = SPQOS_NEW(mp) CXformSet(mp);
	(void) xform_set->ExchangeSet(CXform::ExfLeftSemiApplyIn2LeftSemiJoin);
	(void) xform_set->ExchangeSet(
		CXform::ExfLeftSemiApplyInWithExternalCorrs2InnerJoin);
	(void) xform_set->ExchangeSet(
		CXform::ExfLeftSemiApplyIn2LeftSemiJoinNoCorrelations);

	return xform_set;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalLeftSemiApplyIn::PopCopyWithRemappedColumns
//
//	@doc:
//		Return a copy of the operator with remapped columns
//
//---------------------------------------------------------------------------
COperator *
CLogicalLeftSemiApplyIn::PopCopyWithRemappedColumns(
	CMemoryPool *mp, UlongToColRefMap *colref_mapping, BOOL must_exist)
{
	CColRefArray *pdrspqcrInner =
		CUtils::PdrspqcrRemap(mp, m_pdrspqcrInner, colref_mapping, must_exist);

	return SPQOS_NEW(mp)
		CLogicalLeftSemiApplyIn(mp, pdrspqcrInner, m_eopidOriginSubq);
}

// EOF
