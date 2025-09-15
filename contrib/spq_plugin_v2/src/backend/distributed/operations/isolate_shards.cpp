/*-------------------------------------------------------------------------
 *
 * split_shards.c
 *
 * This file contains functions to split a shard according to a given
 * distribution column value.
 *
 * Copyright (c) Citus Data, Inc.
 *
 *-------------------------------------------------------------------------
 */

#include "postgres.h"

#include "c.h"
#include "fmgr.h"
#include "libpq/libpq-fe.h"

#include "catalog/pg_class.h"
#include "nodes/pg_list.h"
#include "storage/lock/lock.h"
#include "utils/builtins.h"
#include "utils/elog.h"
#include "utils/errcodes.h"
#include "utils/lsyscache.h"
#include "utils/typcache.h"

#include "distributed/colocation_utils.h"
#include "distributed/coordinator_protocol.h"
#include "distributed/metadata_cache.h"
#include "distributed/metadata_sync.h"
#include "distributed/multi_join_order.h"
#include "distributed/multi_partitioning_utils.h"
#include "distributed/multi_router_planner.h"
#include "distributed/pg_dist_partition.h"
#include "distributed/pg_dist_shard.h"
#include "distributed/reference_table_utils.h"
#include "distributed/remote_commands.h"
#include "distributed/resource_lock.h"
#include "distributed/shard_split.h"
#include "distributed/utils/distribution_column_map.h"
#include "distributed/version_compat.h"
#include "distributed/worker_manager.h"
#include "distributed/worker_protocol.h"
#include "distributed/worker_transaction.h"

/* declarations for dynamic loading */
PG_FUNCTION_INFO_V1(worker_hash);
extern "C" Datum worker_hash(PG_FUNCTION_ARGS);

/*
 * worker_hash returns the hashed value of the given value.
 */
Datum worker_hash(PG_FUNCTION_ARGS)
{
    CheckCitusVersion(ERROR);

    Datum valueDatum = PG_GETARG_DATUM(0);

    /* figure out hash function from the data type */
    Oid valueDataType = get_fn_expr_argtype(fcinfo->flinfo, 0);
    TypeCacheEntry* typeEntry =
        lookup_type_cache(valueDataType, TYPECACHE_HASH_PROC_FINFO);

    if (typeEntry->hash_proc_finfo.fn_oid == InvalidOid) {
        ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                        errmsg("cannot find a hash function for the input type"),
                        errhint("Cast input to a data type with a hash function.")));
    }

    FmgrInfo* hashFunction = static_cast<FmgrInfo*>(palloc0(sizeof(FmgrInfo)));
    fmgr_info_copy(hashFunction, &(typeEntry->hash_proc_finfo), CurrentMemoryContext);

    /* calculate hash value */
    Datum hashedValueDatum =
        FunctionCall1Coll(hashFunction, PG_GET_COLLATION(), valueDatum);

    PG_RETURN_INT32(hashedValueDatum);
}
