/* -------------------------------------------------------------------------
 *
 * spq_mutate.cpp
 * 	  Parallelize a PostgreSQL sequential plan tree.
 *
 * Portions Copyright (c) 2004-2008, Greenplum inc
 * Portions Copyright (c) 2012-Present VMware, Inc. or its affiliates.
 *
 *
 * IDENTIFICATION
 *
 * -------------------------------------------------------------------------
 */

#include "postgres.h"
#include "spq/spq_mutate.h"
#include "nodes/relation.h"
#include "optimizer/pathnode.h"
#include "nodes/makefuncs.h"
#include "optimizer/planner.h"
#include "optimizer/planmain.h"
#include "utils/builtins.h"
#include "nodes/pg_list.h"
#include "optimizer/var.h"
#include "optimizer/tlist.h"
#include "utils/spccache.h"
#include "optimizer/cost.h"
#include "spq/spq_util.h"
#include "parser/parsetree.h"

Plan* make_gather_stream(PlannerInfo* root, Plan *subplan, Motion *motion, PlannedStmt *result);

/*
 * Is the node a "subclass" of Plan?
 */
bool is_plan_node(Node *node)
{
    if (node == NULL)
        return false;

    if (nodeTag(node) >= T_Plan_Start && nodeTag(node) < T_Plan_End)
        return true;
    return false;
}

typedef bool (*SHAREINPUT_MUTATOR)(Node *node, PlannerInfo *root, bool fPop);
static void shareinput_walker(SHAREINPUT_MUTATOR f, Node *node, PlannerInfo *root)
{
    Plan *plan = NULL;
    bool recursive_down;

    if (node == NULL)
        return;

    if (IsA(node, List)) {
        List *l = (List *)node;
        ListCell *lc;

        foreach (lc, l) {
            Node *n = (Node *)lfirst(lc);

            shareinput_walker(f, n, root);
        }
        return;
    }

    if (!is_plan_node(node))
        return;

    plan = (Plan *)node;
    recursive_down = (*f)(node, root, false);

    if (recursive_down) {
        ListCell* lc = NULL;
        List* subplans = root->glob->subplans;
        if (subplans) {
            List* subplan_list = check_subplan_list(plan);
            foreach (lc, subplan_list) {
                Node* node = (Node*)lfirst(lc);
                Plan* initNode = NULL;
                SubPlan* subplan = NULL;
                if (IsA(node, SubPlan)) {
                    subplan = (SubPlan*)lfirst(lc);
                    initNode = (Plan*)list_nth(subplans, subplan->plan_id - 1);
                    shareinput_walker(f, (Node *)initNode, root);
                }
            }
            list_free_ext(subplan_list);
        }

        List* initplans = plan->initPlan;
        if (initplans) {
            foreach (lc, initplans) {
                Plan* initplan = (Plan*)lfirst(lc);
                shareinput_walker(f, (Node *)initplan, root);
            }
        }

        if (IsA(node, Append)) {
            ListCell *cell;
            Append *app = (Append *)node;

            foreach (cell, app->appendplans)
                shareinput_walker(f, (Node *)lfirst(cell), root);
        } else if (IsA(node, ModifyTable)) {
            ListCell *cell;
            ModifyTable *mt = (ModifyTable *)node;

            foreach (cell, mt->plans)
                shareinput_walker(f, (Node *)lfirst(cell), root);
        } else if (IsA(node, SubqueryScan)) {
            SubqueryScan *subqscan = (SubqueryScan *)node;
            PlannerGlobal *glob = root->glob;
            PlannerInfo *subroot;
            List *save_rtable;
            RelOptInfo *rel;

            /*
             * If glob->finalrtable is not NULL, rtables have been flatten,
             * thus we should use glob->finalrtable instead.
             */
            save_rtable = glob->share.curr_rtable;
            if (root->glob->finalrtable == NULL) {
                rel = find_base_rel(root, subqscan->scan.scanrelid);
                /*
                 * The Assert() on RelOptInfo's subplan being
                 * same as the subqueryscan's subplan, is valid
                 * in Upstream but for not for OpenGauss, since we
                 * create a new copy of the subplan if two
                 * SubPlans refer to the same initplan.
                 */
                subroot = rel->subroot;
                glob->share.curr_rtable = subroot->parse->rtable;
            } else {
                subroot = root;
                glob->share.curr_rtable = glob->finalrtable;
            }
            shareinput_walker(f, (Node *)subqscan->subplan, subroot);
            glob->share.curr_rtable = save_rtable;
        }
        else if (IsA(node, BitmapAnd)) {
            ListCell *cell;
            BitmapAnd *ba = (BitmapAnd *)node;

            foreach (cell, ba->bitmapplans)
                shareinput_walker(f, (Node *)lfirst(cell), root);
        } else if (IsA(node, BitmapOr)) {
            ListCell *cell;
            BitmapOr *bo = (BitmapOr *)node;

            foreach (cell, bo->bitmapplans)
                shareinput_walker(f, (Node *)lfirst(cell), root);
        } else if (IsA(node, NestLoop)) {
            /*
             * Nest loop join is strange.  Exec order depends on
             * prefetch_inner
             */
            NestLoop *nl = (NestLoop *) node;

            if (nl->join.prefetch_inner) {
                shareinput_walker(f, (Node *)plan->righttree, root);
                shareinput_walker(f, (Node *)plan->lefttree, root);
            } else {
                shareinput_walker(f, (Node *)plan->lefttree, root);
                shareinput_walker(f, (Node *)plan->righttree, root);
            }
        } else if (IsA(node, HashJoin)) {
            /* Hash join the hash table is at inner */
            shareinput_walker(f, (Node *)plan->righttree, root);
            shareinput_walker(f, (Node *)plan->lefttree, root);
        } else if (IsA(node, MergeJoin)) {
            MergeJoin *mj = (MergeJoin *)node;

            if (mj->unique_outer) {
                shareinput_walker(f, (Node *)plan->lefttree, root);
                shareinput_walker(f, (Node *)plan->righttree, root);
            } else {
                shareinput_walker(f, (Node *)plan->righttree, root);
                shareinput_walker(f, (Node *)plan->lefttree, root);
            }
        } else if (IsA(node, Sequence)) {
            ListCell *cell = NULL;
            Sequence *sequence = (Sequence *)node;

            foreach (cell, sequence->subplans) {
                shareinput_walker(f, (Node *)lfirst(cell), root);
            }
        } else {
            shareinput_walker(f, (Node *)plan->lefttree, root);
            shareinput_walker(f, (Node *)plan->righttree, root);
            shareinput_walker(f, (Node *)plan->initPlan, root);
        }
    }

    (*f)(node, root, true);
}

