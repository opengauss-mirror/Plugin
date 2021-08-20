/*
 * contrib/tidrangescan/tidrangescan.cpp
 *	  Routines to support TID range scans of relations
 *
 */
#include "postgres.h"
#include "access/relscan.h"
#include "access/sysattr.h"
#include "catalog/pg_operator.h"
#include "catalog/pg_type.h"
#include "commands/defrem.h"
#include "commands/explain.h"
#include "executor/executor.h"
#include "executor/execdebug.h"
#include "fmgr.h"
#include "miscadmin.h"
#include "nodes/makefuncs.h"
#include "nodes/nodeFuncs.h"
#include "optimizer/clauses.h"
#include "optimizer/cost.h"
#include "optimizer/paths.h"
#include "optimizer/pathnode.h"
#include "optimizer/plancat.h"
#include "optimizer/planmain.h"
#include "optimizer/placeholder.h"
#include "optimizer/restrictinfo.h"
#include "optimizer/subselect.h"
#include "parser/parsetree.h"
#include "utils/builtins.h"
#include "utils/fmgroids.h"
#include "utils/guc.h"
#include "utils/lsyscache.h"
#include "utils/memutils.h"
#include "utils/rel.h"
#include "utils/spccache.h"
#include "access/tableam.h"
#include "access/htup.h"
#include "executor/nodeExtensible.h"
#include "commands/extension.h"

#include "tidrangescan.h"

/* missing declaration in pg_operator.h */
#ifndef TIDGreaterOperator
#define TIDGreaterOperator 2800
#endif
#ifndef TIDLessEqOperator
#define TIDLessEqOperator 2801
#endif
#ifndef TIDGreaterEqOperator
#define TIDGreaterEqOperator 2802
#endif

PG_MODULE_MAGIC;
PG_FUNCTION_INFO_V1(tidrangescan_invoke);

/* ----------------
 * 	 TidRangeScanState information
 *
 * 		trss_tidexprs		list of TidOpExpr structs (see nodeTidrangescan.c)
 * 		trss_mintid			the lowest TID in the scan range
 * 		trss_maxtid			the highest TID in the scan range
 * 		trss_inScan			is a scan currently in progress?
 * ----------------
 */
typedef struct {
    ExtensiblePlanState css;
    List *trss_tidexprs;
    ItemPointerData trss_mintid;
    ItemPointerData trss_maxtid;
    bool trss_inScan;
} TidRangeScanState;

typedef enum {
    TIDEXPR_UPPER_BOUND,
    TIDEXPR_LOWER_BOUND
} TidExprType;

/* Upper or lower range bound for scan */
typedef struct TidOpExpr {
    TidExprType exprtype; /* type of op; lower or upper */
    ExprState *exprstate; /* ExprState for a TID-yielding subexpr */
    bool inclusive;       /* whether op is inclusive */
} TidOpExpr;

static void SetTidRangeScanPath(PlannerInfo *root, RelOptInfo *rel, Index rti, RangeTblEntry *rte);
/* ExtensiblePathMethods */
static Plan *PlanTidRangeScanPath(PlannerInfo *root, RelOptInfo *rel, struct ExtensiblePath *best_path, List *tlist,
    List *clauses, List *extensible_plans);

/* ExtensiblePlanMethods */
static Node *CreateTidRangeScanState(ExtensiblePlan *extensible_plan);

/* ExtensibleExecMethods */
static void ExecInitTidRangeScan(ExtensiblePlanState *node, EState *estate, int eflags);
static TupleTableSlot *ExecTidRangeScan(ExtensiblePlanState *node);
static void ExecEndTidRangeScan(ExtensiblePlanState *node);
static void ExplainTidRangeScan(ExtensiblePlanState *node, List *ancestors, ExplainState *es);
static void ExecReScanTidRangeScan(ExtensiblePlanState *node);
static tidrangescan_context* get_session_context();

static uint32 tidrangescan_index;
#define ENABLE_TIDRANGESCAN (get_session_context()->enable_tidrangescan)

#define IsCTIDVar(node)                                                                                   \
    ((node) != NULL && IsA((node), Var) && ((Var *)(node))->varattno == SelfItemPointerAttributeNumber && \
        ((Var *)(node))->varlevelsup == 0)

/* static table of ExtensiblePlan callbacks */
static ExtensiblePathMethods tidrangescan_path_methods = {
    "Tid Range Scan",     /* ExtensibleName */
    PlanTidRangeScanPath, /* PlanExtensiblePath */
};

static ExtensiblePlanMethods tidrangescan_scan_methods = {
    "Tid Range Scan",        /* ExtensibleName */
    CreateTidRangeScanState, /* CreateExtensiblePlanState */
};

