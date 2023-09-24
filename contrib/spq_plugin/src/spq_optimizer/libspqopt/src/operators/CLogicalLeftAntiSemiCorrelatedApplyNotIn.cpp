//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal Inc.
//
//	@filename:
//		CLogicalLeftAntiSemiCorrelatedApplyNotIn.cpp
//
//	@doc:
//		Implementation of left anti semi correlated apply with NOT-IN/ANY semantics
//---------------------------------------------------------------------------

#include "spqopt/operators/CLogicalLeftAntiSemiCorrelatedApplyNotIn.h"

#include "spqos/base.h"

using namespace spqopt;

//---------------------------------------------------------------------------
//	@function:
//		CLogicalLeftAntiSemiCorrelatedApplyNotIn::PxfsCandidates
//
//	@doc:
//		Get candidate xforms
//
//---------------------------------------------------------------------------
CXformSet *
CLogicalLeftAntiSemiCorrelatedApplyNotIn::PxfsCandidates(CMemoryPool *mp) const
{
	CXformSet *xform_set = SPQOS_NEW(mp) CXformSet(mp);
	(void) xform_set->ExchangeSet(
		CXform::ExfImplementLeftAntiSemiCorrelatedApplyNotIn);

	return xform_set;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalLeftAntiSemiCorrelatedApplyNotIn::PopCopyWithRemappedColumns
//
//	@doc:
//		Return a copy of the operator with remapped columns
//
//---------------------------------------------------------------------------
COperator *
CLogicalLeftAntiSemiCorrelatedApplyNotIn::PopCopyWithRemappedColumns(
	CMemoryPool *mp, UlongToColRefMap *colref_mapping, BOOL must_exist)
{
	CColRefArray *pdrspqcrInner =
		CUtils::PdrspqcrRemap(mp, m_pdrspqcrInner, colref_mapping, must_exist);

	return SPQOS_NEW(mp) CLogicalLeftAntiSemiCorrelatedApplyNotIn(
		mp, pdrspqcrInner, m_eopidOriginSubq);
}


// EOF