/*
 * Memorize the shared plan of a shared input in an array, one per share_id.
 */
static void shareinput_save_producer(ShareInputScan *plan, ApplyShareInputContext *ctxt)
{
    int share_id = plan->share_id;
    int new_shared_input_count = (share_id + 1);

    Assert(plan->share_id >= 0);

    if (ctxt->shared_plans == NULL) {
        ctxt->shared_plans = (Plan **)palloc0(sizeof(Plan *) * new_shared_input_count);
        ctxt->shared_input_count = new_shared_input_count;
    } else if (ctxt->shared_input_count < new_shared_input_count) {
        ctxt->shared_plans = (Plan **)repalloc(ctxt->shared_plans, new_shared_input_count * sizeof(Plan *));
        memset(&ctxt->shared_plans[ctxt->shared_input_count], 0,
            (new_shared_input_count - ctxt->shared_input_count) * sizeof(Plan *));
        ctxt->shared_input_count = new_shared_input_count;
    }

    Assert(ctxt->shared_plans[share_id] == NULL);
    ctxt->shared_plans[share_id] = plan->scan.plan.lefttree;
}

/*
 * Collect all the producer ShareInput nodes into an array, for later use by
 * replace_shareinput_targetlists().
 *
 * This is a stripped-down version of apply_shareinput_dag_to_tree(), for use
 * on ORCA-produced plans. ORCA assigns share_ids to all ShareInputScan nodes,
 * and only producer nodes have a subtree, so we don't need to do the DAG to
 * tree conversion or assign share_ids here.
 */
static bool collect_shareinput_producers_walker(Node *node, PlannerInfo *root, bool fPop)
{
    PlannerGlobal *glob = root->glob;
    ApplyShareInputContext *ctxt = &glob->share;

    if (fPop)
        return true;

    if (IsA(node, ShareInputScan)) {
        ShareInputScan *siscan = (ShareInputScan *)node;
        Plan *subplan = siscan->scan.plan.lefttree;

        Assert(siscan->share_id >= 0);

        if (subplan)
            shareinput_save_producer(siscan, ctxt);
    }
    return true;
}

void collect_shareinput_producers(PlannerInfo *root, Plan *plan)
{
    PlannerGlobal *glob = root->glob;

    glob->share.curr_rtable = glob->finalrtable;
    shareinput_walker(collect_shareinput_producers_walker, (Node *)plan, root);
}
/*
 * Create a fake CTE range table entry that reflects the target list of a
 * shared input.
 */