static ExtensibleExecMethods tidrangescan_exec_methods = {
    "Tid Range Scan",       /* ExtensibleName */
    ExecInitTidRangeScan,   /* BeginExtensiblePlan */
    ExecTidRangeScan,       /* ExecExtensiblePlan */
    ExecEndTidRangeScan,    /* EndExtensiblePlan */
    ExecReScanTidRangeScan, /* ReScanExtensiblePlan */
    ExplainTidRangeScan     /* ExplainExtensiblePlan */
};

void tidrangescan_invoke(void) {
    ereport(DEBUG2, (errmsg("dummy function to let process load this library.")));
    return;
}

List *ExecInitExprList(List *nodes, PlanState *parent)
{
    List *result = NIL;
    ListCell *lc;

    foreach (lc, nodes) {
        Expr *e = (Expr *)lfirst(lc);

        result = lappend(result, ExecInitExpr(e, parent));
    }

    return result;
}

/*
 * Does this Var represent the CTID column of the specified baserel?
 */
static inline bool IsCTIDVarRel(Var *var, RelOptInfo *rel)
{
    /* The vartype check is strictly paranoia */
    if (var->varattno == SelfItemPointerAttributeNumber && var->vartype == TIDOID && var->varno == rel->relid &&
        var->varlevelsup == 0)
        return true;
    return false;
}

/*
 * Check to see if a RestrictInfo is of the form
 * 		CTID OP pseudoconstant
 * or
 * 		pseudoconstant OP CTID
 * where OP is a binary operation, the CTID Var belongs to relation "rel",
 * and nothing on the other side of the clause does.
 */
static bool IsBinaryTidClause(RestrictInfo *rinfo, RelOptInfo *rel)
{
    OpExpr *node;
    Node *arg1, *arg2, *other;
    Relids other_relids;

    /* Must be an OpExpr */
    if (!is_opclause(rinfo->clause))
        return false;
    node = (OpExpr *)rinfo->clause;

    /* OpExpr must have two arguments */
    if (list_length(node->args) != 2)
        return false;
    arg1 = (Node *)linitial(node->args);
    arg2 = (Node *)lsecond(node->args);

    /* Look for CTID as either argument */
    other = NULL;
    other_relids = NULL;
    if (arg1 && IsA(arg1, Var) && IsCTIDVarRel((Var *)arg1, rel)) {
        other = arg2;
        other_relids = rinfo->right_relids;
    }
    if (!other && arg2 && IsA(arg2, Var) && IsCTIDVarRel((Var *)arg2, rel)) {
        other = arg1;
        other_relids = rinfo->left_relids;
    }
    if (!other)
        return false;

    /* The other argument must be a pseudoconstant */
    if (bms_is_member(rel->relid, other_relids) || contain_volatile_functions(other))
        return false;

    return true; /* success */
}

/*
 * Check to see if a RestrictInfo is of the form
 * 		CTID OP pseudoconstant
 * or
 * 		pseudoconstant OP CTID
 * where OP is a range operator such as <, <=, >, or >=, the CTID Var belongs
 * to relation "rel", and nothing on the other side of the clause does.
 */
static bool IsTidRangeClause(RestrictInfo *rinfo, RelOptInfo *rel)
{
    Oid opno;

    if (!IsBinaryTidClause(rinfo, rel))
        return false;
    opno = ((OpExpr *)rinfo->clause)->opno;

    if (opno == TIDLessOperator || opno == TIDLessEqOperator || opno == TIDGreaterOperator ||
        opno == TIDGreaterEqOperator)
        return true;

    return false;
}

/*
 * Extract a set of CTID range conditions from implicit-AND List of RestrictInfos
 *
 * Returns a List of CTID range qual RestrictInfos for the specified rel
 * (with implicit AND semantics across the list), or NIL if there are no
 * usable range conditions or if the rel's table AM does not support TID range
 * scans.
 */
static List *TidRangeQualFromRestrictInfoList(List *rlist, RelOptInfo *rel)
{
    List *rlst = NIL;
    ListCell *l;

    foreach (l, rlist) {
        RestrictInfo *rinfo = lfirst_node(RestrictInfo, l);

        if (IsTidRangeClause(rinfo, rel))
            rlst = lappend(rlst, rinfo);
    }

    return rlst;
}

/*
 * cost_tidrangescan
 * 	  Determines and sets the costs of scanning a relation using a range of
 * 	  TIDs for 'path'
 *
 * 'baserel' is the relation to be scanned
 * 'tidrangequals' is the list of TID-checkable range quals
 * 'param_info' is the ParamPathInfo if this is a parameterized path, else NULL
 */
