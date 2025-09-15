/*-------------------------------------------------------------------------
 *
 * maintenanced.c
 *	  Background worker run for each citus using database in a postgres
 *    cluster.
 *
 * This file provides infrastructure for launching exactly one a background
 * worker for every database in which citus is used.  That background worker
 * can then perform work like deadlock detection, prepared transaction
 * recovery, and cleanup.
 *
 * Copyright (c) Citus Data, Inc.
 *
 *-------------------------------------------------------------------------
 */

#include <time.h>

#include "postgres.h"

#include "miscadmin.h"
#include "pgstat.h"

#include "access/xact.h"
#include "access/xlog.h"
#include "catalog/namespace.h"
#include "catalog/pg_authid.h"
#include "catalog/pg_extension.h"
#include "catalog/pg_namespace.h"
#include "commands/async.h"
#include "commands/extension.h"
#include "libpq/pqsignal.h"
#include "nodes/makefuncs.h"
#include "postmaster/bgworker.h"
#include "postmaster/postmaster.h"
#include "storage/ipc.h"
#include "storage/latch.h"
#include "storage/lmgr.h"
#include "storage/lock/lwlock.h"
#include "storage/proc.h"
#include "tcop/tcopprot.h"
#include "utils/builtins.h"
#include "utils/lsyscache.h"
#include "utils/memutils.h"

#include "spq_version.h"
#include "pg_version_constants.h"

#include "distributed/background_jobs.h"
#include "distributed/citus_safe_lib.h"
#include "distributed/coordinator_protocol.h"
#include "distributed/distributed_deadlock_detection.h"
#include "distributed/maintenanced.h"
#include "distributed/metadata_cache.h"
#include "distributed/metadata_sync.h"
#include "distributed/query_stats.h"
#include "distributed/resource_lock.h"
#include "distributed/shard_cleaner.h"
#include "distributed/transaction_recovery.h"
#include "distributed/version_compat.h"
#include "distributed/session_ctx.h"

/*
 * Per database worker state.
 */
typedef struct MaintenanceDaemonDBData {
    /* hash key: database to run on */
    Oid databaseOid;

    /* information: which user to use */
    Oid userOid;
    ThreadId workerPid;
    bool daemonStarted;
    bool triggerNodeMetadataSync;
    Latch* latch; /* pointer to the background worker's latch */
} MaintenanceDaemonDBData;

static pthread_mutex_t g_maintenanceDaemonControlLock;
/*
 * Hash-table of workers, one entry for each database with citus
 * activated. instance level global var, for all backend use.
 */
static HTAB* MaintenanceDaemonDBHash = nullptr;

static void MaintenanceDaemonSigTermHandler(SIGNAL_ARGS);
static void MaintenanceDaemonSigHupHandler(SIGNAL_ARGS);
static void MaintenanceDaemonShmemExit(int code, Datum arg);
static void MaintenanceDaemonErrorContext(void* arg);
static bool MetadataSyncTriggeredCheckAndReset(MaintenanceDaemonDBData* dbData);
static void WarnMaintenanceDaemonNotStarted(void);

/*
 * InitializeMaintenanceDaemonBackend, called at backend start and
 * configuration changes, is responsible for starting a per-database
 * maintenance worker if necessary.
 */