static RangeTblEntry *create_shareinput_producer_rte(ApplyShareInputContext *ctxt, int share_id, int refno)
{
    int attno = 1;
    ListCell *lc;
    Plan *subplan;
    char buf[100];
    RangeTblEntry *rte;
    List *colnames = NIL;
    List *coltypes = NIL;
    List *coltypmods = NIL;
    List *colcollations = NIL;

    // Assert(ctxt->shared_plans);
    // Assert(ctxt->shared_input_count > share_id);
    subplan = ctxt->shared_plans[share_id];

    foreach (lc, subplan->targetlist) {
        TargetEntry *tle = (TargetEntry *)lfirst(lc);
        Oid vartype;
        int32 vartypmod;
        Oid varcollid;
        char *resname;

        vartype = exprType((Node *)tle->expr);
        vartypmod = exprTypmod((Node *)tle->expr);
        varcollid = exprCollation((Node *)tle->expr);

        /*
         * We should've filled in tle->resname in shareinput_save_producer().
         * Note that it's too late to call get_tle_name() here, because this
         * runs after all the varnos in Vars have already been changed to
         * INNER_VAR/OUTER_VAR.
         */
        resname = tle->resname;
        if (!resname)
            resname = pstrdup("unnamed_attr");

        colnames = lappend(colnames, makeString(resname));
        coltypes = lappend_oid(coltypes, vartype);
        coltypmods = lappend_int(coltypmods, vartypmod);
        colcollations = lappend_oid(colcollations, varcollid);
        attno++;
    }

    /*
     * Create a new RTE. Note that we use a different RTE for each reference,
     * because we want to give each reference a different name.
     */
    snprintf(buf, sizeof(buf), "share%d_ref%d", share_id, refno);

    rte = makeNode(RangeTblEntry);
    rte->rtekind = RTE_CTE;
    rte->ctename = pstrdup(buf);
    rte->ctelevelsup = 0;
    rte->self_reference = false;
    rte->alias = NULL;

    rte->eref = makeAlias(rte->ctename, colnames);
    rte->ctecoltypes = coltypes;
    rte->ctecoltypmods = coltypmods;
    rte->ctecolcollations = colcollations;

    rte->inh = false;
    rte->inFromCl = false;

    rte->requiredPerms = 0;
    rte->checkAsUser = InvalidOid;

    return rte;
}
static bool replace_shareinput_targetlists_walker(Node *node, PlannerInfo *root, bool fPop)
{
    PlannerGlobal *glob = root->glob;
    ApplyShareInputContext *ctxt = &glob->share;

    if (fPop)
        return true;

    if (IsA(node, ShareInputScan)) {
        ShareInputScan *sisc = (ShareInputScan *)node;
        int share_id = sisc->share_id;
        ListCell *lc;
        int attno;
        List *newtargetlist;
        RangeTblEntry *rte;

        /*
         * Note that even though the planner assigns sequential share_ids for
         * each shared node, so that share_id is always below
         * list_length(ctxt->sharedNodes), ORCA has a different assignment
         * scheme. So we have to be prepared for any share_id, at least when
         * ORCA is in use.
         */
        if (ctxt->share_refcounts == NULL) {
            int new_sz = share_id + 1;

            ctxt->share_refcounts = (int *)palloc0(new_sz * sizeof(int));
            ctxt->share_refcounts_sz = new_sz;
        } else if (share_id >= ctxt->share_refcounts_sz) {
            int old_sz = ctxt->share_refcounts_sz;
            int new_sz = share_id + 1;

            ctxt->share_refcounts = (int *)repalloc(ctxt->share_refcounts, new_sz * sizeof(int));
            memset(&ctxt->share_refcounts[old_sz], 0, (new_sz - old_sz) * sizeof(int));
            ctxt->share_refcounts_sz = new_sz;
        }

        ctxt->share_refcounts[share_id]++;

        /*
         * Create a new RTE. Note that we use a different RTE for each
         * reference, because we want to give each reference a different name.
         */
        rte = create_shareinput_producer_rte(ctxt, share_id, ctxt->share_refcounts[share_id]);

        glob->finalrtable = lappend(glob->finalrtable, rte);
        sisc->scan.scanrelid = list_length(glob->finalrtable);

        /*
         * Replace all the target list entries.
         *
         * SharedInputScan nodes are not projection-capable, so the target
         * list of the SharedInputScan matches the subplan's target list.
         */
        newtargetlist = NIL;
        attno = 1;
        foreach (lc, sisc->scan.plan.targetlist) {
            TargetEntry *tle = (TargetEntry *)lfirst(lc);
            TargetEntry *newtle = flatCopyTargetEntry(tle);

            newtle->expr = (Expr *)makeVar(sisc->scan.scanrelid, attno, exprType((Node *)tle->expr),
                exprTypmod((Node *)tle->expr), exprCollation((Node *)tle->expr), 0);
            newtargetlist = lappend(newtargetlist, newtle);
            attno++;
        }
        sisc->scan.plan.targetlist = newtargetlist;
    }

    return true;
}
Plan *replace_shareinput_targetlists(PlannerInfo *root, Plan *plan)
{
    shareinput_walker(replace_shareinput_targetlists_walker, (Node *)plan, root);
    return plan;
}

/* Some helper: implements a stack using List. */
static void shareinput_pushmot(ApplyShareInputContext *ctxt, int motid)
{
    ctxt->motStack = lcons_int(motid, ctxt->motStack);
}
static void shareinput_popmot(ApplyShareInputContext *ctxt)
{
    list_delete_first(ctxt->motStack);
}
static int shareinput_peekmot(ApplyShareInputContext *ctxt)
{
    return linitial_int(ctxt->motStack);
}

/*
 * First walk on shareinput xslice. Collect information about the producer
 * and consumer slice IDs for each share. It also builds a list of shares
 * that should run in the QD.
 */
static bool shareinput_mutator_xslice_1(Node *node, PlannerInfo *root, bool fPop)
{
    PlannerGlobal *glob = root->glob;
    ApplyShareInputContext *ctxt = &glob->share;
    Plan *plan = (Plan *)node;
 
    if (fPop) {
        if (IsA(plan, Motion))
            shareinput_popmot(ctxt);
        return false;
    }
 
    if (IsA(plan, Motion)) {
        Motion *motion = (Motion *)plan;
 
        shareinput_pushmot(ctxt, motion->motionID);
        return true;
    }
 
    if (IsA(plan, ShareInputScan)) {
        ShareInputScan *sisc = (ShareInputScan *)plan;
        int motId = shareinput_peekmot(ctxt);
        Plan *shared = plan->lefttree;
        ApplyShareInputContextPerShare *share_info;
 
        share_info = &ctxt->shared_inputs[sisc->share_id];
 
        /* Remember information about the slice that this instance appears in. */
        if (shared) {
            ctxt->shared_inputs[sisc->share_id].producer_slice_id = motId;
            sisc->is_producer = true;
        } else {
            int producer_slice_id = ctxt->shared_inputs[sisc->share_id].producer_slice_id;
            PlanSlice *producer_slice = &(ctxt->slices[producer_slice_id]);
            PlanSlice *consumer_slice = &(ctxt->slices[motId]);
            if (producer_slice->numsegments != consumer_slice->numsegments) {
                ereport(ERROR, (errmsg("ShareInputScan check dop fail share_id[%d] producer(%d, %d) consumer(%d, %d) ",
                sisc->share_id, producer_slice_id, producer_slice->numsegments,
                motId, consumer_slice->numsegments)));
            } else {
                ereport(DEBUG2, (errmsg("ShareInputScan check dop SUCCESS share_id[%d] producer(%d, %d) consumer(%d, %d) ",
                sisc->share_id, producer_slice_id, producer_slice->numsegments,
                motId, consumer_slice->numsegments)));
            }
        }
        share_info->participant_slices = bms_add_member(share_info->participant_slices, motId);
 
        sisc->this_slice_id = motId;
    }
 
    return true;
}
 
/*
 * Second pass:
 * 1. Mark shareinput scans with multiple consumer slices as cross-slice.
 * 2. Fill 'share_type' and 'share_id' fields in the shared Material/Sort nodes.
 */