void cost_tidrangescan(Path *path, PlannerInfo *root, RelOptInfo *baserel, List *tidrangequals,
    ParamPathInfo *param_info)
{
    Selectivity selectivity;
    double pages;
    Cost startup_cost = 0;
    Cost run_cost = 0;
    QualCost qpqual_cost;
    Cost cpu_per_tuple;
    QualCost tid_qual_cost;
    double ntuples;
    double nseqpages;
    double spc_random_page_cost;
    double spc_seq_page_cost;

    /* Should only be applied to base relations */
    Assert(baserel->relid > 0);
    Assert(baserel->rtekind == RTE_RELATION);

    /* Mark the path with the correct row estimate */
    if (param_info)
        path->rows = param_info->ppi_rows;
    else
        path->rows = baserel->rows;

    /* Count how many tuples and pages we expect to scan */
    selectivity = clauselist_selectivity(root, tidrangequals, baserel->relid, JOIN_INNER, NULL);
    pages = ceil(selectivity * baserel->pages);

    if (pages <= 0.0)
        pages = 1.0;

    /*
     * The first page in a range requires a random seek, but each subsequent
     * page is just a normal sequential page read. NOTE: it's desirable for
     * TID Range Scans to cost more than the equivalent Sequential Scans,
     * because Seq Scans have some performance advantages such as scan
     * synchronization and parallelizability, and we'd prefer one of them to
     * be picked unless a TID Range Scan really is better.
     */
    ntuples = selectivity * baserel->tuples;
    nseqpages = pages - 1.0;

    if (!ENABLE_TIDRANGESCAN)
        startup_cost += g_instance.cost_cxt.disable_cost;

    /*
     * The TID qual expressions will be computed once, any other baserestrict
     * quals once per retrieved tuple.
     */
    cost_qual_eval(&tid_qual_cost, tidrangequals, root);

    /* fetch estimated page cost for tablespace containing table */
    get_tablespace_page_costs(baserel->reltablespace, &spc_random_page_cost, &spc_seq_page_cost);

    /* disk costs; 1 random page and the remainder as seq pages */
    run_cost += spc_random_page_cost + spc_seq_page_cost * nseqpages;

    /* Add scanning CPU costs */
    if (param_info != NULL) {
        /* Include costs of pushed-down clauses */
        cost_qual_eval(&qpqual_cost, param_info->ppi_clauses, root);

        qpqual_cost.startup += baserel->baserestrictcost.startup;
        qpqual_cost.per_tuple += baserel->baserestrictcost.per_tuple;
    } else {
        qpqual_cost = baserel->baserestrictcost;
    }

    /*
     * XXX currently we assume TID quals are a subset of qpquals at this
     * point; they will be removed (if possible) when we create the plan, so
     * we subtract their cost from the total qpqual cost.  (If the TID quals
     * can't be removed, this is a mistake and we're going to underestimate
     * the CPU cost a bit.)
     */
    startup_cost += qpqual_cost.startup + tid_qual_cost.per_tuple;
    cpu_per_tuple = u_sess->attr.attr_sql.cpu_tuple_cost + qpqual_cost.per_tuple - tid_qual_cost.per_tuple;
    run_cost += cpu_per_tuple * ntuples;

    path->startup_cost = startup_cost;
    path->total_cost = startup_cost + run_cost;
}

/*
 * SetTidRangeScanPath - entrypoint of the series of extensible-scan execution.
 * It adds ExtensiblePath if referenced relation has inequality expressions on
 * the ctid system column.
 */
static void SetTidRangeScanPath(PlannerInfo *root, RelOptInfo *baserel, Index rtindex, RangeTblEntry *rte)
{
    List *tidrangequals = NIL;

    /* only plain relations are supported */
    if (baserel->rtekind != RTE_RELATION || rte->relkind == RELKIND_FOREIGN_TABLE || rte->tablesample != NULL)
        return;

    /*
     * If there are range quals in the baserestrict list, generate a
     * TidRangePath.
     */
    tidrangequals = TidRangeQualFromRestrictInfoList(baserel->baserestrictinfo, baserel);

    if (tidrangequals != NIL) {
        ExtensiblePath *cpath = makeNode(ExtensiblePath);
        Relids required_outer;

        /*
         * This path uses no join clauses, but it could still have required
         * parameterization due to LATERAL refs in its tlist.
         */
        required_outer = baserel->lateral_relids;

        cpath->path.pathtype = T_ExtensiblePlan;
        cpath->path.parent = baserel;
        cpath->path.param_info = get_baserel_parampathinfo(root, baserel, required_outer);
        cpath->path.pathkeys = NIL; /* always unordered */
        cpath->flags = EXTENSIBLEPATH_SUPPORT_BACKWARD_SCAN | EXTENSIBLEPATH_SUPPORT_PROJECTION;

        cpath->extensible_private = tidrangequals;
        cpath->methods = &tidrangescan_path_methods;

        cost_tidrangescan(&cpath->path, root, baserel, tidrangequals, cpath->path.param_info);

        add_path(root, baserel, &cpath->path);
    }
}

