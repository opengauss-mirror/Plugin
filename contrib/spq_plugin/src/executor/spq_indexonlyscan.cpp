/* -------------------------------------------------------------------------
 *
 * spq_indexonlyscan.cpp
 *	  Routines to support indexed scans of relations
 *
 * Portions Copyright (c) 2023 Huawei Technologies Co.,Ltd.
 *
 *
 * IDENTIFICATION
 *	  spq_indexonlyscan.cpp
 *
 * -------------------------------------------------------------------------
 *
 * INTERFACE ROUTINES
 *		ExecSpqIndexOnlyScan			scans a relation using an index
 *		SpqIndexOnlyNext				retrieve next tuple using index
 */
#include "postgres.h"
#include "knl/knl_variable.h"
#include "executor/executor.h"
#include "executor/node/nodeIndexonlyscan.h"
#include "executor/spq_indexonlyscan.h"
#include "executor/spq_seqscan.h"
#include "access/tableam.h"
#include "access/visibilitymap.h"
#include "storage/predicate.h"

THR_LOCAL init_spqindexonlyscan_hook_type backup_init_spqindexonlyscan_hook = NULL;
THR_LOCAL exec_spqindexonlyscan_hook_type backup_exec_spqindexonlyscan_hook = NULL;

static inline void ReleaseNodeVMBuffer(IndexOnlyScanState* node)
{
    if (node != NULL && (node->ioss_VMBuffer != InvalidBuffer)) {
        ReleaseBuffer(node->ioss_VMBuffer);
        node->ioss_VMBuffer = InvalidBuffer;
    }
}

inline bool ExecGPIGetNextPartRelation(IndexOnlyScanState* node, IndexScanDesc indexScan)
{
    if (IndexScanNeedSwitchPartRel(indexScan)) {
        /* Release VM buffer pin, if any. */
        ReleaseNodeVMBuffer(node);
        /* Change the heapRelation in indexScanDesc to Partition Relation of current index */
        if (!GPIGetNextPartRelation(indexScan->xs_gpi_scan, CurrentMemoryContext, AccessShareLock)) {
            return false;
        }
        indexScan->heapRelation = indexScan->xs_gpi_scan->fakePartRelation;
    }

    return true;
}

bool IndexOnlyRecheck(IndexOnlyScanState* node, TupleTableSlot* slot)
{
    ereport(ERROR,
            (errcode(ERRCODE_FEATURE_NOT_SUPPORTED), errmsg("EvalPlanQual recheck is not supported in index-only scans")));
    return false; /* keep compiler quiet */
}

/* ----------------------------------------------------------------
 *		SpqIndexOnlyNext
 *
 *		Retrieve a tuple from the IndexOnlyScan node's index.
 * ----------------------------------------------------------------
 */
