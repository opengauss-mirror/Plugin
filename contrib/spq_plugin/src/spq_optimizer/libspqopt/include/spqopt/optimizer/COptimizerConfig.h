//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2018 Pivotal, Inc.
//
//	@filename:
//		COptimizerConfig.h
//
//	@doc:
//		Configurations used by the optimizer
//---------------------------------------------------------------------------

#ifndef SPQOPT_COptimizerConfig_H
#define SPQOPT_COptimizerConfig_H

#include "spqos/base.h"
#include "spqos/common/CDynamicPtrArray.h"
#include "spqos/common/CRefCount.h"

#include "spqopt/base/CWindowOids.h"
#include "spqopt/engine/CCTEConfig.h"
#include "spqopt/engine/CEnumeratorConfig.h"
#include "spqopt/engine/CHint.h"
#include "spqopt/engine/CStatisticsConfig.h"

namespace spqopt
{
using namespace spqos;

// forward decl
class ICostModel;

//---------------------------------------------------------------------------
//	@class:
//		COptimizerConfig
//
//	@doc:
//		Configuration parameters of the optimizer including damping factors used
//		during statistics derivation, CTE inlining cut-off threshold, Id of plan to
//		be extracted (if plan enumeration is enabled), number of plans to be sampled
//		from the space (if plan sampling is enabled) etc.
//
//		Most of these configurations can be changed from outside ORCA through
//		GUCs. They are also included in optimizer’s minidumps under
//		<dxl:OptimizerConfig> element
//
//---------------------------------------------------------------------------
class COptimizerConfig : public CRefCount
{
private:
	// plan enumeration configuration
	CEnumeratorConfig *m_enumerator_cfg;

	// statistics configuration
	CStatisticsConfig *m_stats_conf;

	// CTE configuration
	CCTEConfig *m_cte_conf;

	// cost model configuration
	ICostModel *m_cost_model;

	// hint configuration
	CHint *m_hint;

	// default window oids
	CWindowOids *m_window_oids;

public:
	// ctor
	COptimizerConfig(CEnumeratorConfig *pec, CStatisticsConfig *stats_config,
					 CCTEConfig *pcteconf, ICostModel *pcm, CHint *phint,
					 CWindowOids *pdefoidsSPQDB);

	// dtor
	virtual ~COptimizerConfig();


	// plan enumeration configuration
	CEnumeratorConfig *
	GetEnumeratorCfg() const
	{
		return m_enumerator_cfg;
	}

	// statistics configuration
	CStatisticsConfig *
	GetStatsConf() const
	{
		return m_stats_conf;
	}

	// CTE configuration
	CCTEConfig *
	GetCteConf() const
	{
		return m_cte_conf;
	}

	// cost model configuration
	ICostModel *
	GetCostModel() const
	{
		return m_cost_model;
	}

	// default window oids
	CWindowOids *
	GetWindowOids() const
	{
		return m_window_oids;
	}

	// hint configuration
	CHint *
	GetHint() const
	{
		return m_hint;
	}

	// generate default optimizer configurations
	static COptimizerConfig *PoconfDefault(CMemoryPool *mp);

	// generate default optimizer configurations with the given cost model
	static COptimizerConfig *PoconfDefault(CMemoryPool *mp, ICostModel *pcm);

	void Serialize(CMemoryPool *mp, CXMLSerializer *xml_serializer,
				   CBitSet *pbsTrace) const;

};	// class COptimizerConfig

}  // namespace spqopt

#endif	// !SPQOPT_COptimizerConfig_H

// EOF
