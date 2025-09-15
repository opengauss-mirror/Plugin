/*-------------------------------------------------------------------------
 *
 * backend_data.c
 *
 *  Infrastructure for managing per backend data that can efficiently
 *  accessed by all sessions.
 *
 * Copyright (c) Citus Data, Inc.
 *
 *-------------------------------------------------------------------------
 */

#include "postgres.h"

#include "funcapi.h"
#include "miscadmin.h"
#include "safe_lib.h"
#include "unistd.h"

#include "access/htup.h"
#include "postmaster/bgworker.h"
#include "catalog/pg_authid.h"
#include "catalog/pg_type.h"
#include "datatype/timestamp.h"
#include "nodes/execnodes.h"
#include "postmaster/autovacuum.h" /* to access autovacuum_max_workers */
#include "replication/walsender.h"
#include "storage/ipc.h"
#include "storage/lmgr.h"
#include "storage/lock/lwlock.h"
#include "storage/proc.h"
#include "storage/procarray.h"
#include "storage/lock/s_lock.h"
#include "storage/spin.h"
#include "utils/timestamp.h"

#include "pg_version_compat.h"

#include "distributed/backend_data.h"
#include "distributed/connection_management.h"
#include "distributed/listutils.h"
#include "distributed/lock_graph.h"
#include "distributed/metadata_cache.h"
#include "distributed/memutils.h"
#include "distributed/session_ctx.h"
#include "distributed/remote_commands.h"
#include "distributed/shared_connection_stats.h"
#include "distributed/transaction_identifier.h"
#include "distributed/tuplestore.h"
#include "distributed/worker_manager.h"
#include "distributed/utils/spq_lwlock.h"

#define GET_ACTIVE_TRANSACTION_QUERY "SELECT * FROM get_all_active_transactions();"
#define ACTIVE_TRANSACTION_COLUMN_COUNT 7
#define GLOBAL_PID_NODE_ID_MULTIPLIER 10000000000

/*
 * Each backend's data reside in the shared memory
 * on the BackendManagementShmemData.
 */
typedef struct BackendManagementShmemData {
    LWLock lock;

    /*
     * We prefer to use an atomic integer over sequences for two
     * reasons (i) orders of magnitude performance difference
     * (ii) allowing read-only replicas to be able to generate ids
     */
    pg_atomic_uint64 nextTransactionNumber;

    /*
     * Total number of external client backends that are authenticated.
     *
     * Note that the counter does not consider any background workers
     * or such, and also exludes internal connections between nodes.
     */
    pg_atomic_uint32 externalClientBackendCounter;

    BackendData backends[FLEXIBLE_ARRAY_MEMBER];
} BackendManagementShmemData;

static const char* SpqBackendPrefixes[] = {
    SPQ_APPLICATION_NAME_PREFIX,
    SPQ_REBALANCER_APPLICATION_NAME_PREFIX,
    SPQ_RUN_COMMAND_APPLICATION_NAME_PREFIX,
};

static const CitusBackendType CitusBackendTypes[] = {
    CITUS_INTERNAL_BACKEND,
    CITUS_REBALANCER_BACKEND,
    CITUS_RUN_COMMAND_BACKEND,
};

static void StoreAllActiveTransactions(Tuplestorestate* tupleStore,
                                       TupleDesc tupleDescriptor);
static bool UserHasPermissionToViewStatsOf(Oid currentUserId, Oid backendOwnedId);
static uint64 CalculateGlobalPID(int32 nodeId, pid_t tid);
static uint64 GenerateGlobalPID(void);

static THR_LOCAL shmem_startup_hook_type prev_shmem_startup_hook = NULL;

/** the follows varaibables is come from share memory. */
static BackendManagementShmemData* backendManagementShmemData = NULL;

/** Backenddata will be initialized by InitSession->CheckAuthentication, and then kept in
 Session_ctx::m_backendDataCtx(InitSession->InitExtensionVariable). The root cause can be
 seen in CitusAuthHook. */
static THR_LOCAL BackendData* initMyBackendData = nullptr;
THR_LOCAL CitusBackendType initBackendType = CITUS_BACKEND_NOT_ASSIGNED;

PG_FUNCTION_INFO_V1(assign_distributed_transaction_id);
PG_FUNCTION_INFO_V1(get_current_transaction_id);
PG_FUNCTION_INFO_V1(get_global_active_transactions);
PG_FUNCTION_INFO_V1(get_all_active_transactions);
PG_FUNCTION_INFO_V1(spq_calculate_gpid);
PG_FUNCTION_INFO_V1(spq_backend_gpid);
PG_FUNCTION_INFO_V1(spq_nodeid_for_gpid);
PG_FUNCTION_INFO_V1(spq_pid_for_gpid);

extern "C" Datum assign_distributed_transaction_id(PG_FUNCTION_ARGS);
extern "C" Datum get_current_transaction_id(PG_FUNCTION_ARGS);
extern "C" Datum get_all_active_transactions(PG_FUNCTION_ARGS);
extern "C" Datum get_global_active_transactions(PG_FUNCTION_ARGS);
extern "C" Datum spq_calculate_gpid(PG_FUNCTION_ARGS);
extern "C" Datum spq_backend_gpid(PG_FUNCTION_ARGS);
extern "C" Datum spq_nodeid_for_gpid(PG_FUNCTION_ARGS);
extern "C" Datum spq_pid_for_gpid(PG_FUNCTION_ARGS);

void SessionBackendDataCtx::InitializeBackendDataCtx()
{
    this->MyBackendData = initMyBackendData;
    this->CurrentBackendType = initBackendType;
    /* reset temporary auth variables for initialize for next session. */
    initBackendType = CITUS_BACKEND_NOT_ASSIGNED;
    initMyBackendData = nullptr;
}

/*
 * assign_distributed_transaction_id updates the shared memory allocated for this backend
 * and sets initiatorNodeIdentifier, transactionNumber, timestamp fields with the given
 * inputs. Also, the function sets the database id and process id via the information that
 * Postgres provides.
 *
 * This function is only intended for internal use for managing distributed transactions.
 * Users should not use this function for any purpose.
 */
