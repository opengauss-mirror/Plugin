/* -------------------------------------------------------------------------
*
* spq_bitmapheapscan.cpp
*	  Routines to support indexed scans of relations
*
* Portions Copyright (c) 2023 Huawei Technologies Co.,Ltd.
*
*
* IDENTIFICATION
*	  spq_bitmapheapscan.cpp
*
* -------------------------------------------------------------------------
*
* INTERFACE ROUTINES
*		ExecSpqBitmapHeapNext			workhorse for above
*		ExecSpqInitBitmapHeapScan		creates and initializes state info.
 */
#include "postgres.h"
#include "knl/knl_variable.h"
#include "executor/executor.h"

#include "access/relscan.h"
#include "executor/node/nodeBitmapHeapscan.h"
#include "executor/spq_bitmapheapscan.h"
#include "executor/spq_seqscan.h"
#include "access/tableam.h"
#include "commands/cluster.h"
#include "storage/tcap.h"

THR_LOCAL init_spqbitmapheapscan_hook_type backup_init_spqbitmapheapscan_hook = NULL;
THR_LOCAL exec_spqbitmapheapscan_hook_type backup_exec_spqbitmapheapscan_hook = NULL;

/* ----------------------------------------------------------------
 *		SpqBitmapHeapTblNext
 *
 *		Retrieve next tuple from the BitmapHeapScan node's currentRelation
 * ----------------------------------------------------------------
 */
