/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include "postgres.h"

#include "catalog/pg_type.h"
#include "nodes/makefuncs.h"
#include "nodes/nodeFuncs.h"
#include "nodes/nodes.h"
#include "nodes/parsenodes.h"
#include "nodes/pg_list.h"
#include "nodes/primnodes.h"
#include "parser/analyze.h"
#include "parser/parse_coerce.h"
#include "parser/parse_collate.h"
#include "parser/parse_node.h"
#include "parser/parse_relation.h"
#include "parser/parse_target.h"
#include "parser/parsetree.h"
#include "utils/builtins.h"

#include "catalog/ag_catalog.h"
#include "catalog/ag_graph.h"
#include "nodes/ag_nodes.h"
#include "parser/cypher_analyze.h"
#include "parser/cypher_clause.h"
#include "parser/cypher_parse_node.h"
#include "parser/cypher_parser.h"
#include "utils/age_session_info.h"
#include "utils/ag_func.h"
#include "utils/agtype.h"

static THR_LOCAL Node *extra_node = NULL;

static void build_explain_query(Query *query, Node *explain_node);

static THR_LOCAL post_parse_analyze_hook_type prev_post_parse_analyze_hook;

#define GRAPH_FUNCTION_MIN_ARGUMENTS 2

typedef struct convert_cypher_context {
    ParseState *pstate;
    bool in_ctas;
} convert_cypher_context;

static void post_parse_analyze(ParseState *pstate, Query *query);
static bool convert_cypher_walker(Node *node,
                                  convert_cypher_context *context);
static bool is_rte_cypher(RangeTblEntry *rte);
static bool is_func_cypher(Node *expr);
static void convert_cypher_to_subquery(RangeTblEntry *rte, ParseState *pstate,
                                       bool in_ctas);
static Name expr_get_const_name(Node *expr);
static const char *expr_get_const_cstring(Node *expr, const char *source_str);
static int get_query_location(const int location, const char *source_str);
static Query *analyze_cypher(List *stmt, ParseState *parent_pstate,
                             const char *query_str, int query_loc,
                             char *graph_name, Oid graph_oid, Param *params);
static Query *analyze_cypher_and_coerce(List *stmt, RangeTblEntry *rte,
                                        ParseState *parent_pstate,
                                        const char *query_str, int query_loc,
                                        char *graph_name, Oid graph_oid,
                                        Param *params, bool in_ctas);

void post_parse_analyze_init(void)
{
    prev_post_parse_analyze_hook = post_parse_analyze_hook;
    post_parse_analyze_hook = post_parse_analyze;
}

void post_parse_analyze_fini(void)
{
    post_parse_analyze_hook = prev_post_parse_analyze_hook;
}

static void post_parse_analyze(ParseState *pstate, Query *query)
{
    convert_cypher_context context;

    if (prev_post_parse_analyze_hook)
        prev_post_parse_analyze_hook(pstate, query);

    if (!is_age_extension_exists()) {
        return;
    }

    extra_node = NULL;

    context.pstate = pstate;
    context.in_ctas = false;
    convert_cypher_walker((Node *)query, &context);

    if (extra_node != NULL) {
        if (nodeTag(extra_node) == T_ExplainStmt)
            build_explain_query(query, extra_node);

        pfree(extra_node);
        extra_node = NULL;
    }
}

