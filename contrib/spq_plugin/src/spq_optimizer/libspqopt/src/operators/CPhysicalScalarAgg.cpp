//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CPhysicalScalarAgg.cpp
//
//	@doc:
//		Implementation of scalar aggregation operator
//---------------------------------------------------------------------------

#include "spqopt/operators/CPhysicalScalarAgg.h"

#include "spqos/base.h"

#include "spqopt/base/CDistributionSpecHashed.h"
#include "spqopt/base/CDistributionSpecSingleton.h"
#include "spqopt/base/COptCtxt.h"
#include "spqopt/operators/CExpressionHandle.h"

using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalScalarAgg::CPhysicalScalarAgg
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CPhysicalScalarAgg::CPhysicalScalarAgg(
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
//		CPhysicalScalarAgg::~CPhysicalScalarAgg
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CPhysicalScalarAgg::~CPhysicalScalarAgg()
{
}


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalScalarAgg::PosRequired
//
//	@doc:
//		Compute required sort columns of the n-th child
//
//---------------------------------------------------------------------------
COrderSpec *
CPhysicalScalarAgg::PosRequired(CMemoryPool *mp,
								CExpressionHandle &,  // exprhdl
								COrderSpec *,		  // posRequired
								ULONG
#ifdef SPQOS_DEBUG
									child_index
#endif	// SPQOS_DEBUG
								,
								CDrvdPropArray *,  // pdrspqdpCtxt
								ULONG			   // ulOptReq
) const
{
	SPQOS_ASSERT(0 == child_index);

	// return empty sort order
	return SPQOS_NEW(mp) COrderSpec(mp);
}


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalScalarAgg::PosDerive
//
//	@doc:
//		Derive sort order
//
//---------------------------------------------------------------------------
COrderSpec *
CPhysicalScalarAgg::PosDerive(CMemoryPool *mp,
							  CExpressionHandle &  // exprhdl
) const
{
	// return empty sort order
	return SPQOS_NEW(mp) COrderSpec(mp);
}


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalScalarAgg::EpetOrder
//
//	@doc:
//		Return the enforcing type for order property based on this operator
//
//---------------------------------------------------------------------------
CEnfdProp::EPropEnforcingType
CPhysicalScalarAgg::EpetOrder(CExpressionHandle &,	// exprhdl
							  const CEnfdOrder *
#ifdef SPQOS_DEBUG
								  peo
#endif	// SPQOS_DEBUG
) const
{
	SPQOS_ASSERT(NULL != peo);
	SPQOS_ASSERT(!peo->PosRequired()->IsEmpty());

	// TODO: , 06/20/2012: scalar agg produces one row, and hence it should satisfy any order;
	// a problem happens if we have a NLJ(R,S) where R is Salar Agg, and we require sorting on the
	// agg on top of NLJ, in this case we should satisfy this requirement without introducing a Sort
	// even though the NLJ's max card may be > 1
	return CEnfdProp::EpetRequired;
}

// EOF
