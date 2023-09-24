//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CXformSemiJoinAntiSemiJoinSwap.h
//
//	@doc:
//		Swap cascaded semi-join and anti semi-join
//---------------------------------------------------------------------------
#ifndef SPQOPT_CXformSemiJoinAntiSemiJoinSwap_H
#define SPQOPT_CXformSemiJoinAntiSemiJoinSwap_H

#include "spqos/base.h"

#include "spqopt/operators/CLogicalLeftAntiSemiJoin.h"
#include "spqopt/operators/CLogicalLeftSemiJoin.h"
#include "spqopt/xforms/CXformJoinSwap.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CXformSemiJoinAntiSemiJoinSwap
//
//	@doc:
//		Swap cascaded semi-join and anti semi-join
//
//---------------------------------------------------------------------------
class CXformSemiJoinAntiSemiJoinSwap
	: public CXformJoinSwap<CLogicalLeftSemiJoin, CLogicalLeftAntiSemiJoin>
{
private:
	// private copy ctor
	CXformSemiJoinAntiSemiJoinSwap(const CXformSemiJoinAntiSemiJoinSwap &);

public:
	// ctor
	explicit CXformSemiJoinAntiSemiJoinSwap(CMemoryPool *mp)
		: CXformJoinSwap<CLogicalLeftSemiJoin, CLogicalLeftAntiSemiJoin>(mp)
	{
	}

	// dtor
	virtual ~CXformSemiJoinAntiSemiJoinSwap()
	{
	}

	// Compatibility function
	virtual BOOL
	FCompatible(CXform::EXformId exfid)
	{
		return ExfAntiSemiJoinSemiJoinSwap != exfid;
	}

	// ident accessors
	virtual EXformId
	Exfid() const
	{
		return ExfSemiJoinAntiSemiJoinSwap;
	}

	virtual const CHAR *
	SzId() const
	{
		return "CXformSemiJoinAntiSemiJoinSwap";
	}

};	// class CXformSemiJoinAntiSemiJoinSwap

}  // namespace spqopt

#endif	// !SPQOPT_CXformSemiJoinAntiSemiJoinSwap_H

// EOF
