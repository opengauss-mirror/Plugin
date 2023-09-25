//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal, Inc.
//
//	@filename:
//		CPhysicalDynamicBitmapTableScan.cpp
//
//	@doc:
//		Dynamic bitmap table scan physical operator
//
//	@owner:
//
//
//	@test:
//
//---------------------------------------------------------------------------

#include "spqopt/operators/CPhysicalDynamicBitmapTableScan.h"

#include "spqopt/base/CDistributionSpec.h"
#include "spqopt/base/CUtils.h"
#include "spqopt/metadata/CName.h"
#include "spqopt/metadata/CPartConstraint.h"
#include "spqopt/metadata/CTableDescriptor.h"
#include "spqopt/operators/CExpressionHandle.h"
#include "spqopt/operators/CPredicateUtils.h"
#include "naucrates/statistics/CFilterStatsProcessor.h"
#include "naucrates/statistics/CStatisticsUtils.h"
using namespace spqopt;
using namespace spqos;

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalDynamicBitmapTableScan::CPhysicalDynamicBitmapTableScan
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CPhysicalDynamicBitmapTableScan::CPhysicalDynamicBitmapTableScan(
	CMemoryPool *mp, BOOL is_partial, CTableDescriptor *ptabdesc,
	ULONG ulOriginOpId, const CName *pnameAlias, ULONG scan_id,
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
//		CPhysicalDynamicBitmapTableScan::Matches
//
//	@doc:
//		match operator
//
//---------------------------------------------------------------------------
BOOL
CPhysicalDynamicBitmapTableScan::Matches(COperator *pop) const
{
	return CUtils::FMatchDynamicBitmapScan(this, pop);
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalDynamicBitmapTableScan::PstatsDerive
//
//	@doc:
//		Statistics derivation during costing
//
//---------------------------------------------------------------------------
IStatistics *
CPhysicalDynamicBitmapTableScan::PstatsDerive(
	CMemoryPool *mp, CExpressionHandle &exprhdl, CReqdPropPlan *prpplan,
	IStatisticsArray *stats_ctxt) const
{
	SPQOS_ASSERT(NULL != prpplan);

	IStatistics *pstatsBaseTable = CStatisticsUtils::DeriveStatsForDynamicScan(
		mp, exprhdl, ScanId(), prpplan->Pepp()->PpfmDerived());

	CExpression *pexprCondChild =
		exprhdl.PexprScalarRepChild(0 /*ulChidIndex*/);
	CExpression *local_expr = NULL;
	CExpression *expr_with_outer_refs = NULL;

	// get outer references from expression handle
	CColRefSet *outer_refs = exprhdl.DeriveOuterReferences();

	CPredicateUtils::SeparateOuterRefs(mp, pexprCondChild, outer_refs,
									   &local_expr, &expr_with_outer_refs);

	IStatistics *stats = CFilterStatsProcessor::MakeStatsFilterForScalarExpr(
		mp, exprhdl, pstatsBaseTable, local_expr, expr_with_outer_refs,
		stats_ctxt);

	pstatsBaseTable->Release();
	local_expr->Release();
	expr_with_outer_refs->Release();

	return stats;
}

// EOF
