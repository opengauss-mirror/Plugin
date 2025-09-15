/*-------------------------------------------------------------------------
 *
 * background_jobs.c
 *	  Background jobs run as a background worker, spawned from the
 *	  maintenance daemon. Jobs have tasks, tasks can depend on other
 *	  tasks before execution.
 *
 * This file contains the code for two separate background workers to
 * achieve the goal of running background tasks asynchronously from the
 * main database workload. This first background worker is the
 * Background Tasks Queue Monitor. This background worker keeps track of
 * tasks recorded in pg_dist_background_task and ensures execution based
 * on a statemachine. When a task needs to be executed it starts a
 * Background Task Executor that executes the sql statement defined in the
 * task. The output of the Executor is shared with the Monitor via a
 * shared memory queue.
 *
 * To make sure there is only ever exactly one monitor running per database
 * it takes an exclusive lock on the CITUS_BACKGROUND_TASK_MONITOR
 * operation. This lock is consulted from the maintenance daemon to only
 * spawn a new monitor when the lock is not held.
 *
 * Copyright (c) Citus Data, Inc.
 *
 *-------------------------------------------------------------------------
 */

#include "postgres.h"

#include "libpq/libpq-fe.h"
#include "pgstat.h"
#include "safe_mem_lib.h"

#include "access/xact.h"
#include "commands/dbcommands.h"
#ifdef DISABLE_OG_COMMENTS
#include "common/hashfn.h"
#endif
#include "libpq/pqformat.h"
#ifdef DISABLE_OG_COMMENTS
#include "libpq/pqmq.h"
#endif
#include "libpq/pqsignal.h"
#include "parser/analyze.h"
#ifdef DISABLE_OG_COMMENTS
#include "storage/dsm.h"
#endif
#include "storage/ipc.h"
#include "storage/procarray.h"
#ifdef DISABLE_OG_COMMENTS
#include "storage/shm_mq.h"
#include "storage/shm_toc.h"
#endif
#include "tcop/pquery.h"
#include "tcop/tcopprot.h"
#include "tcop/utility.h"
#ifdef DISABLE_OG_COMMENTS
#include "utils/fmgrprotos.h"
#endif
#include "utils/hsearch.h"
#include "utils/memutils.h"
#include "utils/portal.h"
#include "utils/ps_status.h"
#include "utils/resowner.h"
#include "utils/snapmgr.h"
#ifdef DISABLE_OG_COMMENTS
#include "utils/timeout.h"
#endif
#include "utils/builtins.h"
#include "distributed/background_jobs.h"
#include "distributed/citus_safe_lib.h"
#include "distributed/hash_helpers.h"
#include "distributed/listutils.h"
#include "distributed/maintenanced.h"
#include "distributed/metadata_cache.h"
#include "distributed/metadata_utility.h"
#include "distributed/resource_lock.h"
#include "distributed/shard_cleaner.h"
#include "distributed/shard_rebalancer.h"
#include "distributed/session_ctx.h"

using RawStmt = Node;
/* Table-of-contents constants for our dynamic shared memory segment. */
#define CITUS_BACKGROUND_TASK_MAGIC 0x51028081
#define CITUS_BACKGROUND_TASK_KEY_DATABASE 0
#define CITUS_BACKGROUND_TASK_KEY_USERNAME 1
#define CITUS_BACKGROUND_TASK_KEY_COMMAND 2
#define CITUS_BACKGROUND_TASK_KEY_QUEUE 3
#define CITUS_BACKGROUND_TASK_KEY_TASK_ID 4
#define CITUS_BACKGROUND_TASK_KEY_JOB_ID 5
#define CITUS_BACKGROUND_TASK_NKEYS 6

struct RebalanceShared {
    char* database;
    char* username;
    char* command;
    int64 taskId;
    int64 jobId;
};

/*
 * error_severity --- get localized string representing elevel
 */
static const char* error_severity(int elevel)
{
    const char* prefix = NULL;

    switch (elevel) {
        case DEBUG1:
        case DEBUG2:
        case DEBUG3:
        case DEBUG4:
        case DEBUG5:
            prefix = _("DEBUG");
            break;
        case LOG:
        case COMMERROR:
            prefix = _("LOG");
            break;
        case INFO:
            prefix = _("INFO");
            break;
        case NOTICE:
            prefix = _("NOTICE");
            break;
        case WARNING:
            prefix = _("WARNING");
            break;
        case ERROR:
            prefix = _("ERROR");
            break;
        case FATAL:
            prefix = _("FATAL");
            break;
        case PANIC:
            prefix = _("PANIC");
            break;
        default:
            prefix = "\?\?\?";
            break;
    }

    return prefix;
}

static BackgroundWorkerHandle* StartSpqBackgroundTaskExecutor(char* database, char* user,
                                                              char* command, int64 taskId,
                                                              int64 jobId);

static void UpdateDependingTasks(BackgroundTask* task);
static int64 CalculateBackoffDelay(int retryCount);
static bool NewExecutorExceedsCitusLimit(
    QueueMonitorExecutionContext* queueMonitorExecutionContext);

static bool AssignRunnableTaskToNewExecutor(
    BackgroundTask* runnableTask,
    QueueMonitorExecutionContext* queueMonitorExecutionContext);
static void AssignRunnableTasks(
    QueueMonitorExecutionContext* queueMonitorExecutionContext);
static List* GetRunningTaskEntries(HTAB* currentExecutors);

static void CheckAndResetLastWorkerAllocationFailure(
    QueueMonitorExecutionContext* queueMonitorExecutionContext);
static TaskExecutionStatus TaskConcurrentCancelCheck(
    TaskExecutionContext* taskExecutionContext);
static void TaskHadError(TaskExecutionContext* taskExecutionContext);
static void TaskEnded(TaskExecutionContext* taskExecutionContext);
static void TerminateAllTaskExecutors(HTAB* currentExecutors);
static HTAB* GetRunningUniqueJobIds(HTAB* currentExecutors);
static void CancelAllTaskExecutors(HTAB* currentExecutors);
static bool MonitorGotTerminationOrCancellationRequest();
static void QueueMonitorSigTermHandler(SIGNAL_ARGS);
static void QueueMonitorSigIntHandler(SIGNAL_ARGS);
static void QueueMonitorSigHupHandler(SIGNAL_ARGS);
static void DecrementParallelTaskCountForNodesInvolved(BackgroundTask* task);

