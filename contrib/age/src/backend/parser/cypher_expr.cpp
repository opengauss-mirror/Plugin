/*
 * For PostgreSQL Database Management System:
 * (formerly known as Postgres, then as Postgres95)
 *
 * Portions Copyright (c) 1996-2010, The PostgreSQL Global Development Group
 *
 * Portions Copyright (c) 1994, The Regents of the University of California
 *
 * Permission to use, copy, modify, and distribute this software and its documentation for any purpose,
 * without fee, and without a written agreement is hereby granted, provided that the above copyright notice
 * and this paragraph and the following two paragraphs appear in all copies.
 *
 * IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY FOR DIRECT,
 * INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING LOST PROFITS,
 * ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY
 * OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING,
 * BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 * THE SOFTWARE PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF CALIFORNIA
 * HAS NO OBLIGATIONS TO PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 */

#include "postgres.h"

#include "catalog/dependency.h"
#include "catalog/namespace.h"
#include "catalog/pg_proc.h"
#include "catalog/pg_type.h"
#include "commands/extension.h"
#include "miscadmin.h"
#include "nodes/makefuncs.h"
#include "nodes/nodeFuncs.h"
#include "nodes/nodes.h"
#include "nodes/parsenodes.h"
#include "nodes/value.h"
#include "optimizer/tlist.h"
#include "parser/parse_coerce.h"
#include "parser/parse_collate.h"
#include "parser/parse_expr.h"
#include "parser/parse_func.h"
#include "parser/cypher_clause.h"
#include "parser/parse_node.h"
#include "parser/parse_oper.h"
#include "parser/parse_relation.h"
#include "parser/parse_type.h"
#include "utils/builtins.h"
#include "utils/catcache.h"
#include "plugin_utils/int8.h"
#include "utils/lsyscache.h"
#include "utils/syscache.h"

#include "commands/label_commands.h"
#include "nodes/cypher_nodes.h"
#include "parser/cypher_expr.h"
#include "parser/cypher_clause.h"
#include "parser/cypher_parse_node.h"
#include "utils/ag_func.h"
#include "utils/agtype.h"

/* names of typecast functions */
#define FUNC_AGTYPE_TYPECAST_EDGE "agtype_typecast_edge"
#define FUNC_AGTYPE_TYPECAST_PATH "agtype_typecast_path"
#define FUNC_AGTYPE_TYPECAST_VERTEX "agtype_typecast_vertex"
#define FUNC_AGTYPE_TYPECAST_NUMERIC "agtype_typecast_numeric"
#define FUNC_AGTYPE_TYPECAST_FLOAT "agtype_typecast_float"
#define FUNC_AGTYPE_TYPECAST_INT "agtype_typecast_int"
#define FUNC_AGTYPE_TYPECAST_BOOL "agtype_typecast_bool"
#define FUNC_AGTYPE_TYPECAST_PG_FLOAT8 "agtype_to_float8"
#define FUNC_AGTYPE_TYPECAST_PG_BIGINT "agtype_to_int8"

#define AGTYPE_IN_OPERATOR_ARGUMENT_COUNT 2
#define AGTYPE_LIST_CHUNK_SIZE 100
#define AGE_FUNCTION_NAME_PREFIX_LENGTH (sizeof("age_") - 1)
#define ACCESSOR_NODE_FUNCTION_MIN_ARGUMENTS 2

static Node *transform_cypher_expr_recurse(cypher_parsestate *cpstate,
                                           Node *expr);
static Node *transform_A_Const(cypher_parsestate *cpstate, A_Const *ac);
static Node *transform_ColumnRef(cypher_parsestate *cpstate, ColumnRef *cref);
static Node *transform_A_Indirection(cypher_parsestate *cpstate,
                                     A_Indirection *a_ind);
static Node *transform_AEXPR_OP(cypher_parsestate *cpstate, A_Expr *a);
static Node *transform_cypher_comparison_aexpr_OP(cypher_parsestate *cpstate,
                                                  cypher_comparison_aexpr *a);
static Node *transform_BoolExpr(cypher_parsestate *cpstate, BoolExpr *expr);
static Node *transform_cypher_comparison_boolexpr(cypher_parsestate *cpstate,
                                                  cypher_comparison_boolexpr *b);
static Node *coerce_cypher_expr_to_boolean(ParseState *pstate, Node *expr,
                                           const char *construct_name);
static Node *transform_cypher_bool_const(cypher_parsestate *cpstate,
                                         cypher_bool_const *bc);
static Node *transform_cypher_integer_const(cypher_parsestate *cpstate,
                                            cypher_integer_const *ic);
static Node *transform_AEXPR_IN(cypher_parsestate *cpstate, A_Expr *a);
static Node *transform_cypher_param(cypher_parsestate *cpstate,
                                    cypher_param *cp);
static Node *transform_cypher_map(cypher_parsestate *cpstate, cypher_map *cm);
static Node *transform_cypher_map_projection(cypher_parsestate *cpstate,
                                             cypher_map_projection *cmp);
static Node *transform_cypher_list(cypher_parsestate *cpstate,
                                   cypher_list *cl);
static Node *transform_cypher_string_match(cypher_parsestate *cpstate,
                                           cypher_string_match *csm_node);
static Node *transform_cypher_typecast(cypher_parsestate *cpstate,
                                       cypher_typecast *ctypecast);
static Node *transform_CaseExpr(cypher_parsestate *cpstate,
                                    CaseExpr *cexpr);
static Node *transform_CoalesceExpr(cypher_parsestate *cpstate,
                                    CoalesceExpr *cexpr);
static Node *transform_SubLink(cypher_parsestate *cpstate, SubLink *sublink);
static Node *transform_FuncCall(cypher_parsestate *cpstate, FuncCall *fn);
static Node *transform_cypher_list_comprehension_expr(cypher_parsestate *cpstate,
    cypher_list_comprehension *list_comp);
static Node *transform_cypher_reduce_expr(cypher_parsestate *cpstate,
                                          cypher_reduce *reduce);

static Node *transform_cypher_predicate_function_expr(cypher_parsestate *cpstate,
    cypher_predicate_function *pred_func);
static Node *transform_cypher_reduce_expr(cypher_parsestate *cpstate,
                                          cypher_reduce *reduce);
static Node *coerce_expr_flexible(ParseState *pstate, Node *expr,
                                  Oid source_oid, Oid target_oid,
                                  int32 t_typmod, bool error_out);
static Node *transform_WholeRowRef(ParseState *pstate, RangeTblEntry *rte,
                                   int location);
static ArrayExpr *make_agtype_array_expr(List *args);
static RangeTblEntry *find_current_rte(ParseState *pstate, char *relname);
static Node *transform_column_ref_for_indirection(cypher_parsestate *cpstate,
                                                  ColumnRef *cr);
static Node *transform_external_ext_FuncCall(cypher_parsestate *cpstate,
                                             FuncCall *fn, List *targs,
                                             Form_pg_proc procform);
static bool is_external_vector_typecast(const char *typecast);
static bool has_external_vector_arg(List *args);
static bool is_external_vector_type_oid(Oid type_oid);
static Node *transform_vector_list_typecast(cypher_parsestate *cpstate,
                                            cypher_typecast *ctypecast,
                                            Oid target_oid,
                                            int32 target_typmod);
static Node *transform_vector_string_typecast(cypher_typecast *ctypecast,
                                              Oid target_oid,
                                              int32 target_typmod);
static List *cast_agtype_args_to_target_type(cypher_parsestate *cpstate,
                                             Form_pg_proc procform,
                                             List *fargs,
                                             Oid *target_types);
static Node *wrap_text_output_to_agtype(cypher_parsestate *cpstate,
                                        FuncExpr *fexpr);
static Form_pg_proc get_procform(FuncCall *fn, List *targs,
                                 bool err_not_found);
static char *construct_age_function_name(char *funcname);
static bool function_exists(char *funcname, char *extension);
static Node *coerce_expr_flexible(ParseState *pstate, Node *expr,
                                  Oid source_oid, Oid target_oid,
                                  int32 t_typmod, bool error_out);
static Oid get_entity_record_type(Node *node);
/* transform a cypher expression */
Node *transform_cypher_expr(cypher_parsestate *cpstate, Node *expr,
                            ParseExprKind expr_kind)
{
    ParseState *pstate = (ParseState *)cpstate;
    ParseExprKind old_expr_kind;
    Node *result;

    // save and restore identity of expression type we're parsing
    Assert(expr_kind != EXPR_KIND_NONE);
    old_expr_kind = pstate->p_expr_kind;
    pstate->p_expr_kind = expr_kind;

    result = transform_cypher_expr_recurse(cpstate, expr);

    pstate->p_expr_kind = old_expr_kind;

    return result;
}

static Node *transform_cypher_expr_recurse(cypher_parsestate *cpstate,
                                           Node *expr)
{
    if (!expr)
        return NULL;

    // guard against stack overflow due to overly complex expressions
    check_stack_depth();

    switch (nodeTag(expr))
    {
    case T_A_Const:
        return transform_A_Const(cpstate, (A_Const *)expr);
    case T_ColumnRef:
        return transform_ColumnRef(cpstate, (ColumnRef *)expr);
    case T_A_Indirection:
        return transform_A_Indirection(cpstate, (A_Indirection *)expr);
    case T_A_Expr:
    {
        A_Expr *a = (A_Expr *)expr;

        switch (a->kind)
        {
        case AEXPR_OP:
            return transform_AEXPR_OP(cpstate, a);
        case AEXPR_IN:
            return transform_AEXPR_IN(cpstate, a);
        default:
            ereport(ERROR, (errmsg_internal("unrecognized A_Expr kind: %d",
                                            a->kind)));
        }
    }
    case T_BoolExpr:
        return transform_BoolExpr(cpstate, (BoolExpr *)expr);
    case T_NullTest:
    {
        NullTest *null_test = (NullTest *)expr;
        NullTest *transformed_null_test = makeNode(NullTest);

        transformed_null_test->arg =
            (Expr *)transform_cypher_expr_recurse(cpstate,
                                                  (Node *)null_test->arg);
        transformed_null_test->nulltesttype = null_test->nulltesttype;
        transformed_null_test->argisrow =
            type_is_rowtype(exprType((Node *)transformed_null_test->arg));

        return (Node *)transformed_null_test;
    }
    case T_CaseExpr:
        return transform_CaseExpr(cpstate, (CaseExpr *) expr);
    case T_CaseTestExpr:
        return expr;
    case T_CoalesceExpr:
        return transform_CoalesceExpr(cpstate, (CoalesceExpr *) expr);
    case T_EXTENSIBLE_NODE:
        if (is_ag_node(expr, cypher_bool_const))
            return transform_cypher_bool_const(cpstate,
                                               (cypher_bool_const *)expr);
        if (is_ag_node(expr, cypher_integer_const))
            return transform_cypher_integer_const(cpstate,
                                                  (cypher_integer_const *)expr);
        if (is_ag_node(expr, cypher_param))
            return transform_cypher_param(cpstate, (cypher_param *)expr);
        if (is_ag_node(expr, cypher_map))
            return transform_cypher_map(cpstate, (cypher_map *)expr);
        if (is_ag_node(expr, cypher_map_projection))
            return transform_cypher_map_projection(cpstate,
                                                   (cypher_map_projection *)expr);
        if (is_ag_node(expr, cypher_list))
            return transform_cypher_list(cpstate, (cypher_list *)expr);
        if (is_ag_node(expr, cypher_list_comprehension))
            return transform_cypher_list_comprehension_expr(cpstate,
                                                            (cypher_list_comprehension *)expr);
        if (is_ag_node(expr, cypher_predicate_function))
            return transform_cypher_predicate_function_expr(cpstate,
                                                            (cypher_predicate_function *)expr);
        if (is_ag_node(expr, cypher_reduce))
            return transform_cypher_reduce_expr(cpstate,
                                                (cypher_reduce *)expr);
        if (is_ag_node(expr, cypher_string_match))
            return transform_cypher_string_match(cpstate,
                                                 (cypher_string_match *)expr);
        if (is_ag_node(expr, cypher_typecast))
            return transform_cypher_typecast(cpstate,
                                             (cypher_typecast *)expr);
        if (is_ag_node(expr, cypher_comparison_aexpr))
            return transform_cypher_comparison_aexpr_OP(cpstate,
                                                        (cypher_comparison_aexpr *)expr);
        if (is_ag_node(expr, cypher_comparison_boolexpr))
            return transform_cypher_comparison_boolexpr(cpstate,
                                                        (cypher_comparison_boolexpr *)expr);
        ereport(ERROR,
                (errmsg_internal("unrecognized ExtensibleNode: %s",
                                 ((ExtensibleNode *)expr)->extnodename)));
        return NULL;
    case T_FuncCall:
        return transform_FuncCall(cpstate, (FuncCall *)expr);
    case T_SubLink:
        return transform_SubLink(cpstate, (SubLink *)expr);
        break;
    default:
        ereport(ERROR, (errmsg_internal("unrecognized node type: %d",
                                        nodeTag(expr))));
    }
    return NULL;
}

