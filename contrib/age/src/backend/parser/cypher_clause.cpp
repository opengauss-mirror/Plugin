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

#include "access/sysattr.h"
#include "catalog/pg_type.h"
#include "catalog/pg_proc.h"
#include "catalog/pg_inherits_fn.h"
#include "miscadmin.h"
#include "nodes/makefuncs.h"
#include "nodes/nodeFuncs.h"
#include "nodes/nodes.h"
#include "nodes/parsenodes.h"
#include "nodes/pg_list.h"
#include "nodes/primnodes.h"
#include "nodes/ag_extensible.h"

#include "optimizer/var.h"
#include "optimizer/clauses.h"
#include "parser/parse_clause.h"
#include "parser/parse_coerce.h"
#include "parser/parse_collate.h"
#include "parser/parse_expr.h"
#include "parser/ag_parse_expr.h"
#include "parser/parse_func.h"
#include "parser/parse_node.h"
#include "parser/parse_oper.h"
#include "parser/parse_relation.h"
#include "parser/analyze.h"
#include "parser/parse_target.h"
#include "parser/parsetree.h"
#include "rewrite/rewriteHandler.h"
#include "rewrite/rewriteManip.h"
#include "utils/typcache.h"
#include "utils/lsyscache.h"
#include "utils/builtins.h"
#include "utils/rel.h"

#include "catalog/ag_graph.h"
#include "catalog/ag_label.h"
#include "commands/label_commands.h"
#include "nodes/ag_nodes.h"
#include "nodes/cypher_nodes.h"
#include "parser/cypher_clause.h"
#include "parser/cypher_expr.h"
#include "parser/cypher_item.h"
#include "parser/cypher_parse_agg.h"
#include "parser/cypher_parse_node.h"
#include "utils/ag_cache.h"
#include "utils/ag_func.h"
#include "utils/ag_guc.h"
#include "utils/agtype.h"
#include "utils/graphid.h"

#define VLE_FUNCTION_MIN_ARGUMENTS 5
#define VLE_LOWER_BOUND_ARGUMENT_INDEX 3
#define PATH_NODE_EDGE_POSITION_MODULUS 2
#define OPTIONAL_PATH_SECURITY_BARRIER_MIN_ELEMENTS 7
#define REDUCE_ACCUMULATOR_ATTRIBUTE_NUMBER 1
#define REDUCE_ELEMENT_ATTRIBUTE_NUMBER 2
#define REDUCE_CAPTURE_FIRST_SLOT 2
#define REDUCE_ORDINALITY_TARGET_ATTRIBUTE_NUMBER 5
#define REDUCE_AGGREGATE_ARGUMENT_COUNT 4
#define REDUCE_INIT_ARGUMENT_INDEX 0
#define REDUCE_BODY_ARGUMENT_INDEX 1
#define REDUCE_ELEMENT_ARGUMENT_INDEX 2
#define REDUCE_EXTRAS_ARGUMENT_INDEX 3
#define REDUCE_INIT_TARGET_ENTRY_NUMBER 1
#define REDUCE_BODY_TARGET_ENTRY_NUMBER 2
#define REDUCE_ELEMENT_TARGET_ENTRY_NUMBER 3
#define REDUCE_EXTRAS_TARGET_ENTRY_NUMBER 4
#define EDGE_UNIQUENESS_MIN_EDGE_COUNT 2
#define EDGE_UNIQUENESS_MAX_EDGE_COUNT 4
#define MAX_PARENT_PARSE_LEVEL 2
#define VERTEX_ID_FIELD_NUMBER 1
#define VERTEX_LABEL_FIELD_NUMBER 2
#define VERTEX_PROPERTIES_FIELD_NUMBER 3
#define EDGE_LABEL_FIELD_NUMBER 2
#define EDGE_END_ID_FIELD_NUMBER 3
#define EDGE_START_ID_FIELD_NUMBER 4
#define EDGE_PROPERTIES_FIELD_NUMBER 5

/*
 * Variable string names for makeTargetEntry. As they are going to be variable
 * names that will be hidden from the user, we need to do our best to make sure
 * they won't be picked by mistake. Additionally, their form needs to be easily
 * determined as ours. For now, prefix them as follows -
 *
 *     #define AGE_VARNAME_SOMETHING AGE_DEFAULT_VARNAME_PREFIX"something"
 *
 * We should probably make an automated variable generator, like for aliases,
 * for this.
 *
 * Also, keep these here as nothing outside of this file needs to know these.
 */
#define AGE_VARNAME_CREATE_CLAUSE AGE_DEFAULT_VARNAME_PREFIX"create_clause"
#define AGE_VARNAME_VLE_CLAUSE AGE_DEFAULT_VARNAME_PREFIX"vle_clause"
#define AGE_VARNAME_CREATE_NULL_VALUE AGE_DEFAULT_VARNAME_PREFIX"create_null_value"
#define AGE_VARNAME_DELETE_CLAUSE AGE_DEFAULT_VARNAME_PREFIX"delete_clause"
#define AGE_VARNAME_MERGE_CLAUSE AGE_DEFAULT_VARNAME_PREFIX"merge_clause"
#define AGE_VARNAME_ID AGE_DEFAULT_VARNAME_PREFIX"id"
#define AGE_VARNAME_SET_CLAUSE AGE_DEFAULT_VARNAME_PREFIX"set_clause"
#define AGE_VARNAME_SET_VALUE AGE_DEFAULT_VARNAME_PREFIX"set_value"

#define VLE_LEFT_ALIAS			"l"
#define VLE_VERTEX_ALIAS		"vtx"
#define VLE_COLNAME_IDS			"ids"
#define VLE_COLNAME_EDGES		"edges"
#define VLE_COLNAME_VERTICES	"vertices"
#define VLE_COLNAME_NEXT		"next"
#define VLE_COLNAME_EDGE		"edge"
#define VLE_COLNAME_VERTEX		"vertex"

/*
 * Rules to determine if a node must be included:
 *
 *      1. the node is in a path variable
 *      2. the node is a variable
 *      3. the node contains filter properties
 */
#define INCLUDE_NODE_IN_JOIN_TREE(path, node) \
    (path->var_name || node->name || node->props)

typedef Query *(*transform_method)(cypher_parsestate *cpstate,
                                   cypher_clause *clause);

// projection
static Query *transform_cypher_return(cypher_parsestate *cpstate,
                                      cypher_clause *clause);
static List *transform_cypher_distinct_clause(cypher_parsestate *cpstate,
                                              List **target_list,
                                              List *sort_clause);
static List *transform_cypher_order_by(cypher_parsestate *cpstate,
                                       List *sort_items, List **target_list,
                                       ParseExprKind expr_kind);
static TargetEntry *find_target_list_entry(cypher_parsestate *cpstate,
                                           Node *node, List **target_list,
                                           ParseExprKind expr_kind);
static Node *transform_cypher_limit(cypher_parsestate *cpstate, Node *node,
                                    ParseExprKind expr_kind,
                                    const char *construct_name);
static Query *transform_cypher_with(cypher_parsestate *cpstate,
                                    cypher_clause *clause);
static Query *transform_cypher_clause_with_where(cypher_parsestate *cpstate,
                                                 transform_method transform,
                                                 cypher_clause *clause,
                                                 Node *where);
// match clause
static Query *transform_cypher_match(cypher_parsestate *cpstate,
                                     cypher_clause *clause);
static Query *transform_cypher_match_pattern(cypher_parsestate *cpstate,
                                             cypher_clause *clause);
static List *transform_match_entities(cypher_parsestate *cpstate, Query *query,
                                      cypher_path *path);
static void transform_match_pattern(cypher_parsestate *cpstate, Query *query,
                                    List *pattern,Node *where);
static List *transform_match_path(cypher_parsestate *cpstate, Query *query,
                                  cypher_path *path);
static Expr *transform_cypher_edge(cypher_parsestate *cpstate,
                                   cypher_relationship *rel,
                                   List **target_list, bool valid_label);
static Expr *transform_cypher_node(cypher_parsestate *cpstate,
                                   cypher_node *node, List **target_list,
                                   bool output_node, bool valid_label);
static bool is_zero_lower_bound_vle(Node *varlen);
static bool match_check_valid_label(cypher_match *match,
                                    cypher_parsestate *cpstate);
static bool path_check_valid_label(cypher_path *path,
                                   cypher_parsestate *cpstate);
static Node *make_vertex_expr(cypher_parsestate *cpstate, RangeTblEntry *rte);
static Node *make_edge_expr(cypher_parsestate *cpstate, RangeTblEntry *rte);
static Node *make_qual(cypher_parsestate *cpstate,
                           transform_entity *entity, char *name);
static TargetEntry* transform_match_create_path_variable(cypher_parsestate *cpstate,
                                                         cypher_path *path,
                                                         List *entities);
static List *make_path_join_quals(cypher_parsestate *cpstate, List *entities);
static List *make_directed_edge_join_conditions(cypher_parsestate *cpstate,
                                                transform_entity *prev_entity,
                                                transform_entity *next_entity,
                                                Node *prev_qual,
                                                Node *next_qual,
                                                char *prev_node_label,
                                                char *next_node_label);
static List *join_to_entity(cypher_parsestate *cpstate,
                            transform_entity *entity, Node *qual,
                            enum transform_entity_join_side side);
static Expr *make_undirected_endpoint_prefilter(cypher_parsestate *cpstate,
                                                transform_entity *entity,
                                                Node *start_id_expr,
                                                Node *end_id_expr);
static List *make_join_condition_for_edge(cypher_parsestate *cpstate,
                                          transform_entity *prev_edge,
                                          transform_entity *prev_node,
                                          transform_entity *entity,
                                          transform_entity *next_node,
                                          transform_entity *next_edge);
static List *make_edge_quals(cypher_parsestate *cpstate,
                             transform_entity *edge,
                             enum transform_entity_join_side side);
static A_Expr *filter_vertices_on_label_id(cypher_parsestate *cpstate,
                                           Node *id_field, char *label);
static transform_entity* make_transform_entity(cypher_parsestate *cpstate,
                                               enum transform_entity_type type,
                                               Node *node, Expr *expr);
static transform_entity *find_parent_variable(cypher_parsestate *cpstate,
                                              char *name, Index *levelsup);
static bool has_list_comp_or_subquery(Node *expr);
static bool has_list_comp_or_subquery_walker(Node *expr, void *context);
static Node *coerce_cypher_expr_to_boolean(ParseState *pstate, Node *expr,
                                           const char *construct_name);

static bool is_nonempty_static_cypher_map(Node *property_constraints);
static List *create_property_constraint_quals(cypher_parsestate *cpstate,
                                              transform_entity *entity,
                                              cypher_map *property_map,
                                              List *parent_fields);
static Node *create_property_constraint_function(cypher_parsestate *cpstate,
                                                 transform_entity *entity,
                                                 Node *property_constraints,
                                                 Node *prop_expr);
static TargetEntry *findTarget(List *targetList, char *resname);
// create clause
static Query *transform_cypher_create(cypher_parsestate *cpstate,
                                      cypher_clause *clause);
static List *transform_cypher_create_pattern(cypher_parsestate *cpstate,
                                             Query *query, List *pattern);
static cypher_create_path* transform_cypher_create_path(cypher_parsestate *cpstate,
                                                        List **target_list,
                                                        cypher_path *cp);
static cypher_target_node* transform_create_cypher_node(cypher_parsestate *cpstate,
                                                        List **target_list,
                                                        cypher_node *node,
                                                        bool has_edge);
static cypher_target_node* transform_create_cypher_new_node(cypher_parsestate *cpstate,
                                                            List **target_list, cypher_node *node);
static cypher_target_node *transform_create_cypher_existing_node(cypher_parsestate *cpstate,
                                                                 List **target_list, bool declared_in_current_clause,
                                                                 cypher_node *node);
static cypher_target_node* transform_create_cypher_edge(cypher_parsestate *cpstate, List **target_list,
                                                        cypher_relationship *edge);
static Expr *cypher_create_properties(cypher_parsestate *cpstate,
                                      cypher_target_node *rel,
                                      Relation label_relation, Node *props,
                                      enum transform_entity_type type);
static Expr *add_volatile_wrapper(ParseState *pstate, Expr *node);
static bool variable_exists(cypher_parsestate *cpstate, char *name);
static int get_target_entry_resno(ParseState *pstate, List *target_list,
                                  char *name);
static void handle_prev_clause(cypher_parsestate *cpstate, Query *query,
                               cypher_clause *clause, bool first_rte);
static TargetEntry *placeholder_target_entry(cypher_parsestate *cpstate,
                                             char *name);
static Query *transform_cypher_sub_pattern(cypher_parsestate *cpstate,
                                           cypher_clause *clause);
static Query *transform_cypher_sub_query(cypher_parsestate *cpstate,
                                         cypher_clause *clause);
// set and remove clause
static Query *transform_cypher_set(cypher_parsestate *cpstate,
                                   cypher_clause *clause);
static cypher_update_information *transform_cypher_set_item_list(cypher_parsestate *cpstate,
                                                                 List *set_item_list,
                                                                 Query *query);
static cypher_update_information *transform_cypher_remove_item_list(cypher_parsestate *cpstate,
                                                                    List *remove_item_list,
                                                                    Query *query);
// delete
static Query *transform_cypher_delete(cypher_parsestate *cpstate,
                                      cypher_clause *clause);
static List *transform_cypher_delete_item_list(cypher_parsestate *cpstate,
                                               List *delete_item_list,
                                               Query *query);
//set operators
static Query *transform_cypher_union(cypher_parsestate *cpstate,
                                     cypher_clause *clause);

static Node * transform_cypher_union_tree(cypher_parsestate *cpstate,
                                          cypher_clause *clause,
                                          bool isTopLevel,
                                          List **targetlist);

Query *cypher_parse_sub_analyze_union(cypher_clause *clause,
                                      cypher_parsestate *cpstate,
                                      CommonTableExpr *parentCTE,
                                      bool locked_from_parent,
                                      bool resolve_unknowns);
static void get_res_cols(ParseState *pstate, RangeTblEntry *l_rte,
                         RangeTblEntry *r_rte, List **res_colnames,
                         List **res_colvars);
// unwind
static Query *transform_cypher_unwind(cypher_parsestate *cpstate,
                                      cypher_clause *clause);
// call
static Query *transform_cypher_call_stmt(cypher_parsestate *cpstate,
                                         cypher_clause *clause);
static Query *transform_cypher_call_subquery(cypher_parsestate *cpstate,
                                             cypher_clause *clause);
// list comprehension
static Query *transform_cypher_list_comprehension(cypher_parsestate *cpstate,
                                                  cypher_clause *clause);
// predicate functions
static Query *transform_cypher_predicate_function(cypher_parsestate *cpstate,
                                                  cypher_clause *clause);
// reduce
static Query *transform_cypher_reduce(cypher_parsestate *cpstate,
                                      cypher_clause *clause);
static Node *make_boolean_test(Node *arg, BoolTestType booltesttype);
static Node *make_bool_or_agg(ParseState *pstate, Node *arg);
static Node *make_count_star_filter_agg(ParseState *pstate, Node *filter);
static Node *make_predicate_case_expr(ParseState *pstate, Node *pred,
                                      cypher_predicate_function_kind kind);
// merge
static Query *transform_cypher_merge(cypher_parsestate *cpstate,
                                     cypher_clause *clause);
static cypher_create_path* transform_merge_make_lateral_join(cypher_parsestate *cpstate, Query *query,
                                                             cypher_clause *clause,
                                                             cypher_clause *isolated_merge_clause);
static cypher_create_path* transform_cypher_merge_path(cypher_parsestate *cpstate, List **target_list,
                                                       cypher_path *path);
static cypher_target_node* transform_merge_cypher_edge(cypher_parsestate *cpstate, List **target_list,
                                                       cypher_relationship *edge);
static cypher_target_node* transform_merge_cypher_node(cypher_parsestate *cpstate, List **target_list,
                                                       cypher_node *node,
                                                       bool has_edge);
static Node *transform_clause_for_join(cypher_parsestate *cpstate,
                                       cypher_clause *clause,
                                       RangeTblEntry **rte,
                                       ParseNamespaceItem **nsitem,
                                       Alias* alias);
static cypher_clause *convert_merge_to_match(cypher_merge *merge);
static void transform_cypher_merge_mark_tuple_position(
    ParseState *pstate, List *target_list, cypher_create_path *path);

static List *makeTargetListFromRTE(ParseState *pstate, RangeTblEntry *rte);
// transform
#define PREV_CYPHER_CLAUSE_ALIAS AGE_DEFAULT_ALIAS_PREFIX"previous_cypher_clause"
#define CYPHER_OPT_RIGHT_ALIAS AGE_DEFAULT_ALIAS_PREFIX"cypher_optional_right"
#define transform_prev_cypher_clause(cpstate, prev_clause, add_rte_to_query) \
    transform_cypher_clause_as_subquery(cpstate, transform_cypher_clause, \
                                        prev_clause, NULL, add_rte_to_query)
static char *get_next_default_alias(cypher_parsestate *cpstate);

static RangeTblEntry *transform_cypher_clause_as_subquery(cypher_parsestate *cpstate,
                                                          transform_method transform,
                                                          cypher_clause *clause,
                                                          Alias *alias,
                                                          bool add_rte_to_query);
static Query *analyze_cypher_clause(transform_method transform,
                                    cypher_clause *clause,
                                    cypher_parsestate *parent_cpstate);
static List *transform_group_clause(cypher_parsestate *cpstate,
                                    List *grouplist, List **groupingSets,
                                    List **targetlist, List *sortClause,
                                    ParseExprKind exprKind);
static Node *flatten_grouping_sets(Node *expr, bool toplevel,
                                   bool *hasGroupingSets);
static Index transform_group_clause_expr(List **flatresult,
                                         Bitmapset *seen_local,
                                         cypher_parsestate *cpstate,
                                         Node *gexpr, List **targetlist,
                                         List *sortClause,
                                         ParseExprKind exprKind,
                                         bool toplevel);
static List *add_target_to_group_list(cypher_parsestate *cpstate,
                                      TargetEntry *tle, List *grouplist,
                                      List *targetlist, int location);
static void advance_transform_entities_to_next_clause(List *entities);

static ParseNamespaceItem *create_namespace_item(RangeTblEntry *rte, bool p_rel_visible,
                                             bool p_cols_visible, bool p_lateral_only,
                                             bool p_lateral_ok);
static List *make_target_list_from_join(ParseState *pstate,
                                    RangeTblEntry *rte);
static Expr *add_volatile_wrapper(ParseState *pstate, Expr *node);
static FuncExpr *make_clause_func_expr(char *function_name,
                                       Node *clause_information);
static char *get_entity_name(transform_entity *entity);
/* for VLE support */
static bool isa_special_VLE_case(cypher_path *path);
static bool clause_is_dml(cypher_clause *clause);
static bool clause_chain_has_dml(cypher_clause *clause);
static Node *make_false_where_clause(bool volatile_needed);

/* new logic for VLE */
static RangeTblEntry *
transformVLEtoRTE(cypher_parsestate *pstate, SelectStmt *vle, Alias *alias);
static Node *genVLEEdgeSubselect(cypher_parsestate *pstate, cypher_relationship *crel,
								 char *aliasname);
static Alias* makeAliasNoDup(char* aliasname, List* colnames);  
static ResTarget* makeSimpleResTarget(char* field, char* name) ;
static Node * genEdgeUnion(cypher_parsestate *pstate,char *edge_label, bool only, int location);
static char * getEdgeColname(cypher_relationship *crel, bool prev);                              
static SelectStmt *genVLESubselect(cypher_parsestate *pstate, cypher_relationship *crel,bool pathout);
static Node *genVLELeftChild(cypher_parsestate *pstate, cypher_relationship *crel,
							 bool out, bool pathout);
static char *get_accessor_function_name(enum transform_entity_type type,
                                        char *name);

/*
 * transform a cypher_clause
 */
Query *transform_cypher_clause(cypher_parsestate *cpstate,
                               cypher_clause *clause)
{
    Node *self = clause->self;
    Query *result;

    // examine the type of clause and call the transform logic for it
    if (is_ag_node(self, cypher_return))
    {
        result = transform_cypher_return(cpstate, clause);
    }
    else if (is_ag_node(self, cypher_with))
    {
        result = transform_cypher_with(cpstate, clause);
    }
    else if (is_ag_node(self, cypher_match))
    {
        result = transform_cypher_match(cpstate, clause);
    }
    else if (is_ag_node(self, cypher_create))
    {
        result = transform_cypher_create(cpstate, clause);
    }
    else if (is_ag_node(self, cypher_set))
    {
        result = transform_cypher_set(cpstate, clause);
    }
    else if (is_ag_node(self, cypher_delete))
    {
        result = transform_cypher_delete(cpstate, clause);
    }
    else if (is_ag_node(self, cypher_merge))
    {
        result = transform_cypher_merge(cpstate, clause);
    }
    else if (is_ag_node(self, cypher_sub_pattern))
    {
        result = transform_cypher_sub_pattern(cpstate, clause);
    }
    else if (is_ag_node(self, cypher_sub_query))
    {
        result = transform_cypher_sub_query(cpstate, clause);
    }
    else if (is_ag_node(self, cypher_union))
    {
        result = transform_cypher_union(cpstate, clause);
    }
    else if (is_ag_node(self, cypher_unwind))
    {
        result = transform_cypher_unwind(cpstate, clause);
    }
    else if (is_ag_node(self, cypher_call))
    {
        result = transform_cypher_call_stmt(cpstate, clause);
    }
    else if (is_ag_node(self, cypher_list_comprehension))
    {
        result = transform_cypher_list_comprehension(cpstate, clause);
    }
    else if (is_ag_node(self, cypher_predicate_function))
    {
        result = transform_cypher_predicate_function(cpstate, clause);
    }
    else if (is_ag_node(self, cypher_reduce))
    {
        result = transform_cypher_reduce(cpstate, clause);
    }else
    {
        ereport(ERROR, (errmsg_internal("unexpected Node for cypher_clause")));
        return NULL; /* suppress the static check warmings */
    }

    if (clause->next == NULL && clause_is_dml(clause))
    {
        coerce_target_entities_to_agtype((ParseState *)cpstate,
                                         result->targetList);
    }

    result->querySource = QSRC_ORIGINAL;
    result->canSetTag = true;

    return result;
}

/*
 * Transform the UNION operator/clause. Creates a cypher_union
 * node and the necessary information needed in the execution
 * phase
 */

static cypher_clause *make_cypher_clause(List *stmt)
{
    cypher_clause *clause;
    ListCell *lc;

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
        {
            clause->next = next;
        }
        clause = next;
    }
    return clause;
}

/*
 * transform_cypher_union -
 *    transforms a union tree, derived from postgresql's
 *    transformSetOperationStmt.A lot of the general logic is similar,
 *    with adjustments made for AGE.
 *
 * A union tree is just a return, but with UNION structure to it.
 * We must transform each leaf SELECT and build up a top-level Query
 * that contains the leaf SELECTs as subqueries in its rangetable.
 * The tree of unions is converted into the setOperations field of
 * the top-level Query.
 */

static Query *transform_cypher_union(cypher_parsestate *cpstate,
                                     cypher_clause *clause)
{
    ParseState *pstate = (ParseState *)cpstate;
    Query *qry = makeNode(Query);
    int leftmostRTI;
    Query *leftmostQuery;
    SetOperationStmt *cypher_union_statement;
    Node *skip = NULL; /* equivalent to postgres limitOffset */
    Node *limit = NULL; /* equivalent to postgres limitCount */
    Node *node;

    ListCell *left_tlist, *lct, *lcm, *lcc;
    List *sv_relnamesapce;
    List *sv_varnamespace;
    List *targetvars, *targetnames;

    int sv_rtable_length;
    RangeTblEntry *jrte;
    int tllen;

    qry->commandType = CMD_SELECT;

    /*
     * Union is a node that should never have a previous node because
     * of where it is used in the parse logic. The query parts around it
     * are children located in larg or rarg. Something went wrong if the
     * previous clause field is not null.
     */
    if (clause->prev)
    {
        ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                        errmsg("Union is a parent node, there are no previous"),
                        parser_errposition(&cpstate->pstate, 0)));
    }

    /*
     * Recursively transform the components of the tree.
     */
    cypher_union_statement = (SetOperationStmt *) transform_cypher_union_tree(cpstate,
                                                                clause, true, NULL);

    Assert(cypher_union_statement);
    qry->setOperations = (Node *) cypher_union_statement;

    /*
     * Re-find leftmost return (now it's a sub-query in rangetable)
     */
    node = cypher_union_statement->larg;
    while (node && IsA(node, SetOperationStmt))
    {
        node = ((SetOperationStmt *) cypher_union_statement)->larg;
    }
    Assert(node && IsA(node, RangeTblRef));
    leftmostRTI = ((RangeTblRef *) node)->rtindex;
    leftmostQuery = rt_fetch(leftmostRTI, pstate->p_rtable)->subquery;
    Assert(leftmostQuery != NULL);

    /*
     * Generate dummy targetlist for outer query using column names of
     * leftmost return and common datatypes/collations of topmost set
     * operation.  Also make lists of the dummy vars and their names for use
     * in parsing ORDER BY.
     *
     * Note: we use leftmostRTI as the varno of the dummy variables. It
     * shouldn't matter too much which RT index they have, as long as they
     * have one that corresponds to a real RT entry; else funny things may
     * happen when the tree is mashed by rule rewriting.
     */
    qry->targetList = NIL;
    targetvars = NIL;
    targetnames = NIL;
    left_tlist = list_head(leftmostQuery->targetList);

    forthree(lct, cypher_union_statement->colTypes,
             lcm, cypher_union_statement->colTypmods,
             lcc, cypher_union_statement->colCollations)
    {
        Oid colType = lfirst_oid(lct);
        int32 colTypmod = lfirst_int(lcm);
        Oid colCollation = lfirst_oid(lcc);
        TargetEntry *lefttle = (TargetEntry *) lfirst(left_tlist);
        char *colName;
        TargetEntry *tle;
        Var *var;

        Assert(!lefttle->resjunk);
        colName = pstrdup(lefttle->resname);
        var = makeVar(leftmostRTI,
                      lefttle->resno,
                      colType,
                      colTypmod,
                      colCollation,
                      0);
        var->location = exprLocation((Node *) lefttle->expr);
        tle = makeTargetEntry((Expr *) var,
                              (AttrNumber) pstate->p_next_resno++,
                               colName,
                               false);
        qry->targetList = lappend(qry->targetList, tle);
        targetvars = lappend(targetvars, var);
        targetnames = lappend(targetnames, makeString(colName));
        left_tlist = lnext(left_tlist);
    }

    /*
     * As a first step towards supporting sort clauses that are expressions
     * using the output columns, generate a namespace entry that makes the
     * output columns visible.  A Join RTE node is handy for this, since we
     * can easily control the Vars generated upon matches.
     *
     * Note: we don't yet do anything useful with such cases, but at least
     * "ORDER BY upper(foo)" will draw the right error message rather than
     * "foo not found".
     */
    sv_rtable_length = list_length(pstate->p_rtable);

    jrte = addRangeTableEntryForJoin(pstate,
                                     targetnames,
                                     JOIN_INNER,
                                     targetvars,
                                     NULL,
                                     false);
    sv_relnamesapce=pstate->p_relnamespace;
    sv_varnamespace=pstate->p_varnamespace;
    pstate->p_relnamespace=NULL;
    pstate->p_varnamespace=NULL;
    /* add jrte to column namespace only */
    addRTEtoQuery(pstate, jrte, false, false, true);

    tllen = list_length(qry->targetList);
    /* restore namespace, remove jrte from rtable */
    pstate->p_relnamespace=sv_relnamesapce;
    pstate->p_varnamespace=sv_varnamespace;
    pstate->p_rtable = list_truncate(pstate->p_rtable, sv_rtable_length);

    if (tllen != list_length(qry->targetList))
    {
        ereport(ERROR,
            (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
             errmsg("invalid UNION ORDER BY clause"),
             errdetail("Only result column names can be used, not expressions or functions."),
             parser_errposition(pstate,
                                exprLocation((Node*)list_nth(qry->targetList, tllen)))));
    }

    qry->limitOffset = transform_cypher_limit(cpstate, skip,
                                              EXPR_KIND_OFFSET, "SKIP");
    qry->limitCount = transform_cypher_limit(cpstate, limit,
                                              EXPR_KIND_LIMIT, "LIMIT");

    qry->rtable = pstate->p_rtable;
    qry->jointree = makeFromExpr(pstate->p_joinlist, NULL);
    qry->hasAggs = pstate->p_hasAggs;

    assign_query_collations(pstate, qry);

    /* this must be done after collations, for reliable comparison of exprs */
    if (pstate->p_hasAggs ||
        qry->groupClause || qry->groupingSets || qry->havingQual)
    {
        parse_check_aggregates(pstate, qry);
    }

    return qry;

}

/*
 * transform_cypher_union_tree
 *      Recursively transform leaves and internal nodes of a set-op tree,
 *      derived from postgresql's transformSetOperationTree. A lot of
 *      the general logic is similar, with adjustments made for AGE.
 *
 * In addition to returning the transformed node, if targetlist isn't NULL
 * then we return a list of its non-resjunk TargetEntry nodes.  For a leaf
 * set-op node these are the actual targetlist entries; otherwise they are
 * dummy entries created to carry the type, typmod, collation, and location
 * (for error messages) of each output column of the set-op node.  This info
 * is needed only during the internal recursion of this function, so outside
 * callers pass NULL for targetlist.  Note: the reason for passing the
 * actual targetlist entries of a leaf node is so that upper levels can
 * replace UNKNOWN Consts with properly-coerced constants.
 */
static Node* transform_cypher_union_tree(cypher_parsestate *cpstate, cypher_clause *clause,
                                         bool isTopLevel, List **targetlist)
{
    bool isLeaf = false;

    ParseState *pstate = (ParseState *)cpstate;

    if (IsA(clause, List))
    {
        isLeaf = true;
    }
    else if (is_ag_node(clause->self, cypher_union))
    {
        isLeaf = false;
    }
    else
    {
        ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                errmsg("Cypher union found a clause type it does not support"),
                parser_errposition(pstate, 0)));
    }

    /* Guard against stack overflow due to overly complex set-expressions */
    check_stack_depth();

    if (isLeaf)
    {
        /*process leaf return */
        Query *returnQuery;
        char returnName[32];
        RangeTblEntry *rte PG_USED_FOR_ASSERTS_ONLY;
        RangeTblRef *rtr;
        ListCell *tl;
        cypher_clause *leaf_clause;

        /*
         * Transform SelectStmt into a Query.
         *
         * This works the same as RETURN transformation normally would, except
         * that we prevent resolving unknown-type outputs as TEXT.  This does
         * not change the subquery's semantics since if the column type
         * matters semantically, it would have been resolved to something else
         * anyway.  Doing this lets us resolve such outputs using
         * select_common_type(), below.
         *
         * Note: previously transformed sub-queries don't affect the parsing
         * of this sub-query, because they are not in the toplevel pstate's
         * namespace list.
         */

        /*
         * Convert the List * that the grammar gave us to a cypher_clause.
         * cypher_analyze doesn't do this because the cypher_union clause
         * is hiding it.
         */
        leaf_clause = make_cypher_clause((List *)clause);

        returnQuery = cypher_parse_sub_analyze_union( (cypher_clause *) leaf_clause, cpstate,
                                               NULL, false, false);
        /*
         * Check for bogus references to Vars on the current query level (but
         * upper-level references are okay). Normally this can't happen
         * because the namespace will be empty, but it could happen if we are
         * inside a rule.
         */
        if (pstate->p_relnamespace || pstate->p_varnamespace)
        {
            if (contain_vars_of_level((Node *) returnQuery, 1))
            {
                ereport(ERROR,
                        (errcode(ERRCODE_INVALID_COLUMN_REFERENCE),
                         errmsg("UNION member statement cannot refer to other relations of same query level"),
                         parser_errposition(pstate,
                                            locate_var_of_level((Node *) returnQuery, 1))));
            }
        }

        /*
         * Extract a list of the non-junk TLEs for upper-level processing.
         */
        if (targetlist)
        {
            *targetlist = NIL;
            foreach(tl, returnQuery->targetList)
            {
                TargetEntry *tle = (TargetEntry *) lfirst(tl);

                if(!tle->resjunk)
                {
                    *targetlist = lappend(*targetlist, tle);
                }
            }
        }

        /*
         * Make the leaf query be a subquery in the top-level rangetable.
         */
        snprintf(returnName, sizeof(returnName), "*SELECT* %d ",
                 list_length(pstate->p_rtable) + 1);
        rte = addRangeTableEntryForSubquery(pstate,
                                            returnQuery,
                                            makeAlias(returnName, NIL),
                                            false,
                                            false);

        rtr = makeNode(RangeTblRef);
        /* assume new rte is at end */
        rtr->rtindex = list_length(pstate->p_rtable);
        Assert(rte == rt_fetch(rtr->rtindex, pstate->p_rtable));
        return (Node *) rtr;
    }
    else /*is not a leaf */
    {
        /* Process an internal node (set operation node) */
        SetOperationStmt *op = makeNode(SetOperationStmt);
        List *ltargetlist;
        List *rtargetlist;
        ListCell *ltl;
        ListCell *rtl;
        cypher_union *self = (cypher_union *) clause->self;
        const char *context;

        context = "UNION";

        op->op = self->op;
        op->all = self->all_or_distinct;

        /*
         * Recursively transform the left child node.
         */
        op->larg = transform_cypher_union_tree(cpstate ,(cypher_clause *) self->larg,
                                               false,
                                               &ltargetlist);

        /*
         * If we find ourselves processing a recursive CTE here something
         * went horribly wrong. That is an SQL contruct with no parallel in
         * cypher.
         */
        if (isTopLevel &&
            pstate->p_parent_cte &&
            pstate->p_parent_cte->cterecursive)
        {
            ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                    errmsg("Cypher does not support recursive CTEs"),
                    parser_errposition(pstate, 0)));
        }

        /*
         * Recursively transform the right child node.
         */
        op->rarg = transform_cypher_union_tree(cpstate, (cypher_clause *) self->rarg,
                                               false,
                                               &rtargetlist);

        /*
         * Verify that the two children have the same number of non-junk
         * columns, and determine the types of the merged output columns.
         */
        if (list_length(ltargetlist) != list_length(rtargetlist))
        {
            ereport(ERROR,
                    (errcode(ERRCODE_SYNTAX_ERROR),
                     errmsg("each %s query must have the same number of columns",
                            context),
                     parser_errposition(pstate,
                                        exprLocation((Node *) rtargetlist))));
        }

        if (targetlist)
        {
            *targetlist = NIL;
        }

        op->colTypes = NIL;
        op->colTypmods = NIL;
        op->colCollations = NIL;
        op->groupClauses = NIL;

        forboth(ltl, ltargetlist, rtl, rtargetlist)
        {
            TargetEntry *ltle = (TargetEntry *) lfirst(ltl);
            TargetEntry *rtle = (TargetEntry *) lfirst(rtl);
            Node *lcolnode = (Node *) ltle->expr;
            Node *rcolnode = (Node *) rtle->expr;
            Oid lcoltype = exprType(lcolnode);
            Oid rcoltype = exprType(rcolnode);
            int32 lcoltypmod = exprTypmod(lcolnode);
            int32 rcoltypmod = exprTypmod(rcolnode);
            Node *bestexpr;
            int bestlocation;
            Oid rescoltype;
            int32 rescoltypmod;
            Oid rescolcoll;

            lcolnode = coerce_entity_to_agtype(pstate, lcolnode);
            ltle->expr = (Expr *)lcolnode;
            lcoltype = exprType(lcolnode);

            rcolnode = coerce_entity_to_agtype(pstate, rcolnode);
            rtle->expr = (Expr *)rcolnode;
            rcoltype = exprType(rcolnode);

            /* select common type, same as CASE et al */
            rescoltype = select_common_type(pstate,
                                            list_make2(lcolnode, rcolnode),
                                            context,
                                            &bestexpr);
            bestlocation = exprLocation(bestexpr);
            /* if same type and same typmod, use typmod; else default */
            if (lcoltype == rcoltype && lcoltypmod == rcoltypmod)
            {
                rescoltypmod = lcoltypmod;
            }
            else
            {
                rescoltypmod = -1;
            }

            /*
             * Verify the coercions are actually possible.  If not, we'd fail
             * later anyway, but we want to fail now while we have sufficient
             * context to produce an error cursor position.
             *
             * For all non-UNKNOWN-type cases, we verify coercibility but we
             * don't modify the child's expression, for fear of changing the
             * child query's semantics.
             *
             * If a child expression is an UNKNOWN-type Const or Param, we
             * want to replace it with the coerced expression.  This can only
             * happen when the child is a leaf set-op node.  It's safe to
             * replace the expression because if the child query's semantics
             * depended on the type of this output column, it'd have already
             * coerced the UNKNOWN to something else.  We want to do this
             * because (a) we want to verify that a Const is valid for the
             * target type, or resolve the actual type of an UNKNOWN Param,
             * and (b) we want to avoid unnecessary discrepancies between the
             * output type of the child query and the resolved target type.
             * Such a discrepancy would disable optimization in the planner.
             *
             * If it's some other UNKNOWN-type node, eg a Var, we do nothing
             * (knowing that coerce_to_common_type would fail).  The planner
             * is sometimes able to fold an UNKNOWN Var to a constant before
             * it has to coerce the type, so failing now would just break
             * cases that might work.
             */
            if (lcoltype != UNKNOWNOID)
            {
                lcolnode = coerce_to_common_type(pstate, lcolnode,
                                                 rescoltype, context);
            }
            else if (IsA(lcolnode, Const) || IsA(lcolnode, Param))
            {
                lcolnode = coerce_to_common_type(pstate, lcolnode,
                                                 rescoltype, context);
                ltle->expr = (Expr *) lcolnode;
            }

            if (rcoltype != UNKNOWNOID)
            {
                rcolnode = coerce_to_common_type(pstate, rcolnode,
                                                 rescoltype, context);
            }
            else if (IsA(rcolnode, Const) || IsA(rcolnode, Param))
            {
                rcolnode = coerce_to_common_type(pstate, rcolnode,
                                                 rescoltype, context);
                rtle->expr = (Expr *) rcolnode;
            }

            /*
             * Select common collation.  A common collation is required for
             * all set operators except UNION ALL; see SQL:2008 7.13 <query
             * expression> Syntax Rule 15c.  (If we fail to identify a common
             * collation for a UNION ALL column, the curCollations element
             * will be set to InvalidOid, which may result in a runtime error
             * if something at a higher query level wants to use the column's
             * collation.)
             */
            rescolcoll = select_common_collation(pstate,
                                                 list_make2(lcolnode, rcolnode),
                                                 (op->op == SETOP_UNION && op->all));

            /* emit results */
            op->colTypes = lappend_oid(op->colTypes, rescoltype);
            op->colTypmods = lappend_int(op->colTypmods, rescoltypmod);
            op->colCollations = lappend_oid(op->colCollations, rescolcoll);

            /*
             * For all cases except UNION ALL, identify the grouping operators
             * (and, if available, sorting operators) that will be used to
             * eliminate duplicates.
             */
            if (op->op != SETOP_UNION || !op->all)
            {
                SortGroupClause *grpcl = makeNode(SortGroupClause);
                Oid sortop;
                Oid eqop;
                bool hashable = false;
                ParseCallbackState pcbstate;

                setup_parser_errposition_callback(&pcbstate, pstate,
                                                  bestlocation);

                /*
                 * determine the eqop and optional sortop
                 *
                 * NOTE: for UNION, we set hashable to false and pass a NULL to
                 * isHashable in get_sort_group_operators to prevent a logic error
                 * where UNION fails to exclude duplicate results.
                 *
                 */
                get_sort_group_operators(rescoltype,
                                         false, true, false,
                                         &sortop, &eqop, NULL,
                                         NULL);

                cancel_parser_errposition_callback(&pcbstate);

                /* we don't have a tlist yet, so can't assign sortgrouprefs */
                grpcl->tleSortGroupRef = 0;
                grpcl->eqop = eqop;
                grpcl->sortop = sortop;
                grpcl->nulls_first = false; /* OK with or without sortop */
                grpcl->hashable = hashable;

                op->groupClauses = lappend(op->groupClauses, grpcl);
            }

            /*
             * Construct a dummy tlist entry to return.  We use a SetToDefault
             * node for the expression, since it carries exactly the fields
             * needed, but any other expression node type would do as well.
             */
            if (targetlist)
            {
                SetToDefault *rescolnode = makeNode(SetToDefault);
                TargetEntry *restle;

                rescolnode->typeId = rescoltype;
                rescolnode->typeMod = rescoltypmod;
                rescolnode->collation = rescolcoll;
                rescolnode->location = bestlocation;
                restle = makeTargetEntry((Expr *) rescolnode,
                                         0, /* no need to set resno */
                                         NULL,
                                         false);
                *targetlist = lappend(*targetlist, restle);
            }
        }

        return (Node *)op;
    }//end else (is not leaf)
}

