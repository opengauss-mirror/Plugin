//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CLogicalIntersect.h
//
//	@doc:
//		Logical Intersect operator
//---------------------------------------------------------------------------
#ifndef SPQOPT_CLogicalIntersect_H
#define SPQOPT_CLogicalIntersect_H

#include "spqos/base.h"

#include "spqopt/operators/CExpressionHandle.h"
#include "spqopt/operators/CLogicalSetOp.h"

namespace spqopt
{
//---------------------------------------------------------------------------
//	@class:
//		CLogicalIntersect
//
//	@doc:
//		Intersect operators
//
//---------------------------------------------------------------------------
class CLogicalIntersect : public CLogicalSetOp
{
private:
	// private copy ctor
	CLogicalIntersect(const CLogicalIntersect &);

public:
	// ctor
	explicit CLogicalIntersect(CMemoryPool *mp);

	CLogicalIntersect(CMemoryPool *mp, CColRefArray *pdrspqcrOutput,
					  CColRef2dArray *pdrspqdrspqcrInput);

	// dtor
	virtual ~CLogicalIntersect();

	// ident accessors
	virtual EOperatorId
	Eopid() const
	{
		return EopLogicalIntersect;
	}

	// return a string for operator name
	virtual const CHAR *
	SzId() const
	{
		return "CLogicalIntersect";
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
	static CLogicalIntersect *
	PopConvert(COperator *pop)
	{
		SPQOS_ASSERT(NULL != pop);
		SPQOS_ASSERT(EopLogicalIntersect == pop->Eopid());

		return reinterpret_cast<CLogicalIntersect *>(pop);
	}

};	// class CLogicalIntersect

}  // namespace spqopt


#endif	// !SPQOPT_CLogicalIntersect_H

// EOF
