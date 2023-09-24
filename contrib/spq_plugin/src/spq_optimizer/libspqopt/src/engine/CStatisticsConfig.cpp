//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2015 Pivotal, Inc.
//
//	@filename:
//		CStatisticsConfig.cpp
//
//	@doc:
//		Implementation of statistics context
//---------------------------------------------------------------------------

#include "spqopt/engine/CStatisticsConfig.h"

#include "spqos/base.h"
#include "spqos/common/CAutoP.h"
#include "spqos/common/CHashMap.h"
#include "spqos/common/CHashMapIter.h"

#include "spqopt/base/CColRefSet.h"
#include "naucrates/traceflags/traceflags.h"

using namespace spqopt;

//---------------------------------------------------------------------------
//	@function:
//		CStatisticsConfig::CStatisticsConfig
//
//	@doc:
//		ctor
//
//---------------------------------------------------------------------------
CStatisticsConfig::CStatisticsConfig(CMemoryPool *mp,
									 CDouble damping_factor_filter,
									 CDouble damping_factor_join,
									 CDouble damping_factor_groupby,
									 ULONG max_stats_buckets)
	: m_mp(mp),
	  m_damping_factor_filter(damping_factor_filter),
	  m_damping_factor_join(damping_factor_join),
	  m_damping_factor_groupby(damping_factor_groupby),
	  m_max_stats_buckets(max_stats_buckets),
	  m_phsmdidcolinfo(NULL)
{
	SPQOS_ASSERT(CDouble(0.0) < damping_factor_filter);
	SPQOS_ASSERT(CDouble(0.0) <= damping_factor_join);
	SPQOS_ASSERT(CDouble(0.0) < damping_factor_groupby);
	SPQOS_ASSERT(0 < max_stats_buckets);

	//m_phmmdidcolinfo = New(m_mp) HMMDIdMissingstatscol(m_mp);
	m_phsmdidcolinfo = SPQOS_NEW(m_mp) MdidHashSet(m_mp);
}


//---------------------------------------------------------------------------
//	@function:
//		CStatisticsConfig::~CStatisticsConfig
//
//	@doc:
//		dtor
//		Does not de-allocate memory pool!
//
//---------------------------------------------------------------------------
CStatisticsConfig::~CStatisticsConfig()
{
	m_phsmdidcolinfo->Release();
}

//---------------------------------------------------------------------------
//      @function:
//              CStatisticsConfig::AddMissingStatsColumn
//
//      @doc:
//              Add the information about the column with the missing statistics
//
//---------------------------------------------------------------------------
void
CStatisticsConfig::AddMissingStatsColumn(CMDIdColStats *pmdidCol)
{
	SPQOS_ASSERT(NULL != pmdidCol);

	// add the new column information to the hash set
	if (m_phsmdidcolinfo->Insert(pmdidCol))
	{
		pmdidCol->AddRef();
	}
}


//---------------------------------------------------------------------------
//      @function:
//              CStatisticsConfig::CollectMissingColumns
//
//      @doc:
//              Collect the columns with missing stats
//
//---------------------------------------------------------------------------
void
CStatisticsConfig::CollectMissingStatsColumns(IMdIdArray *pdrgmdid)
{
	SPQOS_ASSERT(NULL != pdrgmdid);

	MdidHashSetIter hsiter(m_phsmdidcolinfo);
	while (hsiter.Advance())
	{
		CMDIdColStats *mdid_col_stats =
			CMDIdColStats::CastMdid(const_cast<IMDId *>(hsiter.Get()));
		mdid_col_stats->AddRef();
		pdrgmdid->Append(mdid_col_stats);
	}
}


// EOF
