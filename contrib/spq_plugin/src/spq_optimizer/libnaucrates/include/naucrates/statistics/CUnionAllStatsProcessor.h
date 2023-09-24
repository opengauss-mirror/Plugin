//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2018 Pivotal, Inc.
//
//	@filename:
//		CUnionAllStatsProcessor.h
//
//	@doc:
//		Compute statistics for union all operation
//---------------------------------------------------------------------------
#ifndef SPQNAUCRATES_CUnionAllStatsProcessor_H
#define SPQNAUCRATES_CUnionAllStatsProcessor_H

#include "spqopt/optimizer/COptimizerConfig.h"
#include "naucrates/statistics/CStatistics.h"

namespace spqnaucrates
{
class CUnionAllStatsProcessor
{
public:
	static CStatistics *CreateStatsForUnionAll(
		CMemoryPool *mp, const CStatistics *stats_first_child,
		const CStatistics *stats_second_child, ULongPtrArray *output_colids,
		ULongPtrArray *first_child_colids, ULongPtrArray *second_child_colids);
};
}  // namespace spqnaucrates

#endif	// !SPQNAUCRATES_CUnionAllStatsProcessor_H

// EOF
