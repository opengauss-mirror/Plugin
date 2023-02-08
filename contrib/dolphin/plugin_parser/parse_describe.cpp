#include "postgres.h"
#include "catalog/namespace.h"
#include "commands/dbcommands.h"
#include "nodes/makefuncs.h"
#include "utils/builtins.h"
#include "utils/lsyscache.h"
#include "plugin_parser/parse_show.h"

static Node* makeTypeColumn(bool smallcase = FALSE);
static List* makeFromList();
static Node* makeWhereTarget(char* schemaName, char* tableName);
static Node* makeCurrentSchemaFunc();

static Node* makeNullColumn(bool smallcase = FALSE);
static Node* makeCaseNode(char* term, char* result);
static Node* makeIndexSelect();
static Node* makeIndexColumn();
static Node* makeConstantSelect();
static Node* makeKeyColumn(bool smallcase = FALSE);
static Node* makeSubLink(Node* stmt);
static Node* makeDefaultSelect();
static Node* makeDefaultColumn(bool smallcase = FALSE);
static Node* makeExtraColumn(bool smallcase = FALSE);
static Node* makePrivilegeColumn(bool smallcase = FALSE);
static Node *makeCommentColumn(bool smallcase = FALSE);
static Node *makeCollationColumn(bool smallcase = FALSE);

extern List* SystemFuncName(char* name);
extern TypeName* SystemTypeName(char* name);

/**
 * Build a parsed tree for '{DESCRIBE | DESC} tbl_name'.
 * This is actually a parse of the following statement:
 *
 * SELECT
 *    a.attname AS "Field",
 *    format_type(a.atttypid, a.atttypmod) AS "Type",
 *    CASE a.attnotnull
 *        WHEN 't' THEN 'NO'
 *        WHEN 'f' THEN 'YES'
 *    END AS "Null",
 *    coalesce(
 *        (SELECT
 *            CASE
 *                WHEN indisprimary = 't' THEN 'PRI'
 *                WHEN indisunique = 't' THEN 'UNI'
 *                ELSE 'MUL'
 *            END
 *        FROM
 *            pg_index
 *        WHERE
 *            c.oid = pg_index.indrelid
 *            AND a.attnum = any(pg_index.indkey) 
 *            order by indisprimary = 't' desc, indisunique = 't' desc 
 *            LIMIT 1),
 *        (SELECT
 *            'MUL'
 *        FROM
 *            pg_constraint
 *        WHERE
 *            pg_constraint.contype = 'f'
 *            AND pg_constraint.conrelid = c.oid
 *            AND a.attnum = any(pg_constraint.conkey) LIMIT 1)
 *    ) AS "Key",
 *    coalesce(
 *        (SELECT
 *            pg_attrdef.adsrc
 *        FROM
 *            pg_attrdef
 *        WHERE
 *            c.oid = pg_attrdef.adrelid
 *            AND a.attnum = pg_attrdef.adnum LIMIT 1),
 *    'NULL') AS "Default",
 *    '' AS "Extra"
 * FROM pg_catalog.pg_namespace n,
 *    pg_catalog.pg_class c,
 *    pg_catalog.pg_attribute a,
 *    pg_catalog.pg_type t
 * WHERE n.nspname = 'schemaName'
 *    AND c.relname = 'tableName'
 *    AND n.oid = c.relnamespace
 *    AND a.attrelid = c.oid
 *    AND a.atttypid = t.oid
 *    AND a.attnum > 0
 * ORDER BY a.attnum;
 *
 * @param schemaName
 * @param tableName
 * @return The parsed tree for '{DESCRIBE | DESC} tbl_name'.
 */
