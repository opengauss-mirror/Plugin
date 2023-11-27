//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2017 Greenplum, Inc.
//
//	Implementation of left outer index nested-loops join operator
//---------------------------------------------------------------------------

#include "spqopt/operators/CPhysicalLeftOuterIndexNLJoin.h"

#include "spqos/base.h"

#include "spqopt/base/CDistributionSpecAny.h"
#include "spqopt/base/CDistributionSpecHashed.h"
#include "spqopt/base/CDistributionSpecNonSingleton.h"
#include "spqopt/base/CDistributionSpecReplicated.h"
#include "spqopt/exception.h"
#include "spqopt/operators/CExpressionHandle.h"
#include "spqopt/operators/CPredicateUtils.h"

using namespace spqopt;

CPhysicalLeftOuterIndexNLJoin::CPhysicalLeftOuterIndexNLJoin(
	CMemoryPool *mp, CColRefArray *colref_array, CExpression *origJoinPred)
	: CPhysicalLeftOuterNLJoin(mp),
	  m_pdrspqcrOuterRefs(colref_array),
	  m_origJoinPred(origJoinPred)
{
	SPQOS_ASSERT(NULL != colref_array);
	if (NULL != origJoinPred)
	{
		origJoinPred->AddRef();
	}
}


CPhysicalLeftOuterIndexNLJoin::~CPhysicalLeftOuterIndexNLJoin()
{
	m_pdrspqcrOuterRefs->Release();
	CRefCount::SafeRelease(m_origJoinPred);
}


BOOL
CPhysicalLeftOuterIndexNLJoin::Matches(COperator *pop) const
{
	if (pop->Eopid() == Eopid())
	{
		return m_pdrspqcrOuterRefs->Equals(
			CPhysicalLeftOuterIndexNLJoin::PopConvert(pop)->PdrgPcrOuterRefs());
	}

	return false;
}

CEnfdProp::EPropEnforcingType
CPhysicalLeftOuterIndexNLJoin::EpetDistribution(
	CExpressionHandle &exprhdl, const CEnfdDistribution *ped) const
{
	SPQOS_ASSERT(NULL != ped);

	// outer index nested loop join cannot have the inner child be random
	if (exprhdl.Pdpplan(1)->Pds()->Edt() == CDistributionSpec::EdtRandom)
	{
		return CEnfdProp::EpetProhibited;
	}

	// get distribution delivered by the physical node
	CDistributionSpec *pds = CDrvdPropPlan::Pdpplan(exprhdl.Pdp())->Pds();
	if (ped->FCompatible(pds))
	{
		// required distribution is already provided
		return CEnfdProp::EpetUnnecessary;
	}

	return CEnfdProp::EpetRequired;
}

CDistributionSpec *
CPhysicalLeftOuterIndexNLJoin::PdsRequired(CMemoryPool * /*mp*/,
										   CExpressionHandle & /*exprhdl*/,
										   CDistributionSpec *,	 //pdsRequired,
										   ULONG /*child_index*/,
										   CDrvdPropArray * /*pdrspqdpCtxt*/,
										   ULONG  // ulOptReq
) const
{
	SPQOS_RAISE(
		CException::ExmaInvalid, CException::ExmiInvalid,
		SPQOS_WSZ_LIT(
			"PdsRequired should not be called for CPhysicalLeftOuterIndexNLJoin"));
	return NULL;
}

