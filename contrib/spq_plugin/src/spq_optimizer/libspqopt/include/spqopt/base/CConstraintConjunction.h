//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CConstraintConjunction.h
//
//	@doc:
//		Representation of a conjunction constraint. A conjunction is a number
//		of ANDed constraints
//---------------------------------------------------------------------------
#ifndef SPQOPT_CConstraintConjunction_H
#define SPQOPT_CConstraintConjunction_H

#include "spqos/base.h"

#include "spqopt/base/CConstraint.h"
#include "spqopt/base/CRange.h"

namespace spqopt
{
using namespace spqos;
using namespace spqmd;

//---------------------------------------------------------------------------
//	@class:
//		CConstraintConjunction
//
//	@doc:
//		Representation of a conjunction constraint
//
//---------------------------------------------------------------------------
class CConstraintConjunction : public CConstraint
{
private:
	// array of constraints
	CConstraintArray *m_pdrspqcnstr;

	// mapping colref -> array of child constraints
	ColRefToConstraintArrayMap *m_phmcolconstr;

	// hidden copy ctor
	CConstraintConjunction(const CConstraintConjunction &);

public:
	// ctor
	CConstraintConjunction(CMemoryPool *mp, CConstraintArray *pdrspqcnstr);

	// dtor
	virtual ~CConstraintConjunction();

	// constraint type accessor
	virtual EConstraintType
	Ect() const
	{
		return CConstraint::EctConjunction;
	}

	// all constraints in conjunction
	CConstraintArray *
	Pdrspqcnstr() const
	{
		return m_pdrspqcnstr;
	}

	// is this constraint a contradiction
	virtual BOOL FContradiction() const;

	// scalar expression
	virtual CExpression *PexprScalar(CMemoryPool *mp);

	// check if there is a constraint on the given column
	virtual BOOL FConstraint(const CColRef *colref) const;

	// return a copy of the constraint with remapped columns
	virtual CConstraint *PcnstrCopyWithRemappedColumns(
		CMemoryPool *mp, UlongToColRefMap *colref_mapping, BOOL must_exist);

	// return constraint on a given column
	virtual CConstraint *Pcnstr(CMemoryPool *mp, const CColRef *colref);

	// return constraint on a given column set
	virtual CConstraint *Pcnstr(CMemoryPool *mp, CColRefSet *pcrs);

	// return a clone of the constraint for a different column
	virtual CConstraint *PcnstrRemapForColumn(CMemoryPool *mp,
											  CColRef *colref) const;

	// Returns the constraint for system column spq_segment_id
	CConstraint *GetConstraintOnSegmentId() const;

	// print
	virtual IOstream &
	OsPrint(IOstream &os) const
	{
		return PrintConjunctionDisjunction(os, m_pdrspqcnstr);
	}

};	// class CConstraintConjunction
}  // namespace spqopt

#endif	// !SPQOPT_CConstraintConjunction_H

// EOF