/*
 * Transform the Delete clause. Creates a _cypher_delete_clause
 * and passes the necessary information that is needed in the
 * execution phase.
 */
static Query *transform_cypher_delete(cypher_parsestate *cpstate,
                                      cypher_clause *clause)
{
    ParseState *pstate = (ParseState *)cpstate;
    cypher_delete *self = (cypher_delete *)clause->self;
    Query *query;
    TargetEntry *tle;
    FuncExpr *func_expr;

    cypher_delete_information *delete_data;

    delete_data = make_ag_node(cypher_delete_information);

    query = makeNode(Query);
    query->commandType = CMD_SELECT;
    query->targetList = NIL;

    if (!clause->prev)
    {
        ereport(ERROR,
                (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                 errmsg("DELETE cannot be the first clause in a Cypher query"),
                 parser_errposition(pstate, self->location)));
    }
    else
    {
        handle_prev_clause(cpstate, query, clause->prev, true);
    }

    delete_data->delete_items = transform_cypher_delete_item_list(cpstate,
                                                                  self->exprs,
                                                                  query);
    delete_data->graph_name = cpstate->graph_name;
    delete_data->graph_oid = cpstate->graph_oid;
    delete_data->detach = self->detach;

    if (!clause->next)
    {
        delete_data->flags |= CYPHER_CLAUSE_FLAG_TERMINAL;
    }

    func_expr = make_clause_func_expr(DELETE_CLAUSE_FUNCTION_NAME,
                                      (Node *)delete_data);

    // Create the target entry
    tle = makeTargetEntry((Expr *)func_expr, pstate->p_next_resno++,
                          AGE_VARNAME_DELETE_CLAUSE, false);
    query->targetList = lappend(query->targetList, tle);

    query->rtable = pstate->p_rtable;
    query->jointree = makeFromExpr(pstate->p_joinlist, NULL);

    return query;
}

/*
 * transform_cypher_unwind
 *      It contains logic to convert the form of an array into a row. Here, we
 *      are simply calling `age_unnest` function, and the actual transformation
 *      is handled by `age_unnest` function.
 */
static Query *transform_cypher_unwind(cypher_parsestate *cpstate,
                                      cypher_clause *clause)
{
    ParseState *pstate = (ParseState *) cpstate;
    cypher_unwind *self = (cypher_unwind *) clause->self;
    int target_syntax_loc;
    Query *query;
    Node *expr;
    FuncCall *unwind;
    ParseExprKind old_expr_kind;
    Node *funcexpr;
    TargetEntry *te;

    query = makeNode(Query);
    query->commandType = CMD_SELECT;

    if (clause->prev)
    {
        RangeTblEntry *rte;
        int rtindex;

        rte = transform_prev_cypher_clause(cpstate, clause->prev, true);
        rtindex = list_length(pstate->p_rtable);
        Assert(rtindex == 1); // rte is the first RangeTblEntry in pstate
        if (rtindex != 1) {
            ereport(ERROR,
                    (errcode(ERRCODE_DATATYPE_MISMATCH),
                     errmsg("invalid value for rtindex")));
        }
        query->targetList = expandRelAttrs(pstate, rte, rtindex, 0, -1);
    }

    target_syntax_loc = exprLocation((const Node *) self->target);

    if (findTarget(query->targetList, self->target->name) != NULL)
    {
        ereport(ERROR,
                (errcode(ERRCODE_DUPLICATE_ALIAS),
                        errmsg("duplicate variable \"%s\"", self->target->name),
                        parser_errposition((ParseState *) cpstate, target_syntax_loc)));
    }

    expr = transform_cypher_expr(cpstate, self->target->val, EXPR_KIND_SELECT_TARGET);

    unwind = makeFuncCall(list_make1(makeString("age_unnest")), NIL, -1);

    old_expr_kind = pstate->p_expr_kind;
    pstate->p_expr_kind = EXPR_KIND_SELECT_TARGET;

    funcexpr = ParseFuncOrColumn(pstate, unwind->funcname,
                                 list_make1(expr),
                                 pstate->p_last_srf, unwind,
                                 target_syntax_loc, false);
    pstate->p_expr_kind = old_expr_kind;

    te = makeTargetEntry((Expr *) funcexpr,
                         (AttrNumber) pstate->p_next_resno++,
                         self->target->name, false);

    query->targetList = lappend(query->targetList, te);
    query->rtable = pstate->p_rtable;
    query->jointree = makeFromExpr(pstate->p_joinlist, NULL);
    query->hasSubLinks = pstate->p_hasSubLinks;
    query->hasTargetSRFs = pstate->p_hasTargetSRFs;

    assign_query_collations(pstate, query);

    return query;
}

/*
 * Iterate through the list of items to delete and extract the variable name.
 * Then find the resno that the variable name belongs to.
 */
static List *transform_cypher_delete_item_list(cypher_parsestate *cpstate,
                                               List *delete_item_list,
                                               Query *query)
{
    ParseState *pstate = (ParseState *)cpstate;
    List *items = NIL;
    ListCell *lc;

    foreach(lc, delete_item_list)
    {
        Node *expr = (Node*)lfirst(lc);
        ColumnRef *col;
        Value *val, *pos;
        int resno;

        cypher_delete_item *item = make_ag_node(cypher_delete_item);

        if (!IsA(expr, ColumnRef))
        {
            ereport(ERROR,
                    (errmsg_internal("unexpected Node for cypher_clause")));
        }

        col = (ColumnRef *)expr;

        if (list_length(col->fields) != 1)
        {
            ereport(ERROR,
                    (errmsg_internal("unexpected Node for cypher_clause")));
        }
        val = (Value*)linitial(col->fields);

        if (!IsA(val, String))
        {
            ereport(ERROR,
                    (errmsg_internal("unexpected Node for cypher_clause")));
        }

        resno = get_target_entry_resno(pstate, query->targetList,
            val->val.str);
        if (resno == -1)
        {
            ereport(ERROR,
                    (errcode(ERRCODE_INVALID_COLUMN_REFERENCE),
                     errmsg("undefined reference to variable %s in DELETE clause",
                            val->val.str),
                     parser_errposition(pstate, col->location)));
        }

        pos = makeInteger(resno);

        item->var_name = val->val.str;
        item->entity_position = pos;

        items = lappend(items, item);
    }

    return items;
}

static Query *transform_cypher_set(cypher_parsestate *cpstate,
                                   cypher_clause *clause)
{
    ParseState *pstate = (ParseState *)cpstate;
    cypher_set *self = (cypher_set *)clause->self;
    Query *query;
    cypher_update_information *set_items_target_list;
    TargetEntry *tle;
    FuncExpr *func_expr;
    char *clause_name;

    query = makeNode(Query);
    query->commandType = CMD_SELECT;
    query->targetList = NIL;

    if (self->is_remove == true)
    {
        clause_name = UPDATE_CLAUSE_REMOVE;
    }
    else
    {
        clause_name = UPDATE_CLAUSE_SET;
    }

    if (!clause->prev)
    {
        ereport(ERROR,
                (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                 errmsg("%s cannot be the first clause in a Cypher query",
                        clause_name),
                 parser_errposition(pstate, self->location)));
    }
    else
    {
        handle_prev_clause(cpstate, query, clause->prev, true);
    }

    if (self->is_remove == true)
    {
        set_items_target_list = transform_cypher_remove_item_list(cpstate,
                                                                  self->items,
                                                                  query);
    }
    else
    {
        set_items_target_list = transform_cypher_set_item_list(cpstate,
                                                               self->items,
                                                               query);
    }

    set_items_target_list->clause_name = clause_name;
    set_items_target_list->graph_name = cpstate->graph_name;

    if (!clause->next)
    {
        set_items_target_list->flags |= CYPHER_CLAUSE_FLAG_TERMINAL;
    }

    func_expr = make_clause_func_expr(SET_CLAUSE_FUNCTION_NAME,
                                      (Node *)set_items_target_list);

    // Create the target entry
    tle = makeTargetEntry((Expr *)func_expr, pstate->p_next_resno++,
                          AGE_VARNAME_SET_CLAUSE, false);
    query->targetList = lappend(query->targetList, tle);

    query->rtable = pstate->p_rtable;
    query->jointree = makeFromExpr(pstate->p_joinlist, NULL);
    query->hasSubLinks = pstate->p_hasSubLinks;
    query->hasTargetSRFs = pstate->p_hasTargetSRFs;

    assign_query_collations(pstate, query);

    return query;
}

cypher_update_information *transform_cypher_remove_item_list(
    cypher_parsestate *cpstate, List *remove_item_list, Query *query)
{
    ParseState *pstate = (ParseState *)cpstate;
    ListCell *li;
    cypher_update_information *info = make_ag_node(cypher_update_information);

    info->set_items = NIL;
    info->flags = 0;

    foreach (li, remove_item_list)
    {
        cypher_set_item *set_item = (cypher_set_item*)lfirst(li);
        cypher_update_item *item;
        ColumnRef *ref;
        A_Indirection *ind;
        char *variable_name, *property_name;
        Value *property_node, *variable_node;

        item = make_ag_node(cypher_update_item);

        if (!is_ag_node(lfirst(li), cypher_set_item))
        {
            ereport(ERROR,
                    (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                     errmsg("unexpected node in cypher update list")));
        }

        if (set_item->is_add)
        {
            ereport(ERROR,
                    (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                     errmsg("REMOVE clause does not support adding properties from maps"),
                     parser_errposition(pstate, set_item->location)));
        }

        item->remove_item = true;
        item->replace_properties = false;

        if (!IsA(set_item->prop, A_Indirection))
        {
            ereport(ERROR,
                    (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                     errmsg("REMOVE clause must be in the format: REMOVE variable.property_name"),
                     parser_errposition(pstate, set_item->location)));
        }

        ind = (A_Indirection *)set_item->prop;

        // extract variable name
        if (!IsA(ind->arg, ColumnRef))
        {
            ereport(ERROR,
                    (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                     errmsg("REMOVE clause must be in the format: REMOVE variable.property_name"),
                     parser_errposition(pstate, set_item->location)));
        }

        ref = (ColumnRef *)ind->arg;
        variable_node = (Value*)linitial(ref->fields);

        variable_name = variable_node->val.str;
        item->var_name = variable_name;
        item->entity_position = get_target_entry_resno(pstate,
                                                       query->targetList,
                                                       variable_name);

        if (item->entity_position == -1)
        {
            ereport(ERROR,
                    (errcode(ERRCODE_INVALID_COLUMN_REFERENCE),
                     errmsg("undefined reference to variable %s in REMOVE clause",
                            variable_name),
                     parser_errposition(pstate, set_item->location)));
        }

        // extract property name
        if (list_length(ind->indirection) != 1)
        {
            ereport(ERROR,
                    (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                     errmsg("REMOVE clause must be in the format: REMOVE variable.property_name"),
                     parser_errposition(pstate, set_item->location)));
        }

        property_node = (Value*)linitial(ind->indirection);

        if (!IsA(property_node, String))
        {
            ereport(ERROR,
                    (errcode(ERRCODE_INVALID_COLUMN_REFERENCE),
                     errmsg("REMOVE clause expects a property name"),
                     parser_errposition(pstate, set_item->location)));
        }
        property_name = property_node->val.str;
        item->prop_name = property_name;

        info->set_items = lappend(info->set_items, item);
    }

    return info;
}

cypher_update_information *transform_cypher_set_item_list(
    cypher_parsestate *cpstate, List *set_item_list, Query *query)
{
    ParseState *pstate = (ParseState *)cpstate;
    ListCell *li;
    cypher_update_information *info = make_ag_node(cypher_update_information);

    info->set_items = NIL;
    info->flags = 0;

    foreach (li, set_item_list)
    {
        cypher_set_item *set_item = (cypher_set_item*)lfirst(li);
        bool rhs_is_map = nodeTag(set_item->expr) == T_EXTENSIBLE_NODE &&
                          is_ag_node(set_item->expr, cypher_map);
        TargetEntry *target_item;
        cypher_update_item *item;
        ColumnRef *ref = NULL;
        A_Indirection *ind = NULL;
        char *variable_name;
        Value *variable_node;

        if (!is_ag_node(lfirst(li), cypher_set_item))
        {
            ereport(ERROR,
                    (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                     errmsg("unexpected node in cypher update list")));
        }

        item = make_ag_node(cypher_update_item);
        item->remove_item = false;

        if (IsA(set_item->prop, ColumnRef))
        {
            if (!rhs_is_map &&
                !IsA(set_item->expr, FuncCall) &&
                !IsA(set_item->expr, ColumnRef)) {
                ereport(ERROR,
                        (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                         errmsg("SET clause expects a map"),
                         parser_errposition(pstate, set_item->location)));
            }

            ref = (ColumnRef *)set_item->prop;
            item->prop_name = NULL;
            item->replace_properties = true;
            item->is_add = set_item->is_add;

            if (IsA(set_item->expr, ColumnRef)) {
                set_item->expr = (Node *)makeFuncCall(
                    list_make1(makeString("properties")),
                    list_make1(set_item->expr), -1);
            }

            if (rhs_is_map)
                ((cypher_map *)set_item->expr)->keep_null = set_item->is_add;
        } else if (IsA(set_item->prop, A_Indirection)) {
            Value *property_node;

            ind = (A_Indirection *)set_item->prop;
            if (!IsA(ind->arg, ColumnRef)) {
                ereport(ERROR,
                        (errcode(ERRCODE_INVALID_COLUMN_REFERENCE),
                         errmsg("SET clause expects a variable name"),
                         parser_errposition(pstate, set_item->location)));
            }

            ref = (ColumnRef *)ind->arg;
            item->replace_properties = false;
            item->is_add = false;

            if (set_item->is_add) {
                ereport(ERROR,
                        (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                         errmsg("SET clause does not yet support incrementing a specific property"),
                         parser_errposition(pstate, set_item->location)));
            }

            if (list_length(ind->indirection) != 1) {
                ereport(ERROR,
                        (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                         errmsg("SET clause does not support updating maps or lists in a property"),
                         parser_errposition(pstate, set_item->location)));
            }

            property_node = (Value *)linitial(ind->indirection);
            if (!IsA(property_node, String)) {
                ereport(ERROR,
                        (errcode(ERRCODE_INVALID_COLUMN_REFERENCE),
                         errmsg("SET clause expects a property name"),
                         parser_errposition(pstate, set_item->location)));
            }

            item->prop_name = property_node->val.str;
        } else {
            ereport(ERROR,
                    (errcode(ERRCODE_INVALID_COLUMN_REFERENCE),
                     errmsg("SET clause expects a variable name"),
                     parser_errposition(pstate, set_item->location)));
        }

        if (list_length(ref->fields) != 1)
        {
            ereport(ERROR,
                    (errcode(ERRCODE_INVALID_COLUMN_REFERENCE),
                     errmsg("SET clause expects a variable name"),
                     parser_errposition(pstate, set_item->location)));
        }

        variable_node = (Value *)linitial(ref->fields);
        if (!IsA(variable_node, String))
        {
            ereport(ERROR,
                    (errcode(ERRCODE_INVALID_COLUMN_REFERENCE),
                     errmsg("SET clause expects a variable name"),
                     parser_errposition(pstate, set_item->location)));
        }

        variable_name = variable_node->val.str;
        item->var_name = variable_name;
        item->entity_position = get_target_entry_resno(pstate,
                                                       query->targetList,
                                                       variable_name);

        if (item->entity_position == -1)
        {
            ereport(ERROR,
                    (errcode(ERRCODE_INVALID_COLUMN_REFERENCE),
                     errmsg("undefined reference to variable %s in SET clause",
                            variable_name),
                     parser_errposition(pstate, set_item->location)));
        }

        /*
         * Keep the SET RHS under an internal name.  FigureColname() would use
         * the referenced variable name for expressions such as SET n.copy = n,
         * producing a second column named "n" and making a following RETURN n
         * ambiguous.  AGE_DEFAULT_VARNAME_PREFIX also keeps this executor-only
         * value out of later user-visible projections.
         */
        item->prop_position = (AttrNumber)pstate->p_next_resno;
        target_item = transform_cypher_item(cpstate, set_item->expr, NULL,
                                            EXPR_KIND_SELECT_TARGET,
                                            AGE_VARNAME_SET_VALUE,
                                            false);

        target_item->expr = add_volatile_wrapper(pstate, target_item->expr);

        query->targetList = lappend(query->targetList, target_item);
        info->set_items = lappend(info->set_items, item);
    }

    return info;
}

/* from PG's static helper function */
static Node *flatten_grouping_sets(Node *expr, bool toplevel,
                                   bool *hasGroupingSets)
{
    /* just in case of pathological input */
    check_stack_depth();

    if (expr == (Node *) NIL)
    {
        return (Node *) NIL;
    }

    switch (expr->type)
    {
        case T_RowExpr:
        {
            RowExpr *r = (RowExpr *) expr;

            if (r->row_format == COERCE_IMPLICIT_CAST)
            {
                return flatten_grouping_sets((Node *) r->args, false, NULL);
            }
            break;
        }
        case T_GroupingSet:
            ereport(ERROR,
                    (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                     errmsg("flattening of GroupingSet is not implemented")));
            break;
        case T_List:
        {
            List *result = NIL;
            ListCell *l;

            foreach(l, (List *) expr)
            {
                Node *n = NULL;

                n = flatten_grouping_sets((Node*)lfirst(l), toplevel, hasGroupingSets);

                if (n != (Node *) NIL)
                {
                    if (IsA(n, List))
                    {
                        result = list_concat(result, (List *) n);
                    }
                    else
                    {
                        result = lappend(result, n);
                    }
                }
            }
            return (Node *) result;
        }
        default:
            break;
    }
    return expr;
}

/* from PG's addTargetToGroupList */
static List *add_target_to_group_list(cypher_parsestate *cpstate,
                                      TargetEntry *tle, List *grouplist,
                                      List *targetlist, int location)
{
    ParseState *pstate = &cpstate->pstate;
    Oid restype = exprType((Node *) tle->expr);

    /* if tlist item is an UNKNOWN literal, change it to TEXT */
    if (restype == UNKNOWNOID)
    {
        tle->expr = (Expr *) coerce_type(pstate, (Node *) tle->expr, restype,
                                         TEXTOID, -1, COERCION_IMPLICIT,
                                         COERCE_IMPLICIT_CAST, NULL, NULL, -1);
        restype = TEXTOID;
    }

    /* avoid making duplicate grouplist entries */
    if (!targetIsInSortList(tle, InvalidOid, grouplist))
    {
        SortGroupClause *grpcl = makeNode(SortGroupClause);
        Oid sortop;
        Oid eqop;
        bool hashable;
        ParseCallbackState pcbstate;

        setup_parser_errposition_callback(&pcbstate, pstate, location);

        /* determine the eqop and optional sortop */
        get_sort_group_operators(restype, false, true, false, &sortop, &eqop,
                                 NULL, &hashable);

        cancel_parser_errposition_callback(&pcbstate);

        grpcl->tleSortGroupRef = assignSortGroupRef(tle, targetlist);
        grpcl->eqop = eqop;
        grpcl->sortop = sortop;
        grpcl->nulls_first = false; /* OK with or without sortop */
        grpcl->hashable = hashable;

        grouplist = lappend(grouplist, grpcl);
    }

    return grouplist;
}

/* from PG's transformGroupClauseExpr */
static Index transform_group_clause_expr(List **flatresult,
                                         Bitmapset *seen_local,
                                         cypher_parsestate *cpstate,
                                         Node *gexpr, List **targetlist,
                                         List *sortClause,
                                         ParseExprKind exprKind, bool toplevel)
{
    TargetEntry *tle = NULL;
    bool found = false;

    tle = find_target_list_entry(cpstate, gexpr, targetlist, exprKind);

    if (tle->ressortgroupref > 0)
    {
        ListCell *sl;

        /*
         * Eliminate duplicates (GROUP BY x, x) but only at local level.
         * (Duplicates in grouping sets can affect the number of returned
         * rows, so can't be dropped indiscriminately.)
         *
         * Since we don't care about anything except the sortgroupref, we can
         * use a bitmapset rather than scanning lists.
         */
        if (bms_is_member(tle->ressortgroupref, seen_local))
        {
            return 0;
        }

        /*
         * If we're already in the flat clause list, we don't need to consider
         * adding ourselves again.
         */
        found = targetIsInSortList(tle, InvalidOid, *flatresult);
        if (found)
        {
            return tle->ressortgroupref;
        }

        /*
         * If the GROUP BY tlist entry also appears in ORDER BY, copy operator
         * info from the (first) matching ORDER BY item.  This means that if
         * you write something like "GROUP BY foo ORDER BY foo USING <<<", the
         * GROUP BY operation silently takes on the equality semantics implied
         * by the ORDER BY.  There are two reasons to do this: it improves the
         * odds that we can implement both GROUP BY and ORDER BY with a single
         * sort step, and it allows the user to choose the equality semantics
         * used by GROUP BY, should she be working with a datatype that has
         * more than one equality operator.
         *
         * If we're in a grouping set, though, we force our requested ordering
         * to be NULLS LAST, because if we have any hope of using a sorted agg
         * for the job, we're going to be tacking on generated NULL values
         * after the corresponding groups. If the user demands nulls first,
         * another sort step is going to be inevitable, but that's the
         * planner's problem.
         */

         foreach(sl, sortClause)
         {
             SortGroupClause *sc = (SortGroupClause *) lfirst(sl);

             if (sc->tleSortGroupRef == tle->ressortgroupref)
             {
                 SortGroupClause *grpc = (SortGroupClause*)copyObject(sc);

                 if (!toplevel)
                 {
                     grpc->nulls_first = false;
                 }
                 *flatresult = lappend(*flatresult, grpc);
                 found = true;
                 break;
             }
         }
    }

    /*
     * If no match in ORDER BY, just add it to the result using default
     * sort/group semantics.
     */
    if (!found)
    {
        *flatresult = add_target_to_group_list(cpstate, tle, *flatresult,
                                               *targetlist, exprLocation(gexpr));
    }

    /* _something_ must have assigned us a sortgroupref by now... */

    return tle->ressortgroupref;
}

/* from PG's transformGroupClause */
static List * transform_group_clause(cypher_parsestate *cpstate,
                                     List *grouplist, List **groupingSets,
                                     List **targetlist, List *sortClause,
                                     ParseExprKind exprKind)
{
    List *result = NIL;
    List *flat_grouplist;
    List *gsets = NIL;
    ListCell *gl;
    bool hasGroupingSets = false;
    Bitmapset *seen_local = NULL;

    /*
     * Recursively flatten implicit RowExprs. (Technically this is only needed
     * for GROUP BY, per the syntax rules for grouping sets, but we do it
     * anyway.)
     */
    flat_grouplist = (List *) flatten_grouping_sets((Node *) grouplist, true,
                                                    &hasGroupingSets);

    foreach(gl, flat_grouplist)
    {
        Node       *gexpr = (Node *) lfirst(gl);

        if (IsA(gexpr, GroupingSet))
        {
            ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                            errmsg("GroupingSet is not implemented")));
            break;
        }
        else
        {
            Index ref = transform_group_clause_expr(&result, seen_local,
                                                    cpstate, gexpr, targetlist,
                                                    sortClause, exprKind, true);
            if (ref > 0)
            {
                seen_local = bms_add_member(seen_local, ref);
                if (hasGroupingSets)
                {
                    ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                            errmsg("GroupingSet is not implemented")));
                }
            }
        }
    }

    /* parser should prevent this */
    Assert(gsets == NIL || groupingSets != NULL);

    if (groupingSets)
    {
        *groupingSets = gsets;
    }

    return result;
}

static List *transform_cypher_distinct_clause(cypher_parsestate *cpstate,
                                              List **target_list,
                                              List *sort_clause)
{
    ParseState *pstate = (ParseState *)cpstate;
    List *entity_entries = NIL;
    List *entity_id_entries = NIL;
    List *distinct_clause;
    ListCell *target_cell;
    ListCell *entity_cell;
    ListCell *entity_id_cell;

    if (sort_clause != NIL) {
        return transformDistinctClause(pstate, target_list, sort_clause, false);
    }

    foreach (target_cell, *target_list)
    {
        TargetEntry *target_entry = (TargetEntry *)lfirst(target_cell);
        Oid target_type = exprType((Node *)target_entry->expr);
        FieldSelect *entity_id;
        TargetEntry *entity_id_entry;

        if (target_entry->resjunk ||
            (target_type != VERTEXOID && target_type != EDGEOID)) {
            continue;
        }

        entity_id = make_field_select((Expr *)copyObject(target_entry->expr),
                                      1, GRAPHIDOID);
        entity_id_entry = makeTargetEntry(
            (Expr *)entity_id, (AttrNumber)pstate->p_next_resno++,
            AGE_DEFAULT_VARNAME_PREFIX"distinct_entity_id", false);

        target_entry->resjunk = true;
        *target_list = lappend(*target_list, entity_id_entry);
        entity_entries = lappend(entity_entries, target_entry);
        entity_id_entries = lappend(entity_id_entries, entity_id_entry);
    }

    distinct_clause = transformDistinctClause(pstate, target_list, NIL, false);

    forboth (entity_cell, entity_entries,
             entity_id_cell, entity_id_entries)
    {
        TargetEntry *entity_entry = (TargetEntry *)lfirst(entity_cell);
        TargetEntry *entity_id_entry = (TargetEntry *)lfirst(entity_id_cell);

        entity_entry->resjunk = false;
        entity_id_entry->resjunk = true;
    }

    return distinct_clause;
}

static Query *transform_cypher_return(cypher_parsestate *cpstate,
                                      cypher_clause *clause)
{
    ParseState *pstate = (ParseState *)cpstate;
    cypher_return *self = (cypher_return *)clause->self;
    Query *query;
    List *groupClause = NIL;

    query = makeNode(Query);
    query->commandType = CMD_SELECT;

    if (self->distinct || self->order_by != NULL ||
			 self->skip != NULL || self->limit != NULL)
	{
		bool distinct = self->distinct;
		List *order_by = self->order_by;
		Node *skip = self->skip;
		Node *limit = self->limit;

		/*
		 * detach options so that this function passes through this if statement
		 * when the function is called again recursively
		 */
		self->distinct = false;
		self->order_by = NIL;
		self->skip = NULL;
		self->limit = NULL;
		query = transform_cypher_clause(cpstate,  clause);
		self->distinct = distinct;
		self->order_by = order_by;
		self->skip = skip;
		self->limit = limit;

        // ORDER BY
        query->sortClause = transform_cypher_order_by(cpstate, self->order_by,
                                                    &query->targetList,
        EXPR_KIND_ORDER_BY);

        // DISTINCT
        if (self->distinct)
        {
            query->distinctClause = transform_cypher_distinct_clause(
                cpstate, &query->targetList, query->sortClause);
            query->hasDistinctOn = false;
        }
        else
        {
            query->distinctClause = NIL;
            query->hasDistinctOn = false;
        }

        // SKIP and LIMIT
        query->limitOffset = transform_cypher_limit(cpstate, self->skip,
                                                    EXPR_KIND_OFFSET, "SKIP");
        query->limitCount = transform_cypher_limit(cpstate, self->limit,
                                                EXPR_KIND_LIMIT, "LIMIT");

	}
	else
	{
        if (clause->prev)
        {
            transform_prev_cypher_clause(cpstate, clause->prev, true);
        }

        query->targetList = transform_cypher_item_list(cpstate, self->items,
                                                    &groupClause,
                                                    EXPR_KIND_SELECT_TARGET);
                                                     /* 'auto' GROUP BY (from PG's transformGroupClause) */
        query->groupClause = transform_group_clause(cpstate, groupClause,
                                                    &query->groupingSets,
                                                    &query->targetList,
                                                    query->sortClause,
                                                    EXPR_KIND_GROUP_BY);
            }

    markTargetListOrigins(pstate, query->targetList);
    query->rtable = pstate->p_rtable;
    query->jointree = makeFromExpr(pstate->p_joinlist, NULL);
    query->hasAggs = pstate->p_hasAggs;
    query->hasSubLinks = pstate->p_hasSubLinks;
    query->hasTargetSRFs = pstate->p_hasTargetSRFs;

    assign_query_collations(pstate, query);

    /* this must be done after collations, for reliable comparison of exprs */
    if (pstate->p_hasAggs ||
        query->groupClause || query->groupingSets || query->havingQual)
    {
        parse_check_aggregates(pstate, query);
    }

    return query;
}

// see transformSortClause()
static List *transform_cypher_order_by(cypher_parsestate *cpstate,
                                       List *sort_items, List **target_list,
                                       ParseExprKind expr_kind)
{
    ParseState *pstate = (ParseState *)cpstate;
    List *sort_list = NIL;
    ListCell *li;

    foreach (li, sort_items)
    {
        SortBy *sort_by = (SortBy*)lfirst(li);
        TargetEntry *te;

        te = find_target_list_entry(cpstate, sort_by->node, target_list,
                                    expr_kind);
        sort_list = addTargetToSortList(pstate, te, sort_list, *target_list,
                                        sort_by, false);
    }

    return sort_list;
}

// see findTargetlistEntrySQL99()
static TargetEntry *find_target_list_entry(cypher_parsestate *cpstate,
                                           Node *node, List **target_list,
                                           ParseExprKind expr_kind)
{
    Node *expr;
    ListCell *lt;
    TargetEntry *te;
    
    if (IsA(node, ColumnRef)) {
        ColumnRef *cref = (ColumnRef *)node;
        
	    int	 nfields = list_length(cref->fields);
        Node* field1 = (Node*)linitial(cref->fields);
	    if (nfields == 1) {
            char*  col = strVal(field1);
            foreach (lt, *target_list) {
                te = (TargetEntry*)lfirst(lt);
                /* resjunk entries (e.g. earlier ORDER BY items) have no name */
                if (te->resname != NULL && strcmp(col, te->resname) == 0) {
                    return te;
                }
            }
        }
    }

    expr = transform_cypher_expr(cpstate, node, expr_kind);

    foreach (lt, *target_list)
    {
        Node *te_expr;

        te = (TargetEntry*)lfirst(lt);
        te_expr = strip_implicit_coercions((Node *)te->expr);

        if (equal(expr, te_expr))
        {
            return te;
        }
    }

    te = transform_cypher_item(cpstate, node, expr, expr_kind, NULL, true);

    *target_list = lappend(*target_list, te);

    return te;
}

// see transformLimitClause()
static Node *transform_cypher_limit(cypher_parsestate *cpstate, Node *node,
                                    ParseExprKind expr_kind,
                                    const char *construct_name)
{
    ParseState *pstate = (ParseState *)cpstate;
    Node *qual;

    if (!node)
    {
        return NULL;
    }

    qual = transform_cypher_expr(cpstate, node, expr_kind);

    qual = coerce_to_specific_type(pstate, qual, INT8OID, construct_name);

    // LIMIT can't refer to any variables of the current query.
    if (contain_vars_of_level(qual, 0))
    {
        ereport(ERROR,
                (errcode(ERRCODE_INVALID_COLUMN_REFERENCE),
                 errmsg("argument of %s must not contain variables",
                        construct_name),
                 parser_errposition(pstate, locate_var_of_level(qual, 0))));
    }

    return qual;
}

static Query *transform_cypher_with(cypher_parsestate *cpstate,
                                    cypher_clause *clause)
{
    cypher_with *self = (cypher_with *)clause->self;
    cypher_return *return_clause;
    cypher_clause *wrapper;

    // TODO: check that all items have an alias for each

    // WITH clause is basically RETURN clause with optional WHERE subclause
    return_clause = make_ag_node(cypher_return);
    return_clause->distinct = self->distinct;
    return_clause->items = self->items;
    return_clause->order_by = self->order_by;
    return_clause->skip = self->skip;
    return_clause->limit = self->limit;

    wrapper = (cypher_clause*)palloc(sizeof(*wrapper));
    wrapper->self = (Node *)return_clause;
    wrapper->prev = clause->prev;

    return transform_cypher_clause_with_where(cpstate, transform_cypher_return,
                                              wrapper, self->where); 
}

static Query *transform_cypher_clause_with_where(cypher_parsestate *cpstate,
                                                 transform_method transform,
                                                 cypher_clause *clause, Node *where)
{
    ParseState *pstate = (ParseState *)cpstate;
    Query *query;
    Node *where_qual = NULL;

    if (where)
    {
        RangeTblEntry *rte;
        int rtindex;

        query = makeNode(Query);
        query->commandType = CMD_SELECT;

        rte = transform_cypher_clause_as_subquery(cpstate, transform, clause, NULL, true);

        rtindex = list_length(pstate->p_rtable);
        Assert(rtindex == 1); // rte is the only RangeTblEntry in pstate
        if (rtindex != 1)
        {
            ereport(ERROR,
                    (errcode(ERRCODE_DATATYPE_MISMATCH),
                     errmsg("invalid value for rtindex")));
        }

        /*
         * add all the target entries in rte to the current target list to pass
         * all the variables that are introduced in the previous clause to the
         * next clause
         */
        query->targetList = expandRelAttrs(pstate, rte, rtindex, 0, -1);

        markTargetListOrigins(pstate, query->targetList);

        query->rtable = pstate->p_rtable;

        where_qual = transform_cypher_expr(cpstate, where,
                                           EXPR_KIND_WHERE);

        where_qual = coerce_cypher_expr_to_boolean(pstate, where_qual,
                                                   "WHERE");
        
        query->jointree = makeFromExpr(pstate->p_joinlist, where_qual);
        assign_query_collations(pstate, query);
    }
    else
    {
        query = transform(cpstate, clause);
    }

    query->hasSubLinks = pstate->p_hasSubLinks;
    query->hasTargetSRFs = pstate->p_hasTargetSRFs;
    query->hasAggs = pstate->p_hasAggs;

    return query;
}

static bool is_zero_lower_bound_vle(Node *varlen)
{
    FuncCall *func;
    Node *lower_node;
    A_Const *lower;

    if (varlen == NULL || !IsA(varlen, FuncCall)) {
        return false;
    }

    func = (FuncCall *)varlen;
    if (list_length(func->args) < VLE_FUNCTION_MIN_ARGUMENTS) {
        return false;
    }

    lower_node = (Node *)list_nth(func->args, VLE_LOWER_BOUND_ARGUMENT_INDEX);
    if (lower_node == NULL || !IsA(lower_node, A_Const)) {
        return false;
    }

    lower = (A_Const *)lower_node;
    return lower->val.type == T_Integer && lower->val.val.ival == 0;
}

