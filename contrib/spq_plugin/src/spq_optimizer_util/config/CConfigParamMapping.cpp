//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CConfigParamMapping.cpp
//
//	@doc:
//		Implementation of SPQDB config params->trace flags mapping
//
//	@test:
//
//
//---------------------------------------------------------------------------

#include "postgres.h"

#include "utils/guc.h"
#include "spq_optimizer_util/config/CConfigParamMapping.h"
#include "spqopt/xforms/CXform.h"
#include "spq/spq_util.h"
#include "knl/knl_session.h"
#include "guc_spq.h"

using namespace spqos;
using namespace spqdxl;
using namespace spqopt;

// array mapping GUCs to traceflags
void CConfigParamMapping::InitConfigParamElements(CBitSet *traceflag_bitset)
{
 	SConfigMappingElem elements[] = {
	{EopttracePrintQuery, &u_sess->attr.attr_spq.spq_optimizer_print_query,
	 false,	 // m_negate_param
	 SPQOS_WSZ_LIT("Prints the optimizer's input query expression tree.")},

	{EopttracePrintPlan, &u_sess->attr.attr_spq.spq_optimizer_print_plan,
	 false,	 // m_negate_param
	 SPQOS_WSZ_LIT(
		 "Prints the plan expression tree produced by the optimizer.")},

	{EopttracePrintXform, &u_sess->attr.attr_spq.spq_optimizer_print_xform,
	 false,	 // m_negate_param
	 SPQOS_WSZ_LIT(
		 "Prints the input and output expression trees of the optimizer transformations.")},

	{EopttracePrintXformResults, &u_sess->attr.attr_spq.spq_optimizer_print_xform_results,
	 false,	 // m_negate_param
	 SPQOS_WSZ_LIT("Print input and output of xforms.")},

	{EopttracePrintMemoAfterExploration,
	 &u_sess->attr.attr_spq.spq_optimizer_print_memo_after_exploration,
	 false,	 // m_negate_param
	 SPQOS_WSZ_LIT("Prints MEMO after exploration.")},

	{EopttracePrintMemoAfterImplementation,
	 &u_sess->attr.attr_spq.spq_optimizer_print_memo_after_implementation,
	 false,	 // m_negate_param
	 SPQOS_WSZ_LIT("Prints MEMO after implementation.")},

	{EopttracePrintMemoAfterOptimization,
	 &u_sess->attr.attr_spq.spq_optimizer_print_memo_after_optimization,
	 false,	 // m_negate_param
	 SPQOS_WSZ_LIT("Prints MEMO after optimization.")},

	{EopttracePrintJobScheduler, &u_sess->attr.attr_spq.spq_optimizer_print_job_scheduler,
	 false,	 // m_negate_param
	 SPQOS_WSZ_LIT("Prints jobs in scheduler on each job completion.")},

	{EopttracePrintExpressionProperties, &u_sess->attr.attr_spq.spq_optimizer_print_expression_properties,
	 false,	 // m_negate_param
	 SPQOS_WSZ_LIT("Prints expression properties.")},

	{EopttracePrintGroupProperties, &u_sess->attr.attr_spq.spq_optimizer_print_group_properties,
	 false,	 // m_negate_param
	 SPQOS_WSZ_LIT("Prints group properties.")},

	{EopttracePrintOptimizationContext, &u_sess->attr.attr_spq.spq_optimizer_print_optimization_context,
	 false,	 // m_negate_param
	 SPQOS_WSZ_LIT("Prints optimization context.")},

    {EopttracePrintOptimizationCost, &u_sess->attr.attr_spq.spq_optimizer_print_optimization_cost,
    false, // m_negate_param
    SPQOS_WSZ_LIT("Prints optimization cost.")},

	{EopttracePrintOptimizationStatistics, &u_sess->attr.attr_spq.spq_optimizer_print_optimization_stats,
	 false,	 // m_negate_param
	 SPQOS_WSZ_LIT("Prints optimization stats.")},

	{EopttraceMinidump,
	 // SPQDB_91_MERGE_FIXME: I turned optimizer_minidump from bool into
	 // an enum-type GUC. It's a bit dirty to cast it like this..
	 (bool *) &u_sess->attr.attr_spq.spq_optimizer_minidump,
	 false,	 // m_negate_param
	 SPQOS_WSZ_LIT("Generate optimizer minidump.")},

	{EopttraceDisableMotions, &u_sess->attr.attr_spq.spq_optimizer_enable_motions,
	 true,	// m_negate_param
	 SPQOS_WSZ_LIT("Disable motion nodes in optimizer.")},

	{EopttraceDisableMotionBroadcast, &u_sess->attr.attr_spq.spq_optimizer_enable_motion_broadcast,
	 true,	// m_negate_param
	 SPQOS_WSZ_LIT("Disable motion broadcast nodes in optimizer.")},

	{EopttraceDisableMotionGather, &u_sess->attr.attr_spq.spq_optimizer_enable_motion_gather,
	 true,	// m_negate_param
	 SPQOS_WSZ_LIT("Disable motion gather nodes in optimizer.")},

	{EopttraceDisableMotionHashDistribute,
	 &u_sess->attr.attr_spq.spq_optimizer_enable_motion_redistribute,
	 true,	// m_negate_param
	 SPQOS_WSZ_LIT("Disable motion hash-distribute nodes in optimizer.")},

	{EopttraceDisableMotionRandom, &u_sess->attr.attr_spq.spq_optimizer_enable_motion_redistribute,
	 true,	// m_negate_param
	 SPQOS_WSZ_LIT("Disable motion random nodes in optimizer.")},

	{EopttraceDisableMotionRountedDistribute,
	 &u_sess->attr.attr_spq.spq_optimizer_enable_motion_redistribute,
	 true,	// m_negate_param
	 SPQOS_WSZ_LIT("Disable motion routed-distribute nodes in optimizer.")},

	{EopttraceDisableSort, &u_sess->attr.attr_spq.spq_optimizer_enable_sort,
	 true,	// m_negate_param
	 SPQOS_WSZ_LIT("Disable sort nodes in optimizer.")},

	{EopttraceDisableSpool, &u_sess->attr.attr_spq.spq_optimizer_enable_materialize,
	 true,	// m_negate_param
	 SPQOS_WSZ_LIT("Disable spool nodes in optimizer.")},

	{EopttraceDisablePartPropagation, &u_sess->attr.attr_spq.spq_optimizer_enable_partition_propagation,
	 true,	// m_negate_param
	 SPQOS_WSZ_LIT("Disable partition propagation nodes in optimizer.")},

	{EopttraceDisablePartSelection, &u_sess->attr.attr_spq.spq_optimizer_enable_partition_selection,
	 true,	// m_negate_param
	 SPQOS_WSZ_LIT("Disable partition selection in optimizer.")},

	{EopttraceDisableOuterJoin2InnerJoinRewrite,
	 &u_sess->attr.attr_spq.spq_optimizer_enable_outerjoin_rewrite,
	 true,	// m_negate_param
	 SPQOS_WSZ_LIT("Disable outer join to inner join rewrite in optimizer.")},

	{EopttraceDonotDeriveStatsForAllGroups,
	 &u_sess->attr.attr_spq.spq_optimizer_enable_derive_stats_all_groups,
	 true,	// m_negate_param
	 SPQOS_WSZ_LIT("Disable deriving stats for all groups after exploration.")},

	{EopttraceEnableSpacePruning, &u_sess->attr.attr_spq.spq_optimizer_enable_space_pruning,
	 false,	 // m_negate_param
	 SPQOS_WSZ_LIT("Enable space pruning in optimizer.")},

	{EopttraceForceMultiStageAgg, &u_sess->attr.attr_spq.spq_optimizer_force_multistage_agg,
	 false,	 // m_negate_param
	 SPQOS_WSZ_LIT(
		 "Force optimizer to always pick multistage aggregates when such a plan alternative is generated.")},

	{EopttracePrintColsWithMissingStats, &u_sess->attr.attr_spq.spq_optimizer_print_missing_stats,
	 false,	 // m_negate_param
	 SPQOS_WSZ_LIT("Print columns with missing statistics.")},

	{EopttraceEnableRedistributeBroadcastHashJoin,
	 &u_sess->attr.attr_spq.spq_optimizer_enable_hashjoin_redistribute_broadcast_children,
	 false,	 // m_negate_param
	 SPQOS_WSZ_LIT(
		 "Enable generating hash join plan where outer child is Redistribute and inner child is Broadcast.")},

	{EopttraceExtractDXLStats, &u_sess->attr.attr_spq.spq_optimizer_extract_dxl_stats,
	 false,	 // m_negate_param
	 SPQOS_WSZ_LIT("Extract plan stats in dxl.")},

	{EopttraceExtractDXLStatsAllNodes, &u_sess->attr.attr_spq.spq_optimizer_extract_dxl_stats_all_nodes,
	 false,	 // m_negate_param
	 SPQOS_WSZ_LIT("Extract plan stats for all physical dxl nodes.")},

	{EopttraceDeriveStatsForDPE, &u_sess->attr.attr_spq.spq_optimizer_dpe_stats,
	 false,	 // m_negate_param
	 SPQOS_WSZ_LIT(
		 "Enable stats derivation of partitioned tables with dynamic partition elimination.")},

	{EopttraceEnumeratePlans, &u_sess->attr.attr_spq.spq_optimizer_enumerate_plans,
	 false,	 // m_negate_param
	 SPQOS_WSZ_LIT("Enable plan enumeration.")},

	{EopttraceSamplePlans, &u_sess->attr.attr_spq.spq_optimizer_sample_plans,
	 false,	 // m_negate_param
	 SPQOS_WSZ_LIT("Enable plan sampling.")},

	{EopttraceEnableCTEInlining, &u_sess->attr.attr_spq.spq_optimizer_cte_inlining,
	 false,	 // m_negate_param
	 SPQOS_WSZ_LIT("Enable CTE inlining.")},

	{EopttraceEnableConstantExpressionEvaluation,
	 &u_sess->attr.attr_spq.spq_optimizer_enable_constant_expression_evaluation,
	 false,	 // m_negate_param
	 SPQOS_WSZ_LIT("Enable constant expression evaluation in the optimizer")},

	{EopttraceUseExternalConstantExpressionEvaluationForInts,
	 &u_sess->attr.attr_spq.spq_optimizer_use_external_constant_expression_evaluation_for_ints,
	 false,	 // m_negate_param
	 SPQOS_WSZ_LIT(
		 "Enable constant expression evaluation for integers in the optimizer")},

	{EopttraceApplyLeftOuter2InnerUnionAllLeftAntiSemiJoinDisregardingStats,
	 &u_sess->attr.attr_spq.spq_optimizer_apply_left_outer_to_union_all_disregarding_stats,
	 false,	 // m_negate_param
	 SPQOS_WSZ_LIT(
		 "Always apply Left Outer Join to Inner Join UnionAll Left Anti Semi Join without looking at stats")},

    {EopttraceRemoveSuperfluousOrder,
     &u_sess->attr.attr_spq.spq_optimizer_remove_superfluous_order,
     false,  // m_negate_param
     SPQOS_WSZ_LIT("Remove superfluous OrderBy")},

	{EopttraceRemoveOrderBelowDML, &u_sess->attr.attr_spq.spq_optimizer_remove_order_below_dml,
	 false,	 // m_negate_param
	 SPQOS_WSZ_LIT("Remove OrderBy below a DML operation")},

	{EopttraceDisableReplicateInnerNLJOuterChild,
	 &u_sess->attr.attr_spq.spq_optimizer_enable_broadcast_nestloop_outer_child,
	 true,	// m_negate_param
	 SPQOS_WSZ_LIT(
		 "Enable plan alternatives where NLJ's outer child is replicated")},

	{EopttraceDiscardRedistributeHashJoin,
	 &u_sess->attr.attr_spq.spq_optimizer_discard_redistribute_hashjoin,
	 false,	 // m_negate_param
	 SPQOS_WSZ_LIT(
		 "Discard plan alternatives where hash join has a redistribute motion child")},

	{EopttraceMotionHazardHandling, &u_sess->attr.attr_spq.spq_optimizer_enable_streaming_material,
	 false,	 // m_fNegate
	 SPQOS_WSZ_LIT(
		 "Enable motion hazard handling during NLJ optimization and generate streaming material when appropriate")},

	{EopttraceDisableNonMasterGatherForDML,
	 &u_sess->attr.attr_spq.spq_optimizer_enable_gather_on_segment_for_dml,
	 true,	// m_fNegate
	 SPQOS_WSZ_LIT(
		 "Enable DML optimization by enforcing a non-master gather when appropriate")},

	{EopttraceEnforceCorrelatedExecution, &u_sess->attr.attr_spq.spq_optimizer_enforce_subplans,
	 false,	 // m_negate_param
	 SPQOS_WSZ_LIT("Enforce correlated execution in the optimizer")},

	{EopttraceForceExpandedMDQAs, &u_sess->attr.attr_spq.spq_optimizer_force_expanded_distinct_aggs,
	 false,	 // m_negate_param
	 SPQOS_WSZ_LIT(
		 "Always pick plans that expand multiple distinct aggregates into join of single distinct aggregate in the optimizer")},

	{EopttraceDisablePushingCTEConsumerReqsToCTEProducer,
	 &u_sess->attr.attr_spq.spq_optimizer_push_requirements_from_consumer_to_producer,
	 true,	// m_negate_param
	 SPQOS_WSZ_LIT(
		 "Optimize CTE producer plan on requirements enforced on top of CTE consumer")},

	{EopttraceDisablePruneUnusedComputedColumns,
	 &u_sess->attr.attr_spq.spq_optimizer_prune_computed_columns,
	 true,	// m_negate_param
	 SPQOS_WSZ_LIT("Prune unused computed columns when pre-processing query")},

	{EopttraceForceThreeStageScalarDQA, &u_sess->attr.attr_spq.spq_optimizer_force_three_stage_scalar_dqa,
	 false,	 // m_negate_param
	 SPQOS_WSZ_LIT(
		 "Force optimizer to always pick 3 stage aggregate plan for scalar distinct qualified aggregate.")},

	{EopttraceEnableParallelAppend, &u_sess->attr.attr_spq.spq_optimizer_parallel_union,
	 false,	 // m_negate_param
	 SPQOS_WSZ_LIT("Enable parallel execution for UNION/UNION ALL queries.")},

	{EopttraceArrayConstraints, &u_sess->attr.attr_spq.spq_optimizer_array_constraints,
	 false,	 // m_negate_param
	 SPQOS_WSZ_LIT(
		 "Allows the constraint framework to derive array constraints in the optimizer.")},

	{EopttraceForceAggSkewAvoidance, &u_sess->attr.attr_spq.spq_optimizer_force_agg_skew_avoidance,
	 false,	 // m_negate_param
	 SPQOS_WSZ_LIT(
		 "Always pick a plan for aggregate distinct that minimizes skew.")},

	{EopttraceEnableEagerAgg, &u_sess->attr.attr_spq.spq_optimizer_enable_eageragg,
	 false,	 // m_negate_param
	 SPQOS_WSZ_LIT(
		 "Enable Eager Agg transform for pushing aggregate below an innerjoin.")},

	{EopttraceEnableOrderedAgg, &u_sess->attr.attr_spq.spq_optimizer_enable_orderedagg,
	 false,	 // m_negate_param
	 SPQOS_WSZ_LIT("Enable ordered aggregate plans.")},

	{EopttraceExpandFullJoin, &u_sess->attr.attr_spq.spq_optimizer_expand_fulljoin,
	 false,	 // m_negate_param
	 SPQOS_WSZ_LIT(
		 "Enable Expand Full Join transform for converting FULL JOIN into UNION ALL.")},
	{EopttracePenalizeSkewedHashJoin, &u_sess->attr.attr_spq.spq_optimizer_penalize_skew,
	 true,	// m_negate_param
	 SPQOS_WSZ_LIT(
		 "Penalize a hash join with a skewed redistribute as a child.")},
	{EopttraceTranslateUnusedColrefs, &u_sess->attr.attr_spq.spq_optimizer_prune_unused_columns,
	 true,	// m_negate_param
	 SPQOS_WSZ_LIT("Prune unused columns from the query.")},
	{EopttraceAllowGeneralPredicatesforDPE,
	 &u_sess->attr.attr_spq.spq_optimizer_enable_range_predicate_dpe,
	 false,	 // m_negate_param
	 SPQOS_WSZ_LIT(
		 "Enable range predicates for dynamic partition elimination.")},
	{EopttraceEnableRedistributeNLLOJInnerChild,
	 &u_sess->attr.attr_spq.spq_optimizer_enable_redistribute_nestloop_loj_inner_child,
	 false,	 // m_negate_param
	 SPQOS_WSZ_LIT(
		 "Enable plan alternatives where NLJ's inner child is redistributed")},
	{EopttraceForceComprehensiveJoinImplementation,
	 &u_sess->attr.attr_spq.spq_optimizer_force_comprehensive_join_implementation,
	 false,	 // m_negate_param
	 SPQOS_WSZ_LIT(
		 "Explore a nested loop join even if a hash join is possible")},
	};
	for (ULONG ul = 0; ul < SPQOS_ARRAY_SIZE(elements); ul++) {
        SConfigMappingElem elem = elements[ul];
        SPQOS_ASSERT(!traceflag_bitset->Get((ULONG)elem.m_trace_flag) && "trace flag already set");

        BOOL value = *elem.m_is_param;
        if (elem.m_negate_param) {
            // negate the value of config param
            value = !value;
        }

        if (value) {
            BOOL is_traceflag_set SPQOS_ASSERTS_ONLY = traceflag_bitset->ExchangeSet((ULONG)elem.m_trace_flag);
            SPQOS_ASSERT(!is_traceflag_set);
        }
    }

}

