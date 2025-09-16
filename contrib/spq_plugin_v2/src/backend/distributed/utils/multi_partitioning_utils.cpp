/*
 * multi_partitioning_utils.c
 *	  Utility functions for declarative partitioning
 *
 * Copyright (c) Citus Data, Inc.
 */
#include "postgres.h"

#include "pgstat.h"

#include "access/genam.h"
#include "access/heapam.h"
#include "access/htup.h"
#include "catalog/index.h"
#include "catalog/indexing.h"
#include "catalog/pg_class.h"
#include "catalog/pg_constraint.h"
#include "catalog/pg_inherits.h"
#include "commands/tablecmds.h"
#include "nodes/pg_list.h"
#include "utils/builtins.h"
#include "utils/fmgroids.h"
#include "utils/lsyscache.h"
#include "utils/rel.h"
#include "utils/syscache.h"

#include "distributed/commands.h"
#include "distributed/listutils.h"
#include "distributed/multi_partitioning_utils.h"
static Relation try_relation_open_nolock(Oid relationId);

/*
 * Returns true if the given relation is a partitioned table.
 */
bool PartitionedTable(Oid relationId)
{
    Relation rel = try_relation_open(relationId, AccessShareLock);

    /* don't error out for tables that are dropped */
    if (rel == NULL) {
        return false;
    }

    bool partitionedTable = false;

    if (rel->rd_rel->parttype == PARTTYPE_PARTITIONED_RELATION ||
        rel->rd_rel->parttype == PARTTYPE_SUBPARTITIONED_RELATION ||
        rel->rd_rel->parttype == PARTTYPE_VALUE_PARTITIONED_RELATION) {
        partitionedTable = true;
    }

    /* keep the lock */
    table_close(rel, NoLock);

    return partitionedTable;
}

/*
 * Returns true if the given relation is a partitioned table. The function
 * doesn't acquire any locks on the input relation, thus the caller is
 * reponsible for holding the appropriate locks.
 */
bool PartitionedTableNoLock(Oid relationId)
{
    Relation rel = try_relation_open_nolock(relationId);
    bool partitionedTable = false;

    /* don't error out for tables that are dropped */
    if (rel == NULL) {
        return false;
    }

    if (rel->rd_rel->parttype == PARTTYPE_PARTITIONED_RELATION ||
        rel->rd_rel->parttype == PARTTYPE_SUBPARTITIONED_RELATION ||
        rel->rd_rel->parttype == PARTTYPE_VALUE_PARTITIONED_RELATION) {
        partitionedTable = true;
    }

    /* keep the lock */
    table_close(rel, NoLock);

    return partitionedTable;
}

/*
 * Returns true if the given relation is a partition.
 */
bool PartitionTable(Oid relationId)
{
    Relation rel = try_relation_open(relationId, AccessShareLock);

    /* don't error out for tables that are dropped */
    if (rel == NULL) {
        return false;
    }

    bool partitionTable = rel->rd_rel->parttype == PARTTYPE_PARTITIONED_RELATION ||
                          rel->rd_rel->parttype == PARTTYPE_SUBPARTITIONED_RELATION ||
                          rel->rd_rel->parttype == PARTTYPE_VALUE_PARTITIONED_RELATION;

    /* keep the lock */
    table_close(rel, NoLock);

    return partitionTable;
}

/*
 * Returns true if the given relation is a partition.  The function
 * doesn't acquire any locks on the input relation, thus the caller is
 * reponsible for holding the appropriate locks.
 */
bool PartitionTableNoLock(Oid relationId)
{
    Relation rel = try_relation_open_nolock(relationId);

    /* don't error out for tables that are dropped */
    if (rel == NULL) {
        return false;
    }

    bool partitionTable = rel->rd_rel->parttype == PARTTYPE_PARTITIONED_RELATION ||
                          rel->rd_rel->parttype == PARTTYPE_SUBPARTITIONED_RELATION ||
                          rel->rd_rel->parttype == PARTTYPE_VALUE_PARTITIONED_RELATION;

    /* keep the lock */
    table_close(rel, NoLock);

    return partitionTable;
}

/*
 * try_relation_open_nolock opens a relation with given relationId without
 * acquiring locks. PostgreSQL's try_relation_open() asserts that caller
 * has already acquired a lock on the relation, which we don't always do.
 *
 * ATTENTION:
 *   1. Sync this with try_relation_open(). It hasn't changed for 10 to 12
 *      releases though.
 *   2. We should remove this after we fix the locking/distributed deadlock
 *      issues with MX Truncate. See https://github.com/citusdata/citus/pull/2894
 *      for more discussion.
 */
