//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CXformAntiSemiJoinAntiSemiJoinSwap.h
//
//	@doc:
//		Swap two cascaded anti semi-joins
//---------------------------------------------------------------------------
#ifndef SPQOPT_CXformAntiSemiJoinAntiSemiJoinSwap_H
#define SPQOPT_CXformAntiSemiJoinAntiSemiJoinSwap_H

#include "spqos/base.h"

#include "spqopt/operators/CLogicalLeftAntiSemiJoin.h"
#include "spqopt/xforms/CXformJoinSwap.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CXformAntiSemiJoinAntiSemiJoinSwap
//
//	@doc:
//		Swap two cascaded anti semi-joins
//
//---------------------------------------------------------------------------
class CXformAntiSemiJoinAntiSemiJoinSwap
	: public CXformJoinSwap<CLogicalLeftAntiSemiJoin, CLogicalLeftAntiSemiJoin>
{
private:
	// private copy ctor
	CXformAntiSemiJoinAntiSemiJoinSwap(
		const CXformAntiSemiJoinAntiSemiJoinSwap &);

public:
	// ctor
	explicit CXformAntiSemiJoinAntiSemiJoinSwap(CMemoryPool *mp)
		: CXformJoinSwap<CLogicalLeftAntiSemiJoin, CLogicalLeftAntiSemiJoin>(mp)
	{
	}

	// dtor
	virtual ~CXformAntiSemiJoinAntiSemiJoinSwap()
	{
	}

	// Compatibility function
	virtual BOOL
	FCompatible(CXform::EXformId exfid)
	{
		return ExfAntiSemiJoinAntiSemiJoinSwap != exfid;
	}

	// ident accessors
	virtual EXformId
	Exfid() const
	{
		return ExfAntiSemiJoinAntiSemiJoinSwap;
	}

	virtual const CHAR *
	SzId() const
	{
		return "CXformAntiSemiJoinAntiSemiJoinSwap";
	}

};	// class CXformAntiSemiJoinAntiSemiJoinSwap

}  // namespace spqopt

#endif	// !SPQOPT_CXformSemiJoinSemiJoinSwap_H

// EOF
