#include "postgres.h"
#include "knl/knl_variable.h"
#include "catalog/pg_constraint.h"
#include "nodes/makefuncs.h"
#include "optimizer/tlist.h"
#include "parser/analyze.h"
#include "parser/parse_agg.h"
#include "parser/parse_clause.h"
#include "parser/parse_collate.h"
#include "parser/parsetree.h"
#include "parser/parse_cte.h"
#include "parser/parse_target.h"
#include "rewrite/rewriteManip.h"
#include "instruments/instr_unique_sql.h"
#include "commands/extension.h"
#include "custom_analyzer_demo.h"

PG_MODULE_MAGIC;

static custom_analyzer_context* get_session_context();
static Query* demo_transformSelectStmt(ParseState* pstate, SelectStmt* stmt, bool isFirstNode, bool isCreateView);
static void demo_parseCheckAggregates(ParseState* pstate, Query* qry);

static uint32 extension_index;
static void *preAnalyzerRoutine;

#define EXTENSION_NAME "custom_analyzer_demo"
#define ENABLE_CUSTOM_ANALYZER (get_session_context()->enable_custom_analyzer)

typedef struct {
    ParseState* pstate;
    Query* qry;
    PlannerInfo* root;
    List* groupClauses;
    List* groupClauseCommonVars;
    bool have_non_var_grouping;
    List** func_grouped_rels;
    int sublevels_up;
    bool in_agg_direct_args;
} check_ungrouped_columns_context;

static AnalyzerRoutine g_routine = {
    transSelect: demo_transformSelectStmt
};

PG_FUNCTION_INFO_V1(custom_analyzer_demo_invoke);
void custom_analyzer_demo_invoke(void)
{
    ereport(DEBUG2, (errmsg("dummy function to let process load this library.")));
    return;
}

static void AssignCustomAnalyzer(bool newval, void* extra)
{
    if (ENABLE_CUSTOM_ANALYZER == false && newval == true) {
        u_sess->hook_cxt.analyzerRoutineHook = &g_routine;
    } else if (ENABLE_CUSTOM_ANALYZER == true && newval == false) {
        u_sess->hook_cxt.analyzerRoutineHook = preAnalyzerRoutine;
    }
}

static custom_analyzer_context* get_session_context()
{
    if (u_sess->attr.attr_common.extension_session_vars_array[extension_index] == NULL) {
        init_session_vars();
    }
    return (custom_analyzer_context*)u_sess->attr.attr_common.extension_session_vars_array[extension_index];
}

void set_extension_index(uint32 index)
{
    extension_index = index;
}

void init_session_vars(void)
{
    RepallocSessionVarsArrayIfNecessary();

    custom_analyzer_context *cxt = (custom_analyzer_context*)MemoryContextAlloc(u_sess->self_mem_cxt, sizeof(custom_analyzer_context));
    u_sess->attr.attr_common.extension_session_vars_array[extension_index] = cxt;
    cxt->enable_custom_analyzer = false;

    DefineCustomBoolVariable(EXTENSION_NAME".enable_custom_analyzer",
                                    "Enable custom analyzer",
                                    NULL,
                                    &ENABLE_CUSTOM_ANALYZER,
                                    false,
                                    PGC_USERSET,
                                    0,
                                    NULL, AssignCustomAnalyzer, NULL);
    EmitWarningsOnPlaceholders(EXTENSION_NAME);
}

void _PG_init(void)
{
    uint32 version = pg_atomic_read_u32(&WorkingGrandVersionNum);
    if (version < ANALYZER_HOOK_VERSION_NUM) {
        ereport(ERROR,
            (errmsg("Current version(%u) does not support analyzer hook, please upgrade your gaussdb to version: %u",
            version, ANALYZER_HOOK_VERSION_NUM)));
    }
    ereport(LOG, (errmsg("init " EXTENSION_NAME)));
    preAnalyzerRoutine = u_sess->hook_cxt.analyzerRoutineHook;
}

void _PG_fini(void)
{
    u_sess->hook_cxt.analyzerRoutineHook = preAnalyzerRoutine;
}

static bool checkForeignTableExist(List* rte_table_list)
{
    bool ret_val = false;
    ListCell* lc = NULL;
    RangeTblEntry* rte = NULL;

    if (rte_table_list == NULL) {
        return ret_val;
    }

    foreach (lc, rte_table_list) {
        rte = (RangeTblEntry*)lfirst(lc);
        if (rte->relkind == RELKIND_FOREIGN_TABLE || rte->relkind == RELKIND_STREAM) {
            ret_val = true;
            break;
        }
    }

    return ret_val;
}