void InitializeMaintenanceDaemonBackend(void)
{
    Oid extensionOwner = CitusExtensionOwner();
    bool found;

    AutoMutexLock maintenanceDaemonControlLock(&g_maintenanceDaemonControlLock);
    maintenanceDaemonControlLock.lock();

    MaintenanceDaemonDBData* dbData = (MaintenanceDaemonDBData*)hash_search(
        MaintenanceDaemonDBHash, &MyDatabaseId, HASH_ENTER_NULL, &found);

    if (dbData == NULL) {
        WarnMaintenanceDaemonNotStarted();
        maintenanceDaemonControlLock.unLock();

        return;
    }

    if (!found) {
        /* ensure the values in MaintenanceDaemonDBData are zero */
        memset(((char*)dbData) + sizeof(Oid), 0,
               sizeof(MaintenanceDaemonDBData) - sizeof(Oid));
    }

    if (Session_ctx::DaemonCtx().IsMaintenanceDaemon) {
        /*
         * InitializeMaintenanceDaemonBackend is called by the maintenance daemon
         * itself. In that case, we clearly don't need to start another maintenance
         * daemon.
         */
        maintenanceDaemonControlLock.unLock();
        return;
    }

    if (!found || !dbData->daemonStarted) {
        Assert(dbData->workerPid == 0);
#ifdef DISABLE_OG_COMMENTS
        BackgroundWorker worker;
        BackgroundWorkerHandle* handle = NULL;

        memset(&worker, 0, sizeof(worker));

        SafeSnprintf(worker.bgw_name, sizeof(worker.bgw_name),
                     "Spq Maintenance Daemon: %u/%u", MyDatabaseId, extensionOwner);

        /* request ability to connect to target database */
        worker.bgw_flags = BGWORKER_SHMEM_ACCESS | BGWORKER_BACKEND_DATABASE_CONNECTION;

        /*
         * No point in getting started before able to run query, but we do
         * want to get started on Hot-Standby.
         */
        worker.bgw_start_time = BgWorkerStart_ConsistentState;

        /* Restart after a bit after errors, but don't bog the system. */
        worker.bgw_restart_time = 5;
        strcpy_s(worker.bgw_library_name, sizeof(worker.bgw_library_name),
                 SPQ_EXTENSION_NAME);
        strcpy_s(worker.bgw_function_name, sizeof(worker.bgw_library_name),
                 "SpqMaintenanceDaemonMain");

        worker.bgw_main_arg = ObjectIdGetDatum(MyDatabaseId);

        memcpy_s(worker.bgw_extra, sizeof(worker.bgw_extra), &extensionOwner,
                 sizeof(Oid));
        worker.bgw_notify_pid = t_thrd.proc_cxt.MyProcPid;
        if (!RegisterDynamicBackgroundWorker(&worker, &handle)) {
            WarnMaintenanceDaemonNotStarted();
            dbData->daemonStarted = false;
            maintenanceDaemonControlLock.unLock();

            return;
        }
#else
        Datum* database_oid = (Datum*)MemoryContextAlloc(
            INSTANCE_GET_MEM_CXT_GROUP(MEMORY_CONTEXT_STORAGE), sizeof(Datum));
        *database_oid = ObjectIdGetDatum(MyDatabaseId);
        if (LaunchBackgroundWorkers(1, database_oid, CitusMaintenanceDaemonMain, NULL,
                                    BGWORKER_FLAG_INDIVIDUAL_THREAD) != 1) {
            WarnMaintenanceDaemonNotStarted();
            dbData->daemonStarted = false;
            maintenanceDaemonControlLock.unLock();
            return;
        }
#endif
        dbData->daemonStarted = true;
        dbData->userOid = extensionOwner;
        dbData->workerPid = 0;
        dbData->triggerNodeMetadataSync = false;
        maintenanceDaemonControlLock.unLock();

        pid_t pid;
#ifdef DISABLE_OG_COMMENTS
        WaitForBackgroundWorkerStartup(handle, &pid);
        pfree(handle);
#endif
    } else {
        Assert(dbData->daemonStarted);

        /*
         * If owner of extension changed, wake up daemon. It'll notice and
         * restart.
         */
        if (dbData->userOid != extensionOwner) {
            dbData->userOid = extensionOwner;
            if (dbData->latch) {
                SetLatch(dbData->latch);
            }
        }
        maintenanceDaemonControlLock.unLock();
    }
}

/*
 * WarnMaintenanceDaemonNotStarted warns that maintenanced couldn't be started.
 */
static void WarnMaintenanceDaemonNotStarted(void)
{
    ereport(WARNING, (errmsg("could not start maintenance background worker"),
                      errhint("Increasing max_worker_processes might help.")));
}

/*
 * CitusMaintenanceDaemonMain is the maintenance daemon's main routine, it'll
 * be started by the background worker infrastructure.  If it errors out,
 * it'll be restarted after a few seconds.
 */
