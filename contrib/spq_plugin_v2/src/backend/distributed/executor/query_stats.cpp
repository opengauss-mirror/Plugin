/*-------------------------------------------------------------------------
 *
 * query_stats.c
 *    Statement-level statistics for distributed queries.
 *    Code is mostly taken from postgres/contrib/pg_stat_statements
 *    and adapted to spq.
 *
 * Copyright (c) Citus Data, Inc.
 *-------------------------------------------------------------------------
 */

#include <unistd.h>

#include "postgres.h"

#include "funcapi.h"
#include "miscadmin.h"
#include "safe_lib.h"

#include "access/hash.h"
#include "access/tableam.h"
#include "catalog/pg_authid.h"
#include "storage/smgr/fd.h"
#include "storage/ipc.h"
#include "storage/spin.h"
#include "tcop/utility.h"
#include "utils/builtins.h"

#include "pg_version_constants.h"

#include "distributed/citus_safe_lib.h"
#include "distributed/function_utils.h"
#include "distributed/hash_helpers.h"
#include "distributed/memutils.h"
#include "distributed/multi_executor.h"
#include "distributed/multi_server_executor.h"
#include "distributed/query_stats.h"
#include "distributed/session_ctx.h"
#include "distributed/tuplestore.h"
#include "distributed/version_compat.h"
#include "distributed/utils/spq_lwlock.h"

#define SPQ_STATS_DUMP_FILE "pg_stat/spq_query_stats.stat"
#define SPQ_STAT_STATEMENTS_COLS 6
#define SPQ_STAT_STATAMENTS_UNIQUE_SQL_ID 0
#define SPQ_STAT_STATAMENTS_USER_ID 1
#define SPQ_STAT_STATAMENTS_NODE_ID 2
#define SPQ_STAT_STATAMENTS_EXECUTOR_TYPE 3
#define SPQ_STAT_STATAMENTS_PARTITION_KEY 4
#define SPQ_STAT_STATAMENTS_CALLS 5

#define USAGE_DECREASE_FACTOR (0.99)  /* decreased every SpqQueryStatsEntryDealloc */
#define STICKY_DECREASE_FACTOR (0.50) /* factor for sticky entries */
#define USAGE_DEALLOC_PERCENT 5       /* free this % of entries at once */
#define USAGE_INIT (1.0)              /* including initial planning */

#define MAX_KEY_LENGTH NAMEDATALEN

/* maximum number of entries in queryStats hash, controlled by GUC spq.stat_statements_max
 */
int StatStatementsMax = 50000;
static const uint32 SPQ_QUERY_STATS_FILE_HEADER = 0x0d756e0f;

/*
 * Hashtable key that defines the identity of a hashtable entry.  We use the
 * same hash as pg_stat_statements
 */
typedef struct QueryStatsHashKey {
    UniqueSQLKey key;
    MultiExecutorType executorType; /* executor type */
    char partitionKey[MAX_KEY_LENGTH];
} QueryStatsHashKey;

/*
 * Statistics per query and executor type
 */
typedef struct queryStatsEntry {
    QueryStatsHashKey key; /* hash key of entry - MUST BE FIRST */
    int64 calls;           /* # of times executed */
    double usage;          /* hashtable usage factor */
    slock_t mutex;         /* protects the counters only */
} QueryStatsEntry;

/*
 * Global shared state
 */
typedef struct QueryStatsSharedState {
    LWLock lock;             /* protects hashtable search/modification */
    double cur_median_usage; /* current median usage in hashtable */
} QueryStatsSharedState;

/* saved hook address in case of unload */
static THR_LOCAL shmem_startup_hook_type prev_shmem_startup_hook = NULL;

/* Links to shared memory state */
static QueryStatsSharedState* queryStats = NULL;
static THR_LOCAL HTAB* queryStatsHash = NULL;

/*--- Functions --- */

