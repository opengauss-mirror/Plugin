//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CPropConstraint.h
//
//	@doc:
//		Representation of constraint property
//---------------------------------------------------------------------------
#ifndef SPQOPT_CPropConstraint_H
#define SPQOPT_CPropConstraint_H

#include "spqos/base.h"
#include "spqos/common/CRefCount.h"

#include "spqopt/base/CColRefSet.h"
#include "spqopt/base/CConstraint.h"

namespace spqopt
{
using namespace spqos;

// forward declaration
class CConstraint;
class CExpression;

//---------------------------------------------------------------------------
//	@class:
//		CPropConstraint
//
//	@doc:
//		Representation of constraint property
//
//---------------------------------------------------------------------------
class CPropConstraint : public CRefCount, public DbgPrintMixin<CPropConstraint>
{
private:
	// array of equivalence classes
	CColRefSetArray *m_pdrspqcrs;

	// mapping from column to equivalence class
	ColRefToColRefSetMap *m_phmcrcrs;

	// constraint
	CConstraint *m_pcnstr;

	// hidden copy ctor
	CPropConstraint(const CPropConstraint &);

	// initialize mapping from columns to equivalence classes
	void InitHashMap(CMemoryPool *mp);

public:
	// ctor
	CPropConstraint(CMemoryPool *mp, CColRefSetArray *pdrspqcrs,
					CConstraint *pcnstr);

	// dtor
	virtual ~CPropConstraint();

	// equivalence classes
	CColRefSetArray *
	PdrspqcrsEquivClasses() const
	{
		return m_pdrspqcrs;
	}

	// mapping
	CColRefSet *
	PcrsEquivClass(const CColRef *colref) const
	{
		if (NULL == m_phmcrcrs)
		{
			return NULL;
		}
		return m_phmcrcrs->Find(colref);
	}

	// constraint
	CConstraint *
	Pcnstr() const
	{
		return m_pcnstr;
	}

	// is this a contradiction
	BOOL FContradiction() const;

	// scalar expression on given column mapped from all constraints
	// on its equivalent columns
	CExpression *PexprScalarMappedFromEquivCols(
		CMemoryPool *mp, CColRef *colref,
		CPropConstraint *constraintsForOuterRefs) const;

	// print
	virtual IOstream &OsPrint(IOstream &os) const;

};	// class CPropConstraint

// shorthand for printing
inline IOstream &
operator<<(IOstream &os, CPropConstraint &pc)
{
	return pc.OsPrint(os);
}
}  // namespace spqopt

#endif	// !SPQOPT_CPropConstraint_H

// EOF