/*
 * PlanTidRangeScanPath - A method of ExtensiblePath; that populate a extensible
 * object being delivered from ExtensiblePlan type, according to the supplied
 * EXtensiblePath object.
 */
static Plan *PlanTidRangeScanPath(PlannerInfo *root, RelOptInfo *rel, struct ExtensiblePath *best_path, List *tlist,
    List *clauses, List *extensible_plans)
{
    ExtensiblePlan *node = makeNode(ExtensiblePlan);
    Index scan_relid = best_path->path.parent->relid;
    List *tidrangequals = best_path->extensible_private;
    Plan *plan = &node->scan.plan;

    /* it should be a base rel... */
    Assert(scan_relid > 0);
    Assert(best_path->path.parent->rtekind == RTE_RELATION);

    /*
     * The qpqual list must contain all restrictions not enforced by the
     * tidrangequals list.  tidrangequals has AND semantics, so we can simply
     * remove any qual that appears in it.
     */
    {
        List *qpqual = NIL;
        ListCell *l;

        foreach (l, clauses) {
            RestrictInfo *rinfo = lfirst_node(RestrictInfo, l);

            if (rinfo->pseudoconstant)
                continue; /* we may drop pseudoconstants here */
            if (list_member_ptr(tidrangequals, rinfo))
                continue; /* simple duplicate */
            qpqual = lappend(qpqual, rinfo);
        }
        clauses = qpqual;
    }

    /* Reduce RestrictInfo lists to bare expressions; ignore pseudoconstants */
    tidrangequals = extract_actual_clauses(tidrangequals, false);
    clauses = extract_actual_clauses(clauses, false);

    plan->targetlist = tlist;
    plan->qual = clauses;
    plan->lefttree = NULL;
    plan->righttree = NULL;
    node->scan.scanrelid = scan_relid;
    node->extensible_exprs = tidrangequals;

    node->flags = best_path->flags;
    node->scan.plan.startup_cost = best_path->path.startup_cost;
    node->scan.plan.total_cost = best_path->path.total_cost;
    node->scan.plan.plan_rows = best_path->path.rows;
    node->scan.plan.plan_width = rel->width;
    node->methods = &tidrangescan_scan_methods;

    return plan;
}

/*
 * CreateTidRangeScanState - A method of ExtensiblePlan; that populate a extensible
 * object being delivered from ExtensiblePlanState type, according to the
 * supplied ExtensiblePath object.
 */
Node *CreateTidRangeScanState(ExtensiblePlan *extensible_plan)
{
    TidRangeScanState *tidrangestate;

    /*
     * create state structure
     */
    tidrangestate = (TidRangeScanState *)palloc0(sizeof(TidRangeScanState));
    NodeSetTag(tidrangestate, T_ExtensiblePlanState);
    tidrangestate->css.methods = &tidrangescan_exec_methods;

    /*
     * mark scan as not in progress, and TID range as not computed yet
     */
    tidrangestate->trss_inScan = false;
    return (Node *)&tidrangestate->css;
}

/*
 * For the given 'expr', build and return an appropriate TidOpExpr taking into
 * account the expr's operator and operand order.
 */
static TidOpExpr *
MakeTidOpExpr(OpExpr *expr, TidRangeScanState *tidstate)
{
    Node *arg1 = get_leftop((Expr *) expr);
    Node *arg2 = get_rightop((Expr *) expr);
    ExprState *exprstate = NULL;
    bool invert = false;
    TidOpExpr *tidopexpr;

    if (IsCTIDVar(arg1))
        exprstate = ExecInitExpr((Expr *)arg2, &tidstate->css.ss.ps);
    else if (IsCTIDVar(arg2)) {
        exprstate = ExecInitExpr((Expr *)arg1, &tidstate->css.ss.ps);
        invert = true;
    } else
        elog(ERROR, "could not identify CTID variable");

    tidopexpr = (TidOpExpr *)palloc(sizeof(TidOpExpr));
    tidopexpr->inclusive = false; /* for now */

    switch (expr->opno) {
        case TIDLessEqOperator:
            tidopexpr->inclusive = true;
            /* fall through */
        case TIDLessOperator:
            tidopexpr->exprtype = invert ? TIDEXPR_LOWER_BOUND : TIDEXPR_UPPER_BOUND;
            break;
        case TIDGreaterEqOperator:
            tidopexpr->inclusive = true;
            /* fall through */
        case TIDGreaterOperator:
            tidopexpr->exprtype = invert ? TIDEXPR_UPPER_BOUND : TIDEXPR_LOWER_BOUND;
            break;
        default:
            elog(ERROR, "could not identify CTID operator");
    }

    tidopexpr->exprstate = exprstate;

    return tidopexpr;
}

