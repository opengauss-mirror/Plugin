//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CXformSemiJoinSemiJoinSwap.h
//
//	@doc:
//		Swap two cascaded semi-joins
//---------------------------------------------------------------------------
#ifndef SPQOPT_CXformSemiJoinSemiJoinSwap_H
#define SPQOPT_CXformSemiJoinSemiJoinSwap_H

#include "spqos/base.h"

#include "spqopt/operators/CLogicalLeftSemiJoin.h"
#include "spqopt/xforms/CXformJoinSwap.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CXformSemiJoinSemiJoinSwap
//
//	@doc:
//		Swap two cascaded semi-joins
//
//---------------------------------------------------------------------------
class CXformSemiJoinSemiJoinSwap
	: public CXformJoinSwap<CLogicalLeftSemiJoin, CLogicalLeftSemiJoin>
{
private:
	// private copy ctor
	CXformSemiJoinSemiJoinSwap(const CXformSemiJoinSemiJoinSwap &);

public:
	// ctor
	explicit CXformSemiJoinSemiJoinSwap(CMemoryPool *mp)
		: CXformJoinSwap<CLogicalLeftSemiJoin, CLogicalLeftSemiJoin>(mp)
	{
	}

	// dtor
	virtual ~CXformSemiJoinSemiJoinSwap()
	{
	}

	// Compatibility function
	virtual BOOL
	FCompatible(CXform::EXformId exfid)
	{
		return ExfSemiJoinSemiJoinSwap != exfid;
	}

	// ident accessors
	virtual EXformId
	Exfid() const
	{
		return ExfSemiJoinSemiJoinSwap;
	}

	virtual const CHAR *
	SzId() const
	{
		return "CXformSemiJoinSemiJoinSwap";
	}

};	// class CXformSemiJoinSemiJoinSwap

}  // namespace spqopt

#endif	// !SPQOPT_CXformSemiJoinSemiJoinSwap_H

// EOF