void CitusMaintenanceDaemonMain(const BgWorkerContext* bwc)
{
    Datum main_arg = *(Datum*)(bwc->bgshared);
    Oid databaseOid = DatumGetObjectId(main_arg);

    TimestampTz lastRecoveryTime = 0;
    TimestampTz lastShardCleanTime = 0;
    TimestampTz lastStatStatementsPurgeTime = 0;
    TimestampTz nextMetadataSyncTime = 0;

    /* state kept for the background tasks queue monitor */
    TimestampTz lastBackgroundTaskQueueCheck = GetCurrentTimestamp();
    BackgroundWorkerHandle* backgroundTasksQueueBgwHandle = nullptr;
    bool backgroundTasksQueueWarnedForLock = false;

    /*
     * We do metadata sync in a separate background worker. We need its
     * handle to be able to check its status.
     */
#ifdef OG_DISABLE_COMMENTS
    BackgroundWorkerHandle* metadataSyncBgwHandle = NULL;
#endif
    /*
     * Look up this worker's configuration.
     */
    AutoMutexLock maintenanceDaemonControlLock(&g_maintenanceDaemonControlLock);
    maintenanceDaemonControlLock.lock();

    MaintenanceDaemonDBData* myDbData = (MaintenanceDaemonDBData*)hash_search(
        MaintenanceDaemonDBHash, &databaseOid, HASH_FIND, NULL);
    if (!myDbData) {
        /*
         * When the database crashes, background workers are restarted, but
         * the state in shared memory is lost. In that case, we exit and
         * wait for a session to call InitializeMaintenanceDaemonBackend
         * to properly add it to the hash.
         */

        proc_exit(0);
    }

    if (myDbData->workerPid != 0) {
        /*
         * Another maintenance daemon is running. This usually happens because
         * postgres restarts the daemon after an non-zero exit, and
         * InitializeMaintenanceDaemonBackend started one before postgres did.
         * In that case, the first one stays and the last one exits.
         */

        proc_exit(0);
    }

    on_shmem_exit(MaintenanceDaemonShmemExit, main_arg);

    /*
     * Signal that I am the maintenance daemon now.
     *
     * From this point, DROP DATABASE/EXTENSION will send a SIGTERM to me.
     */
    myDbData->workerPid = t_thrd.proc_cxt.MyProcPid;

    /*
     * Signal that we are running. This in mainly needed in case of restart after
     * an error, otherwise the daemonStarted flag is already true.
     */
    myDbData->daemonStarted = true;

    /* wire up signals */
    gspqsignal(SIGTERM, MaintenanceDaemonSigTermHandler);
    gspqsignal(SIGHUP, MaintenanceDaemonSigHupHandler);

    myDbData->latch = &(t_thrd.proc->procLatch);
    Session_ctx::DaemonCtx().IsMaintenanceDaemon = true;

    maintenanceDaemonControlLock.unLock();

    /*
     * Setup error context so log messages can be properly attributed. Some of
     * them otherwise sound like they might be from a normal user connection.
     * Do so before setting up signals etc, so we never exit without the
     * context setup.
     */
    ErrorContextCallback errorCallback = {0};
    memset(&errorCallback, 0, sizeof(errorCallback));
    errorCallback.callback = MaintenanceDaemonErrorContext;
    errorCallback.arg = (void*)myDbData;
    errorCallback.previous = t_thrd.log_cxt.error_context_stack;
    t_thrd.log_cxt.error_context_stack = &errorCallback;

    elog(LOG, "starting maintenance daemon on database %u user %u", databaseOid,
         myDbData->userOid);

    SetProcessingMode(NormalProcessing);
    /* make worker recognizable in pg_stat_activity */
    pgstat_report_appname("Spq Maintenance Daemon");
    pgstat_report_activity(STATE_IDLE, NULL);

    /*
     * Terminate orphaned metadata sync daemons spawned from previously terminated
     * or crashed maintenanced instances.
     */
    SignalMetadataSyncDaemon(databaseOid, SIGTERM);

    /* enter main loop */
    while (!Session_ctx::DaemonCtx().got_SIGTERM) {
        int latchFlags = WL_LATCH_SET | WL_TIMEOUT | WL_POSTMASTER_DEATH;
        double timeout = 10000.0; /* use this if the deadlock detection is disabled */
        bool foundDeadlock = false;

        CHECK_FOR_INTERRUPTS();

        CitusTableCacheFlushInvalidatedEntries();

        /*
         * XXX: Each task should clear the metadata cache before every iteration
         * by calling InvalidateMetadataSystemCache(), because otherwise it
         * might contain stale OIDs. It appears that in some cases invalidation
         * messages for a DROP EXTENSION may arrive during these tasks and
         * this causes us to cache a stale pg_dist_node OID. We'd actually expect
         * all invalidations to arrive after obtaining a lock in LockCitusExtension.
         */

        /*
         * Perform Work. If a specific task needs to be called sooner than
         * timeout indicates, it's ok to lower it to that value. Expensive
         * tasks should do their own time math about whether to re-run checks.
         */

#ifdef DISABLE_OG_COMMENTS
        pid_t metadataSyncBgwPid = 0;
        BgwHandleStatus metadataSyncStatus =
            metadataSyncBgwHandle != NULL
                ? GetBackgroundWorkerPid(metadataSyncBgwHandle, &metadataSyncBgwPid)
                : BGW_STOPPED;

        if (metadataSyncStatus != BGW_STOPPED &&
            GetCurrentTimestamp() >= nextMetadataSyncTime) {
            /*
             * Metadata sync is still running, recheck in a short while.
             */
            int nextTimeout = MetadataSyncRetryInterval;
            nextMetadataSyncTime =
                TimestampTzPlusMilliseconds(GetCurrentTimestamp(), nextTimeout);
            timeout = Min(timeout, nextTimeout);
        } else if (!RecoveryInProgress() && metadataSyncStatus == BGW_STOPPED &&
                   (MetadataSyncTriggeredCheckAndReset(myDbData) ||
                    GetCurrentTimestamp() >= nextMetadataSyncTime)) {
            if (metadataSyncBgwHandle) {
                pfree(metadataSyncBgwHandle);
                metadataSyncBgwHandle = NULL;
            }

            InvalidateMetadataSystemCache();
            StartTransactionCommand();
            PushActiveSnapshot(GetTransactionSnapshot());

            int nextTimeout = MetadataSyncRetryInterval;
            bool syncMetadata = false;

            if (!LockCitusExtension()) {
                ereport(DEBUG1, (errmsg("could not lock the citus extension, "
                                        "skipping metadata sync")));
            } else if (CheckCitusVersion(DEBUG1) && CitusHasBeenLoaded()) {
                bool lockFailure = false;
                syncMetadata = ShouldInitiateMetadataSync(&lockFailure);

                /*
                 * If lock fails, we need to recheck in a short while. If we are
                 * going to sync metadata, we should recheck in a short while to
                 * see if it failed. Otherwise, we can wait longer.
                 */
                nextTimeout = (lockFailure || syncMetadata)
                                  ? MetadataSyncRetryInterval
                                  : Session_ctx::Vars().MetadataSyncInterval;
            }

            PopActiveSnapshot();
            CommitTransactionCommand();

            if (syncMetadata) {
                metadataSyncBgwHandle =
                    SpawnSyncNodeMetadataToNodes(MyDatabaseId, myDbData->userOid);
            }

            nextMetadataSyncTime =
                TimestampTzPlusMilliseconds(GetCurrentTimestamp(), nextTimeout);
            timeout = Min(timeout, nextTimeout);
        }
#endif
        /*
         * If enabled, run 2PC recovery on primary nodes (where !RecoveryInProgress()),
         * since we'll write to the pg_dist_transaction log.
         */
        if (Session_ctx::Vars().Recover2PCInterval > 0 && !RecoveryInProgress() &&
            TimestampDifferenceExceeds(lastRecoveryTime, GetCurrentTimestamp(),
                                       Session_ctx::Vars().Recover2PCInterval)) {
            int recoveredTransactionCount = 0;

            InvalidateMetadataSystemCache();
            StartTransactionCommand();

            if (!LockCitusExtension()) {
                ereport(DEBUG1, (errmsg("could not lock the citus extension, "
                                        "skipping 2PC recovery")));
            } else if (CheckCitusVersion(DEBUG1) && CitusHasBeenLoaded()) {
                /*
                 * Record last recovery time at start to ensure we run once per
                 * Recover2PCInterval even if RecoverTwoPhaseCommits takes some time.
                 */
                lastRecoveryTime = GetCurrentTimestamp();

                recoveredTransactionCount = RecoverTwoPhaseCommits();
            }

            CommitTransactionCommand();

            if (recoveredTransactionCount > 0) {
                ereport(LOG, (errmsg("maintenance daemon recovered %d distributed "
                                     "transactions",
                                     recoveredTransactionCount)));
            }

            /* make sure we don't wait too long */
            timeout = Min(timeout, Session_ctx::Vars().Recover2PCInterval);
        }

        /* the config value -1 disables the distributed deadlock detection  */
        if (Session_ctx::Vars().DistributedDeadlockDetectionTimeoutFactor != -1.0) {
            double deadlockTimeout =
                Session_ctx::Vars().DistributedDeadlockDetectionTimeoutFactor *
                (double)u_sess->attr.attr_storage.DeadlockTimeout;

            InvalidateMetadataSystemCache();
            StartTransactionCommand();

            /*
             * We skip the deadlock detection if citus extension
             * is not accessible.
             *
             * Similarly, we skip to run the deadlock checks if
             * there exists any version mismatch or the extension
             * is not fully created yet.
             */
            if (!LockCitusExtension()) {
                ereport(DEBUG1, (errmsg("could not lock the citus extension, "
                                        "skipping deadlock detection")));
            } else if (CheckCitusVersion(DEBUG1) && CitusHasBeenLoaded()) {
                foundDeadlock = CheckForDistributedDeadlocks();
            }

            CommitTransactionCommand();

            /*
             * If we find any deadlocks, run the distributed deadlock detection
             * more often since it is quite possible that there are other
             * deadlocks need to be resolved.
             *
             * Thus, we use 1/20 of the calculated value. With the default
             * values (i.e., deadlock_timeout 1 seconds,
             * spq.distributed_deadlock_detection_factor 2), we'd be able to cancel
             * ~10 distributed deadlocks per second.
             */
            if (foundDeadlock) {
                deadlockTimeout = deadlockTimeout / 20.0;
            }

            /* make sure we don't wait too long */
            timeout = Min(timeout, deadlockTimeout);
        }

        if (!RecoveryInProgress() && Session_ctx::Vars().DeferShardDeleteInterval > 0 &&
            TimestampDifferenceExceeds(lastShardCleanTime, GetCurrentTimestamp(),
                                       Session_ctx::Vars().DeferShardDeleteInterval)) {
            int numberOfDroppedResources = 0;

            InvalidateMetadataSystemCache();
            StartTransactionCommand();

            if (!LockCitusExtension()) {
                ereport(
                    DEBUG1,
                    (errmsg(
                        "could not lock the citus extension, skipping shard cleaning")));
            } else if (CheckCitusVersion(DEBUG1) && CitusHasBeenLoaded()) {
                /*
                 * Record last shard clean time at start to ensure we run once per
                 * DeferShardDeleteInterval.
                 */
                lastShardCleanTime = GetCurrentTimestamp();

                numberOfDroppedResources = TryDropOrphanedResources();
            }

            CommitTransactionCommand();

            if (numberOfDroppedResources > 0) {
                ereport(LOG, (errmsg("maintenance daemon dropped %d "
                                     "resources previously marked to be removed",
                                     numberOfDroppedResources)));
            }

            /* make sure we don't wait too long */
            timeout = Min(timeout, Session_ctx::Vars().DeferShardDeleteInterval);
        }
        if (Session_ctx::Vars().StatStatementsPurgeInterval > 0 &&
            Session_ctx::Vars().StatStatementsTrack != STAT_STATEMENTS_TRACK_NONE &&
            TimestampDifferenceExceeds(
                lastStatStatementsPurgeTime, GetCurrentTimestamp(),
                (Session_ctx::Vars().StatStatementsPurgeInterval * 1000))) {
            StartTransactionCommand();

            if (!LockCitusExtension()) {
                ereport(DEBUG1, (errmsg("could not lock the citus extension, "
                                        "skipping stat statements purging")));
            } else if (CheckCitusVersion(DEBUG1) && CitusHasBeenLoaded()) {
                /*
                 * Record last time we perform the purge to ensure we run once per
                 * StatStatementsPurgeInterval.
                 */
                lastStatStatementsPurgeTime = GetCurrentTimestamp();

                SpqQueryStatsSynchronizeEntries();
            }

            CommitTransactionCommand();

            /* make sure we don't wait too long, need to convert seconds to milliseconds
             */
            timeout =
                Min(timeout, (Session_ctx::Vars().StatStatementsPurgeInterval * 1000));
        }

        pid_t backgroundTaskQueueWorkerPid = 0;
        BgwHandleStatus backgroundTaskQueueWorkerStatus =
            backgroundTasksQueueBgwHandle != nullptr
                ? slist_container(BackgroundWorker, rw_lnode,
                                  backgroundTasksQueueBgwHandle)
                      ->bgw_status
                : BGW_TERMINATED;
        if (!RecoveryInProgress() &&
            Session_ctx::Vars().BackgroundTaskQueueCheckInterval > 0 &&
            TimestampDifferenceExceeds(
                lastBackgroundTaskQueueCheck, GetCurrentTimestamp(),
                Session_ctx::Vars().BackgroundTaskQueueCheckInterval) &&
            (backgroundTaskQueueWorkerStatus == BGW_TERMINATED ||
             backgroundTaskQueueWorkerStatus == BGW_FAILED)) {
            /* clear old background worker for task queue before checking for new tasks */
            if (backgroundTasksQueueBgwHandle != nullptr) {
                backgroundTasksQueueBgwHandle = nullptr;
                /* Quit and return all bgworkers launched by this thread. */
                BgworkerListSyncQuit();
            }

            StartTransactionCommand();

            bool shouldStartBackgroundTaskQueueBackgroundWorker = false;
            if (!LockCitusExtension()) {
                ereport(DEBUG1, (errmsg("could not lock the citus extension, "
                                        "skipping stat statements purging")));
            } else if (CheckCitusVersion(DEBUG1) && CitusHasBeenLoaded()) {
                /* perform catalog precheck */
                shouldStartBackgroundTaskQueueBackgroundWorker =
                    HasRunnableBackgroundTask();
            }

            CommitTransactionCommand();

            if (shouldStartBackgroundTaskQueueBackgroundWorker) {
                /*
                 * Before we start the background worker we want to check if an orphaned
                 * one is still running. This could happen when the maintenance daemon
                 * restarted in a way where the background task queue monitor wasn't
                 * restarted.
                 *
                 * To check if an orphaned background task queue monitor is still running
                 * we quickly acquire the lock without waiting. If we can't acquire the
                 * lock this means that some other backed still has the lock. We prevent a
                 * new backend from starting and log a warning that we found that another
                 * process still holds the lock.
                 */
                LOCKTAG tag = {0};
                SET_LOCKTAG_CITUS_OPERATION(tag, CITUS_BACKGROUND_TASK_MONITOR);
                const bool sessionLock = false;
                const bool dontWait = true;
                LockAcquireResult locked =
                    LockAcquire(&tag, AccessExclusiveLock, sessionLock, dontWait);

                if (locked == LOCKACQUIRE_NOT_AVAIL) {
                    if (!backgroundTasksQueueWarnedForLock) {
                        ereport(WARNING, (errmsg("background task queue monitor already "
                                                 "held"),
                                          errdetail("the background task queue monitor "
                                                    "lock is held by another backend, "
                                                    "indicating the maintenance daemon "
                                                    "has lost track of an already "
                                                    "running background task queue "
                                                    "monitor, not starting a new one")));
                        backgroundTasksQueueWarnedForLock = true;
                    }
                } else {
                    LockRelease(&tag, AccessExclusiveLock, sessionLock);

                    /* we were able to acquire the lock, reset the warning tracker */
                    backgroundTasksQueueWarnedForLock = false;

                    /* spawn background worker */
                    ereport(LOG, (errmsg("found scheduled background tasks, starting new "
                                         "background task queue monitor")));

                    Oid* sharedOid = static_cast<Oid*>(palloc(sizeof(MyDatabaseId)));
                    *sharedOid = MyDatabaseId;

                    if (LaunchBackgroundWorkers(1, static_cast<void*>(sharedOid),
                                                CitusBackgroundTaskQueueMonitorMain, NULL,
                                                BGWORKER_FLAG_INDIVIDUAL_THREAD) != 1) {
                        backgroundTasksQueueBgwHandle = nullptr;
                    } else {
                        backgroundTasksQueueBgwHandle =
                            (BackgroundWorkerHandle*)slist_head_node(
                                &t_thrd.bgworker_cxt.bgwlist);
                    }

                    if (backgroundTasksQueueBgwHandle == nullptr ||
                        slist_container(BackgroundWorker, rw_lnode,
                                        backgroundTasksQueueBgwHandle)
                                ->bgw_status == BGW_STOPPED) {
                        ereport(WARNING, (errmsg("unable to start background worker for "
                                                 "background task execution")));
                    }
                }
            }

            /* interval management */
            lastBackgroundTaskQueueCheck = GetCurrentTimestamp();
            timeout = Min(timeout, Session_ctx::Vars().BackgroundTaskQueueCheckInterval);
        }

        /*
         * Wait until timeout, or until somebody wakes us up. Also cast the timeout to
         * integer where we've calculated it using double for not losing the precision.
         */
        int rc = WaitLatch(&(t_thrd.proc->procLatch), latchFlags, (long)timeout);
        /* emergency bailout if postmaster has died */
        if (rc & WL_POSTMASTER_DEATH) {
            proc_exit(1);
        }

        if (rc & WL_LATCH_SET) {
            ResetLatch(&(t_thrd.proc->procLatch));
            CHECK_FOR_INTERRUPTS();

            /* check for changed configuration */
            if (myDbData->userOid != GetSessionUserId()) {
                /* return code of 1 requests worker restart */
                proc_exit(1);
            }

            /*
             * Could also add code checking whether extension still exists,
             * but that'd complicate things a bit, because we'd have to delete
             * the shared memory entry.  There'd potentially be a race
             * condition where the extension gets re-created, checking that
             * this entry still exists, and it getting deleted just after.
             * Doesn't seem worth catering for that.
             */
        }

        if (Session_ctx::DaemonCtx().got_SIGHUP) {
            Session_ctx::DaemonCtx().got_SIGHUP = false;
            ProcessConfigFile(PGC_SIGHUP);
        }
    }
#ifdef DISABLE_OG_COMMENTS
    if (metadataSyncBgwHandle) {
        TerminateBackgroundWorker(metadataSyncBgwHandle);
    }
#endif
}

