//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CPhysicalSerialUnionAll.cpp
//
//	@doc:
//		Implementation of physical union all operator
//
//	@owner:
//
//
//	@test:
//
//---------------------------------------------------------------------------

#include "spqopt/operators/CPhysicalSerialUnionAll.h"

#include "spqos/base.h"

#include "spqopt/base/CDistributionSpecAny.h"
#include "spqopt/base/CDistributionSpecHashed.h"
#include "spqopt/base/CDistributionSpecNonSingleton.h"
#include "spqopt/base/CDistributionSpecRandom.h"
#include "spqopt/base/CDistributionSpecReplicated.h"
#include "spqopt/base/CDistributionSpecSingleton.h"
#include "spqopt/base/CDrvdPropCtxtPlan.h"
#include "spqopt/exception.h"
#include "spqopt/operators/CExpressionHandle.h"
#include "spqopt/operators/CScalarIdent.h"

using namespace spqopt;

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalSerialUnionAll::CPhysicalSerialUnionAll
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CPhysicalSerialUnionAll::CPhysicalSerialUnionAll(
	CMemoryPool *mp, CColRefArray *pdrspqcrOutput,
	CColRef2dArray *pdrspqdrspqcrInput, ULONG ulScanIdPartialIndex)
	: CPhysicalUnionAll(mp, pdrspqcrOutput, pdrspqdrspqcrInput,
						ulScanIdPartialIndex)
{
	// UnionAll creates two distribution requests to enforce distribution of its children:
	// (1) (Hashed, Hashed): used to pass hashed distribution (requested from above)
	//     to child operators and match request Exactly
	// (2) (ANY, matching_distr): used to request ANY distribution from outer child, and
	//     match its response on the distribution requested from inner child

	SetDistrRequests(2 /*ulDistrReq*/);
	SPQOS_ASSERT(0 < UlDistrRequests());
}

CPhysicalSerialUnionAll::~CPhysicalSerialUnionAll()
{
}


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalSerialUnionAll::PdsRequired
//
//	@doc:
//		Compute required distribution of the n-th child
//
//---------------------------------------------------------------------------
CDistributionSpec *
CPhysicalSerialUnionAll::PdsRequired(
	CMemoryPool *mp, CExpressionHandle &exprhdl, CDistributionSpec *pdsRequired,
	ULONG child_index, CDrvdPropArray *pdrspqdpCtxt, ULONG ulOptReq) const
{
	SPQOS_ASSERT(NULL != PdrspqdrspqcrInput());
	SPQOS_ASSERT(child_index < PdrspqdrspqcrInput()->Size());
	SPQOS_ASSERT(2 > ulOptReq);

	CDistributionSpec *pds = PdsRequireSingletonOrReplicated(
		mp, exprhdl, pdsRequired, child_index, ulOptReq);
	if (NULL != pds)
	{
		return pds;
	}

	if (0 == ulOptReq && CDistributionSpec::EdtHashed == pdsRequired->Edt())
	{
		// attempt passing requested hashed distribution to children
		CDistributionSpecHashed *pdshashed = PdshashedPassThru(
			mp, CDistributionSpecHashed::PdsConvert(pdsRequired), child_index);
		if (NULL != pdshashed)
		{
			return pdshashed;
		}
	}

	if (0 == child_index)
	{
		// otherwise, ANY distribution is requested from outer child
		return SPQOS_NEW(mp) CDistributionSpecAny(this->Eopid());
	}

	// inspect distribution delivered by outer child
	CDistributionSpec *pdsOuter =
		CDrvdPropPlan::Pdpplan((*pdrspqdpCtxt)[0])->Pds();

	if (CDistributionSpec::EdtSingleton == pdsOuter->Edt() ||
		CDistributionSpec::EdtStrictSingleton == pdsOuter->Edt())
	{
		// outer child is Singleton, require inner child to have matching Singleton distribution
		return CPhysical::PdssMatching(
			mp, CDistributionSpecSingleton::PdssConvert(pdsOuter));
	}

	if (CDistributionSpec::EdtUniversal == pdsOuter->Edt())
	{
		// require inner child to be on a single host in order to avoid
		// duplicate values when doing UnionAll operation with Universal outer child
		// Example: select 1 union all select i from x;
		return SPQOS_NEW(mp) CDistributionSpecSingleton();
	}

	if (CDistributionSpec::EdtStrictReplicated == pdsOuter->Edt() ||
		CDistributionSpec::EdtTaintedReplicated == pdsOuter->Edt())
	{
		// outer child is replicated, require inner child to be replicated
		return SPQOS_NEW(mp)
			CDistributionSpecReplicated(CDistributionSpec::EdtReplicated);
	}

	// outer child is non-replicated and is distributed across segments,
	// we need to the inner child to be distributed across segments that does
	// not generate duplicate results. That is, inner child should not be replicated.

	return SPQOS_NEW(mp) CDistributionSpecNonSingleton(
		false /*fAllowReplicated*/, true /*fAllowEnforced*/);
}

// EOF
