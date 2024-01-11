/* -------------------------------------------------------------------------
*
* spq_seqscan.cpp
*	  Support routines for sequential scans of relations.
*
* Portions Copyright (c) 2020 Huawei Technologies Co.,Ltd.
* Portions Copyright (c) 1996-2012, PostgreSQL Global Development Group
* Portions Copyright (c) 1994, Regents of the University of California
* Portions Copyright (c) 2021, openGauss Contributors
*
* -------------------------------------------------------------------------
*
* INTERFACE ROUTINES
*		ExecSeqScan				sequentially scans a relation.
*		ExecSeqNext				retrieve next tuple in sequential order.
*		ExecInitSeqScan			creates and initializes a seqscan node.
*		ExecEndSeqScan			releases any storage allocated.
*		ExecReScanSeqScan		rescans the relation
 */
#include "postgres.h"
#include "storage/lock/lock.h"
#include "storage/predicate.h"
#include "access/valid.h"
#include "utils/guc.h"
#include "utils/builtins.h"
#include "storage/smgr/segment.h"
#include "mpmcqueue.h"
#include "executor/executor.h"
#include "executor/node/nodeSeqscan.h"
#include "pgxc/execRemote.h"
#include "libpq/pqformat.h"
#include "libpq/libpq.h"
#include "executor/spq_seqscan.h"
#include "access/csnlog.h"
#include "utils/snapmgr.h"
#include "storage/procarray.h"
#include "ddes/dms/ss_transaction.h"

#define DECOMPRESS_HEAP_TUPLE(_isCompressed, _heapTuple, _destTupleData, _rd_att, _heapPage)  \
    do {                                                                                      \
        if ((_isCompressed)) {                                                                \
            HeapTupleData srcTuple = *(_heapTuple);                                           \
            Assert((_heapPage));                                                              \
            /* Then set the memory for decompressed tuple */                                  \
            (_heapTuple)->t_data = (_destTupleData);                                          \
            heapCopyCompressedTuple(&srcTuple, (_rd_att), (char*)(_heapPage), (_heapTuple));  \
        }                                                                                     \
    } while (0)
#define BLOCKSIZE (8 * 1024)

constexpr int FETCH_BLOCK_NUM = 128;
constexpr int FETCH_BLOCK_NUM_DIRECT = 512;
constexpr int MAX_ENQUEUE_TIME = 3;
constexpr int PAGE_QUEUE_SIZE = 2048;
enum SpqState {
    SPQ_SUCCESS,
    SPQ_AWAIT_ASYNC,
    SPQ_QUEUE_EMPTY,
    SPQ_QUERY_FAILED,
    SPQ_QUERY_END,
};

THR_LOCAL init_spqscan_hook_type backup_init_spqscan_hook = NULL;
THR_LOCAL exec_spqscan_hook_type backup_exec_spqscan_hook = NULL;
THR_LOCAL end_spqscan_hook_type backup_end_spqscan_hook = NULL;
THR_LOCAL spqscan_rescan_hook_type backup_spqscan_rescan_hook = NULL;

static bool SpqSeqRecheck(SeqScanState* node, TupleTableSlot* slot)
{
    return true;
}

class SpqPageManager : public BaseObject {
public:
    ScanDirection direction;
public:
    SpqPageManager(ScanDirection direction) : direction(direction) {};
    virtual ~SpqPageManager() {};
    virtual SpqState FetchBlocks(uint32 start, uint32 end) = 0;
    // this interface should guarantee a new page or page queue empty
    // if is an async manager, should wait until at least one page return.
    virtual SpqState GetNewPage() = 0;
    // return true if successfully get tuple, false if run out of current page
    virtual bool GetTupleFromPage(TupleTableSlot* slot) = 0;
    virtual void Rescan(TableScanDesc scanDesc) = 0;
    void UpdateDirection(ScanDirection direction)
    {
        this->direction = direction;
    }
};

class SpqBlockManager : public BaseObject {
public:
    ScanDirection direction;
    uint32 step;
public:
    SpqBlockManager(ScanDirection direction, uint32 step) : direction(direction), step(step) {};
    virtual ~SpqBlockManager() {};
    virtual SpqState GetBlockIDs(uint32 &start, uint32 &end) = 0;
    virtual bool IsBlockEnd() = 0;
    virtual void Rescan() = 0;
    void UpdateDirection(ScanDirection direction)
    {
        this->direction = direction;
    }
};

