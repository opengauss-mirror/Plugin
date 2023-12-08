//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Greenplum, Inc.
//
//	@filename:
//		COptTasks.cpp
//
//	@doc:
//		Routines to perform optimization related tasks using the spqos framework
//
//	@test:
//
//
//---------------------------------------------------------------------------

#include "spq_optimizer_util/utils/COptTasks.h"

#include "spqos/base.h"
#include "spqos/error/CException.h"

#include "spq_optimizer_util/config/CConfigParamMapping.h"
#include "spqopt/engine/CHint.h"
#include "spqopt/eval/CConstExprEvaluatorDXL.h"
#include "spq_optimizer_util/relcache/CMDProviderRelcache.h"
#include "spq_optimizer_util/translate/CContextDXLToPlStmt.h"
#include "spqopt/translate/CTranslatorDXLToExpr.h"
#include "spq_optimizer_util/translate/CTranslatorDXLToPlStmt.h"
#include "spqopt/translate/CTranslatorExprToDXL.h"
#include "spq_optimizer_util/translate/CTranslatorQueryToDXL.h"
#include "spq_optimizer_util/translate/CTranslatorRelcacheToDXL.h"
#include "spq_optimizer_util/translate/CTranslatorUtils.h"
#include "spq_optimizer_util/utils/CConstExprEvaluatorProxy.h"
#include "spq_optimizer_util/utils/spqdbdefs.h"

//#include "cdb/cdbvars.h"
#include "utils/fmgroids.h"
#include "utils/guc.h"
#undef setstate

#include "spqopt/engine/CHint.h"
#include "spqos/_api.h"
#include "spqos/common/CAutoP.h"
#include "spqos/io/COstreamFile.h"
#include "spqos/io/COstreamString.h"
#include "spqos/memory/CAutoMemoryPool.h"
#include "spqos/task/CAutoTraceFlag.h"

#include "spqdbcost/CCostModelSPQDB.h"
#include "spqopt/base/CAutoOptCtxt.h"
#include "spqopt/engine/CCTEConfig.h"
#include "spqopt/engine/CEnumeratorConfig.h"
#include "spqopt/engine/CStatisticsConfig.h"
#include "spqopt/exception.h"
#include "spq_optimizer_util/spq_wrappers.h"
#include "spqopt/mdcache/CAutoMDAccessor.h"
#include "spqopt/mdcache/CMDCache.h"
#include "spqopt/minidump/CMinidumperUtils.h"
#include "spqopt/optimizer/COptimizer.h"
#include "spqopt/optimizer/COptimizerConfig.h"
#include "spqopt/translate/CTranslatorDXLToExpr.h"
#include "spqopt/translate/CTranslatorExprToDXL.h"
#include "spqopt/xforms/CXformFactory.h"
#include "naucrates/base/CQueryToDXLResult.h"
#include "naucrates/dxl/CDXLUtils.h"
#include "naucrates/dxl/CIdGenerator.h"
#include "naucrates/dxl/operators/CDXLNode.h"
#include "naucrates/dxl/parser/CParseHandlerDXL.h"
#include "naucrates/exception.h"
#include "naucrates/init.h"
#include "naucrates/md/CMDIdCast.h"
#include "naucrates/md/CMDIdRelStats.h"
#include "naucrates/md/CMDIdScCmp.h"
#include "naucrates/md/CSystemId.h"
#include "naucrates/md/IMDId.h"
#include "naucrates/md/IMDRelStats.h"
#include "naucrates/traceflags/traceflags.h"

using namespace spqos;
using namespace spqopt;
using namespace spqdxl;
using namespace spqdbcost;

// size of error buffer
#define SPQOPT_ERROR_BUFFER_SIZE 10 * 1024 * 1024

// definition of default AutoMemoryPool
#define AUTO_MEM_POOL(amp) CAutoMemoryPool amp(CAutoMemoryPool::ElcExc)

// default id for the source system
const CSystemId default_sysid(IMDId::EmdidGeneral, SPQOS_WSZ_STR_LENGTH("SPQDB"));

// TODO SPQ 下面的改动全部要写入GUC

