//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CXformInnerJoinAntiSemiJoinSwap.h
//
//	@doc:
//		Swap cascaded inner join and anti semi-join
//---------------------------------------------------------------------------
#ifndef SPQOPT_CXformInnerJoinAntiSemiJoinSwap_H
#define SPQOPT_CXformInnerJoinAntiSemiJoinSwap_H

#include "spqos/base.h"

#include "spqopt/operators/CLogicalInnerJoin.h"
#include "spqopt/operators/CLogicalLeftAntiSemiJoin.h"
#include "spqopt/xforms/CXformJoinSwap.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CXformInnerJoinAntiSemiJoinSwap
//
//	@doc:
//		Swap cascaded inner join and anti semi-join
//
//---------------------------------------------------------------------------
class CXformInnerJoinAntiSemiJoinSwap
	: public CXformJoinSwap<CLogicalInnerJoin, CLogicalLeftAntiSemiJoin>
{
private:
	// private copy ctor
	CXformInnerJoinAntiSemiJoinSwap(const CXformInnerJoinAntiSemiJoinSwap &);

public:
	// ctor
	explicit CXformInnerJoinAntiSemiJoinSwap(CMemoryPool *mp)
		: CXformJoinSwap<CLogicalInnerJoin, CLogicalLeftAntiSemiJoin>(mp)
	{
	}

	// dtor
	virtual ~CXformInnerJoinAntiSemiJoinSwap()
	{
	}

	// Compatibility function
	virtual BOOL
	FCompatible(CXform::EXformId exfid)
	{
		return ExfAntiSemiJoinInnerJoinSwap != exfid;
	}

	// ident accessors
	virtual EXformId
	Exfid() const
	{
		return ExfInnerJoinAntiSemiJoinSwap;
	}

	virtual const CHAR *
	SzId() const
	{
		return "CXformInnerJoinAntiSemiJoinSwap";
	}

};	// class CXformInnerJoinAntiSemiJoinSwap

}  // namespace spqopt

#endif	// !SPQOPT_CXformInnerJoinAntiSemiJoinSwap_H

// EOF
