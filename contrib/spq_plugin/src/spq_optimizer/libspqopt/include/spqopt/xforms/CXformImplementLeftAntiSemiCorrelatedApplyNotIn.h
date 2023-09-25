//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal Inc..
//
//	@filename:
//		CXformImplementLeftAntiSemiCorrelatedApplyNotIn.h
//
//	@doc:
//		Transform left anti semi correlated apply with NOT-IN/ALL semantics
//		to physical left anti semi correlated join
//---------------------------------------------------------------------------
#ifndef SPQOPT_CXformImplementLeftAntiSemiCorrelatedApplyNotIn_H
#define SPQOPT_CXformImplementLeftAntiSemiCorrelatedApplyNotIn_H

#include "spqos/base.h"

#include "spqopt/operators/CLogicalLeftAntiSemiCorrelatedApplyNotIn.h"
#include "spqopt/operators/CPhysicalCorrelatedNotInLeftAntiSemiNLJoin.h"
#include "spqopt/xforms/CXformImplementCorrelatedApply.h"

namespace spqopt
{
using namespace spqos;

//-------------------------------------------------------------------------
//	@class:
//		CXformImplementLeftAntiSemiCorrelatedApplyNotIn
//
//	@doc:
//		Transform left anti semi correlated apply with NOT-IN/ALL semantics
//		to physical left anti semi correlated join
//
//-------------------------------------------------------------------------
class CXformImplementLeftAntiSemiCorrelatedApplyNotIn
	: public CXformImplementCorrelatedApply<
		  CLogicalLeftAntiSemiCorrelatedApplyNotIn,
		  CPhysicalCorrelatedNotInLeftAntiSemiNLJoin>
{
private:
	// private copy ctor
	CXformImplementLeftAntiSemiCorrelatedApplyNotIn(
		const CXformImplementLeftAntiSemiCorrelatedApplyNotIn &);

public:
	// ctor
	explicit CXformImplementLeftAntiSemiCorrelatedApplyNotIn(CMemoryPool *mp)
		: CXformImplementCorrelatedApply<
			  CLogicalLeftAntiSemiCorrelatedApplyNotIn,
			  CPhysicalCorrelatedNotInLeftAntiSemiNLJoin>(mp)
	{
	}

	// dtor
	virtual ~CXformImplementLeftAntiSemiCorrelatedApplyNotIn()
	{
	}

	// ident accessors
	virtual EXformId
	Exfid() const
	{
		return ExfImplementLeftAntiSemiCorrelatedApplyNotIn;
	}

	virtual const CHAR *
	SzId() const
	{
		return "CXformImplementLeftAntiSemiCorrelatedApplyNotIn";
	}

};	// class CXformImplementLeftAntiSemiCorrelatedApplyNotIn

}  // namespace spqopt

#endif	// !SPQOPT_CXformImplementLeftAntiSemiCorrelatedApplyNotIn_H

// EOF
