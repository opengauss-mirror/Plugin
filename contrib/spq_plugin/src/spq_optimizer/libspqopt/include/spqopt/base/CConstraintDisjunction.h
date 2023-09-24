//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CConstraintDisjunction.h
//
//	@doc:
//		Representation of a disjunction constraint. A disjunction is a number
//		of ORed constraints
//---------------------------------------------------------------------------
#ifndef SPQOPT_CConstraintDisjunction_H
#define SPQOPT_CConstraintDisjunction_H

#include "spqos/base.h"

#include "spqopt/base/CConstraint.h"

namespace spqopt
{
using namespace spqos;
using namespace spqmd;

//---------------------------------------------------------------------------
//	@class:
//		CConstraintDisjunction
//
//	@doc:
//		Representation of a disjunction constraint
//
//---------------------------------------------------------------------------
class CConstraintDisjunction : public CConstraint
{
private:
	// array of constraints
	CConstraintArray *m_pdrspqcnstr;

	// mapping colref -> array of child constraints
	ColRefToConstraintArrayMap *m_phmcolconstr;

	// hidden copy ctor
	CConstraintDisjunction(const CConstraintDisjunction &);

public:
	// ctor
	CConstraintDisjunction(CMemoryPool *mp, CConstraintArray *pdrspqcnstr);

	// dtor
	virtual ~CConstraintDisjunction();

	// constraint type accessor
	virtual EConstraintType
	Ect() const
	{
		return CConstraint::EctDisjunction;
	}

	// all constraints in disjunction
	CConstraintArray *
	Pdrspqcnstr() const
	{
		return m_pdrspqcnstr;
	}

	// is this constraint a contradiction
	virtual BOOL FContradiction() const;

	// return a copy of the constraint with remapped columns
	virtual CConstraint *PcnstrCopyWithRemappedColumns(
		CMemoryPool *mp, UlongToColRefMap *colref_mapping, BOOL must_exist);

	// scalar expression
	virtual CExpression *PexprScalar(CMemoryPool *mp);

	// check if there is a constraint on the given column
	virtual BOOL FConstraint(const CColRef *colref) const;

	// return constraint on a given column
	virtual CConstraint *Pcnstr(CMemoryPool *mp, const CColRef *colref);

	// return constraint on a given column set
	virtual CConstraint *Pcnstr(CMemoryPool *mp, CColRefSet *pcrs);

	// return a clone of the constraint for a different column
	virtual CConstraint *PcnstrRemapForColumn(CMemoryPool *mp,
											  CColRef *colref) const;

	// print
	virtual IOstream &
	OsPrint(IOstream &os) const
	{
		return PrintConjunctionDisjunction(os, m_pdrspqcnstr);
	}

};	// class CConstraintDisjunction
}  // namespace spqopt

#endif	// !SPQOPT_CConstraintDisjunction_H

// EOF
