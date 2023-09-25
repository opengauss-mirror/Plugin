//	Greenplum Database
//	Copyright (C) 2019 Pivotal Software, Inc.

#ifndef SPQOPT_CPhysicalFullMergeJoin_H
#define SPQOPT_CPhysicalFullMergeJoin_H

#include "spqos/base.h"

#include "spqopt/operators/CPhysicalJoin.h"

namespace spqopt
{
class CPhysicalFullMergeJoin : public CPhysicalJoin
{
private:
	// private copy ctor
	CPhysicalFullMergeJoin(const CPhysicalFullMergeJoin &);

	CExpressionArray *m_outer_merge_clauses;

	CExpressionArray *m_inner_merge_clauses;

public:
	// ctor
	explicit CPhysicalFullMergeJoin(
		CMemoryPool *mp, CExpressionArray *outer_merge_clauses,
		CExpressionArray *inner_merge_clauses,
		IMdIdArray *hash_opfamilies = NULL, BOOL is_null_aware = true,
		CXform::EXformId order_origin_xform = CXform::ExfSentinel);

	// dtor
	virtual ~CPhysicalFullMergeJoin();

	// ident accessors
	virtual EOperatorId
	Eopid() const
	{
		return EopPhysicalFullMergeJoin;
	}

	// return a string for operator name
	virtual const CHAR *
	SzId() const
	{
		return "CPhysicalFullMergeJoin";
	}

	// conversion function
	static CPhysicalFullMergeJoin *
	PopConvert(COperator *pop)
	{
		SPQOS_ASSERT(EopPhysicalFullMergeJoin == pop->Eopid());

		return dynamic_cast<CPhysicalFullMergeJoin *>(pop);
	}

	virtual CDistributionSpec *PdsRequired(CMemoryPool *mp,
										   CExpressionHandle &exprhdl,
										   CDistributionSpec *pdsRequired,
										   ULONG child_index,
										   CDrvdPropArray *pdrspqdpCtxt,
										   ULONG ulOptReq) const;

	virtual CEnfdDistribution *Ped(CMemoryPool *mp, CExpressionHandle &exprhdl,
								   CReqdPropPlan *prppInput, ULONG child_index,
								   CDrvdPropArray *pdrspqdpCtxt,
								   ULONG ulDistrReq);

	virtual COrderSpec *PosRequired(CMemoryPool *mp, CExpressionHandle &exprhdl,
									COrderSpec *posInput, ULONG child_index,
									CDrvdPropArray *pdrspqdpCtxt,
									ULONG ulOptReq) const;

	// compute required rewindability of the n-th child
	virtual CRewindabilitySpec *PrsRequired(CMemoryPool *mp,
											CExpressionHandle &exprhdl,
											CRewindabilitySpec *prsRequired,
											ULONG child_index,
											CDrvdPropArray *pdrspqdpCtxt,
											ULONG ulOptReq) const;

	// return order property enforcing type for this operator
	virtual CEnfdProp::EPropEnforcingType EpetOrder(
		CExpressionHandle &exprhdl, const CEnfdOrder *peo) const;

	virtual CEnfdDistribution::EDistributionMatching Edm(
		CReqdPropPlan *,   // prppInput
		ULONG,			   //child_index,
		CDrvdPropArray *,  // pdrspqdpCtxt,
		ULONG			   // ulOptReq
	);

	virtual CDistributionSpec *PdsDerive(CMemoryPool *mp,
										 CExpressionHandle &exprhdl) const;

};	// class CPhysicalFullMergeJoin

}  // namespace spqopt

#endif	// !SPQOPT_CPhysicalFullMergeJoin_H

// EOF
