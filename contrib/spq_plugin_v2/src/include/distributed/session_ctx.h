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
 */

#ifndef SESSION_CTX_H
#define SESSION_CTX_H

#include "distributed/multi_logical_optimizer.h"
#include "distributed/priority.h"
#include "distributed/commands/utility_hook.h"
#include "distributed/citus_custom_scan.h"
#include "distributed/multi_explain.h"
#include "distributed/local_distributed_join_planner.h"
#include "distributed/time_constants.h"
#include "distributed/multi_executor.h"
#include "distributed/log_utils.h"
#include "distributed/query_stats.h"
#include "distributed/metadata_cache.h"
#include "distributed/metadata_sync.h"
#include "distributed/shared_connection_stats.h"
#include "distributed/shard_cleaner.h"
#include "distributed/distributed_planner.h"
#include "distributed/backend_data.h"
#include "distributed/relation_access_tracking.h"
#include "distributed/connection_management.h"
#include "distributed/worker_shard_visibility.h"
#include "distributed/maintenanced.h"

#include "c.h"

#if defined _WIN32 || defined __CYGWIN__
#ifdef BUILDING_DLL
#ifdef __GNUC__
#define DLL_PUBLIC __attribute__((dllexport))
#else
// Note: actually gcc seems to also supports this syntax.
#define DLL_PUBLIC __declspec(dllexport)
#endif
#else
#ifdef __GNUC__
#define DLL_PUBLIC __attribute__((dllimport))
#else
// Note: actually gcc seems to also supports this syntax.
#define DLL_PUBLIC __declspec(dllimport)
#endif
#endif
#define DLL_LOCAL
#else
#if __GNUC__ >= 4
#define DLL_PUBLIC __attribute__((visibility("default")))
#define DLL_LOCAL __attribute__((visibility("hidden")))
#else
#define DLL_PUBLIC
#define DLL_LOCAL
#endif
#endif

extern char* NodeConninfo;

struct Session_variables {
    /* controls use of locks to enforce safe commutativity */
    bool AllModificationsCommutative{false};

    bool AllowModificationsFromWorkersToReplicatedTables{true};
    /*
     * Setting that controls whether distributed queries should be
     * allowed within a task execution.
     */
    bool AllowNestedDistributedExecution{false};

    /*
     * GUC that controls whether to allow unique/exclude constraints without
     * distribution column.
     */
    bool AllowUnsafeConstraints{false};

    bool EnableAcquiringUnsafeLockFromWorkers{false};

    int BackgroundTaskQueueCheckInterval{5000};

    bool CheckAvailableSpaceBeforeMove{true};

    char* CurrentCluster{"default"};

    int CoordinatorAggregationStrategy{COORDINATOR_AGGREGATION_ROW_GATHER};
    /*
     * Data size threshold to switch over the active placement for a connection.
     * If this is too low, overhead of starting COPY commands will hurt the
     * performance. If this is too high, buffered data will use lots of memory.
     * 4MB is a good balance between memory usage and performance. Note that this
     * is irrelevant in the common case where we open one connection per placement.
     */
    int CopySwitchOverThresholdBytes{4 * 1024 * 1024};

    /* precision of count(distinct) approximate */
    double CountDistinctErrorRate{0.0};

    int CpuPriority{0};

    int CpuPriorityLogicalRepSender{CPU_PRIORITY_INHERIT};

    int CreateObjectPropagationMode{CREATE_OBJECT_PROPAGATION_IMMEDIATE};

    int DeferShardDeleteInterval{15000};

    double DesiredPercentFreeAfterMove{10};
    /* config variable for distributed deadlock detection timeout */
    double DistributedDeadlockDetectionTimeoutFactor{2.0};

    bool EnableAlterRolePropagation{true};

    bool EnableAlterRoleSetPropagation{true};

    bool EnableBinaryProtocol{true};

    bool EnableCostBasedConnectionEstablishment{true};

    bool EnableCreateRolePropagation{true};

    bool EnableDDLPropagation{true};

    /* we've deprecated this flag, keeping here for some time not to break existing users
     */
    bool EnableDeadlockPrevention{true};

    bool EnableFastPathRouterPlanner{true};

    bool EnableLocalExecution{true};

    /* controlled via GUC, should be accessed via GetEnableLocalReferenceForeignKeys() */
    bool EnableLocalReferenceForeignKeys{true};

    bool EnableManualChangesToShards{false};

    char* EnableManualMetadataChangesForUser{""};

    bool EnableMetadataSync{false};

    bool EnableNonColocatedRouterQueryPushdown{false};

    bool EnableRepartitionJoins{false};