// find cypher() calls in FROM clauses and convert them to SELECT subqueries
static bool convert_cypher_walker(Node *node,
                                  convert_cypher_context *context)
{
    ParseState *pstate = context->pstate;

    if (!node) {
        return false;
    }

    if (IsA(node, RangeTblEntry))
    {
        RangeTblEntry *rte = (RangeTblEntry *)node;

        switch (rte->rtekind)
        {
        case RTE_SUBQUERY:
            // traverse other RTE_SUBQUERYs
            return convert_cypher_walker((Node *)rte->subquery, context);
        case RTE_FUNCTION:
            if (is_rte_cypher(rte))
                convert_cypher_to_subquery(rte, pstate, context->in_ctas);
            return false;
        default:
            return false;
        }
    }

    /*
     * This handles a cypher() call with other function calls in a ROWS FROM
     * expression. We can let the FuncExpr case below handle it but do this
     * here to throw a better error message.
     */

    /*
     * This handles cypher() calls in expressions. Those in RTE_FUNCTIONs are
     * handled by either convert_cypher_to_subquery() or the RangeTblFunction
     * case above.
     */
    if (IsA(node, FuncExpr))
    {
        FuncExpr *funcexpr = (FuncExpr *)node;

        if (is_func_cypher((Node *)funcexpr))
        {
            ereport(ERROR,
                    (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                     errmsg("cypher(...) in expressions is not supported"),
                     errhint("Use subquery instead if possible."),
                     parser_errposition(pstate, exprLocation(node))));
        }

        return expression_tree_walker((Node *)funcexpr->args,
                                      (bool (*)())convert_cypher_walker,
                                      context);
    }

    if (IsA(node, Query))
    {
        int flags;
        bool result = false;
        Query *query = (Query *)node;
        convert_cypher_context child_context = *context;

        /*
         * Utility queries are not traversed by query_tree_walker().  Unwrap
         * the utility commands whose inner query was already analyzed by
         * transformExplainStmt() / transformCreateTableAsStmt():
         *
         * EXPLAIN [ANALYZE] SELECT ... FROM cypher(...)
         *     PostgreSQL 14+ runs post_parse_analyze_hook again on the inner
         *     query from ExplainQuery(); openGauss does not, so the hook only
         *     ever sees the utility wrapper.
         *
         * CREATE TABLE AS SELECT ... FROM cypher(...)
         *     The SELECT must be converted before openGauss deparses it into
         *     the follow-up INSERT statement.
         */
        if (query->utilityStmt != NULL &&
            IsA(query->utilityStmt, ExplainStmt))
        {
            ExplainStmt *estmt = (ExplainStmt *)query->utilityStmt;

            if (estmt->query != NULL && IsA(estmt->query, Query)) {
                query = (Query *)estmt->query;
            }
        }

        if (query->utilityStmt != NULL &&
            IsA(query->utilityStmt, CreateTableAsStmt))
        {
            CreateTableAsStmt *ctas =
                (CreateTableAsStmt *)query->utilityStmt;

            if (IsA(ctas->query, Query)) {
                query = (Query *)ctas->query;
                child_context.in_ctas = true;
            }
        }

        /*
         * QTW_EXAMINE_RTES
         *     We convert RTE_FUNCTION (cypher()) to RTE_SUBQUERY (SELECT)
         *     in-place.
         *
         * QTW_IGNORE_RT_SUBQUERIES
         *     After the conversion, we don't need to traverse the resulting
         *     RTE_SUBQUERY. However, we need to traverse other RTE_SUBQUERYs.
         *     This is done manually by the RTE_SUBQUERY case above.
         *
         * QTW_IGNORE_JOINALIASES
         *     We are not interested in this.
         */
        flags = QTW_EXAMINE_RTES | QTW_IGNORE_RT_SUBQUERIES |
                QTW_IGNORE_JOINALIASES;

        /* recurse on query */
        result = query_tree_walker(query, (bool (*)())convert_cypher_walker,
                                   &child_context, flags);

        return result;
    }

    return expression_tree_walker(node, (bool (*)())convert_cypher_walker,
                                  context);
}

static void build_explain_query(Query *query, Node *explain_node)
{
    ExplainStmt *estmt = NULL;
    Query *query_copy = NULL;
    Query *query_node = NULL;

    /*
     * Create a copy of the query node. This is purposely a shallow copy
     * because we are only moving the contents to another pointer.
     */
    query_copy = (Query *)palloc(sizeof(Query));
    memcpy(query_copy, query, sizeof(Query));

    /* build our Explain node and store the query node copy in it */
    estmt = makeNode(ExplainStmt);
    estmt->query = (Node *)query_copy;
    estmt->options = ((ExplainStmt *)explain_node)->options;

    /* build our replacement query node */
    query_node = makeNode(Query);
    query_node->commandType = CMD_UTILITY;
    query_node->utilityStmt = (Node *)estmt;
    query_node->canSetTag = true;

    /* now replace the top query node with our replacement query node */
    memcpy(query, query_node, sizeof(Query));

    /* ExplainStmt options are now owned by the replacement query. */
    ((ExplainStmt *)explain_node)->options = NULL;

    /* we need to free query_node as it is no longer needed */
    pfree(query_node);
}

