/*-------------------------------------------------------------------------
 *
 * vacuum.c
 *    Commands for vacuuming distributed tables.
 *
 * Copyright (c) Citus Data, Inc.
 *
 *-------------------------------------------------------------------------
 */

#include "postgres.h"

#include "access/xact.h"
#include "commands/defrem.h"
#include "commands/vacuum.h"
#ifdef DISABLE_OG_COMMENTS
#include "postmaster/bgworker_internals.h"
#endif
#include "storage/lmgr.h"
#include "utils/builtins.h"
#include "utils/lsyscache.h"

#include "pg_version_constants.h"

#include "distributed/adaptive_executor.h"
#include "distributed/commands.h"
#include "distributed/commands/utility_hook.h"
#include "distributed/deparse_shard_query.h"
#include "distributed/listutils.h"
#include "distributed/metadata_cache.h"
#include "distributed/metadata_sync.h"
#include "distributed/resource_lock.h"
#include "distributed/transaction_management.h"
#include "distributed/version_compat.h"

/*
 * Subset of VacuumParams we care about
 */
typedef struct CitusVacuumParams {
    int options;
} CitusVacuumParams;

/* Local functions forward declarations for processing distributed table commands */
static List* VacuumTaskList(Oid relationId, CitusVacuumParams vacuumParams,
                            List* vacuumColumnList);
static char* DeparseVacuumStmtPrefix(CitusVacuumParams vacuumParams);
static char* DeparseVacuumColumnNames(List* columnNameList);
static List* VacuumColumnList(VacuumStmt* vacuumStmt, int relationIndex);
static void ExecuteVacuumOnDistributedTables(VacuumStmt* vacuumStmt, Oid relationId,
                                             CitusVacuumParams vacuumParams);
static void ExecuteUnqualifiedVacuumTasks(VacuumStmt* vacuumStmt,
                                          CitusVacuumParams vacuumParams);
static CitusVacuumParams VacuumStmtParams(VacuumStmt* vacstmt);

/*
 * PostprocessVacuumStmt processes vacuum statements that may need propagation to
 * citus tables only if ddl propagation is enabled. If a VACUUM or ANALYZE command
 * references a citus table or no table, it is propagated to all involved nodes;
 * otherwise, the statements will not be propagated.
 *
 * Unlike most other Process functions within this file, this function does not
 * return a modified parse node, as it is expected that the local VACUUM or
 * ANALYZE has already been processed.
 */
List* PostprocessVacuumStmt(Node* node, const char* vacuumCommand)
{
    VacuumStmt* vacuumStmt = castNode(VacuumStmt, node);

    CitusVacuumParams vacuumParams = VacuumStmtParams(vacuumStmt);

    /*
     * don't handle VACUUM DELTAMERGE
     */
    if (vacuumParams.options & VACOPT_MERGE) {
        return NIL;
    }

    /*
     * don't handle VACUUM PARTITION and SUBPARTITION
     */
    if (vacuumStmt->relation) {
        RangeVar* rel = vacuumStmt->relation;
        if (rel->partitionname || rel->subpartitionname) {
            return NIL;
        }
    }

    if (vacuumParams.options & VACOPT_VACUUM) {
        /*
         * We commit the current transaction here so that the global lock
         * taken from the shell table for VACUUM is released, which would block execution
         * of shard placements. We don't do this in case of "ANALYZE <table>" command
         * because its semantics are different than VACUUM and it doesn't acquire the
         * global lock.
         */
        CommitTransactionCommand();
        StartTransactionCommand();
    }

    /*
     * when no table is specified propagate the command as it is;
     * otherwise, only propagate when there is at least 1 citus table
     */
    Oid relationId = NULL;
    if (vacuumStmt->relation) {
        relationId = RangeVarGetRelidExtended(vacuumStmt->relation, NoLock, true, false,
                                              false, true, NULL, NULL);
    }
    if (relationId == NULL) {
        /* no table is specified (unqualified vacuum) */
        ExecuteUnqualifiedVacuumTasks(vacuumStmt, vacuumParams);
    } else if (OidIsValid(relationId) && IsCitusTable(relationId)) {
        /* there is at least 1 citus table specified */
        ExecuteVacuumOnDistributedTables(vacuumStmt, relationId, vacuumParams);
    }
    /* else only local tables are specified */

    return NIL;
}

