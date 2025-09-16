/*-------------------------------------------------------------------------
 *
 * transaction_management.c
 *
 *   Transaction management for Citus.  Most of the work is delegated to other
 *   subsystems, this files, and especially CoordinatedTransactionCallback,
 *   coordinates the work between them.
 *
 * Copyright (c) Citus Data, Inc.
 *
 *-------------------------------------------------------------------------
 */

#include "postgres.h"

#include "libpq/libpq-fe.h"
#include "miscadmin.h"

#include "access/twophase.h"
#include "access/xact.h"
#include "catalog/dependency.h"
#include "nodes/print.h"
#include "storage/smgr/fd.h"
#include "utils/datum.h"
#include "utils/guc.h"
#include "utils/hsearch.h"
#include "utils/memutils.h"

#include "distributed/backend_data.h"
#include "distributed/citus_safe_lib.h"
#include "distributed/commands.h"
#include "distributed/connection_management.h"
#include "distributed/distributed_planner.h"
#include "distributed/function_call_delegation.h"
#include "distributed/hash_helpers.h"
#include "distributed/intermediate_results.h"
#include "distributed/listutils.h"
#include "distributed/local_executor.h"
#include "distributed/locally_reserved_shared_connections.h"
#include "distributed/maintenanced.h"
#include "distributed/metadata/dependency.h"
#include "distributed/metadata_cache.h"
#include "distributed/multi_executor.h"
#include "distributed/multi_explain.h"
#include "distributed/placement_connection.h"
#include "distributed/relation_access_tracking.h"
#include "distributed/repartition_join_execution.h"
#include "distributed/shard_cleaner.h"
#include "distributed/shared_connection_stats.h"
#include "distributed/subplan_execution.h"
#include "distributed/session_ctx.h"
#include "distributed/transaction_management.h"
#include "distributed/version_compat.h"
#include "distributed/worker_log_messages.h"

/* transaction management functions */
static void CoordinatedTransactionCallback(XactEvent event, void* arg);
static void CoordinatedSubTransactionCallback(SubXactEvent event, SubTransactionId subId,
                                              SubTransactionId parentSubid, void* arg);

/* remaining functions */
static void AdjustMaxPreparedTransactions(void);
static void PushSubXact(SubTransactionId subId);
static void PopSubXact(SubTransactionId subId, bool commit);
static void ResetGlobalVariables(void);
static bool SwallowErrors(void (*func)(void));
static void ForceAllInProgressConnectionsToClose(void);
static void EnsurePrepareTransactionIsAllowed(void);
static HTAB* CurrentTransactionPropagatedObjects(bool readonly);
static HTAB* ParentTransactionPropagatedObjects(bool readonly);
static void MovePropagatedObjectsToParentTransaction(void);
static bool DependencyInPropagatedObjectsHash(HTAB* propagatedObjects,
                                              const ObjectAddress* dependency);
static HTAB* CreateTxPropagatedObjectsHash(void);

void SessionTransactionCtx::InitializeTransCtx()
{
    InProgressTransactions = DLIST_STATIC_INIT(InProgressTransactions);

    /* hook into transaction machinery */
    RegisterXactCallback(CoordinatedTransactionCallback, NULL);
    RegisterSubXactCallback(CoordinatedSubTransactionCallback, NULL);

    /* set aside 8kb of memory for use in CoordinatedTransactionCallback */
    CitusXactCallbackContext = AllocSetContextCreate(
        u_sess->top_mem_cxt, "CitusXactCallbackContext", 8 * 1024, 8 * 1024, 8 * 1024);
}

/*
 * UseCoordinatedTransaction sets up the necessary variables to use
 * a coordinated transaction, unless one is already in progress.
 */
void UseCoordinatedTransaction(void)
{
    if (Session_ctx::Trans().CurrentCoordinatedTransactionState == COORD_TRANS_STARTED) {
        return;
    }

    if (Session_ctx::Trans().CurrentCoordinatedTransactionState != COORD_TRANS_NONE &&
        Session_ctx::Trans().CurrentCoordinatedTransactionState != COORD_TRANS_IDLE) {
        ereport(ERROR, (errmsg("starting transaction in wrong state")));
    }

    Session_ctx::Trans().CurrentCoordinatedTransactionState = COORD_TRANS_STARTED;

    /*
     * If assign_distributed_transaction_id() has been called, we should reuse
     * that identifier so distributed deadlock detection works properly.
     */
    DistributedTransactionId* transactionId = GetCurrentDistributedTransactionId();
    if (transactionId->transactionNumber == 0) {
        AssignDistributedTransactionId();
    }
}