static bool shareinput_mutator_xslice_2(Node *node, PlannerInfo *root, bool fPop)
{
    PlannerGlobal *glob = root->glob;
    ApplyShareInputContext *ctxt = &glob->share;
    Plan *plan = (Plan *)node;
 
    if (fPop) {
        if (IsA(plan, Motion))
            shareinput_popmot(ctxt);
        return false;
    }
 
    if (IsA(plan, Motion)) {
        Motion *motion = (Motion *)plan;
 
        shareinput_pushmot(ctxt, motion->motionID);
        return true;
    }
 
    if (IsA(plan, ShareInputScan)) {
        ShareInputScan *sisc = (ShareInputScan *)plan;
        ApplyShareInputContextPerShare *pershare;
 
        pershare = &ctxt->shared_inputs[sisc->share_id];
 
        if (bms_num_members(pershare->participant_slices) > 1) {
            Assert(!sisc->cross_slice);
            sisc->cross_slice = true;
        }
 
        sisc->producer_slice_id = pershare->producer_slice_id;
        sisc->nconsumers = bms_num_members(pershare->participant_slices) - 1;
    }
    return true;
}
/*
 * Scan through the plan tree and make note of which Share Input Scans
 * are cross-slice.
 */
Plan *apply_shareinput_xslice(Plan *plan, PlannerInfo *root, PlanSlice *slices)
{
    PlannerGlobal *glob = root->glob;
    ApplyShareInputContext *ctxt = &glob->share;
 
    ctxt->motStack = NULL;
    ctxt->qdShares = NULL;
 
    ctxt->shared_inputs =
        (ApplyShareInputContextPerShare *)palloc0(ctxt->shared_input_count * sizeof(ApplyShareInputContextPerShare));
    ctxt->slices = slices;
    shareinput_pushmot(ctxt, 0);
 
    /*
     * Walk the tree.  See comment for each pass for what each pass will do.
     * The context is used to carry information from one pass to another, as
     * well as within a pass.
     */

    shareinput_walker(shareinput_mutator_xslice_1, (Node *)plan, root);
 
    /* Now walk the tree again, and process all the consumers. */
    shareinput_walker(shareinput_mutator_xslice_2, (Node *)plan, root);
 
    return plan;
}

/*
 * Remove subquery field in RTE's with subquery kind.
 */
void remove_subquery_in_RTEs(Node *node)
{
    if (node == NULL) {
        return;
    }

    if (IsA(node, RangeTblEntry)) {
        RangeTblEntry *rte = (RangeTblEntry *)node;

        if (RTE_SUBQUERY == rte->rtekind && NULL != rte->subquery) {
            /*
             * Replace subquery with a dummy subquery.
             *
             * XXX: We could save a lot more memory by deep-freeing the many
             * fields in the Query too. But I'm not sure which of them might
             * be shared by other objects in the tree.
             */
            pfree(rte->subquery);
            rte->subquery = NULL;
        }

        return;
    }

    if (IsA(node, List)) {
        List *list = (List *)node;
        ListCell *lc = NULL;

        foreach (lc, list) {
            remove_subquery_in_RTEs((Node *)lfirst(lc));
        }
    }
}
List* make_distributed_key_by_groupingset(PlannerInfo* root, Plan *subplan, List* groupcls )
{
    if (groupcls == nullptr) {
        ereport(LOG,(errmsg("groupcls is null, subplan type[%d]", subplan->type)));
        return nullptr;
    }
    double multiple = 0.0;
    List* distributed = spq_get_distributekey_from_tlist(root, subplan->targetlist, groupcls, subplan->plan_rows, &multiple, nullptr);
    if (distributed == nullptr) {
        ereport(ERROR, (errmsg("get_distributekey_from_tlist fail")));
    }
    return distributed;
}
int exec_by_multiple_dop(PlannerInfo* root, Plan *spqplan)
{
    if (u_sess->attr.attr_spq.spq_optimizer_calc_multiple_dop == false) {
        return spqplan->dop;
    }
    if (!IsA(spqplan, SpqSeqScan)) {
        return spqplan->dop;
    }
    int cur_dop = u_sess->opt_cxt.query_dop > 1 ? u_sess->opt_cxt.query_dop : 1;
    if (cur_dop == 1) {
        return cur_dop;
    }
    /* cost_seqscan use for reference
        1. QualCost(startup_cost) is not used because baserestrictinfo is not initialized.
        2. cpu_run_cost is not used because reltarget is not initialized.
        baserestrictinfo & reltarget need to call a series of set_baserel_size_estimates function
    */
    Cost run_cost_single = 0;
    Cost run_cost_mul = 0;
    Cost cpu_per_tuple = 0.0;
    SeqScan *seqscan = (SeqScan *)spqplan;
    Oid reloid = getrelid(seqscan->scanrelid, root->glob->finalrtable);
    Assert(reloid != InvalidOid);
    Relation relation = heap_open(reloid, NoLock);
    Oid reltablespace = RelationGetForm(relation)->reltablespace;
    RelPageType curpages = RelationGetNumberOfBlocks(relation);
    double rows = (double)relation->rd_rel->reltuples;
    double spc_seq_page_cost;

    get_tablespace_page_costs(reltablespace, NULL, &spc_seq_page_cost);
    // cant init qpqual_cost

    run_cost_mul += u_sess->opt_cxt.smp_thread_cost * (cur_dop - 1);

    run_cost_single += spc_seq_page_cost * curpages;
    if (u_sess->attr.attr_sql.enable_seqscan_dopcost) {
        run_cost_mul += spc_seq_page_cost * curpages / cur_dop;
    } else {
        run_cost_mul += spc_seq_page_cost * curpages;
    }
    cpu_per_tuple = u_sess->attr.attr_sql.cpu_tuple_cost;
    run_cost_single += cpu_per_tuple * clamp_row_est(rows);
    run_cost_mul += cpu_per_tuple * clamp_row_est(rows / getSpqsegmentCount());
    if (run_cost_mul > run_cost_single ) {
        spqplan->dop = 1;
        spqplan->parallel_enabled = false;
    }
    heap_close(relation, NoLock);
    return spqplan->dop;
}
bool check_slice_dop(PlanSlice *slices, Plan *subplan, PlannedStmt *result)
{
    int producerDop = slices->numsegments == 1 ? 1 : u_sess->opt_cxt.query_dop;
    if (producerDop != subplan->dop)  {
        ereport(LOG,(errmsg("check_slice_dop fail, slice info:")));
        for (int i = 0; i < result->numSlices; i++) {
            PlanSlice *slices = &(result->slices[i]);
            ereport(LOG, (errmsg("Index[%d] pIndex[%d] type[%d] segments[%d] worker_idx[%d]",
            slices->sliceIndex, slices->parentIndex, slices->gangType, slices->numsegments, slices->worker_idx)));
        }
    }
    return producerDop == subplan->dop;
}

