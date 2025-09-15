/*
 * Copyright (c) 2025 Huawei Technologies Co.,Ltd.
 *
 * openGauss is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *
 *          http://license.coscl.org.cn/MulanPSL2
 *
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 * --------------------------------------------------------------------------------------
 *
 * session_ctx.cc
 *  Sesson_ctx provides some session variables.
 *
 * --------------------------------------------------------------------------------------
 */
#include "postgres.h"

#include "commands/seclabel.h"
#include "distributed/commands.h"
#include "distributed/session_ctx.h"
#include "distributed/backend_data.h"
#include "distributed/shared_connection_stats.h"
#include "distributed/shard_cleaner.h"
#include "distributed/utils/spq_io_multiplex.h"
#include "distributed/worker_shard_visibility.h"
#include "distributed/listutils.h"
#include "commands/extension.h"
#include "spq_version.h"

static char* SpqVersion = SPQ_VERSION;
static constexpr int GUC_STANDARD = 0;
static constexpr int MAX_SHARD_COUNT = 64000;
static constexpr int MAX_SHARD_REPLICATION_FACTOR = 100;
static constexpr uint32 INVALID_EXTENSION_NO = -1;

uint32 Session_ctx::CURRENT_EXTENSION_NO = INVALID_EXTENSION_NO;

extern const char* GetThreadName(knl_thread_role role);

void set_extension_index(uint32 index)
{
    Session_ctx::CURRENT_EXTENSION_NO = index;
}

void init_session_vars(void)
{
    RepallocSessionVarsArrayIfNecessary();
    Assert(u_sess->attr.attr_common
               .extension_session_vars_array[Session_ctx::CURRENT_EXTENSION_NO] ==
           nullptr);

    auto ctxMemCtx = AllocSetContextCreate(
        u_sess->self_mem_cxt, "SpqSessionContextMemoryContext", ALLOCSET_DEFAULT_MINSIZE,
        ALLOCSET_DEFAULT_INITSIZE, ALLOCSET_DEFAULT_MAXSIZE);

    /* we first invoke constructor to initialize all variables. */
    Session_ctx* session_ctx =
        new (MemoryContextAlloc(ctxMemCtx, sizeof(Session_ctx))) Session_ctx(ctxMemCtx);

    /* Assign the value to void recursively invoke. */
    u_sess->attr.attr_common
        .extension_session_vars_array[Session_ctx::CURRENT_EXTENSION_NO] = session_ctx;

    session_ctx->InitializeCtx();

    /*
     * This part is only for SECURITY LABEL tests
     * mimicking what an actual security label provider would do
     */
    if (Session_ctx::Vars().RunningUnderCitusTestSuite) {
        register_label_provider("spq '!tests_label_provider", citus_test_object_relabel);
    }

    if (t_thrd.role != WORKER && t_thrd.role != THREADPOOL_WORKER) {
        ereport(DEBUG1, (errmsg("Spq has been loaded by non-worker thread: %s",
                                GetThreadName(t_thrd.role))));
    }
}

/* GUC enum definitions */
static const struct config_enum_entry propagate_set_commands_options[] = {
    {"none", PROPSETCMD_NONE, false},
    {"local", PROPSETCMD_LOCAL, false},
    {NULL, 0, false}};

static const struct config_enum_entry coordinator_aggregation_options[] = {
    {"disabled", COORDINATOR_AGGREGATION_DISABLED, false},
    {"row-gather", COORDINATOR_AGGREGATION_ROW_GATHER, false},
    {NULL, 0, false}};

static const struct config_enum_entry explain_analyze_sort_method_options[] = {
    {"execution-time", EXPLAIN_ANALYZE_SORT_BY_TIME, false},
    {"taskId", EXPLAIN_ANALYZE_SORT_BY_TASK_ID, false},
    {NULL, 0, false}};

static const struct config_enum_entry multi_shard_modify_connection_options[] = {
    {"parallel", PARALLEL_CONNECTION, false},
    {"sequential", SEQUENTIAL_CONNECTION, false},
    {NULL, 0, false}};

static const struct config_enum_entry local_table_join_policies[] = {
    {"never", LOCAL_JOIN_POLICY_NEVER, false},
    {"prefer-local", LOCAL_JOIN_POLICY_PREFER_LOCAL, false},
    {"prefer-distributed", LOCAL_JOIN_POLICY_PREFER_DISTRIBUTED, false},
    {"auto", LOCAL_JOIN_POLICY_AUTO, false},
    {NULL, 0, false}};

static const struct config_enum_entry log_level_options[] = {
    {"off", CITUS_LOG_LEVEL_OFF, false},
    {"debug5", DEBUG5, false},
    {"debug4", DEBUG4, false},
    {"debug3", DEBUG3, false},
    {"debug2", DEBUG2, false},
    {"debug1", DEBUG1, false},
    {"debug", DEBUG2, true},
    {"log", LOG, false},
    {"info", INFO, true},
    {"notice", NOTICE, false},
    {"warning", WARNING, false},
    {"error", ERROR, false},
    {NULL, 0, false}};

static const struct config_enum_entry task_executor_type_options[] = {
    {"adaptive", MULTI_EXECUTOR_ADAPTIVE, false},
    {"task-tracker", MULTI_EXECUTOR_ADAPTIVE, false},
    {NULL, 0, false}};

static const struct config_enum_entry stat_statements_track_options[] = {
    {"none", STAT_STATEMENTS_TRACK_NONE, false},
    {"all", STAT_STATEMENTS_TRACK_ALL, false},
    {NULL, 0, false}};

/* support never only for now */
static const struct config_enum_entry use_secondary_nodes_options[] = {
    {"never", USE_SECONDARY_NODES_NEVER, false},
#ifdef ENABLE_ALL_CONFIGUARABLE_PARAMETERS
    {"always", USE_SECONDARY_NODES_ALWAYS, false},
#endif
    {NULL, 0, false}};

#ifdef ENABLE_ALL_CONFIGUARABLE_PARAMETERS
static const struct config_enum_entry task_assignment_policy_options[] = {
    {"greedy", TASK_ASSIGNMENT_GREEDY, false},
    {"first-replica", TASK_ASSIGNMENT_FIRST_REPLICA, false},
    {"round-robin", TASK_ASSIGNMENT_ROUND_ROBIN, false},
    {NULL, 0, false}};

static const struct config_enum_entry create_object_propagation_options[] = {
    {"deferred", CREATE_OBJECT_PROPAGATION_DEFERRED, false},
    {"automatic", CREATE_OBJECT_PROPAGATION_AUTOMATIC, false},
    {"immediate", CREATE_OBJECT_PROPAGATION_IMMEDIATE, false},
    {NULL, 0, false}};

/*
 * This used to choose CPU priorities for GUCs. For most other integer options
 * we use the -1 value as inherit/default/unset. For CPU priorities this isn't
 * possible, because they can actually have negative values. So we need a value
 * outside of the range that's valid for priorities. But if this is only one
 * more or less than the valid values, this can also be quite confusing for
 * people that don't know the exact range of valid values.
 *
 * So, instead we opt for using an enum that contains all valid priority values
 * as strings, as well as the "inherit" string to indicate that the priority
 * value should not be changed.
 */
static const struct config_enum_entry cpu_priority_options[] = {
    {"inherit", CPU_PRIORITY_INHERIT, false},
    {"-20", -20, false},
    {"-19", -19, false},
    {"-18", -18, false},
    {"-17", -17, false},
    {"-16", -16, false},
    {"-15", -15, false},
    {"-14", -14, false},
    {"-13", -13, false},
    {"-12", -12, false},
    {"-11", -11, false},
    {"-10", -10, false},
    {"-9", -9, false},
    {"-8", -8, false},
    {"-7", -7, false},
    {"-6", -6, false},
    {"-5", -5, false},
    {"-4", -4, false},
    {"-3", -3, false},
    {"-2", -2, false},
    {"-1", -1, false},
    {"0", 0, false},
    {"1", 1, false},
    {"2", 2, false},
    {"3", 3, false},
    {"4", 4, false},
    {"5", 5, false},
    {"6", 6, false},
    {"7", 7, false},
    {"8", 8, false},
    {"9", 9, false},
    {"10", 10, false},
    {"11", 11, false},
    {"12", 12, false},
    {"13", 13, false},
    {"14", 14, false},
    {"15", 15, false},
    {"16", 16, false},
    {"17", 17, false},
    {"18", 18, false},
    {"19", 19, false},
    {NULL, 0, false}};

static const struct config_enum_entry metadata_sync_mode_options[] = {
    {"transactional", METADATA_SYNC_TRANSACTIONAL, false},
    {"nontransactional", METADATA_SYNC_NON_TRANSACTIONAL, false},
    {NULL, 0, false}};
#endif

bool Session_variables::ErrorIfNotASuitableDeadlockFactor(double* newval, void** extra,
                                                          GucSource source)
{
    if (*newval <= 1.0 && *newval != -1.0) {
        ereport(WARNING, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                          errmsg("spq.distributed_deadlock_detection_factor "
                                 "cannot be less than 1. "
                                 "To disable distributed deadlock detection "
                                 "set the value to -1.")));

        return false;
    }

    return true;
}

bool Session_variables::StatisticsCollectionGucCheckHook(bool* newval, void** extra,
                                                         GucSource source)
{
#ifdef HAVE_LIBCURL
    return true;
#else

    /* if libcurl is not installed, only accept false */
    if (*newval) {
        GUC_check_errcode(ERRCODE_FEATURE_NOT_SUPPORTED);
        GUC_check_errdetail("spq was compiled without libcurl support.");
        return false;
    } else {
        return true;
    }
#endif
}

const char* Session_variables::LocalPoolSizeGucShowHook(void)
{
    StringInfo newvalue = makeStringInfo();

    appendStringInfo(newvalue, "%d", GetLocalSharedPoolSize());

    return (const char*)newvalue->data;
}

const char* Session_variables::MaxSharedPoolSizeGucShowHook(void)
{
    StringInfo newvalue = makeStringInfo();

    if (Session_ctx::Vars().MaxSharedPoolSize == 0) {
        appendStringInfo(newvalue, "%d", GetMaxSharedPoolSize());
    } else {
        appendStringInfo(newvalue, "%d", Session_ctx::Vars().MaxSharedPoolSize);
    }

    return (const char*)newvalue->data;
}

