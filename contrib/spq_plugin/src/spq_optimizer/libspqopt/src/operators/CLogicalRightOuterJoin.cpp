//---------------------------------------------------------------------------
//	Greenplum Database
//  Copyright (c) 2020 VMware, Inc.
//
//	@filename:
//		CLogicalRightOuterJoin.cpp
//
//	@doc:
//		Implementation of right outer join operator
//---------------------------------------------------------------------------

#include "spqopt/operators/CLogicalRightOuterJoin.h"

#include "spqos/base.h"

#include "spqopt/base/CColRefSet.h"
#include "spqopt/operators/CExpression.h"
#include "spqopt/operators/CExpressionHandle.h"

using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CLogicalRightOuterJoin::CLogicalRightOuterJoin
//
//	@doc:
//		ctor
//
//---------------------------------------------------------------------------
CLogicalRightOuterJoin::CLogicalRightOuterJoin(CMemoryPool *mp,
											   CXform::EXformId origin_xform)
	: CLogicalJoin(mp, origin_xform)
{
	SPQOS_ASSERT(NULL != mp);
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalRightOuterJoin::DeriveMaxCard
//
//	@doc:
//		Derive max card
//
//---------------------------------------------------------------------------
CMaxCard
CLogicalRightOuterJoin::DeriveMaxCard(CMemoryPool *,  // mp
									  CExpressionHandle &exprhdl) const
{
	CMaxCard maxCardOuter = exprhdl.DeriveMaxCard(0);
	CMaxCard maxCard = exprhdl.DeriveMaxCard(1);

	// if the outer has a max card of 0, that will not make the ROJ's
	// max card go to 0
	if (0 < maxCardOuter.Ull())
	{
		maxCard *= maxCardOuter;
	}

	return CLogical::Maxcard(exprhdl, 2 /*ulScalarIndex*/, maxCard);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalRightOuterJoin::PxfsCandidates
//
//	@doc:
//		Get candidate xforms
//
//---------------------------------------------------------------------------
CXformSet *
CLogicalRightOuterJoin::PxfsCandidates(CMemoryPool *mp) const
{
	CXformSet *xform_set = SPQOS_NEW(mp) CXformSet(mp);

	(void) xform_set->ExchangeSet(CXform::ExfRightOuterJoin2HashJoin);

	return xform_set;
}

// EOF
