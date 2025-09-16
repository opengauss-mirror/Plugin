/*-------------------------------------------------------------------------
 *
 * stats_statements.h
 *    Statement-level statistics for distributed queries.
 *
 * Copyright (c) Citus Data, Inc.
 *-------------------------------------------------------------------------
 */

#ifndef QUERY_STATS_H
#define QUERY_STATS_H

#include "distributed/multi_server_executor.h"

#define STATS_SHARED_MEM_NAME "spq_query_stats"

extern Size SpqQueryStatsSharedMemSize(void);
extern void InitializeSpqQueryStats(void);
extern void SpqQueryStatsExecutorsEntry(uint64 uniqueSQLId,
                                        MultiExecutorType executorType,
                                        char* partitionKey);
extern void SpqQueryStatsSynchronizeEntries(void);
extern int StatStatementsMax;

typedef enum {
    STAT_STATEMENTS_TRACK_NONE = 0,
    STAT_STATEMENTS_TRACK_ALL = 1
} StatStatementsTrackType;

#endif /* QUERY_STATS_H */