SelectStmt* makeDescribeQuery(char* schemaName, char* tableName)
{
    List* tl = (List*)list_make1(plpsMakeNormalColumn("a", "attname", SHOW_FIELDS_COL));
    tl = lappend(tl, makeTypeColumn());
    tl = lappend(tl, makeNullColumn());
    tl = lappend(tl, makeKeyColumn());
    tl = lappend(tl, makeDefaultColumn());
    tl = lappend(tl, makeExtraColumn());
    List* fl = makeFromList();
    Node* wc = makeWhereTarget(schemaName, tableName);
    List* sl = plpsMakeSortList(plpsMakeColumnRef("a", "attnum"));

    SelectStmt* stmt = plpsMakeSelectStmt(tl, fl, wc, sl);
    return stmt;
}

static Node* makeTypeColumn(bool smallcase)
{
    List* l = list_make2(plpsMakeColumnRef("a", "atttypid"), plpsMakeColumnRef("a", "atttypmod"));
    ResTarget* rt = makeNode(ResTarget);
    if(!smallcase) {
    	rt->name = SHOW_TYPE_COL;
    } else {
    	rt->name = SHOW_TYPE_COL_S;
    }

    rt->indirection = NIL;
    rt->val = plpsMakeFunc("format_type", l);
    rt->location = -1;
    return (Node*)rt;
}

static Node* makeNullColumn(bool smallcase)
{
    List* wl = list_make1(makeCaseNode("t", "NO"));
    wl = lappend(wl, makeCaseNode("f", "YES"));

    CaseExpr* c = makeNode(CaseExpr);
    c->casetype = InvalidOid;
    c->arg = (Expr*)plpsMakeColumnRef("a", "attnotnull");
    c->args = wl;
    c->defresult = NULL;
    c->location = -1;

    ResTarget* rt = makeNode(ResTarget);
    if (!smallcase) {
    	rt->name = SHOW_NULL_COL;
    } else {
    	rt->name = SHOW_NULL_COL_S;
    }

    rt->indirection = NIL;
    rt->val = (Node*)c;
    rt->location = -1;
    return (Node*)rt;
}

static Node* makeCaseNode(char* term, char* result)
{
    CaseWhen* w = makeNode(CaseWhen);
    w->expr = (Expr*)plpsMakeStringConst(term);
    w->result = (Expr*)plpsMakeStringConst(result);
    w->location = -1;
    return (Node*)w;
}

static List* makeFromList()
{
    List* fl = (List*)list_make1(plpsMakeRangeAlias("pg_namespace", "n"));
    fl = lappend(fl, plpsMakeRangeAlias("pg_class", "c"));
    fl = lappend(fl, plpsMakeRangeAlias("pg_attribute", "a"));
    fl = lappend(fl, plpsMakeRangeAlias("pg_type", "t"));
    return fl;
}

Node* plpsAddCond(Node* left, Node* right, int location)
{
    return (Node*)makeA_Expr(AEXPR_AND, NIL, left, right, location);
}

static Node* makeWhereTarget(char* schemaName, char* tableName)
{
    Node* cond1 = (Node*)makeSimpleA_Expr(AEXPR_OP, "=", plpsMakeColumnRef("n", "nspname"), 
                    schemaName ? plpsMakeStringConst(schemaName) : makeCurrentSchemaFunc(), -1);
    Node* cond2 = (Node*)makeSimpleA_Expr(AEXPR_OP, "=", plpsMakeColumnRef("c", "relname"), plpsMakeStringConst(tableName), -1);
    Node* cond3 = (Node*)makeSimpleA_Expr(AEXPR_OP, "=", plpsMakeColumnRef("n", "oid"), plpsMakeColumnRef("c", "relnamespace"), -1);
    Node* cond4 = (Node*)makeSimpleA_Expr(AEXPR_OP, "=", plpsMakeColumnRef("a", "attrelid"), plpsMakeColumnRef("c", "oid"), -1);
    Node* cond5 = (Node*)makeSimpleA_Expr(AEXPR_OP, "=", plpsMakeColumnRef("a", "atttypid"), plpsMakeColumnRef("t", "oid"), -1);
    Node* cond6 = (Node*)makeSimpleA_Expr(AEXPR_OP, ">", plpsMakeColumnRef("a", "attnum"), plpsMakeIntConst(0), -1);
    Node* wt = plpsAddCond(cond1, plpsAddCond(cond2, plpsAddCond(cond3, plpsAddCond(cond4, plpsAddCond(cond5, cond6)))));
    return wt;
}

