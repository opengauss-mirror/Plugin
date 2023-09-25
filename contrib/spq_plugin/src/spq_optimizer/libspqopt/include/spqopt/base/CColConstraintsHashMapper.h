//	Greenplum Database
//	Copyright (C) 2016 Pivotal Software, Inc.

#ifndef SPQOPT_CColConstraintsHashMapper_H
#define SPQOPT_CColConstraintsHashMapper_H

#include "spqos/memory/CMemoryPool.h"

#include "spqopt/base/CConstraint.h"
#include "spqopt/base/IColConstraintsMapper.h"

namespace spqopt
{
class CColConstraintsHashMapper : public IColConstraintsMapper
{
public:
	CColConstraintsHashMapper(CMemoryPool *mp, CConstraintArray *pdrgPcnstr);

	virtual CConstraintArray *PdrgPcnstrLookup(CColRef *colref);
	virtual ~CColConstraintsHashMapper();

private:
	ColRefToConstraintArrayMap *m_phmColConstr;
};
}  // namespace spqopt

#endif	//SPQOPT_CColConstraintsHashMapper_H