    bool EnableRepartitionedInsertSelect{true};

    bool EnableRouterExecution{true};

    bool EnableSingleHashRepartitioning{false};

#if defined(HAVE_LIBCURL) && defined(ENABLE_CITUS_STATISTICS_COLLECTION)
    bool EnableStatisticsCollection{true}; /* send basic usage statistics to Citus */
#else
    bool EnableStatisticsCollection{false};
#endif

    bool EnableUniqueJobIds{true};

    /* GUC that overrides trigger checks for distributed tables and reference tables */
    bool EnableUnsafeTriggers{false};
    /*
     * GUC controls showing of some of the unwanted citus messages, it is intended to be
     * set false before vanilla tests to not break postgres test logs.
     */
    bool EnableUnsupportedFeatureMessages{true};

    /* Citus extension version variables */
    bool EnableVersionChecks{true};

    bool EnforceForeignKeyRestrictions{true};

    /*
     * GUC controls some restrictions for local objects. For example,
     * if it is disabled, a local view with no distributed relation dependency
     * will be created even if it has circular dependency and will not
     * log error or warning. Citus will normally restrict that behaviour for the
     * local views. e.g CREATE TABLE local_t (a int);
     *                  CREATE VIEW vv1 as SELECT * FROM local_t;
     *					CREATE OR REPLACE VIEW vv2 as SELECT * FROM vv1;
     *
     * When the GUC is disabled, citus also wont distribute the local
     * view which has no citus relation dependency to workers. Otherwise, it distributes
     * them by default. e.g create view v as select 1;
     */
    bool EnforceLocalObjectRestrictions{true};

    /* GUC, number of ms to wait between opening connections to the same worker */
    int ExecutorSlowStartInterval{10};

    bool ExplainAllTasks{false};

    int ExplainAnalyzeSortMethod{EXPLAIN_ANALYZE_SORT_BY_TIME};

    /* Config variables that enable printing distributed query plans */
    bool ExplainDistributedQueries{true};

    /* GUC, determining whether Citus opens 1 connection per task */
    bool ForceMaxQueryParallelization{false};

    /* if disabled, distributed statements in a function may run as separate transactions
     */
    bool FunctionOpensTransactionBlock{true};

    char* GrepRemoteCommands{""};

    /*
     * GUC hides any objects, which depends on citus extension, from pg meta class
     * queries, it is intended to be used in vanilla tests to not break postgres test logs
     */
    bool HideCitusDependentObjects{false};

    char* DeprecatedEmptyString{""};

    int LimitClauseRowFetchCount{-1};

    /*
     * LocalCopyFlushThresholdByte is the threshold for local copy to be flushed.
     * There will be one buffer for each local placement, when the buffer size
     * exceeds this threshold, it will be flushed.
     *
     * Managed via GUC, the default is 512 kB.
     */
    int LocalCopyFlushThresholdByte{512 * 1024};

    char* LocalHostName{"localhost"};

    /*
     * Controlled via a GUC, never access directly, use GetLocalSharedPoolSize().
     *  "0" means adjust LocalSharedPoolSize automatically by using MaxConnections.
     * "-1" means do not use any remote connections for local tasks
     * Anything else means use that number
     */
    int LocalSharedPoolSize{0};

    int LocalTableJoinPolicy{LOCAL_JOIN_POLICY_AUTO};

    /* GUC, determining whether debug messages for deadlock detection sent to LOG */
    bool LogDistributedDeadlockDetection{false};

    /* controlled via GUC, used mostly for testing */
    bool LogIntermediateResults{false};

    bool LogLocalCommands{false};

    /* print join order as a debugging aid */
    bool LogMultiJoinOrder{false};

    /* GUC, determining whether statements sent to remote nodes are logged */
    bool LogRemoteCommands{false};

    int MaxAdaptiveExecutorPoolSize{16};

    int MaxBackgroundTaskExecutors{4};

    int MaxBackgroundTaskExecutorsPerNode{1};

    /* maximum lifetime of connections in miliseconds */
    int MaxCachedConnectionLifetime{10 * MS_PER_MINUTE};

    /* maximum number of connections to cache per worker per session */
    int MaxCachedConnectionsPerWorker{1};

    int MaxHighPriorityBackgroundProcesess{2};

    /* maximum size in KB the intermediate result can grow to */
    int MaxIntermediateResult{1048576};

    /* controlled by GUC, in MB */
    int MaxMatViewSizeToAutoRecreate{1024};

    int MaxRebalancerLoggedIgnoredMoves{5};

