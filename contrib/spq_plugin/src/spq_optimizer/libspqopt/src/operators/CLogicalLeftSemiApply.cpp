//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CLogicalLeftSemiApply.cpp
//
//	@doc:
//		Implementation of left semi-apply operator
//---------------------------------------------------------------------------

#include "spqopt/operators/CLogicalLeftSemiApply.h"

#include "spqos/base.h"

#include "spqopt/base/CColRefSet.h"
#include "spqopt/operators/CExpression.h"
#include "spqopt/operators/CExpressionHandle.h"

using namespace spqopt;

//---------------------------------------------------------------------------
//	@function:
//		CLogicalLeftSemiApply::DeriveMaxCard
//
//	@doc:
//		Derive max card
//
//---------------------------------------------------------------------------
CMaxCard
CLogicalLeftSemiApply::DeriveMaxCard(CMemoryPool *,	 // mp
									 CExpressionHandle &exprhdl) const
{
	return CLogical::Maxcard(exprhdl, 2 /*ulScalarIndex*/,
							 exprhdl.DeriveMaxCard(0));
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalLeftSemiApply::PxfsCandidates
//
//	@doc:
//		Get candidate xforms
//
//---------------------------------------------------------------------------
CXformSet *
CLogicalLeftSemiApply::PxfsCandidates(CMemoryPool *mp) const
{
	CXformSet *xform_set = SPQOS_NEW(mp) CXformSet(mp);

	(void) xform_set->ExchangeSet(CXform::ExfLeftSemiApply2LeftSemiJoin);
	(void) xform_set->ExchangeSet(
		CXform::ExfLeftSemiApplyWithExternalCorrs2InnerJoin);
	(void) xform_set->ExchangeSet(
		CXform::ExfLeftSemiApply2LeftSemiJoinNoCorrelations);

	return xform_set;
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalLeftSemiApply::DeriveOutputColumns
//
//	@doc:
//		Derive output columns
//
//---------------------------------------------------------------------------
CColRefSet *
CLogicalLeftSemiApply::DeriveOutputColumns(CMemoryPool *,  // mp
										   CExpressionHandle &exprhdl)
{
	SPQOS_ASSERT(3 == exprhdl.Arity());

	return PcrsDeriveOutputPassThru(exprhdl);
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalLeftSemiApply::PopCopyWithRemappedColumns
//
//	@doc:
//		Return a copy of the operator with remapped columns
//
//---------------------------------------------------------------------------
COperator *
CLogicalLeftSemiApply::PopCopyWithRemappedColumns(
	CMemoryPool *mp, UlongToColRefMap *colref_mapping, BOOL must_exist)
{
	CColRefArray *pdrspqcrInner =
		CUtils::PdrspqcrRemap(mp, m_pdrspqcrInner, colref_mapping, must_exist);

	return SPQOS_NEW(mp)
		CLogicalLeftSemiApply(mp, pdrspqcrInner, m_eopidOriginSubq);
}

// EOF
