//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CDXLMinidump.h
//
//	@doc:
//		DXL-based minidump structure
//---------------------------------------------------------------------------
#ifndef SPQOPT_CDXLMinidump_H
#define SPQOPT_CDXLMinidump_H

#include "spqos/base.h"

#include "naucrates/dxl/CDXLUtils.h"

// fwd decl
namespace spqos
{
class CBitSet;
}

namespace spqdxl
{
class CDXLNode;
}

using namespace spqos;
using namespace spqdxl;

namespace spqopt
{
//---------------------------------------------------------------------------
//	@class:
//		CDXLMinidump
//
//	@doc:
//		DXL-based minidump
//
//---------------------------------------------------------------------------
class CDXLMinidump
{
private:
	// traceflags
	CBitSet *m_pbs;

	// optimizer configuration
	COptimizerConfig *m_optimizer_config;

	// DXL query tree
	CDXLNode *m_query_dxl_root;

	// Array of DXL nodes that represent the query output
	CDXLNodeArray *m_query_output;

	// Array of DXL nodes that represent the CTE producers
	CDXLNodeArray *m_cte_producers;

	// DXL plan
	CDXLNode *m_plan_dxl_root;

	// metadata objects
	IMDCacheObjectArray *m_mdid_cached_obj_array;

	// source system ids
	CSystemIdArray *m_system_id_array;

	// plan Id
	ULLONG m_plan_id;

	// plan space size
	ULLONG m_plan_space_size;

	// private copy ctor
	CDXLMinidump(const CDXLMinidump &);

public:
	// ctor
	CDXLMinidump(CBitSet *pbs, COptimizerConfig *optimizer_config,
				 CDXLNode *query, CDXLNodeArray *query_output_dxlnode_array,
				 CDXLNodeArray *cte_producers, CDXLNode *pdxlnPlan,
				 IMDCacheObjectArray *mdcache_obj_array,
				 CSystemIdArray *pdrspqsysid, ULLONG plan_id,
				 ULLONG plan_space_size);

	// dtor
	~CDXLMinidump();

	// traceflags
	const CBitSet *Pbs() const;

	// optimizer configuration
	COptimizerConfig *
	GetOptimizerConfig() const
	{
		return m_optimizer_config;
	}

	// query object
	const CDXLNode *GetQueryDXLRoot() const;

	// query output columns
	const CDXLNodeArray *PdrspqdxlnQueryOutput() const;

	// CTE list
	const CDXLNodeArray *GetCTEProducerDXLArray() const;

	// plan
	const CDXLNode *PdxlnPlan() const;

	// metadata objects
	const IMDCacheObjectArray *GetMdIdCachedObjArray() const;

	// source system ids
	const CSystemIdArray *GetSysidPtrArray() const;

	// return plan id
	ULLONG GetPlanId() const;

	// return plan space size
	ULLONG GetPlanSpaceSize() const;

};	// class CDXLMinidump
}  // namespace spqopt

#endif	// !SPQOPT_CDXLMinidump_H

// EOF
