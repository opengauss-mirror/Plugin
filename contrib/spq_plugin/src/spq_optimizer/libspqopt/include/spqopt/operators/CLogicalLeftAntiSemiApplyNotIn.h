//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright 2013 EMC Corp.
//
//	@filename:
//		CLogicalLeftAntiSemiApplyNotIn.h
//
//	@doc:
//		Logical Left Anti Semi Apply operator used in NOT IN/ALL subqueries
//---------------------------------------------------------------------------
#ifndef SPQOPT_CLogicalLeftAntiSemiApplyNotIn_H
#define SPQOPT_CLogicalLeftAntiSemiApplyNotIn_H

#include "spqos/base.h"

#include "spqopt/operators/CExpressionHandle.h"
#include "spqopt/operators/CLogicalLeftAntiSemiApply.h"

namespace spqopt
{
//---------------------------------------------------------------------------
//	@class:
//		CLogicalLeftAntiSemiApplyNotIn
//
//	@doc:
//		Logical Apply operator used in NOT IN/ALL subqueries
//
//---------------------------------------------------------------------------
class CLogicalLeftAntiSemiApplyNotIn : public CLogicalLeftAntiSemiApply
{
private:
	// private copy ctor
	CLogicalLeftAntiSemiApplyNotIn(const CLogicalLeftAntiSemiApplyNotIn &);

public:
	// ctor
	explicit CLogicalLeftAntiSemiApplyNotIn(CMemoryPool *mp)
		: CLogicalLeftAntiSemiApply(mp)
	{
	}

	// ctor
	CLogicalLeftAntiSemiApplyNotIn(CMemoryPool *mp, CColRefArray *pdrspqcrInner,
								   EOperatorId eopidOriginSubq)
		: CLogicalLeftAntiSemiApply(mp, pdrspqcrInner, eopidOriginSubq)
	{
	}

	// dtor
	virtual ~CLogicalLeftAntiSemiApplyNotIn()
	{
	}

	// ident accessors
	virtual EOperatorId
	Eopid() const
	{
		return EopLogicalLeftAntiSemiApplyNotIn;
	}

	// return a string for operator name
	virtual const CHAR *
	SzId() const
	{
		return "CLogicalLeftAntiSemiApplyNotIn";
	}

	//-------------------------------------------------------------------------------------
	// Transformations
	//-------------------------------------------------------------------------------------

	// candidate set of xforms
	virtual CXformSet *PxfsCandidates(CMemoryPool *mp) const;

	//-------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------

	// return a copy of the operator with remapped columns
	virtual COperator *PopCopyWithRemappedColumns(
		CMemoryPool *mp, UlongToColRefMap *colref_mapping, BOOL must_exist);

	// conversion function
	static CLogicalLeftAntiSemiApplyNotIn *
	PopConvert(COperator *pop)
	{
		SPQOS_ASSERT(NULL != pop);
		SPQOS_ASSERT(EopLogicalLeftAntiSemiApplyNotIn == pop->Eopid());

		return dynamic_cast<CLogicalLeftAntiSemiApplyNotIn *>(pop);
	}

};	// class CLogicalLeftAntiSemiApplyNotIn

}  // namespace spqopt


#endif	// !SPQOPT_CLogicalLeftAntiSemiApplyNotIn_H

// EOF
