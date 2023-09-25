//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 EMC Corp.
//
//	@filename:
//		CLogicalLeftAntiSemiApplyNotIn.cpp
//
//	@doc:
//		Implementation of left anti-semi-apply operator with NotIn semantics
//---------------------------------------------------------------------------

#include "spqopt/operators/CLogicalLeftAntiSemiApplyNotIn.h"

#include "spqos/base.h"

using namespace spqopt;

//---------------------------------------------------------------------------
//	@function:
//		CLogicalLeftAntiSemiApplyNotIn::PxfsCandidates
//
//	@doc:
//		Get candidate xforms
//
//---------------------------------------------------------------------------
CXformSet *
CLogicalLeftAntiSemiApplyNotIn::PxfsCandidates(CMemoryPool *mp) const
{
	CXformSet *xform_set = SPQOS_NEW(mp) CXformSet(mp);
	(void) xform_set->ExchangeSet(
		CXform::ExfLeftAntiSemiApplyNotIn2LeftAntiSemiJoinNotIn);
	(void) xform_set->ExchangeSet(
		CXform::ExfLeftAntiSemiApplyNotIn2LeftAntiSemiJoinNotInNoCorrelations);

	return xform_set;
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalLeftAntiSemiApplyNotIn::PopCopyWithRemappedColumns
//
//	@doc:
//		Return a copy of the operator with remapped columns
//
//---------------------------------------------------------------------------
COperator *
CLogicalLeftAntiSemiApplyNotIn::PopCopyWithRemappedColumns(
	CMemoryPool *mp, UlongToColRefMap *colref_mapping, BOOL must_exist)
{
	CColRefArray *pdrspqcrInner =
		CUtils::PdrspqcrRemap(mp, m_pdrspqcrInner, colref_mapping, must_exist);

	return SPQOS_NEW(mp)
		CLogicalLeftAntiSemiApplyNotIn(mp, pdrspqcrInner, m_eopidOriginSubq);
}

// EOF
