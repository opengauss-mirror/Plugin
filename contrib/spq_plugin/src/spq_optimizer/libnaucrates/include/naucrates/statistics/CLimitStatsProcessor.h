//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2018 Pivotal, Inc.
//
//	@filename:
//		CLimitStatsProcessor.h
//
//	@doc:
//		Compute statistics for limit operation
//---------------------------------------------------------------------------
#ifndef SPQNAUCRATES_CLimitStatsProcessor_H
#define SPQNAUCRATES_CLimitStatsProcessor_H

#include "spqopt/optimizer/COptimizerConfig.h"
#include "naucrates/statistics/CStatisticsUtils.h"

namespace spqnaucrates
{
using namespace spqos;

class CLimitStatsProcessor
{
public:
	// limit
	static CStatistics *CalcLimitStats(CMemoryPool *mp,
									   const CStatistics *input_stats,
									   CDouble input_limit_rows);
};
}  // namespace spqnaucrates

#endif	// !SPQNAUCRATES_CLimitStatsProcessor_H

// EOF
