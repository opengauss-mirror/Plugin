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

#include <limits.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "postgres.h"

/* necessary to get alloca on illumos */
#ifdef __sun
#include <alloca.h>
#endif

#include "fmgr.h"
#include "miscadmin.h"
#include "safe_lib.h"

#include "catalog/objectaccess.h"
#include "catalog/pg_authid.h"
#include "catalog/pg_extension.h"
#include "commands/explain.h"
#include "commands/extension.h"
#include "commands/seclabel.h"
#include "executor/executor.h"
#include "libpq/auth.h"
#include "optimizer/paths.h"
#include "optimizer/plancat.h"
#include "optimizer/planner.h"
#include "utils/atomic.h"
#include "postmaster/postmaster.h"
#include "replication/walsender.h"
#include "storage/ipc.h"
#include "tcop/tcopprot.h"
#include "utils/guc.h"
#include "utils/guc_tables.h"
#include "utils/inval.h"
#include "utils/lsyscache.h"
#include "utils/syscache.h"
#include "utils/builtins.h"

#include "distributed/adaptive_executor.h"
#include "distributed/backend_data.h"
#include "distributed/background_jobs.h"
#include "distributed/citus_depended_object.h"
#include "distributed/citus_nodefuncs.h"
#include "distributed/citus_safe_lib.h"
#include "distributed/combine_query_planner.h"
#include "distributed/commands.h"
#include "distributed/commands/multi_copy.h"
#include "distributed/commands/utility_hook.h"
#include "distributed/connection_management.h"
#include "distributed/coordinator_protocol.h"
#include "distributed/context/thread_context.h"
#include "distributed/cte_inline.h"
#include "distributed/distributed_deadlock_detection.h"
#include "distributed/distributed_planner.h"
#include "distributed/errormessage.h"
#include "distributed/intermediate_result_pruning.h"
#include "distributed/local_distributed_join_planner.h"
#include "distributed/local_executor.h"
#include "distributed/local_multi_copy.h"
#include "distributed/locally_reserved_shared_connections.h"
#include "distributed/log_utils.h"
#include "distributed/maintenanced.h"
#include "distributed/metadata_cache.h"
#include "distributed/metadata_sync.h"
#include "distributed/metadata_utility.h"
#include "distributed/multi_executor.h"
#include "distributed/multi_explain.h"
#include "distributed/multi_join_order.h"
#include "distributed/multi_logical_optimizer.h"
#include "distributed/multi_physical_planner.h"
#include "distributed/multi_router_planner.h"
#include "distributed/multi_server_executor.h"
#include "distributed/pg_dist_partition.h"
#include "distributed/placement_connection.h"
#include "distributed/priority.h"
#include "distributed/query_pushdown_planning.h"
#include "distributed/query_stats.h"
#include "distributed/recursive_planning.h"
#include "distributed/reference_table_utils.h"
#include "distributed/relation_access_tracking.h"
#include "distributed/remote_commands.h"
#include "distributed/repartition_executor.h"
#include "distributed/resource_lock.h"
#include "distributed/run_from_same_connection.h"
#include "distributed/shard_cleaner.h"
#include "distributed/shard_rebalancer.h"
#include "distributed/shard_transfer.h"
#include "distributed/shared_connection_stats.h"
#include "distributed/shared_library_init.h"
#include "distributed/session_ctx.h"
#include "distributed/subplan_execution.h"
#include "distributed/time_constants.h"
#include "distributed/transaction_management.h"
#include "distributed/transaction_recovery.h"
#include "distributed/utils/directory.h"
#include "distributed/worker_log_messages.h"
#include "distributed/worker_manager.h"
#include "distributed/worker_protocol.h"
#include "distributed/worker_shard_visibility.h"

/* marks shared object as one loadable by the postgres version compiled against */
PG_MODULE_MAGIC;

static char* MitmfifoEmptyString = "";

static THR_LOCAL object_access_hook_type PrevObjectAccessHook = NULL;
static pthread_mutex_t g_initExtensibleNodeLock;
static bool g_extensibleNodeInited = false;

void _PG_init(void);

static void spq_shmem_request(void);
static void CitusObjectAccessHook(ObjectAccessType access, Oid classId, Oid objectId,
                                  int subId, void* arg);
static void DoInitialCleanup(void);
static void ResizeStackToMaximumDepth(void);
static bool IsSequenceOverflowError(ErrorData* edata);
static void RegisterConnectionCleanup(void);
static void RegisterExternalClientBackendCounterDecrement(void);
static void CitusCleanupConnectionsAtExit(int code, Datum arg);
static void DecrementExternalClientBackendCounterAtExit(int code, Datum arg);
static void CreateRequiredDirectories(void);
static bool WarnIfReplicationModelIsSet(int* newval, void** extra, GucSource source);
static void CitusAuthHook(Port* port, int status);
static bool IsSuperuser(char* userName);

