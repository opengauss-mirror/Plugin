//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CLogicalIntersectAll.h
//
//	@doc:
//		Logical Intersect all operator
//---------------------------------------------------------------------------
#ifndef SPQOPT_CLogicalIntersectAll_H
#define SPQOPT_CLogicalIntersectAll_H

#include "spqos/base.h"

#include "spqopt/operators/CExpressionHandle.h"
#include "spqopt/operators/CLogicalSetOp.h"

namespace spqopt
{
//---------------------------------------------------------------------------
//	@class:
//		CLogicalIntersectAll
//
//	@doc:
//		Intersect all operators
//
//---------------------------------------------------------------------------
class CLogicalIntersectAll : public CLogicalSetOp
{
private:
	// private copy ctor
	CLogicalIntersectAll(const CLogicalIntersectAll &);

public:
	// ctor
	explicit CLogicalIntersectAll(CMemoryPool *mp);

	CLogicalIntersectAll(CMemoryPool *mp, CColRefArray *pdrspqcrOutput,
						 CColRef2dArray *pdrspqdrspqcrInput);

	// dtor
	virtual ~CLogicalIntersectAll();

	// ident accessors
	virtual EOperatorId
	Eopid() const
	{
		return EopLogicalIntersectAll;
	}

	// return a string for operator name
	virtual const CHAR *
	SzId() const
	{
		return "CLogicalIntersectAll";
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
		return PpcDeriveConstraintSetop(mp, exprhdl, true /*fIntersect*/);
	}

	//-------------------------------------------------------------------------------------
	// Transformations
	//-------------------------------------------------------------------------------------

	// candidate set of xforms
	CXformSet *PxfsCandidates(CMemoryPool *mp) const;

	//-------------------------------------------------------------------------------------
	// Derived Stats
	//-------------------------------------------------------------------------------------

	// stat promise
	virtual EStatPromise
	Esp(CExpressionHandle &) const
	{
		return CLogical::EspHigh;
	}

	// derive statistics
	virtual IStatistics *PstatsDerive(CMemoryPool *mp,
									  CExpressionHandle &exprhdl,
									  IStatisticsArray *stats_ctxt) const;

	//-------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------

	// conversion function
	static CLogicalIntersectAll *
	PopConvert(COperator *pop)
	{
		SPQOS_ASSERT(NULL != pop);
		SPQOS_ASSERT(EopLogicalIntersectAll == pop->Eopid());

		return reinterpret_cast<CLogicalIntersectAll *>(pop);
	}

	// derive statistics
	static IStatistics *PstatsDerive(CMemoryPool *mp,
									 CExpressionHandle &exprhdl,
									 CColRef2dArray *pdrspqdrspqcrInput,
									 CColRefSetArray *output_colrefsets);

};	// class CLogicalIntersectAll

}  // namespace spqopt

#endif	// !SPQOPT_CLogicalIntersectAll_H

// EOF