static void set_ancestor_ps_contain_foreigntbl(ParseState* subParseState)
{
    ParseState* ancestor_ps = NULL;
    ParseState* temp_ps = NULL;

    if (unlikely(subParseState == NULL)) {
        ereport(ERROR,
            (errcode(ERRCODE_UNEXPECTED_NULL_VALUE), 
                errmsg("subParseState should not be null")));
    }

    /* If the subParseState's parentParseState is NULL then do nothing. */
    if (subParseState->parentParseState == NULL) {
        return;
    }

    /* We go back through parentParseState to find top level parseState. */
    temp_ps = subParseState;
    while (temp_ps != NULL) {
        if (temp_ps->parentParseState != NULL) {
            ancestor_ps = temp_ps->parentParseState;
            /*
             * Because there should at least one insert statement on top of
             * this select statement, we mark find foreign table in all top level
             * insert statements.
             */
            if (ancestor_ps->p_is_insert) {
                ancestor_ps->p_is_foreignTbl_exist = true;
            }
        }
        temp_ps = temp_ps->parentParseState;
    }
}

static bool include_groupingset(Node* groupClause)
{
    if (groupClause == NULL) {
        return false;
    }

    if (IsA(groupClause, List)) {
        List* group_list = (List*)groupClause;

        ListCell* lc = NULL;

        foreach (lc, group_list) {
            Node* node = (Node*)lfirst(lc);

            if (include_groupingset(node)) {
                return true;
            }
        }
    } else if (IsA(groupClause, GroupingSet)) {
        return true;
    }

    return false;
}

static void transformGroupConstToColumn(ParseState* pstate, Node* groupClause, List* targetList)
{
    if (groupClause == NULL) {
        return;
    }

    if (IsA(groupClause, List)) {
        List* group_list = (List*)groupClause;

        ListCell* lc = NULL;

        foreach (lc, group_list) {
            Node* node = (Node*)lfirst(lc);

            /* Replace this const by expr in targetlist. */
            if (IsA(node, A_Const)) {
                Value* val = &((A_Const*)node)->val;
                int location = ((A_Const*)node)->location;

                if (!IsA(val, Integer)) {
                    ereport(ERROR,
                        (errcode(ERRCODE_SYNTAX_ERROR),
                            errmsg("non-integer constant in group clause"),
                            parser_errposition(pstate, location)));
                }

                long target_pos = intVal(val);
                if (target_pos <= list_length(targetList)) {
                    TargetEntry* tle = (TargetEntry*)list_nth(targetList, target_pos - 1);
                    lfirst(lc) = copyObject(tle->expr);

                    pfree_ext(node);
                } else {
                    ereport(ERROR,
                        (errcode(ERRCODE_INVALID_COLUMN_REFERENCE),
                            errmsg("%s position %ld is not in select list", "GROUP BY", target_pos),
                            parser_errposition(pstate, location)));
                }

            } else {
                transformGroupConstToColumn(pstate, node, targetList);
            }
        }
    } else if (IsA(groupClause, GroupingSet)) {
        GroupingSet* grouping_set = (GroupingSet*)groupClause;
        transformGroupConstToColumn(pstate, (Node*)grouping_set->content, targetList);
    }
}

