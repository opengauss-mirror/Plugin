//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		CLogicalUnion.h
//
//	@doc:
//		Logical Union operator
//---------------------------------------------------------------------------
#ifndef SPQOPT_CLogicalUnion_H
#define SPQOPT_CLogicalUnion_H

#include "spqos/base.h"

#include "spqopt/operators/CExpressionHandle.h"
#include "spqopt/operators/CLogicalSetOp.h"

namespace spqopt
{
//---------------------------------------------------------------------------
//	@class:
//		CLogicalUnion
//
//	@doc:
//		union operators
//
//---------------------------------------------------------------------------
class CLogicalUnion : public CLogicalSetOp
{
private:
	// private copy ctor
	CLogicalUnion(const CLogicalUnion &);

public:
	// ctor
	explicit CLogicalUnion(CMemoryPool *mp);

	CLogicalUnion(CMemoryPool *mp, CColRefArray *pdrspqcrOutput,
				  CColRef2dArray *pdrspqdrspqcrInput);

	// dtor
	virtual ~CLogicalUnion();

	// ident accessors
	virtual EOperatorId
	Eopid() const
	{
		return EopLogicalUnion;
	}

	// return a string for operator name
	virtual const CHAR *
	SzId() const
	{
		return "CLogicalUnion";
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
		return PpcDeriveConstraintSetop(mp, exprhdl, false /*fIntersect*/);
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
	static CLogicalUnion *
	PopConvert(COperator *pop)
	{
		SPQOS_ASSERT(NULL != pop);
		SPQOS_ASSERT(EopLogicalUnion == pop->Eopid());

		return reinterpret_cast<CLogicalUnion *>(pop);
	}

};	// class CLogicalUnion

}  // namespace spqopt


#endif	// !SPQOPT_CLogicalUnion_H

// EOF
