//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2018 Pivotal, Inc.
//
//	@filename:
//		CProjectStatsProcessor.h
//
//	@doc:
//		Compute statistics for project operation
//---------------------------------------------------------------------------
#ifndef SPQNAUCRATES_CProjectStatsProcessor_H
#define SPQNAUCRATES_CProjectStatsProcessor_H

#include "spqopt/optimizer/COptimizerConfig.h"
#include "naucrates/statistics/CStatistics.h"

namespace spqnaucrates
{
class CProjectStatsProcessor
{
public:
	// project
	static CStatistics *CalcProjStats(CMemoryPool *mp,
									  const CStatistics *input_stats,
									  ULongPtrArray *projection_colids,
									  UlongToIDatumMap *datum_map);
};
}  // namespace spqnaucrates

#endif	// !SPQNAUCRATES_CProjectStatsProcessor_H

// EOF
