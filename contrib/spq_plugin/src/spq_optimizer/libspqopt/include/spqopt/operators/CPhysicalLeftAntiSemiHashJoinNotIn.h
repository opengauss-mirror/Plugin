//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 EMC Corp.
//
//	@filename:
//		CPhysicalLeftAntiSemiHashJoinNotIn.h
//
//	@doc:
//		Left anti semi hash join operator with NotIn semantics
//---------------------------------------------------------------------------
#ifndef SPQOPT_CPhysicalLeftAntiSemiHashJoinNotIn_H
#define SPQOPT_CPhysicalLeftAntiSemiHashJoinNotIn_H

#include "spqos/base.h"

#include "spqopt/operators/CPhysicalLeftAntiSemiHashJoin.h"

namespace spqopt
{
//---------------------------------------------------------------------------
//	@class:
//		CPhysicalLeftAntiSemiHashJoinNotIn
//
//	@doc:
//		Left anti semi hash join operator with NotIn semantics
//
//---------------------------------------------------------------------------
class CPhysicalLeftAntiSemiHashJoinNotIn : public CPhysicalLeftAntiSemiHashJoin
{
private:
	// private copy ctor
	CPhysicalLeftAntiSemiHashJoinNotIn(
		const CPhysicalLeftAntiSemiHashJoinNotIn &);

public:
	// ctor
	CPhysicalLeftAntiSemiHashJoinNotIn(
		CMemoryPool *mp, CExpressionArray *pdrspqexprOuterKeys,
		CExpressionArray *pdrspqexprInnerKeys,
		IMdIdArray *hash_opfamilies = NULL, BOOL is_null_aware = true,
		CXform::EXformId origin_xform = CXform::ExfSentinel);

	// ident accessors
	virtual EOperatorId
	Eopid() const
	{
		return EopPhysicalLeftAntiSemiHashJoinNotIn;
	}

	// return a string for operator name
	virtual const CHAR *
	SzId() const
	{
		return "CPhysicalLeftAntiSemiHashJoinNotIn";
	}

	//-------------------------------------------------------------------------------------
	// Required Plan Properties
	//-------------------------------------------------------------------------------------

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

	//-------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------

	// conversion function
	static CPhysicalLeftAntiSemiHashJoinNotIn *
	PopConvert(COperator *pop)
	{
		SPQOS_ASSERT(EopPhysicalLeftAntiSemiHashJoinNotIn == pop->Eopid());

		return dynamic_cast<CPhysicalLeftAntiSemiHashJoinNotIn *>(pop);
	}

};	// class CPhysicalLeftAntiSemiHashJoinNotIn

}  // namespace spqopt

#endif	// !SPQOPT_CPhysicalLeftAntiSemiHashJoinNotIn_H

// EOF
