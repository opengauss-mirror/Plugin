#include "postgres.h"
#include "nodes/makefuncs.h"

static Node* makeTypeColumn();
static List* makeFromList();
static Node* addCond(Node* left, Node* right);
static Node* makeWhereTarget(char* schemaName, char* tableName);
static Node* makeFunc(char* funcName, List* args);
static Node* makeCurrentSchemaFunc();
static Node* makeColumnRef(char* colName);
static Node* makeColumnRef(char* relName, char* colName);
static Node* makeNormalColumn(char* relName, char* colName, char* aliasName);
static Node* makeIntConst(int val);
static Node* makeStringConst(char* str);
static List* makeSortList();
static RangeVar* makeRangeAlias(char* varName, char* aliasName);
static Node* makeNullColumn();
static Node* makeCaseNode(char* term, char* result);
static Node* makeIndexSelect();
static Node* makeIndexColumn();
static Node* makeConstantSelect();
static Node* makeKeyColumn();
static Node* makeSubLink(Node* stmt);
static Node* makeDefaultSelect();
static Node* makeDefaultColumn();
static Node* makeExtraColumn();

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
 *            AND a.attnum = any(pg_index.indkey)),
 *        (SELECT
 *            'MUL'
 *        FROM
 *            pg_constraint
 *        WHERE
 *            pg_constraint.contype = 'f'
 *            AND pg_constraint.conrelid = c.oid
 *            AND a.attnum = any(pg_constraint.conkey))
 *    ) AS "Key",
 *    coalesce(
 *        (SELECT
 *            pg_attrdef.adsrc
 *        FROM
 *            pg_attrdef
 *        WHERE
 *            c.oid = pg_attrdef.adrelid
 *            AND a.attnum = pg_attrdef.adnum),
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
    List* tl = (List*)list_make1(makeNormalColumn("a", "attname", "Field"));
    tl = lappend(tl, makeTypeColumn());
    tl = lappend(tl, makeNullColumn());
    tl = lappend(tl, makeKeyColumn());
    tl = lappend(tl, makeDefaultColumn());
    tl = lappend(tl, makeExtraColumn());

    SelectStmt* stmt = makeNode(SelectStmt);
    stmt->distinctClause = NIL;
    stmt->targetList = tl;
    stmt->intoClause = NULL;
    stmt->fromClause = makeFromList();
    stmt->whereClause = makeWhereTarget(schemaName, tableName);
    stmt->sortClause = makeSortList();
    stmt->groupClause = NIL;
    stmt->havingClause = NULL;
    stmt->windowClause = NIL;
    stmt->hintState = NULL;
    stmt->hasPlus = false;
    return stmt;
}

static Node* makeTypeColumn()
{
    List* l = list_make2(makeColumnRef("a", "atttypid"), makeColumnRef("a", "atttypmod"));
    ResTarget* rt = makeNode(ResTarget);
    rt->name = "Type";
    rt->indirection = NIL;
    rt->val = makeFunc("format_type", l);
    rt->location = -1;
    return (Node*)rt;
}

static Node* makeNullColumn()
{
    List* wl = list_make1(makeCaseNode("t", "NO"));
    wl = lappend(wl, makeCaseNode("f", "YES"));

    CaseExpr* c = makeNode(CaseExpr);
    c->casetype = InvalidOid;
    c->arg = (Expr*)makeColumnRef("a", "attnotnull");
    c->args = wl;
    c->defresult = NULL;
    c->location = -1;

    ResTarget* rt = makeNode(ResTarget);
    rt->name = "Null";
    rt->indirection = NIL;
    rt->val = (Node*)c;
    rt->location = -1;
    return (Node*)rt;
}

static Node* makeCaseNode(char* term, char* result)
{
    CaseWhen* w = makeNode(CaseWhen);
    w->expr = (Expr*)makeStringConst(term);
    w->result = (Expr*)makeStringConst(result);
    w->location = -1;
    return (Node*)w;
}

static List* makeFromList()
{
    List* fl = (List*)list_make1(makeRangeAlias("pg_namespace", "n"));
    fl = lappend(fl, makeRangeAlias("pg_class", "c"));
    fl = lappend(fl, makeRangeAlias("pg_attribute", "a"));
    fl = lappend(fl, makeRangeAlias("pg_type", "t"));
    return fl;
}

static RangeVar* makeRangeAlias(char* varName, char* aliasName)
{
    RangeVar* rv = makeRangeVar(NULL, varName, -1);
    Alias* n = makeNode(Alias);
    n->aliasname = aliasName;
    rv->inhOpt = INH_DEFAULT;
    rv->alias = n;
    return rv;
}

