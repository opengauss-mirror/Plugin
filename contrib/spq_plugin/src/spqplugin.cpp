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
#include <unordered_map>
#include "nodes/nodeFuncs.h"
#include "catalog/pg_inherits_fn.h"
#include "commands/explain.h"
#include "commands/prepare.h"
#include "access/transam.h"
#include "optimizer/planner.h"
#include "executor/spq_seqscan.h"
#include "executor/spq_indexscan.h"
#include "executor/spq_indexonlyscan.h"
#include "executor/spq_bitmapheapscan.h"
#include "spq_optimizer_util/SPQOptimizer.h"
#include "spq_opt.h"
#include "guc_spq.h"
#include "spqplugin.h"
#include "storage/ipc.h"
#include "naucrates/init.h"
#include "ddes/dms/ss_transaction.h"
#include "optimizer/planmem_walker.h"

PG_MODULE_MAGIC;
PG_FUNCTION_INFO_V1(spqplugin_invoke);

THR_LOCAL ExecutorStart_hook_type spq_hook_ExecutorStart = NULL;
THR_LOCAL spq_planner_hook_type backup_spq_planner_hook = NULL;
THR_LOCAL bool HOOK_INIT = false;
THR_LOCAL MemoryContext OptimizerMemoryContext = NULL;