static Node *transform_A_Const(cypher_parsestate *cpstate, A_Const *ac)
{
    ParseState *pstate = (ParseState *)cpstate;
    ParseCallbackState pcbstate;
    Value *v = &ac->val;
    Datum d = (Datum)0;
    bool is_null = false;
    Const *c;

    setup_parser_errposition_callback(&pcbstate, pstate, ac->location);
    switch (nodeTag(v))
    {
    case T_Integer:
        d = integer_to_agtype((int64)intVal(v));
        break;
    case T_Float:
        {
            char *n = strVal(v);
            int64 i;

            if (ag_scanint8(n, true, &i))
            {
                d = integer_to_agtype(i);
            }
            else
            {
                float8 f = float8in_internal(n, NULL, NULL);

                d = float_to_agtype(f);
            }
        }
        break;
    case T_String:
        d = string_to_agtype(strVal(v));
        break;
    case T_Null:
        is_null = true;
        break;
    default:
        ereport(ERROR,
                (errmsg_internal("unrecognized node type: %d", nodeTag(v))));
        return NULL;
    }
    cancel_parser_errposition_callback(&pcbstate);

    // typtypmod, typcollation, typlen, and typbyval of agtype are hard-coded.
    c = makeConst(AGTYPEOID, -1, InvalidOid, -1, d, is_null, false);
    c->location = ac->location;
    return (Node *)c;
}

/*
 * Private function borrowed from PG's transformWholeRowRef.
 * Construct a whole-row reference to represent the notation "relation.*".
 */
static Node *transform_WholeRowRef(ParseState *pstate, RangeTblEntry *rte,
                                   int location)
{
    Var *result;
    int vnum;
    int sublevels_up;

    /* Find the RTE's rangetable location */
    vnum = RTERangeTablePosn(pstate, rte, &sublevels_up);

    /*
     * Build the appropriate referencing node.  Note that if the RTE is a
     * function returning scalar, we create just a plain reference to the
     * function value, not a composite containing a single column.  This is
     * pretty inconsistent at first sight, but it's what we've done
     * historically.  One argument for it is that "rel" and "rel.*" mean the
     * same thing for composite relations, so why not for scalar functions...
     */
     result = makeWholeRowVar(rte, vnum, sublevels_up, true);

     /* location is not filled in by makeWholeRowVar */
     result->location = location;

     /* mark relation as requiring whole-row SELECT access */
     markVarForSelectPriv(pstate, result, rte);

     return (Node *)result;
}

/*
 * Function to transform a ColumnRef node from the grammar into a Var node
 * Code borrowed from PG's transformColumnRef.
 */
static Node *transform_ColumnRef(cypher_parsestate *cpstate, ColumnRef *cref)
{
    ParseState *pstate = (ParseState *)cpstate;
    RangeTblEntry *rte = NULL;
    Node *field1 = NULL;
    Node *field2 = NULL;
    char *colname = NULL;
    char *nspname = NULL;
    char *relname = NULL;
    Node *node = NULL;
    int levels_up;

    switch (list_length(cref->fields))
    {
        case 1:
            {
                transform_entity *te;
                field1 = (Node*)linitial(cref->fields);

                Assert(IsA(field1, String));
                colname = strVal(field1);

                /* Try to identify as an unqualified column */
                node = colNameToVar(pstate, colname, false, cref->location);
                if (node != NULL)
                {
                        break;
                }

                /*
                 * Try to find the columnRef as a transform_entity and extract
                 * the expr.
                 */
                te = find_variable(cpstate, colname) ;
                if (te != NULL && te->expr != NULL &&
                    te->declared_in_current_clause)
                {
                    node = (Node *)te->expr;
                    break;
                }

                /*
                 * Not known as a column of any range-table entry.
                 * Try to find the name as a relation.  Note that only
                 * relations already entered into the rangetable will be
                 * recognized.
                 *
                 * This is a hack for backwards compatibility with
                 * PostQUEL-inspired syntax.  The preferred form now is
                 * "rel.*".
                 */
                rte = refnameRangeTblEntry(pstate, NULL, colname,
                                           cref->location, &levels_up);
                if (rte)
                {
                    node = transform_WholeRowRef(pstate, rte, cref->location);
                }
                else
                {
                    ereport(ERROR,
                            (errcode(ERRCODE_UNDEFINED_COLUMN),
                             errmsg("could not find rte for %s", colname),
                             errhint("variable %s does not exist within scope of usage",
                                     colname),
                             parser_errposition(pstate, cref->location)));
                }

                if (node == NULL)
                {
                    ereport(ERROR, (errcode(ERRCODE_DATA_EXCEPTION),
                            errmsg("unable to transform whole row for %s", colname),
                             parser_errposition(pstate, cref->location)));
                }

                break;
            }
        case 2:
            {
                Oid inputTypeId = InvalidOid;
                Oid targetTypeId = InvalidOid;

                field1 = (Node*)linitial(cref->fields);
                field2 = (Node*)lsecond(cref->fields);

                Assert(IsA(field1, String));
                relname = strVal(field1);

                if (IsA(field2, String))
                {
                    colname = strVal(field2);
                }

                /* locate the referenced RTE */
                rte = refnameRangeTblEntry(pstate, nspname, relname,
                                           cref->location, &levels_up);
                if (rte == NULL)
                {
                    ereport(ERROR,
                            (errcode(ERRCODE_UNDEFINED_COLUMN),
                             errmsg("could not find rte for %s.%s", relname,
                                    colname),
                             errhint("variable %s does not exist within scope of usage",
                                     relname),
                             parser_errposition(pstate, cref->location)));
                    break;
                }

                /*
                 * TODO: Left in for potential future use.
                 * Is it a whole-row reference?
                 */
                if (IsA(field2, A_Star))
                {
                    node = transform_WholeRowRef(pstate, rte, cref->location);
                    break;
                }

                Assert(IsA(field2, String));
                /* try to identify as a column of the RTE */
                node = scanRTEForColumn(pstate, rte, colname, cref->location, false);
                if (node == NULL)
                {
                    ereport(ERROR,
                            (errcode(ERRCODE_UNDEFINED_COLUMN),
                             errmsg("could not find column %s in rel %s of rte",
                                    colname, relname),
                             parser_errposition(pstate, cref->location)));
                }

                /* coerce it to AGTYPE if possible */
                inputTypeId = exprType(node);
                targetTypeId = AGTYPEOID;

                if (can_coerce_type(1, &inputTypeId, &targetTypeId,
                                    COERCION_EXPLICIT))
                {
                    node = coerce_type(pstate, node, inputTypeId, targetTypeId,
                                       -1, COERCION_EXPLICIT,
                                       COERCE_EXPLICIT_CAST, NULL, NULL, -1);
                }
                break;
            }
        default:
            {
                ereport(ERROR,
                        (errcode(ERRCODE_SYNTAX_ERROR),
                         errmsg("improper qualified name (too many dotted names): %s",
                                NameListToString(cref->fields)),
                         parser_errposition(pstate, cref->location)));
                break;
            }
    }

    if (node == NULL)
    {
        ereport(ERROR,
                (errcode(ERRCODE_UNDEFINED_COLUMN),
                 errmsg("variable `%s` does not exist", colname),
                 parser_errposition(pstate, cref->location)));
    }

    return node;
}

static Node *transform_AEXPR_OP(cypher_parsestate *cpstate, A_Expr *a)
{
    ParseState *pstate = (ParseState *)cpstate;
    Node *last_srf = pstate->p_last_srf;
    Node *lexpr = transform_cypher_expr_recurse(cpstate, a->lexpr);
    Node *rexpr = transform_cypher_expr_recurse(cpstate, a->rexpr);
    char *opname = strVal(linitial(a->name));
    Oid lexpr_type = exprType(lexpr);
    Oid rexpr_type = exprType(rexpr);
    bool left_is_entity = lexpr_type == VERTEXOID || lexpr_type == EDGEOID;
    bool right_is_entity = rexpr_type == VERTEXOID || rexpr_type == EDGEOID;
    bool is_eq_op = strcmp(opname, "=") == 0 || strcmp(opname, "<>") == 0;
    if (is_eq_op && lexpr_type == rexpr_type && left_is_entity) {
        return (Node *)make_op(pstate, a->name, lexpr, rexpr, last_srf,
                               a->location);
    }

    if (left_is_entity) {
        lexpr = coerce_entity_to_agtype(pstate, lexpr);
    }
    if (right_is_entity) {
        rexpr = coerce_entity_to_agtype(pstate, rexpr);
    }

    return (Node *)make_op(pstate, a->name, lexpr, rexpr, last_srf,
                           a->location);
}

/*
 * function for transforming cypher comparison A_Expr. Since this node is a
 * wrapper to let us know when a comparison occurs in a chained comparison,
 * we convert it to a regular A_Expr and transform it.
 */
static Node *transform_cypher_comparison_aexpr_OP(cypher_parsestate *cpstate,
                                                  cypher_comparison_aexpr *a)
{
    A_Expr *n = makeNode(A_Expr);

    n->kind = a->kind;
    n->name = a->name;
    n->lexpr = a->lexpr;
    n->rexpr = a->rexpr;
    n->location = a->location;

    return (Node *)transform_AEXPR_OP(cpstate, n);
}