TupleTableSlot* SpqIndexOnlyNext(IndexOnlyScanState* node)
{
    EState* estate = NULL;
    ExprContext* econtext = NULL;
    ScanDirection direction;
    IndexScanDesc scandesc;
    TupleTableSlot* slot = NULL;
    TupleTableSlot* tmpslot = NULL;
    ItemPointer tid;
    bool isVersionScan = node->ss.isVersionScan;
    bool isUHeap = false;

    /*
     * extract necessary information from index scan node
     */
    estate = node->ss.ps.state;
    direction = estate->es_direction;
    /* flip direction if this is an overall backward scan */
    if (ScanDirectionIsBackward(((IndexOnlyScan*)node->ss.ps.plan)->indexorderdir)) {
        if (ScanDirectionIsForward(direction))
            direction = BackwardScanDirection;
        else if (ScanDirectionIsBackward(direction))
            direction = ForwardScanDirection;
    }
    scandesc = node->ioss_ScanDesc;
    econtext = node->ss.ps.ps_ExprContext;
    slot = node->ss.ss_ScanTupleSlot;

    if (IS_SPQ_EXECUTOR && scandesc->spq_scan == NULL && node->ss.ps.plan->spq_scan_partial) {
        SPQScanDesc	spq_scan = NULL;
        spq_scan = (SPQScanDescData*)palloc0(sizeof(SPQScanDescData));
        GetInstanceIDAndSliceNumber(estate->es_plannedstmt, ((Plan *)node->ss.ps.plan)->dop,
                                    spq_scan->slice_num,
                                    spq_scan->instance_id);
        scandesc->spq_scan = spq_scan;
    }

    isUHeap = RelationIsUstoreFormat(node->ss.ss_currentRelation);
    if (isUHeap) {
        tmpslot = MakeSingleTupleTableSlot(RelationGetDescr(scandesc->heapRelation),
                                           false, scandesc->heapRelation->rd_tam_ops);
    }
    /*
     * OK, now that we have what we need, fetch the next tuple.
     */
    while ((tid = scan_handler_idx_getnext_tid(scandesc, direction)) != NULL) {
        HeapTuple tuple = NULL;
        IndexScanDesc indexScan = GetIndexScanDesc(scandesc);

        CHECK_FOR_INTERRUPTS();

        /*
         * We can skip the heap fetch if the TID references a heap page on
         * which all tuples are known visible to everybody.  In any case,
         * we'll use the index tuple not the heap tuple as the data source.
         *
         * Note on Memory Ordering Effects: visibilitymap_test does not lock
         * the visibility map buffer, and therefore the result we read here
         * could be slightly stale.  However, it can't be stale enough to
         * matter.	It suffices to show that (1) there is a read barrier
         * between the time we read the index TID and the time we test the
         * visibility map; and (2) there is a write barrier between the time
         * some other concurrent process clears the visibility map bit and the
         * time it inserts the index TID.  Since acquiring or releasing a
         * LWLock interposes a full barrier, this is easy to show: (1) is
         * satisfied by the release of the index buffer content lock after
         * reading the TID; and (2) is satisfied by the acquisition of the
         * buffer content lock in order to insert the TID.
         */
        if (!ExecGPIGetNextPartRelation(node, indexScan)) {
            continue;
        }
        if (!ExecCBIFixHBktRel(scandesc, &node->ioss_VMBuffer)) {
            continue;
        }

        if (isUHeap) {
            /* ustore with multi-version ubtree only recheck IndexTuple when xs_recheck_itup is set */
            if (indexScan->xs_recheck_itup) {
                node->ioss_HeapFetches++;
                if (!IndexFetchUHeap(indexScan, tmpslot, &node->ss.ps.state->have_current_xact_date)) {
                    continue; /* this TID indicate no visible tuple */
                }
                if (!RecheckIndexTuple(indexScan, tmpslot)) {
                    continue; /* the visible version not match the IndexTuple */
                }
            }
        } else if (isVersionScan ||
                   !visibilitymap_test(indexScan->heapRelation, ItemPointerGetBlockNumber(tid), &node->ioss_VMBuffer)) {
            /* IMPORTANT: We ALWAYS visit the heap to check visibility in VERSION SCAN. */
            /*
             * Rats, we have to visit the heap to check visibility.
             */
            node->ioss_HeapFetches++;
            if (!IndexFetchSlot(indexScan, slot, isUHeap, &node->ss.ps.state->have_current_xact_date)) {
#ifdef DEBUG_INPLACE
                /* Now ustore does not support hash bucket table */
                Assert(indexScan == scandesc);
                /* Record whether the invisible heap tuple is all dead or not */
                if (indexScan->kill_prior_tuple)
                    INPLACEHEAPSTAT_COUNT_INDEX_FETCH_TUPLE(INPLACEHEAP_TUPLE_INVISIBLE_ALL_DEAD);
                else
                    INPLACEHEAPSTAT_COUNT_INDEX_FETCH_TUPLE(INPLACEHEAP_TUPLE_INVISIBLE_NOT_ALL_DEAD);
#endif
                continue; /* no visible tuple, try next index entry */
            }

#ifdef DEBUG_INPLACE
            Assert(indexScan == scandesc);
            Assert(!indexScan->kill_prior_tuple);
            /* Record Heap Tuple is visible */
            INPLACEHEAPSTAT_COUNT_INDEX_FETCH_TUPLE(INPLACEHEAP_TUPLE_VISIBLE);
#endif


            /*
             * Only MVCC snapshots are supported here, so there should be no
             * need to keep following the HOT chain once a visible entry has
             * been found.	If we did want to allow that, we'd need to keep
             * more state to remember not to call index_getnext_tid next time.
             */
            if (indexScan->xs_continue_hot)
                ereport(ERROR,
                        (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                         errmsg("non-MVCC snapshots are not supported in index-only scans")));

            /*
             * Note: at this point we are holding a pin on the heap page, as
             * recorded in scandesc->xs_cbuf.  We could release that pin now,
             * but it's not clear whether it's a win to do so.	The next index
             * entry might require a visit to the same heap page.
             */
        }

        /*
         * Fill the scan tuple slot with data from the index.
         */
        StoreIndexTuple(slot, indexScan->xs_itup, indexScan->xs_itupdesc);

        /*
         * If the index was lossy, we have to recheck the index quals.
         * (Currently, this can never happen, but we should support the case
         * for possible future use, eg with GiST indexes.)
         */
        if (indexScan->xs_recheck) {
            econtext->ecxt_scantuple = slot;
            ResetExprContext(econtext);
            if (!ExecQual(node->indexqual, econtext, false)) {
                /* Fails recheck, so drop it and loop back for another */
                InstrCountFiltered2(node, 1);
                continue;
            }
        }

        /*
         * Predicate locks for index-only scans must be acquired at the page
         * level when the heap is not accessed, since tuple-level predicate
         * locks need the tuple's xmin value.  If we had to visit the tuple
         * anyway, then we already have the tuple-level lock and can skip the
         * page lock.
         */
        if (tuple == NULL)
            PredicateLockPage(indexScan->heapRelation, ItemPointerGetBlockNumber(tid), estate->es_snapshot);
        if (isUHeap) {
            ExecDropSingleTupleTableSlot(tmpslot);
        }
        return slot;
    }

    /*
     * if we get here it means the index scan failed so we are at the end of
     * the scan..
     */
    if (isUHeap) {
        ExecDropSingleTupleTableSlot(tmpslot);
    }
    return ExecClearTuple(slot);
}

