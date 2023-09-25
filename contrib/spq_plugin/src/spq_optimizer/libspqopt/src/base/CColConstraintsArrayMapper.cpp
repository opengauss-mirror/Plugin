//	Greenplum Database
//	Copyright (C) 2016 Pivotal Software, Inc.

#include "spqopt/base/CColConstraintsArrayMapper.h"

#include "spqopt/base/CConstraint.h"

using namespace spqopt;

CConstraintArray *
CColConstraintsArrayMapper::PdrgPcnstrLookup(CColRef *colref)
{
	const BOOL fExclusive = true;
	return CConstraint::PdrspqcnstrOnColumn(m_mp, m_pdrspqcnstr, colref,
										   fExclusive);
}

CColConstraintsArrayMapper::CColConstraintsArrayMapper(
	spqos::CMemoryPool *mp, CConstraintArray *pdrspqcnstr)
	: m_mp(mp), m_pdrspqcnstr(pdrspqcnstr)
{
}

CColConstraintsArrayMapper::~CColConstraintsArrayMapper()
{
	m_pdrspqcnstr->Release();
}
