//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal Inc..
//
//	@filename:
//		CLogicalMaxOneRow.cpp
//
//	@doc:
//		Implementation of logical MaxOneRow operator
//---------------------------------------------------------------------------

#include "spqopt/operators/CLogicalMaxOneRow.h"

#include "spqos/base.h"

#include "spqopt/operators/CExpression.h"
#include "spqopt/operators/CExpressionHandle.h"
#include "spqopt/xforms/CXformUtils.h"

using namespace spqopt;

//---------------------------------------------------------------------------
//	@function:
//		CLogicalMaxOneRow::Esp
//
//	@doc:
//		Promise level for stat derivation
//
//---------------------------------------------------------------------------
CLogical::EStatPromise
CLogicalMaxOneRow::Esp(CExpressionHandle &exprhdl) const
{
	// low promise for stat derivation if logical expression has outer-refs
	// or is part of an Apply expression
	if (exprhdl.HasOuterRefs() ||
		(NULL != exprhdl.Pgexpr() &&
		 CXformUtils::FGenerateApply(exprhdl.Pgexpr()->ExfidOrigin())))
	{
		return EspLow;
	}

	return EspHigh;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalMaxOneRow::PcrsStat
//
//	@doc:
//		Promise level for stat derivation
//
//---------------------------------------------------------------------------
CColRefSet *
CLogicalMaxOneRow::PcrsStat(CMemoryPool *mp, CExpressionHandle &exprhdl,
							CColRefSet *pcrsInput, ULONG child_index) const
{
	SPQOS_ASSERT(0 == child_index);

	CColRefSet *pcrs = SPQOS_NEW(mp) CColRefSet(mp);
	pcrs->Union(pcrsInput);

	// intersect with the output columns of relational child
	pcrs->Intersection(exprhdl.DeriveOutputColumns(child_index));

	return pcrs;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalMaxOneRow::PxfsCandidates
//
//	@doc:
//		Compute candidate xforms
//
//---------------------------------------------------------------------------
CXformSet *
CLogicalMaxOneRow::PxfsCandidates(CMemoryPool *mp) const
{
	CXformSet *xform_set = SPQOS_NEW(mp) CXformSet(mp);
	(void) xform_set->ExchangeSet(CXform::ExfMaxOneRow2Assert);
	return xform_set;
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalMaxOneRow::PstatsDerive
//
//	@doc:
//		Derive statistics
//
//---------------------------------------------------------------------------
IStatistics *
CLogicalMaxOneRow::PstatsDerive(CMemoryPool *mp, CExpressionHandle &exprhdl,
								IStatisticsArray *	// stats_ctxt
) const
{
	// no more than one row can be produced by operator, scale down input statistics accordingly
	IStatistics *stats = exprhdl.Pstats(0);
	return stats->ScaleStats(mp, CDouble(1.0 / stats->Rows()));
}


// EOF