//---------------------------------------------------------------------------
//	@function:
//		SOptContext::SOptContext
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
SOptContext::SOptContext()
	: m_query_dxl(NULL),
	  m_query(NULL),
	  m_plan_dxl(NULL),
	  m_plan_stmt(NULL),
	  m_should_generate_plan_stmt(false),
	  m_should_serialize_plan_dxl(false),
	  m_is_unexpected_failure(false),
	  m_should_error_out(false),
	  m_error_msg(NULL)
{
}

//---------------------------------------------------------------------------
//	@function:
//		SOptContext::Free
//
//	@doc:
//		Free all members except those pointed to by either input or
//		output
//
//---------------------------------------------------------------------------
void
SOptContext::Free(SOptContext::EPin input, SOptContext::EPin output)
{
	if (NULL != m_query_dxl && epinQueryDXL != input && epinQueryDXL != output)
	{
		spqdb::SPQDBFree(m_query_dxl);
	}

	if (NULL != m_query && epinQuery != input && epinQuery != output)
	{
		spqdb::SPQDBFree(m_query);
	}

	if (NULL != m_plan_dxl && epinPlanDXL != input && epinPlanDXL != output)
	{
		spqdb::SPQDBFree(m_plan_dxl);
	}

	if (NULL != m_plan_stmt && epinPlStmt != input && epinPlStmt != output)
	{
		spqdb::SPQDBFree(m_plan_stmt);
	}

	if (NULL != m_error_msg && epinErrorMsg != input && epinErrorMsg != output)
	{
		spqdb::SPQDBFree(m_error_msg);
	}
}

//---------------------------------------------------------------------------
//	@function:
//		SOptContext::CloneErrorMsg
//
//	@doc:
//		Clone m_error_msg to given memory context. Return NULL if there is no
//		error message.
//
//---------------------------------------------------------------------------
CHAR *
SOptContext::CloneErrorMsg(MemoryContext context)
{
	if (NULL == context || NULL == m_error_msg)
	{
		return NULL;
	}
	return spqdb::MemCtxtStrdup(context, m_error_msg);
}


//---------------------------------------------------------------------------
//	@function:
//		SOptContext::Cast
//
//	@doc:
//		Casting function
//
//---------------------------------------------------------------------------
SOptContext *
SOptContext::Cast(void *ptr)
{
	SPQOS_ASSERT(NULL != ptr);

	return reinterpret_cast<SOptContext *>(ptr);
}

//---------------------------------------------------------------------------
//	@function:
//		CreateMultiByteCharStringFromWCString
//
//	@doc:
//		Return regular string from wide-character string
//
//---------------------------------------------------------------------------
CHAR *
COptTasks::CreateMultiByteCharStringFromWCString(const WCHAR *wcstr)
{
	SPQOS_ASSERT(NULL != wcstr);

	const ULONG input_len = SPQOS_WSZ_LENGTH(wcstr);
	const ULONG wchar_size = SPQOS_SIZEOF(WCHAR);
	const ULONG max_len = (input_len + 1) * wchar_size;

	CHAR *str = (CHAR *) spqdb::SPQDBAlloc(max_len);

	spqos::clib::Wcstombs(str, const_cast<WCHAR *>(wcstr), max_len);
	str[max_len - 1] = '\0';

	return str;
}

//---------------------------------------------------------------------------
//	@function:
//		COptTasks::Execute
//
//	@doc:
//		Execute a task using SPQOS. TODO extend spqos to provide
//		this functionality
//
//---------------------------------------------------------------------------
void
COptTasks::Execute(void *(*func)(void *), void *func_arg)
{
	Assert(func);

	CHAR *err_buf = (CHAR *) palloc(SPQOPT_ERROR_BUFFER_SIZE);
	err_buf[0] = '\0';

	// initialize DXL support
	InitDXL();

	bool abort_flag = false;

	CAutoMemoryPool amp(CAutoMemoryPool::ElcNone);

	spqos_exec_params params;
	params.func = func;
	params.arg = func_arg;
	params.stack_start = &params;
	params.error_buffer = err_buf;
	params.error_buffer_size = SPQOPT_ERROR_BUFFER_SIZE;
	params.abort_requested = &abort_flag;

	// execute task and send log message to server log
	SPQOS_TRY
	{
		(void) spqos_exec(&params);
	}
	SPQOS_CATCH_EX(ex)
	{
		LogExceptionMessageAndDelete(err_buf, ex.SeverityLevel());
		SPQOS_RETHROW(ex);
	}
	SPQOS_CATCH_END;
	LogExceptionMessageAndDelete(err_buf);
}