static TupleTableSlot* SpqBitmapHeapTblNext(BitmapHeapScanState* node)
{
    ExprContext* econtext = NULL;
    TableScanDesc scan = NULL;
    TIDBitmap* tbm = NULL;
    TBMHandler tbm_handler;
    TBMIterator* tbmiterator = NULL;
    TBMIterateResult* tbmres = NULL;
    HBktTblScanDesc hpscan = NULL;

#ifdef USE_PREFETCH
    TBMIterator* prefetch_iterator = NULL;
#endif
    TupleTableSlot* slot = NULL;

    /*
     * extract necessary information from index scan node
     */
    econtext = node->ss.ps.ps_ExprContext;
    slot = node->ss.ss_ScanTupleSlot;
    if (node->ss.ss_currentRelation != NULL && RelationIsPartitionedHashBucketTable(node->ss.ss_currentRelation)) {
        Assert(node->ss.ss_currentScanDesc != NULL);
        hpscan = (HBktTblScanDesc)node->ss.ss_currentScanDesc;
        scan = (TableScanDesc)hpscan->currBktScan;
    } else {
        scan = GetTableScanDesc(node->ss.ss_currentScanDesc, node->ss.ss_currentRelation);
    }
    tbm = node->tbm;
    tbmiterator = node->tbmiterator;
    tbmres = node->tbmres;
#ifdef USE_PREFETCH
    prefetch_iterator = node->prefetch_iterator;
#endif

    /*
     * If we haven't yet performed the underlying index scan, do it, and begin
     * the iteration over the bitmap.
     *
     * For prefetching, we use *two* iterators, one for the pages we are
     * actually scanning and another that runs ahead of the first for
     * prefetching.  node->prefetch_pages tracks exactly how many pages ahead
     * the prefetch iterator is.  Also, node->prefetch_target tracks the
     * desired prefetch distance, which starts small and increases up to the
     * GUC-controlled maximum, target_prefetch_pages.  This is to avoid doing
     * a lot of prefetching in a scan that stops after a few tuples because of
     * a LIMIT.
     */
    if (tbm == NULL) {
        tbm = (TIDBitmap*)MultiExecProcNode(outerPlanState(node));
        tbm_handler = tbm_get_handler(tbm);

        if (tbm == NULL || !IsA(tbm, TIDBitmap)) {
            ereport(ERROR,
                    (errcode(ERRCODE_UNRECOGNIZED_NODE_TYPE),
                     errmodule(MOD_EXECUTOR),
                     errmsg("unrecognized result from subplan for BitmapHeapScan.")));
        }

        node->tbm = tbm;
        node->tbmiterator = tbmiterator = tbm_handler._begin_iterate(tbm);
        node->tbmres = tbmres = NULL;

#ifdef USE_PREFETCH
        if (u_sess->storage_cxt.target_prefetch_pages > 0) {
            node->prefetch_iterator = prefetch_iterator = tbm_handler._begin_iterate(tbm);
            node->prefetch_pages = 0;
            node->prefetch_target = -1;
        }
#endif
    }

    /*
     * Now tbm is not NULL, we have enough information to
     * determine whether need to assign hpscan. Also need
     * to make sure we are not scanning a virtual hashbucket
     * table.
     */
    if (hpscan == NULL && tbm_is_crossbucket(tbm) && RELATION_OWN_BUCKET(node->ss.ss_currentScanDesc->rs_rd)) {
        hpscan = (HBktTblScanDesc)node->ss.ss_currentScanDesc;
    }

    for (;;) {
        /*
         * Get next page of results if needed
         */
        if (tbmres == NULL) {
            node->tbmres = tbmres = tbm_iterate(tbmiterator);
            if (tbmres == NULL) {
                /* no more entries in the bitmap */
                break;
            }

#ifdef USE_PREFETCH
            if (node->prefetch_pages > 0) {
                /* The main iterator has closed the distance by one page */
                node->prefetch_pages--;
            } else if (prefetch_iterator != NULL) {
                /* Do not let the prefetch iterator get behind the main one */
                TBMIterateResult* tbmpre = tbm_iterate(prefetch_iterator);

                if (tbmpre == NULL || tbmpre->blockno != tbmres->blockno) {
                    ereport(ERROR,
                            (errcode(ERRCODE_DATA_EXCEPTION),
                             errmodule(MOD_EXECUTOR),
                             errmsg("prefetch and main iterators are out of sync for BitmapHeapScan.")));
                }
            }
#endif /* USE_PREFETCH */

            int rc = TableScanBitmapNextTargetRel(scan, node);
            if (rc != 0) {
                /*
                 * If the current partition is invalid,
                 * the next page is directly processed.
                 */
                tbmres = NULL;
#ifdef USE_PREFETCH
                if (rc == 1) {
                    BitmapHeapPrefetchNext(node, scan, tbm, &prefetch_iterator);
                }
#endif /* USE_PREFETCH */
                continue;
            }

            /* update bucket scan */
            if (hpscan != NULL && scan != hpscan->currBktScan) {
                scan = hpscan->currBktScan;
            }

            /*
             * Fetch the current table page and identify candidate tuples.
             */

            HeapScanDesc heap_scan = (HeapScanDesc) scan;
            if (NULL == heap_scan->spq_scan) {
                if (!TableScanBitmapNextBlock(scan, tbmres, &node->ss.ps.state->have_current_xact_date)) {
                    node->tbmres = tbmres = NULL;
                    continue;
                }
            }
            else {
                BlockNumber unitno = SPQSCAN_BlockNum2UnitNum(tbmres->blockno);
                if ((unitno % heap_scan->spq_scan->slice_num) != heap_scan->spq_scan->instance_id)
                {
                    heap_scan->rs_base.rs_ntuples = 0;
                }
                else{
                    if (!TableScanBitmapNextBlock(scan, tbmres, &node->ss.ps.state->have_current_xact_date)) {
                        node->tbmres = tbmres = NULL;
                        continue;
                    }
                }
            }

            if (tbmres->ntuples >= 0) {
                node->exact_pages++;
            } else {
                node->lossy_pages++;
            }

#ifdef USE_PREFETCH

            /*
             * Increase prefetch target if it's not yet at the max.  Note that
             * we will increase it to zero after fetching the very first
             * page/tuple, then to one after the second tuple is fetched, then
             * it doubles as later pages are fetched.
             */
            if (node->prefetch_target >= u_sess->storage_cxt.target_prefetch_pages)
                /* don't increase any further */;
            else if (node->prefetch_target >= u_sess->storage_cxt.target_prefetch_pages / 2)
                node->prefetch_target = u_sess->storage_cxt.target_prefetch_pages;
            else if (node->prefetch_target > 0)
                node->prefetch_target *= 2;
            else
                node->prefetch_target++;
#endif /* USE_PREFETCH */
        } else {
            /*
             * Continuing in previously obtained page.
             */

#ifdef USE_PREFETCH

            /*
             * Try to prefetch at least a few pages even before we get to the
             * second page if we don't stop reading after the first tuple.
             */
            if (node->prefetch_target < u_sess->storage_cxt.target_prefetch_pages)
                node->prefetch_target++;
#endif /* USE_PREFETCH */
        }

#ifdef USE_PREFETCH
        BitmapHeapPrefetchNext(node, scan, tbm, &prefetch_iterator);
#endif /* USE_PREFETCH */

        /*
         * Attempt to fetch tuple from AM.
         */
        if (!TableScanBitmapNextTuple(scan, tbmres, slot)) {
            /* nothing more to look at on this page */
            node->tbmres = tbmres = NULL;
            continue;
        }

        /*
         * If we are using lossy info, we have to recheck the qual conditions
         * at every tuple.
         */
        if (tbmres->recheck) {
            econtext->ecxt_scantuple = slot;
            ResetExprContext(econtext);

            if (!ExecQual(node->bitmapqualorig, econtext)) {
                /* Fails recheck, so drop it and loop back for another */
                InstrCountFiltered2(node, 1);
                (void)ExecClearTuple(slot);
                continue;
            }
        }

        /* OK to return this tuple */
        return slot;
    }

    /*
     * if we get here it means we are at the end of the scan..
     */
    return ExecClearTuple(slot);
}