static THR_LOCAL ClientAuthentication_hook_type original_client_auth_hook = NULL;

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

/* shared library initialization function */
void _PG_init(void)
{
    if (g_instance.attr.attr_common.enable_thread_pool) {
        ereport(ERROR, (errmsg("Currently spq is not compatiable with thread pool. "),
                        errhint("please disable thread pool by configuring "
                                "enable_thread_pool = OFF. ")));
        return;
    }

    if (!u_sess->misc_cxt.process_shared_preload_libraries_in_progress) {
        if (!ENABLE_THREAD_POOL) {
            ereport(ERROR,
                    (errmsg("Spq can only be loaded via shared_preload_libraries"),
                     errhint("Add spq to shared_preload_libraries configuration "
                             "variable in postgresql.conf in master and workers. Note "
                             "that spq should be at the beginning of "
                             "shared_preload_libraries.")));
        }

        return;
    }

    /*
     * Register contstraint_handler hooks of safestringlib first. This way
     * loading the extension will error out if one of these constraints are hit
     * during load.
     */
    if (t_thrd.role == MASTER_THREAD) {
        set_str_constraint_handler_s(ereport_constraint_handler);
        set_mem_constraint_handler_s(ereport_constraint_handler);
    }

    /*
     * Perform checks before registering any hooks, to avoid erroring out in a
     * partial state.
     *
     * In many cases (e.g. planner and utility hook, to run inside
     * pg_stat_statements et. al.) we have to be loaded before other hooks
     * (thus as the innermost/last running hook) to be able to do our
     * duties. For simplicity insist that all hooks are previously unused.
     *
     * @FIXME: currently, there are some extensions are not compatible with citus
     * such as dolphin..., do we need some check?
     */
    if (u_sess->hook_cxt.pluginPlannerHook != NULL || ProcessUtility_hook != NULL ||
        ExecutorStart_hook != NULL || ExecutorRun_hook != NULL) {
        ereport(WARNING, (errmsg("Spq has to be loaded first"),
                          errhint("Place spq at the beginning of "
                                  "shared_preload_libraries.")));
    }

    ResizeStackToMaximumDepth();

    /*
     * Extend the database directory structure before continuing with
     * initialization - one of the later steps might require them to exist.
     * If in a sub-process (windows / EXEC_BACKEND) this already has been
     * done.
     */
    if (!IsUnderPostmaster && t_thrd.role == MASTER_THREAD) {
        CreateRequiredDirectories();
    }

    if (t_thrd.role == MASTER_THREAD) {
        Spq::GlobalROVariables::RegisterAll();
    }

    /* make our additional node types known */
    if (!g_extensibleNodeInited) {
        AutoMutexLock extensible_lock(&g_initExtensibleNodeLock);
        /* double check after get lock */
        if (!g_extensibleNodeInited) {
            RegisterNodes();
            /* make our custom scan nodes known */
            RegisterCitusCustomScanMethods();
            g_extensibleNodeInited = true;
        }
        extensible_lock.unLock();
    }

    spq_shmem_request();

    /* intercept planner */
    u_sess->hook_cxt.pluginPlannerHook = reinterpret_cast<void*>(distributed_planner);

    /* register for planner hook */
    set_rel_pathlist_hook = multi_relation_restriction_hook;
    get_relation_info_hook = multi_get_relation_info_hook;
    set_join_pathlist_hook = multi_join_restriction_hook;
    ExecutorStart_hook = CitusExecutorStart;
    ExecutorRun_hook = CitusExecutorRun;

    /*
     * Register hook for counting client backends that
     * are successfully authenticated.
     */
    original_client_auth_hook = ClientAuthentication_hook;
    ClientAuthentication_hook = CitusAuthHook;

    MaintenanceDaemonShmemInit();
    /* initialize coordinated transaction management */
    InitializeTransactionManagement();
    InitializeBackendManagement();
    InitializeSpqQueryStats();
    InitializeSharedConnectionStats();

    /* enable modification of pg_catalog tables during pg_upgrade */
    if (u_sess->proc_cxt.IsBinaryUpgrade) {
        SetConfigOption("allow_system_table_mods", "true", PGC_POSTMASTER,
                        PGC_S_OVERRIDE);
    }

    /*
     * In postmasters execution of _PG_init, IsUnderPostmaster will be false and
     * we want to do the cleanup at that time only, otherwise there is a chance that
     * there will be parallel queries and we might do a cleanup for things that are
     * already in use. This is only needed in Windows.
     */
    if (!IsUnderPostmaster && t_thrd.role == MASTER_THREAD) {
        DoInitialCleanup();
    }

    PrevObjectAccessHook = object_access_hook;
    object_access_hook = CitusObjectAccessHook;

    /*
     * Register utility hook. This must be done after loading columnar, so
     * that the citus hook is called first, followed by the columnar hook,
     * followed by standard_ProcessUtility. That allows citus to distribute
     * ALTER TABLE commands before columnar strips out the columnar-specific
     * options.
     */
    PrevProcessUtility =
        (ProcessUtility_hook != NULL) ? ProcessUtility_hook : standard_ProcessUtility;
    ProcessUtility_hook = multi_ProcessUtility;

    Spq::ThreadContext::Initialize();
    InitialzeCitusWaitEventSet();
}