/*
 * EnsureDistributedTransactionId makes sure that the current transaction
 * has a distributed transaction id. It is either assigned by a previous
 * call of assign_distributed_transaction_id(), or by starting a coordinated
 * transaction.
 */
void EnsureDistributedTransactionId(void)
{
    DistributedTransactionId* transactionId = GetCurrentDistributedTransactionId();
    if (transactionId->transactionNumber == 0) {
        UseCoordinatedTransaction();
    }
}

/*
 * InCoordinatedTransaction returns whether a coordinated transaction has been
 * started.
 */
bool InCoordinatedTransaction(void)
{
    return Session_ctx::Trans().CurrentCoordinatedTransactionState != COORD_TRANS_NONE &&
           Session_ctx::Trans().CurrentCoordinatedTransactionState != COORD_TRANS_IDLE;
}

/*
 * Use2PCForCoordinatedTransaction() signals that the current coordinated
 * transaction should use 2PC to commit.
 *
 * Note that even if 2PC is enabled, it is only used for connections that make
 * modification (DML or DDL).
 */
void Use2PCForCoordinatedTransaction(void)
{
    /*
     * If this transaction is also a coordinated
     * transaction, use 2PC. Otherwise, this
     * state change does nothing.
     *
     * In other words, when this flag is set,
     * we "should" use 2PC when needed (e.g.,
     * we are in a coordinated transaction and
     * the coordinated transaction does a remote
     * modification).
     */
    Session_ctx::Trans().ShouldCoordinatedTransactionUse2PC = true;
}

/*
 * GetCoordinatedTransactionShouldUse2PC is a wrapper function to read the value
 * of CoordinatedTransactionShouldUse2PCFlag.
 */
bool GetCoordinatedTransactionShouldUse2PC(void)
{
    return Session_ctx::Trans().ShouldCoordinatedTransactionUse2PC;
}

void InitializeTransactionManagement(void)
{
    AdjustMaxPreparedTransactions();
}

/*
 * Transaction management callback, handling coordinated transaction, and
 * transaction independent connection management.
 *
 * NB: There should only ever be a single transaction callback in citus, the
 * ordering between the callbacks and the actions within those callbacks
 * otherwise becomes too undeterministic / hard to reason about.
 */
