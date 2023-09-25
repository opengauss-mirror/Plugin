//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 Greenplum, Inc.
//
//	@filename:
//		CPhysicalInnerIndexNLJoin.cpp
//
//	@doc:
//		Implementation of index inner nested-loops join operator
//---------------------------------------------------------------------------

#include "spqopt/operators/CPhysicalInnerIndexNLJoin.h"

#include "spqos/base.h"

#include "spqopt/base/CDistributionSpecAny.h"
#include "spqopt/base/CDistributionSpecHashed.h"
#include "spqopt/base/CDistributionSpecNonSingleton.h"
#include "spqopt/base/CDistributionSpecReplicated.h"
#include "spqopt/operators/CExpressionHandle.h"
#include "spqopt/operators/CPredicateUtils.h"



using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalInnerIndexNLJoin::CPhysicalInnerIndexNLJoin
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CPhysicalInnerIndexNLJoin::CPhysicalInnerIndexNLJoin(CMemoryPool *mp,
													 CColRefArray *colref_array,
													 CExpression *origJoinPred)
	: CPhysicalInnerNLJoin(mp),
	  m_pdrspqcrOuterRefs(colref_array),
	  m_origJoinPred(origJoinPred)
{
	SPQOS_ASSERT(NULL != colref_array);
	if (NULL != origJoinPred)
	{
		origJoinPred->AddRef();
	}
}


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalInnerIndexNLJoin::~CPhysicalInnerIndexNLJoin
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CPhysicalInnerIndexNLJoin::~CPhysicalInnerIndexNLJoin()
{
	m_pdrspqcrOuterRefs->Release();
	CRefCount::SafeRelease(m_origJoinPred);
}


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalInnerIndexNLJoin::Matches
//
//	@doc:
//		Match function
//
//---------------------------------------------------------------------------
BOOL
CPhysicalInnerIndexNLJoin::Matches(COperator *pop) const
{
	if (pop->Eopid() == Eopid())
	{
		return m_pdrspqcrOuterRefs->Equals(
			CPhysicalInnerIndexNLJoin::PopConvert(pop)->PdrgPcrOuterRefs());
	}

	return false;
}


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalInnerIndexNLJoin::PdsRequired
//
//	@doc:
//		Compute required distribution of the n-th child;
//
//---------------------------------------------------------------------------
CDistributionSpec *
CPhysicalInnerIndexNLJoin::PdsRequired(CMemoryPool * /*mp*/,
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
			"PdsRequired should not be called for CPhysicalInnerIndexNLJoin"));
	return NULL;
}

CEnfdDistribution *
CPhysicalInnerIndexNLJoin::Ped(CMemoryPool *mp, CExpressionHandle &exprhdl,
							   CReqdPropPlan *prppInput, ULONG child_index,
							   CDrvdPropArray *pdrspqdpCtxt, ULONG ulDistrReq)
{
	SPQOS_ASSERT(2 > child_index);

	CEnfdDistribution::EDistributionMatching dmatch =
		Edm(prppInput, child_index, pdrspqdpCtxt, ulDistrReq);

	if (1 == child_index)
	{
		// inner (index-scan side) is requested for Any distribution,
		// we allow outer references on the inner child of the join since it needs
		// to refer to columns in join's outer child
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
	}

	// otherwise, require outer child to be replicated
	// The match type for this request has to be "Satisfy" since EdtReplicated
	// is required only property. Since a Broadcast motion will always
	// derive a EdtStrictReplicated distribution spec, it will never "Match"
	// the required distribution spec and hence will not be optimized.
	return SPQOS_NEW(mp) CEnfdDistribution(
		SPQOS_NEW(mp)
			CDistributionSpecReplicated(CDistributionSpec::EdtReplicated),
		CEnfdDistribution::EdmSatisfy);
}


// EOF