/* flags set by signal handlers */
static THR_LOCAL volatile sig_atomic_t GotSigterm = false;
static THR_LOCAL volatile sig_atomic_t GotSigint = false;
static THR_LOCAL volatile sig_atomic_t GotSighup = false;

/* keeping track of parallel background tasks per node */
static THR_LOCAL HTAB* ParallelTasksPerNode = NULL;

PG_FUNCTION_INFO_V1(citus_job_cancel);
PG_FUNCTION_INFO_V1(citus_job_wait);
PG_FUNCTION_INFO_V1(citus_task_wait);

/*
 * pg_catalog.citus_job_cancel(jobid bigint) void
 *   cancels a scheduled/running job
 *
 * When cancelling a job there are two phases.
 *  1. scan all associated tasks and transition all tasks that are not already in their
 *     terminal state to cancelled. Except if the task is currently running.
 *  2. for all running tasks we send a cancelation signal to the backend running the
 *     query. The background executor/monitor will transition this task to cancelled.
 *
 * We apply the same policy checks as pg_cancel_backend to check if a user can cancel a
 * job.
 */
Datum citus_job_cancel(PG_FUNCTION_ARGS)
{
    CheckCitusVersion(ERROR);
    EnsureCoordinator();

    int64 jobid = PG_GETARG_INT64(0);

    /* Cancel all tasks that were scheduled before */
    List* pids = CancelTasksForJob(jobid);

    /* send cancellation to any running backends */
    int pid = 0;
    foreach_declared_int(pid, pids)
    {
        Datum pidDatum = Int32GetDatum(pid);
        Datum signalSuccessDatum = DirectFunctionCall1(pg_cancel_backend, pidDatum);
        bool signalSuccess = DatumGetBool(signalSuccessDatum);
        if (!signalSuccess) {
            ereport(WARNING, (errmsg("could not send signal to process %d: %m", pid)));
        }
    }

    UpdateBackgroundJob(jobid);

    PG_RETURN_VOID();
}

/*
 * pg_catalog.citus_job_wait(jobid bigint,
 *                            desired_status citus_job_status DEFAULT NULL) boolean
 *   waits till a job reaches a desired status, or can't reach the status anymore because
 *   it reached a (different) terminal state. When no desired_status is given it will
 *   assume any terminal state as its desired status. The function returns if the
 *   desired_state was reached.
 *
 * The current implementation is a polling implementation with an interval of 1 second.
 * Ideally we would have some synchronization between the background tasks queue monitor
 * and any backend calling this function to receive a signal when the job changes state.
 */
Datum citus_job_wait(PG_FUNCTION_ARGS)
{
    CheckCitusVersion(ERROR);
    EnsureCoordinator();

    int64 jobid = PG_GETARG_INT64(0);

    /* parse the optional desired_status argument */
    bool hasDesiredStatus = !PG_ARGISNULL(1);
    BackgroundJobStatus desiredStatus = {BACKGROUND_JOB_STATUS_SCHEDULED};
    if (hasDesiredStatus) {
        desiredStatus = BackgroundJobStatusByOid(PG_GETARG_OID(1));
    }

    citus_job_wait_internal(jobid, hasDesiredStatus ? &desiredStatus : NULL);

    PG_RETURN_VOID();
}

/*
 * pg_catalog.citus_task_wait(taskid bigint,
 *                            desired_status citus_task_status DEFAULT NULL) boolean
 *   waits till a task reaches a desired status, or can't reach the status anymore because
 *   it reached a (different) terminal state. When no desired_status is given it will
 *   assume any terminal state as its desired status. The function returns if the
 *   desired_state was reached.
 *
 * The current implementation is a polling implementation with an interval of 0.1 seconds.
 * Ideally we would have some synchronization between the background tasks queue monitor
 * and any backend calling this function to receive a signal when the task changes state.
 */
Datum citus_task_wait(PG_FUNCTION_ARGS)
{
    CheckCitusVersion(ERROR);
    EnsureCoordinator();

    int64 taskid = PG_GETARG_INT64(0);

    /* parse the optional desired_status argument */
    bool hasDesiredStatus = !PG_ARGISNULL(1);
    BackgroundTaskStatus desiredStatus = {BACKGROUND_TASK_STATUS_BLOCKED};
    if (hasDesiredStatus) {
        desiredStatus = BackgroundTaskStatusByOid(PG_GETARG_OID(1));
    }

    citus_task_wait_internal(taskid, hasDesiredStatus ? &desiredStatus : NULL);

    PG_RETURN_VOID();
}

/*
 * citus_job_wait_internal implements the waiting on a job for reuse in other areas where
 * we want to wait on jobs. eg the background rebalancer.
 *
 * When a desiredStatus is provided it will provide an error when a different state is
 * reached and the state cannot ever reach the desired state anymore.
 */
void citus_job_wait_internal(int64 jobid, BackgroundJobStatus* desiredStatus)
{
    /*
     * Since we are wait polling we will actually allocate memory on every poll. To make
     * sure we don't put unneeded pressure on the memory we create a context that we clear
     * every iteration.
     */
    MemoryContext waitContext = AllocSetContextCreate(
        CurrentMemoryContext, "JobsWaitContext", ALLOCSET_DEFAULT_MINSIZE,
        ALLOCSET_DEFAULT_INITSIZE, ALLOCSET_DEFAULT_MAXSIZE);
    MemoryContext oldContext = MemoryContextSwitchTo(waitContext);

    while (true) {
        MemoryContextReset(waitContext);

        BackgroundJob* job = GetBackgroundJobByJobId(jobid);
        if (!job) {
            ereport(ERROR, (errmsg("no job found for job with jobid: %ld", jobid)));
        }

        if (desiredStatus && job->state == *desiredStatus) {
            /* job has reached its desired status, done waiting */
            break;
        }

        if (IsBackgroundJobStatusTerminal(job->state)) {
            if (desiredStatus) {
                /*
                 * We have reached a terminal state, which is not the desired state we
                 * were waiting for, otherwise we would have escaped earlier. Since it is
                 * a terminal state we know that we can never reach the desired state.
                 */

                Oid reachedStatusOid = BackgroundJobStatusOid(job->state);
                Datum reachedStatusNameDatum =
                    DirectFunctionCall1(enum_out, reachedStatusOid);
                char* reachedStatusName = DatumGetCString(reachedStatusNameDatum);

                Oid desiredStatusOid = BackgroundJobStatusOid(*desiredStatus);
                Datum desiredStatusNameDatum =
                    DirectFunctionCall1(enum_out, desiredStatusOid);
                char* desiredStatusName = DatumGetCString(desiredStatusNameDatum);

                ereport(ERROR,
                        (errmsg("Job reached terminal state \"%s\" instead of desired "
                                "state \"%s\"",
                                reachedStatusName, desiredStatusName)));
            }

            /* job has reached its terminal state, done waiting */
            break;
        }

        /* sleep for a while, before rechecking the job status */
        CHECK_FOR_INTERRUPTS();
        const long delay_ms = 1000;
        (void)WaitLatch(&t_thrd.proc->procLatch,
                        WL_LATCH_SET | WL_TIMEOUT | WL_POSTMASTER_DEATH, delay_ms);

        ResetLatch(&t_thrd.proc->procLatch);
    }

    MemoryContextSwitchTo(oldContext);
    MemoryContextDelete(waitContext);
}