Datum assign_distributed_transaction_id(PG_FUNCTION_ARGS)
{
    CheckCitusVersion(ERROR);

    /* prepare data before acquiring spinlock to protect against errors */
    int32 initiatorNodeIdentifier = PG_GETARG_INT32(0);
    uint64 transactionNumber = PG_GETARG_INT64(1);
    TimestampTz timestamp = PG_GETARG_TIMESTAMPTZ(2);

    /* MyBackendData should always be avaliable, just out of paranoia */
    if (!Session_ctx::BackendCtx().MyBackendData) {
        ereport(ERROR, (errmsg("backend is not ready for distributed transactions")));
    }

    /*
     * Note that we don't need to lock shared memory (i.e., LockBackendSharedMemory())
     * here since this function is executed after AssignDistributedTransactionId() issued
     * on the initiator node, which already takes the required lock to enforce the
     * consistency.
     */

    SpinLockAcquire(&Session_ctx::BackendCtx().MyBackendData->mutex);

    /* if an id is already assigned, release the lock and error */
    if (Session_ctx::BackendCtx().MyBackendData->transactionId.transactionNumber != 0) {
        SpinLockRelease(&Session_ctx::BackendCtx().MyBackendData->mutex);

        ereport(ERROR, (errmsg("the backend has already been assigned a "
                               "transaction id")));
    }

    Session_ctx::BackendCtx().MyBackendData->transactionId.initiatorNodeIdentifier =
        initiatorNodeIdentifier;
    Session_ctx::BackendCtx().MyBackendData->transactionId.transactionNumber =
        transactionNumber;
    Session_ctx::BackendCtx().MyBackendData->transactionId.timestamp = timestamp;
    Session_ctx::BackendCtx().MyBackendData->transactionId.transactionOriginator = false;

    SpinLockRelease(&Session_ctx::BackendCtx().MyBackendData->mutex);

    PG_RETURN_VOID();
}

/*
 * get_current_transaction_id returns a tuple with (databaseId, processId,
 * initiatorNodeIdentifier, transactionNumber, timestamp) that exists in the
 * shared memory associated with this backend. Note that if the backend
 * is not in a transaction, the function returns uninitialized data where
 * transactionNumber equals to 0.
 */
Datum get_current_transaction_id(PG_FUNCTION_ARGS)
{
    CheckCitusVersion(ERROR);

    TupleDesc tupleDescriptor = NULL;

    Datum values[5];
    bool isNulls[5];

    /* build a tuple descriptor for our result type */
    if (get_call_result_type(fcinfo, NULL, &tupleDescriptor) != TYPEFUNC_COMPOSITE) {
        elog(ERROR, "return type must be a row type");
    }

    /* MyBackendData should always be avaliable, just out of paranoia */
    if (!Session_ctx::BackendCtx().MyBackendData) {
        ereport(ERROR, (errmsg("backend is not ready for distributed transactions")));
    }

    DistributedTransactionId* distributedTransctionId =
        GetCurrentDistributedTransactionId();

    memset(values, 0, sizeof(values));
    memset(isNulls, false, sizeof(isNulls));

    /* first two fields do not change for this backend, so get directly */
    values[0] = ObjectIdGetDatum(u_sess->proc_cxt.MyDatabaseId);
    values[1] = Int32GetDatum(t_thrd.proc_cxt.MyProcPid);

    values[2] = Int32GetDatum(distributedTransctionId->initiatorNodeIdentifier);
    values[3] = UInt64GetDatum(distributedTransctionId->transactionNumber);

    /* provide a better output */
    if (distributedTransctionId->transactionNumber != 0) {
        values[4] = TimestampTzGetDatum(distributedTransctionId->timestamp);
    } else {
        isNulls[4] = true;
    }

    HeapTuple heapTuple = heap_form_tuple(tupleDescriptor, values, isNulls);

    PG_RETURN_DATUM(HeapTupleGetDatum(heapTuple));
}

/*
 * get_global_active_transactions returns all the available information about all
 * the active backends from each node of the cluster. If you call that function from
 * the coordinator, it will returns back active transaction from the coordinator as
 * well. Yet, if you call it from the worker, result won't include the transactions
 * on the coordinator node, since worker nodes are not aware of the coordinator.
 */
Datum get_global_active_transactions(PG_FUNCTION_ARGS)
{
    CheckCitusVersion(ERROR);

    TupleDesc tupleDescriptor = NULL;
    List* workerNodeList = ActivePrimaryNonCoordinatorNodeList(NoLock);
    List* connectionList = NIL;
    StringInfo queryToSend = makeStringInfo();

    Tuplestorestate* tupleStore = SetupTuplestore(fcinfo, &tupleDescriptor);

    appendStringInfo(queryToSend, GET_ACTIVE_TRANSACTION_QUERY);

    /* add active transactions for local node */
    StoreAllActiveTransactions(tupleStore, tupleDescriptor);

    int32 localGroupId = GetLocalGroupId();

    /* open connections in parallel */
    WorkerNode* workerNode = NULL;
    foreach_declared_ptr(workerNode, workerNodeList)
    {
        const char* nodeName = workerNode->workerName;
        int nodePort = workerNode->workerPort;
        int connectionFlags = 0;

        if (workerNode->groupId == localGroupId) {
            /* we already get these transactions via GetAllActiveTransactions() */
            continue;
        }

        MultiConnection* connection =
            StartNodeConnection(connectionFlags, nodeName, nodePort);

        connectionList = lappend(connectionList, connection);
    }

    FinishConnectionListEstablishment(connectionList);

    /* send commands in parallel */
    MultiConnection* connection = NULL;
    foreach_declared_ptr(connection, connectionList)
    {
        int querySent = SendRemoteCommand(connection, queryToSend->data);
        if (querySent == 0) {
            ReportConnectionError(connection, WARNING);
        }
    }

    /* receive query results */
    foreach_declared_ptr(connection, connectionList)
    {
        bool raiseInterrupts = true;
        Datum values[ACTIVE_TRANSACTION_COLUMN_COUNT];
        bool isNulls[ACTIVE_TRANSACTION_COLUMN_COUNT];

        if (PQstatus(connection->pgConn) != CONNECTION_OK) {
            continue;
        }

        PGresult* result = GetRemoteCommandResult(connection, raiseInterrupts);
        if (!IsResponseOK(result)) {
            ReportResultError(connection, result, WARNING);
            continue;
        }

        int64 rowCount = PQntuples(result);
        int64 colCount = PQnfields(result);

        /* Although it is not expected */
        if (colCount != ACTIVE_TRANSACTION_COLUMN_COUNT) {
            ereport(WARNING, (errmsg("unexpected number of columns from "
                                     "get_all_active_transactions")));
            continue;
        }

        for (int64 rowIndex = 0; rowIndex < rowCount; rowIndex++) {
            memset(values, 0, sizeof(values));
            memset(isNulls, false, sizeof(isNulls));

            values[0] = ParseIntField(result, rowIndex, 0);
            values[1] = ParseIntField(result, rowIndex, 1);
            values[2] = ParseIntField(result, rowIndex, 2);
            values[3] = ParseBoolField(result, rowIndex, 3);
            values[4] = ParseIntField(result, rowIndex, 4);
            values[5] = ParseTimestampTzField(result, rowIndex, 5);
            values[6] = ParseIntField(result, rowIndex, 6);

            tuplestore_putvalues(tupleStore, tupleDescriptor, values, isNulls);
        }

        PQclear(result);
        ForgetResults(connection);
    }

    PG_RETURN_VOID();
}

