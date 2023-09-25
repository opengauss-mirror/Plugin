//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2018 Pivotal, Inc.
//
//	@filename:
//		CInnerJoinStatsProcessor.h
//
//	@doc:
//		Processor for computing statistics for Inner Join
//---------------------------------------------------------------------------
#ifndef SPQNAUCRATES_CInnerJoinStatsProcessor_H
#define SPQNAUCRATES_CInnerJoinStatsProcessor_H

#include "naucrates/statistics/CJoinStatsProcessor.h"


namespace spqnaucrates
{
class CInnerJoinStatsProcessor : public CJoinStatsProcessor
{
public:
	// inner join with another stats structure
	static CStatistics *CalcInnerJoinStatsStatic(
		CMemoryPool *mp, const IStatistics *outer_stats_input,
		const IStatistics *inner_stats_input,
		CStatsPredJoinArray *join_preds_stats);
};
}  // namespace spqnaucrates

#endif	// !SPQNAUCRATES_CInnerJoinStatsProcessor_H

// EOF