/*
 * Extract the qual subexpressions that yield TIDs to search for,
 * and compile them into ExprStates if they're ordinary expressions.
 */
static void TidExprListCreate(TidRangeScanState *tidrangestate)
{
    ExtensiblePlan *node = (ExtensiblePlan *)tidrangestate->css.ss.ps.plan;
    List *tidexprs = NIL;
    ListCell *l;

    foreach (l, node->extensible_exprs) {
        OpExpr *opexpr = (OpExpr *)lfirst(l);
        TidOpExpr *tidopexpr;

        if (!IsA(opexpr, OpExpr))
            elog(ERROR, "could not identify CTID expression");

        tidopexpr = MakeTidOpExpr(opexpr, tidrangestate);
        tidexprs = lappend(tidexprs, tidopexpr);
    }

    tidrangestate->trss_tidexprs = tidexprs;
}

/* ----------------------------------------------------------------
 * 		ExecInitTidRangeScan
 *
 * 		Initializes the tid range scan's state information, creates
 * 		scan keys, and opens the scan relation.
 *
 * 		Parameters:
 * 		  node: TidRangeScan node produced by the planner.
 * 		  estate: the execution state initialized in InitPlan.
 * ----------------------------------------------------------------
 */
static void ExecInitTidRangeScan(ExtensiblePlanState *node, EState *estate, int eflags)
{
    TidRangeScanState *ctss = (TidRangeScanState *)node;

    ctss->css.ss.ss_currentScanDesc = NULL; /* no table scan here */
    TidExprListCreate(ctss);
}

/*
 * ItemPointerInc
 * 		Increment 'pointer' by 1 only paying attention to the ItemPointer's
 * 		type's range limits and not MaxOffsetNumber and FirstOffsetNumber.
 * 		This may result in 'pointer' becoming !OffsetNumberIsValid.
 *
 * If the pointer is already the maximum possible values permitted by the
 * range of the ItemPointer's types, then do nothing.
 */
void ItemPointerInc(ItemPointer pointer)
{
    BlockNumber blk = ItemPointerGetBlockNumberNoCheck(pointer);
    OffsetNumber off = ItemPointerGetOffsetNumberNoCheck(pointer);

    if (off == PG_UINT16_MAX) {
        if (blk != InvalidBlockNumber) {
            off = 0;
            blk++;
        }
    } else
        off++;

    ItemPointerSet(pointer, blk, off);
}

/*
 * ItemPointerDec
 * 		Decrement 'pointer' by 1 only paying attention to the ItemPointer's
 * 		type's range limits and not MaxOffsetNumber and FirstOffsetNumber.
 * 		This may result in 'pointer' becoming !OffsetNumberIsValid.
 *
 * If the pointer is already the minimum possible values permitted by the
 * range of the ItemPointer's types, then do nothing.  This does rely on
 * FirstOffsetNumber being 1 rather than 0.
 */
void ItemPointerDec(ItemPointer pointer)
{
    BlockNumber blk = ItemPointerGetBlockNumberNoCheck(pointer);
    OffsetNumber off = ItemPointerGetOffsetNumberNoCheck(pointer);

    if (off == 0) {
        if (blk != 0) {
            off = PG_UINT16_MAX;
            blk--;
        }
    } else
        off--;

    ItemPointerSet(pointer, blk, off);
}

/* ----------------------------------------------------------------
 * 		TidRangeEval
 *
 * 		Compute and set node's block and offset range to scan by evaluating
 * 		the trss_tidexprs.  Returns false if we detect the range cannot
 * 		contain any tuples.  Returns true if it's possible for the range to
 * 		contain tuples.
 * ----------------------------------------------------------------
 */
