//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CXformAntiSemiJoinInnerJoinSwap.h
//
//	@doc:
//		Swap cascaded anti semi-join and inner join
//---------------------------------------------------------------------------
#ifndef SPQOPT_CXformAntiSemiJoinInnerJoinSwap_H
#define SPQOPT_CXformAntiSemiJoinInnerJoinSwap_H

#include "spqos/base.h"

#include "spqopt/operators/CLogicalInnerJoin.h"
#include "spqopt/operators/CLogicalLeftAntiSemiJoin.h"
#include "spqopt/xforms/CXformJoinSwap.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CXformAntiSemiJoinInnerJoinSwap
//
//	@doc:
//		Swap cascaded anti semi-join and inner join
//
//---------------------------------------------------------------------------
class CXformAntiSemiJoinInnerJoinSwap
	: public CXformJoinSwap<CLogicalLeftAntiSemiJoin, CLogicalInnerJoin>
{
private:
	// private copy ctor
	CXformAntiSemiJoinInnerJoinSwap(const CXformAntiSemiJoinInnerJoinSwap &);

public:
	// ctor
	explicit CXformAntiSemiJoinInnerJoinSwap(CMemoryPool *mp)
		: CXformJoinSwap<CLogicalLeftAntiSemiJoin, CLogicalInnerJoin>(mp)
	{
	}

	// dtor
	virtual ~CXformAntiSemiJoinInnerJoinSwap()
	{
	}

	// Compatibility function
	virtual BOOL
	FCompatible(CXform::EXformId exfid)
	{
		return ExfInnerJoinAntiSemiJoinSwap != exfid;
	}

	// ident accessors
	virtual EXformId
	Exfid() const
	{
		return ExfAntiSemiJoinInnerJoinSwap;
	}

	virtual const CHAR *
	SzId() const
	{
		return "CXformAntiSemiJoinInnerJoinSwap";
	}

};	// class CXformAntiSemiJoinInnerJoinSwap

}  // namespace spqopt

#endif	// !SPQOPT_CXformAntiSemiJoinInnerJoinSwap_H

// EOF
