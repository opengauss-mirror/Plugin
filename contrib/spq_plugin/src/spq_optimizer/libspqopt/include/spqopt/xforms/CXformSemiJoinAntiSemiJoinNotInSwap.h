//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 EMC Corp.
//
//	@filename:
//		CXformSemiJoinAntiSemiJoinNotInSwap.h
//
//	@doc:
//		Swap cascaded semi-join and anti semi-join with NotIn semantics
//---------------------------------------------------------------------------
#ifndef SPQOPT_CXformSemiJoinAntiSemiJoinNotInSwap_H
#define SPQOPT_CXformSemiJoinAntiSemiJoinNotInSwap_H

#include "spqos/base.h"

#include "spqopt/operators/CLogicalLeftAntiSemiJoinNotIn.h"
#include "spqopt/operators/CLogicalLeftSemiJoin.h"
#include "spqopt/xforms/CXformJoinSwap.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CXformSemiJoinAntiSemiJoinNotInSwap
//
//	@doc:
//		Swap cascaded semi-join and anti semi-join with NotIn semantics
//
//---------------------------------------------------------------------------
class CXformSemiJoinAntiSemiJoinNotInSwap
	: public CXformJoinSwap<CLogicalLeftSemiJoin, CLogicalLeftAntiSemiJoinNotIn>
{
private:
	// private copy ctor
	CXformSemiJoinAntiSemiJoinNotInSwap(
		const CXformSemiJoinAntiSemiJoinNotInSwap &);

public:
	// ctor
	explicit CXformSemiJoinAntiSemiJoinNotInSwap(CMemoryPool *mp)
		: CXformJoinSwap<CLogicalLeftSemiJoin, CLogicalLeftAntiSemiJoinNotIn>(
			  mp)
	{
	}

	// dtor
	virtual ~CXformSemiJoinAntiSemiJoinNotInSwap()
	{
	}

	// Compatibility function
	virtual BOOL
	FCompatible(CXform::EXformId exfid)
	{
		return ExfAntiSemiJoinNotInSemiJoinSwap != exfid;
	}

	// ident accessors
	virtual EXformId
	Exfid() const
	{
		return ExfSemiJoinAntiSemiJoinNotInSwap;
	}

	virtual const CHAR *
	SzId() const
	{
		return "CXformSemiJoinAntiSemiJoinNotInSwap";
	}

};	// class CXformSemiJoinAntiSemiJoinNotInSwap

}  // namespace spqopt

#endif	// !SPQOPT_CXformSemiJoinAntiSemiJoinNotInSwap_H

// EOF
