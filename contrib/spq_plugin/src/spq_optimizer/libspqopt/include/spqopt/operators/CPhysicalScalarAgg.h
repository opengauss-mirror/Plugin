//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CPhysicalScalarAgg.h
//
//	@doc:
//		Scalar Aggregate operator
//---------------------------------------------------------------------------
#ifndef SPQOS_CPhysicalScalarAgg_H
#define SPQOS_CPhysicalScalarAgg_H

#include "spqos/base.h"

#include "spqopt/operators/CPhysicalAgg.h"

namespace spqopt
{
// fwd declaration
class CDistributionSpec;

//---------------------------------------------------------------------------
//	@class:
//		CPhysicalScalarAgg
//
//	@doc:
//		scalar aggregate operator
//
//---------------------------------------------------------------------------
class CPhysicalScalarAgg : public CPhysicalAgg
{
private:
	// private copy ctor
	CPhysicalScalarAgg(const CPhysicalScalarAgg &);

public:
	// ctor
	CPhysicalScalarAgg(
		CMemoryPool *mp, CColRefArray *colref_array,
		CColRefArray *pdrspqcrMinimal,  // minimal grouping columns based on FD's
		COperator::EGbAggType egbaggtype, BOOL fGeneratesDuplicates,
		CColRefArray *pdrspqcrArgDQA, BOOL fMultiStage, BOOL isAggFromSplitDQA,
		CLogicalGbAgg::EAggStage aggStage, BOOL should_enforce_distribution);

	// dtor
	virtual ~CPhysicalScalarAgg();


	// ident accessors
	virtual EOperatorId
	Eopid() const
	{
		return EopPhysicalScalarAgg;
	}

	// return a string for operator name
	virtual const CHAR *
	SzId() const
	{
		return "CPhysicalScalarAgg";
	}

	//-------------------------------------------------------------------------------------
	// Required Plan Properties
	//-------------------------------------------------------------------------------------

	// compute required sort columns of the n-th child
	virtual COrderSpec *PosRequired(CMemoryPool *mp, CExpressionHandle &exprhdl,
									COrderSpec *posRequired, ULONG child_index,
									CDrvdPropArray *pdrspqdpCtxt,
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

	// conversion function
	static CPhysicalScalarAgg *
	PopConvert(COperator *pop)
	{
		SPQOS_ASSERT(NULL != pop);
		SPQOS_ASSERT(EopPhysicalScalarAgg == pop->Eopid());

		return reinterpret_cast<CPhysicalScalarAgg *>(pop);
	}

};	// class CPhysicalScalarAgg

}  // namespace spqopt


#endif	// !SPQOS_CPhysicalScalarAgg_H

// EOF