static void CoordinatedTransactionCallback(XactEvent event, void* arg)
{
    switch (event) {
        case XACT_EVENT_COMMIT: {
            /*
             * ERRORs thrown during XACT_EVENT_COMMIT will cause postgres to abort, at
             * this point enough work has been done that it's not possible to rollback.
             *
             * One possible source of errors is memory allocation failures. To minimize
             * the chance of those happening we've pre-allocated some memory in the
             * CitusXactCallbackContext, it has 8kb of memory that we're allowed to use.
             *
             * We only do this in the COMMIT callback because:
             * - Errors thrown in other callbacks (such as PRE_COMMIT) won't cause
             *   crashes, they will simply cause the ABORT handler to be called.
             * - The exception is ABORT, errors thrown there could also cause crashes, but
             *   postgres already creates a TransactionAbortContext which performs this
             *   trick, so there's no need for us to do it again.
             */
            MemoryContext previousContext =
                MemoryContextSwitchTo(Session_ctx::Trans().CitusXactCallbackContext);

            if (Session_ctx::Trans().CurrentCoordinatedTransactionState ==
                COORD_TRANS_PREPARED) {
                /* handles both already prepared and open transactions */
                CoordinatedRemoteTransactionsCommit();
            }

            /* close connections etc. */
            if (Session_ctx::Trans().CurrentCoordinatedTransactionState !=
                COORD_TRANS_NONE) {
                ResetPlacementConnectionManagement();
                AfterXactConnectionHandling(true);
            }

            /*
             * Changes to catalog tables are now visible to the metadata sync
             * daemon, so we can trigger node metadata sync if necessary.
             */
            if (Session_ctx::Trans().NodeMetadataSyncOnCommit) {
                TriggerNodeMetadataSync(u_sess->proc_cxt.MyDatabaseId);
            }

            ResetGlobalVariables();
            ResetRelationAccessHash();
            ResetPropagatedObjects();

            /*
             * Make sure that we give the shared connections back to the shared
             * pool if any. This operation is a no-op if the reserved connections
             * are already given away.
             */
            DeallocateReservedConnections();

            UnSetDistributedTransactionId(Session_ctx::BackendCtx().MyBackendData);

            Session_ctx::UtilCtx().PlacementMovedUsingLogicalReplicationInTX = false;

            /* empty the CitusXactCallbackContext to ensure we're not leaking memory */
            MemoryContextSwitchTo(previousContext);
            MemoryContextReset(Session_ctx::Trans().CitusXactCallbackContext);

            /* Set CreateCitusTransactionLevel to 0 since original transaction is about to
             * be committed.
             */

            if (GetCitusCreationLevel() > 0) {
                /* Check CitusCreationLevel was correctly decremented to 1 */
                Assert(GetCitusCreationLevel() == 1);
                SetCreateCitusTransactionLevel(0);
            }
            break;
        }

        case XACT_EVENT_ABORT: {
            /* stop propagating notices from workers, we know the query is failed */
            DisableWorkerMessagePropagation();

            RemoveIntermediateResultsDirectories();

            /* handles both already prepared and open transactions */
            if (Session_ctx::Trans().CurrentCoordinatedTransactionState >
                COORD_TRANS_IDLE) {
                /*
                 * Since CoordinateRemoteTransactionsAbort may cause an error and it is
                 * not allowed to error out at that point, swallow the error if any.
                 *
                 * Particular error we've observed was CreateWaitEventSet throwing an
                 * error when out of file descriptor.
                 *
                 * If an error is swallowed, connections of all active transactions must
                 * be forced to close at the end of the transaction explicitly.
                 */
                bool errorSwallowed = SwallowErrors(CoordinatedRemoteTransactionsAbort);
                if (errorSwallowed == true) {
                    ForceAllInProgressConnectionsToClose();
                }
            }

            /*
             * Close connections etc. Contrary to a successful transaction we reset the
             * placement connection management irregardless of state of the statemachine
             * as recorded in CurrentCoordinatedTransactionState.
             * The hashmaps recording the connection management live a memory context
             * higher compared to most of the data referenced in the hashmap. This causes
             * use after free errors when the contents are retained due to an error caused
             * before the CurrentCoordinatedTransactionState changed.
             */
            ResetPlacementConnectionManagement();
            AfterXactConnectionHandling(false);

            ResetGlobalVariables();
            ResetRelationAccessHash();
            ResetPropagatedObjects();

            /* empty the CitusXactCallbackContext to ensure we're not leaking memory */
            MemoryContextReset(Session_ctx::Trans().CitusXactCallbackContext);

            /*
             * Clear MetadataCache table if we're aborting from a CREATE EXTENSION Citus
             * so that any created OIDs from the table are cleared and invalidated. We
             * also set CreateCitusTransactionLevel to 0 since that process has been
             * aborted
             */
            if (GetCitusCreationLevel() > 0) {
                /* Checks CitusCreationLevel correctly decremented to 1 */
                Assert(GetCitusCreationLevel() == 1);

                InvalidateMetadataSystemCache();
                SetCreateCitusTransactionLevel(0);
            }

            /*
             * Make sure that we give the shared connections back to the shared
             * pool if any. This operation is a no-op if the reserved connections
             * are already given away.
             */
            DeallocateReservedConnections();

            /*
             * We reset these mainly for posterity. The only way we would normally
             * get here with ExecutorLevel or PlannerLevel > 0 is during a fatal
             * error when the process is about to end.
             */
            Session_ctx::ExecCtx().ExecutorLevel = 0;
            Session_ctx::PlanCtx().PlannerLevel = 0;

            /*
             * We should reset SubPlanLevel in case a transaction is aborted,
             * otherwise this variable would stay +ve if the transaction is
             * aborted in the middle of a CTE/complex subquery execution
             * which would cause the subsequent queries to error out in
             * case the copy size is greater than
             * spq.max_intermediate_result_size
             */
            Session_ctx::ExecCtx().SubPlanLevel = 0;
            UnSetDistributedTransactionId(Session_ctx::BackendCtx().MyBackendData);

            Session_ctx::UtilCtx().PlacementMovedUsingLogicalReplicationInTX = false;
            break;
        }

        case XACT_EVENT_PREPARE: {
            /* we need to reset SavedExplainPlan before TopTransactionContext is deleted
             */
            FreeSavedExplainPlan();

            /*
             * This callback is only relevant for worker queries since
             * distributed queries cannot be executed with 2PC, see
             * XACT_EVENT_PRE_PREPARE.
             *
             * We should remove the intermediate results before unsetting the
             * distributed transaction id. That is necessary, otherwise Citus
             * would try to remove a non-existing folder and leak some of the
             * existing folders that are associated with distributed transaction
             * ids on the worker nodes.
             */
            RemoveIntermediateResultsDirectories();

            UnSetDistributedTransactionId(Session_ctx::BackendCtx().MyBackendData);
            break;
        }

        case XACT_EVENT_PRE_COMMIT: {
            /*
             * If the distributed query involves 2PC, we already removed
             * the intermediate result directory on XACT_EVENT_PREPARE. However,
             * if not, we should remove it here on the COMMIT. Since
             * RemoveIntermediateResultsDirectories() is idempotent, we're safe
             * to call it here again even if the transaction involves 2PC.
             */
            RemoveIntermediateResultsDirectories();

            /* nothing further to do if there's no managed remote xacts */
            if (Session_ctx::Trans().CurrentCoordinatedTransactionState ==
                COORD_TRANS_NONE) {
                break;
            }

            /*
             * TODO: It'd probably be a good idea to force constraints and
             * such to 'immediate' here. Deferred triggers might try to send
             * stuff to the remote side, which'd not be good.  Doing so
             * remotely would also catch a class of errors where committing
             * fails, which can lead to divergence when not using 2PC.
             */

            if (Session_ctx::Trans().ShouldCoordinatedTransactionUse2PC) {
                CoordinatedRemoteTransactionsPrepare();
                Session_ctx::Trans().CurrentCoordinatedTransactionState =
                    COORD_TRANS_PREPARED;

                /*
                 * Make sure we did not have any failures on connections marked as
                 * critical before committing.
                 */
                CheckRemoteTransactionsHealth();
            } else {
                CheckRemoteTransactionsHealth();

                /*
                 * Have to commit remote transactions in PRE_COMMIT, to allow
                 * us to mark failed placements as invalid.  Better don't use
                 * this for anything important (i.e. DDL/metadata).
                 */
                CoordinatedRemoteTransactionsCommit();
                Session_ctx::Trans().CurrentCoordinatedTransactionState =
                    COORD_TRANS_COMMITTED;
            }

            /*
             * Check again whether shards/placement successfully
             * committed. This handles failure at COMMIT time.
             */
            ErrorIfPostCommitFailedShardPlacements();
            break;
        }

        case XACT_EVENT_PRE_PREPARE: {
            EnsurePrepareTransactionIsAllowed();
            break;
        }

        /** skip all MOT opetions */
        case XACT_EVENT_START:
        case XACT_EVENT_RECORD_COMMIT:
        case XACT_EVENT_PREROLLBACK_CLEANUP:
        case XACT_EVENT_POST_COMMIT_CLEANUP:
        case XACT_EVENT_STMT_FINISH:
            break;

        case XACT_EVENT_END_TRANSACTION:
        case XACT_EVENT_COMMIT_PREPARED:
        case XACT_EVENT_ROLLBACK_PREPARED:
            break;
    }
}

