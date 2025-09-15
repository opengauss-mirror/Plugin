/*-------------------------------------------------------------------------
 *
 * worker_log_messages.c
 *   Logic for handling log messages from workers.
 *
 * Copyright (c) Citus Data, Inc.
 *
 *-------------------------------------------------------------------------
 */

#include "postgres.h"

#include "utils/elog.h"

#include "distributed/connection_management.h"
#include "distributed/error_codes.h"
#include "distributed/errormessage.h"
#include "distributed/log_utils.h"
#include "distributed/remote_commands.h"
#include "distributed/session_ctx.h"
#include "distributed/worker_log_messages.h"

/* list of log level names we might see from the worker */
static const char* LogLevelNames[] = {"DEBUG", "NOTICE", "INFO",  "WARNING",
                                      "ERROR", "FATAL",  "PANIC", NULL};

/* postgres log level values corresponding to LogLevelNames */
static const int LogLevels[] = {DEBUG1, NOTICE, INFO, WARNING, ERROR, FATAL, PANIC};

static void DefaultCitusNoticeReceiver(void* arg, const PGresult* result);
static int LogLevelNameToLogLevel(char* levelName);
static char* TrimLogLevel(const char* message);

/*
 * SetCitusNoticeReceiver sets the NoticeReceiver to DefaultCitusNoticeReceivere
 */
void SetCitusNoticeReceiver(MultiConnection* connection)
{
    PQsetNoticeReceiver(connection->pgConn, DefaultCitusNoticeReceiver, connection);
}

/*
 * EnableWorkerMessagePropagation indicates that we want to propagate messages
 * from workers to the client using the same log level.
 */
void EnableWorkerMessagePropagation(void)
{
    Session_ctx::UtilCtx().PreserveWorkerMessageLogLevel = true;
}

/*
 * DisableWorkerMessagePropagation indiciates that we want all messages from the
 * workers to only be sent to the client as debug messages.
 */
void DisableWorkerMessagePropagation(void)
{
    Session_ctx::UtilCtx().PreserveWorkerMessageLogLevel = false;
}

/*
 * DefaultCitusNoticeReceiver is used to redirect worker notices
 * from logfile to console.
 */
static void DefaultCitusNoticeReceiver(void* arg, const PGresult* result)
{
    MultiConnection* connection = (MultiConnection*)arg;
    char* nodeName = connection->hostname;
    uint32 nodePort = connection->port;
    char* message = PQresultErrorMessage(result);
    char* trimmedMessage = TrimLogLevel(message);
    char* levelName = PQresultErrorField(result, PG_DIAG_SEVERITY);
    int logLevel = LogLevelNameToLogLevel(levelName);
    int sqlState = ERRCODE_INTERNAL_ERROR;
    char* sqlStateString = PQresultErrorField(result, PG_DIAG_SQLSTATE);

    if (sqlStateString != NULL) {
        sqlState = MAKE_SQLSTATE(sqlStateString[0], sqlStateString[1], sqlStateString[2],
                                 sqlStateString[3], sqlStateString[4]);
    }

    /*
     * When read_intermediate_result cannot find a file it might mean that the
     * transaction that created the file already deleted it because it aborted.
     * That's an expected situation, unless there is no actual error. We
     * therefore store a DeferredError and raise it if we reach the end of
     * execution without errors.
     */
    if (sqlState == ERRCODE_CITUS_INTERMEDIATE_RESULT_NOT_FOUND && logLevel == WARNING) {
        if (Session_ctx::UtilCtx().WorkerErrorIndication == nullptr) {
            /* we'll at most need this for the lifetime of the transaction */
            MemoryContext oldContext =
                MemoryContextSwitchTo(u_sess->top_transaction_mem_cxt);

            Session_ctx::UtilCtx().WorkerErrorIndication =
                DeferredError(sqlState, pstrdup(trimmedMessage), NULL, NULL);

            MemoryContextSwitchTo(oldContext);
        }

        /* if we get the error we're expecting, the user does not need to know */
        logLevel = DEBUG4;
    }

    if (logLevel < Session_ctx::Vars().WorkerMinMessages ||
        Session_ctx::Vars().WorkerMinMessages == CITUS_LOG_LEVEL_OFF) {
        /* user does not want to see message */
        return;
    }

    if (!Session_ctx::UtilCtx().PreserveWorkerMessageLogLevel) {
        /*
         * We sometimes want to suppress notices (e.g. DROP TABLE cascading),
         * since the user already gets the relevant notices for the distributed
         * table. In that case, we change the log level to DEBUG1.
         */
        logLevel = DEBUG1;
    }

    ereport(logLevel, (errcode(sqlState), errmsg("%s", trimmedMessage),
                       errdetail("from %s:%d", nodeName, nodePort)));
}

/*
 * TrimLogLevel returns a copy of the string with the leading log level
 * and spaces removed such as
 *      From:
 *          INFO:  "normal2_102070": scanned 0 of 0 pages...
 *      To:
 *          "normal2_102070": scanned 0 of 0 pages...
 */
static char* TrimLogLevel(const char* message)
{
    char* chompedMessage = pchomp(message);

    size_t n = 0;
    while (n < strlen(chompedMessage) && chompedMessage[n] != ':') {
        n++;
    }

    do {
        n++;
    } while (n < strlen(chompedMessage) && chompedMessage[n] == ' ');

    return chompedMessage + n;
}

/*
 * LogLevelNameToLogLevel translates the prefix of Postgres log messages
 * back to a native log level.
 */
static int LogLevelNameToLogLevel(char* levelName)
{
    int levelIndex = 0;

    while (LogLevelNames[levelIndex] != NULL) {
        if (strcmp(levelName, LogLevelNames[levelIndex]) == 0) {
            return LogLevels[levelIndex];
        }

        levelIndex++;
    }

    return DEBUG1;
}

/*
 * ErrorIfWorkerErrorIndicationReceived throws the deferred error in
 * WorkerErrorIndication, if any.
 *
 * A fatal warning arrives to us as a WARNING in one session, that is triggered
 * by an ERROR in another session in the same distributed transaction. We therefore
 * do not expect to throw it, unless there is a bug in Citus.
 */
void ErrorIfWorkerErrorIndicationReceived(void)
{
    if (Session_ctx::UtilCtx().WorkerErrorIndication != NULL) {
        RaiseDeferredError(Session_ctx::UtilCtx().WorkerErrorIndication, ERROR);
    }
}

/*
 * ResetWorkerErrorIndication resets the fatal warning if one was received.
 */
void ResetWorkerErrorIndication(void)
{
    Session_ctx::UtilCtx().WorkerErrorIndication = NULL;
}
