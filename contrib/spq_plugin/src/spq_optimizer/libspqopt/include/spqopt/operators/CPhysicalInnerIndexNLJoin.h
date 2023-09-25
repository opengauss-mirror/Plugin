//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 Pivotal, Inc.
//
//	@filename:
//		CPhysicalInnerIndexNLJoin.h
//
//	@doc:
//		Inner index nested-loops join operator
//---------------------------------------------------------------------------
#ifndef SPQOPT_CPhysicalInnerIndexNLJoin_H
#define SPQOPT_CPhysicalInnerIndexNLJoin_H

#include "spqos/base.h"

#include "spqopt/operators/CPhysicalInnerNLJoin.h"

namespace spqopt
{
//---------------------------------------------------------------------------
//	@class:
//		CPhysicalInnerIndexNLJoin
//
//	@doc:
//		Inner index nested-loops join operator
//
//---------------------------------------------------------------------------
class CPhysicalInnerIndexNLJoin : public CPhysicalInnerNLJoin
{
private:
	// columns from outer child used for index lookup in inner child
	CColRefArray *m_pdrspqcrOuterRefs;

	// a copy of the original join predicate that has been pushed down to the inner side
	CExpression *m_origJoinPred;

	// private copy ctor
	CPhysicalInnerIndexNLJoin(const CPhysicalInnerIndexNLJoin &);

public:
	// ctor
	CPhysicalInnerIndexNLJoin(CMemoryPool *mp, CColRefArray *colref_array,
							  CExpression *origJoinPred);

	// dtor
	virtual ~CPhysicalInnerIndexNLJoin();

	// ident accessors
	virtual EOperatorId
	Eopid() const
	{
		return EopPhysicalInnerIndexNLJoin;
	}

	// return a string for operator name
	virtual const CHAR *
	SzId() const
	{
		return "CPhysicalInnerIndexNLJoin";
	}

	// match function
	virtual BOOL Matches(COperator *pop) const;

	// outer column references accessor
	CColRefArray *
	PdrgPcrOuterRefs() const
	{
		return m_pdrspqcrOuterRefs;
	}

	// compute required distribution of the n-th child
	virtual CDistributionSpec *PdsRequired(CMemoryPool *mp,
										   CExpressionHandle &exprhdl,
										   CDistributionSpec *pdsRequired,
										   ULONG child_index,
										   CDrvdPropArray *pdrspqdpCtxt,
										   ULONG ulOptReq) const;

	virtual CEnfdDistribution *Ped(CMemoryPool *mp, CExpressionHandle &exprhdl,
								   CReqdPropPlan *prppInput, ULONG child_index,
								   CDrvdPropArray *pdrspqdpCtxt,
								   ULONG ulDistrReq);

	// execution order of children
	virtual EChildExecOrder
	Eceo() const
	{
		// we optimize inner (right) child first to be able to match child hashed distributions
		return EceoRightToLeft;
	}

	// conversion function
	static CPhysicalInnerIndexNLJoin *
	PopConvert(COperator *pop)
	{
		SPQOS_ASSERT(EopPhysicalInnerIndexNLJoin == pop->Eopid());

		return dynamic_cast<CPhysicalInnerIndexNLJoin *>(pop);
	}

	CExpression *
	OrigJoinPred()
	{
		return m_origJoinPred;
	}

};	// class CPhysicalInnerIndexNLJoin

}  // namespace spqopt

#endif	// !SPQOPT_CPhysicalInnerIndexNLJoin_H

// EOF
