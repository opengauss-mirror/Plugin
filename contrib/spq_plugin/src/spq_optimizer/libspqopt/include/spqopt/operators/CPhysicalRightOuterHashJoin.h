//---------------------------------------------------------------------------
//	Greenplum Database
//  Copyright (c) 2020 VMware, Inc.
//
//	@filename:
//		CPhysicalRightOuterHashJoin.h
//
//	@doc:
//		Right outer hash join operator
//---------------------------------------------------------------------------
#ifndef SPQOPT_CPhysicalRightOuterHashJoin_H
#define SPQOPT_CPhysicalRightOuterHashJoin_H

#include "spqos/base.h"

#include "spqopt/operators/CPhysicalHashJoin.h"

namespace spqopt
{
//---------------------------------------------------------------------------
//	@class:
//		CPhysicalRightOuterHashJoin
//
//	@doc:
//		Right outer hash join operator
//
//---------------------------------------------------------------------------
class CPhysicalRightOuterHashJoin : public CPhysicalHashJoin
{
private:
	// private copy ctor
	CPhysicalRightOuterHashJoin(const CPhysicalRightOuterHashJoin &);

protected:
	// create optimization requests
	virtual void CreateOptRequests(CMemoryPool *mp);

public:
	// ctor
	CPhysicalRightOuterHashJoin(
		CMemoryPool *mp, CExpressionArray *pdrspqexprOuterKeys,
		CExpressionArray *pdrspqexprInnerKeys,
		IMdIdArray *hash_opfamilies = NULL, BOOL is_null_aware = true,
		CXform::EXformId origin_xform = CXform::ExfSentinel);

	// dtor
	virtual ~CPhysicalRightOuterHashJoin();

	// ident accessors
	virtual EOperatorId
	Eopid() const
	{
		return EopPhysicalRightOuterHashJoin;
	}

	// return a string for operator name
	virtual const CHAR *
	SzId() const
	{
		return "CPhysicalRightOuterHashJoin";
	}

	// conversion function
	static CPhysicalRightOuterHashJoin *
	PopConvert(COperator *pop)
	{
		SPQOS_ASSERT(NULL != pop);
		SPQOS_ASSERT(EopPhysicalRightOuterHashJoin == pop->Eopid());

		return dynamic_cast<CPhysicalRightOuterHashJoin *>(pop);
	}

	// compute required partition propagation of the n-th child
	virtual CPartitionPropagationSpec *PppsRequired(
		CMemoryPool *mp, CExpressionHandle &exprhdl,
		CPartitionPropagationSpec *pppsRequired, ULONG child_index,
		CDrvdPropArray *pdrspqdpCtxt, ULONG ulOptReq);

	//-------------------------------------------------------------------------------------
	// Required Plan Properties
	//-------------------------------------------------------------------------------------

	// compute required distribution of the n-th child
	virtual CEnfdDistribution *Ped(CMemoryPool *mp, CExpressionHandle &exprhdl,
								   CReqdPropPlan *prppInput, ULONG child_index,
								   CDrvdPropArray *pdrspqdpCtxt, ULONG ulOptReq);

};	// class CPhysicalRightOuterHashJoin

}  // namespace spqopt

#endif	// !SPQOPT_CPhysicalRightOuterHashJoin_H

// EOF
