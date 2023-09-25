//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CConstraint.h
//
//	@doc:
//		Base class for representing constraints
//---------------------------------------------------------------------------
#ifndef SPQOPT_CConstraint_H
#define SPQOPT_CConstraint_H

#include "spqos/base.h"
#include "spqos/common/CDynamicPtrArray.h"
#include "spqos/common/CHashMap.h"
#include "spqos/common/CRefCount.h"
#include "spqos/types.h"

#include "spqopt/base/CColRef.h"
#include "spqopt/base/CColRefSet.h"

namespace spqopt
{
using namespace spqos;

// fwd declaration
class CExpression;
class CConstraint;

// constraint array
typedef CDynamicPtrArray<CConstraint, CleanupRelease> CConstraintArray;

// hash map mapping CColRef -> CConstraintArray
typedef CHashMap<CColRef, CConstraintArray, CColRef::HashValue, CColRef::Equals,
				 CleanupNULL<CColRef>, CleanupRelease<CConstraintArray> >
	ColRefToConstraintArrayMap;

// mapping CConstraint -> BOOL to cache previous containment queries,
// we use pointer equality here for fast map lookup -- since we do shallow comparison, we do not take ownership
// of pointer values
typedef CHashMap<CConstraint, BOOL, spqos::HashPtr<CConstraint>,
				 spqos::EqualPtr<CConstraint>, CleanupNULL<CConstraint>,
				 CleanupNULL<BOOL> >
	ConstraintContainmentMap;

// hash map mapping ULONG -> CConstraint
typedef CHashMap<ULONG, CConstraint, spqos::HashValue<ULONG>,
				 spqos::Equals<ULONG>, CleanupDelete<ULONG>,
				 CleanupRelease<CConstraint> >
	UlongToConstraintMap;

//---------------------------------------------------------------------------
//	@class:
//		CConstraint
//
//	@doc:
//		Base class for representing constraints
//
//---------------------------------------------------------------------------
class CConstraint : public CRefCount, public DbgPrintMixin<CConstraint>
{
public:
	enum EConstraintType
	{
		EctInterval,	 // a single interval on a single columns
		EctConjunction,	 // a set of ANDed constraints
		EctDisjunction,	 // a set of ORed constraints
		EctNegation		 // a negated constraint
	};

private:
	// containment map
	ConstraintContainmentMap *m_phmcontain;

	// constant true
	static BOOL m_fTrue;

	// constant false
	static BOOL m_fFalse;

	// hidden copy ctor
	CConstraint(const CConstraint &);

	// return address of static BOOL constant based on passed BOOL value
	static BOOL *
	PfVal(BOOL value)
	{
		if (value)
		{
			return &m_fTrue;
		}

		return &m_fFalse;
	}

	// add column as a new equivalence class, if it is not already in one of the
	// existing equivalence classes
	static void AddColumnToEquivClasses(CMemoryPool *mp, const CColRef *colref,
										CColRefSetArray *ppdrspqcrs);

	// create constraint from scalar comparison
	static CConstraint *PcnstrFromScalarCmp(CMemoryPool *mp, CExpression *pexpr,
											CColRefSetArray **ppdrspqcrs,
											BOOL infer_nulls_as = false);

	// create constraint from scalar boolean expression
	static CConstraint *PcnstrFromScalarBoolOp(CMemoryPool *mp,
											   CExpression *pexpr,
											   CColRefSetArray **ppdrspqcrs,
											   BOOL infer_nulls_as = false);

	// create conjunction/disjunction from array of constraints
	static CConstraint *PcnstrConjDisj(CMemoryPool *mp,
									   CConstraintArray *pdrspqcnstr,
									   BOOL fConj);

protected:
	// memory pool -- used for local computations
	CMemoryPool *m_mp;

	// columns used in this constraint
	CColRefSet *m_pcrsUsed;

	// equivalent scalar expression
	CExpression *m_pexprScalar;

	// print
	IOstream &PrintConjunctionDisjunction(IOstream &os,
										  CConstraintArray *pdrspqcnstr) const;

	// construct a conjunction or disjunction scalar expression from an
	// array of constraints
	CExpression *PexprScalarConjDisj(CMemoryPool *mp,
									 CConstraintArray *pdrspqcnstr,
									 BOOL fConj) const;

	// flatten an array of constraints to be used as constraint children
	CConstraintArray *PdrspqcnstrFlatten(CMemoryPool *mp,
										CConstraintArray *pdrspqcnstr,
										EConstraintType ect) const;

	// combine any two or more constraints that reference only one particular column
	CConstraintArray *PdrspqcnstrDeduplicate(CMemoryPool *mp,
											CConstraintArray *pdrspqcnstr,
											EConstraintType ect) const;

