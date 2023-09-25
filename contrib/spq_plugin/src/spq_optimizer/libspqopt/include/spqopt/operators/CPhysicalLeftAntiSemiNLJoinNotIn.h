//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 EMC Corp.
//
//	@filename:
//		CPhysicalLeftAntiSemiNLJoinNotIn.h
//
//	@doc:
//		Left anti semi nested-loops join operator with NotIn semantics
//---------------------------------------------------------------------------
#ifndef SPQOPT_CPhysicalLeftAntiSemiNLJoinNotIn_H
#define SPQOPT_CPhysicalLeftAntiSemiNLJoinNotIn_H

#include "spqos/base.h"

#include "spqopt/operators/CPhysicalLeftAntiSemiNLJoin.h"

namespace spqopt
{
//---------------------------------------------------------------------------
//	@class:
//		CPhysicalLeftAntiSemiNLJoinNotIn
//
//	@doc:
//		Left anti semi nested-loops join operator with NotIn semantics
//
//---------------------------------------------------------------------------
class CPhysicalLeftAntiSemiNLJoinNotIn : public CPhysicalLeftAntiSemiNLJoin
{
private:
	// private copy ctor
	CPhysicalLeftAntiSemiNLJoinNotIn(const CPhysicalLeftAntiSemiNLJoinNotIn &);

public:
	// ctor
	explicit CPhysicalLeftAntiSemiNLJoinNotIn(CMemoryPool *mp)
		: CPhysicalLeftAntiSemiNLJoin(mp)
	{
	}

	// ident accessors
	virtual EOperatorId
	Eopid() const
	{
		return EopPhysicalLeftAntiSemiNLJoinNotIn;
	}

	// return a string for operator name
	virtual const CHAR *
	SzId() const
	{
		return "CPhysicalLeftAntiSemiNLJoinNotIn";
	}

	// conversion function
	static CPhysicalLeftAntiSemiNLJoinNotIn *
	PopConvert(COperator *pop)
	{
		SPQOS_ASSERT(EopPhysicalLeftAntiSemiNLJoinNotIn == pop->Eopid());

		return dynamic_cast<CPhysicalLeftAntiSemiNLJoinNotIn *>(pop);
	}

};	// class CPhysicalLeftAntiSemiNLJoinNotIn

}  // namespace spqopt

#endif	// !SPQOPT_CPhysicalLeftAntiSemiNLJoinNotIn_H

// EOF
