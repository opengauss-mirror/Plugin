/*-------------------------------------------------------------------------
 * placement_connection.h
 *
 * Copyright (c) Citus Data, Inc.
 *
 *-------------------------------------------------------------------------
 */

#ifndef PLACEMENT_CONNECTION_H
#define PLACEMENT_CONNECTION_H

#include "distributed/connection_management.h"
#include "distributed/placement_access.h"

/*
 * A connection reference is used to register that a connection has been used
 * to read or modify either a) a shard placement as a particular user b) a
 * group of colocated placements (which depend on whether the reference is
 * from ConnectionPlacementHashEntry or ColocatedPlacementHashEntry).
 */
typedef struct ConnectionReference {
    /*
     * The user used to read/modify the placement. We cannot reuse connections
     * that were performed using a different role, since it would not have the
     * right permissions.
     */
    const char* userName;

    /* the connection */
    MultiConnection* connection;

    /*
     * Information about what the connection is used for. There can only be
     * one connection executing DDL/DML for a placement to avoid deadlock
     * issues/read-your-own-writes violations.  The difference between DDL/DML
     * currently is only used to emit more precise error messages.
     */
    bool hadDML;
    bool hadDDL;

    /* colocation group of the placement, if any */
    uint32 colocationGroupId;
    uint32 representativeValue;

    /* placementId of the placement, used only for append distributed tables */
    uint64 placementId;

    /* membership in MultiConnection->referencedPlacements */
    dlist_node connectionNode;
} ConnectionReference;

/*
 * Hash table mapping placements to a list of connections.
 *
 * This stores a list of connections for each placement, because multiple
 * connections to the same placement may exist at the same time. E.g. an
 * adaptive executor query may reference the same placement in several
 * sub-tasks.
 *
 * We keep track about a connection having executed DML or DDL, since we can
 * only ever allow a single transaction to do either to prevent deadlocks and
 * consistency violations (e.g. read-your-own-writes).
 */

/* hash key */
typedef struct ConnectionPlacementHashKey {
    uint64 placementId;
} ConnectionPlacementHashKey;

struct ColocatedPlacementsHashEntry;
/* hash entry */
typedef struct ConnectionPlacementHashEntry {
    ConnectionPlacementHashKey key;

    /* did any remote transactions fail? */
    bool failed;

    /* primary connection used to access the placement */
    ConnectionReference* primaryConnection;

    /* are any other connections reading from the placements? */
    bool hasSecondaryConnections;

    /* entry for the set of co-located placements */
    struct ColocatedPlacementsHashEntry* colocatedEntry;

    /* membership in ConnectionShardHashEntry->placementConnections */
    dlist_node shardNode;
} ConnectionPlacementHashEntry;

/*
 * A hash-table mapping colocated placements to connections. Colocated
 * placements being the set of placements on a single node that represent the
 * same value range. This is needed because connections for colocated
 * placements (i.e. the corresponding placements for different colocated
 * distributed tables) need to share connections.  Otherwise things like
 * foreign keys can very easily lead to unprincipled deadlocks.  This means
 * that there can only be one DML/DDL connection for a set of colocated
 * placements.
 *
 * A set of colocated placements is identified, besides node identifying
 * information, by the associated colocation group id and the placement's
 * 'representativeValue' which currently is the lower boundary of it's
 * hash-range.
 *
 * Note that this hash-table only contains entries for hash-partitioned
 * tables, because others so far don't support colocation.
 */

/* hash key */
typedef struct ColocatedPlacementsHashKey {
    /* to identify host - database can't differ */
    uint32 nodeId;

    /* colocation group, or invalid */
    uint32 colocationGroupId;

    /* to represent the value range */
    uint32 representativeValue;
} ColocatedPlacementsHashKey;

/* hash entry */
typedef struct ColocatedPlacementsHashEntry {
    ColocatedPlacementsHashKey key;

    /* primary connection used to access the co-located placements */
    ConnectionReference* primaryConnection;

    /* are any other connections reading from the placements? */
    bool hasSecondaryConnections;
} ColocatedPlacementsHashEntry;

/*
 * Hash table mapping shard ids to placements.
 *
 * This is used to track whether placements of a shard have to be marked
 * invalid after a failure, or whether a coordinated transaction has to be
 * aborted, to avoid all placements of a shard to be marked invalid.
 */

/* hash key */
typedef struct ConnectionShardHashKey {
    uint64 shardId;
} ConnectionShardHashKey;

/* hash entry */
typedef struct ConnectionShardHashEntry {
    ConnectionShardHashKey key;
    dlist_head placementConnections;
} ConnectionShardHashEntry;

extern uint32 ColocatedPlacementsHashHash(const void* key, Size keysize);

extern int ColocatedPlacementsHashCompare(const void* a, const void* b, Size keysize);

extern MultiConnection* GetPlacementConnection(uint32 flags,
                                               struct ShardPlacement* placement,
                                               const char* userName);
extern MultiConnection* StartPlacementConnection(uint32 flags,
                                                 struct ShardPlacement* placement,
                                                 const char* userName);
extern MultiConnection* GetConnectionIfPlacementAccessedInXact(int flags,
                                                               List* placementAccessList,
                                                               const char* userName);
extern MultiConnection* StartPlacementListConnection(uint32 flags,
                                                     List* placementAccessList,
                                                     const char* userName);
extern ShardPlacementAccess* CreatePlacementAccess(ShardPlacement* placement,
                                                   ShardPlacementAccessType accessType);
extern void AssignPlacementListToConnection(List* placementAccessList,
                                            MultiConnection* connection);

extern void ResetPlacementConnectionManagement(void);
extern void ErrorIfPostCommitFailedShardPlacements(void);

extern void CloseShardPlacementAssociation(struct MultiConnection* connection);
extern void ResetShardPlacementAssociation(struct MultiConnection* connection);

extern bool ConnectionModifiedPlacement(MultiConnection* connection);
extern bool UseConnectionPerPlacement(void);

#endif /* PLACEMENT_CONNECTION_H */
