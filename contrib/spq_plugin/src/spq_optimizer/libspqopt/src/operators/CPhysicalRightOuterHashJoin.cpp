//---------------------------------------------------------------------------
//	Greenplum Database
//  Copyright (c) 2020 VMware, Inc.
//
//	@filename:
//		CPhysicalRightOuterHashJoin.cpp
//
//	@doc:
//		Implementation of right outer hash join operator
//---------------------------------------------------------------------------

#include "spqopt/operators/CPhysicalRightOuterHashJoin.h"

#include "spqos/base.h"

#include "spqopt/base/CDistributionSpecNonSingleton.h"
#include "spqopt/base/CDistributionSpecReplicated.h"
#include "spqopt/operators/CExpressionHandle.h"


using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalRightOuterHashJoin::CPhysicalRightOuterHashJoin
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CPhysicalRightOuterHashJoin::CPhysicalRightOuterHashJoin(
	CMemoryPool *mp, CExpressionArray *pdrspqexprOuterKeys,
	CExpressionArray *pdrspqexprInnerKeys, IMdIdArray *hash_opfamilies,
	BOOL is_null_aware, CXform::EXformId origin_xform)
	: CPhysicalHashJoin(mp, pdrspqexprOuterKeys, pdrspqexprInnerKeys,
						hash_opfamilies, is_null_aware, origin_xform)
{
	ULONG ulDistrReqs = 1 + NumDistrReq();
	SetDistrRequests(ulDistrReqs);
	SetPartPropagateRequests(2);
}


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalRightOuterHashJoin::~CPhysicalRightOuterHashJoin
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CPhysicalRightOuterHashJoin::~CPhysicalRightOuterHashJoin()
{
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalRightOuterHashJoin::PppsRequired
//
//	@doc:
//		Compute required partition propagation of the n-th child
//
//---------------------------------------------------------------------------
CPartitionPropagationSpec *
CPhysicalRightOuterHashJoin::PppsRequired(
	CMemoryPool *mp, CExpressionHandle &exprhdl,
	CPartitionPropagationSpec *pppsRequired, ULONG child_index,
	CDrvdPropArray *pdrspqdpCtxt, ULONG ulOptReq)
{
	// This is identical to the inner join partition request
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

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalRightOuterHashJoin::PdsRequired
//
//	@doc:
//		Compute required distribution of the n-th child
//
//---------------------------------------------------------------------------
CEnfdDistribution *
CPhysicalRightOuterHashJoin::Ped(CMemoryPool *mp, CExpressionHandle &exprhdl,
								 CReqdPropPlan *prppInput, ULONG child_index,
								 CDrvdPropArray *pdrspqdpCtxt, ULONG ulOptReq)
{
	// create the following requests:
	// 1) hash-hash (provided by CPhysicalHashJoin::Ped)
	// 2) singleton-singleton
	//
	// We also could create a replicated-hashed and replicated-non-singleton request, but that isn't a promising
	// alternative as we would be broadcasting the outer side. In that case, an LOJ would be better.

	CDistributionSpec *const pdsInput = prppInput->Ped()->PdsRequired();
	CEnfdDistribution::EDistributionMatching dmatch =
		Edm(prppInput, child_index, pdrspqdpCtxt, ulOptReq);

	if (exprhdl.NeedsSingletonExecution() || exprhdl.HasOuterRefs())
	{
		return SPQOS_NEW(mp) CEnfdDistribution(
			PdsRequireSingleton(mp, exprhdl, pdsInput, child_index), dmatch);
	}

	const ULONG ulHashDistributeRequests = NumDistrReq();

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
	SPQOS_ASSERT(ulOptReq == NumDistrReq());
	return SPQOS_NEW(mp) CEnfdDistribution(
		PdsRequiredSingleton(mp, exprhdl, pdsInput, child_index, pdrspqdpCtxt),
		dmatch);
}

void
CPhysicalRightOuterHashJoin::CreateOptRequests(CMemoryPool *mp)
{
	CreateHashRedistributeRequests(mp);

	// given an optimization context, Right Hash Join creates 2 optimization requests
	// to enforce distribution of its children:
	// Req(1 to N) (redistribute, redistribute), where we request the first hash join child
	//              to be distributed on single hash join keys separately, as well as the set
	//              of all hash join keys, the second hash join child is always required to
	// 				match the distribution returned by first child
	// Req(N + 1) (singleton, singleton)
	ULONG ulDistrReqs = 1 + NumDistrReq();
	SetDistrRequests(ulDistrReqs);

	SetPartPropagateRequests(2);
}
// EOF