static Node *transform_AEXPR_IN(cypher_parsestate *cpstate, A_Expr *a)
{
    ParseState *pstate = (ParseState *)cpstate;
    cypher_list *raw_rexpr;
    Node *result = NULL;
    Node *lexpr;
    List *rexprs = NIL;
    List *rvars = NIL;
    List *rnonvars = NIL;
    bool use_or;
    ListCell *list_cell;

    if (!is_ag_node(a->rexpr, cypher_list)) {
        Oid function_oid;
        FuncExpr *function_expression;
        List *arguments = NIL;

        arguments = lappend(arguments,
                            transform_cypher_expr_recurse(cpstate, a->rexpr));
        arguments = lappend(arguments,
                            transform_cypher_expr_recurse(cpstate, a->lexpr));
        function_oid = get_ag_func_oid("agtype_in_operator",
                                       AGTYPE_IN_OPERATOR_ARGUMENT_COUNT, AGTYPEOID,
                                       AGTYPEOID);
        function_expression = makeFuncExpr(function_oid, AGTYPEOID, arguments,
                                           InvalidOid, InvalidOid,
                                           COERCE_EXPLICIT_CALL);
        function_expression->location = exprLocation(a->lexpr);

        return (Node *)function_expression;
    }

    raw_rexpr = (cypher_list *)a->rexpr;
    use_or = strcmp(strVal(linitial(a->name)), "<>") != 0;

    if (raw_rexpr->elems == NIL) {
        return (Node *)makeBoolConst(!use_or, false);
    }

    lexpr = transform_cypher_expr_recurse(cpstate, a->lexpr);

    foreach (list_cell, raw_rexpr->elems)
    {
        Node *rexpr = transform_cypher_expr_recurse(
            cpstate, (Node *)lfirst(list_cell));

        rexprs = lappend(rexprs, rexpr);
        if (contain_vars_of_level(rexpr, 0)) {
            rvars = lappend(rvars, rexpr);
        } else {
            rnonvars = lappend(rnonvars, rexpr);
        }
    }

    if (list_length(rnonvars) > 1) {
        List *array_elements = NIL;
        ArrayExpr *array_expr;

        foreach (list_cell, rnonvars)
        {
            Node *rexpr = (Node *)lfirst(list_cell);

            rexpr = coerce_to_common_type(pstate, rexpr, AGTYPEOID, "IN");
            array_elements = lappend(array_elements, rexpr);
        }

        array_expr = makeNode(ArrayExpr);
        array_expr->array_typeid = get_array_type(AGTYPEOID);
        array_expr->element_typeid = AGTYPEOID;
        array_expr->elements = array_elements;
        array_expr->multidims = false;
        array_expr->location = -1;

        result = (Node *)make_scalar_array_op(
            pstate, a->name, use_or, lexpr, (Node *)array_expr, a->location);
        rexprs = rvars;
    }

    foreach (list_cell, rexprs)
    {
        Node *rexpr = (Node *)lfirst(list_cell);
        Node *comparison = (Node *)make_op(
            pstate, a->name, (Node *)copyObject(lexpr), rexpr,
            pstate->p_last_srf, a->location);

        comparison = coerce_to_boolean(pstate, comparison, "IN");
        if (result == NULL) {
            result = comparison;
        } else {
            result = (Node *)makeBoolExpr(
                use_or ? OR_EXPR : AND_EXPR,
                list_make2(result, comparison), a->location);
        }
    }

    return result;
}

static Node *transform_BoolExpr(cypher_parsestate *cpstate, BoolExpr *expr)
{
    ParseState *pstate = (ParseState *)cpstate;
    List *args = NIL;
    const char *opname;
    ListCell *la;

    switch (expr->boolop)
    {
    case AND_EXPR:
        opname = "AND";
        break;
    case OR_EXPR:
        opname = "OR";
        break;
    case NOT_EXPR:
        opname = "NOT";
        break;
    default:
        ereport(ERROR, (errmsg_internal("unrecognized boolop: %d",
                                        (int)expr->boolop)));
        return NULL;
    }

    foreach (la, expr->args)
    {
        Node *arg = (Node*)lfirst(la);

        arg = transform_cypher_expr_recurse(cpstate, arg);
        arg = coerce_cypher_expr_to_boolean(pstate, arg, opname);

        args = lappend(args, arg);
    }

    return (Node *)makeBoolExpr(expr->boolop, args, expr->location);
}

/*
 * function for transforming cypher_comparison_boolexpr. Since this node is a
 * wrapper to let us know when a comparison occurs in a chained comparison,
 * we convert it to a PG BoolExpr and transform it.
 */
static Node *transform_cypher_comparison_boolexpr(cypher_parsestate *cpstate,
                                                  cypher_comparison_boolexpr *b)
{
    BoolExpr *n = makeNode(BoolExpr);

    n->boolop = b->boolop;
    n->args = b->args;
    n->location = b->location;

    return transform_BoolExpr(cpstate, n);
}

static Node *coerce_cypher_expr_to_boolean(ParseState *pstate, Node *expr,
                                           const char *construct_name)
{
    if (expr != NULL && IsA(expr, BoolExpr)) {
        return expr;
    }

    return coerce_to_boolean(pstate, expr, construct_name);
}

static Node *transform_cypher_bool_const(cypher_parsestate *cpstate,
                                         cypher_bool_const *bc)
{
    ParseState *pstate = (ParseState *)cpstate;
    ParseCallbackState pcbstate;
    Datum agt;
    Const *c;

    setup_parser_errposition_callback(&pcbstate, pstate, bc->location);
    agt = boolean_to_agtype(bc->boolean);
    cancel_parser_errposition_callback(&pcbstate);

    // typtypmod, typcollation, typlen, and typbyval of agtype are hard-coded.
    c = makeConst(AGTYPEOID, -1, InvalidOid, -1, agt, false, false);
    c->location = bc->location;

    return (Node *)c;
}

static Node *transform_cypher_integer_const(cypher_parsestate *cpstate,
                                            cypher_integer_const *ic)
{
    ParseState *pstate = (ParseState *)cpstate;
    ParseCallbackState pcbstate;
    Datum agt;
    Const *c;

    setup_parser_errposition_callback(&pcbstate, pstate, ic->location);
    agt = integer_to_agtype(ic->integer);
    cancel_parser_errposition_callback(&pcbstate);

    // typtypmod, typcollation, typlen, and typbyval of agtype are hard-coded.
    c = makeConst(AGTYPEOID, -1, InvalidOid, -1, agt, false, false);
    c->location = ic->location;

    return (Node *)c;
}

static Node *transform_cypher_param(cypher_parsestate *cpstate,
                                    cypher_param *cp)
{
    ParseState *pstate = (ParseState *)cpstate;
    Const *const_str;
    FuncExpr *func_expr;
    Oid func_access_oid;
    List *args = NIL;

    if (!cpstate->params)
    {
        ereport(
            ERROR,
            (errcode(ERRCODE_UNDEFINED_PARAMETER),
             errmsg(
                 "parameters argument is missing from cypher() function call"),
             parser_errposition(pstate, cp->location)));
    }

    /* get the agtype_access_operator function */
    func_access_oid = get_ag_func_oid("agtype_access_operator", 1,
                                      AGTYPEARRAYOID);

    args = lappend(args, copyObject(cpstate->params));

    const_str = makeConst(AGTYPEOID, -1, InvalidOid, -1,
                          string_to_agtype(cp->name), false, false);

    args = lappend(args, const_str);

    func_expr = makeFuncExpr(func_access_oid, AGTYPEOID, args, InvalidOid,
                             InvalidOid, COERCE_EXPLICIT_CALL);
    func_expr->location = cp->location;

    return (Node *)func_expr;
}

static Node *transform_cypher_map(cypher_parsestate *cpstate, cypher_map *cm)
{
    ParseState *pstate = (ParseState *)cpstate;
    List *newkeyvals = NIL;
    ListCell *le;
    FuncExpr *fexpr;
    Oid func_oid;

    Assert(list_length(cm->keyvals) % 2 == 0);

    le = list_head(cm->keyvals);
    while (le != NULL)
    {
        Node *key;
        Node *val;
        Node *newval;
        ParseCallbackState pcbstate;
        Const *newkey;
        key = (Node*)lfirst(le);
        le = lnext(le);
        val = (Node*)lfirst(le);
        le = lnext(le);

        newval = transform_cypher_expr_recurse(cpstate, val);

        setup_parser_errposition_callback(&pcbstate, pstate, cm->location);
        // typtypmod, typcollation, typlen, and typbyval of agtype are
        // hard-coded.
        newkey = makeConst(TEXTOID, -1, InvalidOid, -1,
                           CStringGetTextDatum(strVal(key)), false, false);
        cancel_parser_errposition_callback(&pcbstate);

        newkeyvals = lappend(lappend(newkeyvals, newkey), newval);
    }

    if (list_length(newkeyvals) == 0)
        func_oid = get_ag_func_oid("agtype_build_map", 0);
    else if (cm->keep_null)
        func_oid = get_ag_func_oid("agtype_build_map", 1, ANYOID);
    else
        func_oid = get_ag_func_oid("agtype_build_map_nonull", 1, ANYOID);

    fexpr = makeFuncExpr(func_oid, AGTYPEOID, newkeyvals, InvalidOid,
                         InvalidOid, COERCE_EXPLICIT_CALL);
    fexpr->location = cm->location;

    return (Node *)fexpr;
}

static Oid get_entity_record_type(Node *node)
{
    Oid entity_type_oid;

    if (IsA(node, RowExpr)) {
        entity_type_oid = ((RowExpr *)node)->row_typeid;
    } else if (IsA(node, Var)) {
        entity_type_oid = ((Var *)node)->vartype;
    } else {
        ereport(ERROR,
                (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                 errmsg("argument must be a vertex or edge record")));
        pg_unreachable();
    }

    if (entity_type_oid != VERTEXOID && entity_type_oid != EDGEOID) {
        ereport(ERROR,
                (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                 errmsg("argument must be a vertex or edge record")));
    }

    return entity_type_oid;
}

bool is_vertex_or_edge(Node *node)
{
    Oid entity_type_oid;

    if (node == NULL) {
        return false;
    }

    if (IsA(node, RowExpr)) {
        entity_type_oid = ((RowExpr *)node)->row_typeid;
    } else if (IsA(node, Var)) {
        entity_type_oid = ((Var *)node)->vartype;
        } else {
        return false;
    }

    return entity_type_oid == VERTEXOID || entity_type_oid == EDGEOID;
}

Node *extract_field_from_record(Node *node, const char *field_name)
{
    AttrNumber fieldnum;
    Oid fieldtype;
    Oid entity_type_oid = get_entity_record_type(node);

    get_record_field_info(field_name, entity_type_oid, &fieldnum, &fieldtype);
    if (fieldnum == InvalidAttrNumber || fieldtype == InvalidOid) {
        ereport(ERROR,
                (errcode(ERRCODE_UNDEFINED_COLUMN),
                 errmsg("field '%s' does not exist for the entity",
                        field_name)));
    }

    if (IsA(node, RowExpr)) {
        RowExpr *row_expr = (RowExpr *)node;

        if (list_length(row_expr->args) < fieldnum) {
            ereport(ERROR,
                    (errcode(ERRCODE_UNDEFINED_COLUMN),
                     errmsg("field '%s' does not exist in entity row",
                            field_name)));
        }

        return (Node *)list_nth(row_expr->args, fieldnum - 1);
    }

    return (Node *)make_field_select((Expr *)node, fieldnum, fieldtype);
}