/*
 * get_all_active_transactions returns all the avaliable information about all
 * the active backends.
 */
Datum get_all_active_transactions(PG_FUNCTION_ARGS)
{
    CheckCitusVersion(ERROR);

    TupleDesc tupleDescriptor = NULL;
    Tuplestorestate* tupleStore = SetupTuplestore(fcinfo, &tupleDescriptor);

    StoreAllActiveTransactions(tupleStore, tupleDescriptor);

    PG_RETURN_VOID();
}

/*
 * StoreAllActiveTransactions gets active transaction from the local node and inserts
 * them into the given tuplestore.
 */
static void StoreAllActiveTransactions(Tuplestorestate* tupleStore,
                                       TupleDesc tupleDescriptor)
{
    Datum values[ACTIVE_TRANSACTION_COLUMN_COUNT];
    bool isNulls[ACTIVE_TRANSACTION_COLUMN_COUNT];
    const Oid userId = GetUserId();
    bool showAllBackends = superuser() || isMonitoradmin(userId);

    /* we're reading all distributed transactions, prevent new backends */
    LockBackendSharedMemory(LW_SHARED);

    for (int backendIndex = 0; backendIndex < TotalProcCount(); ++backendIndex) {
        bool showCurrentBackendDetails = showAllBackends;
        BackendData* currentBackend = &backendManagementShmemData->backends[backendIndex];
        PGPROC* currentProc = g_instance.proc_base_all_procs[backendIndex];

        /* to work on data after releasing g spinlock to protect against errors */
        uint64 transactionNumber = 0;

        SpinLockAcquire(&currentBackend->mutex);

        if (currentProc->pid == 0 || !currentBackend->activeBackend) {
            /* unused PGPROC slot or the backend already exited */
            SpinLockRelease(&currentBackend->mutex);
            continue;
        }

        /*
         * Unless the user has a role that allows seeing all transactions (superuser,
         * pg_monitor), we only follow pg_stat_statements owner checks.
         */
        if (!showCurrentBackendDetails &&
            UserHasPermissionToViewStatsOf(userId, currentProc->roleId)) {
            showCurrentBackendDetails = true;
        }

        Oid databaseId = currentBackend->databaseId;
        ThreadId backendPid = g_instance.proc_base_all_procs[backendIndex]->pid;

        /*
         * We prefer to use worker_query instead of distributedCommandOriginator in
         * the user facing functions since it's more intuitive. Thus,
         * we negate the result before returning.
         */
        bool distributedCommandOriginator = currentBackend->distributedCommandOriginator;

        transactionNumber = currentBackend->transactionId.transactionNumber;
        TimestampTz transactionIdTimestamp = currentBackend->transactionId.timestamp;

        SpinLockRelease(&currentBackend->mutex);

        memset(values, 0, sizeof(values));
        memset(isNulls, false, sizeof(isNulls));

        /*
         * We imitate pg_stat_activity such that if a user doesn't have enough
         * privileges, we only show the minimal information including the pid,
         * global pid and distributedCommandOriginator.
         *
         * pid is already can be found in pg_stat_activity for any process, and
         * the rest doesn't reveal anything critial for under priviledge users
         * but still could be useful for monitoring purposes of Citus.
         */
        if (showCurrentBackendDetails) {
            bool missingOk = true;
            int initiatorNodeId =
                ExtractNodeIdFromGlobalPID(currentBackend->globalPID, missingOk);

            values[0] = ObjectIdGetDatum(databaseId);
            values[1] = Int64GetDatum(backendPid);
            values[2] = Int32GetDatum(initiatorNodeId);
            values[3] = !distributedCommandOriginator;
            values[4] = UInt64GetDatum(transactionNumber);
            values[5] = TimestampTzGetDatum(transactionIdTimestamp);
            values[6] = UInt64GetDatum(currentBackend->globalPID);
        } else {
            isNulls[0] = true;
            values[1] = Int64GetDatum(backendPid);
            isNulls[2] = true;
            values[3] = !distributedCommandOriginator;
            isNulls[4] = true;
            isNulls[5] = true;
            values[6] = UInt64GetDatum(currentBackend->globalPID);
        }

        tuplestore_putvalues(tupleStore, tupleDescriptor, values, isNulls);

        /*
         * We don't want to initialize memory while spinlock is held so we
         * prefer to do it here. This initialization is done for the rows
         * starting from the second one.
         */
        memset(values, 0, sizeof(values));
        memset(isNulls, false, sizeof(isNulls));
    }

    UnlockBackendSharedMemory();
}

/*
 * UserHasPermissionToViewStatsOf returns true if currentUserId can
 * see backends of backendOwnedId.
 *
 * We follow the same approach with pg_stat_activity.
 */
static bool UserHasPermissionToViewStatsOf(Oid currentUserId, Oid backendOwnedId)
{
    if (has_privs_of_role(currentUserId, backendOwnedId)) {
        return true;
    }
#ifdef DISABLE_OG_COMMENTS
    if (is_member_of_role(currentUserId, ROLE_PG_READ_ALL_STATS)) {
        return true;
    }
#endif

    return false;
}

