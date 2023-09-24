//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		CPhysicalHashAgg.cpp
//
//	@doc:
//		Implementation of hash aggregation operator
//---------------------------------------------------------------------------

#include "spqopt/operators/CPhysicalHashAgg.h"

#include "spqos/base.h"

#include "spqopt/base/CDistributionSpecHashed.h"
#include "spqopt/base/CDistributionSpecSingleton.h"
#include "spqopt/base/COptCtxt.h"
#include "spqopt/operators/CExpressionHandle.h"

using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalHashAgg::CPhysicalHashAgg
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CPhysicalHashAgg::CPhysicalHashAgg(
	CMemoryPool *mp, CColRefArray *colref_array, CColRefArray *pdrspqcrMinimal,
	COperator::EGbAggType egbaggtype, BOOL fGeneratesDuplicates,
	CColRefArray *pdrspqcrArgDQA, BOOL fMultiStage, BOOL isAggFromSplitDQA,
	CLogicalGbAgg::EAggStage aggStage, BOOL should_enforce_distribution)
	: CPhysicalAgg(mp, colref_array, pdrspqcrMinimal, egbaggtype,
				   fGeneratesDuplicates, pdrspqcrArgDQA, fMultiStage,
				   isAggFromSplitDQA, aggStage, should_enforce_distribution)
{
}


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalHashAgg::~CPhysicalHashAgg
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CPhysicalHashAgg::~CPhysicalHashAgg()
{
}


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalHashAgg::PosRequired
//
//	@doc:
//		Compute required sort columns of the n-th child
//
//---------------------------------------------------------------------------
COrderSpec *
CPhysicalHashAgg::PosRequired(CMemoryPool *mp,
							  CExpressionHandle &,	// exprhdl
							  COrderSpec *,			// posRequired
							  ULONG
#ifdef SPQOS_DEBUG
								  child_index
#endif	// SPQOS_DEBUG
							  ,
							  CDrvdPropArray *,	 // pdrspqdpCtxt
							  ULONG				 // ulOptReq
) const
{
	SPQOS_ASSERT(0 == child_index);

	// return empty sort order
	return SPQOS_NEW(mp) COrderSpec(mp);
}


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalHashAgg::PosDerive
//
//	@doc:
//		Derive sort order
//
//---------------------------------------------------------------------------
COrderSpec *
CPhysicalHashAgg::PosDerive(CMemoryPool *mp,
							CExpressionHandle &	 // exprhdl
) const
{
	// return empty sort order
	return SPQOS_NEW(mp) COrderSpec(mp);
}


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalHashAgg::EpetOrder
//
//	@doc:
//		Return the enforcing type for order property based on this operator
//
//---------------------------------------------------------------------------
CEnfdProp::EPropEnforcingType
CPhysicalHashAgg::EpetOrder(CExpressionHandle &,  // exprhdl
							const CEnfdOrder *
#ifdef SPQOS_DEBUG
								peo
#endif	// SPQOS_DEBUG
) const
{
	SPQOS_ASSERT(NULL != peo);
	SPQOS_ASSERT(!peo->PosRequired()->IsEmpty());

	return CEnfdProp::EpetRequired;
}

// EOF
