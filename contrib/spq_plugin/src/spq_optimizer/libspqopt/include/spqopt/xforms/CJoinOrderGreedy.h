//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2018 Pivotal Software Inc.
//
//	@filename:
//		CJoinOrderGreedy.h
//
//	@doc:
//		Cardinality-based join order generation with delayed cross joins
//---------------------------------------------------------------------------
#ifndef SPQOPT_CJoinOrderGreedy_H
#define SPQOPT_CJoinOrderGreedy_H

#include "spqos/base.h"
#include "spqos/io/IOstream.h"

#include "spqopt/xforms/CJoinOrder.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CJoinOrderGreedy
//
//	@doc:
//		Helper class for creating join orders based on cardinality of results
//
//---------------------------------------------------------------------------
class CJoinOrderGreedy : public CJoinOrder
{
private:
	// result component
	SComponent *m_pcompResult;

	// returns starting joins with minimal cardinality
	SComponent *GetStartingJoins();

public:
	// ctor
	CJoinOrderGreedy(CMemoryPool *pmp, CExpressionArray *pdrspqexprComponents,
					 CExpressionArray *pdrspqexprConjuncts);

	// dtor
	virtual ~CJoinOrderGreedy();

	// main handler
	virtual CExpression *PexprExpand();

	ULONG
	PickBestJoin(CBitSet *candidate_nodes);

	CBitSet *GetAdjacentComponentsToJoinCandidate();

	virtual CXform::EXformId
	EOriginXForm() const
	{
		return CXform::ExfExpandNAryJoinGreedy;
	}

};	// class CJoinOrderGreedy

}  // namespace spqopt

#endif	// !SPQOPT_CJoinOrderGreedy_H

// EOF