/*
 * citus_task_wait_internal implements the waiting on a task for reuse in other areas
 * where we want to wait on tasks.
 *
 * When a desiredStatus is provided it will provide an error when a different state is
 * reached and the state cannot ever reach the desired state anymore.
 */
void citus_task_wait_internal(int64 taskid, BackgroundTaskStatus* desiredStatus)
{
    /*
     * Since we are wait polling we will actually allocate memory on every poll. To make
     * sure we don't put unneeded pressure on the memory we create a context that we clear
     * every iteration.
     */
    MemoryContext waitContext = AllocSetContextCreate(
        CurrentMemoryContext, "TasksWaitContext", ALLOCSET_DEFAULT_MINSIZE,
        ALLOCSET_DEFAULT_INITSIZE, ALLOCSET_DEFAULT_MAXSIZE);
    MemoryContext oldContext = MemoryContextSwitchTo(waitContext);

    while (true) {
        MemoryContextReset(waitContext);

        BackgroundTask* task = GetBackgroundTaskByTaskId(taskid);
        if (!task) {
            ereport(ERROR, (errmsg("no task found with taskid: %ld", taskid)));
        }

        if (desiredStatus && task->status == *desiredStatus) {
            /* task has reached its desired status, done waiting */
            break;
        }

        if (IsBackgroundTaskStatusTerminal(task->status)) {
            if (desiredStatus) {
                /*
                 * We have reached a terminal state, which is not the desired state we
                 * were waiting for, otherwise we would have escaped earlier. Since it is
                 * a terminal state we know that we can never reach the desired state.
                 */

                Oid reachedStatusOid = BackgroundTaskStatusOid(task->status);
                Datum reachedStatusNameDatum =
                    DirectFunctionCall1(enum_out, reachedStatusOid);
                char* reachedStatusName = DatumGetCString(reachedStatusNameDatum);

                Oid desiredStatusOid = BackgroundTaskStatusOid(*desiredStatus);
                Datum desiredStatusNameDatum =
                    DirectFunctionCall1(enum_out, desiredStatusOid);
                char* desiredStatusName = DatumGetCString(desiredStatusNameDatum);

                ereport(ERROR,
                        (errmsg("Task reached terminal state \"%s\" instead of desired "
                                "state \"%s\"",
                                reachedStatusName, desiredStatusName)));
            }

            /* task has reached its terminal state, done waiting */
            break;
        }

        /* sleep for a while, before rechecking the task status */
        CHECK_FOR_INTERRUPTS();
        const long delay_ms = 100;
        (void)WaitLatch(&t_thrd.proc->procLatch,
                        WL_LATCH_SET | WL_TIMEOUT | WL_POSTMASTER_DEATH, delay_ms);

        ResetLatch(&t_thrd.proc->procLatch);
    }

    MemoryContextSwitchTo(oldContext);
    MemoryContextDelete(waitContext);
}

/*
 * context for any log/error messages emitted from the background task queue monitor.
 */
typedef struct CitusBackgroundTaskQueueMonitorErrorCallbackContext {
    const char* database;
} CitusBackgroundTaskQueueMonitorCallbackContext;

/*
 * CitusBackgroundTaskQueueMonitorErrorCallback is a callback handler that gets called for
 * any ereport to add extra context to the message.
 */
static void CitusBackgroundTaskQueueMonitorErrorCallback(void* arg)
{
    CitusBackgroundTaskQueueMonitorCallbackContext* context =
        (CitusBackgroundTaskQueueMonitorCallbackContext*)arg;
    errcontext("Citus Background Task Queue Monitor: %s", context->database);
}

/*
 * NewExecutorExceedsCitusLimit returns true if currently we reached Citus' max worker
 * count.
 */
static bool NewExecutorExceedsCitusLimit(
    QueueMonitorExecutionContext* queueMonitorExecutionContext)
{
    if (queueMonitorExecutionContext->currentExecutorCount >=
        Session_ctx::Vars().MaxBackgroundTaskExecutors) {
        /*
         * we hit to Citus' maximum task executor count. Warn for the first failure
         * after a successful worker allocation happened, that is, we do not warn if
         * we repeatedly come here without a successful worker allocation.
         */
        if (queueMonitorExecutionContext->backgroundWorkerFailedStartTime == 0) {
            ereport(WARNING,
                    (errmsg("unable to start background worker for "
                            "background task execution"),
                     errdetail("Already reached the maximum number of task "
                               "executors: %ld/%d",
                               queueMonitorExecutionContext->currentExecutorCount,
                               Session_ctx::Vars().MaxBackgroundTaskExecutors)));
            queueMonitorExecutionContext->backgroundWorkerFailedStartTime =
                GetCurrentTimestamp();
        }

        return true;
    }

    return false;
}

/*
 * AssignRunnableTaskToNewExecutor tries to assign given runnable task to a new task
 * executor. It reports the assignment status as return value.
 */