static bool path_check_valid_label(cypher_path *path,
                                   cypher_parsestate *cpstate)
{
    ListCell *lc;
    int index = 0;

    foreach (lc, path->path)
    {
        label_cache_data *label;

        if (index % PATH_NODE_EDGE_POSITION_MODULUS == 0) {
            cypher_node *node = (cypher_node *)lfirst(lc);

            if (node->label != NULL) {
                label = search_label_name_graph_cache(node->label,
                                                      cpstate->graph_oid);
                if (label == NULL || label->kind != LABEL_KIND_VERTEX) {
                    return false;
                }
            }
        } else {
            cypher_relationship *rel =
                (cypher_relationship *)lfirst(lc);

            if (rel->label != NULL) {
                label = search_label_name_graph_cache(rel->label,
                                                      cpstate->graph_oid);
                /*
                 * A zero-hop VLE does not consume an edge. Preserve its
                 * self-binding even when the requested edge label does
                 * not exist; positive-hop traversal still compares the
                 * requested label and rejects every real edge.
                 */
                if ((label == NULL || label->kind != LABEL_KIND_EDGE) &&
                    !is_zero_lower_bound_vle(rel->varlen)) {
                    return false;
                }
            }
        }

        index++;
    }

    return true;
}

static bool match_check_valid_label(cypher_match *match,
                                    cypher_parsestate *cpstate)
{
    ListCell *lc;

    foreach (lc, match->pattern)
    {
        if (!path_check_valid_label((cypher_path *)lfirst(lc), cpstate)) {
            return false;
        }
    }

    return true;
}

static Query *transform_cypher_match(cypher_parsestate *cpstate,
                                     cypher_clause *clause)
{
    cypher_match *match_self = (cypher_match*) clause->self;
    Node *where;

    /*
     * A preceding write may create labels while its clause is transformed.
     * Defer validation until that transformation has completed.
     */
    if (!clause_chain_has_dml(clause->prev) &&
        !match_check_valid_label(match_self, cpstate)) {
        match_self->where = make_false_where_clause(false);
    }

    where = match_self->where;

    if (!match_self->optional && has_list_comp_or_subquery(where)) {
        match_self->where = NULL;
        return transform_cypher_clause_with_where(cpstate,
                                                  transform_cypher_match_pattern,
                                                  clause, where);
    }

    return transform_cypher_match_pattern(cpstate, clause);
}

/*
 * Transform the clause into a subquery. This subquery will be used
 * in a join so setup the namespace item and the created the rtr
 * for the join to use.
 */
static Node *transform_clause_for_join(cypher_parsestate *cpstate,
                                       cypher_clause *clause,
                                       RangeTblEntry **rte,
                                       ParseNamespaceItem **nsitem,
                                       Alias* alias)
{
    ParseState *pstate = (ParseState *)cpstate;
    RangeTblRef *rtr;

    *rte = transform_cypher_clause_as_subquery(cpstate,
                                               transform_cypher_clause,
                                               clause, alias, false);

    *nsitem = create_namespace_item(*rte, false, true, false, true);

    rtr = makeNode(RangeTblRef);
    rtr->rtindex = RTERangeTablePosn(pstate, *rte, NULL);

    return (Node *) rtr;
}

/*
 * For cases where we need to join two subqueries together (OPTIONAL MATCH and
 * MERGE) we need to take the columns available in each rte and merge them
 * together. The l_rte has precedence when there is a conflict, because that
 * means that the pattern create in the current clause is referencing a
 * variable declared in a previous clause (the l_rte). The output is the
 * res_colnames and res_colvars that are passed in.
 */
static void get_res_cols(ParseState *pstate, RangeTblEntry *l_rte,
                         RangeTblEntry *r_rte, List **res_colnames,
                         List **res_colvars)
{
    List *l_colnames, *l_colvars;
    List *r_colnames, *r_colvars;
    ListCell *r_lname, *r_lvar;
    List *colnames = NIL;
    List *colvars = NIL;

    expandRTE(l_rte, RTERangeTablePosn(pstate, l_rte, NULL), 0, -1, false,
              &l_colnames, &l_colvars);
    expandRTE(r_rte, RTERangeTablePosn(pstate, r_rte, NULL), 0, -1, false,
              &r_colnames, &r_colvars);

    // add in all colnames and colvars from the l_rte.
    *res_colnames = list_concat(*res_colnames, l_colnames);
    *res_colvars = list_concat(*res_colvars, l_colvars);

    // find new columns and if they are a var, pass them in.
    forboth(r_lname, r_colnames, r_lvar, r_colvars)
    {
        char *r_colname = strVal(lfirst(r_lname));
        ListCell *lname;
        ListCell *lvar;
        Var *var = NULL;

        forboth(lname, *res_colnames, lvar, *res_colvars)
        {
            char *colname = strVal(lfirst(lname));

            if (strcmp(r_colname, colname) == 0)
            {
                var = (Var*)lfirst(lvar);
                break;
            }
        }

        if (var == NULL)
        {
            colnames = lappend(colnames, lfirst(r_lname));
            colvars = lappend(colvars, lfirst(r_lvar));
        }
    }

    *res_colnames = list_concat(*res_colnames, colnames);
    *res_colvars = list_concat(*res_colvars, colvars);
}

/*
 * transform_cypher_optional_match_clause
 *      Transform the previous clauses and OPTIONAL MATCH clauses to be LATERAL LEFT JOIN
 *   transform_cypher_optional_match_clause   to construct a result value.
 */
static RangeTblEntry *transform_cypher_optional_match_clause(cypher_parsestate *cpstate,
                                                             cypher_clause *clause)
{
    cypher_clause *prevclause;
    RangeTblEntry *rte;
    RangeTblEntry *l_rte, *r_rte;
    ParseNamespaceItem *l_nsitem, *r_nsitem;
    ParseState *pstate = (ParseState *) cpstate;
    JoinExpr* j = makeNode(JoinExpr);
    List *res_colnames = NIL, *res_colvars = NIL;
    Alias *l_alias, *r_alias;
    ParseNamespaceItem *nsitem;
    cypher_match *match_self = (cypher_match *)clause->self;
    Node *saved_where = match_self->where;
    int i = 0;

    j->jointype = JOIN_LEFT;

    /*
     * OPTIONAL MATCH WHERE is a LEFT JOIN predicate. Detach it while the
     * right-side subquery is transformed, then resolve it against both sides.
     */
    match_self->where = NULL;

    l_alias = makeAlias(PREV_CYPHER_CLAUSE_ALIAS, NIL);
    r_alias = makeAlias(CYPHER_OPT_RIGHT_ALIAS, NIL);

    j->larg = transform_clause_for_join(cpstate, clause->prev, &l_rte,
                                        &l_nsitem, l_alias);
    pstate->p_relnamespace = lappend(pstate->p_relnamespace, l_nsitem);
    pstate->p_varnamespace = lappend(pstate->p_varnamespace, l_nsitem);

    /*
     * Remove the previous clause so when the transform_clause_for_join function
     * transforms the OPTIONAL MATCH, the previous clause will not be transformed
     * again.
     */
    prevclause = clause->prev;
    clause->prev = NULL;

    //set the lateral flag to true
    pstate->p_lateral_active = true;

    j->rarg = transform_clause_for_join(cpstate, clause, &r_rte,
                                        &r_nsitem, r_alias);

    /*
     * Keep long correlated OPTIONAL MATCH patterns as parameterized lateral
     * subqueries. If openGauss pulls one up, endpoint predicates that reference
     * the left side are postponed until after the right-side path has been
     * expanded, which can turn a selective traversal into full edge-table
     * joins. A security barrier preserves the LEFT JOIN boundary and lets the
     * bound outer vertices drive endpoint index scans inside the pattern.
     *
     * Short optional patterns are intentionally left flattenable: preserving
     * their subquery boundary adds measurable overhead without avoiding a
     * costly intermediate path. Three relationships are the first case where
     * delaying both bound endpoints can materialize a large multi-edge join.
     */
    if (contain_vars_of_level((Node *)r_rte->subquery, 1))
    {
        ListCell *path_cell;

        foreach (path_cell, match_self->pattern)
        {
            cypher_path *path = (cypher_path *)lfirst(path_cell);

            if (list_length(path->path) >=
                OPTIONAL_PATH_SECURITY_BARRIER_MIN_ELEMENTS) {
                r_rte->security_barrier = true;
                break;
            }
        }
    }

    pstate->p_relnamespace = lappend(pstate->p_relnamespace, r_nsitem);
    pstate->p_varnamespace = lappend(pstate->p_varnamespace, r_nsitem);

    if (saved_where != NULL)
    {
        Node *where_qual = transform_cypher_expr(cpstate, saved_where,
                                                 EXPR_KIND_WHERE);

        j->quals = coerce_cypher_expr_to_boolean(pstate, where_qual, "WHERE");
    }

    match_self->where = saved_where;

    // we are done transform the lateral left join
    pstate->p_lateral_active = false;

    /*
     * We are done with the previous clause in the transform phase, but
     * reattach the previous clause for semantics.
     */
    clause->prev = prevclause;
    pstate->p_relnamespace = NULL;
    pstate->p_varnamespace=NULL;

    // get the colnames and colvars from the rtes
    get_res_cols(pstate, l_rte, r_rte, &res_colnames, &res_colvars);

    rte = addRangeTableEntryForJoin(pstate, res_colnames, j->jointype,
                                    res_colvars, j->alias, true);

    j->rtindex = RTERangeTablePosn(pstate, rte, NULL);

    for (i = list_length(pstate->p_joinexprs) + 1; i < j->rtindex; i++)
    {
        pstate->p_joinexprs = lappend(pstate->p_joinexprs, NULL);
    }
    pstate->p_joinexprs = lappend(pstate->p_joinexprs, j);
    Assert(list_length(pstate->p_joinexprs) == j->rtindex);

    pstate->p_joinlist = lappend(pstate->p_joinlist, j);

    nsitem = create_namespace_item(rte, false, true, false, true);
    pstate->p_relnamespace = lappend(pstate->p_relnamespace, nsitem);
    pstate->p_varnamespace = lappend(pstate->p_varnamespace, nsitem);

    return rte;
}

static Query *transform_cypher_match_pattern(cypher_parsestate *cpstate,
                                             cypher_clause *clause)
{
    ParseState *pstate = (ParseState *)cpstate;
    cypher_match *self = (cypher_match *)clause->self;
    Query *query;
    Node *where = self->where;

    query = makeNode(Query);
    query->commandType = CMD_SELECT;

    if (self->optional == true && clause->next)
    {
        cypher_clause *next = clause->next;

        if (is_ag_node(next->self, cypher_match))
        {
            cypher_match *next_self = (cypher_match *)next->self;

            if (!next_self->optional) {
                ereport(ERROR,
                        (errcode(ERRCODE_SYNTAX_ERROR),
                         errmsg("MATCH cannot follow OPTIONAL MATCH"),
                         parser_errposition(pstate,
                                            exprLocation((Node *)next_self))));
            }
        }
    }

    // If there is no previous clause, transform to a general MATCH clause.
    if (self->optional == true && clause->prev != NULL)
    {
        RangeTblEntry *rte = transform_cypher_optional_match_clause(cpstate, clause);

        query->targetList = make_target_list_from_join(pstate, rte);
        query->rtable = pstate->p_rtable;
        query->jointree = makeFromExpr(pstate->p_joinlist, NULL);
    }
    else
    {
        if (clause->prev)
        {
            RangeTblEntry *rte;
            int rtindex;
            bool has_dml;

            rte = transform_prev_cypher_clause(cpstate, clause->prev, true);
            has_dml = clause_chain_has_dml(clause->prev);
            if (has_dml) {
                /* MATCH quals must not run before the preceding write chain. */
                rte->security_barrier = true;
            }

            rtindex = list_length(pstate->p_rtable);
            Assert(rtindex == 1); // rte is the first RangeTblEntry in pstate
            if (rtindex != 1) {
                ereport(ERROR,
                        (errcode(ERRCODE_DATATYPE_MISMATCH),
                         errmsg("invalid value for rtindex")));
            }

            /*
             * add all the target entries in rte to the current target list to pass
             * all the variables that are introduced in the previous clause to the
             * next clause
             */
            query->targetList = expandRelAttrs(pstate, rte, rtindex, 0, -1);

            /*
             * CREATE can introduce labels while the predecessor is transformed.
             * Recheck now, after those catalog and cache changes are visible.
             * Keep the false predicate volatile so planning cannot discard the
             * predecessor write before execution.
             */
            if (has_dml && !match_check_valid_label(self, cpstate)) {
                where = make_false_where_clause(true);
            }
        }

        transform_match_pattern(cpstate, query, self->pattern, where);
    }

    markTargetListOrigins(pstate, query->targetList);

    query->hasSubLinks = pstate->p_hasSubLinks;
    query->hasWindowFuncs = pstate->p_hasWindowFuncs;
    query->hasTargetSRFs = pstate->p_hasTargetSRFs;
    query->hasAggs = pstate->p_hasAggs;
    query->hasSubLinks = pstate->p_hasSubLinks;
    query->hasTargetSRFs = pstate->p_hasTargetSRFs;

    assign_query_collations(pstate, query);

    return query;
}

/*
 * Function to make a target list from an RTE. Taken from AgensGraph and PG
 */
static List *make_target_list_from_join(ParseState *pstate, RangeTblEntry *rte)
{
    List *targetlist = NIL;
    ListCell *lt;
    ListCell *ln;

    AssertArg(rte->rtekind == RTE_JOIN);

    forboth(lt, rte->joinaliasvars, ln, rte->eref->colnames)
    {
        Var *varnode = (Var*)lfirst(lt);
        char *resname = strVal(lfirst(ln));
        TargetEntry *tmp;

        tmp = makeTargetEntry((Expr *) varnode,
                              (AttrNumber) pstate->p_next_resno++,
                              pstrdup(resname),
                              false);
        targetlist = lappend(targetlist, tmp);
    }

    return targetlist;
}

/*
 * Function to make a target list from an RTE. Borrowed from AgensGraph and PG
 */
static List *makeTargetListFromRTE(ParseState *pstate, RangeTblEntry *rte)
{
    List *targetlist = NIL;
    int rtindex;
    int varattno;
    ListCell *ln;
    ListCell *lt;

    /* right now this is only for subqueries */
    AssertArg(rte->rtekind == RTE_SUBQUERY);

    rtindex = RTERangeTablePosn(pstate, rte, NULL);

    varattno = 1;
    ln = list_head(rte->eref->colnames);
    foreach(lt, rte->subquery->targetList)
    {
        TargetEntry *te = (TargetEntry*)lfirst(lt);
        Var *varnode;
        char *resname;
        TargetEntry *tmp;

        if (te->resjunk)
        {
            continue;
        }

        Assert(varattno == te->resno);

        /* no transform here, just use `te->expr` */
        varnode = makeVar(rtindex, varattno, exprType((Node *) te->expr),
                          exprTypmod((Node *) te->expr),
                          exprCollation((Node *) te->expr), 0);

        resname = strVal(lfirst(ln));

        tmp = makeTargetEntry((Expr *)varnode,
                              (AttrNumber)pstate->p_next_resno++, resname,
                              false);
        targetlist = lappend(targetlist, tmp);

        varattno++;
        ln = lnext(ln);
    }

    return targetlist;
}

/*
 * Transform a cypher sub pattern. This is put here because it is a sub clause.
 * This works in tandem with transform_Sublink in cypher_expr.c
 */
static Query *transform_cypher_sub_pattern(cypher_parsestate *cpstate,
                                           cypher_clause *clause)
{
    cypher_match *match;
    cypher_clause *c;
    Query *qry;
    RangeTblEntry *rte;
    ParseState *pstate = (ParseState *)cpstate;
    cypher_sub_pattern *subpat = (cypher_sub_pattern*)clause->self;

    cypher_parsestate *child_parse_state = make_cypher_parsestate(cpstate);
    ParseState *p_child_parse_state = (ParseState *) child_parse_state;
    p_child_parse_state->p_expr_kind = pstate->p_expr_kind;

    /* create a cypher match node and assign it the sub pattern */
    match = make_ag_node(cypher_match);
    match->pattern = subpat->pattern;
    match->where = NULL;
    /* wrap it in a clause */
    c = (cypher_clause*)palloc(sizeof(cypher_clause));
    c->self = (Node *)match;
    c->prev = NULL;
    c->next = NULL;

    /* set up a select query and run it as a sub query to the parent match */
    qry = makeNode(Query);
    qry->commandType = CMD_SELECT;

    rte = transform_cypher_clause_as_subquery(child_parse_state,
                                              transform_cypher_clause, c,
                                              NULL, true);

    qry->targetList = makeTargetListFromRTE(p_child_parse_state, rte);

    markTargetListOrigins(p_child_parse_state, qry->targetList);

    qry->rtable = p_child_parse_state->p_rtable;
    qry->jointree = makeFromExpr(p_child_parse_state->p_joinlist, NULL);

    /* the state will be destroyed so copy the data we need */
    qry->hasSubLinks = p_child_parse_state->p_hasSubLinks;
    qry->hasAggs = p_child_parse_state->p_hasAggs;

    if (qry->hasAggs)
    {
        parse_check_aggregates(p_child_parse_state, qry);
    }

    assign_query_collations(p_child_parse_state, qry);

    free_cypher_parsestate(child_parse_state);

    return qry;
}

static Query *transform_cypher_sub_query(cypher_parsestate *cpstate,
                                         cypher_clause *clause)
{
    cypher_clause *c;
    Query *qry;
    RangeTblEntry *rte;
    ParseState *pstate = (ParseState *)cpstate;
    cypher_sub_query *sub_query = (cypher_sub_query*)clause->self;
    cypher_parsestate *child_parse_state = make_cypher_parsestate(cpstate);
    ParseState *p_child_parse_state = (ParseState *)child_parse_state;

    p_child_parse_state->p_expr_kind = pstate->p_expr_kind;
    child_parse_state->subquery_where_flag = true;

    c = make_cypher_clause((List *)sub_query->query);

    qry = makeNode(Query);
    qry->commandType = CMD_SELECT;

    rte = transform_cypher_clause_as_subquery(child_parse_state,
                                              transform_cypher_clause, c,
                                              NULL, true);

    qry->targetList = makeTargetListFromRTE(p_child_parse_state, rte);

    markTargetListOrigins(p_child_parse_state, qry->targetList);

    qry->rtable = p_child_parse_state->p_rtable;
    qry->jointree = makeFromExpr(p_child_parse_state->p_joinlist, NULL);

    qry->hasSubLinks = p_child_parse_state->p_hasSubLinks;
    qry->hasAggs = p_child_parse_state->p_hasAggs;

    if (qry->hasAggs) {
        parse_check_aggregates(p_child_parse_state, qry);
    }

    assign_query_collations(p_child_parse_state, qry);

    free_cypher_parsestate(child_parse_state);

    return qry;
}

static Node *make_boolean_test(Node *arg, BoolTestType booltesttype)
{
    BooleanTest *bt = makeNode(BooleanTest);

    bt->arg = (Expr *)arg;
    bt->booltesttype = booltesttype;

    return (Node *)bt;
}

static Node *make_bool_or_agg(ParseState *pstate, Node *arg)
{
    Aggref *agg = makeNode(Aggref);
    TargetEntry *te = makeTargetEntry((Expr *)arg, 1, NULL, false);
    Oid argtypes[1] = {BOOLOID};

    agg->aggfnoid = LookupFuncName(list_make1(makeString("bool_or")), 1,
                                   argtypes, false);
    agg->aggtype = BOOLOID;
    agg->aggcollid = InvalidOid;
    agg->inputcollid = InvalidOid;
    agg->aggtranstype = InvalidOid;
    agg->aggargtypes = list_make1_oid(BOOLOID);
    agg->aggdirectargs = NIL;
    agg->args = list_make1(te);
    agg->aggorder = NIL;
    agg->aggdistinct = NIL;
    agg->aggfilter = NULL;
    agg->aggstar = false;
    agg->aggvariadic = false;
    agg->aggkind = 'n';
    agg->agglevelsup = 0;
    agg->location = -1;

    pstate->p_hasAggs = true;
    return (Node *)agg;
}

static Node *make_count_star_filter_agg(ParseState *pstate, Node *filter)
{
    Aggref *agg = makeNode(Aggref);

    agg->aggfnoid = LookupFuncName(list_make1(makeString("count")), 0, NULL,
                                   false);
    agg->aggtype = INT8OID;
    agg->aggcollid = InvalidOid;
    agg->inputcollid = InvalidOid;
    agg->aggtranstype = InvalidOid;
    agg->aggargtypes = NIL;
    agg->aggdirectargs = NIL;
    agg->args = NIL;
    agg->aggorder = NIL;
    agg->aggdistinct = NIL;
    agg->aggfilter = (Expr *)filter;
    agg->aggstar = true;
    agg->aggvariadic = false;
    agg->aggkind = 'n';
    agg->agglevelsup = 0;
    agg->location = -1;

    pstate->p_hasAggs = true;
    return (Node *)agg;
}

static Node *make_predicate_case_expr(ParseState *pstate, Node *pred,
                                      cypher_predicate_function_kind kind)
{
    CaseExpr *cexpr = makeNode(CaseExpr);
    CaseWhen *when1 = makeNode(CaseWhen);
    CaseWhen *when2 = makeNode(CaseWhen);
    Node *true_const = (Node *)makeBoolConst(true, false);
    Node *false_const = (Node *)makeBoolConst(false, false);
    Node *null_const = (Node *)makeBoolConst(false, true);

    when2->expr = (Expr *)make_bool_or_agg(pstate,
                                           make_boolean_test(pred, IS_UNKNOWN));
    when2->result = (Expr *)null_const;
    when2->location = -1;

    if (kind == CPFK_ALL) {
        when1->expr = (Expr *)make_bool_or_agg(pstate,
                                               make_boolean_test(pred, IS_FALSE));
        when1->result = (Expr *)false_const;
        cexpr->defresult = (Expr *)true_const;
    } else if (kind == CPFK_ANY) {
        when1->expr = (Expr *)make_bool_or_agg(pstate,
                                               make_boolean_test(pred, IS_TRUE));
        when1->result = (Expr *)true_const;
        cexpr->defresult = (Expr *)false_const;
    } else if (kind == CPFK_NONE) {
        when1->expr = (Expr *)make_bool_or_agg(pstate,
                                               make_boolean_test(pred, IS_TRUE));
        when1->result = (Expr *)false_const;
        cexpr->defresult = (Expr *)true_const;
    } else {
        CaseWhen *when3 = makeNode(CaseWhen);
        Node *count_true_ge_two;
        Node *count_true_eq_one;
        Node *last_srf = pstate->p_last_srf;
        Const *two = makeConst(INT8OID, -1, InvalidOid, sizeof(int64),
                               Int64GetDatum(2), false, FLOAT8PASSBYVAL);
        Const *one = makeConst(INT8OID, -1, InvalidOid, sizeof(int64),
                               Int64GetDatum(1), false, FLOAT8PASSBYVAL);

        count_true_ge_two = (Node *)make_op(
            pstate, list_make1(makeString(">=")),
            make_count_star_filter_agg(pstate,
                                       make_boolean_test(pred, IS_TRUE)),
            (Node *)two, last_srf, -1);
        count_true_eq_one = (Node *)make_op(
            pstate, list_make1(makeString("=")),
            make_count_star_filter_agg(pstate,
                                       make_boolean_test(pred, IS_TRUE)),
            (Node *)one, last_srf, -1);

        when1->expr = (Expr *)count_true_ge_two;
        when1->result = (Expr *)false_const;
        when3->expr = (Expr *)count_true_eq_one;
        when3->result = (Expr *)true_const;
        when3->location = -1;

        cexpr->defresult = (Expr *)false_const;
        cexpr->args = list_make3(when1, when2, when3);
    }

    when1->location = -1;
    cexpr->casetype = BOOLOID;
    cexpr->arg = NULL;
    if (cexpr->args == NIL)
        cexpr->args = list_make2(when1, when2);
    cexpr->location = -1;

    return (Node *)cexpr;
}

static Query *transform_cypher_list_comprehension(cypher_parsestate *cpstate,
                                                  cypher_clause *clause)
{
    cypher_list_comprehension *list_comp;
    cypher_parsestate *child_cpstate;
    ParseState *child_pstate;
    Query *query;
    RangeFunction *rf;
    FuncCall *unwind;
    Node *list_expr;
    Node *funcexpr;
    RangeTblEntry *rte;
    RangeTblRef *rtr;
    List *namespaceoid;
    Node *pred;
    Node *result_expr;
    TargetEntry *te;
    int rtindex;
    ParseExprKind old_expr_kind;
    FuncCall *count_call;
    Node *count_expr;
    Node *count_eq_zero;
    Node *last_srf;
    Const *zero;
    CaseExpr *empty_case;
    CaseWhen *empty_when;
    FuncCall *empty_list_func;

    list_comp = (cypher_list_comprehension *)clause->self;
    child_cpstate = make_cypher_parsestate(cpstate);
    child_pstate = (ParseState *)child_cpstate;
    child_pstate->p_expr_kind = ((ParseState *)cpstate)->p_expr_kind;

    query = makeNode(Query);
    query->commandType = CMD_SELECT;

    list_expr = transform_cypher_expr(child_cpstate, list_comp->expr,
                                      EXPR_KIND_SELECT_TARGET);
    unwind = makeFuncCall(list_make1(makeString("age_unnest")), NIL, -1);

    old_expr_kind = child_pstate->p_expr_kind;
    child_pstate->p_expr_kind = EXPR_KIND_SELECT_TARGET;
    funcexpr = ParseFuncOrColumn(child_pstate, unwind->funcname,
                                 list_make1(list_expr),
                                 child_pstate->p_last_srf, unwind,
                                 exprLocation(list_comp->expr), false);
    child_pstate->p_expr_kind = old_expr_kind;

    rf = makeNode(RangeFunction);
    rf->lateral = contain_vars_of_level(funcexpr, 0);
    rf->ordinality = false;
    rf->funccallnode = (Node *)unwind;
    rf->alias = makeAlias(list_comp->varname, NIL);
    rf->coldeflist = NIL;

    rte = addRangeTableEntryForFunction(child_pstate, list_comp->varname,
                                        funcexpr, rf, rf->lateral, true);
    rtindex = list_length(child_pstate->p_rtable);
    Assert(rte == rt_fetch(rtindex, child_pstate->p_rtable));

    rtr = makeNode(RangeTblRef);
    rtr->rtindex = rtindex;
    namespaceoid = list_make1(create_namespace_item(rte, true, true, false, true));
    checkNameSpaceConflicts(child_pstate, child_pstate->p_relnamespace, namespaceoid);
    child_pstate->p_joinlist = lappend(child_pstate->p_joinlist, rtr);
    child_pstate->p_relnamespace = list_concat(child_pstate->p_relnamespace, namespaceoid);
    child_pstate->p_varnamespace = lappend(child_pstate->p_varnamespace,
                                           makeNamespaceItem(rte, true, true));
    setNamespaceLateralState(child_pstate->p_relnamespace, false, true);
    setNamespaceLateralState(child_pstate->p_varnamespace, false, true);

    if (list_comp->where != NULL) {
        pred = transform_cypher_expr(child_cpstate, list_comp->where, EXPR_KIND_WHERE);
        pred = coerce_cypher_expr_to_boolean(child_pstate, pred, "WHERE");
    } else {
        pred = NULL;
    }

    result_expr = transform_cypher_expr(child_cpstate, list_comp->mapping_expr,
                                        EXPR_KIND_SELECT_TARGET);

    count_call = makeFuncCall(list_make1(makeString("count")), NIL, -1);
    count_call->agg_star = true;
    count_expr = transformExpr(child_pstate, (Node *)count_call, EXPR_KIND_SELECT_TARGET);
    zero = makeConst(INT8OID, -1, InvalidOid, sizeof(int64), Int64GetDatum(0), false, FLOAT8PASSBYVAL);
    last_srf = child_pstate->p_last_srf;
    count_eq_zero = (Node *)make_op(child_pstate, list_make1(makeString("=")),
                                    count_expr, (Node *)zero, last_srf, -1);

    empty_list_func = makeFuncCall(list_make2(makeString("ag_catalog"),
                                              makeString("agtype_build_list")),
                                   NIL, -1);

    empty_when = makeNode(CaseWhen);
    empty_when->expr = (Expr *)count_eq_zero;
    empty_when->result = (Expr *)transformExpr(child_pstate, (Node *)empty_list_func,
                                               EXPR_KIND_SELECT_TARGET);
    empty_when->location = -1;

    empty_case = makeNode(CaseExpr);
    empty_case->casetype = AGTYPEOID;
    empty_case->arg = NULL;
    empty_case->args = list_make1(empty_when);
    {
        Aggref *array_agg = makeNode(Aggref);
        TargetEntry *array_te = makeTargetEntry((Expr *)result_expr, 1, NULL,
                                                false);
        Oid array_to_agtype_oid = get_ag_func_oid("agtype_array_to_agtype", 1,
                                                  AGTYPEARRAYOID);

        /*
         * list comprehensions preserve null elements, unlike collect(). Build
         * an agtype[] first so SQL NULL rows survive, then convert the array
         * back to an agtype list at this expression boundary. Bypass function
         * name lookup because openGauss exposes the built-in polymorphic
         * aggregate through a fixed catalog OID; the Aggref below carries the
         * resolved agtype input and agtype[] result types.
         */
        array_agg->aggfnoid = ARRAYAGGFUNCOID;
        array_agg->aggtype = AGTYPEARRAYOID;
        array_agg->aggcollid = InvalidOid;
        array_agg->inputcollid = InvalidOid;
        array_agg->aggtranstype = InvalidOid;
        array_agg->aggargtypes = list_make1_oid(AGTYPEOID);
        array_agg->aggdirectargs = NIL;
        array_agg->args = list_make1(array_te);
        array_agg->aggorder = NIL;
        array_agg->aggdistinct = NIL;
        array_agg->aggfilter = NULL;
        array_agg->aggstar = false;
        array_agg->aggvariadic = false;
        array_agg->aggkind = 'n';
        array_agg->agglevelsup = 0;
        array_agg->location = -1;
        child_pstate->p_hasAggs = true;

        empty_case->defresult = (Expr *)makeFuncExpr(
            array_to_agtype_oid, AGTYPEOID, list_make1(array_agg), InvalidOid,
            InvalidOid, COERCE_EXPLICIT_CALL);
    }
    empty_case->location = -1;

    te = makeTargetEntry((Expr *)empty_case,
                         (AttrNumber)child_pstate->p_next_resno++,
                         "result", false);
    query->targetList = lappend(query->targetList, te);
    query->jointree = makeFromExpr(child_pstate->p_joinlist, pred);

    markTargetListOrigins(child_pstate, query->targetList);
    query->rtable = child_pstate->p_rtable;
    query->hasSubLinks = child_pstate->p_hasSubLinks;
    query->hasTargetSRFs = child_pstate->p_hasTargetSRFs;
    query->hasAggs = true;

    assign_query_collations(child_pstate, query);
    parse_check_aggregates(child_pstate, query);

    free_cypher_parsestate(child_cpstate);
    return query;
}

/*
 * Mutator context for rewriting the fold body's accumulator/element Vars
 * (columns 1 and 2 of the throwaway namespace RTE) into PARAM_EXEC params.
 */
typedef struct reduce_var_param_context {
    int varno; /* rangetable index of the dummy (acc, elem) RTE */
} reduce_var_param_context;

/*
 * Rewrite Var(varno, 1) -> Param(PARAM_EXEC, 0) [accumulator] and
 * Var(varno, 2) -> Param(PARAM_EXEC, 1) [element]. Other Vars and Params are
 * left in place for the capture pass, which assigns them slots 2 ... N.
 */
static Node *reduce_var_to_param_mutator(Node *node, reduce_var_param_context *ctx)
{
    if (node == NULL) {
        return NULL;
    }

    if (IsA(node, Var)) {
        Var *var = (Var *) node;

        /*
         * An outer RTE can share this varno, so only rewrite Vars at the body
         * level. Without the level check an outer column 1/2 could silently
         * become the accumulator or element.
         */
        if (var->varno == ctx->varno && var->varlevelsup == 0 &&
            (var->varattno == REDUCE_ACCUMULATOR_ATTRIBUTE_NUMBER ||
             var->varattno == REDUCE_ELEMENT_ATTRIBUTE_NUMBER)) {
            Param *param = makeNode(Param);

            param->paramkind = PARAM_EXEC;
            param->paramid = var->varattno - 1;
            param->paramtype = AGTYPEOID;
            param->paramtypmod = -1;
            param->paramcollid = InvalidOid;
            param->location = -1;

            return (Node *) param;
        }
    }

    return expression_tree_mutator(node, (Node *(*)(Node *, void *))
                                   reduce_var_to_param_mutator, (void *) ctx);
}

/* A captured subtree must not depend on the per-element fold slots. */
static bool reduce_expr_has_acc_elem(Node *node, void *context)
{
    if (node == NULL) {
        return false;
    }

    if (IsA(node, Param)) {
        Param *param = (Param *) node;

        if (param->paramkind == PARAM_EXEC &&
            (param->paramid == 0 || param->paramid == 1)) {
            return true;
        }
    }

    return expression_tree_walker(node, (bool (*)()) reduce_expr_has_acc_elem,
                                  context);
}

/* Aggregates and windows cannot be evaluated by the standalone fold body. */
static bool reduce_expr_has_aggregate(Node *node, void *context)
{
    if (node == NULL) {
        return false;
    }

    if (IsA(node, Aggref) || IsA(node, GroupingFunc) || IsA(node, WindowFunc)) {
        return true;
    }

    return expression_tree_walker(node, (bool (*)()) reduce_expr_has_aggregate,
                                  context);
}

/* Outer Vars and non-executor Params require an aggregate-argument capture. */
static bool reduce_expr_needs_capture(Node *node, void *context)
{
    if (node == NULL) {
        return false;
    }

    if (IsA(node, Var)) {
        return true;
    }

    if (IsA(node, Param) && ((Param *) node)->paramkind != PARAM_EXEC) {
        return true;
    }

    return expression_tree_walker(node, (bool (*)()) reduce_expr_needs_capture,
                                  context);
}

static bool reduce_expr_has_sublink(Node *node, void *context)
{
    if (node == NULL) {
        return false;
    }

    if (IsA(node, SubLink)) {
        return true;
    }

    return expression_tree_walker(node, (bool (*)()) reduce_expr_has_sublink,
                                  context);
}

/*
 * A graph entity can contain graphid or other non-agtype leaves. Such leaves
 * cannot be elements of agtype[], so capture their smallest agtype wrapper.
 */
static bool reduce_expr_has_nonagtype_outer(Node *node, void *context)
{
    if (node == NULL) {
        return false;
    }

    if (IsA(node, Var) && ((Var *) node)->vartype != AGTYPEOID) {
        return true;
    }

    if (IsA(node, Param)) {
        Param *param = (Param *) node;

        if (param->paramkind != PARAM_EXEC && param->paramtype != AGTYPEOID) {
            return true;
        }
    }

    return expression_tree_walker(node,
                                  (bool (*)()) reduce_expr_has_nonagtype_outer,
                                  context);
}

typedef struct reduce_capture_context {
    int next_slot;  /* PARAM_EXEC 0/1 are accumulator and element */
    List *captured; /* aggregate arguments in slot order */
} reduce_capture_context;

/*
 * Capture only outer leaves so CASE/AND/OR/coalesce and operators remain in the
 * fold body and preserve short-circuit behavior. A subtree containing a
 * non-agtype outer leaf is captured at its smallest agtype-typed wrapper.
 */
static Node *reduce_capture_mutator(Node *node, void *context)
{
    reduce_capture_context *ctx = (reduce_capture_context *) context;

    if (node == NULL) {
        return NULL;
    }

    if (IsA(node, List) || IsA(node, CaseWhen)) {
        return expression_tree_mutator(node,
                                       (Node *(*)(Node *, void *))
                                       reduce_capture_mutator, context);
    }

    if (IsA(node, Aggref) || IsA(node, GroupingFunc) || IsA(node, WindowFunc)) {
        ereport(ERROR,
                (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                 errmsg("aggregate functions are not supported in a reduce() expression")));
    }

    if (exprType(node) == AGTYPEOID &&
        !reduce_expr_has_acc_elem(node, NULL) &&
        !reduce_expr_has_aggregate(node, NULL) &&
        !reduce_expr_has_sublink(node, NULL) &&
        reduce_expr_needs_capture(node, NULL)) {
        if (IsA(node, Var) ||
            (IsA(node, Param) &&
             ((Param *) node)->paramkind != PARAM_EXEC) ||
            reduce_expr_has_nonagtype_outer(node, NULL)) {
            Param *param = makeNode(Param);

            param->paramkind = PARAM_EXEC;
            param->paramid = ctx->next_slot++;
            param->paramtype = AGTYPEOID;
            param->paramtypmod = -1;
            param->paramcollid = InvalidOid;
            param->location = -1;

            ctx->captured = lappend(ctx->captured, copyObject(node));
            return (Node *) param;
        }

        return expression_tree_mutator(node,
                                       (Node *(*)(Node *, void *))
                                       reduce_capture_mutator, context);
    }

    if (IsA(node, SubLink)) {
        ereport(ERROR,
                (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                 errmsg("subqueries (including a nested reduce()) are not supported in a reduce() expression")));
    }

    return expression_tree_mutator(node, (Node *(*)(Node *, void *))
                                   reduce_capture_mutator, context);
}

/*
 * After capture, the standalone body may contain only PARAM_EXEC references.
 * Residual Vars or external Params indicate a value that cannot cross the
 * agtype[] capture boundary.
 */