static void transformLockingClause(ParseState* pstate, Query* qry, LockingClause* lc, bool pushedDown)
{
    List* lockedRels = lc->lockedRels;
    ListCell* l = NULL;
    ListCell* rt = NULL;
    Index i;
    LockingClause* allrels = NULL;
    Relation rel;

    CheckSelectLocking(qry);

    /* make a clause we can pass down to subqueries to select all rels */
    allrels = makeNode(LockingClause);
    allrels->lockedRels = NIL; /* indicates all rels */
    allrels->forUpdate = lc->forUpdate;
    allrels->noWait = lc->noWait;

    if (lockedRels == NIL) {
        /* all regular tables used in query */
        i = 0;
        foreach (rt, qry->rtable) {
            RangeTblEntry* rte = (RangeTblEntry*)lfirst(rt);

            ++i;
            switch (rte->rtekind) {
                case RTE_RELATION:
                    rel = relation_open(rte->relid, AccessShareLock);
                    if (RelationIsColStore(rel)) {
                        heap_close(rel, AccessShareLock);
                        ereport(ERROR,
                            (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                errmsg("SELECT FOR UPDATE/SHARE cannot be used with column table \"%s\"",
                                    rte->eref->aliasname)));
                    }

                    heap_close(rel, AccessShareLock);

                    applyLockingClause(qry, i, lc->forUpdate, lc->noWait, pushedDown);
                    rte->requiredPerms |= ACL_SELECT_FOR_UPDATE;
                    break;
                case RTE_SUBQUERY:
                    applyLockingClause(qry, i, lc->forUpdate, lc->noWait, pushedDown);

                    /*
                     * FOR UPDATE/SHARE of subquery is propagated to all of
                     * subquery's rels, too.  We could do this later (based on
                     * the marking of the subquery RTE) but it is convenient
                     * to have local knowledge in each query level about which
                     * rels need to be opened with RowShareLock.
                     */
                    transformLockingClause(pstate, rte->subquery, allrels, true);
                    break;
                case RTE_CTE:
                    ereport(ERROR,
                        (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                            errmsg("SELECT FOR UPDATE/SHARE cannot be applied to a WITH query")));
                    break;
                default:
                    /* ignore JOIN, SPECIAL, FUNCTION, VALUES, CTE RTEs */
                    break;
            }
        }
    } else {
        /* just the named tables */
        foreach (l, lockedRels) {
            RangeVar* thisrel = (RangeVar*)lfirst(l);

            /* For simplicity we insist on unqualified alias names here */
            if (thisrel->catalogname || thisrel->schemaname) {
                ereport(ERROR,
                    (errcode(ERRCODE_SYNTAX_ERROR),
                        errmsg("SELECT FOR UPDATE/SHARE must specify unqualified relation names"),
                        parser_errposition(pstate, thisrel->location)));
            }

            i = 0;
            foreach (rt, qry->rtable) {
                RangeTblEntry* rte = (RangeTblEntry*)lfirst(rt);

                ++i;
                if (strcmp(rte->eref->aliasname, thisrel->relname) == 0) {
                    switch (rte->rtekind) {
                        case RTE_RELATION:
                            rel = relation_open(rte->relid, AccessShareLock);
                            if (RelationIsColStore(rel)) {
                                heap_close(rel, AccessShareLock);
                                ereport(ERROR,
                                    (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                        errmsg("SELECT FOR UPDATE/SHARE cannot be used with column table \"%s\"",
                                            rte->eref->aliasname),
                                        parser_errposition(pstate, thisrel->location)));
                            }

                            heap_close(rel, AccessShareLock);
                            applyLockingClause(qry, i, lc->forUpdate, lc->noWait, pushedDown);
                            rte->requiredPerms |= ACL_SELECT_FOR_UPDATE;
                            break;
                        case RTE_SUBQUERY:
                            applyLockingClause(qry, i, lc->forUpdate, lc->noWait, pushedDown);
                            /* see comment above */
                            transformLockingClause(pstate, rte->subquery, allrels, true);
                            break;
                        case RTE_JOIN:
                            ereport(ERROR,
                                (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                    errmsg("SELECT FOR UPDATE/SHARE cannot be applied to a join"),
                                    parser_errposition(pstate, thisrel->location)));
                            break;
                        case RTE_FUNCTION:
                            ereport(ERROR,
                                (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                    errmsg("SELECT FOR UPDATE/SHARE cannot be applied to a function"),
                                    parser_errposition(pstate, thisrel->location)));
                            break;
                        case RTE_VALUES:
                            ereport(ERROR,
                                (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                    errmsg("SELECT FOR UPDATE/SHARE cannot be applied to VALUES"),
                                    parser_errposition(pstate, thisrel->location)));
                            break;
                        case RTE_CTE:
                            ereport(ERROR,
                                (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                    errmsg("SELECT FOR UPDATE/SHARE cannot be applied to a WITH query"),
                                    parser_errposition(pstate, thisrel->location)));
                            break;
                        default:
                            ereport(ERROR,
                                (errcode(ERRCODE_UNRECOGNIZED_NODE_TYPE),
                                    errmsg("unrecognized RTE type: %d", (int)rte->rtekind)));
                            break;
                    }
                    break; /* out of foreach loop */
                }
            }
            if (rt == NULL) {
                ereport(ERROR,
                    (errcode(ERRCODE_UNDEFINED_TABLE),
                        errmsg("relation \"%s\" in FOR UPDATE/SHARE clause not found in FROM clause", thisrel->relname),
                        parser_errposition(pstate, thisrel->location)));
            }
        }
    }
}