static bool is_rte_cypher(RangeTblEntry *rte)
{
    if (rte->funcexpr == NULL)
        return false;

    /*
     * A plain function call or a ROWS FROM expression with one function call
     * reaches here. At this point, it is impossible to distinguish between the
     * two. However, it doesn't matter because they are identical in terms of
     * their meaning.
     */

    return is_func_cypher(rte->funcexpr);
}

/*
 * Return true if the qualified name of the given function is
 * <"ag_catalog"."cypher">. Otherwise, return false.
 */
static bool is_func_cypher(Node *expr)
{
    FuncExpr *funcexpr;

    /*
     * openGauss stores any FROM expression in RangeTblEntry::funcexpr,
     * including Const, CoerceViaIO, OpExpr, Var, and BoolExpr nodes.  Only a
     * real FuncExpr has funcid; treating another node as FuncExpr makes
     * constructs such as SELECT * FROM agtype(NULL) dereference garbage.
     */
    if (!IsA(expr, FuncExpr))
        return false;

    funcexpr = (FuncExpr *)expr;
    return is_oid_ag_func(funcexpr->funcid, "cypher");
}

// convert cypher() call to SELECT subquery in-place
static void convert_cypher_to_subquery(RangeTblEntry *rte, ParseState *pstate,
                                       bool in_ctas)
{
    FuncExpr *funcexpr = (FuncExpr *)rte->funcexpr;
    Node *arg1 = NULL;
    Node *arg2 = NULL;
    Node *arg3 = NULL;
    Name graph_name = NULL;
    char *graph_name_str = NULL;
    Oid graph_oid;
    const char *query_str = NULL;
    int query_loc;
    Param *params;
    errpos_ecb_state ecb_state;
    List *stmt;
    Query *query;

    if (funcexpr->args == NULL ||
        list_length(funcexpr->args) < GRAPH_FUNCTION_MIN_ARGUMENTS)
    {
        ereport(ERROR, (errcode(ERRCODE_SYNTAX_ERROR),
                        errmsg("cypher function requires a minimum of 2 arguments"),
                        parser_errposition(pstate, -1)));
    }

    arg1 = (Node*)linitial(funcexpr->args);
    arg2 = (Node*)lsecond(funcexpr->args);
    Assert(exprType(arg1) == NAMEOID);
    Assert(exprType(arg2) == CSTRINGOID);

    graph_name = expr_get_const_name(arg1);
    query_str = expr_get_const_cstring(arg2, pstate->p_sourcetext);

    if (is_session_info_prepared())
    {
        if (graph_name != NULL || query_str != NULL)
        {
            Node *arg = graph_name == NULL ? arg1 : arg2;

            reset_session_info();
            ereport(ERROR,
                    (errcode(ERRCODE_SYNTAX_ERROR),
                     errmsg("session info requires cypher(NULL, NULL) to be passed"),
                     parser_errposition(pstate, exprLocation(arg))));
        }

        graph_name_str = get_session_info_graph_name();
        query_str = get_session_info_cypher_statement();
        if (graph_name_str == NULL || query_str == NULL)
        {
            reset_session_info();
            ereport(ERROR,
                    (errcode(ERRCODE_SYNTAX_ERROR),
                     errmsg("both session info parameters need to be non-NULL"),
                     parser_errposition(pstate, -1)));
        }
    }
    else
    {
        if (!graph_name)
        {
            ereport(ERROR, (errcode(ERRCODE_SYNTAX_ERROR),
                            errmsg("a name constant is expected"),
                            parser_errposition(pstate, exprLocation(arg1))));
            return; /* suppress the static check warnings */
        }

        graph_name_str = NameStr(*graph_name);

        if (!query_str)
        {
            ereport(ERROR, (errcode(ERRCODE_SYNTAX_ERROR),
                            errmsg("a dollar-quoted string constant is expected"),
                            parser_errposition(pstate, exprLocation(arg2))));
        }
    }

    if (is_session_info_prepared())
    {
        reset_session_info();
        query_loc = 0;
    }
    else
    {
        query_loc = get_query_location(((Const *)arg2)->location,
                                       pstate->p_sourcetext);
    }

    graph_oid = get_graph_oid(graph_name_str);
    if (!OidIsValid(graph_oid))
    {
        ereport(ERROR,
                (errcode(ERRCODE_UNDEFINED_SCHEMA),
                 errmsg("graph \"%s\" does not exist", graph_name_str),
                 parser_errposition(pstate, exprLocation(arg1))));
    }

    /*
     * Check to see if the cypher function had any parameters passed to it,
     * if so make sure Postgres parsed the second argument to a Param node.
     */
    if (list_length(funcexpr->args) == 3)
    {
        arg3 = (Node*)lthird(funcexpr->args);
        if (!IsA(arg3, Param))
        {
            ereport(ERROR,
                    (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                     errmsg("third argument of cypher function must be a parameter"),
                     parser_errposition(pstate, exprLocation(arg3))));
        }

        params = (Param *)arg3;
    }
    else
    {
        params = NULL;
    }

    /*
     * install error context callback to adjust an error position for
     * parse_cypher() since locations that parse_cypher() stores are 0 based
     */
    setup_errpos_ecb(&ecb_state, pstate, query_loc);

    stmt = parse_cypher(query_str);

    /*
     * Extract any extra node passed up and assign it to the global variable
     * 'extra_node' - if it wasn't already set. It will be at the end of the
     * stmt list and needs to be removed for normal processing, regardless.
     * It is done this way to allow utility commands to be processed against the
     * AGE query tree. Currently, only EXPLAIN is passed here. But, it need not
     * just be EXPLAIN - so long as it is carefully documented and carefully
     * done.
     */
    if (extra_node == NULL)
    {
        extra_node = (Node*)llast(stmt);
        list_delete_ptr(stmt, extra_node);
    }
    else
    {
        Node *temp = (Node*)llast(stmt);

        ereport(WARNING,
                (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                 errmsg("too many extra_nodes passed from parser")));

        list_delete_ptr(stmt, temp);
    }

    cancel_errpos_ecb(&ecb_state);

    Assert(pstate->p_expr_kind == EXPR_KIND_NONE);
    pstate->p_expr_kind = EXPR_KIND_FROM_SUBSELECT;
    // transformRangeFunction() always sets p_lateral_active to true.
    // FYI, rte is RTE_FUNCTION and is being converted to RTE_SUBQUERY here.
    pstate->p_lateral_active = true;

    /*
     * Cypher queries that end with CREATE clause do not need to have the
     * coercion logic applied to them because we are forcing the column
     * definition list to be a particular way in this case.
     */
    if (is_ag_node(llast(stmt), cypher_create) || is_ag_node(llast(stmt), cypher_set) ||
        is_ag_node(llast(stmt), cypher_delete) || is_ag_node(llast(stmt), cypher_merge))
    {
        // column definition list must be ... AS relname(colname agtype) ...
        if (!(rte->funccoltypes->length == 1 &&
              linitial_oid(rte->funccoltypes) == AGTYPEOID))
        {
            ereport(ERROR,
                    (errcode(ERRCODE_DATATYPE_MISMATCH),
                     errmsg("column definition list for CREATE clause must contain a single agtype attribute"),
                     errhint("... cypher($$ ... CREATE ... $$) AS t(c agtype) ..."),
                     parser_errposition(pstate, exprLocation(rte->funcexpr))));
        }

        query = analyze_cypher(stmt, pstate, query_str, query_loc,
                               graph_name_str, graph_oid, params);
    }
    else
    {
        query = analyze_cypher_and_coerce(stmt, rte, pstate, query_str,
                                          query_loc, graph_name_str,
                                          graph_oid, params, in_ctas);
    }

    pstate->p_lateral_active = false;
    pstate->p_expr_kind = EXPR_KIND_NONE;

    // rte->functions and rte->funcordinality are kept for debugging.
    // rte->alias, rte->eref, and rte->lateral need to be the same.
    // rte->inh is always false for both RTE_FUNCTION and RTE_SUBQUERY.
    // rte->inFromCl is always true for RTE_FUNCTION.
    rte->rtekind = RTE_SUBQUERY;
    rte->subquery = query;
}