bool Session_variables::ShowShardsForAppNamePrefixesCheckHook(char** newval, void** extra,
                                                              GucSource source)
{
    List* prefixList = NIL;

    /* SplitGUCList scribbles on the input */
    char* splitCopy = pstrdup(*newval);

    /* check whether we can split into a list of identifiers */
    if (!SplitGUCList(splitCopy, ',', &prefixList)) {
        GUC_check_errdetail("not a valid list of identifiers");
        return false;
    }

    char* appNamePrefix = NULL;
    foreach_declared_ptr(appNamePrefix, prefixList)
    {
        int prefixLength = strlen(appNamePrefix);
        if (prefixLength >= NAMEDATALEN) {
            GUC_check_errdetail("prefix %s is more than %d characters", appNamePrefix,
                                NAMEDATALEN);
            return false;
        }

        char* prefixAscii = pstrdup(appNamePrefix);
        pg_clean_ascii_compat(prefixAscii, 0);

        if (strcmp(prefixAscii, appNamePrefix) != 0) {
            GUC_check_errdetail("prefix %s in spq.show_shards_for_app_name_prefixes "
                                "contains non-ascii characters",
                                appNamePrefix);
            return false;
        }
    }

    return true;
}

/*
 * ShowShardsForAppNamePrefixesAssignHook ensures changes to
 * spq.show_shards_for_app_name_prefixes are reflected in the decision
 * whether or not to show shards.
 */
void Session_variables::ShowShardsForAppNamePrefixesAssignHook(const char* newval,
                                                               void* extra)
{
    ResetHideShardsDecision();
}

/*
 * NoticeIfSubqueryPushdownEnabled prints a notice when a user sets
 * spq.subquery_pushdown to ON. It doesn't print the notice if the
 * value is already true.
 */
bool Session_variables::NoticeIfSubqueryPushdownEnabled(bool* newval, void** extra,
                                                        GucSource source)
{
    /* notice only when the value changes */
    if (*newval == true && Session_ctx::Vars().SubqueryPushdown == false) {
        ereport(NOTICE,
                (errcode(ERRCODE_WARNING_DEPRECATED_FEATURE),
                 errmsg("Setting spq.subquery_pushdown flag is "
                        "discouraged becuase it forces the planner "
                        "to pushdown certain queries, skipping "
                        "relevant correctness checks."),
                 errdetail("When enabled, the planner skips many correctness checks "
                           "for subqueries and pushes down the queries to shards as-is. "
                           "It means that the queries are likely to return wrong results "
                           "unless the user is absolutely sure that pushing down the "
                           "subquery is safe. This GUC is maintained only for backward "
                           "compatibility, no new users are supposed to use it. The "
                           "planner "
                           "is capable of pushing down as much computation as possible "
                           "to the "
                           "shards depending on the query.")));
    }

    return true;
}

/*
 * CpuPriorityAssignHook changes the priority of the current backend to match
 * the chosen value.
 */
void Session_variables::CpuPriorityAssignHook(int newval, void* extra)
{
    SetOwnPriority(newval);
}

