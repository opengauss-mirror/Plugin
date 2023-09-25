//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal Inc..
//
//	@filename:
//		CXformImplementLeftAntiSemiCorrelatedApply.h
//
//	@doc:
//		Transform left anti semi correlated apply (for NOT EXISTS subqueries)
//		to physical left anti semi correlated join
//---------------------------------------------------------------------------
#ifndef SPQOPT_CXformImplementLeftAntiSemiCorrelatedApply_H
#define SPQOPT_CXformImplementLeftAntiSemiCorrelatedApply_H

#include "spqos/base.h"

#include "spqopt/operators/CLogicalLeftAntiSemiCorrelatedApply.h"
#include "spqopt/operators/CPhysicalCorrelatedLeftAntiSemiNLJoin.h"
#include "spqopt/xforms/CXformImplementCorrelatedApply.h"

namespace spqopt
{
using namespace spqos;

//-------------------------------------------------------------------------
//	@class:
//		CXformImplementLeftAntiSemiCorrelatedApply
//
//	@doc:
//		Transform left anti semi correlated apply  (for NOT EXISTS subqueries)
//		to physical left anti semi correlated join
//
//-------------------------------------------------------------------------
class CXformImplementLeftAntiSemiCorrelatedApply
	: public CXformImplementCorrelatedApply<
		  CLogicalLeftAntiSemiCorrelatedApply,
		  CPhysicalCorrelatedLeftAntiSemiNLJoin>
{
private:
	// private copy ctor
	CXformImplementLeftAntiSemiCorrelatedApply(
		const CXformImplementLeftAntiSemiCorrelatedApply &);

public:
	// ctor
	explicit CXformImplementLeftAntiSemiCorrelatedApply(CMemoryPool *mp)
		: CXformImplementCorrelatedApply<CLogicalLeftAntiSemiCorrelatedApply,
										 CPhysicalCorrelatedLeftAntiSemiNLJoin>(
			  mp)
	{
	}

	// dtor
	virtual ~CXformImplementLeftAntiSemiCorrelatedApply()
	{
	}

	// ident accessors
	virtual EXformId
	Exfid() const
	{
		return ExfImplementLeftAntiSemiCorrelatedApply;
	}

	virtual const CHAR *
	SzId() const
	{
		return "CXformImplementLeftAntiSemiCorrelatedApply";
	}

};	// class CXformImplementLeftAntiSemiCorrelatedApply

}  // namespace spqopt

#endif	// !SPQOPT_CXformImplementLeftAntiSemiCorrelatedApply_H

// EOF