static Node* addCond(Node* left, Node* right)
{
    return (Node*)makeA_Expr(AEXPR_AND, NIL, left, right, -1);
}

static Node* makeWhereTarget(char* schemaName, char* tableName)
{
    Node* cond1 = (Node*)makeSimpleA_Expr(AEXPR_OP, "=", makeColumnRef("n", "nspname"), 
                    schemaName ? makeStringConst(schemaName) : makeCurrentSchemaFunc(), -1);
    Node* cond2 = (Node*)makeSimpleA_Expr(AEXPR_OP, "=", makeColumnRef("c", "relname"), makeStringConst(tableName), -1);
    Node* cond3 = (Node*)makeSimpleA_Expr(AEXPR_OP, "=", makeColumnRef("n", "oid"), makeColumnRef("c", "relnamespace"), -1);
    Node* cond4 = (Node*)makeSimpleA_Expr(AEXPR_OP, "=", makeColumnRef("a", "attrelid"), makeColumnRef("c", "oid"), -1);
    Node* cond5 = (Node*)makeSimpleA_Expr(AEXPR_OP, "=", makeColumnRef("a", "atttypid"), makeColumnRef("t", "oid"), -1);
    Node* cond6 = (Node*)makeSimpleA_Expr(AEXPR_OP, ">", makeColumnRef("a", "attnum"), makeIntConst(0), -1);
    Node* wt = addCond(cond1, addCond(cond2, addCond(cond3, addCond(cond4, addCond(cond5, cond6)))));
    return wt;
}

static List* makeSortList()
{
    SortBy* n = makeNode(SortBy);
    n->node = makeColumnRef("a", "attnum");
    n->sortby_dir = SORTBY_DEFAULT;
    n->sortby_nulls = SORTBY_NULLS_DEFAULT;
    n->useOp = NIL;
    n->location = -1;
    List* sl = (List*)list_make1(n);
    return sl;
}

static Node* makeIntConst(int val)
{
    A_Const* n = makeNode(A_Const);
    n->val.type = T_Integer;
    n->val.val.ival = val;
    n->location = -1;
    return (Node*)n;
}

static Node* makeStringConst(char* str)
{
    A_Const* n = makeNode(A_Const);
    n->val.type = T_String;
    n->val.val.str = str;
    n->location = -1;
    return (Node*)n;
}