    /*
     * Controlled via a GUC, never access directly, use GetMaxSharedPoolSize().
     *  "0" means adjust MaxSharedPoolSize automatically by using MaxConnections.
     * "-1" means do not apply connection throttling
     * Anything else means use that number
     */
    int MaxSharedPoolSize{0};

    /* config variables for metadata sync timeout */
    int MetadataSyncInterval{60000};

    int MetadataSyncTransMode{METADATA_SYNC_TRANSACTIONAL};

    int MetadataSyncRetryInterval{5000};

    /*
     * Controls the connection type for multi shard modifications, DDLs
     * TRUNCATE and multi-shard SELECT queries.
     */
    int MultiShardConnectionType{PARALLEL_CONNECTION};

    /* multi-task query log level */
    int MultiTaskQueryLogLevel{CITUS_LOG_LEVEL_OFF};

    int NextCleanupRecordId{0};

    int NextOperationId{0};

    int NextPlacementId{0};

    int NextShardId{0};

    /* maximum duration to wait for connection */
    int NodeConnectionTimeout{30000};

    bool OverrideTableVisibility{true};

    bool PreventIncompleteConnectionEstablishment{true};

    bool PropagateSessionSettingsForLoopbackConnection{false};

    PropSetCmdBehavior PropagateSetCommands{PROPSETCMD_NONE};

    int RebalancerByDiskSizeBaseCost{100 * 1024 * 1024};

    /* GUC to configure interval for 2PC auto-recovery */
    int Recover2PCInterval{60000};

    /*
     * Setting that controls how many bytes of COPY data libpq is allowed to buffer
     * internally before we force a flush.
     */
    int RemoteCopyFlushThreshold{8 * 1024 * 1024};

    /* per cycle sleep interval in millisecs */
    int RemoteTaskCheckInterval{10};

    /* RepartitionJoinBucketCountPerNode determines bucket amount during repartitions */
    int RepartitionJoinBucketCountPerNode{4};

    bool RunningUnderCitusTestSuite{false};

    /*
     * GUC that determines whether a SELECT in a transaction block should also run in
     * a transaction block on the worker even if no writes have occurred yet.
     */
    bool SelectOpensTransactionBlock{true};

    /* Shard related configuration */
    int ShardCount{32};

    /* desired replication factor for shards */
    int ShardReplicationFactor{1};

    /* show shards when the application_name starts with one of: */
    char* ShowShardsForAppNamePrefixes{""};

    bool SkipAdvisoryLockPermissionChecks{false};

    bool SkipConstraintValidation{false};

    /* if true, skip validation of JSONB columns during COPY */
    bool SkipJsonbValidationInCopy{true};

    /* sort the returning to get consistent outputs, used only for testing */
    bool SortReturning{false};

    /* time interval in seconds for maintenance daemon to call
     * CitusQueryStatsSynchronizeEntries */
    int StatStatementsPurgeInterval{10};

    /* tracking all or none, for spq_stat_statements, controlled by GUC
     * spq.stat_statements_track */
    int StatStatementsTrack{STAT_STATEMENTS_TRACK_NONE};

    /* is subquery pushdown enabled */
    bool SubqueryPushdown{false};

    /* Policy to use when assigning tasks to worker nodes */
    int TaskAssignmentPolicy{TASK_ASSIGNMENT_GREEDY};

    /* distributed executor type */
    int TaskExecutorType{MULTI_EXECUTOR_ADAPTIVE};

    /*
     * Global variable for the GUC spq.use_citus_managed_tables.
     * This is used after every CREATE TABLE statement in utility_hook.c
     * If this variable is set to true, we add all created tables to metadata.
     */
    bool AddAllLocalTablesToMetadata{false};

    int ReadFromSecondaries{USE_SECONDARY_NODES_NEVER};

    int ValuesMaterializationThreshold{100};

    /*
     * WorkerMinMessages reflects the value of the spq.worker_min_messages setting which
     * control the minimum log level of messages from the worker that are propagated to
     * the client and the log on the coordinator.
     */
    int WorkerMinMessages{NOTICE};

    bool WritableStandbyCoordinator{false};

    /** Register all session configuration variables. */
    void RegisterSessionVariables();

    /*
     * We don't want to allow values less than 1.0. However, we define -1 as the value to
     * disable distributed deadlock checking. Here we enforce our special constraint.
     */
    static bool ErrorIfNotASuitableDeadlockFactor(double* newval, void** extra,
                                                  GucSource source);