static bool reduce_body_check_walker(Node *node, void *context)
{
    if (node == NULL) {
        return false;
    }

    if (IsA(node, Var)) {
        ereport(ERROR,
                (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                 errmsg("a reduce() expression references a value that cannot be used in the fold body")));
    }

    if (IsA(node, Param) && ((Param *) node)->paramkind != PARAM_EXEC) {
        ereport(ERROR,
                (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                 errmsg("a reduce() expression references a value that cannot be used in the fold body")));
    }

    if (IsA(node, Aggref) || IsA(node, GroupingFunc) || IsA(node, WindowFunc)) {
        ereport(ERROR,
                (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                 errmsg("aggregate functions are not supported in a reduce() expression")));
    }

    if (IsA(node, SubLink)) {
        ereport(ERROR,
                (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                 errmsg("subqueries (including a nested reduce()) are not supported in a reduce() expression")));
    }

    return expression_tree_walker(node, (bool (*)())reduce_body_check_walker,
                                  context);
}

/*
 * Build a throwaway subquery "SELECT NULL::agtype AS <acc>, NULL::agtype AS
 * <elem>" used only to give the fold body a namespace in which the accumulator
 * and element variables resolve to agtype columns. Those references are later
 * rewritten to PARAM_EXEC params and the subquery is discarded.
 */
static Query *make_reduce_var_subquery(char *acc_name, char *elem_name)
{
    Query *subquery = makeNode(Query);
    Const *acc_const;
    Const *elem_const;
    TargetEntry *acc_te;
    TargetEntry *elem_te;

    acc_const = makeConst(AGTYPEOID, -1, InvalidOid, -1, (Datum) 0, true, false);
    elem_const = makeConst(AGTYPEOID, -1, InvalidOid, -1, (Datum) 0, true, false);

    acc_te = makeTargetEntry((Expr *) acc_const,
                             REDUCE_INIT_TARGET_ENTRY_NUMBER,
                             acc_name, false);
    elem_te = makeTargetEntry((Expr *) elem_const,
                              REDUCE_BODY_TARGET_ENTRY_NUMBER,
                              elem_name, false);

    subquery->commandType = CMD_SELECT;
    subquery->targetList = list_make2(acc_te, elem_te);
    subquery->jointree = makeFromExpr(NIL, NULL);
    subquery->rtable = NIL;

    return subquery;
}

/*
 * Transform a cypher_reduce node into a query tree.
 *
 * reduce(acc = init, var IN list | body) is rewritten into a scalar subquery
 * over the age_reduce aggregate, with the list unnested WITH ORDINALITY and the
 * aggregate ordered by that ordinality so the fold runs in list order:
 *
 *     SELECT ag_catalog.age_reduce(<init>, '<serialized-body>'::text,
 *                                  r.elem, <captured-outer-values>
 *                                  ORDER BY r.ord)
 *     FROM   ag_catalog.age_unnest(<list>) WITH ORDINALITY AS r(elem, ord)
 *
 * The fold body is transformed separately with the accumulator and element
 * rewritten to PARAM_EXEC params 0 and 1 (and any loop-invariant outer
 * reference to params 2 ..), serialized into the text argument, and evaluated
 * per element inside age_reduce_transfn.
 *
 * The null/empty-list guard is built at the grammar/expr level; here we build
 * the aggregate query proper.
 */
static Query *transform_cypher_reduce(cypher_parsestate *cpstate,
                                      cypher_clause *clause)
{
    cypher_reduce *reduce = (cypher_reduce *) clause->self;
    cypher_parsestate *body_cpstate;
    ParseState *body_pstate;
    RangeTblEntry *body_rte;
    int body_rtindex;
    Node *body_node;
    char *body_serialized;
    reduce_var_param_context mutator_ctx;
    reduce_capture_context capture_ctx;
    List *extras_exprs;
    cypher_parsestate *child_cpstate;
    ParseState *child_pstate;
    Query *query = makeNode(Query);
    FuncCall *unwind;
    Node *funcexpr;
    RangeFunction *rf;
    RangeTblEntry *rte;
    RangeTblRef *rtr;
    int rtindex;
    List *namespaceoid;
    Node *list_expr;
    Node *init_node;
    Node *elem_var;
    Node *ord_var;
    TargetEntry *ord_te;
    SortGroupClause *sortcl;
    Oid sort_ltop;
    Oid sort_eqop;
    bool sort_hashable;
    Const *body_const;
    Aggref *agg;
    Oid agg_oid;
    Oid agg_argtypes[4];
    TargetEntry *result_te;
    Node *extras_arg;
    ParseExprKind old_expr_kind;

    /*
     * 1. Resolve the fold body's accumulator and element variables against a
     *    throwaway 2-column agtype subquery, rewrite those Vars to PARAM_EXEC
     *    params 0/1 (and captured outer references to params 2 ..), validate,
     *    and serialize the body for age_reduce_transfn.
     */
    body_cpstate = make_cypher_parsestate(cpstate);
    body_pstate = (ParseState *) body_cpstate;
    body_pstate->p_expr_kind = ((ParseState *) cpstate)->p_expr_kind;

    body_rte = addRangeTableEntryForSubquery(body_pstate,
                                             make_reduce_var_subquery(reduce->accumname,
                                                                      reduce->varname),
                                             makeAlias("reduce_vars",
                                                       list_make2(makeString(reduce->accumname),
                                                                  makeString(reduce->varname))),
                                             false, true);
    body_rtindex = list_length(body_pstate->p_rtable);
    namespaceoid = list_make1(create_namespace_item(body_rte, true, true, false,
                                                    true));
    body_pstate->p_relnamespace = list_concat(body_pstate->p_relnamespace,
                                              namespaceoid);
    body_pstate->p_varnamespace = lappend(body_pstate->p_varnamespace,
                                          makeNamespaceItem(body_rte, false, true));

    body_node = transform_cypher_expr(body_cpstate, reduce->mapping_expr,
                                      EXPR_KIND_SELECT_TARGET);
    /*
     * The accumulator is always an agtype value (the aggregate's stype is
     * agtype). A fold body can legitimately produce a non-agtype scalar (e.g.
     * "s AND x" yields a boolean), so normalize the body to agtype here. A
     * boolean is wrapped in ag_catalog.bool_to_agtype(); any other non-agtype
     * type is coerced through the normal cast machinery.
     */
    if (exprType(body_node) != AGTYPEOID) {
        if (exprType(body_node) == BOOLOID) {
            Oid bool_to_agtype_oid = get_ag_func_oid("bool_to_agtype", 1,
                                                     BOOLOID);

            body_node = (Node *) makeFuncExpr(bool_to_agtype_oid, AGTYPEOID,
                                              list_make1(body_node),
                                              InvalidOid, InvalidOid,
                                              COERCE_EXPLICIT_CALL);
        } else {
            body_node = coerce_to_common_type(body_pstate, body_node,
                                              AGTYPEOID, "reduce");
        }
    }

    mutator_ctx.varno = body_rtindex;
    body_node = reduce_var_to_param_mutator(body_node, &mutator_ctx);

    capture_ctx.next_slot = REDUCE_CAPTURE_FIRST_SLOT;
    capture_ctx.captured = NIL;
    body_node = reduce_capture_mutator(body_node, &capture_ctx);
    extras_exprs = capture_ctx.captured;

    reduce_body_check_walker(body_node, NULL);

    body_serialized = age_node_to_string(body_node);

    /*
     * 2. Build the outer aggregate query:
     *    SELECT age_reduce(<init>, '<body>'::text, r.elem, <extras>
     *                      ORDER BY r.ord)
     *    FROM ag_catalog.age_unnest(<list>) WITH ORDINALITY AS r(elem, ord)
     */
    child_cpstate = make_cypher_parsestate(cpstate);
    child_pstate = (ParseState *) child_cpstate;
    child_pstate->p_expr_kind = ((ParseState *) cpstate)->p_expr_kind;

    query->commandType = CMD_SELECT;

    /* the list expression is evaluated in the child (outer) context */
    list_expr = transform_cypher_expr(child_cpstate, reduce->expr,
                                      EXPR_KIND_SELECT_TARGET);

    unwind = makeFuncCall(list_make2(makeString("ag_catalog"),
        makeString("age_unnest")), NIL, -1);

    old_expr_kind = child_pstate->p_expr_kind;
    child_pstate->p_expr_kind = EXPR_KIND_SELECT_TARGET;
    funcexpr = ParseFuncOrColumn(child_pstate, unwind->funcname,
                                 list_make1(list_expr),
                                 child_pstate->p_last_srf, unwind,
                                 exprLocation(reduce->expr), false);
    child_pstate->p_expr_kind = old_expr_kind;

    rf = makeNode(RangeFunction);
    rf->lateral = contain_vars_of_level(funcexpr, 0);
    rf->ordinality = true;
    rf->funccallnode = (Node *) unwind;
    rf->alias = makeAlias(reduce->varname, NIL);
    rf->coldeflist = NIL;

    rte = addRangeTableEntryForFunction(child_pstate, reduce->varname,
                                        funcexpr, rf, rf->lateral, true);
    rtindex = list_length(child_pstate->p_rtable);
    Assert(rte == rt_fetch(rtindex, child_pstate->p_rtable));

    rtr = makeNode(RangeTblRef);
    rtr->rtindex = rtindex;
    namespaceoid = list_make1(create_namespace_item(rte, true, true, false,
                                                    true));
    checkNameSpaceConflicts(child_pstate, child_pstate->p_relnamespace,
                            namespaceoid);
    child_pstate->p_joinlist = lappend(child_pstate->p_joinlist, rtr);
    child_pstate->p_relnamespace = list_concat(child_pstate->p_relnamespace,
                                               namespaceoid);
    child_pstate->p_varnamespace = lappend(child_pstate->p_varnamespace,
                                           makeNamespaceItem(rte, true, true));
    setNamespaceLateralState(child_pstate->p_relnamespace, false, true);
    setNamespaceLateralState(child_pstate->p_varnamespace, false, true);

    /* argument 1: init, evaluated once (in the outer/child context) */
    init_node = transform_cypher_expr(child_cpstate, reduce->initial,
                                      EXPR_KIND_SELECT_TARGET);

    /* argument 3: the per-element value column from the WITH ORDINALITY RTE */
    elem_var = colNameToVar(child_pstate, reduce->varname, false, -1);

    /* argument 2: serialized fold body */
    body_const = makeConst(TEXTOID, -1, InvalidOid, -1,
                           CStringGetTextDatum(body_serialized), false, false);

    /*
     * The WITH ORDINALITY column (bigint), column 2 of the function RTE, used
     * only to order the fold.
     */
    ord_var = (Node *) makeVar(rtindex, REDUCE_ELEMENT_ATTRIBUTE_NUMBER,
                               INT8OID, -1, InvalidOid, 0);
    get_sort_group_operators(INT8OID, true, true, false,
                             &sort_ltop, &sort_eqop, NULL, &sort_hashable);

    ord_te = makeTargetEntry((Expr *) ord_var,
                             REDUCE_ORDINALITY_TARGET_ATTRIBUTE_NUMBER,
                             NULL, true);
    ord_te->ressortgroupref = 1;

    sortcl = makeNode(SortGroupClause);
    sortcl->tleSortGroupRef = 1;
    sortcl->eqop = sort_eqop;
    sortcl->sortop = sort_ltop;
    sortcl->nulls_first = false;
    sortcl->hashable = sort_hashable;

    /*
     * Aggregate arguments are evaluated for every input row, while the
     * transition function only consumes init on its first call. Gate init by
     * ordinality so volatile or expensive expressions run exactly once.
     */
    {
        OpExpr *ord_is_first = makeNode(OpExpr);
        Const *one_const = makeConst(INT8OID, -1, InvalidOid, sizeof(int64),
                                     Int64GetDatum(1), false,
                                     FLOAT8PASSBYVAL);
        CaseWhen *init_when = makeNode(CaseWhen);
        CaseExpr *init_case = makeNode(CaseExpr);
        Const *null_init = makeConst(AGTYPEOID, -1, InvalidOid, -1,
                                     (Datum) 0, true, false);

        ord_is_first->opno = sort_eqop;
        ord_is_first->opfuncid = get_opcode(sort_eqop);
        ord_is_first->opresulttype = BOOLOID;
        ord_is_first->opretset = false;
        ord_is_first->opcollid = InvalidOid;
        ord_is_first->inputcollid = InvalidOid;
        ord_is_first->args = list_make2(copyObject(ord_var), one_const);
        ord_is_first->location = -1;

        init_when->expr = (Expr *) ord_is_first;
        init_when->result = (Expr *) init_node;
        init_when->location = -1;

        init_case->casetype = AGTYPEOID;
        init_case->casecollid = InvalidOid;
        init_case->arg = NULL;
        init_case->args = list_make1(init_when);
        init_case->defresult = (Expr *) null_init;
        init_case->location = -1;

        init_node = (Node *) init_case;
    }

    /*
     * Captures are aggregate arguments in the same order as PARAM_EXEC slots
     * 2..N. An empty ArrayExpr represents a body with no outer references.
     */
    {
        ArrayExpr *arr = makeNode(ArrayExpr);

        arr->array_typeid = AGTYPEARRAYOID;
        arr->element_typeid = AGTYPEOID;
        arr->elements = extras_exprs;
        arr->multidims = false;
        arr->location = -1;
        extras_arg = (Node *) arr;
    }

    /* look up the age_reduce(agtype, text, agtype, agtype[]) aggregate */
    agg_argtypes[REDUCE_INIT_ARGUMENT_INDEX] = AGTYPEOID;
    agg_argtypes[REDUCE_BODY_ARGUMENT_INDEX] = TEXTOID;
    agg_argtypes[REDUCE_ELEMENT_ARGUMENT_INDEX] = AGTYPEOID;
    agg_argtypes[REDUCE_EXTRAS_ARGUMENT_INDEX] = get_array_type(AGTYPEOID);
    agg_oid = LookupFuncName(list_make2(makeString("ag_catalog"),
                                        makeString("age_reduce")),
                             REDUCE_AGGREGATE_ARGUMENT_COUNT, agg_argtypes,
                             false);

    agg = makeNode(Aggref);
    agg->aggfnoid = agg_oid;
    agg->aggtype = AGTYPEOID;
    agg->aggcollid = InvalidOid;
    agg->inputcollid = InvalidOid;
    agg->aggtranstype = InvalidOid;     /* filled by the planner */
    agg->aggargtypes = list_make4_oid(AGTYPEOID, TEXTOID, AGTYPEOID,
                                      get_array_type(AGTYPEOID));
    agg->aggdirectargs = NIL;
    agg->args = list_make4(
        makeTargetEntry((Expr *) init_node, REDUCE_INIT_TARGET_ENTRY_NUMBER,
                        NULL, false),
        makeTargetEntry((Expr *) body_const, REDUCE_BODY_TARGET_ENTRY_NUMBER,
                        NULL, false),
        makeTargetEntry((Expr *) elem_var,
                        REDUCE_ELEMENT_TARGET_ENTRY_NUMBER,
                        NULL, false),
        makeTargetEntry((Expr *) extras_arg,
                        REDUCE_EXTRAS_TARGET_ENTRY_NUMBER,
                        NULL, false));
    agg->args = lappend(agg->args, ord_te);
    agg->aggorder = list_make1(sortcl);
    agg->aggdistinct = NIL;
    agg->aggfilter = NULL;
    agg->aggstar = false;
    agg->aggvariadic = false;
    agg->aggkind = 'n';
    agg->agglevelsup = 0;
    agg->location = -1;

    child_pstate->p_hasAggs = true;

    result_te = makeTargetEntry((Expr *) agg,
                                (AttrNumber) child_pstate->p_next_resno++,
                                "reduce", false);

    query->targetList = list_make1(result_te);
    query->jointree = makeFromExpr(child_pstate->p_joinlist, NULL);
    query->rtable = child_pstate->p_rtable;
    query->hasAggs = true;
    query->hasSubLinks = child_pstate->p_hasSubLinks;
    query->hasTargetSRFs = child_pstate->p_hasTargetSRFs;

    assign_query_collations(child_pstate, query);
    parse_check_aggregates(child_pstate, query);

    free_cypher_parsestate(body_cpstate);
    free_cypher_parsestate(child_cpstate);

    return query;
}

static Query *transform_cypher_predicate_function(cypher_parsestate *cpstate,
                                                  cypher_clause *clause)
{
    cypher_predicate_function *pred_func;
    cypher_parsestate *child_cpstate;
    ParseState *child_pstate;
    Query *query;
    RangeFunction *rf;
    FuncCall *unwind;
    Node *list_expr;
    Node *funcexpr;
    RangeTblEntry *rte;
    RangeTblRef *rtr;
    List *namespaceoid;
    Node *pred;
    TargetEntry *te;
    int rtindex;
    ParseExprKind old_expr_kind;

    pred_func = (cypher_predicate_function *)clause->self;
    child_cpstate = make_cypher_parsestate(cpstate);
    child_pstate = (ParseState *)child_cpstate;
    child_pstate->p_expr_kind = ((ParseState *)cpstate)->p_expr_kind;

    query = makeNode(Query);
    query->commandType = CMD_SELECT;

    list_expr = transform_cypher_expr(cpstate, pred_func->expr,
                                      EXPR_KIND_SELECT_TARGET);
    IncrementVarSublevelsUp(list_expr, 1, 0);
    unwind = makeFuncCall(list_make1(makeString("age_unnest")), NIL, -1);

    old_expr_kind = child_pstate->p_expr_kind;
    child_pstate->p_expr_kind = EXPR_KIND_SELECT_TARGET;
    funcexpr = ParseFuncOrColumn(child_pstate, unwind->funcname,
                                 list_make1(list_expr),
                                 child_pstate->p_last_srf, unwind,
                                 exprLocation(pred_func->expr), false);
    child_pstate->p_expr_kind = old_expr_kind;

    rf = makeNode(RangeFunction);
    rf->lateral = true;
    rf->ordinality = false;
    rf->funccallnode = (Node *)unwind;
    rf->alias = makeAlias(pred_func->varname, NIL);
    rf->coldeflist = NIL;

    rte = addRangeTableEntryForFunction(child_pstate, pred_func->varname,
                                        funcexpr, rf, rf->lateral, true);
    rtindex = list_length(child_pstate->p_rtable);
    Assert(rte == rt_fetch(rtindex, child_pstate->p_rtable));

    rtr = makeNode(RangeTblRef);
    rtr->rtindex = rtindex;
    namespaceoid = list_make1(create_namespace_item(rte, true, true, false,
                                                    true));
    checkNameSpaceConflicts(child_pstate, child_pstate->p_relnamespace,
                            namespaceoid);
    child_pstate->p_joinlist = lappend(child_pstate->p_joinlist, rtr);
    child_pstate->p_relnamespace = list_concat(child_pstate->p_relnamespace,
                                               namespaceoid);
    child_pstate->p_varnamespace = lappend(child_pstate->p_varnamespace,
                                           makeNamespaceItem(rte, true, true));
    setNamespaceLateralState(child_pstate->p_relnamespace, false, true);
    setNamespaceLateralState(child_pstate->p_varnamespace, false, true);

    pred = transform_cypher_expr(child_cpstate, pred_func->where,
                                 EXPR_KIND_WHERE);
    pred = coerce_cypher_expr_to_boolean(child_pstate, pred, "WHERE");

    {
        Node *case_expr = make_predicate_case_expr(child_pstate, pred,
                                                   pred_func->kind);
        te = makeTargetEntry((Expr *)case_expr,
                             (AttrNumber)child_pstate->p_next_resno++,
                             "result", false);
        query->targetList = lappend(query->targetList, te);
        query->jointree = makeFromExpr(child_pstate->p_joinlist, NULL);
    }

    markTargetListOrigins(child_pstate, query->targetList);
    query->rtable = child_pstate->p_rtable;
    query->hasSubLinks = child_pstate->p_hasSubLinks;
    query->hasTargetSRFs = child_pstate->p_hasTargetSRFs;
    query->hasAggs = child_pstate->p_hasAggs;

    assign_query_collations(child_pstate, query);
    if (child_pstate->p_hasAggs || query->groupClause || query->groupingSets ||
        query->havingQual) {
        parse_check_aggregates(child_pstate, query);
    }

    free_cypher_parsestate(child_cpstate);
    return query;
}

/*
 * Code borrowed and inspired by PG's transformFromClauseItem. This function
 * will transform the VLE function, depending on type. Currently, only
 * RangeFunctions are supported. But, others may be in the future.
 */

/*
 * Code borrowed and inspired by PG's transformFromClauseItem. Static function
 * to add in the VLE function as a FROM clause entry.
 */

static void transform_match_pattern(cypher_parsestate *cpstate, Query *query,
                                    List *pattern, Node *where)
{
    ParseState *pstate = (ParseState *)cpstate;
    ListCell *lc;
    List *quals = NIL;
    Expr *q = NULL;
    Expr *expr = NULL;

    /*
     * Loop through a comma separated list of paths like (u)-[e]-(v), (w), (x)
     */
    foreach (lc, pattern)
    {
        List *qual = NULL;
        cypher_path *path = NULL;

        /* get the path and transform it */
        path = (cypher_path *) lfirst(lc);

        qual = transform_match_path(cpstate, query, path);

        quals = list_concat(quals, qual);
    }

    if (quals != NIL)
    {
        q = makeBoolExpr(AND_EXPR, quals, -1);
        expr = (Expr *)ag_transformExpr(&cpstate->pstate, (Node *)q, EXPR_KIND_WHERE);
    }

    if (cpstate->property_constraint_quals != NIL)
    {
        Expr *prop_qual = makeBoolExpr(AND_EXPR,
                                       cpstate->property_constraint_quals, -1);

        if (quals == NULL)
        {
            expr = prop_qual;
        }
        else
        {
            expr = makeBoolExpr(AND_EXPR, list_make2(expr, prop_qual), -1);
        }
    }

    // transform the where clause quals and add to the quals,
    if (where != NULL)
    {
        Expr *where_qual;

        where_qual = (Expr *)transform_cypher_expr(cpstate, where,
                                                   EXPR_KIND_WHERE);
        if (expr == NULL)
        {
            expr = where_qual;
        }
        else
        {
            where_qual = (Expr *)coerce_cypher_expr_to_boolean(pstate,
                                                               (Node *)where_qual,
                                                               "WHERE");
            expr = makeBoolExpr(AND_EXPR, list_make2(expr, where_qual), -1);
        }
    }

    /*
     * Coerce to WHERE clause to a bool, denoting whether the constructed
     * clause is true or false.
     */
    if (expr != NULL)
    {
        expr = (Expr *)coerce_cypher_expr_to_boolean(pstate, (Node *)expr,
                                                     "WHERE");
    }

    query->rtable = cpstate->pstate.p_rtable;
    query->jointree = makeFromExpr(cpstate->pstate.p_joinlist, (Node *)expr);
}

static char *get_next_default_alias(cypher_parsestate *cpstate)
{
    ParseState *pstate = (ParseState *)cpstate;
    cypher_parsestate *parent_cpstate =
        (cypher_parsestate *)pstate->parentParseState;
    char *alias_name;
    int nlen = 0;

    /* Anonymous names must be unique across clause subqueries. */
    if (parent_cpstate != NULL) {
        return get_next_default_alias(parent_cpstate);
    }

    /* get the length of the combinded string */
    nlen = snprintf(NULL, 0, "%s%d", AGE_DEFAULT_ALIAS_PREFIX,
                    cpstate->default_alias_num);

    /* allocate the space */
    alias_name = (char*)palloc0(nlen + 1);

    /* create the name */
    snprintf(alias_name, nlen + 1, "%s%d", AGE_DEFAULT_ALIAS_PREFIX,
             cpstate->default_alias_num);

    /* increment the default alias number */
    cpstate->default_alias_num++;

    return alias_name;
}

/*
 * Creates a FuncCall node that will prevent an edge from being joined
 * to twice.
 */
static FuncCall *prevent_duplicate_edges(cypher_parsestate *cpstate,
                                         List *entities)
{
    List *edges = NIL;
    ListCell *lc;
    List *qualified_function_name;
    Value *ag_catalog, *edge_fn;

    bool is_vle_edge = false;

    ag_catalog = makeString("ag_catalog");

    // iterate through each entity, collecting the access node for each edge
    foreach (lc, entities)
    {
        transform_entity *entity = (transform_entity*)lfirst(lc);
        Node *edge;

        if (entity->type == ENT_EDGE)
        {
            edge = make_qual(cpstate, entity, AG_EDGE_COLNAME_ID);

            edges = lappend(edges, edge);
        }
        else if (entity->type == ENT_VLE_EDGE)
        {
            is_vle_edge = true;
            edges = lappend(edges, entity->expr); 
        }
    }

    /*
     * Fast path: for a fixed 2/3/4 non-VLE edge count, use the specialized
     * pairwise-comparison functions which take plain graphid arguments and
     * skip the general variadic-agtype hashtable path. A VLE edge or any other
     * count falls back to the general _ag_enforce_edge_uniqueness.
     */
    if (!is_vle_edge &&
        list_length(edges) >= EDGE_UNIQUENESS_MIN_EDGE_COUNT &&
        list_length(edges) <= EDGE_UNIQUENESS_MAX_EDGE_COUNT) {
        if (list_length(edges) == EDGE_UNIQUENESS_MIN_EDGE_COUNT)
        {
            edge_fn = makeString("_ag_enforce_edge_uniqueness2");
        }
        else if (list_length(edges) == EDGE_UNIQUENESS_MIN_EDGE_COUNT + 1)
        {
            edge_fn = makeString("_ag_enforce_edge_uniqueness3");
        } else {
            edge_fn = makeString("_ag_enforce_edge_uniqueness4");
        }
    } else {
        edge_fn = makeString("_ag_enforce_edge_uniqueness");
    }

    qualified_function_name = list_make2(ag_catalog, edge_fn);

    return makeFuncCall(qualified_function_name, edges, -1);
}

/*
 * For any given edge, the previous entity is joined with the edge
 * via the prev_qual node, and the next entity is join with the
 * next_qual node. If there is a filter on the previous vertex label,
 * create a filter, same with the next node.
 */
static List *make_directed_edge_join_conditions(
    cypher_parsestate *cpstate, transform_entity *prev_entity,
    transform_entity *next_entity, Node *prev_qual, Node *next_qual,
    char *prev_node_filter, char *next_node_filter)
{
    List *quals = NIL;

    if (prev_entity->in_join_tree)
    {
        quals = list_concat(quals, join_to_entity(cpstate, prev_entity,
                                                  prev_qual, JOIN_SIDE_LEFT));
    }

    if (next_entity->in_join_tree && next_entity->type != ENT_VLE_EDGE)
    {
        quals = list_concat(quals, join_to_entity(cpstate, next_entity,
                                                  next_qual, JOIN_SIDE_RIGHT));
    }
    else if (next_entity->in_join_tree)
    {
        quals = list_concat(quals, join_to_entity(cpstate, next_entity,
                                                  next_qual, JOIN_SIDE_RIGHT));
    }

    if (prev_node_filter != NULL && !IS_DEFAULT_LABEL_VERTEX(prev_node_filter))
    {
        A_Expr *qual;
        qual = filter_vertices_on_label_id(cpstate, prev_qual,
                                           prev_node_filter);

        quals = lappend(quals, qual);
    }

    if (next_node_filter != NULL && !IS_DEFAULT_LABEL_VERTEX(next_node_filter))
    {
        A_Expr *qual;
        qual = filter_vertices_on_label_id(cpstate, next_qual,
                                           next_node_filter);

        quals = lappend(quals, qual);
    }

    return quals;
}
/*
 * The joins are driven by edges. Under specific conditions, it becomes
 * necessary to have knowledge about the previous edge and vertex and
 * the next vertex and edge.
 *
 * [prev_edge]-(prev_node)-[edge]-(next_node)-[next_edge]
 *
 * prev_edge and next_edge are allowed to be null.
 * prev_node and next_node are not allowed to be null.
 */
static Node *
getColumnVar(cypher_parsestate *cpstate, transform_entity *edge, char *colname)
{
    ParseState *pstate = (ParseState *)cpstate;
	
    if (edge->expr != NULL && IsA(edge->expr, Var))
    {
        if (edge->type == ENT_VLE_EDGE)
        {
            ListCell *lcn;
            AttrNumber attrno;
            Var *var;
            Var *edgesVar = (Var *)edge->expr;
            Index rtindex = edgesVar->varno;
            RangeTblEntry *rte = rt_fetch(rtindex, pstate->p_rtable);

            attrno = 1;
            foreach (lcn, rte->eref->colnames)
            {
                const char *tmp = strVal(lfirst(lcn));

                if (strcmp(tmp, colname) == 0)
                {
                    /*
                     * No ambiguous reference check is needed because the VLE
                     * subquery target names are unique.
                     */
                    var = make_var(pstate, rte, attrno, -1);
                    markVarForSelectPriv(pstate, var, rte);

                    return (Node *)var;
                }

                attrno++;
            }

            ereport(ERROR,
                    (errcode(ERRCODE_INTERNAL_ERROR),
                     errmsg("VLE endpoint column \"%s\" is missing from subquery \"%s\"",
                            colname,
                            rte->eref != NULL &&
                                    rte->eref->aliasname != NULL ?
                                rte->eref->aliasname : "<unnamed>")));
        }
        else
        {
            List *qualified_name, *args;
            Node *node;
            char *function_name;

            function_name = get_accessor_function_name(edge->type, colname);

            qualified_name = list_make2(makeString("ag_catalog"),
                                        makeString(function_name));
            ColumnRef *cr = makeNode(ColumnRef);
            cr->fields = list_make1(makeString(edge->entity.rel->name));
        
            args = list_make1(cr);
            node = (Node *)makeFuncCall(qualified_name, args, -1);

            Node *graphId = (Node *)makeFuncCall(
                list_make2(makeString("ag_catalog"),
                           makeString("agtype_to_graphid")),
                list_make1(node), -1);
            return graphId;
        }
    }
    else
    {
        Node * node;
        char *entity_name = NULL;
        ColumnRef *cr = makeNode(ColumnRef);

        if (edge->type == ENT_EDGE || edge->type == ENT_VLE_EDGE)
        {
            entity_name = edge->entity.rel->name;
        }else  if (edge->type == ENT_VERTEX)
        {
            entity_name = edge->entity.node->name;
        }
        else
        {
            ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                            errmsg("unknown entity type")));
        }

        cr->fields = list_make2(makeString(entity_name), makeString(colname));
        node = (Node *)cr;
        return node;
    }

    return NULL;
}
/*
 * Builds "vertex.id = edge.start_id OR vertex.id = edge.end_id" for an
 * undirected relationship, or returns NULL when the entity is not a vertex
 * that is already bound in the join tree.
 *
 * Restricting this to ENT_VERTEX matters: in a multi-edge path the adjacent
 * entity can be the neighbouring edge, and reading a vertex "id" column off an
 * edge would be wrong.
 */
static Expr *make_undirected_endpoint_prefilter(cypher_parsestate *cpstate,
                                                transform_entity *entity,
                                                Node *start_id_expr,
                                                Node *end_id_expr)
{
    Node *vertex_id;
    A_Expr *matches_start;
    A_Expr *matches_end;

    if (entity == NULL || entity->type != ENT_VERTEX || !entity->in_join_tree) {
        return NULL;
    }

    vertex_id = make_qual(cpstate, entity, AG_VERTEX_COLNAME_ID);

    matches_start = makeSimpleA_Expr(AEXPR_OP, "=",
                                     (Node *)copyObject(vertex_id),
                                     (Node *)copyObject(start_id_expr), -1);
    matches_end = makeSimpleA_Expr(AEXPR_OP, "=", vertex_id,
                                   (Node *)copyObject(end_id_expr), -1);

    return makeBoolExpr(OR_EXPR, list_make2(matches_start, matches_end), -1);
}

static List *make_join_condition_for_edge(cypher_parsestate *cpstate,
                                          transform_entity *prev_edge,
                                          transform_entity *prev_node,
                                          transform_entity *entity,
                                          transform_entity *next_node,
                                          transform_entity *next_edge)
{
    char *next_label_name_to_filter = NULL;
    char *prev_label_name_to_filter = NULL;
    transform_entity *next_entity;
    transform_entity *prev_entity;

    /*
     * VLE execution exposes logical start/end graphid columns directly.
     * Join them with adjacent vertices, or with the previous VLE endpoint,
     * using ordinary equality quals. This avoids per-row path
     * materialization and the legacy terminal/two-VLE helper functions.
     */
    if (entity->type == ENT_VLE_EDGE) 
    {
        List *quals = NIL;

        if (!next_node->in_join_tree) {
            return NIL;
        }

        if (prev_node && cpstate->p_vle_initial_vid == NULL) {
            Node *id = make_qual(cpstate, prev_node, AG_VERTEX_COLNAME_ID);
            Node *vid = getColumnVar(
                cpstate, entity, getEdgeColname(entity->entity.rel, false));
            A_Expr *expr = makeSimpleA_Expr(AEXPR_OP, "=", id, vid, -1);
            quals = lappend(quals, expr);
        }

        if (next_node) {
            Node *id = make_qual(cpstate, next_node, AG_VERTEX_COLNAME_ID);
            Node *vid = getColumnVar(
                cpstate, entity, getEdgeColname(entity->entity.rel, true));
            A_Expr *expr = makeSimpleA_Expr(AEXPR_OP, "=", id, vid, -1);
            quals = lappend(quals, expr);
        }

        if (prev_edge) {
            Node *prev_vid = NULL;
            prev_vid = getColumnVar(
                cpstate, prev_edge,
                getEdgeColname(prev_edge->entity.rel, true));
            Node *vid = getColumnVar(
                cpstate, entity, getEdgeColname(entity->entity.rel, false));
            A_Expr *expr =
                makeSimpleA_Expr(AEXPR_OP, "=", prev_vid, vid, -1);
            quals = lappend(quals, expr);
        }

        return quals;
    }
    /*
     *  If the previous node is not in the join tree, set the previous
     *  label filter.
     */
    if (!prev_node->in_join_tree)
    {
        prev_label_name_to_filter = prev_node->entity.node->label;
    }

    /*
     * If the next node is not in the join tree and there is not
     * another edge, set the label filter. When there is another
     * edge, we don't need to set it, because that edge will set the
     * filter for that node.
     */
    if (!next_node->in_join_tree && next_edge == NULL)
    {
        next_label_name_to_filter = next_node->entity.node->label;
    }

    /*
     * When the previous node is not in the join tree, and there
     * is a previous edge, set the previous entity to that edge.
     * Otherwise, use the previous node/
     */
    if (!prev_node->in_join_tree && prev_edge != NULL)
    {
        prev_entity = prev_edge;
    }
    else
    {
        prev_entity = prev_node;
    }

    /*
     * When the next node is not in the join tree, and there
     * is a next edge, set the next entity to that edge.
     * Otherwise, use the next node.
     */
    if (!next_node->in_join_tree && next_edge != NULL)
    {
        next_entity = next_edge;
    }
    else
    {
        next_entity = next_node;
    }

    switch (entity->entity.rel->dir)
    {
        case CYPHER_REL_DIR_RIGHT:
        {
            Node *prev_qual = make_qual(cpstate, entity,
                                            AG_EDGE_COLNAME_START_ID);
            Node *next_qual = make_qual(cpstate, entity,
                                            AG_EDGE_COLNAME_END_ID);

            return make_directed_edge_join_conditions(cpstate, prev_entity,
                                                      next_node, prev_qual,
                                                      next_qual,
                                                      prev_label_name_to_filter,
                                                      next_label_name_to_filter);
        }
        case CYPHER_REL_DIR_LEFT:
        {
            Node *prev_qual = make_qual(cpstate, entity,
                                            AG_EDGE_COLNAME_END_ID);
            Node *next_qual = make_qual(cpstate, entity,
                                            AG_EDGE_COLNAME_START_ID);

            return make_directed_edge_join_conditions(cpstate, prev_entity,
                                                      next_node, prev_qual,
                                                      next_qual,
                                                      prev_label_name_to_filter,
                                                      next_label_name_to_filter);
        }
        case CYPHER_REL_DIR_NONE:
        {
            /*
             * For undirected relationships, we can use the left directed
             * relationship OR'd by the right directed relationship.
             */
            Node *start_id_expr = make_qual(cpstate, entity,
                                                AG_EDGE_COLNAME_START_ID);
            Node *end_id_expr = make_qual(cpstate, entity,
                                              AG_EDGE_COLNAME_END_ID);
            List *first_join_quals = NIL, *second_join_quals = NIL;
            List *quals = NIL;
            Expr *first_qual, *second_qual;
            Expr *or_qual;
            Expr *prev_prefilter;
            Expr *next_prefilter;

            first_join_quals = make_directed_edge_join_conditions(cpstate,
                                                                  prev_entity,
                                                                  next_entity,
                                                                  start_id_expr,
                                                                  end_id_expr,
                                                                  prev_label_name_to_filter,
                                                                  next_label_name_to_filter);

            second_join_quals = make_directed_edge_join_conditions(cpstate,
                                                                   prev_entity,
                                                                   next_entity,
                                                                   end_id_expr,
                                                                   start_id_expr,
                                                                   prev_label_name_to_filter,
                                                                   next_label_name_to_filter);

            first_qual = makeBoolExpr(AND_EXPR, first_join_quals, -1);
            second_qual = makeBoolExpr(AND_EXPR, second_join_quals, -1);

            or_qual = makeBoolExpr(OR_EXPR, list_make2(first_qual, second_qual),
                               -1);

            quals = list_make1(or_qual);

            /*
             * The OR above spans both endpoint columns, so when only one side
             * of the relationship is bound the optimizer cannot turn it into an
             * index access on start_id/end_id and scans the whole edge table.
             * Adding the redundant (but indexable) "this vertex is one of the
             * two endpoints" filter recovers the index path. It is implied by
             * the OR, so results are unchanged.
             *
             * Restrict this to single-relationship patterns. In a longer path
             * the surrounding relationships already constrain both endpoints,
             * so the extra OR qual adds no index path and measurably degrades
             * the chosen plan (LDBC SQ7).
             */
            if (prev_edge == NULL && next_edge == NULL) {
                prev_prefilter = make_undirected_endpoint_prefilter(
                    cpstate, prev_entity, start_id_expr, end_id_expr);
                if (prev_prefilter != NULL) {
                    quals = lappend(quals, prev_prefilter);
                }

                next_prefilter = next_entity != prev_entity ?
                    make_undirected_endpoint_prefilter(cpstate, next_entity,
                                                       start_id_expr,
                                                       end_id_expr) :
                    NULL;
                if (next_prefilter != NULL) {
                    quals = lappend(quals, next_prefilter);
                }
            }

            return quals;
        }
        default:
            return NULL;
    }
}