void
COptTasks::LogExceptionMessageAndDelete(CHAR *err_buf, ULONG severity_level)
{
	if ('\0' != err_buf[0])
	{
		int spqdb_severity_level;

		if (severity_level == CException::ExsevDebug1)
			spqdb_severity_level = DEBUG1;
		else
			spqdb_severity_level = LOG;

		elog(spqdb_severity_level, "%s",
			 CreateMultiByteCharStringFromWCString((WCHAR *) err_buf));
	}

	pfree(err_buf);
}


//---------------------------------------------------------------------------
//	@function:
//		COptTasks::ConvertToPlanStmtFromDXL
//
//	@doc:
//		Translate a DXL tree into a planned statement
//
//---------------------------------------------------------------------------
PlannedStmt *
COptTasks::ConvertToPlanStmtFromDXL(
	CMemoryPool *mp, CMDAccessor *md_accessor, const CDXLNode *dxlnode,
	bool can_set_tag, DistributionHashOpsKind distribution_hashops, Query *orig_query)
{
	SPQOS_ASSERT(NULL != md_accessor);
	SPQOS_ASSERT(NULL != dxlnode);

	CIdGenerator plan_id_generator(1 /* ulStartId */);
	CIdGenerator motion_id_generator(1 /* ulStartId */);
	CIdGenerator param_id_generator(0 /* ulStartId */);

	List *table_list = NULL;
	List *subplans_list = NULL;

	CContextDXLToPlStmt dxl_to_plan_stmt_ctxt(
		mp, &plan_id_generator, &motion_id_generator, &param_id_generator,
		distribution_hashops, &table_list, &subplans_list);

	// translate DXL -> PlannedStmt
	CTranslatorDXLToPlStmt dxl_to_plan_stmt_translator(
		mp, md_accessor, &dxl_to_plan_stmt_ctxt, spqdb::GetSPQSegmentCount());
	return dxl_to_plan_stmt_translator.GetPlannedStmtFromDXL(dxlnode,
															 can_set_tag);
}


//---------------------------------------------------------------------------
//	@function:
//		COptTasks::LoadSearchStrategy
//
//	@doc:
//		Load search strategy from given file
//
//---------------------------------------------------------------------------
CSearchStageArray *
COptTasks::LoadSearchStrategy(CMemoryPool *mp, char *path)
{
	CSearchStageArray *search_strategy_arr = NULL;
	CParseHandlerDXL *dxl_parse_handler = NULL;

	SPQOS_TRY
	{
		if (NULL != path)
		{
			dxl_parse_handler =
				CDXLUtils::GetParseHandlerForDXLFile(mp, path, NULL);
			if (NULL != dxl_parse_handler)
			{
				elog(DEBUG2, "\n[OPT]: Using search strategy in (%s)", path);

				search_strategy_arr = dxl_parse_handler->GetSearchStageArray();
				search_strategy_arr->AddRef();
			}
		}
	}
	SPQOS_CATCH_EX(ex)
	{
		if (SPQOS_MATCH_EX(ex, spqdxl::ExmaSPQDB, spqdxl::ExmiSPQDBError))
		{
			SPQOS_RETHROW(ex);
		}
		elog(DEBUG2, "\n[OPT]: Using default search strategy");
		SPQOS_RESET_EX;
	}
	SPQOS_CATCH_END;

	SPQOS_DELETE(dxl_parse_handler);

	return search_strategy_arr;
}