    static bool StatisticsCollectionGucCheckHook(bool* newval, void** extra,
                                                 GucSource source);
    /*
     * LocalPoolSizeGucShowHook overrides the value that is shown to the
     * user when the default value has not been set.
     */
    static const char* LocalPoolSizeGucShowHook(void);
    /*
     * MaxSharedPoolSizeGucShowHook overrides the value that is shown to the
     * user when the default value has not been set.
     */
    static const char* MaxSharedPoolSizeGucShowHook(void);

    /*
     * ShowShardsForAppNamePrefixesCheckHook ensures that the
     * spq.show_shards_for_app_name_prefixes holds a valid list of application_name
     * values.
     */
    static bool ShowShardsForAppNamePrefixesCheckHook(char** newval, void** extra,
                                                      GucSource source);
    /*
     * ShowShardsForAppNamePrefixesAssignHook ensures changes to
     * spq.show_shards_for_app_name_prefixes are reflected in the decision
     * whether or not to show shards.
     */
    static void ShowShardsForAppNamePrefixesAssignHook(const char* newval, void* extra);
    /*
     * NoticeIfSubqueryPushdownEnabled prints a notice when a user sets
     * spq.subquery_pushdown to ON. It doesn't print the notice if the
     * value is already true.
     */
    static bool NoticeIfSubqueryPushdownEnabled(bool* newval, void** extra,
                                                GucSource source);

    /*
     * CpuPriorityAssignHook changes the priority of the current backend to match
     * the chosen value.
     */
    static void CpuPriorityAssignHook(int newval, void* extra);

    /*
     * ApplicationNameAssignHook is called whenever application_name changes
     * to allow us to reset our hide shards decision.
     */
    static void ApplicationNameAssignHook(const char* newval, void* extra);

    /* we override the application_name assign_hook and keep a pointer to the old one */
    GucStringAssignHook OldApplicationNameAssignHook{nullptr};

private:
    /*
     * OverridePostgresConfigProperties overrides GUC properties where we want
     * custom behaviour. We should consider using Postgres function find_option
     * in this function once it is exported by Postgres in a later release.
     */
    void OverridePostgresConfigProperties(void);
};

namespace Spq {

struct GlobalROVariables {
    /** Register all read-only variables */
    static void RegisterAll();

    /** The hook for disable updating some spq's variables, because PGC_POSTMASTER is
     not allowed in init_session_vars. */
    static bool ReadOnlyIntCheckHook(int* newval, void** extra, GucSource source);

    /*
     * NodeConninfoGucCheckHook ensures conninfo settings are in the expected form
     * and that the keywords of all non-null settings are on a allowlist devised to
     * keep users from setting options that may result in confusion.
     */
    static bool NodeConninfoGucCheckHook(char** newval, void** extra, GucSource source);

    /*
     * NodeConninfoGucAssignHook is the assignment hook for the node_conninfo GUC
     * variable. Though this GUC is a "string", we actually parse it as a non-URI
     * PQconninfo key/value setting, storing the resultant PQconninfoOption values
     * using the public functions in connection_configuration.c.
     */
    static void NodeConninfoGucAssignHook(const char* newval, void* extra);
};

}  // namespace Spq

struct SessionUtilCtx {
    /* global variable keeping track of whether we are in a delegated function call */
    bool InTopLevelDelegatedFunctionCall{false};

    /*
     * WorkerErrorIndication can contain a warning that arrives to use from one session,
     * but occurred because another session in the same distributed transaction threw an
     * error. We store this warning in case we do not get an error, in which case the
     * warning should have been an error (and usually indicates a bug).
     */
    DeferredErrorMessage* WorkerErrorIndication{nullptr};

    /*
     * PreserveWorkerMessageLogLevel specifies whether to propagate messages from workers
     * to the client and the log on the coordinator with their original log level. When
     * false, messages are propagated using DEBUG1.
     *
     * This flag used to suppress redundant notices in some commands (e.g. VACUUM, DROP
     * TABLE).
     */
    bool PreserveWorkerMessageLogLevel{false};

    /* global variable tracking whether we are in a delegated procedure call */
    bool InDelegatedProcedureCall{false};

    /*
     * Flag to keep track of whether the process is currently in a function converting the
     * type of the table. Since it only affects the level of the log shown while dropping/
     * recreating table within the table type conversion, rollbacking to the savepoint
     * hasn't been implemented for the sake of simplicity. If you are planning to use that
     * flag for any other purpose, please consider implementing that.
     */
    bool InTableTypeConversionFunctionCall{false};

    /** Some statistics for utility hook */
    int activeAlterTables{0};
    int activeDropSchemaOrDBs{0};
    bool ConstraintDropped{false};

    int UtilityHookLevel{0};