static bool DirectReadXidVisibleInSnapshot(TransactionId xid, Snapshot snapshot, bool* sync)
{
    volatile CommitSeqNo csn;
    bool looped = false;
    TransactionId parentXid = InvalidTransactionId;

#ifdef XIDVIS_DEBUG
    ereport(DEBUG1,
            (errmsg("DirectReadXidVisibleInSnapshot xid %ld cur_xid %ld snapshot csn %lu xmax %ld",
                    xid,
                    GetCurrentTransactionIdIfAny(),
                    snapshot->snapshotcsn,
                    snapshot->xmax)));
#endif

loop:
    if (ENABLE_DMS) {
        /* fetch TXN info locally if either reformer, original primary, or normal primary */
        if (SS_PRIMARY_MODE || SS_OFFICIAL_PRIMARY) {
            csn = TransactionIdGetCommitSeqNo(xid, false, true, false, snapshot);
        } else {
            csn = SSTransactionIdGetCommitSeqNo(xid, false, true, false, snapshot, sync);
        }
    } else {
        csn = TransactionIdGetCommitSeqNo(xid, false, true, false, snapshot);
    }

#ifdef XIDVIS_DEBUG
    ereport(DEBUG1,
            (errmsg("DirectReadXidVisibleInSnapshot xid %ld cur_xid %ld csn %ld snapshot"
                    "csn %ld xmax %ld",
                    xid,
                    GetCurrentTransactionIdIfAny(),
                    csn,
                    snapshot->snapshotcsn,
                    snapshot->xmax)));
#endif

    if (COMMITSEQNO_IS_COMMITTED(csn)) {
        if (csn < snapshot->snapshotcsn)
            return true;
        else
            return false;
    } else if (COMMITSEQNO_IS_COMMITTING(csn)) {
        /* SS master node would've already sync-waited, so this should never happen */
        if (SS_STANDBY_MODE) {
            ereport(FATAL, (errmsg("SS xid %lu's csn %lu is still COMMITTING after Master txn waited.", xid, csn)));
        }
        if (looped) {
            ereport(DEBUG1, (errmsg("transaction id %lu's csn %ld may ABORT but direct read can't change.", xid, csn)));
            return false;
        } else {
            if (!COMMITSEQNO_IS_SUBTRANS(csn)) {
                /* If snapshotcsn lower than csn stored in csn log, don't need to wait. */
                CommitSeqNo latestCSN = GET_COMMITSEQNO(csn);
                if (latestCSN >= snapshot->snapshotcsn) {
                    ereport(DEBUG1,
                            (errmsg(
                                "snapshotcsn %lu lower than csn %lu stored in csn log, don't need to sync wait, trx id %lu",
                                snapshot->snapshotcsn,
                                csn,
                                xid)));
                    return false;
                }
            } else {
                parentXid = (TransactionId)GET_PARENTXID(csn);
            }

            if (u_sess->attr.attr_common.xc_maintenance_mode || t_thrd.xact_cxt.bInAbortTransaction) {
                return false;
            }

            /* Wait for txn end and check again. */
            if (sync != NULL) {
                *sync = true;
            }
            if (TransactionIdIsValid(parentXid))
                SyncLocalXidWait(parentXid, snapshot);
            else
                SyncLocalXidWait(xid, snapshot);
            looped = true;
            parentXid = InvalidTransactionId;
            goto loop;
        }
    } else {
        return false;
    }
}

static bool DirectReadCommittedXidVisibleInSnapshot(TransactionId xid, Snapshot snapshot)
{
    CommitSeqNo csn;
    bool looped = false;
    TransactionId parentXid = InvalidTransactionId;

    /*
     * Make a quick range check to eliminate most XIDs without looking at the
     * CSN log.
     */
    if (TransactionIdPrecedes(xid, snapshot->xmin))
        return true;

loop:
    if (ENABLE_DMS) {
        /* fetch TXN info locally if either reformer, original primary, or normal primary */
        if (SS_PRIMARY_MODE || SS_OFFICIAL_PRIMARY) {
            csn = TransactionIdGetCommitSeqNo(xid, true, true, false, snapshot);
        } else {
            csn = SSTransactionIdGetCommitSeqNo(xid, true, true, false, snapshot, NULL);
        }
    } else {
        csn = TransactionIdGetCommitSeqNo(xid, true, true, false, snapshot);
    }

    if (COMMITSEQNO_IS_COMMITTING(csn)) {
        /* SS master node would've already sync-waited, so this should never happen */
        if (SS_STANDBY_MODE) {
            ereport(FATAL, (errmsg("SS xid %lu's csn %lu is still COMMITTING after Master txn waited.", xid, csn)));
        }
        if (looped) {
            ereport(WARNING, (errmsg("transaction id %lu's csn %ld may frozen but direct read can't change.",
                                     xid, csn)));
            return true;
        } else {
            if (!COMMITSEQNO_IS_SUBTRANS(csn)) {
                /* If snapshotcsn lower than csn stored in csn log, don't need to wait. */
                CommitSeqNo latestCSN = GET_COMMITSEQNO(csn);
                if (latestCSN >= snapshot->snapshotcsn) {
                    ereport(DEBUG1,
                            (errmsg("snapshotcsn %lu lower than csn %lu"
                                    " stored in csn log, don't need to sync wait, trx id %lu",
                                    snapshot->snapshotcsn,
                                    csn,
                                    xid)));
                    return false;
                }
            } else {
                parentXid = (TransactionId)GET_PARENTXID(csn);
            }

            if (u_sess->attr.attr_common.xc_maintenance_mode || t_thrd.xact_cxt.bInAbortTransaction) {
                return false;
            }

            /* Wait for txn end and check again. */
            if (TransactionIdIsValid(parentXid))
                SyncLocalXidWait(parentXid);
            else
                SyncLocalXidWait(xid);
            looped = true;
            parentXid = InvalidTransactionId;
            goto loop;
        }
    } else if (!COMMITSEQNO_IS_COMMITTED(csn)) {
        ereport(WARNING,
                (errmsg("transaction/csn %lu/%lu was hinted as "
                        "committed, but was not marked as committed in "
                        "the transaction log",
                        xid,
                        csn)));
        /*
         * We have contradicting evidence on whether the transaction committed or
         * not. Let's assume that it did. That seems better than erroring out.
         */
        return true;
    }

    if (csn < snapshot->snapshotcsn)
        return true;
    else
        return false;
}

