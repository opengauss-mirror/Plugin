//	Greenplum Database
//	Copyright (C) 2016 Pivotal Software, Inc.

#ifndef SPQOPT_CColConstraintsArrayMapper_H
#define SPQOPT_CColConstraintsArrayMapper_H

#include "spqos/memory/CMemoryPool.h"

#include "spqopt/base/CConstraint.h"
#include "spqopt/base/IColConstraintsMapper.h"

namespace spqopt
{
class CColConstraintsArrayMapper : public IColConstraintsMapper
{
public:
	CColConstraintsArrayMapper(spqos::CMemoryPool *mp,
							   CConstraintArray *pdrspqcnstr);
	virtual CConstraintArray *PdrgPcnstrLookup(CColRef *colref);

	virtual ~CColConstraintsArrayMapper();

private:
	spqos::CMemoryPool *m_mp;
	CConstraintArray *m_pdrspqcnstr;
};
}  // namespace spqopt

#endif	//SPQOPT_CColConstraintsArrayMapper_H