TableScanDesc add_spq_scan(BitmapHeapScanState* node, TableScanDesc scan) {
    EState* estate = node->ss.ps.state;
    HeapScanDesc heap_scan = (HeapScanDesc) scan;
    SPQScanDesc	spq_scan = NULL;
    int slice_num = 0;
    int instance_id = 0;

    spq_scan = (SPQScanDescData*)palloc0(sizeof(SPQScanDescData));

    GetInstanceIDAndSliceNumber(estate->es_plannedstmt, ((Plan*)node->ss.ps.plan)->dop, slice_num, instance_id);

    spq_scan->slice_num = slice_num;
    spq_scan->instance_id = instance_id;
    heap_scan->spq_scan = spq_scan;

    return (TableScanDesc) scan;
}

static inline void InitSpqBitmapHeapScanNextMtd(BitmapHeapScanState* bmstate)
{
    if (RELATION_OWN_BUCKET(bmstate->ss.ss_currentRelation)) {
        bmstate->ss.ScanNextMtd = (ExecScanAccessMtd)BitmapHbucketTblNext;
        return;
    }
    bmstate->ss.ScanNextMtd = (ExecScanAccessMtd)SpqBitmapHeapTblNext;
}

/* ----------------------------------------------------------------
 *		ExecInitSpqBitmapHeapScan
 *
 *		Initializes the scan's state information.
 * ----------------------------------------------------------------
 */