//---------------------------------------------------------------------------
//	@function:
//		COptTasks::CreateOptimizerConfig
//
//	@doc:
//		Create the optimizer configuration
//
//---------------------------------------------------------------------------
COptimizerConfig *
COptTasks::CreateOptimizerConfig(CMemoryPool *mp, ICostModel *cost_model)
{
	// get chosen plan number, cost threshold
	ULLONG plan_id = (ULLONG) u_sess->attr.attr_spq.spq_optimizer_plan_id;
	ULLONG num_samples = (ULLONG) u_sess->attr.attr_spq.spq_optimizer_samples_number;
	DOUBLE cost_threshold = (DOUBLE) u_sess->attr.attr_spq.spq_optimizer_cost_threshold;

	DOUBLE damping_factor_filter = (DOUBLE) u_sess->attr.attr_spq.spq_optimizer_damping_factor_filter;
	DOUBLE damping_factor_join = (DOUBLE) u_sess->attr.attr_spq.spq_optimizer_damping_factor_join;
	DOUBLE damping_factor_groupby = (DOUBLE) u_sess->attr.attr_spq.spq_optimizer_damping_factor_groupby;

	ULONG cte_inlining_cutoff = (ULONG) u_sess->attr.attr_spq.spq_optimizer_cte_inlining_bound;
	ULONG join_arity_for_associativity_commutativity =
		(ULONG) u_sess->attr.attr_spq.spq_optimizer_join_arity_for_associativity_commutativity;
	ULONG array_expansion_threshold =
		(ULONG) u_sess->attr.attr_spq.spq_optimizer_array_expansion_threshold;
	ULONG join_order_threshold = (ULONG) u_sess->attr.attr_spq.spq_optimizer_join_order_threshold;
	ULONG broadcast_threshold = (ULONG) u_sess->attr.attr_spq.spq_optimizer_penalize_broadcast_threshold;
	ULONG push_group_by_below_setop_threshold =
		(ULONG) u_sess->attr.attr_spq.spq_optimizer_push_group_by_below_setop_threshold;
	ULONG xform_bind_threshold = (ULONG) u_sess->attr.attr_spq.spq_optimizer_xform_bind_threshold;
	ULONG skew_factor = (ULONG) u_sess->attr.attr_spq.spq_optimizer_skew_factor;

	return SPQOS_NEW(mp) COptimizerConfig(
		SPQOS_NEW(mp)
			CEnumeratorConfig(mp, plan_id, num_samples, cost_threshold),
		SPQOS_NEW(mp)
			CStatisticsConfig(mp, damping_factor_filter, damping_factor_join,
							  damping_factor_groupby, MAX_STATS_BUCKETS),
		SPQOS_NEW(mp) CCTEConfig(cte_inlining_cutoff), cost_model,
		SPQOS_NEW(mp)
			CHint(join_arity_for_associativity_commutativity,
				  array_expansion_threshold, join_order_threshold,
				  broadcast_threshold,
				  false, /* don't create Assert nodes for constraints, we'll
								      * enforce them ourselves in the executor */
				  push_group_by_below_setop_threshold, xform_bind_threshold,
				  skew_factor),
		SPQOS_NEW(mp) CWindowOids(OID(F_WINDOW_ROW_NUMBER), OID(F_WINDOW_RANK)));
}

