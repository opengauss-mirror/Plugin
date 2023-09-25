//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		CPhysicalInnerNLJoin.cpp
//
//	@doc:
//		Implementation of inner nested-loops join operator
//---------------------------------------------------------------------------

#include "spqopt/operators/CPhysicalInnerNLJoin.h"

#include "spqos/base.h"

#include "spqopt/base/CCastUtils.h"
#include "spqopt/base/CDistributionSpecHashed.h"
#include "spqopt/base/CDistributionSpecNonSingleton.h"
#include "spqopt/base/CDistributionSpecReplicated.h"
#include "spqopt/base/CUtils.h"
#include "spqopt/operators/CExpressionHandle.h"
#include "spqopt/operators/CPredicateUtils.h"


using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalInnerNLJoin::CPhysicalInnerNLJoin
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CPhysicalInnerNLJoin::CPhysicalInnerNLJoin(CMemoryPool *mp)
	: CPhysicalNLJoin(mp)
{
	// Inner NLJ creates two distribution requests for children:
	// (0) Outer child is requested for ANY distribution, and inner child is requested for a Replicated (or a matching) distribution
	// (1) Outer child is requested for Replicated distribution, and inner child is requested for Non-Singleton

	SetDistrRequests(2);
}


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalInnerNLJoin::~CPhysicalInnerNLJoin
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CPhysicalInnerNLJoin::~CPhysicalInnerNLJoin()
{
}



//---------------------------------------------------------------------------
//	@function:
//		CPhysicalInnerNLJoin::PdsRequired
//
//	@doc:
//		Compute required distribution of the n-th child;
//		this function creates two distribution requests:
//
//		(0) Outer child is requested for ANY distribution, and inner child is
//		  requested for a Replicated (or a matching) distribution,
//		  this request is created by calling CPhysicalJoin::PdsRequired()
//
//		(1) Outer child is requested for Replicated distribution, and inner child
//		  is requested for Non-Singleton (or Singleton if outer delivered Universal distribution)
//
//---------------------------------------------------------------------------
CDistributionSpec *
CPhysicalInnerNLJoin::PdsRequired(CMemoryPool * /*mp*/,
								  CExpressionHandle & /*exprhdl*/,
								  CDistributionSpec *,	//pdsRequired,
								  ULONG /*child_index*/,
								  CDrvdPropArray * /*pdrspqdpCtxt*/,
								  ULONG	 // ulOptReq
) const
{
	SPQOS_RAISE(
		CException::ExmaInvalid, CException::ExmiInvalid,
		SPQOS_WSZ_LIT(
			"PdsRequired should not be called for CPhysicalInnerNLJoin"));
	return NULL;
}

CEnfdDistribution *
CPhysicalInnerNLJoin::Ped(CMemoryPool *mp, CExpressionHandle &exprhdl,
						  CReqdPropPlan *prppInput, ULONG child_index,
						  CDrvdPropArray *pdrspqdpCtxt, ULONG ulOptReq)
{
	SPQOS_ASSERT(2 > child_index);
	SPQOS_ASSERT(ulOptReq < UlDistrRequests());

	CEnfdDistribution::EDistributionMatching dmatch =
		Edm(prppInput, child_index, pdrspqdpCtxt, ulOptReq);
	CDistributionSpec *const pdsRequired = prppInput->Ped()->PdsRequired();

	// if expression has to execute on a single host then we need a gather
	if (exprhdl.NeedsSingletonExecution())
	{
		return SPQOS_NEW(mp) CEnfdDistribution(
			PdsRequireSingleton(mp, exprhdl, pdsRequired, child_index), dmatch);
	}

	if (exprhdl.HasOuterRefs())
	{
		if (CDistributionSpec::EdtSingleton == pdsRequired->Edt() ||
			CDistributionSpec::EdtStrictReplicated == pdsRequired->Edt())
		{
			return SPQOS_NEW(mp) CEnfdDistribution(
				PdsPassThru(mp, exprhdl, pdsRequired, child_index), dmatch);
		}
		return SPQOS_NEW(mp) CEnfdDistribution(
			SPQOS_NEW(mp)
				CDistributionSpecReplicated(CDistributionSpec::EdtReplicated),
			CEnfdDistribution::EdmSatisfy);
	}

	if (SPQOS_FTRACE(EopttraceDisableReplicateInnerNLJOuterChild) ||
		0 == ulOptReq)
	{
		if (1 == child_index)
		{
			CEnfdDistribution *pEnfdHashedDistribution =
				CPhysicalJoin::PedInnerHashedFromOuterHashed(
					mp, exprhdl, dmatch, (*pdrspqdpCtxt)[0]);
			if (pEnfdHashedDistribution)
				return pEnfdHashedDistribution;
		}
		return CPhysicalJoin::Ped(mp, exprhdl, prppInput, child_index,
								  pdrspqdpCtxt, ulOptReq);
	}
	SPQOS_ASSERT(1 == ulOptReq);

	if (0 == child_index)
	{
		return SPQOS_NEW(mp) CEnfdDistribution(
			SPQOS_NEW(mp)
				CDistributionSpecReplicated(CDistributionSpec::EdtReplicated),
			dmatch);
	}

	// compute a matching distribution based on derived distribution of outer child
	CDistributionSpec *pdsOuter =
		CDrvdPropPlan::Pdpplan((*pdrspqdpCtxt)[0])->Pds();
	if (CDistributionSpec::EdtUniversal == pdsOuter->Edt())
	{
		// first child is universal, request second child to execute on a single host to avoid duplicates
		return SPQOS_NEW(mp) CEnfdDistribution(
			SPQOS_NEW(mp) CDistributionSpecSingleton(), dmatch);
	}

	return SPQOS_NEW(mp)
		CEnfdDistribution(SPQOS_NEW(mp) CDistributionSpecNonSingleton(), dmatch);
}

// EOF
