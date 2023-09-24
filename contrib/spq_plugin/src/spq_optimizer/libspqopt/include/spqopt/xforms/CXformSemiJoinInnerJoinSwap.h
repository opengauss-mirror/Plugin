//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CXformSemiJoinInnerJoinSwap.h
//
//	@doc:
//		Swap cascaded semi-join and inner join
//---------------------------------------------------------------------------
#ifndef SPQOPT_CXformSemiJoinInnerJoinSwap_H
#define SPQOPT_CXformSemiJoinInnerJoinSwap_H

#include "spqos/base.h"

#include "spqopt/operators/CLogicalInnerJoin.h"
#include "spqopt/operators/CLogicalLeftSemiJoin.h"
#include "spqopt/xforms/CXformJoinSwap.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CXformSemiJoinInnerJoinSwap
//
//	@doc:
//		Swap cascaded semi-join and inner join
//
//---------------------------------------------------------------------------
class CXformSemiJoinInnerJoinSwap
	: public CXformJoinSwap<CLogicalLeftSemiJoin, CLogicalInnerJoin>
{
private:
	// private copy ctor
	CXformSemiJoinInnerJoinSwap(const CXformSemiJoinInnerJoinSwap &);

public:
	// ctor
	explicit CXformSemiJoinInnerJoinSwap(CMemoryPool *mp)
		: CXformJoinSwap<CLogicalLeftSemiJoin, CLogicalInnerJoin>(mp)
	{
	}

	// dtor
	virtual ~CXformSemiJoinInnerJoinSwap()
	{
	}

	// ident accessors
	virtual EXformId
	Exfid() const
	{
		return ExfSemiJoinInnerJoinSwap;
	}

	virtual const CHAR *
	SzId() const
	{
		return "CXformSemiJoinInnerJoinSwap";
	}

};	// class CXformSemiJoinInnerJoinSwap

}  // namespace spqopt

#endif	// !SPQOPT_CXformSemiJoinInnerJoinSwap_H

// EOF
