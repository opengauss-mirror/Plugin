//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2018 Pivotal, Inc.
//
//	@filename:
//		CLogicalJoin.cpp
//
//	@doc:
//		Implementation of logical join class
//---------------------------------------------------------------------------

#include "spqopt/operators/CLogicalJoin.h"

#include "spqos/base.h"

#include "spqopt/base/CColRefSet.h"
#include "spqopt/base/COptCtxt.h"
#include "spqopt/operators/CExpression.h"
#include "spqopt/operators/CExpressionHandle.h"
#include "spqopt/operators/CPredicateUtils.h"
#include "naucrates/statistics/CJoinStatsProcessor.h"

using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CLogicalJoin::CLogicalJoin
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
//CLogicalJoin::CLogicalJoin(CMemoryPool *mp)
//	: CLogical(mp), m_join_order_origin_xform(CXform::ExfSentinel)
//{
//	SPQOS_ASSERT(NULL != mp);
//}

CLogicalJoin::CLogicalJoin(CMemoryPool *mp, CXform::EXformId origin_xform)
	: CLogical(mp), m_origin_xform(origin_xform)
{
	SPQOS_ASSERT(NULL != mp);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalJoin::Matches
//
//	@doc:
//		Match function on operator level
//
//---------------------------------------------------------------------------
BOOL
CLogicalJoin::Matches(COperator *pop) const
{
	return (pop->Eopid() == Eopid());
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalJoin::PstatsDerive
//
//	@doc:
//		Derive statistics
//
//---------------------------------------------------------------------------
IStatistics *
CLogicalJoin::PstatsDerive(CMemoryPool *mp, CExpressionHandle &exprhdl,
						   IStatisticsArray *stats_ctxt) const
{
	return CJoinStatsProcessor::DeriveJoinStats(mp, exprhdl, stats_ctxt);
}

// EOF
