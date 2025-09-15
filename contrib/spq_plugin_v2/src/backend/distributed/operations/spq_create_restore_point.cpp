/*-------------------------------------------------------------------------
 *
 * spq_create_restore_point.c
 *
 * UDF for creating a consistent restore point across all nodes.
 *
 * Copyright (c) Citus Data, Inc.
 *
 *-------------------------------------------------------------------------
 */

#include "postgres.h"

#include "libpq/libpq-fe.h"

#include "access/xlog.h"
#include "access/xlog_internal.h"
#include "catalog/pg_type.h"
#include "nodes/pg_list.h"
#include "storage/lmgr.h"
#include "storage/lock/lock.h"
#include "utils/builtins.h"
#include "utils/pg_lsn.h"

#include "distributed/connection_management.h"
#include "distributed/listutils.h"
#include "distributed/metadata_cache.h"
#include "distributed/metadata_utility.h"
#include "distributed/remote_commands.h"
#include "funcapi.h"

#define CREATE_RESTORE_POINT_COMMAND "SELECT pg_catalog.pg_create_restore_point($1::text)"

#define GET_NODE_ID_COMMAND                                               \
    "SELECT nodeid from pg_catalog.pg_dist_node where nodename = $1 and " \
    "nodeport = $2"

/* local functions forward declarations */
static List* OpenConnectionsToAllWorkerNodes(LOCKMODE lockMode);
static void BlockDistributedTransactions(void);
static List* CreateRemoteRestorePoints(char* restoreName, List* connectionList);

/* exports for SQL callable functions */
PG_FUNCTION_INFO_V1(spq_create_restore_point);
extern "C" Datum spq_create_restore_point(PG_FUNCTION_ARGS);

struct SpqRestorePoint {
    int32_t nodeId = 0;
    char* nodeName = nullptr;
    int32_t nodePort = 0;
    char* xlogPtr = nullptr;
};
static SpqRestorePoint* GetLocalSpqRestorePoint(char* restoreNameString);
static void QueryWorkerInfoViaSpi(List* spqRestorePointList);

/*
 * spq_create_restore_point blocks writes to distributed tables and then
 * runs pg_create_restore_point on all nodes. This creates a consistent
 * restore point under the assumption that there are no other writers
 * than the coordinator.
 */
Datum spq_create_restore_point(PG_FUNCTION_ARGS)
{
    FuncCallContext* functionContext = NULL;
    List* SpqRestorePointList = NIL;

    if (SRF_IS_FIRSTCALL()) {
        CheckCitusVersion(ERROR);
        EnsureCoordinator();

        text* restoreNameText = PG_GETARG_TEXT_P(0);

        if (RecoveryInProgress()) {
            ereport(
                ERROR,
                (errcode(ERRCODE_OBJECT_NOT_IN_PREREQUISITE_STATE),
                 (errmsg("recovery is in progress"),
                  errhint("WAL control functions cannot be executed during recovery."))));
        }

        if (!XLogIsNeeded()) {
            ereport(
                ERROR,
                (errcode(ERRCODE_OBJECT_NOT_IN_PREREQUISITE_STATE),
                 errmsg("WAL level not sufficient for creating a restore point"),
                 errhint("wal_level must be set to \"replica\" or \"logical\" at server "
                         "start.")));
        }

        char* restoreNameString = text_to_cstring(restoreNameText);
        if (strlen(restoreNameString) >= MAXFNAMELEN) {
            ereport(ERROR,
                    (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                     errmsg("value too long for restore point (maximum %d characters)",
                            MAXFNAMELEN - 1)));
        }

        /* create a function context for cross-call persistence */
        functionContext = SRF_FIRSTCALL_INIT();

        /* switch to memory context appropriate for multiple function calls */
        MemoryContext oldContext =
            MemoryContextSwitchTo(functionContext->multi_call_memory_ctx);

        /*
         * establish connections to all nodes before taking any locks
         * ShareLock prevents new nodes being added, rendering connectionList incomplete
         */
        List* connectionList = OpenConnectionsToAllWorkerNodes(ShareLock);

        /*
         * Send a BEGIN to bust through pgbouncer. We won't actually commit since
         * that takes time. Instead we just close the connections and roll back,
         * which doesn't undo pg_create_restore_point.
         */
        RemoteTransactionListBegin(connectionList);

        /* DANGER: finish as quickly as possible after this */
        BlockDistributedTransactions();

        /* Get and append the local restore point */
        SpqRestorePoint* localRestorePoint = GetLocalSpqRestorePoint(restoreNameString);

        /* run pg_create_restore_point on all nodes */
        SpqRestorePointList =
            CreateRemoteRestorePoints(restoreNameString, connectionList);

        QueryWorkerInfoViaSpi(SpqRestorePointList);

        SpqRestorePointList = lcons(localRestorePoint, SpqRestorePointList);

        functionContext->user_fctx = SpqRestorePointList;
        functionContext->max_calls = (uint32)list_length(SpqRestorePointList);

        /*
         * This tuple descriptor must match the output parameters declared for
         * the function in pg_proc.
         */
        TupleDesc tupleDescriptor = CreateTemplateTupleDesc(4, false);
        TupleDescInitEntry(tupleDescriptor, (AttrNumber)1, "nodeid", INT4OID, -1, 0);
        TupleDescInitEntry(tupleDescriptor, (AttrNumber)2, "nodename", TEXTOID, -1, 0);
        TupleDescInitEntry(tupleDescriptor, (AttrNumber)3, "nodeport", INT4OID, -1, 0);
        TupleDescInitEntry(tupleDescriptor, (AttrNumber)4, "xlogptr", TEXTOID, -1, 0);

        functionContext->tuple_desc = BlessTupleDesc(tupleDescriptor);

        MemoryContextSwitchTo(oldContext);
    }

    functionContext = SRF_PERCALL_SETUP();
    if (functionContext->call_cntr < functionContext->max_calls) {
        List* restorePoints = (List*)functionContext->user_fctx;
        SpqRestorePoint* point =
            (SpqRestorePoint*)list_nth(restorePoints, functionContext->call_cntr);

        Datum values[4];
        bool nulls[4] = {false};

        values[0] = Int32GetDatum(point->nodeId);
        values[1] = CStringGetTextDatum(point->nodeName);
        values[2] = Int32GetDatum(point->nodePort);
        values[3] = CStringGetTextDatum(point->xlogPtr);

        HeapTuple tuple = heap_form_tuple(functionContext->tuple_desc, values, nulls);
        SRF_RETURN_NEXT(functionContext, HeapTupleGetDatum(tuple));
    }

    list_free((List*)functionContext->user_fctx);

    SRF_RETURN_DONE(functionContext);
}