static bool DirectReadHeapTupleSatisfiesVisibility(HeapTuple htup, Snapshot snapshot, Page page)
{
    if (snapshot->satisfies != SNAPSHOT_MVCC)
        ereport(ERROR,
                (errcode(ERRCODE_SYNTAX_ERROR),
                 errmsg("DirectRead only support SNAPSHOT_MVCC")));
    HeapTupleHeader tuple = htup->t_data;
    Assert(ItemPointerIsValid(&htup->t_self));
    Assert(htup->t_tableOid != InvalidOid);
    bool visible = false;

    if (SHOW_DEBUG_MESSAGE()) {
        ereport(DEBUG1,
                (errmsg("HeapTupleSatisfiesMVCC self(%d,%d) ctid(%d,%d) cur_xid %ld xmin %ld"
                        " xmax %ld csn %lu",
                        ItemPointerGetBlockNumber(&htup->t_self),
                        ItemPointerGetOffsetNumber(&htup->t_self),
                        ItemPointerGetBlockNumber(&tuple->t_ctid),
                        ItemPointerGetOffsetNumber(&tuple->t_ctid),
                        GetCurrentTransactionIdIfAny(),
                        HeapTupleHeaderGetXmin(page, tuple),
                        HeapTupleHeaderGetXmax(page, tuple),
                        snapshot->snapshotcsn)));
    }

    /*
     * Just valid for read-only transaction when u_sess->attr.attr_common.XactReadOnly is true.
     * Show any tuples including dirty ones when u_sess->attr.attr_storage.enable_show_any_tuples is true.
     * GUC param u_sess->attr.attr_storage.enable_show_any_tuples is just for analyse or maintenance
     */
    if (u_sess->attr.attr_common.XactReadOnly && u_sess->attr.attr_storage.enable_show_any_tuples)
        return true;

    if (!HeapTupleHeaderXminCommitted(tuple)) {
        if (HeapTupleHeaderXminInvalid(tuple))
            return false;

        if (TransactionIdIsCurrentTransactionId(HeapTupleHeaderGetXmin(page, tuple))) {
            if ((tuple->t_infomask & HEAP_COMBOCID) && CheckStreamCombocid(tuple, snapshot->curcid, page))
                return true; /* delete after stream producer thread scan started */

            if (HeapTupleHeaderGetCmin(tuple, page) >= snapshot->curcid)
                return false; /* inserted after scan started */

            if (tuple->t_infomask & HEAP_XMAX_INVALID) /* xid invalid */
                return true;

            if (HEAP_XMAX_IS_LOCKED_ONLY(tuple->t_infomask, tuple->t_infomask2)) /* not deleter */
                return true;

            if (tuple->t_infomask & HEAP_XMAX_IS_MULTI) {
                TransactionId xmax = HeapTupleHeaderMultiXactGetUpdateXid(page, tuple);
                /* not LOCKED_ONLY, so it has to have an xmax */
                Assert(TransactionIdIsValid(xmax));

                /* updating subtransaction must have aborted */
                if (!TransactionIdIsCurrentTransactionId(xmax))
                    return true;
                else if (HeapTupleHeaderGetCmax(tuple, page) >= snapshot->curcid)
                    return true; /* updated after scan started */
                else
                    return false; /* updated before scan started */
            }

            if (!TransactionIdIsCurrentTransactionId(HeapTupleHeaderGetXmax(page, tuple))) {
                /* deleting subtransaction must have aborted */
                Assert(!TransactionIdDidCommit(HeapTupleHeaderGetXmax(page, tuple)));
                return true;
            }

            if (HeapTupleHeaderGetCmax(tuple, page) >= snapshot->curcid)
                return true; /* deleted after scan started */
            else
                return false; /* deleted before scan started */
        } else {
            visible = DirectReadXidVisibleInSnapshot(HeapTupleHeaderGetXmin(page, tuple), snapshot, NULL);
            if (!visible)
                return false;
        }
    } else {
        /* xmin is committed, but maybe not according to our snapshot */
        if (!HeapTupleHeaderXminFrozen(tuple) &&
            !DirectReadCommittedXidVisibleInSnapshot(HeapTupleHeaderGetXmin(page, tuple), snapshot))
            return false; /* treat as still in progress */
    }

recheck_xmax:
    if (tuple->t_infomask & HEAP_XMAX_INVALID) /* xid invalid or aborted */
        return true;

    if (HEAP_XMAX_IS_LOCKED_ONLY(tuple->t_infomask, tuple->t_infomask2))
        return true;

    if (tuple->t_infomask & HEAP_XMAX_IS_MULTI) {
        TransactionId xmax = HeapTupleHeaderMultiXactGetUpdateXid(page, tuple);
        /* not LOCKED_ONLY, so it has to have an xmax */
        Assert(TransactionIdIsValid(xmax));
        if (TransactionIdIsCurrentTransactionId(xmax)) {
            if (HeapTupleHeaderGetCmax(tuple, page) >= snapshot->curcid)
                return true; /* deleted after scan started */
            else
                return false; /* deleted before scan started */
        }
        if (TransactionIdIsInProgress(xmax))
            return true;
        if (TransactionIdDidCommit(xmax)) {
            /* updating transaction committed, but when? */
            if (!DirectReadCommittedXidVisibleInSnapshot(xmax, snapshot))
                return true; /* treat as still in progress */
            return false;
        }
        /* it must have aborted or crashed */
        return true;
    }

    if (!(tuple->t_infomask & HEAP_XMAX_COMMITTED)) {
        bool sync = false;
        TransactionId xmax = HeapTupleHeaderGetXmax(page, tuple);

        if (TransactionIdIsCurrentTransactionId(HeapTupleHeaderGetXmax(page, tuple))) {
            if (HeapTupleHeaderGetCmax(tuple, page) >= snapshot->curcid)
                return true; /* deleted after scan started */
            else
                return false; /* deleted before scan started */
        }

        visible = DirectReadXidVisibleInSnapshot(HeapTupleHeaderGetXmax(page, tuple), snapshot, &sync);
        /*
         * If sync wait, xmax may be modified by others. So we need to check xmax again after acquiring the page lock.
         */
        if (sync && (xmax != HeapTupleHeaderGetXmax(page, tuple))) {
            goto recheck_xmax;
        }

        if (!visible) {
            if (sync && (xmax != HeapTupleHeaderGetXmax(page, tuple))) {
                goto recheck_xmax;
            }
            return true; /* treat as still in progress */
        }
    } else {
        /* xmax is committed, but maybe not according to our snapshot */
        if (!DirectReadCommittedXidVisibleInSnapshot(HeapTupleHeaderGetXmax(page, tuple), snapshot))
            return true; /* treat as still in progress */
    }
    return false;
}

