/* -------------------------------------------------------------------------
 * spq.cpp
 *
 * Portions Copyright (c) 2023 Huawei Technologies Co.,Ltd.
 *
 * IDENTIFICATION
 *        contrib/spq/spq.cpp
 *
 * -------------------------------------------------------------------------
 */
#include "postgres.h"
#include <stdint.h>
#include "nodes/nodeFuncs.h"
#include "catalog/pg_inherits_fn.h"
#include "commands/explain.h"
#include "commands/prepare.h"
#include "access/transam.h"
#include "optimizer/planner.h"
#include "executor/spq_seqscan.h"
#include "spq_optimizer_util/SPQOptimizer.h"
#include "spq_opt.h"
#include "guc_spq.h"
#include "spqplugin.h"
#include "storage/ipc.h"
#include "naucrates/init.h"

PG_MODULE_MAGIC;
PG_FUNCTION_INFO_V1(spqplugin_invoke);

THR_LOCAL spq_planner_hook_type backup_spq_planner_hook = NULL;
THR_LOCAL bool HOOK_INIT = false;
THR_LOCAL MemoryContext OptimizerMemoryContext = NULL;

static bool check_rangetbl_support(List* rtable)
{
    if (rtable == NULL)
        return false;
 
    ListCell* lc = NULL;
    foreach (lc, rtable) {
        RangeTblEntry* rte = (RangeTblEntry*)lfirst(lc);
        Assert(IsA(rte, RangeTblEntry));
        if (rte->rtekind == RTE_FUNCTION || rte->ispartrel) {
            return false;
        }
    }
    return true;
}

static bool check_disable_spq_planner_walker(Node *node, void *context)
{
#define CHECK_RETURN_HELP_LOG(condition, msg) \
    if ((condition)) {                         \
        elog(DEBUG1, "SPQ Failed: " msg);      \
        return true;                           \
    }

    ListCell *lc;
    Query *query;

    if (node == NULL)
        return false;

    if (!IsA(node, Query)) {
        return expression_tree_walker(node, (bool (*)())check_disable_spq_planner_walker, context);
    }

    /* Query node */
    query = (Query *)node;

    if (u_sess->attr.attr_spq.spq_enable_pre_optimizer_check) {
        CHECK_RETURN_HELP_LOG(query->groupingSets, "sql with groupingsets was not allowed in spq");
    }

    foreach (lc, query->rtable) {
        RangeTblEntry *rte = lfirst_node(RangeTblEntry, lc);

        switch (rte->rtekind) {
            case RTE_RELATION:
                if (rte->relkind == RELKIND_RELATION) {
                    Oid relid = rte->relid;
                    LOCKMODE lockmode = AccessShareLock;
                    Relation rel = heap_open(relid, lockmode);
                    /*
                     * No use SPQOPT:
                     * 1. temp table
                     * 2. table have any childrens
                     * 3. system catalogs
                     */

                    if (rel->rd_islocaltemp || (has_subclass(relid) && rte->relkind == RELKIND_RELATION) ||
                        rel->rd_id < FirstNormalObjectId) {
                        heap_close(rel, lockmode);

                        CHECK_RETURN_HELP_LOG(true, "sql not match some conditions was not allowed in spq");
                    }

                    heap_close(rel, lockmode);
                } else if (rte->relkind != RELKIND_VIEW && rte->relkind != RELKIND_MATVIEW) {
                    CHECK_RETURN_HELP_LOG(true, "only support relkind_relation and view/matview under RTE_RELATION");
                }
                break;
            case RTE_SUBQUERY:
                if (check_disable_spq_planner_walker((Node *)rte->subquery, context))
                    return true; 
                break;
            case RTE_JOIN:
            case RTE_CTE:
                break;
            case RTE_TABLEFUNC:
            case RTE_VALUES:
            case RTE_TABLEFUNCTION:
            case RTE_VOID:
            case RTE_NAMEDTUPLESTORE:
            default:
                CHECK_RETURN_HELP_LOG(true, "the RTEKIND is not supported");
        }     
    }

    return query_tree_walker(query, (bool (*)())check_disable_spq_planner_walker, context, 0);
}

