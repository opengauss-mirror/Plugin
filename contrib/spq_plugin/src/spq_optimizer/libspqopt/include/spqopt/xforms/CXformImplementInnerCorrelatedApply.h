//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CXformImplementInnerCorrelatedApply.h
//
//	@doc:
//		Transform inner correlated apply to physical inner correlated apply
//---------------------------------------------------------------------------
#ifndef SPQOPT_CXformImplementInnerCorrelatedApply_H
#define SPQOPT_CXformImplementInnerCorrelatedApply_H

#include "spqos/base.h"

#include "spqopt/operators/CLogicalInnerCorrelatedApply.h"
#include "spqopt/operators/CPhysicalCorrelatedInnerNLJoin.h"
#include "spqopt/xforms/CXformImplementCorrelatedApply.h"

namespace spqopt
{
using namespace spqos;

//-------------------------------------------------------------------------
//	@class:
//		CXformImplementInnerCorrelatedApply
//
//	@doc:
//		Transform inner correlated apply to physical inner correlated apply
//
//-------------------------------------------------------------------------
class CXformImplementInnerCorrelatedApply
	: public CXformImplementCorrelatedApply<CLogicalInnerCorrelatedApply,
											CPhysicalCorrelatedInnerNLJoin>
{
private:
	// private copy ctor
	CXformImplementInnerCorrelatedApply(
		const CXformImplementInnerCorrelatedApply &);

public:
	// ctor
	explicit CXformImplementInnerCorrelatedApply(CMemoryPool *mp)
		: CXformImplementCorrelatedApply<CLogicalInnerCorrelatedApply,
										 CPhysicalCorrelatedInnerNLJoin>(mp)
	{
	}

	// dtor
	virtual ~CXformImplementInnerCorrelatedApply()
	{
	}

	// ident accessors
	virtual EXformId
	Exfid() const
	{
		return ExfImplementInnerCorrelatedApply;
	}

	virtual const CHAR *
	SzId() const
	{
		return "CXformImplementInnerCorrelatedApply";
	}

};	// class CXformImplementInnerCorrelatedApply

}  // namespace spqopt

#endif	// !SPQOPT_CXformImplementInnerCorrelatedApply_H

// EOF