    /* operation ID set by RegisterOperationNeedingCleanup */
    OperationId CurrentOperationId{INVALID_OPERATION_ID};

    /* cache of whether or not to hide shards */
    HideShardsMode HideShards{CHECK_APPLICATION_NAME};

    /* see the comment in master_move_shard_placement */
    bool PlacementMovedUsingLogicalReplicationInTX{false};

    /** Whether or not citus extension has been created in current session. */
    SpqExtensionState m_spqState{SpqExtensionState::UNKNOWN};
};

/** The class for session context, because opengauss's seesion and thread are detachable,
 the session context is binded on u_sess->attr.attr_common.extension_session_vars_array.
 So the class Session_ctx can only be instantiated from function init_session_vars, and
 can only be access by u_sess->attr.attr_common.extension_session_vars_array. */
class Session_ctx {
public:
    /** Construct a new Session_ctx object
     @param[in]     mem     the memorycontext for current member variables's memory
     allocation. */
    Session_ctx(MemoryContext mem) : m_ctxMemory(mem)
    {}

    /*
     * Flag to indicate when ApplicationNameAssignHook becomes responsible for
     * updating the global pid.
     */
    bool FinishedStartupCitusBackend{false};

    /** Initialize all member context. */
    void InitializeCtx();

    /** Whether or not the session context is initiized.
     @return  true  the current session initiized. */
    static bool CurrentInited();

    /** Current extension's load number, it will be assgined in function
     * set_extension_index. */
    static uint32 CURRENT_EXTENSION_NO;

    /** Get current Session_ctx's instance from u_sess->attr.attr_common.
     extension_session_vars_array.
     @return a pointer for current session context instance */
    static Session_ctx* Ctx();

    /** Get current session vars's instance.
     @return the reference of the context for all citus session variables. */
    static Session_variables& Vars()
    {
        return Ctx()->m_sessionVars;
    }

    /** Get the session's distributed plan context.
     @return the reference of the session's distributed plan context object. */
    static SessionDistributedPlanCtx& PlanCtx()
    {
        return Ctx()->m_distPlanCtx;
    }

    /** Get the session's backend thread data context.
     @return the reference of the session's backend thread data context object. */
    static SessionBackendDataCtx& BackendCtx()
    {
        return Ctx()->m_backendDataCtx;
    }

    /** Get the session's trnasaction context.
     @return the reference of the session's trnasaction context object. */
    static SessionTransactionCtx& Trans()
    {
        return Ctx()->m_transCtx;
    }

    /** Get the session's relation accessing context.
     @return the reference of the session's relation accessing context object. */
    static SessionRelationAccessCtx& RACtx()
    {
        return Ctx()->m_rAcessCtx;
    }

    /** Get the session's worker connections cache context.
     @return the reference of the session's worker connections cache context object.*/
    static SessionConnCtx& ConnCtx()
    {
        return Ctx()->m_connCtx;
    }

    /** Get the session's daemon context.
     @return the reference of session's daemon context object. */
    static SessionDaemonCtx& DaemonCtx()
    {
        return Ctx()->m_daemonCtx;
    }

    /** Get the session's executor context.
     @return the reference of session's executor context object. */
    static SessionExecutorCtx& ExecCtx()
    {
        return Ctx()->m_executorCtx;
    }

    /** The session's utility context.
     @return the reference of session's utility context object. */
    static SessionUtilCtx& UtilCtx()
    {
        return Ctx()->m_utilCtx;
    }

private:
    /** All following contexts allocated in this memorycontext. */
    MemoryContext m_ctxMemory{nullptr};

    /** All session's variables. */
    Session_variables m_sessionVars{};

    /** The session's distributed plan context. */
    SessionDistributedPlanCtx m_distPlanCtx{};

    /** The session's backend thread data context. */
    SessionBackendDataCtx m_backendDataCtx{};

    /** The session's trnasaction context. */
    SessionTransactionCtx m_transCtx{};

    /** The session's relation accessing context. */
    SessionRelationAccessCtx m_rAcessCtx{};

    /** The session's daemon context. */
    SessionDaemonCtx m_daemonCtx{};

    /** The session's worker connections cache context. */
    SessionConnCtx m_connCtx{};

    /** The session's executor context. */
    SessionExecutorCtx m_executorCtx{};

    /** The session's utility context. */
    SessionUtilCtx m_utilCtx{};

    bool m_inited{false};
};

/** The following functions will be revoked automatically during load the extension
library, detail see opengauss's internal_load_library function. */
extern "C" DLL_PUBLIC void set_extension_index(uint32 index);
extern "C" DLL_PUBLIC void init_session_vars(void);
#endif