/*
 * ForceAllInProgressConnectionsToClose forces all connections of in progress transactions
 * to close at the end of the transaction.
 */
static void ForceAllInProgressConnectionsToClose(void)
{
    dlist_iter iter;
    dlist_foreach(iter, &Session_ctx::Trans().InProgressTransactions)
    {
        MultiConnection* connection =
            dlist_container(MultiConnection, transactionNode, iter.cur);

        connection->forceCloseAtTransactionEnd = true;
    }
}

/*
 * If an ERROR is thrown while processing a transaction the ABORT handler is called.
 * ERRORS thrown during ABORT are not treated any differently, the ABORT handler is also
 * called during processing of those. If an ERROR was raised the first time through it's
 * unlikely that the second try will succeed; more likely that an ERROR will be thrown
 * again. This loop continues until Postgres notices and PANICs, complaining about a stack
 * overflow.
 *
 * Instead of looping and crashing, SwallowErrors lets us attempt to continue running the
 * ABORT logic. This wouldn't be safe in most other parts of the codebase, in
 * approximately none of the places where we emit ERROR do we first clean up after
 * ourselves! It's fine inside the ABORT handler though; Postgres is going to clean
 * everything up before control passes back to us.
 *
 * If it swallows any error, returns true. Otherwise, returns false.
 */
static bool SwallowErrors(void (*func)())
{
    MemoryContext savedContext = CurrentMemoryContext;
    volatile bool anyErrorSwallowed = false;

    PG_TRY();
    {
        func();
    }
    PG_CATCH();
    {
        MemoryContextSwitchTo(savedContext);
        ErrorData* edata = CopyErrorData();
        FlushErrorState();

        /* rethrow as WARNING */
        edata->elevel = WARNING;
        ThrowErrorData(edata);

        anyErrorSwallowed = true;
    }
    PG_END_TRY();

    return anyErrorSwallowed;
}

