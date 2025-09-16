/*-------------------------------------------------------------------------
 *
 * worker_shard_copy.c
 *	 Copy data to destination shard in a push approach.
 *
 * Copyright (c) Citus Data, Inc.
 *
 *
 *-------------------------------------------------------------------------
 */

#ifndef WORKER_SHARD_COPY_H_
#define WORKER_SHARD_COPY_H_

extern DestReceiver* CreateShardCopyDestReceiver(EState* executorState,
                                                 List* destinationShardFullyQualifiedName,
                                                 uint32_t destinationNodeId);

extern DestReceiver* CreateShardCopyDestReceiver(EState* executorState,
                                                 List* destinationShardFullyQualifiedName,
                                                 char* destinationNodeName,
                                                 int32 destinationNodePort,
                                                 bool localCopy);

extern const char* CopyableColumnNamesFromRelationName(const char* schemaName,
                                                       const char* relationName);

extern const char* CopyableColumnNamesFromTupleDesc(TupleDesc tupdesc);

#endif /* WORKER_SHARD_COPY_H_ */