/*
 * InitializeBackendManagement requests the necessary shared memory
 * from Postgres and sets up the shared memory startup hook.
 */
void InitializeBackendManagement(void)
{
    prev_shmem_startup_hook = t_thrd.storage_cxt.shmem_startup_hook;
    t_thrd.storage_cxt.shmem_startup_hook = BackendManagementShmemInit;
}

/*
 * BackendManagementShmemInit is the callback that is to be called on shared
 * memory startup hook. The function sets up the necessary shared memory
 * segment for the backend manager.
 */
void BackendManagementShmemInit(void)
{
    bool alreadyInitialized = false;

    /* we may update the shmem, acquire lock exclusively */
    LWLockAcquire(AddinShmemInitLock, LW_EXCLUSIVE);
    if (likely(backendManagementShmemData != NULL)) {
        LWLockRelease(AddinShmemInitLock);
        if (prev_shmem_startup_hook != NULL) {
            prev_shmem_startup_hook();
        }

        return;
    }

    char* unaligned_buf = (char*)ShmemInitStruct(
        "Backend Management Shmem",
        BackendManagementShmemSize() + spq::SHARE_MEM_ALIGN_SIZE, &alreadyInitialized);

    Assert(!alreadyInitialized);

    backendManagementShmemData =
        (BackendManagementShmemData*)TYPEALIGN(spq::SHARE_MEM_ALIGN_SIZE, unaligned_buf);

    /* start by zeroing out all the memory */
    memset(backendManagementShmemData, 0, BackendManagementShmemSize());
    Spq::InitTrancheLwlock(Spq::BACKEND_DATA_LWLOCK, &backendManagementShmemData->lock);
    /* start the distributed transaction ids from 1 */
    pg_atomic_init_u64(&backendManagementShmemData->nextTransactionNumber, 1);

    /* there are no active backends yet, so start with zero */
    pg_atomic_init_u32(&backendManagementShmemData->externalClientBackendCounter, 0);

    /*
     * We need to init per backend's spinlock before any backend
     * starts its execution. Note that we initialize TotalProcs (e.g., not
     * MaxBackends) since some of the blocking processes could be prepared
     * transactions, which aren't covered by MaxBackends.
     *
     * We also initiate initiatorNodeIdentifier to -1, which can never be
     * used as a node id.
     */
    for (int backendIndex = 0; backendIndex < TotalProcCount(); ++backendIndex) {
        BackendData* backendData = &backendManagementShmemData->backends[backendIndex];
        SpinLockInit(&backendData->mutex);
    }

    LWLockRelease(AddinShmemInitLock);

    if (prev_shmem_startup_hook != NULL) {
        prev_shmem_startup_hook();
    }
}

/*
 * BackendManagementShmemSize returns the size that should be allocated
 * on the shared memory for backend management.
 */
size_t BackendManagementShmemSize(void)
{
    Size size = 0;
    int totalProcs = TotalProcCount();

    size = add_size(size, sizeof(BackendManagementShmemData));
    size = add_size(size, mul_size(sizeof(BackendData), totalProcs));

    return size;
}

/*
 * TotalProcCount returns the total processes that could run via the current
 * postgres server. See the details in the function comments.
 *
 * There is one thing we should warn the readers. Citus enforces to be loaded
 * as the first extension in shared_preload_libraries. However, if any other
 * extension overrides MaxConnections, autovacuum_max_workers or
 * max_worker_processes, our reasoning in this function may not work as expected.
 * Given that it is not a usual pattern for extension, we consider Citus' behaviour
 * good enough for now.
 */
int TotalProcCount(void)
{
    return GLOBAL_ALL_PROCS;
}

/*
 * InitializeBackendData initialises MyBackendData to the shared memory segment
 * belonging to the current backend.
 *
 * The function is called through CitusHasBeenLoaded when we first detect that
 * the Citus extension is present, and after any subsequent invalidation of
 * pg_dist_partition (see InvalidateMetadataSystemCache()).
 *
 * We only need to initialise MyBackendData once. The main goal here is to make
 * sure that we don't use the backend data from a previous backend with the same
 * pgprocno. Resetting the backend data after a distributed transaction happens
 * on COMMIT/ABORT through transaction callbacks.
 *
 * We do also initialize the distributedCommandOriginator and globalPID values
 * based on these values. This is to make sure that once the backend date is
 * initialized this backend can be correctly shown in spq_lock_waits.
 */
void InitializeBackendData(const char* applicationName, bool authThread)
{
    BackendData** backendData =
        authThread ? &initMyBackendData : &(Session_ctx::BackendCtx().MyBackendData);
    bool distributedCommandOriginator = false;

    if (!authThread && *backendData != NULL) {
        /*
         * We already initialized MyBackendData before. We definitely should
         * not initialise it again, because we might be in the middle of a
         * distributed transaction.
         */
        return;
    }

    if (authThread) {
        distributedCommandOriginator = (initBackendType == EXTERNAL_CLIENT_BACKEND);
    } else {
        distributedCommandOriginator = IsExternalClientBackend();
    }

    uint64 gpid = ExtractGlobalPID(applicationName);
    *backendData = &backendManagementShmemData->backends[getProcNo_compat(t_thrd.proc)];
    Assert(*backendData);

    LockBackendSharedMemory(LW_EXCLUSIVE);

    /* zero out the backend its transaction id */
    UnSetDistributedTransactionId(*backendData);
    UnSetGlobalPID(*backendData);

    SpinLockAcquire(&((*backendData)->mutex));
    (*backendData)->distributedCommandOriginator = distributedCommandOriginator;
    (*backendData)->globalPID = gpid;
    (*backendData)->activeBackend = true;
    (*backendData)->tid = gettid();
    SpinLockRelease(&((*backendData)->mutex));

    UnlockBackendSharedMemory();
}

/*
 * UnSetDistributedTransactionId simply acquires the mutex and resets the backend's
 * distributed transaction data in shared memory to the initial values.
 */
