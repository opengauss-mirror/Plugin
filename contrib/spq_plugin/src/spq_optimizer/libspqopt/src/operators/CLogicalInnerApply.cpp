//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CLogicalInnerApply.cpp
//
//	@doc:
//		Implementation of inner apply operator
//---------------------------------------------------------------------------

#include "spqopt/operators/CLogicalInnerApply.h"

#include "spqos/base.h"

using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CLogicalInnerApply::CLogicalInnerApply
//
//	@doc:
//		ctor
//
//---------------------------------------------------------------------------
CLogicalInnerApply::CLogicalInnerApply(CMemoryPool *mp) : CLogicalApply(mp)
{
	SPQOS_ASSERT(NULL != mp);

	m_fPattern = true;
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalInnerApply::CLogicalInnerApply
//
//	@doc:
//		ctor
//
//---------------------------------------------------------------------------
CLogicalInnerApply::CLogicalInnerApply(CMemoryPool *mp,
									   CColRefArray *pdrspqcrInner,
									   EOperatorId eopidOriginSubq)
	: CLogicalApply(mp, pdrspqcrInner, eopidOriginSubq)
{
	SPQOS_ASSERT(0 < pdrspqcrInner->Size());
}



//---------------------------------------------------------------------------
//	@function:
//		CLogicalInnerApply::~CLogicalInnerApply
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CLogicalInnerApply::~CLogicalInnerApply()
{
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalInnerApply::DeriveMaxCard
//
//	@doc:
//		Derive max card
//
//---------------------------------------------------------------------------
CMaxCard
CLogicalInnerApply::DeriveMaxCard(CMemoryPool *,  // mp
								  CExpressionHandle &exprhdl) const
{
	return CLogical::Maxcard(exprhdl, 2 /*ulScalarIndex*/, MaxcardDef(exprhdl));
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalInnerApply::PxfsCandidates
//
//	@doc:
//		Get candidate xforms
//
//---------------------------------------------------------------------------
CXformSet *
CLogicalInnerApply::PxfsCandidates(CMemoryPool *mp) const
{
	CXformSet *xform_set = SPQOS_NEW(mp) CXformSet(mp);

	(void) xform_set->ExchangeSet(CXform::ExfInnerApply2InnerJoin);
	(void) xform_set->ExchangeSet(
		CXform::ExfInnerApply2InnerJoinNoCorrelations);
	(void) xform_set->ExchangeSet(CXform::ExfInnerApplyWithOuterKey2InnerJoin);

	return xform_set;
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalInnerApply::PopCopyWithRemappedColumns
//
//	@doc:
//		Return a copy of the operator with remapped columns
//
//---------------------------------------------------------------------------
COperator *
CLogicalInnerApply::PopCopyWithRemappedColumns(CMemoryPool *mp,
											   UlongToColRefMap *colref_mapping,
											   BOOL must_exist)
{
	CColRefArray *pdrspqcrInner =
		CUtils::PdrspqcrRemap(mp, m_pdrspqcrInner, colref_mapping, must_exist);

	return SPQOS_NEW(mp) CLogicalInnerApply(mp, pdrspqcrInner, m_eopidOriginSubq);
}

// EOF
