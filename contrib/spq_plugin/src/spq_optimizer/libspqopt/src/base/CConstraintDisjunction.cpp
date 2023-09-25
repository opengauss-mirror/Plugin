//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CConstraintDisjunction.cpp
//
//	@doc:
//		Implementation of disjunction constraints
//---------------------------------------------------------------------------

#include "spqopt/base/CConstraintDisjunction.h"

#include "spqos/base.h"

#include "spqopt/base/CConstraintInterval.h"
#include "spqopt/base/CUtils.h"
#include "spqopt/operators/CPredicateUtils.h"

using namespace spqopt;

//---------------------------------------------------------------------------
//	@function:
//		CConstraintDisjunction::CConstraintDisjunction
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CConstraintDisjunction::CConstraintDisjunction(CMemoryPool *mp,
											   CConstraintArray *pdrspqcnstr)
	: CConstraint(mp), m_pdrspqcnstr(NULL)
{
	SPQOS_ASSERT(NULL != pdrspqcnstr);
	m_pdrspqcnstr = PdrspqcnstrFlatten(mp, pdrspqcnstr, EctDisjunction);

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
//		CConstraintDisjunction::~CConstraintDisjunction
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CConstraintDisjunction::~CConstraintDisjunction()
{
	m_pdrspqcnstr->Release();
	m_pcrsUsed->Release();
	m_phmcolconstr->Release();
}

//---------------------------------------------------------------------------
//	@function:
//		CConstraintDisjunction::FContradiction
//
//	@doc:
//		Check if this constraint is a contradiction
//
//---------------------------------------------------------------------------
BOOL
CConstraintDisjunction::FContradiction() const
{
	const ULONG length = m_pdrspqcnstr->Size();

	BOOL fContradiction = true;
	for (ULONG ul = 0; fContradiction && ul < length; ul++)
	{
		fContradiction = (*m_pdrspqcnstr)[ul]->FContradiction();
	}

	return fContradiction;
}

//---------------------------------------------------------------------------
//	@function:
//		CConstraintDisjunction::FConstraint
//
//	@doc:
//		Check if there is a constraint on the given column
//
//---------------------------------------------------------------------------
BOOL
CConstraintDisjunction::FConstraint(const CColRef *colref) const
{
	CConstraintArray *pdrspqcnstrCol = m_phmcolconstr->Find(colref);
	return (NULL != pdrspqcnstrCol &&
			m_pdrspqcnstr->Size() == pdrspqcnstrCol->Size());
}

//---------------------------------------------------------------------------
//	@function:
//		CConstraintDisjunction::PcnstrCopyWithRemappedColumns
//
//	@doc:
//		Return a copy of the constraint with remapped columns
//
//---------------------------------------------------------------------------
CConstraint *
CConstraintDisjunction::PcnstrCopyWithRemappedColumns(
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
	return SPQOS_NEW(mp) CConstraintDisjunction(mp, pdrspqcnstr);
}

//---------------------------------------------------------------------------
//	@function:
//		CConstraintDisjunction::Pcnstr
//
//	@doc:
//		Return constraint on a given column
//
//---------------------------------------------------------------------------
CConstraint *
CConstraintDisjunction::Pcnstr(CMemoryPool *mp, const CColRef *colref)
{
	// all children referencing given column
	CConstraintArray *pdrspqcnstrCol = m_phmcolconstr->Find(colref);
	if (NULL == pdrspqcnstrCol)
	{
		return NULL;
	}

	// if not all children have this col, return unbounded constraint
	const ULONG length = pdrspqcnstrCol->Size();
	if (length != m_pdrspqcnstr->Size())
	{
		return CConstraintInterval::PciUnbounded(mp, colref,
												 true /*fIncludesNull*/);
	}

	CConstraintArray *pdrspqcnstr = SPQOS_NEW(mp) CConstraintArray(mp);

	for (ULONG ul = 0; ul < length; ul++)
	{
		// the part of the child that references this column
		CConstraint *pcnstrCol = (*pdrspqcnstrCol)[ul]->Pcnstr(mp, colref);
		if (NULL == pcnstrCol)
		{
			pcnstrCol =
				CConstraintInterval::PciUnbounded(mp, colref, true /*is_null*/);
		}
		if (pcnstrCol->IsConstraintUnbounded())
		{
			pdrspqcnstr->Release();
			return pcnstrCol;
		}
		pdrspqcnstr->Append(pcnstrCol);
	}

	return CConstraint::PcnstrDisjunction(mp, pdrspqcnstr);
}

//---------------------------------------------------------------------------
//	@function:
//		CConstraintDisjunction::Pcnstr
//
//	@doc:
//		Return constraint on a given column set
//
//---------------------------------------------------------------------------
CConstraint *
CConstraintDisjunction::Pcnstr(CMemoryPool *mp, CColRefSet *pcrs)
{
	const ULONG length = m_pdrspqcnstr->Size();

	CConstraintArray *pdrspqcnstr = SPQOS_NEW(mp) CConstraintArray(mp);

	for (ULONG ul = 0; ul < length; ul++)
	{
		CConstraint *pcnstr = (*m_pdrspqcnstr)[ul];
		if (pcnstr->PcrsUsed()->IsDisjoint(pcrs))
		{
			// a child has none of these columns... return unbounded constraint
			pdrspqcnstr->Release();
			return CConstraintInterval::PciUnbounded(mp, pcrs,
													 true /*fIncludesNull*/);
		}

		// the part of the child that references these columns
		CConstraint *pcnstrCol = pcnstr->Pcnstr(mp, pcrs);

		if (NULL == pcnstrCol)
		{
			pcnstrCol = CConstraintInterval::PciUnbounded(
				mp, pcrs, true /*fIncludesNull*/);
		}
		SPQOS_ASSERT(NULL != pcnstrCol);
		pdrspqcnstr->Append(pcnstrCol);
	}

	return CConstraint::PcnstrDisjunction(mp, pdrspqcnstr);
}

//---------------------------------------------------------------------------
//	@function:
//		CConstraintDisjunction::PcnstrRemapForColumn
//
//	@doc:
//		Return a copy of the constraint for a different column
//
//---------------------------------------------------------------------------
CConstraint *
CConstraintDisjunction::PcnstrRemapForColumn(CMemoryPool *mp,
											 CColRef *colref) const
{
	return PcnstrConjDisjRemapForColumn(mp, colref, m_pdrspqcnstr,
										false /*fConj*/);
}

//---------------------------------------------------------------------------
//	@function:
//		CConstraintDisjunction::PexprScalar
//
//	@doc:
//		Scalar expression
//
//---------------------------------------------------------------------------
CExpression *
CConstraintDisjunction::PexprScalar(CMemoryPool *mp)
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
				PexprScalarConjDisj(mp, m_pdrspqcnstr, false /*fConj*/);
		}
	}

	return m_pexprScalar;
}

// EOF