Node *make_properties_expr(Node *prop_var)
{
    Oid func_oid;

    if (is_vertex_or_edge(prop_var)) {
        return extract_field_from_record(prop_var, "properties");
    }

    func_oid = get_ag_func_oid("age_properties", 1, AGTYPEOID);
    return (Node *)makeFuncExpr(func_oid, AGTYPEOID, list_make1(prop_var),
                                InvalidOid, InvalidOid,
                                COERCE_EXPLICIT_CALL);
}

static Node *transform_cypher_map_projection(cypher_parsestate *cpstate,
                                             cypher_map_projection *cmp)
{
    ParseState *pstate = (ParseState *)cpstate;
    ListCell *lc;
    List *keyvals = NIL;
    FuncExpr *fexpr_new_map = NULL;
    bool has_all_prop_selector = false;
    Node *transformed_map_var;
    Node *fexpr_orig_map;

    transformed_map_var = transform_cypher_expr_recurse(cpstate,
                                                        (Node *)cmp->map_var);
    fexpr_orig_map = make_properties_expr(transformed_map_var);

    foreach (lc, cmp->map_elements)
    {
        cypher_map_projection_element *elem;
        Const *key = NULL;
        Node *val = NULL;

        elem = (cypher_map_projection_element *)lfirst(lc);
        if (elem->type == ALL_PROPERTIES_SELECTOR) {
            has_all_prop_selector = true;
            continue;
        }

        switch (elem->type) {
            case PROPERTY_SELECTOR:
                {
                Oid func_access_oid;
                FuncExpr *func_access_expr;
                ArrayExpr *args_access_expr;
                Const *key_agtype;

                key = makeConst(TEXTOID, -1, InvalidOid, -1,
                                CStringGetTextDatum(elem->key), false, false);
                key_agtype = makeConst(AGTYPEOID, -1, InvalidOid, -1,
                                       string_to_agtype(elem->key), false,
                                       false);
                func_access_oid = get_ag_func_oid("agtype_access_operator", 1,
                                                  AGTYPEARRAYOID);
                args_access_expr = make_agtype_array_expr(
                    list_make2(fexpr_orig_map, key_agtype));
                func_access_expr = makeFuncExpr(func_access_oid, AGTYPEOID,
                                                list_make1(args_access_expr),
                                                InvalidOid, InvalidOid,
                                                COERCE_EXPLICIT_CALL);
                func_access_expr->funcvariadic = true;
                func_access_expr->location = elem->location;
                val = (Node *)func_access_expr;
                break;
            }
            case LITERAL_ENTRY:
                key = makeConst(TEXTOID, -1, InvalidOid, -1,
                                CStringGetTextDatum(elem->key), false, false);
                val = transform_cypher_expr_recurse(cpstate, elem->value);
                break;
            case VARIABLE_SELECTOR:
                {
                List *fields;
                char *key_str;

                Assert(IsA(elem->value, ColumnRef));
                fields = ((ColumnRef *)elem->value)->fields;
                key_str = strVal(lfirst(list_head(fields)));
                key = makeConst(TEXTOID, -1, InvalidOid, -1,
                                CStringGetTextDatum(key_str), false, false);
                val = transform_cypher_expr_recurse(cpstate, elem->value);
                break;
            }
            case ALL_PROPERTIES_SELECTOR:
                break;
            default:
                elog(ERROR, "unknown map projection element type");
        }

        Assert(key);
        Assert(val);
        keyvals = lappend(lappend(keyvals, key), val);
    }

    if (keyvals != NIL) {
        Oid func_oid;

        func_oid = get_ag_func_oid("agtype_build_map_nonull", 1, ANYOID);
        fexpr_new_map = makeFuncExpr(func_oid, AGTYPEOID, keyvals, InvalidOid,
                                     InvalidOid, COERCE_EXPLICIT_CALL);
        fexpr_new_map->location = cmp->location;
    }

    if (has_all_prop_selector) {
        if (keyvals == NIL) {
            return fexpr_orig_map;
        }

        return (Node *)make_op(pstate, list_make1(makeString("+")),
                               fexpr_orig_map, (Node *)fexpr_new_map,
                               pstate->p_last_srf, cmp->location);
    }

    Assert(fexpr_new_map);
    return (Node *)fexpr_new_map;
}

static Node *transform_cypher_list(cypher_parsestate *cpstate, cypher_list *cl)
{
    List *build_args = NIL;
    ListCell *le = NULL;
    FuncExpr *concat_lhs = NULL;
    FuncExpr *fexpr = NULL;
    Oid build_func_oid = InvalidOid;
    Oid concat_func_oid = InvalidOid;
    int nelems = list_length(cl->elems);
    int chunk_size = 0;

    /* openGauss, like PostgreSQL, limits a function call to 100 arguments. */
    if (nelems == 0) {
        build_func_oid = get_ag_func_oid("agtype_build_list", 0);
    } else {
        build_func_oid = get_ag_func_oid("agtype_build_list", 1, ANYOID);
    }

    if (nelems > AGTYPE_LIST_CHUNK_SIZE) {
        concat_func_oid = get_ag_func_oid("agtype_add",
                                          AGTYPE_IN_OPERATOR_ARGUMENT_COUNT, AGTYPEOID,
                                          AGTYPEOID);
    }

    foreach (le, cl->elems)
    {
        Node *newv = transform_cypher_expr_recurse(cpstate,
                                                   (Node *)lfirst(le));

        if (chunk_size >= AGTYPE_LIST_CHUNK_SIZE) {
            fexpr = makeFuncExpr(build_func_oid, AGTYPEOID, build_args,
                                 InvalidOid, InvalidOid,
                                 COERCE_EXPLICIT_CALL);
            fexpr->location = cl->location;

            if (concat_lhs == NULL) {
                concat_lhs = fexpr;
            } else {
                concat_lhs = makeFuncExpr(
                    concat_func_oid, AGTYPEOID,
                    list_make2(concat_lhs, fexpr), InvalidOid, InvalidOid,
                    COERCE_EXPLICIT_CALL);
                concat_lhs->location = cl->location;
            }

            build_args = NIL;
            chunk_size = 0;
        }

        build_args = lappend(build_args, newv);
        chunk_size++;
    }

    fexpr = makeFuncExpr(build_func_oid, AGTYPEOID, build_args, InvalidOid,
                         InvalidOid, COERCE_EXPLICIT_CALL);
    fexpr->location = cl->location;

    if (concat_lhs != NULL) {
        fexpr = makeFuncExpr(concat_func_oid, AGTYPEOID,
                             list_make2(concat_lhs, fexpr), InvalidOid,
                             InvalidOid, COERCE_EXPLICIT_CALL);
        fexpr->location = cl->location;
    }

    return (Node *)fexpr;
}


// makes a VARIADIC agtype array
static ArrayExpr *make_agtype_array_expr(List *args)
{
    ArrayExpr  *newa = makeNode(ArrayExpr);

    newa->elements = args;

    /* assume all the variadic arguments were coerced to the same type */
    newa->element_typeid = AGTYPEOID;
    newa->array_typeid = AGTYPEARRAYOID;

    if (!OidIsValid(newa->array_typeid))
    {
        ereport(ERROR,
                (errcode(ERRCODE_UNDEFINED_OBJECT),
                 errmsg("could not find array type for data type %s",
                        format_type_be(newa->element_typeid))));
    }

    /* array_collid will be set by parse_collate.c */
    newa->multidims = false;

    return newa;
}

static RangeTblEntry *find_current_rte(ParseState *pstate, char *relname)
{
    ListCell *lc;

    foreach (lc, pstate->p_relnamespace)
    {
        ParseNamespaceItem *nsitem = (ParseNamespaceItem *)lfirst(lc);
        RangeTblEntry *rte = nsitem->p_rte;

        if (rte != NULL && rte->eref != NULL &&
            strcmp(rte->eref->aliasname, relname) == 0) {
            return rte;
        }
    }

    return NULL;
}

/*
 * Transforms a column ref for indirection. Try to find the rte that the
 * columnRef is references and pass the properties of that rte as what the
 * columnRef is referencing. Otherwise, reference the Var
 */
static Node *transform_column_ref_for_indirection(cypher_parsestate *cpstate,
                                                  ColumnRef *cr)
{
    ParseState *pstate = (ParseState *)cpstate;
    RangeTblEntry *rte = NULL;
    Node *field1 = (Node *)linitial(cr->fields);
    char *relname = NULL;
    Node *node = NULL;
    int levels_up = 0;

    Assert(IsA(field1, String));
    relname = strVal(field1);

    /* Resolve the nearest visible namespace item, including parent scopes. */
    rte = refnameRangeTblEntry(pstate, NULL, relname, cr->location,
                               &levels_up);
    if (rte == NULL)
    {
        rte = find_rte(cpstate, relname);
    }
    if (rte == NULL)
    {
        rte = find_current_rte(pstate, relname);
    }
    if (rte == NULL)
    {
        node = transform_cypher_expr_recurse(cpstate, (Node *)cr);
    } else {
        node = scanRTEForColumn(pstate, rte, "properties", cr->location,
                                false);
    }

    if (node != NULL && is_vertex_or_edge(node))
    {
        return extract_field_from_record(node, "properties");
    }

    if (node == NULL)
    {
        /*
         * Scalar agtype loop vars, such as list-comprehension variables, do not
         * expose a properties column. Use the variable itself as the access root.
         */
        node = transform_cypher_expr_recurse(cpstate, (Node *)cr);
    }

    return node;
}