static bool TidRangeEval(TidRangeScanState *node)
{
    ExprContext *econtext = node->css.ss.ps.ps_ExprContext;
    ItemPointerData lowerBound;
    ItemPointerData upperBound;
    ListCell *l;

    /*
     * Set the upper and lower bounds to the absolute limits of the range of
     * the ItemPointer type.  Below we'll try to narrow this range on either
     * side by looking at the TidOpExprs.
     */
    ItemPointerSet(&lowerBound, 0, 0);
    ItemPointerSet(&upperBound, InvalidBlockNumber, PG_UINT16_MAX);

    foreach (l, node->trss_tidexprs) {
        TidOpExpr *tidopexpr = (TidOpExpr *)lfirst(l);
        ItemPointer itemptr;
        bool isNull;

        /* Evaluate this bound. */
        itemptr =
            (ItemPointer)DatumGetPointer(ExecEvalExprSwitchContext(tidopexpr->exprstate, econtext, &isNull, NULL));

        /* If the bound is NULL, *nothing* matches the qual. */
        if (isNull)
            return false;

        if (tidopexpr->exprtype == TIDEXPR_LOWER_BOUND) {
            ItemPointerData lb;

            ItemPointerCopy(itemptr, &lb);

            /*
             * Normalize non-inclusive ranges to become inclusive.  The
             * resulting ItemPointer here may not be a valid item pointer.
             */
            if (!tidopexpr->inclusive)
                ItemPointerInc(&lb);

            /* Check if we can narrow the range using this qual */
            if (ItemPointerCompare(&lb, &lowerBound) > 0)
                ItemPointerCopy(&lb, &lowerBound);
        }

        else if (tidopexpr->exprtype == TIDEXPR_UPPER_BOUND) {
            ItemPointerData ub;

            ItemPointerCopy(itemptr, &ub);

            /*
             * Normalize non-inclusive ranges to become inclusive.  The
             * resulting ItemPointer here may not be a valid item pointer.
             */
            if (!tidopexpr->inclusive)
                ItemPointerDec(&ub);

            /* Check if we can narrow the range using this qual */
            if (ItemPointerCompare(&ub, &upperBound) < 0)
                ItemPointerCopy(&ub, &upperBound);
        }
    }

    ItemPointerCopy(&lowerBound, &node->trss_mintid);
    ItemPointerCopy(&upperBound, &node->trss_maxtid);

    return true;
}

bool heap_getnextslot_tidrange(TableScanDesc sscan, ScanDirection direction, TupleTableSlot *slot,
                               ItemPointer mintid, ItemPointer maxtid)
{
    HeapTuple tuple;

    while((tuple = heap_getnext(sscan, direction)) != NULL) {
        if (ItemPointerCompare(&tuple->t_self, mintid) < 0)
        {
            ExecClearTuple(slot);

            /*
             * When scanning backwards, the TIDs will be in descending order.
             * Future tuples in this direction will be lower still, so we can
             * just return false to indicate there will be no more tuples.
             */
            if (ScanDirectionIsBackward(direction))
                return false;

            continue;
        }

        /*
         * Likewise for the final page, we must filter out TIDs greater than
         * maxtid.
         */
        if (ItemPointerCompare(&tuple->t_self, maxtid) > 0)
        {
            ExecClearTuple(slot);

            /*
             * When scanning forward, the TIDs will be in ascending order.
             * Future tuples in this direction will be higher still, so we can
             * just return false to indicate there will be no more tuples.
             */
            if (ScanDirectionIsForward(direction))
                return false;
            continue;
        }

        break;
    }
    if (tuple) {
        ExecStoreTuple(tuple, slot, sscan->rs_cbuf, false);
        return true;
    }

    ExecClearTuple(slot);
    return false;
}

/*
 * heap_setscanlimits - restrict range of a heapscan
 *
 * startBlk is the page to start at
 * numBlks is number of pages to scan (InvalidBlockNumber means "all")
 */
void heap_setscanlimits(TableScanDesc sscan, BlockNumber startBlk, BlockNumber numBlks)
{
    Assert(!sscan->rs_inited); /* else too late to change */
    /* else rs_startblock is significant */
    Assert(!(sscan->rs_flags & SO_ALLOW_SYNC));

    /* Check startBlk is valid (but allow case of zero blocks...) */
    Assert(startBlk == 0 || startBlk < sscan->rs_nblocks);

    sscan->rs_startblock = startBlk;
}