Plan* make_stream(PlannerInfo* root, Plan *subplan, Motion *motion, PlannedStmt *result)
{
    PlanSlice *slices = &(result->slices[motion->motionID]);
    PlanSlice *parentSlices = &(result->slices[slices->parentIndex]);
    if (check_slice_dop(slices, subplan, result) == false) {
        ereport(ERROR, (errmsg("check_slice_dop in remote check fail motion[%d]", motion->motionID)));
    }
    if (parentSlices->numsegments == 1) {
        return make_gather_stream(root, subplan, motion, result);
    }
    Stream* stream = makeNode(Stream);
    Plan* plan = &stream->scan.plan;
    Distribution* distribution = ng_get_dest_distribution(subplan);
    stream->distribute_keys = make_distributed_key_by_groupingset(root, subplan, motion->hashExprs);
    stream->is_sorted = false;
    stream->sort = NULL;
    copy_plan_costsize(plan, &motion->plan);
    plan->distributed_keys = stream->distribute_keys;
    plan->targetlist = list_copy(motion->plan.targetlist);
    plan->lefttree = subplan;
    plan->righttree = NULL;
    plan->exec_nodes = ng_get_dest_execnodes(subplan);
    plan->hasUniqueResults = subplan->hasUniqueResults;
    plan->multiple = 1.0;

    // set by redistribute_keys?
    stream->smpDesc.producerDop = exec_by_multiple_dop(root, subplan);
    stream->smpDesc.consumerDop = parentSlices->numsegments > 1 ? u_sess->opt_cxt.query_dop : 1;;
    plan->dop = stream->smpDesc.consumerDop;


    if (motion->motionType == MOTIONTYPE_BROADCAST) {
        stream->smpDesc.distriType = REMOTE_BROADCAST;
        stream->type = STREAM_BROADCAST;
        stream->consumer_nodes = ng_convert_to_exec_nodes(distribution, LOCATOR_TYPE_REPLICATED, RELATION_ACCESS_READ);
    } else if (motion->motionType == MOTIONTYPE_HASH ||
                motion->motionType == MOTIONTYPE_EXPLICIT) {
        stream->smpDesc.distriType = REMOTE_SPLIT_DISTRIBUTE;
        stream->type = STREAM_REDISTRIBUTE;
        stream->consumer_nodes = ng_convert_to_exec_nodes(distribution, LOCATOR_TYPE_HASH, RELATION_ACCESS_READ);
        if (stream->distribute_keys == nullptr) {
            stream->smpDesc.distriType = REMOTE_ROUNDROBIN;
        }
    } else {
        ereport(ERROR,(errmsg("unknown motion type [%d]", motion->motionType)));
    }
    stream->streamID = motion->motionID;
    return (Plan*)stream;
}

Plan* make_sort(Motion *motion, Plan *subplan)
{
    Sort* node = makeNode(Sort);
    Plan* plan = &node->plan;
    inherit_plan_locator_info(plan, (Plan*)motion);
    copy_plan_costsize(plan, (Plan*)motion); /* only care about copying size */
    plan->targetlist = subplan->targetlist;
    plan->qual = NIL;
    plan->lefttree = subplan;
    plan->righttree = NULL;
    plan->hasUniqueResults = subplan->hasUniqueResults;
    plan->dop = subplan->dop;
    node->numCols = motion->numSortCols;
    node->sortColIdx = motion->sortColIdx;
    node->sortOperators = motion->sortOperators;
    node->collations = motion->collations;
    node->nullsFirst = motion->nullsFirst;
    return (Plan*)node;
}

Plan* make_dml_stream(PlannerInfo* root, Plan *subplan, Motion *motion, PlannedStmt *result)
{
    Stream* stream = makeNode(Stream);
    Plan* plan = &stream->scan.plan;
    Distribution* distribution = ng_get_dest_distribution(subplan);
    stream->distribute_keys = make_distributed_key_by_groupingset(root, subplan, motion->hashExprs);
    stream->is_sorted = false;
    stream->sort = NULL;
    copy_plan_costsize(plan, &motion->plan);
    plan->distributed_keys = stream->distribute_keys;
    plan->targetlist = list_copy(motion->plan.targetlist);
    plan->lefttree = subplan;
    plan->righttree = NULL;
    plan->exec_nodes = ng_get_dest_execnodes(subplan);
    plan->hasUniqueResults = subplan->hasUniqueResults;
    plan->multiple = 1.0;

    // set by redistribute_keys?
    stream->smpDesc.producerDop = subplan->dop;
    stream->smpDesc.consumerDop = 1;

    plan->dop = stream->smpDesc.consumerDop;

    stream->smpDesc.distriType = REMOTE_DML_WRITE_NODE;
    stream->type = STREAM_REDISTRIBUTE;
    stream->consumer_nodes = ng_convert_to_exec_nodes(distribution, LOCATOR_TYPE_HASH, RELATION_ACCESS_READ);

    stream->streamID = motion->motionID;
    if (motion->sendSorted) {
        return make_sort(motion, (Plan*)stream);
    }
    return (Plan*)stream;
}

