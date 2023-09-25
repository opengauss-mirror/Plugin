//	Greenplum Database
//	Copyright (C) 2016 Pivotal Software, Inc.


#ifndef SPQOPT_CDistributionSpecHashedNoOp_H
#define SPQOPT_CDistributionSpecHashedNoOp_H

#include "spqopt/base/CDistributionSpecHashed.h"

namespace spqopt
{
class CDistributionSpecHashedNoOp : public CDistributionSpecHashed
{
public:
	CDistributionSpecHashedNoOp(CExpressionArray *pdrspqexr);

	virtual EDistributionType Edt() const;

	virtual BOOL Matches(const CDistributionSpec *pds) const;

	virtual const CHAR *
	SzId() const
	{
		return "HASHED NO-OP";
	}

	virtual void AppendEnforcers(CMemoryPool *mp, CExpressionHandle &exprhdl,
								 CReqdPropPlan *prpp,
								 CExpressionArray *pdrspqexpr,
								 CExpression *pexpr);
};
}  // namespace spqopt

#endif
