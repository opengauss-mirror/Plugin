//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CLogicalInnerCorrelatedApply.h
//
//	@doc:
//		Logical Inner Correlated Apply operator;
//		a variant of inner apply that captures the need to implement a
//		correlated-execution strategy on the physical side
//---------------------------------------------------------------------------
#ifndef SPQOPT_CLogicalInnerCorrelatedApply_H
#define SPQOPT_CLogicalInnerCorrelatedApply_H

#include "spqos/base.h"

#include "spqopt/operators/CLogicalInnerApply.h"

namespace spqopt
{
//---------------------------------------------------------------------------
//	@class:
//		CLogicalInnerCorrelatedApply
//
//	@doc:
//		Logical Apply operator used in scalar subquery transformations
//
//---------------------------------------------------------------------------
class CLogicalInnerCorrelatedApply : public CLogicalInnerApply
{
private:
	// private copy ctor
	CLogicalInnerCorrelatedApply(const CLogicalInnerCorrelatedApply &);

public:
	// ctor
	CLogicalInnerCorrelatedApply(CMemoryPool *mp, CColRefArray *pdrspqcrInner,
								 EOperatorId eopidOriginSubq);

	// ctor for patterns
	explicit CLogicalInnerCorrelatedApply(CMemoryPool *mp);

	// dtor
	virtual ~CLogicalInnerCorrelatedApply()
	{
	}

	// ident accessors
	virtual EOperatorId
	Eopid() const
	{
		return EopLogicalInnerCorrelatedApply;
	}

	// return a string for operator name
	virtual const CHAR *
	SzId() const
	{
		return "CLogicalInnerCorrelatedApply";
	}

	// applicable transformations
	virtual CXformSet *PxfsCandidates(CMemoryPool *mp) const;

	// match function
	virtual BOOL Matches(COperator *pop) const;

	// return a copy of the operator with remapped columns
	virtual COperator *PopCopyWithRemappedColumns(
		CMemoryPool *mp, UlongToColRefMap *colref_mapping, BOOL must_exist);

	// return true if operator is a correlated apply
	virtual BOOL
	FCorrelated() const
	{
		return true;
	}

	// conversion function
	static CLogicalInnerCorrelatedApply *
	PopConvert(COperator *pop)
	{
		SPQOS_ASSERT(NULL != pop);
		SPQOS_ASSERT(EopLogicalInnerCorrelatedApply == pop->Eopid());

		return dynamic_cast<CLogicalInnerCorrelatedApply *>(pop);
	}

};	// class CLogicalInnerCorrelatedApply

}  // namespace spqopt


#endif	// !SPQOPT_CLogicalInnerCorrelatedApply_H

// EOF