namespace Spq {
bool GlobalROVariables::NodeConninfoGucCheckHook(char** newval, void** extra,
                                                 GucSource source)
{
    if (t_thrd.role != MASTER_THREAD) {
        if (Session_ctx::CurrentInited()) {
            ereport(WARNING,
                    (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                     errmsg("Ignoring the setting, the spq variable is read only. ")));
            return false;
        }
    }

    /* this array _must_ be kept in an order usable by bsearch */
    const char* allowedConninfoKeywords[] = {
        "connect_timeout",
#if defined(ENABLE_GSS) && defined(ENABLE_SSPI)
        "gsslib",
#endif
        "host",
        "keepalives",
        "keepalives_count",
        "keepalives_idle",
        "keepalives_interval",
#if defined(ENABLE_GSS) || defined(ENABLE_SSPI)
        "krbsrvname",
#endif
        "sslcert",
        "sslcompression",
        "sslcrl",
        "sslkey",
        "sslmode",
        "sslrootcert",
        "tcp_user_timeout",
    };
    char* errorMsg = NULL;
    bool conninfoValid = CheckConninfo(*newval, allowedConninfoKeywords,
                                       lengthof(allowedConninfoKeywords), &errorMsg);

    if (!conninfoValid) {
        GUC_check_errdetail("%s", errorMsg);
    }

    return conninfoValid;
}

void GlobalROVariables::NodeConninfoGucAssignHook(const char* newval, void* extra)
{
    if (t_thrd.role != MASTER_THREAD) {
        return;
    }

    if (newval == NULL) {
        newval = "";
    }

    if (strcmp(newval, NodeConninfo) == 0) {
        /* It did not change, no need to do anything */
        return;
    }

    PQconninfoOption* optionArray = PQconninfoParse(newval, NULL);
    if (optionArray == NULL) {
        ereport(FATAL, (errmsg("cannot parse node_conninfo value"),
                        errdetail("The GUC check hook should prevent "
                                  "all malformed values.")));
    }

    ResetConnParams();

    for (PQconninfoOption* option = optionArray; option->keyword != NULL; option++) {
        if (option->val == NULL || option->val[0] == '\0') {
            continue;
        }

        AddConnParam(option->keyword, option->val);
    }

    PQconninfoFree(optionArray);
}

bool GlobalROVariables::ReadOnlyIntCheckHook(int* newval, void** extra, GucSource source)
{
    if (t_thrd.role == MASTER_THREAD) {
        return true;
    }

    if (Session_ctx::CurrentInited()) {
        ereport(WARNING,
                (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                 errmsg("Ignoring the setting, the spq variable is read only. ")));
        return false;
    }

    return true;
}

void GlobalROVariables::RegisterAll()
{
#ifdef ENABLE_ALL_CONFIGUARABLE_PARAMETERS
    DefineCustomBoolVariable(
        "spq.enable_alter_database_owner",
        gettext_noop("Enables propagating ALTER DATABASE ... OWNER TO ... statements to "
                     "workers"),
        NULL, &EnableAlterDatabaseOwner, true, PGC_POSTMASTER,
        GUC_NO_SHOW_ALL | GUC_NOT_IN_SAMPLE, NULL, NULL, NULL);

    DefineCustomBoolVariable(
        "spq.enable_create_type_propagation",
        gettext_noop("Enables propagating of CREATE TYPE statements to workers"), NULL,
        &EnableCreateTypePropagation, true, PGC_POSTMASTER,
        GUC_NO_SHOW_ALL | GUC_NOT_IN_SAMPLE, NULL, NULL, NULL);

    DefineCustomIntVariable("spq.isolation_test_session_process_id", NULL, NULL,
                            &IsolationTestSessionProcessID, -1, -1, INT_MAX,
                            PGC_POSTMASTER, GUC_NO_SHOW_ALL | GUC_NOT_IN_SAMPLE, NULL,
                            NULL, NULL);

    DefineCustomIntVariable("spq.isolation_test_session_remote_process_id", NULL, NULL,
                            &IsolationTestSessionRemoteProcessID, -1, -1, INT_MAX,
                            PGC_POSTMASTER, GUC_NO_SHOW_ALL | GUC_NOT_IN_SAMPLE, NULL,
                            NULL, NULL);

    DefineCustomIntVariable(
        "spq.max_worker_nodes_tracked",
        gettext_noop("Sets the maximum number of worker nodes that are tracked."),
        gettext_noop("Worker nodes' network locations, their membership and "
                     "health status are tracked in a shared hash table on "
                     "the master node. This configuration value limits the "
                     "size of the hash table, and consequently the maximum "
                     "number of worker nodes that can be tracked. "
                     "spq keeps some information about the worker nodes "
                     "in the shared memory for certain optimizations. The "
                     "optimizations are enforced up to this number of worker "
                     "nodes. Any additional worker nodes may not benefit from "
                     "the optimizations."),
        &MaxWorkerNodesTracked, 2048, 1024, INT_MAX, PGC_POSTMASTER, GUC_STANDARD, NULL,
        NULL, NULL);

    /*
     * Previously we setting this configuration parameter
     * in the fly for failure tests schedule.
     * However, PG15 doesn't allow that anymore: reserved prefixes
     * like "citus" cannot be used to set non-existing GUCs.
     * Relevant PG commit: 88103567cb8fa5be46dc9fac3e3b8774951a2be7
     */

    DefineCustomStringVariable(
        "spq.mitmfifo", gettext_noop("Sets the spq mitm fifo path for failure tests"),
        gettext_noop("This GUC is only used for testing."), &MitmfifoEmptyString, "",
        PGC_POSTMASTER, GUC_NO_SHOW_ALL | GUC_NOT_IN_SAMPLE, NULL, NULL, NULL);

    /*
     * It takes about 140 bytes of shared memory to store one row, therefore
     * this setting should be used responsibly. setting it to 10M will require
     * 1.4GB of shared memory.
     */
    DefineCustomIntVariable(
        "spq.stat_statements_max",
        gettext_noop("Determines maximum number of statements tracked by "
                     "spq_stat_statements."),
        NULL, &StatStatementsMax, 50000, 1000, 10000000, PGC_POSTMASTER,
        GUC_NO_SHOW_ALL | GUC_NOT_IN_SAMPLE, NULL, NULL, NULL);
#endif
    DefineCustomIntVariable(
        "spq.max_client_connections",
        gettext_noop("Sets the maximum number of connections regular clients can make"),
        gettext_noop("To ensure that a spq cluster has a sufficient number of "
                     "connection slots to serve queries internally, it can be "
                     "useful to reserve connection slots for spq internal "
                     "connections. When max_client_connections is set to a value "
                     "below max_connections, the remaining connections are reserved "
                     "for connections between spq nodes. This does not affect "
                     "superuser_reserved_connections. If set to -1, no connections "
                     "are reserved."),
        &MaxClientConnections, -1, -1, g_instance.attr.attr_network.MaxConnections,
        PGC_SIGHUP, GUC_STANDARD, ReadOnlyIntCheckHook, NULL, NULL);

    DefineCustomStringVariable(
        "spq.node_conninfo",
        gettext_noop("Sets parameters used for outbound connections."), NULL,
        &NodeConninfo, "sslmode=disable", PGC_SIGHUP, GUC_SUPERUSER_ONLY,
        NodeConninfoGucCheckHook, NodeConninfoGucAssignHook, NULL);

    DefineCustomEnumVariable(
        "spq.remote_io_multiplex_model",
        gettext_noop("defines the IO multiplex model for remote connecitons "
                     "between CN and DN"),
        gettext_noop(
            "There are 2 values available, 'poll' and 'epoll'. The default is 'epoll'"),
        &SpqIOMultiplexModel, SPQ_IO_MULTIPLEX_EPOLL, spq_io_multiplex_model_options,
        PGC_SIGHUP, GUC_STANDARD, NULL, NULL, NULL);

    DefineCustomStringVariable(
        "spq.version", gettext_noop("Shows the spq library version"), NULL, &SpqVersion,
        SPQ_VERSION, PGC_INTERNAL, GUC_STANDARD, NULL, NULL, NULL);
}

}  // namespace Spq
void Session_variables::RegisterSessionVariables()
{
    /* initialize default connection parameter for variable spq.node_conninfo*/
    InitConnParams();

    DefineCustomStringVariable(
        "spq.cluster_name", gettext_noop("Which cluster this node is a part of"), NULL,
        &CurrentCluster, "default", PGC_BACKEND, GUC_STANDARD, NULL, NULL, NULL);

    DefineCustomEnumVariable(
        "spq.coordinator_aggregation_strategy",
        gettext_noop("Sets the strategy for when an aggregate cannot be pushed down. "
                     "'row-gather' will pull up intermediate rows to the coordinator, "
                     "while 'disabled' will error if coordinator aggregation is "
                     "necessary"),
        NULL, &CoordinatorAggregationStrategy, COORDINATOR_AGGREGATION_ROW_GATHER,
        coordinator_aggregation_options, PGC_USERSET, GUC_STANDARD, NULL, NULL, NULL);

    DefineCustomRealVariable(
        "spq.distributed_deadlock_detection_factor",
        gettext_noop("Sets the time to wait before checking for distributed "
                     "deadlocks. Postgres' deadlock_timeout setting is "
                     "multiplied with the value. If the value is set to -1, "
                     "distributed deadlock detection is disabled."),
        NULL, &DistributedDeadlockDetectionTimeoutFactor, 2.0, -1.0, 1000.0, PGC_SIGHUP,
        GUC_STANDARD, ErrorIfNotASuitableDeadlockFactor, NULL, NULL);

    DefineCustomBoolVariable(
        "spq.enable_alter_role_propagation",
        gettext_noop("Enables propagating ALTER ROLE statements to workers (excluding "
                     "ALTER ROLE SET)"),
        NULL, &EnableAlterRolePropagation, true, PGC_USERSET,
        GUC_NO_SHOW_ALL | GUC_NOT_IN_SAMPLE, NULL, NULL, NULL);

    DefineCustomBoolVariable(
        "spq.enable_alter_role_set_propagation",
        gettext_noop("Enables propagating ALTER ROLE SET statements to workers"), NULL,
        &EnableAlterRoleSetPropagation, true, PGC_USERSET,
        GUC_NO_SHOW_ALL | GUC_NOT_IN_SAMPLE, NULL, NULL, NULL);

    DefineCustomBoolVariable(
        "spq.enable_binary_protocol",
        gettext_noop("Enables communication between nodes using binary "
                     "protocol when possible"),
        NULL, &EnableBinaryProtocol, true, PGC_USERSET, GUC_STANDARD, NULL, NULL, NULL);

    DefineCustomBoolVariable(
        "spq.enable_cost_based_connection_establishment",
        gettext_noop("When enabled the connection establishment times "
                     "and task execution times into account for deciding "
                     "whether or not to establish new connections."),
        NULL, &EnableCostBasedConnectionEstablishment, true, PGC_USERSET,
        GUC_NO_SHOW_ALL | GUC_NOT_IN_SAMPLE, NULL, NULL, NULL);

    DefineCustomBoolVariable("spq.enable_create_role_propagation",
                             gettext_noop("Enables propagating CREATE ROLE "
                                          "and DROP ROLE statements to workers"),
                             NULL, &EnableCreateRolePropagation, true, PGC_USERSET,
                             GUC_STANDARD, NULL, NULL, NULL);

    DefineCustomBoolVariable(
        "spq.enable_ddl_propagation",
        gettext_noop("Enables propagating DDL statements to worker shards"), NULL,
        &EnableDDLPropagation, true, PGC_USERSET, GUC_NO_SHOW_ALL | GUC_NOT_IN_SAMPLE,
        NULL, NULL, NULL);

    DefineCustomBoolVariable(
        "spq.enable_deadlock_prevention",
        gettext_noop("Avoids deadlocks by preventing concurrent multi-shard commands"),
        gettext_noop("Multi-shard modifications such as UPDATE, DELETE, and "
                     "INSERT...SELECT are typically executed in parallel. If multiple "
                     "such commands run concurrently and affect the same rows, then "
                     "they are likely to deadlock. When enabled, this flag prevents "
                     "multi-shard modifications from running concurrently when they "
                     "affect the same shards in order to prevent deadlocks."),
        &EnableDeadlockPrevention, true, PGC_USERSET, GUC_STANDARD, NULL, NULL, NULL);

    DefineCustomBoolVariable("spq.enable_fast_path_router_planner",
                             gettext_noop("Enables fast path router planner"), NULL,
                             &EnableFastPathRouterPlanner, true, PGC_USERSET,
                             GUC_NO_SHOW_ALL | GUC_NOT_IN_SAMPLE, NULL, NULL, NULL);

    DefineCustomBoolVariable(
        "spq.enable_local_execution",
        gettext_noop("Enables queries on shards that are local to the current node "
                     "to be planned and executed locally."),
        NULL, &EnableLocalExecution, true, PGC_USERSET, GUC_STANDARD, NULL, NULL, NULL);

    DefineCustomIntVariable(
        "spq.executor_slow_start_interval",
        gettext_noop("Time to wait between opening connections to the same worker node"),
        gettext_noop("When the individual tasks of a multi-shard query take very "
                     "little time, they can often be finished over a single (often "
                     "already cached) connection. To avoid redundantly opening "
                     "additional connections, the executor waits between connection "
                     "attempts for the configured number of milliseconds. At the end "
                     "of the interval, it increases the number of connections it is "
                     "allowed to open next time."),
        &ExecutorSlowStartInterval, 1, 0, INT_MAX, PGC_USERSET,
        GUC_UNIT_MS | GUC_NO_SHOW_ALL | GUC_NOT_IN_SAMPLE, NULL, NULL, NULL);

    DefineCustomBoolVariable(
        "spq.explain_all_tasks",
        gettext_noop("Enables showing output for all tasks in Explain."),
        gettext_noop("The Explain command for distributed queries shows "
                     "the remote plan for a single task by default. When "
                     "this configuration entry is enabled, the plan for "
                     "all tasks is shown, but the Explain takes longer."),
        &ExplainAllTasks, false, PGC_USERSET, GUC_STANDARD, NULL, NULL, NULL);

    DefineCustomBoolVariable(
        "spq.explain_distributed_queries",
        gettext_noop("Enables Explain for distributed queries."),
        gettext_noop("When enabled, the Explain command shows remote and local "
                     "plans when used with a distributed query. It is enabled "
                     "by default, but can be disabled for regression tests."),
        &ExplainDistributedQueries, true, PGC_USERSET,
        GUC_NO_SHOW_ALL | GUC_NOT_IN_SAMPLE, NULL, NULL, NULL);

    DefineCustomBoolVariable(
        "spq.force_max_query_parallelization",
        gettext_noop("Open as many connections as possible to maximize query "
                     "parallelization"),
        gettext_noop("When enabled, spq will force the executor to use "
                     "as many connections as possible while executing a "
                     "parallel distributed query. If not enabled, the executor "
                     "might choose to use less connections to optimize overall "
                     "query execution throughput. Internally, setting this true "
                     "will end up with using one connection per task."),
        &ForceMaxQueryParallelization, true, PGC_USERSET,
        GUC_NO_SHOW_ALL | GUC_NOT_IN_SAMPLE, NULL, NULL, NULL);

    DefineCustomBoolVariable(
        "spq.function_opens_transaction_block",
        gettext_noop("Open transaction blocks for function calls"),
        gettext_noop("When enabled, spq will always send a BEGIN to workers when "
                     "running distributed queres in a function. When disabled, the "
                     "queries may be committed immediately after the statemnent "
                     "completes. Disabling this flag is dangerous, it is only provided "
                     "for backwards compatibility with pre-8.2 behaviour."),
        &FunctionOpensTransactionBlock, true, PGC_USERSET,
        GUC_NO_SHOW_ALL | GUC_NOT_IN_SAMPLE, NULL, NULL, NULL);

    /*
     * This was a GUC we added on Citus 11.0.1, and
     * replaced with another name on 11.0.2 via #5920.
     * However, as this GUC has been used in
     * sqp_shard_indexes_on_worker-2.0.0.sql
     * script. So, it is not easy to completely get rid
     * of the GUC. Especially with PG 15+, Postgres verifies
     * existence of the GUCs that are used. So, without this
     * CREATE EXTENSION fails.
     */
    DefineCustomStringVariable(
        "spq.hide_shards_from_app_name_prefixes",
        gettext_noop("Deprecated, use spq.show_shards_for_app_name_prefixes"), NULL,
        &DeprecatedEmptyString, "", PGC_SUSET, GUC_NO_SHOW_ALL | GUC_NOT_IN_SAMPLE, NULL,
        NULL, NULL);

    DefineCustomIntVariable(
        "spq.limit_clause_row_fetch_count",
        gettext_noop("Number of rows to fetch per task for limit clause optimization."),
        gettext_noop("Select queries get partitioned and executed as smaller "
                     "tasks. In some cases, select queries with limit clauses "
                     "may need to fetch all rows from each task to generate "
                     "results. In those cases, and where an approximation would "
                     "produce meaningful results, this configuration value sets "
                     "the number of rows to fetch from each task."),
        &LimitClauseRowFetchCount, -1, -1, INT_MAX, PGC_USERSET, GUC_STANDARD, NULL, NULL,
        NULL);

    DefineCustomIntVariable(
        "spq.local_copy_flush_threshold",
        gettext_noop("Sets the threshold for local copy to be flushed, in KB Unit."),
        NULL, &LocalCopyFlushThresholdByte, 512, 1, INT_MAX, PGC_USERSET,
        GUC_UNIT_KB | GUC_NO_SHOW_ALL | GUC_NOT_IN_SAMPLE, NULL, NULL, NULL);

    DefineCustomStringVariable(
        "spq.local_hostname",
        gettext_noop("Sets the hostname when connecting back to itself."),
        gettext_noop("For some operations nodes, mostly the coordinator, "
                     "connect back to "
                     "itself. When configuring SSL certificates it sometimes "
                     "is required "
                     "to use a specific hostname to match the CN of the "
                     "certificate when "
                     "verify-full is used."),
        &LocalHostName, "localhost", PGC_SUSET, GUC_STANDARD, NULL, NULL, NULL);

    DefineCustomIntVariable(
        "spq.local_shared_pool_size",
        gettext_noop("Sets the maximum number of connections allowed for the shards on "
                     "the "
                     "local node across all the backends from this node. Setting to -1 "
                     "disables "
                     "connections throttling. Setting to 0 makes it auto-adjust, "
                     "meaning "
                     "equal to the half of max_connections on the coordinator."),
        gettext_noop("As a rule of thumb, the value should be at most equal to the "
                     "max_connections on the local node."),
        &LocalSharedPoolSize, 0, -1, INT_MAX, PGC_SIGHUP, GUC_SUPERUSER_ONLY, NULL, NULL,
        LocalPoolSizeGucShowHook);

    DefineCustomEnumVariable(
        "spq.local_table_join_policy",
        gettext_noop("defines the behaviour when a distributed table "
                     "is joined with a local table"),
        gettext_noop("There are 4 values available. The default, 'auto' will "
                     "recursively plan "
                     "distributed tables if there is a constant filter on a unique "
                     "index. "
                     "'prefer-local' will choose local tables if possible. "
                     "'prefer-distributed' will choose distributed tables if possible. "
                     "'never' will basically skip local table joins."),
        &LocalTableJoinPolicy, LOCAL_JOIN_POLICY_AUTO, local_table_join_policies,
        PGC_USERSET, GUC_STANDARD, NULL, NULL, NULL);

    DefineCustomBoolVariable(
        "spq.log_distributed_deadlock_detection",
        gettext_noop("Log distributed deadlock detection related processing in "
                     "the server log"),
        NULL, &LogDistributedDeadlockDetection, false, PGC_SIGHUP,
        GUC_NO_SHOW_ALL | GUC_NOT_IN_SAMPLE, NULL, NULL, NULL);

    DefineCustomBoolVariable("spq.log_intermediate_results",
                             gettext_noop("Log intermediate results sent to other nodes"),
                             NULL, &LogIntermediateResults, false, PGC_USERSET,
                             GUC_NO_SHOW_ALL | GUC_NOT_IN_SAMPLE, NULL, NULL, NULL);

    DefineCustomBoolVariable(
        "spq.log_local_commands",
        gettext_noop("Log queries that are executed locally, can be overriden by "
                     "spq.log_remote_commands"),
        NULL, &LogLocalCommands, false, PGC_USERSET, GUC_NO_SHOW_ALL | GUC_NOT_IN_SAMPLE,
        NULL, NULL, NULL);

    DefineCustomBoolVariable(
        "spq.log_multi_join_order",
        gettext_noop("Logs the distributed join order to the server log."),
        gettext_noop("We use this private configuration entry as a debugging aid. "
                     "If enabled, we print the distributed join order."),
        &LogMultiJoinOrder, false, PGC_USERSET, GUC_NO_SHOW_ALL | GUC_NOT_IN_SAMPLE, NULL,
        NULL, NULL);

    DefineCustomBoolVariable(
        "spq.log_remote_commands",
        gettext_noop("Log queries sent to other nodes in the server log"), NULL,
        &LogRemoteCommands, false, PGC_USERSET, GUC_STANDARD, NULL, NULL, NULL);

    DefineCustomIntVariable(
        "spq.max_adaptive_executor_pool_size",
        gettext_noop("Sets the maximum number of connections per worker node used by "
                     "the adaptive executor to execute a multi-shard command"),
        gettext_noop("The adaptive executor may open multiple connections per worker "
                     "node when running multi-shard commands to parallelize the command "
                     "across multiple cores on the worker. This setting specifies the "
                     "maximum number of connections it will open. The number of "
                     "connections is also bounded by the number of shards on the node. "
                     "This setting can be used to reduce the memory usage of a query "
                     "and allow a higher degree of concurrency when concurrent "
                     "multi-shard queries open too many connections to a worker."),
        &MaxAdaptiveExecutorPoolSize, 256, 1, INT_MAX, PGC_USERSET, GUC_STANDARD, NULL,
        NULL, NULL);

    DefineCustomIntVariable(
        "spq.max_cached_connection_lifetime",
        gettext_noop("Sets the maximum lifetime of cached connections to other nodes."),
        NULL, &MaxCachedConnectionLifetime, 10 * MS_PER_MINUTE, -1, INT_MAX, PGC_USERSET,
        GUC_UNIT_MS | GUC_STANDARD, NULL, NULL, NULL);

    DefineCustomIntVariable(
        "spq.max_cached_conns_per_worker",
        gettext_noop("Sets the maximum number of connections to cache per worker."),
        gettext_noop("Each backend opens connections to the workers to query the "
                     "shards. At the end of the transaction, the configurated number "
                     "of connections is kept open to speed up subsequent commands. "
                     "Increasing this value will reduce the latency of multi-shard "
                     "queries, but increases overhead on the workers"),
        &MaxCachedConnectionsPerWorker, 16, 0, INT_MAX, PGC_USERSET, GUC_STANDARD, NULL,
        NULL, NULL);

    DefineCustomIntVariable(
        "spq.max_intermediate_result_size",
        gettext_noop("Sets the maximum size of the intermediate results in KB for "
                     "CTEs and complex subqueries."),
        NULL, &MaxIntermediateResult, 1048576, -1, MAX_KILOBYTES, PGC_USERSET,
        GUC_UNIT_KB | GUC_STANDARD, NULL, NULL, NULL);

    DefineCustomIntVariable(
        "spq.max_matview_size_to_auto_recreate",
        gettext_noop("Sets the maximum size of materialized views in KB to "
                     "automatically distribute them."),
        NULL, &MaxMatViewSizeToAutoRecreate, 1024 * 1024, -1, INT_MAX, PGC_USERSET,
        GUC_UNIT_KB | GUC_STANDARD, NULL, NULL, NULL);

    DefineCustomIntVariable(
        "spq.max_shared_pool_size",
        gettext_noop("Sets the maximum number of connections allowed per worker node "
                     "across all the backends from this node. Setting to -1 disables "
                     "connections throttling. Setting to 0 makes it auto-adjust, "
                     "meaning "
                     "equal to max_connections on the coordinator."),
        gettext_noop("As a rule of thumb, the value should be at most equal to the "
                     "max_connections on the remote nodes."),
        &MaxSharedPoolSize, 0, -1, INT_MAX, PGC_SIGHUP, GUC_SUPERUSER_ONLY, NULL, NULL,
        MaxSharedPoolSizeGucShowHook);

    /*
     * Previously we setting this configuration parameter
     * in the fly for failure tests schedule.
     * However, PG15 doesn't allow that anymore: reserved prefixes
     * like "spq" cannot be used to set non-existing GUCs.
     * Relevant PG commit: 88103567cb8fa5be46dc9fac3e3b8774951a2be7
     */

    DefineCustomEnumVariable(
        "spq.multi_shard_modify_mode",
        gettext_noop("Sets the connection type for multi shard modify queries"), NULL,
        &MultiShardConnectionType, PARALLEL_CONNECTION,
        multi_shard_modify_connection_options, PGC_USERSET, GUC_STANDARD, NULL, NULL,
        NULL);

    DefineCustomEnumVariable(
        "spq.multi_task_query_log_level",
        gettext_noop("Sets the level of multi task query execution log messages"), NULL,
        &MultiTaskQueryLogLevel, CITUS_LOG_LEVEL_OFF, log_level_options, PGC_USERSET,
        GUC_STANDARD, NULL, NULL, NULL);

    DefineCustomIntVariable(
        "spq.next_cleanup_record_id",
        gettext_noop("Set the next cleanup record ID to use in operation creation."),
        gettext_noop("Cleanup record IDs are normally generated using a sequence. If "
                     "next_cleanup_record_id is set to a non-zero value, cleanup record "
                     "IDs will "
                     "instead be generated by incrementing from the value of "
                     "this GUC and this will be reflected in the GUC. This is "
                     "mainly useful to ensure consistent cleanup record IDs when "
                     "running "
                     "tests in parallel."),
        &NextCleanupRecordId, 0, 0, INT_MAX, PGC_USERSET,
        GUC_NO_SHOW_ALL | GUC_NOT_IN_SAMPLE, NULL, NULL, NULL);

    DefineCustomIntVariable(
        "spq.next_operation_id",
        gettext_noop("Set the next operation ID to use in operation creation."),
        gettext_noop("Operation IDs are normally generated using a sequence. If "
                     "next_operation_id is set to a non-zero value, operation IDs will "
                     "instead be generated by incrementing from the value of "
                     "this GUC and this will be reflected in the GUC. This is "
                     "mainly useful to ensure consistent operation IDs when running "
                     "tests in parallel."),
        &NextOperationId, 0, 0, INT_MAX, PGC_USERSET, GUC_NO_SHOW_ALL | GUC_NOT_IN_SAMPLE,
        NULL, NULL, NULL);

    DefineCustomIntVariable(
        "spq.next_placement_id",
        gettext_noop("Set the next placement ID to use in placement creation."),
        gettext_noop("Placement IDs are normally generated using a sequence. If "
                     "next_placement_id is set to a non-zero value, placement IDs will "
                     "instead be generated by incrementing from the value of "
                     "this GUC and this will be reflected in the GUC. This is "
                     "mainly useful to ensure consistent placement IDs when running "
                     "tests in parallel."),
        &NextPlacementId, 0, 0, INT_MAX, PGC_USERSET, GUC_NO_SHOW_ALL | GUC_NOT_IN_SAMPLE,
        NULL, NULL, NULL);

    DefineCustomIntVariable(
        "spq.next_shard_id",
        gettext_noop("Set the next shard ID to use in shard creation."),
        gettext_noop("Shard IDs are normally generated using a sequence. If "
                     "next_shard_id is set to a non-zero value, shard IDs will "
                     "instead be generated by incrementing from the value of "
                     "this GUC and this will be reflected in the GUC. This is "
                     "mainly useful to ensure consistent shard IDs when running "
                     "tests in parallel."),
        &NextShardId, 0, 0, INT_MAX, PGC_USERSET, GUC_NO_SHOW_ALL | GUC_NOT_IN_SAMPLE,
        NULL, NULL, NULL);

    DefineCustomIntVariable(
        "spq.node_connection_timeout",
        gettext_noop("Sets the maximum duration to connect to worker nodes."), NULL,
        &NodeConnectionTimeout, 30 * MS_PER_SECOND, 10 * MS, MS_PER_HOUR, PGC_USERSET,
        GUC_UNIT_MS | GUC_STANDARD, NULL, NULL, NULL);

    DefineCustomBoolVariable(
        "spq.prevent_incomplete_connection_establishment",
        gettext_noop("When enabled, the executor waits until all the connections "
                     "are successfully established."),
        gettext_noop("Under some load, the executor may decide to establish some "
                     "extra connections to further parallelize the execution. However, "
                     "before the connection establishment is done, the execution might "
                     "have already finished. When this GUC is set to true, the "
                     "execution "
                     "waits for such connections to be established."),
        &PreventIncompleteConnectionEstablishment, true, PGC_USERSET,
        GUC_NO_SHOW_ALL | GUC_NOT_IN_SAMPLE, NULL, NULL, NULL);

    DefineCustomBoolVariable(
        "spq.propagate_session_settings_for_loopback_connection",
        gettext_noop("When enabled, rebalancer propagates all the allowed GUC "
                     "settings to new connections."),
        NULL, &PropagateSessionSettingsForLoopbackConnection, false, PGC_USERSET,
        GUC_NO_SHOW_ALL | GUC_NOT_IN_SAMPLE, NULL, NULL, NULL);

    DefineCustomEnumVariable(
        "spq.propagate_set_commands",
        gettext_noop("Sets which SET commands are propagated to workers."), NULL,
        (int*)(&PropagateSetCommands), PROPSETCMD_NONE, propagate_set_commands_options,
        PGC_USERSET, GUC_STANDARD, NULL, NULL, NULL);

    DefineCustomIntVariable(
        "spq.recover_2pc_interval",
        gettext_noop("Sets the time to wait between recovering 2PCs."),
        gettext_noop("2PC transaction recovery needs to run every so often "
                     "to clean up records in pg_dist_transaction and "
                     "potentially roll failed 2PCs forward. This setting "
                     "determines how often recovery should run, "
                     "use -1 to disable."),
        &Recover2PCInterval, 60 * MS_PER_SECOND, -1, 7 * MS_PER_DAY, PGC_SIGHUP,
        GUC_UNIT_MS | GUC_STANDARD, NULL, NULL, NULL);

    DefineCustomIntVariable(
        "spq.remote_copy_flush_threshold",
        gettext_noop("Sets the threshold for remote copy to be flushed."),
        gettext_noop("When sending data over remote connections via the COPY protocol, "
                     "bytes are first buffered internally by libpq. If the number of "
                     "bytes buffered exceeds the threshold, spq waits for all the "
                     "bytes to flush, in KB Unit"),
        &RemoteCopyFlushThreshold, 8 * 1024, 0, INT_MAX, PGC_USERSET,
        GUC_UNIT_KB | GUC_NO_SHOW_ALL | GUC_NOT_IN_SAMPLE, NULL, NULL, NULL);

    DefineCustomBoolVariable(
        "spq.running_under_spq_test_suite",
        gettext_noop("Only useful for testing purposes, when set to true, spq does "
                     "some "
                     "tricks to implement useful isolation tests with rebalancing. It "
                     "also "
                     "registers a dummy label provider for SECURITY LABEL tests. Should "
                     "never be set to true on production systems "),
        gettext_noop("for details of the tricks implemented, refer to the source code"),
        &RunningUnderCitusTestSuite, false, PGC_SUSET,
        GUC_SUPERUSER_ONLY | GUC_NO_SHOW_ALL | GUC_NOT_IN_SAMPLE, NULL, NULL, NULL);

    DefineCustomBoolVariable(
        "spq.select_opens_transaction_block",
        gettext_noop("Open transaction blocks for SELECT commands"),
        gettext_noop("When enabled, spq will always send a BEGIN to workers when "
                     "running a distributed SELECT in a transaction block (the "
                     "default). When disabled, spq will only send BEGIN before "
                     "the first write or other operation that requires a distributed "
                     "transaction, meaning the SELECT on the worker commits "
                     "immediately, releasing any locks and apply any changes made "
                     "through function calls even if the distributed transaction "
                     "aborts."),
        &SelectOpensTransactionBlock, true, PGC_USERSET,
        GUC_NO_SHOW_ALL | GUC_NOT_IN_SAMPLE, NULL, NULL, NULL);

    DefineCustomIntVariable(
        "spq.shard_count",
        gettext_noop("Sets the number of shards for a new hash-partitioned table "
                     "created with create_distributed_table()."),
        NULL, &ShardCount, 8, 1, MAX_SHARD_COUNT, PGC_USERSET, GUC_STANDARD, NULL, NULL,
        NULL);

    DefineCustomIntVariable(
        "spq.shard_replication_factor",
        gettext_noop("Sets the replication factor for shards."),
        gettext_noop("Shards are replicated across nodes according to this "
                     "replication factor. Note that shards read this "
                     "configuration value at sharded table creation time, "
                     "and later reuse the initially read value."),
        &ShardReplicationFactor, 1, 1, MAX_SHARD_REPLICATION_FACTOR, PGC_USERSET,
        GUC_STANDARD, NULL, NULL, NULL);

    DefineCustomStringVariable(
        "spq.show_shards_for_app_name_prefixes",
        gettext_noop("If application_name starts with one of these values, show shards"),
        gettext_noop("spq places distributed tables and shards in the same schema. "
                     "That can cause confusion when inspecting the list of tables on "
                     "a node with shards. By default the shards are hidden from "
                     "pg_class. This GUC can be used to show the shards to certain "
                     "applications based on the application_name of the connection. "
                     "The default is empty string, which hides shards from all "
                     "applications. This behaviour can be overridden using the "
                     "spq.override_table_visibility setting"),
        &ShowShardsForAppNamePrefixes, "", PGC_USERSET, GUC_STANDARD,
        ShowShardsForAppNamePrefixesCheckHook, ShowShardsForAppNamePrefixesAssignHook,
        NULL);

    DefineCustomEnumVariable(
        "spq.explain_analyze_sort_method",
        gettext_noop("Sets the sorting method for EXPLAIN ANALYZE queries."),
        gettext_noop("This parameter is intended for testing. It is developed "
                     "to get consistent regression test outputs. When it is set "
                     "to 'time', EXPLAIN ANALYZE output is sorted by execution "
                     "duration on workers. When it is set to 'taskId', it is "
                     "sorted by task id. By default, it is set to 'time'; but "
                     "in regression tests, it's set to 'taskId' for consistency."),
        &ExplainAnalyzeSortMethod, EXPLAIN_ANALYZE_SORT_BY_TIME,
        explain_analyze_sort_method_options, PGC_USERSET, 0, NULL, NULL, NULL);

    DefineCustomEnumVariable(
        "spq.stat_statements_track",
        gettext_noop("Enables/Disables the stats collection for spq_stat_statements."),
        gettext_noop("Enables the stats collection when set to 'all'. "
                     "Disables when set to 'none'. Disabling can be useful for "
                     "avoiding extra CPU cycles needed for the calculations."),
        &StatStatementsTrack, STAT_STATEMENTS_TRACK_NONE, stat_statements_track_options,
        PGC_SUSET, GUC_STANDARD, NULL, NULL, NULL);

    DefineCustomIntVariable("spq.stat_statements_purge_interval",
                            gettext_noop("Determines time interval in seconds for "
                                         "spq_stat_statements to purge expired entries."),
                            NULL, &StatStatementsPurgeInterval, 10, -1, INT_MAX,
                            PGC_SIGHUP, GUC_UNIT_MS | GUC_NO_SHOW_ALL | GUC_NOT_IN_SAMPLE,
                            NULL, NULL, NULL);

    DefineCustomEnumVariable(
        "spq.use_secondary_nodes",
        gettext_noop("Sets the policy to use when choosing nodes for SELECT queries."),
        NULL, &ReadFromSecondaries, USE_SECONDARY_NODES_NEVER,
        use_secondary_nodes_options, PGC_BACKEND, GUC_STANDARD, NULL, NULL, NULL);

    DefineCustomBoolVariable(
        "spq.enable_repartition_joins",
        gettext_noop("Allows spq to repartition data between nodes."), NULL,
        &EnableRepartitionJoins, false, PGC_USERSET, GUC_STANDARD, NULL, NULL, NULL);

    DefineCustomRealVariable(
        "spq.desired_percent_disk_available_after_move",
        gettext_noop("Sets how many percentage of free disk space should be "
                     "after a shard move"),
        gettext_noop("This setting controls how much free space should be "
                     "available after a shard move. "
                     "If the free disk space will be lower than this "
                     "parameter, then shard move will result in "
                     "an error."),
        &DesiredPercentFreeAfterMove, 10.0, 0.0, 100.0, PGC_SIGHUP, GUC_STANDARD, NULL,
        NULL, NULL);

    /** @TODO: following parameters should be look more deeply if we really want to make
     them to be configurable parameters */
#ifdef ENABLE_ALL_CONFIGUARABLE_PARAMETERS
    DefineCustomEnumVariable(
        "spq.task_executor_type",
        gettext_noop("Sets the executor type to be used for distributed queries."),
        gettext_noop("The master node chooses between two different executor types "
                     "when executing a distributed query.The adaptive executor is "
                     "optimal for simple key-value lookup queries and queries that "
                     "involve aggregations and/or co-located joins on multiple "
                     "shards. "),
        &TaskExecutorType, MULTI_EXECUTOR_ADAPTIVE, task_executor_type_options,
        PGC_USERSET, GUC_STANDARD, NULL, NULL, NULL);

    DefineCustomBoolVariable("spq.all_modifications_commutative",
                             gettext_noop("Bypasses commutativity checks when enabled"),
                             NULL, &AllModificationsCommutative, false, PGC_USERSET,
                             GUC_STANDARD, NULL, NULL, NULL);

    DefineCustomBoolVariable(
        "spq.allow_modifications_from_workers_to_replicated_tables",
        gettext_noop("Enables modifications from workers to replicated "
                     "tables such as reference tables or hash "
                     "distributed tables with replication factor "
                     "greater than 1."),
        gettext_noop("Allowing modifications from the worker nodes "
                     "requires extra locking which might decrease "
                     "the throughput. Disabling this GUC skips the "
                     "extra locking and prevents modifications from "
                     "worker nodes."),
        &AllowModificationsFromWorkersToReplicatedTables, true, PGC_USERSET,
        GUC_NO_SHOW_ALL | GUC_NOT_IN_SAMPLE, NULL, NULL, NULL);

    DefineCustomBoolVariable(
        "spq.allow_nested_distributed_execution",
        gettext_noop("Enables distributed execution within a task "
                     "of another distributed execution."),
        gettext_noop("Nested distributed execution can happen when spq "
                     "pushes down a call to a user-defined function within "
                     "a distributed query, and the function contains another "
                     "distributed query. In this scenario, spq makes no "
                     "guarantess with regards to correctness and it is therefore "
                     "disallowed by default. This setting can be used to allow "
                     "nested distributed execution."),
        &AllowNestedDistributedExecution, false, PGC_USERSET,
        GUC_NO_SHOW_ALL | GUC_NOT_IN_SAMPLE, NULL, NULL, NULL);

    DefineCustomBoolVariable(
        "spq.allow_unsafe_constraints",
        gettext_noop("Enables unique constraints and exclusion constraints "
                     "that do not include a distribution column."),
        gettext_noop("To enforce global uniqueness, spq normally requires "
                     "that unique constraints and exclusion constraints contain "
                     "the distribution column. If the tuple does not include the "
                     "distribution column, spq cannot ensure that the same value "
                     "is not present in another shard. However, in some cases the "
                     "index creator knows that uniqueness within the shard implies "
                     "global uniqueness (e.g. when indexing an expression derived "
                     "from the distribution column) and adding the distribution column "
                     "separately may not be desirable. This setting can then be used "
                     "to disable the check."),
        &AllowUnsafeConstraints, false, PGC_USERSET, GUC_NO_SHOW_ALL | GUC_NOT_IN_SAMPLE,
        NULL, NULL, NULL);

    DefineCustomBoolVariable(
        "spq.allow_unsafe_locks_from_workers",
        gettext_noop("Enables acquiring a distributed lock from a worker "
                     "when the coordinator is not in the metadata"),
        gettext_noop("Set to false by default. If set to true, enables "
                     "acquiring a distributed lock from a worker "
                     "when the coordinator is not in the metadata. "
                     "This type of lock is unsafe because the worker will not be "
                     "able to lock the coordinator; the coordinator will be able to "
                     "intialize distributed operations on the resources locked "
                     "by the worker. This can lead to concurrent operations from the "
                     "coordinator and distributed deadlocks since the coordinator "
                     "and the workers would not acquire locks across the same nodes "
                     "in the same order."),
        &EnableAcquiringUnsafeLockFromWorkers, false, PGC_USERSET,
        GUC_NO_SHOW_ALL | GUC_NOT_IN_SAMPLE, NULL, NULL, NULL);

    DefineCustomIntVariable(
        "spq.background_task_queue_interval",
        gettext_noop("Time to wait between checks for scheduled background tasks."), NULL,
        &BackgroundTaskQueueCheckInterval, 5000, -1, 7 * 24 * 3600 * 1000, PGC_SIGHUP,
        GUC_UNIT_MS, NULL, NULL, NULL);

    DefineCustomBoolVariable(
        "spq.check_available_space_before_move",
        gettext_noop("When enabled will check free disk space before a shard move"),
        gettext_noop("Free disk space will be checked when this setting is "
                     "enabled before each shard move."),
        &CheckAvailableSpaceBeforeMove, true, PGC_USERSET,
        GUC_NO_SHOW_ALL | GUC_NOT_IN_SAMPLE, NULL, NULL, NULL);

    DefineCustomIntVariable(
        "spq.copy_switchover_threshold",
        gettext_noop("Sets the threshold for copy to be switched "
                     "over per connection."),
        gettext_noop("Data size threshold to switch over the active placement for "
                     "a connection. If this is too low, overhead of starting COPY "
                     "commands will hurt the performance. If this is too high, "
                     "buffered data will use lots of memory. 4MB is a good balance "
                     "between memory usage and performance. Note that this is "
                     "irrelevant "
                     "in the common case where we open one connection per placement."),
        &CopySwitchOverThresholdBytes, 4 * 1024, 1, INT_MAX, PGC_USERSET,
        GUC_UNIT_KB | GUC_NO_SHOW_ALL | GUC_NOT_IN_SAMPLE, NULL, NULL, NULL);

    DefineCustomRealVariable(
        "spq.count_distinct_error_rate",
        gettext_noop("Desired error rate when calculating count(distinct) "
                     "approximates using the postgresql-hll extension. "
                     "0.0 disables approximations for count(distinct); 1.0 "
                     "provides no guarantees about the accuracy of results."),
        NULL, &CountDistinctErrorRate, 0.0, 0.0, 1.0, PGC_USERSET, GUC_STANDARD, NULL,
        NULL, NULL);

    /*
     * This doesn't use cpu_priority_options on purpose, because we always need
     * to know the actual priority value so that `RESET spq.cpu_priority`
     * actually changes the priority back.
     */
    DefineCustomIntVariable(
        "spq.cpu_priority", gettext_noop("Sets the CPU priority of the current backend."),
        gettext_noop("Lower numbers cause more favorable scheduling, so the "
                     "queries that this backend runs will be able to use more "
                     "CPU resources compared to queries from other backends. "
                     "WARNING: Changing this setting can lead to a pnemomenom "
                     "called 'priority inversion', due to locks being held "
                     "between different backends. This means that processes "
                     "might be scheduled in the exact oposite way of what you "
                     "want, i.e. processes that you want scheduled a lot, are "
                     "scheduled very little. So use this setting at your own "
                     "risk."),
        &CpuPriority, GetOwnPriority(), -20, 19, PGC_SUSET, GUC_STANDARD, NULL,
        CpuPriorityAssignHook, NULL);

    DefineCustomEnumVariable(
        "spq.cpu_priority_for_logical_replication_senders",
        gettext_noop("Sets the CPU priority for backends that send logical "
                     "replication changes to other nodes for online shard "
                     "moves and splits."),
        gettext_noop("Lower numbers cause more favorable scheduling, so the "
                     "backends used to do the shard move will get more CPU "
                     "resources. 'inherit' is a special value and disables "
                     "overriding the CPU priority for backends that send "
                     "logical replication changes."),
        &CpuPriorityLogicalRepSender, CPU_PRIORITY_INHERIT, cpu_priority_options,
        PGC_SUSET, GUC_STANDARD, NULL, NULL, NULL);

    DefineCustomEnumVariable(
        "spq.create_object_propagation",
        gettext_noop("Controls the behavior of CREATE statements in transactions for "
                     "supported objects"),
        gettext_noop("When creating new objects in transactions this setting is used to "
                     "determine the behavior for propagating. When objects are created "
                     "in a multi-statement transaction block spq needs to switch to "
                     "sequential mode (if not already) to make sure the objects are "
                     "visible to later statements on shards. The switch to sequential "
                     "is "
                     "not always desired. By changing this behavior the user can trade "
                     "off performance for full transactional consistency on the "
                     "creation "
                     "of new objects."),
        &CreateObjectPropagationMode, CREATE_OBJECT_PROPAGATION_IMMEDIATE,
        create_object_propagation_options, PGC_USERSET,
        GUC_NO_SHOW_ALL | GUC_NOT_IN_SAMPLE, NULL, NULL, NULL);

    DefineCustomIntVariable(
        "spq.defer_shard_delete_interval",
        gettext_noop("Sets the time to wait between background deletion for shards."),
        gettext_noop("Shards that are marked for deferred deletion need to be deleted "
                     "in "
                     "the background at a later time. This is done at a regular "
                     "interval "
                     "configured here. The deletion is executed optimistically, it "
                     "tries "
                     "to take a lock on a shard to clean, if the lock can't be acquired "
                     "the background worker moves on. When set to -1 this background "
                     "process is skipped."),
        &DeferShardDeleteInterval, 15000, -1, 7 * 24 * 3600 * 1000, PGC_SIGHUP,
        GUC_UNIT_MS, NULL, NULL, NULL);

    DefineCustomIntVariable(
        "spq.max_rebalancer_logged_ignored_moves",
        gettext_noop("Sets the maximum number of ignored moves the rebalance logs"), NULL,
        &MaxRebalancerLoggedIgnoredMoves, 5, -1, INT_MAX, PGC_USERSET,
        GUC_NO_SHOW_ALL | GUC_NOT_IN_SAMPLE, NULL, NULL, NULL);

    DefineCustomBoolVariable(
        "spq.enable_local_reference_table_foreign_keys",
        gettext_noop("Enables foreign keys from/to local tables"),
        gettext_noop("When enabled, foreign keys between local tables and reference "
                     "tables supported."),
        &EnableLocalReferenceForeignKeys, true, PGC_USERSET, GUC_STANDARD, NULL, NULL,
        NULL);

    DefineCustomBoolVariable(
        "spq.enable_manual_changes_to_shards",
        gettext_noop("Enables dropping and truncating known shards."),
        gettext_noop("Set to false by default. If set to true, enables "
                     "dropping and truncating shards on the coordinator "
                     "(or the workers with metadata)"),
        &EnableManualChangesToShards, false, PGC_USERSET,
        GUC_NO_SHOW_ALL | GUC_NOT_IN_SAMPLE, NULL, NULL, NULL);

    DefineCustomStringVariable("spq.enable_manual_metadata_changes_for_user",
                               gettext_noop("Enables some helper UDFs to modify metadata "
                                            "for the given user"),
                               NULL, &EnableManualMetadataChangesForUser, "", PGC_SIGHUP,
                               GUC_SUPERUSER_ONLY | GUC_NO_SHOW_ALL | GUC_NOT_IN_SAMPLE,
                               NULL, NULL, NULL);

    DefineCustomBoolVariable(
        "spq.enable_metadata_sync", gettext_noop("Enables object and metadata syncing."),
        NULL, &EnableMetadataSync, false, /** @FIXME: should be true */
        PGC_USERSET, GUC_NO_SHOW_ALL | GUC_NOT_IN_SAMPLE, NULL, NULL, NULL);

    DefineCustomBoolVariable(
        "spq.enable_non_colocated_router_query_pushdown",
        gettext_noop("Enables router planner for the queries that reference "
                     "non-colocated distributed tables."),
        gettext_noop("Normally, router planner planner is only enabled for "
                     "the queries that reference colocated distributed tables "
                     "because it is not guaranteed to have the target shards "
                     "always on the same node, e.g., after rebalancing the "
                     "shards. For this reason, while enabling this flag allows "
                     "some degree of optimization for the queries that reference "
                     "non-colocated distributed tables, it is not guaranteed "
                     "that the same query will work after rebalancing the shards "
                     "or altering the shard count of one of those distributed "
                     "tables."),
        &EnableNonColocatedRouterQueryPushdown, false, PGC_USERSET,
        GUC_NO_SHOW_ALL | GUC_NOT_IN_SAMPLE, NULL, NULL, NULL);

    DefineCustomBoolVariable("spq.enable_repartitioned_insert_select",
                             gettext_noop("Enables repartitioned INSERT/SELECTs"), NULL,
                             &EnableRepartitionedInsertSelect, true, PGC_USERSET,
                             GUC_NO_SHOW_ALL | GUC_NOT_IN_SAMPLE, NULL, NULL, NULL);

    DefineCustomBoolVariable("spq.enable_router_execution",
                             gettext_noop("Enables router execution"), NULL,
                             &EnableRouterExecution, true, PGC_USERSET,
                             GUC_NO_SHOW_ALL | GUC_NOT_IN_SAMPLE, NULL, NULL, NULL);

    DefineCustomBoolVariable(
        "spq.enable_single_hash_repartition_joins",
        gettext_noop("Enables single hash repartitioning between hash "
                     "distributed tables"),
        NULL, &EnableSingleHashRepartitioning, false, PGC_USERSET,
        GUC_NO_SHOW_ALL | GUC_NOT_IN_SAMPLE, NULL, NULL, NULL);

    DefineCustomBoolVariable(
        "spq.enable_statistics_collection",
        gettext_noop("Enables sending basic usage statistics to spq."),
        gettext_noop("spq uploads daily anonymous usage reports containing "
                     "rounded node count, shard size, distributed table count, "
                     "and operating system name. This configuration value controls "
                     "whether these reports are sent."),
        &EnableStatisticsCollection,
#if defined(HAVE_LIBCURL) && defined(ENABLE_CITUS_STATISTICS_COLLECTION)
        true,
#else
        false,
#endif
        PGC_SIGHUP, GUC_SUPERUSER_ONLY, &StatisticsCollectionGucCheckHook, NULL, NULL);

    DefineCustomBoolVariable(
        "spq.enable_unique_job_ids",
        gettext_noop("Enables unique job IDs by prepending the local process ID and "
                     "group ID. This should usually be enabled, but can be disabled "
                     "for repeatable output in regression tests."),
        NULL, &EnableUniqueJobIds, true, PGC_USERSET,
        GUC_SUPERUSER_ONLY | GUC_NO_SHOW_ALL | GUC_NOT_IN_SAMPLE, NULL, NULL, NULL);

    DefineCustomBoolVariable(
        "spq.enable_unsafe_triggers",
        gettext_noop("Enables arbitrary triggers on distributed tables which may cause "
                     "visibility and deadlock issues. Use at your own risk."),
        NULL, &EnableUnsafeTriggers, false, PGC_USERSET,
        GUC_STANDARD | GUC_NO_SHOW_ALL | GUC_NOT_IN_SAMPLE, NULL, NULL, NULL);

    DefineCustomBoolVariable(
        "spq.enable_unsupported_feature_messages",
        gettext_noop("Controls showing of some spq related messages. It is intended "
                     "to "
                     "be used before vanilla tests to stop unwanted spq messages."),
        NULL, &EnableUnsupportedFeatureMessages, true, PGC_SUSET,
        GUC_SUPERUSER_ONLY | GUC_NO_SHOW_ALL | GUC_NOT_IN_SAMPLE, NULL, NULL, NULL);

    DefineCustomBoolVariable(
        "spq.enable_version_checks",
        gettext_noop("Enables version checks during CREATE/ALTER EXTENSION commands"),
        NULL, &EnableVersionChecks, true, PGC_USERSET,
        GUC_NO_SHOW_ALL | GUC_NOT_IN_SAMPLE, NULL, NULL, NULL);

    DefineCustomBoolVariable(
        "spq.enforce_foreign_key_restrictions",
        gettext_noop("Enforce restrictions while querying distributed/reference "
                     "tables with foreign keys"),
        gettext_noop("When enabled, cascading modifications from reference tables "
                     "to distributed tables are traced and acted accordingly "
                     "to avoid creating distributed deadlocks and ensure correctness."),
        &EnforceForeignKeyRestrictions, true, PGC_USERSET,
        GUC_NO_SHOW_ALL | GUC_NOT_IN_SAMPLE, NULL, NULL, NULL);

    DefineCustomBoolVariable(
        "spq.enforce_object_restrictions_for_local_objects",
        gettext_noop("Controls some restrictions for local objects."), NULL,
        &EnforceLocalObjectRestrictions, true, PGC_USERSET,
        GUC_NO_SHOW_ALL | GUC_NOT_IN_SAMPLE, NULL, NULL, NULL);

    DefineCustomStringVariable(
        "spq.grep_remote_commands",
        gettext_noop("Applies \"command\" like spq.grep_remote_commands, if returns "
                     "true, the command is logged."),
        NULL, &GrepRemoteCommands, "", PGC_USERSET, GUC_NO_SHOW_ALL | GUC_NOT_IN_SAMPLE,
        NULL, NULL, NULL);

    DefineCustomBoolVariable(
        "spq.hide_spq_dependent_objects",
        gettext_noop("Hides some objects, which depends on spq extension, "
                     "from pg meta class queries. "
                     "It is intended to be used only before postgres vanilla "
                     "tests to not break them."),
        NULL, &HideCitusDependentObjects, false, PGC_USERSET,
        GUC_SUPERUSER_ONLY | GUC_NO_SHOW_ALL | GUC_NOT_IN_SAMPLE, NULL, NULL, NULL);
    DefineCustomIntVariable(
        "spq.max_background_task_executors",
        gettext_noop("Sets the maximum number of parallel task executor "
                     "workers for scheduled "
                     "background tasks"),
        gettext_noop("Controls the maximum number of parallel task executors the task "
                     "monitor "
                     "can create for scheduled background tasks. Note that the value is "
                     "not effective "
                     "if it is set a value higher than 'max_worker_processes' postgres "
                     "parameter . It is "
                     "also not guaranteed to have exactly specified number of parallel "
                     "task executors "
                     "because total background worker count is shared by all background "
                     "workers. The value "
                     "represents the possible maximum number of task executors."),
        &MaxBackgroundTaskExecutors, 4, 1, MAX_BG_TASK_EXECUTORS, PGC_SIGHUP,
        GUC_STANDARD, NULL, NULL, NULL);

    DefineCustomIntVariable(
        "spq.max_background_task_executors_per_node",
        gettext_noop("Sets the maximum number of parallel background task executor "
                     "workers "
                     "for scheduled background tasks that involve a particular node"),
        NULL, &MaxBackgroundTaskExecutorsPerNode, 1, 1, 128, PGC_SIGHUP, GUC_STANDARD,
        NULL, NULL, NULL);

    DefineCustomIntVariable(
        "spq.max_high_priority_background_processes",
        gettext_noop("Sets the maximum number of background processes "
                     "that can have their CPU priority increased at the same "
                     "time on a specific node."),
        gettext_noop("This setting is useful to make sure logical replication "
                     "senders don't take over the CPU of the entire machine."),
        &MaxHighPriorityBackgroundProcesess, 2, 0, 10000, PGC_SUSET, GUC_STANDARD, NULL,
        NULL, NULL);

    DefineCustomIntVariable("spq.metadata_sync_interval",
                            gettext_noop("Sets the time to wait between metadata syncs."),
                            gettext_noop("metadata sync needs to run every so often "
                                         "to synchronize metadata to metadata nodes "
                                         "that are out of sync."),
                            &MetadataSyncInterval, 60 * MS_PER_SECOND, 1, 7 * MS_PER_DAY,
                            PGC_SIGHUP, GUC_UNIT_MS | GUC_NO_SHOW_ALL | GUC_NOT_IN_SAMPLE,
                            NULL, NULL, NULL);

    DefineCustomEnumVariable(
        "spq.metadata_sync_mode",
        gettext_noop("Sets transaction mode for metadata syncs."),
        gettext_noop("metadata sync can be run inside a single coordinated "
                     "transaction or with multiple small transactions in "
                     "idempotent way. By default we sync metadata in single "
                     "coordinated transaction. When we hit memory problems "
                     "at workers, we have alternative nontransactional mode "
                     "where we send each command with separate transaction."),
        &MetadataSyncTransMode, METADATA_SYNC_TRANSACTIONAL, metadata_sync_mode_options,
        PGC_SUSET, GUC_SUPERUSER_ONLY | GUC_NO_SHOW_ALL | GUC_NOT_IN_SAMPLE, NULL, NULL,
        NULL);

    DefineCustomIntVariable(
        "spq.metadata_sync_retry_interval",
        gettext_noop("Sets the interval to retry failed metadata syncs."),
        gettext_noop("metadata sync needs to run every so often "
                     "to synchronize metadata to metadata nodes "
                     "that are out of sync."),
        &MetadataSyncRetryInterval, 5 * MS_PER_SECOND, 1, 7 * MS_PER_DAY, PGC_SIGHUP,
        GUC_UNIT_MS | GUC_NO_SHOW_ALL | GUC_NOT_IN_SAMPLE, NULL, NULL, NULL);

    DefineCustomBoolVariable(
        "spq.override_table_visibility",
        gettext_noop("Enables replacing occurrrences of pg_catalog.pg_table_visible() "
                     "with pg_catalog.spq_table_visible()"),
        gettext_noop("When enabled, shards on the spq MX worker (data) nodes would be "
                     "filtered out by many psql commands to provide better user "
                     "experience."),
        &OverrideTableVisibility, true, PGC_USERSET, GUC_NO_SHOW_ALL | GUC_NOT_IN_SAMPLE,
        NULL, NULL, NULL);

    DefineCustomIntVariable(
        "spq.rebalancer_by_disk_size_base_cost",
        gettext_noop("When using the by_disk_size rebalance strategy each shard group "
                     "will get this cost in bytes added to its actual disk size. This "
                     "is used to avoid creating a bad balance when there's very little "
                     "data in some of the shards. The assumption is that even empty "
                     "shards have some cost, because of parallelism and because empty "
                     "shard groups will likely grow in the future, in KB Unit"),
        gettext_noop("The main reason this is configurable, is so it can be "
                     "lowered for spq its regression tests."),
        &RebalancerByDiskSizeBaseCost, 100 * 1024, 0, INT_MAX, PGC_USERSET,
        GUC_UNIT_KB | GUC_NO_SHOW_ALL | GUC_NOT_IN_SAMPLE, NULL, NULL, NULL);

    DefineCustomIntVariable(
        "spq.remote_task_check_interval",
        gettext_noop("Sets the frequency at which we check job statuses."),
        gettext_noop("The master node assigns tasks to workers nodes, and "
                     "then regularly checks with them about each task's "
                     "progress. This configuration value sets the time "
                     "interval between two consequent checks."),
        &RemoteTaskCheckInterval, 10, 1, INT_MAX, PGC_USERSET, GUC_UNIT_MS | GUC_STANDARD,
        NULL, NULL, NULL);

    DefineCustomIntVariable(
        "spq.repartition_join_bucket_count_per_node",
        gettext_noop("Sets the bucket size for repartition joins per node"),
        gettext_noop("Repartition joins create buckets in each node and "
                     "uses those to shuffle data around nodes. "),
        &RepartitionJoinBucketCountPerNode, 4, 1, INT_MAX, PGC_SIGHUP,
        GUC_STANDARD | GUC_NO_SHOW_ALL | GUC_NOT_IN_SAMPLE, NULL, NULL, NULL);

    DefineCustomBoolVariable("spq.skip_advisory_lock_permission_checks",
                             gettext_noop("Postgres would normally enforce some "
                                          "ownership checks while acquiring locks. "
                                          "When this setting is 'on', spq skips "
                                          "ownership checks on internal advisory "
                                          "locks."),
                             NULL, &SkipAdvisoryLockPermissionChecks, false,
                             (GucContext)GUC_SUPERUSER_ONLY,
                             GUC_NO_SHOW_ALL | GUC_NOT_IN_SAMPLE, NULL, NULL, NULL);

    DefineCustomBoolVariable(
        "spq.skip_constraint_validation", gettext_noop("Skip validation of constraints"),
        gettext_noop("Validating constraints is a costly operation which effects spq' "
                     "performance negatively. With this GUC set to true, we skip "
                     "validating them. Constraint validation can be redundant for some "
                     "cases. For instance, when moving a shard, which has already "
                     "validated constraints at the source; we don't need to validate "
                     "the constraints again at the destination."),
        &SkipConstraintValidation, false, PGC_SUSET, 0, NULL, NULL, NULL);

    DefineCustomBoolVariable(
        "spq.skip_jsonb_validation_in_copy",
        gettext_noop("Skip validation of JSONB columns on the coordinator during COPY "
                     "into a distributed table"),
        gettext_noop("Parsing large JSON objects may incur significant CPU overhead, "
                     "which can lower COPY throughput. If this GUC is set (the "
                     "default), "
                     "JSON parsing is skipped on the coordinator, which means you "
                     "cannot "
                     "see the line number in case of malformed JSON, but throughput "
                     "will "
                     "be higher. This setting does not apply if the input format is "
                     "binary."),
        &SkipJsonbValidationInCopy, true, PGC_USERSET, 0, NULL, NULL, NULL);

    DefineCustomBoolVariable(
        "spq.sort_returning",
        gettext_noop("Sorts the RETURNING clause to get consistent test output"),
        gettext_noop("This feature is not intended for users. It is developed "
                     "to get consistent regression test outputs. When enabled, "
                     "the RETURNING clause returns the tuples sorted. The sort "
                     "is done for all the entries, starting from the first one. "
                     "Finally, the sorting is done in ASC order."),
        &SortReturning, false, PGC_SUSET, GUC_NO_SHOW_ALL | GUC_NOT_IN_SAMPLE, NULL, NULL,
        NULL);

    DefineCustomBoolVariable(
        "spq.enable_subquery_pushdown",
        gettext_noop("Usage of this GUC is highly discouraged, please read the long "
                     "description"),
        gettext_noop("When enabled, the planner skips many correctness checks "
                     "for subqueries and pushes down the queries to shards as-is. "
                     "It means that the queries are likely to return wrong results "
                     "unless the user is absolutely sure that pushing down the "
                     "subquery is safe. This GUC is maintained only for backward "
                     "compatibility, no new users are supposed to use it. The planner "
                     "is capable of pushing down as much computation as possible to the "
                     "shards depending on the query."),
        &SubqueryPushdown, false, PGC_USERSET, GUC_NO_SHOW_ALL | GUC_NOT_IN_SAMPLE,
        NoticeIfSubqueryPushdownEnabled, NULL, NULL);

    DefineCustomEnumVariable(
        "spq.task_assignment_policy",
        gettext_noop("Sets the policy to use when assigning tasks to worker nodes."),
        gettext_noop("The master node assigns tasks to worker nodes based on shard "
                     "locations. This configuration value specifies the policy to "
                     "use when making these assignments. The greedy policy aims to "
                     "evenly distribute tasks across worker nodes, first-replica just "
                     "assigns tasks in the order shard placements were created, "
                     "and the round-robin policy assigns tasks to worker nodes in "
                     "a round-robin fashion."),
        &TaskAssignmentPolicy, TASK_ASSIGNMENT_GREEDY, task_assignment_policy_options,
        PGC_USERSET, GUC_STANDARD, NULL, NULL, NULL);

    DefineCustomBoolVariable(
        "spq.use_spq_managed_tables",
        gettext_noop("Allows new local tables to be accessed on workers"),
        gettext_noop("Adds all newly created tables to spq metadata by default, "
                     "when enabled. Set to false by default."),
        &AddAllLocalTablesToMetadata, false, PGC_USERSET, GUC_STANDARD, NULL, NULL, NULL);

    DefineCustomIntVariable(
        "spq.values_materialization_threshold",
        gettext_noop("Sets the maximum number of rows allowed for pushing down "
                     "VALUES clause in multi-shard queries. If the number of "
                     "rows exceeds the threshold, the VALUES is materialized "
                     "via pull-push execution. When set to -1, materialization "
                     "is disabled. When set to 0, all VALUES are materialized."),
        gettext_noop("When the VALUES is pushed down (i.e., not materialized), "
                     "the VALUES clause needs to be deparsed for every shard on "
                     "the coordinator - and parsed on the workers. As this "
                     "setting increased, the associated overhead is multiplied "
                     "by the shard count. When materialized, the VALUES is "
                     "deparsed and parsed once. The downside of materialization "
                     "is that Postgres may choose a poor plan when joining "
                     "the materialized result with tables."),
        &ValuesMaterializationThreshold, 100, -1, INT_MAX, PGC_USERSET, GUC_STANDARD,
        NULL, NULL, NULL);

    DefineCustomEnumVariable(
        "spq.worker_min_messages",
        gettext_noop("Log messages from workers only if their log level is at or above "
                     "the configured level"),
        NULL, &WorkerMinMessages, NOTICE, log_level_options, PGC_USERSET, GUC_STANDARD,
        NULL, NULL, NULL);

    DefineCustomBoolVariable(
        "spq.writable_standby_coordinator",
        gettext_noop("Enables simple DML via a streaming replica of the coordinator"),
        NULL, &WritableStandbyCoordinator, false, PGC_USERSET, GUC_STANDARD, NULL, NULL,
        NULL);
#endif

    /* warn about config items in the citus namespace that are not registered
     * above */
    EmitWarningsOnPlaceholders("spq");

    OverridePostgresConfigProperties();
}

