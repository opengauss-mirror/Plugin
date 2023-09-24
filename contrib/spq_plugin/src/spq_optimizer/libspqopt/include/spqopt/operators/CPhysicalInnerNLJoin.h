//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		CPhysicalInnerNLJoin.h
//
//	@doc:
//		Inner nested-loops join operator
//---------------------------------------------------------------------------
#ifndef SPQOPT_CPhysicalInnerNLJoin_H
#define SPQOPT_CPhysicalInnerNLJoin_H

#include "spqos/base.h"

#include "spqopt/operators/CPhysicalNLJoin.h"

namespace spqopt
{
//---------------------------------------------------------------------------
//	@class:
//		CPhysicalInnerNLJoin
//
//	@doc:
//		Inner nested-loops join operator
//
//---------------------------------------------------------------------------
class CPhysicalInnerNLJoin : public CPhysicalNLJoin
{
private:
	// private copy ctor
	CPhysicalInnerNLJoin(const CPhysicalInnerNLJoin &);

public:
	// ctor
	explicit CPhysicalInnerNLJoin(CMemoryPool *mp);

	// dtor
	virtual ~CPhysicalInnerNLJoin();

	// ident accessors
	virtual EOperatorId
	Eopid() const
	{
		return EopPhysicalInnerNLJoin;
	}

	// return a string for operator name
	virtual const CHAR *
	SzId() const
	{
		return "CPhysicalInnerNLJoin";
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

	// conversion function
	static CPhysicalInnerNLJoin *
	PopConvert(COperator *pop)
	{
		SPQOS_ASSERT(EopPhysicalInnerNLJoin == pop->Eopid());

		return dynamic_cast<CPhysicalInnerNLJoin *>(pop);
	}


};	// class CPhysicalInnerNLJoin

}  // namespace spqopt

#endif	// !SPQOPT_CPhysicalInnerNLJoin_H

// EOF
