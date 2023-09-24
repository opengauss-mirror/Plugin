//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CPhysicalMotion.h
//
//	@doc:
//		Base class for Motion operators
//---------------------------------------------------------------------------
#ifndef SPQOPT_CPhysicalMotion_H
#define SPQOPT_CPhysicalMotion_H

#include "spqos/base.h"

#include "spqopt/base/CUtils.h"
#include "spqopt/operators/CPhysical.h"


namespace spqopt
{
//---------------------------------------------------------------------------
//	@class:
//		CPhysicalMotion
//
//	@doc:
//		Base class for Motion operators
//
//---------------------------------------------------------------------------
class CPhysicalMotion : public CPhysical
{
private:
	// private copy ctor
	CPhysicalMotion(const CPhysicalMotion &);

protected:
	// ctor
	explicit CPhysicalMotion(CMemoryPool *mp) : CPhysical(mp)
	{
	}

	// sensitivity to order of inputs
	virtual BOOL
	FInputOrderSensitive() const
	{
		return true;
	}

public:
	// output distribution accessor
	virtual CDistributionSpec *Pds() const = 0;

	// check if optimization contexts is valid
	virtual BOOL FValidContext(CMemoryPool *mp, COptimizationContext *poc,
							   COptimizationContextArray *pdrspqocChild) const;

	//-------------------------------------------------------------------------------------
	// Required Plan Properties
	//-------------------------------------------------------------------------------------

	// compute required ctes of the n-th child
	virtual CCTEReq *PcteRequired(CMemoryPool *mp, CExpressionHandle &exprhdl,
								  CCTEReq *pcter, ULONG child_index,
								  CDrvdPropArray *pdrspqdpCtxt,
								  ULONG ulOptReq) const;

	// compute required distribution of the n-th child
	virtual CDistributionSpec *PdsRequired(CMemoryPool *mp,
										   CExpressionHandle &exprhdl,
										   CDistributionSpec *pdsRequired,
										   ULONG child_index,
										   CDrvdPropArray *pdrspqdpCtxt,
										   ULONG ulOptReq) const;

	// compute required rewindability of the n-th child
	virtual CRewindabilitySpec *PrsRequired(
		CMemoryPool *mp,
		CExpressionHandle &,   // exprhdl
		CRewindabilitySpec *,  // prsRequired
		ULONG,				   // child_index
		CDrvdPropArray *pdrspqdpCtxt, ULONG ulOptReq) const;

	// compute required partition propagation of the n-th child
	virtual CPartitionPropagationSpec *PppsRequired(
		CMemoryPool *mp, CExpressionHandle &exprhdl,
		CPartitionPropagationSpec *pppsRequired, ULONG child_index,
		CDrvdPropArray *pdrspqdpCtxt, ULONG ulOptReq);

	//-------------------------------------------------------------------------------------
	// Derived Plan Properties
	//-------------------------------------------------------------------------------------

	// derive distribution
	virtual CDistributionSpec *PdsDerive(CMemoryPool *mp,
										 CExpressionHandle &exprhdl) const;

	// derive rewindability
	virtual CRewindabilitySpec *PrsDerive(CMemoryPool *mp,
										  CExpressionHandle &exprhdl) const;

	// derive partition index map
	virtual CPartIndexMap *
	PpimDerive(CMemoryPool *,  // mp
			   CExpressionHandle &exprhdl,
			   CDrvdPropCtxt *	//pdpctxt
	) const
	{
		return PpimPassThruOuter(exprhdl);
	}

	// derive partition filter map
	virtual CPartFilterMap *
	PpfmDerive(CMemoryPool *,  // mp
			   CExpressionHandle &exprhdl) const
	{
		return PpfmPassThruOuter(exprhdl);
	}


	//-------------------------------------------------------------------------------------
	// Enforced Properties
	//-------------------------------------------------------------------------------------

	// return distribution property enforcing type for this operator
	virtual CEnfdProp::EPropEnforcingType EpetDistribution(
		CExpressionHandle &exprhdl, const CEnfdDistribution *ped) const;

	// return rewindability property enforcing type for this operator
	virtual CEnfdProp::EPropEnforcingType EpetRewindability(
		CExpressionHandle &,		// exprhdl
		const CEnfdRewindability *	// per
	) const;

	// return true if operator passes through stats obtained from children,
	// this is used when computing stats during costing
	virtual BOOL
	FPassThruStats() const
	{
		return true;
	}

	// conversion function
	static CPhysicalMotion *
	PopConvert(COperator *pop)
	{
		SPQOS_ASSERT(CUtils::FPhysicalMotion(pop));

		return dynamic_cast<CPhysicalMotion *>(pop);
	}

};	// class CPhysicalMotion

}  // namespace spqopt

#endif	// !SPQOPT_CPhysicalMotion_H

// EOF