extern "C" Datum spq_query_stats_reset(PG_FUNCTION_ARGS);
extern "C" Datum spq_query_stats(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1(spq_stat_statements_reset);
PG_FUNCTION_INFO_V1(spq_query_stats);
PG_FUNCTION_INFO_V1(spq_executor_name);

extern "C" Datum spq_stat_statements_reset(PG_FUNCTION_ARGS);
extern "C" Datum spq_executor_name(PG_FUNCTION_ARGS);

static char* SpqExecutorName(MultiExecutorType executorType);

static void SpqQueryStatsShmemStartup(void);
static void SpqQueryStatsShmemShutdown(int code, Datum arg);
static QueryStatsEntry* SpqQueryStatsEntryAlloc(QueryStatsHashKey* key, bool sticky);
static void SpqQueryStatsEntryDealloc(void);
static void SpqQueryStatsEntryReset(void);
static uint32 SpqQuerysStatsHashFn(const void* key, Size keysize);
static int SpqQuerysStatsMatchFn(const void* key1, const void* key2, Size keysize);

static void SpqQueryStatsRemoveExpiredEntries();

void InitializeSpqQueryStats(void)
{
    /* Install hook */
    prev_shmem_startup_hook = t_thrd.storage_cxt.shmem_startup_hook;
    t_thrd.storage_cxt.shmem_startup_hook = SpqQueryStatsShmemStartup;
}

static void SpqQueryStatsShmemStartup(void)
{
    bool found;
    HASHCTL info;
    uint32 header;
    int32 num;
    QueryStatsEntry* buffer = NULL;

    if (prev_shmem_startup_hook) {
        prev_shmem_startup_hook();
    }

    /* Create or attach to the shared memory state */
    LWLockAcquire(AddinShmemInitLock, LW_EXCLUSIVE);

    /* global access lock */
    if (unlikely(queryStats == NULL)) {
        char* unaligned_buf = (char*)(ShmemInitStruct(
            STATS_SHARED_MEM_NAME,
            sizeof(QueryStatsSharedState) + spq::SHARE_MEM_ALIGN_SIZE, &found));
        Assert(!found);
        queryStats =
            (QueryStatsSharedState*)TYPEALIGN(spq::SHARE_MEM_ALIGN_SIZE, unaligned_buf);
        Spq::InitTrancheLwlock(Spq::QUERY_STAT_LWLOCK, &queryStats->lock);
    }

    memset(&info, 0, sizeof(info));
    info.keysize = sizeof(QueryStatsHashKey);
    info.entrysize = sizeof(QueryStatsEntry);
    info.hash = SpqQuerysStatsHashFn;
    info.match = SpqQuerysStatsMatchFn;

    /* allocate stats shared memory hash */
    queryStatsHash =
        ShmemInitHash("spq_query_stats hash", StatStatementsMax, StatStatementsMax, &info,
                      HASH_ELEM | HASH_FUNCTION | HASH_COMPARE);

    LWLockRelease(AddinShmemInitLock);

    if (!IsUnderPostmaster) {
        on_shmem_exit(SpqQueryStatsShmemShutdown, (Datum)0);
    }

    /*
     * Done if some other process already completed our initialization.
     */
    if (found) {
        return;
    }

    /* Load stat file, don't care about locking */
    FILE* file = AllocateFile(SPQ_STATS_DUMP_FILE, PG_BINARY_R);
    if (file == NULL) {
        if (errno == ENOENT) {
            return; /* ignore not-found error */
        }
        goto error;
    }

    /* check is header is valid */
    if (fread(&header, sizeof(uint32), 1, file) != 1 ||
        header != SPQ_QUERY_STATS_FILE_HEADER) {
        goto error;
    }

    /* get number of entries */
    if (fread(&num, sizeof(int32), 1, file) != 1) {
        goto error;
    }

    for (int i = 0; i < num; i++) {
        QueryStatsEntry temp;

        if (fread(&temp, sizeof(QueryStatsEntry), 1, file) != 1) {
            goto error;
        }

        /* Skip loading "sticky" entries */
        if (temp.calls == 0) {
            continue;
        }

        QueryStatsEntry* entry = SpqQueryStatsEntryAlloc(&temp.key, false);

        /* copy in the actual stats */
        entry->calls = temp.calls;
        entry->usage = temp.usage;

        /* don't initialize spinlock, already done */
    }

    FreeFile(file);

    /*
     * Remove the file so it's not included in backups/replicas, etc. A new file will be
     * written on next shutdown.
     */
    unlink(SPQ_STATS_DUMP_FILE);

    return;

error:
    ereport(LOG, (errcode_for_file_access(),
                  errmsg("could not read spq_query_stats file \"%s\": %m",
                         SPQ_STATS_DUMP_FILE)));
    if (buffer) {
        pfree(buffer);
    }
    if (file) {
        FreeFile(file);
    }

    /* delete bogus file, don't care of errors in this case */
    unlink(SPQ_STATS_DUMP_FILE);
}

/*
 * SpqQueryStatsShmemShutdown is a shmem_shutdown hook,
 * it dumps statistics into file.
 */
static void SpqQueryStatsShmemShutdown(int code, Datum arg)
{
    if (t_thrd.role != MASTER_THREAD) {
        return;
    }

    HASH_SEQ_STATUS hash_seq;
    QueryStatsEntry* entry;

    /* Don't try to dump during a crash. */
    if (code) {
        return;
    }

    if (!queryStats) {
        return;
    }

    int32 num_entries = hash_get_num_entries(queryStatsHash);
    FILE* file = AllocateFile(SPQ_STATS_DUMP_FILE ".tmp", PG_BINARY_W);
    if (file == NULL) {
        goto error;
    }

    if (fwrite(&SPQ_QUERY_STATS_FILE_HEADER, sizeof(uint32), 1, file) != 1) {
        goto error;
    }

    if (fwrite(&num_entries, sizeof(int32), 1, file) != 1) {
        goto error;
    }

    hash_seq_init(&hash_seq, queryStatsHash);
    while ((entry = static_cast<QueryStatsEntry*>(hash_seq_search(&hash_seq))) != NULL) {
        if (fwrite(entry, sizeof(QueryStatsEntry), 1, file) != 1) {
            /* note: we assume hash_seq_term won't change errno */
            hash_seq_term(&hash_seq);
            goto error;
        }
    }

    if (FreeFile(file)) {
        file = NULL;
        goto error;
    }

    /*
     * Rename file inplace
     */
    if (rename(SPQ_STATS_DUMP_FILE ".tmp", SPQ_STATS_DUMP_FILE) != 0) {
        ereport(LOG, (errcode_for_file_access(),
                      errmsg("could not rename spq_query_stats file \"%s\": %m",
                             SPQ_STATS_DUMP_FILE ".tmp")));
    }

    return;

error:
    ereport(LOG, (errcode_for_file_access(),
                  errmsg("could not read spq_query_stats file \"%s\": %m",
                         SPQ_STATS_DUMP_FILE)));

    if (file) {
        FreeFile(file);
    }
    unlink(SPQ_STATS_DUMP_FILE);
}

/*
 * SpqQueryStatsSharedMemSize calculates and returns shared memory size
 * required to keep query statistics.
 */
Size SpqQueryStatsSharedMemSize(void)
{
    Assert(StatStatementsMax >= 0);

    Size size = MAXALIGN(sizeof(QueryStatsSharedState));
    size = add_size(size, hash_estimate_size(StatStatementsMax, sizeof(QueryStatsEntry)));

    return size;
}

/*
 * SpqQueryStatsExecutorsEntry is the function to update statistics
 * for a given query id.
 */
void SpqQueryStatsExecutorsEntry(uint64 uniqueSQLId, MultiExecutorType executorType,
                                 char* partitionKey)
{
    QueryStatsHashKey key;

    /* Safety check... */
    if (!queryStats || !queryStatsHash) {
        return;
    }

    /* early return if tracking is disabled */
    if (!Session_ctx::Vars().StatStatementsTrack) {
        return;
    }

    /* Set up key for hashtable search */
    key.key.user_id = GetUserId();
    key.key.cn_id = u_sess->unique_sql_cxt.unique_sql_cn_id;
    key.key.unique_sql_id = uniqueSQLId;
    key.executorType = executorType;
    memset(key.partitionKey, 0, MAX_KEY_LENGTH);
    if (partitionKey != NULL) {
        strlcpy(key.partitionKey, partitionKey, MAX_KEY_LENGTH);
    }

    /* Lookup the hash table entry with shared lock. */
    LWLockAcquire(&queryStats->lock, LW_SHARED);

    QueryStatsEntry* entry =
        (QueryStatsEntry*)hash_search(queryStatsHash, &key, HASH_FIND, NULL);

    /* Create new entry, if not present */
    if (!entry) {
        /* Need exclusive lock to make a new hashtable entry - promote */
        LWLockRelease(&queryStats->lock);
        LWLockAcquire(&queryStats->lock, LW_EXCLUSIVE);

        /* OK to create a new hashtable entry */
        entry = SpqQueryStatsEntryAlloc(&key, false);
    }

    /*
     * Grab the spinlock while updating the counters (see comment about
     * locking rules at the head of the pg_stat_statements file)
     */
    volatile QueryStatsEntry* e = (volatile QueryStatsEntry*)entry;

    SpinLockAcquire(&e->mutex);

    /* "Unstick" entry if it was previously sticky */
    if (e->calls == 0) {
        e->usage = USAGE_INIT;
    }

    e->calls += 1;

    SpinLockRelease(&e->mutex);

    LWLockRelease(&queryStats->lock);
}

/*
 * Allocate a new hashtable entry.
 * caller must hold an exclusive lock on queryStats->lock
 */
static QueryStatsEntry* SpqQueryStatsEntryAlloc(QueryStatsHashKey* key, bool sticky)
{
    bool found;
    long StatStatementsMaxLong = StatStatementsMax;

    /* Make space if needed */
    while (hash_get_num_entries(queryStatsHash) >= StatStatementsMaxLong) {
        SpqQueryStatsEntryDealloc();
    }

    /* Find or create an entry with desired hash code */
    QueryStatsEntry* entry =
        (QueryStatsEntry*)hash_search(queryStatsHash, key, HASH_ENTER, &found);

    if (!found) {
        /* New entry, initialize it */

        /* set the appropriate initial usage count */
        entry->usage = sticky ? queryStats->cur_median_usage : USAGE_INIT;

        /* re-initialize the mutex each time ... we assume no one using it */
        SpinLockInit(&entry->mutex);
    }

    entry->calls = 0;
    entry->usage = (0.0);

    return entry;
}

/*
 * entry_cmp is qsort comparator for sorting into increasing usage order
 */
static int entry_cmp(const void* lhs, const void* rhs)
{
    double l_usage = (*(QueryStatsEntry* const*)lhs)->usage;
    double r_usage = (*(QueryStatsEntry* const*)rhs)->usage;

    if (l_usage < r_usage) {
        return -1;
    } else if (l_usage > r_usage) {
        return +1;
    } else {
        return 0;
    }
}

/*
 * SpqQueryStatsEntryDealloc deallocates least used entries.
 * Caller must hold an exclusive lock on queryStats->lock.
 */
static void SpqQueryStatsEntryDealloc(void)
{
    HASH_SEQ_STATUS hash_seq;
    QueryStatsEntry* entry;

    /*
     * Sort entries by usage and deallocate USAGE_DEALLOC_PERCENT of them.
     * While we're scanning the table, apply the decay factor to the usage
     * values.
     */
    QueryStatsEntry** entries = static_cast<QueryStatsEntry**>(
        palloc(hash_get_num_entries(queryStatsHash) * sizeof(QueryStatsEntry*)));

    int i = 0;
    hash_seq_init(&hash_seq, queryStatsHash);
    while ((entry = static_cast<QueryStatsEntry*>(hash_seq_search(&hash_seq))) != NULL) {
        entries[i++] = entry;

        /* "Sticky" entries get a different usage decay rate. */
        if (entry->calls == 0) {
            entry->usage *= STICKY_DECREASE_FACTOR;
        } else {
            entry->usage *= USAGE_DECREASE_FACTOR;
        }
    }

    SafeQsort(entries, i, sizeof(QueryStatsEntry*), entry_cmp);

    if (i > 0) {
        /* Record the (approximate) median usage */
        queryStats->cur_median_usage = entries[i / 2]->usage;
    }

    int nvictims = Max(10, i * USAGE_DEALLOC_PERCENT / 100);
    nvictims = Min(nvictims, i);

    for (i = 0; i < nvictims; i++) {
        hash_search(queryStatsHash, &entries[i]->key, HASH_REMOVE, NULL);
    }

    pfree(entries);
}

/*
 * SpqQueryStatsEntryReset resets statistics.
 */
static void SpqQueryStatsEntryReset(void)
{
    HASH_SEQ_STATUS hash_seq;
    QueryStatsEntry* entry;

    LWLockAcquire(&queryStats->lock, LW_EXCLUSIVE);

    hash_seq_init(&hash_seq, queryStatsHash);
    while ((entry = static_cast<QueryStatsEntry*>(hash_seq_search(&hash_seq))) != NULL) {
        hash_search(queryStatsHash, &entry->key, HASH_REMOVE, NULL);
    }

    LWLockRelease(&queryStats->lock);
}

/*
 * SpqQuerysStatsHashFn calculates and returns hash value for a key
 */
static uint32 SpqQuerysStatsHashFn(const void* key, Size keysize)
{
    const QueryStatsHashKey* k = (const QueryStatsHashKey*)key;

    if (k->partitionKey[0] != '\0') {
        return hash_uint32((uint32)k->key.user_id) ^ hash_uint32((uint32)k->key.cn_id) ^
               hash_any((const unsigned char*)&(k->key.unique_sql_id), sizeof(uint64)) ^
               hash_uint32((uint32)k->executorType) ^
               hash_any((const unsigned char*)(k->partitionKey), strlen(k->partitionKey));
    } else {
        return hash_uint32((uint32)k->key.user_id) ^ hash_uint32((uint32)k->key.cn_id) ^
               hash_any((const unsigned char*)&(k->key.unique_sql_id), sizeof(uint64)) ^
               hash_uint32((uint32)k->executorType);
    }
}

/*
 * SpqQuerysStatsMatchFn compares two keys - zero means match.
 * See definition of HashCompareFunc in hsearch.h for more info.
 */
static int SpqQuerysStatsMatchFn(const void* key1, const void* key2, Size keysize)
{
    const QueryStatsHashKey* k1 = (const QueryStatsHashKey*)key1;
    const QueryStatsHashKey* k2 = (const QueryStatsHashKey*)key2;

    if (k1->key.user_id == k2->key.user_id && k1->key.cn_id == k2->key.cn_id &&
        k1->key.unique_sql_id == k2->key.unique_sql_id &&
        k1->executorType == k2->executorType) {
        return 0;
    }
    return 1;
}

/*
 * Reset statistics.
 */
Datum spq_stat_statements_reset(PG_FUNCTION_ARGS)
{
    SpqQueryStatsEntryReset();
    PG_RETURN_VOID();
}

/*
 * spq_query_stats returns query stats kept in memory.
 */
Datum spq_query_stats(PG_FUNCTION_ARGS)
{
    TupleDesc tupdesc;
    HASH_SEQ_STATUS hash_seq;
    QueryStatsEntry* entry;
    Oid currentUserId = GetUserId();

    if (!queryStats) {
        ereport(ERROR, (errcode(ERRCODE_OBJECT_NOT_IN_PREREQUISITE_STATE),
                        errmsg("spq_query_stats: shared memory not initialized")));
    }

    bool canSeeStats = superuser() || isMonitoradmin(currentUserId);

    Tuplestorestate* tupstore = SetupTuplestore(fcinfo, &tupdesc);

    /* exclusive lock on queryStats->lock is acquired and released inside the function */
    SpqQueryStatsSynchronizeEntries();

    LWLockAcquire(&queryStats->lock, LW_SHARED);

    hash_seq_init(&hash_seq, queryStatsHash);
    while ((entry = static_cast<QueryStatsEntry*>(hash_seq_search(&hash_seq))) != NULL) {
        Datum values[SPQ_STAT_STATEMENTS_COLS];
        bool nulls[SPQ_STAT_STATEMENTS_COLS];

        /* following vars are to keep data for processing after spinlock release */
        uint64 unique_sql_id = 0;
        Oid userid = InvalidOid;
        uint32 nodeid = InvalidOid;
        MultiExecutorType executorType = MULTI_EXECUTOR_INVALID_FIRST;
        char partitionKey[MAX_KEY_LENGTH];
        int64 calls = 0;

        memset(values, 0, sizeof(values));
        memset(nulls, 0, sizeof(nulls));
        memset(partitionKey, 0, MAX_KEY_LENGTH);

        SpinLockAcquire(&entry->mutex);

        /*
         * Skip entry if unexecuted (ie, it's a pending "sticky" entry) or
         * the user does not have permission to view it.
         */
        if (entry->calls == 0 ||
            !(currentUserId == entry->key.key.user_id || canSeeStats)) {
            SpinLockRelease(&entry->mutex);
            continue;
        }

        unique_sql_id = entry->key.key.unique_sql_id;
        userid = entry->key.key.user_id;
        nodeid = entry->key.key.cn_id;
        executorType = entry->key.executorType;

        if (entry->key.partitionKey[0] != '\0') {
            memcpy_s(partitionKey, sizeof(partitionKey), entry->key.partitionKey,
                     sizeof(entry->key.partitionKey));
        }

        calls = entry->calls;

        SpinLockRelease(&entry->mutex);

        values[SPQ_STAT_STATAMENTS_UNIQUE_SQL_ID] = UInt64GetDatum(unique_sql_id);
        values[SPQ_STAT_STATAMENTS_USER_ID] = ObjectIdGetDatum(userid);
        values[SPQ_STAT_STATAMENTS_NODE_ID] = UInt32GetDatum(nodeid);
        values[SPQ_STAT_STATAMENTS_EXECUTOR_TYPE] = UInt32GetDatum((uint32)executorType);

        if (partitionKey[0] != '\0') {
            values[SPQ_STAT_STATAMENTS_PARTITION_KEY] = CStringGetTextDatum(partitionKey);
        } else {
            nulls[SPQ_STAT_STATAMENTS_PARTITION_KEY] = true;
        }

        values[SPQ_STAT_STATAMENTS_CALLS] = Int64GetDatumFast(calls);

        tuplestore_putvalues(tupstore, tupdesc, values, nulls);
    }

    LWLockRelease(&queryStats->lock);

    return (Datum)0;
}

/*
 * SpqQueryStatsSynchronizeEntries removes all entries in queryStats hash
 * that does not have matching queryId in pg_stat_statements.
 *
 * A function called inside (SpqQueryStatsRemoveExpiredEntries) acquires
 * an exclusive lock on queryStats->lock.
 */
void SpqQueryStatsSynchronizeEntries(void)
{
    SpqQueryStatsRemoveExpiredEntries();
}

/*
 * SpqQueryStatsRemoveExpiredEntries iterates over queryStats hash entries
 * and removes entries with keys that do not exists in the provided hash of
 * queryIds.
 *
 * Acquires and releases exclusive lock on queryStats->lock.
 */
static void SpqQueryStatsRemoveExpiredEntries()
{
    HASH_SEQ_STATUS hash_seq;
    QueryStatsEntry* entry;
    int removedCount = 0;
    Oid currentUserId = GetUserId();
    bool canSeeStats = superuser() || isMonitoradmin(currentUserId);

    LWLockAcquire(&queryStats->lock, LW_EXCLUSIVE);

    hash_seq_init(&hash_seq, queryStatsHash);
    while ((entry = static_cast<QueryStatsEntry*>(hash_seq_search(&hash_seq))) != NULL) {
        UniqueSQLKey existingStatsKey;

        /*
         * pg_stat_statements returns NULL in the queryId field for queries
         * belonging to other users. Those queries are therefore not reflected
         * in the existingQueryIdHash, but that does not mean that we should
         * remove them as they are relevant to other users.
         */
        if (!(currentUserId == entry->key.key.user_id || canSeeStats)) {
            continue;
        }

        existingStatsKey.user_id = entry->key.key.user_id;
        existingStatsKey.cn_id = entry->key.key.cn_id;
        existingStatsKey.unique_sql_id = entry->key.key.unique_sql_id;
        if (!FindUniqueSQL(existingStatsKey, NULL)) {
            hash_search(queryStatsHash, &entry->key, HASH_REMOVE, NULL);
            removedCount++;
        }
    }

    LWLockRelease(&queryStats->lock);

    if (removedCount > 0) {
        elog(DEBUG2, "spq_stat_statements removed %d expired entries", removedCount);
    }
}

/*
 * spq_executor_name is a UDF that returns the name of the executor
 * given the internal enum value.
 */
Datum spq_executor_name(PG_FUNCTION_ARGS)
{
    MultiExecutorType executorType = static_cast<MultiExecutorType>(PG_GETARG_UINT32(0));

    char* executorName = SpqExecutorName(executorType);

    PG_RETURN_TEXT_P(cstring_to_text(executorName));
}

/*
 * SpqExecutorName returns the name of the executor given the internal
 * enum value.
 */
static char* SpqExecutorName(MultiExecutorType executorType)
{
    switch (executorType) {
        case MULTI_EXECUTOR_ADAPTIVE: {
            return "adaptive";
        }

        case MULTI_EXECUTOR_NON_PUSHABLE_INSERT_SELECT: {
            return "insert-select";
        }

        default: {
            return "unknown";
        }
    }
}
