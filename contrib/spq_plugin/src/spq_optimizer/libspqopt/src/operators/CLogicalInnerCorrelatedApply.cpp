//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CLogicalInnerCorrelatedApply.cpp
//
//	@doc:
//		Implementation of inner correlated apply operator
//---------------------------------------------------------------------------

#include "spqopt/operators/CLogicalInnerCorrelatedApply.h"

#include "spqos/base.h"

using namespace spqopt;

//---------------------------------------------------------------------------
//	@function:
//		CLogicalInnerCorrelatedApply::CLogicalInnerCorrelatedApply
//
//	@doc:
//		Ctor - for patterns
//
//---------------------------------------------------------------------------
CLogicalInnerCorrelatedApply::CLogicalInnerCorrelatedApply(CMemoryPool *mp)
	: CLogicalInnerApply(mp)
{
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalInnerCorrelatedApply::CLogicalInnerCorrelatedApply
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CLogicalInnerCorrelatedApply::CLogicalInnerCorrelatedApply(
	CMemoryPool *mp, CColRefArray *pdrspqcrInner, EOperatorId eopidOriginSubq)
	: CLogicalInnerApply(mp, pdrspqcrInner, eopidOriginSubq)
{
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalInnerCorrelatedApply::PxfsCandidates
//
//	@doc:
//		Get candidate xforms
//
//---------------------------------------------------------------------------
CXformSet *
CLogicalInnerCorrelatedApply::PxfsCandidates(CMemoryPool *mp) const
{
	CXformSet *xform_set = SPQOS_NEW(mp) CXformSet(mp);
	(void) xform_set->ExchangeSet(CXform::ExfImplementInnerCorrelatedApply);

	return xform_set;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalInnerCorrelatedApply::Matches
//
//	@doc:
//		Match function
//
//---------------------------------------------------------------------------
BOOL
CLogicalInnerCorrelatedApply::Matches(COperator *pop) const
{
	if (pop->Eopid() == Eopid())
	{
		return m_pdrspqcrInner->Equals(
			CLogicalInnerCorrelatedApply::PopConvert(pop)->PdrgPcrInner());
	}

	return false;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalInnerCorrelatedApply::PopCopyWithRemappedColumns
//
//	@doc:
//		Return a copy of the operator with remapped columns
//
//---------------------------------------------------------------------------
COperator *
CLogicalInnerCorrelatedApply::PopCopyWithRemappedColumns(
	CMemoryPool *mp, UlongToColRefMap *colref_mapping, BOOL must_exist)
{
	CColRefArray *pdrspqcrInner =
		CUtils::PdrspqcrRemap(mp, m_pdrspqcrInner, colref_mapping, must_exist);

	return SPQOS_NEW(mp)
		CLogicalInnerCorrelatedApply(mp, pdrspqcrInner, m_eopidOriginSubq);
}

// EOF