static Node *transform_A_Indirection(cypher_parsestate *cpstate,
                                     A_Indirection *a_ind)
{
    ParseState *pstate = (ParseState *)cpstate;
    int location;
    ListCell *lc;
    Node *ind_arg_expr;
    FuncExpr *func_expr = NULL;
    Oid func_access_oid;
    Oid func_slice_oid;
    List *args = NIL;
    bool is_access = false;

    /* validate that we have an indirection with at least 1 entry */
    Assert(a_ind != NULL && list_length(a_ind->indirection));
    /* get the agtype_access_operator function */
    func_access_oid = get_ag_func_oid("agtype_access_operator", 1,
                                      AGTYPEARRAYOID);
    /* get the agtype_access_slice function */
    func_slice_oid = get_ag_func_oid("agtype_access_slice", 3, AGTYPEOID,
                                     AGTYPEOID, AGTYPEOID);

    if (IsA(a_ind->arg, ColumnRef))
    {
        ColumnRef *cr = (ColumnRef *)a_ind->arg;

        ind_arg_expr = transform_column_ref_for_indirection(cpstate, cr);
    }
    else
    {
        ind_arg_expr = transform_cypher_expr_recurse(cpstate, a_ind->arg);
    }                                 

    if (ind_arg_expr == NULL)
    {
        ind_arg_expr = transform_cypher_expr_recurse(cpstate, a_ind->arg);
    }

    ind_arg_expr = coerce_entity_to_agtype(pstate, ind_arg_expr);
    ind_arg_expr = coerce_to_common_type(pstate, ind_arg_expr, AGTYPEOID,
                                         "A_indirection");
    location = exprLocation(ind_arg_expr);

    args = lappend(args, ind_arg_expr);
    foreach (lc, a_ind->indirection)
    {
        Node *node = (Node*)lfirst(lc);
        /* is this a slice? */
        if (IsA(node, A_Indices) && ((A_Indices*)node)->is_slice) {
            A_Indices *indices = (A_Indices *)node;

            /* were we working on an access? if so, wrap and close it */
            if (is_access)
            {
                ArrayExpr *newa = make_agtype_array_expr(args);

                func_expr = makeFuncExpr(func_access_oid, AGTYPEOID,
                                         list_make1(newa),
                                         InvalidOid, InvalidOid,
                                         COERCE_EXPLICIT_CALL);

                func_expr->funcvariadic = true;
                func_expr->location = location;

                /*
                 * The completed access is the input container for the
                 * following slice. Do not leave the original root and index
                 * arguments in the slice call.
                 */
                args = lappend(NIL, func_expr);

                /* we are no longer working on an access */
                is_access = false;
            }
            /* add slice bounds to args */
            if (!indices->lidx)
            {
                A_Const *n = makeNode(A_Const);
                n->val.type = T_Null;
                n->location = -1;
                node = transform_cypher_expr_recurse(cpstate, (Node *)n);
            }
            else
                node = transform_cypher_expr_recurse(cpstate, indices->lidx);
            args = lappend(args, node);
            if (!indices->uidx)
            {
                A_Const *n = makeNode(A_Const);
                n->val.type = T_Null;
                n->location = -1;
                node = transform_cypher_expr_recurse(cpstate, (Node *)n);
            }
            else
                node = transform_cypher_expr_recurse(cpstate, indices->uidx);
            args = lappend(args, node);
            /* wrap and close it */
            func_expr = makeFuncExpr(func_slice_oid, AGTYPEOID, args,
                                     InvalidOid, InvalidOid,
                                     COERCE_EXPLICIT_CALL);
            func_expr->location = location;
            args = lappend(NIL, func_expr);
        }
        /* is this a string or index?*/
        else if (IsA(node, String) || IsA(node, A_Indices)) {
            /* we are working on an access */
            is_access = true;
            /* is this an index? */
            if (IsA(node, A_Indices))
            {
                A_Indices *indices = (A_Indices *)node;

                node = transform_cypher_expr_recurse(cpstate, indices->uidx);
                args = lappend(args, node);
            }
            /* it must be a string */
            else
            {
                Const *const_str = makeConst(AGTYPEOID, -1, InvalidOid, -1,
                                             string_to_agtype(strVal(node)),
                                             false, false);
                args = lappend(args, const_str);
            }
        }
        /* not an indirection we understand */
        else {
            ereport(ERROR,
                    (errmsg("invalid indirection node %d", nodeTag(node))));
        }
    }

    /* if we were doing an access, we need wrap the args with access func. */
    if (is_access)    {
        ArrayExpr *newa = make_agtype_array_expr(args);

        func_expr = makeFuncExpr(func_access_oid, AGTYPEOID, list_make1(newa),
                                 InvalidOid, InvalidOid,
                                 COERCE_EXPLICIT_CALL);
        func_expr->funcvariadic = true;
    }
    Assert(func_expr != NULL);
    func_expr->location = location;

    return (Node *)func_expr;
}

static Node *transform_cypher_string_match(cypher_parsestate *cpstate,
                                           cypher_string_match *csm_node)
{
    Node *expr;
    FuncExpr *func_expr;
    Oid func_access_oid;
    List *args = NIL;
    const char *func_name;

    switch (csm_node->operation)
    {
    case CSMO_STARTS_WITH:
        func_name = "agtype_string_match_starts_with";
        break;
    case CSMO_ENDS_WITH:
        func_name = "agtype_string_match_ends_with";
        break;
    case CSMO_CONTAINS:
        func_name = "agtype_string_match_contains";
        break;

    default:
        ereport(ERROR,
                (errmsg_internal("unknown Cypher string match operation")));
        pg_unreachable();
    }

    func_access_oid = get_ag_func_oid(func_name, 2, AGTYPEOID, AGTYPEOID);

    expr = transform_cypher_expr_recurse(cpstate, csm_node->lhs);
    args = lappend(args, expr);
    expr = transform_cypher_expr_recurse(cpstate, csm_node->rhs);
    args = lappend(args, expr);

    func_expr = makeFuncExpr(func_access_oid, AGTYPEOID, args, InvalidOid,
                             InvalidOid, COERCE_EXPLICIT_CALL);
    func_expr->location = csm_node->location;

    return (Node *)func_expr;
}

/*
 * Function to create a typecasting node
 */
static Node *transform_cypher_typecast(cypher_parsestate *cpstate,
                                       cypher_typecast *ctypecast)
{
    ParseState *pstate;
    TypeName *target_typ;
    char *typecast;

    /* verify input parameter */
    Assert (cpstate != NULL);
    Assert (ctypecast != NULL);
    Assert (ctypecast->typname != NULL);
    Assert (ctypecast->typname->names != NIL);

    pstate = &cpstate->pstate;
    target_typ = ctypecast->typname;
    typecast = strVal((Node *)llast(target_typ->names));

    /*
     * Preserve AGE's Cypher aliases when they are unqualified and have no
     * typmod.  Qualified names and typmod-bearing casts use PostgreSQL's
     * normal type resolution below.
     */
    if (list_length(target_typ->names) == 1 && target_typ->typmods == NIL)
    {
        List *fname = list_make1(makeString("ag_catalog"));
        char *funcname = NULL;

        if (pg_strcasecmp(typecast, "edge") == 0) {
            funcname = FUNC_AGTYPE_TYPECAST_EDGE;
        } else if (pg_strcasecmp(typecast, "path") == 0) {
            funcname = FUNC_AGTYPE_TYPECAST_PATH;
        } else if (pg_strcasecmp(typecast, "vertex") == 0) {
            funcname = FUNC_AGTYPE_TYPECAST_VERTEX;
        } else if (pg_strcasecmp(typecast, "numeric") == 0) {
            funcname = FUNC_AGTYPE_TYPECAST_NUMERIC;
        } else if (pg_strcasecmp(typecast, "float") == 0) {
            funcname = FUNC_AGTYPE_TYPECAST_FLOAT;
        } else if (pg_strcasecmp(typecast, "int") == 0 ||
                 pg_strcasecmp(typecast, "integer") == 0) {
            funcname = FUNC_AGTYPE_TYPECAST_INT;
        } else if (pg_strcasecmp(typecast, "bool") == 0 ||
                 pg_strcasecmp(typecast, "boolean") == 0) {
            funcname = FUNC_AGTYPE_TYPECAST_BOOL;
        } else if (pg_strcasecmp(typecast, "pg_float8") == 0) {
            funcname = FUNC_AGTYPE_TYPECAST_PG_FLOAT8;
        } else if (pg_strcasecmp(typecast, "pg_bigint") == 0) {
            funcname = FUNC_AGTYPE_TYPECAST_PG_BIGINT;
        }

        if (funcname != NULL) {
            FuncCall *fnode;

            fname = lappend(fname, makeString(funcname));
            fnode = makeFuncCall(fname, list_make1(ctypecast->expr),
                                 ctypecast->location);
            return transform_FuncCall(cpstate, fnode);
        }
    }

    {
        Oid target_oid = InvalidOid;
        int32 target_typmod = -1;
        Node *expr = NULL;

        typenameTypeIdAndMod(pstate, target_typ, &target_oid, &target_typmod);

        if (is_external_vector_typecast(typecast)) {
            if (IsA(ctypecast->expr, A_Const) &&
                nodeTag(&((A_Const *)ctypecast->expr)->val) == T_String)
                return transform_vector_string_typecast(ctypecast, target_oid,
                                                        target_typmod);

            if (is_ag_node(ctypecast->expr, cypher_list) &&
                pg_strcasecmp(typecast, "sparsevec") != 0)
                return transform_vector_list_typecast(cpstate, ctypecast,
                                                      target_oid,
                                                      target_typmod);
        }

        expr = transform_cypher_expr_recurse(cpstate, ctypecast->expr);

        return coerce_expr_flexible(pstate, expr, exprType(expr), target_oid,
                                    target_typmod, true);
    }
}

static bool is_external_vector_typecast(const char *typecast)
{
    return pg_strcasecmp(typecast, "vector") == 0 ||
           pg_strcasecmp(typecast, "halfvec") == 0 ||
           pg_strcasecmp(typecast, "sparsevec") == 0;
}

static bool is_external_vector_type_oid(Oid type_oid)
{
    char *typname = get_typename(type_oid);
    bool is_vector_type = false;

    if (typname == NULL) {
        return false;
    }

    is_vector_type = is_external_vector_typecast(typname);
    pfree(typname);

    return is_vector_type;
}

static bool has_external_vector_arg(List *args)
{
    ListCell *lc = NULL;

    foreach(lc, args)
    {
        if (is_external_vector_type_oid(exprType((Node *)lfirst(lc))))
            return true;
    }

    return false;
}

static Node *transform_vector_list_typecast(cypher_parsestate *cpstate,
                                            cypher_typecast *ctypecast,
                                            Oid target_oid,
                                            int32 target_typmod)
{
    ParseState *pstate = &cpstate->pstate;
    cypher_list *cl = (cypher_list *)ctypecast->expr;
    ArrayExpr *array = makeNode(ArrayExpr);
    List *elements = NIL;
    ListCell *lc = NULL;

    foreach(lc, cl->elems)
    {
        Node *elem = (Node *)lfirst(lc);
        float8 value = 0;
        Const *c = NULL;

        if (!IsA(elem, A_Const))
            ereport(ERROR,
                    (errmsg_internal("vector typecast only supports numeric list literals")));

        A_Const *aconst = (A_Const *)elem;

        if (nodeTag(&aconst->val) == T_Integer)
            value = (float8)intVal(&aconst->val);
        else if (nodeTag(&aconst->val) == T_Float)
            value = float8in_internal(strVal(&aconst->val), NULL, NULL);
        else
            ereport(ERROR,
                    (errmsg_internal("vector typecast only supports numeric list literals")));

        c = makeConst(FLOAT8OID, -1, InvalidOid, sizeof(float8),
                      Float8GetDatum(value), false, FLOAT8PASSBYVAL);
        c->location = aconst->location;
        elements = lappend(elements, c);
    }

    array->array_typeid = get_array_type(FLOAT8OID);
    array->array_collid = InvalidOid;
    array->element_typeid = FLOAT8OID;
    array->elements = elements;
    array->multidims = false;
    array->location = cl->location;

    return coerce_expr_flexible(pstate, (Node *)array, array->array_typeid,
                                target_oid, target_typmod, true);
}