/*
 * ExecuteVacuumOnDistributedTables executes the vacuum for the shard placements of given
 * tables if they are citus tables.
 */
static void ExecuteVacuumOnDistributedTables(VacuumStmt* vacuumStmt, Oid relationId,
                                             CitusVacuumParams vacuumParams)
{
    List* taskList = VacuumTaskList(relationId, vacuumParams, vacuumStmt->va_cols);
    /* local execution is not implemented for VACUUM commands */
    ExecuteUtilityTaskList(taskList, false);
}

/*
 * VacuumTaskList returns a list of tasks to be executed as part of processing
 * a VacuumStmt which targets a distributed relation.
 */
static List* VacuumTaskList(Oid relationId, CitusVacuumParams vacuumParams,
                            List* vacuumColumnList)
{
    LOCKMODE lockMode = (vacuumParams.options & VACOPT_FULL) ? AccessExclusiveLock
                                                             : ShareUpdateExclusiveLock;

    /* resulting task list */
    List* taskList = NIL;

    /* enumerate the tasks when putting them to the taskList */
    int taskId = 1;

    Oid schemaId = get_rel_namespace(relationId);
    char* schemaName = get_namespace_name(schemaId);
    char* relationName = get_rel_name(relationId);

    const char* vacuumStringPrefix = DeparseVacuumStmtPrefix(vacuumParams);
    const char* columnNames = DeparseVacuumColumnNames(vacuumColumnList);

    LockRelationOid(relationId, lockMode);

    List* shardIntervalList = LoadShardIntervalList(relationId);

    /* grab shard lock before getting placement list */
    LockShardListMetadata(shardIntervalList, ShareLock);

    ShardInterval* shardInterval = NULL;
    foreach_declared_ptr(shardInterval, shardIntervalList)
    {
        uint64 shardId = shardInterval->shardId;
        char* shardRelationName = pstrdup(relationName);

        /* build shard relation name */
        AppendShardIdToName(&shardRelationName, shardId);

        char* quotedShardName = quote_qualified_identifier(schemaName, shardRelationName);

        /* copy base vacuum string and build the shard specific command */
        StringInfo vacuumStringForShard = makeStringInfo();
        appendStringInfoString(vacuumStringForShard, vacuumStringPrefix);

        appendStringInfoString(vacuumStringForShard, quotedShardName);
        appendStringInfoString(vacuumStringForShard, columnNames);

        Task* task = CitusMakeNode(Task);
        task->jobId = INVALID_JOB_ID;
        task->taskId = taskId++;
        task->taskType = VACUUM_ANALYZE_TASK;
        SetTaskQueryString(task, vacuumStringForShard->data);
        task->dependentTaskList = NULL;
        task->replicationModel = REPLICATION_MODEL_INVALID;
        task->anchorShardId = shardId;
        task->taskPlacementList = ActiveShardPlacementList(shardId);
        task->cannotBeExecutedInTransction = ((vacuumParams.options & VACOPT_VACUUM) ||
                                              (vacuumParams.options & VACOPT_ANALYZE));

        taskList = lappend(taskList, task);
    }

    return taskList;
}

/*
 * DeparseVacuumStmtPrefix returns a StringInfo appropriate for use as a prefix
 * during distributed execution of a VACUUM or ANALYZE statement. Callers may
 * reuse this prefix within a loop to generate shard-specific VACUUM or ANALYZE
 * statements.
 */
static char* DeparseVacuumStmtPrefix(CitusVacuumParams vacuumParams)
{
    int vacuumFlags = vacuumParams.options;
    StringInfo vacuumPrefix = makeStringInfo();

    /* determine actual command and block out its bits */
    if (vacuumFlags & VACOPT_VACUUM) {
        appendStringInfoString(vacuumPrefix, "VACUUM ");
        if (vacuumFlags == VACOPT_VACUUM) {
            return vacuumPrefix->data;
        }
        vacuumFlags &= ~VACOPT_VACUUM;
    } else {
        Assert((vacuumFlags & VACOPT_ANALYZE) != 0);

        appendStringInfoString(vacuumPrefix, "ANALYZE ");
        vacuumFlags &= ~VACOPT_ANALYZE;

        if (vacuumFlags & VACOPT_VERBOSE) {
            appendStringInfoString(vacuumPrefix, "VERBOSE ");
            vacuumFlags &= ~VACOPT_VERBOSE;
        }

        return vacuumPrefix->data;
    }

    /* otherwise, handle options */
    appendStringInfoChar(vacuumPrefix, '(');

    if (vacuumFlags & VACOPT_ANALYZE) {
        appendStringInfoString(vacuumPrefix, "ANALYZE,");
    }

    if (vacuumFlags & VACOPT_FREEZE) {
        appendStringInfoString(vacuumPrefix, "FREEZE,");
    }

    if (vacuumFlags & VACOPT_FULL) {
        appendStringInfoString(vacuumPrefix, "FULL,");
    }

    if (vacuumFlags & VACOPT_VERBOSE) {
        appendStringInfoString(vacuumPrefix, "VERBOSE,");
    }

    vacuumPrefix->data[vacuumPrefix->len - 1] = ')';

    appendStringInfoChar(vacuumPrefix, ' ');

    return vacuumPrefix->data;
}