typedef struct SpqDirectReadWalkerContext {
    MethodPlanWalkerContext cxt;
    std::unordered_map<Oid, SpqDirectReadEntry>* directMap;
} SpqDirectReadWalkerContext;

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
        } else if (rte->rtekind == RTE_SUBQUERY) {
            Assert(rte->subquery != NULL);
            return check_rangetbl_support(rte->subquery->rtable);
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

#ifdef PGXC
    if (IsA(node, Aggref) && !((Aggref*)node)->agghas_collectfn) {
        return true;
    }
#endif

    if (!IsA(node, Query)) {
        return expression_tree_walker(node, (bool (*)())check_disable_spq_planner_walker, context);
    }

    /* Query node */
    query = (Query *)node;

    /* openGauss spq: ordered sensitive */
    if ((query->sortClause || !query->rtable) && query->commandType == CMD_INSERT) {
        if (u_sess->attr.attr_spq.spq_enable_insert_order_sensitive && 1 != u_sess->attr.attr_spq.spq_insert_dop_num) {
            elog(DEBUG1,"spq multi insert Failed: spq_enable_insert_order_sensitive is on. One writer worker should be used");
            u_sess->attr.attr_spq.spq_insert_dop_num = 1;
        }
    }
    /* openGauss end */

    /* check insert select from tableless_scan */
    if (query->commandType == CMD_INSERT && !query->rtable) {
        CHECK_RETURN_HELP_LOG(1 == u_sess->attr.attr_spq.spq_insert_dop_num,
            "Insert worker is only one, select from tableless doesn't use spq");
        CHECK_RETURN_HELP_LOG(!u_sess->attr.attr_spq.spq_enable_insert_from_tableless,
            "gauss_spq_enable_insert_from_tableless is off");
    }

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
            case RTE_VALUES:
            case RTE_TABLEFUNCTION:
            case RTE_VOID:
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

    if (CMD_INSERT == parse->commandType) {
        /* If spq_enable_insert_select is off, no use SPQOPT */
        if (!u_sess->attr.attr_spq.spq_enable_insert_select) {
            elog(DEBUG1, "spq Failed: param spq_enable_insert_select is off");
            return false;
        }

        /* Insert into .. VALUES(only one node) Case */
        if (1 == parse->rtable->length) {
            if (1 == u_sess->attr.attr_spq.spq_insert_dop_num) {
                elog(DEBUG1, "spq Failed: Insert worker is only one, select from tableless doesn't use spq");
                return false;
            }
            if (!u_sess->attr.attr_spq.spq_enable_insert_from_tableless) {
                elog(DEBUG1, "gauss_spq_enable_insert_from_tableless is off");
                return false;
            }
        }
    } else if (CMD_UPDATE == parse->commandType) {
        if (!u_sess->attr.attr_spq.spq_enable_update) {
            elog(DEBUG1, "spq Failed: param spq_enable_update is off");
            return false;
        }
    } else if(CMD_DELETE == parse->commandType) {
        if (!u_sess->attr.attr_spq.spq_enable_delete) {
            elog(DEBUG1, "spq Failed: param spq_enable_delete is off");
            return false;
        }
        if (parse->hasModifyingCTE) {
            elog(DEBUG1, "spq Failed: parallel delete can't support ModifyingCTE");
            return false;
        }
    } else {
        if (parse->commandType != CMD_SELECT) {
            elog(DEBUG1, "spq can not support this commandType.");
            return false;
        }
    }

    if (!u_sess->attr.attr_spq.spq_enable_transaction && IsTransactionBlock()) {
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
    if ((cursorOptions & CURSOR_OPT_SPQ_OK) && should_spq_planner(parse)) {
        t_thrd.spq_ctx.spq_role = ROLE_QUERY_COORDINTOR;
        t_thrd.spq_ctx.spq_session_id = u_sess->debug_query_id;
        t_thrd.spq_ctx.current_id = 0; 
        t_thrd.spq_ctx.skip_direct_distribute_result = false;
        // if build spq plan fail go back
        result = spq_planner(parse, boundParams);
        if (result == nullptr) {
            if (unlikely(cursorOptions & CURSOR_OPT_SPQ_FORCE))
            {
                ereport(ERROR,
                        (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                         errmsg("expected a spq plan but get a normal gauss plan.")));
            }
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

static void TryDirectRead(PlannedStmt* stmt, SpqSeqScan* scan, std::unordered_map<Oid, SpqDirectReadEntry>* directMap)
{
    RangeTblEntry *rte = (RangeTblEntry*)list_nth(stmt->rtable, (scan->scan.scanrelid)-1);
    if (rte->rtekind == RTE_RELATION) {
        Relation rel = heap_open(rte->relid, AccessShareLock);
        BlockNumber nblocks = RelationGetNumberOfBlocks(rel);
        BlockNumber nlimit = NORMAL_SHARED_BUFFER_NUM * u_sess->attr.attr_spq.spq_small_table_threshold;
        auto it = directMap->find(rel->rd_id);
        if (it == directMap->end()) {
            if (SS_STANDBY_MODE || nblocks <= nlimit) {
                // SS_STANDBY_MODE treat as false, may be change after libpq connected
                SpqDirectReadEntry tmp;
                tmp.rel_id = rel->rd_id;
                tmp.nums = InvalidBlockNumber;
                tmp.spq_seq_scan_node_list = NIL;
                if (nblocks > nlimit) {
                    tmp.spq_seq_scan_node_list = lappend(tmp.spq_seq_scan_node_list, scan);
                }
                (*directMap)[rel->rd_id] = tmp;
                scan->isDirectRead = false;
                scan->DirectReadBlkNum = InvalidBlockNumber;
            } else {
                heap_sync(rel);
                SpqDirectReadEntry tmp;
                tmp.rel_id = rel->rd_id;
                tmp.nums = nblocks;
                tmp.spq_seq_scan_node_list = NIL;
                (*directMap)[rel->rd_id] = tmp;
                scan->isDirectRead = true;
                scan->DirectReadBlkNum = nblocks;
            }
        } else {
            if (it->second.nums != InvalidBlockNumber) {
                scan->isDirectRead = true;
                scan->DirectReadBlkNum = it->second.nums;
            } else {
                scan->isDirectRead = false;
                scan->DirectReadBlkNum = InvalidBlockNumber;
                if (it->second.spq_seq_scan_node_list != NIL) {
                    it->second.spq_seq_scan_node_list = lappend(it->second.spq_seq_scan_node_list, scan);
                }
            }
        }
        heap_close(rel, AccessShareLock);
    }
}

static bool TraversePlan(Node* plan, void* cxt)
{
    if (plan == nullptr) return false;

    if (IsA(plan, RemoteQuery)) {
        return walk_plan_node_fields((Plan*)plan, (MethodWalker)TraversePlan, cxt);
    }

    if (IsA(plan, SpqSeqScan)) {
        SpqDirectReadWalkerContext* walkerCxt = (SpqDirectReadWalkerContext*)cxt;
        PlannedStmt* stmt = (PlannedStmt*)walkerCxt->cxt.base.node;
        TryDirectRead(stmt, castNode(SpqSeqScan, plan), walkerCxt->directMap);
    }

    return plan_tree_walker(plan, (MethodWalker)TraversePlan, cxt);
}

static void spq_executor_start(QueryDesc* queryDesc, int eflags)
{
    if (t_thrd.spq_ctx.spq_role == ROLE_QUERY_COORDINTOR && !(eflags & EXEC_FLAG_EXPLAIN_ONLY) &&
        u_sess->attr.attr_spq.spq_enable_direct_read) {
        u_sess->spq_cxt.direct_read_map = NIL;
        PlannedStmt *stmt = queryDesc->plannedstmt;
        std::unordered_map<Oid, SpqDirectReadEntry> directMap;
        SpqDirectReadWalkerContext cxt;
        errno_t rc = 0;
        rc = memset_s(&cxt, sizeof(SpqDirectReadWalkerContext), 0, sizeof(SpqDirectReadWalkerContext));
        securec_check(rc, "\0", "\0");
        cxt.cxt.base.init_plans = NIL;
        cxt.cxt.base.traverse_flag = NULL;
        exec_init_plan_tree_base(&cxt.cxt.base, stmt);
        cxt.directMap = &directMap;
        TraversePlan((Node *)stmt->planTree, (void*)&cxt);
        for (auto it = directMap.begin(); it != directMap.end(); ++it) {
            if (it->second.spq_seq_scan_node_list != NIL) {
                SpqDirectReadEntry* entry = (SpqDirectReadEntry*)palloc0(sizeof(SpqDirectReadEntry));
                entry->rel_id = it->second.rel_id;
                entry->nums = it->second.nums;
                entry->spq_seq_scan_node_list = it->second.spq_seq_scan_node_list;
                u_sess->spq_cxt.direct_read_map = lappend(u_sess->spq_cxt.direct_read_map, entry);
            }
        }
    }

    if (spq_hook_ExecutorStart)
        spq_hook_ExecutorStart(queryDesc, eflags);
    else
        standard_ExecutorStart(queryDesc, eflags);
}

void _PG_init(void)
{
    InitDXLManager();
    if (!HOOK_INIT) {
        spq_hook_ExecutorStart = ExecutorStart_hook;
        ExecutorStart_hook = spq_executor_start;
        backup_spq_planner_hook = spq_planner_hook;
        spq_planner_hook = spq_optimize_query;
        init_spqseqscan_hook();
        init_spqindexscan_hook();
        init_spqindexonlyscan_hook();
        init_spqbitmapheapscan_hook();
		spq_guc_init(&u_sess->spq_cxt);
    }
    HOOK_INIT = true;
}

void _PG_fini(void)
{
    ExecutorStart_hook = spq_hook_ExecutorStart;
    spq_planner_hook = backup_spq_planner_hook;
    MemoryContextDelete(u_sess->spq_cxt.spq_worker_context);
    restore_spqseqscan_hook();
    restore_spqindexscan_hook();
    restore_spqindexonlyscan_hook();
    restore_spqbitmapheapscan_hook();
}

void spqplugin_invoke(void)
{
    ereport(DEBUG2, (errmsg("dummy function to let process load this library.")));
    return;
}