static Node *transform_vector_string_typecast(cypher_typecast *ctypecast,
                                              Oid target_oid,
                                              int32 target_typmod)
{
    A_Const *aconst = (A_Const *)ctypecast->expr;
    Oid typinput;
    Oid typioparam;
    int16 typlen;
    bool typbyval;
    Datum value;
    Const *c = NULL;

    getTypeInputInfo(target_oid, &typinput, &typioparam);
    get_typlenbyval(target_oid, &typlen, &typbyval);

    value = OidInputFunctionCall(typinput, strVal(&aconst->val), typioparam,
                                 target_typmod);

    c = makeConst(target_oid, target_typmod, InvalidOid, typlen, value, false,
                  typbyval);
    c->location = aconst->location;

    return (Node *)c;
}

static Node *transform_cypher_list_comprehension_expr(cypher_parsestate *cpstate,
    cypher_list_comprehension *list_comp)
{
    Node *list_expr = transform_cypher_expr_recurse(cpstate, list_comp->expr);
    SubLink *sublink = makeNode(SubLink);
    Node *result = NULL;
    CaseExpr *guard = makeNode(CaseExpr);
    CaseWhen *when = makeNode(CaseWhen);
    NullTest *is_null = makeNode(NullTest);
    Const *null_list = makeConst(AGTYPEOID, -1, InvalidOid, -1, (Datum)0, true, false);

    sublink->subLinkType = EXPR_SUBLINK;
    sublink->testexpr = NULL;
    sublink->operName = NIL;
    sublink->subselect = (Node *)list_comp;
    sublink->location = -1;

    result = transform_SubLink(cpstate, sublink);

    is_null->arg = (Expr *)list_expr;
    is_null->nulltesttype = IS_NULL;
    is_null->argisrow = type_is_rowtype(exprType(list_expr));

    when->expr = (Expr *)is_null;
    when->result = (Expr *)null_list;
    when->location = -1;

    guard->casetype = AGTYPEOID;
    guard->arg = NULL;
    guard->args = list_make1(when);
    guard->defresult = (Expr *)result;
    guard->location = -1;

    return (Node *)guard;
}

static Node *transform_cypher_reduce_expr(cypher_parsestate *cpstate,
                                          cypher_reduce *reduce)
{
    Node *list_expr = transform_cypher_expr_recurse(cpstate, reduce->expr);
    SubLink *sublink = makeNode(SubLink);
    Node *result = NULL;
    Node *init_expr = NULL;
    CoalesceExpr *coalesce = makeNode(CoalesceExpr);
    CaseExpr *guard = makeNode(CaseExpr);
    CaseWhen *when = makeNode(CaseWhen);
    NullTest *is_null = makeNode(NullTest);
    Const *null_agtype = makeConst(AGTYPEOID, -1, InvalidOid, -1, (Datum)0,
                                   true, false);

    sublink->subLinkType = EXPR_SUBLINK;
    sublink->testexpr = NULL;
    sublink->operName = NIL;
    sublink->subselect = (Node *)reduce;
    sublink->location = -1;

    result = transform_SubLink(cpstate, sublink);

    /*
     * The fold aggregate returns no rows for an empty list, so the scalar
     * sublink yields SQL NULL in that case. Cypher's reduce() must return the
     * initial value for an empty (but non-null) list, so wrap the sublink in
     * COALESCE(<agg>, <init>). The init expression is normalized to agtype so
     * the COALESCE result type stays agtype.
     */
    init_expr = transform_cypher_expr_recurse(cpstate, reduce->initial);
    if (exprType(init_expr) != AGTYPEOID) {
        init_expr = coerce_to_common_type((ParseState *)cpstate, init_expr,
                                          AGTYPEOID, "reduce");
    }

    coalesce->coalescetype = AGTYPEOID;
    coalesce->args = list_make2(result, init_expr);
    coalesce->location = -1;

    is_null->arg = (Expr *)list_expr;
    is_null->nulltesttype = IS_NULL;
    is_null->argisrow = type_is_rowtype(exprType(list_expr));

    when->expr = (Expr *)is_null;
    when->result = (Expr *)null_agtype;
    when->location = -1;

    guard->casetype = AGTYPEOID;
    guard->arg = NULL;
    guard->args = list_make1(when);
    guard->defresult = (Expr *)coalesce;
    guard->location = -1;

    return (Node *)guard;
}

static Node *transform_cypher_predicate_function_expr(cypher_parsestate *cpstate,
    cypher_predicate_function *pred_func)
{
    Node *list_expr = transform_cypher_expr_recurse(cpstate, pred_func->expr);
    SubLink *sublink = makeNode(SubLink);
    Node *result = NULL;
    CaseExpr *guard = makeNode(CaseExpr);
    CaseWhen *when = makeNode(CaseWhen);
    NullTest *is_null = makeNode(NullTest);
    Const *null_bool = makeConst(BOOLOID, -1, InvalidOid, 1, (Datum)0, true, true);

    sublink->subLinkType = EXPR_SUBLINK;
    sublink->testexpr = NULL;
    sublink->operName = NIL;
    sublink->subselect = (Node *)pred_func;
    sublink->location = -1;

    result = transform_SubLink(cpstate, sublink);

    is_null->arg = (Expr *)list_expr;
    is_null->nulltesttype = IS_NULL;
    is_null->argisrow = type_is_rowtype(exprType(list_expr));

    when->expr = (Expr *)is_null;
    when->result = (Expr *)null_bool;
    when->location = -1;

    guard->casetype = BOOLOID;
    guard->arg = NULL;
    guard->args = list_make1(when);
    guard->defresult = (Expr *)result;
    guard->location = -1;

    return (Node *)guard;
}

/*
 * Helper function to coerce an expression to the target type. If no direct cast
 * exists, try casting through text when agtype is involved.
 */
static Node *coerce_expr_flexible(ParseState *pstate, Node *expr,
                                  Oid source_oid, Oid target_oid,
                                  int32 t_typmod, bool error_out)
{
    Node *result;

    if (expr == NULL)
        return NULL;

    result = coerce_to_target_type(pstate, expr, source_oid, target_oid,
                                   t_typmod, COERCION_EXPLICIT,
                                   COERCE_EXPLICIT_CAST, NULL, NULL, -1);
    if (result != NULL)
        return result;

    if (source_oid == AGTYPEOID || target_oid == AGTYPEOID) {
        Node *to_text = coerce_to_target_type(pstate, expr, source_oid,
                                              TEXTOID, -1,
                                              COERCION_EXPLICIT,
                                              COERCE_EXPLICIT_CAST, NULL, NULL,
                                              -1);
        if (to_text != NULL) {
            result = coerce_to_target_type(pstate, to_text, TEXTOID,
                                           target_oid, t_typmod,
                                           COERCION_EXPLICIT,
                                           COERCE_EXPLICIT_CAST, NULL, NULL,
                                           -1);
            if (result != NULL)
                return result;
        }
    }

    if (is_external_vector_type_oid(source_oid) &&
        is_external_vector_type_oid(target_oid)) {
        TypeName *typname = makeTypeName("vector");
        Oid vector_oid = InvalidOid;
        int32 vector_typmod = -1;

        typenameTypeIdAndMod(pstate, typname, &vector_oid, &vector_typmod);

        if (OidIsValid(vector_oid) &&
            source_oid != vector_oid &&
            target_oid != vector_oid) {
            Node *to_vector = coerce_to_target_type(pstate, expr, source_oid,
                                                    vector_oid, -1,
                                                    COERCION_EXPLICIT,
                                                    COERCE_EXPLICIT_CAST, NULL,
                                                    NULL, -1);
            if (to_vector != NULL) {
                result = coerce_to_target_type(pstate, to_vector, vector_oid,
                                               target_oid, t_typmod,
                                               COERCION_EXPLICIT,
                                               COERCE_EXPLICIT_CAST, NULL,
                                               NULL, -1);
            }
            /* result is still NULL here unless the vector route succeeded */
            if (result != NULL) {
                return result;
            }
        }
    }

    if (error_out)
        ereport(ERROR,
                (errmsg_internal("typecast '%s' not supported",
                                 format_type_be(target_oid))));

    return NULL;
}

static List *cast_agtype_args_to_target_type(cypher_parsestate *cpstate,
                                             Form_pg_proc procform,
                                             List *fargs,
                                             Oid *target_types)
{
    char *funcname = NameStr(procform->proname);
    int nargs = procform->pronargs;
    int argno = 0;
    ListCell *lc = NULL;

    if (list_length(fargs) != nargs)
        ereport(ERROR,
                (errcode(ERRCODE_DATA_EXCEPTION),
                 errmsg("function %s requires %d arguments, %d given",
                        funcname, nargs, list_length(fargs))));

    foreach(lc, fargs)
    {
        Node *expr = (Node *)lfirst(lc);
        Oid source_oid = exprType(expr);
        Oid target_oid = target_types[argno++];

        expr = coerce_expr_flexible(&cpstate->pstate, expr, source_oid,
                                    target_oid, -1, true);
        lfirst(lc) = expr;
    }

    return fargs;
}

static Node *wrap_text_output_to_agtype(cypher_parsestate *cpstate,
                                        FuncExpr *fexpr)
{
    ParseState *pstate = &cpstate->pstate;
    Node *last_srf = pstate->p_last_srf;
    List *fname = NIL;
    FuncCall *fnode = NULL;

    if (fexpr->funcresulttype != TEXTOID)
        ereport(ERROR,
                (errcode(ERRCODE_DATA_EXCEPTION),
                 errmsg("can only wrap text to agtype")));

    fname = list_make2(makeString("ag_catalog"), makeString("text_to_agtype"));
    fnode = makeFuncCall(fname, list_make1(fexpr), -1);

    return ParseFuncOrColumn(pstate, fname, list_make1(fexpr), last_srf,
                             fnode, -1, false);
}

static Node *transform_external_ext_FuncCall(cypher_parsestate *cpstate,
                                             FuncCall *fn, List *targs,
                                             Form_pg_proc procform)
{
    ParseState *pstate = &cpstate->pstate;
    Node *last_srf = pstate->p_last_srf;
    FuncExpr *fexpr = NULL;
    Node *retval = NULL;

    Assert(procform != NULL);

    targs = cast_agtype_args_to_target_type(cpstate, procform, targs,
                                            procform->proargtypes.values);

    fexpr = (FuncExpr *)ParseFuncOrColumn(pstate, fn->funcname, targs,
                                          last_srf, fn, fn->location, false);
    if (fexpr->funcresulttype == TEXTOID)
        retval = wrap_text_output_to_agtype(cpstate, fexpr);
    else
        retval = (Node *)fexpr;

    if (retval != NULL && retval->type == T_Aggref)
        cpstate->exprHasAgg = true;

    return retval;
}

static Form_pg_proc copy_procform(Form_pg_proc candidate)
{
    Form_pg_proc procform = NULL;
    size_t procform_size;

    procform_size = offsetof(FormData_pg_proc, proargtypes.values) +
                    candidate->pronargs * sizeof(Oid);
    procform = (Form_pg_proc)palloc0(procform_size);
    memcpy(procform, candidate, offsetof(FormData_pg_proc, proargtypes.values));
    procform->proargtypes.dim1 = candidate->pronargs;
    for (int argno = 0; argno < candidate->pronargs; argno++)
        procform->proargtypes.values[argno] =
            candidate->proargtypes.values[argno];

    return procform;
}

