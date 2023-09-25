//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CPhysicalLeftOuterHashJoin.h
//
//	@doc:
//		Left outer hash join operator
//---------------------------------------------------------------------------
#ifndef SPQOPT_CPhysicalLeftOuterHashJoin_H
#define SPQOPT_CPhysicalLeftOuterHashJoin_H

#include "spqos/base.h"

#include "spqopt/operators/CPhysicalHashJoin.h"

namespace spqopt
{
//---------------------------------------------------------------------------
//	@class:
//		CPhysicalLeftOuterHashJoin
//
//	@doc:
//		Left outer hash join operator
//
//---------------------------------------------------------------------------
class CPhysicalLeftOuterHashJoin : public CPhysicalHashJoin
{
private:
	// helper for deriving hash join distribution from hashed children
	CDistributionSpec *PdsDeriveFromHashedChildren(
		CMemoryPool *mp, CDistributionSpec *pdsOuter,
		CDistributionSpec *pdsInner) const;

	// private copy ctor
	CPhysicalLeftOuterHashJoin(const CPhysicalLeftOuterHashJoin &);

public:
	// ctor
	CPhysicalLeftOuterHashJoin(
		CMemoryPool *mp, CExpressionArray *pdrspqexprOuterKeys,
		CExpressionArray *pdrspqexprInnerKeys,
		IMdIdArray *hash_opfamilies = NULL, BOOL is_null_aware = true,
		CXform::EXformId origin_xform = CXform::ExfSentinel);

	// dtor
	virtual ~CPhysicalLeftOuterHashJoin();

	// ident accessors
	virtual EOperatorId
	Eopid() const
	{
		return EopPhysicalLeftOuterHashJoin;
	}

	// return a string for operator name
	virtual const CHAR *
	SzId() const
	{
		return "CPhysicalLeftOuterHashJoin";
	}

	// derive distribution
	virtual CDistributionSpec *PdsDerive(CMemoryPool *mp,
										 CExpressionHandle &exprhdl) const;

	// conversion function
	static CPhysicalLeftOuterHashJoin *
	PopConvert(COperator *pop)
	{
		SPQOS_ASSERT(NULL != pop);
		SPQOS_ASSERT(EopPhysicalLeftOuterHashJoin == pop->Eopid());

		return dynamic_cast<CPhysicalLeftOuterHashJoin *>(pop);
	}


};	// class CPhysicalLeftOuterHashJoin

}  // namespace spqopt

#endif	// !SPQOPT_CPhysicalLeftOuterHashJoin_H

// EOF
