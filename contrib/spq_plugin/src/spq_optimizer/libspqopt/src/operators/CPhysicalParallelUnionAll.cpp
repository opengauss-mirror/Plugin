//	Greenplum Database
//	Copyright (C) 2016 Pivotal Software, Inc.

#include "spqopt/operators/CPhysicalParallelUnionAll.h"

#include "spqopt/base/CDistributionSpecHashedNoOp.h"
#include "spqopt/base/CDistributionSpecRandom.h"
#include "spqopt/base/CDistributionSpecStrictHashed.h"
#include "spqopt/base/CUtils.h"
#include "spqopt/operators/CExpressionHandle.h"
#include "spqopt/operators/CStrictHashedDistributions.h"

namespace spqopt
{
CPhysicalParallelUnionAll::CPhysicalParallelUnionAll(
	CMemoryPool *mp, CColRefArray *pdrspqcrOutput,
	CColRef2dArray *pdrspqdrspqcrInput, ULONG ulScanIdPartialIndex)
	: CPhysicalUnionAll(mp, pdrspqcrOutput, pdrspqdrspqcrInput,
						ulScanIdPartialIndex),
	  m_pdrspqds(SPQOS_NEW(mp) CStrictHashedDistributions(mp, pdrspqcrOutput,
														pdrspqdrspqcrInput))
{
	// ParallelUnionAll creates two distribution requests to enforce distribution of its children:
	// (1) (StrictHashed, StrictHashed, ...): used to force redistribute motions that mirror the
	//     output columns
	// (2) (HashedNoOp, HashedNoOp, ...): used to force redistribution motions that mirror the
	//     underlying distribution of each relational child

	SetDistrRequests(2);
}

COperator::EOperatorId
CPhysicalParallelUnionAll::Eopid() const
{
	return EopPhysicalParallelUnionAll;
}

const CHAR *
CPhysicalParallelUnionAll::SzId() const
{
	return "CPhysicalParallelUnionAll";
}

CDistributionSpec *
CPhysicalParallelUnionAll::PdsRequired(CMemoryPool *mp, CExpressionHandle &,
									   CDistributionSpec *, ULONG child_index,
									   CDrvdPropArray *, ULONG ulOptReq) const
{
	if (0 == ulOptReq)
	{
		CDistributionSpec *pdsChild = (*m_pdrspqds)[child_index];
		pdsChild->AddRef();
		return pdsChild;
	}
	else
	{
		CColRefArray *pdrspqcrChildInputColumns =
			(*PdrspqdrspqcrInput())[child_index];
		CExpressionArray *pdrspqexprFakeRequestedColumns =
			SPQOS_NEW(mp) CExpressionArray(mp);

		CColRef *pcrFirstColumn = (*pdrspqcrChildInputColumns)[0];
		CExpression *pexprScalarIdent =
			CUtils::PexprScalarIdent(mp, pcrFirstColumn);
		pdrspqexprFakeRequestedColumns->Append(pexprScalarIdent);

		return SPQOS_NEW(mp)
			CDistributionSpecHashedNoOp(pdrspqexprFakeRequestedColumns);
	}
}

CEnfdDistribution::EDistributionMatching
CPhysicalParallelUnionAll::Edm(CReqdPropPlan *,	  // prppInput
							   ULONG,			  // child_index
							   CDrvdPropArray *,  //pdrspqdpCtxt
							   ULONG			  // ulOptReq
)
{
	return CEnfdDistribution::EdmExact;
}

CPhysicalParallelUnionAll::~CPhysicalParallelUnionAll()
{
	m_pdrspqds->Release();
}
}  // namespace spqopt