static Node* makeCurrentSchemaFunc()
{
    FuncCall* n = makeNode(FuncCall);
    n->funcname = SystemFuncName("current_schema");
    n->args = NIL;
    n->agg_order = NIL;
    n->agg_star = FALSE;
    n->agg_distinct = FALSE;
    n->func_variadic = FALSE;
    n->over = NULL;
    n->location = -1;
    n->call_func = false;
    return (Node*)n;
}

static Node* makeExtraColumn(bool smallcase)
{
    ResTarget* rt = makeNode(ResTarget);
    if (!smallcase) {
    	rt->name = SHOW_EXTRA_COL;
    } else {
    	rt->name = SHOW_EXTRA_COL_S;
    }

    rt->indirection = NIL;
    rt->val = plpsMakeStringConst("");
    rt->location = -1;
    return (Node*)rt;
}

static Node *makeIndexSelect()
{
    List *tl = list_make1(makeIndexColumn());
    List *fl = list_make1(makeRangeVar(NULL, "pg_index", -1));
    Node *wc = (Node *)makeA_Expr(AEXPR_AND, NIL,
                                  (Node *)makeSimpleA_Expr(AEXPR_OP, "=", plpsMakeColumnRef("c", "oid"),
                                                           plpsMakeColumnRef("pg_index", "indrelid"), -1),
                                  (Node *)makeSimpleA_Expr(AEXPR_OP_ANY, "=", plpsMakeColumnRef("a", "attnum"),
                                                           plpsMakeColumnRef("pg_index", "indkey"), -1),
                                  -1);
    Node *lc = plpsMakeIntConst(1);
    Node *sn1 =
        plpsMakeSortByNode((Node *)makeSimpleA_Expr(AEXPR_OP, "=", plpsMakeColumnRef("pg_index", "indisprimary"),
                                                    plpsMakeStringConst("t"), -1),
                           SORTBY_DESC);
    Node *sn2 = plpsMakeSortByNode((Node *)makeSimpleA_Expr(AEXPR_OP, "=", plpsMakeColumnRef("pg_index", "indisunique"),
                                                            plpsMakeStringConst("t"), -1),
                                   SORTBY_DESC);
    List *sl = (List *)list_make2(sn1, sn2);

    SelectStmt *stmt = plpsMakeSelectStmt(tl, fl, wc, sl, lc);
    return (Node *)stmt;
}

static Node* makeIndexColumn()
{
    CaseWhen* wf = makeNode(CaseWhen);
    wf->expr = (Expr*)makeSimpleA_Expr(AEXPR_OP, "=", plpsMakeColumnRef(NULL, "indisprimary"), plpsMakeStringConst("t"), -1);
    wf->result = (Expr*)plpsMakeStringConst("PRI");
    wf->location = -1;

    CaseWhen* ws = makeNode(CaseWhen);
    ws->expr = (Expr*)makeSimpleA_Expr(AEXPR_OP, "=", plpsMakeColumnRef(NULL, "indisunique"), plpsMakeStringConst("t"), -1);
    ws->result = (Expr*)plpsMakeStringConst("UNI");
    ws->location = -1;

    List* wl = list_make1((Node*)wf);
    wl = lappend(wl, (Node*)ws);

    CaseExpr* c = makeNode(CaseExpr);
    c->casetype = InvalidOid;
    c->arg = NULL;
    c->args = wl;
    c->defresult = (Expr*)plpsMakeStringConst("MUL");
    c->location = -1;

    ResTarget* rt = makeNode(ResTarget);
    rt->indirection = NIL;
    rt->val = (Node*)c;
    rt->location = -1;

    return (Node*)rt;
}

