//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CLogicalLeftAntiSemiApply.cpp
//
//	@doc:
//		Implementation of left anti-semi-apply operator
//---------------------------------------------------------------------------

#include "spqopt/operators/CLogicalLeftAntiSemiApply.h"

#include "spqos/base.h"

#include "spqopt/base/CColRefSet.h"
#include "spqopt/operators/CExpression.h"
#include "spqopt/operators/CExpressionHandle.h"

using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CLogicalLeftAntiSemiApply::PkcDeriveKeys
//
//	@doc:
//		Derive key collection
//
//---------------------------------------------------------------------------
CKeyCollection *
CLogicalLeftAntiSemiApply::DeriveKeyCollection(CMemoryPool *,  // mp
											   CExpressionHandle &exprhdl) const
{
	return PkcDeriveKeysPassThru(exprhdl, 0 /* ulChild */);
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalLeftAntiSemiApply::DeriveMaxCard
//
//	@doc:
//		Derive max card
//
//---------------------------------------------------------------------------
CMaxCard
CLogicalLeftAntiSemiApply::DeriveMaxCard(CMemoryPool *,	 // mp
										 CExpressionHandle &exprhdl) const
{
	// pass on max card of first child
	return exprhdl.DeriveMaxCard(0);
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalLeftAntiSemiApply::PxfsCandidates
//
//	@doc:
//		Get candidate xforms
//
//---------------------------------------------------------------------------
CXformSet *
CLogicalLeftAntiSemiApply::PxfsCandidates(CMemoryPool *mp) const
{
	CXformSet *xform_set = SPQOS_NEW(mp) CXformSet(mp);

	(void) xform_set->ExchangeSet(
		CXform::ExfLeftAntiSemiApply2LeftAntiSemiJoin);
	(void) xform_set->ExchangeSet(
		CXform::ExfLeftAntiSemiApply2LeftAntiSemiJoinNoCorrelations);

	return xform_set;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalLeftAntiSemiApply::PopCopyWithRemappedColumns
//
//	@doc:
//		Return a copy of the operator with remapped columns
//
//---------------------------------------------------------------------------
COperator *
CLogicalLeftAntiSemiApply::PopCopyWithRemappedColumns(
	CMemoryPool *mp, UlongToColRefMap *colref_mapping, BOOL must_exist)
{
	CColRefArray *pdrspqcrInner =
		CUtils::PdrspqcrRemap(mp, m_pdrspqcrInner, colref_mapping, must_exist);

	return SPQOS_NEW(mp)
		CLogicalLeftAntiSemiApply(mp, pdrspqcrInner, m_eopidOriginSubq);
}

// EOF