Plan* create_spq_local_gather(PlannerInfo* root, Plan* plan, Motion *motion)
{
    if (IsA(plan, Stream)) {
        Stream* st = (Stream*)plan;
        plan->dop = 1;
        st->smpDesc.consumerDop = 1;
        return plan;
    }
    Stream* stream_node = NULL;
    Plan* stream_plan = NULL;
    stream_node = makeNode(Stream);
    stream_node->type = STREAM_REDISTRIBUTE;
    stream_node->consumer_nodes = (ExecNodes*)copyObject(plan->exec_nodes);
    stream_node->is_sorted = false;
    stream_node->is_dummy = false;
    stream_node->sort = NULL;
    stream_node->smpDesc.consumerDop = 1;
    stream_node->smpDesc.producerDop = exec_by_multiple_dop(root, plan);
    stream_node->smpDesc.distriType = LOCAL_ROUNDROBIN;
    stream_node->distribute_keys = NIL;
 
    /* Set plan struct parameter. */
    stream_plan = &stream_node->scan.plan;
    copy_plan_costsize(stream_plan, &motion->plan);
    stream_plan->targetlist = list_copy(motion->plan.targetlist);
    stream_plan->distributed_keys = plan->distributed_keys;
    stream_plan->lefttree = plan;
    stream_plan->righttree = NULL;
    stream_plan->exec_nodes = (ExecNodes*)copyObject(ng_get_dest_execnodes(plan));
    if (!stream_plan->exec_nodes)
        stream_plan->exec_nodes = makeNode(ExecNodes);
    stream_plan->exec_nodes->baselocatortype = LOCATOR_TYPE_RROBIN;
    stream_plan->hasUniqueResults = plan->hasUniqueResults;
    stream_plan->dop = 1;
    stream_node->streamID = motion->motionID;
    return stream_plan;
}

Plan* make_gather_Remote(PlannerInfo* root, Plan *lefttree, Motion *motion, PlannedStmt *result) {

    PlanSlice *slices = &(result->slices[motion->motionID]);
    if (check_slice_dop(slices, lefttree, result) == false) {
        ereport(ERROR, (errmsg("check_slice_dop in remote check fail motion[%d]", motion->motionID)));
    }
    if (lefttree->dop > 1) {
        lefttree = create_spq_local_gather(root, lefttree, motion);
    }
    RemoteQuery* remote_query = makeNode(RemoteQuery);
    remote_query->combine_type = COMBINE_TYPE_NONE;
    remote_query->scan.plan.targetlist = list_copy(motion->plan.targetlist);
    remote_query->base_tlist = NIL;
    remote_query->sql_statement = NULL;
    remote_query->exec_nodes = (ExecNodes *)copyObject(ng_get_dest_execnodes(lefttree));
    remote_query->read_only = true;
    
    remote_query->scan.plan.exec_nodes = remote_query->exec_nodes;
    remote_query->scan.plan.lefttree = lefttree;
    remote_query->scan.plan.exec_type = EXEC_ON_COORDS;
    remote_query->is_simple = true;
    remote_query->rq_need_proj = false;
    copy_plan_costsize(&remote_query->scan.plan, &motion->plan);
    remote_query->scan.plan.plan_width = lefttree->plan_width;
    remote_query->sort = NULL;
    remote_query->streamID = motion->motionID;
    remote_query->scan.plan.dop = 1;

    remote_query->nodeCount = slices->numsegments > 1 ? t_thrd.spq_ctx.num_nodes : 1;
    if (motion->sendSorted) {
        return make_sort(motion, (Plan*)remote_query);
    }

    return (Plan*)remote_query;

}
Plan* make_gather_stream(PlannerInfo* root, Plan *subplan, Motion *motion, PlannedStmt *result)
{
    PlanSlice *slices = &(result->slices[motion->motionID]);
    PlanSlice *parentSlices = &(result->slices[slices->parentIndex]);
    if (check_slice_dop(slices, subplan, result) == false) {
        ereport(ERROR, (errmsg("check_slice_dop in remote check fail motion[%d]", motion->motionID)));
    }
    Stream *stream_node = makeNode(Stream);
    stream_node->type = STREAM_GATHER;
    stream_node->consumer_nodes = (ExecNodes *)copyObject(subplan->exec_nodes);
    stream_node->is_sorted = false;
    stream_node->is_dummy = false;
    stream_node->sort = NULL;
    stream_node->smpDesc.consumerDop = parentSlices->numsegments > 1 ? u_sess->opt_cxt.query_dop : 1;
    stream_node->smpDesc.producerDop = exec_by_multiple_dop(root, subplan); /* plan->dop */
    stream_node->smpDesc.distriType = REMOTE_DIRECT_DISTRIBUTE;
    stream_node->distribute_keys = NIL;
    /* Set plan struct parameter. */
    Plan* stream_plan = &stream_node->scan.plan;
    stream_plan->distributed_keys = subplan->distributed_keys;
    stream_plan->targetlist = list_copy(motion->plan.targetlist);
    stream_plan->lefttree = subplan;
    stream_plan->righttree = NULL;
    stream_plan->exec_nodes = (ExecNodes *)copyObject(ng_get_dest_execnodes(subplan));
    if (!stream_plan->exec_nodes)
        stream_plan->exec_nodes = makeNode(ExecNodes);
    stream_plan->hasUniqueResults = subplan->hasUniqueResults;
    copy_plan_costsize(stream_plan, &motion->plan);
    stream_plan->dop = stream_node->smpDesc.consumerDop;
    stream_node->streamID = motion->motionID;
    if (motion->sendSorted) {
        return make_sort(motion, (Plan*)stream_node);
    }
    return (Plan*)stream_node;
}
Plan *make_gather_remote_top(Plan *lefttree, PlannedStmt *result)
{
    PlanSlice *slices = &(result->slices[0]);
    RemoteQuery* remote_query = makeNode(RemoteQuery);
    remote_query->combine_type = COMBINE_TYPE_NONE;
    remote_query->base_tlist = NIL;
    remote_query->sql_statement = NULL;
    remote_query->exec_nodes = lefttree->exec_nodes;
    remote_query->read_only = true;

    remote_query->scan.plan.exec_nodes = remote_query->exec_nodes;
    remote_query->scan.plan.lefttree = lefttree;
    remote_query->scan.plan.exec_type = EXEC_ON_COORDS;
    remote_query->is_simple = true;
    remote_query->rq_need_proj = false;
    remote_query->scan.plan.plan_width = lefttree->plan_width;
    remote_query->sort = NULL;
    remote_query->scan.plan.dop = 1;
    remote_query->nodeCount = slices->numsegments > 1 ? t_thrd.spq_ctx.num_nodes : 1;
    return (Plan*)remote_query;
}