static Name expr_get_const_name(Node *expr)
{
    Const *con;

    if (!IsA(expr, Const))
        return NULL;

    con = (Const *)expr;
    if (con->constisnull)
        return NULL;

    return DatumGetName(con->constvalue);
}

static const char *expr_get_const_cstring(Node *expr, const char *source_str)
{
    Const *con;
    const char *p;

    if (!IsA(expr, Const))
        return NULL;

    con = (Const *)expr;
    if (con->constisnull)
        return NULL;

    Assert(con->location > -1);
    p = source_str + con->location;
    if (*p != '$')
        return NULL;

    return DatumGetCString(con->constvalue);
}

static int get_query_location(const int location, const char *source_str)
{
    const char *p;

    Assert(location > -1);

    p = source_str + location;
    Assert(*p == '$');

    return strchr(p + 1, '$') - source_str + 1;
}

static Query *analyze_cypher(List *stmt, ParseState *parent_pstate,
                             const char *query_str, int query_loc,
                             char *graph_name, Oid graph_oid, Param *params)
{
    cypher_clause *clause;
    ListCell *lc;
    cypher_parsestate parent_cpstate;
    cypher_parsestate *cpstate;
    ParseState *pstate;
    errpos_ecb_state ecb_state;
    Query *query;

    /*
     * Since the first clause in stmt is the innermost subquery, the order of
     * the clauses is inverted.
     */
    clause = NULL;
    foreach (lc, stmt)
    {
        cypher_clause *next;

        next = (cypher_clause*)palloc(sizeof(*next));
        next->next = NULL;
        next->self = (Node*)lfirst(lc);
        next->prev = clause;

        if (clause != NULL)
            clause->next = next;
        clause = next;
    }

    /*
     * convert ParseState into cypher_parsestate temporarily to pass it to
     * make_cypher_parsestate()
     */
    MemSet(&parent_cpstate, 0, sizeof(parent_cpstate));
    parent_cpstate.pstate = *parent_pstate;

    cpstate = make_cypher_parsestate(&parent_cpstate);

    pstate = (ParseState *)cpstate;

    /* we don't want functions that go up the pstate parent chain to access the
     * original SQL query pstate.
     */
    pstate->parentParseState = NULL;
    /*
     * override p_sourcetext with query_str to make parser_errposition() work
     * correctly with errpos_ecb()
     */
    pstate->p_sourcetext = query_str;

    cpstate->graph_name = graph_name;
    cpstate->graph_oid = graph_oid;
    cpstate->params = params;
    cpstate->default_alias_num = 0;
    cpstate->entities = NIL;
    cpstate->subquery_where_flag = false;
    /*
     * install error context callback to adjust an error position since
     * locations in stmt are 0 based
     */
    setup_errpos_ecb(&ecb_state, parent_pstate, query_loc);

    query = transform_cypher_clause(cpstate, clause);

    cancel_errpos_ecb(&ecb_state);

    free_cypher_parsestate(cpstate);

    return query;
}