void heap_set_tidrange(TableScanDesc sscan, ItemPointer mintid, ItemPointer maxtid)
{
    BlockNumber startBlk;
    BlockNumber numBlks;
    ItemPointerData highestItem;
    ItemPointerData lowestItem;

    /*
     * For relations without any pages, we can simply leave the TID range
     * unset.  There will be no tuples to scan, therefore no tuples outside
     * the given TID range.
     */
    if (sscan->rs_nblocks == 0)
        return;

    /*
     * Set up some ItemPointers which point to the first and last possible
     * tuples in the heap.
     */
    ItemPointerSet(&highestItem, sscan->rs_nblocks - 1, MaxOffsetNumber);
    ItemPointerSet(&lowestItem, 0, FirstOffsetNumber);

    /*
     * If the given maximum TID is below the highest possible TID in the
     * relation, then restrict the range to that, otherwise we scan to the end
     * of the relation.
     */
    if (ItemPointerCompare(maxtid, &highestItem) < 0)
        ItemPointerCopy(maxtid, &highestItem);

    /*
     * If the given minimum TID is above the lowest possible TID in the
     * relation, then restrict the range to only scan for TIDs above that.
     */
    if (ItemPointerCompare(mintid, &lowestItem) > 0)
        ItemPointerCopy(mintid, &lowestItem);

    /*
     * Check for an empty range and protect from would be negative results
     * from the numBlks calculation below.
     */
    if (ItemPointerCompare(&highestItem, &lowestItem) < 0)
    {
        /* Set an empty range of blocks to scan */
        heap_setscanlimits(sscan, 0, 0);
        return;
    }

    /*
     * Calculate the first block and the number of blocks we must scan. We
     * could be more aggressive here and perform some more validation to try
     * and further narrow the scope of blocks to scan by checking if the
     * lowerItem has an offset above MaxOffsetNumber.  In this case, we could
     * advance startBlk by one.  Likewise, if highestItem has an offset of 0
     * we could scan one fewer blocks.  However, such an optimization does not
     * seem worth troubling over, currently.
     */
    startBlk = ItemPointerGetBlockNumberNoCheck(&lowestItem);

    numBlks = ItemPointerGetBlockNumberNoCheck(&highestItem) -
        ItemPointerGetBlockNumberNoCheck(&lowestItem) + 1;

    /* Set the start block and number of blocks to scan */
    heap_setscanlimits(sscan, startBlk, numBlks);
}

/* ----------------------------------------------------------------
 * 		ExecReScanTidRangeScan(node)
 * ----------------------------------------------------------------
 */
static void ExecReScanTidRangeScan(ExtensiblePlanState *node)
{
    /* mark scan as not in progress, and tid range list as not computed yet */
    ((TidRangeScanState *)node)->trss_inScan = false;

    /*
     * We must wait until TidRangeNext before calling table_rescan_tidrange.
     */
    ExecScanReScan(&node->ss);
}

#define SO_TYPE_TIDRANGESCAN 1 << 9

/*
 * table_beginscan_tidrange is the entry point for setting up a TableScanDesc
 * for a TID range scan.
 */
static inline TableScanDesc table_beginscan_tidrange(Relation rel, Snapshot snapshot, ItemPointer mintid,
    ItemPointer maxtid)
{
    TableScanDesc sscan;
    uint32 flags = SO_TYPE_TIDRANGESCAN | SO_ALLOW_PAGEMODE;

    sscan = heap_beginscan(rel, snapshot, 0, NULL);

    sscan->rs_flags = flags;
    /* Set the range of TIDs to scan */
    heap_set_tidrange(sscan, mintid, maxtid);

    return sscan;
}

/*
 * table_rescan_tidrange resets the scan position and sets the minimum and
 * maximum TID range to scan for a TableScanDesc created by
 * table_beginscan_tidrange.
 */
static inline void table_rescan_tidrange(TableScanDesc sscan, ItemPointer mintid, ItemPointer maxtid)
{
    /* Ensure table_beginscan_tidrange() was used. */
    Assert((sscan->rs_flags & SO_TYPE_TIDRANGESCAN) != 0);

    heap_rescan(sscan, NULL);
    heap_set_tidrange(sscan, mintid, maxtid);
}

/* ----------------------------------------------------------------
 * 		TidRangeNext
 *
 * 		Retrieve a tuple from the TidRangeScan node's currentRelation
 * 		using the TIDs in the TidRangeScanState information.
 *
 * ----------------------------------------------------------------
 */
static TupleTableSlot *TidRangeNext(ExtensiblePlanState *node)
{
    TableScanDesc scandesc;
    EState *estate;
    ScanDirection direction;
    TupleTableSlot *slot;
    TidRangeScanState *trss;

    /*
     * extract necessary information from TID scan node
     */
    scandesc = node->ss.ss_currentScanDesc;
    estate = node->ss.ps.state;
    slot = node->ss.ss_ScanTupleSlot;
    direction = estate->es_direction;
    trss = (TidRangeScanState *)node;

    if (!trss->trss_inScan) {
        /* First time through, compute TID range to scan */
        if (!TidRangeEval(trss))
            return NULL;

        if (scandesc == NULL) {
            scandesc = table_beginscan_tidrange(node->ss.ss_currentRelation, estate->es_snapshot, &trss->trss_mintid,
                &trss->trss_maxtid);
            node->ss.ss_currentScanDesc = scandesc;
        } else {
            /* rescan with the updated TID range */
            table_rescan_tidrange(scandesc, &trss->trss_mintid, &trss->trss_maxtid);
        }

        trss->trss_inScan = true;
    }

    /* Fetch the next tuple. */
    if (!heap_getnextslot_tidrange(scandesc, direction, slot, &trss->trss_mintid, &trss->trss_maxtid)) {
        trss->trss_inScan = false;
        ExecClearTuple(slot);
    }

    return slot;
}

