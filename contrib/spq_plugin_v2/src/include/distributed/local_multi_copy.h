
#ifndef LOCAL_MULTI_COPY
#define LOCAL_MULTI_COPY

extern void WriteTupleToLocalShard(TupleTableSlot* slot, CitusCopyDestReceiver* copyDest,
                                   int64 shardId, CopyOutState localCopyOutState);
extern void WriteTupleToLocalFile(TupleTableSlot* slot, CitusCopyDestReceiver* copyDest,
                                  int64 shardId, CopyOutState localFileCopyOutState,
                                  FileCompat* fileCompat);
extern void FinishLocalCopyToShard(CitusCopyDestReceiver* copyDest, int64 shardId,
                                   CopyOutState localCopyOutState);
extern void FinishLocalCopyToFile(CopyOutState localFileCopyOutState,
                                  FileCompat* fileCompat);

#endif /* LOCAL_MULTI_COPY */