//---------------------------------------------------------------------------
//		@function:
//			COptTasks::SetCostModelParams
//
//      @doc:
//			Set cost model parameters
//
//---------------------------------------------------------------------------
void
COptTasks::SetCostModelParams(ICostModel *cost_model)
{
	SPQOS_ASSERT(NULL != cost_model);

	if (u_sess->attr.attr_spq.spq_optimizer_nestloop_factor > 1.0)
	{
		// change NLJ cost factor
		ICostModelParams::SCostParam *cost_param =
			cost_model->GetCostModelParams()->PcpLookup(
				CCostModelParamsSPQDB::EcpNLJFactor);
		CDouble nlj_factor(u_sess->attr.attr_spq.spq_optimizer_nestloop_factor);
		cost_model->GetCostModelParams()->SetParam(
			cost_param->Id(), nlj_factor, nlj_factor - 0.5, nlj_factor + 0.5);
	}

	if (u_sess->attr.attr_spq.spq_optimizer_sort_factor > 1.0 ||
            u_sess->attr.attr_spq.spq_optimizer_sort_factor < 1.0)
	{
		// change sort cost factor
		ICostModelParams::SCostParam *cost_param =
			cost_model->GetCostModelParams()->PcpLookup(
				CCostModelParamsSPQDB::EcpSortTupWidthCostUnit);

		CDouble sort_factor(u_sess->attr.attr_spq.spq_optimizer_sort_factor);
		cost_model->GetCostModelParams()->SetParam(
			cost_param->Id(), cost_param->Get() * u_sess->attr.attr_spq.spq_optimizer_sort_factor,
			cost_param->GetLowerBoundVal() * u_sess->attr.attr_spq.spq_optimizer_sort_factor,
			cost_param->GetUpperBoundVal() * u_sess->attr.attr_spq.spq_optimizer_sort_factor);
	}
    /* SPQ: for ShareScan */
    if (u_sess->attr.attr_spq.spq_optimizer_share_tablescan_factor != 1.0)
    {
        // change sort cost factor
        ICostModelParams::SCostParam *cost_param =
            cost_model->GetCostModelParams()->PcpLookup(
                CCostModelParamsSPQDB::EcpSPQTableShareScanFactor);
        CDouble factor(u_sess->attr.attr_spq.spq_optimizer_share_tablescan_factor);
        cost_model->GetCostModelParams()->SetParam(cost_param->Id(),
                            cost_param->Get() * factor,
                            cost_param->GetLowerBoundVal() * factor,
                            cost_param->GetUpperBoundVal() * factor);
    }
    /* SPQ: for shareindexscan */
    if (u_sess->attr.attr_spq.spq_optimizer_share_indexscan_factor != 1.0)
    {
        // change sort cost factor
        ICostModelParams::SCostParam *cost_param =
            cost_model->GetCostModelParams()->PcpLookup(
                CCostModelParamsSPQDB::EcpSPQShareIndexScanFactor);
        CDouble factor(u_sess->attr.attr_spq.spq_optimizer_share_indexscan_factor);
        cost_model->GetCostModelParams()->SetParam(cost_param->Id(),
                            cost_param->Get() * factor,
                            cost_param->GetLowerBoundVal() * factor,
                            cost_param->GetUpperBoundVal() * factor);
    }
    /* SPQ: for hashJoin*/
    if (u_sess->attr.attr_spq.spq_optimizer_hashjoin_spilling_mem_threshold != 50 * 1024 * 1024)
    {
        // change memory threshold for hash join spilling
        ICostModelParams::SCostParam *cost_param =
            cost_model->GetCostModelParams()->PcpLookup(
                CCostModelParamsSPQDB::EcpHJSpillingMemThreshold);
        CDouble factor(u_sess->attr.attr_spq.spq_optimizer_hashjoin_spilling_mem_threshold);
        cost_model->GetCostModelParams()->SetParam(cost_param->Id(), factor, factor, factor);
    }
    if (u_sess->attr.attr_spq.spq_optimizer_hashjoin_inner_cost_factor != 1.0)
    {
        // change hash join outer width factor
        ICostModelParams::SCostParam *cost_param =
            cost_model->GetCostModelParams()->PcpLookup(
                CCostModelParamsSPQDB::EcpHJInnerCostFactor);
        CDouble factor(u_sess->attr.attr_spq.spq_optimizer_hashjoin_inner_cost_factor);
        cost_model->GetCostModelParams()->SetParam(cost_param->Id(), factor, factor, factor);
    }
}


//---------------------------------------------------------------------------
//      @function:
//			COptTasks::GetCostModel
//
//      @doc:
//			Generate an instance of optimizer cost model
//
//---------------------------------------------------------------------------
ICostModel *
COptTasks::GetCostModel(CMemoryPool *mp, ULONG num_segments)
{
	ICostModel *cost_model = SPQOS_NEW(mp) CCostModelSPQDB(mp, num_segments);

	SetCostModelParams(cost_model);

	return cost_model;
}

