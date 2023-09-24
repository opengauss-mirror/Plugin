//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 EMC Corp.
//
//	@filename:
//		CXformInnerJoinAntiSemiJoinNotInSwap.h
//
//	@doc:
//		Swap cascaded inner join and anti semi-join with NotIn semantics
//---------------------------------------------------------------------------
#ifndef SPQOPT_CXformInnerJoinAntiSemiJoinNotInSwap_H
#define SPQOPT_CXformInnerJoinAntiSemiJoinNotInSwap_H

#include "spqos/base.h"

#include "spqopt/operators/CLogicalInnerJoin.h"
#include "spqopt/operators/CLogicalLeftAntiSemiJoinNotIn.h"
#include "spqopt/xforms/CXformJoinSwap.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CXformInnerJoinAntiSemiJoinNotInSwap
//
//	@doc:
//		Swap cascaded inner join and anti semi-join with NotIn semantics
//
//---------------------------------------------------------------------------
class CXformInnerJoinAntiSemiJoinNotInSwap
	: public CXformJoinSwap<CLogicalInnerJoin, CLogicalLeftAntiSemiJoinNotIn>
{
private:
	// private copy ctor
	CXformInnerJoinAntiSemiJoinNotInSwap(
		const CXformInnerJoinAntiSemiJoinNotInSwap &);

public:
	// ctor
	explicit CXformInnerJoinAntiSemiJoinNotInSwap(CMemoryPool *mp)
		: CXformJoinSwap<CLogicalInnerJoin, CLogicalLeftAntiSemiJoinNotIn>(mp)
	{
	}

	// dtor
	virtual ~CXformInnerJoinAntiSemiJoinNotInSwap()
	{
	}

	// Compatibility function
	virtual BOOL
	FCompatible(CXform::EXformId exfid)
	{
		return ExfAntiSemiJoinNotInInnerJoinSwap != exfid;
	}

	// ident accessors
	virtual EXformId
	Exfid() const
	{
		return ExfInnerJoinAntiSemiJoinNotInSwap;
	}

	virtual const CHAR *
	SzId() const
	{
		return "CXformInnerJoinAntiSemiJoinNotInSwap";
	}

};	// class CXformInnerJoinAntiSemiJoinNotInSwap

}  // namespace spqopt

#endif	// !SPQOPT_CXformInnerJoinAntiSemiJoinNotInSwap_H

// EOF
