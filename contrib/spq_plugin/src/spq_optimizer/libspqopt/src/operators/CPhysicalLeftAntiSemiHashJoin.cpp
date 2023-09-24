//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CPhysicalLeftAntiSemiHashJoin.cpp
//
//	@doc:
//		Implementation of left anti semi hash join operator
//---------------------------------------------------------------------------

#include "spqopt/operators/CPhysicalLeftAntiSemiHashJoin.h"

#include "spqos/base.h"

#include "spqopt/base/CDistributionSpecHashed.h"


using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalLeftAntiSemiHashJoin::CPhysicalLeftAntiSemiHashJoin
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CPhysicalLeftAntiSemiHashJoin::CPhysicalLeftAntiSemiHashJoin(
	CMemoryPool *mp, CExpressionArray *pdrspqexprOuterKeys,
	CExpressionArray *pdrspqexprInnerKeys, IMdIdArray *hash_opfamilies,
	BOOL is_null_aware, CXform::EXformId origin_xform)
	: CPhysicalHashJoin(mp, pdrspqexprOuterKeys, pdrspqexprInnerKeys,
						hash_opfamilies, is_null_aware, origin_xform)
{
}


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalLeftAntiSemiHashJoin::~CPhysicalLeftAntiSemiHashJoin
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CPhysicalLeftAntiSemiHashJoin::~CPhysicalLeftAntiSemiHashJoin()
{
}


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalLeftAntiSemiHashJoin::FProvidesReqdCols
//
//	@doc:
//		Check if required columns are included in output columns
//
//---------------------------------------------------------------------------
BOOL
CPhysicalLeftAntiSemiHashJoin::FProvidesReqdCols(CExpressionHandle &exprhdl,
												 CColRefSet *pcrsRequired,
												 ULONG	// ulOptReq
) const
{
	// left anti semi join only propagates columns from left child
	return FOuterProvidesReqdCols(exprhdl, pcrsRequired);
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalLeftAntiSemiHashJoin::PppsRequired
//
//	@doc:
//		Compute required partition propagation of the n-th child
//
//---------------------------------------------------------------------------
CPartitionPropagationSpec *
CPhysicalLeftAntiSemiHashJoin::PppsRequired(
	CMemoryPool *mp, CExpressionHandle &exprhdl,
	CPartitionPropagationSpec *pppsRequired, ULONG child_index,
	CDrvdPropArray *,  // pdrspqdpCtxt,
	ULONG			   // ulOptReq
)
{
	// no partition elimination for LASJ: push request to the respective child
	return CPhysical::PppsRequiredPushThruNAry(mp, exprhdl, pppsRequired,
											   child_index);
}

// EOF