/*
 * Requests any additional shared memory required for spq.
 */
static void spq_shmem_request(void)
{
    RequestAddinShmemSpace(BackendManagementShmemSize());
    RequestAddinShmemSpace(SharedConnectionStatsShmemSize());
    RequestAddinShmemSpace(SpqQueryStatsSharedMemSize());
}

/*
 * DoInitialCleanup does cleanup at start time.
 * Currently it:
 * - Removes intermediate result directories ( in case there are any leftovers)
 */
static void DoInitialCleanup(void)
{
    CleanupJobCacheDirectory();
}

/*
 * Stack size increase during high memory load may cause unexpected crashes.
 * With this alloca call, we are increasing stack size explicitly, so that if
 * it is not possible to increase stack size, we will get an OOM error instead
 * of a crash.
 *
 * This function is called on backend startup. The allocated memory will
 * automatically be released at the end of the function's scope. However, we'd
 * have already expanded the stack and it wouldn't shrink back. So, in a sense,
 * per backend we're securing max_stack_depth kB's of memory on the stack upfront.
 *
 * Not all the backends require max_stack_depth kB's on the stack, so we might end
 * up with unnecessary allocations. However, the default value is 2MB, which seems
 * an acceptable trade-off. Also, allocating memory upfront may perform better
 * under some circumstances.
 */
static void ResizeStackToMaximumDepth(void)
{
#ifndef WIN32
    long max_stack_depth_bytes = u_sess->attr.attr_common.max_stack_depth * 1024L;

    /*
     * Explanation of IGNORE-BANNED:
     * alloca is safe to use here since we limit the allocated size. We cannot
     * use malloc as a replacement, since we actually want to grow the stack
     * here.
     */
    volatile char* stack_resizer =
        static_cast<char*>(alloca(max_stack_depth_bytes)); /* IGNORE-BANNED */

    /*
     * Different architectures might have different directions while
     * growing the stack. So, touch both ends.
     */
    stack_resizer[0] = 0;
    stack_resizer[max_stack_depth_bytes - 1] = 0;

    /*
     * Passing the address to external function also prevents the function
     * from being optimized away, and the debug elog can also help with
     * diagnosis if needed.
     */
    elog(DEBUG5,
         "entry stack is at %p, increased to %p, the top and bottom values of "
         "the stack is %d and %d",
         &stack_resizer[0], &stack_resizer[max_stack_depth_bytes - 1],
         stack_resizer[max_stack_depth_bytes - 1], stack_resizer[0]);

#endif
}

/*
 * IsSequenceOverflowError returns true if the given error is a sequence
 * overflow error.
 */
static bool IsSequenceOverflowError(ErrorData* edata)
{
    static const char* sequenceOverflowedMsgPrefix =
        "nextval: reached maximum value of sequence";
    static const int sequenceOverflowedMsgPrefixLen = 42;

    return edata->elevel == ERROR &&
#ifdef DISABLE_OG_COMMENTS
           edata->sqlerrcode == ERRCODE_SEQUENCE_GENERATOR_LIMIT_EXCEEDED &&
#endif
           edata->message != NULL &&
           strncmp(edata->message, sequenceOverflowedMsgPrefix,
                   sequenceOverflowedMsgPrefixLen) == 0;
}

/*
 * StartupCitusBackend initializes per-backend infrastructure, and is called
 * the first time citus is used in a database.
 *
 * NB: All code here has to be able to cope with this routine being called
 * multiple times in the same backend.  This will e.g. happen when the
 * extension is created, upgraded or dropped. Due to the way we detect the
 * extension being dropped this can also happen when autovacuum runs ANALYZE on
 * pg_dist_partition, see InvalidateDistRelationCacheCallback for details.
 */
