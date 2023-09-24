//	Greenplum Database
//	Copyright (C) 2016 Pivotal Software, Inc.

#ifndef SPQOPT_CDistributionSpecStrictHashed_H
#define SPQOPT_CDistributionSpecStrictHashed_H

#include "spqopt/base/CDistributionSpecHashed.h"

namespace spqopt
{
// Class to represent a "forced" hashed distribution. Introduced to support
// parallel append, this distribution is meant to be incompatible with
// (or unsatisfiable by) most other distributions to force a motion.
class CDistributionSpecStrictHashed : public CDistributionSpecHashed
{
public:
	CDistributionSpecStrictHashed(CExpressionArray *pdrspqexpr,
								  BOOL fNullsColocated);

	virtual EDistributionType Edt() const;

	virtual const CHAR *
	SzId() const
	{
		return "STRICT HASHED";
	}
};
}  // namespace spqopt

#endif