BitmapHeapScanState* ExecInitSpqBitmapHeapScan(SpqBitmapHeapScan* spqnode, EState* estate, int eflags)
{
    BitmapHeapScan* node = (BitmapHeapScan*) spqnode;
    BitmapHeapScanState* scanstate = NULL;
    Relation currentRelation;
    bool isUstoreRel = false;
    Snapshot scanSnap;

    /* check for unsupported flags */
    Assert(!(eflags & (EXEC_FLAG_BACKWARD | EXEC_FLAG_MARK)));

    /*
     * Assert caller didn't ask for an unsafe snapshot --- see comments at
     * head of file.
     */
    Assert(IsMVCCSnapshot(estate->es_snapshot));

    /*
     * create state structure
     */
    scanstate = makeNode(BitmapHeapScanState);
    scanstate->ss.ps.plan = (Plan*)node;
    scanstate->ss.ps.state = estate;

    scanstate->tbm = NULL;
    scanstate->tbmiterator = NULL;
    scanstate->tbmres = NULL;
    scanstate->exact_pages = 0.0;
    scanstate->lossy_pages = 0.0;
    scanstate->prefetch_iterator = NULL;
    scanstate->prefetch_pages = 0;
    scanstate->prefetch_target = 0;
    scanstate->ss.isPartTbl = node->scan.isPartTbl;
    scanstate->ss.currentSlot = 0;
    scanstate->ss.partScanDirection = node->scan.partScanDirection;
    scanstate->ss.ps.ExecProcNode = ExecBitmapHeapScan;

    /* initialize Global partition index scan information */
    GPIScanInit(&scanstate->gpi_scan);

    /* initialize cross-bucket index scan information */
    cbi_scan_init(&scanstate->cbi_scan);

    /*
     * Miscellaneous initialization
     *
     * create expression context for node
     */
    ExecAssignExprContext(estate, &scanstate->ss.ps);

    /*
     * initialize child expressions
     */
    if (estate->es_is_flt_frame) {
        scanstate->ss.ps.qual = (List*)ExecInitQualByFlatten(node->scan.plan.qual, (PlanState*)scanstate);
        scanstate->bitmapqualorig = (List*)ExecInitQualByFlatten(node->bitmapqualorig, (PlanState*)scanstate);
    } else {
        scanstate->ss.ps.targetlist = (List*)ExecInitExprByRecursion((Expr*)node->scan.plan.targetlist, (PlanState*)scanstate);
        scanstate->ss.ps.qual = (List*)ExecInitExprByRecursion((Expr*)node->scan.plan.qual, (PlanState*)scanstate);
        scanstate->bitmapqualorig = (List*)ExecInitExprByRecursion((Expr*)node->bitmapqualorig, (PlanState*)scanstate);
    }

    /*
     * open the base relation and acquire appropriate lock on it.
     */
    currentRelation = ExecOpenScanRelation(estate, node->scan.scanrelid);

    scanstate->ss.ss_currentRelation = currentRelation;
    scanstate->gpi_scan->parentRelation = currentRelation;

    isUstoreRel = RelationIsUstoreFormat(currentRelation);

    /*
     * tuple table initialization
     */
    ExecInitResultTupleSlot(estate, &scanstate->ss.ps, currentRelation->rd_tam_ops);
    ExecInitScanTupleSlot(estate, &scanstate->ss, currentRelation->rd_tam_ops);

    InitSpqBitmapHeapScanNextMtd(scanstate);

    /*
     * Choose user-specified snapshot if TimeCapsule clause exists, otherwise
     * estate->es_snapshot instead.
     */
    scanSnap = TvChooseScanSnap(currentRelation, &node->scan, &scanstate->ss);

    /*
     * Even though we aren't going to do a conventional seqscan, it is useful
     * to create a HeapScanDesc --- most of the fields in it are usable.
     */
    if (scanstate->ss.isPartTbl) {
        scanstate->ss.ss_currentScanDesc = NULL;
        ExecInitPartitionForBitmapHeapScan(scanstate, estate);

        if (node->scan.itrs > 0) {
            Partition partition = NULL;
            Relation partitiontrel = NULL;

            if (scanstate->ss.partitions != NIL) {
                /* construct a dummy table relation with the next table partition for scan */
                partition = (Partition)list_nth(scanstate->ss.partitions, 0);
                partitiontrel = partitionGetRelation(currentRelation, partition);
                scanstate->ss.ss_currentPartition = partitiontrel;

                /*
                 * Verify if a DDL operation that froze all tuples in the relation
                 * occured after taking the snapshot. Skip for explain only commands.
                 */
                if (isUstoreRel && !(eflags & EXEC_FLAG_EXPLAIN_ONLY)) {
                    TransactionId relfrozenxid64 = InvalidTransactionId;
                    getPartitionRelxids(partitiontrel, &relfrozenxid64);
                    if (TransactionIdPrecedes(FirstNormalTransactionId, scanSnap->xmax) &&
                        !TransactionIdIsCurrentTransactionId(relfrozenxid64) &&
                        TransactionIdPrecedes(scanSnap->xmax, relfrozenxid64)) {
                        ereport(ERROR, (errcode(ERRCODE_SNAPSHOT_INVALID),
                                        (errmsg("Snapshot too old, BitmapHeapScan is PartTbl, the info: snapxmax is %lu, "
                                                "snapxmin is %lu, csn is %lu, relfrozenxid64 is %lu, globalRecycleXid is %lu.",
                                                scanSnap->xmax, scanSnap->xmin, scanSnap->snapshotcsn, relfrozenxid64,
                                                g_instance.undo_cxt.globalRecycleXid))));
                    }
                }

                if (IS_SPQ_EXECUTOR && scanstate->ss.ps.plan->spq_scan_partial){
                    scanstate->ss.ss_currentScanDesc = add_spq_scan(scanstate, scan_handler_tbl_beginscan_bm(
                                                                                   partitiontrel, scanSnap, 0, NULL, &scanstate->ss));
                }
                else
                    scanstate->ss.ss_currentScanDesc =
                        scan_handler_tbl_beginscan_bm(partitiontrel, scanSnap, 0, NULL, &scanstate->ss);


            }
        }
    } else {
        if (!isUstoreRel) {
            if (IS_SPQ_EXECUTOR && scanstate->ss.ps.plan->spq_scan_partial) {
                scanstate->ss.ss_currentScanDesc = add_spq_scan(scanstate, scan_handler_tbl_beginscan_bm(
                                                                               currentRelation, scanSnap, 0, NULL, &scanstate->ss));
            }
            else
                scanstate->ss.ss_currentScanDesc =
                    scan_handler_tbl_beginscan_bm(currentRelation, scanSnap, 0, NULL, &scanstate->ss);
        } else {
            /*
             * Verify if a DDL operation that froze all tuples in the relation
             * occured after taking the snapshot. Skip for explain only commands.
             */
            if (!(eflags & EXEC_FLAG_EXPLAIN_ONLY)) {
                TransactionId relfrozenxid64 = InvalidTransactionId;
                getRelationRelxids(currentRelation, &relfrozenxid64);
                if (TransactionIdPrecedes(FirstNormalTransactionId, scanSnap->xmax) &&
                    !TransactionIdIsCurrentTransactionId(relfrozenxid64) &&
                    TransactionIdPrecedes(scanSnap->xmax, relfrozenxid64)) {
                    ereport(ERROR, (errcode(ERRCODE_SNAPSHOT_INVALID),
                                    (errmsg("Snapshot too old, BitmapHeapScan is not PartTbl, the info: snapxmax is %lu, "
                                            "snapxmin is %lu, csn is %lu, relfrozenxid64 is %lu, globalRecycleXid is %lu.",
                                            scanSnap->xmax, scanSnap->xmin, scanSnap->snapshotcsn, relfrozenxid64,
                                            g_instance.undo_cxt.globalRecycleXid))));
                }
            }

            scanstate->ss.ss_currentScanDesc = UHeapBeginScan(currentRelation, scanSnap, 0, NULL);
        }
    }
    if (scanstate->ss.ss_currentScanDesc == NULL) {
        scanstate->ss.ps.stubType = PST_Scan;
    }

    /*
     * get the scan type from the relation descriptor.
     */
    ExecAssignScanType(&scanstate->ss, RelationGetDescr(currentRelation));

    /*
     * Initialize result tuple type and projection info.
     */
    ExecAssignResultTypeFromTL(
        &scanstate->ss.ps,
        scanstate->ss.ss_ScanTupleSlot->tts_tupleDescriptor->td_tam_ops);

    ExecAssignScanProjectionInfo(&scanstate->ss);

    Assert(scanstate->ss.ps.ps_ResultTupleSlot->tts_tupleDescriptor->td_tam_ops);

    /*
     * initialize child nodes
     *
     * We do this last because the child nodes will open indexscans on our
     * relation's indexes, and we want to be sure we have acquired a lock on
     * the relation first.
     */
    outerPlanState(scanstate) = ExecInitNode(outerPlan(node), estate, eflags);

    /*
     * all done.
     */
    return scanstate;
}

void init_spqbitmapheapscan_hook()
{
    backup_init_spqbitmapheapscan_hook = init_bitmapheapscan_hook;
    backup_exec_spqbitmapheapscan_hook = exec_bitmapheapscan_hook;
    init_bitmapheapscan_hook = ExecInitSpqBitmapHeapScan;
    exec_bitmapheapscan_hook = ExecBitmapHeapScan;
}

void restore_spqbitmapheapscan_hook()
{
    init_bitmapheapscan_hook = backup_init_spqbitmapheapscan_hook;
    exec_bitmapheapscan_hook = backup_exec_spqbitmapheapscan_hook;
}
