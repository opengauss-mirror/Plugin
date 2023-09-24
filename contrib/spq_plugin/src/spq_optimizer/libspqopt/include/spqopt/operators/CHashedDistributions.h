//	Greenplum Database
//	Copyright (C) 2016 Pivotal Software, Inc.

#ifndef SPQOPT_CHashedDistributions_H
#define SPQOPT_CHashedDistributions_H

#include "spqos/memory/CMemoryPool.h"

#include "spqopt/base/CColRef.h"
#include "spqopt/base/CDistributionSpec.h"
#include "spqopt/base/CDistributionSpecHashed.h"

namespace spqopt
{
// Build hashed distributions used in physical union all during
// distribution derivation. The class is an array of hashed
// distribution on input column of each child, and an output hashed
// distribution on UnionAll output columns

class CHashedDistributions : public CDistributionSpecArray
{
public:
	CHashedDistributions(CMemoryPool *mp, CColRefArray *pdrspqcrOutput,
						 CColRef2dArray *pdrspqdrspqcrInput);
};
}  // namespace spqopt

#endif	//SPQOPT_CHashedDistributions_H
