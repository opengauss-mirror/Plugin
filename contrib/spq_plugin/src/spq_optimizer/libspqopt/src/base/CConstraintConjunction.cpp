//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CConstraintConjunction.cpp
//
//	@doc:
//		Implementation of conjunction constraints
//---------------------------------------------------------------------------

#include "spqopt/base/CConstraintConjunction.h"

#include "spqos/base.h"

#include "spqopt/base/CConstraintInterval.h"
#include "spqopt/base/CUtils.h"
#include "spqopt/operators/CPredicateUtils.h"

using namespace spqopt;

//---------------------------------------------------------------------------
//	@function:
//		CConstraintConjunction::CConstraintConjunction
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CConstraintConjunction::CConstraintConjunction(CMemoryPool *mp,
											   CConstraintArray *pdrspqcnstr)
	: CConstraint(mp), m_pdrspqcnstr(NULL)
{
	SPQOS_ASSERT(NULL != pdrspqcnstr);
	m_pdrspqcnstr = PdrspqcnstrFlatten(mp, pdrspqcnstr, EctConjunction);

	const ULONG length = m_pdrspqcnstr->Size();
	SPQOS_ASSERT(0 < length);
	m_pcrsUsed = SPQOS_NEW(mp) CColRefSet(mp);

	for (ULONG ul = 0; ul < length; ul++)
	{
		CConstraint *pcnstr = (*m_pdrspqcnstr)[ul];
		m_pcrsUsed->Include(pcnstr->PcrsUsed());
	}

	m_phmcolconstr = Phmcolconstr(mp, m_pcrsUsed, m_pdrspqcnstr);
}

//---------------------------------------------------------------------------
//	@function:
//		CConstraintConjunction::~CConstraintConjunction
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CConstraintConjunction::~CConstraintConjunction()
{
	m_pdrspqcnstr->Release();
	m_pcrsUsed->Release();
	m_phmcolconstr->Release();
}

//---------------------------------------------------------------------------
//	@function:
//		CConstraintConjunction::FContradiction
//
//	@doc:
//		Check if this constraint is a contradiction
//
//---------------------------------------------------------------------------
BOOL
CConstraintConjunction::FContradiction() const
{
	const ULONG length = m_pdrspqcnstr->Size();

	BOOL fContradiction = false;
	for (ULONG ul = 0; !fContradiction && ul < length; ul++)
	{
		fContradiction = (*m_pdrspqcnstr)[ul]->FContradiction();
	}

	return fContradiction;
}

//---------------------------------------------------------------------------
//	@function:
//		CConstraintConjunction::FConstraint
//
//	@doc:
//		Check if there is a constraint on the given column
//
//---------------------------------------------------------------------------
BOOL
CConstraintConjunction::FConstraint(const CColRef *colref) const
{
	CConstraintArray *pdrspqcnstrCol = m_phmcolconstr->Find(colref);
	return (NULL != pdrspqcnstrCol && 0 < pdrspqcnstrCol->Size());
}

//---------------------------------------------------------------------------
//	@function:
//		CConstraintConjunction::PcnstrCopyWithRemappedColumns
//
//	@doc:
//		Return a copy of the constraint with remapped columns. If must_exist is
//		set to true, then every column reference in this constraint must be in
//		the hashmap in order to be replace. Otherwise, some columns may not be
//		in the mapping, and hence will not be replaced
//
//---------------------------------------------------------------------------
CConstraint *
CConstraintConjunction::PcnstrCopyWithRemappedColumns(
	CMemoryPool *mp, UlongToColRefMap *colref_mapping, BOOL must_exist)
{
	CConstraintArray *pdrspqcnstr = SPQOS_NEW(mp) CConstraintArray(mp);
	const ULONG length = m_pdrspqcnstr->Size();
	for (ULONG ul = 0; ul < length; ul++)
	{
		CConstraint *pcnstr = (*m_pdrspqcnstr)[ul];
		CConstraint *pcnstrCopy = pcnstr->PcnstrCopyWithRemappedColumns(
			mp, colref_mapping, must_exist);
		pdrspqcnstr->Append(pcnstrCopy);
	}
	return SPQOS_NEW(mp) CConstraintConjunction(mp, pdrspqcnstr);
}

