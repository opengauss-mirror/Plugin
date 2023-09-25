//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CXformImplementLeftOuterCorrelatedApply.h
//
//	@doc:
//		Transform LeftOuter correlated apply to physical LeftOuter correlated
//		apply
//---------------------------------------------------------------------------
#ifndef SPQOPT_CXformImplementLeftOuterCorrelatedApply_H
#define SPQOPT_CXformImplementLeftOuterCorrelatedApply_H

#include "spqos/base.h"

#include "spqopt/operators/CLogicalLeftOuterCorrelatedApply.h"
#include "spqopt/operators/CPhysicalCorrelatedLeftOuterNLJoin.h"
#include "spqopt/xforms/CXformImplementCorrelatedApply.h"

namespace spqopt
{
using namespace spqos;

//-------------------------------------------------------------------------
//	@class:
//		CXformImplementLeftOuterCorrelatedApply
//
//	@doc:
//		Transform LeftOuter correlated apply to physical LeftOuter correlated
//		apply
//
//-------------------------------------------------------------------------
class CXformImplementLeftOuterCorrelatedApply
	: public CXformImplementCorrelatedApply<CLogicalLeftOuterCorrelatedApply,
											CPhysicalCorrelatedLeftOuterNLJoin>
{
private:
	// private copy ctor
	CXformImplementLeftOuterCorrelatedApply(
		const CXformImplementLeftOuterCorrelatedApply &);

public:
	// ctor
	explicit CXformImplementLeftOuterCorrelatedApply(CMemoryPool *mp)
		: CXformImplementCorrelatedApply<CLogicalLeftOuterCorrelatedApply,
										 CPhysicalCorrelatedLeftOuterNLJoin>(mp)
	{
	}

	// dtor
	virtual ~CXformImplementLeftOuterCorrelatedApply()
	{
	}

	// ident accessors
	virtual EXformId
	Exfid() const
	{
		return ExfImplementLeftOuterCorrelatedApply;
	}

	virtual const CHAR *
	SzId() const
	{
		return "CXformImplementLeftOuterCorrelatedApply";
	}

};	// class CXformImplementLeftOuterCorrelatedApply

}  // namespace spqopt

#endif	// !SPQOPT_CXformImplementLeftOuterCorrelatedApply_H

// EOF
