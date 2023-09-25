//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 EMC Corp.
//
//	@filename:
//		CStatisticsConfig.h
//
//	@doc:
//		Statistics configurations
//---------------------------------------------------------------------------
#ifndef SPQOPT_CStatisticsConfig_H
#define SPQOPT_CStatisticsConfig_H

#include "spqos/base.h"
#include "spqos/common/CDouble.h"
#include "spqos/common/CRefCount.h"
#include "spqos/memory/CMemoryPool.h"

#include "naucrates/md/CMDIdColStats.h"
#include "naucrates/md/IMDId.h"

#define MAX_STATS_BUCKETS ULONG(100)

namespace spqopt
{
using namespace spqos;
using namespace spqmd;

//---------------------------------------------------------------------------
//	@class:
//		CStatisticsConfig
//
//	@doc:
//		Statistics configurations
//
//---------------------------------------------------------------------------
class CStatisticsConfig : public CRefCount
{
private:
	// shared memory pool
	CMemoryPool *m_mp;

	// damping factor for filter
	CDouble m_damping_factor_filter;

	// damping factor for join
	CDouble m_damping_factor_join;

	// damping factor for group by
	CDouble m_damping_factor_groupby;

	// max stats buckets for combining histograms
	// See CHistogram::MakeUnionAllHistogramNormalize/MakeUnionHistogramNormalize
	ULONG m_max_stats_buckets;

	// hash set of md ids for columns with missing statistics
	MdidHashSet *m_phsmdidcolinfo;

public:
	// ctor
	CStatisticsConfig(CMemoryPool *mp, CDouble damping_factor_filter,
					  CDouble damping_factor_join,
					  CDouble damping_factor_groupby, ULONG max_stats_buckets);

	// dtor
	~CStatisticsConfig();

	// damping factor for filter
	CDouble
	DDampingFactorFilter() const
	{
		return m_damping_factor_filter;
	}

	// damping factor for join
	CDouble
	DDampingFactorJoin() const
	{
		return m_damping_factor_join;
	}

	// damping factor for group by
	CDouble
	DDampingFactorGroupBy() const
	{
		return m_damping_factor_groupby;
	}

	// max stats buckets for combining histograms
	ULONG
	UlMaxStatsBuckets() const
	{
		return m_max_stats_buckets;
	}

	// add the information about the column with the missing statistics
	void AddMissingStatsColumn(CMDIdColStats *pmdidCol);

	// collect the missing statistics columns
	void CollectMissingStatsColumns(IMdIdArray *pdrgmdid);

	// generate default optimizer configurations
	static CStatisticsConfig *
	PstatsconfDefault(CMemoryPool *mp)
	{
		return SPQOS_NEW(mp) CStatisticsConfig(
			mp, 0.75 /* damping_factor_filter */,
			0.01 /* damping_factor_join */, 0.75 /* damping_factor_groupby */,
			MAX_STATS_BUCKETS);
	}


};	// class CStatisticsConfig
}  // namespace spqopt

#endif	// !SPQOPT_CStatisticsConfig_H

// EOF