static Node* makeConstantSelect()
{
    ResTarget* rt = makeNode(ResTarget);
    rt->indirection = NIL;
    rt->val = plpsMakeStringConst("MUL");
    rt->location = -1;

    List* tl = list_make1((Node*)rt);
    List* fl = list_make1(makeRangeVar(NULL, "pg_constraint", -1));
    Node* wc = (Node*)makeA_Expr(AEXPR_AND, NIL,
                                 (Node*)makeSimpleA_Expr(AEXPR_OP, "=",
                                                         plpsMakeColumnRef("pg_constraint", "contype"),
                                                         plpsMakeStringConst("f"), -1),
                                 (Node*)makeA_Expr(AEXPR_AND, NIL,
                                                   (Node*)makeSimpleA_Expr(AEXPR_OP, "=",
                                                                           plpsMakeColumnRef("pg_constraint", "conrelid"),
                                                                           plpsMakeColumnRef("c", "oid"), -1),
                                                   (Node*)makeSimpleA_Expr(AEXPR_OP_ANY, "=",
                                                                           plpsMakeColumnRef("a", "attnum"),
                                                                           plpsMakeColumnRef("pg_constraint", "conkey"),
                                                                           -1), -1), -1);
    Node* lc = plpsMakeIntConst(1);

    SelectStmt* stmt = plpsMakeSelectStmt(tl, fl, wc, NULL, lc);
    return (Node*)stmt;
}

static Node* makeCoalesce(Node* first, Node* second)
{
    CoalesceExpr* expr = makeNode(CoalesceExpr);
    expr->args = list_make2(first, second);
    expr->isnvl = true;
    return (Node*)expr;
}

Node *plpsMakeCoalesce(Node* first, Node* second)
{
    return makeCoalesce(first, second);
}

static Node* makeSubLink(Node* stmt)
{
    SubLink* nf = makeNode(SubLink);
    nf->subLinkType = EXPR_SUBLINK;
    nf->testexpr = NULL;
    nf->operName = NIL;
    nf->subselect = stmt;
    nf->location = -1;
    return (Node*)nf;
}

Node* plpsMakeSubLink(Node* stmt)
{
    return makeSubLink(stmt);
}

static Node* makeKeyColumn(bool smallcase)
{
    ResTarget* rt = makeNode(ResTarget);
    if (!smallcase) {
    	rt->name = SHOW_KEY_COL;
    } else {
    	rt->name = SHOW_KEY_COL_S;
    }

    rt->indirection = NIL;
    rt->val = makeCoalesce(makeSubLink(makeIndexSelect()), makeSubLink(makeConstantSelect()));
    rt->location = -1;
    return (Node*)rt;
}

static Node* makeDefaultSelect()
{
    ResTarget* rt = makeNode(ResTarget);
    rt->indirection = NIL;
    rt->val = plpsMakeColumnRef("pg_attrdef", "adsrc");
    rt->location = -1;

    List* tl = list_make1((Node*)rt);
    List* fl = list_make1(makeRangeVar(NULL, "pg_attrdef", -1));
    Node* wc = (Node*)makeA_Expr(AEXPR_AND, NIL,
                                 (Node*)makeSimpleA_Expr(AEXPR_OP, "=",
                                                         plpsMakeColumnRef("c", "oid"),
                                                         plpsMakeColumnRef("pg_attrdef", "adrelid"), -1),
                                 (Node*)makeSimpleA_Expr(AEXPR_OP, "=",
                                                         plpsMakeColumnRef("a", "attnum"),
                                                         plpsMakeColumnRef("pg_attrdef", "adnum"),
                                                         -1), -1);
    Node* lc = plpsMakeIntConst(1);

    SelectStmt* stmt = plpsMakeSelectStmt(tl, fl, wc, NULL, lc);
    return (Node*)stmt;
}