static bool finalize_grouping_exprs_walker(Node* node, check_ungrouped_columns_context* context)
{
    ListCell* gl = NULL;

    if (node == NULL) {
        return false;
    }
    if (IsA(node, Const) || IsA(node, Param)) {
        return false; /* constants are always acceptable */
    }

    if (IsA(node, Aggref)) {
        Aggref* agg = (Aggref*)node;

        if ((int)agg->agglevelsup == context->sublevels_up) {
            /*
             * If we find an aggregate call of the original level, do not
             * recurse into its normal arguments, ORDER BY arguments, or
             * filter; GROUPING exprs of this level are not allowed there. But
             * check direct arguments as though they weren't in an aggregate.
             */
            bool result = false;

            AssertEreport(!context->in_agg_direct_args, MOD_OPT, "");
            context->in_agg_direct_args = true;
            result = finalize_grouping_exprs_walker((Node*)agg->aggdirectargs, context);
            context->in_agg_direct_args = false;
            return result;
        }

        /*
         * We can skip recursing into aggregates of higher levels altogether,
         * since they could not possibly contain exprs of concern to us (see
         * transformAggregateCall).  We do need to look at aggregates of lower
         * levels, however.
         */
        if ((int)agg->agglevelsup > context->sublevels_up) {
            return false;
        }
    }

    if (IsA(node, GroupingFunc)) {
        GroupingFunc* grp = (GroupingFunc*)node;

        /*
         * We only need to check GroupingFunc nodes at the exact level to
         * which they belong, since they cannot mix levels in arguments.
         */
        if ((int)grp->agglevelsup == context->sublevels_up) {
            ListCell* lc = NULL;
            List* ref_list = NIL;

            foreach (lc, grp->args) {
                Node* expr = (Node*)lfirst(lc);
                Index ref = 0;

                if (context->root != NULL) {
                    expr = flatten_join_alias_vars(context->root, expr);
                }

                /*
                 * Each expression must match a grouping entry at the current
                 * query level. Unlike the general expression case, we don't
                 * allow functional dependencies or outer references.
                 */
                if (IsA(expr, Var)) {
                    Var* var = (Var*)expr;

                    if ((int)var->varlevelsup == context->sublevels_up) {
                        foreach (gl, context->groupClauses) {
                            TargetEntry* tle = (TargetEntry*)lfirst(gl);
                            Var* gvar = (Var*)tle->expr;

                            if (IsA(gvar, Var) && gvar->varno == var->varno && gvar->varattno == var->varattno &&
                                gvar->varlevelsup == 0) {
                                ref = tle->ressortgroupref;
                                break;
                            }
                        }
                    }
                } else if (context->have_non_var_grouping && context->sublevels_up == 0) {
                    foreach (gl, context->groupClauses) {
                        TargetEntry* tle = (TargetEntry*)lfirst(gl);

                        if (equal(expr, tle->expr)) {
                            ref = tle->ressortgroupref;
                            break;
                        }
                    }
                }

                if (ref == 0) {
                    ereport(ERROR,
                        (errcode(ERRCODE_GROUPING_ERROR),
                            errmsg("arguments to GROUPING must be grouping expressions of the associated query level"),
                            parser_errposition(context->pstate, exprLocation(expr))));
                }

                ref_list = lappend_int(ref_list, ref);
            }

            grp->refs = ref_list;
        }

        if ((int)grp->agglevelsup > context->sublevels_up) {
            return false;
        }
    }

    if (IsA(node, Query)) {
        /* Recurse into subselects */
        bool result = false;

        context->sublevels_up++;
        result = query_tree_walker((Query*)node, (bool (*)())finalize_grouping_exprs_walker, (void*)context, 0);
        context->sublevels_up--;
        return result;
    }
    return expression_tree_walker(node, (bool (*)())finalize_grouping_exprs_walker, (void*)context);
}

#ifndef ENABLE_MULTIPLE_NODES
static void find_rownum_in_groupby_clauses(Rownum *rownumVar, check_ungrouped_columns_context *context)
{
    bool haveRownum = false;
    ListCell *gl = NULL;

    if (!context->have_non_var_grouping || context->sublevels_up != 0) {
        foreach (gl, context->groupClauses) {
            Node *gnode = (Node *)((TargetEntry *)lfirst(gl))->expr;
            if (IsA(gnode, Rownum)) {
                haveRownum = true;
                break;
            }
        }

        if (haveRownum == false) {
            ereport(ERROR, (errcode(ERRCODE_GROUPING_ERROR),
                errmsg("ROWNUM must appear in the GROUP BY clause or be used in an aggregate function"),
                parser_errposition(context->pstate, rownumVar->location)));
        }
    }
}
#endif

static void finalize_grouping_exprs(
    Node* node, ParseState* pstate, Query* qry, List* groupClauses, PlannerInfo* root, bool have_non_var_grouping)
{
    check_ungrouped_columns_context context;

    context.pstate = pstate;
    context.qry = qry;
    context.root = root;
    context.groupClauses = groupClauses;
    context.groupClauseCommonVars = NIL;
    context.have_non_var_grouping = have_non_var_grouping;
    context.func_grouped_rels = NULL;
    context.sublevels_up = 0;
    context.in_agg_direct_args = false;
    (void)finalize_grouping_exprs_walker(node, &context);
}

