/*-------------------------------------------------------------------------
 *
 * maintenanced.h
 *	  Background worker run for each citus using database in a postgres
 *    cluster.
 *
 * Copyright (c) Citus Data, Inc.
 *
 *-------------------------------------------------------------------------
 */

#ifndef MAINTENANCED_H
#define MAINTENANCED_H

#include "postmaster/bgworker.h"

/* collect statistics every 24 hours */
#define STATS_COLLECTION_TIMEOUT_MILLIS (24 * 60 * 60 * 1000)

/* if statistics collection fails, retry in 1 minute */
#define STATS_COLLECTION_RETRY_TIMEOUT_MILLIS (60 * 1000)

struct SessionDaemonCtx {
    /* set to true when becoming a maintenance daemon */
    bool IsMaintenanceDaemon{false};

    volatile sig_atomic_t got_SIGHUP{false};
    volatile sig_atomic_t got_SIGTERM{false};
};

extern void StopMaintenanceDaemon(Oid databaseId);
extern void TriggerNodeMetadataSync(Oid databaseId);
extern void MaintenanceDaemonShmemInit(void);
extern void InitializeMaintenanceDaemonBackend(void);
extern bool LockCitusExtension(void);

extern PGDLLEXPORT void CitusMaintenanceDaemonMain(const BgWorkerContext* bwc);

#endif /* MAINTENANCED_H */
