//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CPhysicalInnerHashJoin.cpp
//
//	@doc:
//		Implementation of inner hash join operator
//---------------------------------------------------------------------------

#include "spqopt/operators/CPhysicalInnerHashJoin.h"

#include "spqos/base.h"

#include "spqopt/base/CDistributionSpecHashed.h"
#include "spqopt/base/CUtils.h"
#include "spqopt/operators/CExpressionHandle.h"

using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalInnerHashJoin::CPhysicalInnerHashJoin
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CPhysicalInnerHashJoin::CPhysicalInnerHashJoin(
	CMemoryPool *mp, CExpressionArray *pdrspqexprOuterKeys,
	CExpressionArray *pdrspqexprInnerKeys, IMdIdArray *hash_opfamilies,
	BOOL is_null_aware, CXform::EXformId origin_xform)
	: CPhysicalHashJoin(mp, pdrspqexprOuterKeys, pdrspqexprInnerKeys,
						hash_opfamilies, is_null_aware, origin_xform)
{
}


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalInnerHashJoin::~CPhysicalInnerHashJoin
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CPhysicalInnerHashJoin::~CPhysicalInnerHashJoin()
{
}


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalInnerHashJoin::PdshashedCreateMatching
//
//	@doc:
//		Helper function for creating a matching hashed distribution
//
//---------------------------------------------------------------------------
CDistributionSpecHashed *
CPhysicalInnerHashJoin::PdshashedCreateMatching(
	CMemoryPool *mp, CDistributionSpecHashed *pdshashed,
	ULONG
		ulSourceChild  // index of child that delivered the given hashed distribution
) const
{
	SPQOS_ASSERT(NULL != pdshashed);

	CDistributionSpecHashed *pdshashedMatching =
		PdshashedMatching(mp, pdshashed, ulSourceChild);

	// create a new spec with input and the output spec as equivalents, as you don't want to lose
	// the already existing equivalent specs of pdshashed
	// NB: The matching spec is added at the beginning.
	pdshashedMatching->Pdrspqexpr()->AddRef();
	pdshashed->AddRef();
	if (NULL != pdshashedMatching->Opfamilies())
	{
		pdshashedMatching->Opfamilies()->AddRef();
	}
	CDistributionSpecHashed *pdsHashedMatchingEquivalents =
		SPQOS_NEW(mp) CDistributionSpecHashed(
			pdshashedMatching->Pdrspqexpr(),
			pdshashedMatching->FNullsColocated(),
			pdshashed,	// matching distribution spec is equivalent to passed distribution spec
			pdshashedMatching->Opfamilies());
	pdshashedMatching->Release();
	return pdsHashedMatchingEquivalents;
}


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalInnerHashJoin::PdsDeriveFromHashedChildren
//
//	@doc:
//		Derive hash join distribution from hashed children;
//		return NULL if derivation failed
//
//---------------------------------------------------------------------------
CDistributionSpec *
CPhysicalInnerHashJoin::PdsDeriveFromHashedChildren(
	CMemoryPool *mp, CDistributionSpec *pdsOuter,
	CDistributionSpec *pdsInner) const
{
	SPQOS_ASSERT(NULL != pdsOuter);
	SPQOS_ASSERT(NULL != pdsInner);

	CDistributionSpecHashed *pdshashedOuter =
		CDistributionSpecHashed::PdsConvert(pdsOuter);
	CDistributionSpecHashed *pdshashedInner =
		CDistributionSpecHashed::PdsConvert(pdsInner);

	if (pdshashedOuter->IsCoveredBy(PdrspqexprOuterKeys()) &&
		pdshashedInner->IsCoveredBy(PdrspqexprInnerKeys()))
	{
		// if both sides are hashed on subsets of hash join keys, join's output can be
		// seen as distributed on outer spec or (equivalently) on inner spec,
		// so create a new spec and mark outer and inner as equivalent
		CDistributionSpecHashed *combined_hashed_spec =
			pdshashedOuter->Combine(mp, pdshashedInner);
		return combined_hashed_spec;
	}

	return NULL;
}


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalInnerHashJoin::PdsDeriveFromReplicatedOuter
//
//	@doc:
//		Derive hash join distribution from a replicated outer child;
//
//---------------------------------------------------------------------------
CDistributionSpec *
CPhysicalInnerHashJoin::PdsDeriveFromReplicatedOuter(
	CMemoryPool *mp,
	CDistributionSpec *
#ifdef SPQOS_DEBUG
		pdsOuter
#endif	// SPQOS_DEBUG
	,
	CDistributionSpec *pdsInner) const
{
	SPQOS_ASSERT(NULL != pdsOuter);
	SPQOS_ASSERT(NULL != pdsInner);
	SPQOS_ASSERT(CDistributionSpec::EdtStrictReplicated == pdsOuter->Edt());

	// if outer child is replicated, join results distribution is defined by inner child
	if (CDistributionSpec::EdtHashed == pdsInner->Edt())
	{
		CDistributionSpecHashed *pdshashedInner =
			CDistributionSpecHashed::PdsConvert(pdsInner);
		if (pdshashedInner->IsCoveredBy(PdrspqexprInnerKeys()))
		{
			// inner child is hashed on a subset of inner hashkeys,
			// return a hashed distribution equivalent to a matching outer distribution
			return PdshashedCreateMatching(mp, pdshashedInner,
										   1 /*ulSourceChild*/);
		}
	}

	// otherwise, pass-through inner distribution
	pdsInner->AddRef();
	return pdsInner;
}


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalInnerHashJoin::PdsDeriveFromHashedOuter
//
//	@doc:
//		Derive hash join distribution from a hashed outer child;
//		return NULL if derivation failed
//
//---------------------------------------------------------------------------
CDistributionSpec *
CPhysicalInnerHashJoin::PdsDeriveFromHashedOuter(CMemoryPool *mp,
												 CDistributionSpec *pdsOuter,
												 CDistributionSpec *
#ifdef SPQOS_DEBUG
													 pdsInner
#endif	// SPQOS_DEBUG
) const
{
	SPQOS_ASSERT(NULL != pdsOuter);
	SPQOS_ASSERT(NULL != pdsInner);

	SPQOS_ASSERT(CDistributionSpec::EdtHashed == pdsOuter->Edt());

	CDistributionSpecHashed *pdshashedOuter =
		CDistributionSpecHashed::PdsConvert(pdsOuter);
	if (pdshashedOuter->IsCoveredBy(PdrspqexprOuterKeys()))
	{
		// outer child is hashed on a subset of outer hashkeys,
		// return a hashed distribution equivalent to a matching outer distribution
		return PdshashedCreateMatching(mp, pdshashedOuter, 0 /*ulSourceChild*/);
	}

	return NULL;
}


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalInnerHashJoin::PdsDerive
//
//	@doc:
//		Derive distribution
//
//---------------------------------------------------------------------------
CDistributionSpec *
CPhysicalInnerHashJoin::PdsDerive(CMemoryPool *mp,
								  CExpressionHandle &exprhdl) const
{
	CDistributionSpec *pdsOuter = exprhdl.Pdpplan(0 /*child_index*/)->Pds();
	CDistributionSpec *pdsInner = exprhdl.Pdpplan(1 /*child_index*/)->Pds();

	if (CDistributionSpec::EdtUniversal == pdsOuter->Edt())
	{
		// if outer is universal, pass through inner distribution
		pdsInner->AddRef();
		return pdsInner;
	}

	if (CDistributionSpec::EdtHashed == pdsOuter->Edt() &&
		CDistributionSpec::EdtHashed == pdsInner->Edt())
	{
		CDistributionSpec *pdsDerived =
			PdsDeriveFromHashedChildren(mp, pdsOuter, pdsInner);
		if (NULL != pdsDerived)
		{
			return pdsDerived;
		}
	}

	if (CDistributionSpec::EdtStrictReplicated == pdsOuter->Edt())
	{
		return PdsDeriveFromReplicatedOuter(mp, pdsOuter, pdsInner);
	}

	if (CDistributionSpec::EdtHashed == pdsOuter->Edt())
	{
		CDistributionSpec *pdsDerived =
			PdsDeriveFromHashedOuter(mp, pdsOuter, pdsInner);
		if (NULL != pdsDerived)
		{
			return pdsDerived;
		}
	}

	// otherwise, pass through outer distribution
	pdsOuter->AddRef();
	return pdsOuter;
}


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalInnerHashJoin::PppsRequired
//
//	@doc:
//		Compute required partition propagation of the n-th child
//
//---------------------------------------------------------------------------
CPartitionPropagationSpec *
CPhysicalInnerHashJoin::PppsRequired(CMemoryPool *mp,
									 CExpressionHandle &exprhdl,
									 CPartitionPropagationSpec *pppsRequired,
									 ULONG child_index,
									 CDrvdPropArray *pdrspqdpCtxt,
									 ULONG ulOptReq)
{
	if (1 == ulOptReq)
	{
		// request (1): push partition propagation requests to join's children,
		// do not consider possible dynamic partition elimination using join predicate here,
		// this is handled by optimization request (0) below
		return CPhysical::PppsRequiredPushThruNAry(mp, exprhdl, pppsRequired,
												   child_index);
	}

	// request (0): push partition progagation requests to join child considering
	// DPE possibility. For HJ, PS request is pushed to the inner child if there
	// is a consumer (DTS) on the outer side of the join.
	SPQOS_ASSERT(0 == ulOptReq);
	return PppsRequiredJoinChild(mp, exprhdl, pppsRequired, child_index,
								 pdrspqdpCtxt, false);
}

// EOF