//---------------------------------------------------------------------------
//	@function:
//		CConfigParamMapping::PackConfigParamInBitset
//
//	@doc:
//		Pack the SPQDB config params into a bitset
//
//---------------------------------------------------------------------------
CBitSet *
CConfigParamMapping::PackConfigParamInBitset(
	CMemoryPool *mp,
	ULONG xform_id	// number of available xforms
)
{
	CBitSet *traceflag_bitset = SPQOS_NEW(mp) CBitSet(mp, EopttraceSentinel);

    InitConfigParamElements(traceflag_bitset);

	// pack disable flags of xforms
	for (ULONG ul = 0; ul < xform_id; ul++)
	{
		SPQOS_ASSERT(!traceflag_bitset->Get(EopttraceDisableXformBase + ul) &&
					"xform trace flag already set");

		if (spq_xforms()[ul])
		{
			BOOL is_traceflag_set SPQOS_ASSERTS_ONLY =
				traceflag_bitset->ExchangeSet(EopttraceDisableXformBase + ul);
			SPQOS_ASSERT(!is_traceflag_set);
		}
	}

	if (!u_sess->attr.attr_spq.spq_optimizer_enable_indexjoin)
	{
		CBitSet *index_join_bitset = CXform::PbsIndexJoinXforms(mp);
		traceflag_bitset->Union(index_join_bitset);
		index_join_bitset->Release();
	}

	// disable bitmap scan if the corresponding GUC is turned off
	if (!u_sess->attr.attr_spq.spq_optimizer_enable_bitmapscan)
	{
		CBitSet *bitmap_index_bitset = CXform::PbsBitmapIndexXforms(mp);
		traceflag_bitset->Union(bitmap_index_bitset);
		bitmap_index_bitset->Release();
	}

	// disable outerjoin to unionall transformation if GUC is turned off
	if (!u_sess->attr.attr_spq.spq_optimizer_enable_outerjoin_to_unionall_rewrite)
	{
		traceflag_bitset->ExchangeSet(SPQOPT_DISABLE_XFORM_TF(
			CXform::ExfLeftOuter2InnerUnionAllLeftAntiSemiJoin));
	}

	// disable Assert MaxOneRow plans if GUC is turned off
	if (!u_sess->attr.attr_spq.spq_optimizer_enable_assert_maxonerow)
	{
		traceflag_bitset->ExchangeSet(
			SPQOPT_DISABLE_XFORM_TF(CXform::ExfMaxOneRow2Assert));
	}

	if (!u_sess->attr.attr_spq.spq_optimizer_enable_partial_index)
	{
		CBitSet *heterogeneous_index_bitset =
			CXform::PbsHeterogeneousIndexXforms(mp);
		traceflag_bitset->Union(heterogeneous_index_bitset);
		heterogeneous_index_bitset->Release();
	}

	if (!u_sess->attr.attr_spq.spq_optimizer_enable_hashjoin)
	{
		// disable hash-join if the corresponding GUC is turned off
		CBitSet *hash_join_bitste = CXform::PbsHashJoinXforms(mp);
		traceflag_bitset->Union(hash_join_bitste);
		hash_join_bitste->Release();
	}

	if (!u_sess->attr.attr_spq.spq_optimizer_enable_dynamictablescan)
	{
		// disable dynamic table scan if the corresponding GUC is turned off
		traceflag_bitset->ExchangeSet(
			SPQOPT_DISABLE_XFORM_TF(CXform::ExfDynamicGet2DynamicTableScan));
	}

	if (!u_sess->attr.attr_spq.spq_optimizer_enable_tablescan)
	{
		// disable table scan if the corresponding GUC is turned off
		traceflag_bitset->ExchangeSet(
			SPQOPT_DISABLE_XFORM_TF(CXform::ExfGet2TableScan));
	}
    /* SPQ: for ShareScan */
    if (!u_sess->attr.attr_spq.spq_optimizer_enable_seqsharescan)
    {
        // disable table scan if the corresponding GUC is turned off
        traceflag_bitset->ExchangeSet(
            SPQOPT_DISABLE_XFORM_TF(CXform::ExfGet2TableShareScan));
    }
    /* SPQ: for shareindexscan */
    if (!u_sess->attr.attr_spq.spq_optimizer_enable_shareindexscan)
    {
        // disable share index if the corresponding GUC is turned off
        traceflag_bitset->ExchangeSet(
            SPQOPT_DISABLE_XFORM_TF(CXform::ExfIndexGet2ShareIndexScan));
    }

	if (!u_sess->attr.attr_spq.spq_optimizer_enable_indexscan)
	{
		// disable index scan if the corresponding GUC is turned off
		traceflag_bitset->ExchangeSet(
			SPQOPT_DISABLE_XFORM_TF(CXform::ExfIndexGet2IndexScan));
	}

	if (!u_sess->attr.attr_spq.spq_optimizer_enable_indexonlyscan)
	{
		// disable index only scan if the corresponding GUC is turned off
		traceflag_bitset->ExchangeSet(
			SPQOPT_DISABLE_XFORM_TF(CXform::ExfIndexGet2IndexOnlyScan));
	}

	if (!u_sess->attr.attr_spq.spq_optimizer_enable_hashagg)
	{
		traceflag_bitset->ExchangeSet(
			SPQOPT_DISABLE_XFORM_TF(CXform::ExfGbAgg2HashAgg));
		traceflag_bitset->ExchangeSet(
			SPQOPT_DISABLE_XFORM_TF(CXform::ExfGbAggDedup2HashAggDedup));
	}

	if (!u_sess->attr.attr_spq.spq_optimizer_enable_groupagg)
	{
		traceflag_bitset->ExchangeSet(
			SPQOPT_DISABLE_XFORM_TF(CXform::ExfGbAgg2StreamAgg));
		traceflag_bitset->ExchangeSet(
			SPQOPT_DISABLE_XFORM_TF(CXform::ExfGbAggDedup2StreamAggDedup));
	}

	if (!u_sess->attr.attr_spq.spq_optimizer_enable_mergejoin)
	{
		traceflag_bitset->ExchangeSet(
			SPQOPT_DISABLE_XFORM_TF(CXform::ExfImplementFullOuterMergeJoin));
	}

	CBitSet *join_heuristic_bitset = NULL;
	switch (u_sess->attr.attr_spq.spq_optimizer_join_order)
	{
		case JOIN_ORDER_IN_QUERY:
			join_heuristic_bitset = CXform::PbsJoinOrderInQueryXforms(mp);
			break;
		case JOIN_ORDER_GREEDY_SEARCH:
			join_heuristic_bitset = CXform::PbsJoinOrderOnGreedyXforms(mp);
			break;
		case JOIN_ORDER_EXHAUSTIVE_SEARCH:
			join_heuristic_bitset = CXform::PbsJoinOrderOnExhaustiveXforms(mp);
			break;
		case JOIN_ORDER_EXHAUSTIVE2_SEARCH:
			join_heuristic_bitset = CXform::PbsJoinOrderOnExhaustive2Xforms(mp);
			break;
		default:
			elog(ERROR,
				 "Invalid value for spq_optimizer_join_order, must \
				 not come here");
			break;
	}
	traceflag_bitset->Union(join_heuristic_bitset);
	join_heuristic_bitset->Release();

	// disable join associativity transform if the corresponding GUC
	// is turned off independent of the join order algorithm chosen
	if (!u_sess->attr.attr_spq.spq_optimizer_enable_associativity)
	{
		traceflag_bitset->ExchangeSet(
			SPQOPT_DISABLE_XFORM_TF(CXform::ExfJoinAssociativity));
	}

	if (OPTIMIZER_SPQDB_LEGACY == u_sess->attr.attr_spq.spq_optimizer_cost_model)
	{
		traceflag_bitset->ExchangeSet(EopttraceLegacyCostModel);
	}
	else if (OPTIMIZER_SPQDB_EXPERIMENTAL == u_sess->attr.attr_spq.spq_optimizer_cost_model)
	{
		traceflag_bitset->ExchangeSet(EopttraceExperimentalCostModel);
	}

	// enable nested loop index plans using nest params
	// instead of outer reference as in the case with SPQDB 4/5
	traceflag_bitset->ExchangeSet(EopttraceIndexedNLJOuterRefAsParams);

	// enable using opfamilies in distribution specs for SPQDB 6
	traceflag_bitset->ExchangeSet(EopttraceConsiderOpfamiliesForDistribution);

	// disable external partitioned tables until feature complete
	traceflag_bitset->ExchangeClear(EopttraceEnableExternalPartitionedTables);
	traceflag_bitset->ExchangeSet(
		SPQOPT_DISABLE_XFORM_TF(CXform::ExfMultiExternalGet2MultiExternalScan));
	traceflag_bitset->ExchangeSet(SPQOPT_DISABLE_XFORM_TF(
		CXform::ExfExpandDynamicGetWithExternalPartitions));

	return traceflag_bitset;
}

// EOF
