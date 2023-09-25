//	Greenplum Database
//	Copyright (C) 2016 Pivotal Software, Inc.

#include "spqopt/base/CColConstraintsHashMapper.h"

#include "spqos/common/CAutoRef.h"

using namespace spqopt;

CConstraintArray *
CColConstraintsHashMapper::PdrgPcnstrLookup(CColRef *colref)
{
	CConstraintArray *pdrspqcnstrCol = m_phmColConstr->Find(colref);
	pdrspqcnstrCol->AddRef();
	return pdrspqcnstrCol;
}

// mapping between columns and single column constraints in array of constraints
static ColRefToConstraintArrayMap *
PhmcolconstrSingleColConstr(CMemoryPool *mp, CConstraintArray *drgPcnstr)
{
	CAutoRef<CConstraintArray> arpdrspqcnstr(drgPcnstr);
	ColRefToConstraintArrayMap *phmcolconstr =
		SPQOS_NEW(mp) ColRefToConstraintArrayMap(mp);

	const ULONG length = arpdrspqcnstr->Size();

	for (ULONG ul = 0; ul < length; ul++)
	{
		CConstraint *pcnstrChild = (*arpdrspqcnstr)[ul];
		CColRefSet *pcrs = pcnstrChild->PcrsUsed();

		if (1 == pcrs->Size())
		{
			CColRef *colref = pcrs->PcrFirst();
			CConstraintArray *pcnstrMapped = phmcolconstr->Find(colref);
			if (NULL == pcnstrMapped)
			{
				pcnstrMapped = SPQOS_NEW(mp) CConstraintArray(mp);
				phmcolconstr->Insert(colref, pcnstrMapped);
			}
			pcnstrChild->AddRef();
			pcnstrMapped->Append(pcnstrChild);
		}
	}

	return phmcolconstr;
}

CColConstraintsHashMapper::CColConstraintsHashMapper(
	CMemoryPool *mp, CConstraintArray *pdrspqcnstr)
	: m_phmColConstr(PhmcolconstrSingleColConstr(mp, pdrspqcnstr))
{
}

CColConstraintsHashMapper::~CColConstraintsHashMapper()
{
	m_phmColConstr->Release();
}