void StartupCitusBackend(void)
{
    InitializeMaintenanceDaemonBackend();

    /*
     * For query backends this will be a no-op, because InitializeBackendData
     * is already called from the CitusAuthHook. But for background workers we
     * still need to initialize the backend data.
     */
    InitializeBackendData(u_sess->attr.attr_common.application_name);

    /*
     * If this is an external connection or a background workers this will
     * generate the global PID for this connection. For internal connections
     * this is a no-op, since InitializeBackendData will already have extracted
     * the gpid from the application_name.
     */
    AssignGlobalPID(u_sess->attr.attr_common.application_name);

    SetBackendDataDatabaseId();
    RegisterConnectionCleanup();
    Session_ctx::Ctx()->FinishedStartupCitusBackend = true;
}

/*
 * GetCurrentClientMinMessageLevelName returns the name of the
 * the GUC client_min_messages for its specified value.
 */
const char* GetClientMinMessageLevelNameForValue(int minMessageLevel)
{
    struct config_enum record = {0};
    record.options = log_level_options;
    const char* clientMinMessageLevelName =
        config_enum_lookup_by_value(&record, minMessageLevel);
    return clientMinMessageLevelName;
}

/*
 * RegisterConnectionCleanup cleans up any resources left at the end of the
 * session. We prefer to cleanup before shared memory exit to make sure that
 * this session properly releases anything hold in the shared memory.
 */
static void RegisterConnectionCleanup(void)
{
    static THR_LOCAL bool registeredCleanup = false;
    if (registeredCleanup == false) {
        on_shmem_exit(CitusCleanupConnectionsAtExit, 0);
        registeredCleanup = true;
    }
}

/*
 * RegisterExternalClientBackendCounterDecrement is called when the backend terminates.
 * For all client backends, we register a callback that will undo
 */
static void RegisterExternalClientBackendCounterDecrement(void)
{
    static THR_LOCAL bool registeredCleanup = false;
    if (registeredCleanup == false) {
        on_shmem_exit(DecrementExternalClientBackendCounterAtExit, 0);
        registeredCleanup = true;
    }
}

/*
 * CitusCleanupConnectionsAtExit is called before_shmem_exit() of the
 * backend for the purposes of any clean-up needed.
 */
static void CitusCleanupConnectionsAtExit(int code, Datum arg)
{
    /* properly close all the cached connections */
    ShutdownAllConnections();

    /*
     * Make sure that we give the shared connections back to the shared
     * pool if any. This operation is a no-op if the reserved connections
     * are already given away.
     */
    DeallocateReservedConnections();

    /* we don't want any monitoring view/udf to show already exited backends */
    SetActiveMyBackend(false);
    UnSetGlobalPID(Session_ctx::BackendCtx().MyBackendData);
}

/*
 * DecrementExternalClientBackendCounterAtExit is called before_shmem_exit() of the
 * backend for the purposes decrementing
 */
static void DecrementExternalClientBackendCounterAtExit(int code, Datum arg)
{
    DecrementExternalClientBackendCounter();
}

/*
 * CreateRequiredDirectories - Create directories required for Citus to
 * function.
 *
 * These used to be created by initdb, but that's not possible anymore.
 */
static void CreateRequiredDirectories(void)
{
    const char* subdirs[] = {"pg_foreign_file", "pg_foreign_file/cached",
                             ("base/" PG_JOB_CACHE_DIR)};

    for (int dirNo = 0; dirNo < lengthof(subdirs); dirNo++) {
        int ret = mkdir(subdirs[dirNo], S_IRWXU);

        if (ret != 0 && errno != EEXIST) {
            ereport(ERROR,
                    (errcode_for_file_access(),
                     errmsg("could not create directory \"%s\": %m", subdirs[dirNo])));
        }
    }
}

/*
 * WarnIfReplicationModelIsSet prints a warning when a user sets
 * spq.replication_model.
 */
static bool WarnIfReplicationModelIsSet(int* newval, void** extra, GucSource source)
{
    /* print a warning only when user sets the guc */
    if (source == PGC_S_SESSION) {
        ereport(
            NOTICE,
            (errcode(ERRCODE_WARNING_DEPRECATED_FEATURE),
             errmsg("Setting spq.replication_model has no effect. Please use "
                    "spq.shard_replication_factor instead."),
             errdetail("spq determines the replication model based on the "
                       "replication factor and the replication models of the colocated "
                       "shards. If a colocated table is present, the replication model "
                       "is inherited. Otherwise 'streaming' replication is preferred if "
                       "supported by the replication factor.")));
    }

    return true;
}