Plan *tran_motion_to_stream(PlannerInfo* root, SpqSliceContext *cxt, Plan *plan, bool &top, bool fromdml)
{
    bool backtop = top;
    top = false;
    Motion *motion = (Motion *)plan;
    Assert(!motion->plan.righttree);
    int backIndex = cxt->curentIndex;
    if (motion->motionID >= cxt->result->numSlices) {
        ereport(ERROR, (errmsg("MotionID check fail id[%d] numslice[%d]", motion->motionID, cxt->result->numSlices)));
    }
    cxt->curentIndex = motion->motionID;
    Plan *subplan = replace_motion_stream_recurse(root, cxt, motion->plan.lefttree, top);
    cxt->curentIndex = backIndex;

    if (u_sess->attr.attr_spq.spq_debug_slice_print && motion->motionID < cxt->result->numSlices) {
        PlanSlice *slices = &(cxt->result->slices[motion->motionID]);
        PlanSlice *parentSlices = &(cxt->result->slices[slices->parentIndex]);
        ereport(LOG,(errmsg("[MotionInfo] motionID[%d] motiontype[%d] recvcout[%d] sendcount[%d]",
        motion->motionID, motion->motionType, parentSlices->numsegments, slices->numsegments )));
        ereport(LOG,(errmsg("[SliceInfo] sliceIndex[%d] slicetype[%d] worker_idx[%d] parentIndex[%d]",
        slices->sliceIndex, slices->gangType, slices->worker_idx, slices->parentIndex)));
    }
    if (fromdml && backtop) {
        top = backtop;
        return make_dml_stream(root, subplan, motion, cxt->result);
    }

    // no need check motion->motionID again in below func;
    if (motion->motionType == MOTIONTYPE_GATHER) {
        if (backtop) {
            top = backtop;
            return make_gather_Remote(root, subplan, motion, cxt->result);
        }
        Plan *gather_stream = make_gather_stream(root, subplan, motion, cxt->result);
        top = backtop;
        return gather_stream;
    } else {
        return make_stream(root, subplan, motion, cxt->result);
    }
}
//TODO SPQ need fix: dops and multiple gather  
Plan *replace_motion_stream_recurse(PlannerInfo *root, SpqSliceContext *cxt, Plan *plan, bool &top, bool fromdml)
{

    ListCell* lc = NULL;

    if (plan == NULL)
        return NULL;
    if (plan->exec_nodes == NULL || plan->exec_nodes->nodeList == NULL) {
        ereport(ERROR, (errmsg("exec_nodes check fail plan type[%d]", plan->type)));
    }

    /* replace motion stream for subplan */
    List* subplans = root->glob->subplans;
    if (subplans) {
        List* subplan_list = check_subplan_list(plan);
        foreach (lc, subplan_list) {
            Node* node = (Node*)lfirst(lc);
            Plan* initNode = NULL;
            SubPlan* subplan = NULL;
            if (IsA(node, SubPlan)) {
                subplan = (SubPlan*)lfirst(lc);
                initNode = (Plan*)list_nth(subplans, subplan->plan_id - 1);
                lfirst(lc) = replace_motion_stream_recurse(root, cxt, initNode, top, fromdml);
            }
        }
        list_free_ext(subplan_list);
    }

    /* replace motion stream for initplan */
    List* initplans = plan->initPlan;
    foreach (lc, initplans) {
        Plan* initplan = (Plan*)lfirst(lc);
        lfirst(lc) = replace_motion_stream_recurse(root, cxt, initplan, top, fromdml);
    }

    if (IsA(plan, Append)) {
        Append* node = (Append*)plan;
        foreach(lc, node->appendplans) {
            Plan* initNode = (Plan*)lfirst(lc);
            lfirst(lc) = replace_motion_stream_recurse(root, cxt, initNode, top, fromdml);
        }
    }  

    if (IsA(plan, Sequence)) {
        if (top == true) {
            ereport(ERROR, (errmsg("There's no gather on sequence curentIndex[%d]", cxt->curentIndex)));
        }
	if (fromdml) {
            ereport(ERROR, (errmsg("not support SPQ DML with ShareInputScan")));
        }
        Sequence* node = (Sequence*)plan;
        foreach(lc, node->subplans) {
            Plan* subplan = (Plan*)lfirst(lc);
            lfirst(lc) = replace_motion_stream_recurse(root, cxt, subplan, top, fromdml); 
        }
    }

    if (IsA(plan, Motion)) {
        return tran_motion_to_stream(root, cxt, plan, top, fromdml);
    } else {
        if (plan->lefttree) {
            plan->lefttree = replace_motion_stream_recurse(root, cxt, plan->lefttree, top, fromdml);
            plan->dop = plan->lefttree->dop;
        }   
        if (plan->righttree) {
            plan->righttree = replace_motion_stream_recurse(root, cxt, plan->righttree, top, fromdml);
        }
        if (plan->lefttree == nullptr && plan->righttree == nullptr) {
            if (cxt->curentIndex >= cxt->result->numSlices) {
                ereport(ERROR, (errmsg("curentIndex check fail curentIndex[%d] numslice[%d]", cxt->curentIndex, cxt->result->numSlices)));
            }
            PlanSlice *slices = &(cxt->result->slices[cxt->curentIndex]);
            plan->dop = slices->numsegments > 1 ? u_sess->opt_cxt.query_dop : 1;
        }
        plan->parallel_enabled = (plan->dop > 1);
        return plan;
    }
}
/*
* TODO SPQ Init Remote info by guc, need fix gauss_cluster_map max len 64
* 
*/
static void InitRemoteNodeDefinition(PlannedStmt* planstmt)
{
    if (t_thrd.spq_ctx.num_nodes <= 0) {
        planstmt->num_nodes = 0;
        return;
    }
    int nodes_size = sizeof(NodeDefinition) * t_thrd.spq_ctx.num_nodes;
    planstmt->num_nodes = t_thrd.spq_ctx.num_nodes;
    planstmt->nodesDefinition = (NodeDefinition *)palloc0(nodes_size);
    memcpy_s(planstmt->nodesDefinition, nodes_size,  t_thrd.spq_ctx.nodesDefinition, nodes_size); 
}
Plan *replace_motion_dml(PlannerInfo* root, SpqSliceContext *cxt, Plan *plan, bool &top)
{
    if (!IsA(plan, ModifyTable)) {
        ereport(ERROR, (errmsg("replace_motion_dml is %d",  (int)nodeTag(plan))));
        return NULL;
    }
    PlannerGlobal *glob = root->glob;
    int subplan_id = 0;
    ListCell *lp;
    foreach (lp, glob->subplans) {
        if (cxt->result->subplan_sliceIds[subplan_id] == 0) {
            ereport(ERROR, (errmsg("not support SPQ DML when DML_STREAM in subplan")));
        }
        subplan_id++;
    }
    Plan *remote_query_plan = make_gather_remote_top(plan, cxt->result);
    ModifyTable* node = (ModifyTable*)plan;
    ListCell* l = NULL;
    foreach (l, node->plans) {
        Plan *subplan = (Plan*)lfirst(l);
        if (subplan) {
            subplan = replace_motion_stream_recurse(root, cxt, subplan, top, true);
            lfirst(l) = subplan;
        }
    }
    plan->dop = 1;
    plan->parallel_enabled = (plan->dop > 1);
    return remote_query_plan;
}

