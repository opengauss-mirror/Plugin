//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 EMC Corp.
//
//	@filename:
//		CLogicalFullOuterJoin.cpp
//
//	@doc:
//		Implementation of full outer join operator
//---------------------------------------------------------------------------

#include "spqopt/operators/CLogicalFullOuterJoin.h"

#include "spqos/base.h"

#include "spqopt/base/CColRefSet.h"
#include "spqopt/operators/CExpression.h"
#include "spqopt/operators/CExpressionHandle.h"

using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CLogicalFullOuterJoin::CLogicalFullOuterJoin
//
//	@doc:
//		ctor
//
//---------------------------------------------------------------------------
CLogicalFullOuterJoin::CLogicalFullOuterJoin(CMemoryPool *mp,
											 CXform::EXformId origin_xform)
	: CLogicalJoin(mp, origin_xform)
{
	SPQOS_ASSERT(NULL != mp);
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalFullOuterJoin::DeriveMaxCard
//
//	@doc:
//		Derive max card
//
//---------------------------------------------------------------------------
CMaxCard
CLogicalFullOuterJoin::DeriveMaxCard(CMemoryPool *,	 // mp
									 CExpressionHandle &exprhdl) const
{
	CMaxCard left_child_maxcard = exprhdl.DeriveMaxCard(0);
	CMaxCard right_child_maxcard = exprhdl.DeriveMaxCard(1);

	if (left_child_maxcard.Ull() > 0 && right_child_maxcard.Ull() > 0)
	{
		CMaxCard result_max_card = left_child_maxcard;
		result_max_card *= right_child_maxcard;
		return result_max_card;
	}

	if (left_child_maxcard <= right_child_maxcard)
	{
		return right_child_maxcard;
	}

	return left_child_maxcard;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalFullOuterJoin::PxfsCandidates
//
//	@doc:
//		Get candidate xforms
//
//---------------------------------------------------------------------------
CXformSet *
CLogicalFullOuterJoin::PxfsCandidates(CMemoryPool *mp) const
{
	CXformSet *xform_set = SPQOS_NEW(mp) CXformSet(mp);
	(void) xform_set->ExchangeSet(CXform::ExfExpandFullOuterJoin);
	(void) xform_set->ExchangeSet(CXform::ExfImplementFullOuterMergeJoin);
	return xform_set;
}

// EOF