// creates a type cast node to agtype

/*
 * Makes an agtype bool node that Postgres' transform expression logic
 * can handle. Used when contructed the join quals for building the paths
 */

/*
 * For the given entity, join it to the current edge, via the passed
 * qual node. The side denotes if the entity is on the right
 * or left of the current edge. Which we will need to know if the
 * passed entity is a directed edge.
 */
static List *join_to_entity(cypher_parsestate *cpstate,
                            transform_entity *entity, Node *qual,
                            enum transform_entity_join_side side)
{
    ParseState *pstate = (ParseState *)cpstate;
    A_Expr *expr;
    List *quals = NIL;

    if (entity->type == ENT_VERTEX)
    {
        Node *id_qual = make_qual(cpstate, entity, AG_EDGE_COLNAME_ID);

        expr = makeSimpleA_Expr(AEXPR_OP, "=", (Node *)qual, (Node *)id_qual,
                                -1);

        quals = lappend(quals, expr);
    }
    else if (entity->type == ENT_EDGE)
    {
        List *edge_quals = make_edge_quals(cpstate, entity, side);

        if (list_length(edge_quals) > 1)
        {
            expr = makeSimpleA_Expr(AEXPR_IN, "=", (Node *)qual,
                                    (Node *)edge_quals, -1);
        }
        else
        {
            expr = makeSimpleA_Expr(AEXPR_OP, "=", (Node *)qual,
                                    (Node*)linitial(edge_quals), -1);
        }

        quals = lappend(quals, expr);
    }
    else if (entity->type == ENT_VLE_EDGE)
    {
        Node *endpoint;

        if (side == JOIN_SIDE_LEFT)
        {
            /* [vle_edge]-()-[regular_edge]: join the VLE end endpoint. */
            endpoint = getColumnVar(
                cpstate, entity, getEdgeColname(entity->entity.rel, true));
        }
        else if (side == JOIN_SIDE_RIGHT)
        {
            /* [regular_edge]-()-[vle_edge]: join the VLE start endpoint. */
            endpoint = getColumnVar(
                cpstate, entity, getEdgeColname(entity->entity.rel, false));
        }
        else
        {
            ereport(ERROR,
                    (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                     errmsg("unknown join side found"),
                     parser_errposition(pstate, entity->entity.rel->location)));
        }

        expr = makeSimpleA_Expr(AEXPR_OP, "=", (Node *)qual, endpoint, -1);
        quals = lappend(quals, expr);
    }
    else
    {
        ereport(ERROR,
                (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                 errmsg("unknown entity type to join to")));
    }

    return quals;
}

// makes the quals neccessary when an edge is joining to another edge.
static List *make_edge_quals(cypher_parsestate *cpstate,
                             transform_entity *edge,
                             enum transform_entity_join_side side)
{
    ParseState *pstate = (ParseState *)cpstate;
    char *left_dir;
    char *right_dir;

    Assert(edge->type == ENT_EDGE);

    /*
     * When the rel is on the left side in a pattern, then a left directed path
     * is concerned with the start id and a right directed path is concerned
     * with the end id. When the rel is on the right side of a pattern, the
     * above statement is inverted.
     */
    switch (side)
    {
        case JOIN_SIDE_LEFT:
        {
            left_dir = AG_EDGE_COLNAME_START_ID;
            right_dir = AG_EDGE_COLNAME_END_ID;
            break;
        }
        case JOIN_SIDE_RIGHT:
        {
            left_dir = AG_EDGE_COLNAME_END_ID;
            right_dir = AG_EDGE_COLNAME_START_ID;
            break;
        }
        default:
            ereport(ERROR,
                    (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                     errmsg("unknown join type found"),
                     parser_errposition(pstate, edge->entity.rel->location)));
    }

    switch (edge->entity.rel->dir)
    {
        case CYPHER_REL_DIR_LEFT:
        {
            return list_make1(make_qual(cpstate, edge, left_dir));
        }
        case CYPHER_REL_DIR_RIGHT:
        {
            return list_make1(make_qual(cpstate, edge, right_dir));
        }
        case CYPHER_REL_DIR_NONE:
        {
            return list_make2(make_qual(cpstate, edge, left_dir),
                              make_qual(cpstate, edge, right_dir));
        }
        default:
            ereport(ERROR,
                    (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                     errmsg("Unknown relationship direction")));
    }
    return NIL;
}

/*
 * Creates a node that will create a filter on the passed field node
 * that removes all labels that do not have the same label_id
 */
static A_Expr *filter_vertices_on_label_id(cypher_parsestate *cpstate,
                                           Node *id_field, char *label)
{
    label_cache_data *lcd = search_label_name_graph_cache(label,
                                                          cpstate->graph_oid);
    A_Const *n;
    FuncCall *fc;
    Value *ag_catalog, *extract_label_id;
    int32 label_id = lcd->id;

    n = makeNode(A_Const);
    n->val.type = T_Integer;
    n->val.val.ival = label_id;
    n->location = -1;

    ag_catalog = makeString("ag_catalog");
    extract_label_id = makeString("_extract_label_id");
    fc = makeFuncCall(list_make2(ag_catalog, extract_label_id),
                      list_make1(id_field), -1);

    return makeSimpleA_Expr(AEXPR_OP, "=", (Node *)fc, (Node *)n, -1);
}

static transform_entity *make_transform_entity(cypher_parsestate *cpstate,
                                               enum transform_entity_type type,
                                               Node *node, Expr *expr)
{
    transform_entity *entity;
    entity = (transform_entity*)palloc(sizeof(transform_entity));

    entity->type = type;
    if (type == ENT_VERTEX)
    {
        entity->entity.node = (cypher_node *)node;
    }
    else if (entity->type == ENT_EDGE || entity->type == ENT_VLE_EDGE)
    {
        entity->entity.rel = (cypher_relationship *)node;
    }
    else if (entity->type == ENT_PATH)
    {
        entity->entity.path = (cypher_path *)node;
    }
    else
    {
        ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                        errmsg("unknown entity type")));
    }

    entity->declared_in_current_clause = true;
    entity->expr = expr;
    entity->vle_vertices = NULL;
    entity->in_join_tree = expr != NULL;

    return entity;
}

transform_entity *find_variable(cypher_parsestate *cpstate, char *name)
{
    ListCell *lc;

    foreach (lc, cpstate->entities)
    {
        transform_entity *entity = (transform_entity*)lfirst(lc);
        char *entity_name = nullptr;

        if (entity->type == ENT_VERTEX)
        {
            entity_name = entity->entity.node->name;
        }
        else if (entity->type == ENT_EDGE || entity->type == ENT_VLE_EDGE)
        {
            entity_name = entity->entity.rel->name;
        }
        else if (entity->type == ENT_PATH)
        {
            entity_name = entity->entity.path->var_name;
        }
        else
        {
            ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                            errmsg("unknown entity type")));
        }

        if (entity_name != NULL && !strcmp(name, entity_name))
        {
            return entity;
        }
    }

    return NULL;
}

static transform_entity *find_parent_variable(cypher_parsestate *cpstate,
                                              char *name, Index *levelsup)
{
    ParseState *pstate = (ParseState *)cpstate;
    ParseState *parent_pstate = pstate->parentParseState;
    Index level = 1;

    while (parent_pstate != NULL && level <= MAX_PARENT_PARSE_LEVEL) {
        cypher_parsestate *parent_cpstate =
            (cypher_parsestate *)parent_pstate;
        transform_entity *entity = find_variable(parent_cpstate, name);

        if (entity != NULL) {
            *levelsup = level;
            return entity;
        }

        parent_pstate = parent_pstate->parentParseState;
        level++;
    }

    return NULL;
}

static bool has_list_comp_or_subquery_walker(Node *expr, void *context)
{
    ListCell *lc;

    if (expr == NULL) {
        return false;
    }

    if (IsA(expr, List)) {
        foreach (lc, (List *)expr)
        {
            if (has_list_comp_or_subquery((Node *)lfirst(lc))) {
                return true;
            }
        }

        return false;
    }

    if (IsA(expr, BoolExpr)) {
        BoolExpr *bexpr = (BoolExpr *)expr;

        return has_list_comp_or_subquery((Node *)bexpr->args);
    }

    if (IsA(expr, EXTENSIBLE_NODE)) {
        if (is_ag_node(expr, cypher_list_comprehension) ||
            is_ag_node(expr, cypher_sub_query)) {
            return true;
        }

        if (is_ag_node(expr, cypher_map)) {
            return has_list_comp_or_subquery(
                (Node *)((cypher_map *)expr)->keyvals);
        }

        if (is_ag_node(expr, cypher_list)) {
            return has_list_comp_or_subquery(
                (Node *)((cypher_list *)expr)->elems);
        }

        if (is_ag_node(expr, cypher_map_projection)) {
            cypher_map_projection *projection = (cypher_map_projection *)expr;

            return has_list_comp_or_subquery((Node *)projection->map_var) ||
                   has_list_comp_or_subquery((Node *)projection->map_elements);
        }

        if (is_ag_node(expr, cypher_comparison_aexpr)) {
            cypher_comparison_aexpr *aexpr = (cypher_comparison_aexpr *)expr;

            return has_list_comp_or_subquery(aexpr->lexpr) ||
                   has_list_comp_or_subquery(aexpr->rexpr);
        }

        if (is_ag_node(expr, cypher_comparison_boolexpr)) {
            cypher_comparison_boolexpr *bexpr =
                (cypher_comparison_boolexpr *)expr;

            return has_list_comp_or_subquery((Node *)bexpr->args);
        }

        return false;
    }

    return raw_expression_tree_walker(expr,
                                      (bool (*)())has_list_comp_or_subquery_walker,
                                      context);
}

static bool has_list_comp_or_subquery(Node *expr)
{
    return has_list_comp_or_subquery_walker(expr, NULL);
}

static Node *coerce_cypher_expr_to_boolean(ParseState *pstate, Node *expr,
                                           const char *construct_name)
{
    if (expr != NULL && IsA(expr, BoolExpr)) {
        return expr;
    }

    return coerce_to_boolean(pstate, expr, construct_name);
}

static char *get_entity_name(transform_entity *entity)
{
    if (entity->type == ENT_EDGE || entity->type == ENT_VLE_EDGE)
    {
        return entity->entity.rel->name;
    }
    else if (entity->type == ENT_VERTEX)
    {
        return entity->entity.node->name;
    }
    else if (entity->type == ENT_PATH)
    {
        return entity->entity.path->var_name;
    }
    else
    {
        ereport(ERROR,
                (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                        errmsg("cannot get entity name from transform_entity type %i", entity->type)));
    }

    return NULL;
}
/*
 * Return true when a property constraint can be decomposed at parse time.
 */
static bool is_nonempty_static_cypher_map(Node *property_constraints)
{
    cypher_map *property_map;

    if (property_constraints == NULL ||
        nodeTag(property_constraints) != T_EXTENSIBLE_NODE ||
        !is_ag_node(property_constraints, cypher_map)) {
        return false;
    }

    property_map = (cypher_map *)property_constraints;
    return property_map->keyvals != NIL;
}

/*
 * Recursively decompose a static property map into scalar equality and
 * collection containment conditions.
 */
static List *create_property_constraint_quals(cypher_parsestate *cpstate,
                                              transform_entity *entity,
                                              cypher_map *property_map,
                                              List *parent_fields)
{
    ParseState *pstate = (ParseState *)cpstate;
    List *quals = NIL;
    ListCell *key_cell;

    check_stack_depth();
    Assert(property_map->keyvals != NIL);

    key_cell = list_head(property_map->keyvals);
    while (key_cell != NULL) {
        ListCell *value_cell = lnext(key_cell);
        Node *key = (Node *)lfirst(key_cell);
        Node *value;
        char *property_name;

        Assert(value_cell != NULL);
        if (value_cell == NULL) {
            ereport(ERROR,
                    (errcode(ERRCODE_INTERNAL_ERROR),
                     errmsg("property map contains an unmatched key")));
        }

        value = (Node *)lfirst(value_cell);
        Assert(IsA(key, String));
        property_name = strVal(key);

        if (is_nonempty_static_cypher_map(value)) {
            List *nested_parent_fields;
            List *nested_quals;

            nested_parent_fields = lappend(list_copy(parent_fields),
                                           makeString(property_name));
            nested_quals = create_property_constraint_quals(
                cpstate, entity, (cypher_map *)value, nested_parent_fields);
            quals = list_concat(quals, nested_quals);
        } else {
            ColumnRef *entity_ref = makeNode(ColumnRef);
            A_Indirection *property_ref = makeNode(A_Indirection);
            Node *lhs;
            Node *rhs;
            Node *qual;
            Node *last_srf = pstate->p_last_srf;
            char *operator_name;

            entity_ref->fields =
                list_make1(makeString(get_entity_name(entity)));
            entity_ref->location = -1;

            property_ref->arg = (Node *)entity_ref;
            property_ref->indirection =
                lappend(list_copy(parent_fields), makeString(property_name));

            lhs = transform_cypher_expr(cpstate, (Node *)property_ref,
                                        EXPR_KIND_WHERE);
            rhs = transform_cypher_expr(cpstate, value, EXPR_KIND_WHERE);
            operator_name =
                nodeTag(value) == T_EXTENSIBLE_NODE &&
                        (is_ag_node(value, cypher_list) ||
                         is_ag_node(value, cypher_map))
                    ? (char *)"@>"
                    : (char *)"=";

            qual = (Node *)make_op(pstate,
                                   list_make1(makeString(operator_name)),
                                   lhs, rhs, last_srf, -1);
            quals = lappend(quals, qual);
        }

        key_cell = lnext(value_cell);
    }

    return quals;
}

/*
 * Create a function to handle property constraints on an edge/vertex.
 */
static Node *create_property_constraint_function(cypher_parsestate *cpstate,
                                                 transform_entity *entity,
                                                 Node *property_constraints,
                                                 Node *prop_expr)
{
    ParseState *pstate = (ParseState *)cpstate;
    char *entity_name;
    bool use_equals;
    Node *const_expr;
    RangeTblEntry *rte;
    Node *last_srf = pstate->p_last_srf;
    char *operator_name;

    if (prop_expr == NULL) {
        ColumnRef *cr = makeNode(ColumnRef);

        entity_name = get_entity_name(entity);
        cr->fields = list_make2(makeString(entity_name),
                                makeString("properties"));

        // use Postgres to get the properties' transform node
        if ((rte = find_rte(cpstate, entity_name)))
        {
            prop_expr = scanRTEForColumn(pstate, rte,
                                         AG_VERTEX_COLNAME_PROPERTIES,
                                         -1, false);
        }
        else
        {
            prop_expr = transformExpr(pstate, (Node *)cr, EXPR_KIND_WHERE);
        }
    }

    use_equals =
        (entity->type == ENT_VERTEX && entity->entity.node->use_equals) ||
        ((entity->type == ENT_EDGE || entity->type == ENT_VLE_EDGE) &&
         entity->entity.rel->use_equals);

    if (!age_enable_containment && !use_equals &&
        is_nonempty_static_cypher_map(property_constraints)) {
        List *quals = create_property_constraint_quals(
            cpstate, entity, (cypher_map *)property_constraints, NIL);

        Assert(quals != NIL);
        if (list_length(quals) == 1)
        {
            return (Node *)linitial(quals);
        }

        return (Node *)makeBoolExpr(AND_EXPR, quals, -1);
    }

    // use cypher to get the constraints' transform node
    const_expr = transform_cypher_expr(cpstate, property_constraints,
                                       EXPR_KIND_WHERE);
    operator_name = use_equals ? (char *)"@>>" : (char *)"@>";

    return (Node *)make_op(pstate,
                           list_make1(makeString(operator_name)),
                           prop_expr,
                           const_expr, last_srf, -1);
}

/*
 * For the given path, transform each entity within the path, create
 * the path variable if needed, and construct the quals to enforce the
 * correct join tree, and enforce edge uniqueness.
 */
static List *transform_match_path(cypher_parsestate *cpstate, Query *query,
                                  cypher_path *path)
{
    List *qual = NIL;
    List *entities = NIL;
    FuncCall *duplicate_edge_qual;
    List *join_quals;

    // transform the entities in the path
    entities = transform_match_entities(cpstate, query, path);

    // create the path variable, if needed.
    if (path->var_name != NULL)
    {
        TargetEntry *path_te;

        path_te = transform_match_create_path_variable(cpstate, path,
                                                       entities);
        query->targetList = lappend(query->targetList, path_te);
    }

    // construct the quals for the join tree
    join_quals = make_path_join_quals(cpstate, entities);
    qual = list_concat(qual, join_quals);

    // construct the qual to prevent duplicate edges
    if (list_length(entities) > 3)
    {
        duplicate_edge_qual = prevent_duplicate_edges(cpstate, entities);
        qual = lappend(qual, duplicate_edge_qual);
    }

    return qual;
}
/*
 * from postgresql parse_sub_analyze
 * Modified entry point for recursively analyzing a sub-statement in union.
 */

static RangeTblEntry *
transformVLEtoRTE(cypher_parsestate *cpstate, SelectStmt *vle, Alias *alias)
{
	ParseState *pstate = NULL;
    /* set the pstate */
    pstate = &cpstate->pstate;
	Query	   *qry;
	RangeTblEntry *rte;

	Assert(!pstate->p_lateral_active);

	/* make the RTE temporarily visible */
	if (cpstate->p_vle_initial_nsitem != NULL)
	{
		cpstate->p_vle_initial_nsitem->p_lateral_ok = true;
	}

	pstate->p_lateral_active = true;
	pstate->p_expr_kind = EXPR_KIND_FROM_SUBSELECT;

	qry = parse_sub_analyze((Node *) vle, pstate, NULL,
							isLockedRefname(pstate, alias->aliasname), true);
	Assert(qry->commandType == CMD_SELECT);

	pstate->p_lateral_active = false;
	pstate->p_expr_kind = EXPR_KIND_NONE;

	if (cpstate->p_vle_initial_nsitem)
		cpstate->p_vle_initial_nsitem->p_lateral_ok = true;

	rte = addRangeTableEntryForSubquery(pstate, qry, alias, true, true);

     /* function is like a plain relation */
    RangeTblRef* rtr = makeNodeFast(RangeTblRef);
    int rtindex = RTERangeTablePosn(pstate, rte, NULL);
    

    rtr->rtindex = rtindex;
    List *namespaceoid = list_make1(create_namespace_item(rte, true, true, true,
                                                    true));
       /* verify there aren't any conflicts */
    checkNameSpaceConflicts(pstate, pstate->p_relnamespace, namespaceoid);

    // /* mark the new namespace items as visible only to LATERAL */
    setNamespaceLateralState(namespaceoid, true, true);

    /* add the entry to the joinlist and namespace */
    pstate->p_joinlist = lappend(pstate->p_joinlist, rtr);
    pstate->p_relnamespace = list_concat(pstate->p_relnamespace, namespaceoid);
    pstate->p_varnamespace = lappend(pstate->p_varnamespace, makeNamespaceItem(rte, true, true));
    // /* make all namespace items unconditionally visible */
    setNamespaceLateralState(pstate->p_relnamespace, false, true);
    setNamespaceLateralState(pstate->p_varnamespace, false, true);
	return rte;
}
/*
 * SELECT id, start, "end", properties, ctid, start AS _start, "end" AS _end
 * FROM `get_graph_path()`.`edge_label`
 * UNION ALL
 * SELECT id, start, "end", properties, ctid, "end" AS _start, start AS _end
 * FROM `get_graph_path()`.`edge_label`
 */
static Node *
genEdgeUnion(cypher_parsestate *cpstate,char *edge_label, bool only, int location)
{
	ResTarget  *id;
	ResTarget  *start;
	ResTarget  *end;
	ResTarget  *prop_map;
	ResTarget  *tid;
	RangeVar   *r;
	SelectStmt *lsel;
	SelectStmt *rsel;
	SelectStmt *u;

	id = makeSimpleResTarget(AG_EDGE_COLNAME_ID, NULL);
	start = makeSimpleResTarget(AG_EDGE_COLNAME_START_ID, NULL);
	end = makeSimpleResTarget(AG_EDGE_COLNAME_END_ID, NULL);
	prop_map = makeSimpleResTarget(AG_EDGE_COLNAME_PROPERTIES, NULL);
	tid = makeSimpleResTarget("ctid", NULL);
    char *schema_name = get_graph_namespace_name(cpstate->graph_name);
	r = makeRangeVar(schema_name, edge_label, location);
	r->inhOpt = INH_YES;

	lsel = makeNode(SelectStmt);
	lsel->targetList =lcons(id, list_make4(start, end, prop_map, tid));
	lsel->fromClause = list_make1(r);

	rsel = (SelectStmt*)copyObject(lsel);

	lsel->targetList = lappend(lsel->targetList,
							   makeSimpleResTarget(AG_EDGE_COLNAME_START_ID,
												   EDGE_VLE_UNION_START_ID));
	lsel->targetList = lappend(lsel->targetList,
							   makeSimpleResTarget(AG_EDGE_COLNAME_END_ID,
												   EDGE_VLE_UNION_END_ID));

	rsel->targetList = lappend(rsel->targetList,
							   makeSimpleResTarget(AG_EDGE_COLNAME_END_ID,
												   AG_EDGE_COLNAME_START_ID));
	rsel->targetList = lappend(rsel->targetList,
							   makeSimpleResTarget(AG_EDGE_COLNAME_START_ID,
												   EDGE_VLE_UNION_END_ID));

	u = makeNode(SelectStmt);
	u->op = SETOP_UNION;
	u->all = true;
	u->larg = lsel;
	u->rarg = rsel;

	return (Node *) u;
}
/*
 * UNION ALL the relation whose OID is `parentoid` and its child relations.
 *
 * SELECT id, start, "end", properties, ctid FROM `r`
 * UNION ALL
 * SELECT id, start, "end", properties, ctid FROM edge
 * ...
 */
static RangeSubselect *
genInhEdge( cypher_parsestate* cpstate ,RangeVar *r, Oid parentoid)
{
	ResTarget  *id;
	ResTarget  *start;
	ResTarget  *end;
	ResTarget  *prop_map;
	ResTarget  *tid;
	SelectStmt *sel;
	SelectStmt *lsel;
	List	   *children;
	ListCell   *lc;
	RangeSubselect *sub;

	id = makeSimpleResTarget(AG_EDGE_COLNAME_ID, NULL);
	start = makeSimpleResTarget(AG_EDGE_COLNAME_START_ID, NULL);
	end = makeSimpleResTarget(AG_EDGE_COLNAME_END_ID, NULL);
	prop_map = makeSimpleResTarget(AG_EDGE_COLNAME_PROPERTIES, NULL);
	tid = makeSimpleResTarget("ctid", NULL);

	sel = makeNode(SelectStmt);
	sel->targetList = lcons(id, list_make4(start, end, prop_map, tid));
	sel->fromClause = list_make1(r);
	lsel = sel;

	children = find_inheritance_children(parentoid, AccessShareLock);
	foreach(lc, children)
	{
		Oid			childoid = lfirst_oid(lc);
		Relation	childrel;
		RangeVar   *childrv;
		SelectStmt *rsel;
		SelectStmt *u;

		childrel = heap_open(childoid, AccessShareLock);
        char *schema_name = get_graph_namespace_name(cpstate->graph_name);
		childrv = makeRangeVar(schema_name,  RelationGetRelationName(childrel), -1);
		childrv->inhOpt = INH_YES;

		heap_close(childrel, AccessShareLock);

		rsel = (SelectStmt *) copyObject(sel);
		rsel->fromClause = list_delete_first(rsel->fromClause);
		rsel->fromClause = list_make1(childrv);

		u = makeNode(SelectStmt);
		u->op = SETOP_UNION;
		u->all = true;
		u->larg = lsel;
		u->rarg = rsel;

		lsel = u;
	}

	sub = makeNode(RangeSubselect);
	sub->subquery = (Node *) lsel;

	return sub;
}

static void
getCypherRelType(cypher_relationship *crel, char **typname)
{ 

	if (crel->label == NULL || strlen(crel->label) ==0)
	{
		*typname = AG_DEFAULT_LABEL_EDGE;
	}
	else
	{
           *typname = crel->label;

	}
}
static Node *
genVLEEdgeSubselect(cypher_parsestate *cpstate, cypher_relationship *crel, char *aliasname)
{
	ParseState *pstate = NULL;
    /* set the pstate */
    pstate = &cpstate->pstate;
    char	   *typname;
	Alias	   *alias;
	Node	   *edge;

	getCypherRelType(crel, &typname);
	alias = makeAliasNoDup(aliasname, NIL);

	if (crel->dir == CYPHER_REL_DIR_NONE)
	{
		RangeSubselect *sub;

		/* id, start, "end", properties, ctid, _start, _end */
		sub = makeNode(RangeSubselect);
		sub->subquery = genEdgeUnion(cpstate,typname, false, -1);
		sub->alias = alias;
		edge = (Node *) sub;
	}
	else
	{
		RangeVar   *r;
		LOCKMODE	lockmode;
		Relation	rel;

        char *schema_name = get_graph_namespace_name(cpstate->graph_name);
        char *rel_name = get_label_relation_name(crel->label?crel->label:AG_DEFAULT_LABEL_EDGE, cpstate->graph_oid);
        r = makeRangeVar(schema_name, rel_name, -1);
		r->inhOpt = INH_YES;

		if (isLockedRefname(pstate, aliasname))
			lockmode = RowShareLock;
		else
			lockmode = AccessShareLock;

		rel = parserOpenTable(pstate, r, lockmode);

		/* id, start, "end", properties, ctid */
        if (has_subclass(rel->rd_id))
		{
			RangeSubselect *sub;

			r->inhOpt = INH_NO;
			sub = genInhEdge(cpstate ,r, rel->rd_id);
			sub->alias = alias;
			edge = (Node *) sub;
		}
		else
		{
			r->alias = alias;
			edge = (Node *) r;
		}
 

		heap_close(rel, NoLock);
	}

	return edge;
}

static char *
getEdgeColname(cypher_relationship *crel, bool prev)
{
	if (prev)
	{
		if (crel->dir == CYPHER_REL_DIR_NONE)
			return EDGE_VLE_UNION_END_ID;
		else if (crel->dir == CYPHER_REL_DIR_LEFT)
			return AG_EDGE_COLNAME_START_ID;
		else
			return AG_EDGE_COLNAME_END_ID;
	}
	else
	{
		if (crel->dir == CYPHER_REL_DIR_NONE)
			return EDGE_VLE_UNION_START_ID;
		else if (crel->dir == CYPHER_REL_DIR_LEFT)
			return AG_EDGE_COLNAME_END_ID;
		else
			return AG_EDGE_COLNAME_START_ID;
	}
}
static List* genQualifiedName(char* name1, char* name2)
{
    if (name1 == NULL)
        return list_make1(makeString(name2));
    else
        return list_make2(makeString(name1), makeString(name2));
}
Node* makeColumnRef(List* fields)
{
    ColumnRef* n = makeNode(ColumnRef);

    n->fields = fields;
    n->location = -1;
    return (Node*)n;
}

static ResTarget* makeResTarget(Node* val, char* name)
{
    ResTarget* res;

    res = makeNode(ResTarget);
    if (name != NULL)
        res->name = pstrdup(name);
    res->val = val;
    res->location = -1;

    return res;
}
static Node *
genEdgeSimple(char *aliasname,cypher_parsestate *cpstate)
{
	Node	   *id;
	Node	   *start;
	Node	   *end;
	Node	   *prop_map;

    List *args, *label_name_args;
    FuncCall *func_expr;
    FuncCall *label_name_func_expr;
    Const *graph_oid_const;
    Oid func_oid;

	id = makeColumnRef(genQualifiedName(aliasname, AG_EDGE_COLNAME_ID));
	start = makeColumnRef(genQualifiedName(aliasname, AG_EDGE_COLNAME_START_ID));
	end = makeColumnRef(genQualifiedName(aliasname, AG_EDGE_COLNAME_END_ID));
	prop_map = makeColumnRef(genQualifiedName(aliasname, AG_EDGE_COLNAME_PROPERTIES));

    func_oid = get_ag_func_oid("_agtype_build_edge", 5, GRAPHIDOID, GRAPHIDOID,
                               GRAPHIDOID, AGTYPEOID, AGTYPEOID);

    graph_oid_const = makeConst(OIDOID, -1, InvalidOid, sizeof(Oid),
                                ObjectIdGetDatum(cpstate->graph_oid), false,
                                true);

    label_name_args = list_make2(graph_oid_const, id);

    label_name_func_expr = makeFuncCall(list_make2(makeString("ag_catalog"),
                                   makeString("_label_name")),
                                        label_name_args, -1);                                    
    label_name_func_expr->location = -1;
    args=lcons(id, list_make4(start, end, label_name_func_expr, prop_map));

    func_expr = makeFuncCall(list_make2(makeString("ag_catalog"),
                                   makeString("_agtype_build_edge")), args, -1);
    func_expr->location = -1;

    return (Node *)func_expr;
}

static SelectStmt *
genVLESubselect(cypher_parsestate *cpstate, cypher_relationship *crel,bool pathout)
{
	ParseState *pstate = NULL;
    /* set the pstate */
    pstate = &cpstate->pstate;
    char	   *prev_colname;
	Node	   *prev_col;
	ResTarget  *prev;
	char	   *curr_colname;
	Node	   *curr_col;
	ResTarget  *curr;
	Node	   *ids_col;
	ResTarget  *ids;
	List	   *tlist;
	Node	   *left;
	SelectStmt *sel;
    bool out = true ;

	prev_colname = getEdgeColname(crel, false);
	prev_col = makeColumnRef(genQualifiedName(VLE_LEFT_ALIAS, prev_colname));
	prev = makeResTarget(prev_col, prev_colname);

	curr_colname = getEdgeColname(crel, true);
	curr_col = makeColumnRef(genQualifiedName(VLE_LEFT_ALIAS, curr_colname));
	curr = makeResTarget(curr_col, curr_colname);

	ids_col = makeColumnRef(genQualifiedName(VLE_LEFT_ALIAS, VLE_COLNAME_IDS));
	ids = makeResTarget(ids_col, VLE_COLNAME_IDS);

	tlist = list_make3(prev, curr, ids);

	if (out)
	{
		Node	   *edges_col;
		ResTarget  *edges;

		edges_col = makeColumnRef(genQualifiedName(VLE_LEFT_ALIAS,
												   VLE_COLNAME_EDGES));
		edges = makeResTarget(edges_col, VLE_COLNAME_EDGES);

		tlist = lappend(tlist, edges);
	}

	if (pathout)
	{
		Node       *vertices_col;
		ResTarget  *vertices;

		vertices_col = makeColumnRef(genQualifiedName(VLE_LEFT_ALIAS,
      VLE_COLNAME_VERTICES));
		vertices = makeResTarget(vertices_col, VLE_COLNAME_VERTICES);
		tlist = lappend(tlist, vertices);
	}

	left = genVLELeftChild(cpstate, crel, out,pathout);

	sel = makeNode(SelectStmt);
	sel->targetList = tlist;
	sel->fromClause = list_make1(left);

	return sel;

}

static bool
isZeroLengthVLE(cypher_relationship *crel)
{

	if (crel == NULL)
		return false;

	if (crel->varlen == NULL)
		return false;

	return true;
}
/* same as makeAlias() but no pstrdup(aliasname) */
static Alias*
makeAliasNoDup(char* aliasname, List* colnames)
{
    Alias* alias;

    alias = makeNode(Alias);
    alias->aliasname = aliasname;
    alias->colnames = colnames;

    return alias;
}
static ResTarget*
makeSimpleResTarget(char* field, char* name)
{
    ColumnRef* cref;

    cref = makeNode(ColumnRef);
    cref->fields = list_make1(makeString(pstrdup(field)));
    cref->location = -1;

    return makeResTarget((Node*)cref, name);
}
static Node *
genVLEQual(char *alias, Node *propMap,cypher_parsestate * cpstate)
{
	ColumnRef  *prop;
	A_Expr	   *propcond;

	prop = makeNode(ColumnRef);
	prop->fields = genQualifiedName(alias, AG_EDGE_COLNAME_PROPERTIES);
	prop->location = -1;

    Node *   const_expr = transform_cypher_expr(cpstate, propMap,
                                       EXPR_KIND_EXECUTE_PARAMETER);

	propcond = makeSimpleA_Expr(AEXPR_OP, "@>", (Node *) prop,const_expr,
								-1);

	return (Node *) propcond;
}

/*
 * CYPHER_REL_DIR_NONE
 *
 *     SELECT _start, _end, ARRAY[id] AS ids,
 *            ARRAY[(id, start, "end", properties, ctid)::edge] AS edges
 *            ARRAY[NULL::vertex] AS vertices
 *     FROM <edge label with additional _start and _end columns> AS l
 *     WHERE <outer vid> = _start AND l.properties @> ...)
 *
 * CYPHER_REL_DIR_LEFT
 *
 *     SELECT "end", start, ARRAY[id] AS ids,
 *            ARRAY[(id, start, "end", properties, ctid)::edge] AS edges
 *            ARRAY[NULL::vertex] AS vertices
 *     FROM <edge label (and its children)> AS l
 *     WHERE <outer vid> = "end" AND l.properties @> ...)
 *
 * CYPHER_REL_DIR_RIGHT
 *
 *     SELECT start, "end", ARRAY[id] AS ids,
 *            ARRAY[(id, start, "end", properties, ctid)::edge] AS edges
 *            ARRAY[NULL::vertex] AS vertices
 *     FROM <edge label (and its children)> AS l
 *     WHERE <outer vid> = start AND l.properties @> ...)
 *
 * If `isZeroLengthVLE(crel)`, then
 *
 *     CYPHER_REL_DIR_NONE
 *
 *         VALUES (<outer vid>, <outer vid>, ARRAY[]::graphid,
 *                 ARRAY[]::_edge, ARRAY[]::_vertex)
 *         AS l(_start, _end, ids, edges, vertices)
 *
 *     CYPHER_REL_DIR_LEFT
 *
 *         VALUES (<outer vid>, <outer vid>, ARRAY[]::graphid,
 *                 ARRAY[]::_edge, ARRAY[]::_vertices)
 *         AS l("end", start, ids, edges, vertices)
 *
 *     CYPHER_REL_DIR_RIGHT
 *
 *         VALUES (<outer vid>, <outer vid>, ARRAY[]::graphid,
 *                 ARRAY[]::_edge, ARRAY[]::_vertices)
 *         AS l(start, "end", ids, edges, vertices)
 */