/*
 * ResetGlobalVariables resets global variables that
 * might be changed during the execution of queries.
 */
static void ResetGlobalVariables()
{
    Session_ctx::Trans().CurrentCoordinatedTransactionState = COORD_TRANS_NONE;
    Session_ctx::Trans().XactModificationLevel = XACT_MODIFICATION_NONE;
    SetLocalExecutionStatus(LOCAL_EXECUTION_OPTIONAL);
    FreeSavedExplainPlan();
    dlist_init(&Session_ctx::Trans().InProgressTransactions);
    Session_ctx::Trans().activeSetStmts = NULL;
    Session_ctx::Trans().ShouldCoordinatedTransactionUse2PC = false;
    Session_ctx::Trans().TransactionModifiedNodeMetadata = false;
    Session_ctx::Trans().NodeMetadataSyncOnCommit = false;
    Session_ctx::UtilCtx().InTopLevelDelegatedFunctionCall = false;
    Session_ctx::UtilCtx().InTableTypeConversionFunctionCall = false;
    Session_ctx::UtilCtx().CurrentOperationId = INVALID_OPERATION_ID;
    Session_ctx::Trans().BeginXactReadOnly = BeginXactReadOnly_NotSet;
    Session_ctx::Trans().BeginXactDeferrable = BeginXactDeferrable_NotSet;
    ResetWorkerErrorIndication();
    memset(&Session_ctx::Trans().AllowedDistributionColumnValue, 0,
           sizeof(AllowedDistributionColumn));
}

/*
 * CoordinatedSubTransactionCallback is the callback used to implement
 * distributed ROLLBACK TO SAVEPOINT.
 */
static void CoordinatedSubTransactionCallback(SubXactEvent event, SubTransactionId subId,
                                              SubTransactionId parentSubid, void* arg)
{
    switch (event) {
        /*
         * Our sub-transaction stack should be consistent with postgres' internal
         * transaction stack. In case of subxact begin, postgres calls our
         * callback after it has pushed the transaction into stack, so we have to
         * do the same even if worker commands fail, so we PushSubXact() first.
         * In case of subxact commit, callback is called before pushing subxact to
         * the postgres transaction stack, so we call PopSubXact() after making sure
         * worker commands didn't fail. Otherwise, Postgres would roll back that
         * would cause us to call PopSubXact again.
         */
        case SUBXACT_EVENT_START_SUB: {
            MemoryContext previousContext =
                MemoryContextSwitchTo(Session_ctx::Trans().CitusXactCallbackContext);

            PushSubXact(subId);
            if (InCoordinatedTransaction()) {
                CoordinatedRemoteTransactionsSavepointBegin(subId);
            }

            MemoryContextSwitchTo(previousContext);

            break;
        }

        case SUBXACT_EVENT_COMMIT_SUB: {
            MemoryContext previousContext =
                MemoryContextSwitchTo(Session_ctx::Trans().CitusXactCallbackContext);

            if (InCoordinatedTransaction()) {
                CoordinatedRemoteTransactionsSavepointRelease(subId);
            }
            PopSubXact(subId, true);

            /* Set CachedDuringCitusCreation to one level lower to represent citus
             * creation is done */

            if (GetCitusCreationLevel() == GetCurrentTransactionNestLevel()) {
                SetCreateCitusTransactionLevel(GetCitusCreationLevel() - 1);
            }

            MemoryContextSwitchTo(previousContext);

            break;
        }

        case SUBXACT_EVENT_ABORT_SUB: {
            MemoryContext previousContext =
                MemoryContextSwitchTo(Session_ctx::Trans().CitusXactCallbackContext);

            /*
             * Stop showing message for now, will re-enable when executing
             * the next statement.
             */
            DisableWorkerMessagePropagation();

            /*
             * Given that we aborted, worker error indications can be ignored.
             */
            ResetWorkerErrorIndication();

            if (InCoordinatedTransaction()) {
                CoordinatedRemoteTransactionsSavepointRollback(subId);
            }
            PopSubXact(subId, false);

            /*
             * Clear MetadataCache table if we're aborting from a CREATE EXTENSION Citus
             * so that any created OIDs from the table are cleared and invalidated. We
             * also set CreateCitusTransactionLevel to 0 since subtransaction has been
             * aborted
             */
            if (GetCitusCreationLevel() == GetCurrentTransactionNestLevel()) {
                InvalidateMetadataSystemCache();
                SetCreateCitusTransactionLevel(0);
            }

            MemoryContextSwitchTo(previousContext);

            break;
        }
        case SUBXACT_EVENT_CLEANUP_SUB: {
            /* nothing to do */
            break;
        }
    }
}

