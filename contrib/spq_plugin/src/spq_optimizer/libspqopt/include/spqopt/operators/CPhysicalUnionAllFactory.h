//	Greenplum Database
//	Copyright (C) 2016 Pivotal Software, Inc.
#ifndef SPQOPT_CPhysicalUnionAllFactory_H
#define SPQOPT_CPhysicalUnionAllFactory_H

#include "spqos/types.h"

#include "spqopt/operators/CLogicalUnionAll.h"
#include "spqopt/operators/CPhysicalUnionAll.h"

namespace spqopt
{
// Constructs a spqopt::CPhysicalUnionAll operator instance. Depending the
// parameter fParallel we construct either a CPhysicalParallelUnionAll or
// a CPhysicalSerialUnionAll instance.
class CPhysicalUnionAllFactory
{
private:
	CLogicalUnionAll *const m_popLogicalUnionAll;

public:
	CPhysicalUnionAllFactory(CLogicalUnionAll *popLogicalUnionAll);

	CPhysicalUnionAll *PopPhysicalUnionAll(CMemoryPool *mp, BOOL fParallel);
};

}  // namespace spqopt

#endif	//SPQOPT_CPhysicalUnionAllFactory_H
