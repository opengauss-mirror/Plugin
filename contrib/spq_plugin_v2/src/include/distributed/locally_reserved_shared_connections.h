/*-------------------------------------------------------------------------
 *
 * locally_reserved_shared_connection_stats.h
 *   Management of connection reservations in shard memory pool
 *
 * Copyright (c) Citus Data, Inc.
 *
 *-------------------------------------------------------------------------
 */

#ifndef LOCALLY_RESERVED_SHARED_CONNECTIONS_H_
#define LOCALLY_RESERVED_SHARED_CONNECTIONS_H_

#include "distributed/connection_management.h"

/*
 * Hash key for connection reservations
 */
typedef struct ReservedConnectionHashKey {
    char hostname[MAX_NODE_LENGTH];
    int32 port;
    Oid databaseOid;
    Oid userId;
} ReservedConnectionHashKey;

/*
 * Hash entry for per worker information. The rules are as follows:
 *  - If there is no entry in the hash, we can make a reservation.
 *  - If usedReservation is false, we have a reservation that we can use.
 *  - If usedReservation is true, we used the reservation and cannot make more
 * reservations.
 */
typedef struct ReservedConnectionHashEntry {
    ReservedConnectionHashKey key;

    bool usedReservation;
} ReservedConnectionHashEntry;

extern bool CanUseReservedConnection(const char* hostName, int nodePort, Oid userId,
                                     Oid databaseOid);
extern void MarkReservedConnectionUsed(const char* hostName, int nodePort, Oid userId,
                                       Oid databaseOid);
extern void DeallocateReservedConnections(void);
extern void EnsureConnectionPossibilityForRemotePrimaryNodes(void);
extern bool TryConnectionPossibilityForLocalPrimaryNode(void);
extern bool IsReservationPossible(void);

extern uint32 LocalConnectionReserveHashHash(const void* key, Size keysize);
extern int LocalConnectionReserveHashCompare(const void* a, const void* b, Size keysize);
#endif /* LOCALLY_RESERVED_SHARED_CONNECTIONS_H_ */