//---------------------------------------------------------------------------
//	@function:
//		COptTasks::OptimizeTask
//
//	@doc:
//		task that does the optimizes query to physical DXL
//
//---------------------------------------------------------------------------
void *
COptTasks::OptimizeTask(void *ptr)
{
	SPQOS_ASSERT(NULL != ptr);
	SOptContext *opt_ctxt = SOptContext::Cast(ptr);

	SPQOS_ASSERT(NULL != opt_ctxt->m_query);
	SPQOS_ASSERT(NULL == opt_ctxt->m_plan_dxl);
	SPQOS_ASSERT(NULL == opt_ctxt->m_plan_stmt);

	AUTO_MEM_POOL(amp);
	CMemoryPool *mp = amp.Pmp();

	// Does the metadatacache need to be reset?
	//
	// On the first call, before the cache has been initialized, we
	// don't care about the return value of MDCacheNeedsReset(). But
	// we need to call it anyway, to give it a chance to initialize
	// the invalidation mechanism.
	bool reset_mdcache = spqdb::MDCacheNeedsReset();

	// initialize metadata cache, or purge if needed, or change size if requested
	if (!CMDCache::FInitialized())
	{
		CMDCache::Init();
		CMDCache::SetCacheQuota(u_sess->attr.attr_spq.spq_optimizer_mdcache_size * 1024L);
	}
	else if (reset_mdcache)
	{
		CMDCache::Reset();
		CMDCache::SetCacheQuota(u_sess->attr.attr_spq.spq_optimizer_mdcache_size * 1024L);
	}
	else if (CMDCache::ULLGetCacheQuota() !=
			 (ULLONG) u_sess->attr.attr_spq.spq_optimizer_mdcache_size * 1024L)
	{
		CMDCache::SetCacheQuota(u_sess->attr.attr_spq.spq_optimizer_mdcache_size * 1024L);
	}


	// load search strategy
    char *optimizer_search_strategy_path = nullptr;
	CSearchStageArray *search_strategy_arr =
	LoadSearchStrategy(mp, optimizer_search_strategy_path);

	CBitSet *trace_flags = NULL;
	CBitSet *enabled_trace_flags = NULL;
	CBitSet *disabled_trace_flags = NULL;
	CDXLNode *plan_dxl = NULL;

	IMdIdArray *col_stats = NULL;
	MdidHashSet *rel_stats = NULL;

	SPQOS_TRY
	{
		// set trace flags
		trace_flags = CConfigParamMapping::PackConfigParamInBitset(
			mp, CXform::ExfSentinel);
		SetTraceflags(mp, trace_flags, &enabled_trace_flags,
					  &disabled_trace_flags);

		// set up relcache MD provider
		CMDProviderRelcache *relcache_provider =
			SPQOS_NEW(mp) CMDProviderRelcache(mp);

		{
			// scope for MD accessor
			CMDAccessor mda(mp, CMDCache::Pcache(), default_sysid,
							relcache_provider);

			ULONG num_segments = spqdb::GetSPQSegmentCount();
			ULONG num_segments_for_costing = 0;//optimizer_segments;
			if (0 == num_segments_for_costing)
			{
				num_segments_for_costing = num_segments;
			}

			CAutoP<CTranslatorQueryToDXL> query_to_dxl_translator;
			query_to_dxl_translator = CTranslatorQueryToDXL::QueryToDXLInstance(
				mp, &mda, (Query *) opt_ctxt->m_query);

			ICostModel *cost_model = GetCostModel(mp, num_segments_for_costing);
			COptimizerConfig *optimizer_config =
				CreateOptimizerConfig(mp, cost_model);
			CConstExprEvaluatorProxy expr_eval_proxy(mp, &mda);
			IConstExprEvaluator *expr_evaluator =
				SPQOS_NEW(mp) CConstExprEvaluatorDXL(mp, &mda, &expr_eval_proxy);

			CDXLNode *query_dxl =
				query_to_dxl_translator->TranslateQueryToDXL();
			CDXLNodeArray *query_output_dxlnode_array =
				query_to_dxl_translator->GetQueryOutputCols();
			CDXLNodeArray *cte_dxlnode_array =
				query_to_dxl_translator->GetCTEs();
			SPQOS_ASSERT(NULL != query_output_dxlnode_array);

			BOOL is_master_only =
				false || //!optimizer_enable_motions ||
				(true &&//(!optimizer_enable_motions_masteronly_queries &&
				 !query_to_dxl_translator->HasDistributedTables());
			// See NoteDistributionPolicyOpclasses() in src/backend/spq_optimizer_util/translate/CTranslatorQueryToDXL.cpp
			BOOL use_legacy_opfamilies =
				(query_to_dxl_translator->GetDistributionHashOpsKind() ==
				 DistrUseLegacyHashOps);
			CAutoTraceFlag atf1(EopttraceDisableMotions, is_master_only);
			CAutoTraceFlag atf2(EopttraceUseLegacyOpfamilies,
								use_legacy_opfamilies);
            if (u_sess->attr.attr_spq.spq_optimizer_log) {
                CWStringDynamic wstr(mp);
                COstreamString os(&wstr);
                CXMLSerializer xml_serializer(mp, os, true);
                query_dxl->SerializeToDXL(&xml_serializer);
                CHAR *sz = CUtils::CreateMultiByteCharStringFromWCString(mp, const_cast<WCHAR *>(wstr.GetBuffer()));
                elog(INFO, "\n[query_dxl]: %s\n", sz);
			}
            int spq_session_id = 0;
            int spq_command_count = 0;

			plan_dxl = COptimizer::PdxlnOptimize(
				mp, &mda, query_dxl, query_output_dxlnode_array,
				cte_dxlnode_array, expr_evaluator, num_segments, spq_session_id,
				spq_command_count, search_strategy_arr, optimizer_config);

			if (opt_ctxt->m_should_serialize_plan_dxl)
			{
				// serialize DXL to xml
				CWStringDynamic plan_str(mp);
				COstreamString oss(&plan_str);
				CDXLUtils::SerializePlan(
					mp, oss, plan_dxl,
					optimizer_config->GetEnumeratorCfg()->GetPlanId(),
					optimizer_config->GetEnumeratorCfg()->GetPlanSpaceSize(),
					true /*serialize_header_footer*/, true /*indentation*/);
				opt_ctxt->m_plan_dxl =
					CreateMultiByteCharStringFromWCString(plan_str.GetBuffer());
			}

			// translate DXL->PlStmt only when needed
			if (opt_ctxt->m_should_generate_plan_stmt)
			{
				// always use opt_ctxt->m_query->can_set_tag as the query_to_dxl_translator->Pquery() is a mutated Query object
				// that may not have the correct can_set_tag
				opt_ctxt->m_plan_stmt =
					(PlannedStmt *) spqdb::CopyObject(ConvertToPlanStmtFromDXL(
						mp, &mda, plan_dxl, opt_ctxt->m_query->canSetTag,
						query_to_dxl_translator->GetDistributionHashOpsKind(), opt_ctxt->m_query));

                        if (u_sess->attr.attr_spq.spq_optimizer_log) {
                            CWStringDynamic wstr(mp);
                            COstreamString os(&wstr);
                            CXMLSerializer xml_serializer(mp, os, true);
                            plan_dxl->SerializeToDXL(&xml_serializer);
                            CHAR *sz = CUtils::CreateMultiByteCharStringFromWCString(mp, const_cast<WCHAR *>(wstr.GetBuffer()));
                            elog(INFO, "\n[plan_dxl]: %s\n", sz);
                        }

			}

			CStatisticsConfig *stats_conf = optimizer_config->GetStatsConf();
			col_stats = SPQOS_NEW(mp) IMdIdArray(mp);
			stats_conf->CollectMissingStatsColumns(col_stats);

			rel_stats = SPQOS_NEW(mp) MdidHashSet(mp);
			PrintMissingStatsWarning(mp, &mda, col_stats, rel_stats);

			rel_stats->Release();
			col_stats->Release();

			expr_evaluator->Release();
			query_dxl->Release();
			optimizer_config->Release();
			plan_dxl->Release();
		}
	}
	SPQOS_CATCH_EX(ex)
	{
		ResetTraceflags(enabled_trace_flags, disabled_trace_flags);
		CRefCount::SafeRelease(rel_stats);
		CRefCount::SafeRelease(col_stats);
		CRefCount::SafeRelease(enabled_trace_flags);
		CRefCount::SafeRelease(disabled_trace_flags);
		CRefCount::SafeRelease(trace_flags);
		CRefCount::SafeRelease(plan_dxl);
		CMDCache::Shutdown();

		IErrorContext *errctxt = CTask::Self()->GetErrCtxt();

		opt_ctxt->m_should_error_out = ShouldErrorOut(ex);
		opt_ctxt->m_is_unexpected_failure = IsLoggableFailure(ex);
		opt_ctxt->m_error_msg =
			CreateMultiByteCharStringFromWCString(errctxt->GetErrorMsg());

		SPQOS_RETHROW(ex);
	}
	SPQOS_CATCH_END;

	// cleanup
	ResetTraceflags(enabled_trace_flags, disabled_trace_flags);
	CRefCount::SafeRelease(enabled_trace_flags);
	CRefCount::SafeRelease(disabled_trace_flags);
	CRefCount::SafeRelease(trace_flags);
	if (false/*!optimizer_metadata_caching*/)
	{
		CMDCache::Shutdown();
	}

	return NULL;
}