/*
 * MaintenanceDaemonShmemInit initializes the requested shared memory for the
 * maintenance daemon.
 */
void MaintenanceDaemonShmemInit(void)
{
    HASHCTL hashInfo;

    AutoMutexLock maintenanceDaemonControlLock(&g_maintenanceDaemonControlLock);
    maintenanceDaemonControlLock.lock();
    if (MaintenanceDaemonDBHash == nullptr) {
        memset(&hashInfo, 0, sizeof(hashInfo));
        hashInfo.keysize = sizeof(Oid);
        hashInfo.entrysize = sizeof(MaintenanceDaemonDBData);
        hashInfo.hash = tag_hash;
        hashInfo.hcxt = INSTANCE_GET_MEM_CXT_GROUP(MEMORY_CONTEXT_STORAGE);
        hashInfo.dsize = hashInfo.max_dsize = hash_select_dirsize(g_max_worker_processes);
        int hashFlags = (HASH_ELEM | HASH_FUNCTION | HASH_CONTEXT | HASH_DIRSIZE);

        MaintenanceDaemonDBHash = hash_create(
            "Maintenance Database Hash", g_max_worker_processes, &hashInfo, hashFlags);
    }
    maintenanceDaemonControlLock.unLock();
}

/*
 * MaintenaceDaemonShmemExit is the before_shmem_exit handler for cleaning up
 * MaintenanceDaemonDBHash
 */