static Form_pg_proc get_procform(FuncCall *fn, List *targs,
                                 bool err_not_found)
{
    CatCList *catlist = NULL;
    Form_pg_proc procform = NULL;
    List *search_path = NIL;
    char *funcname = strVal(linitial(fn->funcname));
    int nargs = list_length(fn->args);
    Oid *source_oids = NULL;
    Oid pg_catalog_oid;
    int i;

    catlist = SearchSysCacheList1(PROCNAMEARGSNSP, CStringGetDatum(funcname));
    if (catlist->n_members == 0) {
        ReleaseSysCacheList(catlist);
        return NULL;
    }

    if (targs != NIL) {
        ListCell *lc = NULL;
        int argno = 0;

        source_oids = (Oid *)palloc0(nargs * sizeof(Oid));
        foreach(lc, targs)
        {
            source_oids[argno++] = exprType((Node *)lfirst(lc));
        }
    }

    search_path = fetch_search_path(false);
    pg_catalog_oid = get_namespace_oid("pg_catalog", false);

    for (i = 0; i < catlist->n_members; i++) {
        ListCell *nsp = NULL;
        HeapTuple proctup = t_thrd.lsc_cxt.FetchTupleFromCatCList(catlist, i);
        Form_pg_proc candidate = (Form_pg_proc)GETSTRUCT(proctup);
        bool visible = false;
        bool exact = true;
        bool coercible = true;
        int argno;

        if (pg_strcasecmp(funcname, NameStr(candidate->proname)) != 0 ||
            nargs != candidate->pronargs ||
            fn->func_variadic != candidate->provariadic)
            continue;

        if (candidate->pronamespace == pg_catalog_oid)
            visible = true;
        else {
            foreach(nsp, search_path)
            {
                Oid oid = lfirst_oid(nsp);
                if (candidate->pronamespace == oid &&
                    isTempNamespace(candidate->pronamespace) == false) {
                    visible = true;
                    break;
                }
            }
        }

        if (!visible)
            continue;

        for (argno = 0; source_oids != NULL && argno < nargs; argno++) {
            Oid target_oid = candidate->proargtypes.values[argno];

            if (source_oids[argno] != target_oid)
                exact = false;

            if (!can_coerce_type(1, &source_oids[argno], &target_oid,
                                 COERCION_EXPLICIT)) {
                coercible = false;
                break;
            }
        }

        if (coercible && exact) {
            procform = copy_procform(candidate);
            break;
        }

        if (coercible && procform == NULL)
            procform = copy_procform(candidate);
    }

    if (err_not_found && procform == NULL)
        ereport(ERROR,
                (errcode(ERRCODE_UNDEFINED_FUNCTION),
                 errmsg("function %s does not exist", funcname),
                 errhint("If the function is from an external extension, "
                         "make sure the extension is installed and the "
                         "function is in the search path.")));

    ReleaseSysCacheList(catlist);
    list_free(search_path);
    if (source_oids != NULL)
        pfree(source_oids);

    return procform;
}

static char *construct_age_function_name(char *funcname)
{
    int pnlen = strlen(funcname);
    char *ag_name = (char *)palloc(pnlen + AGE_FUNCTION_NAME_PREFIX_LENGTH + 1);
    int i;

    memcpy(ag_name, "age_", AGE_FUNCTION_NAME_PREFIX_LENGTH);
    for (i = 0; i < pnlen; i++) {
        ag_name[i + AGE_FUNCTION_NAME_PREFIX_LENGTH] =
            tolower((unsigned char)funcname[i]);
    }
    ag_name[i + AGE_FUNCTION_NAME_PREFIX_LENGTH] = '\0';

    return ag_name;
}

static bool function_exists(char *funcname, char *extension)
{
    CatCList *catlist = NULL;
    bool found = false;
    int i;

    catlist = SearchSysCacheList1(PROCNAMEARGSNSP, CStringGetDatum(funcname));
    if (catlist->n_members == 0) {
        ReleaseSysCacheList(catlist);
        return false;
    }

    if (extension == NULL) {
        ReleaseSysCacheList(catlist);
        return true;
    }

    if (pg_strcasecmp(extension, "age") == 0) {
        Oid ag_catalog_oid = get_namespace_oid("ag_catalog", true);

        for (i = 0; i < catlist->n_members; i++) {
            HeapTuple proctup = t_thrd.lsc_cxt.FetchTupleFromCatCList(catlist, i);
            Form_pg_proc procform = (Form_pg_proc)GETSTRUCT(proctup);
            if (procform->pronamespace == ag_catalog_oid) {
                found = true;
                break;
            }
        }
    }

    ReleaseSysCacheList(catlist);

    return found;
}

static bool is_accessor_function(const char *func_name)
{
    return pg_strcasecmp("id", func_name) == 0 ||
           pg_strcasecmp("properties", func_name) == 0 ||
           pg_strcasecmp("type", func_name) == 0 ||
           pg_strcasecmp("label", func_name) == 0 ||
           pg_strcasecmp("start_id", func_name) == 0 ||
           pg_strcasecmp("end_id", func_name) == 0 ||
           pg_strcasecmp("startnode", func_name) == 0 ||
           pg_strcasecmp("endnode", func_name) == 0;
}

static Node *optimize_accessor_function(cypher_parsestate *cpstate,
                                        const char *func_name,
                                        FuncCall *fn, List *targs)
{
    ParseState *pstate = (ParseState *)cpstate;
    bool is_node_func = pg_strcasecmp(func_name, "startNode") == 0 ||
                        pg_strcasecmp(func_name, "endNode") == 0;
    Node *arg;
    AttrNumber fieldnum;
    Oid fieldtype;
    Oid entity_type;
    Node *field;

    if (is_node_func) {
        if (list_length(targs) < ACCESSOR_NODE_FUNCTION_MIN_ARGUMENTS) {
            return NULL;
        }
        arg = (Node *)lsecond(targs);
    } else {
        if (list_length(targs) < 1) {
            return NULL;
        }
        arg = (Node *)linitial(targs);
    }

    if (!is_vertex_or_edge(arg)) {
        return NULL;
    }

    entity_type = get_entity_record_type(arg);
    get_record_field_info(func_name, entity_type, &fieldnum, &fieldtype);
    if (fieldnum == InvalidAttrNumber || fieldtype == InvalidOid) {
        ereport(ERROR,
                (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                 errmsg("%s() is not valid for this entity type", func_name),
                 parser_errposition(pstate, fn->location)));
    }

    field = extract_field_from_record(arg, func_name);
    if (is_node_func) {
        Oid func_oid;
        Const *graph_name;

        graph_name = makeConst(TEXTOID, -1, InvalidOid, -1,
                               CStringGetTextDatum(cpstate->graph_name), false,
                               false);
        func_oid = get_ag_func_oid("_get_vertex_by_graphid",
                                   AGTYPE_IN_OPERATOR_ARGUMENT_COUNT, TEXTOID,
                                   GRAPHIDOID);
        return (Node *)makeFuncExpr(func_oid, AGTYPEOID,
                                    list_make2(graph_name, field),
                                    InvalidOid, InvalidOid,
                                    COERCE_EXPLICIT_CALL);
    }

    return field;
}

/*
 * Code borrowed from PG's transformFuncCall and updated for AGE
 */
static Node *transform_FuncCall(cypher_parsestate *cpstate, FuncCall *fn)
{
    ParseState *pstate = &cpstate->pstate;
    Node *last_srf = pstate->p_last_srf;
    List *targs = NIL;
    List *fname = NIL;
    ListCell *arg;
    Node *retval = NULL;
    bool is_accessor = false;

    if (list_length(fn->funcname) == 1)
    {
        is_accessor = is_accessor_function(
            ((Value *)linitial(fn->funcname))->val.str);
    }

    if (list_length(fn->funcname) == 1 && list_length(fn->args) == 1)
    {
        char *name = ((Value *)linitial(fn->funcname))->val.str;
        Node *raw_arg = (Node *)linitial(fn->args);

        if ((pg_strcasecmp(name, "all") == 0 ||
             pg_strcasecmp(name, "any") == 0 ||
             pg_strcasecmp(name, "none") == 0 ||
             pg_strcasecmp(name, "single") == 0) &&
            is_ag_node(raw_arg, cypher_predicate_function))
        {
            return transform_cypher_predicate_function_expr(cpstate,
                (cypher_predicate_function *)raw_arg);
        }
    }

    /* Transform the list of arguments ... */
    foreach(arg, fn->args)
    {
        Node *farg = NULL;

        farg = (Node *)lfirst(arg);
        targs = lappend(targs, transform_cypher_expr_recurse(cpstate, farg));
    }

    /* within group should not happen */
    Assert(!fn->agg_within_group);

    if (list_length(fn->funcname) == 1)
    {
        char *name = ((Value*)linitial(fn->funcname))->val.str;
        char *ag_name = construct_age_function_name(name);
        Form_pg_proc procform = NULL;
        Node *optimized = NULL;

        /*
         * Cypher's avg/sum/etc. normally map to AGE's agtype aggregates, but
         * external datavec values already have concrete PostgreSQL types after
         * an explicit cast. Prefer pg_catalog overloads such as avg(vector) or
         * sum(halfvec) when they exist, and fall back to the AGE aggregate for
         * ordinary agtype expressions.
         */
        if (has_external_vector_arg(targs) &&
            (procform = get_procform(fn, targs, false)) != NULL)
        {
            Oid pg_catalog_oid = get_namespace_oid("pg_catalog", false);
            if (procform->pronamespace == pg_catalog_oid)
                return transform_external_ext_FuncCall(cpstate, fn, targs,
                                                       procform);
        }

        if (function_exists(ag_name, "age"))
        {
            fname = list_make2(makeString("ag_catalog"), makeString(ag_name));

            if ((list_length(targs) != 0) &&
                ((pg_strcasecmp("startNode", name) == 0 ||
                  pg_strcasecmp("endNode", name) == 0 ||
                  pg_strcasecmp("vle", name) == 0) ||
                  pg_strcasecmp("vertex_stats", name) == 0 ||
                  pg_strcasecmp("shortest_path", name) == 0 ||
                  pg_strcasecmp("all_shortest_paths", name) == 0)) {
                char *graph_name = cpstate->graph_name;
                Datum d = string_to_agtype(graph_name);
                Const *c = makeConst(AGTYPEOID, -1, InvalidOid, -1, d, false,
                                     false);

                targs = lcons(c, targs);
            }

            /* accessor functions may collapse into a direct field access */
            optimized = is_accessor
                ? optimize_accessor_function(cpstate, name, fn, targs)
                : NULL;
            if (optimized != NULL) {
                return optimized;
            }
        } else if (function_exists(name, NULL)) {
            Form_pg_proc procform = get_procform(fn, targs, true);

            return transform_external_ext_FuncCall(cpstate, fn, targs,
                                                   procform);
        } else {
            ereport(ERROR,
                    (errcode(ERRCODE_UNDEFINED_FUNCTION),
                     errmsg("function %s does not exist", name),
                     errhint("If the function is from an external extension, "
                             "make sure the extension is installed and the "
                             "function is in the search path.")));
        }
    } else {
        fname = fn->funcname;
    }

    retval = ParseFuncOrColumn(pstate, fname, targs, last_srf, fn,
                               fn->location, false);
    if (retval != NULL && retval->type == T_Aggref)
    {
        cpstate->exprHasAgg = true;
    }

    return retval;
}

