//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 EMC Corp.
//
//	@filename:
//		CXformAntiSemiJoinNotInInnerJoinSwap.h
//
//	@doc:
//		Swap cascaded anti semi-join (NotIn) and inner join
//---------------------------------------------------------------------------
#ifndef SPQOPT_CXformAntiSemiJoinNotInInnerJoinSwap_H
#define SPQOPT_CXformAntiSemiJoinNotInInnerJoinSwap_H

#include "spqos/base.h"

#include "spqopt/operators/CLogicalInnerJoin.h"
#include "spqopt/operators/CLogicalLeftAntiSemiJoinNotIn.h"
#include "spqopt/xforms/CXformJoinSwap.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CXformAntiSemiJoinNotInInnerJoinSwap
//
//	@doc:
//		Swap cascaded anti semi-join (NotIn) and inner join
//
//---------------------------------------------------------------------------
class CXformAntiSemiJoinNotInInnerJoinSwap
	: public CXformJoinSwap<CLogicalLeftAntiSemiJoinNotIn, CLogicalInnerJoin>
{
private:
	// private copy ctor
	CXformAntiSemiJoinNotInInnerJoinSwap(
		const CXformAntiSemiJoinNotInInnerJoinSwap &);

public:
	// ctor
	explicit CXformAntiSemiJoinNotInInnerJoinSwap(CMemoryPool *mp)
		: CXformJoinSwap<CLogicalLeftAntiSemiJoinNotIn, CLogicalInnerJoin>(mp)
	{
	}

	// dtor
	virtual ~CXformAntiSemiJoinNotInInnerJoinSwap()
	{
	}

	// Compatibility function
	virtual BOOL
	FCompatible(CXform::EXformId exfid)
	{
		return ExfInnerJoinAntiSemiJoinNotInSwap != exfid;
	}

	// ident accessors
	virtual EXformId
	Exfid() const
	{
		return ExfAntiSemiJoinNotInInnerJoinSwap;
	}

	virtual const CHAR *
	SzId() const
	{
		return "CXformAntiSemiJoinNotInInnerJoinSwap";
	}

};	// class CXformAntiSemiJoinNotInInnerJoinSwap

}  // namespace spqopt

#endif	// !SPQOPT_CXformAntiSemiJoinNotInInnerJoinSwap_H

// EOF
