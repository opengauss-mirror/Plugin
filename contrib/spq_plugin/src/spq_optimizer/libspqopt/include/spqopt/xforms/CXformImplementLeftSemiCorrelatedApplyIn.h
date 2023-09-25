//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal Inc..
//
//	@filename:
//		CXformImplementLeftSemiCorrelatedApplyIn.h
//
//	@doc:
//		Transform left semi correlated apply with IN/ANY semantics
//		to physical left semi correlated join
//---------------------------------------------------------------------------
#ifndef SPQOPT_CXformImplementLeftSemiCorrelatedApplyIn_H
#define SPQOPT_CXformImplementLeftSemiCorrelatedApplyIn_H

#include "spqos/base.h"

#include "spqopt/operators/CLogicalLeftSemiCorrelatedApplyIn.h"
#include "spqopt/operators/CPhysicalCorrelatedInLeftSemiNLJoin.h"
#include "spqopt/xforms/CXformImplementCorrelatedApply.h"

namespace spqopt
{
using namespace spqos;

//-------------------------------------------------------------------------
//	@class:
//		CXformImplementLeftSemiCorrelatedApplyIn
//
//	@doc:
//		Transform left semi correlated apply with IN/ANY semantics
//		to physical left semi correlated join
//
//-------------------------------------------------------------------------
class CXformImplementLeftSemiCorrelatedApplyIn
	: public CXformImplementCorrelatedApply<CLogicalLeftSemiCorrelatedApplyIn,
											CPhysicalCorrelatedInLeftSemiNLJoin>
{
private:
	// private copy ctor
	CXformImplementLeftSemiCorrelatedApplyIn(
		const CXformImplementLeftSemiCorrelatedApplyIn &);

public:
	// ctor
	explicit CXformImplementLeftSemiCorrelatedApplyIn(CMemoryPool *mp)
		: CXformImplementCorrelatedApply<CLogicalLeftSemiCorrelatedApplyIn,
										 CPhysicalCorrelatedInLeftSemiNLJoin>(
			  mp)
	{
	}

	// dtor
	virtual ~CXformImplementLeftSemiCorrelatedApplyIn()
	{
	}

	// ident accessors
	virtual EXformId
	Exfid() const
	{
		return ExfImplementLeftSemiCorrelatedApplyIn;
	}

	virtual const CHAR *
	SzId() const
	{
		return "CXformImplementLeftSemiCorrelatedApplyIn";
	}

};	// class CXformImplementLeftSemiCorrelatedApplyIn

}  // namespace spqopt

#endif	// !SPQOPT_CXformImplementLeftSemiCorrelatedApplyIn_H

// EOF
