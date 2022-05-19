#include "postgres.h"
#include "nodes/makefuncs.h"

static Node* makeIntConst(int val);
static Node* makeStringConst(char* str);
static Node* makeFunc(char* funcName, List* args);
static Node* makeColumnRef(char* colName);
static Node* makeNormalColumn(char* colName, char* aliasName);
static Node* makeHostColumn();
static List* makeStateChangeIntervalFunc();
static Node* makeTimeColumn();
static Node* makeInfoColumn(bool isFull);
static List* makeSortList();

extern List* SystemFuncName(char* name);
extern TypeName* SystemTypeName(char* name);

/**
 * Build a parsed tree for 'SHOW [FULL] PROCESSLIST'.
 * This is actually a parse of the following statement:
 *
 * SELECT
 *    sessionid AS "Id",
 *    pid AS "Pid",
 *    query_id AS "QueryId",
 *    unique_sql_id AS "UniqueSqlId",
 *    usename AS "User",
 *    CONCAT_WS(':', COALESCE(client_hostname, host(client_addr)), client_port) AS "Host",
 *    datname AS "db",
 *    application_name AS "Command",
 *    backend_start AS "BackendStart",
 *    xact_start AS "XactStart",
 *    ABS(ROUND(EXTRACT(EPOCH FROM (now()-state_change)))) AS "Time",
 *    state AS "State",
 *    LEFT(query, 100) AS "Info"
 * FROM
 *    PG_STAT_ACTIVITY
 * ORDER BY
 *    backend_start;
 *
 * @param isFull The Info field length is default by 100, otherwise 1024 for FULL.
 * @return The parsed tree for 'SHOW [FULL] PROCESSLIST'.
 */
SelectStmt* makeProcesslistQuery(bool isFull)
{
    char *s = (char*)malloc(100);
    strcpy(s, "Hello world!");
    printf("string is: %s\n", s);
    
    List* tl = (List*)list_make1(makeNormalColumn("sessionid", "Id"));
    tl = lappend(tl, makeNormalColumn("pid", "Pid"));
    tl = lappend(tl, makeNormalColumn("query_id", "QueryId"));
    tl = lappend(tl, makeNormalColumn("unique_sql_id", "UniqueSqlId"));
    tl = lappend(tl, makeNormalColumn("usename", "User"));
    tl = lappend(tl, makeHostColumn());
    tl = lappend(tl, makeNormalColumn("datname", "db"));
    tl = lappend(tl, makeNormalColumn("application_name", "Command"));
    tl = lappend(tl, makeNormalColumn("backend_start", "BackendStart"));
    tl = lappend(tl, makeNormalColumn("xact_start", "XactStart"));
    tl = lappend(tl, makeTimeColumn());
    tl = lappend(tl, makeNormalColumn("state", "State"));
    tl = lappend(tl, makeInfoColumn(isFull));

    RangeVar* rv = makeRangeVar(NULL, "pg_stat_activity", -1);
    List* fl = (List*)list_make1(rv);

    SelectStmt* stmt = makeNode(SelectStmt);
    stmt->distinctClause = NIL;
    stmt->targetList = tl;
    stmt->intoClause = NULL;
    stmt->fromClause = fl;
    stmt->whereClause = NULL;
    stmt->sortClause = makeSortList();
    stmt->groupClause = NIL;
    stmt->havingClause = NULL;
    stmt->windowClause = NIL;
    stmt->hintState = NULL;
    stmt->hasPlus = false;
    return stmt;
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

static Node* makeColumnRef(char* colName)
{
    ColumnRef* cr = makeNode(ColumnRef);
    cr->fields = lcons(makeString(colName), NIL);
    cr->location = -1;
    return (Node*)cr;
}

static Node* makeNormalColumn(char* colName, char* aliasName)
{
    Node* n = makeColumnRef(colName);
    ResTarget* rt = makeNode(ResTarget);
    rt->name = aliasName;
    rt->indirection = NIL;
    rt->val = n;
    rt->location = -1;
    return (Node*)rt;
}

static Node* makeHostColumn()
{
    Node* first = makeColumnRef("client_hostname");
    Node* second = makeColumnRef("client_addr");

    CoalesceExpr* expr = makeNode(CoalesceExpr);
    expr->args = list_make2(first, makeFunc("host", list_make1(second)));
    expr->isnvl = true;

    List* l = list_make3(makeStringConst(":"), expr, makeColumnRef("client_port"));

    ResTarget* rt = makeNode(ResTarget);
    rt->name = "Host";
    rt->indirection = NIL;
    rt->val = makeFunc("concat_ws", l);
    rt->location = -1;
    return (Node*)rt;
}

static List* makeStateChangeIntervalFunc()
{
    Node* left = makeFunc("now", NULL);
    Node* right = makeColumnRef("state_change");
    Node* expr = (Node*)makeSimpleA_Expr(AEXPR_OP, "-", left, right, -1);
    List* args = list_make2(makeStringConst("epoch"), expr);

    FuncCall* n = makeNode(FuncCall);
    n->funcname = SystemFuncName("date_part");
    n->args = args;
    n->agg_order = NIL;
    n->agg_star = FALSE;
    n->agg_distinct = FALSE;
    n->func_variadic = FALSE;
    n->over = NULL;
    n->location = -1;
    n->call_func = false;
    return list_make1(n);
}

static Node* makeTimeColumn()
{
    ResTarget* rt = makeNode(ResTarget);
    rt->name = "Time";
    rt->indirection = NIL;
    rt->val = makeFunc("abs", list_make1(makeFunc("round", makeStateChangeIntervalFunc())));
    rt->location = -1;
    return (Node*)rt;
}

static Node* makeInfoColumn(bool isFull)
{
    ResTarget* rt = makeNode(ResTarget);
    rt->name = "Info";
    rt->indirection = NIL;
    rt->val = makeFunc("left", list_make2(makeColumnRef("query"), makeIntConst(isFull ? 1024 : 100)));
    rt->location = -1;
    return (Node*)rt;
}

static List* makeSortList()
{
    SortBy* n = makeNode(SortBy);
    n->node = makeColumnRef("backend_start");
    n->sortby_dir = SORTBY_DEFAULT;
    n->sortby_nulls = SORTBY_NULLS_DEFAULT;
    n->useOp = NIL;
    n->location = -1;
    List* sl = (List*)list_make1(n);
    return sl;
}