/*
 * TidRangeRecheck -- access method routine to recheck a tuple in EvalPlanQual
 */
static bool TidRangeRecheck(TidRangeScanState *node, TupleTableSlot *slot)
{
    return true;
}

/* ----------------------------------------------------------------
 * 		ExecTidRangeScan(node)
 *
 * 		Scans the relation using tids and returns the next qualifying tuple.
 * 		We call the ExecScan() routine and pass it the appropriate
 * 		access method functions.
 *
 * 		Conditions:
 * 		  -- the "cursor" maintained by the AMI is positioned at the tuple
 * 			 returned previously.
 *
 * 		Initial States:
 * 		  -- the relation indicated is opened for TID range scanning.
 * ----------------------------------------------------------------
 */
static TupleTableSlot *ExecTidRangeScan(ExtensiblePlanState *pstate)
{
    return ExecScan(&pstate->ss, (ExecScanAccessMtd)TidRangeNext, (ExecScanRecheckMtd)TidRangeRecheck);
}

/* ----------------------------------------------------------------
 * 		ExecEndTidRangeScan
 *
 * 		Releases any storage allocated through C routines.
 * 		Returns nothing.
 * ----------------------------------------------------------------
 */
static void ExecEndTidRangeScan(ExtensiblePlanState *node)
{
    TableScanDesc scan = node->ss.ss_currentScanDesc;

    if (scan != NULL)
        heap_endscan(scan);

    /*
     * Free the exprcontext
     */
    ExecFreeExprContext(&node->ss.ps);

    /*
     * clear out tuple table slots
     */
    if (node->ss.ps.ps_ResultTupleSlot)
        ExecClearTuple(node->ss.ps.ps_ResultTupleSlot);
    ExecClearTuple(node->ss.ss_ScanTupleSlot);
}

/*
 * ExplainTidRangeScan - A method of ExtensiblePlanState; that shows extra info
 * on EXPLAIN command.
 */
static void ExplainTidRangeScan(ExtensiblePlanState *node, List *ancestors, ExplainState *es)
{
    TidRangeScanState *ctss = (TidRangeScanState *) node;
    ExtensiblePlan *cscan = (ExtensiblePlan *) ctss->css.ss.ps.plan;

    /* logic copied from show_qual and show_expression */
    if (cscan->extensible_exprs)
    {
        bool useprefix = es->verbose;
        Node *qual;
        List *context;
        char *exprstr;

        /*
         * The tidrangequals list has AND semantics, so be sure to
         * show it as an AND condition.
         */
        qual = (Node*)make_ands_explicit(cscan->extensible_exprs);

        /* Set up deparsing context */
        context = deparse_context_for_planstate((Node*)ctss, ancestors, es->rtable);

        /* Deparse the expression */
        exprstr = deparse_expression(qual, context, useprefix, false);

        /* And add to es->str */
        ExplainPropertyText("TID Cond", exprstr, es);
    }
}

static tidrangescan_context* get_session_context()
{
    if (u_sess->attr.attr_common.extension_session_vars_array[tidrangescan_index] == NULL) {
        init_session_vars();
    }
    return (tidrangescan_context*)u_sess->attr.attr_common.extension_session_vars_array[tidrangescan_index];
}

void set_extension_index(uint32 index)
{
    tidrangescan_index = index;
}

void init_session_vars(void)
{
    RepallocSessionVarsArrayIfNecessary();

    tidrangescan_context *cxt = (tidrangescan_context*)MemoryContextAlloc(u_sess->self_mem_cxt, sizeof(tidrangescan_context));
    u_sess->attr.attr_common.extension_session_vars_array[tidrangescan_index] = cxt;
    cxt->enable_tidrangescan = true;

    DefineCustomBoolVariable("enable_tidrangescan",
                             "Enables the planner's use of tidrange-scan plans.",
                             NULL,
                             &ENABLE_TIDRANGESCAN,
                             true,
                             PGC_USERSET,
                             0,
                             NULL, NULL, NULL);
}

/*
 * Entrypoint of this extension
 */
void _PG_init(void)
{
    ereport(LOG, (errmsg("init tidrangescan.")));
    set_rel_pathlist_hook = SetTidRangeScanPath;
}

void _PG_fini(void)
{
    set_rel_pathlist_hook = NULL;
}