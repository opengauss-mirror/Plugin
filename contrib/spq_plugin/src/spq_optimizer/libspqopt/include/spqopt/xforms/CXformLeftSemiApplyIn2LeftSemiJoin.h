//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal Inc.
//
//	@filename:
//		CXformLeftSemiApplyIn2LeftSemiJoin.h
//
//	@doc:
//		Turn Left Semi Apply with IN semantics into Left Semi Join
//---------------------------------------------------------------------------
#ifndef SPQOPT_CXformLeftSemiApplyIn2LeftSemiJoin_H
#define SPQOPT_CXformLeftSemiApplyIn2LeftSemiJoin_H

#include "spqos/base.h"

#include "spqopt/operators/CLogicalLeftSemiApplyIn.h"
#include "spqopt/operators/CPatternLeaf.h"
#include "spqopt/operators/CPatternTree.h"
#include "spqopt/xforms/CXformLeftSemiApply2LeftSemiJoin.h"


namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CXformLeftSemiApplyIn2LeftSemiJoin
//
//	@doc:
//		Transform Apply into Join by decorrelating the inner side
//
//---------------------------------------------------------------------------
class CXformLeftSemiApplyIn2LeftSemiJoin
	: public CXformLeftSemiApply2LeftSemiJoin
{
private:
	// private copy ctor
	CXformLeftSemiApplyIn2LeftSemiJoin(
		const CXformLeftSemiApplyIn2LeftSemiJoin &);

public:
	// ctor
	explicit CXformLeftSemiApplyIn2LeftSemiJoin(CMemoryPool *mp)
		: CXformLeftSemiApply2LeftSemiJoin(
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
	virtual ~CXformLeftSemiApplyIn2LeftSemiJoin()
	{
	}

	// ident accessors
	virtual EXformId
	Exfid() const
	{
		return ExfLeftSemiApplyIn2LeftSemiJoin;
	}

	virtual const CHAR *
	SzId() const
	{
		return "CXformLeftSemiApplyIn2LeftSemiJoin";
	}


};	// class CXformLeftSemiApplyIn2LeftSemiJoin

}  // namespace spqopt

#endif	// !SPQOPT_CXformLeftSemiApplyIn2LeftSemiJoin_H

// EOF