static Node* makeDefaultColumn(bool smallcase)
{
    ResTarget* rt = makeNode(ResTarget);
    if (!smallcase) {
    	rt->name = SHOW_DEFAULT_COL;
    } else {
    	rt->name = SHOW_DEFAULT_COL_S;
    }
    
    rt->indirection = NIL;
    rt->val = makeCoalesce(makeSubLink(makeDefaultSelect()), plpsMakeStringConst("NULL"));
    rt->location = -1;
    return (Node*)rt;
}

// add the show full column from table: the 'Privileges' column result
static Node* makePrivilegeColumn(bool smallcase)
{
    ResTarget *rt = NULL;
    Node *cond = NULL;
    Node *cond1 = NULL;
    Node *cond2 = NULL;

    if (superuser()) {
        rt = makeNode(ResTarget);
        if (smallcase) {
            rt->name = SHOW_PRIVILEGES_COL_S;
        } else {
            rt->name = SHOW_PRIVILEGES_COL;
        }
        rt->indirection = NIL;
        rt->val = plpsMakeStringConst("UPDATE,SELECT,REFERENCES,INSERT,COMMENT");
        rt->location = PLPS_LOC_UNKNOWN;
        return (Node*)rt;
    }

    rt = makeNode(ResTarget);
    rt->name = NULL;
    rt->indirection = NIL;
    rt->val = plpsMakeFunc("string_agg", list_make2(plpsMakeColumnRef(NULL, "privilege_type"), plpsMakeStringConst(",")));
    rt->location = PLPS_LOC_UNKNOWN;

    List* tl = list_make1(rt);
    List* fl = list_make1(makeRangeVar("information_schema", "column_privileges", -1));
    cond1 = (Node*)makeSimpleA_Expr(AEXPR_OP, "=",
                                    plpsMakeColumnRef(PG_CLASS_ALIAS, "relname"),
                                    plpsMakeColumnRef("column_privileges", "table_name"), -1);
    cond2 = (Node*)makeSimpleA_Expr(AEXPR_OP, "=",
                                    plpsMakeColumnRef(PG_ATTRIBUTE_ALIAS, "attname"),
                                    plpsMakeColumnRef("column_privileges", "column_name"), -1);
    cond = plpsAddCond(cond1, cond2);
    cond1 = (Node*)makeSimpleA_Expr(AEXPR_OP, "=",
                                    plpsMakeColumnRef("column_privileges", "grantee"),
                                    plpsMakeStringConst(GetUserNameFromId(GetUserId())), -1);
    cond2 = (Node*)makeSimpleA_Expr(AEXPR_OP, "=",
                                    plpsMakeColumnRef("column_privileges", "table_schema"),
                                    plpsMakeColumnRef("n", "nspname"), -1);
    cond = plpsAddCond(cond, plpsAddCond(cond1, cond2));
    SelectStmt* stmt = plpsMakeSelectStmt(tl, fl, cond, NULL);

    rt = makeNode(ResTarget);
    if (smallcase) {
        rt->name = SHOW_PRIVILEGES_COL_S;
    } else {
        rt->name = SHOW_PRIVILEGES_COL;
    }

    rt->indirection = NIL;
    rt->val = makeCoalesce(makeSubLink((Node*)stmt), plpsMakeStringConst("NULL"));;
    rt->location = PLPS_LOC_UNKNOWN; // just set unknown token pos now

    return (Node*)rt;
}

