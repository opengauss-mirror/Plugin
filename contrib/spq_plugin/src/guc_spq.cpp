/* --------------------------------------------------------------------
 * guc_spq.cpp
 *
 * Additional OpenGauss-SPQ-specific GUCs are defined in this file, to
 * avoid adding so much stuff to guc.c. This makes it easier to diff
 * and merge with upstream.
 *
 * Portions Copyright (c) 2005-2010, Greenplum inc
 * Portions Copyright (c) 2012-Present VMware, Inc. or its affiliates.
 * Copyright (c) 2000-2009, PostgreSQL Global Development Group
 * Copyright (c) 2022 Huawei Technologies Co.,Ltd.
 *
 * IDENTIFICATION
 * 	  src/backend/utils/misc/guc/guc_spq.cpp
 *
 * --------------------------------------------------------------------
 */
#include "postgres.h"

#include <float.h>
#include <sys/stat.h>
#include <sys/unistd.h>
#include "miscadmin.h"
#include "utils/builtins.h"
#include "utils/guc_tables.h"
#include "guc_spq.h"
#include "spq/spq_vars.h"

#include "spq/spq_util.h"
#include "pgxc/nodemgr.h"

const char* sync_guc_names_array[] = {
    "log_min_messages",
    "logging_module",
    "gauss_spq_print_direct_dispatch_info",
    "gauss_spq_interconnect_log",
    "gauss_spq_gang_log"
};

void make_spq_sync_options(StringInfo option)
{
    for (uint32 i = 0; i < sizeof(sync_guc_names_array) / sizeof(char*); i++) {
        const char* config = GetConfigOption(sync_guc_names_array[i], false, false);
        appendStringInfo(option, " -c %s=%s", sync_guc_names_array[i], config);
    }
}

static void InitSpqConfigureNamesBool();
static void InitSpqConfigureNamesInt();
static void InitSpqConfigureNamesReal();
static void InitSpqConfigureNamesString();
static void InitSpqConfigureNamesEnum();


static bool spq_check_scan_unit_size(int *newval, void **extra, GucSource source);
static const char* spq_show_scan_unit_size(void);
static bool check_spq_cluster_map(char **newval, void **extra, GucSource source);
static const char *show_spq_cluster_map(void);
void assign_spq_nodes(const char *newval, void *extra);

static const struct config_enum_entry spq_optimizer_minidump_options[] = {
	{"onerror", OPTIMIZER_MINIDUMP_FAIL, false},
	{"always", OPTIMIZER_MINIDUMP_ALWAYS, false},
	{NULL, 0, false}
};

static const struct config_enum_entry spq_optimizer_cost_model_options[] = {
	{"legacy", OPTIMIZER_SPQDB_LEGACY, false},
	{"calibrated", OPTIMIZER_SPQDB_CALIBRATED, false},
	{"experimental", OPTIMIZER_SPQDB_EXPERIMENTAL, false},
	{NULL, 0, false}
};

static const struct config_enum_entry spq_optimizer_join_order_options[] = {
	{"query", JOIN_ORDER_IN_QUERY, false},
	{"greedy", JOIN_ORDER_GREEDY_SEARCH, false},
	{"exhaustive", JOIN_ORDER_EXHAUSTIVE_SEARCH, false},
	{"exhaustive2", JOIN_ORDER_EXHAUSTIVE2_SEARCH, false},
	{NULL, 0, false}
};

static const struct config_enum_entry spq_log_verbosity[] = {
    {"terse", GAUSSVARS_VERBOSITY_TERSE},
    {"off", GAUSSVARS_VERBOSITY_OFF},
    {"verbose", GAUSSVARS_VERBOSITY_VERBOSE},
    {"debug", GAUSSVARS_VERBOSITY_DEBUG},
    {NULL, 0}
};

/*
 * Contents of GUC tables
 *
 * See src/backend/utils/misc/README for design notes.
 *
 * TO ADD AN OPTION AS FOLLOWS.
 *
 * 1. Declare a global variable of type bool, int, double, or char*
 * 	  and make use of it.
 *
 * 2. Decide at what times it's safe to set the option. See guc.h for
 * 	  details.
 *
 * 3. Decide on a name, a default value, upper and lower bounds (if
 * 	  applicable), etc.
 *
 * 4. Add a record below.
 *
 * 5. Add it to src/backend/utils/misc/postgresql_single.conf.sample or
 * 	  src/backend/utils/misc/postgresql_distribute.conf.sample or both,
 * 	  if appropriate.
 *
 * 6. Don't forget to document the option (at least in config.sgml).
 *
 * 7. If it's a new GUC_LIST option you must edit pg_dumpall.c to ensure
 * 	  it is not single quoted at dump time.
 */