static void MaintenanceDaemonShmemExit(int code, Datum arg)
{
    Oid databaseOid = DatumGetObjectId(arg);

    AutoMutexLock maintenanceDaemonControlLock(&g_maintenanceDaemonControlLock);
    maintenanceDaemonControlLock.lock();

    MaintenanceDaemonDBData* myDbData = (MaintenanceDaemonDBData*)hash_search(
        MaintenanceDaemonDBHash, &databaseOid, HASH_FIND, NULL);

    /* myDbData is NULL after StopMaintenanceDaemon */
    if (myDbData != NULL) {
        /*
         * Confirm that I am still the registered maintenance daemon before exiting.
         */
        Assert(myDbData->workerPid == t_thrd.proc_cxt.MyProcPid);

        myDbData->daemonStarted = false;
        myDbData->workerPid = 0;
    }

    maintenanceDaemonControlLock.unLock();
}

/* MaintenanceDaemonSigTermHandler calls proc_exit(0) */
static void MaintenanceDaemonSigTermHandler(SIGNAL_ARGS)
{
    int save_errno = errno;

    Session_ctx::DaemonCtx().got_SIGTERM = true;
    if (t_thrd.proc != NULL) {
        SetLatch(&t_thrd.proc->procLatch);
    }
    errno = save_errno;
}

