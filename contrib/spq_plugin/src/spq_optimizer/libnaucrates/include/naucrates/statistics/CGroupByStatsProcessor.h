//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2018 Pivotal, Inc.
//
//	@filename:
//		CGroupByStatsProcessor.h
//
//	@doc:
//		Compute statistics for group by operation
//---------------------------------------------------------------------------
#ifndef SPQNAUCRATES_CGroupByStatsProcessor_H
#define SPQNAUCRATES_CGroupByStatsProcessor_H

#include "spqopt/optimizer/COptimizerConfig.h"
#include "naucrates/statistics/CGroupByStatsProcessor.h"
#include "naucrates/statistics/CStatistics.h"
#include "naucrates/statistics/CStatisticsUtils.h"

namespace spqnaucrates
{
class CGroupByStatsProcessor
{
public:
	// group by
	static CStatistics *CalcGroupByStats(CMemoryPool *mp,
										 const CStatistics *input_stats,
										 ULongPtrArray *GCs,
										 ULongPtrArray *aggs, CBitSet *keys);
};
}  // namespace spqnaucrates

#endif	// !SPQNAUCRATES_CGroupByStatsProcessor_H

// EOF
