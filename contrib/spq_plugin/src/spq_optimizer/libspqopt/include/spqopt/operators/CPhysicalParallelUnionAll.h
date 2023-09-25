//	Greenplum Database
//	Copyright (C) 2016 Pivotal Software, Inc.

#ifndef SPQOPT_CPhysicalParallelUnionAll_H
#define SPQOPT_CPhysicalParallelUnionAll_H

#include "spqopt/operators/CPhysicalUnionAll.h"

namespace spqopt
{
// Operator that implements logical union all, but creates a slice for each
// child relation to maximize concurrency.
// See spqopt::CPhysicalSerialUnionAll for its serial sibling.
class CPhysicalParallelUnionAll : public CPhysicalUnionAll
{
private:
	// array of child hashed distributions -- used locally for distribution derivation
	CDistributionSpecArray *const m_pdrspqds;

public:
	CPhysicalParallelUnionAll(CMemoryPool *mp, CColRefArray *pdrspqcrOutput,
							  CColRef2dArray *pdrspqdrspqcrInput,
							  ULONG ulScanIdPartialIndex);

	virtual EOperatorId Eopid() const;

	virtual const CHAR *SzId() const;

	virtual CDistributionSpec *PdsRequired(CMemoryPool *mp,
										   CExpressionHandle &exprhdl,
										   CDistributionSpec *pdsRequired,
										   ULONG child_index,
										   CDrvdPropArray *pdrspqdpCtxt,
										   ULONG ulOptReq) const;

	virtual CEnfdDistribution::EDistributionMatching Edm(
		CReqdPropPlan *,   // prppInput
		ULONG,			   // child_index
		CDrvdPropArray *,  //pdrspqdpCtxt
		ULONG			   // ulOptReq
	);

	virtual ~CPhysicalParallelUnionAll();
};
}  // namespace spqopt

#endif	//SPQOPT_CPhysicalParallelUnionAll_H