/*
 * OpenConnectionsToAllNodes opens connections to all nodes and returns the list
 * of connections.
 */
static List* OpenConnectionsToAllWorkerNodes(LOCKMODE lockMode)
{
    List* connectionList = NIL;
    int connectionFlags = FORCE_NEW_CONNECTION;

    List* workerNodeList = ActivePrimaryNonCoordinatorNodeList(lockMode);

    WorkerNode* workerNode = NULL;
    foreach_declared_ptr(workerNode, workerNodeList)
    {
        MultiConnection* connection = StartNodeConnection(
            connectionFlags, workerNode->workerName, workerNode->workerPort);
        MarkRemoteTransactionCritical(connection);

        connectionList = lappend(connectionList, connection);
    }

    FinishConnectionListEstablishment(connectionList);

    return connectionList;
}

/*
 * BlockDistributedTransactions blocks distributed transactions that use 2PC
 * and changes to pg_dist_node (e.g. node addition) and pg_dist_partition
 * (table creation).
 */
static void BlockDistributedTransactions(void)
{
    LockRelationOid(DistNodeRelationId(), ExclusiveLock);
    LockRelationOid(DistPartitionRelationId(), ExclusiveLock);
    LockRelationOid(DistTransactionRelationId(), ExclusiveLock);
}

struct pg_conn;

/*
 * CreateRemoteRestorePoints creates a restore point via each of the
 * connections in the list in parallel.
 */
static List* CreateRemoteRestorePoints(char* restoreName, List* connectionList)
{
    int parameterCount = 1;
    Oid parameterTypes[1] = {TEXTOID};
    const char* parameterValues[1] = {restoreName};
    List* res = NIL;

    MultiConnection* connection = NULL;
    foreach_declared_ptr(connection, connectionList)
    {
        SpqRestorePoint* spqRestorePoint =
            (SpqRestorePoint*)palloc0(sizeof(SpqRestorePoint));
        spqRestorePoint->nodeName = pstrdup(connection->hostname);
        spqRestorePoint->nodePort = connection->port;

        if (PQstatus(connection->pgConn) != CONNECTION_OK) {
            ReportConnectionError(connection, ERROR);
            return NIL;
        }
        int querySent = SendRemoteCommandParams(connection, CREATE_RESTORE_POINT_COMMAND,
                                                parameterCount, parameterTypes,
                                                parameterValues, false);
        if (querySent == 0) {
            ReportConnectionError(connection, ERROR);
            return NIL;
        }

        PGresult* result = GetRemoteCommandResult(connection, true);
        if (!IsResponseOK(result)) {
            ReportResultError(connection, result, ERROR);
            return NIL;
        }
        char* xlogPtr = PQgetvalue(result, 0, 0);
        if (NULL == xlogPtr) {
            PQclear(result);
            ReportResultError(connection, result, ERROR);
            return NIL;
        }
        spqRestorePoint->xlogPtr = pstrdup(xlogPtr);
        PQclear(result);
        ForgetResults(connection);

        if (PQstatus(connection->pgConn) != CONNECTION_OK) {
            ReportConnectionError(connection, ERROR);
            ReportResultError(connection, result, ERROR);
            return NIL;
        }

        res = lappend(res, spqRestorePoint);
        CloseConnection(connection);
    }

    return res;
}