static void spq_guc_init(knl_u_spq_context* spq_cxt)
{
    if (spq_cxt->spq_worker_context != nullptr) {
        return;
    }
    OptimizerMemoryContext = AllocSetContextCreate(t_thrd.top_mem_cxt,
                                                   "Optimizer Memory Context", ALLOCSET_DEFAULT_SIZES);
    spq_cxt->spq_worker_context = AllocSetContextCreate(t_thrd.top_mem_cxt,
                                                        "SpqSelfMemoryContext",
                                                        ALLOCSET_DEFAULT_MINSIZE,
                                                        ALLOCSET_DEFAULT_INITSIZE,
                                                        ALLOCSET_DEFAULT_MAXSIZE);

    MemoryContext oldContext = MemoryContextSwitchTo(spq_cxt->spq_worker_context);
    InitSpqConfigureNames();
    MemoryContextSwitchTo(oldContext);
}

static bool should_spq_planner(Query *parse)
{
    spq_guc_init(&u_sess->spq_cxt);
    if (!u_sess->attr.attr_spq.gauss_enable_spq || t_thrd.spq_ctx.num_nodes == 0) {
        elog(DEBUG1, "gauss_enable_spq is off or spq cluster map error");
        return false;
    }

    if (unlikely(parse == NULL)) {
        elog(ERROR, "parse should not be null.");
    }

    if (!parse->is_support_spq) {
        return false;
    }

    if (parse->commandType != CMD_SELECT) {
        elog(DEBUG1, "spq can not support this commandType.");
        return false;
    }

    if (IsTransactionBlock()) {
        elog(DEBUG1, "sql in transaction can`t run on spq node");
        return false;
    }
 
    if (!check_rangetbl_support(parse->rtable)) {
        return false;
    }

    if (!check_disable_spq_planner_walker((Node *)parse, NULL)) {
        InitSPQOPT();
        return true;
    }

    return false;
}

PlannedStmt* spq_optimize_query(Query* parse, int cursorOptions, ParamListInfo boundParams)
{
    PlannedStmt* result = NULL;
    instr_time starttime;
    double totaltime = 0;
    t_thrd.spq_ctx.spq_role = ROLE_UTILITY;
    if (should_spq_planner(parse)) {
        t_thrd.spq_ctx.spq_role = ROLE_QUERY_COORDINTOR;
        t_thrd.spq_ctx.spq_session_id = u_sess->debug_query_id;
        t_thrd.spq_ctx.current_id = 0; 
        t_thrd.spq_ctx.skip_direct_distribute_result = false;
        // if build spq plan fail go back
        result = spq_planner(parse, boundParams);
        if (result == nullptr) {
            t_thrd.spq_ctx.spq_role = ROLE_UTILITY;
        } else {
            set_default_stream();
            return result;
        }
    }

    INSTR_TIME_SET_CURRENT(starttime);

#ifdef PGXC
    /*
     * streaming engine hook for agg rewrite.
     */
    if (t_thrd.streaming_cxt.streaming_planner_hook)
        (*(planner_hook_type) t_thrd.streaming_cxt.streaming_planner_hook)\
                 (parse, cursorOptions, boundParams);
    /*
     * A Coordinator receiving a query from another Coordinator
     * is not allowed to go into PGXC planner.
     */
    if ((IS_PGXC_COORDINATOR || IS_SINGLE_NODE) && !IsConnFromCoord())
        result = pgxc_planner(parse, cursorOptions, boundParams);
    else
#endif
        result = standard_planner(parse, cursorOptions, boundParams);

    totaltime += elapsed_time(&starttime);
    result->plannertime = totaltime;
    if (u_sess->attr.attr_common.max_datanode_for_plan > 0 && IS_PGXC_COORDINATOR && !IsConnFromCoord()) {
        GetRemoteQuery(result, NULL);
    }
    return result;
}

void _PG_init(void)
{
    InitDXLManager();
    if (!HOOK_INIT) {
        backup_spq_planner_hook = spq_planner_hook;
        spq_planner_hook = spq_optimize_query;
        init_spqseqscan_hook();
		spq_guc_init(&u_sess->spq_cxt);
    }
    HOOK_INIT = true;
}

void _PG_fini(void)
{
    spq_planner_hook = backup_spq_planner_hook;
    MemoryContextDelete(u_sess->spq_cxt.spq_worker_context);
    restore_spqseqscan_hook();
}

void spqplugin_invoke(void)
{
    ereport(DEBUG2, (errmsg("dummy function to let process load this library.")));
    return;
}