//---------------------------------------------------------------------------
//	@function:
//		CConstraintConjunction::Pcnstr
//
//	@doc:
//		Return constraint on a given column
//
//---------------------------------------------------------------------------
CConstraint *
CConstraintConjunction::Pcnstr(CMemoryPool *mp, const CColRef *colref)
{
	// all children referencing given column
	CConstraintArray *pdrspqcnstrCol = m_phmcolconstr->Find(colref);
	if (NULL == pdrspqcnstrCol)
	{
		return NULL;
	}

	CConstraintArray *pdrspqcnstr = SPQOS_NEW(mp) CConstraintArray(mp);

	const ULONG length = pdrspqcnstrCol->Size();
	for (ULONG ul = 0; ul < length; ul++)
	{
		// the part of the child that references this column
		CConstraint *pcnstrCol = (*pdrspqcnstrCol)[ul]->Pcnstr(mp, colref);
		if (NULL == pcnstrCol || pcnstrCol->IsConstraintUnbounded())
		{
			CRefCount::SafeRelease(pcnstrCol);
			continue;
		}
		pdrspqcnstr->Append(pcnstrCol);
	}

	return CConstraint::PcnstrConjunction(mp, pdrspqcnstr);
}

//---------------------------------------------------------------------------
//	@function:
//		CConstraintConjunction::Pcnstr
//
//	@doc:
//		Return constraint on a given column set
//
//---------------------------------------------------------------------------
CConstraint *
CConstraintConjunction::Pcnstr(CMemoryPool *mp, CColRefSet *pcrs)
{
	const ULONG length = m_pdrspqcnstr->Size();

	CConstraintArray *pdrspqcnstr = SPQOS_NEW(mp) CConstraintArray(mp);

	for (ULONG ul = 0; ul < length; ul++)
	{
		CConstraint *pcnstr = (*m_pdrspqcnstr)[ul];
		if (pcnstr->PcrsUsed()->IsDisjoint(pcrs))
		{
			continue;
		}

		// the part of the child that references these columns
		CConstraint *pcnstrCol = pcnstr->Pcnstr(mp, pcrs);
		if (NULL != pcnstrCol)
		{
			pdrspqcnstr->Append(pcnstrCol);
		}
	}

	return CConstraint::PcnstrConjunction(mp, pdrspqcnstr);
}

//---------------------------------------------------------------------------
//	@function:
//		CConstraintConjunction::PcnstrRemapForColumn
//
//	@doc:
//		Return a copy of the constraint for a different column
//
//---------------------------------------------------------------------------
CConstraint *
CConstraintConjunction::PcnstrRemapForColumn(CMemoryPool *mp,
											 CColRef *colref) const
{
	return PcnstrConjDisjRemapForColumn(mp, colref, m_pdrspqcnstr,
										true /*fConj*/);
}

//---------------------------------------------------------------------------
//	@function:
//		CConstraintConjunction::PexprScalar
//
//	@doc:
//		Scalar expression
//
//---------------------------------------------------------------------------
CExpression *
CConstraintConjunction::PexprScalar(CMemoryPool *mp)
{
	if (NULL == m_pexprScalar)
	{
		if (FContradiction())
		{
			m_pexprScalar = CUtils::PexprScalarConstBool(mp, false /*fval*/,
														 false /*is_null*/);
		}
		else
		{
			m_pexprScalar =
				PexprScalarConjDisj(mp, m_pdrspqcnstr, true /*fConj*/);
		}
	}

	return m_pexprScalar;
}

//---------------------------------------------------------------------------
//	@function:
//		CConstraintConjunction::GetConstraintOnSegmentId
//
//	@doc:
//		Returns the constraint for system column spq_segment_id
//
//---------------------------------------------------------------------------
CConstraint *
CConstraintConjunction::GetConstraintOnSegmentId() const
{
	for (ULONG ul = 0; ul < m_pdrspqcnstr->Size(); ul++)
	{
		CConstraint *pcnstr = (*m_pdrspqcnstr)[ul];
		if (pcnstr->FConstraintOnSegmentId())
		{
			return pcnstr;
		}
	}
	return NULL;
}

// EOF