/*
 * DeparseVacuumColumnNames joins the list of strings using commas as a
 * delimiter. The whole thing is placed in parenthesis and set off with a
 * single space in order to facilitate appending it to the end of any VACUUM
 * or ANALYZE command which uses explicit column names. If the provided list
 * is empty, this function returns an empty string to keep the calling code
 * simplest.
 */
static char* DeparseVacuumColumnNames(List* columnNameList)
{
    StringInfo columnNames = makeStringInfo();

    if (columnNameList == NIL) {
        return columnNames->data;
    }

    appendStringInfoString(columnNames, " (");

    List* columnName = NULL;
    foreach_declared_ptr(columnName, columnNameList)
    {
        appendStringInfo(columnNames, "%s,", strVal(columnName));
    }

    columnNames->data[columnNames->len - 1] = ')';

    return columnNames->data;
}

/*
 * VacuumStmtParams returns a CitusVacuumParams based on the supplied VacuumStmt.
 */

/*
 * This is mostly ExecVacuum from Postgres's commands/vacuum.c
 * Note that ExecVacuum does an actual vacuum as well and we don't want
 * that to happen in the coordinator hence we copied the rest here.
 */
static CitusVacuumParams VacuumStmtParams(VacuumStmt* vacstmt)
{
    CitusVacuumParams params;
    params.options = vacstmt->options;
    return params;
}

/*
 * ExecuteUnqualifiedVacuumTasks executes tasks for unqualified vacuum commands
 */
static void ExecuteUnqualifiedVacuumTasks(VacuumStmt* vacuumStmt,
                                          CitusVacuumParams vacuumParams)
{
    /* don't allow concurrent node list changes that require an exclusive lock */
    List* workerNodes = TargetWorkerSetNodeList(ALL_SHARD_NODES, RowShareLock);

    if (list_length(workerNodes) == 0) {
        return;
    }

    const char* vacuumStringPrefix = DeparseVacuumStmtPrefix(vacuumParams);

    StringInfo vacuumCommand = makeStringInfo();
    appendStringInfoString(vacuumCommand, vacuumStringPrefix);

    List* unqualifiedVacuumCommands =
        list_make3(DISABLE_DDL_PROPAGATION, vacuumCommand->data, ENABLE_DDL_PROPAGATION);

    Task* task = CitusMakeNode(Task);
    task->jobId = INVALID_JOB_ID;
    task->taskType = VACUUM_ANALYZE_TASK;
    SetTaskQueryStringList(task, unqualifiedVacuumCommands);
    task->dependentTaskList = NULL;
    task->replicationModel = REPLICATION_MODEL_INVALID;
    task->cannotBeExecutedInTransction = ((vacuumParams.options) & VACOPT_VACUUM);

    bool hasPeerWorker = false;
    int32 localNodeGroupId = GetLocalGroupId();

    WorkerNode* workerNode = NULL;
    foreach_declared_ptr(workerNode, workerNodes)
    {
        if (workerNode->groupId != localNodeGroupId) {
            ShardPlacement* targetPlacement = CitusMakeNode(ShardPlacement);
            targetPlacement->nodeName = workerNode->workerName;
            targetPlacement->nodePort = workerNode->workerPort;
            targetPlacement->groupId = workerNode->groupId;

            task->taskPlacementList = lappend(task->taskPlacementList, targetPlacement);
            hasPeerWorker = true;
        }
    }

    if (hasPeerWorker) {
        bool localExecution = false;
        ExecuteUtilityTaskList(list_make1(task), localExecution);
    }
}
