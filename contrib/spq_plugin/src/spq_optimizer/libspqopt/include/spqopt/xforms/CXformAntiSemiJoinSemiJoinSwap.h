//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CXformAntiSemiJoinSemiJoinSwap.h
//
//	@doc:
//		Swap cascaded anti semi-join and semi-join
//---------------------------------------------------------------------------
#ifndef SPQOPT_CXformAntiSemiJoinSemiJoinSwap_H
#define SPQOPT_CXformAntiSemiJoinSemiJoinSwap_H

#include "spqos/base.h"

#include "spqopt/operators/CLogicalLeftAntiSemiJoin.h"
#include "spqopt/operators/CLogicalLeftSemiJoin.h"
#include "spqopt/xforms/CXformJoinSwap.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CXformAntiSemiJoinSemiJoinSwap
//
//	@doc:
//		Swap cascaded anti semi-join and semi-join
//
//---------------------------------------------------------------------------
class CXformAntiSemiJoinSemiJoinSwap
	: public CXformJoinSwap<CLogicalLeftAntiSemiJoin, CLogicalLeftSemiJoin>
{
private:
	// private copy ctor
	CXformAntiSemiJoinSemiJoinSwap(const CXformAntiSemiJoinSemiJoinSwap &);

public:
	// ctor
	explicit CXformAntiSemiJoinSemiJoinSwap(CMemoryPool *mp)
		: CXformJoinSwap<CLogicalLeftAntiSemiJoin, CLogicalLeftSemiJoin>(mp)
	{
	}

	// dtor
	virtual ~CXformAntiSemiJoinSemiJoinSwap()
	{
	}

	// Compatibility function
	virtual BOOL
	FCompatible(CXform::EXformId exfid)
	{
		return ExfSemiJoinAntiSemiJoinSwap != exfid;
	}

	// ident accessors
	virtual EXformId
	Exfid() const
	{
		return ExfAntiSemiJoinSemiJoinSwap;
	}

	virtual const CHAR *
	SzId() const
	{
		return "CXformAntiSemiJoinSemiJoinSwap";
	}

};	// class CXformAntiSemiJoinSemiJoinSwap

}  // namespace spqopt

#endif	// !SPQOPT_CXformAntiSemiJoinSemiJoinSwap_H

// EOF