static bool AssignRunnableTaskToNewExecutor(
    BackgroundTask* runnableTask,
    QueueMonitorExecutionContext* queueMonitorExecutionContext)
{
    Assert(runnableTask && runnableTask->status == BACKGROUND_TASK_STATUS_RUNNABLE);

    if (NewExecutorExceedsCitusLimit(queueMonitorExecutionContext)) {
        /* escape if we hit citus executor limit */
        return false;
    }

    char* databaseName = get_database_name(MyDatabaseId);
    char* userName = GetUserNameFromId(runnableTask->owner);

    /* try to create new executor and make it alive during queue monitor lifetime */
    MemoryContext oldContext = MemoryContextSwitchTo(queueMonitorExecutionContext->ctx);

    BackgroundWorkerHandle* handle =
        StartSpqBackgroundTaskExecutor(databaseName, userName, runnableTask->command,
                                       runnableTask->taskid, runnableTask->jobid);
    if (handle == nullptr) {
        return false;
    }

    MemoryContextSwitchTo(oldContext);

    /* assign the allocated executor to the runnable task and increment total executor
     * count */
    bool handleEntryFound = false;
    BackgroundExecutorHashEntry* handleEntry = static_cast<BackgroundExecutorHashEntry*>(
        hash_search(queueMonitorExecutionContext->currentExecutors, &runnableTask->taskid,
                    HASH_ENTER, &handleEntryFound));
    Assert(!handleEntryFound);
    handleEntry->handle = handle;
    handleEntry->jobid = runnableTask->jobid;

    /* reset worker allocation timestamp and log time elapsed since the last failure */
    CheckAndResetLastWorkerAllocationFailure(queueMonitorExecutionContext);

    /* make message alive during queue monitor lifetime */
    oldContext = MemoryContextSwitchTo(queueMonitorExecutionContext->ctx);
    handleEntry->message = makeStringInfo();
    MemoryContextSwitchTo(oldContext);

    /* set runnable task's status as running */
    runnableTask->status = BACKGROUND_TASK_STATUS_RUNNING;
    UpdateBackgroundTask(runnableTask);
    UpdateBackgroundJob(runnableTask->jobid);

    queueMonitorExecutionContext->currentExecutorCount++;

    ereport(LOG, (errmsg("task jobid/taskid started: %ld/%ld", runnableTask->jobid,
                         runnableTask->taskid)));

    return true;
}

/*
 * AssignRunnableTasks tries to assign all runnable tasks to a new task executor.
 * If an assignment fails, it stops in case we hit some limitation. We do not load
 * all the runnable tasks in memory at once as it can load memory much + we have
 * limited worker to which we can assign task.
 */
static void AssignRunnableTasks(
    QueueMonitorExecutionContext* queueMonitorExecutionContext)
{
    BackgroundTask* runnableTask = NULL;
    bool taskAssigned = false;
    do {
        /* fetch a runnable task from catalog */
        runnableTask = GetRunnableBackgroundTask();
        if (runnableTask) {
            taskAssigned = AssignRunnableTaskToNewExecutor(runnableTask,
                                                           queueMonitorExecutionContext);
        } else {
            taskAssigned = false;
        }
    } while (taskAssigned);
}

/*
 * GetRunningTaskEntries returns list of BackgroundExecutorHashEntry from given hash table
 */
static List* GetRunningTaskEntries(HTAB* currentExecutors)
{
    List* runningTaskEntries = NIL;

    HASH_SEQ_STATUS status;
    BackgroundExecutorHashEntry* backgroundExecutorHashEntry;
    foreach_htab(backgroundExecutorHashEntry, &status, currentExecutors)
    {
        runningTaskEntries = lappend(runningTaskEntries, backgroundExecutorHashEntry);
    }

    return runningTaskEntries;
}

/*
 * CheckAndResetLastWorkerAllocationFailure checks the last time background worker
 * allocation is failed. If it is set, we print how long we have waited to successfully
 * allocate the worker. It also resets the failure timestamp.
 */
static void CheckAndResetLastWorkerAllocationFailure(
    QueueMonitorExecutionContext* queueMonitorExecutionContext)
{
    if (queueMonitorExecutionContext->backgroundWorkerFailedStartTime > 0) {
        /*
         * we had a delay in starting the background worker for task execution. Report
         * the actual delay and reset the time. This allows a subsequent task to
         * report again if it can't start a background worker directly.
         */
        long secs = 0;
        int microsecs = 0;
        TimestampDifference(queueMonitorExecutionContext->backgroundWorkerFailedStartTime,
                            GetCurrentTimestamp(), &secs, &microsecs);
        ereport(LOG, (errmsg("able to start a background worker with %ld seconds "
                             "delay",
                             secs)));

        queueMonitorExecutionContext->backgroundWorkerFailedStartTime = 0;
    }
}

/*
 * TaskConcurrentCancelCheck checks if concurrent task cancellation or removal happened by
 * taking Exclusive lock. It mutates task's pid and status. Returns execution status for
 * the task.
 */
static TaskExecutionStatus TaskConcurrentCancelCheck(
    TaskExecutionContext* taskExecutionContext)
{
    /*
     * here we take exclusive lock on pg_dist_background_task table to prevent a
     * concurrent modification. A separate process could have cancelled or removed
     * the task by now, they would not see the pid and status update, so it is our
     * responsibility to stop the backend and update the pid and status.
     *
     * The lock will release on transaction commit.
     */
    LockRelationOid(DistBackgroundTaskRelationId(), ExclusiveLock);

    BackgroundExecutorHashEntry* handleEntry = taskExecutionContext->handleEntry;
    BackgroundTask* task = GetBackgroundTaskByTaskId(handleEntry->taskid);
    taskExecutionContext->task = task;

    if (!task) {
        ereport(ERROR,
                (errmsg("task with taskid:%ld is not found", handleEntry->taskid)));
    }

    if (task->status == BACKGROUND_TASK_STATUS_CANCELLING) {
        /*
         * being in that step means that a concurrent cancel or removal happened. we
         * should mark task status as cancelled. We also want to reflect cancel message by
         * consuming task executor queue.
         */
        bool hadError = false;
        ereport(LOG, (errmsg("task jobid/taskid is cancelled: %ld/%ld", task->jobid,
                             task->taskid)));

        if (handleEntry->handle != nullptr) {
            BackgroundWorker* bgw =
                slist_container(BackgroundWorker, rw_lnode, handleEntry->handle);
            if (gs_signal_send(bgw->bgw_notify_pid, SIGTERM) != 0) {
                ereport(WARNING, (errmsg("Shutdown cancled rebalance task kill(pid %lu, "
                                         "stat %d, jobid/taskid  %ld/%ld) failed: %m",
                                         bgw->bgw_notify_pid, bgw->bgw_status,
                                         task->jobid, task->taskid)));
            }
        }

        task->status = BACKGROUND_TASK_STATUS_CANCELLED;

        return TASK_EXECUTION_STATUS_CANCELLED;
    } else {
        /*
         * now that we have verified the task has not been cancelled and still exist we
         * update it to reflect the new state. If task is already in running status,
         * the operation is idempotent. But for runnable tasks, we make their status
         * as running.
         */
        BackgroundWorker* bgw =
            slist_container(BackgroundWorker, rw_lnode, handleEntry->handle);
        pid_t pid = bgw->bgw_notify_pid;
        task->status = BACKGROUND_TASK_STATUS_RUNNING;
        SET_NULLABLE_FIELD(task, pid, pid);

        /* Update task status to indicate it is running */
        UpdateBackgroundTask(task);
        UpdateBackgroundJob(task->jobid);

        return TASK_EXECUTION_STATUS_RUNNING;
    }
}

