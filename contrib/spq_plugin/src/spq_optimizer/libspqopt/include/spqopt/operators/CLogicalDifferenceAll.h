//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CLogicalDifferenceAll.h
//
//	@doc:
//		Logical Difference all operator (Difference all does not remove
//		duplicates from the left child)
//---------------------------------------------------------------------------
#ifndef SPQOPT_CLogicalDifferenceAll_H
#define SPQOPT_CLogicalDifferenceAll_H

#include "spqos/base.h"

#include "spqopt/operators/CExpressionHandle.h"
#include "spqopt/operators/CLogicalSetOp.h"

namespace spqopt
{
//---------------------------------------------------------------------------
//	@class:
//		CLogicalDifferenceAll
//
//	@doc:
//		Difference all operators
//
//---------------------------------------------------------------------------
class CLogicalDifferenceAll : public CLogicalSetOp
{
private:
	// private copy ctor
	CLogicalDifferenceAll(const CLogicalDifferenceAll &);

public:
	// ctor
	explicit CLogicalDifferenceAll(CMemoryPool *mp);

	CLogicalDifferenceAll(CMemoryPool *mp, CColRefArray *pdrspqcrOutput,
						  CColRef2dArray *pdrspqdrspqcrInput);

	// dtor
	virtual ~CLogicalDifferenceAll();

	// ident accessors
	virtual EOperatorId
	Eopid() const
	{
		return EopLogicalDifferenceAll;
	}

	// return a string for operator name
	virtual const CHAR *
	SzId() const
	{
		return "CLogicalDifferenceAll";
	}

	// sensitivity to order of inputs
	BOOL
	FInputOrderSensitive() const
	{
		return true;
	}

	// return a copy of the operator with remapped columns
	virtual COperator *PopCopyWithRemappedColumns(
		CMemoryPool *mp, UlongToColRefMap *colref_mapping, BOOL must_exist);

	//-------------------------------------------------------------------------------------
	// Derived Relational Properties
	//-------------------------------------------------------------------------------------

	// derive max card
	virtual CMaxCard DeriveMaxCard(CMemoryPool *mp,
								   CExpressionHandle &exprhdl) const;

	// derive key collections
	virtual CKeyCollection *DeriveKeyCollection(
		CMemoryPool *mp, CExpressionHandle &exprhdl) const;

	// derive constraint property
	virtual CPropConstraint *
	DerivePropertyConstraint(CMemoryPool *mp, CExpressionHandle &exprhdl) const
	{
		return PpcDeriveConstraintSetop(mp, exprhdl, false /*fIntersect*/);
	}

	//-------------------------------------------------------------------------------------
	// Transformations
	//-------------------------------------------------------------------------------------

	// candidate set of xforms
	CXformSet *PxfsCandidates(CMemoryPool *mp) const;

	// stat promise
	virtual EStatPromise
	Esp(CExpressionHandle &	 // exprhdl
	) const
	{
		return CLogical::EspLow;
	}

	// derive statistics
	virtual IStatistics *PstatsDerive(CMemoryPool *mp,
									  CExpressionHandle &exprhdl,
									  IStatisticsArray *stats_ctxt) const;

	// conversion function
	static CLogicalDifferenceAll *
	PopConvert(COperator *pop)
	{
		SPQOS_ASSERT(NULL != pop);
		SPQOS_ASSERT(EopLogicalDifferenceAll == pop->Eopid());

		return reinterpret_cast<CLogicalDifferenceAll *>(pop);
	}

};	// class CLogicalDifferenceAll

}  // namespace spqopt

#endif	// !SPQOPT_CLogicalDifferenceAll_H

// EOF