template<bool fromBuffer>
bool GetNextTupleFromPage(HeapScanDesc scan, Page pageptr, ScanDirection direction, OffsetNumber &lineOff, TupleTableSlot* slot)
{
    Page dp;
    if (fromBuffer) {
        LockBuffer(scan->rs_base.rs_cbuf, BUFFER_LOCK_SHARE);
        dp = (Page)BufferGetPage(scan->rs_base.rs_cbuf);
    } else {
        dp = pageptr;
    }
    int lines = PageGetMaxOffsetNumber(dp);
    ItemId lpp = HeapPageGetItemId(dp, lineOff);
    HeapTuple tuple = &(scan->rs_ctup);
    int linesLeft;
    if (ScanDirectionIsForward(direction)) {
        /* page and line_off now reference the physically next tid */
        linesLeft = lines - lineOff + 1;
    } else if (ScanDirectionIsBackward(direction)) {
        linesLeft = lineOff;
    }
    BlockNumber page = scan->rs_base.rs_cblock;
    Snapshot snapshot = scan->rs_base.rs_snapshot;

    while (linesLeft > 0) {
        if (ItemIdIsNormal(lpp)) {
            bool valid = false;

            tuple->t_data = (HeapTupleHeader)PageGetItem((Page)dp, lpp);
            tuple->t_len = ItemIdGetLength(lpp);
            ItemPointerSet(&(tuple->t_self), page, lineOff);
            HeapTupleCopyBaseFromPage(tuple, dp);

            /*
                 * if current tuple qualifies, return it.
             */
            if (fromBuffer) {
                valid = HeapTupleSatisfiesVisibility(tuple, snapshot, scan->rs_base.rs_cbuf);
                CheckForSerializableConflictOut(valid, scan->rs_base.rs_rd, (void *)tuple, scan->rs_base.rs_cbuf,
                                                snapshot);
            } else {
                valid = DirectReadHeapTupleSatisfiesVisibility(tuple, snapshot, dp);
            }

            CheckForSerializableConflictOut(valid, scan->rs_base.rs_rd, (void *)tuple, scan->rs_base.rs_cbuf,
                                            snapshot);
            if (valid) {
                /* make sure this tuple is visible and then uncompress it */
                if (scan->rs_base.rs_rd->is_compressed && HEAP_TUPLE_IS_COMPRESSED(tuple->t_data)) {
                    DECOMPRESS_HEAP_TUPLE(true, tuple, &(scan->rs_ctbuf_hdr), (scan->rs_tupdesc), dp);
                }

                if (scan->rs_base.rs_key != NULL) {
                    HeapKeyTest(tuple, (scan->rs_tupdesc), scan->rs_base.rs_nkeys, scan->rs_base.rs_key, valid);
                }
            }

            if (valid) {
                if (fromBuffer) {
                    LockBuffer(scan->rs_base.rs_cbuf, BUFFER_LOCK_UNLOCK);
                }
                Assert(slot != NULL);
                Assert(slot->tts_tupleDescriptor != NULL);
                heap_slot_store_heap_tuple(tuple, slot, scan->rs_base.rs_cbuf, false, false);

                if (ScanDirectionIsForward(direction)) {
                    ++lineOff;
                } else {
                    --lineOff;
                }
                return true;
            }
        }

        if (ScanDirectionIsForward(direction)) {
            ++lpp; /* move forward in this page's ItemId array */
            ++lineOff;
        } else {
            --lpp; /* move forward in this page's ItemId array */
            --lineOff;
        }

        --linesLeft;
    }
    if (fromBuffer) {
        LockBuffer(scan->rs_base.rs_cbuf, BUFFER_LOCK_UNLOCK);
    }
    return false;
}