void UnSetDistributedTransactionId(BackendData* backendData)
{
    /* backend does not exist if the extension is not created */
    if (backendData) {
        SpinLockAcquire(&backendData->mutex);

        backendData->cancelledDueToDeadlock = false;
        backendData->transactionId.initiatorNodeIdentifier = 0;
        backendData->transactionId.transactionOriginator = false;
        backendData->transactionId.transactionNumber = 0;
        backendData->transactionId.timestamp = 0;

        SpinLockRelease(&backendData->mutex);
    }
}

/*
 * UnSetGlobalPID resets the global pid for the current backend.
 */
void UnSetGlobalPID(BackendData* backendData)
{
    /* backend does not exist if the extension is not created */
    if (backendData) {
        SpinLockAcquire(&backendData->mutex);

        backendData->globalPID = 0;
        backendData->databaseId = 0;
        backendData->distributedCommandOriginator = false;

        SpinLockRelease(&backendData->mutex);
    }
}

/*
 * SetActiveMyBackend is a wrapper around MyBackendData->activeBackend.
 */
void SetActiveMyBackend(bool value)
{
    /* backend does not exist if the extension is not created */
    if (Session_ctx::BackendCtx().MyBackendData) {
        SpinLockAcquire(&Session_ctx::BackendCtx().MyBackendData->mutex);

        Session_ctx::BackendCtx().MyBackendData->activeBackend = value;

        SpinLockRelease(&Session_ctx::BackendCtx().MyBackendData->mutex);
    }
}

/*
 * LockBackendSharedMemory is a simple wrapper around LWLockAcquire on the
 * shared memory lock.
 *
 * We use the backend shared memory lock for preventing new backends to be part
 * of a new distributed transaction or an existing backend to leave a distributed
 * transaction while we're reading the all backends' data.
 *
 * The primary goal is to provide consistent view of the current distributed
 * transactions while doing the deadlock detection.
 */
void LockBackendSharedMemory(LWLockMode lockMode)
{
    LWLockAcquire(&backendManagementShmemData->lock, lockMode);
}

/*
 * UnlockBackendSharedMemory is a simple wrapper around LWLockRelease on the
 * shared memory lock.
 */
void UnlockBackendSharedMemory(void)
{
    LWLockRelease(&backendManagementShmemData->lock);
}

/*
 * GetCurrentDistributedTransactionId reads the backend's distributed transaction id and
 * returns a copy of it.
 *
 * When called from a parallel worker, it uses the parent's transaction ID per the logic
 * in GetBackendDataForProc.
 */
DistributedTransactionId* GetCurrentDistributedTransactionId(void)
{
    DistributedTransactionId* currentDistributedTransactionId =
        (DistributedTransactionId*)palloc(sizeof(DistributedTransactionId));
    BackendData backendData;

    GetBackendDataForProc(t_thrd.proc, &backendData);

    currentDistributedTransactionId->initiatorNodeIdentifier =
        backendData.transactionId.initiatorNodeIdentifier;
    currentDistributedTransactionId->transactionOriginator =
        backendData.transactionId.transactionOriginator;
    currentDistributedTransactionId->transactionNumber =
        backendData.transactionId.transactionNumber;
    currentDistributedTransactionId->timestamp = backendData.transactionId.timestamp;

    return currentDistributedTransactionId;
}

/*
 * AssignDistributedTransactionId generates a new distributed transaction id and
 * sets it for the current backend. It also sets the databaseId and
 * processId fields.
 *
 * This function should only be called on UseCoordinatedTransaction(). Any other
 * callers is very likely to break the distributed transaction management.
 */
void AssignDistributedTransactionId(void)
{
    /*
     * MyBackendData should always be available. However, we observed some
     * crashes where certain hooks were not executed.
     * Bug 3697586: Server crashes when assigning distributed transaction
     */
    if (!Session_ctx::BackendCtx().MyBackendData) {
        ereport(ERROR, (errmsg("backend is not ready for distributed transactions")));
    }

    pg_atomic_uint64* transactionNumberSequence =
        &backendManagementShmemData->nextTransactionNumber;

    uint64 nextTransactionNumber = pg_atomic_fetch_add_u64(transactionNumberSequence, 1);
    int32 localGroupId = GetLocalGroupId();
    TimestampTz currentTimestamp = GetCurrentTimestamp();

    SpinLockAcquire(&Session_ctx::BackendCtx().MyBackendData->mutex);

    Session_ctx::BackendCtx().MyBackendData->transactionId.initiatorNodeIdentifier =
        localGroupId;
    Session_ctx::BackendCtx().MyBackendData->transactionId.transactionOriginator = true;
    Session_ctx::BackendCtx().MyBackendData->transactionId.transactionNumber =
        nextTransactionNumber;
    Session_ctx::BackendCtx().MyBackendData->transactionId.timestamp = currentTimestamp;

    SpinLockRelease(&Session_ctx::BackendCtx().MyBackendData->mutex);
}

/*
 * AssignGlobalPID assigns a global process id for the current backend based on
 * the given applicationName. If this is a Citus initiated backend, which means
 * it is distributed part of a distributed query, then this function assigns
 * the global pid extracted from the application name. If not, this function
 * assigns a new generated global pid.
 *
 * There's one special case where we don't want to assign a new pid and keep
 * the old pid on purpose: The current backend is an external backend and the
 * node id of the current node changed since the previous call to
 * AssingGlobalPID. Updating the gpid to match the nodeid might seem like a
 * desirable property, but that's not the case. Mainly, because existing cached
 * connections will still report as the old gpid on the worker. So updating the
 * gpid with the new nodeid would mess up distributed deadlock and originator
 * detection of queries done using those old connections. So if this is an
 * external backend for which a gpid was already generated, then we don't
 * change the gpid.
 *
 * NOTE: This function can be called arbitrary amount of times for the same
 * backend, due to being called by StartupCitusBackend.
 */