/*
 * MaintenanceDaemonSigHupHandler set a flag to re-read config file at next
 * convenient time.
 */
static void MaintenanceDaemonSigHupHandler(SIGNAL_ARGS)
{
    int save_errno = errno;

    Session_ctx::DaemonCtx().got_SIGHUP = true;
    if (t_thrd.proc != NULL) {
        SetLatch(&t_thrd.proc->procLatch);
    }
    errno = save_errno;
}

/*
 * MaintenanceDaemonErrorContext adds some context to log messages to make it
 * easier to associate them with the maintenance daemon.
 */
static void MaintenanceDaemonErrorContext(void* arg)
{
    MaintenanceDaemonDBData* myDbData = (MaintenanceDaemonDBData*)arg;
    errcontext("Spq maintenance daemon for database %u user %u", myDbData->databaseOid,
               myDbData->userOid);
}

/*
 * LockCitusExtension acquires a lock on the Citus extension or returns
 * false if the extension does not exist or is being dropped.
 */
bool LockCitusExtension(void)
{
    Oid extensionOid = get_extension_oid(SPQ_EXTENSION_NAME, true);
    if (extensionOid == InvalidOid) {
        /* citus extension does not exist */
        return false;
    }

    LockDatabaseObject(ExtensionRelationId, extensionOid, 0, AccessShareLock);

    /*
     * The extension may have been dropped and possibly recreated prior to
     * obtaining a lock. Check whether we still get the expected OID.
     */
    Oid recheckExtensionOid = get_extension_oid(SPQ_EXTENSION_NAME, true);
    if (recheckExtensionOid != extensionOid) {
        return false;
    }

    return true;
}