//---------------------------------------------------------------------------
//	@function:
//		COptTasks::PrintMissingStatsWarning
//
//	@doc:
//		Print warning messages for columns with missing statistics
//
//---------------------------------------------------------------------------
void
COptTasks::PrintMissingStatsWarning(CMemoryPool *mp, CMDAccessor *md_accessor,
									IMdIdArray *col_stats,
									MdidHashSet *rel_stats)
{
	SPQOS_ASSERT(NULL != md_accessor);
	SPQOS_ASSERT(NULL != col_stats);
	SPQOS_ASSERT(NULL != rel_stats);

	CWStringDynamic wcstr(mp);
	COstreamString oss(&wcstr);

	const ULONG num_missing_col_stats = col_stats->Size();
	for (ULONG ul = 0; ul < num_missing_col_stats; ul++)
	{
		IMDId *mdid = (*col_stats)[ul];
		CMDIdColStats *mdid_col_stats = CMDIdColStats::CastMdid(mdid);

		IMDId *rel_mdid = mdid_col_stats->GetRelMdId();
		const ULONG pos = mdid_col_stats->Position();
		const IMDRelation *rel = md_accessor->RetrieveRel(rel_mdid);

		if (IMDRelation::ErelstorageExternal != rel->RetrieveRelStorageType())
		{
			if (!rel_stats->Contains(rel_mdid))
			{
				if (0 != ul)
				{
					oss << ", ";
				}

				rel_mdid->AddRef();
				rel_stats->Insert(rel_mdid);
				oss << rel->Mdname().GetMDName()->GetBuffer();
			}

			CMDName mdname = rel->GetMdCol(pos)->Mdname();

			char msgbuf[NAMEDATALEN * 2 + 100];
			snprintf(msgbuf, sizeof(msgbuf),
					 "Missing statistics for column: %s.%s",
					 CreateMultiByteCharStringFromWCString(
						 rel->Mdname().GetMDName()->GetBuffer()),
					 CreateMultiByteCharStringFromWCString(
						 mdname.GetMDName()->GetBuffer()));
			GpdbEreport(ERRCODE_SUCCESSFUL_COMPLETION, LOG, msgbuf, NULL);
		}
	}

	if (0 < rel_stats->Size())
	{
		int length = NAMEDATALEN * rel_stats->Size() + 200;
		char msgbuf[length];
		snprintf(
			msgbuf, sizeof(msgbuf),
			"One or more columns in the following table(s) do not have statistics: %s",
			CreateMultiByteCharStringFromWCString(wcstr.GetBuffer()));
		GpdbEreport(
			ERRCODE_SUCCESSFUL_COMPLETION, LOG, msgbuf,
			"For non-partitioned tables, run analyze <table_name>(<column_list>)."
			" For partitioned tables, run analyze rootpartition <table_name>(<column_list>)."
			" See log for columns missing statistics.");
	}
}