static void QueryNodeInfoViaSpi(const char* query, SpqRestorePoint* spqRestorePoint,
                                MemoryContext currCtx)
{
    List* results = NIL;

    if (SPI_connect() != SPI_OK_CONNECT) {
        ereport(ERROR,
                (errcode(ERRCODE_CONNECTION_FAILURE), errmsg("can't connect to spi")));
        return;
    }
    int ret = SPI_execute(query, true, 0);
    if (ret != SPI_OK_SELECT) {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                        errmsg("spi execute failed: %s", SPI_result_code_string(ret))));
        SPI_finish();
        return;
    }
    uint64 processed = SPI_processed;
    if (processed != 1) {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                        errmsg("spi execute failed: %s, processed: %ul",
                               SPI_result_code_string(ret), &processed)));
        SPI_finish();
        return;
    }

    TupleDesc tupdesc = SPI_tuptable->tupdesc;
    SPITupleTable* tuptable = SPI_tuptable;
    HeapTuple tuple = tuptable->vals[0];
    bool isNull;

    Datum nodeidDatum = SPI_getbinval(tuple, tupdesc, 1, &isNull);
    spqRestorePoint->nodeId = isNull ? -1 : DatumGetInt32(nodeidDatum);
    Datum nodenameDatum = SPI_getbinval(tuple, tupdesc, 2, &isNull);
    if (!isNull) {
        MemoryContext oldcontext = MemoryContextSwitchTo(currCtx);
        spqRestorePoint->nodeName = pstrdup(TextDatumGetCString(nodenameDatum));
        MemoryContextSwitchTo(oldcontext);
    } else {
        spqRestorePoint->nodeName = nullptr;
    }
    Datum nodeportDatum = SPI_getbinval(tuple, tupdesc, 3, &isNull);
    spqRestorePoint->nodePort = isNull ? -1 : DatumGetInt32(nodeportDatum);

    SPI_finish();
}

static void QueryCoordinatorInfoViaSpi(SpqRestorePoint* spqRestorePoint)
{
    const char* query = "SELECT nodeid, nodename, nodeport FROM pg_catalog.pg_dist_node "
                        "WHERE groupid = 0";
    QueryNodeInfoViaSpi(query, spqRestorePoint, CurrentMemoryContext);
}

static void QueryWorkerInfoViaSpi(List* spqRestorePointList)
{
    SpqRestorePoint* spqRestorePoint = nullptr;
    foreach_declared_ptr(spqRestorePoint, spqRestorePointList)
    {
        char* portStr = psprintf("%d", spqRestorePoint->nodePort);
        char* query = psprintf("SELECT nodeid, nodename, nodeport FROM pg_dist_node "
                               "WHERE nodename = '%s' AND nodeport = %s",
                               spqRestorePoint->nodeName, portStr);
        QueryNodeInfoViaSpi(query, spqRestorePoint, CurrentMemoryContext);
    }
}

static char* XLogRecPtrToString(XLogRecPtr xlogRecPtr)
{
    char* recStr = (char*)palloc0(64);
    int res = snprintf_s(recStr, 64, 64 - 1, "%X/%X", (uint32)(xlogRecPtr >> 32),
                         (uint32)xlogRecPtr);
    securec_check_ss(res, "", "");
    return recStr;
}

static SpqRestorePoint* GetLocalSpqRestorePoint(char* restoreNameString)
{
    SpqRestorePoint* spqRestorePoint = (SpqRestorePoint*)palloc0(sizeof(SpqRestorePoint));
    XLogRecPtr localRestorePoint = XLogRestorePoint(restoreNameString);
    spqRestorePoint->xlogPtr = XLogRecPtrToString(localRestorePoint);
    QueryCoordinatorInfoViaSpi(spqRestorePoint);
    return spqRestorePoint;
}