class SpqBufmgrPageManager : public SpqPageManager {
public:
    uint32 start;
    uint32 end;
    HeapScanDesc scan;
    uint32 total;
    uint32 nextBlock;
    OffsetNumber lineOff;
public:
    explicit SpqBufmgrPageManager(HeapScanDesc scan, ScanDirection direction)
        : SpqPageManager(direction), scan(scan), total(0), nextBlock(InvalidBlockNumber) {}
    SpqState FetchBlocks(uint32 start, uint32 end)
    {
        this->start = start;
        this->end = end;
        total = end - start;
        nextBlock = 0;
        return SpqState::SPQ_SUCCESS;
    }

    SpqState GetNewPage()
    {
        if (nextBlock > total) {
            return SpqState::SPQ_QUEUE_EMPTY;
        }
        heapgetpage(&scan->rs_base, start + nextBlock);
        ++nextBlock;
        if (ScanDirectionIsForward(direction)) {
            lineOff = FirstOffsetNumber;
        } else if (ScanDirectionIsBackward(direction)) {
            LockBuffer(scan->rs_base.rs_cbuf, BUFFER_LOCK_SHARE);
            Page dp = (Page)BufferGetPage(scan->rs_base.rs_cbuf);
            lineOff = PageGetMaxOffsetNumber(dp);
            LockBuffer(scan->rs_base.rs_cbuf, BUFFER_LOCK_UNLOCK);
        } else {
            return SpqState::SPQ_QUERY_END;
        }
        return SpqState::SPQ_SUCCESS;
    }

    void GetTupleNoDirection(TupleTableSlot* slot)
    {
        LockBuffer(scan->rs_base.rs_cbuf, BUFFER_LOCK_SHARE);
        Page dp = (Page)BufferGetPage(scan->rs_base.rs_cbuf);
        ItemId lpp = HeapPageGetItemId(dp, lineOff);
        HeapTuple tuple = &(scan->rs_ctup);
        Assert(ItemIdIsNormal(lpp));
        tuple->t_data = (HeapTupleHeader)PageGetItem((Page)dp, lpp);
        tuple->t_len = ItemIdGetLength(lpp);

        HeapTupleCopyBaseFromPage(tuple, dp);

        if (scan->rs_base.rs_rd->is_compressed && HEAP_TUPLE_IS_COMPRESSED(tuple->t_data)) {
            DECOMPRESS_HEAP_TUPLE(true, tuple, &(scan->rs_ctbuf_hdr), (scan->rs_tupdesc), dp);
        }
        heap_slot_store_heap_tuple(tuple, slot, scan->rs_base.rs_cbuf, false, false);
        LockBuffer(scan->rs_base.rs_cbuf, BUFFER_LOCK_UNLOCK);
    }

    bool GetTupleFromPage(TupleTableSlot* slot)
    {
        // never get page yet
        if (nextBlock == InvalidBlockNumber) {
            return false;
        }

        if (ScanDirectionIsNoMovement(direction)) {
            GetTupleNoDirection(slot);
            return true;
        }

        return GetNextTupleFromPage<true>(scan, nullptr, direction, lineOff, slot);
    }

    void Rescan(TableScanDesc scanDesc)
    {
        lineOff = FirstOffsetNumber;
        total = 0;
        nextBlock = InvalidBlockNumber;
        scan = reinterpret_cast<HeapScanDesc>(scanDesc);
    }
};

struct DirectReadBuff {
    size_t size;
    uint32 start;
    uint32 locStart;
    uint32 current;
    Page currentPage;
    OffsetNumber lineOff;
    char* buff;
};

class SpqDirectReadPageManager : public SpqPageManager {
public:
    HeapScanDesc scan;
    MpmcBoundedQueue<DirectReadBuff*> pagequeue;
    DirectReadBuff *currentPages;
public:
    SpqDirectReadPageManager(HeapScanDesc scan, ScanDirection direction)
        : SpqPageManager(direction), scan(scan), pagequeue(PAGE_QUEUE_SIZE), currentPages(nullptr) {
        scan->rs_base.rs_cbuf = InvalidBuffer;
    }

    SpqState FetchBlocks(uint32 start, uint32 end)
    {
        uint32 step = 0;

        do {
            start = start + step;
            step = seg_direct_read_get_range(start);
            if (start + step - 1 >= end) {
                step = end - start + 1;
            }

            DirectReadBuff *buffer = (DirectReadBuff*)palloc(sizeof(DirectReadBuff) + BLOCKSIZE * step);
            if (buffer == nullptr) {
                elog(ERROR, "SpqDirectReadPageManager: try palloc memory failed.");
            }
            bool enqueued = false;
            for (int i = 0; i < MAX_ENQUEUE_TIME; ++i) {
                if (pagequeue.Enqueue(buffer)) {
                    enqueued = true;
                    break;
                }
            }
            if (!enqueued) {
                pfree(buffer);
                elog(ERROR, "SpqDirectReadPageManager: try push buffer to page queue failed.");
            }
            buffer->buff = (char *)(buffer + 1);
            // sync read
            seg_direct_read(scan->rs_base.rs_rd->rd_smgr, MAIN_FORKNUM, start, &step, buffer->buff, &buffer->locStart);
            buffer->start = start;
            buffer->size = step;
            buffer->current = InvalidBlockNumber;
            buffer->currentPage = buffer->buff;
        } while (start + step - 1 < end);
        return SpqState::SPQ_SUCCESS;
    }

