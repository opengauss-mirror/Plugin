//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CPhysicalDynamicTableScan.cpp
//
//	@doc:
//		Implementation of dynamic table scan operator
//---------------------------------------------------------------------------

#include "spqopt/operators/CPhysicalDynamicTableScan.h"

#include "spqos/base.h"

#include "spqopt/base/CDistributionSpec.h"
#include "spqopt/base/CDistributionSpecHashed.h"
#include "spqopt/base/CDistributionSpecRandom.h"
#include "spqopt/base/CDistributionSpecSingleton.h"
#include "spqopt/base/CPartIndexMap.h"
#include "spqopt/base/CUtils.h"
#include "spqopt/metadata/CName.h"
#include "spqopt/metadata/CTableDescriptor.h"
#include "naucrates/statistics/CStatisticsUtils.h"

using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalDynamicTableScan::CPhysicalDynamicTableScan
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CPhysicalDynamicTableScan::CPhysicalDynamicTableScan(
	CMemoryPool *mp, BOOL is_partial, const CName *pnameAlias,
	CTableDescriptor *ptabdesc, ULONG ulOriginOpId, ULONG scan_id,
	CColRefArray *pdrspqcrOutput, CColRef2dArray *pdrspqdrspqcrParts,
	ULONG ulSecondaryScanId, CPartConstraint *ppartcnstr,
	CPartConstraint *ppartcnstrRel)
	: CPhysicalDynamicScan(mp, is_partial, ptabdesc, ulOriginOpId, pnameAlias,
						   scan_id, pdrspqcrOutput, pdrspqdrspqcrParts,
						   ulSecondaryScanId, ppartcnstr, ppartcnstrRel)
{
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalDynamicTableScan::Matches
//
//	@doc:
//		match operator
//
//---------------------------------------------------------------------------
BOOL
CPhysicalDynamicTableScan::Matches(COperator *pop) const
{
	return CUtils::FMatchDynamicScan(this, pop);
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalDynamicTableScan::PstatsDerive
//
//	@doc:
//		Statistics derivation during costing
//
//---------------------------------------------------------------------------
IStatistics *
CPhysicalDynamicTableScan::PstatsDerive(CMemoryPool *mp,
										CExpressionHandle &exprhdl,
										CReqdPropPlan *prpplan,
										IStatisticsArray *	// stats_ctxt
) const
{
	SPQOS_ASSERT(NULL != prpplan);

	return CStatisticsUtils::DeriveStatsForDynamicScan(
		mp, exprhdl, ScanId(), prpplan->Pepp()->PpfmDerived());
}

// EOF