void AssignGlobalPID(const char* applicationName)
{
    uint64 globalPID = INVALID_CITUS_INTERNAL_BACKEND_GPID;
    bool distributedCommandOriginator = IsExternalClientBackend();

    if (distributedCommandOriginator) {
        globalPID = GenerateGlobalPID();
    } else {
        globalPID = ExtractGlobalPID(applicationName);
    }

    SpinLockAcquire(&Session_ctx::BackendCtx().MyBackendData->mutex);

    /*
     * Skip updating globalpid when we were a command originator and still are
     * and we already have a valid global pid assigned.
     * See function comment for detailed explanation.
     */
    if (!Session_ctx::BackendCtx().MyBackendData->distributedCommandOriginator ||
        !distributedCommandOriginator ||
        Session_ctx::BackendCtx().MyBackendData->globalPID ==
            INVALID_CITUS_INTERNAL_BACKEND_GPID) {
        Session_ctx::BackendCtx().MyBackendData->globalPID = globalPID;
        Session_ctx::BackendCtx().MyBackendData->distributedCommandOriginator =
            distributedCommandOriginator;
    }
    SpinLockRelease(&Session_ctx::BackendCtx().MyBackendData->mutex);
}

/*
 * SetBackendDataDatabaseId sets the databaseId in the backend data.
 *
 * NOTE: this needs to be run after the auth hook, because in the auth hook the
 * database is not known yet.
 */
void SetBackendDataDatabaseId(void)
{
    Assert(u_sess->proc_cxt.MyDatabaseId != InvalidOid);
    SpinLockAcquire(&Session_ctx::BackendCtx().MyBackendData->mutex);
    Session_ctx::BackendCtx().MyBackendData->databaseId = u_sess->proc_cxt.MyDatabaseId;
    SpinLockRelease(&Session_ctx::BackendCtx().MyBackendData->mutex);
}

/*
 * SetBackendDataGlobalPID is used to set the gpid field on MyBackendData.
 *
 * IMPORTANT: This should not be used for normal operations. It's a very hacky
 * way of setting the gpid, which is only used in our MX isolation tests. The
 * main problem is that it does not set distributedCommandOriginator to the
 * correct value.
 */
void SetBackendDataGlobalPID(uint64 gpid)
{
    if (!Session_ctx::BackendCtx().MyBackendData) {
        return;
    }
    SpinLockAcquire(&Session_ctx::BackendCtx().MyBackendData->mutex);
    Session_ctx::BackendCtx().MyBackendData->globalPID = gpid;
    SpinLockRelease(&Session_ctx::BackendCtx().MyBackendData->mutex);
}

/*
 * SetBackendDataDistributedCommandOriginator sets the distributedCommandOriginator
 * field on MyBackendData.
 */
void SetBackendDataDistributedCommandOriginator(bool distributedCommandOriginator)
{
    if (!Session_ctx::BackendCtx().MyBackendData) {
        return;
    }
    SpinLockAcquire(&Session_ctx::BackendCtx().MyBackendData->mutex);
    Session_ctx::BackendCtx().MyBackendData->distributedCommandOriginator =
        distributedCommandOriginator;
    SpinLockRelease(&Session_ctx::BackendCtx().MyBackendData->mutex);
}

/*
 * GetGlobalPID returns the global process id of the current backend.
 */
uint64 GetGlobalPID(void)
{
    uint64 globalPID = INVALID_CITUS_INTERNAL_BACKEND_GPID;

    if (Session_ctx::BackendCtx().MyBackendData) {
        SpinLockAcquire(&Session_ctx::BackendCtx().MyBackendData->mutex);
        globalPID = Session_ctx::BackendCtx().MyBackendData->globalPID;
        SpinLockRelease(&Session_ctx::BackendCtx().MyBackendData->mutex);
    }

    return globalPID;
}

/*
 * spq_calculate_gpid calculates the gpid for any given process on any
 * given node.
 */
Datum spq_calculate_gpid(PG_FUNCTION_ARGS)
{
    CheckCitusVersion(ERROR);

    int32 nodeId = PG_GETARG_INT32(0);
    int32 tid = PG_GETARG_INT32(1);

    return UInt64GetDatum(CalculateGlobalPID(nodeId, tid));
}

/*
 * CalculateGlobalPID gets a nodeId and tid, and returns the global pid
 * that can be assigned for a process with the given input.
 */
static uint64 CalculateGlobalPID(int32 nodeId, pid_t tid)
{
    /*
     * We try to create a human readable global pid that consists of node id and thread
     * id. By multiplying node id with 10^10 and adding tid we generate a number where the
     * smallest 10 digit represent the tid and the remaining digits are the node id.
     *
     * Both node id and pid are 32 bit. We use 10^10 to fit all possible tids. Some very
     * large node ids might cause overflow. But even for the applications that scale
     * around 50 nodes every day it'd take about 100K years. So we are not worried.
     */
    return (((uint64)nodeId) * GLOBAL_PID_NODE_ID_MULTIPLIER) + tid;
}

/*
 * GenerateGlobalPID generates the global process id for the current backend.
 * See CalculateGlobalPID for the details.
 */
static uint64 GenerateGlobalPID(void)
{
    /* use gettid(), it will return thread id(pid_t), unique in one process */
    return CalculateGlobalPID(GetLocalNodeId(), gettid());
}

/*
 * spq_backend_gpid similar to pg_backend_pid, but returns Citus
 * assigned gpid.
 */
Datum spq_backend_gpid(PG_FUNCTION_ARGS)
{
    CheckCitusVersion(ERROR);

    return UInt64GetDatum(GetGlobalPID());
}

/*
 * spq_nodeid_for_gpid returns node id for the global process with given global pid
 */
Datum spq_nodeid_for_gpid(PG_FUNCTION_ARGS)
{
    CheckCitusVersion(ERROR);

    uint64 globalPID = PG_GETARG_INT64(0);

    bool missingOk = false;
    PG_RETURN_INT32(ExtractNodeIdFromGlobalPID(globalPID, missingOk));
}

/*
 * spq_pid_for_gpid returns process id for the global process with given global pid
 */
Datum spq_pid_for_gpid(PG_FUNCTION_ARGS)
{
    CheckCitusVersion(ERROR);

    uint64 globalPID = PG_GETARG_INT64(0);

    PG_RETURN_INT32(ExtractProcessIdFromGlobalPID(globalPID));
}

/*
 * ExtractGlobalPID extracts the global process id from the application name and returns
 * it if the application name is not compatible with Citus' application names returns 0.
 */