/*
 * AdjustMaxPreparedTransactions configures the number of available prepared
 * transaction slots at startup.
 */
static void AdjustMaxPreparedTransactions(void)
{
    /*
     * As Citus uses 2PC internally, there always should be some available. As
     * the default is 0, we increase it to something appropriate
     * (connections * 2 currently).  If the user explicitly configured 2PC, we
     * leave the configuration alone - there might have been intent behind the
     * decision.
     */
    if (g_instance.attr.attr_storage.max_prepared_xacts == 0) {
        char newvalue[12];

        SafeSnprintf(newvalue, sizeof(newvalue), "%d",
                     g_instance.attr.attr_network.MaxConnections * 2);

        SetConfigOption("max_prepared_transactions", newvalue, PGC_POSTMASTER,
                        PGC_S_OVERRIDE);

        ereport(LOG, (errmsg("number of prepared transactions has not been "
                             "configured, overriding"),
                      errdetail("max_prepared_transactions is now set to %s", newvalue)));
    }
}

/* PushSubXact pushes subId to the stack of active sub-transactions. */
static void PushSubXact(SubTransactionId subId)
{
    /* save provided subId as well as propagated SET LOCAL stmts */
    SubXactContext* state = static_cast<SubXactContext*>(palloc(sizeof(SubXactContext)));
    state->subId = subId;
    state->setLocalCmds = Session_ctx::Trans().activeSetStmts;

    /* we lazily create hashset when any object is propagated during sub-transaction */
    state->propagatedObjects = NULL;

    /* append to list and reset active set stmts for upcoming sub-xact */
    Session_ctx::Trans().activeSubXactContexts =
        lappend(Session_ctx::Trans().activeSubXactContexts, state);
    Session_ctx::Trans().activeSetStmts = makeStringInfo();
}

/* PopSubXact pops subId from the stack of active sub-transactions. */
static void PopSubXact(SubTransactionId subId, bool commit)
{
    SubXactContext* state =
        static_cast<SubXactContext*>(llast(Session_ctx::Trans().activeSubXactContexts));

    Assert(state->subId == subId);

    /*
     * Free activeSetStmts to avoid memory leaks when we create subxacts
     * for each row, e.g. in exception handling of UDFs.
     */
    if (Session_ctx::Trans().activeSetStmts != NULL) {
        pfree(Session_ctx::Trans().activeSetStmts->data);
        pfree(Session_ctx::Trans().activeSetStmts);
    }

    /*
     * SET LOCAL commands are local to subxact blocks. When a subxact commits
     * or rolls back, we should roll back our set of SET LOCAL commands to the
     * ones we had in the upper commit.
     */
    Session_ctx::Trans().activeSetStmts = state->setLocalCmds;

    /*
     * Keep subtransaction's propagated objects at parent transaction
     * if subtransaction committed. Otherwise, discard them.
     */
    if (commit) {
        MovePropagatedObjectsToParentTransaction();
    }
    hash_destroy(state->propagatedObjects);

    /*
     * Free state to avoid memory leaks when we create subxacts for each row,
     * e.g. in exception handling of UDFs.
     */
    pfree(state);

    Session_ctx::Trans().activeSubXactContexts =
        list_delete_last(Session_ctx::Trans().activeSubXactContexts);
}

/* ActiveSubXactContexts returns the list of active sub-xact context in temporal order. */
List* ActiveSubXactContexts(void)
{
    return Session_ctx::Trans().activeSubXactContexts;
}

/*
 * IsMultiStatementTransaction determines whether the current statement is
 * part of a bigger multi-statement transaction. This is the case when the
 * statement is wrapped in a transaction block (comes after BEGIN), or it
 * is called from a stored procedure or function.
 */
bool IsMultiStatementTransaction(void)
{
    if (IsTransactionBlock()) {
        /* in a BEGIN...END block */
        return true;
    } else if (Session_ctx::Trans().DoBlockLevel > 0) {
        /* in (a transaction within) a do block */
        return true;
    } else if (Session_ctx::Trans().StoredProcedureLevel > 0) {
        /* in (a transaction within) a stored procedure */
        return true;
    } else if (MaybeExecutingUDF() && Session_ctx::Vars().FunctionOpensTransactionBlock) {
        /* in a language-handler function call, open a transaction if configured to do so
         */
        return true;
    } else {
        return false;
    }
}