/*
 * OverridePostgresConfigProperties overrides GUC properties where we want
 * custom behaviour. We should consider using Postgres function find_option
 * in this function once it is exported by Postgres in a later release.
 */
void Session_variables::OverridePostgresConfigProperties(void)
{
    int gucCount = 0;
    struct config_generic** guc_vars = get_guc_variables_compat(&gucCount);

    for (int gucIndex = 0; gucIndex < gucCount; gucIndex++) {
        struct config_generic* var = (struct config_generic*)guc_vars[gucIndex];

        if (strcmp(var->name, "application_name") == 0) {
            struct config_string* stringVar = (struct config_string*)var;

            OldApplicationNameAssignHook = stringVar->assign_hook;
            stringVar->assign_hook = ApplicationNameAssignHook;
        }

        /*
         * Turn on GUC_REPORT for search_path. GUC_REPORT provides that an S
         * (Parameter Status) packet is appended after the C (Command Complete)
         * packet sent from the server for SET command. S packet contains the
         * new value of the parameter if its value has been changed.
         */
        if (strcmp(var->name, "search_path") == 0) {
            var->flags |= GUC_REPORT;
        }
    }
}

/*
 * ApplicationNameAssignHook is called whenever application_name changes
 * to allow us to reset our hide shards decision.
 */