    SpqState GetNewPage()
    {
        if (pagequeue.Empty() && currentPages == nullptr) {
            return SpqState::SPQ_QUEUE_EMPTY;
        }

        while (true) {
            // if currentPage is empty, try get a new page from pagequeue
            if (currentPages == nullptr) {
                if (!pagequeue.Dequeue(currentPages)) {
                    return SpqState::SPQ_QUEUE_EMPTY;
                }
            }

            if (currentPages->current == InvalidBlockNumber) {
                currentPages->current = 0;
            } else {
                currentPages->current++;
            }
            while (currentPages->current < currentPages->size) {
                currentPages->currentPage = currentPages->buff + BLOCKSIZE * currentPages->current;
                if (PageIsVerified(currentPages->currentPage, currentPages->locStart + currentPages->current)) {
                    if (ScanDirectionIsForward(direction)) {
                        currentPages->lineOff = FirstOffsetNumber;
                    } else if (ScanDirectionIsBackward(direction)) {
                        currentPages->lineOff = PageGetMaxOffsetNumber(currentPages->currentPage);
                    } else {
                        return SpqState::SPQ_QUERY_END;
                    }
                    return SpqState::SPQ_SUCCESS;
                }
                currentPages->current++;
            }

            pfree(currentPages);
            currentPages = nullptr;
        }
    }

    bool GetTupleFromPage(TupleTableSlot* slot)
    {
        if (currentPages == nullptr) {
            return false;
        }

        return GetNextTupleFromPage<false>(scan, currentPages->currentPage, direction, currentPages->lineOff, slot);
    }

    void Rescan(TableScanDesc scanDesc)
    {
        while (pagequeue.Dequeue(currentPages)) {
            pfree(currentPages);
        }
        currentPages = nullptr;
    }
};

class SpqLocalBlockManager : public SpqBlockManager {
public:
    uint32 instanceID;
    uint32 sliceNumber;
    uint32 nextBlock;
    uint32 maxBlockNum;
public:
    SpqLocalBlockManager(uint32 instanceID, uint32 sliceNumber, uint32 nBlocks, ScanDirection direction, uint32 step)
        : SpqBlockManager(direction, step), instanceID(instanceID), sliceNumber(sliceNumber), maxBlockNum(nBlocks)
    {
        if (ScanDirectionIsBackward(direction)) {
            nextBlock = nBlocks - instanceID * step - 1;
        } else {
            nextBlock = instanceID * step;
        }
    }

    SpqState GetBlockIDs(uint32 &start, uint32 &end)
    {
        if (ScanDirectionIsForward(direction)) {
            start = nextBlock;
            end = start + step - 1;
            nextBlock = start + sliceNumber * step;
            if (end >= maxBlockNum - 1) {
                end = maxBlockNum - 1;
            }
        } else {
            end = nextBlock;
            start = nextBlock - step + 1;
            nextBlock = end - sliceNumber * step;
            if (start > end) {
                start = 0;
                nextBlock = -1;
            }
        }
        return SPQ_SUCCESS;
    }

    bool IsBlockEnd()
    {
        if (unlikely(ScanDirectionIsNoMovement(direction))) {
            // has no direction, means will not get new page for scanning.
            return true;
        } else {
            return nextBlock >= maxBlockNum;
        }
    }

    void Rescan()
    {
        if (ScanDirectionIsBackward(direction)) {
            nextBlock = maxBlockNum - instanceID * step - 1;
        } else {
            nextBlock = instanceID * step;
        }
    }
};

class SpqAdaptiveBlockManager : public SpqBlockManager {
public:
    uint32 maxBlockNum;
    int plan_node_id;
    int64_t iter_no;
    bool isBlockEnd;
    uint32 end;
    bool connected;
    gsocket forward_conn;
    gsocket backward_conn;
public:
    SpqAdaptiveBlockManager(uint32 maxBlockNum, ScanDirection direction, int plan_node_id, uint32 step)
        : SpqBlockManager(direction, step), maxBlockNum(maxBlockNum), plan_node_id(plan_node_id)
    {
        isBlockEnd = false;
        iter_no = 0;
        end = InvalidBlockNumber;
        connected = false;
    }

    void BuildConnect()
    {
        QCConnKey key = {
            .query_id = u_sess->debug_query_id,
            .plan_node_id = plan_node_id,
            .node_id = 0,
            .type = SPQ_QC_CONNECTION,
        };

        bool found = false;
        QCConnEntry* entry;
        pthread_rwlock_rdlock(&g_instance.spq_cxt.adp_connects_lock);
        entry = (QCConnEntry*)hash_search(g_instance.spq_cxt.adp_connects, (void*)&key, HASH_FIND, &found);
        if (!found) {
            pthread_rwlock_unlock(&g_instance.spq_cxt.adp_connects_lock);
            ereport(ERROR, (errmsg("spq seq scan: can not found adaptive connection")));
        }
        backward_conn = entry->backward;
        forward_conn = entry->forward;
        pthread_rwlock_unlock(&g_instance.spq_cxt.adp_connects_lock);
    }

