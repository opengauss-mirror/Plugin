//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CPhysicalHashJoin.cpp
//
//	@doc:
//		Implementation of hash join operator
//---------------------------------------------------------------------------

#include "spqopt/operators/CPhysicalHashJoin.h"

#include "spqos/base.h"

#include "spqopt/base/CCastUtils.h"
#include "spqopt/base/CDistributionSpecHashed.h"
#include "spqopt/base/CDistributionSpecNonSingleton.h"
#include "spqopt/base/CDistributionSpecReplicated.h"
#include "spqopt/base/CDistributionSpecSingleton.h"
#include "spqopt/base/COptCtxt.h"
#include "spqopt/base/CPartIndexMap.h"
#include "spqopt/base/CUtils.h"
#include "spqopt/operators/CExpressionHandle.h"
#include "spqopt/operators/CPredicateUtils.h"
#include "spqopt/operators/CScalarConst.h"
#include "spqopt/operators/CScalarIdent.h"

using namespace spqopt;

// number of non-redistribute requests created by hash join
#define SPQOPT_NON_HASH_DIST_REQUESTS 3

// maximum number of redistribute requests on single hash join keys
#define SPQOPT_MAX_HASH_DIST_REQUESTS 6

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalHashJoin::CPhysicalHashJoin
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CPhysicalHashJoin::CPhysicalHashJoin(CMemoryPool *mp,
									 CExpressionArray *pdrspqexprOuterKeys,
									 CExpressionArray *pdrspqexprInnerKeys,
									 IMdIdArray *hash_opfamilies,
									 BOOL is_null_aware,
									 CXform::EXformId origin_xform)
	: CPhysicalJoin(mp, origin_xform),
	  m_pdrspqexprOuterKeys(pdrspqexprOuterKeys),
	  m_pdrspqexprInnerKeys(pdrspqexprInnerKeys),
	  m_hash_opfamilies(NULL),
	  m_is_null_aware(is_null_aware),
	  m_pdrspqdsRedistributeRequests(NULL)
{
	SPQOS_ASSERT(NULL != mp);
	SPQOS_ASSERT(NULL != pdrspqexprOuterKeys);
	SPQOS_ASSERT(NULL != pdrspqexprInnerKeys);
	SPQOS_ASSERT(pdrspqexprOuterKeys->Size() == pdrspqexprInnerKeys->Size());

	if (SPQOS_FTRACE(EopttraceConsiderOpfamiliesForDistribution))
	{
		SPQOS_ASSERT(NULL != hash_opfamilies);
		m_hash_opfamilies = hash_opfamilies;
		SPQOS_ASSERT(pdrspqexprOuterKeys->Size() == m_hash_opfamilies->Size());
	}

	CreateOptRequests(mp);
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalHashJoin::~CPhysicalHashJoin
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CPhysicalHashJoin::~CPhysicalHashJoin()
{
	m_pdrspqexprOuterKeys->Release();
	m_pdrspqexprInnerKeys->Release();
	CRefCount::SafeRelease(m_hash_opfamilies);
	CRefCount::SafeRelease(m_pdrspqdsRedistributeRequests);
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalHashJoin::CreateHashRedistributeRequests
//
//	@doc:
//		Create the set of redistribute requests to send to first
//		hash join child
//
//---------------------------------------------------------------------------
void
CPhysicalHashJoin::CreateHashRedistributeRequests(CMemoryPool *mp)
{
	SPQOS_ASSERT(NULL == m_pdrspqdsRedistributeRequests);
	SPQOS_ASSERT(NULL != m_pdrspqexprOuterKeys);
	SPQOS_ASSERT(NULL != m_pdrspqexprInnerKeys);

	CExpressionArray *pdrspqexpr = NULL;
	if (EceoRightToLeft == Eceo())
	{
		pdrspqexpr = m_pdrspqexprInnerKeys;
	}
	else
	{
		pdrspqexpr = m_pdrspqexprOuterKeys;
	}

	m_pdrspqdsRedistributeRequests = SPQOS_NEW(mp) CDistributionSpecArray(mp);
	const ULONG ulExprs =
		std::min((ULONG) SPQOPT_MAX_HASH_DIST_REQUESTS, pdrspqexpr->Size());
	if (1 < ulExprs)
	{
		for (ULONG ul = 0; ul < ulExprs; ul++)
		{
			CExpressionArray *pdrspqexprCurrent =
				SPQOS_NEW(mp) CExpressionArray(mp);
			CExpression *pexpr = (*pdrspqexpr)[ul];
			pexpr->AddRef();
			pdrspqexprCurrent->Append(pexpr);

			IMdIdArray *opfamilies = NULL;
			if (SPQOS_FTRACE(EopttraceConsiderOpfamiliesForDistribution))
			{
				SPQOS_ASSERT(NULL != m_hash_opfamilies);
				opfamilies = SPQOS_NEW(mp) IMdIdArray(mp);
				IMDId *opfamily = (*m_hash_opfamilies)[ul];
				opfamily->AddRef();
				opfamilies->Append(opfamily);
			}

			// add a separate request for each hash join key

			CDistributionSpecHashed *pdshashedCurrent =
				SPQOS_NEW(mp) CDistributionSpecHashed(
					pdrspqexprCurrent, true /* fNullsCollocated */, opfamilies);
			m_pdrspqdsRedistributeRequests->Append(pdshashedCurrent);
		}
	}
	// add a request that contains all hash join keys
	pdrspqexpr->AddRef();
	if (SPQOS_FTRACE(EopttraceConsiderOpfamiliesForDistribution))
	{
		SPQOS_ASSERT(NULL != m_hash_opfamilies);
		m_hash_opfamilies->AddRef();
	}
	CDistributionSpecHashed *pdshashed = SPQOS_NEW(mp) CDistributionSpecHashed(
		pdrspqexpr, true /* fNullsCollocated */, m_hash_opfamilies);
	m_pdrspqdsRedistributeRequests->Append(pdshashed);
}


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalHashJoin::PosRequired
//
//	@doc:
//		Compute required sort order of the n-th child
//
//---------------------------------------------------------------------------
COrderSpec *
CPhysicalHashJoin::PosRequired(CMemoryPool *mp,
							   CExpressionHandle &,	 //exprhdl
							   COrderSpec *,		 // posInput,
							   ULONG
#ifdef SPQOS_DEBUG
								   child_index
#endif	// SPQOS_DEBUG
							   ,
							   CDrvdPropArray *,  // pdrspqdpCtxt
							   ULONG			  // ulOptReq
) const
{
	SPQOS_ASSERT(
		child_index < 2 &&
		"Required sort order can be computed on the relational child only");

	// hash join does not have order requirements to both children, and it
	// does not preserve any sort order
	return SPQOS_NEW(mp) COrderSpec(mp);
}


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalHashJoin::PrsRequired
//
//	@doc:
//		Compute required rewindability of the n-th child
//
//---------------------------------------------------------------------------
CRewindabilitySpec *
CPhysicalHashJoin::PrsRequired(CMemoryPool *mp, CExpressionHandle &exprhdl,
							   CRewindabilitySpec *prsRequired,
							   ULONG child_index,
							   CDrvdPropArray *,  // pdrspqdpCtxt
							   ULONG			  // ulOptReq
) const
{
	SPQOS_ASSERT(
		child_index < 2 &&
		"Required rewindability can be computed on the relational child only");

	if (1 == child_index)
	{
		// If the inner child contains outer references, and the required
		// rewindability is not ErtNone, we must ensure that the inner subtree is
		// at least rescannable, even though a Hash op on the inner side
		// materialized the subtree results.
		if (exprhdl.HasOuterRefs(1) &&
			(prsRequired->Ert() == CRewindabilitySpec::ErtRescannable ||
			 prsRequired->Ert() == CRewindabilitySpec::ErtRewindable))
		{
			return SPQOS_NEW(mp) CRewindabilitySpec(
				CRewindabilitySpec::ErtRescannable, prsRequired->Emht());
		}
		// Otherwise, the inner Hash op will take care of materializing the
		// subtree, so no rewindability type is required
		else
		{
			return SPQOS_NEW(mp) CRewindabilitySpec(CRewindabilitySpec::ErtNone,
												   prsRequired->Emht());
		}
	}

	// pass through requirements to outer child
	return PrsPassThru(mp, exprhdl, prsRequired, 0 /*child_index*/);
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalHashJoin::PdsMatch
//
//	@doc:
//		Compute a distribution matching the distribution delivered by
//		given child
//
//---------------------------------------------------------------------------
CDistributionSpec *
CPhysicalHashJoin::PdsMatch(CMemoryPool *mp, CDistributionSpec *pds,
							ULONG ulSourceChildIndex) const
{
	SPQOS_ASSERT(NULL != pds);

	EChildExecOrder eceo = Eceo();

	// check the type of distribution delivered by first (inner) child
	switch (pds->Edt())
	{
		case CDistributionSpec::EdtUniversal:
			// first child is universal, request second child to execute on a single host to avoid duplicates
			return SPQOS_NEW(mp) CDistributionSpecSingleton();

		case CDistributionSpec::EdtSingleton:
		case CDistributionSpec::EdtStrictSingleton:
			// require second child to provide a matching singleton distribution
			return PdssMatching(mp,
								CDistributionSpecSingleton::PdssConvert(pds));

		case CDistributionSpec::EdtHashed:
			// require second child to provide a matching hashed distribution
			return PdshashedMatching(mp,
									 CDistributionSpecHashed::PdsConvert(pds),
									 ulSourceChildIndex);

		default:
			SPQOS_ASSERT(CDistributionSpec::EdtStrictReplicated == pds->Edt() ||
						CDistributionSpec::EdtTaintedReplicated == pds->Edt());

			if (EceoRightToLeft == eceo)
			{
				SPQOS_ASSERT(1 == ulSourceChildIndex);

				// inner child is replicated, for ROJ outer must be executed on a single (non-master) segment to avoid duplicates
				if (this->Eopid() == EopPhysicalRightOuterHashJoin)
				{
					return SPQOS_NEW(mp) CDistributionSpecSingleton(
						CDistributionSpecSingleton::EstSegment);
				}
				// inner child is replicated, request outer child to have non-singleton distribution
				return SPQOS_NEW(mp) CDistributionSpecNonSingleton();
			}

			SPQOS_ASSERT(0 == ulSourceChildIndex);

			// outer child is replicated, replicate inner child too in order to preserve correctness of semi-join
			return SPQOS_NEW(mp) CDistributionSpecReplicated(
				CDistributionSpec::EdtStrictReplicated);
	}
}


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalHashJoin::PdshashedMatching
//
//	@doc:
//		Compute a hashed distribution matching the given distribution
//
//---------------------------------------------------------------------------
CDistributionSpecHashed *
CPhysicalHashJoin::PdshashedMatching(
	CMemoryPool *mp, CDistributionSpecHashed *pdshashed,
	ULONG
		ulSourceChild  // index of child that delivered the given hashed distribution
) const
{
	SPQOS_ASSERT(2 > ulSourceChild);

	CExpressionArray *pdrspqexprSource = m_pdrspqexprOuterKeys;
	CExpressionArray *pdrspqexprTarget = m_pdrspqexprInnerKeys;
	if (1 == ulSourceChild)
	{
		pdrspqexprSource = m_pdrspqexprInnerKeys;
		pdrspqexprTarget = m_pdrspqexprOuterKeys;
	}

	const CExpressionArray *pdrspqexprDist = pdshashed->Pdrspqexpr();
	const ULONG ulDlvrdSize = pdrspqexprDist->Size();
	const ULONG ulSourceSize = pdrspqexprSource->Size();

	// construct an array of target key expressions matching source key expressions
	CExpressionArray *pdrspqexpr = SPQOS_NEW(mp) CExpressionArray(mp);
	CExpressionArrays *all_equiv_exprs = pdshashed->HashSpecEquivExprs();
	IMdIdArray *opfamilies = NULL;

	if (SPQOS_FTRACE(EopttraceConsiderOpfamiliesForDistribution))
	{
		opfamilies = SPQOS_NEW(mp) IMdIdArray(mp);
	}

	for (ULONG ulDlvrdIdx = 0; ulDlvrdIdx < ulDlvrdSize; ulDlvrdIdx++)
	{
		CExpression *pexprDlvrd = (*pdrspqexprDist)[ulDlvrdIdx];
		CExpressionArray *equiv_distribution_exprs = NULL;
		if (NULL != all_equiv_exprs && all_equiv_exprs->Size() > 0)
			equiv_distribution_exprs = (*all_equiv_exprs)[ulDlvrdIdx];
		for (ULONG idx = 0; idx < ulSourceSize; idx++)
		{
			BOOL fSuccess = false;
			CExpression *source_expr = (*pdrspqexprSource)[idx];
			fSuccess = CUtils::Equals(pexprDlvrd, source_expr);
			if (!fSuccess)
			{
				// if failed to find a equal match in the source distribution expr
				// array, check the equivalent exprs to find a match
				fSuccess =
					CUtils::Contains(equiv_distribution_exprs, source_expr);
			}
			if (fSuccess)
			{
				// TODO: 02/21/2012 - ; source column may be mapped to multiple
				// target columns (e.g. i=j and i=k);
				// in this case, we need to generate multiple optimization requests to the target child
				CExpression *pexprTarget = (*pdrspqexprTarget)[idx];
				pexprTarget->AddRef();
				pdrspqexpr->Append(pexprTarget);

				if (NULL != opfamilies)
				{
					SPQOS_ASSERT(NULL != m_hash_opfamilies);
					IMDId *opfamily = (*m_hash_opfamilies)[idx];
					opfamily->AddRef();
					opfamilies->Append(opfamily);
				}
				break;
			}
		}
	}
	// check if we failed to compute required distribution
	if (pdrspqexpr->Size() != ulDlvrdSize)
	{
		pdrspqexpr->Release();
		if (NULL != pdshashed->PdshashedEquiv())
		{
			CRefCount::SafeRelease(opfamilies);
			// try again using the equivalent hashed distribution
			return PdshashedMatching(mp, pdshashed->PdshashedEquiv(),
									 ulSourceChild);
		}
	}
	if (pdrspqexpr->Size() != ulDlvrdSize)
	{
		// it should never happen, but instead of creating wrong spec, raise an exception
		SPQOS_RAISE(
			CException::ExmaInvalid, CException::ExmiInvalid,
			SPQOS_WSZ_LIT("Unable to create matching hashed distribution."));
	}

	// nulls colocated for inner hash joins, but not colocated in outer hash joins
	BOOL fNullsColocated = true;

	if (!m_is_null_aware &&
		(COperator::EopPhysicalLeftOuterHashJoin == Eopid() ||
		 COperator::EopPhysicalRightOuterHashJoin == Eopid()))
	{
		fNullsColocated = false;
	}

	return SPQOS_NEW(mp)
		CDistributionSpecHashed(pdrspqexpr, fNullsColocated, opfamilies);
}


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalHashJoin::PdsRequiredSingleton
//
//	@doc:
//		Create (singleton, singleton) optimization request
//
//
//---------------------------------------------------------------------------
CDistributionSpec *
CPhysicalHashJoin::PdsRequiredSingleton(CMemoryPool *mp,
										CExpressionHandle &,  // exprhdl
										CDistributionSpec *,  // pdsInput
										ULONG child_index,
										CDrvdPropArray *pdrspqdpCtxt) const
{
	if (FFirstChildToOptimize(child_index))
	{
		// require first child to be singleton
		return SPQOS_NEW(mp) CDistributionSpecSingleton();
	}

	// require a matching distribution from second child
	SPQOS_ASSERT(NULL != pdrspqdpCtxt);
	CDistributionSpec *pdsFirst =
		CDrvdPropPlan::Pdpplan((*pdrspqdpCtxt)[0])->Pds();
	SPQOS_ASSERT(NULL != pdsFirst);

	if (CDistributionSpec::EdtUniversal == pdsFirst->Edt() ||
		CDistributionSpec::EdtTaintedReplicated == pdsFirst->Edt())
	{
		// first child is universal, request second child to execute on a single host to avoid duplicates
		return SPQOS_NEW(mp) CDistributionSpecSingleton();
	}

	if (COptCtxt::PoctxtFromTLS()->OptimizeDMLQueryWithSingletonSegment() &&
		CDistributionSpec::EdtStrictReplicated == pdsFirst->Edt())
	{
		// For a DML query that can be optimized by enforcing a non-master gather motion,
		// we request singleton-segment distribution on the outer child. If the outer child
		// is replicated, no enforcer gets added; in which case pdsFirst is EdtStrictReplicated.
		// Hence handle this scenario here and require a singleton-segment on the
		// inner child to produce a singleton execution alternavtive for the HJ.
		return SPQOS_NEW(mp)
			CDistributionSpecSingleton(CDistributionSpecSingleton::EstSegment);
	}

	SPQOS_ASSERT(CDistributionSpec::EdtSingleton == pdsFirst->Edt() ||
				CDistributionSpec::EdtStrictSingleton == pdsFirst->Edt());

	// require second child to have matching singleton distribution
	return CPhysical::PdssMatching(
		mp, CDistributionSpecSingleton::PdssConvert(pdsFirst));
}


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalHashJoin::PdsRequiredReplicate
//
//	@doc:
//		Create (hashed/non-singleton, broadcast) optimization request
//
//
//---------------------------------------------------------------------------
CDistributionSpec *
CPhysicalHashJoin::PdsRequiredReplicate(
	CMemoryPool *mp, CExpressionHandle &exprhdl, CDistributionSpec *pdsInput,
	ULONG child_index, CDrvdPropArray *pdrspqdpCtxt, ULONG ulOptReq,
	CReqdPropPlan *prppInput)
{
	EChildExecOrder eceo = Eceo();
	if (EceoLeftToRight == eceo)
	{
		// if optimization order is left to right, fall-back to implementation of parent Join operator
		CEnfdDistribution *ped = CPhysicalJoin::Ped(
			mp, exprhdl, prppInput, child_index, pdrspqdpCtxt, ulOptReq);
		CDistributionSpec *pds = ped->PdsRequired();
		pds->AddRef();
		SafeRelease(ped);
		return pds;
	}
	SPQOS_ASSERT(EceoRightToLeft == eceo);

	if (1 == child_index)
	{
		// require inner child to be replicated
		return SPQOS_NEW(mp)
			CDistributionSpecReplicated(CDistributionSpec::EdtReplicated);
	}
	SPQOS_ASSERT(0 == child_index);

	// require a matching distribution from outer child
	CDistributionSpec *pdsInner =
		CDrvdPropPlan::Pdpplan((*pdrspqdpCtxt)[0])->Pds();
	SPQOS_ASSERT(NULL != pdsInner);

	if (CDistributionSpec::EdtUniversal == pdsInner->Edt())
	{
		// first child is universal, request second child to execute on a single host to avoid duplicates
		return SPQOS_NEW(mp) CDistributionSpecSingleton();
	}

	if (ulOptReq == m_pdrspqdsRedistributeRequests->Size() &&
		CDistributionSpec::EdtHashed == pdsInput->Edt())
	{
		// attempt to propagate hashed request to child
		CDistributionSpecHashed *pdshashed = PdshashedPassThru(
			mp, exprhdl, CDistributionSpecHashed::PdsConvert(pdsInput),
			child_index, pdrspqdpCtxt, ulOptReq);
		if (NULL != pdshashed)
		{
			return pdshashed;
		}
	}

	// otherwise, require second child to deliver non-singleton distribution
	SPQOS_ASSERT(CDistributionSpec::EdtStrictReplicated == pdsInner->Edt() ||
				CDistributionSpec::EdtTaintedReplicated == pdsInner->Edt());
	return SPQOS_NEW(mp) CDistributionSpecNonSingleton();
}


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalHashJoin::PdshashedPassThru
//
//	@doc:
//		Create a child hashed distribution request based on input hashed
//		distribution,
//		return NULL if no such request can be created
//
//
//---------------------------------------------------------------------------
CDistributionSpecHashed *
CPhysicalHashJoin::PdshashedPassThru(CMemoryPool *mp,
									 CExpressionHandle &exprhdl,
									 CDistributionSpecHashed *pdshashedInput,
									 ULONG,				// child_index
									 CDrvdPropArray *,	// pdrspqdpCtxt
									 ULONG
#ifdef SPQOS_DEBUG
										 ulOptReq
#endif	// SPQOS_DEBUG
) const
{
	SPQOS_ASSERT(ulOptReq == m_pdrspqdsRedistributeRequests->Size());
	SPQOS_ASSERT(NULL != pdshashedInput);

	if (!SPQOS_FTRACE(EopttraceEnableRedistributeBroadcastHashJoin))
	{
		// this option is disabled
		return NULL;
	}

	// since incoming request is hashed, we attempt here to propagate this request to outer child
	CColRefSet *pcrsOuterOutput =
		exprhdl.DeriveOutputColumns(0 /*child_index*/);
	CExpressionArray *pdrspqexprIncomingRequest = pdshashedInput->Pdrspqexpr();
	CColRefSet *pcrsAllUsed =
		CUtils::PcrsExtractColumns(mp, pdrspqexprIncomingRequest);
	BOOL fSubset = pcrsOuterOutput->ContainsAll(pcrsAllUsed);
	BOOL fDisjoint = pcrsOuterOutput->IsDisjoint(pcrsAllUsed);
	pcrsAllUsed->Release();
	if (fSubset)
	{
		// incoming request uses columns from outer child only, pass it through
		// but create a copy
		CDistributionSpecHashed *pdsHashedRequired = pdshashedInput->Copy(mp);
		return pdsHashedRequired;
	}

	if (!fDisjoint)
	{
		// incoming request intersects with columns from outer child,
		// we restrict the request to outer child columns only, then we pass it through
		CExpressionArray *pdrspqexprChildRequest =
			SPQOS_NEW(mp) CExpressionArray(mp);
		const ULONG size = pdrspqexprIncomingRequest->Size();
		for (ULONG ul = 0; ul < size; ul++)
		{
			CExpression *pexpr = (*pdrspqexprIncomingRequest)[ul];
			CColRefSet *pcrsUsed = pexpr->DeriveUsedColumns();
			if (pcrsOuterOutput->ContainsAll(pcrsUsed))
			{
				// hashed expression uses columns from outer child only, add it to request
				pexpr->AddRef();
				pdrspqexprChildRequest->Append(pexpr);
			}
		}
		SPQOS_ASSERT(0 < pdrspqexprChildRequest->Size());

		CDistributionSpecHashed *pdshashed =
			SPQOS_NEW(mp) CDistributionSpecHashed(
				pdrspqexprChildRequest, pdshashedInput->FNullsColocated());

		// since the other child of the join is replicated, we need to enforce hashed-distribution across segments here
		pdshashed->MarkUnsatisfiableBySingleton();

		return pdshashed;
	}

	return NULL;
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalHashJoin::PdsRequiredRedistribute
//
//	@doc:
//		Compute (redistribute, redistribute) optimization request
//
//
//---------------------------------------------------------------------------
CDistributionSpec *
CPhysicalHashJoin::PdsRequiredRedistribute(CMemoryPool *mp,
										   CExpressionHandle &exprhdl,
										   CDistributionSpec *,	 // pdsInput
										   ULONG child_index,
										   CDrvdPropArray *pdrspqdpCtxt,
										   ULONG ulOptReq) const
{
	if (FFirstChildToOptimize(child_index))
	{
		// require first child to provide a hashed distribution,
		return PdshashedRequired(mp, child_index, ulOptReq);
	}

	// find the distribution delivered by first child
	CDistributionSpec *pdsFirst =
		CDrvdPropPlan::Pdpplan((*pdrspqdpCtxt)[0])->Pds();
	SPQOS_ASSERT(NULL != pdsFirst);

	CDistributionSpec *pdsInputForMatch = NULL;
	if (pdsFirst->Edt() == CDistributionSpec::EdtHashed)
	{
		// we need to create a matching required spec based on the derived distribution spec from
		// the first child. Since that does not contain the m_equiv_hash_exprs (as they are not populated
		// for derived specs), so compute that here.
		CDistributionSpecHashed *pdsHashedFirstChild =
			CDistributionSpecHashed::PdsConvert(pdsFirst);
		CDistributionSpecHashed *pdsHashed = pdsHashedFirstChild->Copy(mp);
		pdsHashed->ComputeEquivHashExprs(mp, exprhdl);
		pdsInputForMatch = pdsHashed;
	}
	else
	{
		pdsInputForMatch = pdsFirst;
	}

	// find the index of the first child
	ULONG ulFirstChild = 0;
	if (EceoRightToLeft == Eceo())
	{
		ulFirstChild = 1;
	}

	// return a matching distribution request for the second child
	CDistributionSpec *pdsMatch = PdsMatch(mp, pdsInputForMatch, ulFirstChild);
	if (pdsFirst->Edt() == CDistributionSpec::EdtHashed)
	{
		// if the input spec was created as a copy, release it
		pdsInputForMatch->Release();
	}
	return pdsMatch;
}

CDistributionSpec *
CPhysicalHashJoin::PdsRequired(
	CMemoryPool * /*mp*/, CExpressionHandle & /*exprhdl*/,
	CDistributionSpec * /*pdsInput*/, ULONG /*child_index*/,
	CDrvdPropArray * /*pdrspqdpCtxt*/,
	ULONG /*ulOptReq*/	// identifies which optimization request should be created
) const
{
	SPQOS_RAISE(
		CException::ExmaInvalid, CException::ExmiInvalid,
		SPQOS_WSZ_LIT("PdsRequired should not be called for CPhysicalHashJoin"));
	return NULL;
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalHashJoin::Ped
//
//	@doc:
//		Compute required distribution of the n-th child;
//		this function creates three optimization requests to join children:
//		Req(1 to N) (redistribute, redistribute), where we request the first hash join child
//			to be distributed on single hash join keys separately, as well as the set
//			of all hash join keys,
//			the second hash join child is always required to match the distribution returned
//			by first child
// 		Req(N + 1) (hashed, broadcast)
// 		Req(N + 2) (non-singleton, broadcast)
// 		Req(N + 3) (singleton, singleton)
//
//		we always check the distribution delivered by the first child (as
//		given by child optimization order), and then match the delivered
//		distribution on the second child
//
//
//---------------------------------------------------------------------------
CEnfdDistribution *
CPhysicalHashJoin::Ped(CMemoryPool *mp, CExpressionHandle &exprhdl,
					   CReqdPropPlan *prppInput, ULONG child_index,
					   CDrvdPropArray *pdrspqdpCtxt, ULONG ulOptReq)
{
	SPQOS_ASSERT(2 > child_index);
	SPQOS_ASSERT(ulOptReq < UlDistrRequests());

	CEnfdDistribution::EDistributionMatching dmatch =
		Edm(prppInput, child_index, pdrspqdpCtxt, ulOptReq);
	CDistributionSpec *const pdsInput = prppInput->Ped()->PdsRequired();

	// if expression has to execute on a single host then we need a gather
	if (exprhdl.NeedsSingletonExecution())
	{
		return SPQOS_NEW(mp) CEnfdDistribution(
			PdsRequireSingleton(mp, exprhdl, pdsInput, child_index), dmatch);
	}

	if (exprhdl.HasOuterRefs())
	{
		if (CDistributionSpec::EdtSingleton == pdsInput->Edt() ||
			CDistributionSpec::EdtStrictReplicated == pdsInput->Edt())
		{
			return SPQOS_NEW(mp) CEnfdDistribution(
				PdsPassThru(mp, exprhdl, pdsInput, child_index), dmatch);
		}
		return SPQOS_NEW(mp)
			CEnfdDistribution(SPQOS_NEW(mp) CDistributionSpecReplicated(
								  CDistributionSpec::EdtStrictReplicated),
							  dmatch);
	}

	const ULONG ulHashDistributeRequests =
		m_pdrspqdsRedistributeRequests->Size();
	if (ulOptReq < ulHashDistributeRequests)
	{
		// requests 1 .. N are (redistribute, redistribute)
		CDistributionSpec *pds = PdsRequiredRedistribute(
			mp, exprhdl, pdsInput, child_index, pdrspqdpCtxt, ulOptReq);
		if (CDistributionSpec::EdtHashed == pds->Edt())
		{
			CDistributionSpecHashed *pdsHashed =
				CDistributionSpecHashed::PdsConvert(pds);
			pdsHashed->ComputeEquivHashExprs(mp, exprhdl);
		}
		return SPQOS_NEW(mp) CEnfdDistribution(pds, dmatch);
	}

	if (ulOptReq == ulHashDistributeRequests ||
		ulOptReq == ulHashDistributeRequests + 1)
	{
		// requests N+1, N+2 are (hashed/non-singleton, replicate)

		CDistributionSpec *pds =
			PdsRequiredReplicate(mp, exprhdl, pdsInput, child_index,
								 pdrspqdpCtxt, ulOptReq, prppInput);
		if (CDistributionSpec::EdtHashed == pds->Edt())
		{
			CDistributionSpecHashed *pdsHashed =
				CDistributionSpecHashed::PdsConvert(pds);
			pdsHashed->ComputeEquivHashExprs(mp, exprhdl);
		}
		return SPQOS_NEW(mp) CEnfdDistribution(pds, dmatch);
	}

	SPQOS_ASSERT(ulOptReq == ulHashDistributeRequests + 2);

	// requests N+3 is (singleton, singleton)

	return SPQOS_NEW(mp) CEnfdDistribution(
		PdsRequiredSingleton(mp, exprhdl, pdsInput, child_index, pdrspqdpCtxt),
		dmatch);
}


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalHashJoin::PdshashedRequired
//
//	@doc:
//		Compute required hashed distribution of the n-th child
//
//---------------------------------------------------------------------------
CDistributionSpecHashed *
CPhysicalHashJoin::PdshashedRequired(CMemoryPool *,	 // mp
									 ULONG,			 // child_index
									 ULONG ulReqIndex) const
{
	SPQOS_ASSERT(ulReqIndex < m_pdrspqdsRedistributeRequests->Size());
	CDistributionSpec *pds = (*m_pdrspqdsRedistributeRequests)[ulReqIndex];

	pds->AddRef();
	return CDistributionSpecHashed::PdsConvert(pds);
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalHashJoin::EpetOrder
//
//	@doc:
//		Return the enforcing type for order property based on this operator;
//
//
//---------------------------------------------------------------------------
CEnfdProp::EPropEnforcingType
CPhysicalHashJoin::EpetOrder(CExpressionHandle &,  // exprhdl
							 const CEnfdOrder *
#ifdef SPQOS_DEBUG
								 peo
#endif	// SPQOS_DEBUG
) const
{
	SPQOS_ASSERT(NULL != peo);
	SPQOS_ASSERT(!peo->PosRequired()->IsEmpty());

	// hash join is not order-preserving;
	// any order requirements have to be enforced on top
	return CEnfdProp::EpetRequired;
}


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalHashJoin::FNullableInnerHashKeys
//
//	@doc:
//		Check whether the hash keys from one child are nullable. pcrsNotNull must
//		be all the "not null" columns coming from that child
//
//---------------------------------------------------------------------------
BOOL
CPhysicalHashJoin::FNullableHashKeys(CColRefSet *pcrsNotNull, BOOL fInner) const
{
	ULONG ulHashKeys = 0;
	if (fInner)
	{
		ulHashKeys = m_pdrspqexprInnerKeys->Size();
	}
	else
	{
		ulHashKeys = m_pdrspqexprOuterKeys->Size();
	}

	for (ULONG ul = 0; ul < ulHashKeys; ul++)
	{
		if (FNullableHashKey(ul, pcrsNotNull, fInner))
		{
			return true;
		}
	}

	return false;
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalHashJoin::FNullableHashKey
//
//	@doc:
//		Check whether a hash key is nullable
//
//---------------------------------------------------------------------------
BOOL
CPhysicalHashJoin::FNullableHashKey(ULONG ulKey, CColRefSet *pcrsNotNull,
									BOOL fInner) const
{
	COperator *pop = NULL;
	if (fInner)
	{
		pop = (*m_pdrspqexprInnerKeys)[ulKey]->Pop();
	}
	else
	{
		pop = (*m_pdrspqexprOuterKeys)[ulKey]->Pop();
	}
	EOperatorId op_id = pop->Eopid();

	if (COperator::EopScalarIdent == op_id)
	{
		const CColRef *colref = CScalarIdent::PopConvert(pop)->Pcr();
		return (!pcrsNotNull->FMember(colref));
	}

	if (COperator::EopScalarConst == op_id)
	{
		return CScalarConst::PopConvert(pop)->GetDatum()->IsNull();
	}

	// be conservative for all other scalar expressions where we cannot easily
	// determine nullability
	return true;
}

void
CPhysicalHashJoin::CreateOptRequests(CMemoryPool *mp)
{
	CreateHashRedistributeRequests(mp);

	// given an optimization context, HJN creates three optimization requests
	// to enforce distribution of its children:
	// Req(1 to N) (redistribute, redistribute), where we request the first hash join child
	//		to be distributed on single hash join keys separately, as well as the set
	//		of all hash join keys,
	//		the second hash join child is always required to match the distribution returned
	//		by first child
	// Req(N + 1) (hashed, broadcast)
	// Req(N + 2) (non-singleton, broadcast)
	// Req(N + 3) (singleton, singleton)

	ULONG ulDistrReqs =
		SPQOPT_NON_HASH_DIST_REQUESTS + m_pdrspqdsRedistributeRequests->Size();
	SetDistrRequests(ulDistrReqs);

	// With DP enabled, there are several (max 10 controlled by macro)
	// alternatives generated for a join tree and during optimization of those
	// alternatives expressions PS is inserted in almost all the groups possibly.
	// However, if DP is turned off, i.e in query or greedy join order,
	// PS must be inserted in the group with DTS else in some cases HJ plan
	// cannot be created. So, to ensure pushing PS without DPE 2 partition
	// propagation request are required if DP is disabled.
	//    Req 0 => Push PS with considering DPE possibility
	//    Req 1 => Push PS without considering DPE possibility
	// Ex case: select * from non_part_tbl1 t1, part_tbl t2, non_part_tbl2 t3
	// where t1.b = t2.b and t2.b = t3.b;
	// Note: b is the partitioned column for part_tbl. If DP is turned off, HJ
	// will not be created for the above query if we send only 1 request.
	// Also, increasing the number of request increases the optimization time, so
	// set 2 only when needed.
	//
	// There are also cases where greedy does generate a better plan
	// without DPE. This adds some overhead (<10%)to optimization time in
	// some cases, but can create better alternatives to DPE, so
	// we also generate this additional request for expressions that originated
	// from CXformExpandNAryJoinGreedy.
	CPhysicalJoin *physical_join = dynamic_cast<CPhysicalJoin *>(this);
	if ((SPQOPT_FDISABLED_XFORM(CXform::ExfExpandNAryJoinDP) &&
		 SPQOPT_FDISABLED_XFORM(CXform::ExfExpandNAryJoinDPv2)) ||
		physical_join->OriginXform() == CXform::ExfExpandNAryJoinGreedy)
	{
		SetPartPropagateRequests(2);
	}
	else
	{
		SetPartPropagateRequests(1);
	}
}
// EOF
