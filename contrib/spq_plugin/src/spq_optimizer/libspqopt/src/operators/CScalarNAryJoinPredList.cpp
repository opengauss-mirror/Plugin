//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2019 Pivotal Inc.
//
//	@filename:
//		CScalarNAryJoinPredList.cpp
//
//	@doc:
//		Join predicate list for NAry joins with some non-inner joins
//---------------------------------------------------------------------------

#include "spqopt/operators/CScalarNAryJoinPredList.h"

#include "spqos/base.h"


using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CScalarNAryJoinPredList::CScalarNAryJoinPredList
//
//	@doc:
//		ctor
//
//---------------------------------------------------------------------------
CScalarNAryJoinPredList::CScalarNAryJoinPredList(CMemoryPool *mp) : CScalar(mp)
{
}


//---------------------------------------------------------------------------
//	@function:
//		CScalarNAryJoinPredList::Matches
//
//	@doc:
//		Match function on operator level
//
//---------------------------------------------------------------------------
BOOL
CScalarNAryJoinPredList::Matches(COperator *pop) const
{
	return (pop->Eopid() == Eopid());
}


//---------------------------------------------------------------------------
//	@function:
//		CScalarNAryJoinPredList::FInputOrderSensitive
//
//	@doc:
//		Join predicate lists are sensitive to order
//
//---------------------------------------------------------------------------
BOOL
CScalarNAryJoinPredList::FInputOrderSensitive() const
{
	return true;
}


// EOF