static Node *makeCommentColumn(bool smallcase)
{
    ResTarget *rt = NULL;
    
    FuncCall *func = makeNode(FuncCall);
    func->funcname = list_make1(makeString("col_description"));
    func->args = list_make2(plpsMakeColumnRef(PG_CLASS_ALIAS, "oid"), plpsMakeColumnRef(PG_ATTRIBUTE_ALIAS, "attnum"));
    func->agg_order = NIL;
    func->agg_star = FALSE;
    func->agg_distinct = FALSE;
    func->func_variadic = FALSE;
    func->location = PLPS_LOC_UNKNOWN;
    func->over = NULL;
    func->call_func = FALSE;
    
    rt = makeNode(ResTarget);
    if (smallcase) {
    	rt->name = SHOW_COMMENT_COL_S;
    } else {
    	rt->name = SHOW_COMMENT_COL;
    }

    rt->indirection = NIL;
    rt->val = (Node*)func;
    rt->location = -1;
    
    return (Node*)rt;
}

static Node *makeCollationColumn(bool smallcase)
{
    ResTarget *rt = makeNode(ResTarget);
    rt->val = (Node*)plpsMakeColumnRef("columns", "collation_name");
    rt->indirection = NIL;
    rt->location = PLPS_LOC_UNKNOWN;

    List* tl = list_make1(rt);
    List* fl = list_make1(makeRangeVar("information_schema", "columns", -1));
    Node* wc = (Node*)makeA_Expr(AEXPR_AND, NIL,
                                 (Node*)makeA_Expr(AEXPR_AND, NIL,
                                                   (Node*)makeSimpleA_Expr(AEXPR_OP, "=",
                                                                           plpsMakeColumnRef(PG_CLASS_ALIAS, "relname"),
                                                                           plpsMakeColumnRef("columns", "table_name"), -1),
                                                   (Node*)makeSimpleA_Expr(AEXPR_OP, "=",
                                                                           plpsMakeColumnRef(PG_ATTRIBUTE_ALIAS, "attname"),
                                                                           plpsMakeColumnRef("columns", "column_name"), -1), -1),
                                 (Node*)makeSimpleA_Expr(AEXPR_OP, ">",
                                                         plpsMakeFunc("length",
                                                                      list_make1(plpsMakeColumnRef(NULL, "collation_name"))),
                                                         plpsMakeIntConst(0), -1), -1);

    SelectStmt* stmt = plpsMakeSelectStmt(tl, fl, wc, NULL);
    
    rt = makeNode(ResTarget);
    if (smallcase) {
    	rt->name = SHOW_COLLATION_COL_S;
    } else {
    	rt->name = SHOW_COLLATION_COL;
    }

    rt->indirection = NIL;
    rt->val = makeCoalesce(makeSubLink((Node*)stmt), plpsMakeStringConst("NULL")); 
    rt->location = -1;
    
    return (Node*)rt;
}

SelectStmt *makeShowColumnsDirectQuery(char *schemaname, char *tablename, bool fullmode, bool smallcase,
                                       bool isLikeExpr, Node *likeWhereOpt = NULL)
{
    Node* privfilter = (Node*)makeSimpleA_Expr(AEXPR_OP, "~~", plpsMakeColumnRef(NULL, SHOW_PRIVILEGES_COL_S),
					       plpsMakeStringConst("%SELECT%"), -1);

    if (isLikeExpr && likeWhereOpt != NULL) {
    	likeWhereOpt = (Node *)makeSimpleA_Expr(AEXPR_OP, "~~",
    				                plpsMakeColumnRef(NULL, SHOW_FIELDS_COL_S), likeWhereOpt, -1);
    }

    List* tl = NULL;
    if (smallcase) {
    	tl = (List*)list_make1(plpsMakeNormalColumn(PG_ATTRIBUTE_ALIAS, "attname", SHOW_FIELDS_COL_S));
    } else {
    	tl = (List*)list_make1(plpsMakeNormalColumn(PG_ATTRIBUTE_ALIAS, "attname", SHOW_FIELDS_COL));
    }	
    
    tl = lappend(tl, makeTypeColumn(smallcase));
    if (fullmode) {
	tl = lappend(tl, makeCollationColumn(smallcase));
    }
	
    tl = lappend(tl, makeNullColumn(smallcase));
    tl = lappend(tl, makeKeyColumn(smallcase));
    tl = lappend(tl, makeDefaultColumn(smallcase));
    tl = lappend(tl, makeExtraColumn(smallcase));
    tl = lappend(tl, makePrivilegeColumn(smallcase));
    if (fullmode) {
    	tl = lappend(tl, makeCommentColumn(smallcase));
    }

    List* fl = makeFromList();
    Node* wc = makeWhereTarget(schemaname, tablename);
    wc = plpsAddCond(wc, privfilter);
    if (likeWhereOpt != NULL) {
        wc = plpsAddCond(wc, likeWhereOpt);
    }
    List* sl = plpsMakeSortList(plpsMakeColumnRef("a", "attnum"));

    SelectStmt* stmt = plpsMakeSelectStmt(tl, fl, wc, sl);
    return stmt;
}