uint64 ExtractGlobalPID(const char* applicationName)
{
    /* does application name exist */
    if (!applicationName) {
        return INVALID_CITUS_INTERNAL_BACKEND_GPID;
    }

    /* we create our own copy of application name incase the original changes */
    char* applicationNameCopy = pstrdup(applicationName);

    for (int i = 0; i < lengthof(SpqBackendPrefixes); i++) {
        uint64 prefixLength = strlen(SpqBackendPrefixes[i]);

        /* does application name start with this prefix prefix */
        if (strncmp(applicationNameCopy, SpqBackendPrefixes[i], prefixLength) != 0) {
            continue;
        }

        char* globalPIDString = &applicationNameCopy[prefixLength];
        uint64 globalPID = strtoul(globalPIDString, NULL, 10);
        if (globalPID == 0) {
            /*
             * INVALID_CITUS_INTERNAL_BACKEND_GPID is 0, but just to be explicit
             * about how we handle strtoul errors.
             */
            return INVALID_CITUS_INTERNAL_BACKEND_GPID;
        }

        return globalPID;
    }
    return INVALID_CITUS_INTERNAL_BACKEND_GPID;
}

/*
 * ExtractNodeIdFromGlobalPID extracts the node id from the global pid.
 * Global pid is constructed by multiplying node id with GLOBAL_PID_NODE_ID_MULTIPLIER
 * and adding process id. So integer division of global pid by
 * GLOBAL_PID_NODE_ID_MULTIPLIER gives us the node id.
 */
int ExtractNodeIdFromGlobalPID(uint64 globalPID, bool missingOk)
{
    int nodeId = (int)(globalPID / GLOBAL_PID_NODE_ID_MULTIPLIER);

    if (!missingOk && nodeId == GLOBAL_PID_NODE_ID_FOR_NODES_NOT_IN_METADATA) {
        ereport(ERROR, (errmsg("originator node of the query with the global pid "
                               "%lu is not in Citus' metadata",
                               globalPID),
                        errhint("connect to the node directly run pg_cancel_backend(pid) "
                                "or pg_terminate_backend(pid)")));
    }

    return nodeId;
}

/*
 * ExtractProcessIdFromGlobalPID extracts the process id from the global pid.
 * Global pid is constructed by multiplying node id with GLOBAL_PID_NODE_ID_MULTIPLIER
 * and adding process id. So global pid mod GLOBAL_PID_NODE_ID_MULTIPLIER gives us the
 * process id.
 */
int ExtractProcessIdFromGlobalPID(uint64 globalPID)
{
    return (int)(globalPID % GLOBAL_PID_NODE_ID_MULTIPLIER);
}

/*
 * CurrentDistributedTransactionNumber returns the transaction number of the
 * current distributed transaction. The caller must make sure a distributed
 * transaction is in progress.
 */
uint64 CurrentDistributedTransactionNumber(void)
{
    Assert(Session_ctx::BackendCtx().MyBackendData != NULL);

    return Session_ctx::BackendCtx().MyBackendData->transactionId.transactionNumber;
}

/*
 * GetBackendDataForProc writes the backend data for the given process to
 * result. If the process is part of a lock group (parallel query) it
 * returns the leader data instead.
 */
void GetBackendDataForProc(PGPROC* proc, BackendData* result)
{
    int pgprocno = getProcNo_compat(proc);

    if (proc->lockGroupLeader != NULL) {
        pgprocno = getProcNo_compat(proc->lockGroupLeader);
    }

    BackendData* backendData = &backendManagementShmemData->backends[pgprocno];

    SpinLockAcquire(&backendData->mutex);

    *result = *backendData;

    SpinLockRelease(&backendData->mutex);
}

/*
 * CancelTransactionDueToDeadlock cancels the input proc and also marks the backend
 * data with this information.
 */
void CancelTransactionDueToDeadlock(PGPROC* proc)
{
    BackendData* backendData =
        &backendManagementShmemData->backends[getProcNo_compat(proc)];

    /* backend might not have used citus yet and thus not initialized backend data */
    if (!backendData) {
        return;
    }

    SpinLockAcquire(&backendData->mutex);

    /* send a SIGINT only if the process is still in a distributed transaction */
    if (backendData->transactionId.transactionNumber != 0) {
        backendData->cancelledDueToDeadlock = true;
        SpinLockRelease(&backendData->mutex);

        if (gs_signal_send(proc->pid, SIGINT) != 0) {
            ereport(WARNING,
                    (errmsg("attempted to cancel this backend (pid: %lu) to resolve a "
                            "distributed deadlock but the backend could not "
                            "be cancelled",
                            proc->pid)));
        }
    } else {
        SpinLockRelease(&backendData->mutex);
    }
}

/*
 * MyBackendGotCancelledDueToDeadlock returns whether the current distributed
 * transaction was cancelled due to a deadlock. If the backend is not in a
 * distributed transaction, the function returns false.
 * We keep some session level state to keep track of if we were cancelled
 * because of a distributed deadlock. When clearState is true, this function
 * also resets that state. So after calling this function with clearState true,
 * a second would always return false.
 */
bool MyBackendGotCancelledDueToDeadlock(bool clearState)
{
    bool cancelledDueToDeadlock = false;

    /* backend might not have used citus yet and thus not initialized backend data */
    if (!Session_ctx::BackendCtx().MyBackendData) {
        return false;
    }

    SpinLockAcquire(&Session_ctx::BackendCtx().MyBackendData->mutex);

    if (IsInDistributedTransaction(Session_ctx::BackendCtx().MyBackendData)) {
        cancelledDueToDeadlock =
            Session_ctx::BackendCtx().MyBackendData->cancelledDueToDeadlock;
    }
    if (clearState) {
        Session_ctx::BackendCtx().MyBackendData->cancelledDueToDeadlock = false;
    }

    SpinLockRelease(&Session_ctx::BackendCtx().MyBackendData->mutex);

    return cancelledDueToDeadlock;
}

/*
 * ActiveDistributedTransactionNumbers returns a list of pointers to
 * transaction numbers of distributed transactions that are in progress
 * and were started by the node on which it is called.
 */