/*
 * StopMaintenanceDaemon stops the maintenance daemon for the
 * given database and removes it from the maintenance daemon
 * control hash.
 */
void StopMaintenanceDaemon(Oid databaseId)
{
    bool found = false;
    ThreadId workerPid = 0;

    AutoMutexLock maintenanceDaemonControlLock(&g_maintenanceDaemonControlLock);
    maintenanceDaemonControlLock.lock();

    MaintenanceDaemonDBData* dbData = (MaintenanceDaemonDBData*)hash_search(
        MaintenanceDaemonDBHash, &databaseId, HASH_REMOVE, &found);

    if (found) {
        workerPid = dbData->workerPid;
    }

    maintenanceDaemonControlLock.unLock();

    if (workerPid > 0) {
        (void)gs_signal_send(workerPid, SIGTERM);
    }
}

/*
 * TriggerMetadataSync triggers the maintenance daemon to do
 * a node metadata sync for the given database.
 */
void TriggerNodeMetadataSync(Oid databaseId)
{
    bool found = false;

    AutoMutexLock maintenanceDaemonControlLock(&g_maintenanceDaemonControlLock);
    maintenanceDaemonControlLock.lock();

    MaintenanceDaemonDBData* dbData = (MaintenanceDaemonDBData*)hash_search(
        MaintenanceDaemonDBHash, &databaseId, HASH_FIND, &found);
    if (found) {
        dbData->triggerNodeMetadataSync = true;

        /* set latch to wake-up the maintenance loop */
        SetLatch(dbData->latch);
    }

    maintenanceDaemonControlLock.unLock();
}

/*
 * MetadataSyncTriggeredCheckAndReset checks if metadata sync has been
 * triggered for the given database, and resets the flag.
 */
static bool MetadataSyncTriggeredCheckAndReset(MaintenanceDaemonDBData* dbData)
{
    AutoMutexLock maintenanceDaemonControlLock(&g_maintenanceDaemonControlLock);
    maintenanceDaemonControlLock.lock();

    bool metadataSyncTriggered = dbData->triggerNodeMetadataSync;
    dbData->triggerNodeMetadataSync = false;

    maintenanceDaemonControlLock.unLock();
    return metadataSyncTriggered;
}
