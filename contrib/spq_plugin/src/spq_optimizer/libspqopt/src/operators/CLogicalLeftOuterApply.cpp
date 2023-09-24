//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CLogicalLeftOuterApply.cpp
//
//	@doc:
//		Implementation of left outer apply operator
//---------------------------------------------------------------------------

#include "spqopt/operators/CLogicalLeftOuterApply.h"

#include "spqos/base.h"

using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CLogicalLeftOuterApply::CLogicalLeftOuterApply
//
//	@doc:
//		ctor
//
//---------------------------------------------------------------------------
CLogicalLeftOuterApply::CLogicalLeftOuterApply(CMemoryPool *mp)
	: CLogicalApply(mp)
{
	SPQOS_ASSERT(NULL != mp);

	m_fPattern = true;
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalLeftOuterApply::CLogicalLeftOuterApply
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CLogicalLeftOuterApply::CLogicalLeftOuterApply(CMemoryPool *mp,
											   CColRefArray *pdrspqcrInner,
											   EOperatorId eopidOriginSubq)
	: CLogicalApply(mp, pdrspqcrInner, eopidOriginSubq)
{
	SPQOS_ASSERT(0 < pdrspqcrInner->Size());
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalLeftOuterApply::~CLogicalLeftOuterApply
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CLogicalLeftOuterApply::~CLogicalLeftOuterApply()
{
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalLeftOuterApply::DeriveMaxCard
//
//	@doc:
//		Derive max card
//
//---------------------------------------------------------------------------
CMaxCard
CLogicalLeftOuterApply::DeriveMaxCard(CMemoryPool *,  // mp
									  CExpressionHandle &exprhdl) const
{
	return CLogical::Maxcard(exprhdl, 2 /*ulScalarIndex*/,
							 exprhdl.DeriveMaxCard(0));
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalLeftOuterApply::PxfsCandidates
//
//	@doc:
//		Get candidate xforms
//
//---------------------------------------------------------------------------
CXformSet *
CLogicalLeftOuterApply::PxfsCandidates(CMemoryPool *mp) const
{
	CXformSet *xform_set = SPQOS_NEW(mp) CXformSet(mp);

	(void) xform_set->ExchangeSet(CXform::ExfLeftOuterApply2LeftOuterJoin);
	(void) xform_set->ExchangeSet(
		CXform::ExfLeftOuterApply2LeftOuterJoinNoCorrelations);

	return xform_set;
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalLeftOuterApply::PopCopyWithRemappedColumns
//
//	@doc:
//		Return a copy of the operator with remapped columns
//
//---------------------------------------------------------------------------
COperator *
CLogicalLeftOuterApply::PopCopyWithRemappedColumns(
	CMemoryPool *mp, UlongToColRefMap *colref_mapping, BOOL must_exist)
{
	CColRefArray *pdrspqcrInner =
		CUtils::PdrspqcrRemap(mp, m_pdrspqcrInner, colref_mapping, must_exist);

	return SPQOS_NEW(mp)
		CLogicalLeftOuterApply(mp, pdrspqcrInner, m_eopidOriginSubq);
}

// EOF