//---------------------------------------------------------------------------
//	@function:
//		COptTasks::Optimize
//
//	@doc:
//		optimizes a query to physical DXL
//
//---------------------------------------------------------------------------
char *
COptTasks::Optimize(Query *query)
{
	Assert(query);

	SOptContext spqopt_context;
	spqopt_context.m_query = query;
	spqopt_context.m_should_serialize_plan_dxl = true;
	Execute(&OptimizeTask, &spqopt_context);

	// clean up context
	spqopt_context.Free(spqopt_context.epinQuery, spqopt_context.epinPlanDXL);

	return spqopt_context.m_plan_dxl;
}


//---------------------------------------------------------------------------
//	@function:
//		COptTasks::SPQOPTOptimizedPlan
//
//	@doc:
//		optimizes a query to plannedstmt
//
//---------------------------------------------------------------------------
PlannedStmt *
COptTasks::SPQOPTOptimizedPlan(Query *query, SOptContext *spqopt_context)
{
	Assert(query);
	Assert(spqopt_context);

	spqopt_context->m_query = query;
	spqopt_context->m_should_generate_plan_stmt = true;
	Execute(&OptimizeTask, spqopt_context);
	return spqopt_context->m_plan_stmt;
}

//---------------------------------------------------------------------------
//	@function:
//		COptTasks::SetXform
//
//	@doc:
//		Enable/Disable a given xform
//
//---------------------------------------------------------------------------
bool
COptTasks::SetXform(char *xform_str, bool should_disable)
{
	CXform *xform = CXformFactory::Pxff()->Pxf(xform_str);
	if (NULL != xform)
	{
		//optimizer_xforms[xform->Exfid()] = should_disable;

		return true;
	}

	return false;
}

// EOF