List* ActiveDistributedTransactionNumbers(void)
{
    List* activeTransactionNumberList = NIL;

    /* build list of starting procs */
    for (int curBackend = 0; curBackend < g_instance.shmem_cxt.MaxBackends;
         curBackend++) {
        PGPROC* currentProc = g_instance.proc_base_all_procs[curBackend];
        BackendData currentBackendData;

        if (currentProc->pid == 0) {
            /* unused PGPROC slot */
            continue;
        }

        GetBackendDataForProc(currentProc, &currentBackendData);
        if (!currentBackendData.activeBackend) {
            /*
             * Skip if the PGPROC slot is unused. We should normally use
             * IsBackendPid() to be able to skip reliably all the exited
             * processes. However, that is a costly operation. Instead, we
             * keep track of activeBackend in Citus code.
             */
            continue;
        }

        if (!IsInDistributedTransaction(&currentBackendData)) {
            /* not a distributed transaction */
            continue;
        }

        if (!currentBackendData.transactionId.transactionOriginator) {
            /* not a coordinator process */
            continue;
        }

        uint64* transactionNumber = (uint64*)palloc0(sizeof(uint64));
        *transactionNumber = currentBackendData.transactionId.transactionNumber;

        activeTransactionNumberList =
            lappend(activeTransactionNumberList, transactionNumber);
    }

    return activeTransactionNumberList;
}

/*
 * GetMyProcLocalTransactionId() is a wrapper for
 * getting lxid of MyProc.
 */
LocalTransactionId GetMyProcLocalTransactionId(void)
{
    return getLxid_compat(t_thrd.proc);
}

/*
 * GetExternalClientBackendCount returns externalClientBackendCounter in
 * the shared memory.
 */
int GetExternalClientBackendCount(void)
{
    uint32 activeBackendCount =
        pg_atomic_read_u32(&backendManagementShmemData->externalClientBackendCounter);

    return activeBackendCount;
}

/*
 * IncrementExternalClientBackendCounter increments externalClientBackendCounter in
 * the shared memory by one.
 */
uint32 IncrementExternalClientBackendCounter(void)
{
    return pg_atomic_add_fetch_u32(
        &backendManagementShmemData->externalClientBackendCounter, 1);
}

/*
 * DecrementExternalClientBackendCounter decrements externalClientBackendCounter in
 * the shared memory by one.
 */
void DecrementExternalClientBackendCounter(void)
{
    pg_atomic_sub_fetch_u32(&backendManagementShmemData->externalClientBackendCounter, 1);
}

/*
 * IsRebalancerInitiatedBackend returns true if we are in a backend that citus
 * rebalancer initiated.
 */
bool IsRebalancerInternalBackend(void)
{
    if (Session_ctx::BackendCtx().CurrentBackendType == CITUS_BACKEND_NOT_ASSIGNED) {
        DetermineCitusBackendType(u_sess->attr.attr_common.application_name);
    }

    return Session_ctx::BackendCtx().CurrentBackendType == CITUS_REBALANCER_BACKEND;
}

/*
 * IsCitusInitiatedRemoteBackend returns true if we are in a backend that citus
 * initiated via remote connection.
 */
bool IsCitusInternalBackend(void)
{
    if (Session_ctx::BackendCtx().CurrentBackendType == CITUS_BACKEND_NOT_ASSIGNED) {
        DetermineCitusBackendType(u_sess->attr.attr_common.application_name);
    }

    return Session_ctx::BackendCtx().CurrentBackendType == CITUS_INTERNAL_BACKEND;
}

/*
 * IsCitusRunCommandBackend returns true if we are in a backend that one of
 * the run_command_on_* functions initiated.
 */
bool IsCitusRunCommandBackend(void)
{
    if (Session_ctx::BackendCtx().CurrentBackendType == CITUS_BACKEND_NOT_ASSIGNED) {
        DetermineCitusBackendType(u_sess->attr.attr_common.application_name);
    }

    return Session_ctx::BackendCtx().CurrentBackendType == CITUS_RUN_COMMAND_BACKEND;
}

bool IsExternalClientBackend(void)
{
    if (Session_ctx::BackendCtx().CurrentBackendType == CITUS_BACKEND_NOT_ASSIGNED) {
        DetermineCitusBackendType(u_sess->attr.attr_common.application_name);
    }

    return Session_ctx::BackendCtx().CurrentBackendType == EXTERNAL_CLIENT_BACKEND;
}

/*
 * IsRebalancerInitiatedBackend returns true if we are in a backend that citus
 * rebalancer initiated.
 */
bool IsCitusShardTransferBackend(void)
{
    int prefixLength = strlen(CITUS_SHARD_TRANSFER_APPLICATION_NAME_PREFIX);

    return strncmp(u_sess->attr.attr_common.application_name,
                   CITUS_SHARD_TRANSFER_APPLICATION_NAME_PREFIX, prefixLength) == 0;
}

/*
 * DetermineCitusBackendType determines the type of backend based on the application_name.
 */
void DetermineCitusBackendType(const char* applicationName)
{
    if (applicationName &&
        ExtractGlobalPID(applicationName) != INVALID_CITUS_INTERNAL_BACKEND_GPID) {
        for (int i = 0; i < lengthof(SpqBackendPrefixes); i++) {
            uint64 prefixLength = strlen(SpqBackendPrefixes[i]);

            /* does application name start with this prefix prefix */
            if (strncmp(applicationName, SpqBackendPrefixes[i], prefixLength) == 0) {
                Session_ctx::BackendCtx().CurrentBackendType = CitusBackendTypes[i];
                return;
            }
        }
    }
    Session_ctx::BackendCtx().CurrentBackendType = EXTERNAL_CLIENT_BACKEND;
}

/*
 * DetermineCitusBackendType determines the type of backend based on the application_name.
 */
void DetermineAuthBackendType(const char* applicationName)
{
    if (applicationName &&
        ExtractGlobalPID(applicationName) != INVALID_CITUS_INTERNAL_BACKEND_GPID) {
        for (int i = 0; i < lengthof(SpqBackendPrefixes); i++) {
            uint64 prefixLength = strlen(SpqBackendPrefixes[i]);

            /* does application name start with this prefix prefix */
            if (strncmp(applicationName, SpqBackendPrefixes[i], prefixLength) == 0) {
                initBackendType = CitusBackendTypes[i];
                return;
            }
        }
    }
    initBackendType = EXTERNAL_CLIENT_BACKEND;
}
