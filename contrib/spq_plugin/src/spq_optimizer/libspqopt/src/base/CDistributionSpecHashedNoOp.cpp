//	Greenplum Database
//	Copyright (C) 2016 Pivotal Software, Inc.

#include "spqopt/base/CDistributionSpecHashedNoOp.h"

#include "spqopt/operators/CExpressionHandle.h"
#include "spqopt/operators/CPhysicalMotionHashDistribute.h"

using namespace spqopt;

CDistributionSpecHashedNoOp::CDistributionSpecHashedNoOp(
	CExpressionArray *pdrspqexpr)
	: CDistributionSpecHashed(pdrspqexpr, true)
{
}

CDistributionSpec::EDistributionType
CDistributionSpecHashedNoOp::Edt() const
{
	return CDistributionSpec::EdtHashedNoOp;
}

BOOL
CDistributionSpecHashedNoOp::Matches(const CDistributionSpec *pds) const
{
	return pds->Edt() == Edt();
}

void
CDistributionSpecHashedNoOp::AppendEnforcers(CMemoryPool *mp,
											 CExpressionHandle &exprhdl,
											 CReqdPropPlan *,
											 CExpressionArray *pdrspqexpr,
											 CExpression *pexpr)
{
	CDrvdProp *pdp = exprhdl.Pdp();
	CDistributionSpec *pdsChild = CDrvdPropPlan::Pdpplan(pdp)->Pds();
	CDistributionSpecHashed *pdsChildHashed =
		dynamic_cast<CDistributionSpecHashed *>(pdsChild);

	if (NULL == pdsChildHashed)
	{
		return;
	}

	CExpressionArray *pdrspqexprNoOpRedistributionColumns =
		pdsChildHashed->Pdrspqexpr();
	pdrspqexprNoOpRedistributionColumns->AddRef();
	CDistributionSpecHashedNoOp *pdsNoOp = SPQOS_NEW(mp)
		CDistributionSpecHashedNoOp(pdrspqexprNoOpRedistributionColumns);
	pexpr->AddRef();
	CExpression *pexprMotion = SPQOS_NEW(mp) CExpression(
		mp, SPQOS_NEW(mp) CPhysicalMotionHashDistribute(mp, pdsNoOp), pexpr);
	pdrspqexpr->Append(pexprMotion);
}