/* ******* option records follow ******* */
void InitSpqConfigureNames()
{
    InitSpqConfigureNamesBool();
    InitSpqConfigureNamesInt();
    InitSpqConfigureNamesReal();
    InitSpqConfigureNamesString();
    InitSpqConfigureNamesEnum();

    return;
}

/* Optimizer related gucs */
static void InitSpqConfigureNamesBool()
{
    DefineCustomBoolVariable("spqplugin.enable_spq",
                             "Enable spq engine",
                             NULL,
                             &u_sess->attr.attr_spq.gauss_enable_spq,
                             false,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_log",
                             "Log optimizer messages.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_log,
                             false,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_metadata_caching",
                             "This guc enables the optimizer to cache and reuse metadata.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_metadata_caching,
                             true,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_use_gauss_allocators",
                             "Enable ORCA to use OpenGauss Memory Contexts.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_use_gauss_allocators,
                             false,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_print_query",
                             "Prints the optimizer's input query expression tree.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_print_query,
                             false,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_print_plan",
                             "Prints the plan expression tree produced by the optimizer.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_print_plan,
                             false,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_print_xform",
                             "Prints optimizer transformation information.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_print_xform,
                             false,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_print_xform_results",
                             "Print the input and output of optimizer transformations.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_print_xform_results,
                             false,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_print_memo_after_exploration",
                             "Print optimizer memo structure after the exploration phase.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_print_memo_after_exploration,
                             false,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_print_memo_after_implementation",
                             "Print optimizer memo structure after the implementation phase.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_print_memo_after_implementation,
                             false,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_print_memo_after_optimization",
                             "Print optimizer memo structure after the optimization phase.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_print_memo_after_optimization,
                             false,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_print_job_scheduler",
                             "Print the jobs in the scheduler on each job completion.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_print_job_scheduler,
                             false,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_print_expression_properties",
                             "Print expression properties.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_print_expression_properties,
                             false,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_print_group_properties",
                             "Print group properties.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_print_group_properties,
                             false,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_print_optimization_context",
                             "Print the optimization context.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_print_optimization_context,
                             false,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_print_optimization_cost",
                             "Print the optimization cost.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_print_optimization_cost,
                             false,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_print_optimization_stats",
                             "Print optimization stats.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_print_optimization_stats,
                             false,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_enable_nljoin",
                             "Enable nested loops join plans in the optimizer.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_enable_nljoin,
                             true,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_enable_indexjoin",
                             "Enable index nested loops join plans in the optimizer.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_enable_indexjoin,
                             true,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_enable_motions_masteronly_queries",
                             "Enable plans with Motion operators in the optimizer for queries with no distributed tables.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_enable_motions_masteronly_queries,
                             false,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_enable_motions",
                             "Enable plans with Motion operators in the optimizer.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_enable_motions,
                             true,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_enable_motion_broadcast",
                             "Enable plans with Motion Broadcast operators in the optimizer.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_enable_motion_broadcast,
                             true,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_enable_motion_gather",
                             "Enable plans with Motion Gather operators in the optimizer.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_enable_motion_gather,
                             true,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_enable_motion_redistribute",
                             "Enable plans with Motion Redistribute operators in the optimizer.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_enable_motion_redistribute,
                             true,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_enable_sort",
                             "Enable plans with Sort operators in the optimizer.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_enable_sort,
                             true,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_enable_materialize",
                             "Enable plans with Materialize operators in the optimizer.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_enable_materialize,
                             true,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_enable_partition_propagation",
                             "Enable plans with Partition Propagation operators in the optimizer.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_enable_partition_propagation,
                             true,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_enable_partition_selection",
                             "Enable plans with Partition Selection operators in the optimizer.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_enable_partition_selection,
                             true,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_enable_outerjoin_rewrite",
                             "Enable outer join to inner join rewrite in the optimizer.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_enable_outerjoin_rewrite,
                             true,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_enable_multiple_distinct_aggs",
                             "Enable plans with multiple distinct aggregates in the optimizer.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_enable_multiple_distinct_aggs,
                             false,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_enable_direct_dispatch",
                             "Enable direct dispatch in the optimizer.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_enable_direct_dispatch,
                             true,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_enable_hashjoin_redistribute_broadcast_children",
                             "Enable hash join plans with, Redistribute outer child and Broadcast inner child, in the optimizer.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_enable_hashjoin_redistribute_broadcast_children,
                             false,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_discard_redistribute_hashjoin",
                             "Discard redistribute hashjoin.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_discard_redistribute_hashjoin,
                             false,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_enable_broadcast_nestloop_outer_child",
                             "Enable nested loops join plans with replicated outer child in the optimizer.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_enable_broadcast_nestloop_outer_child,
                             true,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_enable_streaming_material",
                             "Enable plans with a streaming material node in the optimizer.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_enable_streaming_material,
                             true,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_enable_gather_on_segment_for_dml",
                             "Enable DML optimization by enforcing a non-master gather in the optimizer.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_enable_gather_on_segment_for_dml,
                             true,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_enable_assert_maxonerow",
                             "Enable Assert MaxOneRow plans to check number of rows at runtime.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_enable_assert_maxonerow,
                             true,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_enable_constant_expression_evaluation",
                             "Enable constant expression evaluation in the optimizer",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_enable_constant_expression_evaluation,
                             true,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_enable_bitmapscan",
                             "Enable bitmap plans in the optimizer",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_enable_bitmapscan,
                             true,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_enable_outerjoin_to_unionall_rewrite",
                             "Enable rewriting Left Outer Join to UnionAll",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_enable_outerjoin_to_unionall_rewrite,
                             false,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_enable_ctas",
                             "Enable CTAS plans in the optimizer",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_enable_ctas,
                             false,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_enable_partial_index",
                             "Enable heterogeneous index plans.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_enable_partial_index,
                             true,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_enable_dml",
                             "Enable DML plans in OpenGauss SPQ Optimizer.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_enable_dml,
                             false,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_enable_dml_triggers",
                             "Enable DML plans in OpenGauss SPQ Optimizer.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_enable_dml_triggers,
                             false,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_enable_dml_constraints",
                             "Support DML with CHECK constraints and NOT NULL constraints.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_enable_dml_constraints,
                             false,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_enable_master_only_queries",
                             "Process master only queries via the optimizer.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_enable_master_only_queries,
                             false,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_enable_hashjoin",
                             "Enables the optimizer's use of hash join plans.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_enable_hashjoin,
                             true,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_enable_dynamictablescan",
                             "Enables the optimizer's use of plans with dynamic table scan.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_enable_dynamictablescan,
                             true,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_enable_indexscan",
                             "Enables the optimizer's use of plans with index scan.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_enable_indexscan,
                             true,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_enable_indexonlyscan",
                             "Enables the optimizer's use of plans with index only scan.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_enable_indexonlyscan,
                             true,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_enable_tablescan",
                             "Enables the optimizer's use of plans with table scan.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_enable_tablescan,
                             true,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_enable_seqsharescan",
                             "Enables the optimizer's use of sequential-share-scan plans.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_enable_seqsharescan,
                             true,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_enable_shareindexscan",
                             "Enables the optimizer's use of share index scan plans.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_enable_shareindexscan,
                             true,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_enable_hashagg",
                             "Enables Pivotal Optimizer (GPORCA) to use hash aggregates.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_enable_hashagg,
                             true,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_enable_groupagg",
                             "Enables Pivotal Optimizer (GPORCA) to use group aggregates.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_enable_groupagg,
                             true,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_expand_fulljoin",
                             "Enables the optimizer's support of expanding full outer joins using union all.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_expand_fulljoin,
                             false,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_enable_mergejoin",
                             "Enables the optimizer's support of merge joins.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_enable_mergejoin,
                             true,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_prune_unused_columns",
                             "Prune unused table columns during query optimization.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_prune_unused_columns,
                             true,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_enable_redistribute_nestloop_loj_inner_child",
                             "Enable nested loops left join plans with redistributed inner child in the optimizer.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_enable_redistribute_nestloop_loj_inner_child,
                             true,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_force_comprehensive_join_implementation",
                             "Explore a nested loop join even if a hash join is possible",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_force_comprehensive_join_implementation,
                             false,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_enable_replicated_table",
                             "Enable replicated tables.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_enable_replicated_table,
                             true,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_enumerate_plans",
                             "Enable plan enumeration",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_enumerate_plans,
                             false,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_sample_plans",
                             "Enable plan sampling",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_sample_plans,
                             false,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_extract_dxl_stats",
                             "Extract plan stats in dxl.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_extract_dxl_stats,
                             false,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_extract_dxl_stats_all_nodes",
                             "Extract plan stats for all physical dxl nodes.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_extract_dxl_stats_all_nodes,
                             false,
                             PGC_USERSET,
                             0,   
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_print_missing_stats",
                             "Print columns with missing statistics.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_print_missing_stats,
                             true,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_dpe_stats",
                             "Enable statistics derivation for partitioned tables with dynamic partition elimination.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_dpe_stats,
                             true,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_enable_derive_stats_all_groups",
                             "Enable stats derivation for all groups after exploration.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_enable_derive_stats_all_groups,
                             false,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_force_multistage_agg",
                             "Force optimizer to always pick multistage aggregates when such a plan alternative is generated.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_force_multistage_agg,
                             false,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_force_three_stage_scalar_dqa",
                             "Force optimizer to always pick 3 stage aggregate plan for scalar distinct qualified aggregate.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_force_three_stage_scalar_dqa,
                             true,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_force_expanded_distinct_aggs",
                             "Always pick plans that expand multiple distinct aggregates into join of single distinct aggregate in the optimizer.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_force_expanded_distinct_aggs,
                             true,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_force_agg_skew_avoidance",
                             "Always pick a plan for aggregate distinct that minimizes skew.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_force_agg_skew_avoidance,
                             true,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_penalize_skew",
                             "Penalize operators with skewed hash redistribute below it.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_penalize_skew,
                             true,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_prune_computed_columns",
                             "Prune unused computed columns when pre-processing query",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_prune_computed_columns,
                             true,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_push_requirements_from_consumer_to_producer",
                             "Optimize CTE producer plan on requirements enforced on top of CTE consumer in the optimizer.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_push_requirements_from_consumer_to_producer,
                             true,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_enforce_subplans",
                             "Enforce correlated execution in the optimizer",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_enforce_subplans,
                             false,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_use_external_constant_expression_evaluation_for_ints",
                             "Use external constant expression evaluation in the optimizer for all integer types",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_use_external_constant_expression_evaluation_for_ints,
                             false,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_apply_left_outer_to_union_all_disregarding_stats",
                             "Always apply Left Outer Join to Inner Join UnionAll Left Anti Semi Join without looking at stats.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_apply_left_outer_to_union_all_disregarding_stats,
                             false,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_remove_superfluous_order",
                             "This guc remove superfluous order.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_remove_superfluous_order,
                             true,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_remove_order_below_dml",
                             "Remove OrderBy below a DML operation",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_remove_order_below_dml,
                             false,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_multilevel_partitioning",
                             "Enable optimization of queries on multilevel partitioned tables.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_multilevel_partitioning,
                             true,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_parallel_union",
                             "Enable parallel execution for UNION/UNION ALL queries.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_parallel_union,
                             false,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_array_constraints",
                             "Allows the optimizer's constraint framework to derive array constraints.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_array_constraints,
                             true,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_cte_inlining",
                             "Enable CTE inlining",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_cte_inlining,
                             false,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_enable_space_pruning",
                             "Enable space pruning in the optimizer.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_enable_space_pruning,
                             true,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_enable_associativity",
                             "Enables Join Associativity in optimizer",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_enable_associativity,
                             false,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_enable_eageragg",
                             "Enable Eager Agg transform for pushing aggregate below an innerjoin.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_enable_eageragg,
                             false,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_enable_orderedagg",
                             "Enable optimizer enable orderedagg.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_enable_orderedagg,
                             false,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_enable_range_predicate_dpe",
                             "Enable range predicates for dynamic partition elimination.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_enable_range_predicate_dpe,
                             false,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_enable_pre_optimizer_check",
                             "This guc allows spq enable pre optimizer check.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_enable_pre_optimizer_check,
                             true,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_enable_result_hash_filter",
                             "This guc allows spq support hash filter in result.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_enable_result_hash_filter,
                             false,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_debug_cancel_print",
                             "Print cancel detail information.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_debug_cancel_print,
                             false,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_debug_print_full_dtm",
                             "Prints full DTM information to server log.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_debug_print_full_dtm,
                             false,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_debug_slice_print",
                             "Prints slice detail information.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_debug_slice_print,
                             false,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_print_direct_dispatch_info",
                             "For testing purposes, print information about direct dispatch decisions.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_print_direct_dispatch_info,
                             false,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_log_dispatch_stats",
                             "Writes dispatcher performance statistics to the log.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_log_dispatch_stats,
                             false,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_enable_btbuild",
                             "Enable spq btbuild.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_enable_btbuild,
                             false,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_enable_btbuild_cic",
                             "Enable spq btbuild for create index concurrently.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_enable_btbuild_cic,
                             false,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_enable_adaptive_scan",
                             "Enable spq adaptive scan for dynamic table scan range.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_enable_adaptive_scan,
                             false,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_optimizer_calc_multiple_dop",
                             "Enable spq select 1 or n dop.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_calc_multiple_dop,
                             false,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_enable_transaction",
                             "Enable spq execution in transaction.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_enable_transaction,
                             false,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_enable_left_index_nestloop_join",
                             "Enable left index nestloop join.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_enable_left_index_nestloop_join,
                             true,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_enable_insert_select",
                             "Enable insert concurrently on spq.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_enable_insert_select,
                             false,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_enable_insert_from_tableless",
                             "Enable insert from tableless on spq.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_enable_insert_from_tableless,
                             false,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_enable_insert_order_sensitive",
                             "Enable parallel insert need ordered.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_enable_insert_order_sensitive,
                             true,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_enable_delete",
                             "Enable delete concurrently on spq.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_enable_delete,
                             false,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_enable_update",
                             "Enable update concurrently on spq.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_enable_update,
                             false,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_enable_direct_read",
                             "Enable spq direct read without buffer",
                             NULL,
                             &u_sess->attr.attr_spq.spq_enable_direct_read,
                             false,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_enable_remove_delete_redundant_motion",
                             "Remove delete redundant motion when delete_dop_num == read_dop_num.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_enable_remove_delete_redundant_motion,
                             false,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomBoolVariable("spqplugin.spq_enable_remove_update_redundant_motion",
                             "Remove update redundant motion when update_dop_num == read_dop_num.",
                             NULL,
                             &u_sess->attr.attr_spq.spq_enable_remove_update_redundant_motion,
                             false,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
}

static void InitSpqConfigureNamesInt()
{
    DefineCustomIntVariable("spqplugin.spq_optimizer_mdcache_size",
                            "Sets the size of MDCache.",
                            NULL,
                            &u_sess->attr.attr_spq.spq_optimizer_mdcache_size,
                            16384,
                            0,
                            INT_MAX,
                            PGC_USERSET,
                            GUC_UNIT_KB,
                            NULL,
                            NULL,
                            NULL);
    DefineCustomIntVariable("spqplugin.spq_optimizer_plan_id",
                            "Choose a plan alternative.",
                            NULL,
                            &u_sess->attr.attr_spq.spq_optimizer_plan_id,
                            0,
                            0,
                            INT_MAX,
                            PGC_USERSET,
                            0,
                            NULL,
                            NULL,
                            NULL);
    DefineCustomIntVariable("spqplugin.spq_optimizer_samples_number",
                            "Set the number of plan samples",
                            NULL,
                            &u_sess->attr.attr_spq.spq_optimizer_samples_number,
                            1000,
                            1,
                            INT_MAX,
                            PGC_USERSET,
                            0,
                            NULL,
                            NULL,
                            NULL);
    DefineCustomIntVariable("spqplugin.spq_optimizer_penalize_broadcast_threshold",
                            "Maximum number of rows of a relation that can be broadcasted without penalty.",
                            NULL,
                            &u_sess->attr.attr_spq.spq_optimizer_penalize_broadcast_threshold,
                            100000,
                            0,
                            INT_MAX,
                            PGC_USERSET,
                            0,
                            NULL,
                            NULL,
                            NULL);
    DefineCustomIntVariable("spqplugin.spq_optimizer_join_arity_for_associativity_commutativity",
                            "Maximum number of children n-ary-join have without disabling commutativity and associativity transform",
                            NULL,
                            &u_sess->attr.attr_spq.spq_optimizer_join_arity_for_associativity_commutativity,
                            18,
                            0,
                            INT_MAX,
                            PGC_USERSET,
                            0,
                            NULL,
                            NULL,
                            NULL);
    DefineCustomIntVariable("spqplugin.spq_optimizer_array_expansion_threshold",
                            "Item limit for expansion of arrays in WHERE clause for constraint derivation.",
                            NULL,
                            &u_sess->attr.attr_spq.spq_optimizer_array_expansion_threshold,
                            100,
                            0,
                            INT_MAX,
                            PGC_USERSET,
                            0,
                            NULL,
                            NULL,
                            NULL);
    DefineCustomIntVariable("spqplugin.spq_optimizer_join_order_threshold",
                            "Maximum number of join children to use dynamic programming based join ordering algorithm.",
                            NULL,
                            &u_sess->attr.attr_spq.spq_optimizer_join_order_threshold,
                            10,
                            0,
                            12,
                            PGC_USERSET,
                            0,
                            NULL,
                            NULL,
                            NULL);
    DefineCustomIntVariable("spqplugin.spq_optimizer_cte_inlining_bound",
                            "Set the CTE inlining cutoff",
                            NULL,
                            &u_sess->attr.attr_spq.spq_optimizer_cte_inlining_bound,
                            0,
                            0,
                            INT_MAX,
                            PGC_USERSET,
                            0,
                            NULL,
                            NULL,
                            NULL);
    DefineCustomIntVariable("spqplugin.spq_optimizer_push_group_by_below_setop_threshold",
                            "Maximum number of children setops have to consider pushing group bys below it",
                            NULL,
                            &u_sess->attr.attr_spq.spq_optimizer_push_group_by_below_setop_threshold,
                            10,
                            0,
                            INT_MAX,
                            PGC_USERSET,
                            0,
                            NULL,
                            NULL,
                            NULL);
    DefineCustomIntVariable("spqplugin.spq_optimizer_xform_bind_threshold",
                            "Maximum number bindings per xform per group expression",
                            NULL,
                            &u_sess->attr.attr_spq.spq_optimizer_xform_bind_threshold,
                            0,
                            0,
                            INT_MAX,
                            PGC_USERSET,
                            0,
                            NULL,
                            NULL,
                            NULL);
    DefineCustomIntVariable("spqplugin.spq_optimizer_skew_factor",
                            "Maximum number bskew factor",
                            NULL,
                            &u_sess->attr.attr_spq.spq_optimizer_skew_factor,
                            0,
                            0,
                            INT_MAX,
                            PGC_USERSET,
                            0,
                            NULL,
                            NULL,
                            NULL);
    DefineCustomIntVariable("spqplugin.spq_scan_unit_size",
                            "Sets size of scan unit when using Spq Parallel Execution",
                            NULL,
                            &u_sess->attr.attr_spq.spq_scan_unit_size,
                            512,
                            1,
                            1024,
                            PGC_USERSET,
                            GUC_UNIT_BLOCKS,
                            spq_check_scan_unit_size,
                            NULL,
                            spq_show_scan_unit_size);
    DefineCustomIntVariable("spqplugin.spq_batch_size",
                            "Sets spq batch size",
                            NULL,
                            &u_sess->attr.attr_spq.spq_batch_size,
                            512,
                            16,
                            INT_MAX,
                            PGC_USERSET,
                            0,
                            NULL,
                            NULL,
                            NULL);
    DefineCustomIntVariable("spqplugin.spq_mem_size",
                            "Sets spq mem size",
                            NULL,
                            &u_sess->attr.attr_spq.spq_mem_size,
                            512,
                            16,
                            INT_MAX,
                            PGC_USERSET,
                            0,
                            NULL,
                            NULL,
                            NULL);
    DefineCustomIntVariable("spqplugin.spq_queue_size",
                            "Sets spq queue size",
                            NULL,
                            &u_sess->attr.attr_spq.spq_queue_size,
                            1024,
                            128,
                            INT_MAX,
                            PGC_USERSET,
                            0,
                            NULL,
                            NULL,
                            NULL);
    DefineCustomIntVariable("spqplugin.spq_insert_dop_num",
                            "Sets spq the num of insert dop",
                            NULL,
                            &u_sess->attr.attr_spq.spq_insert_dop_num,
                            1,
                            1,
                            1,
                            PGC_USERSET,
                            0,
                            NULL,
                            NULL,
                            NULL);
    DefineCustomIntVariable("spqplugin.spq_delete_dop_num",
                            "Sets spq the num of delete dop",
                            NULL,
                            &u_sess->attr.attr_spq.spq_delete_dop_num,
                            1,
                            1,
                            1,
                            PGC_USERSET,
                            0,
                            NULL,
                            NULL,
                            NULL);
    DefineCustomIntVariable("spqplugin.spq_update_dop_num",
                            "Sets spq the num of update dop",
                            NULL,
                            &u_sess->attr.attr_spq.spq_update_dop_num,
                            1,
                            1,
                            1,
                            PGC_USERSET,
                            0,
                            NULL,
                            NULL,
                            NULL);
}

static void InitSpqConfigureNamesReal()
{
    DefineCustomRealVariable("spqplugin.spq_optimizer_damping_factor_filter",
                             "select predicate damping factor in optimizer, 1.0 means no damping",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_damping_factor_filter,
                             0.75,
                             0.0,
                             1.0,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomRealVariable("spqplugin.spq_optimizer_damping_factor_join",
                             "join predicate damping factor in optimizer, 1.0 means no damping, 0.0 means square root method",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_damping_factor_join,
                             0.0,
                             0.0,
                             1.0,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomRealVariable("spqplugin.spq_optimizer_hashjoin_spilling_mem_threshold",
                             "memory threshold for hash join spilling (in bytes), 50M is defult value",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_hashjoin_spilling_mem_threshold,
                             50 * 1024 * 1024,
                             0.0,
                             DBL_MAX,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomRealVariable("spqplugin.spq_optimizer_hashjoin_inner_cost_factor",
                             "hash join inner cost factor in optimizer, reducing the cost for inner",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_hashjoin_inner_cost_factor,
                             1.0,
                             0.0,
                             1.0,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomRealVariable("spqplugin.spq_optimizer_damping_factor_groupby",
                             "groupby operator damping factor in optimizer, 1.0 means no damping",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_damping_factor_groupby,
                             0.75,
                             0.0,
                             1.0,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomRealVariable("spqplugin.spq_optimizer_cost_threshold",
                             "Set the threshold for plan sampling relative to the cost of best plan, 0.0 means unbounded",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_cost_threshold,
                             0.0,
                             0.0,
                             INT_MAX,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomRealVariable("spqplugin.spq_optimizer_nestloop_factor",
                             "Set the nestloop join cost factor in the optimizer",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_nestloop_factor,
                             1024.0,
                             1.0,
                             DBL_MAX,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomRealVariable("spqplugin.spq_optimizer_sort_factor",
                             "Set the sort cost factor in the optimizer, 1.0 means same as default, > 1.0 means more costly than default, < 1.0 means means less costly than default",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_sort_factor,
                             1.0,
                             0.0,
                             DBL_MAX,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomRealVariable("spqplugin.spq_optimizer_share_tablescan_factor",
                             "Set the table sharescan cost factor in the optimizer, 1.0 means same as default, > 1.0 means more costly than default, < 1.0 means means less costly than default",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_share_tablescan_factor,
                             1.0,
                             0.0,
                             DBL_MAX,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomRealVariable("spqplugin.spq_optimizer_share_indexscan_factor",
                             "Set the share indexscan cost factor in the optimizer, 1.0 means same as default, > 1.0 means more costly than default, < 1.0 means means less costly than default",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_share_indexscan_factor,
                             1.0,
                             0.0,
                             DBL_MAX,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomRealVariable("spqplugin.spq_small_table_threshold",
                             "Set the threshold for small tables. The actual value is spq_small_table_threshold * NORMAL_SHARED_BUFFER_NUM",
                             NULL,
                             &u_sess->attr.attr_spq.spq_small_table_threshold,
                             0.02,
                             0.0,
                             1.0,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
}

static void InitSpqConfigureNamesString()
{
    DefineCustomStringVariable("spqplugin.cluster_map",
                               "gauss_cluster_map",
                               NULL,
                               &u_sess->attr.attr_spq.gauss_cluster_map,
                               "",
                               PGC_USERSET,
                               GUC_NO_RESET_ALL | GUC_NO_SHOW_ALL,
                               check_spq_cluster_map,
                               NULL,
                               show_spq_cluster_map);
}

static void InitSpqConfigureNamesEnum() {
    DefineCustomEnumVariable("spqplugin.spq_optimizer_minidump",
                             "Valid values are onerror, always",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_minidump,
                             OPTIMIZER_MINIDUMP_FAIL,
                             spq_optimizer_minidump_options,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomEnumVariable("spqplugin.spq_optimizer_cost_model",
                             "Valid values are legacy, calibrated, experimental",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_cost_model,
                             OPTIMIZER_SPQDB_CALIBRATED,
                             spq_optimizer_cost_model_options,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
    DefineCustomEnumVariable("spqplugin.spq_optimizer_join_order",
                             "Valid values are query, greedy, exhaustive and exhaustive2",
                             NULL,
                             &u_sess->attr.attr_spq.spq_optimizer_join_order,
                             JOIN_ORDER_EXHAUSTIVE2_SEARCH,
                             spq_optimizer_join_order_options,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);
}

static bool spq_check_scan_unit_size(int *newval, void **extra, GucSource source)
{
    if ((*newval <= 0) || (*newval & (*newval - 1)) != 0) {
        GUC_check_errdetail("scan unit size in blocks, it should be power of 2, min size is 1 block");
        return false;
    }

    {
        int size = *newval;
        u_sess->attr.attr_spq.spq_scan_unit_bit = 0;
        while (size > 1) {
            size = size >> 1;
            u_sess->attr.attr_spq.spq_scan_unit_bit++;
        }
    }

    return true;
}

static const char* spq_show_scan_unit_size(void)
{
	static char nbuf[120];
	snprintf(nbuf, sizeof(nbuf), "scan_unit_size: %d, scan_unit_bit: %d",
		u_sess->attr.attr_spq.spq_scan_unit_size, u_sess->attr.attr_spq.spq_scan_unit_bit);
	return nbuf;
}

static bool spq_verify_gauss_cluster_map_syntax(const char *liststring)
{
    if (liststring == nullptr || liststring[0] == '\0') {
        GUC_check_errdetail("spq cluster map is null");
        return false;
    }
    char *rawname = nullptr;
    List *nodelist = nullptr;
    int array_size;
    NodeDefinition* nodesDefinition = nullptr;
    NodeDefinition* node;
    int idx = 0;
    errno_t rc;
    MemoryContext oldContext = MemoryContextSwitchTo(u_sess->spq_cxt.spq_worker_context);
    rawname = pstrdup(liststring);
    if (rawname == nullptr) {
        GUC_check_errdetail("spq cluster map pstrdup is null");
        MemoryContextSwitchTo(oldContext);
        return false;
    }
    if (!SplitIdentifierString(rawname, ',', &nodelist)) {
        pfree(rawname);
        /* syntax error in name list */
        GUC_check_errdetail("spq cluster map is invalid, name|ip|port,...");
        MemoryContextSwitchTo(oldContext);
        return false;
    }
    array_size = list_length(nodelist);
    // mem build in spq_cxt->spq_worker_context
    nodesDefinition = (NodeDefinition *)palloc0(sizeof(NodeDefinition) * (array_size));
    foreach_cell(lnode, nodelist) {
        node = &nodesDefinition[idx];
        List *itemlist;
        char *name, *ip, *port;
        char *nodestring = pstrdup((char *)lfirst(lnode));
        (void)SplitIdentifierString(nodestring, '|', &itemlist);
        if (list_length(itemlist) != 3) {
            GUC_check_errdetail("spq cluster map is invalid, name|ip|port,...");
            pfree(rawname);
            pfree(nodestring);
            list_free(nodelist);
            list_free(itemlist);
            pfree(nodesDefinition);
            MemoryContextSwitchTo(oldContext);
            return false;
        }
        name = (char *)list_nth(itemlist, 0);
        ip = (char *)list_nth(itemlist, 1);
        port = (char *)list_nth(itemlist, 2);
        node->nodeid = idx;
        rc = strncpy_s(node->nodename.data, NAMEDATALEN, name, NAMEDATALEN);
        securec_check_c(rc, "\0", "\0");
        rc = strncpy_s(node->nodehost.data, NAMEDATALEN, ip, NAMEDATALEN);
        securec_check_c(rc, "\0", "\0");
        rc = strncpy_s(node->nodehost1.data, NAMEDATALEN, ip, NAMEDATALEN);
        securec_check_c(rc, "\0", "\0");
        node->nodeport = (int)strtol(port, NULL, 10);
        idx++;
        pfree(nodestring);
        list_free(itemlist);
    }
    pfree(rawname);
    list_free(nodelist);
    for (int i = 0; i < array_size; ++i) {
        for (int j = i + 1; j < array_size; ++j) {
            if (strcmp(nodesDefinition[i].nodename.data, nodesDefinition[j].nodename.data) == 0) {
                GUC_check_errdetail("spq cluster map is invalid, have duplicate node name.");
                pfree(nodesDefinition);
                MemoryContextSwitchTo(oldContext);
                return false;
            }
        }
    }
    if (t_thrd.spq_ctx.nodesDefinition != nullptr) {
        pfree(t_thrd.spq_ctx.nodesDefinition);
        t_thrd.spq_ctx.nodesDefinition = nullptr;
    }
    t_thrd.spq_ctx.num_nodes = array_size;
    t_thrd.spq_ctx.nodesDefinition = nodesDefinition;
    MemoryContextSwitchTo(oldContext);
    return true;
}

static bool check_spq_cluster_map(char **newval, void **extra, GucSource source)
{
    if (source == PGC_S_DEFAULT) {
        return true;
    }
    return spq_verify_gauss_cluster_map_syntax(*newval);
}

static const char *show_spq_cluster_map(void)
{
    return u_sess->attr.attr_spq.gauss_cluster_map;
}