/**
 * Build a parsed tree for 'SHOW [FULL] {COLUMNS | FIELDS} {FROM | IN} tbl_name [{FROM | IN} db_name] [LIKE 'pattern' | WHERE expr]'
 * This is actually a parse of the following statement:
 *
 * SELECT "field" AS "Field", "type" as "Type", "collation" AS "Collation", "null" AS "Null", "default" AS "Default",
 *        "extra" AS "Extra", "privileges" AS "Privileges", "comment" AS "Comment"
 * FROM
 * (
 *   SELECT
 *     a.attname AS "field",
 *     format_type(a.atttypid, a.atttypmod) AS "type",
 *     coalesce(
 *       (SELECT
 *          collation_name
 *        FROM
 *          information_schema.columns
 *        WHERE
 *          table_name = c.relname
 *          AND column_name = a.attname
 *          AND length(collation_name) > 0 ),
 *     'NULL' ) AS "collation",
 *     CASE a.attnotnull
 *       WHEN 't' THEN 'NO'
 *       WHEN 'f' THEN 'YES'
 *     END AS "null",
 *     coalesce(
 *       (SELECT
 *          CASE
 *            WHEN indisprimary = 't' THEN 'PRI'
 *            WHEN indisunique = 't' THEN 'UNI'
 *            ELSE 'MUL'
 *          END
 *        FROM
 *          pg_index
 *        WHERE
 *          c.oid = pg_index.indrelid
 *          AND a.attnum = any(pg_index.indkey) LIMIT 1 ),
 *       (SELECT
 *          'MUL'
 *        FROM
 *          pg_constraint
 *        WHERE
 *          pg_constraint.contype = 'f'
 *          AND pg_constraint.conrelid = c.oid
 *          AND a.attnum = any(pg_constraint.conkey)
 *       )
 *     ) AS "key",
 *     coalesce(
 *       (SELECT
 *          pg_attrdef.adsrc
 *        FROM
 *          pg_attrdef
 *        WHERE
 *          c.oid = pg_attrdef.adrelid
 *          AND a.attnum = pg_attrdef.adnum LIMIT 1),
 *     'NULL' ) AS "default",
 *     '' AS "extra",
 *     coalesce(
 *       (SELECT
 * 	    string_agg(privilege_type, ',')
 *        FROM
 *          information_schema.column_privileges
 *        WHERE
 *          column_privileges.table_name = c.relname
 *          AND column_privileges.column_name = a.attname
 *          AND column_privileges.grantee = (select current_user)
 *          AND column_privileges.is_grantable = 'YES' ),
 *     'NULL' ) AS "privileges",
 *     pg_catalog.col_description(c.oid, a.attnum) AS "comment"
 *   FROM pg_catalog.pg_namespace n,
 *     pg_catalog.pg_class c,
 *     pg_catalog.pg_attribute a,
 *     pg_catalog.pg_type t
 *   WHERE n.nspname = 'tst_schema'
 *     AND c.relname = 'txx'
 *     AND n.oid = c.relnamespace
 *     AND a.attrelid = c.oid
 *     AND a.atttypid = t.oid
 *     AND a.attnum > 0
 *     AND Privileges like '%SELECT%'
 *     AND [expr] // other option convery from like or where clause, "like" => where Columns = '%pattern%'
 *   ORDER BY a.attnum
 * )
 *
 * @param schemaName
 * @param tableName
 * @param optDbName
 * @param fullmode
 * @param isLikeExpr
 * @param likeWhereOpt
 * @return The parsed tree for 
 *         'SHOW [FULL] {COLUMNS | FIELDS} {FROM | IN} tbl_name [{FROM | IN} db_name] [LIKE 'pattern' | WHERE expr]'.
 */
