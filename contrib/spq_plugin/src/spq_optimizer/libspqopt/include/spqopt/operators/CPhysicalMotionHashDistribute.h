//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CPhysicalMotionHashDistribute.h
//
//	@doc:
//		Physical Hash distribute motion operator
//---------------------------------------------------------------------------
#ifndef SPQOPT_CPhysicalMotionHashDistribute_H
#define SPQOPT_CPhysicalMotionHashDistribute_H

#include "spqos/base.h"

#include "spqopt/base/CDistributionSpecHashed.h"
#include "spqopt/base/COrderSpec.h"
#include "spqopt/operators/CPhysicalMotion.h"

namespace spqopt
{
//---------------------------------------------------------------------------
//	@class:
//		CPhysicalMotionHashDistribute
//
//	@doc:
//		Hash distribute motion operator
//
//---------------------------------------------------------------------------
class CPhysicalMotionHashDistribute : public CPhysicalMotion
{
private:
	// hash distribution spec
	CDistributionSpecHashed *m_pdsHashed;

	// required columns in distribution spec
	CColRefSet *m_pcrsRequiredLocal;

	// private copy ctor
	CPhysicalMotionHashDistribute(const CPhysicalMotionHashDistribute &);

public:
	// ctor
	CPhysicalMotionHashDistribute(CMemoryPool *mp,
								  CDistributionSpecHashed *pdsHashed);

	// dtor
	virtual ~CPhysicalMotionHashDistribute();

	// ident accessors
	virtual EOperatorId
	Eopid() const
	{
		return EopPhysicalMotionHashDistribute;
	}

	virtual const CHAR *
	SzId() const
	{
		return "CPhysicalMotionHashDistribute";
	}

	// output distribution accessor
	virtual CDistributionSpec *
	Pds() const
	{
		return m_pdsHashed;
	}

	// is motion eliminating duplicates
	BOOL
	IsDuplicateSensitive() const
	{
		return m_pdsHashed->IsDuplicateSensitive();
	}

	// match function
	virtual BOOL Matches(COperator *) const;

	//-------------------------------------------------------------------------------------
	// Required Plan Properties
	//-------------------------------------------------------------------------------------

	// compute required output columns of the n-th child
	virtual CColRefSet *PcrsRequired(CMemoryPool *mp,
									 CExpressionHandle &exprhdl,
									 CColRefSet *pcrsInput, ULONG child_index,
									 CDrvdPropArray *pdrspqdpCtxt,
									 ULONG ulOptReq);

	// compute required sort order of the n-th child
	virtual COrderSpec *PosRequired(CMemoryPool *mp, CExpressionHandle &exprhdl,
									COrderSpec *posInput, ULONG child_index,
									CDrvdPropArray *pdrspqdpCtxt,
									ULONG ulOptReq) const;

	// check if required columns are included in output columns
	virtual BOOL FProvidesReqdCols(CExpressionHandle &exprhdl,
								   CColRefSet *pcrsRequired,
								   ULONG ulOptReq) const;

	//-------------------------------------------------------------------------------------
	// Derived Plan Properties
	//-------------------------------------------------------------------------------------

	// derive sort order
	virtual COrderSpec *PosDerive(CMemoryPool *mp,
								  CExpressionHandle &exprhdl) const;

	//-------------------------------------------------------------------------------------
	// Enforced Properties
	//-------------------------------------------------------------------------------------

	// return order property enforcing type for this operator
	virtual CEnfdProp::EPropEnforcingType EpetOrder(
		CExpressionHandle &exprhdl, const CEnfdOrder *peo) const;

	//-------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------

	// print
	virtual IOstream &OsPrint(IOstream &) const;

	// conversion function
	static CPhysicalMotionHashDistribute *PopConvert(COperator *pop);

	virtual CDistributionSpec *PdsRequired(CMemoryPool *mp,
										   CExpressionHandle &exprhdl,
										   CDistributionSpec *pdsRequired,
										   ULONG child_index,
										   CDrvdPropArray *pdrspqdpCtxt,
										   ULONG ulOptReq) const;

};	// class CPhysicalMotionHashDistribute

}  // namespace spqopt

#endif	// !SPQOPT_CPhysicalMotionHashDistribute_H

// EOF