/*
 * TaskHadError updates retry count of a failed task inside taskExecutionContext.
 * If maximum retry count is reached, task status is marked as failed. Otherwise, backoff
 * delay is calculated, notBefore time is updated and the task is marked as runnable.
 */
static void TaskHadError(TaskExecutionContext* taskExecutionContext)
{
    BackgroundTask* task = taskExecutionContext->task;

    /*
     * when we had an error in response queue, we need to decide if we want to retry (keep
     * the runnable state), or move to error state
     */

    if (!task->retry_count) {
        SET_NULLABLE_FIELD(task, retry_count, 1);
    } else {
        (*task->retry_count)++;
    }

    /*
     * based on the retry count we either transition the task to its error
     * state, or we calculate a new backoff time for future execution.
     */
    int64 delayMs = CalculateBackoffDelay(*(task->retry_count));
    if (delayMs < 0) {
        task->status = BACKGROUND_TASK_STATUS_ERROR;
        UNSET_NULLABLE_FIELD(task, not_before);
    } else {
        TimestampTz notBefore =
            TimestampTzPlusMilliseconds(GetCurrentTimestamp(), delayMs);
        SET_NULLABLE_FIELD(task, not_before, notBefore);

        task->status = BACKGROUND_TASK_STATUS_RUNNABLE;
    }

    TaskEnded(taskExecutionContext);
}

/*
 * TaskEnded updates task inside taskExecutionContext. It also updates depending
 * tasks and the job to which task belongs. At the end, it also updates executor map and
 * count inside queueMonitorExecutionContext after terminating the executor.
 */
static void TaskEnded(TaskExecutionContext* taskExecutionContext)
{
    QueueMonitorExecutionContext* queueMonitorExecutionContext =
        taskExecutionContext->queueMonitorExecutionContext;

    HTAB* currentExecutors = queueMonitorExecutionContext->currentExecutors;
    BackgroundExecutorHashEntry* handleEntry = taskExecutionContext->handleEntry;
    BackgroundTask* task = taskExecutionContext->task;

    /*
     * we update task and job fields. We also update depending jobs.
     * At the end, do cleanup.
     */
    UNSET_NULLABLE_FIELD(task, pid);
    task->message = handleEntry->message->data;

    UpdateBackgroundTask(task);
    UpdateDependingTasks(task);
    UpdateBackgroundJob(task->jobid);
    DecrementParallelTaskCountForNodesInvolved(task);

    /* we are sure that at least one task did not block on current iteration */
    queueMonitorExecutionContext->allTasksWouldBlock = false;

    handleEntry->handle == nullptr;
    hash_search(currentExecutors, &task->taskid, HASH_REMOVE, NULL);
}

/*
 * IncrementParallelTaskCountForNodesInvolved
 * Checks whether we have reached the limit of parallel tasks per node
 * per each of the nodes involved with the task
 * If at least one limit is reached, it returns false.
 * If limits aren't reached, it increments the parallel task count
 * for each of the nodes involved with the task, and returns true.
 */
bool IncrementParallelTaskCountForNodesInvolved(BackgroundTask* task)
{
    if (task->nodesInvolved) {
        int node;

        /* first check whether we have reached the limit for any of the nodes */
        foreach_declared_int(node, task->nodesInvolved)
        {
            bool found;
            ParallelTasksPerNodeEntry* hashEntry =
                static_cast<ParallelTasksPerNodeEntry*>(
                    hash_search(ParallelTasksPerNode, &(node), HASH_ENTER, &found));
            if (!found) {
                hashEntry->counter = 0;
            } else if (hashEntry->counter >=
                       Session_ctx::Vars().MaxBackgroundTaskExecutorsPerNode) {
                /* at least one node's limit is reached */
                return false;
            }
        }

        /* then, increment the parallel task count per each node */
        foreach_declared_int(node, task->nodesInvolved)
        {
            ParallelTasksPerNodeEntry* hashEntry =
                static_cast<ParallelTasksPerNodeEntry*>(
                    hash_search(ParallelTasksPerNode, &(node), HASH_FIND, NULL));
            Assert(hashEntry);
            hashEntry->counter += 1;
        }
    }

    return true;
}

/*
 * DecrementParallelTaskCountForNodesInvolved
 * Decrements the parallel task count for each of the nodes involved
 * with the task.
 * We call this function after the task has gone through Running state
 * and then has ended.
 */
static void DecrementParallelTaskCountForNodesInvolved(BackgroundTask* task)
{
    if (task->nodesInvolved) {
        int node;
        foreach_declared_int(node, task->nodesInvolved)
        {
            ParallelTasksPerNodeEntry* hashEntry =
                static_cast<ParallelTasksPerNodeEntry*>(
                    hash_search(ParallelTasksPerNode, &(node), HASH_FIND, NULL));

            hashEntry->counter -= 1;
        }
    }
}

/*
 * QueueMonitorSigHupHandler handles SIGHUP to update monitor related config params.
 */