void make_spq_remote_query(PlannerInfo *root, PlannedStmt *result, PlannerGlobal *glob)
{
    bool top = true;
    if (u_sess->attr.attr_spq.spq_debug_slice_print) {
        for (int i = 0; i < result->numSlices; i++) {
            PlanSlice *slices = &(result->slices[i]);
            ereport(LOG, (errmsg("Index[%d] pIndex[%d] type[%d] segments[%d] worker_idx[%d]",
            slices->sliceIndex, slices->parentIndex, slices->gangType, slices->numsegments, slices->worker_idx)));
        }
    }
    SpqSliceContext sliceCxt;
    sliceCxt.result = result;
    sliceCxt.curentIndex = 0;
    /* whether select's part top stream has appeared,
     * top stream is to send scaning data to qc to modify table*/
    if (root->parse->commandType == CMD_SELECT) {
        result->planTree = replace_motion_stream_recurse(root, &sliceCxt, result->planTree, top);
    } else {
        result->planTree = replace_motion_dml(root, &sliceCxt, result->planTree, top);
    }    
    // should fix all?
    //result->planTree = set_plan_references(root, result->planTree);
    int parent_node_id = INITIAL_PARENT_NODE_ID; /* beginning with INITIAL_PARENT_NODE_ID */
    int plan_node_id = INITIAL_PLAN_NODE_ID;     /* beginning with INITIAL_PLAN_NODE_ID */
    int num_streams = 0;
    int num_plannodes = 0;
    int total_num_streams = 0;
    int gather_count = 0;
    int max_push_sql_num = 0;
    List* init_plan = NIL;

    int* subplan_ids = (int*)palloc0(sizeof(int) * (list_length(glob->subplans) + 1));

    finalize_node_id(result->planTree,
        &plan_node_id,
        &parent_node_id,
        &num_streams,
        &num_plannodes,
        &total_num_streams,
        &max_push_sql_num,
        &gather_count,
        glob->subplans, /* NIL */
        glob->subroots, /* NIL */
        &init_plan,
        subplan_ids,
        true,
        false,
        false,
        true, /* true */
        NULL); /* node_group_info_context */

     for (int i = 1; i <= list_length(result->subplans); i++)
        result->subplan_ids = lappend_int(result->subplan_ids, subplan_ids[i]);

    result->num_streams = total_num_streams;
    result->max_push_sql_num = max_push_sql_num;
    result->gather_count = gather_count;
    result->num_plannodes = num_plannodes;
    result->query_dop = u_sess->opt_cxt.query_dop;
    InitRemoteNodeDefinition(result);
    pfree_ext(subplan_ids);
}