CEnfdDistribution *
CPhysicalLeftOuterIndexNLJoin::Ped(CMemoryPool *mp, CExpressionHandle &exprhdl,
								   CReqdPropPlan *prppInput, ULONG child_index,
								   CDrvdPropArray *pdrspqdpCtxt, ULONG ulOptReq)
{
	SPQOS_ASSERT(2 > child_index);

	CEnfdDistribution::EDistributionMatching dmatch =
		Edm(prppInput, child_index, pdrspqdpCtxt, ulOptReq);


	if (SPQOS_FTRACE(EopttraceEnableLeftIndexNLJoin))
	{
		if (1 == child_index)
			return SPQOS_NEW(mp) CEnfdDistribution(SPQOS_NEW(mp)
				CDistributionSpecReplicated(CDistributionSpec::EdtStrictReplicated), dmatch);
		else
			return SPQOS_NEW(mp) CEnfdDistribution(SPQOS_NEW(mp)
				CDistributionSpecAny(this->Eopid(), true /*fAllowOuterRefs*/), dmatch);
	}

	if (1 == child_index)
	{
		// inner (index-scan side) is requested for Any distribution,
		// we allow outer references on the inner child of the join since it needs
		// to refer to columns in join's outer child
		//
		// this distribution is intentionally invalid so we can reject invalid
		// plans in EpetDistribution. there is a special case in CEnfdDistribution
		// where fDistributionReqd is false if the distribution spec is any *or* the
		// Eopid is CPhysicalLeftOuterIndexNLJoin (this case)
		return SPQOS_NEW(mp) CEnfdDistribution(
			SPQOS_NEW(mp)
				CDistributionSpecAny(this->Eopid(), true /*fAllowOuterRefs*/),
			dmatch);
	}

	// we need to match distribution of inner
	CDistributionSpec *pdsInner =
		CDrvdPropPlan::Pdpplan((*pdrspqdpCtxt)[0])->Pds();
	CDistributionSpec::EDistributionType edtInner = pdsInner->Edt();
	if (CDistributionSpec::EdtSingleton == edtInner ||
		CDistributionSpec::EdtStrictSingleton == edtInner ||
		CDistributionSpec::EdtUniversal == edtInner)
	{
		// enforce executing on a single host
		return SPQOS_NEW(mp) CEnfdDistribution(
			SPQOS_NEW(mp) CDistributionSpecSingleton(), dmatch);
	}

	if (CDistributionSpec::EdtHashed == edtInner)
	{
		// check if we could create an equivalent hashed distribution request to the inner child
		CDistributionSpecHashed *pdshashed =
			CDistributionSpecHashed::PdsConvert(pdsInner);
		CDistributionSpecHashed *pdshashedEquiv = pdshashed->PdshashedEquiv();

		// If the inner child is a IndexScan on a multi-key distributed index, it
		// may derive an incomplete equiv spec (see CPhysicalScan::PdsDerive()).
		// However, there is no point to using that here since there will be no
		// operator above this that can complete it.
		//
		// NB: Technically for Outer joins, the entire distribution key of the
		// table must be present in the join clause to produce the index scan
		// alternative in the first place (see CXformJoin2IndexApplyBase).
		// Therefore, when an incomplete spec is created in the inner subtree for
		// such a table, there will also be a CPhysicalFilter (that has the
		// remaining predicates) on top to complete the spec. Thus, at this point
		// in the code, pdshashedEquiv should be complete. However, just in case
		// that precondition is not met, it is safer to to check for completeness
		// properly anyway.
		if (pdshashed->HasCompleteEquivSpec(mp))
		{
			// request hashed distribution from outer
			pdshashedEquiv->Pdrspqexpr()->AddRef();
			CDistributionSpecHashed *pdsHashedRequired = SPQOS_NEW(mp)
				CDistributionSpecHashed(pdshashedEquiv->Pdrspqexpr(),
										pdshashedEquiv->FNullsColocated());
			pdsHashedRequired->ComputeEquivHashExprs(mp, exprhdl);

			return SPQOS_NEW(mp) CEnfdDistribution(pdsHashedRequired, dmatch);
		}

		// if the equivalent spec cannot be used, request the original - even
		// though this spec will fail to produce a plan during property
		// enforcement, it is still better than falling back to planner, since
		// there may be other alternatives that will succeed.
		pdshashed->AddRef();
		return SPQOS_NEW(mp) CEnfdDistribution(pdshashed, dmatch);
	}

	// otherwise, require outer child to be replicated
	// this will end up generating an invalid plan, but we reject it in EpetDistribution
	return SPQOS_NEW(mp) CEnfdDistribution(
		SPQOS_NEW(mp)
			CDistributionSpecReplicated(CDistributionSpec::EdtReplicated),
		CEnfdDistribution::EdmSatisfy);
}


// EOF