static bool check_ungrouped_columns_walker(Node* node, check_ungrouped_columns_context* context)
{
    ListCell* gl = NULL;

    if (node == NULL) {
        return false;
    }
    if (IsA(node, Const) || IsA(node, Param)) {
        return false; /* constants are always acceptable */
    }

    if (IsA(node, Aggref)) {
        Aggref* agg = (Aggref*)node;

        if ((int)agg->agglevelsup == context->sublevels_up) {
            /*
             * For ordered set agg, its direct args should not inside an
             * aggregate. If we find an aggregate call of the original level
             * (that means if it is inside an outer query , the context should
             * be same), do not recurse into its normal arguments, ORDER BY
             * arguments, or filter; ungrouped vars there are not an error.
             * We use in_agg_direct_args in the context to help produce a useful
             * error message for ungrouped vars in direct arguments.
             */
            bool result = false;

            if (context->in_agg_direct_args) {
                ereport(ERROR, (errcode(ERRCODE_INVALID_AGG), errmsg("unexpected args inside agg direct args")));
            }
            context->in_agg_direct_args = true;
            result = check_ungrouped_columns_walker((Node*)agg->aggdirectargs, context);
            context->in_agg_direct_args = false;
            return result;
        }

        /*
         * We can also skip looking at the arguments of aggregates of higher levels,
         * since they could not possibly contain Vars of concern to us (see
         * transformAggregateCall).  We do need to look into arguments of aggregates
         * of lower levels, however.
         */
        if ((int)agg->agglevelsup > context->sublevels_up) {
            return false;
        }
    }

    if (IsA(node, GroupingFunc)) {
        GroupingFunc* grp = (GroupingFunc*)node;

        /* handled GroupingFunc separately, no need to recheck at this level */
        if ((int)grp->agglevelsup >= context->sublevels_up) {
            return false;
        }
    }
    /*
     * If we have any GROUP BY items that are not simple Vars, check to see if
     * subexpression as a whole matches any GROUP BY item. We need to do this
     * at every recursion level so that we recognize GROUPed-BY expressions
     * before reaching variables within them. But this only works at the outer
     * query level, as noted above.
     */
    if (context->have_non_var_grouping && context->sublevels_up == 0) {
        foreach (gl, context->groupClauses) {
            TargetEntry* tle = (TargetEntry*)lfirst(gl);
            if (equal(node, tle->expr)) {
                return false; /* acceptable, do not descend more */
            }
        }
    }

#ifndef ENABLE_MULTIPLE_NODES
    /* If There is ROWNUM, it must appear in the GROUP BY clause or be used in an aggregate function. */
    if (IsA(node, Rownum)) {
        find_rownum_in_groupby_clauses((Rownum *)node, context);
    }
#endif
    /*
     * If we have an ungrouped Var of the original query level, we have a
     * failure.  Vars below the original query level are not a problem, and
     * neither are Vars from above it.	(If such Vars are ungrouped as far as
     * their own query level is concerned, that's someone else's problem...)
     */
    if (IsA(node, Var)) {
        Var* var = (Var*)node;
        RangeTblEntry* rte = NULL;
        char* attname = NULL;

        if (var->varlevelsup != (unsigned int)context->sublevels_up) {
            return false; /* it's not local to my query, ignore */
        }

        /*
         * Check for a match, if we didn't do it above.
         */
        if (!context->have_non_var_grouping || context->sublevels_up != 0) {
            foreach (gl, context->groupClauses) {
                Var* gvar = (Var*)((TargetEntry*)lfirst(gl))->expr;

                if (IsA(gvar, Var) && gvar->varno == var->varno && gvar->varattno == var->varattno &&
                    gvar->varlevelsup == 0)
                    return false; /* acceptable, we're okay */
            }
        }

        /*
         * Check whether the Var is known functionally dependent on the GROUP
         * BY columns.	If so, we can allow the Var to be used, because the
         * grouping is really a no-op for this table.  However, this deduction
         * depends on one or more constraints of the table, so we have to add
         * those constraints to the query's constraintDeps list, because it's
         * not semantically valid anymore if the constraint(s) get dropped.
         * (Therefore, this check must be the last-ditch effort before raising
         * error: we don't want to add dependencies unnecessarily.)
         *
         * Because this is a pretty expensive check, and will have the same
         * outcome for all columns of a table, we remember which RTEs we've
         * already proven functional dependency for in the func_grouped_rels
         * list.  This test also prevents us from adding duplicate entries to
         * the constraintDeps list.
         */
        if (list_member_int(*context->func_grouped_rels, var->varno)) {
            return false; /* previously proven acceptable */
        }

        AssertEreport(
            var->varno > 0 && (int)var->varno <= list_length(context->pstate->p_rtable), MOD_OPT, "Var is unexpected");
        rte = rt_fetch(var->varno, context->pstate->p_rtable);
        if (rte->rtekind == RTE_RELATION) {
            if (check_functional_grouping(
                    rte->relid, var->varno, 0, context->groupClauseCommonVars, &context->qry->constraintDeps)) {
                *context->func_grouped_rels = lappend_int(*context->func_grouped_rels, var->varno);
                return false; /* acceptable */
            }
        }

        /* Found an ungrouped local variable; generate error message */
        attname = get_rte_attribute_name(rte, var->varattno);
        if (context->sublevels_up == 0) {
            ereport(DEBUG2,
                (errmsg("column \"%s.%s\" not appear in the GROUP BY clause or not be used in an aggregate function",
                        rte->eref->aliasname,
                        attname)));
        } else {
            ereport(ERROR,
                (errcode(ERRCODE_GROUPING_ERROR),
                    errmsg("subquery uses ungrouped column \"%s.%s\" from outer query", rte->eref->aliasname, attname),
                    parser_errposition(context->pstate, var->location)));
        }
        if (attname != NULL) {
            pfree_ext(attname);
        }
    }

    if (IsA(node, Query)) {
        /* Recurse into subselects */
        bool result = false;

        context->sublevels_up++;
        result = query_tree_walker((Query*)node, (bool (*)())check_ungrouped_columns_walker, (void*)context, 0);
        context->sublevels_up--;
        return result;
    }
    return expression_tree_walker(node, (bool (*)())check_ungrouped_columns_walker, (void*)context);
}