	// mapping between columns and arrays of constraints
	ColRefToConstraintArrayMap *Phmcolconstr(
		CMemoryPool *mp, CColRefSet *pcrs, CConstraintArray *pdrspqcnstr) const;

	// return a copy of the conjunction/disjunction constraint for a different column
	CConstraint *PcnstrConjDisjRemapForColumn(CMemoryPool *mp, CColRef *colref,
											  CConstraintArray *pdrspqcnstr,
											  BOOL fConj) const;

	// create constraint from scalar array comparison expression originally generated for
	// "scalar op ANY/ALL (array)" construct
	static CConstraint *PcnstrFromScalarArrayCmp(CMemoryPool *mp,
												 CExpression *pexpr,
												 CColRef *colref,
												 BOOL infer_nulls_as = false);

public:
	// ctor
	explicit CConstraint(CMemoryPool *mp);

	// dtor
	virtual ~CConstraint();

	// constraint type accessor
	virtual EConstraintType Ect() const = 0;

	// is this constraint a contradiction
	virtual BOOL FContradiction() const = 0;

	// is this constraint unbounded
	virtual BOOL
	IsConstraintUnbounded() const
	{
		return false;
	}

	// does the current constraint contain the given one
	virtual BOOL Contains(CConstraint *pcnstr);

	// equality function
	virtual BOOL Equals(CConstraint *pcnstr);

	// columns in this constraint
	virtual CColRefSet *
	PcrsUsed() const
	{
		return m_pcrsUsed;
	}

	// scalar expression
	virtual CExpression *PexprScalar(CMemoryPool *mp) = 0;

	// check if there is a constraint on the given column
	virtual BOOL FConstraint(const CColRef *colref) const = 0;

	virtual BOOL
	FConstraintOnSegmentId() const
	{
		return false;
	}

	virtual CConstraint *
	GetConstraintOnSegmentId() const
	{
		return NULL;
	}

	// return a copy of the constraint with remapped columns
	virtual CConstraint *PcnstrCopyWithRemappedColumns(
		CMemoryPool *mp, UlongToColRefMap *colref_mapping, BOOL must_exist) = 0;

	// return constraint on a given column
	virtual CConstraint *
	Pcnstr(CMemoryPool *,	//mp,
		   const CColRef *	//colref
	)
	{
		return NULL;
	}

	// return constraint on a given set of columns
	virtual CConstraint *
	Pcnstr(CMemoryPool *,  //mp,
		   CColRefSet *	   //pcrs
	)
	{
		return NULL;
	}

	// return a clone of the constraint for a different column
	virtual CConstraint *PcnstrRemapForColumn(CMemoryPool *mp,
											  CColRef *colref) const = 0;

	// create constraint from scalar expression and pass back any discovered
	// equivalence classes
	static CConstraint *PcnstrFromScalarExpr(CMemoryPool *mp,
											 CExpression *pexpr,
											 CColRefSetArray **ppdrspqcrs,
											 BOOL infer_nulls_as = false);

	// create constraint from EXISTS/ANY scalar subquery
	static CConstraint *PcnstrFromExistsAnySubquery(
		CMemoryPool *mp, CExpression *pexpr, CColRefSetArray **ppdrspqcrs);

	// create conjunction from array of constraints
	static CConstraint *PcnstrConjunction(CMemoryPool *mp,
										  CConstraintArray *pdrspqcnstr);

	// create disjunction from array of constraints
	static CConstraint *PcnstrDisjunction(CMemoryPool *mp,
										  CConstraintArray *pdrspqcnstr);

	// merge equivalence classes coming from children of a bool op
	static CColRefSetArray *PdrspqcrsMergeFromBoolOp(
		CMemoryPool *mp, CExpression *pexpr, CColRefSetArray *pdrspqcrsFst,
		CColRefSetArray *pdrspqcrsSnd);

	// subset of the given constraints, which reference the given column
	static CConstraintArray *PdrspqcnstrOnColumn(CMemoryPool *mp,
												CConstraintArray *pdrspqcnstr,
												CColRef *colref,
												BOOL fExclusive);

};	// class CConstraint

// shorthand for printing, pointer.
inline IOstream &
operator<<(IOstream &os, const CConstraint *cnstr)
{
	return cnstr->OsPrint(os);
}
// shorthand for printing
inline IOstream &
operator<<(IOstream &os, const CConstraint &cnstr)
{
	return cnstr.OsPrint(os);
}
}  // namespace spqopt

#endif	// !SPQOPT_CConstraint_H

// EOF