    SpqAdpScanPagesRes adps_get_adps_response(uint32 nblocks, int64_t iter_no)
    {
        if (!connected) {
            BuildConnect();
            connected = true;
        }
        SpqAdpScanPagesRes seqRes;
        SpqAdpScanPagesReq req = {
            .plan_node_id = plan_node_id,
            .direction = SpqBlockManager::direction,
            .nblocks = nblocks,
            .cur_scan_iter_no = iter_no,
        };

        int rc = gs_send(&forward_conn, (char*)&req, sizeof(SpqAdpScanPagesReq), -1, true);
        if (rc <= 0) {
            ereport(ERROR, (errmsg("spq seq scan: try send adaptive request failed")));
        }

        do {
            rc = gs_recv(&backward_conn, (char*)&seqRes, sizeof(SpqAdpScanPagesRes));
        } while (rc == 0 || errno == ECOMMTCPNODATA);

        if (rc < 0) {
            ereport(ERROR, (errmsg("spq seq scan: try recv adaptive request failed")));
        }

        return seqRes;
    }

    SpqState GetBlockIDs(uint32 &start, uint32 &end)
    {
        SpqAdpScanPagesRes response = adps_get_adps_response(maxBlockNum, iter_no);
        if (response.success == false) {
            isBlockEnd = true;
            return SPQ_QUERY_END;
        }
        start = response.page_start;
        end = response.page_end;
        this->end = end;

        return SPQ_SUCCESS;
    }

    bool IsBlockEnd()
    {
        if (ScanDirectionIsNoMovement(direction)) {
            // has no direction, means will not get new page for scanning.
            return true;
        } else {
            return isBlockEnd;
        }
    }

    void Rescan()
    {
        ++iter_no;
        isBlockEnd = false;
    }
};

TupleTableSlot* SpqScanNext(ScanState* node)
{
    if (node->ps.type != T_SpqSeqScanState) {
        ereport(ERROR, (errcode(ERRCODE_SYNTAX_ERROR), errmsg("non spq scan type, should not appear here")));
    }

    SpqSeqScanState* spqScan = reinterpret_cast<SpqSeqScanState*>(node);
    TupleTableSlot* slot = node->ss_ScanTupleSlot;
    EState* estate = node->ps.state;
    SpqPageManager* pageManager = reinterpret_cast<SpqPageManager*>(spqScan->pageManager);
    SpqBlockManager* blockManager = reinterpret_cast<SpqBlockManager*>(spqScan->blockManager);
    pageManager->UpdateDirection(estate->es_direction);
    pageManager->UpdateDirection(estate->es_direction);

    SpqState state = SpqState::SPQ_QUERY_FAILED;
    while (true) {
        do {
            if (pageManager->GetTupleFromPage(slot)) {
                return slot;
            }
            state = pageManager->GetNewPage();
        } while (state == SpqState::SPQ_SUCCESS);

        if (blockManager->IsBlockEnd()) {
            return NULL;
        }
        CHECK_FOR_INTERRUPTS();
        uint32 start, end;
        state = blockManager->GetBlockIDs(start, end);
        if (state == SpqState::SPQ_QUERY_END) {
            return NULL;
        }
        if (state != SpqState::SPQ_SUCCESS) {
            ereport(ERROR, (errcode(ERRCODE_SYNTAX_ERROR),
                            errmsg("block manager get block ids error, code: %d", state)));
        }
        pageManager->FetchBlocks(start, end);
        state = pageManager->GetNewPage();
        switch (state) {
            case SpqState::SPQ_SUCCESS:
            case SpqState::SPQ_QUEUE_EMPTY: {
                break;
            }
            default:
                ereport(ERROR, (errcode(ERRCODE_SYNTAX_ERROR), errmsg("spq get new page error, code: %d", state)));
        }
    }
    return NULL;
}

void GetInstanceIDAndSliceNumber(PlannedStmt* stmt, int totaldop, int &sliceNumber, int &instanceID)
{
    const char* nodeName = GetConfigOption("pgxc_node_name", false, false);
    if (nodeName == nullptr || stmt->nodesDefinition == nullptr) {
        ereport(ERROR, (errcode(ERRCODE_SYNTAX_ERROR), errmsg("can't get nodename and cluster map.")));
        return;
    }

    for (int i = 0; i < stmt->num_nodes; ++i) {
        if (strcmp(nodeName, stmt->nodesDefinition[i].nodename.data) == 0) {
            sliceNumber = stmt->num_nodes * totaldop;
            instanceID = i * totaldop + u_sess->stream_cxt.smp_id;
            return;
        }
    }
    // if is query coordinator, is possible in explain process
    if (!IS_SPQ_COORDINATOR) {
        ereport(WARNING, (errcode(ERRCODE_SYNTAX_ERROR), errmsg("can't match any nodename in cluster map.")));
    }
}

/* ----------------------------------------------------------------
 *		ExecSeqScan(node)
 *
 *		Scans the relation sequentially and returns the next qualifying
 *		tuple.
 *		We call the ExecScan() routine and pass it the appropriate
 *		access method functions.
 * ----------------------------------------------------------------
 */
TupleTableSlot* ExecSpqSeqScan(PlanState* ps)
{
    SpqSeqScanState* node = (SpqSeqScanState*)ps;

    if (!node->pageManager && !node->blockManager) {
        return SpqScanNext((SeqScanState*)node);
    }
    return ExecScan((ScanState *) node, node->ss.ScanNextMtd, (ExecScanRecheckMtd) SpqSeqRecheck);
}

/* ----------------------------------------------------------------
 *		ExecInitSeqScan
 * ----------------------------------------------------------------
 */
