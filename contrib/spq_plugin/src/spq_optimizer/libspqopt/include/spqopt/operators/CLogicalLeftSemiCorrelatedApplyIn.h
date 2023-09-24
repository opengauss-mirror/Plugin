//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal Inc.
//
//	@filename:
//		CLogicalLeftSemiCorrelatedApplyIn.h
//
//	@doc:
//		Logical Left Semi Correlated Apply operator;
//		a variant of left semi apply that captures the need to implement a
//		correlated-execution strategy on the physical side
//---------------------------------------------------------------------------
#ifndef SPQOPT_CLogicalLeftSemiCorrelatedApplyIn_H
#define SPQOPT_CLogicalLeftSemiCorrelatedApplyIn_H

#include "spqos/base.h"

#include "spqopt/operators/CLogicalLeftSemiApplyIn.h"

namespace spqopt
{
//---------------------------------------------------------------------------
//	@class:
//		CLogicalLeftSemiCorrelatedApplyIn
//
//	@doc:
//		Logical Apply operator used in scalar subquery transformations
//
//---------------------------------------------------------------------------
class CLogicalLeftSemiCorrelatedApplyIn : public CLogicalLeftSemiApplyIn
{
private:
	// private copy ctor
	CLogicalLeftSemiCorrelatedApplyIn(
		const CLogicalLeftSemiCorrelatedApplyIn &);

public:
	// ctor for patterns
	explicit CLogicalLeftSemiCorrelatedApplyIn(CMemoryPool *mp);

	// ctor
	CLogicalLeftSemiCorrelatedApplyIn(CMemoryPool *mp,
									  CColRefArray *pdrspqcrInner,
									  EOperatorId eopidOriginSubq);

	// dtor
	virtual ~CLogicalLeftSemiCorrelatedApplyIn()
	{
	}

	// ident accessors
	virtual EOperatorId
	Eopid() const
	{
		return EopLogicalLeftSemiCorrelatedApplyIn;
	}

	// return a string for operator name
	virtual const CHAR *
	SzId() const
	{
		return "CLogicalLeftSemiCorrelatedApplyIn";
	}

	// applicable transformations
	virtual CXformSet *PxfsCandidates(CMemoryPool *mp) const;

	// return true if operator is a correlated apply
	virtual BOOL
	FCorrelated() const
	{
		return true;
	}

	// return a copy of the operator with remapped columns
	virtual COperator *PopCopyWithRemappedColumns(
		CMemoryPool *mp, UlongToColRefMap *colref_mapping, BOOL must_exist);

	// conversion function
	static CLogicalLeftSemiCorrelatedApplyIn *
	PopConvert(COperator *pop)
	{
		SPQOS_ASSERT(NULL != pop);
		SPQOS_ASSERT(EopLogicalLeftSemiCorrelatedApplyIn == pop->Eopid());

		return dynamic_cast<CLogicalLeftSemiCorrelatedApplyIn *>(pop);
	}

};	// class CLogicalLeftSemiCorrelatedApplyIn

}  // namespace spqopt


#endif	// !SPQOPT_CLogicalLeftSemiCorrelatedApplyIn_H

// EOF
