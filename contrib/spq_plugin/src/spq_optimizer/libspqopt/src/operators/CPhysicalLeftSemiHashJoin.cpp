//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CPhysicalLeftSemiHashJoin.cpp
//
//	@doc:
//		Implementation of left semi hash join operator
//---------------------------------------------------------------------------

#include "spqopt/operators/CPhysicalLeftSemiHashJoin.h"

#include "spqos/base.h"

#include "spqopt/base/CDistributionSpecHashed.h"
#include "spqopt/base/CUtils.h"


using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalLeftSemiHashJoin::CPhysicalLeftSemiHashJoin
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CPhysicalLeftSemiHashJoin::CPhysicalLeftSemiHashJoin(
	CMemoryPool *mp, CExpressionArray *pdrspqexprOuterKeys,
	CExpressionArray *pdrspqexprInnerKeys, IMdIdArray *hash_opfamilies,
	BOOL is_null_aware, CXform::EXformId origin_xform)
	: CPhysicalHashJoin(mp, pdrspqexprOuterKeys, pdrspqexprInnerKeys,
						hash_opfamilies, is_null_aware, origin_xform)
{
}


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalLeftSemiHashJoin::~CPhysicalLeftSemiHashJoin
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CPhysicalLeftSemiHashJoin::~CPhysicalLeftSemiHashJoin()
{
}


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalLeftSemiHashJoin::FProvidesReqdCols
//
//	@doc:
//		Check if required columns are included in output columns
//
//---------------------------------------------------------------------------
BOOL
CPhysicalLeftSemiHashJoin::FProvidesReqdCols(CExpressionHandle &exprhdl,
											 CColRefSet *pcrsRequired,
											 ULONG	// ulOptReq
) const
{
	// left semi join only propagates columns from left child
	return FOuterProvidesReqdCols(exprhdl, pcrsRequired);
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalLeftSemiHashJoin::PppsRequired
//
//	@doc:
//		Compute required partition propagation of the n-th child
//
//---------------------------------------------------------------------------
CPartitionPropagationSpec *
CPhysicalLeftSemiHashJoin::PppsRequired(CMemoryPool *mp,
										CExpressionHandle &exprhdl,
										CPartitionPropagationSpec *pppsRequired,
										ULONG child_index,
										CDrvdPropArray *pdrspqdpCtxt,
										ULONG  // ulOptReq
)
{
	return PppsRequiredJoinChild(mp, exprhdl, pppsRequired, child_index,
								 pdrspqdpCtxt, false);
}

// EOF