static void check_ungrouped_columns(Node* node, ParseState* pstate, Query* qry, List* groupClauses,
    List* groupClauseCommonVars, bool have_non_var_grouping, List** func_grouped_rels)
{
    check_ungrouped_columns_context context;

    context.pstate = pstate;
    context.qry = qry;
    context.root = NULL;
    context.groupClauses = groupClauses;
    context.groupClauseCommonVars = groupClauseCommonVars;
    context.have_non_var_grouping = have_non_var_grouping;
    context.func_grouped_rels = func_grouped_rels;
    context.sublevels_up = 0;
    context.in_agg_direct_args = false;
    (void)check_ungrouped_columns_walker(node, &context);
}

/*
 * transformSelectStmt -
 *	  transforms a Select Statement
 *
 * Note: this covers only cases with no set operations and no VALUES lists;
 * see below for the other cases.
 */
static Query* demo_transformSelectStmt(ParseState* pstate, SelectStmt* stmt, bool isFirstNode, bool isCreateView)
{
    Query* qry = makeNode(Query);
    Node* qual = NULL;
    ListCell* l = NULL;

    qry->commandType = CMD_SELECT;

    /* process the WITH clause independently of all else */
    if (stmt->withClause) {
        qry->hasRecursive = stmt->withClause->recursive;
        qry->cteList = transformWithClause(pstate, stmt->withClause);
        qry->hasModifyingCTE = pstate->p_hasModifyingCTE;
    }

    /* Complain if we get called from someplace where INTO is not allowed */
    if (stmt->intoClause) {
        ereport(ERROR,
            (errcode(ERRCODE_SYNTAX_ERROR),
                errmsg("SELECT ... INTO is not allowed here"),
                parser_errposition(pstate, exprLocation((Node*)stmt->intoClause))));
    }

    /* make FOR UPDATE/FOR SHARE info available to addRangeTableEntry */
    pstate->p_locking_clause = stmt->lockingClause;

    /* make WINDOW info available for window functions, too */
    pstate->p_windowdefs = stmt->windowClause;

    /* process the FROM clause */
    transformFromClause(pstate, stmt->fromClause, isFirstNode, isCreateView);

    /* transform targetlist */
    qry->targetList = transformTargetList(pstate, stmt->targetList);

    /* Transform operator "(+)" to outer join */
    if (stmt->hasPlus && stmt->whereClause != NULL) {
        transformOperatorPlus(pstate, &stmt->whereClause);
    }

    qry->starStart = list_copy(pstate->p_star_start);
    qry->starEnd = list_copy(pstate->p_star_end);
    qry->starOnly = list_copy(pstate->p_star_only);

    /* mark column origins */
    markTargetListOrigins(pstate, qry->targetList);

    /* transform WHERE
     * Only "(+)" is valid when  it's in WhereClause of Select, set the flag to be trure
     * during transform Whereclause.
     */
    setIgnorePlusFlag(pstate, true);
    qual = transformWhereClause(pstate, stmt->whereClause, "WHERE");
    setIgnorePlusFlag(pstate, false);

    /*
     * Initial processing of HAVING clause is just like WHERE clause.
     */
    qry->havingQual = transformWhereClause(pstate, stmt->havingClause, "HAVING");

    /*
     * Transform sorting/grouping stuff.  Do ORDER BY first because both
     * transformGroupClause and transformDistinctClause need the results. Note
     * that these functions can also change the targetList, so it's passed to
     * them by reference.
     */
    qry->sortClause = transformSortClause(
        pstate, stmt->sortClause, &qry->targetList, true /* fix unknowns */, false /* allow SQL92 rules */);

    /*
     * Transform A_const to columnref type in group by clause, So that repeated group column
     * will deleted in function transformGroupClause. If not to delete repeated column, for
     * group by rollup can have error result, because we need set null to non- group column.
     *
     * select a, b, b
     *	from t1
     *	group by rollup(1, 2), 3;
     *
     * To this example, column b should not be set to null, but if not to delete repeated column
     * b will be set to null and two b value is not equal.
     */
    if (include_groupingset((Node*)stmt->groupClause)) {
        transformGroupConstToColumn(pstate, (Node*)stmt->groupClause, qry->targetList);
    }

    qry->groupClause = transformGroupClause(pstate,
        stmt->groupClause,
        &qry->groupingSets,
        &qry->targetList,
        qry->sortClause,
        false /* allow SQL92 rules */);

    if (stmt->distinctClause == NIL) {
        qry->distinctClause = NIL;
        qry->hasDistinctOn = false;
    } else if (linitial(stmt->distinctClause) == NULL) {
        /* We had SELECT DISTINCT */
        qry->distinctClause = transformDistinctClause(pstate, &qry->targetList, qry->sortClause, false);
        qry->hasDistinctOn = false;
    } else {
        /* We had SELECT DISTINCT ON */
        qry->distinctClause =
            transformDistinctOnClause(pstate, stmt->distinctClause, &qry->targetList, qry->sortClause);
        qry->hasDistinctOn = true;
    }

    /* transform LIMIT */
    qry->limitOffset = transformLimitClause(pstate, stmt->limitOffset, "OFFSET");
    qry->limitCount = transformLimitClause(pstate, stmt->limitCount, "LIMIT");

    /* transform window clauses after we have seen all window functions */
    qry->windowClause = transformWindowDefinitions(pstate, pstate->p_windowdefs, &qry->targetList);

    /* resolve any still-unresolved output columns as being type text */
    if (pstate->p_resolve_unknowns) {
        resolveTargetListUnknowns(pstate, qry->targetList);
    }

    qry->rtable = pstate->p_rtable;
    qry->jointree = makeFromExpr(pstate->p_joinlist, qual);

    qry->hasSubLinks = pstate->p_hasSubLinks;
    qry->hasWindowFuncs = pstate->p_hasWindowFuncs;
    if (pstate->p_hasWindowFuncs) {
        parseCheckWindowFuncs(pstate, qry);
    }
    qry->hasAggs = pstate->p_hasAggs;

    foreach (l, stmt->lockingClause) {
        transformLockingClause(pstate, qry, (LockingClause*)lfirst(l), false);
    }

    qry->hintState = stmt->hintState;

    /*
     * If query is under one insert statement and include a foreign table,
     * then set top level parsestate p_is_foreignTbl_exist to true.
     */
    if (u_sess->attr.attr_sql.td_compatible_truncation && u_sess->attr.attr_sql.sql_compatibility == C_FORMAT &&
        pstate->p_is_in_insert && checkForeignTableExist(pstate->p_rtable))
        set_ancestor_ps_contain_foreigntbl(pstate);

    assign_query_collations(pstate, qry);

    /* this must be done after collations, for reliable comparison of exprs */
    if (pstate->p_hasAggs || qry->groupClause || qry->groupingSets || qry->havingQual) {
        demo_parseCheckAggregates(pstate, qry);
    }

    return qry;
}