static Node * makeAArrayExpr(List *elements, Oid typeOid)
{
	A_ArrayExpr *arr;
	TypeCast   *cast;

	arr = makeNode(A_ArrayExpr);
	arr->elements = elements;
	arr->location = -1;

	cast = makeNode(TypeCast);
	cast->arg = (Node *) arr;
	cast->typname = makeTypeNameFromOid(typeOid, -1);
	cast->location = -1;

	return (Node *) cast;
}
static Node *
makeAArrayExpr(List *elements)
{
	A_ArrayExpr *n = makeNode(A_ArrayExpr);

	n->elements = elements;
	n->location = -1;
	return (Node *) n;
}
static Node *
genVLELeftChild(cypher_parsestate *cpstate, cypher_relationship *crel, bool out,bool pathout)
{
	ParseState *pstate = NULL;
    /* set the pstate */
    pstate = &cpstate->pstate;
    Node	   *vid;
	List	   *colnames = NIL;
	SelectStmt *sel;
	RangeSubselect *sub;

	/*
	 * `vid` is NULL only if
	 * (there is no previous edge of the vertex in the path
	 *  and the vertex is transformed first time in the pattern)
	 * and `crel` is not zero-length
	 */
	vid = cpstate->p_vle_initial_vid;

	if (isZeroLengthVLE(crel) && vid !=NULL)
	{
		Node	   *ids;
		List	   *values;

		Assert(vid != NULL);

		ids = makeAArrayExpr(NIL,GRAPHIDARRAYOID);

		values = list_make3(vid, vid, ids);
		colnames = list_make3(makeString(getEdgeColname(crel, false)),
							  makeString(getEdgeColname(crel, true)),
							  makeString(VLE_COLNAME_IDS));

		if (out)
		{
			Node	   *edge_arr = makeAArrayExpr(NIL,AGTYPEARRAYOID);

			values = lappend(values, edge_arr);
			colnames = lappend(colnames, makeString(VLE_COLNAME_EDGES));
		}

		if (pathout)
		{
			Node	   *vtxarr = makeAArrayExpr(NIL,AGTYPEARRAYOID);

			values = lappend(values, vtxarr);
			colnames = lappend(colnames, makeString(VLE_COLNAME_VERTICES));
		}

		sel = makeNode(SelectStmt);
		sel->valuesLists = list_make1(values);
	}
	else
	{
		List	   *prev_colname;
		Node	   *prev_col;
		ResTarget  *prev;
		ResTarget  *curr;
		Node	   *id;
		Node	   *id_array;
		ResTarget  *ids;
		List	   *tlist = NIL;
		Node	   *from;
		List	   *where_args = NIL;

		prev_colname = genQualifiedName(NULL, getEdgeColname(crel, false));
		prev_col = makeColumnRef(prev_colname);
		prev = makeResTarget(prev_col, NULL);
		from = genVLEEdgeSubselect(cpstate, crel, VLE_LEFT_ALIAS);

		if (vid == NULL)
		{
			Node *curr_from_start = makeColumnRef(
       genQualifiedName(NULL, getEdgeColname(crel, false)));
			Node *empty_ids = makeAArrayExpr(NIL, GRAPHIDARRAYOID);

			curr = makeResTarget(curr_from_start,
       getEdgeColname(crel, true));
			ids = makeResTarget(empty_ids, VLE_COLNAME_IDS);
			tlist = list_make3(prev, curr, ids);

			if (out) {
				Node *empty_edges = makeAArrayExpr(NIL, AGTYPEARRAYOID);
				ResTarget *edges = makeResTarget(empty_edges,
        VLE_COLNAME_EDGES);

				tlist = lappend(tlist, edges);
			}

			if (pathout) {
				Node *empty_vertices = makeAArrayExpr(NIL, AGTYPEARRAYOID);
				ResTarget *vertices = makeResTarget(
        empty_vertices, VLE_COLNAME_VERTICES);

				tlist = lappend(tlist, vertices);
			}
		} else {
			A_Expr *vidcond;

			curr = makeSimpleResTarget(getEdgeColname(crel, true), NULL);
			id = makeColumnRef(genQualifiedName(NULL, "id"));
			id_array = makeAArrayExpr(list_make1(id));
			ids = makeResTarget((Node *) id_array, VLE_COLNAME_IDS);
			tlist = list_make3(prev, curr, ids);

			if (out) {
				Node *edge_arr = makeAArrayExpr(
        list_make1(genEdgeSimple(VLE_LEFT_ALIAS, cpstate)));
				ResTarget *edges = makeResTarget(edge_arr,
        VLE_COLNAME_EDGES);

				tlist = lappend(tlist, edges);
			}

			if (pathout) {
				Node *empty_vertices = makeAArrayExpr(NIL, AGTYPEARRAYOID);
				ResTarget *vertices = makeResTarget(
        empty_vertices, VLE_COLNAME_VERTICES);

				tlist = lappend(tlist, vertices);
			}

			vidcond = makeSimpleA_Expr(AEXPR_OP, "=", vid, prev_col, -1);
			where_args = lappend(where_args, vidcond);
		}
		/* TODO: cannot see properties of future vertices */
		if (crel->props != NULL)
			where_args = lappend(where_args, genVLEQual(VLE_LEFT_ALIAS,
														crel->props,cpstate));

		sel = makeNode(SelectStmt);
		sel->targetList = tlist;
		sel->fromClause = list_make1(from);
		if (vid == NULL)
			sel->distinctClause = list_make1(NIL);
		if (where_args == NIL)
		{
			sel->whereClause = NULL;
		} else if (list_length(where_args) == 1) {
			sel->whereClause = (Node *) linitial(where_args);
		} else {
			sel->whereClause =
				(Node *) makeBoolExpr(AND_EXPR, where_args, -1);
		}
	}

	sub = makeNode(RangeSubselect);
	sub->subquery = (Node *) sel;
	sub->alias = makeAliasNoDup(VLE_LEFT_ALIAS, colnames);

	return (Node *) sub;
}

static transform_entity *transform_VLE_edge_entity_newlogic(cypher_parsestate *cpstate,
                                                   cypher_relationship *rel,
                                                   Query *query,bool pathout)
{
	transform_entity *vle_entity = NULL;
    ParseState *pstate = NULL;
    /* set the pstate */
    pstate = &cpstate->pstate;
    char	   *varname = rel->name;
	bool		out = (varname != NULL);
	SelectStmt *sel;
	Alias	   *alias;
	RangeTblEntry *rte;

	sel = genVLESubselect(cpstate, rel,pathout);

    alias = makeNode(Alias);
    alias->aliasname = get_next_default_alias(cpstate);
    alias->colnames = NIL;

	rte = transformVLEtoRTE(cpstate, sel, alias);

    /*
     * Keep outer restrictions on the immediate SubqueryScan. The VLE plan
     * builder relocates that qual above the custom traversal; without this
     * barrier openGauss pushes it further into the seed's label scans before
     * the plan builder can detach it.
     */
    rte->security_barrier = pathout;

    Node	   *var;
    Node       *vertices_var = NULL;
    /* Get the var node for the VLE functions column name. */
    var = scanRTEForColumn(pstate, rte, "edges", -1, false);
    if (pathout)
    {
        vertices_var = scanRTEForColumn(pstate, rte,
                                        VLE_COLNAME_VERTICES, -1, false);
        Assert(vertices_var != NULL);
    }
    if (out)
    {	
        FuncExpr *fexpr;
        List *args = list_make1(var);
        Oid func_oid = InvalidOid;

        /*
         * Get the oid for the materialize function that returns a list of
         * edges. For a VLE edge variable we need to return a list of edges,
         * not a path.
         */
        func_oid = get_ag_func_oid("age_materialize_vle_edges_arr", 1, AGTYPEOID);

        /* build the expr node for the function */
        fexpr = makeFuncExpr(func_oid, AGTYPEOID, args, InvalidOid, InvalidOid,
                             COERCE_EXPLICIT_CALL);

        /* make the target entry and apply the provided variable */
        TargetEntry * te = makeTargetEntry((Expr*)fexpr, pstate->p_next_resno++, rel->name,
                             false);
        /* add it to the query */
        query->targetList = lappend(query->targetList, te);
    }
    // for vle
    cypher_vle_target_nodes* target_nodes = make_ag_node(cypher_vle_target_nodes);

    target_nodes->graph_oid =cpstate->graph_oid;
    target_nodes->label_name = rel->label;
    target_nodes->edge_property_constraint = (Node *)transform_cypher_expr(cpstate,  rel->props,
                                                   EXPR_KIND_VALUES); 
    FuncCall *	funcall = (FuncCall *) rel->varlen;
    List * args =   funcall->args;
    Node *lidx = (Node*)list_nth(args, 3);
	Node *uidx = (Node*)list_nth(args, 4);
     if (lidx == NULL  || ((A_Const *)lidx)->val.type == T_Null)
    {
        target_nodes->minimum_output_depth = 1;
    }
    else
    {
         target_nodes->minimum_output_depth = ((A_Const *)lidx)->val.val.ival;
    }

    /* get the right range index. NULL means infinite */
    if (uidx == NULL  || ((A_Const *)uidx)->val.type == T_Null)
    {
       target_nodes->maximum_output_depth = INT_MAX;
    }
    else
    {
        target_nodes->maximum_output_depth =  ((A_Const *)uidx)->val.val.ival;
    }
    target_nodes->cypher_rel_direction = rel->dir;

    FuncExpr* func_expr = make_clause_func_expr(VLE_CLAUSE_FUNCTION_NAME,
                                      (Node *)target_nodes);
    Query * subquery =  rte->subquery;
    // Create the target entry
    TargetEntry* tle = makeTargetEntry((Expr *)func_expr, list_length(subquery->targetList)+1,
                          AGE_VARNAME_VLE_CLAUSE, true);
    subquery->targetList = lappend(subquery->targetList, tle);

    /* Make a transform entity for the vle. */
    vle_entity = make_transform_entity(cpstate, ENT_VLE_EDGE, (Node *)rel,
                                       (Expr *)var);
    vle_entity->vle_vertices = (Expr *)vertices_var;

    /* return the vle entity */
    return vle_entity;
}

/* helper function to check for specific VLE cases */
static bool isa_special_VLE_case(cypher_path *path)
{
    cypher_relationship *cr = NULL;

    if (path->var_name == NULL)
    {
        return false;
    }

    if (list_length(path->path) != 3)
    {
        return false;
    }

    cr = (cypher_relationship*)lfirst(lnext(list_head(path->path)));

    if (cr->varlen != NULL)
    {
        return true;
    }

    return false;
}

static void
setInitialVidForVLE(cypher_parsestate *cpstate, cypher_relationship *crel, transform_entity *vertex)
{
    if (!vertex->in_join_tree) {
        cpstate->p_vle_initial_vid = NULL;
        return;
    }

    cpstate->p_vle_initial_vid = getColumnVar(cpstate, vertex,
                                              AG_VERTEX_COLNAME_ID);
}

/*
 * Iterate through the path and construct all edges and necessary vertices
 */
static List *transform_match_entities(cypher_parsestate *cpstate, Query *query,
                                      cypher_path *path)
{
    ParseState *pstate = (ParseState *)cpstate;
    ListCell *lc = NULL;
    List *entities = NIL;
    int i = 0;
    bool node_declared_in_prev_clause = false;

    transform_entity *prev_entity = NULL;
    bool special_VLE_case = false;
    bool valid_label;

    special_VLE_case = isa_special_VLE_case(path);
    valid_label = path_check_valid_label(path, cpstate);

    /*
     * Iterate through every node in the path, construct the expr node
     * that is needed for the remaining steps
     */

    foreach (lc, path->path)
    {
        Expr *expr = NULL;
        transform_entity *entity = NULL;

        /* even increments of i are vertices */
        if (i % 2 == 0)
        {
            cypher_node *node = NULL;
            bool output_node = false;
            bool adjacent_to_regular_edge = false;
            bool adjacent_to_vle_edge = false;
            node = (cypher_node*)lfirst(lc);
            if (path->var_name != NULL && node->name != NULL &&
                strcmp(path->var_name, node->name) == 0)
            {
                ereport(ERROR,
                        (errcode(ERRCODE_DUPLICATE_ALIAS),
                         errmsg("variable '%s' is for a path", node->name),
                         parser_errposition(pstate, node->location)));
            }

            /*
             * The vle needs to know if the start vertex was
             * created in a previous clause. Check to see if it
             * was so the edge logic can handle changing its argument
             * if necessary.
             */
            if (node->name != NULL)
            {
                /*
                 * Checks the previous clauses to see if the variable already
                 * exists.
                 */
                Node *expr = colNameToVar(pstate, node->name, false,
                                          node->location);
                if (expr != NULL)
                {
                    node_declared_in_prev_clause = true;
                }
            }

            if (i > 0)
            {
                cypher_relationship *previous_relationship =
                    (cypher_relationship *)list_nth(path->path, i - 1);

                if (previous_relationship->varlen == NULL)
                    adjacent_to_regular_edge = true;
                else
                    adjacent_to_vle_edge = true;
            }

            if (i + 1 < list_length(path->path))
            {
                cypher_relationship *next_relationship =
                    (cypher_relationship *)list_nth(path->path, i + 1);

                if (next_relationship->varlen == NULL)
                    adjacent_to_regular_edge = true;
                else
                    adjacent_to_vle_edge = true;
            }

            /*
             * A standalone anonymous MATCH still has cardinality: its scan
             * must feed one tuple per matched vertex to later clauses and
             * aggregates (Apache AGE #1288). A regular-only endpoint must
             * also scan its vertex relation so a dangling edge cannot satisfy
             * the pattern. Keep endpoint elision at a VLE boundary, whose
             * executor already supplies the logical endpoint.
             */
            output_node = list_length(path->path) == 1 ||
                          (adjacent_to_regular_edge &&
                           !adjacent_to_vle_edge) ||
                          INCLUDE_NODE_IN_JOIN_TREE(path, node);
            if (special_VLE_case && !adjacent_to_regular_edge &&
                !node->name && !node->props)
                output_node = false;

            /* transform vertex */
            expr = transform_cypher_node(cpstate, node, &query->targetList,
                                         output_node, valid_label);

            entity = make_transform_entity(cpstate, ENT_VERTEX, (Node *)node,
                                           expr);

            cpstate->entities = lappend(cpstate->entities, entity);
            entities = lappend(entities, entity);

         /* transform the properties if they exist */
            if (node->props)
            {
                Node *n = NULL;
                
                Node *prop_expr = NULL;
                Node *prop_var = NULL;

                /*
                 * We need to build a transformed properties(prop_var)
                 * expression IF the properties variable already exists from a
                 * previous clause. Please note that the "found" prop_var was
                 * previously transformed.
                 */

                /* get the prop_var if it was previously resolved */
                if (node->name != NULL)
                {
                    prop_var = colNameToVar(pstate, node->name, false,
                                            node->location);
                }

                /*
                 * If prop_var exists and is an alias, just pass it through by
                 * assigning the prop_expr the prop_var.
                 */
                if (prop_var != NULL &&
                    pg_strncasecmp(node->name, AGE_DEFAULT_ALIAS_PREFIX,
                                   strlen(AGE_DEFAULT_ALIAS_PREFIX)) == 0)
                {
                    prop_expr = prop_var;
                }
                /*
                 * Else, if it exists and is not an alias, create the prop_expr
                 * as a transformed properties(prop_var) function node.
                 */
                else if (prop_var != NULL)
                {
                    prop_expr = make_properties_expr(prop_var);
                }

                 
                n = create_property_constraint_function(cpstate, entity, node->props,
                                                prop_expr);

                cpstate->property_constraint_quals =
                    lappend(cpstate->property_constraint_quals, n);
            }
            prev_entity = entity;
        }
        /* odd increments of i are edges */
        else
        {
            cypher_relationship *rel = NULL;

            rel = (cypher_relationship*)lfirst(lc);
            if (path->var_name != NULL && rel->name != NULL &&
                strcmp(path->var_name, rel->name) == 0)
            {
                ereport(ERROR,
                        (errcode(ERRCODE_DUPLICATE_ALIAS),
                         errmsg("variable '%s' is for a path", rel->name),
                         parser_errposition(pstate, rel->location)));
            }

            /*
             * There are 2 edge cases - 1) a regular edge and 2) a VLE edge.
             * A VLE edge is not added like a regular edge - it is a function.
             */

            /* if it is a regular edge */
            if (rel->varlen == NULL)
            {
                if (list_length(path->path) == 3 &&
                        rel->dir == CYPHER_REL_DIR_NONE &&
                        !prev_entity->in_join_tree)
                {
                    
                    cypher_node *node = (cypher_node *)lfirst(lnext(lc));

                    if (!INCLUDE_NODE_IN_JOIN_TREE(path, node))
                    {
                        /*
                         * Assigning a variable name here will ensure that when
                         * the next vertex is processed, the vertex will be
                         * included in the join tree.
                         */
                        node->name = get_next_default_alias(cpstate);
                    }
                }

                expr = transform_cypher_edge(cpstate, rel,
                                             &query->targetList, valid_label);

                entity = make_transform_entity(cpstate, ENT_EDGE, (Node *)rel,
                                               expr);

                /*
                 * We want to add transformed entity to entities before transforming props
                 * so that props referencing currently transformed entity can be resolved.
                 */
                cpstate->entities = lappend(cpstate->entities, entity);
                entities = lappend(entities, entity);

                if (rel->props)
                {
                    Node *r = NULL;
                    Node *prop_var = NULL;
                    Node *prop_expr = NULL;

                    /*
                     * We need to build a transformed properties(prop_var)
                     * expression IF the properties variable already exists from
                     * a previous clause. Please note that the "found" prop_var
                     * was previously transformed.
                     */

                    /* get the prop_var if it was previously resolved */
                    if (rel->name != NULL)
                    {
                        prop_var = colNameToVar(pstate, rel->name, false,
                                                rel->location);
                    }

                    /*
                     * If prop_var exists and is an alias, just pass it through by
                     * assigning the prop_expr the prop_var.
                     */
                    if (prop_var != NULL &&
                        pg_strncasecmp(rel->name, AGE_DEFAULT_ALIAS_PREFIX,
                                       strlen(AGE_DEFAULT_ALIAS_PREFIX)) == 0)
                    {
                        prop_expr = prop_var;
                    }
                    /*
                     * Else, if it exists and is not an alias, create the prop_expr
                     * as a transformed properties(prop_var) function node.
                     */
                    else if (prop_var != NULL)
                    {
                        prop_expr = make_properties_expr(prop_var);
                    }

                    r = create_property_constraint_function(cpstate, entity, rel->props,
                                                    prop_expr);

                    cpstate->property_constraint_quals =
                        lappend(cpstate->property_constraint_quals, r);
                }

                prev_entity = entity;
            }
            /* if we have a VLE edge */
            else
            {
                transform_entity *vle_entity = NULL;

                /*
                 * Check to see if the previous node was originally created
                 * in a predecessing clause. If it was, then remove the id field
                 * from the column ref. Just reference the agtype vertex
                 * variable that the prev clause created and the vle will handle
                 * extracting the id.
                 */
                if (node_declared_in_prev_clause)
                {
                    FuncCall *func = (FuncCall*)rel->varlen;

                    ColumnRef *cr = (ColumnRef*)linitial(func->args);

                    Assert(IsA(cr, ColumnRef));
                    Assert(list_length(cr->fields) == 2);

                    cr->fields = list_make1(linitial(cr->fields));
                }
                setInitialVidForVLE(cpstate,rel, prev_entity);
                /* make a transform entity for the vle */
                bool pathout = path->var_name!=NULL;
                vle_entity = transform_VLE_edge_entity_newlogic(cpstate, rel, query,pathout);

                /* add the entity in */
                cpstate->entities = lappend(cpstate->entities, vle_entity);
                entities = lappend(entities, vle_entity);
                
                prev_entity = entity;
            }

            node_declared_in_prev_clause = false;
        }

        i++;
    }
    return entities;
}

/*
 * Iterate through the list of entities setup the join conditions. Joins
 * are driven through edges. To correctly setup the joins, we must
 * aquire information about the previous edge and vertex, and the next
 * edge and vertex.
 */
static List *make_path_join_quals(cypher_parsestate *cpstate, List *entities)
{
    transform_entity *prev_node = NULL, *prev_edge = NULL, *edge = NULL,
                     *next_node = NULL, *next_edge = NULL;
    ListCell *lc;
    List *quals = NIL;
    List *join_quals;

    /*
     * Vertex-only patterns have no edges, so the edge-driven correlation and
     * label-filter logic below never runs. That is correct for a freshly
     * scanned vertex -- its label comes from its label-table scan. But a
     * vertex that refers to a variable from an ENCLOSING query -- e.g. the
     * (a:Person) in MATCH (a) WHERE (a:Person) / EXISTS((a:Person)) -- is not
     * scanned from its label table here. Without an explicit filter such a
     * sub-pattern is uncorrelated and trivially true (the label is never
     * tested). If the vertex carries a non-default label and its variable
     * exists in an ancestor parse state, emit a label-id filter: make_qual
     * builds a name-based id reference that resolves to the outer variable,
     * which both correlates the sub-pattern to it and enforces the label.
     */
    if (list_length(entities) < 3)
    {
        ListCell *vlc;

        foreach (vlc, entities)
        {
            transform_entity *ent = (transform_entity *) lfirst(vlc);
            char *label;
            char *name;
            Index levelsup = 0;

            if (ent->type != ENT_VERTEX)
            {
                continue;
            }

            label = ent->entity.node->label;
            name = ent->entity.node->name;

            /*
             * find_parent_variable walks the ancestor parse-state chain (the
             * plugin nests one extra sub-query level for EXISTS sub-patterns,
             * so the outer variable is not necessarily in the immediate
             * parent). A match means the vertex is a correlated reference to
             * an enclosing-query variable, so emit a label-id filter.
             */
            if (label != NULL && !IS_DEFAULT_LABEL_VERTEX(label) &&
                name != NULL &&
                find_parent_variable(cpstate, name, &levelsup) != NULL)
            {
                Node *id_field = make_qual(cpstate, ent, "id");

                quals = lappend(quals,
                                filter_vertices_on_label_id(cpstate,
                                                            id_field,
                                                            label));
            }
        }

        return quals;
    }

    lc = list_head(entities);
    for (;;)
    {
        /*
         * Initial setup, set the initial vertex as the previous vertex
         * and get the first edge
         */
        if (prev_node == NULL)
        {
            prev_node = (transform_entity*)lfirst(lc);
            lc = (ListCell*)lnext(lc);
            edge = (transform_entity*)lfirst(lc);
        }

        // Retrieve the next node and edge in the pattern.
        if (lnext(lc) != NULL)
        {
            lc = lnext(lc);
            next_node = (transform_entity*)lfirst(lc);

            if (lnext(lc) != NULL)
            {
                lc = lnext(lc);
                next_edge = (transform_entity*)lfirst(lc);
            }
        }

        // create the join quals for the node
        join_quals = make_join_condition_for_edge(
            cpstate, prev_edge, prev_node, edge, next_node, next_edge);

        quals = list_concat(quals, join_quals);

        /* Set the edge as the previous edge and the next edge as
         * the current edge. If there is not a new edge, exit the
         * for loop.
         */
        prev_edge = edge;
        prev_node = next_node;
        edge = next_edge;
        next_node = NULL;
        next_edge = NULL;

        if (edge == NULL)
        {
            return quals;
        }
    }
}

/*
 * Create the path variable. Takes the list of entities, extracts the variable
 * and passes as the argument list for the _agtype_build_path function.
 */
static TargetEntry* transform_match_create_path_variable(cypher_parsestate *cpstate,
                                                         cypher_path *path,
                                                         List *entities)
{
    ParseState *pstate = (ParseState *)cpstate;
    Oid build_path_oid;
    FuncExpr *fexpr;
    int resno;
    List *entity_exprs = NIL;
    ListCell *lc;
    int entity_index = 0;

    if (list_length(entities) < 1)
    {
        ereport(ERROR,
                (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                 errmsg("paths require at least 1 vertex"),
                 parser_errposition(pstate, path->location)));
    }

    // extract the expr for each entity
    foreach (lc, entities)
    {
        transform_entity *entity = (transform_entity*)lfirst(lc);
        transform_entity *next_entity = NULL;

        if (entity_index + 1 < list_length(entities))
        {
            next_entity = (transform_entity *)list_nth(entities,
                                                       entity_index + 1);
        }

        /*
         * The custom openGauss VLE returns parallel agtype[] arrays.  The
         * vertex array contains the segment's start and intermediate
         * vertices; expr contains its ordered edges. Replace the vertex
         * immediately before every VLE with that array. _agtype_build_path
         * interleaves each pair at execution time, including paths that mix
         * regular and multiple VLE segments.
         */
        if (entity->type == ENT_VERTEX && next_entity != NULL &&
            next_entity->type == ENT_VLE_EDGE)
        {
            Assert(next_entity->vle_vertices != NULL);
            entity_exprs = lappend(entity_exprs,
                                   next_entity->vle_vertices);
        } else if (entity->expr != NULL) {
            Node *entity_expr = (Node *)entity->expr;

            entity_expr = coerce_entity_to_agtype(pstate, entity_expr);
            entity_exprs = lappend(entity_exprs, entity_expr);
        } else if (entity->type == ENT_VERTEX && entity_index > 0) {
            transform_entity *previous_entity =
                (transform_entity *)list_nth(entities, entity_index - 1);
            cypher_relationship *relationship;
            Node *endpoint_id;
            Const *graph_name = makeConst(
                TEXTOID, -1, InvalidOid, -1,
                CStringGetTextDatum(cpstate->graph_name), false, false);
            Oid get_vertex_oid = get_ag_func_oid(
                "_get_vertex_by_graphid", 2, TEXTOID, GRAPHIDOID);
            FuncExpr *endpoint_expr;

            Assert(previous_entity->type == ENT_VLE_EDGE);
            relationship = previous_entity->entity.rel;
            endpoint_id = getColumnVar(
                cpstate, previous_entity,
                getEdgeColname(relationship, true));
            endpoint_expr = makeFuncExpr(
                get_vertex_oid, AGTYPEOID, list_make2(graph_name, endpoint_id),
                InvalidOid, InvalidOid, COERCE_EXPLICIT_CALL);

            entity_exprs = lappend(entity_exprs, endpoint_expr);
        }

        entity_index++;
    }

    // get the oid for the path creation function
    build_path_oid = get_ag_func_oid("_agtype_build_path", 1, ANYOID);

    // build the expr node for the function
    fexpr = makeFuncExpr(build_path_oid, AGTYPEOID, entity_exprs, InvalidOid,
                         InvalidOid, COERCE_EXPLICIT_CALL);

    /* Make path variables available while this MATCH's WHERE is transformed. */
    cpstate->entities = lappend(
        cpstate->entities,
        make_transform_entity(cpstate, ENT_PATH, (Node *)path, (Expr *)fexpr));

    resno = cpstate->pstate.p_next_resno++;

    // create the target entry
    return makeTargetEntry((Expr *)fexpr, resno, path->var_name, false);
}

/*
 * Maps a column name to the a function access name. In others word when
 * passed the name for the vertex's id column name, return the function name
 * for the vertex's agtype id element, etc.
 */
static char *get_accessor_function_name(enum transform_entity_type type,
                                        char *name)
{
    if (type == ENT_VERTEX)
    {
        // id
        if (!strcmp(AG_VERTEX_COLNAME_ID, name))
        {
            return AG_VERTEX_ACCESS_FUNCTION_ID;
        }
        // props
        else if (!strcmp(AG_VERTEX_COLNAME_PROPERTIES, name))
        {
            return AG_VERTEX_ACCESS_FUNCTION_PROPERTIES;
        }
    }
    if (type == ENT_EDGE || type == ENT_VLE_EDGE)
    {
        // id
        if (!strcmp(AG_EDGE_COLNAME_ID, name))
        {
            return AG_EDGE_ACCESS_FUNCTION_ID;
        }
        // start id
        else if (!strcmp(AG_EDGE_COLNAME_START_ID, name))
        {
            return AG_EDGE_ACCESS_FUNCTION_START_ID;
        }
        // end id
        else if (!strcmp(AG_EDGE_COLNAME_END_ID, name))
        {
            return AG_EDGE_ACCESS_FUNCTION_END_ID;
        }
        // props
        else if (!strcmp(AG_VERTEX_COLNAME_PROPERTIES, name))
        {
            return AG_VERTEX_ACCESS_FUNCTION_PROPERTIES;
        }
    }

    ereport(ERROR,
            (errcode(ERRCODE_INVALID_COLUMN_REFERENCE),
             errmsg("column %s does not have an accessor function", name)));

    // keeps compiler silent
    return NULL;
}

/*
 * For the given entity and column name, construct an expression that will
 * access the column or get the access function if the entity is a variable.
 */
static Node *make_qual(cypher_parsestate *cpstate,
                       transform_entity *entity, char *col_name)
{
    List *qualified_name, *args;
    Node *node;

    if (is_vertex_or_edge((Node *)entity->expr))
    {
        node = extract_field_from_record((Node *)entity->expr, col_name);
    }
    else if (entity->expr != NULL && IsA(entity->expr, Var))
    {
        char *function_name;

        function_name = get_accessor_function_name(entity->type, col_name);
        qualified_name = list_make2(makeString("ag_catalog"),
                                    makeString(function_name));
        args = list_make1(entity->expr);
        node = (Node *)makeFuncCall(qualified_name, args, -1);
    }
    else
    {
        char *entity_name = NULL;
        ColumnRef *cr = makeNode(ColumnRef);

        if (entity->type == ENT_EDGE)
        {
            entity_name = entity->entity.node->name;
        }
        else if (entity->type == ENT_VERTEX)
        {
            entity_name = entity->entity.rel->name;
        }
        else
        {
            ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                            errmsg("unknown entity type")));
        }

        cr->fields = list_make2(makeString(entity_name), makeString(col_name));
        node = (Node *)cr;
    }

    return node;
}

void get_record_field_info(const char *field_name, Oid entity_type,
                           AttrNumber *fieldnum, Oid *fieldtype)
{
    bool is_vertex = entity_type == VERTEXOID;
    bool is_edge = entity_type == EDGEOID;

    Assert(field_name != NULL);
    Assert(fieldnum != NULL);
    Assert(fieldtype != NULL);
    Assert(is_vertex || is_edge);

    *fieldnum = InvalidAttrNumber;
    *fieldtype = InvalidOid;

    if (pg_strcasecmp(field_name, "id") == 0) {
        *fieldnum = VERTEX_ID_FIELD_NUMBER;
        *fieldtype = GRAPHIDOID;
    } else if (is_vertex) {
        if (pg_strcasecmp(field_name, "label") == 0) {
            *fieldnum = VERTEX_LABEL_FIELD_NUMBER;
            *fieldtype = AGTYPEOID;
        } else if (pg_strcasecmp(field_name, "properties") == 0) {
            *fieldnum = VERTEX_PROPERTIES_FIELD_NUMBER;
            *fieldtype = AGTYPEOID;
        }
    } else if (is_edge) {
        if (pg_strcasecmp(field_name, "label") == 0 ||
            pg_strcasecmp(field_name, "type") == 0) {
            *fieldnum = EDGE_LABEL_FIELD_NUMBER;
            *fieldtype = AGTYPEOID;
        } else if (pg_strcasecmp(field_name, "end_id") == 0 ||
                 pg_strcasecmp(field_name, "endnode") == 0) {
            *fieldnum = EDGE_END_ID_FIELD_NUMBER;
            *fieldtype = GRAPHIDOID;
        } else if (pg_strcasecmp(field_name, "start_id") == 0 ||
                 pg_strcasecmp(field_name, "startnode") == 0) {
            *fieldnum = EDGE_START_ID_FIELD_NUMBER;
            *fieldtype = GRAPHIDOID;
        } else if (pg_strcasecmp(field_name, "properties") == 0) {
            *fieldnum = EDGE_PROPERTIES_FIELD_NUMBER;
            *fieldtype = AGTYPEOID;
        }
    }
}

FieldSelect *make_field_select(Expr *expr, AttrNumber fieldnum,
    Oid resulttype)
{
    FieldSelect *fselect = makeNode(FieldSelect);

    fselect->arg = (Expr *)copyObject(expr);
    fselect->fieldnum = fieldnum;
    fselect->resulttype = resulttype;
    fselect->resulttypmod = -1;
    fselect->resultcollid = InvalidOid;

    return fselect;
}

static Expr *get_relative_expr(transform_entity *entity, Index levelsup)
{
    Node *relative_expr;

    Assert(entity != NULL);
    Assert(entity->expr != NULL);

    relative_expr = (Node *)copyObject(entity->expr);
    IncrementVarSublevelsUp(relative_expr, levelsup, 0);

    return (Expr *)relative_expr;
}

static Expr *transform_cypher_edge(cypher_parsestate *cpstate,
                                   cypher_relationship *rel,
                                   List **target_list, bool valid_label)
{
    ParseState *pstate = (ParseState *)cpstate;
    char *schema_name = NULL;
    char *rel_name = NULL;
    RangeVar *label_range_var = NULL;
    Alias *alias = NULL;
    int resno = -1;
    TargetEntry *te = NULL;
    transform_entity *entity = NULL;
    cypher_relationship *cr = NULL;
    Node *expr = NULL;
    Var *previous_clause_var = NULL;
    bool refs_var = false;
    RangeTblEntry *rte = NULL;

    if (rel->label != NULL && rel->parsed_label == NULL)
    {
        rel->parsed_label = rel->label;
    }

    /*
     * If we have an edge name, get any potential variable or column
     * references. Additionally, verify that they are for edges.
     */
    if (rel->name != NULL)
    {
        te = findTarget(*target_list, rel->name);
        entity = find_variable(cpstate, rel->name);
        previous_clause_var = (Var *)colNameToVar(pstate, rel->name, false,
                                                  rel->location);

        /*
         * If we have a valid entity and te for this rel name, go ahead and get
         * the cypher relationship as we will need this for later and flag that
         * we have a variable reference.
         */
        if (entity != NULL && (te || previous_clause_var)) {
            cr = (cypher_relationship *)entity->entity.rel;
            refs_var = true;
        }

        /* If the variable already exists, verify that it is for an edge */
        if (refs_var)
        {
            if (entity->type == ENT_VERTEX)
            {
                ereport(ERROR,
                       (errcode(ERRCODE_DUPLICATE_ALIAS),
                        errmsg("variable '%s' is for a vertex", rel->name),
                        parser_errposition(pstate, rel->location)));
            }
            else if (entity->type == ENT_VLE_EDGE)
            {
                ereport(ERROR,
                       (errcode(ERRCODE_DUPLICATE_ALIAS),
                        errmsg("variable '%s' is for a VLE edge", rel->name),
                        parser_errposition(pstate, rel->location)));
            }
            else if (entity->type == ENT_PATH)
            {
                ereport(ERROR,
                       (errcode(ERRCODE_DUPLICATE_ALIAS),
                        errmsg("variable '%s' is for a path", rel->name),
                        parser_errposition(pstate, rel->location)));
            }
        }

        else if (te && !entity)
        {
            ereport(ERROR,
                    (errcode(ERRCODE_DUPLICATE_ALIAS),
                     errmsg("variable '%s' already exists", rel->name),
                     parser_errposition(pstate, rel->location)));
        }
    }

    /*
     * If we do not have a label for this edge, we either need to find one
     * from a referenced variable or we need to set it to the default label.
     */
    if (rel->label == NULL)
    {
        /* if there is a variable for this rel name */
        if (refs_var)
        {
            /*
             * If the referenced var has a non NULL label, copy it. This is
             * usually the case when it uses a variable that is already defined.
             * Fx -
             *
             *     MATCH (u:people)-[e:knows]->(v:people), (v)-[e]->(u) RETURN e
             *     MATCH (u:people)-[]->()-[]->(u) RETURN u
             *
             * We copy it so that we know what label it is referencing.
             */
            if (cr->parsed_label != NULL)
            {
                rel->parsed_label = cr->parsed_label;
                rel->label = cr->label;
            }
            else
            {
                rel->label = AG_DEFAULT_LABEL_EDGE;
            }
        }
        /* otherwise, just give it the default label */
        else
        {
            rel->label = AG_DEFAULT_LABEL_EDGE;
        }
    }
    /* if we do have a label, is it valid */
    else if (!valid_label)
    {
        /*
         *  XXX: Need to determine proper rules, for when label does not exist
         *  or is for an vertex. Maybe labels and edges should share names, like
         *  in openCypher. But these are stand in errors, to prevent
         *  segmentation faults, and other errors.
         *
         *  Update: Nonexistent and mismatched labels now return a NULL value to
         *  prevent segmentation faults, and other errors. We can also consider
         *  if an all-purpose label would be useful.
         */
        rel->label = NULL;
    }
    if (valid_label)
    {
        label_cache_data *lcd =
            search_label_name_graph_cache(rel->label, cpstate->graph_oid);

        if (lcd == NULL)
        {
            ereport(ERROR,
                    (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                     errmsg("label %s does not exists", rel->label),
                     parser_errposition(pstate, rel->location)));
        }

        if (lcd->kind != LABEL_KIND_EDGE)
        {
            ereport(ERROR,
                    (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                     errmsg("label %s is for vertices, not edges", rel->label),
                     parser_errposition(pstate, rel->location)));
        }
    }

    /*
     * Variables for edges are not allowed to be used multiple times within the
     * same clause.
     */
    if (previous_clause_var == NULL && refs_var)
    {
        ereport(ERROR,
                (errcode(ERRCODE_DUPLICATE_ALIAS),
                 errmsg("duplicate edge variable '%s' within a clause",
                        rel->name),
                 parser_errposition(pstate, rel->location)));
    }

    /*
     * If this edge uses a variable that already exists, verify that the label
     * names are the same.
     */
    if (refs_var && rel->parsed_label != NULL &&
        ((cr->parsed_label != NULL ? cr->parsed_label : cr->label) == NULL ||
         strcmp(cr->parsed_label != NULL ? cr->parsed_label : cr->label,
                rel->parsed_label) != 0))
    {
        ereport(ERROR,
                (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                 errmsg("multiple labels for variable '%s' are not supported",
                        rel->name),
                 parser_errposition(pstate, rel->location)));
    }

    /*
     * Now we need to do a few checks and either return the existing var or
     * or build a new edge.
     */
    if (rel->name != NULL)
    {
        /*
         * If we are in a WHERE clause transform, we don't want to create new
         * variables, we want to use the existing ones. So, error if otherwise.
         * If we are in a subquery transform, we are allowed to create new variables
         * in the match, and all variables outside are visible to
         * the subquery. Since there is no existing SQL logic that allows
         * subqueries to alter variables of outer queries, we bypass this
         * logic we would normally use to process WHERE clauses.
         *
         * Currently, the EXISTS subquery logic is naive. It returns a boolean
         * result on the outer queries, but does not restrict the results set.
         *
         * TODO: Implement logic to alter outer scope results.
         *
         */
        if (pstate->p_expr_kind == EXPR_KIND_WHERE &&
            cpstate->subquery_where_flag == false)
        {
            cypher_parsestate *parent_cpstate =
               (cypher_parsestate *)pstate->parentParseState->parentParseState;
            /*
             *  If expr_kind is WHERE, the expressions are in the parent's
             *  parent's parsestate, due to the way we transform sublinks.
             */
            transform_entity *tentity = NULL;

            /* if we have the referenced var, just return it */
            if (previous_clause_var != NULL)
            {
                return (Expr *)previous_clause_var;
            }

            tentity = find_variable(parent_cpstate, rel->name);
            if (tentity != NULL)
            {
                return get_relative_expr(tentity, 2);
            }
            else
            {
                ereport(ERROR,
                        (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                         errmsg("variable `%s` does not exist", rel->name),
                         parser_errposition(pstate, rel->location)));
            }
        }

        /* if this vertex is referencing an existing te var, return its expr */
        if (refs_var)
        {
            return (te != NULL) ? te->expr : (Expr *)previous_clause_var;
        }

        if (cpstate->subquery_where_flag == true)
        {
            Index levelsup = 0;
            transform_entity *tentity =
                find_parent_variable(cpstate, rel->name, &levelsup);

            if (tentity != NULL)
            {
                return get_relative_expr(tentity, levelsup);
            }
        }
    }

    /* if we aren't using a variable, build the edge */
    if (!rel->name)
    {
        rel->name = get_next_default_alias(cpstate);
    }

    schema_name = get_graph_namespace_name(cpstate->graph_name);

    if (valid_label)
    {
        rel_name = get_label_relation_name(rel->label, cpstate->graph_oid);
    }
    else
    {
        rel_name = AG_DEFAULT_LABEL_EDGE;
    }

    label_range_var = makeRangeVar(schema_name, rel_name, -1);
    alias = makeAlias(rel->name, NIL);
    rte = addRangeTableEntry(pstate, label_range_var, alias,
                             label_range_var->inhOpt, true);
    Assert(rte != NULL);

    /*
     * relation is visible (r.a in expression works) but attributes in the
     * relation are not visible (a in expression doesn't work)
     */
    addRTEtoQuery(pstate, rte, true, true, false);

    resno = pstate->p_next_resno++;

    if (valid_label)
    {
        expr = make_edge_expr(cpstate, rte);
    }
    else
    {
        expr = (Node *)makeNullConst(AGTYPEOID, -1, InvalidOid);
    }

    if (rel->name)
    {
        te = makeTargetEntry((Expr *)expr, resno, rel->name, false);
        *target_list = lappend(*target_list, te);
    }

    return (Expr *)expr;
}