/*
 * Code borrowed from PG's transformCoalesceExpr and updated for AGE
 */
static Node *transform_CoalesceExpr(cypher_parsestate *cpstate, CoalesceExpr
                                    *cexpr)
{
    ParseState *pstate = &cpstate->pstate;
    CoalesceExpr *newcexpr = makeNode(CoalesceExpr);
    List *newargs = NIL;
    List *newcoercedargs = NIL;
    ListCell *args;

    foreach(args, cexpr->args)
    {
        Node *e = (Node *)lfirst(args);
        Node *newe;

        newe = transform_cypher_expr_recurse(cpstate, e);
        newargs = lappend(newargs, newe);
    }

    newcexpr->coalescetype = select_common_type(pstate, newargs, "COALESCE",
                                                NULL);
    /* coalescecollid will be set by parse_collate.c */

    /* Convert arguments if necessary */
    foreach(args, newargs)
    {
        Node *e = (Node *)lfirst(args);
        Node *newe;

        newe = coerce_to_common_type(pstate, e, newcexpr->coalescetype,
                                     "COALESCE");
        newcoercedargs = lappend(newcoercedargs, newe);
    }
    /* if any subexpression contained a SRF, complain */

    newcexpr->args = newcoercedargs;
    newcexpr->location = cexpr->location;
    return (Node *) newcexpr;
}

/*
 * Code borrowed from PG's transformCaseExpr and updated for AGE
 */
static Node *transform_CaseExpr(cypher_parsestate *cpstate,CaseExpr
                                *cexpr)
{
    ParseState *pstate = &cpstate->pstate;
    CaseExpr   *newcexpr = makeNode(CaseExpr);
    Node       *arg;
    CaseTestExpr *placeholder;
    List       *newargs;
    List       *resultexprs;
    ListCell   *l;
    Node       *defresult;
    Oid         ptype;

    /* If we already transformed this node, do nothing */
    if (OidIsValid(cexpr->casetype)) {
        return (Node*)cexpr;
    }
    bool saved_is_case_when = pstate->p_is_decode;
    pstate->p_is_decode = true;

    /*
     * A chained comparison used as the CASE test expression must become an
     * agtype boolean before the placeholder type is derived. openGauss's
     * DECODE-style CASE otherwise resolves the boolean placeholder against
     * the agtype WHEN values inconsistently and no branch ever matches.
     */
    if (cexpr->arg != NULL &&
        (is_ag_node((Node *) cexpr->arg, cypher_comparison_aexpr) ||
         is_ag_node((Node *) cexpr->arg, cypher_comparison_boolexpr)))
    {
        List *funcname = list_make1(makeString("ag_catalog"));

        funcname = lappend(funcname, makeString("bool_to_agtype"));
        cexpr->arg = (Expr *) makeFuncCall(funcname,
                                           list_make1(cexpr->arg),
                                           cexpr->location);
    }

    /* transform the test expression, if any */
    arg = transform_cypher_expr_recurse(cpstate, (Node *) cexpr->arg);
    /* generate placeholder for test expression */
    if (arg)
    {
        Oid argtype = exprType(arg);
        if (argtype == UNKNOWNOID)
        {
            arg = coerce_to_common_type(pstate, arg, TEXTOID, "CASE");
        } else if (argtype == VERTEXOID || argtype == EDGEOID) {
            arg = coerce_entity_to_agtype(pstate, arg);
        }
        assign_expr_collations(pstate, arg);

        placeholder = makeNode(CaseTestExpr);
        placeholder->typeId = exprType(arg);
        placeholder->typeMod = exprTypmod(arg);
        placeholder->collation = exprCollation(arg);
    }
    else
    {
        placeholder = NULL;
    }

    newcexpr->arg = (Expr *) arg;

    /* transform the list of arguments */
    newargs = NIL;
    resultexprs = NIL;
    foreach(l, cexpr->args)
    {
        CaseWhen   *w = lfirst_node(CaseWhen, l);
        CaseWhen   *neww = makeNode(CaseWhen);
        Node       *warg;

        warg = (Node *) w->expr;
        if (placeholder)
        {
            /*
             * A chained comparison used as the WHEN value must be compared
             * as an agtype boolean, not merged into the comparison chain.
             */
            if (is_ag_node(warg, cypher_comparison_aexpr) ||
                is_ag_node(warg, cypher_comparison_boolexpr)) {
                List *funcname = list_make1(makeString("ag_catalog"));

                funcname = lappend(funcname, makeString("bool_to_agtype"));
                warg = (Node *) makeFuncCall(funcname, list_make1(warg),
                                             cexpr->location);
            }

            /* shorthand form was specified, so expand... */
            warg = (Node *) makeSimpleA_Expr(AEXPR_OP, "=",
                                             (Node *) placeholder,
                                             warg,
                                             w->location);
        }
        neww->expr = (Expr *) transform_cypher_expr_recurse(cpstate, warg);

        neww->expr = (Expr *) coerce_cypher_expr_to_boolean(pstate,
                                                            (Node *)neww->expr,
                                                            "CASE/WHEN");

        warg = (Node *) w->result;

        /* THEN results that are chained comparisons become agtype booleans */
        if (is_ag_node(warg, cypher_comparison_aexpr) ||
            is_ag_node(warg, cypher_comparison_boolexpr))
        {
            List *funcname = list_make1(makeString("ag_catalog"));

            funcname = lappend(funcname, makeString("bool_to_agtype"));
            warg = (Node *) makeFuncCall(funcname, list_make1(warg),
                                         cexpr->location);
        }

        neww->result = (Expr *) transform_cypher_expr_recurse(cpstate, warg);
        neww->location = w->location;

        newargs = lappend(newargs, neww);
        resultexprs = lappend(resultexprs, neww->result);
    }

    newcexpr->args = newargs;

    /* transform the default clause */
    defresult = (Node *) cexpr->defresult;
    if (defresult == NULL)
    {
        A_Const    *n = makeNode(A_Const);

        n->val.type = T_Null;
        n->location = -1;
        defresult = (Node *) n;
    }
    newcexpr->defresult = (Expr *) transform_cypher_expr_recurse(cpstate, defresult);

    resultexprs = lcons(newcexpr->defresult, resultexprs);

    /*
     * Cypher CASE results are usually agtype, but predicate subexpressions can
     * still be plain SQL boolean. Let AGE-specific boolean/record mixes choose
     * a concrete common type instead of erroring out in select_common_type().
     */
    ptype = select_common_type(pstate, resultexprs, NULL, NULL);
    if (ptype == InvalidOid)
    {
        ListCell *result_lc;
        bool has_entity = false;

        foreach(result_lc, resultexprs)
        {
            Oid result_type = exprType((Node *)lfirst(result_lc));
            if (result_type == VERTEXOID || result_type == EDGEOID) {
                has_entity = true;
                break;
            }
        }

        ptype = has_entity ? AGTYPEOID : BOOLOID;
    }

    Assert(OidIsValid(ptype));
    newcexpr->casetype = ptype;
    /* casecollid will be set by parse_collate.c */

    /* Convert default result clause, if necessary */
    newcexpr->defresult = (Expr *)
        coerce_to_common_type(pstate,
                              (Node *) newcexpr->defresult,
                              ptype,
                              "CASE/ELSE");

    /* Convert when-clause results, if necessary */
    foreach(l, newcexpr->args)
    {
        CaseWhen   *w = (CaseWhen *) lfirst(l);

        w->result = (Expr *)
            coerce_to_common_type(pstate,
                                  (Node *) w->result,
                                  ptype,
                                  "CASE/WHEN");
    }

    /* if any subexpression contained a SRF, complain */

    newcexpr->location = cexpr->location;
    pstate->p_is_decode = saved_is_case_when;
    return (Node *) newcexpr;
}

/* from PG's transformSubLink but reduced and hooked into our parser */
static Node *transform_SubLink(cypher_parsestate *cpstate, SubLink *sublink)
{
    Node *result = (Node*)sublink;
    Query *qtree;
    ParseState *pstate = (ParseState*)cpstate;
    const char *err = NULL;
    /*
     * Check to see if the sublink is in an invalid place within the query. We
     * allow sublinks everywhere in SELECT/INSERT/UPDATE/DELETE, but generally
     * not in utility statements.
     */
    switch (pstate->p_expr_kind)
    {
        case EXPR_KIND_NONE:
            Assert(false);          /* can't happen */
            break;
        case EXPR_KIND_OTHER:
            /* Accept sublink here; caller must throw error if wanted */

            break;
        case EXPR_KIND_SELECT_TARGET:
        case EXPR_KIND_FROM_SUBSELECT:
        case EXPR_KIND_WHERE:
            /* okay */
            break;
        default:
            ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                            errmsg_internal("unsupported SubLink"),
                            parser_errposition(pstate, sublink->location)));
    }
    if (err)
        ereport(ERROR,
                (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                 errmsg_internal("%s", err),
                 parser_errposition(pstate, sublink->location)));

    pstate->p_hasSubLinks = true;

    /*
     * OK, let's transform the sub-SELECT.
     */
    qtree = cypher_parse_sub_analyze(sublink->subselect, cpstate, NULL, false,
                                     true);

    /*
     * Check that we got a SELECT.  Anything else should be impossible given
     * restrictions of the grammar, but check anyway.
     */
    if (!IsA(qtree, Query) || qtree->commandType != CMD_SELECT)
        elog(ERROR, "unexpected non-SELECT command in SubLink");

    sublink->subselect = (Node *)qtree;

    if (sublink->subLinkType == EXISTS_SUBLINK ||
        sublink->subLinkType == EXPR_SUBLINK)
    {
        sublink->testexpr = NULL;
        sublink->operName = NIL;
    }
    else
        elog(ERROR, "unsupported SubLink type");

    return result;
}

Node *coerce_entity_to_agtype(ParseState *pstate, Node *node)
{
    Oid node_type;
    Node *coerced;

    if (node == NULL) {
        return NULL;
    }

    node_type = exprType(node);
    if (node_type != VERTEXOID && node_type != EDGEOID) {
        return node;
    }

    coerced = coerce_to_target_type(pstate, node, node_type, AGTYPEOID, -1,
                                    COERCION_EXPLICIT,
                                    COERCE_IMPLICIT_CAST, NULL, NULL, -1);
    if (coerced == NULL) {
        ereport(ERROR,
                (errcode(ERRCODE_CANNOT_COERCE),
                 errmsg("cannot convert graph entity to agtype")));
    }

    return coerced;
}

void coerce_target_entities_to_agtype(ParseState *pstate, List *target_list)
{
    ListCell *lc;

    foreach (lc, target_list)
    {
        TargetEntry *te = (TargetEntry *)lfirst(lc);

        te->expr = (Expr *)coerce_entity_to_agtype(pstate,
                                                   (Node *)te->expr);
    }
}