static void QueueMonitorSigHupHandler(SIGNAL_ARGS)
{
    int saved_errno = errno;

    GotSighup = true;

    if (t_thrd.proc) {
        SetLatch(&t_thrd.proc->procLatch);
    }

    errno = saved_errno;
}

/*
 * MonitorGotTerminationOrCancellationRequest returns true if monitor had SIGTERM or
 * SIGINT signals
 */
static bool MonitorGotTerminationOrCancellationRequest()
{
    return GotSigterm || GotSigint;
}

/*
 * QueueMonitorSigTermHandler handles SIGTERM by setting a flag to inform the monitor
 * process so that it can terminate active task executors properly. It also sets the latch
 * to awake the monitor if it waits on it.
 */
static void QueueMonitorSigTermHandler(SIGNAL_ARGS)
{
    int saved_errno = errno;

    GotSigterm = true;

    if (t_thrd.proc) {
        SetLatch(&t_thrd.proc->procLatch);
    }

    errno = saved_errno;
}

/*
 * QueueMonitorSigIntHandler handles SIGINT by setting a flag to inform the monitor
 * process so that it can terminate active task executors properly. It also sets the latch
 * to awake the monitor if it waits on it.
 */
static void QueueMonitorSigIntHandler(SIGNAL_ARGS)
{
    int saved_errno = errno;

    GotSigint = true;

    if (t_thrd.proc) {
        SetLatch(&t_thrd.proc->procLatch);
    }

    errno = saved_errno;
}

/*
 * TerminateAllTaskExecutors terminates task executors given in the hash map.
 */
static void TerminateAllTaskExecutors(HTAB* currentExecutors)
{
    HASH_SEQ_STATUS status;
    BackgroundExecutorHashEntry* backgroundExecutorHashEntry;
    foreach_htab(backgroundExecutorHashEntry, &status, currentExecutors)
    {
#ifdef DISABLE_OG_COMMENTS
        TerminateBackgroundWorker(backgroundExecutorHashEntry->handle);
#endif
    }
}

/*
 * GetRunningUniqueJobIds returns unique job ids from currentExecutors
 */
static HTAB* GetRunningUniqueJobIds(HTAB* currentExecutors)
{
    /* create a set to store unique job ids for currently executing tasks */
    HTAB* uniqueJobIds = CreateSimpleHashSetWithSize(int64, MAX_BG_TASK_EXECUTORS);

    HASH_SEQ_STATUS status;
    BackgroundExecutorHashEntry* backgroundExecutorHashEntry;
    foreach_htab(backgroundExecutorHashEntry, &status, currentExecutors)
    {
        hash_search(uniqueJobIds, &backgroundExecutorHashEntry->jobid, HASH_ENTER, NULL);
    }

    return uniqueJobIds;
}

/*
 * CancelAllTaskExecutors cancels task executors given in the hash map.
 */
static void CancelAllTaskExecutors(HTAB* currentExecutors)
{
    StartTransactionCommand();
    PushActiveSnapshot(GetTransactionSnapshot());

    /* get unique job id set for running tasks in currentExecutors */
    HTAB* uniqueJobIds = GetRunningUniqueJobIds(currentExecutors);

    HASH_SEQ_STATUS status;
    int64* uniqueJobId;
    foreach_htab(uniqueJobId, &status, uniqueJobIds)
    {
        ereport(DEBUG1, (errmsg("cancelling job: %ld", *uniqueJobId)));
        Datum jobidDatum = Int64GetDatum(*uniqueJobId);
        DirectFunctionCall1(citus_job_cancel, jobidDatum);
    }

    PopActiveSnapshot();
    CommitTransactionCommand();
}

/*
 * CitusBackgroundTaskQueueMonitorMain is the main entry point for the background worker
 * running the background tasks queue monitor.
 *
 * It's mainloop reads a runnable task from pg_dist_background_task and progressing the
 * tasks and jobs state machines associated with the task. When no new task can be found
 * it will exit(0) and lets the maintenance daemon poll for new tasks.
 *
 * The main loop is implemented as asynchronous loop stepping through the task
 * and update its state before going to the next. Loop assigns runnable tasks to new task
 * executors as much as possible. If the max task executor limit is hit, the tasks will be
 * waiting in runnable status until currently running tasks finish. Each parallel worker
 * executes one task at a time without blocking each other by using nonblocking api.
 */