/* ----------------------------------------------------------------
 *		ExecIndexScan(node)
 * ----------------------------------------------------------------
 */
TupleTableSlot* ExecSpqIndexOnlyScan(PlanState* state)
{
    IndexOnlyScanState* node = castNode(IndexOnlyScanState, state);
    /*
     * If we have runtime keys and they've not already been set up, do it now.
     */
    if (node->ioss_NumRuntimeKeys != 0 && !node->ioss_RuntimeKeysReady) {
        if (node->ss.isPartTbl) {
            if (PointerIsValid(node->ss.partitions)) {
                node->ss.ss_ReScan = true;

                ExecReScan((PlanState*)node);
            }
        } else {
            ExecReScan((PlanState*)node);
        }
    }

    return ExecScan(&node->ss, (ExecScanAccessMtd)SpqIndexOnlyNext, (ExecScanRecheckMtd)IndexOnlyRecheck);
}


IndexOnlyScanState* ExecInitSpqIndexOnlyScan(SpqIndexOnlyScan* node, EState* estate, int eflags)
{
    IndexOnlyScanState* indexScan = ExecInitIndexOnlyScan((IndexOnlyScan*)node, estate, eflags);
    indexScan->ss.ps.type = T_IndexOnlyScanState;
    indexScan->ss.ps.ExecProcNode = ExecSpqIndexOnlyScan;
    return indexScan;
}

void init_spqindexonlyscan_hook()
{
    backup_init_spqindexonlyscan_hook = init_indexonlyscan_hook;
    backup_exec_spqindexonlyscan_hook = exec_indexonlyscan_hook;
    init_indexonlyscan_hook = ExecInitSpqIndexOnlyScan;
    exec_indexonlyscan_hook = ExecSpqIndexOnlyScan;
}

void restore_spqindexonlyscan_hook()
{
    init_indexonlyscan_hook = backup_init_spqindexonlyscan_hook;
    exec_indexonlyscan_hook = backup_exec_spqindexonlyscan_hook;
}
