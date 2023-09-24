//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal Inc..
//
//	@filename:
//		CXformImplementLeftSemiCorrelatedApply.h
//
//	@doc:
//		Transform left semi correlated apply to physical left semi
//		correlated join
//---------------------------------------------------------------------------
#ifndef SPQOPT_CXformImplementLeftSemiCorrelatedApply_H
#define SPQOPT_CXformImplementLeftSemiCorrelatedApply_H

#include "spqos/base.h"

#include "spqopt/operators/CLogicalLeftSemiCorrelatedApply.h"
#include "spqopt/operators/CPhysicalCorrelatedLeftSemiNLJoin.h"
#include "spqopt/xforms/CXformImplementCorrelatedApply.h"

namespace spqopt
{
using namespace spqos;

//-------------------------------------------------------------------------
//	@class:
//		CXformImplementLeftSemiCorrelatedApply
//
//	@doc:
//		Transform left semi correlated apply to physical left semi
//		correlated join
//
//-------------------------------------------------------------------------
class CXformImplementLeftSemiCorrelatedApply
	: public CXformImplementCorrelatedApply<CLogicalLeftSemiCorrelatedApply,
											CPhysicalCorrelatedLeftSemiNLJoin>
{
private:
	// private copy ctor
	CXformImplementLeftSemiCorrelatedApply(
		const CXformImplementLeftSemiCorrelatedApply &);

public:
	// ctor
	explicit CXformImplementLeftSemiCorrelatedApply(CMemoryPool *mp)
		: CXformImplementCorrelatedApply<CLogicalLeftSemiCorrelatedApply,
										 CPhysicalCorrelatedLeftSemiNLJoin>(mp)
	{
	}

	// dtor
	virtual ~CXformImplementLeftSemiCorrelatedApply()
	{
	}

	// ident accessors
	virtual EXformId
	Exfid() const
	{
		return ExfImplementLeftSemiCorrelatedApply;
	}

	virtual const CHAR *
	SzId() const
	{
		return "CXformImplementLeftSemiCorrelatedApply";
	}

};	// class CXformImplementLeftSemiCorrelatedApply

}  // namespace spqopt

#endif	// !SPQOPT_CXformImplementLeftSemiCorrelatedApply_H

// EOF