void CitusBackgroundTaskQueueMonitorMain(const BgWorkerContext* bwc)
{
    gs_signal_setmask(&t_thrd.libpq_cxt.BlockSig, NULL);

    /* handle SIGTERM to properly terminate active task executors */
    pqsignal(SIGTERM, QueueMonitorSigTermHandler);

    /* handle SIGINT to properly cancel active task executors */
    pqsignal(SIGINT, QueueMonitorSigIntHandler);

    /* handle SIGHUP to update MaxBackgroundTaskExecutors and
     * MaxBackgroundTaskExecutorsPerNode */
    pqsignal(SIGHUP, QueueMonitorSigHupHandler);

    gs_signal_setmask(&t_thrd.libpq_cxt.UnBlockSig, NULL);

    Oid databaseOid = *(Oid*)(bwc->bgshared);

    /* extension owner is passed via bgw_extra */
    Oid extensionOwner = InvalidOid;

    /*
     * save old context until monitor loop exits, we use backgroundTaskContext for
     * all allocations.
     */
    MemoryContext firstContext = CurrentMemoryContext;
    MemoryContext backgroundTaskContext = AllocSetContextCreate(
        TopMemoryContext, "BackgroundTaskContext", ALLOCSET_DEFAULT_SIZES);

    StartTransactionCommand();
    PushActiveSnapshot(GetTransactionSnapshot());

    const char* databasename = get_database_name(MyDatabaseId);

    /* make databasename alive during queue monitor lifetime */
    MemoryContext oldContext = MemoryContextSwitchTo(backgroundTaskContext);
    databasename = pstrdup(databasename);
    MemoryContextSwitchTo(oldContext);

    /* setup error context to indicate the errors came from a running background task */
    ErrorContextCallback errorCallback = {0};
    struct CitusBackgroundTaskQueueMonitorErrorCallbackContext context = {
        .database = databasename,
    };
    errorCallback.callback = CitusBackgroundTaskQueueMonitorErrorCallback;
    errorCallback.arg = (void*)&context;
    errorCallback.previous = t_thrd.log_cxt.error_context_stack;
    t_thrd.log_cxt.error_context_stack = &errorCallback;

    PopActiveSnapshot();
    CommitTransactionCommand();

    /*
     * There should be exactly one background task monitor running, running multiple would
     * cause conflicts on processing the tasks in the catalog table as well as violate
     * parallelism guarantees. To make sure there is at most, exactly one backend running
     * we take a session lock on the CITUS_BACKGROUND_TASK_MONITOR operation.
     */
    LOCKTAG tag = {0};
    SET_LOCKTAG_CITUS_OPERATION(tag, CITUS_BACKGROUND_TASK_MONITOR);
    const bool sessionLock = true;
    const bool dontWait = true;
    LockAcquireResult locked =
        LockAcquire(&tag, AccessExclusiveLock, sessionLock, dontWait);
    if (locked == LOCKACQUIRE_NOT_AVAIL) {
        ereport(ERROR, (errmsg("background task queue monitor already running for "
                               "database")));
    }

    /* make worker recognizable in pg_stat_activity */
    pgstat_report_appname("citus background task queue monitor");

    ereport(DEBUG1, (errmsg("started citus background task queue monitor")));

    /*
     * First we find all jobs that are running, we need to check if they are still running
     * if not reset their state back to scheduled.
     */
    StartTransactionCommand();
    PushActiveSnapshot(GetTransactionSnapshot());

    ResetRunningBackgroundTasks();

    PopActiveSnapshot();
    CommitTransactionCommand();

    /* create a map to store parallel task executors. Persist it in monitor memory context
     */
    oldContext = MemoryContextSwitchTo(backgroundTaskContext);
    HTAB* currentExecutors = CreateSimpleHashWithNameAndSize(
        int64, BackgroundExecutorHashEntry, "Background Executor Hash",
        MAX_BG_TASK_EXECUTORS);
    MemoryContextSwitchTo(oldContext);

    /*
     * monitor execution context that is useful during the monitor loop.
     * we store current executor count, last background failure timestamp,
     * currently executed task context and also a memory context to persist
     * some allocations throughout the loop.
     */
    QueueMonitorExecutionContext queueMonitorExecutionContext = {
        .currentExecutorCount = 0,
        .currentExecutors = currentExecutors,
        .backgroundWorkerFailedStartTime = 0,
        .allTasksWouldBlock = true,
        .ctx = backgroundTaskContext};

    /* flag to prevent duplicate termination and cancellation of task executors */
    bool terminateExecutorsStarted = false;
    bool cancelExecutorsStarted = false;
    StartTransactionCommand();
    PushActiveSnapshot(GetTransactionSnapshot());

    /* loop exits if there is no running or runnable tasks left */
    bool hasAnyTask = true;
    while (hasAnyTask) {
        /* handle signals */
        CHECK_FOR_INTERRUPTS();

        /* invalidate cache for new data in catalog */
        InvalidateMetadataSystemCache();

        /*
         * if the flag is set, we should terminate all task executor workers to prevent
         * duplicate runs of the same task on the next start of the monitor, which is
         * dangerous for non-idempotent tasks. We do not break the loop here as we want to
         * reflect tasks' messages. Hence, we wait until all tasks finish and also do not
         * allow new runnable tasks to start running. After all current tasks finish, we
         * can exit the loop safely.
         */
        if (GotSigterm && !terminateExecutorsStarted) {
            ereport(LOG, (errmsg("handling termination signal")));
            terminateExecutorsStarted = true;
            TerminateAllTaskExecutors(queueMonitorExecutionContext.currentExecutors);
        }

        if (GotSigint && !cancelExecutorsStarted) {
            ereport(LOG, (errmsg("handling cancellation signal")));
            cancelExecutorsStarted = true;
            CancelAllTaskExecutors(queueMonitorExecutionContext.currentExecutors);
        }

        if (GotSighup) {
            GotSighup = false;

            /* update max_background_task_executors and
             * max_background_task_executors_per_node if changed */
            ProcessConfigFile(PGC_SIGHUP);
        }

        if (ParallelTasksPerNode == NULL) {
            ParallelTasksPerNode = CreateSimpleHash(int32, ParallelTasksPerNodeEntry);
        }

        /* assign runnable tasks, if any, to new task executors in a transaction if we do
         * not have SIGTERM or SIGINT */
        if (!MonitorGotTerminationOrCancellationRequest()) {
            AssignRunnableTasks(&queueMonitorExecutionContext);
        }

        /* get running task entries from hash table */
        List* runningTaskEntries =
            GetRunningTaskEntries(queueMonitorExecutionContext.currentExecutors);
        hasAnyTask = list_length(runningTaskEntries) > 0;

        /* useful to sleep if all tasks ewouldblock on current iteration */
        queueMonitorExecutionContext.allTasksWouldBlock = true;

        /* iterate over all handle entries and monitor each task's output */
        BackgroundExecutorHashEntry* handleEntry = NULL;
        foreach_declared_ptr(handleEntry, runningTaskEntries)
        {
            /* create task execution context and assign it to queueMonitorExecutionContext
             */
            TaskExecutionContext taskExecutionContext = {
                .handleEntry = handleEntry,
                .task = NULL,
                .queueMonitorExecutionContext = &queueMonitorExecutionContext};

            /* check if concurrent cancellation occurred */
            TaskExecutionStatus taskExecutionStatus =
                TaskConcurrentCancelCheck(&taskExecutionContext);

            /*
             * check task status. If it is cancelled, we do not need to consume queue
             * as we already consumed the queue.
             */
            if (taskExecutionStatus == TASK_EXECUTION_STATUS_CANCELLED) {
                TaskEnded(&taskExecutionContext);
                continue;
            }

            /* Get task execution status */
            if (handleEntry->handle == nullptr) {
                continue;
            }

            BackgroundWorker* bgw =
                slist_container(BackgroundWorker, rw_lnode, handleEntry->handle);
            if (bgw->bgw_status == BGW_FAILED) {
                TaskHadError(&taskExecutionContext);
            } else if (bgw->bgw_status == BGW_TERMINATED) {
                taskExecutionContext.task->status = BACKGROUND_TASK_STATUS_DONE;
                TaskEnded(&taskExecutionContext);
            }
        }

        if (queueMonitorExecutionContext.allTasksWouldBlock) {
            /*
             * sleep to lower cpu consumption if all tasks responded with EWOULD_BLOCK on
             * the last iteration. That will also let those tasks to progress to generate
             * some output probably.
             */
            const long delay_ms = 1000;
            (void)WaitLatch(&t_thrd.proc->procLatch,
                            WL_LATCH_SET | WL_TIMEOUT | WL_POSTMASTER_DEATH, delay_ms);
            ResetLatch(&t_thrd.proc->procLatch);
        }
    }

    PopActiveSnapshot();
    CommitTransactionCommand();

    /* Quit and return all bgworkers launched by this thread. */
    BgworkerListSyncQuit();

    MemoryContextSwitchTo(firstContext);

    /* Cleanup and returen bgworkers */
    MemoryContextDelete(backgroundTaskContext);
}