/*
 * MaybeExecutingUDF returns true if we are possibly executing a function call.
 * We use nested level of executor to check this, so this can return true for
 * CTEs, etc. which also start nested executors.
 *
 * If the planner is being called from the executor, then we may also be in
 * a UDF.
 */
bool MaybeExecutingUDF(void)
{
    return Session_ctx::ExecCtx().ExecutorLevel > 1 ||
           (Session_ctx::ExecCtx().ExecutorLevel == 1 &&
            Session_ctx::PlanCtx().PlannerLevel > 0);
}

/*
 * TriggerNodeMetadataSyncOnCommit sets a flag to do node metadata sync
 * on commit. This is because new metadata only becomes visible to the
 * metadata sync daemon after commit happens.
 */
void TriggerNodeMetadataSyncOnCommit(void)
{
    Session_ctx::Trans().NodeMetadataSyncOnCommit = true;
}

/*
 * Function raises an exception, if the current backend started a coordinated
 * transaction and got a PREPARE event to become a participant in a 2PC
 * transaction coordinated by another node.
 */
static void EnsurePrepareTransactionIsAllowed(void)
{
    if (!InCoordinatedTransaction()) {
        /* If the backend has not started a coordinated transaction. */
        return;
    }

    if (IsCitusInternalBackend()) {
        /*
         * If this is a Citus-initiated backend.
         */
        return;
    }

    ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                    errmsg("cannot use 2PC in transactions involving "
                           "multiple servers")));
}

/*
 * CurrentTransactionPropagatedObjects returns the objects propagated in current
 * sub-transaction or the root transaction if no sub-transaction exists.
 *
 * If the propagated objects are readonly it will not create the hashmap if it does not
 * already exist in the current sub-transaction.
 */
static HTAB* CurrentTransactionPropagatedObjects(bool readonly)
{
    if (Session_ctx::Trans().activeSubXactContexts == NIL) {
        /* hashset in the root transaction if there is no sub-transaction */
        if (Session_ctx::Trans().PropagatedObjectsInTx == NULL && !readonly) {
            /* lazily create hashset for root transaction, for mutating uses */
            Session_ctx::Trans().PropagatedObjectsInTx = CreateTxPropagatedObjectsHash();
        }
        return Session_ctx::Trans().PropagatedObjectsInTx;
    }

    /* hashset in top level sub-transaction */
    SubXactContext* state =
        static_cast<SubXactContext*>(llast(Session_ctx::Trans().activeSubXactContexts));
    if (state->propagatedObjects == NULL && !readonly) {
        /* lazily create hashset for sub-transaction, for mutating uses */
        state->propagatedObjects = CreateTxPropagatedObjectsHash();
    }
    return state->propagatedObjects;
}

/*
 * ParentTransactionPropagatedObjects returns the objects propagated in parent
 * transaction of active sub-transaction. It returns the root transaction if
 * no sub-transaction exists.
 *
 * If the propagated objects are readonly it will not create the hashmap if it does not
 * already exist in the target sub-transaction.
 */
static HTAB* ParentTransactionPropagatedObjects(bool readonly)
{
    int nestingLevel = list_length(Session_ctx::Trans().activeSubXactContexts);
    if (nestingLevel <= 1) {
        /*
         * The parent is the root transaction, when there is single level sub-transaction
         * or no sub-transaction.
         */
        if (Session_ctx::Trans().PropagatedObjectsInTx == NULL && !readonly) {
            /* lazily create hashset for root transaction, for mutating uses */
            Session_ctx::Trans().PropagatedObjectsInTx = CreateTxPropagatedObjectsHash();
        }
        return Session_ctx::Trans().PropagatedObjectsInTx;
    }

    /* parent is upper sub-transaction */
    Assert(nestingLevel >= 2);
    SubXactContext* state = static_cast<SubXactContext*>(
        list_nth(Session_ctx::Trans().activeSubXactContexts, nestingLevel - 2));
    if (state->propagatedObjects == NULL && !readonly) {
        /* lazily create hashset for parent sub-transaction */
        state->propagatedObjects = CreateTxPropagatedObjectsHash();
    }
    return state->propagatedObjects;
}

/*
 * MovePropagatedObjectsToParentTransaction moves all objects propagated in the current
 * sub-transaction to the parent transaction. This should only be called when there is
 * active sub-transaction.
 */