void Session_variables::ApplicationNameAssignHook(const char* newval, void* extra)
{
    ResetHideShardsDecision();
    DetermineCitusBackendType(newval);

    /*
     * We use StartupCitusBackend to initialize the global pid after catalogs
     * are available. After that happens this hook becomes responsible to update
     * the global pid on later application_name changes. So we set the
     * FinishedStartupCitusBackend flag in StartupCitusBackend to indicate when
     * this responsibility handoff has happened.
     *
     * Also note that when application_name changes, we don't actually need to
     * try re-assigning the global pid for external client backends and
     * background workers because application_name doesn't affect the global
     * pid for such backends - note that !IsExternalClientBackend() check covers
     * both types of backends. Plus,
     * trying to re-assign the global pid for such backends would unnecessarily
     * cause performing a catalog access when the cached local node id is
     * invalidated. However, accessing to the catalog tables is dangerous in
     * certain situations like when we're not in a transaction block. And for
     * the other types of backends, i.e., the Citus internal backends, we need
     * to re-assign the global pid when the application_name changes because for
     * such backends we simply extract the global pid inherited from the
     * originating backend from the application_name -that's specified by
     * originating backend when openning that connection- and this doesn't
     * require catalog access.
     *
     * Another solution to the catalog table acccess problem would be to update
     * global pid lazily, like we do for HideShards. But that's not possible
     * for the global pid, since it is stored in shared memory instead of in a
     * process-local global variable. So other processes might want to read it
     * before this process has updated it. So instead we try to set it as early
     * as reasonably possible, which is also why we extract global pids in the
     * AuthHook already (extracting doesn't require catalog access).
     */
    if (Session_ctx::Ctx()->FinishedStartupCitusBackend && !IsExternalClientBackend()) {
        AssignGlobalPID(newval);
    }

    Session_ctx::Vars().OldApplicationNameAssignHook(newval, extra);
}

