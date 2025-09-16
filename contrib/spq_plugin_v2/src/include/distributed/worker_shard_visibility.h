/*-------------------------------------------------------------------------
 *
 * worker_shard_visibility.h
 *   Hide shard names on MX worker nodes.
 *
 * Copyright (c) Citus Data, Inc.
 *
 *-------------------------------------------------------------------------
 */

#ifndef WORKER_SHARD_VISIBILITY_H
#define WORKER_SHARD_VISIBILITY_H

#include "nodes/nodes.h"

/* HideShardsMode is used to determine whether to hide shards */
typedef enum HideShardsMode {
    CHECK_APPLICATION_NAME,
    HIDE_SHARDS_FROM_APPLICATION,
    DO_NOT_HIDE_SHARDS
} HideShardsMode;

extern void HideShardsFromSomeApplications(Query* query);
extern void ResetHideShardsDecision(void);
extern void ErrorIfRelationIsAKnownShard(Oid relationId);
extern void ErrorIfIllegallyChangingKnownShard(Oid relationId);
extern bool RelationIsAKnownShard(Oid shardRelationId);

#endif /* WORKER_SHARD_VISIBILITY_H */
