//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal Inc.
//
//	@filename:
//		CLogicalLeftSemiCorrelatedApply.cpp
//
//	@doc:
//		Implementation of left semi correlated apply for EXISTS subqueries
//---------------------------------------------------------------------------

#include "spqopt/operators/CLogicalLeftSemiCorrelatedApply.h"

#include "spqos/base.h"

using namespace spqopt;

//---------------------------------------------------------------------------
//	@function:
//		CLogicalLeftSemiCorrelatedApply::CLogicalLeftSemiCorrelatedApply
//
//	@doc:
//		Ctor - for patterns
//
//---------------------------------------------------------------------------
CLogicalLeftSemiCorrelatedApply::CLogicalLeftSemiCorrelatedApply(
	CMemoryPool *mp)
	: CLogicalLeftSemiApply(mp)
{
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalLeftSemiCorrelatedApply::CLogicalLeftSemiCorrelatedApply
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CLogicalLeftSemiCorrelatedApply::CLogicalLeftSemiCorrelatedApply(
	CMemoryPool *mp, CColRefArray *pdrspqcrInner, EOperatorId eopidOriginSubq)
	: CLogicalLeftSemiApply(mp, pdrspqcrInner, eopidOriginSubq)
{
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalLeftSemiCorrelatedApply::PxfsCandidates
//
//	@doc:
//		Get candidate xforms
//
//---------------------------------------------------------------------------
CXformSet *
CLogicalLeftSemiCorrelatedApply::PxfsCandidates(CMemoryPool *mp) const
{
	CXformSet *xform_set = SPQOS_NEW(mp) CXformSet(mp);
	(void) xform_set->ExchangeSet(CXform::ExfImplementLeftSemiCorrelatedApply);

	return xform_set;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalLeftSemiCorrelatedApply::PopCopyWithRemappedColumns
//
//	@doc:
//		Return a copy of the operator with remapped columns
//
//---------------------------------------------------------------------------
COperator *
CLogicalLeftSemiCorrelatedApply::PopCopyWithRemappedColumns(
	CMemoryPool *mp, UlongToColRefMap *colref_mapping, BOOL must_exist)
{
	CColRefArray *pdrspqcrInner =
		CUtils::PdrspqcrRemap(mp, m_pdrspqcrInner, colref_mapping, must_exist);

	return SPQOS_NEW(mp)
		CLogicalLeftSemiCorrelatedApply(mp, pdrspqcrInner, m_eopidOriginSubq);
}


// EOF