static Relation try_relation_open_nolock(Oid relationId)
{
    if (!SearchSysCacheExists1(RELOID, ObjectIdGetDatum(relationId))) {
        return NULL;
    }

    Relation relation = RelationIdGetRelation(relationId);
    if (!RelationIsValid(relation)) {
        return NULL;
    }

    return relation;
}

/*
 * IsChildTable returns true if the table is inherited. Note that
 * partition tables inherites by default. However, this function
 * returns false if the given table is a partition.
 */
bool IsChildTable(Oid relationId)
{
    ScanKeyData key[1];
    HeapTuple inheritsTuple = NULL;
    bool tableInherits = false;

    Relation pgInherits = table_open(InheritsRelationId, AccessShareLock);

    ScanKeyInit(&key[0], Anum_pg_inherits_inhrelid, BTEqualStrategyNumber, F_OIDEQ,
                ObjectIdGetDatum(relationId));

    SysScanDesc scan = systable_beginscan(pgInherits, InvalidOid, false, NULL, 1, key);

    while ((inheritsTuple = systable_getnext(scan)) != NULL) {
        Oid inheritedRelationId = ((Form_pg_inherits)GETSTRUCT(inheritsTuple))->inhrelid;

        if (relationId == inheritedRelationId) {
            tableInherits = true;
            break;
        }
    }

    systable_endscan(scan);
    table_close(pgInherits, AccessShareLock);

    if (tableInherits && PartitionTable(relationId)) {
        tableInherits = false;
    }

    return tableInherits;
}

/*
 * IsParentTable returns true if the table is inherited. Note that
 * partitioned tables inherited by default. However, this function
 * returns false if the given table is a partitioned table.
 */
bool IsParentTable(Oid relationId)
{
    ScanKeyData key[1];
    bool tableInherited = false;

    Relation pgInherits = table_open(InheritsRelationId, AccessShareLock);

    ScanKeyInit(&key[0], Anum_pg_inherits_inhparent, BTEqualStrategyNumber, F_OIDEQ,
                ObjectIdGetDatum(relationId));

    SysScanDesc scan =
        systable_beginscan(pgInherits, InheritsParentIndexId, true, NULL, 1, key);

    if (systable_getnext(scan) != NULL) {
        tableInherited = true;
    }
    systable_endscan(scan);
    table_close(pgInherits, AccessShareLock);

    Relation relation = try_relation_open(relationId, AccessShareLock);
    if (relation == NULL) {
        ereport(ERROR, (errmsg("relation with OID %u does not exist", relationId)));
    }

    if (tableInherited && PartitionedTableNoLock(relationId)) {
        tableInherited = false;
    }

    relation_close(relation, AccessShareLock);

    return tableInherited;
}

/*
 * Wrapper around get_partition_parent
 *
 * Note: Because this function assumes that the relation whose OID is passed
 * as an argument will have precisely one parent, it should only be called
 * when it is known that the relation is a partition.
 */
Oid PartitionParentOid(Oid partitionOid)
{
    Oid partitionParentOid = InvalidOid;
#ifdef DISABLE_OG_COMMENTS
    get_partition_parent(partitionOid, false);
#endif

    return partitionParentOid;
}

/*
 * PartitionWithLongestNameRelationId is a utility function that returns the
 * oid of the partition table that has the longest name in terms of number of
 * characters.
 */
Oid PartitionWithLongestNameRelationId(Oid parentRelationId)
{
    Oid longestNamePartitionId = InvalidOid;
    int longestNameLength = 0;
    List* partitionList = NIL;

    Oid partitionRelationId = InvalidOid;
    foreach_declared_oid(partitionRelationId, partitionList)
    {
        char* partitionName = get_rel_name(partitionRelationId);
        int partitionNameLength = strnlen(partitionName, NAMEDATALEN);
        if (partitionNameLength > longestNameLength) {
            longestNamePartitionId = partitionRelationId;
            longestNameLength = partitionNameLength;
        }
    }

    return longestNamePartitionId;
}

/*
 * ListShardsUnderParentRelation returns a list of ShardInterval for every
 * shard under a given relation, meaning it includes the shards of child
 * tables in a partitioning hierarchy.
 */
List* ListShardsUnderParentRelation(Oid relationId)
{
    List* shardList = LoadShardIntervalList(relationId);
    return shardList;
}
