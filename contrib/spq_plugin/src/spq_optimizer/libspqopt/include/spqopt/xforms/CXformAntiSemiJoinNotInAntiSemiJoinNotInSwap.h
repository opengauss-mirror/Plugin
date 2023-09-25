//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 EMC Corp.
//
//	@filename:
//		CXformAntiSemiJoinNotInAntiSemiJoinNotInSwap.h
//
//	@doc:
//		Swap two cascaded anti semi-joins with NotIn semantics
//---------------------------------------------------------------------------
#ifndef SPQOPT_CXformAntiSemiJoinNotInAntiSemiJoinNotInSwap_H
#define SPQOPT_CXformAntiSemiJoinNotInAntiSemiJoinNotInSwap_H

#include "spqos/base.h"

#include "spqopt/operators/CLogicalLeftAntiSemiJoinNotIn.h"
#include "spqopt/xforms/CXformJoinSwap.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CXformAntiSemiJoinNotInAntiSemiJoinNotInSwap
//
//	@doc:
//		Swap two cascaded anti semi-joins with NotIn semantics
//
//---------------------------------------------------------------------------
class CXformAntiSemiJoinNotInAntiSemiJoinNotInSwap
	: public CXformJoinSwap<CLogicalLeftAntiSemiJoinNotIn,
							CLogicalLeftAntiSemiJoinNotIn>
{
private:
	// private copy ctor
	CXformAntiSemiJoinNotInAntiSemiJoinNotInSwap(
		const CXformAntiSemiJoinNotInAntiSemiJoinNotInSwap &);

public:
	// ctor
	explicit CXformAntiSemiJoinNotInAntiSemiJoinNotInSwap(CMemoryPool *mp)
		: CXformJoinSwap<CLogicalLeftAntiSemiJoinNotIn,
						 CLogicalLeftAntiSemiJoinNotIn>(mp)
	{
	}

	// dtor
	virtual ~CXformAntiSemiJoinNotInAntiSemiJoinNotInSwap()
	{
	}

	// Compatibility function
	virtual BOOL
	FCompatible(CXform::EXformId exfid)
	{
		return ExfAntiSemiJoinNotInAntiSemiJoinNotInSwap != exfid;
	}

	// ident accessors
	virtual EXformId
	Exfid() const
	{
		return ExfAntiSemiJoinNotInAntiSemiJoinNotInSwap;
	}

	virtual const CHAR *
	SzId() const
	{
		return "CXformAntiSemiJoinNotInAntiSemiJoinNotInSwap";
	}

};	// class CXformAntiSemiJoinNotInAntiSemiJoinNotInSwap

}  // namespace spqopt

#endif	// !SPQOPT_CXformAntiSemiJoinNotInAntiSemiJoinNotInSwap_H

// EOF
