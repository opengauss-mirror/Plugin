//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CXformInnerJoinSemiJoinSwap.h
//
//	@doc:
//		Swap cascaded inner join and semi-join
//---------------------------------------------------------------------------
#ifndef SPQOPT_CXformInnerJoinSemiJoinSwap_H
#define SPQOPT_CXformInnerJoinSemiJoinSwap_H

#include "spqos/base.h"

#include "spqopt/operators/CLogicalInnerJoin.h"
#include "spqopt/operators/CLogicalLeftSemiJoin.h"
#include "spqopt/xforms/CXformJoinSwap.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CXformInnerJoinSemiJoinSwap
//
//	@doc:
//		Swap cascaded inner join and semi-join
//
//---------------------------------------------------------------------------
class CXformInnerJoinSemiJoinSwap
	: public CXformJoinSwap<CLogicalInnerJoin, CLogicalLeftSemiJoin>
{
private:
	// private copy ctor
	CXformInnerJoinSemiJoinSwap(const CXformInnerJoinSemiJoinSwap &);

public:
	// ctor
	explicit CXformInnerJoinSemiJoinSwap(CMemoryPool *mp)
		: CXformJoinSwap<CLogicalInnerJoin, CLogicalLeftSemiJoin>(mp)
	{
	}

	// dtor
	virtual ~CXformInnerJoinSemiJoinSwap()
	{
	}

	// Compatibility function
	virtual BOOL
	FCompatible(CXform::EXformId exfid)
	{
		return ExfSemiJoinInnerJoinSwap != exfid;
	}

	// ident accessors
	virtual EXformId
	Exfid() const
	{
		return ExfInnerJoinSemiJoinSwap;
	}

	virtual const CHAR *
	SzId() const
	{
		return "CXformInnerJoinSemiJoinSwap";
	}

};	// class CXformInnerJoinSemiJoinSwap

}  // namespace spqopt

#endif	// !SPQOPT_CXformInnerJoinSemiJoinSwap_H

// EOF
