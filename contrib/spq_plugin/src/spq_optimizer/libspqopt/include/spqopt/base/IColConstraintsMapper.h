//	Greenplum Database
//	Copyright (C) 2016 Pivotal Software, Inc.

#ifndef SPQOPT_IColConstraintsMapper_H
#define SPQOPT_IColConstraintsMapper_H

#include "spqos/common/CRefCount.h"

#include "spqopt/base/CConstraint.h"

namespace spqopt
{
class IColConstraintsMapper : public CRefCount
{
public:
	virtual CConstraintArray *PdrgPcnstrLookup(CColRef *colref) = 0;

	virtual ~IColConstraintsMapper() = 0;
};
}  // namespace spqopt

#endif	//SPQOPT_IColConstraintsMapper_H
