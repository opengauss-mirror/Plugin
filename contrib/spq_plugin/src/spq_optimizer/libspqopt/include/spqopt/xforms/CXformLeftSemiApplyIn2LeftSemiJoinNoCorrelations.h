//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal Inc.
//
//	@filename:
//		CXformLeftSemiApplyIn2LeftSemiJoinNoCorrelations.h
//
//	@doc:
//		Turn Left Semi Apply (with IN semantics) into Left Semi join when inner
//		child has no outer references
//---------------------------------------------------------------------------
#ifndef SPQOPT_CXformLeftSemiApplyIn2LeftSemiJoinNoCorrelations_H
#define SPQOPT_CXformLeftSemiApplyIn2LeftSemiJoinNoCorrelations_H

#include "spqos/base.h"

#include "spqopt/xforms/CXformLeftSemiApply2LeftSemiJoinNoCorrelations.h"


namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CXformLeftSemiApplyIn2LeftSemiJoinNoCorrelations
//
//	@doc:
//		Transform Apply into Join by decorrelating the inner side
//
//---------------------------------------------------------------------------
class CXformLeftSemiApplyIn2LeftSemiJoinNoCorrelations
	: public CXformLeftSemiApply2LeftSemiJoinNoCorrelations
{
private:
	// private copy ctor
	CXformLeftSemiApplyIn2LeftSemiJoinNoCorrelations(
		const CXformLeftSemiApplyIn2LeftSemiJoinNoCorrelations &);

public:
	// ctor
	explicit CXformLeftSemiApplyIn2LeftSemiJoinNoCorrelations(CMemoryPool *mp)
		: CXformLeftSemiApply2LeftSemiJoinNoCorrelations(
			  mp, SPQOS_NEW(mp) CExpression(
					  mp, SPQOS_NEW(mp) CLogicalLeftSemiApplyIn(mp),
					  SPQOS_NEW(mp) CExpression(
						  mp, SPQOS_NEW(mp) CPatternLeaf(mp)),  // left child
					  SPQOS_NEW(mp) CExpression(
						  mp, SPQOS_NEW(mp) CPatternTree(mp)),  // right child
					  SPQOS_NEW(mp) CExpression(
						  mp, SPQOS_NEW(mp) CPatternTree(mp))  // predicate
					  ))
	{
	}

	// dtor
	virtual ~CXformLeftSemiApplyIn2LeftSemiJoinNoCorrelations()
	{
	}

	// ident accessors
	virtual EXformId
	Exfid() const
	{
		return ExfLeftSemiApplyIn2LeftSemiJoinNoCorrelations;
	}

	virtual const CHAR *
	SzId() const
	{
		return "CXformLeftSemiApplyIn2LeftSemiJoinNoCorrelations";
	}


};	// class CXformLeftSemiApplyIn2LeftSemiJoinNoCorrelations

}  // namespace spqopt

#endif	// !SPQOPT_CXformLeftSemiApplyIn2LeftSemiJoinNoCorrelations_H

// EOF