/*
 * CitusAuthHook is a callback for client authentication that Postgres provides.
 * Citus uses this hook to count the number of active backends.
 */
static void CitusAuthHook(Port* port, int status)
{
    /*
     * We determine the backend type here because other calls in this hook rely
     * on it, both IsExternalClientBackend and InitializeBackendData. These
     * calls would normally initialize its value based on the application_name
     * global, but this global is not set yet at this point in the connection
     * initialization. So here we determine it based on the value from Port.
     */
    DetermineAuthBackendType(u_sess->proc_cxt.applicationName);

    /* external connections to not have a GPID immediately */
    if (initBackendType == EXTERNAL_CLIENT_BACKEND) {
        /*
         * We raise the shared connection counter pre-emptively. As a result, we may
         * have scenarios in which a few simultaneous connection attempts prevent
         * each other from succeeding, but we avoid scenarios where we oversubscribe
         * the system.
         *
         * By also calling RegisterExternalClientBackendCounterDecrement here, we
         * immediately lower the counter if we throw a FATAL error below. The client
         * connection counter may temporarily exceed maxClientConnections in between.
         */
        RegisterExternalClientBackendCounterDecrement();

        uint32 externalClientCount = IncrementExternalClientBackendCounter();

        /*
         * Limit non-superuser client connections if spq.max_client_connections
         * is set.
         */
        if (MaxClientConnections >= 0 && !IsSuperuser(port->user_name) &&
            externalClientCount > MaxClientConnections) {
            ereport(FATAL, (errcode(ERRCODE_TOO_MANY_CONNECTIONS),
                            errmsg("remaining connection slots are reserved for "
                                   "non-replication superuser connections"),
                            errdetail("the server is configured to accept up to %d "
                                      "regular client connections",
                                      MaxClientConnections)));
        }
    }

    /*
     * Right after this, but before we assign global pid, this backend might
     * get blocked by a DDL as that happens during parsing.
     *
     * That's why, we now initialize its backend data, with the gpid.
     *
     * We do this so that this backend gets the chance to show up in
     * spq_lock_waits.
     *
     * We cannot assign a new global PID yet here, because that would require
     * reading from catalogs, but that's not allowed this early in the
     * connection startup (because no database has been assigned yet).
     *
     * A second reason is for backends that never call StartupCitusBackend. For
     * those we already set the global PID in the backend data here to be able
     * to do blocked process detection on connections that are opened over a
     * replication connection. A replication connection backend will never call
     * StartupCitusBackend, which normally sets up the global PID.
     */
    InitializeBackendData(u_sess->proc_cxt.applicationName, true);

    /* let other authentication hooks to kick in first */
    if (original_client_auth_hook) {
        original_client_auth_hook(port, status);
    }
}

/*
 * IsSuperuser returns whether the role with the given name is superuser. If
 * the user doesn't exist, this simply returns false instead of throwing an
 * error. This is done to not leak information about users existing or not, in
 * some cases postgres is vague about this on purpose. So, by returning false
 * we let postgres return this possibly vague error message.
 */
static bool IsSuperuser(char* roleName)
{
    if (roleName == NULL) {
        return false;
    }

    HeapTuple roleTuple = SearchSysCache1(AUTHNAME, CStringGetDatum(roleName));
    if (!HeapTupleIsValid(roleTuple)) {
        return false;
    }

    Form_pg_authid rform = (Form_pg_authid)GETSTRUCT(roleTuple);
    bool isSuperuser = rform->rolsuper;

    ReleaseSysCache(roleTuple);

    return isSuperuser;
}

/*
 * CitusObjectAccessHook is called when an object is created.
 *
 * We currently use it to track CREATE EXTENSION citus; operations to make sure we
 * clear the metadata if the transaction is rolled back.
 */
static void CitusObjectAccessHook(ObjectAccessType access, Oid classId, Oid objectId,
                                  int subId, void* arg)
{
    if (PrevObjectAccessHook) {
        PrevObjectAccessHook(access, classId, objectId, subId, arg);
    }

    if (!Session_ctx::CurrentInited()) {
        return;
    }

    /* Checks if the access is post_create and that it's an extension id */
    if (access == OAT_POST_CREATE && classId == ExtensionRelationId) {
        /* There's currently an engine bug that makes it difficult to check
         * the provided objectId with extension oid so we will set the value
         * regardless if it's citus being created */
        SetCreateCitusTransactionLevel(GetCurrentTransactionNestLevel());
    }
}
