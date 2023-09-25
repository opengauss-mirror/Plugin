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
#include "storage/predicate.h"
#include "access/valid.h"
#include "utils/guc.h"
#include "utils/builtins.h"
#include "libpq/pqformat.h"
#include "pgxc/execRemote.h"
#include "libpq/libpq.h"
#include "storage/smgr/segment.h"
#include "mpmcqueue.h"
#include "executor/executor.h"
#include "executor/node/nodeSeqscan.h"
#include "executor/spq_seqscan.h"

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
            valid = HeapTupleSatisfiesVisibility(tuple, snapshot, scan->rs_base.rs_cbuf);

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
    Buffer bufferid;
    char* buff;
};

class SpqDirectReadBlockManager : public SpqPageManager {
public:
    HeapScanDesc scan;
    MpmcBoundedQueue<DirectReadBuff*> pagequeue;
    DirectReadBuff *currentPages;
public:
    SpqDirectReadBlockManager(HeapScanDesc scan, ScanDirection direction)
        : SpqPageManager(direction), scan(scan), pagequeue(PAGE_QUEUE_SIZE), currentPages(nullptr) {
        scan->rs_base.rs_cbuf = InvalidBuffer;
        scan->rs_base.rs_snapshot = SnapshotAny;
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
                elog(ERROR, "SpqDirectReadBlockManager: try palloc memory failed.");
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
                elog(ERROR, "SpqDirectReadBlockManager: try push buffer to page queue failed.");
            }
            buffer->buff = (char *)(buffer + 1);
            buffer->start = start;
            buffer->size = step;
            buffer->current = InvalidBlockNumber;
            buffer->currentPage = buffer->buff;
            // sync read
            seg_direct_read(scan->rs_base.rs_rd->rd_smgr, MAIN_FORKNUM, start, step, buffer->buff, buffer->locStart);
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
SpqAdpScanPagesRes adps_get_adps_response(int plan_node_id, ScanDirection direction, uint32 nblocks, int64_t iter_no)
{
    StringInfoData buffer;
    SpqAdpScanPagesRes seqRes;
    int request_size, response_size, code;
    char *encoded_msg;
    const char *msg_buffer;
    SpqAdpScanPagesReq req = {
        .plan_node_id = plan_node_id,
        .direction = direction,
        .nblocks = nblocks,
        .cur_scan_iter_no = iter_no,
    };

    request_size = sizeof(SpqAdpScanPagesReq);
    encoded_msg = (char *)palloc(request_size * 2 + 1);
    hex_encode((const char *)&req, request_size, encoded_msg);
    encoded_msg[request_size * 2] = '\0';
    pq_beginmessage(&buffer, 'a');
    pq_sendstring(&buffer, encoded_msg);
    pfree(encoded_msg);
    pq_endmessage_reuse(&buffer);

    /* Must flush this message */
    if (EOF == pq_flush()) {
      elog(ERROR, "block_iter: can`t send paging response");
      goto finish;
    }
    response_size = sizeof(SpqAdpScanPagesRes);

    code = pq_getbyte();
    if (code == EOF) {
        elog(ERROR, "block_iter: can`t get paging response");
        goto finish;
    }
    if (code != ADPS_RESPONSE_PAGE) {
        elog(ERROR, "block_iter: get an error paging response, code:%d", code);
        goto finish;
    }
    if (EOF == pq_getmessage(&buffer, response_size + sizeof(int))) {
        elog(ERROR, "block_iter: can`t get paging response");
        goto finish;
    }
    msg_buffer = pq_getmsgbytes(&buffer, response_size);
    memcpy(&seqRes, msg_buffer, response_size);

finish:
    pfree(buffer.data);

    return seqRes;
}

class SpqAdaptiveBlockManager : public SpqBlockManager {
public:
    uint32 maxBlockNum;
    int plan_node_id;
    int64_t iter_no;
    bool isBlockEnd;
    uint32 end;
public:
    SpqAdaptiveBlockManager(uint32 maxBlockNum, ScanDirection direction, int plan_node_id, uint32 step)
        : SpqBlockManager(direction, step), maxBlockNum(maxBlockNum), plan_node_id(plan_node_id)
    {
        isBlockEnd = false;
        iter_no = 0;
        end = InvalidBlockNumber;
    }

    SpqState GetBlockIDs(uint32 &start, uint32 &end)
    {
        SpqAdpScanPagesRes response = adps_get_adps_response(plan_node_id, SpqBlockManager::direction, maxBlockNum, iter_no);
        if (response.success == false) {
            isBlockEnd = true;
            return SPQ_SUCCESS;
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

    if (!node->isDirectRead) {
        if (spqScan->ss.ss_currentRelation->rd_tam_ops != TableAmHeap) {
            ereport(ERROR, (errcode(ERRCODE_SYNTAX_ERROR), errmsg("error relation type.")));
        }
        spqScan->pageManager = New(CurrentMemoryContext) SpqBufmgrPageManager(scanDesc, estate->es_direction);
    } else {
        spqScan->pageManager = New(CurrentMemoryContext) SpqDirectReadBlockManager(scanDesc, estate->es_direction);
    }

    SpqBlockManager* blockManager = nullptr;
    seqScan->ss_currentScanDesc->rs_nblocks = RelationGetNumberOfBlocks(seqScan->ss_currentScanDesc->rs_rd);
    if (node->isFullTableScan) {
        blockManager = New(CurrentMemoryContext) SpqLocalBlockManager(0,
                                                                      1,
                                                                      seqScan->ss_currentScanDesc->rs_nblocks,
                                                                      estate->es_direction,
                                                                      FETCH_BLOCK_NUM);
    } else if (node->isAdaptiveScan) {
        blockManager = New(CurrentMemoryContext) SpqAdaptiveBlockManager(seqScan->ss_currentScanDesc->rs_nblocks,
                                                                         estate->es_direction,
                                                                         node->scan.plan.plan_node_id,
                                                                         FETCH_BLOCK_NUM);
    } else {
        int sliceNumber;
        int instanceID;
        GetInstanceIDAndSliceNumber(estate->es_plannedstmt, node->scan.plan.dop, sliceNumber, instanceID);
        blockManager = New(CurrentMemoryContext) SpqLocalBlockManager(instanceID,
                                                                      sliceNumber,
                                                                      seqScan->ss_currentScanDesc->rs_nblocks,
                                                                      estate->es_direction,
                                                                      FETCH_BLOCK_NUM);
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