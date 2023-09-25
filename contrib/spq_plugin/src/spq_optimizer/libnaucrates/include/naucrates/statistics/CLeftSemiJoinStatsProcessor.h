//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2018 Pivotal, Inc.
//
//	@filename:
//		CLeftSemiJoinStatsProcessor.h
//
//	@doc:
//		Processor for computing statistics for Left Semi Join
//---------------------------------------------------------------------------
#ifndef SPQNAUCRATES_CLeftSemiJoinStatsProcessor_H
#define SPQNAUCRATES_CLeftSemiJoinStatsProcessor_H

#include "naucrates/statistics/CJoinStatsProcessor.h"

namespace spqnaucrates
{
class CLeftSemiJoinStatsProcessor : public CJoinStatsProcessor
{
public:
	static CStatistics *CalcLSJoinStatsStatic(
		CMemoryPool *mp, const IStatistics *outer_stats,
		const IStatistics *inner_side_stats,
		CStatsPredJoinArray *join_preds_stats);
};
}  // namespace spqnaucrates

#endif	// !SPQNAUCRATES_CLeftSemiJoinStatsProcessor_H

// EOF
