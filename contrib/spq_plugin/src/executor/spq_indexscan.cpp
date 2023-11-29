/* -------------------------------------------------------------------------
 *
 * spq_indexscan.cpp
 *	  Routines to support indexed scans of relations
 *
 * Portions Copyright (c) 2023 Huawei Technologies Co.,Ltd.
 *
 *
 * IDENTIFICATION
 *	  spq_indexscan.cpp
 *
 * -------------------------------------------------------------------------
 *
 * INTERFACE ROUTINES
 *		ExecSpqIndexScan			scans a relation using an index
 *		SpqIndexNext				retrieve next tuple using index
 */
#include "postgres.h"
#include "knl/knl_variable.h"
#include "executor/executor.h"
#include "executor/node/nodeIndexscan.h"
#include "executor/spq_indexscan.h"
#include "executor/spq_seqscan.h"
#include "access/tableam.h"

THR_LOCAL init_spqindexscan_hook_type backup_init_spqindexscan_hook = NULL;
THR_LOCAL exec_spqindexscan_hook_type backup_exec_spqindexscan_hook = NULL;

bool IndexRecheck(IndexScanState* node, TupleTableSlot* slot)
{
    ExprContext* econtext = NULL;

    /*
     * extract necessary information from index scan node
     */
    econtext = node->ss.ps.ps_ExprContext;

    /* Does the tuple meet the indexqual condition? */
    econtext->ecxt_scantuple = slot;

    ResetExprContext(econtext);

    return ExecQual(node->indexqualorig, econtext, false);
}

/* ----------------------------------------------------------------
 *		SpqIndexNext
 *
 *		Retrieve a tuple from the IndexScan node's current_relation
 *		using the index specified in the IndexScanState information.
 * ----------------------------------------------------------------
 */

TupleTableSlot* SpqIndexNext(IndexScanState* node)
{
    EState* estate = NULL;
    ExprContext* econtext = NULL;
    ScanDirection direction;
    IndexScanDesc scandesc;
    HeapTuple tuple;
    TupleTableSlot* slot = NULL;
    bool isUstore = false;

    /*
     * extract necessary information from index scan node
     */
    estate = node->ss.ps.state;
    direction = estate->es_direction;
    /* flip direction if this is an overall backward scan */
    if (ScanDirectionIsBackward(((IndexScan*)node->ss.ps.plan)->indexorderdir)) {
        if (ScanDirectionIsForward(direction))
            direction = BackwardScanDirection;
        else if (ScanDirectionIsBackward(direction))
            direction = ForwardScanDirection;
    }
    scandesc = node->iss_ScanDesc;
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

    isUstore = RelationIsUstoreFormat(node->ss.ss_currentRelation);

    /*
     * ok, now that we have what we need, fetch the next tuple.
     */
    // we should change abs_idx_getnext to call IdxScanAm(scan)->idx_getnext and channge .idx_getnext in g_HeapIdxAm to
    // IndexGetnextSlot
    while (true) {
        CHECK_FOR_INTERRUPTS();

        IndexScanDesc indexScan = GetIndexScanDesc(scandesc);
        if (isUstore) {
            if (!IndexGetnextSlot(scandesc, direction, slot, &node->ss.ps.state->have_current_xact_date)) {
                break;
            }
        } else {
            if ((tuple = scan_handler_idx_getnext(scandesc, direction, InvalidOid, InvalidBktId,
                                                  &node->ss.ps.state->have_current_xact_date)) == NULL) {
                break;
            }
            /* Update indexScan, because hashbucket may switch current index in scan_handler_idx_getnext */
            indexScan = GetIndexScanDesc(scandesc);
            /*
             * Store the scanned tuple in the scan tuple slot of the scan state.
             * Note: we pass 'false' because tuples returned by amgetnext are
             * pointers onto disk pages and must not be pfree_ext()'d.
             */
            (void)ExecStoreTuple(tuple, /* tuple to store */
                slot,                   /* slot to store in */
                indexScan->xs_cbuf,     /* buffer containing tuple */
                false);                 /* don't pfree */
        }

        /*
         * If the index was lossy, we have to recheck the index quals using
         * the fetched tuple.
         */
        if (indexScan->xs_recheck) {
            econtext->ecxt_scantuple = slot;
            ResetExprContext(econtext);
            if (!ExecQual(node->indexqualorig, econtext, false)) {
                /* Fails recheck, so drop it and loop back for another */
                InstrCountFiltered2(node, 1);
                continue;
            }
        }

        return slot;
    }

    /*
     * if we get here it means the index scan failed so we are at the end of
     * the scan..
     */
    return ExecClearTuple(slot);
}

/* ----------------------------------------------------------------
 *		ExecIndexScan(node)
 * ----------------------------------------------------------------
 */
TupleTableSlot* ExecSpqIndexScan(PlanState* state)
{
    IndexScanState* node = castNode(IndexScanState, state);
    /*
     * If we have runtime keys and they've not already been set up, do it now.
     */
    if (node->iss_NumRuntimeKeys != 0 && (!node->iss_RuntimeKeysReady || (u_sess->parser_cxt.has_set_uservar && DB_IS_CMPT(B_FORMAT)))) {
        /*
         * set a flag for partitioned table, so we can deal with it specially
         * when we rescan the partitioned table
         */
        if (node->ss.isPartTbl) {
            if (PointerIsValid(node->ss.partitions)) {
                node->ss.ss_ReScan = true;
                ExecReScan((PlanState*)node);
            }
        } else {
            ExecReScan((PlanState*)node);
        }
    }

    return ExecScan(&node->ss, (ExecScanAccessMtd)SpqIndexNext, (ExecScanRecheckMtd)IndexRecheck);
}


IndexScanState* ExecInitSpqIndexScan(SpqIndexScan* node, EState* estate, int eflags)
{
    IndexScanState* indexScan = ExecInitIndexScan((IndexScan*)node, estate, eflags);
    indexScan->ss.ps.type = T_IndexScanState;
    indexScan->ss.ps.ExecProcNode = ExecSpqIndexScan;
    return indexScan;
}

void init_spqindexscan_hook()
{
    backup_init_spqindexscan_hook = init_indexscan_hook;
    backup_exec_spqindexscan_hook = exec_indexscan_hook;
    init_indexscan_hook = ExecInitSpqIndexScan;
    exec_indexscan_hook = ExecSpqIndexScan;
}

void restore_spqindexscan_hook()
{
    init_indexscan_hook = backup_init_spqindexscan_hook;
    exec_indexscan_hook = backup_exec_spqindexscan_hook;
}
