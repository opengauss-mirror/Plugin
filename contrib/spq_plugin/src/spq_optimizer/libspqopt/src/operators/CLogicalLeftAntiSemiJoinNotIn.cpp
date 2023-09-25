//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 EMC Corp.
//
//	@filename:
//		CLogicalLeftAntiSemiJoinNotIn.cpp
//
//	@doc:
//		Implementation of left anti semi join operator
//---------------------------------------------------------------------------

#include "spqopt/operators/CLogicalLeftAntiSemiJoinNotIn.h"

#include "spqos/base.h"

#include "spqopt/base/CColRefSet.h"
#include "spqopt/operators/CExpression.h"
#include "spqopt/operators/CExpressionHandle.h"

using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CLogicalLeftAntiSemiJoinNotIn::CLogicalLeftAntiSemiJoinNotIn
//
//	@doc:
//		ctor
//
//---------------------------------------------------------------------------
CLogicalLeftAntiSemiJoinNotIn::CLogicalLeftAntiSemiJoinNotIn(
	CMemoryPool *mp, CXform::EXformId origin_xform)
	: CLogicalLeftAntiSemiJoin(mp, origin_xform)
{
	SPQOS_ASSERT(NULL != mp);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalLeftAntiSemiJoinNotIn::PxfsCandidates
//
//	@doc:
//		Get candidate xforms
//
//---------------------------------------------------------------------------
CXformSet *
CLogicalLeftAntiSemiJoinNotIn::PxfsCandidates(CMemoryPool *mp) const
{
	CXformSet *xform_set = SPQOS_NEW(mp) CXformSet(mp);

	(void) xform_set->ExchangeSet(
		CXform::ExfAntiSemiJoinNotInAntiSemiJoinNotInSwap);
	(void) xform_set->ExchangeSet(CXform::ExfAntiSemiJoinNotInAntiSemiJoinSwap);
	(void) xform_set->ExchangeSet(CXform::ExfAntiSemiJoinNotInSemiJoinSwap);
	(void) xform_set->ExchangeSet(CXform::ExfAntiSemiJoinNotInInnerJoinSwap);
	(void) xform_set->ExchangeSet(
		CXform::ExfLeftAntiSemiJoinNotIn2CrossProduct);
	(void) xform_set->ExchangeSet(CXform::ExfLeftAntiSemiJoinNotIn2NLJoinNotIn);
	(void) xform_set->ExchangeSet(
		CXform::ExfLeftAntiSemiJoinNotIn2HashJoinNotIn);
	return xform_set;
}

// EOF
