//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 EMC Corp.
//
//	@filename:
//		CXformLeftAntiSemiJoinNotIn2CrossProduct.h
//
//	@doc:
//		Transform left anti semi join with NotIn semantics to cross product
//---------------------------------------------------------------------------
#ifndef SPQOPT_CXformLeftAntiSemiJoinNotIn2CrossProduct_H
#define SPQOPT_CXformLeftAntiSemiJoinNotIn2CrossProduct_H

#include "spqos/base.h"

#include "spqopt/operators/CLogicalLeftAntiSemiJoinNotIn.h"
#include "spqopt/operators/CPatternLeaf.h"
#include "spqopt/operators/CPatternTree.h"
#include "spqopt/xforms/CXformLeftAntiSemiJoin2CrossProduct.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CXformLeftAntiSemiJoinNotIn2CrossProduct
//
//	@doc:
//		Transform left anti semi join with NotIn semantics to cross product
//
//---------------------------------------------------------------------------
class CXformLeftAntiSemiJoinNotIn2CrossProduct
	: public CXformLeftAntiSemiJoin2CrossProduct
{
private:
	// private copy ctor
	CXformLeftAntiSemiJoinNotIn2CrossProduct(
		const CXformLeftAntiSemiJoinNotIn2CrossProduct &);

public:
	// ctor
	explicit CXformLeftAntiSemiJoinNotIn2CrossProduct(CMemoryPool *mp)
		:  // pattern
		  CXformLeftAntiSemiJoin2CrossProduct(SPQOS_NEW(mp) CExpression(
			  mp, SPQOS_NEW(mp) CLogicalLeftAntiSemiJoinNotIn(mp),
			  SPQOS_NEW(mp) CExpression(
				  mp,
				  SPQOS_NEW(mp) CPatternTree(
					  mp)),	 // left child is a tree since we may need to push predicates down
			  SPQOS_NEW(mp) CExpression(
				  mp, SPQOS_NEW(mp) CPatternLeaf(mp)),  // right child
			  SPQOS_NEW(mp) CExpression(
				  mp,
				  SPQOS_NEW(mp) CPatternTree(
					  mp))	// predicate is a tree since we may need to do clean-up of scalar expression
			  ))
	{
	}

	// ident accessors
	virtual EXformId
	Exfid() const
	{
		return ExfLeftAntiSemiJoinNotIn2CrossProduct;
	}

	// return a string for xform name
	virtual const CHAR *
	SzId() const
	{
		return "CXformLeftAntiSemiJoinNotIn2CrossProduct";
	}

};	// class CXformLeftAntiSemiJoinNotIn2CrossProduct

}  // namespace spqopt


#endif	// !SPQOPT_CXformLeftAntiSemiJoinNotIn2CrossProduct_H

// EOF