/*
 * Since some target entries of subquery may be referenced for sorting (ORDER
 * BY), we cannot apply the coercion directly to the expressions of the target
 * entries. Therefore, we do the coercion by doing SELECT over subquery.
 */
static Query *analyze_cypher_and_coerce(List *stmt, RangeTblEntry *rte,
                                        ParseState *parent_pstate,
                                        const char *query_str, int query_loc,
                                        char *graph_name, Oid graph_oid,
                                        Param *params, bool in_ctas)
{
    ParseState *pstate;
    Query *query;
    const bool lateral = false;
    Query *subquery;
    RangeTblEntry *newRte;
    int rtindex;
    ListCell *lt;
    ListCell *lc1;
    ListCell *lc2;
    ListCell *lc3;

    int attr_count = 0;

    pstate = make_parsestate(parent_pstate);

    query = makeNode(Query);
    query->commandType = CMD_SELECT;

    /*
     * Below is similar to transform_prev_cypher_clause().
     */

    Assert(pstate->p_expr_kind == EXPR_KIND_NONE);
    pstate->p_expr_kind = EXPR_KIND_FROM_SUBSELECT;
    pstate->p_lateral_active = lateral;

    subquery = analyze_cypher(stmt, pstate, query_str, query_loc, graph_name,
                              graph_oid, (Param *)params);

    pstate->p_lateral_active = false;
    pstate->p_expr_kind = EXPR_KIND_NONE;

    // ALIAS Syntax makes `RESJUNK`. So, It must be skipping.
    foreach(lt, subquery->targetList)
    {
        TargetEntry *te = (TargetEntry*)lfirst(lt);
        if (!te->resjunk)
        {
            attr_count++;
        }
    }

    // check the number of attributes first
    if (attr_count != list_length(rte->eref->colnames))
    {
        ereport(ERROR,
                (errcode(ERRCODE_DATATYPE_MISMATCH),
                 errmsg("return row and column definition list do not match"),
                 parser_errposition(pstate, exprLocation(rte->funcexpr))));
    }

    /*
     * Keep the transformed subquery's visible names aligned with the SQL
     * column definition list.  openGauss deparses CTAS into a second INSERT
     * statement and, unlike PostgreSQL, does not emit an implicit subquery
     * column alias list for this RTE.  Stale names such as "?column?" would
     * therefore make the generated INSERT reference a non-existent column.
     */
    if (in_ctas)
    {
        lc1 = list_head(rte->eref->colnames);
        foreach (lt, subquery->targetList)
        {
            TargetEntry *te = (TargetEntry *)lfirst(lt);

            if (!te->resjunk)
            {
                te->resname = pstrdup(strVal(lfirst(lc1)));
                lc1 = lnext(lc1);
            }
        }
    }

    newRte = addRangeTableEntryForSubquery(pstate, subquery, makeAlias("_", NIL),
        lateral, true);
    rtindex = list_length(pstate->p_rtable);
    Assert(rtindex == 1); // rte is the only RangeTblEntry in pstate
    if (rtindex != 1)
    {
        ereport(ERROR,
                (errcode(ERRCODE_DATATYPE_MISMATCH),
                 errmsg("invalid value for rtindex")));
    }
    addRTEtoQuery(pstate, newRte, true, true, true);

    query->targetList = expandRelAttrs(pstate, newRte, rtindex, 0, -1);

    markTargetListOrigins(pstate, query->targetList);

    // do the type coercion for each target entry
    lc1 = list_head(rte->eref->colnames);
    lc2 = list_head(rte->funccoltypes);
    lc3 = list_head(rte->funccoltypmods);
    foreach (lt, query->targetList)
    {
        TargetEntry *te = (TargetEntry*)lfirst(lt);
        Node *expr = (Node *)te->expr;
        Oid current_type;
        Oid target_type;

        Assert(!te->resjunk);

        current_type = exprType(expr);
        target_type = lfirst_oid(lc2);
        if (current_type != target_type)
        {
            int32 target_typmod = lfirst_int(lc3);
            Node *new_expr;

            /*
             * The coercion context of this coercion is COERCION_EXPLICIT
             * because the target type is explicitly metioned in the column
             * definition list and we need to do this by looking up all
             * possible coercion.
             */
            new_expr = coerce_to_target_type(pstate, expr, current_type,
                                             target_type, target_typmod,
                                             COERCION_EXPLICIT,
                                             COERCE_EXPLICIT_CAST,
                                             NULL, NULL, -1);
            if (!new_expr)
            {
                char *colname = strVal(lfirst(lc1));

                ereport(ERROR,
                        (errcode(ERRCODE_CANNOT_COERCE),
                         errmsg("cannot cast type %s to %s for column \"%s\"",
                                format_type_be(current_type),
                                format_type_be(target_type), colname),
                         parser_errposition(pstate,
                                            exprLocation(rte->funcexpr))));
            }

            te->expr = (Expr *)new_expr;
        }

        lc1 = lnext(lc1);
        lc2 = lnext(lc2);
        lc3 = lnext(lc3);
    }

    query->rtable = pstate->p_rtable;
    query->jointree = makeFromExpr(pstate->p_joinlist, NULL);

    assign_query_collations(pstate, query);

    free_parsestate(pstate);

    return query;
}
