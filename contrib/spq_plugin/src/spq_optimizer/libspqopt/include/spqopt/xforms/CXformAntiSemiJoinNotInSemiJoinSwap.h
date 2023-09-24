//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 EMC Corp.
//
//	@filename:
//		CXformAntiSemiJoinNotInSemiJoinSwap.h
//
//	@doc:
//		Swap cascaded anti semi-join (NotIn) and semi-join
//---------------------------------------------------------------------------
#ifndef SPQOPT_CXformAntiSemiJoinNotInSemiJoinSwap_H
#define SPQOPT_CXformAntiSemiJoinNotInSemiJoinSwap_H

#include "spqos/base.h"

#include "spqopt/operators/CLogicalLeftAntiSemiJoinNotIn.h"
#include "spqopt/operators/CLogicalLeftSemiJoin.h"
#include "spqopt/xforms/CXformJoinSwap.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CXformAntiSemiJoinNotInSemiJoinSwap
//
//	@doc:
//		Swap cascaded anti semi-join (NotIn) and semi-join
//
//---------------------------------------------------------------------------
class CXformAntiSemiJoinNotInSemiJoinSwap
	: public CXformJoinSwap<CLogicalLeftAntiSemiJoinNotIn, CLogicalLeftSemiJoin>
{
private:
	// private copy ctor
	CXformAntiSemiJoinNotInSemiJoinSwap(
		const CXformAntiSemiJoinNotInSemiJoinSwap &);

public:
	// ctor
	explicit CXformAntiSemiJoinNotInSemiJoinSwap(CMemoryPool *mp)
		: CXformJoinSwap<CLogicalLeftAntiSemiJoinNotIn, CLogicalLeftSemiJoin>(
			  mp)
	{
	}

	// dtor
	virtual ~CXformAntiSemiJoinNotInSemiJoinSwap()
	{
	}

	// Compatibility function
	virtual BOOL
	FCompatible(CXform::EXformId exfid)
	{
		return ExfSemiJoinAntiSemiJoinNotInSwap != exfid;
	}

	// ident accessors
	virtual EXformId
	Exfid() const
	{
		return ExfAntiSemiJoinNotInSemiJoinSwap;
	}

	virtual const CHAR *
	SzId() const
	{
		return "CXformAntiSemiJoinNotInSemiJoinSwap";
	}

};	// class CXformAntiSemiJoinNotInSemiJoinSwap

}  // namespace spqopt

#endif	// !SPQOPT_CXformAntiSemiJoinNotInSemiJoinSwap_H

// EOF
