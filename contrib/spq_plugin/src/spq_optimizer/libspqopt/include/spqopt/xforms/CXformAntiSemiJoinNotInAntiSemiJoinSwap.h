//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 EMC Corp.
//
//	@filename:
//		CXformAntiSemiJoinNotInAntiSemiJoinSwap.h
//
//	@doc:
//		Swap cascaded anti semi-join (NotIn) and anti semi-join
//---------------------------------------------------------------------------
#ifndef SPQOPT_CXformAntiSemiJoinNotInAntiSemiJoinSwap_H
#define SPQOPT_CXformAntiSemiJoinNotInAntiSemiJoinSwap_H

#include "spqos/base.h"

#include "spqopt/operators/CLogicalLeftAntiSemiJoin.h"
#include "spqopt/operators/CLogicalLeftAntiSemiJoinNotIn.h"
#include "spqopt/xforms/CXformJoinSwap.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CXformAntiSemiJoinNotInAntiSemiJoinSwap
//
//	@doc:
//		Swap cascaded anti semi-join (NotIn) and anti semi-join
//
//---------------------------------------------------------------------------
class CXformAntiSemiJoinNotInAntiSemiJoinSwap
	: public CXformJoinSwap<CLogicalLeftAntiSemiJoinNotIn,
							CLogicalLeftAntiSemiJoin>
{
private:
	// private copy ctor
	CXformAntiSemiJoinNotInAntiSemiJoinSwap(
		const CXformAntiSemiJoinNotInAntiSemiJoinSwap &);

public:
	// ctor
	explicit CXformAntiSemiJoinNotInAntiSemiJoinSwap(CMemoryPool *mp)
		: CXformJoinSwap<CLogicalLeftAntiSemiJoinNotIn,
						 CLogicalLeftAntiSemiJoin>(mp)
	{
	}

	// dtor
	virtual ~CXformAntiSemiJoinNotInAntiSemiJoinSwap()
	{
	}

	// Compatibility function
	virtual BOOL
	FCompatible(CXform::EXformId exfid)
	{
		return ExfAntiSemiJoinAntiSemiJoinNotInSwap != exfid;
	}

	// ident accessors
	virtual EXformId
	Exfid() const
	{
		return ExfAntiSemiJoinNotInAntiSemiJoinSwap;
	}

	virtual const CHAR *
	SzId() const
	{
		return "CXformAntiSemiJoinNotInAntiSemiJoinSwap";
	}

};	// class CXformAntiSemiJoinNotInAntiSemiJoinSwap

}  // namespace spqopt

#endif	// !SPQOPT_CXformAntiSemiJoinNotInAntiSemiJoinSwap_H

// EOF
