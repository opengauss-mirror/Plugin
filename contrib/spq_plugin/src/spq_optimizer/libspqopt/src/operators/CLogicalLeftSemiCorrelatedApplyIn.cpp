//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal Inc.
//
//	@filename:
//		CLogicalLeftSemiCorrelatedApplyIn.cpp
//
//	@doc:
//		Implementation of left semi correlated apply with IN semantics
//---------------------------------------------------------------------------

#include "spqopt/operators/CLogicalLeftSemiCorrelatedApplyIn.h"

#include "spqos/base.h"

using namespace spqopt;

//---------------------------------------------------------------------------
//	@function:
//		CLogicalLeftSemiCorrelatedApplyIn::CLogicalLeftSemiCorrelatedApplyIn
//
//	@doc:
//		Ctor - for patterns
//
//---------------------------------------------------------------------------
CLogicalLeftSemiCorrelatedApplyIn::CLogicalLeftSemiCorrelatedApplyIn(
	CMemoryPool *mp)
	: CLogicalLeftSemiApplyIn(mp)
{
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalLeftSemiCorrelatedApplyIn::CLogicalLeftSemiCorrelatedApplyIn
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CLogicalLeftSemiCorrelatedApplyIn::CLogicalLeftSemiCorrelatedApplyIn(
	CMemoryPool *mp, CColRefArray *pdrspqcrInner, EOperatorId eopidOriginSubq)
	: CLogicalLeftSemiApplyIn(mp, pdrspqcrInner, eopidOriginSubq)
{
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalLeftSemiCorrelatedApplyIn::PxfsCandidates
//
//	@doc:
//		Get candidate xforms
//
//---------------------------------------------------------------------------
CXformSet *
CLogicalLeftSemiCorrelatedApplyIn::PxfsCandidates(CMemoryPool *mp) const
{
	CXformSet *xform_set = SPQOS_NEW(mp) CXformSet(mp);
	(void) xform_set->ExchangeSet(
		CXform::ExfImplementLeftSemiCorrelatedApplyIn);

	return xform_set;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalLeftSemiCorrelatedApplyIn::PopCopyWithRemappedColumns
//
//	@doc:
//		Return a copy of the operator with remapped columns
//
//---------------------------------------------------------------------------
COperator *
CLogicalLeftSemiCorrelatedApplyIn::PopCopyWithRemappedColumns(
	CMemoryPool *mp, UlongToColRefMap *colref_mapping, BOOL must_exist)
{
	CColRefArray *pdrspqcrInner =
		CUtils::PdrspqcrRemap(mp, m_pdrspqcrInner, colref_mapping, must_exist);

	return SPQOS_NEW(mp)
		CLogicalLeftSemiCorrelatedApplyIn(mp, pdrspqcrInner, m_eopidOriginSubq);
}


// EOF