bool Session_ctx::CurrentInited()
{
    if (CURRENT_EXTENSION_NO == INVALID_EXTENSION_NO ||
        u_sess->attr.attr_common.extension_session_vars_array_size <
            (CURRENT_EXTENSION_NO + 1) ||
        u_sess->attr.attr_common.extension_session_vars_array[CURRENT_EXTENSION_NO] ==
            nullptr) {
        return false;
    }

    return Ctx()->m_inited;
}

Session_ctx* Session_ctx::Ctx()
{
    Assert(CURRENT_EXTENSION_NO != INVALID_EXTENSION_NO);

    if (unlikely(u_sess->attr.attr_common.extension_session_vars_array_size == 0)) {

        /* Some background workers will not call init_session_vars function, such as:
        BGWORKER. But it will use some Session_ctx variables, such as: PlannerLevel
        in SessionDistributedPlanCtx. we create the Session_ctx just for simplify the
        error handling... if we find a better way to handle those Sesscion_ctx variables,
        we can remove following codes. */

        Assert(t_thrd.role != WORKER);
        static constexpr int initExtArraySize = 10;
        u_sess->attr.attr_common.extension_session_vars_array =
            (void**)MemoryContextAllocZero(u_sess->self_mem_cxt,
                                           (Size)(initExtArraySize * sizeof(void*)));
        u_sess->attr.attr_common.extension_session_vars_array_size = initExtArraySize;
    }

    if (unlikely(u_sess->attr.attr_common.extension_session_vars_array_size <
                 CURRENT_EXTENSION_NO + 1)) {
        RepallocSessionVarsArrayIfNecessary();
    }

    if (unlikely(
            u_sess->attr.attr_common.extension_session_vars_array[CURRENT_EXTENSION_NO] ==
            nullptr)) {
        init_session_vars();
    }

    Session_ctx* session_ctx = static_cast<Session_ctx*>(
        u_sess->attr.attr_common.extension_session_vars_array[CURRENT_EXTENSION_NO]);

    return session_ctx;
}

void Session_ctx::InitializeCtx()
{
    if (u_sess->hook_cxt.pluginPlannerHook == nullptr) {
        Assert(ENABLE_THREAD_POOL);
        u_sess->hook_cxt.pluginPlannerHook = reinterpret_cast<void*>(distributed_planner);
    }

    /*
     * init connection hash first. CoordinatedTransactionCallback ->
     * ResetPlacementConnectionManagement/ResetRelationAccessHash will need this.
     * CallXactCallbacks -> CoordinatedTransactionCallback need CitusXactCallbackContext.
     * actually this should be called in _PG_init, before callback assigned, maybe we need
     * to fix it.
     */
    /* Init all transaction context. */
    m_transCtx.InitializeTransCtx();
    m_connCtx.InitializeConnCtx();
    m_rAcessCtx.InitializeRelationAccessCtx();

    /* initialize backendata data before session variables's registion, some assgin hooks
    need those. */
    m_backendDataCtx.InitializeBackendDataCtx();

    /** Register global read-only variables for show command. */
    if (t_thrd.role != MASTER_THREAD) {
        Spq::GlobalROVariables::RegisterAll();
    }

    /* Parse all session parameters. */
    m_sessionVars.RegisterSessionVariables();

    m_inited = true;
}
