//	Greenplum Database
//	Copyright (C) 2016 Pivotal Software, Inc.

#include "spqopt/base/CDistributionSpecStrictHashed.h"

namespace spqopt
{
CDistributionSpecStrictHashed::CDistributionSpecStrictHashed(
	CExpressionArray *pdrspqexpr, BOOL fNullsColocated)
	: CDistributionSpecHashed(pdrspqexpr, fNullsColocated)
{
}

CDistributionSpec::EDistributionType
CDistributionSpecStrictHashed::Edt() const
{
	return CDistributionSpec::EdtStrictHashed;
}

}  // namespace spqopt