SpqSeqScanState* ExecInitSpqSeqScan(SpqSeqScan* node, EState* estate, int eflags)
{
    SeqScanState* seqScan = ExecInitSeqScan((SeqScan*)node, estate, eflags);
    seqScan->ps.type = T_SpqSeqScanState;
    SpqSeqScanState* spqScan = makeNode(SpqSeqScanState);
    seqScan->ps.ExecProcNode = ExecSpqSeqScan;
    errno_t rc = memcpy_s(spqScan, sizeof(SeqScanState), seqScan, sizeof(SeqScanState));

    securec_check(rc, "\0", "\0");

    HeapScanDesc scanDesc = reinterpret_cast<HeapScanDesc>(seqScan->ss_currentScanDesc);

    if (spqScan->ss.ss_currentRelation->rd_tam_ops != TableAmHeap) {
        ereport(ERROR, (errcode(ERRCODE_SYNTAX_ERROR), errmsg("error relation type.")));
    }

    if (!node->isDirectRead) {
        seqScan->ss_currentScanDesc->rs_nblocks = RelationGetNumberOfBlocks(seqScan->ss_currentScanDesc->rs_rd);
    } else if (t_thrd.spq_ctx.spq_role == ROLE_QUERY_COORDINTOR) {
        if (node->DirectReadBlkNum == InvalidBlockNumber) {
            node->isDirectRead = false;
        }
    } else {
        if (node->DirectReadBlkNum == InvalidBlockNumber) {
            ereport(ERROR,
                    (errcode(ERRCODE_SYNTAX_ERROR),
                     errmsg("DirectRead nblocks error")));
        }
        seqScan->ss_currentScanDesc->rs_nblocks = node->DirectReadBlkNum;
    }

    if (!node->isDirectRead) {
        spqScan->pageManager = New(CurrentMemoryContext) SpqBufmgrPageManager(scanDesc, estate->es_direction);
    } else {
        spqScan->pageManager = New(CurrentMemoryContext) SpqDirectReadPageManager(scanDesc, estate->es_direction);
    }

    SpqBlockManager* blockManager = nullptr;
    int fetchNum = node->isDirectRead ? FETCH_BLOCK_NUM_DIRECT : FETCH_BLOCK_NUM;
    if (node->isFullTableScan) {
        blockManager = New(CurrentMemoryContext) SpqLocalBlockManager(0,
                                                                      1,
                                                                      seqScan->ss_currentScanDesc->rs_nblocks,
                                                                      estate->es_direction,
                                                                      fetchNum);
    } else if (node->isAdaptiveScan) {
        blockManager = New(CurrentMemoryContext) SpqAdaptiveBlockManager(seqScan->ss_currentScanDesc->rs_nblocks,
                                                                         estate->es_direction,
                                                                         node->scan.plan.plan_node_id,
                                                                         fetchNum);
    } else {
        int sliceNumber;
        int instanceID;
        GetInstanceIDAndSliceNumber(estate->es_plannedstmt, node->scan.plan.dop, sliceNumber, instanceID);
        blockManager = New(CurrentMemoryContext) SpqLocalBlockManager(instanceID,
                                                                      sliceNumber,
                                                                      seqScan->ss_currentScanDesc->rs_nblocks,
                                                                      estate->es_direction,
                                                                      fetchNum);
    }
    spqScan->blockManager = blockManager;
    spqScan->ss.ScanNextMtd = SpqScanNext;

    return spqScan;
}

/* ----------------------------------------------------------------
 *		ExecEndSeqScan
 *
 *		frees any storage allocated through C routines.
 * ----------------------------------------------------------------
 */
void ExecEndSpqSeqScan(SpqSeqScanState* node)
{
    SpqPageManager* pageManager = reinterpret_cast<SpqPageManager*>(node->pageManager);
    SpqBlockManager* blockManager = reinterpret_cast<SpqBlockManager*>(node->blockManager);
    delete pageManager;
    delete blockManager;
    return ExecEndSeqScan((SeqScanState*)node);
}

/* ----------------------------------------------------------------
 *		ExecReScanSeqScan
 *
 *		Rescans the relation.
 * ----------------------------------------------------------------
 */
void ExecReScanSpqSeqScan(SpqSeqScanState* node)
{
    ExecReScanSeqScan((SeqScanState*)node);
    SpqPageManager* pageManager = reinterpret_cast<SpqPageManager*>(node->pageManager);
    SpqBlockManager* blockManager = reinterpret_cast<SpqBlockManager*>(node->blockManager);
    pageManager->Rescan(node->ss.ss_currentScanDesc);
    blockManager->Rescan();
}

void init_spqseqscan_hook()
{
    backup_init_spqscan_hook = init_spqscan_hook;
    backup_exec_spqscan_hook = exec_spqscan_hook;
    backup_end_spqscan_hook  = end_spqscan_hook;
    backup_spqscan_rescan_hook = spqscan_rescan_hook;
    init_spqscan_hook = ExecInitSpqSeqScan;
    exec_spqscan_hook = ExecSpqSeqScan;
    end_spqscan_hook  = ExecEndSpqSeqScan;
    spqscan_rescan_hook = ExecReScanSpqSeqScan;
}

void restore_spqseqscan_hook()
{
    init_spqscan_hook = backup_init_spqscan_hook;
    exec_spqscan_hook = backup_exec_spqscan_hook;
    end_spqscan_hook  = backup_end_spqscan_hook;
    spqscan_rescan_hook = backup_spqscan_rescan_hook;
}
