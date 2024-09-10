#ifndef VECINDEX_H
#define VECINDEX_H

#define MIN(A, B) ((B) < (A) ? (B) : (A))
#define MAX(A, B) ((B) > (A) ? (B) : (A))

#define VecIndexTupleGetXid(itup) (((char*)(itup)) + HNSW_ELEMENT_TUPLE_SIZE(VARSIZE_ANY(&(itup)->data)))

struct VectorScanData {
    /*
    * used in ustore only, indicate the last returned index tuple which is modified
    * by current transaction. see VecVisibilityCheckCid() for more information.
    */
    char *lastSelfModifiedItup;
    uint16 lastSelfModifiedItupBufferSize;
    Buffer buf;
};

bool VecItupGetXminXmax(Page page, OffsetNumber offnum, TransactionId oldest_xmin, TransactionId *xmin,
                        TransactionId *xmax, bool *xminCommitted, bool *xmaxCommitted, bool isToast);
bool VecVisibilityCheck(IndexScanDesc scan, Page page, OffsetNumber offnum, bool *needRecheck);

#endif //VECINDEX_H