static Expr *transform_cypher_node(cypher_parsestate *cpstate,
                                   cypher_node *node, List **target_list,
                                   bool output_node, bool valid_label)
{
    ParseState *pstate = (ParseState *)cpstate;
    char *schema_name = NULL;
    char *rel_name = NULL;
    RangeVar *label_range_var = NULL;
    Alias *alias = NULL;
    int resno = -1;
    TargetEntry *te = NULL;
    Expr *expr = NULL;
    transform_entity *entity = NULL;
    cypher_node *cn = NULL;
    bool refs_var = false;
    bool has_explicit_label = node->label != NULL;
    RangeTblEntry *rte   = NULL;
    Var *previous_clause_var = NULL;

    if (has_explicit_label && node->parsed_label == NULL)
    {
        node->parsed_label = node->label;
    }

    /* if we have a node name, get any potential variable references */
    if (node->name != NULL)
    {
        te = findTarget(*target_list, node->name);
        entity = find_variable(cpstate, node->name);
        previous_clause_var = (Var *)colNameToVar(pstate, node->name, false,
                                                  node->location);

        /*
         * If we have a valid entity and te or a valid entity and a previous var
         * ref for this rel name, go ahead and get the cypher relationship. We
         * will need this information for later. Additionally, flag that we have
         * a variable reference.
         */
        if (entity != NULL && (te || previous_clause_var)) {
            cn = (cypher_node *)entity->entity.node;
            refs_var = true;
         
        }
        /* If the variable already exists, verify that it is for a vertex */
        if (refs_var)
        {
            if (entity->type == ENT_EDGE)
            {
                ereport(ERROR,
                       (errcode(ERRCODE_DUPLICATE_ALIAS),
                        errmsg("variable '%s' is for an edge", node->name),
                        parser_errposition(pstate, node->location)));
            }
            else if (entity->type == ENT_VLE_EDGE)
            {
                ereport(ERROR,
                       (errcode(ERRCODE_DUPLICATE_ALIAS),
                        errmsg("variable '%s' is for a VLE edge", node->name),
                        parser_errposition(pstate, node->location)));
            }
            else if (entity->type == ENT_PATH)
            {
                ereport(ERROR,
                       (errcode(ERRCODE_DUPLICATE_ALIAS),
                        errmsg("variable '%s' is for a path", node->name),
                        parser_errposition(pstate, node->location)));
            }
    
        }

        /* If their is a te but no entity, it implies that their is
         * some variable that exists but not an edge,vle or a vertex
         */
        else if (te && !entity)
        {
            ereport(ERROR,
                    (errcode(ERRCODE_DUPLICATE_ALIAS),
                     errmsg("variable '%s' already exists", node->name),
                     parser_errposition(pstate, node->location)));
        }
    }

    /*
     * If we do not have a label for this vertex, we either need to find one
     * from a referenced variable or we need to set it to the default label.
     */
    if (node->label == NULL)
    {
        if (refs_var)
        {
            /*
             * If the referenced var has a non NULL label, copy it. This is
             * usually the case when it uses a variable that is already defined.
             * Fx -
             *
             *     MATCH (u:people)-[e:knows]->(v:people), (v)-[e]->(u) RETURN e
             *     MATCH (u:people)-[]->()-[]->(u) RETURN u
             *
             * We copy it so that we know what label it is referencing.
             */
            if (cn->parsed_label != NULL)
            {
                node->parsed_label = cn->parsed_label;
                node->label = cn->label;
            }
            else
            {
                node->label = AG_DEFAULT_LABEL_VERTEX;
            }
        }
        /* otherwise, just give it the default label */
        else
        {
            node->label = AG_DEFAULT_LABEL_VERTEX;
        }
    }
    /* if we do have a label, is it valid */
    else if (!valid_label)
    {
        /*
         *  XXX: Need to determine proper rules, for when label does not exist
         *  or is for an edge. Maybe labels and edges should share names, like
         *  in openCypher. But these are stand in errors, to prevent
         *  segmentation faults, and other errors.
         *
         *  Update: Nonexistent and mismatched labels now return a NULL value to
         *  prevent segmentation faults, and other errors. We can also consider
         *  if an all-purpose label would be useful.
         */
        node->label = NULL;
    }

    if (valid_label)
    {
        label_cache_data *lcd =
            search_label_name_graph_cache(node->label, cpstate->graph_oid);

        if (lcd == NULL)
        {
            ereport(ERROR,
                    (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                     errmsg("label %s does not exists", node->label),
                     parser_errposition(pstate, node->location)));
        }
        if (lcd->kind != LABEL_KIND_VERTEX)
        {
            ereport(ERROR,
                    (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                     errmsg("label %s is for edges, not vertices", node->label),
                     parser_errposition(pstate, node->location)));
        }
    }

    /*
     * If this vertex uses a variable that already exists, verify that the label
     * being used is of the same name.
     */
    if (refs_var && has_explicit_label &&
        ((cn->parsed_label != NULL ? cn->parsed_label : cn->label) == NULL ||
         node->parsed_label == NULL ||
         strcmp(cn->parsed_label != NULL ? cn->parsed_label : cn->label,
                node->parsed_label) != 0))
    {
        ereport(ERROR,
                (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                 errmsg("multiple labels for variable '%s' are not supported",
                        node->name),
                 parser_errposition(pstate, node->location)));
    }

    /* if it is not an output node, just return null */
    if (!output_node)
    {
        return NULL;
    }

    /*
     * Now we need to do a few checks and either return the existing var or
     * or build a new vertex.
     */
    if (node->name != NULL)
    {
        Node *expr = NULL;

        /*
         * If we are in a WHERE clause transform, we don't want to create new
         * variables, we want to use the existing ones. So, error if otherwise.
         * If we are in a subquery transform, we are allowed to create new variables
         * in the match, and all variables outside are visible to
         * the subquery. Since there is no existing SQL logic that allows
         * subqueries to alter variables of outer queries, we bypass this
         * logic we would normally use to process WHERE clauses.
         *
         * Currently, the EXISTS subquery logic is naive. It returns a boolean
         * result on the outer queries, but does not restrict the results set.
         *
         * TODO: Implement logic to alter outer scope results.
         *
         */
        if (pstate->p_expr_kind == EXPR_KIND_WHERE &&
            cpstate->subquery_where_flag == false)
        {
            cypher_parsestate *parent_cpstate =
               (cypher_parsestate *)pstate->parentParseState->parentParseState;
            /*
             *  If expr_kind is WHERE, the expressions are in the parent's
             *  parent's parsestate, due to the way we transform sublinks.
             */
            transform_entity *tentity = NULL;

            /* if we have the referenced var, just return it */
            if (previous_clause_var != NULL)
            {
                return (Expr *)previous_clause_var;
            }

            tentity = find_variable(parent_cpstate, node->name);
            if (tentity != NULL)
            {
                return get_relative_expr(tentity, 2);
            }
            else
            {
                ereport(ERROR,
                       (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                        errmsg("variable `%s` does not exist", node->name),
                        parser_errposition(pstate, node->location)));
            }
        }

        /* if this vertex is referencing an existing var, return its expr */
        if (refs_var)
        {
            return (te != NULL) ? te->expr : (Expr *)previous_clause_var;
        }

        if (cpstate->subquery_where_flag == true)
        {
            Index levelsup = 0;
            transform_entity *tentity =
                find_parent_variable(cpstate, node->name, &levelsup);

            if (tentity != NULL)
            {
                return get_relative_expr(tentity, levelsup);
            }
        }

        /* if this vertex is referencing an existing col var, return its expr */
        expr = colNameToVar(pstate, node->name, false, node->location);
        if (expr != NULL)
        {
            return (Expr*)expr;
        }
    }
    else
    {
        node->name = get_next_default_alias(cpstate);
    }

    /* now build a new vertex */
    schema_name = get_graph_namespace_name(cpstate->graph_name);

    if (valid_label)
    {
        rel_name = get_label_relation_name(node->label, cpstate->graph_oid);
    }
    else
    {
        rel_name = AG_DEFAULT_LABEL_VERTEX;
    }
    
    label_range_var = makeRangeVar(schema_name, rel_name, -1);
    alias = makeAlias(node->name, NIL);
    rte = addRangeTableEntry(pstate, label_range_var, alias,
                            label_range_var->inhOpt, true);
    /*
    * relation is visible (r.a in expression works) but attributes in the
    * relation are not visible (a in expression doesn't work)
    */
    addRTEtoQuery(pstate, rte, true, true, true);

    resno = pstate->p_next_resno++;

    if (valid_label)
    {
        expr = (Expr *)make_vertex_expr(cpstate, rte);
    }
    else
    {
        expr = (Expr*)makeNullConst(AGTYPEOID, -1, InvalidOid);
    }

    /* make target entry and add it */
    te = makeTargetEntry(expr, resno, node->name, false);
    *target_list = lappend(*target_list, te);

    return expr;
}

static Node *make_edge_expr(cypher_parsestate *cpstate, RangeTblEntry *rte)
{
    ParseState *pstate = (ParseState *)cpstate;
    Oid label_name_func_oid;
    Node *id, *start_id, *end_id;
    Const *graph_oid_const;
    Node *props;
    List *label_name_args;
    FuncExpr *label_name_func_expr;
    RowExpr *row_expr;

    id = scanRTEForColumn(pstate, rte, AG_EDGE_COLNAME_ID, -1, false);
    start_id = scanRTEForColumn(pstate, rte, AG_EDGE_COLNAME_START_ID, -1,
                                false);
    end_id = scanRTEForColumn(pstate, rte, AG_EDGE_COLNAME_END_ID, -1, false);

    label_name_func_oid = get_ag_func_oid("_label_name", 2, OIDOID,
                                          GRAPHIDOID);
    graph_oid_const = makeConst(OIDOID, -1, InvalidOid, sizeof(Oid),
                                ObjectIdGetDatum(cpstate->graph_oid), false,
                                true);
    label_name_args = list_make2(graph_oid_const, id);
    label_name_func_expr = makeFuncExpr(label_name_func_oid, AGTYPEOID,
                                        label_name_args, InvalidOid,
                                        InvalidOid, COERCE_EXPLICIT_CALL);
    label_name_func_expr->location = -1;
    props = scanRTEForColumn(pstate, rte, AG_EDGE_COLNAME_PROPERTIES, -1,
                             false);

    row_expr = makeNode(RowExpr);
    row_expr->args = lappend(list_make4(id, label_name_func_expr, end_id,
                                       start_id),
                             props);
    row_expr->row_typeid = EDGEOID;
    /* Preserve the named edge type when openGauss deparses CTAS queries. */
    row_expr->row_format = COERCE_EXPLICIT_CAST;
    row_expr->colnames = lappend(list_make4(makeString("id"),
                                            makeString("label"),
                                            makeString("end_id"),
                                            makeString("start_id")),
        makeString("properties"));
    row_expr->location = -1;

    return (Node *)row_expr;
}

static Node *make_vertex_expr(cypher_parsestate *cpstate, RangeTblEntry *rte)
{
    ParseState *pstate = (ParseState *)cpstate;
    Oid label_name_func_oid;
    Node *id;
    Const *graph_oid_const;
    Node *props;
    List *label_name_args;
    FuncExpr *label_name_func_expr;
    RowExpr *row_expr;

    id = scanRTEForColumn(pstate, rte, AG_VERTEX_COLNAME_ID, -1, false);
    label_name_func_oid = get_ag_func_oid("_label_name", 2, OIDOID,
                                          GRAPHIDOID);
    graph_oid_const = makeConst(OIDOID, -1, InvalidOid, sizeof(Oid),
                                ObjectIdGetDatum(cpstate->graph_oid), false,
                                true);
    label_name_args = list_make2(graph_oid_const, id);
    label_name_func_expr = makeFuncExpr(label_name_func_oid, AGTYPEOID,
                                        label_name_args, InvalidOid,
                                        InvalidOid, COERCE_EXPLICIT_CALL);
    label_name_func_expr->location = -1;
    props = scanRTEForColumn(pstate, rte, AG_VERTEX_COLNAME_PROPERTIES, -1,
                             false);

    row_expr = makeNode(RowExpr);
    row_expr->args = list_make3(id, label_name_func_expr, props);
    row_expr->row_typeid = VERTEXOID;
    /* Preserve the named vertex type when openGauss deparses CTAS queries. */
    row_expr->row_format = COERCE_EXPLICIT_CAST;
    row_expr->colnames = list_make3(makeString("id"), makeString("label"),
                                    makeString("properties"));
    row_expr->location = -1;

    return (Node *)row_expr;
}

static Query *transform_cypher_create(cypher_parsestate *cpstate,
                                      cypher_clause *clause)
{
    ParseState *pstate = (ParseState *)cpstate;
    cypher_create *self = (cypher_create *)clause->self;
    cypher_create_target_nodes *target_nodes;
    Const *null_const;
    List *transformed_pattern;
    FuncExpr *func_expr;
    Query *query;
    TargetEntry *tle;

    target_nodes = make_ag_node(cypher_create_target_nodes);
    target_nodes->flags = CYPHER_CLAUSE_FLAG_NONE;
    target_nodes->graph_oid = cpstate->graph_oid;

    query = makeNode(Query);
    query->commandType = CMD_SELECT;
    query->targetList = NIL;

    if (clause->prev)
    {
        handle_prev_clause(cpstate, query, clause->prev, true);

        target_nodes->flags |= CYPHER_CLAUSE_FLAG_PREVIOUS_CLAUSE;
    }

    null_const = makeNullConst(AGTYPEOID, -1, InvalidOid);
    tle = makeTargetEntry((Expr *)null_const, pstate->p_next_resno++,
                          AGE_VARNAME_CREATE_NULL_VALUE, false);
    query->targetList = lappend(query->targetList, tle);

    /*
     * Create the Const Node to hold the pattern. skip the parse node,
     * because we would not be able to control how our pointer to the
     * internal type is copied.
     */
    transformed_pattern = transform_cypher_create_pattern(cpstate, query,
                                                          self->pattern);

    target_nodes->paths = transformed_pattern;
    if (!clause->next)
    {
        target_nodes->flags |= CYPHER_CLAUSE_FLAG_TERMINAL;
    }

    func_expr = make_clause_func_expr(CREATE_CLAUSE_FUNCTION_NAME,
                                      (Node *)target_nodes);

    // Create the target entry
    tle = makeTargetEntry((Expr *)func_expr, pstate->p_next_resno++,
                          AGE_VARNAME_CREATE_CLAUSE, false);
    query->targetList = lappend(query->targetList, tle);

    query->rtable = pstate->p_rtable;
    query->jointree = makeFromExpr(pstate->p_joinlist, NULL);

    return query;
}

static List *transform_cypher_create_pattern(cypher_parsestate *cpstate,
                                             Query *query, List *pattern)
{
    ListCell *lc;
    List *transformed_pattern = NIL;

    foreach (lc, pattern)
    {
        cypher_create_path *transformed_path;

        transformed_path = transform_cypher_create_path(
            cpstate, &query->targetList, (cypher_path*)lfirst(lc));

        transformed_pattern = lappend(transformed_pattern, transformed_path);
    }

    return transformed_pattern;
}

static cypher_create_path* transform_cypher_create_path(cypher_parsestate *cpstate,
                                                        List **target_list,
                                                        cypher_path *path)
{
    ParseState *pstate = (ParseState *)cpstate;
    ListCell *lc;
    List *transformed_path = NIL;
    cypher_create_path *ccp = make_ag_node(cypher_create_path);
    bool in_path = path->var_name != NULL;
    bool has_edge = list_length(path->path) > 1;

    ccp->path_attr_num = InvalidAttrNumber;

    if (in_path && findTarget(*target_list, path->var_name) != NULL)
    {
        ereport(ERROR,
                (errcode(ERRCODE_DUPLICATE_ALIAS),
                 errmsg("variable \"%s\" already exists", path->var_name),
                 parser_errposition(pstate, path->location)));
    }

    foreach (lc, path->path)
    {
        if (is_ag_node(lfirst(lc), cypher_node))
        {
            cypher_node *node = (cypher_node*)lfirst(lc);
            transform_entity *entity;

            cypher_target_node *rel =
                transform_create_cypher_node(cpstate, target_list, node,
                                             has_edge);

            if (in_path) {
                if (node->name != NULL &&
                    strcmp(node->name, path->var_name) == 0) {
                    ereport(ERROR,
                            (errcode(ERRCODE_DUPLICATE_ALIAS),
                             errmsg("variable \"%s\" already exists",
                                    path->var_name),
                             parser_errposition(pstate, path->location)));
                }

                rel->flags |= CYPHER_TARGET_NODE_IN_PATH_VAR;
            }

            transformed_path = lappend(transformed_path, rel);

            entity = make_transform_entity(cpstate, ENT_VERTEX, (Node *)node,
                                           NULL);

            cpstate->entities = lappend(cpstate->entities, entity);
        }
        else if (is_ag_node(lfirst(lc), cypher_relationship))
        {
            cypher_relationship *edge = (cypher_relationship*)lfirst(lc);
            transform_entity *entity;

            cypher_target_node *rel =
                transform_create_cypher_edge(cpstate, target_list, edge);

            if (in_path) {
                if (edge->name != NULL &&
                    strcmp(edge->name, path->var_name) == 0) {
                    ereport(ERROR,
                            (errcode(ERRCODE_DUPLICATE_ALIAS),
                             errmsg("variable \"%s\" already exists",
                                    path->var_name),
                             parser_errposition(pstate, path->location)));
                }

                rel->flags |= CYPHER_TARGET_NODE_IN_PATH_VAR;
            }

            transformed_path = lappend(transformed_path, rel);

            entity = make_transform_entity(cpstate, ENT_EDGE, (Node *)edge,
                                           NULL);

            cpstate->entities = lappend(cpstate->entities, entity);
        }
        else
        {
            ereport(ERROR,
                    (errmsg_internal("unreconized node in create pattern")));
        }
    }

    ccp->target_nodes = transformed_path;

    /*
     * If this path a variable, create a placeholder entry that we can fill
     * in with during the execution phase.
     */
    if (path->var_name)
    {
        TargetEntry *te;

        if (list_length(transformed_path) < 1)
        {
            ereport(ERROR,
                    (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                     errmsg("paths require at least 1 vertex"),
                     parser_errposition(pstate, path->location)));
        }

        te = placeholder_target_entry(cpstate, path->var_name);

        ccp->path_attr_num = te->resno;

        *target_list = lappend(*target_list, te);
    }

    return ccp;
}

static cypher_target_node* transform_create_cypher_edge(cypher_parsestate *cpstate,
                                                        List **target_list,
                                                        cypher_relationship *edge)
{
    ParseState *pstate = (ParseState *)cpstate;
    cypher_target_node *rel = make_ag_node(cypher_target_node);
    Expr *props;
    Relation label_relation;
    RangeVar *rv;
    RangeTblEntry *rte;
    TargetEntry *te;
    char *alias;
    AttrNumber resno;

    if (edge->label)
    {
        label_cache_data *lcd =
            search_label_name_graph_cache(edge->label, cpstate->graph_oid);

        if (lcd && lcd->kind != LABEL_KIND_EDGE)
        {
            ereport(ERROR,
                    (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                     errmsg("label %s is for vertices, not edges", edge->label),
                     parser_errposition(pstate, edge->location)));
        }
    }

    rel->type = LABEL_KIND_EDGE;
    rel->flags = CYPHER_TARGET_NODE_FLAG_INSERT;
    rel->label_name = edge->label;
    rel->resultRelInfo = NULL;

    if (edge->name)
    {
        /*
         * Variables can be declared in a CREATE clause, but not used if
         * it already exists.
         */
        if (find_variable(cpstate, edge->name) != NULL ||
            variable_exists(cpstate, edge->name))
        {
            ereport(ERROR,
                    (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                     errmsg("variable %s already exists", edge->name)));
        }

        rel->variable_name = edge->name;
        te = placeholder_target_entry(cpstate, edge->name);
        rel->tuple_position = te->resno;
        *target_list = lappend(*target_list, te);

        rel->flags |= CYPHER_TARGET_NODE_IS_VAR;
    }
    else
    {
        rel->variable_name = NULL;
        rel->tuple_position = 0;
    }

    if (edge->dir == CYPHER_REL_DIR_NONE)
    {
        ereport(ERROR,
                (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                 errmsg("only directed relationships are allowed in CREATE"),
                 parser_errposition(&cpstate->pstate, edge->location)));
    }

    rel->dir = edge->dir;

    if (!edge->label)
    {
        ereport(ERROR,
                (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                 errmsg("relationships must be specify a label in CREATE."),
                 parser_errposition(&cpstate->pstate, edge->location)));
    }

    // create the label entry if it does not exist
    if (!label_exists(edge->label, cpstate->graph_oid))
    {
        List *parent;
        RangeVar *rv;

        rv = get_label_range_var(cpstate->graph_name, cpstate->graph_oid,
                                 AG_DEFAULT_LABEL_EDGE);

        parent = list_make1(rv);

        create_label(cpstate->graph_name, edge->label, LABEL_TYPE_EDGE,
                     parent);
    }

    // lock the relation of the label
    rv = makeRangeVar(cpstate->graph_name, edge->label, -1);
    label_relation = parserOpenTable(&cpstate->pstate, rv, RowExclusiveLock);

    // Store the relid
    rel->relid = RelationGetRelid(label_relation);

    rte = addRangeTableEntryForRelation((ParseState *)cpstate, label_relation,
                                        NULL, false, false);
    rte->requiredPerms = ACL_INSERT;

    // Build Id expression, always use the default logic
    rel->id_expr = (Expr *)build_column_default(label_relation,
                                      Anum_ag_label_edge_table_id);

    // Build properties expression, if no map is given, use the default logic
    alias = get_next_default_alias(cpstate);
    resno = pstate->p_next_resno++;

    props = cypher_create_properties(cpstate, rel, label_relation, edge->props,
                                     ENT_EDGE);

    rel->prop_attr_num = resno - 1;
    te = makeTargetEntry(props, resno, alias, false);

    *target_list = lappend(*target_list, te);

    // Keep the lock
    heap_close(label_relation, NoLock);

    return rel;
}

static bool variable_exists(cypher_parsestate *cpstate, char *name)
{
    ParseState *pstate = (ParseState *)cpstate;
    Node *id;
    RangeTblEntry *rte;

    if (name == NULL)
    {
        return false;
    }

    rte = find_rte(cpstate, PREV_CYPHER_CLAUSE_ALIAS);
    if (rte)
    {
        id = scanRTEForColumn(pstate, rte, name, -1, false);
        return id != NULL;
    }

    return false;
}

// transform nodes, check to see if the variable name already exists.
static cypher_target_node* transform_create_cypher_node(cypher_parsestate *cpstate,
                                                        List **target_list,
                                                        cypher_node *node,
                                                        bool has_edge)
{
    ParseState *pstate = (ParseState *)cpstate;

    if (node->label)
    {
        label_cache_data *lcd =
            search_label_name_graph_cache(node->label, cpstate->graph_oid);

        if (lcd && lcd->kind != LABEL_KIND_VERTEX)
        {
            ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                            errmsg("label %s is for edges, not vertices",
                                   node->label),
                            parser_errposition(pstate, node->location)));
        }
    }

    /*
     *  Check if the variable already exists, if so find the entity and
     *  setup the target node
     */
    if (node->name)
    {
        transform_entity *entity;
        TargetEntry *te;

        entity = find_variable(cpstate, node->name);
        te = findTarget(*target_list, node->name);
        if (entity && te)
        {
            if (entity->type != ENT_VERTEX || !has_edge)
            {
                ereport(ERROR,
                        (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                         errmsg("variable %s already exists", node->name),
                         parser_errposition(pstate, node->location)));
            }

            return transform_create_cypher_existing_node(cpstate, target_list,
                                                         entity->declared_in_current_clause, node);
        }

        /*
         * WITH aliases and expression-backed vertices are visible in the
         * previous clause target list even when there is no transform_entity
         * carrying the new alias.  Treat them as existing endpoints instead
         * of appending a second placeholder with the same column name.
         */
        if (te)
        {
            if (!has_edge)
            {
                ereport(ERROR,
                        (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                         errmsg("variable %s already exists", node->name),
                         parser_errposition(pstate, node->location)));
            }

            return transform_create_cypher_existing_node(cpstate, target_list,
                                                         false, node);
        }

        if (variable_exists(cpstate, node->name) && !has_edge)
        {
            ereport(ERROR,
                    (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                     errmsg("variable %s already exists", node->name),
                     parser_errposition(pstate, node->location)));
        }
    }

    // otherwise transform the target node as a new node
    return transform_create_cypher_new_node(cpstate, target_list, node);
}

/*
 * Returns the resno for the TargetEntry with the resname equal to the name
 * passed. Returns -1 otherwise.
 */
static int get_target_entry_resno(ParseState *pstate, List *target_list,
                                  char *name)
{
    ListCell *lc;

    foreach (lc, target_list)
    {
        TargetEntry *te = (TargetEntry *)lfirst(lc);
        if (!strcmp(te->resname, name))
        {
            te->expr = add_volatile_wrapper(pstate, te->expr);
            return te->resno;
        }
    }

    return -1;
}

/*
 * Transform logic for a previously declared variable in a CREATE clause.
 * All we need from the variable node is its id, and whether we can skip
 * some tests in the execution phase..
 */
static cypher_target_node *transform_create_cypher_existing_node(
    cypher_parsestate *cpstate, List **target_list, bool declared_in_current_clause,
    cypher_node *node)
{
    cypher_target_node *rel = make_ag_node(cypher_target_node);

    rel->type = LABEL_KIND_VERTEX;
    rel->flags = CYPHER_TARGET_NODE_FLAG_NONE;
    rel->resultRelInfo = NULL;
    rel->variable_name = node->name;

    if (node->props)
    {
        ereport(ERROR,
                (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                 errmsg("previously declared nodes in a create clause cannot have properties")));
    }
    if (node->label)
    {
        ereport(ERROR,
                (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                 errmsg("previously declared variables cannot have a label")));
    }
    /*
     * When the variable is declared in the same clause this vertex is a part of
     * we can skip some expensive checks in the execution phase.
     */
    if (declared_in_current_clause)
    {
        rel->flags |= EXISTING_VARAIBLE_DECLARED_SAME_CLAUSE;
    }

    /*
     * Get the AttrNumber the variable is stored in, so we can extract the id
     * later.
     */
    rel->tuple_position = get_target_entry_resno((ParseState *)cpstate,
        *target_list, node->name);

    return rel;
}

/*
 * Transform logic for a node in a create clause that was not previously
 * declared.
 */
static cypher_target_node* transform_create_cypher_new_node(cypher_parsestate *cpstate,
                                                            List **target_list,
                                                            cypher_node *node)
{
    ParseState *pstate = (ParseState *)cpstate;
    cypher_target_node *rel = make_ag_node(cypher_target_node);
    Relation label_relation;
    RangeVar *rv;
    RangeTblEntry *rte;
    TargetEntry *te;
    Expr *props;
    char *alias;
    int resno;

    rel->type = LABEL_KIND_VERTEX;
    rel->tuple_position = InvalidAttrNumber;
    rel->variable_name = NULL;
    rel->resultRelInfo = NULL;

    if (!node->label)
    {
        rel->label_name = "";
        /*
         *  If no label is specified, assign the generic label name that
         *  all labels are descendents of.
         */
        node->label = AG_DEFAULT_LABEL_VERTEX;
    }
    else
    {
        rel->label_name = node->label;
    }

    // create the label entry if it does not exist
    if (!label_exists(node->label, cpstate->graph_oid))
    {
        List *parent;
        RangeVar *rv;

        rv = get_label_range_var(cpstate->graph_name, cpstate->graph_oid,
                                 AG_DEFAULT_LABEL_VERTEX);

        parent = list_make1(rv);

        create_label(cpstate->graph_name, node->label, LABEL_TYPE_VERTEX,
                     parent);
    }

    rel->flags = CYPHER_TARGET_NODE_FLAG_INSERT;

    rv = makeRangeVar(cpstate->graph_name, node->label, -1);
    label_relation = parserOpenTable(&cpstate->pstate, rv, RowExclusiveLock);

    // Store the relid
    rel->relid = RelationGetRelid(label_relation);

    rte = addRangeTableEntryForRelation((ParseState *)cpstate, label_relation,
                                        NULL, false, false);
    rte->requiredPerms = ACL_INSERT;

    // id
    rel->id_expr = (Expr *)build_column_default(label_relation,
                                                Anum_ag_label_vertex_table_id);

    // properties
    alias = get_next_default_alias(cpstate);
    resno = pstate->p_next_resno++;

    props = cypher_create_properties(cpstate, rel, label_relation, node->props,
                                     ENT_VERTEX);

    rel->prop_attr_num = resno - 1;
    te = makeTargetEntry(props, resno, alias, false);
    *target_list = lappend(*target_list, te);

    heap_close(label_relation, NoLock);

    if (node->name)
    {
        rel->variable_name = node->name;
        te = placeholder_target_entry(cpstate, node->name);
        rel->tuple_position = te->resno;
        *target_list = lappend(*target_list, te);
        rel->flags |= CYPHER_TARGET_NODE_IS_VAR;
    }
    else
    {
        node->name = get_next_default_alias(cpstate);
    }

    return rel;
}

/*
 * Variable Edges cannot be created until the executor phase, because we
 * don't know what their start and end node ids will be. Therefore, path
 * variables cannot be created either. Create a placeholder entry that we
 * will replace in the execution phase. Do this for nodes too, to be
 * consistent.
 */
static TargetEntry *placeholder_target_entry(cypher_parsestate *cpstate,
                                             char *name)
{
    ParseState *pstate = (ParseState *)cpstate;
    Expr *n;
    int resno;

    n = (Expr *)makeNullConst(AGTYPEOID, -1, InvalidOid);
    n = add_volatile_wrapper(pstate, n);

    resno = pstate->p_next_resno++;

    return makeTargetEntry(n, resno, name, false);
}

/*
 * Build the target list for an entity that is not a previously declared
 * variable.
 */
static Expr *cypher_create_properties(cypher_parsestate *cpstate,
                                      cypher_target_node *rel,
                                      Relation label_relation, Node *props,
                                      enum transform_entity_type type)
{
    ParseState *pstate = (ParseState *)cpstate;
    Expr *properties;

    if (props != NULL && is_ag_node(props, cypher_param))
    {
        cypher_param *param = (cypher_param *)props;

        ereport(ERROR,
                (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                 errmsg("properties in a CREATE clause as a parameter is not supported"),
                 parser_errposition(pstate, param->location)));
    }

    if (props)
    {
        if (is_ag_node(props, cypher_map))
            ((cypher_map *)props)->keep_null = false;

        properties = (Expr *)transform_cypher_expr(cpstate, props,
                                                   EXPR_KIND_INSERT_TARGET);
    }
    else if (type == ENT_VERTEX)
    {
        properties = (Expr *)build_column_default(
            label_relation, Anum_ag_label_vertex_table_properties);
    }
    else if (type == ENT_EDGE)
    {
        properties = (Expr *)build_column_default(
            label_relation, Anum_ag_label_edge_table_properties);
    }
    else
    {
        ereport(ERROR, (errmsg_internal("unreconized entity type")));
    }

    // add a volatile wrapper call to prevent the optimizer from removing it
    return add_volatile_wrapper(pstate, properties);
}

/*
 * This function is similar to transformFromClause() that is called with a
 * single RangeSubselect.
 */
static RangeTblEntry* transform_cypher_clause_as_subquery(cypher_parsestate *cpstate,
    transform_method transform,
    cypher_clause *clause,
    Alias *alias,
    bool add_rte_to_query)
{
    ParseState *pstate = (ParseState *)cpstate;
    Query *query;
    RangeTblEntry *rte;
    ParseExprKind old_expr_kind = pstate->p_expr_kind;
    bool lateral = pstate->p_lateral_active;

    /*
     * We allow expression kinds of none, where, and subselect. Others MAY need
     * to be added depending. However, at this time, only these are needed.
     */
    Assert(pstate->p_expr_kind == EXPR_KIND_NONE ||
           pstate->p_expr_kind == EXPR_KIND_OTHER ||
           pstate->p_expr_kind == EXPR_KIND_WHERE ||
           pstate->p_expr_kind == EXPR_KIND_SELECT_TARGET ||
           pstate->p_expr_kind == EXPR_KIND_FROM_SUBSELECT);

    /*
     * As these are all sub queries, if this is just of type NONE, note it as a
     * SUBSELECT. Other types will be dealt with as needed.
     */
    if (pstate->p_expr_kind == EXPR_KIND_NONE)
    {
        pstate->p_expr_kind = EXPR_KIND_FROM_SUBSELECT;
    }
    else if (pstate->p_expr_kind == EXPR_KIND_OTHER)
    {
	// this is a lateral subselect for the MERGE
        pstate->p_expr_kind = EXPR_KIND_FROM_SUBSELECT;
        lateral = true;
    }
    /*
     * If this is a WHERE, pass it through and set lateral to true because it
     * needs to see what comes before it.
     */

    query = analyze_cypher_clause(transform, clause, cpstate);

    /* set pstate kind back */
    pstate->p_expr_kind = old_expr_kind;

    if (alias == NULL)
    {
        alias = makeAlias(PREV_CYPHER_CLAUSE_ALIAS, NIL);
    }

    rte = addRangeTableEntryForSubquery(pstate, query, alias, lateral, true);

    /*
     * NOTE: skip namespace conflicts check if the rte will be the only
     *       RangeTblEntry in pstate
     */
    if (list_length(pstate->p_rtable) > 1)
    {
        List *namespaceoid = NULL;
        int rtindex = 0;

        /* get the index of the last entry */
        rtindex = list_length(pstate->p_rtable);
        /* the rte at the end should be the rte just added */
        if (rte != rt_fetch(rtindex, pstate->p_rtable))
        {
            ereport(ERROR,
                    (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                     errmsg("rte must be last entry in p_rtable")));
        }

        namespaceoid = list_make1(create_namespace_item(rte, true, true, false, true));
        checkNameSpaceConflicts(pstate, pstate->p_relnamespace, namespaceoid);
    }

    if(add_rte_to_query)
    {
        // all variables(attributes) from the previous clause(subquery) are visible
        addRTEtoQuery(pstate, rte, true, false, true);
    }

    return rte;
}

/*
 * When we are done transforming a clause, before transforming the next clause
 * iterate through the transform entities and mark them as not belonging to
 * the clause that is currently being transformed.
 */
static void advance_transform_entities_to_next_clause(List *entities)
{
    ListCell *lc;

    foreach (lc, entities)
    {
        transform_entity *entity = (transform_entity*)lfirst(lc);

        entity->declared_in_current_clause = false;
    }
}

static Query *analyze_cypher_clause(transform_method transform,
                                    cypher_clause *clause,
                                    cypher_parsestate *parent_cpstate)
{
    cypher_parsestate *cpstate;
    Query *query;
    ParseState *parent_pstate = (ParseState*)parent_cpstate;
    ParseState *pstate;

    cpstate = make_cypher_parsestate(parent_cpstate);
    pstate = (ParseState*)cpstate;

    /* copy the expr_kind down to the child */
    pstate->p_expr_kind = parent_pstate->p_expr_kind;

    query = transform(cpstate, clause);

    advance_transform_entities_to_next_clause(cpstate->entities);

    parent_cpstate->entities = list_concat(parent_cpstate->entities,
                                           cpstate->entities);

    free_cypher_parsestate(cpstate);

    return query;
}

static TargetEntry *findTarget(List *targetList, char *resname)
{
    ListCell *lt;
    TargetEntry *te = NULL;

    if (resname == NULL)
    {
        return NULL;
    }

    foreach (lt, targetList)
    {
        te = (TargetEntry*)lfirst(lt);

        if (te->resjunk)
        {
            continue;
        }

        if (strcmp(te->resname, resname) == 0)
        {
            return te;
        }
    }

    return NULL;
}

static bool clause_is_dml(cypher_clause *clause)
{
    return is_ag_node(clause->self, cypher_create) ||
           is_ag_node(clause->self, cypher_set) ||
           is_ag_node(clause->self, cypher_delete) ||
           is_ag_node(clause->self, cypher_merge);
}

static bool clause_chain_has_dml(cypher_clause *clause)
{
    while (clause != NULL) {
        if (clause_is_dml(clause)) {
            return true;
        }

        clause = clause->prev;
    }

    return false;
}

/*
 * Build a false WHERE clause for an invalid label. A volatile predicate is
 * required after DML so constant folding cannot skip the predecessor write.
 */
