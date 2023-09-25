//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CPhysicalLeftAntiSemiNLJoin.h
//
//	@doc:
//		Left anti semi nested-loops join operator
//---------------------------------------------------------------------------
#ifndef SPQOPT_CPhysicalLeftAntiSemiNLJoin_H
#define SPQOPT_CPhysicalLeftAntiSemiNLJoin_H

#include "spqos/base.h"

#include "spqopt/operators/CPhysicalNLJoin.h"

namespace spqopt
{
//---------------------------------------------------------------------------
//	@class:
//		CPhysicalLeftAntiSemiNLJoin
//
//	@doc:
//		Left anti semi nested-loops join operator
//
//---------------------------------------------------------------------------
class CPhysicalLeftAntiSemiNLJoin : public CPhysicalNLJoin
{
private:
	// private copy ctor
	CPhysicalLeftAntiSemiNLJoin(const CPhysicalLeftAntiSemiNLJoin &);

public:
	// ctor
	explicit CPhysicalLeftAntiSemiNLJoin(CMemoryPool *mp);

	// dtor
	virtual ~CPhysicalLeftAntiSemiNLJoin();

	// ident accessors
	virtual EOperatorId
	Eopid() const
	{
		return EopPhysicalLeftAntiSemiNLJoin;
	}

	// return a string for operator name
	virtual const CHAR *
	SzId() const
	{
		return "CPhysicalLeftAntiSemiNLJoin";
	}

	// check if required columns are included in output columns
	virtual BOOL FProvidesReqdCols(CExpressionHandle &exprhdl,
								   CColRefSet *pcrsRequired,
								   ULONG ulOptReq) const;

	// compute required partition propagation of the n-th child
	virtual CPartitionPropagationSpec *PppsRequired(
		CMemoryPool *mp, CExpressionHandle &exprhdl,
		CPartitionPropagationSpec *pppsRequired, ULONG child_index,
		CDrvdPropArray *pdrspqdpCtxt, ULONG ulOptReq);

	// conversion function
	static CPhysicalLeftAntiSemiNLJoin *
	PopConvert(COperator *pop)
	{
		SPQOS_ASSERT(EopPhysicalLeftAntiSemiNLJoin == pop->Eopid());

		return dynamic_cast<CPhysicalLeftAntiSemiNLJoin *>(pop);
	}


};	// class CPhysicalLeftAntiSemiNLJoin

}  // namespace spqopt

#endif	// !SPQOPT_CPhysicalLeftAntiSemiNLJoin_H

// EOF
