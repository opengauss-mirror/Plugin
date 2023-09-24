//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CJoinOrderMinCard.h
//
//	@doc:
//		Cardinality-based join order generation
//---------------------------------------------------------------------------
#ifndef SPQOPT_CJoinOrderMinCard_H
#define SPQOPT_CJoinOrderMinCard_H

#include "spqos/base.h"
#include "spqos/common/CBitSet.h"
#include "spqos/common/CRefCount.h"
#include "spqos/io/IOstream.h"

#include "spqopt/xforms/CJoinOrder.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CJoinOrderMinCard
//
//	@doc:
//		Helper class for creating join orders based on cardinality of results
//
//---------------------------------------------------------------------------
class CJoinOrderMinCard : public CJoinOrder
{
private:
	// result component
	SComponent *m_pcompResult;

public:
	// ctor
	CJoinOrderMinCard(CMemoryPool *mp, CExpressionArray *pdrspqexprComponents,
					  CExpressionArray *pdrspqexprConjuncts);

	// dtor
	virtual ~CJoinOrderMinCard();

	// main handler
	virtual CExpression *PexprExpand();

	// print function
	virtual IOstream &OsPrint(IOstream &) const;

	virtual CXform::EXformId
	EOriginXForm() const
	{
		return CXform::ExfExpandNAryJoinMinCard;
	}

};	// class CJoinOrderMinCard

}  // namespace spqopt

#endif	// !SPQOPT_CJoinOrderMinCard_H

// EOF
