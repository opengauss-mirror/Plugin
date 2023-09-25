//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		CLogicalInnerJoin.cpp
//
//	@doc:
//		Implementation of inner join operator
//---------------------------------------------------------------------------

#include "spqopt/operators/CLogicalInnerJoin.h"

#include "spqos/base.h"

#include "spqopt/base/CColRefSet.h"
#include "spqopt/base/CColRefSetIter.h"
#include "spqopt/operators/CExpression.h"
#include "spqopt/operators/CExpressionHandle.h"
#include "spqopt/operators/CPredicateUtils.h"

using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CLogicalInnerJoin::CLogicalInnerJoin
//
//	@doc:
//		ctor
//		Note: 04/09/2009 - ; so far inner join doesn't have any specific
//			members, hence, no need for a separate pattern ctor
//
//---------------------------------------------------------------------------
CLogicalInnerJoin::CLogicalInnerJoin(CMemoryPool *mp,
									 CXform::EXformId origin_xform)
	: CLogicalJoin(mp, origin_xform)
{
	SPQOS_ASSERT(NULL != mp);
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalInnerJoin::DeriveMaxCard
//
//	@doc:
//		Derive max card
//
//---------------------------------------------------------------------------
CMaxCard
CLogicalInnerJoin::DeriveMaxCard(CMemoryPool *,	 // mp
								 CExpressionHandle &exprhdl) const
{
	return CLogical::Maxcard(exprhdl, 2 /*ulScalarIndex*/, MaxcardDef(exprhdl));
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalInnerJoin::PxfsCandidates
//
//	@doc:
//		Get candidate xforms
//
//---------------------------------------------------------------------------
CXformSet *
CLogicalInnerJoin::PxfsCandidates(CMemoryPool *mp) const
{
	CXformSet *xform_set = SPQOS_NEW(mp) CXformSet(mp);

	(void) xform_set->ExchangeSet(CXform::ExfImplementInnerJoin);
	(void) xform_set->ExchangeSet(CXform::ExfSubqJoin2Apply);
	(void) xform_set->ExchangeSet(
		CXform::ExfInnerJoin2PartialDynamicIndexGetApply);
	(void) xform_set->ExchangeSet(
		CXform::ExfInnerJoinWithInnerSelect2PartialDynamicIndexGetApply);
	(void) xform_set->ExchangeSet(CXform::ExfJoin2BitmapIndexGetApply);
	(void) xform_set->ExchangeSet(CXform::ExfJoin2IndexGetApply);

	(void) xform_set->ExchangeSet(CXform::ExfJoinCommutativity);
	(void) xform_set->ExchangeSet(CXform::ExfJoinAssociativity);
	(void) xform_set->ExchangeSet(CXform::ExfInnerJoinSemiJoinSwap);
	(void) xform_set->ExchangeSet(CXform::ExfInnerJoinAntiSemiJoinSwap);
	(void) xform_set->ExchangeSet(CXform::ExfInnerJoinAntiSemiJoinNotInSwap);

	return xform_set;
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalInnerJoin::FFewerConj
//
//	@doc:
//		Compare two innerJoin group expressions, test whether the first one
//		has less join predicates than the second one. This is used to
//		prioritize innerJoin with less predicates for stats derivation
//
//---------------------------------------------------------------------------
BOOL
CLogicalInnerJoin::FFewerConj(CMemoryPool *mp, CGroupExpression *pgexprFst,
							  CGroupExpression *pgexprSnd)
{
	if (NULL == pgexprFst || NULL == pgexprSnd)
	{
		return false;
	}

	if (COperator::EopLogicalInnerJoin != pgexprFst->Pop()->Eopid() ||
		COperator::EopLogicalInnerJoin != pgexprSnd->Pop()->Eopid())
	{
		return false;
	}

	// third child must be the group for join conditions
	CGroup *pgroupScalarFst = (*pgexprFst)[2];
	CGroup *pgroupScalarSnd = (*pgexprSnd)[2];
	SPQOS_ASSERT(pgroupScalarFst->FScalar());
	SPQOS_ASSERT(pgroupScalarSnd->FScalar());

	CExpressionArray *pdrspqexprConjFst = CPredicateUtils::PdrspqexprConjuncts(
		mp, pgroupScalarFst->PexprScalarRep());
	CExpressionArray *pdrspqexprConjSnd = CPredicateUtils::PdrspqexprConjuncts(
		mp, pgroupScalarSnd->PexprScalarRep());

	ULONG ulConjFst = pdrspqexprConjFst->Size();
	ULONG ulConjSnd = pdrspqexprConjSnd->Size();

	pdrspqexprConjFst->Release();
	pdrspqexprConjSnd->Release();

	return ulConjFst < ulConjSnd;
}

// EOF