static void demo_parseCheckAggregates(ParseState* pstate, Query* qry)
{
    List* gset_common = NIL;
    List* groupClauses = NIL;
    List* groupClauseCommonVars = NIL;
    bool have_non_var_grouping = false;
    List* func_grouped_rels = NIL;
    ListCell* l = NULL;
    bool hasJoinRTEs = false;
    bool hasSelfRefRTEs = false;
    PlannerInfo* root = NULL;
    Node* clause = NULL;

    /* This should only be called if we found aggregates or grouping */
    AssertEreport(pstate->p_hasAggs || qry->groupClause || qry->havingQual || qry->groupingSets,
        MOD_OPT,
        "only be called if we found aggregates or grouping");

    /*
     * If we have grouping sets, expand them and find the intersection of all
     * sets.
     */
    if (qry->groupingSets) {
        /*
         * The limit of 4096 is arbitrary and exists simply to avoid resource
         * issues from pathological constructs.
         */
        List* gsets = expand_grouping_sets(qry->groupingSets, 4096);

        if (gsets == NULL)
            ereport(ERROR,
                (errcode(ERRCODE_STATEMENT_TOO_COMPLEX),
                    errmsg("too many grouping sets present (max 4096)"),
                    parser_errposition(pstate,
                        qry->groupClause ? exprLocation((Node*)qry->groupClause)
                                         : exprLocation((Node*)qry->groupingSets))));

        /*
         * The intersection will often be empty, so help things along by
         * seeding the intersect with the smallest set.
         */
        gset_common = (List*)linitial(gsets);

        if (gset_common != NULL) {
            for_each_cell(l, lnext(list_head(gsets))) {
                gset_common = list_intersection_int(gset_common, (List*)lfirst(l));
                if (gset_common == NULL) {
                    break;
                }
            }
        }

        /*
         * If there was only one grouping set in the expansion, AND if the
         * groupClause is non-empty (meaning that the grouping set is not
         * empty either), then we can ditch the grouping set and pretend we
         * just had a normal GROUP BY.
         */
        if (list_length(gsets) == 1 && qry->groupClause) {
            qry->groupingSets = NIL;
        }
    }
    /*
     * Scan the range table to see if there are JOIN or self-reference CTE
     * entries.  We'll need this info below.
     */
    hasJoinRTEs = hasSelfRefRTEs = false;
    foreach (l, pstate->p_rtable) {
        RangeTblEntry* rte = (RangeTblEntry*)lfirst(l);

        if (rte->rtekind == RTE_JOIN) {
            hasJoinRTEs = true;
        } else if (rte->rtekind == RTE_CTE && rte->self_reference) {
            hasSelfRefRTEs = true;
        }
    }

    /*
     * Aggregates must never appear in WHERE or JOIN/ON clauses.
     *
     * (Note this check should appear first to deliver an appropriate error
     * message; otherwise we are likely to complain about some innocent
     * variable in the target list, which is outright misleading if the
     * problem is in WHERE.)
     */
    if (checkExprHasAggs(qry->jointree->quals)) {
        ereport(ERROR,
            (errcode(ERRCODE_GROUPING_ERROR),
                errmsg("aggregates not allowed in WHERE clause"),
                parser_errposition(pstate, locate_agg_of_level(qry->jointree->quals, 0))));
    }
    if (checkExprHasAggs((Node*)qry->jointree->fromlist)) { 
        ereport(ERROR,
            (errcode(ERRCODE_GROUPING_ERROR),
                errmsg("aggregates not allowed in JOIN conditions"),
                parser_errposition(pstate, locate_agg_of_level((Node*)qry->jointree->fromlist, 0))));
    }

    /*
     * No aggregates allowed in GROUP BY clauses, either.
     *
     * While we are at it, build a list of the acceptable GROUP BY expressions
     * for use by check_ungrouped_columns().
     */
    foreach (l, qry->groupClause) {
        SortGroupClause* grpcl = (SortGroupClause*)lfirst(l);
        TargetEntry* expr = NULL;

        expr = get_sortgroupclause_tle(grpcl, qry->targetList);
        if (expr == NULL) {
            continue; /* probably cannot happen */
        }
        if (checkExprHasAggs((Node*)expr->expr)) {
            ereport(ERROR,
                (errcode(ERRCODE_GROUPING_ERROR),
                    errmsg("aggregates not allowed in GROUP BY clause"),
                    parser_errposition(pstate, locate_agg_of_level((Node*)expr->expr, 0))));
        }
        groupClauses = lcons(expr, groupClauses);
    }

    /*
     * If there are join alias vars involved, we have to flatten them to the
     * underlying vars, so that aliased and unaliased vars will be correctly
     * taken as equal.	We can skip the expense of doing this if no rangetable
     * entries are RTE_JOIN kind. We use the planner's flatten_join_alias_vars
     * routine to do the flattening; it wants a PlannerInfo root node, which
     * fortunately can be mostly dummy.
     */
    if (hasJoinRTEs) {
        root = makeNode(PlannerInfo);
        root->parse = qry;
        root->planner_cxt = CurrentMemoryContext;
        root->hasJoinRTEs = true;

        groupClauses = (List*)flatten_join_alias_vars(root, (Node*)groupClauses);
    } else
        root = NULL; /* keep compiler quiet */

    /*
     * Detect whether any of the grouping expressions aren't simple Vars; if
     * they're all Vars then we don't have to work so hard in the recursive
     * scans.  (Note we have to flatten aliases before this.)
     *
     * Track Vars that are included in all grouping sets separately in
     * groupClauseCommonVars, since these are the only ones we can use to
     * check for functional dependencies.
     */
    have_non_var_grouping = false;
    foreach (l, groupClauses) {
        TargetEntry* tle = (TargetEntry*)lfirst(l);
        if (!IsA(tle->expr, Var)) {
            have_non_var_grouping = true;
        } else if (!qry->groupingSets || list_member_int(gset_common, tle->ressortgroupref)) {
            groupClauseCommonVars = lappend(groupClauseCommonVars, tle->expr);
        }
    }

    /*
     * Check the targetlist and HAVING clause for ungrouped variables.
     *
     * Note: because we check resjunk tlist elements as well as regular ones,
     * this will also find ungrouped variables that came from ORDER BY and
     * WINDOW clauses.	For that matter, it's also going to examine the
     * grouping expressions themselves --- but they'll all pass the test ...
     *
     * We also finalize GROUPING expressions, but for that we need to traverse
     * the original (unflattened) clause in order to modify nodes.
     */
    clause = (Node*)qry->targetList;
    finalize_grouping_exprs(clause, pstate, qry, groupClauses, root, have_non_var_grouping);
    if (hasJoinRTEs) {
        clause = flatten_join_alias_vars(root, clause);
    }
    check_ungrouped_columns(
        clause, pstate, qry, groupClauses, groupClauseCommonVars, have_non_var_grouping, &func_grouped_rels);

    clause = (Node*)qry->havingQual;
    finalize_grouping_exprs(clause, pstate, qry, groupClauses, root, have_non_var_grouping);
    if (hasJoinRTEs) {
        clause = flatten_join_alias_vars(root, clause);
    }
    check_ungrouped_columns(
        clause, pstate, qry, groupClauses, groupClauseCommonVars, have_non_var_grouping, &func_grouped_rels);

    /*
     * Per spec, aggregates can't appear in a recursive term.
     */
    if (pstate->p_hasAggs && hasSelfRefRTEs) {
        ereport(ERROR,
            (errcode(ERRCODE_INVALID_RECURSION),
                errmsg("aggregate functions not allowed in a recursive query's recursive term"),
                parser_errposition(pstate, locate_agg_of_level((Node*)qry, 0))));
    }
}