static Node* makeFunc(char* funcName, List* args)
{
    FuncCall* n = (FuncCall*)makeNode(FuncCall);
    n->funcname = list_make1(makeString(funcName));
    n->args = args;
    n->agg_star = FALSE;
    n->agg_distinct = FALSE;
    n->location = -1;
    n->call_func = false;
    return (Node*)n;
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

static Node* makeColumnRef(char* colName)
{
    ColumnRef* cr = makeNode(ColumnRef);
    cr->fields = lcons(makeString(colName), NIL);
    cr->location = -1;
    return (Node*)cr;
}

static Node* makeColumnRef(char* relName, char* colName)
{
    ColumnRef* c = makeNode(ColumnRef);
    c->fields = list_make2((Node*)makeString(relName), (Node*)makeString(colName));
    c->location = -1;
    return (Node*)c;
}

static Node* makeNormalColumn(char* relName, char* colName, char* aliasName)
{
    Node* n = makeColumnRef(relName, colName);
    ResTarget* rt = makeNode(ResTarget);
    rt->name = aliasName;
    rt->indirection = NIL;
    rt->val = n;
    rt->location = -1;
    return (Node*)rt;
}

static Node* makeExtraColumn()
{
    ResTarget* rt = makeNode(ResTarget);
    rt->name = "Extra";
    rt->indirection = NIL;
    rt->val = makeStringConst("");
    rt->location = -1;
    return (Node*)rt;
}

static Node* makeIndexSelect()
{
    SelectStmt* stmt = makeNode(SelectStmt);
    stmt->distinctClause = NIL;
    stmt->targetList = list_make1(makeIndexColumn());
    stmt->intoClause = NULL;
    stmt->fromClause = list_make1(makeRangeVar(NULL, "pg_index", -1));
    stmt->whereClause = (Node*)makeA_Expr(AEXPR_AND, NIL,
                                           (Node*)makeSimpleA_Expr(AEXPR_OP, "=",
                                                                    makeColumnRef("c", "oid"),
                                                                    makeColumnRef("pg_index", "indrelid"), -1),
                                           (Node*)makeSimpleA_Expr(AEXPR_OP_ANY, "=",
                                                                    makeColumnRef("a", "attnum"),
                                                                    makeColumnRef("pg_index", "indkey"),
                                                                    -1), -1);
    stmt->groupClause = NIL;
    stmt->havingClause = NULL;
    stmt->windowClause = NIL;
    stmt->hintState = NULL;
    stmt->hasPlus = false;
    return (Node*)stmt;
}

static Node* makeIndexColumn()
{
    CaseWhen* wf = makeNode(CaseWhen);
    wf->expr = (Expr*)makeSimpleA_Expr(AEXPR_OP, "=", makeColumnRef("indisprimary"), makeStringConst("t"), -1);
    wf->result = (Expr*)makeStringConst("PRI");
    wf->location = -1;

    CaseWhen* ws = makeNode(CaseWhen);
    ws->expr = (Expr*)makeSimpleA_Expr(AEXPR_OP, "=", makeColumnRef("indisunique"), makeStringConst("t"), -1);
    ws->result = (Expr*)makeStringConst("UNI");
    ws->location = -1;

    List* wl = list_make1((Node*)wf);
    wl = lappend(wl, (Node*)ws);

    CaseExpr* c = makeNode(CaseExpr);
    c->casetype = InvalidOid;
    c->arg = NULL;
    c->args = wl;
    c->defresult = (Expr*)makeStringConst("MUL");
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
    rt->val = makeStringConst("MUL");
    rt->location = -1;

    SelectStmt* stmt = makeNode(SelectStmt);
    stmt->distinctClause = NIL;
    stmt->targetList = list_make1((Node*)rt);
    stmt->intoClause = NULL;
    stmt->fromClause = list_make1(makeRangeVar(NULL, "pg_constraint", -1));
    stmt->whereClause = (Node*)makeA_Expr(AEXPR_AND, NIL,
                                           (Node*)makeSimpleA_Expr(AEXPR_OP, "=",
                                                                    makeColumnRef("pg_constraint", "contype"),
                                                                    makeStringConst("f"), -1),
                                           (Node*)makeA_Expr(AEXPR_AND, NIL,
                                                              (Node*)makeSimpleA_Expr(AEXPR_OP, "=",
                                                                                       makeColumnRef("pg_constraint", "conrelid"),
                                                                                       makeColumnRef("c", "oid"), -1),
                                                              (Node*)makeSimpleA_Expr(AEXPR_OP_ANY, "=",
                                                                                       makeColumnRef("a", "attnum"),
                                                                                       makeColumnRef("pg_constraint", "conkey"),
                                                                                       -1), -1), -1);
    stmt->groupClause = NIL;
    stmt->havingClause = NULL;
    stmt->windowClause = NIL;
    stmt->hintState = NULL;
    stmt->hasPlus = false;
    return (Node*)stmt;
}

static Node* makeCoalesce(Node* first, Node* second)
{
    CoalesceExpr* expr = makeNode(CoalesceExpr);
    expr->args = list_make2(first, second);
    expr->isnvl = true;
    return (Node*)expr;
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

static Node* makeKeyColumn()
{
    ResTarget* rt = makeNode(ResTarget);
    rt->name = "Key";
    rt->indirection = NIL;
    rt->val = makeCoalesce(makeSubLink(makeIndexSelect()), makeSubLink(makeConstantSelect()));
    rt->location = -1;
    return (Node*)rt;
}

static Node* makeDefaultSelect()
{
    ResTarget* rt = makeNode(ResTarget);
    rt->indirection = NIL;
    rt->val = makeColumnRef("pg_attrdef", "adsrc");
    rt->location = -1;

    SelectStmt* stmt = makeNode(SelectStmt);
    stmt->distinctClause = NIL;
    stmt->targetList = list_make1((Node*)rt);
    stmt->intoClause = NULL;
    stmt->fromClause = list_make1(makeRangeVar(NULL, "pg_attrdef", -1));
    stmt->whereClause = (Node*)makeA_Expr(AEXPR_AND, NIL,
                                           (Node*)makeSimpleA_Expr(AEXPR_OP, "=",
                                                                    makeColumnRef("c", "oid"),
                                                                    makeColumnRef("pg_attrdef", "adrelid"), -1),
                                           (Node*)makeSimpleA_Expr(AEXPR_OP, "=",
                                                                    makeColumnRef("a", "attnum"),
                                                                    makeColumnRef("pg_attrdef", "adnum"),
                                                                    -1), -1);
    stmt->groupClause = NIL;
    stmt->havingClause = NULL;
    stmt->windowClause = NIL;
    stmt->hintState = NULL;
    stmt->hasPlus = false;
    return (Node*)stmt;
}

static Node* makeDefaultColumn()
{
    ResTarget* rt = makeNode(ResTarget);
    rt->name = "Default";
    rt->indirection = NIL;
    rt->val = makeCoalesce(makeSubLink(makeDefaultSelect()), makeStringConst("NULL"));
    rt->location = -1;
    return (Node*)rt;
}