/*
 * CalculateBackoffDelay calculates the time to backoff between retries.
 *
 * Per try we increase the delay as follows:
 *   retry 1: 5 sec
 *   retry 2: 20 sec
 *   retry 3-32 (30 tries in total): 1 min
 *
 * returns -1 when retrying should stop.
 *
 * In the future we would like a callback on the job_type that could
 * distinguish the retry count and delay + potential jitter on a
 * job_type basis. For now we only assume this to be used by the
 * rebalancer and settled on the retry scheme above.
 */
static int64 CalculateBackoffDelay(int retryCount)
{
    if (retryCount == 1) {
        return 5 * 1000;
    } else if (retryCount == 2) {
        return 20 * 1000;
    } else if (retryCount <= 32) {
        return 60 * 1000;
    }
    return -1;
}

/*
 * bgw_generate_returned_message -
 *      generates the message to be inserted into the job_run_details table
 *      first part is comming from error_severity (elog.c)
 */
static void bgw_generate_returned_message(StringInfoData* display_msg, ErrorData edata)
{
    const char* prefix = error_severity(edata.elevel);
    appendStringInfo(display_msg, "%s: %s", prefix, edata.message);
    if (edata.detail != NULL) {
        appendStringInfo(display_msg, "\nDETAIL: %s", edata.detail);
    }

    if (edata.hint != NULL) {
        appendStringInfo(display_msg, "\nHINT: %s", edata.hint);
    }

    if (edata.context != NULL) {
        appendStringInfo(display_msg, "\nCONTEXT: %s", edata.context);
    }
}

/*
 * UpdateDependingTasks updates all depending tasks, based on the type of terminal state
 * the current task reached.
 */
static void UpdateDependingTasks(BackgroundTask* task)
{
    switch (task->status) {
        case BACKGROUND_TASK_STATUS_DONE: {
            UnblockDependingBackgroundTasks(task);
            break;
        }

        case BACKGROUND_TASK_STATUS_ERROR: {
            /* when we error this task, we need to unschedule all dependant tasks */
            UnscheduleDependentTasks(task);
            break;
        }

        default: {
            /* nothing to do for other states */
            break;
        }
    }
}

/*
 * StartSpqBackgroundTaskExecutor start a new background worker for the execution of a
 * background task. Callers interested in the shared memory segment that is created
 * between the background worker and the current backend can pass in a segOut to get a
 * pointer to the dynamic shared memory.
 */
static BackgroundWorkerHandle* StartSpqBackgroundTaskExecutor(char* database, char* user,
                                                              char* command, int64 taskId,
                                                              int64 jobId)
{
    RebalanceShared* rbshared = (RebalanceShared*)palloc0(sizeof(RebalanceShared));

    rbshared->database = pstrdup(database);
    rbshared->username = pstrdup(user);
    rbshared->command = pstrdup(command);
    rbshared->taskId = taskId;
    rbshared->jobId = jobId;

    int started =
        LaunchBackgroundWorkers(1, rbshared, SpqBackgroundTaskExecutor, NULL, NULL);
    if (started != 1) {
        ereport(
            WARNING,
            (errmsg(
                "Spq Background Task Queue Executor start failed: %s/%s for (%ld/%ld)",
                database, user, jobId, taskId)));
        return nullptr;
    }

    return (BackgroundWorkerHandle*)slist_head_node(&t_thrd.bgworker_cxt.bgwlist);
}

/*
 * context for any log/error messages emitted from the background task executor.
 */
typedef struct CitusBackgroundJobExecutorErrorCallbackContext {
    const char* database;
    const char* username;
    int64 taskId;
    int64 jobId;
} CitusBackgroundJobExecutorErrorCallbackContext;

/*
 * CitusBackgroundJobExecutorErrorCallback is a callback handler that gets called for any
 * ereport to add extra context to the message.
 */
static void CitusBackgroundJobExecutorErrorCallback(void* arg)
{
    CitusBackgroundJobExecutorErrorCallbackContext* context =
        (CitusBackgroundJobExecutorErrorCallbackContext*)arg;
    errcontext("Citus Background Task Queue Executor: %s/%s for (%ld/%ld)",
               context->database, context->username, context->jobId, context->taskId);
}

/*
 * SpqBackgroundTaskExecutor is the main function of the background tasks queue
 * executor. This backend attaches to a shared memory segment as identified by the
 * main_arg of the background worker.
 *
 * This is mostly based on the background worker logic in pg_cron
 */
void SpqBackgroundTaskExecutor(const BgWorkerContext* bwc)
{
    RebalanceShared* shared = (RebalanceShared*)bwc->bgshared;

    /* make sure we are the only backend running for this task */
    LOCKTAG locktag = {0};
    SET_LOCKTAG_BACKGROUND_TASK(locktag, shared->taskId);
    const bool sessionLock = true;
    const bool dontWait = true;
    LockAcquireResult locked =
        LockAcquire(&locktag, AccessExclusiveLock, sessionLock, dontWait);
    if (locked == LOCKACQUIRE_NOT_AVAIL) {
        ereport(ERROR, (errmsg("unable to acquire background task lock for taskId: %ld",
                               shared->taskId),
                        errdetail("this indicates that an other backend is already "
                                  "executing this task")));
    }

    /* Execute the rebalance query. */
    ExecuteRebalancerCommandInSeparateTransaction(shared->command);

    ereport(LOG,
            (errmsg("task jobid/taskid done: %ld/%ld", shared->jobId, shared->taskId)));
}