SelectStmt* makeShowColumnsQuery(char *schemaname, char *tablename, char *optDbName,
	                         bool fullmode, bool isLikeExpr, Node* likeWhereOpt)
{
    // optDbName adapted to used as schemaname
    if (optDbName != NULL) {
        schemaname = optDbName; // prior to use optDbName as schemaname
    }

    if (schemaname == NULL) {
        schemaname = DatumGetCString(DirectFunctionCall1(current_schema, PointerGetDatum(NULL)));
    }

    (void)plps_check_schema_or_table_valid(schemaname, tablename, FALSE); // check error exit

    RangeSubselect *rsubselect = makeNode(RangeSubselect);
    Alias *alias = makeNode(Alias);
    /* sub select choose use smallcase column name */
    bool smallcase_beneath = TRUE; 
    
    alias->aliasname = pstrdup("__unnamed_subquery__");
    rsubselect->subquery = (Node*)makeShowColumnsDirectQuery(schemaname, tablename, fullmode,
							     smallcase_beneath, isLikeExpr, likeWhereOpt);
    rsubselect->alias = alias;

    // aliasname for representating to users  
    List *tl = list_make1(plpsMakeNormalColumn(NULL, SHOW_FIELDS_COL_S, SHOW_FIELDS_COL));
    tl = lappend(tl, plpsMakeNormalColumn(NULL, SHOW_TYPE_COL_S, SHOW_TYPE_COL));
    if (fullmode) {
    	tl = lappend(tl, plpsMakeNormalColumn(NULL, SHOW_COLLATION_COL_S, SHOW_COLLATION_COL));
    }
    tl = lappend(tl, plpsMakeNormalColumn(NULL, SHOW_NULL_COL_S, SHOW_NULL_COL));
    tl = lappend(tl, plpsMakeNormalColumn(NULL, SHOW_KEY_COL_S, SHOW_KEY_COL));
    tl = lappend(tl, plpsMakeNormalColumn(NULL, SHOW_DEFAULT_COL_S, SHOW_DEFAULT_COL));
    tl = lappend(tl, plpsMakeNormalColumn(NULL, SHOW_EXTRA_COL_S, SHOW_EXTRA_COL));
    if (fullmode) {
    	tl = lappend(tl, plpsMakeNormalColumn(NULL, SHOW_PRIVILEGES_COL_S, SHOW_PRIVILEGES_COL));
    	tl = lappend(tl, plpsMakeNormalColumn(NULL, SHOW_COMMENT_COL_S, SHOW_COMMENT_COL));
    }
    List* fl = list_make1(rsubselect);

    SelectStmt* stmt = plpsMakeSelectStmt(tl, fl, NULL, NULL);
    return stmt;
}

SelectStmt *checkTableExistence(RangeVar *classrel)
{
    SelectStmt *n = NULL;
    char *nspname = NULL;
    recomputeNamespacePath();
    nspname = (classrel->schemaname != NULL) ? classrel->schemaname : get_namespace_name(getCurrentNamespace());
 
    (void)plps_check_schema_or_table_valid(nspname, classrel->relname, false, true);

    n = makeDescribeQuery(classrel->schemaname, classrel->relname);
    return n;
}