static void MovePropagatedObjectsToParentTransaction(void)
{
    Assert(llast(Session_ctx::Trans().activeSubXactContexts) != NULL);
    HTAB* currentPropagatedObjects = CurrentTransactionPropagatedObjects(true);
    if (currentPropagatedObjects == NULL) {
        /* nothing to move */
        return;
    }

    /*
     * Only after we know we have objects to move into the parent do we get a handle on
     * a guaranteed existing parent hash table. This makes sure that the parents only
     * get populated once there are objects to be tracked.
     */
    HTAB* parentPropagatedObjects = ParentTransactionPropagatedObjects(false);

    HASH_SEQ_STATUS propagatedObjectsSeq;
    hash_seq_init(&propagatedObjectsSeq, currentPropagatedObjects);
    ObjectAddress* objectAddress = NULL;
    while ((objectAddress = static_cast<ObjectAddress*>(
                hash_seq_search(&propagatedObjectsSeq))) != NULL) {
        hash_search(parentPropagatedObjects, objectAddress, HASH_ENTER, NULL);
    }
}

/*
 * DependencyInPropagatedObjectsHash checks if dependency is in given hashset
 * of propagated objects.
 */
static bool DependencyInPropagatedObjectsHash(HTAB* propagatedObjects,
                                              const ObjectAddress* dependency)
{
    if (propagatedObjects == NULL) {
        return false;
    }

    bool found = false;
    hash_search(propagatedObjects, dependency, HASH_FIND, &found);
    return found;
}

/*
 * CreateTxPropagatedObjectsHash creates a hashset to keep track of the objects
 * propagated in the current root transaction or sub-transaction.
 */
static HTAB* CreateTxPropagatedObjectsHash(void)
{
    HASHCTL info;
    memset(&info, 0, sizeof(info));
    info.keysize = sizeof(ObjectAddress);
    info.entrysize = sizeof(ObjectAddress);
    info.hash = tag_hash;
    info.hcxt = Session_ctx::Trans().CitusXactCallbackContext;

    int hashFlags = (HASH_ELEM | HASH_CONTEXT | HASH_FUNCTION);
    return hash_create("Tx Propagated Objects", 16, &info, hashFlags);
}

/*
 * TrackPropagatedObject adds given object into the objects propagated in the current
 * sub-transaction.
 */
void TrackPropagatedObject(const ObjectAddress* objectAddress)
{
    HTAB* currentPropagatedObjects = CurrentTransactionPropagatedObjects(false);
    hash_search(currentPropagatedObjects, objectAddress, HASH_ENTER, NULL);
}

/*
 * TrackPropagatedTableAndSequences adds given table and its sequences to the objects
 * propagated in the current sub-transaction.
 */
void TrackPropagatedTableAndSequences(Oid relationId)
{
    /* track table */
    ObjectAddress* tableAddress =
        static_cast<ObjectAddress*>(palloc0(sizeof(ObjectAddress)));
    ObjectAddressSet(*tableAddress, RelationRelationId, relationId);
    TrackPropagatedObject(tableAddress);

    /* track its sequences */
    List* ownedSeqIdList = getOwnedSequences(relationId);
    Oid ownedSeqId = InvalidOid;
    foreach_declared_oid(ownedSeqId, ownedSeqIdList)
    {
        ObjectAddress* seqAddress =
            static_cast<ObjectAddress*>(palloc0(sizeof(ObjectAddress)));
        ObjectAddressSet(*seqAddress, RelationRelationId, ownedSeqId);
        TrackPropagatedObject(seqAddress);
    }
}

/*
 * ResetPropagatedObjects destroys hashset of propagated objects in the root transaction.
 */
void ResetPropagatedObjects(void)
{
    hash_destroy(Session_ctx::Trans().PropagatedObjectsInTx);
    Session_ctx::Trans().PropagatedObjectsInTx = nullptr;
}

/*
 * HasAnyDependencyInPropagatedObjects decides if any dependency of given object is
 * propagated in the current transaction.
 */
bool HasAnyDependencyInPropagatedObjects(const ObjectAddress* objectAddress)
{
    List* dependencyList = GetAllSupportedDependenciesForObject(objectAddress);
    ObjectAddress* dependency = NULL;
    foreach_declared_ptr(dependency, dependencyList)
    {
        /* first search in root transaction */
        if (DependencyInPropagatedObjectsHash(Session_ctx::Trans().PropagatedObjectsInTx,
                                              dependency)) {
            return true;
        }

        /* search in all nested sub-transactions */
        if (Session_ctx::Trans().activeSubXactContexts == NIL) {
            continue;
        }
        SubXactContext* state = NULL;
        foreach_declared_ptr(state, Session_ctx::Trans().activeSubXactContexts)
        {
            if (DependencyInPropagatedObjectsHash(state->propagatedObjects, dependency)) {
                return true;
            }
        }
    }

    return false;
}