static Node *make_false_where_clause(bool volatile_needed)
{
    if (volatile_needed) {
        FuncCall *random_fn;
        NullTest *nt;

        random_fn = makeFuncCall(
            list_make2(makeString("pg_catalog"), makeString("random")),
            NIL, -1);

        nt = makeNode(NullTest);
        nt->arg = (Expr *)random_fn;
        nt->nulltesttype = IS_NULL;
        nt->argisrow = false;

        return (Node *)nt;
    } else {
        cypher_bool_const *lhs = make_ag_node(cypher_bool_const);
        cypher_bool_const *rhs = make_ag_node(cypher_bool_const);

        lhs->boolean = true;
        lhs->location = -1;
        rhs->boolean = false;
        rhs->location = -1;

        return (Node *)makeSimpleA_Expr(AEXPR_OP, "=",
                                        (Node *)lhs, (Node *)rhs, -1);
    }
}

/*
 * Wrap the expression with a volatile function, to prevent the optimer from
 * elimating the expression.
 */
static Expr *add_volatile_wrapper(ParseState *pstate, Expr *node)
{
    Oid node_type;
    Oid oid;
    Node *coerced;

    if (node == NULL) {
        ereport(ERROR,
                (errmsg_internal("add_volatile_wrapper: NULL expression")));
    }

    oid = get_ag_func_oid("agtype_volatile_wrapper", 1, ANYOID);
    if (IsA(node, FuncExpr) && oid == ((FuncExpr *)node)->funcid) {
        return node;
    }

    coerced = coerce_entity_to_agtype(pstate, (Node *)node);
    node_type = exprType(coerced);
    if (node_type != AGTYPEOID) {
        coerced = coerce_to_target_type(pstate, coerced, node_type,
                                        AGTYPEOID, -1, COERCION_EXPLICIT,
                                        COERCE_IMPLICIT_CAST, NULL, NULL, -1);
        if (coerced == NULL) {
            ereport(ERROR,
                    (errcode(ERRCODE_CANNOT_COERCE),
                     errmsg("cannot convert volatile expression to agtype")));
        }
    }

    return (Expr *)makeFuncExpr(oid, AGTYPEOID, list_make1(coerced),
                                InvalidOid, InvalidOid,
                                COERCE_EXPLICIT_CALL);
}

/*
 * from postgresql parse_sub_analyze
 * Modified entry point for recursively analyzing a sub-statement in union.
 */
Query *cypher_parse_sub_analyze_union(cypher_clause *clause,
                                cypher_parsestate *cpstate,
                                CommonTableExpr *parentCTE,
                                bool locked_from_parent,
                                bool resolve_unknowns)
{
    cypher_parsestate *state = make_cypher_parsestate(cpstate);
    Query *query;

    state->pstate.p_parent_cte = parentCTE;
    state->pstate.p_locked_from_parent = locked_from_parent;
    state->pstate.p_resolve_unknowns = resolve_unknowns;

    query = transform_cypher_clause(state, clause);

    free_cypher_parsestate(state);

    return query;
}

/*
 * from postgresql parse_sub_analyze
 * Entry point for recursively analyzing a sub-statement.
 */
Query *cypher_parse_sub_analyze(Node *parseTree,
                                cypher_parsestate *cpstate,
                                CommonTableExpr *parentCTE,
                                bool locked_from_parent,
                                bool resolve_unknowns)
{
    ParseState *pstate = make_parsestate((ParseState*)cpstate);
    cypher_clause *clause;
    Query *query;

    pstate->p_parent_cte = parentCTE;
    pstate->p_locked_from_parent = locked_from_parent;
    pstate->p_resolve_unknowns = resolve_unknowns;

    clause = (cypher_clause*)palloc0(sizeof(cypher_clause));
    clause->self = parseTree;
    query = transform_cypher_clause(cpstate, clause);

    free_parsestate(pstate);

    return query;
}

/*
 * MERGE stores ON SET expressions inside executor metadata because the
 * planner can remove their target-list entries. Resolve each expression while
 * the transformed target list still has the original expression tree.
 */
static void resolve_merge_set_exprs(List *set_items, List *target_list,
                                    const char *clause_name)
{
    ListCell *lc;

    foreach (lc, set_items)
    {
        cypher_update_item *item = (cypher_update_item *)lfirst(lc);
        TargetEntry *tle = get_tle_by_resno(target_list, item->prop_position);

        if (tle == NULL) {
            ereport(ERROR,
                    (errcode(ERRCODE_INTERNAL_ERROR),
                     errmsg("%s target entry not found at position %d",
                            clause_name, item->prop_position)));
        }

        item->prop_expr = (Node *)tle->expr;
    }
}

/*
 * Function for transforming MERGE.
 *
 * There are two cases for the form Query that is returned from here will
 * take:
 *
 * 1. If there is no previous clause, the query will have a subquery that
 * represents the path as a select staement, similar to match with a targetList
 * that is all declared variables and the FuncExpr that represents the MERGE
 * clause with its needed metadata information, that will be caught in the
 * planner phase and converted into a path.
 *
 * 2. If there is a previous clause then the query will have two subqueries.
 * The first query will be for the previous clause that we recursively handle.
 * The second query will be for the path that this MERGE clause defines. The
 * two subqueries will be joined together using a LATERAL LEFT JOIN with the
 * previous query on the left and the MERGE path subquery on the right. Like
 * case 1 the targetList will have all the decalred variables and a FuncExpr
 * that represents the MERGE clause with its needed metadata information, that
 * will be caught in the planner phase and converted into a path.
 *
 * This will allow us to be capable of handling the 2 cases that exist with a
 * MERGE clause correctly.
 *
 * Case 1: the path already exists. In this case we do not need to create
 * the path and MERGE will simply pass the tuple information up the execution
 * tree.
 *
 * Case 2: the path does not exist. In this case the LEFT part of the join
 * will not prevent the tuples from the previous clause from being emitted. We
 * can catch when this happens in the execution phase and create the missing
 * data, before passing up the execution tree.
 *
 * It should be noted that both cases can happen in the same query. If the
 * MERGE clause references a variable from a previous clause, it could be that
 * for one tuple the path exists (or there is multiple paths that exist and all
 * paths must be emitted) and for another the path does not exist. This is
 * similar to OPTIONAL MATCH, however with the added feature of creating the
 * path if not there, rather than just emiting NULL.
 */
static Query *transform_cypher_merge(cypher_parsestate *cpstate,
                                     cypher_clause *clause)
{
    ParseState *pstate = (ParseState *) cpstate;
    cypher_clause *merge_clause_as_match;
    cypher_create_path *merge_path;
    cypher_merge *self = (cypher_merge *)clause->self;
    cypher_merge_information *merge_information;
    Query *query;
    FuncExpr *func_expr;
    TargetEntry *tle;

    Assert(is_ag_node(self->path, cypher_path));

    merge_information = make_ag_node(cypher_merge_information);

    query = makeNode(Query);
    query->commandType = CMD_SELECT;
    query->targetList = NIL;

    merge_information->flags = CYPHER_CLAUSE_FLAG_NONE;

    // make the merge node into a match node
    merge_clause_as_match = convert_merge_to_match(self);

    /*
     * If there is a previous clause we need to turn this query into a lateral
     * join. See the function transform_merge_make_lateral_join for details.
     */
    if (clause->prev != NULL)
    {
        merge_path = transform_merge_make_lateral_join(cpstate, query, clause,
                                                       merge_clause_as_match);

        merge_information->flags |= CYPHER_CLAUSE_FLAG_PREVIOUS_CLAUSE;
    }
    else
    {
        // make the merge node into a match node
        cypher_clause *merge_clause_as_match = convert_merge_to_match(self);

        /*
         * Create the metadata needed for creating missing paths.
         */
        merge_path = transform_cypher_merge_path(cpstate, &query->targetList,
                                                 (cypher_path *)self->path);

        /*
         * If there is not a previous clause, then treat the MERGE's path
         * itself as the previous clause. We need to do this because if the
         * pattern exists, then we need to path all paths that match the
         * query patterns in the execution phase. WE way to do that by
         * converting the merge to a match and have the match logic create the
         * query. the merge execution phase will just pass the results up the
         * execution tree if the path exists.
         */
        handle_prev_clause(cpstate, query, merge_clause_as_match, false);

        /*
         * For the metadata need to create paths, find the tuple position that
         * will represent the entity in the execution phase.
         */
        transform_cypher_merge_mark_tuple_position(pstate,
                                                   query->targetList,
                                                   merge_path);
    }

    merge_information->graph_oid = cpstate->graph_oid;
    merge_information->path = merge_path;

    if (self->on_match != NIL)
    {
        merge_information->on_match_set_info =
            transform_cypher_set_item_list(cpstate, self->on_match, query);
        merge_information->on_match_set_info->clause_name =
            "MERGE ON MATCH SET";
        merge_information->on_match_set_info->graph_name = cpstate->graph_name;
        resolve_merge_set_exprs(
            merge_information->on_match_set_info->set_items,
            query->targetList, "ON MATCH SET");
    }

    if (self->on_create != NIL)
    {
        merge_information->on_create_set_info =
            transform_cypher_set_item_list(cpstate, self->on_create, query);
        merge_information->on_create_set_info->clause_name =
            "MERGE ON CREATE SET";
        merge_information->on_create_set_info->graph_name = cpstate->graph_name;
        resolve_merge_set_exprs(
            merge_information->on_create_set_info->set_items,
            query->targetList, "ON CREATE SET");
    }

    if (!clause->next)
    {
        merge_information->flags |= CYPHER_CLAUSE_FLAG_TERMINAL;
    }

    /*
     * Creates the function expression that the planner will find and
     * convert to a MERGE path.
     */
    func_expr = make_clause_func_expr(MERGE_CLAUSE_FUNCTION_NAME,
                                      (Node *)merge_information);

    // Create the target entry
    tle = makeTargetEntry((Expr *)func_expr, pstate->p_next_resno++,
                          AGE_VARNAME_MERGE_CLAUSE, false);

    merge_information->merge_function_attr = tle->resno;
    query->targetList = lappend(query->targetList, tle);

    markTargetListOrigins(pstate, query->targetList);

    query->rtable = pstate->p_rtable;
    query->jointree = makeFromExpr(pstate->p_joinlist, NULL);

    query->hasSubLinks = pstate->p_hasSubLinks;

    assign_query_collations(pstate, query);

    return query;
}

static Query *transform_cypher_call_stmt(cypher_parsestate *cpstate,
                                         cypher_clause *clause)
{
    ParseState *pstate = (ParseState *)cpstate;
    cypher_call *self = (cypher_call *)clause->self;

    if (!clause->prev && !clause->next) {
        if (self->where) {
            Assert(self->yield_items);

            ereport(ERROR,
                    (errcode(ERRCODE_SYNTAX_ERROR),
                     errmsg("Cannot use standalone CALL with WHERE"),
                     errhint("Instead use `CALL ... WITH * WHERE ... RETURN *`"),
                     parser_errposition(pstate,
                                        exprLocation((Node *)self->where))));
        }

        return transform_cypher_call_subquery(cpstate, clause);
    }

    if (!self->yield_items) {
        ereport(ERROR,
                (errcode(ERRCODE_SYNTAX_ERROR),
                 errmsg("Procedure call inside a query does not support naming results implicitly"),
                 errhint("Name explicitly using `YIELD` instead"),
                 parser_errposition(pstate, exprLocation((Node *)self))));
    }

    if (!clause->next) {
        ereport(ERROR,
                (errcode(ERRCODE_SYNTAX_ERROR),
                 errmsg("Query cannot conclude with CALL"),
                 errhint("Must be RETURN or an update clause"),
                 parser_errposition(pstate, exprLocation((Node *)self))));
    }

    return transform_cypher_clause_with_where(cpstate,
                                              transform_cypher_call_subquery,
                                              clause,
                                              self->where);
}

static Query *transform_cypher_call_subquery(cypher_parsestate *cpstate,
                                             cypher_clause *clause)
{
    ParseState *pstate = (ParseState *)cpstate;
    cypher_call *self = (cypher_call *)clause->self;
    Query *query;
    char *colname;
    FuncExpr *func_expr;
    TargetEntry *tle;

    query = makeNode(Query);
    query->commandType = CMD_SELECT;

    if (clause->prev) {
        handle_prev_clause(cpstate, query, clause->prev, false);
    }

    func_expr = (FuncExpr *)transform_cypher_expr(cpstate,
        (Node *)self->funccall,
        EXPR_KIND_FROM_FUNCTION);

    colname = strVal(llast(self->funccall->funcname));
    tle = makeTargetEntry((Expr *)func_expr,
                          (AttrNumber)(list_length(query->targetList) + 1),
                          colname,
                          false);

    if (self->yield_items) {
        List *yield_target_list = list_make1(tle);
        ListCell *lc;

        foreach (lc, self->yield_items)
        {
            ResTarget *target = (ResTarget *)lfirst(lc);
            ColumnRef *var;
            TargetEntry *yielded_tle;
            char *yield_name;

            if (!IsA(target->val, ColumnRef)) {
                ereport(ERROR,
                        (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                         errmsg("YIELD item must be ColumnRef"),
                         parser_errposition(pstate,
                                            exprLocation((Node *)target))));
            }

            var = (ColumnRef *)target->val;
            yield_name = strVal(linitial(var->fields));
            if (findTarget(yield_target_list, yield_name) == NULL) {
                ereport(ERROR,
                        (errcode(ERRCODE_INVALID_COLUMN_REFERENCE),
                         errmsg("Unknown CALL output"),
                         parser_errposition(pstate,
                                            exprLocation((Node *)target))));
            }

            if (target->name) {
                if (findTarget(query->targetList, target->name) != NULL) {
                    ereport(ERROR,
                            (errcode(ERRCODE_DUPLICATE_ALIAS),
                             errmsg("duplicate variable \"%s\"", target->name),
                             parser_errposition(pstate,
                                                exprLocation((Node *)target))));
                }

                yielded_tle = makeTargetEntry((Expr *)func_expr,
                                              (AttrNumber)(list_length(query->targetList) + 1),
                                              target->name,
                                              false);
            } else {
                if (findTarget(query->targetList, yield_name) != NULL) {
                    ereport(ERROR,
                            (errcode(ERRCODE_DUPLICATE_ALIAS),
                             errmsg("duplicate variable \"%s\"", yield_name),
                             parser_errposition(pstate,
                                                exprLocation((Node *)target))));
                }

                yielded_tle = makeTargetEntry((Expr *)func_expr,
                                              (AttrNumber)(list_length(query->targetList) + 1),
                                              colname,
                                              false);
            }

            query->targetList = lappend(query->targetList, yielded_tle);
        }
        } else {
        query->targetList = list_make1(tle);
    }

    markTargetListOrigins(pstate, query->targetList);

    query->rtable = pstate->p_rtable;
    query->jointree = makeFromExpr(pstate->p_joinlist, NULL);
    query->hasAggs = pstate->p_hasAggs;
    query->hasSubLinks = pstate->p_hasSubLinks;
    query->hasTargetSRFs = pstate->p_hasTargetSRFs;

    assign_query_collations(pstate, query);

    if (pstate->p_hasAggs ||
        query->groupClause || query->groupingSets || query->havingQual) {
        parse_check_aggregates(pstate, query);
    }

    return query;
}

/*
 * This function does the heavy lifting of transforming a MERGE clause that has
 * a clause before it in the query of turning that into a lateral left join.
 * The previous clause will still be able to emit tuples if the path defined in
 * MERGE clause is not found. In that case variable assinged in the MERGE
 * clause will be emitted as NULL (same as OPTIONAL MATCH).
 */
static cypher_create_path* transform_merge_make_lateral_join(cypher_parsestate *cpstate,
                                                             Query *query,
                                                             cypher_clause *clause,
                                                             cypher_clause *isolated_merge_clause)
{
    cypher_create_path *merge_path;
    ParseState *pstate = (ParseState *) cpstate;
    int i;
    Alias *l_alias;
    Alias *r_alias;
    RangeTblEntry *rte;
    RangeTblEntry *l_rte, *r_rte;
    ParseNamespaceItem *l_nsitem, *r_nsitem;
    JoinExpr *j = makeNode(JoinExpr);
    List *res_colnames = NIL, *res_colvars = NIL;
    ParseNamespaceItem *nsitem;
    ParseExprKind tmp;
    ListCell *lc;
    cypher_merge *self = (cypher_merge *)clause->self;
    cypher_path *path;

    Assert(is_ag_node(self->path, cypher_path));

    path = (cypher_path *)self->path;

    r_alias = makeAlias(CYPHER_OPT_RIGHT_ALIAS, NIL);
    l_alias = makeAlias(PREV_CYPHER_CLAUSE_ALIAS, NIL);

    j->jointype = JOIN_LEFT;

    /*
     * transform the previous clause
     */
    j->larg = transform_clause_for_join(cpstate, clause->prev, &l_rte,
                                            &l_nsitem, l_alias);

    /*
     * The write executor consumes agtype tuples. Convert entity outputs before
     * exposing the left subquery to name resolution so its RTE and all Vars
     * derived from it agree on the same type.
     */
    foreach (lc, l_rte->subquery->targetList)
    {
        TargetEntry *te = (TargetEntry *)lfirst(lc);
        Oid te_type = exprType((Node *)te->expr);
        if (te_type == VERTEXOID || te_type == EDGEOID) {
            te->expr = add_volatile_wrapper(pstate, te->expr);
        }
    }

    pstate->p_relnamespace = lappend(pstate->p_relnamespace, l_nsitem);
    pstate->p_varnamespace = lappend(pstate->p_varnamespace, l_nsitem);

    /*
     * Get the merge path now. This is the only moment where it is simple
     * to know if a variable was declared in the MERGE clause or a previous
     * clause. Unlike create, we do not add these missing variables to the
     * targetList, we just create all the metadata necessary to make the
     * potentially missing parts of the path.
     */
    merge_path = transform_cypher_merge_path(cpstate, &query->targetList,
                                             path);

    /*
     * Transform this MERGE clause as a match clause, mark the parsestate
     * with the flag that a lateral join is active
     */
    pstate->p_lateral_active = true;
    tmp = pstate->p_expr_kind;
    pstate->p_expr_kind = EXPR_KIND_OTHER;

    // transform MERGE
    j->rarg = transform_clause_for_join(cpstate, isolated_merge_clause, &r_rte,
                                            &r_nsitem, r_alias);

    /*
     * MERGE metadata is evaluated against an agtype scantuple. Normalize new
     * entity variables before the join RTE exposes them to ON SET expressions.
     */
    foreach (lc, r_rte->subquery->targetList)
    {
        TargetEntry *te = (TargetEntry *)lfirst(lc);
        Oid te_type = exprType((Node *)te->expr);
        if (te_type == VERTEXOID || te_type == EDGEOID) {
            te->expr = add_volatile_wrapper(pstate, te->expr);
        }
    }

    // deactivate the lateral flag
    pstate->p_lateral_active = false;
    pstate->p_relnamespace = NULL;
    pstate->p_varnamespace=NULL;

    /*
     * Resolve the column names and variables between the two subqueries,
     * in most cases, we can expect there to be overlap
     */
    get_res_cols(pstate, l_rte, r_rte, &res_colnames, &res_colvars);

    // make the RTE for the join
    rte = addRangeTableEntryForJoin(pstate, res_colnames, j->jointype,
                                        res_colvars, j->alias, true);

    j->rtindex = RTERangeTablePosn(pstate, rte, NULL);

    /*
     * The index of a node in the p_joinexpr list is expected to match the
     * rtindex the join expression is for. Add NULLs for all the previous
     * rtindexes and add the JoinExpr.
     */
    for (i = list_length(pstate->p_joinexprs) + 1; i < j->rtindex; i++)
    {
        pstate->p_joinexprs = lappend(pstate->p_joinexprs, NULL);
    }

    pstate->p_joinexprs = lappend(pstate->p_joinexprs, j);

    Assert(list_length(pstate->p_joinexprs) == j->rtindex);

    pstate->p_joinlist = lappend(pstate->p_joinlist, j);

    pstate->p_expr_kind = tmp;

    /*
     * Create the namespace item for the joined subqueries, and append
     * to the ParseState's list of namespaces.
     */
    nsitem = create_namespace_item(rte, true, true, false, true);
    pstate->p_relnamespace = lappend(pstate->p_relnamespace, nsitem);
    pstate->p_varnamespace = lappend(pstate->p_varnamespace, nsitem);
    /*
     * Create the targetList from the joined subqueries, add everything.
     */
    query->targetList = list_concat(query->targetList,
                                    make_target_list_from_join(pstate, rte));

    /*
     * Preserve every user-visible value needed by the write executor. The
     * wrapper also establishes the legacy agtype tuple contract for scalar
     * expressions such as id(entity).
     */
    foreach (lc, query->targetList)
    {
        TargetEntry *te = (TargetEntry *)lfirst(lc);

        if (IsA(te->expr, Var) && te->resname != NULL &&
            pg_strncasecmp(te->resname, AGE_DEFAULT_VARNAME_PREFIX,
                           strlen(AGE_DEFAULT_VARNAME_PREFIX)) != 0) {
            te->expr = add_volatile_wrapper(pstate, te->expr);
        }
    }

    /*
     * For the metadata need to create paths, find the tuple position that
     * will represent the entity in the execution phase.
     */
    transform_cypher_merge_mark_tuple_position(pstate, query->targetList,
                                               merge_path);

    return merge_path;
}

/*
 * Iterate through the path and find the TargetEntry in the target_list
 * that each cypher_target_node is referencing. Add the volatile wrapper
 * function to keep the optimizer from removing the TargetEntry.
 */
static void
transform_cypher_merge_mark_tuple_position(ParseState *pstate,
                                           List *target_list,
                                           cypher_create_path *path)
{
    ListCell *lc = NULL;

    if (path->var_name)
    {
        TargetEntry *te = findTarget(target_list, path->var_name);

        /*
         * Add the volatile wrapper function around the expression, this
         * ensures the optimizer will not remove the expression, if nothing
         * other than a private data structure needs it.
         */
        te->expr = add_volatile_wrapper(pstate, te->expr);

        // Mark the tuple position the target_node is for.
        path->path_attr_num = te->resno;
    }

    foreach (lc, path->target_nodes)
    {
        cypher_target_node *node = (cypher_target_node*)lfirst(lc);

        TargetEntry *te = findTarget(target_list, node->variable_name);

        /*
         * Add the volatile wrapper function around the expression, this
         * ensures the optimizer will not remove the expression, if nothing
         * other than a private data structure needs it.
         */
        te->expr = add_volatile_wrapper(pstate, te->expr);

        // Mark the tuple position the target_node is for.
        node->tuple_position = te->resno;
    }
}

/*
 * Creates the target nodes for a merge path. If MERGE has a path that doesn't
 * exist then in the MERGE clause we act like a CREATE clause. This function
 * sets up the metadata needed for that process.
 */
static cypher_create_path* transform_cypher_merge_path(cypher_parsestate *cpstate,
                                                       List **target_list,
                                                       cypher_path *path)
{
    ParseState *pstate = (ParseState *)cpstate;
    ListCell *lc;
    List *transformed_path = NIL;
    cypher_create_path *ccp = make_ag_node(cypher_create_path);
    bool in_path = path->var_name != NULL;
    bool has_edge = list_length(path->path) > 1;

    ccp->path_attr_num = InvalidAttrNumber;

    foreach (lc, path->path)
    {
        if (is_ag_node(lfirst(lc), cypher_node))
        {
            cypher_node *node = (cypher_node*)lfirst(lc);
            cypher_target_node *rel = NULL;
            ListCell *transformed_cell;

            if (in_path && node->name != NULL &&
                strcmp(path->var_name, node->name) == 0)
            {
                ereport(ERROR,
                        (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                         errmsg("variable \"%s\" is for a path", node->name),
                         parser_errposition(pstate, node->location)));
            }

            foreach (transformed_cell, transformed_path)
            {
                cypher_target_node *existing =
                    (cypher_target_node *)lfirst(transformed_cell);
                bool same_name = node->name != NULL &&
                                 existing->variable_name != NULL &&
                                 strcmp(node->name,
                                        existing->variable_name) == 0;
                if (!same_name)
                    continue;

                if (existing->type != LABEL_KIND_VERTEX) {
                    ereport(ERROR,
                            (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                             errmsg("variable \"%s\" is for an edge",
                                    node->name),
                             parser_errposition(pstate, node->location)));
                }

                if (node->label != NULL &&
                    (existing->label_name == NULL ||
                     strcmp(existing->label_name, node->label) != 0)) {
                    ereport(ERROR,
                            (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                             errmsg("multiple labels for variable '%s' are not supported",
                                    node->name),
                             parser_errposition(pstate, node->location)));
                }

                rel = make_ag_node(cypher_target_node);
                rel->type = existing->type;
                rel->flags = existing->flags;
                rel->dir = existing->dir;
                rel->id_expr = existing->id_expr;
                rel->id_expr_state = existing->id_expr_state;
                rel->prop_expr = existing->prop_expr;
                rel->prop_expr_state = existing->prop_expr_state;
                rel->prop_attr_num = existing->prop_attr_num;
                rel->resultRelInfo = existing->resultRelInfo;
                rel->elemTupleSlot = existing->elemTupleSlot;
                rel->relid = existing->relid;
                rel->label_name = existing->label_name;
                rel->variable_name = existing->variable_name;
                rel->tuple_position = existing->tuple_position;
                rel->flags &= ~CYPHER_TARGET_NODE_FLAG_INSERT;
                rel->flags |= EXISTING_VARAIBLE_DECLARED_SAME_CLAUSE;
                break;
            }

            if (rel == NULL)
            {
                rel = transform_merge_cypher_node(cpstate, target_list, node,
                                                  has_edge);
            }

            if (in_path)
            {
                rel->flags |= CYPHER_TARGET_NODE_IN_PATH_VAR;
            }

            transformed_path = lappend(transformed_path, rel);
        }
        else if (is_ag_node(lfirst(lc), cypher_relationship))
        {
            cypher_relationship *edge = (cypher_relationship*)lfirst(lc);
            ListCell *transformed_cell;

            if (in_path && edge->name != NULL &&
                strcmp(path->var_name, edge->name) == 0)
            {
                ereport(ERROR,
                        (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                         errmsg("variable \"%s\" is for a path", edge->name),
                         parser_errposition(pstate, edge->location)));
            }

            foreach (transformed_cell, transformed_path)
            {
                cypher_target_node *existing =
                    (cypher_target_node *)lfirst(transformed_cell);

                if (edge->name == NULL || existing->variable_name == NULL ||
                    strcmp(edge->name, existing->variable_name) != 0) {
                    continue;
                }

                if (existing->type == LABEL_KIND_EDGE) {
                    ereport(ERROR,
                            (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                             errmsg("a duplicate edge variable \"%s\" is not permitted within a path",
                                    edge->name),
                             parser_errposition(pstate, edge->location)));
                }

                ereport(ERROR,
                        (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                         errmsg("variable \"%s\" is for a vertex",
                                edge->name),
                         parser_errposition(pstate, edge->location)));
            }

            cypher_target_node *rel =
                transform_merge_cypher_edge(cpstate, target_list, edge);

            if (in_path)
            {
                rel->flags |= CYPHER_TARGET_NODE_IN_PATH_VAR;
            }

            transformed_path = lappend(transformed_path, rel);
        }
        else
        {
            ereport(ERROR,
                    (errmsg_internal("unreconized node in create pattern")));
        }
    }

    // store the path's variable name
    if (path->var_name)
    {
        ccp->var_name = path->var_name;
    }

    ccp->target_nodes = transformed_path;

    return ccp;
}

/*
 * Transforms the parse cypher_relationship to a target_entry for merge.
 * All edges that have variables assigned in a merge must be declared in
 * the merge. Throw an error otherwise.
 */
static cypher_target_node* transform_merge_cypher_edge(cypher_parsestate *cpstate, List **target_list,
                             cypher_relationship *edge)
{
    ParseState *pstate = (ParseState *)cpstate;
    cypher_target_node *rel = make_ag_node(cypher_target_node);
    Relation label_relation;
    RangeVar *rv;
    RangeTblEntry *rte;

    if (edge->name != NULL)
    {
        transform_entity *entity = find_variable(cpstate, edge->name);

        // We found a variable with this variable name, throw an error.
        if (entity != NULL || variable_exists(cpstate, edge->name))
        {
            ereport(ERROR,
                (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                 errmsg("variable %s already exists", edge->name),
                 parser_errposition(pstate, edge->location)));
        }

	rel->flags |= CYPHER_TARGET_NODE_IS_VAR;
    }
    else
    {
        // assign a default variable name.
        edge->name = get_next_default_alias(cpstate);
    }

    rel->type = LABEL_KIND_EDGE;

    // all edges are marked with insert
    rel->flags |= CYPHER_TARGET_NODE_FLAG_INSERT;
    rel->label_name = edge->label;
    rel->variable_name = edge->name;
    rel->resultRelInfo = NULL;

    rel->dir = edge->dir;

    if (!edge->label)
    {
        ereport(ERROR,
                (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                 errmsg("edges declared in a MERGE clause must have a label"),
                 parser_errposition(&cpstate->pstate, edge->location)));
    }

    label_cache_data *edge_label =
        search_label_name_graph_cache(edge->label, cpstate->graph_oid);
    if (edge_label != NULL && edge_label->kind != LABEL_KIND_EDGE)
    {
        ereport(ERROR,
                (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                 errmsg("label %s is for vertices, not edges", edge->label),
                 parser_errposition(pstate, edge->location)));
    }

    // check to see if the label exists, create the label entry if it does not.
    if (edge->label && !label_exists(edge->label, cpstate->graph_oid))
    {
        List *parent;
        RangeVar *rv;

        /*
         * setup the default edge table as the parent table, that we
         * will inherit from.
         */
        rv = get_label_range_var(cpstate->graph_name, cpstate->graph_oid,
                                 AG_DEFAULT_LABEL_EDGE);

        parent = list_make1(rv);

        // create the label
        create_label(cpstate->graph_name, edge->label, LABEL_TYPE_EDGE,
                     parent);
    }

    // lock the relation of the label
    rv = makeRangeVar(cpstate->graph_name, edge->label, -1);
    label_relation = parserOpenTable(&cpstate->pstate, rv, RowExclusiveLock);

    // Store the relid
    rel->relid = RelationGetRelid(label_relation);

    rte = addRangeTableEntryForRelation((ParseState *)cpstate, label_relation,
                                        NULL, false, false);
    rte->requiredPerms = ACL_INSERT;

    // Build Id expression, always use the default logic
    rel->id_expr = (Expr *)build_column_default(label_relation,
                                      Anum_ag_label_edge_table_id);

    rel->prop_expr = cypher_create_properties(cpstate, rel, label_relation,
                                              edge->props, ENT_EDGE);

    // Keep the lock
    heap_close(label_relation, NoLock);

    return rel;
}

/*
 * Function for creating the metadata MERGE will need if MERGE does not find
 * a path to exist
 */
static cypher_target_node* transform_merge_cypher_node(cypher_parsestate *cpstate,
                                                       List **target_list,
                                                       cypher_node *node,
                                                       bool has_edge)
{
    ParseState *pstate = (ParseState *)cpstate;
    cypher_target_node *rel = make_ag_node(cypher_target_node);
    Relation label_relation;
    RangeVar *rv;
    RangeTblEntry *rte;

    if (node->name != NULL)
    {
        transform_entity *entity = find_variable(cpstate, node->name);
        bool var_exists = variable_exists(cpstate, node->name);
        /*
         * A previous-clause column is the authoritative visibility check.
         * WITH may rename a vertex without creating a matching
         * transform_entity, so an entity lookup alone is insufficient.
         */
        if (var_exists)
        {
            if ((entity != NULL && entity->type != ENT_VERTEX) || !has_edge) {
                ereport(ERROR,
                        (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                         errmsg("variable %s already exists", node->name),
                         parser_errposition(pstate, node->location)));
            }

            rel->type = LABEL_KIND_VERTEX;
            rel->tuple_position = InvalidAttrNumber;
            rel->variable_name = node->name;
            rel->resultRelInfo = NULL;
            rel->flags |= CYPHER_TARGET_NODE_MERGE_EXISTS;
            return rel;
        }

        rel->flags |= CYPHER_TARGET_NODE_IS_VAR;
    }
    else
    {
        // assign a default variable name.
        node->name = get_next_default_alias(cpstate);
    }

    rel->type = LABEL_KIND_VERTEX;
    rel->tuple_position = InvalidAttrNumber;
    rel->variable_name = node->name;
    rel->resultRelInfo = NULL;

    if (!node->label)
    {
        rel->label_name = "";
        /*
         *  If no label is specified, assign the generic label name that
         *  all labels are descendents of.
         */
        node->label = AG_DEFAULT_LABEL_VERTEX;
    }
    else
    {
        rel->label_name = node->label;

        label_cache_data *node_label =
            search_label_name_graph_cache(node->label, cpstate->graph_oid);
        if (node_label != NULL && node_label->kind != LABEL_KIND_VERTEX)
        {
            ereport(ERROR,
                    (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                     errmsg("label %s is for edges, not vertices", node->label),
                     parser_errposition(pstate, node->location)));
        }
    }

    // check to see if the label exists, create the label entry if it does not.
    if (node->label && !label_exists(node->label, cpstate->graph_oid))
    {
        List *parent;
        RangeVar *rv;

        /*
         * setup the default vertex table as the parent table, that we
         * will inherit from.
         */
        rv = get_label_range_var(cpstate->graph_name, cpstate->graph_oid,
                                 AG_DEFAULT_LABEL_VERTEX);

        parent = list_make1(rv);

        // create the label
        create_label(cpstate->graph_name, node->label, LABEL_TYPE_VERTEX,
                     parent);
    }

    rel->flags |= CYPHER_TARGET_NODE_FLAG_INSERT;

    rv = makeRangeVar(cpstate->graph_name, node->label, -1);
    label_relation = parserOpenTable(&cpstate->pstate, rv, RowExclusiveLock);

    // Store the relid
    rel->relid = RelationGetRelid(label_relation);

    rte = addRangeTableEntryForRelation((ParseState *)cpstate, label_relation,
                                        NULL, false, false);
    rte->requiredPerms = ACL_INSERT;

    // id
    rel->id_expr = (Expr *)build_column_default(label_relation,
                                                Anum_ag_label_vertex_table_id);

    rel->prop_expr = cypher_create_properties(cpstate, rel, label_relation,
                                              node->props, ENT_VERTEX);

    heap_close(label_relation, NoLock);

    return rel;
}

/*
 * Takes a MERGE parse node and converts it to a MATCH parse node
 */
static cypher_clause *convert_merge_to_match(cypher_merge *merge)
{
    cypher_match *match = make_ag_node(cypher_match);
    cypher_clause *clause = (cypher_clause*)palloc(sizeof(cypher_clause));

    // match supports multiple paths, whereas merge only supports one.
    match->pattern = list_make1(merge->path);
    // MERGE does not support where
    match->where = NULL;

    /*
     *  We do not want the transform logic to transform the previous clauses
     *  with this, just handle this one clause.
     */
    clause->prev = NULL;
    clause->self = (Node *)match;
    clause->next = NULL;

    return clause;
}

/*
 * Creates a namespace item for the given rte. boolean arguements will
 * let the rest of the ParseState know if the relation and/or columns are
 * visible, whether the rte is only usable in lateral joins, and if the rte
 * is accessible in lateral joins.
 */
static ParseNamespaceItem *create_namespace_item(RangeTblEntry *rte,
                                                 bool p_rel_visible,
					         bool p_cols_visible,
                                                 bool p_lateral_only,
					         bool p_lateral_ok)
{
    ParseNamespaceItem *nsitem;

    nsitem = (ParseNamespaceItem*)palloc(sizeof(*nsitem));
    nsitem->p_rte = rte;
    nsitem->p_lateral_only = p_lateral_only;
    nsitem->p_lateral_ok = p_lateral_ok;

    return nsitem;
}

/*
 * Creates the function expression that represents the clause. Adds the
 * extensible node that represents the metadata that the clause needs to
 * handle the clause in the execution phase.
 */
static FuncExpr *make_clause_func_expr(char *function_name,
                                       Node *clause_information)
{
    Const *clause_information_const;
    Oid func_oid;
    FuncExpr *func_expr;

    /*
     * Serialize the clause_information data structure. In certain
     * cases (Prepared Statements and PL/pgsql), the MemoryContext that
     * it is stored in will be destroyed. We need to get it into a format
     * that Postgres' can copy between MemoryContexts. Just making it into
     * an ExtensibleNode does not work, because there are certain parts of
     * Postgres that cannot handle an ExtensibleNode in a function call.
     * So we serialize the data structure and place it into a Const node
     * that can handle these situations AND be copied correctly.
     */
    char* nodeStr = AgNodeToString(clause_information);
    clause_information_const = makeConst(INTERNALOID, -1, InvalidOid, strlen(nodeStr),
                                         PointerGetDatum(nodeStr), false, false);
    func_oid = get_ag_func_oid(function_name, 1, INTERNALOID);

    func_expr = makeFuncExpr(func_oid, AGTYPEOID,
                             list_make1(clause_information_const), InvalidOid,
                             InvalidOid, COERCE_EXPLICIT_CALL);

    return func_expr;
}

/*
 * Utility function that helps a clause add the information needed to
 * the query from the previous clause.
 */
static void handle_prev_clause(cypher_parsestate *cpstate, Query *query,
                               cypher_clause *clause, bool first_rte)
{
    ParseState *pstate = (ParseState *) cpstate;
    RangeTblEntry *rte;
    int rtindex;

    rte = transform_prev_cypher_clause(cpstate, clause, true);
    rtindex = list_length(pstate->p_rtable);

    // rte is the first RangeTblEntry in pstate
    if (first_rte)
    {
        Assert(rtindex == 1);
        if (rtindex != 1) {
            ereport(ERROR,
                    (errcode(ERRCODE_DATATYPE_MISMATCH),
                     errmsg("invalid value for rtindex")));
        }
    }

    // add all the rte's attributes to the current queries targetlist
    query->targetList = expandRelAttrs(pstate, rte, rtindex, 0, -1);
}
