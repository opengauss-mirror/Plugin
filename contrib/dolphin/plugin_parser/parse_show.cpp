#include "postgres.h"
#include "commands/dbcommands.h"
#include "nodes/makefuncs.h"
#include "catalog/namespace.h"
#include "utils/lsyscache.h"
#include "utils/builtins.h"
#include "plugin_parser/parse_show.h"
#include "utils/guc_tables.h"
#include "plugin_postgres.h"
#include "plugin_parser/parser.h"

static Node* makeColumnRef(char* colName, int loc = PLPS_LOC_UNKNOWN);
static Node* makeHostColumn();
static List* makeStateChangeIntervalFunc();
static Node* makeTimeColumn();
static Node* makeInfoColumn(bool isFull);
static Node* makeRangeSubselect(SelectStmt* stmt);
static Node* makeMsRangeFunction();
static Node* makeStarColumn();

static Node* makePluginsStatusColumn(bool smallcase = FALSE);
static Node* makePluginsTypeColumn(bool smallcase = FALSE);
static Node* makePluginsLibraryColumn(bool smallcase = FALSE);
static Node* makePluginsLicenseColumn(bool smallcase = FALSE);

static Node* makeEngineColumn();
static Node* makeRowFormatColumn();
static Node* makeAutoIncrementColumn();
static List* makeShowTableStatusColumns();
static Node* makeShowTableStatusJoinTable();

extern List* SystemFuncName(char* name);
extern TypeName* SystemTypeName(char* name);
extern Tuplestorestate* BuildTupleResult(FunctionCallInfo fcinfo, TupleDesc* tupdesc);

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
SelectStmt* makeShowProcesslistQuery(bool isFull)
{
    List* tl = (List*)list_make1(plpsMakeNormalColumn(NULL, "sessionid", "Id"));
    tl = lappend(tl, plpsMakeNormalColumn(NULL, "pid", "Pid"));
    tl = lappend(tl, plpsMakeNormalColumn(NULL, "query_id", "QueryId"));
    tl = lappend(tl, plpsMakeNormalColumn(NULL, "unique_sql_id", "UniqueSqlId"));
    tl = lappend(tl, plpsMakeNormalColumn(NULL, "usename", "User"));
    tl = lappend(tl, makeHostColumn());
    tl = lappend(tl, plpsMakeNormalColumn(NULL, "datname", "db"));
    tl = lappend(tl, plpsMakeNormalColumn(NULL, "application_name", "Command"));
    tl = lappend(tl, plpsMakeNormalColumn(NULL, "backend_start", "BackendStart"));
    tl = lappend(tl, plpsMakeNormalColumn(NULL, "xact_start", "XactStart"));
    tl = lappend(tl, makeTimeColumn());
    tl = lappend(tl, plpsMakeNormalColumn(NULL, "state", "State"));
    tl = lappend(tl, makeInfoColumn(isFull));

    List* fl = list_make1(makeRangeVar(NULL, "pg_stat_activity", -1));
    List* sl = plpsMakeSortList(plpsMakeColumnRef(NULL, "backend_start"));

    SelectStmt* stmt = plpsMakeSelectStmt(tl, fl, NULL, sl);
    return stmt;
}

/**
 * Build a parsed tree for 'SHOW {DATABASES | SCHEMAS} [LIKE 'pattern' | WHERE expr]'.
 * This is actually a parse of the following statement:
 *
 * SELECT 
 *    n.database AS "Database"
 * FROM 
 *    (
 *    SELECT 
 *       nspname AS "database",
 *       (ACLEXPLODE(COALESCE(nspacl, ACLDEFAULT('n', nspowner)))).grantee AS grantee,
 *       (ACLEXPLODE(COALESCE(nspacl, ACLDEFAULT('n', nspowner)))).privilege_type AS priv
 *    FROM 
 *       PG_NAMESPACE
 *    ) n,
 * PG_ROLES r
 * WHERE 
 *    (n.grantee = r.oid OR n.grantee = 0) AND 
 *    n.priv = 'USAGE' AND
 *    r.rolname = user
 * GROUP BY 
 *    n.database
 * ORDER BY
 *    n.database = 'information_schema' DESC,
 *    1;
 *
 * @param likeNode
 * @param whereExpr
 * @return The parsed tree for 'SHOW {DATABASES | SCHEMAS} [LIKE 'pattern' | WHERE expr]'.
 */
SelectStmt* makeShowDatabasesQuery(Node* likeNode, Node* whereExpr)
{
    Node* func = plpsMakeFunc("acldefault", list_make2(plpsMakeStringConst("n"), plpsMakeColumnRef(NULL, "nspowner")));
    Node* expr = plpsMakeCoalesce(plpsMakeColumnRef(NULL, "nspacl"), func);
    Node* rt1 = plpsMakeTargetFuncDirecAlias("aclexplode", list_make1((Node*)expr), list_make1(makeString("grantee")), "grantee");
    Node* rt2 = plpsMakeTargetFuncDirecAlias("aclexplode", list_make1((Node*)expr), list_make1(makeString("privilege_type")), "priv");
    List* tl = list_make3(plpsMakeNormalColumn(NULL, "nspname", "database"), (Node*)rt1, (Node*)rt2);
    List* fl = list_make1(makeRangeVar(NULL, "pg_namespace", -1));

    RangeSubselect* rs = makeNode(RangeSubselect);
    rs->subquery = (Node*)plpsMakeSelectStmt(tl, fl, NULL, NULL);
    rs->alias = makeAlias(pstrdup("n"), NIL);

    List* tl2 = list_make1(plpsMakeNormalColumn("n", "database", "Database"));
    List* fl2 = list_make1((Node*)rs);

    Node* wc = NULL;
    if (!superuser()) {
        fl2 = lappend(fl2, plpsMakeRangeAlias("pg_roles", "r"));
        Node* cond1 = (Node*)makeSimpleA_Expr(AEXPR_OP, "=", plpsMakeColumnRef("n", "grantee"), plpsMakeColumnRef("r", "oid"), -1);
        Node* cond2 = (Node*)makeSimpleA_Expr(AEXPR_OP, "=", plpsMakeColumnRef("n", "grantee"), plpsMakeIntConst(0), -1);
        Node* cond3 = (Node*)makeSimpleA_Expr(AEXPR_OP, "=", plpsMakeColumnRef("n", "priv"), plpsMakeStringConst("USAGE"), -1);
        Node* cond4 = (Node*)makeSimpleA_Expr(AEXPR_OP, "=", plpsMakeColumnRef("r", "rolname"), plpsMakeFunc("current_user", NULL), -1);
        wc = (Node*)makeA_Expr(AEXPR_AND, NIL, 
        (Node*)makeA_Expr(AEXPR_AND, NIL, 
        (Node*)makeA_Expr(AEXPR_OR, NIL, cond1, cond2, -1),  
        cond3, -1), cond4, -1);
    }

    if (likeNode) {
        Node* le = (Node*)makeSimpleA_Expr(AEXPR_OP, "~~", plpsMakeColumnRef(NULL, "database"), likeNode, -1);
        if (wc) {
            wc = (Node*)makeA_Expr(AEXPR_AND, NIL, wc, le, -1);
        } else {
            wc = le;
        }
    } else if (whereExpr) {
        if (wc) {
            wc = (Node*)makeA_Expr(AEXPR_AND, NIL, wc, whereExpr, -1);
        } else {
            wc = whereExpr;
        }
    }

    List* gl = list_make1(plpsMakeColumnRef("n", "database"));
    Node* sn1 = plpsMakeSortByNode(
        (Node*)makeSimpleA_Expr(AEXPR_OP, "=", 
        plpsMakeColumnRef("n", "database"), 
        plpsMakeStringConst("information_schema"), -1), 
        SORTBY_DESC);
    Node* sn2 = plpsMakeSortByNode(plpsMakeIntConst(1));
    List* sl2 = (List*)list_make2(sn1, sn2);

    SelectStmt* stmt = plpsMakeSelectStmt(tl2, fl2, wc, sl2);
    stmt->groupClause = gl;
    return stmt;
}

/**
 * Build a parsed tree for 'SHOW MASTER STATUS'.
 * This is actually a parse of the following statement:
 *
 * SELECT
 *    *
 * FROM
 *    gs_master_status();
 *
 * @param void
 * @return The parsed tree for 'SHOW MASTER STATUS'.
 */
SelectStmt* makeShowMasterStatusQuery(void)
{
    List* tl = (List*)list_make1(makeStarColumn());
    List* fl = list_make1(makeMsRangeFunction());

    SelectStmt* stmt = plpsMakeSelectStmt(tl,  fl, NULL, NULL);
    return stmt;
}

/**
 * Build a parsed tree for 'SHOW SLAVE HOSTS'.
 * This is actually a parse of the following statement:
 *
 * SELECT
 *    *
 * FROM
 *    pg_stat_replication;
 *
 * @param void
 * @return The parsed tree for 'SHOW SLAVE HOSTS'.
 */
SelectStmt* makeShowSlaveHostsQuery(void)
{
    List* tl = (List*)list_make1(makeStarColumn());
    List* fl = list_make1(makeRangeVar(NULL, "pg_stat_replication", -1));

    SelectStmt* stmt = plpsMakeSelectStmt(tl, fl, NULL, NULL);
    return stmt;
}

SelectStmt* plpsMakeSelectStmt(List* targetList, List* fromList, Node* whereClause, List* sortClause, Node* limitCount)
{
    SelectStmt* n = makeNode(SelectStmt);
    n->distinctClause = NIL;
    n->targetList = targetList;
    n->intoClause = NULL;
    n->fromClause = fromList;
    n->whereClause = whereClause;
    n->sortClause = sortClause;
    n->groupClause = NIL;
    n->havingClause = NULL;
    n->windowClause = NIL;
    n->limitCount = limitCount;
    return n;
}

Node* plpsMakeFunc(char* funcname, List* args, int location)
{
    return (Node*)makeFuncCall(list_make1(makeString(funcname)), args, location);
}

Node* plpsMakeColumnRef(char* relname, char *colname, int location)
{
    if (relname == NULL) {
        return makeColumnRef(colname, location);
    }

    return (Node*)makeColumnRef(relname, colname, location);
}

static Node* makeTypeCast(Node *arg, TypeName *typname, int location)
{
    TypeCast *n = makeNode(TypeCast);
    n->arg = arg;
    n->typname = typname;
    n->location = location;
    return (Node *) n;
}

Node* plpsMakeTypeCast(Node *castExpr, char *type_name, int location)
{
    TypeName *typname = NULL;
    Node *typcast = NULL;

    typname = makeTypeName(type_name);
    typname->typmods = NIL; /* not used here */
    typcast = makeTypeCast(castExpr, typname, location);

    return typcast;
}

Node* plpsMakeNormalColumn(char* relname, char* colname, char* aliasname, int location)
{
    Node* n = plpsMakeColumnRef(relname, colname);
    ResTarget* rt = makeNode(ResTarget);
    rt->name = aliasname;
    rt->indirection = NIL;
    rt->val = n;
    rt->location = location;
    return (Node*)rt;
}

Node* plpsMakeSortByNode(Node* sortExpr, SortByDir sortBd)
{
    SortBy* n = makeNode(SortBy);
    n->node = sortExpr;
    n->sortby_dir = sortBd;
    n->sortby_nulls = SORTBY_NULLS_DEFAULT;
    n->useOp = NIL;
    n->location = -1;
    return (Node*)n;
}

List* plpsMakeSortList(Node* sortExpr)
{
    return (List*)list_make1(plpsMakeSortByNode(sortExpr));
}

Node* plpsMakeIntConst(int val)
{
    A_Const* n = makeNode(A_Const);
    n->val.type = T_Integer;
    n->val.val.ival = val;
    n->location = -1;
    return (Node*)n;
}

Node* plpsMakeStringConst(char* str)
{
    A_Const* n = makeNode(A_Const);
    n->val.type = T_String;
    n->val.val.str = str;
    n->location = -1;
    return (Node*)n;
}

Node* plpsMakeRangeAlias(char* varName, char* aliasName)
{
    RangeVar* rv = makeRangeVar(NULL, varName, -1);
    Alias* n = makeNode(Alias);
    n->aliasname = aliasName;
    rv->inhOpt = INH_DEFAULT;
    rv->alias = n;
    return (Node*)rv;
}

Node* plpsMakeTargetFuncDirecAlias(char* funName, List* funcArgs, List* funcDirection, char* aliasName)
{
    FuncCall* fn = (FuncCall*)makeNode(FuncCall);
    fn->funcname = SystemFuncName(funName);
    fn->args = funcArgs;

    A_Indirection* n = makeNode(A_Indirection);
    n->arg = (Node*)fn;
    n->indirection = funcDirection;

    ResTarget* rt = makeNode(ResTarget);
    rt->name = aliasName;
    rt->indirection = NIL;
    rt->val = (Node*)n;
    rt->location = -1;
    return (Node*)rt;
}

static Node* makeStarColumn()
{
    ColumnRef *n = makeNode(ColumnRef);
    n->fields = list_make1(makeNode(A_Star));
    n->location = -1;
    n->indnum = 0;

    ResTarget* rt = makeNode(ResTarget);
    rt->name = NULL;
    rt->indirection = NIL;
    rt->val = (Node*)n;
    rt->location = -1;
    return (Node*)rt;
}

static Node* makeMsRangeFunction()
{
    Node* func = plpsMakeFunc("gs_master_status", NULL);

    RangeFunction *n = makeNode(RangeFunction);
    n->funccallnode = func;
    n->coldeflist = NIL;
    return (Node*)n;
}

static Node* makeRangeSubselect(SelectStmt* stmt)
{
    Alias* a = makeNode(Alias);
    a->aliasname = pstrdup("__unnamed_subquery__");
    RangeSubselect* rs = makeNode(RangeSubselect);
    rs->subquery = (Node*)stmt;
    rs->alias = NULL;
    rs->alias = a;
    return (Node*)rs;
}

static Node* makeColumnRef(char* colName, int loc)
{
    ColumnRef* cr = makeNode(ColumnRef);
    cr->fields = lcons(makeString(colName), NIL);
    cr->location = loc;
    return (Node*)cr;
}

static Node* makeHostColumn()
{
    Node* first = plpsMakeColumnRef(NULL, "client_hostname");
    Node* second = plpsMakeColumnRef(NULL, "client_addr");

    CoalesceExpr* expr = makeNode(CoalesceExpr);
    expr->args = list_make2(first, plpsMakeFunc("host", list_make1(second)));
    expr->isnvl = true;

    List* l = list_make3(plpsMakeStringConst(":"), expr, plpsMakeColumnRef(NULL, "client_port"));

    ResTarget* rt = makeNode(ResTarget);
    rt->name = "Host";
    rt->indirection = NIL;
    rt->val = plpsMakeFunc("concat_ws", l);
    rt->location = -1;
    return (Node*)rt;
}

static List* makeStateChangeIntervalFunc()
{
    Node* left = plpsMakeFunc("now", NULL);
    Node* right = plpsMakeColumnRef(NULL, "state_change");
    Node* expr = (Node*)makeSimpleA_Expr(AEXPR_OP, "-", left, right, -1);
    List* args = list_make2(plpsMakeStringConst("epoch"), expr);

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
    rt->val = plpsMakeFunc("abs", list_make1(plpsMakeFunc("round", makeStateChangeIntervalFunc())));
    rt->location = -1;
    return (Node*)rt;
}

static Node* makeInfoColumn(bool isFull)
{
    ResTarget* rt = makeNode(ResTarget);
    rt->name = "Info";
    rt->indirection = NIL;
    rt->val = plpsMakeFunc("left", list_make2(plpsMakeColumnRef(NULL, "query"), plpsMakeIntConst(isFull ? 1024 : 100)));
    rt->location = -1;
    return (Node*)rt;
}

static Node* makePluginsStatusColumn(bool smallcase)
{
    Node* cargnode = (Node*)makeSimpleA_Expr(AEXPR_OP, ">",
        plpsMakeFunc("length", list_make1(plpsMakeColumnRef(NULL, "installed_version"))),
        plpsMakeIntConst(0), -1);
    CaseExpr *c = makeNode(CaseExpr);
    c->casetype = InvalidOid;
    c->arg = (Expr *)cargnode;

    CaseWhen *w = makeNode(CaseWhen);
    w->expr = (Expr *)plpsMakeStringConst("t");
    w->result = (Expr *)plpsMakeStringConst("ACTIVE");
    c->args = list_make1(w);
    c->defresult = (Expr *)plpsMakeStringConst("DISABLED");

    ResTarget *rt = makeNode(ResTarget);
    if (!smallcase) {
        rt->name = SHOW_STATUS_COL;
    } else {
        rt->name = SHOW_STATUS_COL_S;
    }
    rt->indirection = NIL;
    rt->val = (Node*)c;
    rt->location = PLPS_LOC_UNKNOWN;

    return (Node*)rt;
}

static Node* makePluginsTypeColumn(bool smallcase)
{
    ResTarget *rt = makeNode(ResTarget);
    if (!smallcase) {
        rt->name = SHOW_TYPE_COL;
    } else {
        rt->name = SHOW_TYPE_COL_S;
    }
    rt->indirection = NIL;
    rt->val = plpsMakeStringConst("");
    rt->location = PLPS_LOC_UNKNOWN;

    return (Node*)rt;
}

static Node* makePluginsLibraryColumn(bool smallcase)
{
    ResTarget *rt = makeNode(ResTarget);
    if (!smallcase) {
        rt->name = SHOW_LIBRARY_COL;
    } else {
        rt->name = SHOW_LIBRARY_COL_S;
    }
    rt->indirection = NIL;
    rt->val = plpsMakeStringConst("NULL");
    rt->location = PLPS_LOC_UNKNOWN;

    return (Node*)rt;
}

static Node* makePluginsLicenseColumn(bool smallcase)
{
    ResTarget *rt = makeNode(ResTarget);
    if (!smallcase) {
        rt->name = SHOW_LICENSE_COL;
    } else {
        rt->name = SHOW_LICENSE_COL_S;
    }
    rt->indirection = NIL;
    rt->val = plpsMakeStringConst("");
    rt->location = PLPS_LOC_UNKNOWN;

    return (Node*)rt;
}

/**
 * Build a parsed tree for 'SHOW PLUGINS'.
 * This is actually a parse of the following statement:
 *
 * SELECT name AS "Name",
 *    CASE length(installed_version) > 0
 *        WHEN 't' then 'ACTIVE'
 *        ELSE 'DISABLED'
 *    END AS "Status",
 *    '' AS "Type",
 *    'NULL' AS "Library",
 *    '' AS "License",
 *    comment AS "Comment"
 * FROM
 *    pg_available_extensions
 *
 * @return The parsed tree for 'SHOW PLUGINS'
 */
SelectStmt* makeShowPluginsQuery(void)
{
    List *tl = list_make1(plpsMakeNormalColumn(NULL, SHOW_NAME_COL_S, SHOW_NAME_COL));
    tl = lappend(tl, makePluginsStatusColumn());
    tl = lappend(tl, makePluginsTypeColumn());
    tl = lappend(tl, makePluginsLibraryColumn());
    tl = lappend(tl, makePluginsLicenseColumn());
    tl = lappend(tl, plpsMakeNormalColumn(NULL, SHOW_COMMENT_COL_S, SHOW_COMMENT_COL));
    List* fl = list_make1(makeRangeVar(PG_CATALOG_NAME, "pg_available_extensions", -1));

    SelectStmt* stmt = plpsMakeSelectStmt(tl, fl, NULL, NULL);
    return stmt;
}

static Node* makeShowTablesTypeColumn(bool smallcase)
{
    CaseExpr *c = makeNode(CaseExpr);
    c->casetype = InvalidOid;
    c->arg = (Expr*)plpsMakeColumnRef(NULL, "relkind");

    CaseWhen *w = makeNode(CaseWhen);
    w->expr = (Expr*)plpsMakeStringConst("r");
    w->result = (Expr *)plpsMakeStringConst("BASE TABLE");

    List *cl = list_make1(w);
    w = makeNode(CaseWhen);
    w->expr = (Expr*)plpsMakeStringConst("v");
    w->result = (Expr*)plpsMakeStringConst("VIEW");
    cl = lappend(cl, w);

    c->args = cl;
    c->defresult = (Expr*)plpsMakeStringConst("UNKNOWN");

    ResTarget *rt = makeNode(ResTarget);
    if (!smallcase) {
        rt->name = SHOW_TBL_TYPE_COL;
    } else {
        rt->name = SHOW_TBL_TYPE_COL_S;
    }

    rt->indirection = NIL;
    rt->val = (Node*)c;
    rt->location = PLPS_LOC_UNKNOWN;

    return (Node *)rt;
}

static Node* makeShowTablesWhereTarget(char *schemaname, Node *likeWhereOpt)
{
    List *rkl = list_make2(plpsMakeStringConst("r"), plpsMakeStringConst("v"));
    Node *cond = NULL;
    Node *cond1 = NULL;
    Node *cond2 = NULL;

    cond1 = (Node*)makeA_Expr(AEXPR_AND, NIL,
                              (Node*)makeSimpleA_Expr(AEXPR_OP, "=",
                                                      plpsMakeColumnRef(PG_CLASS_NAME, "relnamespace"),
                                                      plpsMakeColumnRef(PG_NAMESPACE_NAME, "oid"), -1),
                              (Node*)makeSimpleA_Expr(AEXPR_IN, "=",
                                                      plpsMakeColumnRef(PG_CLASS_NAME, "relkind"), (Node*)rkl, -1), -1);

    cond2 = (Node*)makeSimpleA_Expr(AEXPR_OP, "=", plpsMakeColumnRef(PG_NAMESPACE_NAME, "nspname"),
                                    plpsMakeStringConst(schemaname), -1);
    cond = (Node*)makeA_Expr(AEXPR_AND, NIL, cond1, cond2, -1);

    if (likeWhereOpt != NULL) {
        cond = (Node*)makeA_Expr(AEXPR_AND, NIL, cond, likeWhereOpt, -1);
    }

    return cond;
}

bool plps_check_schema_or_table_valid(char *schemaname, char *tablename, bool is_missingok)
{
    Oid nspid = InvalidOid;
    Oid relid = InvalidOid;
    bool ret = TRUE;

    if (schemaname == NULL) {
        if (!is_missingok) {
            ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("schemaname NULL!!!!")));
        }
        return ret;
    }

    do {
        nspid = get_namespace_oid(schemaname, TRUE);
        if (!OidIsValid(nspid)) {
            ret = FALSE;
            break;
        }

        if (tablename == NULL) {
            break;
        }

        relid = get_relname_relid(tablename, nspid);
        if (!OidIsValid(relid)) {
            ret = FALSE;
        }
    } while(0);

    if (!ret && !is_missingok) {
        if (tablename == NULL) {
            ereport(ERROR,
                    (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("Unkown Schema '%s'.", schemaname)));
        } else {
            ereport(ERROR,
                    (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("Table '%s.%s' doesn't exist.", schemaname, tablename)));
        }
    }

    return ret;
}

SelectStmt* makeShowTablesDirectQuery(char *colTbl, char *schemaname, bool fullmode, bool smallcase, Node *likeWhereOpt)
{
    List* tl = list_make1(plpsMakeNormalColumn(NULL, "relname", colTbl));
    if (fullmode) {
        tl = lappend(tl, makeShowTablesTypeColumn(smallcase));
    }
    List* fl = list_make2(makeRangeVar(NULL, PG_CLASS_NAME, -1), makeRangeVar(NULL, PG_NAMESPACE_NAME, -1));
    Node* wc = makeShowTablesWhereTarget(schemaname, likeWhereOpt);

    SelectStmt* stmt = plpsMakeSelectStmt(tl, fl, wc, NULL);
    return stmt;
}

/**
 * Build a parsed tree for 'SHOW [FULL] TABLES [{FROM|IN} dbname] [LIKE 'pattern' | WHERE expr]'.
 * This is actually a parse of the following statement:
 *
 * SELECT
 *    "table_in_dbname(like_pattern)" AS "Table_in_dbname(like_pattern)",
 *    "type" AS "Type"
 * FROM
 * (
 *    SELECT c.relname AS "tables_in_dbname (like_pattern)",
 *        CASE c.relkind
 *            WHEN 'r' THEN 'NORMAL TABLE'
 *            WHEN 'v' then 'VIEW'
 *            ELSE 'UNKNOWN'
 *        END AS "type"
 *    FROM
 *        pg_class c, pg_namespace n
 *    WHERE c.relnamespace = n.oid
 *        AND c.relkind in ('r', 'v')
 *        AND n.nspname = `current_schema or input schemaname` // the input parameter, not valid sql
 *        AND a_expr // where clause or like 'pattern' converted
 * )
 *
 * @param fullmode
 * @param optDbName
 * @param likeWhereOpt
 * @param isLikeExpr
 * @return The parsed tree for 'SHOW [FULL] TABLES [{FROM|IN} dbname] [LIKE 'pattern' | WHERE expr]'
 */
SelectStmt* makeShowTablesQuery(bool fullmode, char *optDbName, Node *likeWhereOpt, bool isLikeExpr)
{
    char *schemaname = NULL;
    int rc;

    if (optDbName != NULL) {
        schemaname = optDbName;
    }

    if (schemaname == NULL) {
        schemaname = DatumGetCString(DirectFunctionCall1(current_schema, PointerGetDatum(NULL)));
    }

    (void)plps_check_schema_or_table_valid(schemaname, NULL, FALSE); // check error exit

    char *retColTbl = NULL;
    char *colTbl = NULL; // inner table
    int len;

    if (isLikeExpr && likeWhereOpt != NULL) {
        char *likeStr = ((A_Const*)likeWhereOpt)->val.val.str; // LIKE restrict Sconst
        len = strlen("Tables_in_") + strlen(schemaname) + strlen(" (") + strlen(likeStr) + 2; //2 for ')' and '\0'
        retColTbl = (char *)palloc0(len);
        colTbl = (char *)palloc0(len);
        rc = sprintf_s(retColTbl, len, "Tables_in_%s (%s)", schemaname, likeStr);
        securec_check_ss(rc, "", "");
        rc = sprintf_s(colTbl, len, "tables_in_%s (%s)", schemaname, likeStr);
        securec_check_ss(rc, "", "");

        likeWhereOpt = (Node*)makeSimpleA_Expr(AEXPR_OP, "~~", plpsMakeColumnRef(NULL, colTbl), likeWhereOpt, -1);
    } else {
        len = strlen("Tables_in_") + strlen(schemaname) + 1;
        retColTbl = (char *)palloc0(len);
        colTbl = (char *)palloc0(len);
        rc = sprintf_s(retColTbl, len, "Tables_in_%s", schemaname);
        securec_check_ss(rc, "", "");
        rc = sprintf_s(colTbl, len, "tables_in_%s", schemaname);
        securec_check_ss(rc, "", "");
    }

    bool smallcase_beneath = TRUE;
    List* tl = list_make1(plpsMakeNormalColumn(NULL, colTbl, retColTbl));
    if (fullmode) tl = lappend(tl, plpsMakeNormalColumn(NULL, SHOW_TBL_TYPE_COL_S, SHOW_TBL_TYPE_COL));
    List* fl = list_make1(makeRangeSubselect(
            makeShowTablesDirectQuery(colTbl, schemaname, fullmode, smallcase_beneath, likeWhereOpt)));

    SelectStmt* stmt = plpsMakeSelectStmt(tl, fl, NULL, NULL);
    return stmt;
}

SelectStmt* makeShowIndexQuery(char *schemaName, char *tableName, Node* whereClause)
{
    if (schemaName == NULL) {
        schemaName = DatumGetCString(DirectFunctionCall1(current_schema, PointerGetDatum(NULL)));
    }
    (void)plps_check_schema_or_table_valid(schemaName, tableName, false);

    List* tl = (List*)list_make1(plpsMakeNormalColumn(NULL, "table", "table"));
    tl = lappend(tl, plpsMakeNormalColumn(NULL, "non_unique", "non_unique"));
    tl = lappend(tl, plpsMakeNormalColumn(NULL, "key_name", "key_name"));
    tl = lappend(tl, plpsMakeNormalColumn(NULL, "seq_in_index", "seq_in_index"));
    tl = lappend(tl, plpsMakeNormalColumn(NULL, "column_name", "column_name"));
    tl = lappend(tl, plpsMakeNormalColumn(NULL, "collation", "collation"));
    tl = lappend(tl, plpsMakeNormalColumn(NULL, "cardinality", "cardinality"));
    tl = lappend(tl, plpsMakeNormalColumn(NULL, "sub_part", "sub_part"));
    tl = lappend(tl, plpsMakeNormalColumn(NULL, "packed", "packed"));
    tl = lappend(tl, plpsMakeNormalColumn(NULL, "null", "null"));
    tl = lappend(tl, plpsMakeNormalColumn(NULL, "index_type", "index_type"));
    tl = lappend(tl, plpsMakeNormalColumn(NULL, "comment", "comment"));
    tl = lappend(tl, plpsMakeNormalColumn(NULL, "index_comment", "index_comment"));

    RangeVar* rv = makeRangeVar(NULL, "index_statistic", -1);
    List* fl = (List*)list_make1(rv);

    Node* condSchema = (Node*)makeSimpleA_Expr(AEXPR_OP, "=", makeColumnRef("namespace"),
                                               plpsMakeStringConst(schemaName), -1);
    Node* condTable = (Node*)makeSimpleA_Expr(AEXPR_OP, "=", makeColumnRef("table"), plpsMakeStringConst(tableName), -1);
    Node* condST = plpsAddCond(condSchema, condTable);
    Node* wc = whereClause ? plpsAddCond(condST, whereClause) : condST;

    SelectStmt* stmt = plpsMakeSelectStmt(tl, fl, wc, NULL);
    return stmt;
}

#define VARIABLES_TABLES_COLS 2
PG_FUNCTION_INFO_V1_PUBLIC(ShowAllGUCReset);
extern "C" DLL_PUBLIC Datum ShowAllGUCReset(PG_FUNCTION_ARGS);

Datum ShowAllGUCReset(PG_FUNCTION_ARGS)
{
    TupleDesc tupdesc;
    Tuplestorestate *tupstore = NULL;
    Datum values[VARIABLES_TABLES_COLS];
    bool am_superuser = superuser();
    bool is_reset = PG_GETARG_BOOL(0);

    tupstore = BuildTupleResult(fcinfo, &tupdesc);
    for (int i = 0; i < u_sess->num_guc_variables; i++) {
        struct config_generic *conf = u_sess->guc_variables[i];
        const char *setting = NULL;
        bool isnull[VARIABLES_TABLES_COLS] = {false, false};
        unsigned int flags = (unsigned int)conf->flags;

        if ((flags & GUC_NO_SHOW_ALL) || ((flags & GUC_SUPERUSER_ONLY) && !am_superuser))
            continue;

        /* assign to the values array */
        values[0] = PointerGetDatum(cstring_to_text(conf->name));

        setting = is_reset ? GetConfigOptionResetString(conf->name) : GetConfigOptionByName(conf->name, NULL);

        if (setting != NULL) {
            values[1] = PointerGetDatum(cstring_to_text(setting));
            isnull[1] = false;
        } else {
            values[1] = PointerGetDatum(NULL);
            isnull[1] = true;
        }
        tuplestore_putvalues(tupstore, tupdesc, values, isnull);
    }

    tuplestore_donestoring(tupstore);
    PG_RETURN_VOID();
}

static Node *makeFuncRange(char *funcName, List *fl)
{
    FuncCall *fn = (FuncCall *)makeNode(FuncCall);
    fn->funcname = SystemFuncName(funcName);
    fn->args = fl;

    RangeFunction *n = makeNode(RangeFunction);
    n->funccallnode = (Node *)fn;
    n->coldeflist = NIL;
    return (Node *)n;
}

SelectStmt *makeShowVariablesQuery(bool globalMode, Node *likeWhereOpt, bool isLikeExpr)
{
    Node *wc = NULL;

    List *tl = (List *)list_make1(plpsMakeNormalColumn(NULL, "variable_name", "Variable_name"));
    tl = lappend(tl, plpsMakeNormalColumn(NULL, "value", "Value"));
    List *fl = list_make1(makeFuncRange("showallgucreset",list_make1(makeBoolAConst(globalMode, -1))));

    wc = isLikeExpr && likeWhereOpt != NULL
             ? (Node *)makeSimpleA_Expr(AEXPR_OP, "~~", plpsMakeColumnRef(NULL, "variable_name"), likeWhereOpt, -1)
             : likeWhereOpt;

    SelectStmt *stmt = plpsMakeSelectStmt(tl, fl, wc, NIL);
    return stmt;
}

Node* plpsMakeTargetNode(Node* val, char* name)
{
    ResTarget* rt = makeNode(ResTarget);
    rt->name = name;
    rt->indirection = NIL; 
    rt->val = val;
    rt->location = -1;
    return (Node*)rt;
}

Node* plpsMakeSimpleJoinNode(JoinType jointype, Node* larg, Node* rarg, Node* quals)
{
    JoinExpr* join = makeNode(JoinExpr);
    join->jointype = jointype;
    join->larg = larg;
    join->rarg = rarg;
    join->quals = quals;
    return (Node*)join;
}

static Node* makeEngineColumn()
{
    CaseWhen* w1 = makeNode(CaseWhen);
    w1->expr = (Expr*)makeSimpleA_Expr(AEXPR_OP, "=", 
                        plpsMakeColumnRef(PG_CLASS_NAME, "relkind"), plpsMakeStringConst("v"), -1);
    w1->result = (Expr*)makeNullAConst(-1);

    Node* ustroe = plpsMakeFunc("instr",
                    list_make2(plpsMakeTypeCast(plpsMakeColumnRef(PG_CLASS_NAME, "reloptions"), "text", -1), 
                            plpsMakeStringConst("ustore")));

    CaseWhen* w2 = makeNode(CaseWhen);
    w2->expr = (Expr*)makeSimpleA_Expr(AEXPR_OP, ">", ustroe, plpsMakeIntConst(0), -1);
    w2->result = (Expr*)plpsMakeStringConst("USTORE");

    CaseExpr* c = makeNode(CaseExpr);
    c->casetype = InvalidOid;
    c->arg = NULL;
    c->args = list_make2((Node*)w1, (Node*)w2);
    c->defresult = (Expr*)plpsMakeStringConst("ASTORE");
    c->location = -1;

    Node* rs = plpsMakeTargetNode((Node*)c, "engine");
    return rs;
}

static Node* makeRowFormatColumn()
{
    CaseWhen* w1 = makeNode(CaseWhen);
    w1->expr = (Expr*)makeSimpleA_Expr(AEXPR_OP, "=", 
                        plpsMakeColumnRef(PG_CLASS_NAME, "relkind"), plpsMakeStringConst("v"), -1);
    w1->result = (Expr*)makeNullAConst(-1);

    Node* cloumn = plpsMakeFunc("instr",
                    list_make2(plpsMakeTypeCast(plpsMakeColumnRef(PG_CLASS_NAME, "reloptions"), "text", -1), 
                            plpsMakeStringConst("column")));

    CaseWhen* w2 = makeNode(CaseWhen);
    w2->expr = (Expr*)makeSimpleA_Expr(AEXPR_OP, ">", cloumn, plpsMakeIntConst(0), -1);
    w2->result = (Expr*)plpsMakeStringConst("COLUMN");

    CaseExpr* c = makeNode(CaseExpr);
    c->casetype = InvalidOid;
    c->arg = NULL;
    c->args = list_make2((Node*)w1, (Node*)w2);
    c->defresult = (Expr*)plpsMakeStringConst("ROW");
    c->location = -1;

    Node* rs = plpsMakeTargetNode((Node*)c, "row_format");
    return rs;
}

static Node* makeAutoIncrementColumn()
{
    Node* fn = plpsMakeFunc("pg_sequence_last_value", (List*)list_make1(plpsMakeColumnRef(PG_DEPEND, "refobjid")));

    A_Indirection *n = makeNode(A_Indirection);
    n->arg = (Node*)fn;
    n->indirection = (List*)list_make1(makeString("last_value"));

    Node* rs = plpsMakeTargetNode((Node*)n, "auto_increment");
    return rs;
}

static List* makeShowTableStatusColumns()
{
    List* tl = (List*)list_make1(plpsMakeNormalColumn(PG_CLASS_NAME, "relname", "name"));
    tl = lappend(tl, makeEngineColumn());
    tl = lappend(tl, plpsMakeTargetNode(makeNullAConst(-1), "version"));
    tl = lappend(tl, makeRowFormatColumn());
    tl = lappend(tl, plpsMakeNormalColumn(PG_CLASS_NAME, "reltuples", "rows"));
    tl = lappend(tl, plpsMakeTargetNode(plpsMakeIntConst(0), "avg_row_length"));
    tl = lappend(tl, plpsMakeTargetNode(plpsMakeFunc("pg_relation_size", (List*)list_make1(plpsMakeColumnRef(PG_CLASS_NAME, "oid"))), "data_length"));
    tl = lappend(tl, plpsMakeTargetNode(plpsMakeIntConst(0), "max_data_length"));
    tl = lappend(tl, plpsMakeTargetNode(plpsMakeFunc("pg_indexes_size", (List*)list_make1(plpsMakeColumnRef(PG_CLASS_NAME, "oid"))), "index_length"));
    tl = lappend(tl, plpsMakeTargetNode(plpsMakeIntConst(0), "data_free"));
    tl = lappend(tl, makeAutoIncrementColumn());
    tl = lappend(tl, plpsMakeTargetNode(plpsMakeFunc("date_trunc", (List*)list_make2(plpsMakeStringConst("second"), 
                                        plpsMakeTypeCast(plpsMakeColumnRef(PG_OBJECT, "ctime"), "timestamp", -1))), "create_time"));
    tl = lappend(tl, plpsMakeTargetNode(plpsMakeFunc("date_trunc", (List*)list_make2(plpsMakeStringConst("second"), 
                                        plpsMakeTypeCast(plpsMakeColumnRef(PG_OBJECT, "mtime"), "timestamp", -1))), "update_time"));
    tl = lappend(tl, plpsMakeTargetNode(makeNullAConst(-1), "check_time"));
    tl = lappend(tl, plpsMakeTargetNode(plpsMakeFunc("current_setting", (List*)list_make1(plpsMakeStringConst("lc_collate"))), "collation"));
    tl = lappend(tl, plpsMakeTargetNode(makeNullAConst(-1), "checksum"));
    tl = lappend(tl, plpsMakeTargetNode(plpsMakeTypeCast(plpsMakeColumnRef(PG_CLASS_NAME, "reloptions"), "text", -1), "create_options"));
    tl = lappend(tl, plpsMakeNormalColumn(PG_DESCRIPTION, "description", "comment"));
    return tl;
}

static Node* makeShowTableStatusJoinTable()
{
    Node* pgClass = (Node*)makeRangeVar(NULL, PG_CLASS_NAME, -1);
    Node* pgNamspace = (Node*)makeRangeVar(NULL, PG_NAMESPACE_NAME, -1);
    Node* quals1 = (Node*)makeSimpleA_Expr(AEXPR_OP, "=", 
                            plpsMakeColumnRef(PG_CLASS_NAME, "relnamespace"), 
                            plpsMakeColumnRef(PG_NAMESPACE_NAME, "oid"), -1);
    Node* joinNamespace = plpsMakeSimpleJoinNode(JOIN_LEFT,  pgClass, pgNamspace, quals1);

    Node* pgObject = (Node*)makeRangeVar(NULL, PG_OBJECT, -1);
    Node* quals2 = (Node*)makeSimpleA_Expr(AEXPR_OP, "=", 
                            plpsMakeColumnRef(PG_CLASS_NAME, "oid"), 
                            plpsMakeColumnRef(PG_OBJECT, "object_oid"), -1);
    Node* joinObject = plpsMakeSimpleJoinNode(JOIN_LEFT,  joinNamespace, pgObject, quals2);

    Node* pgDescription = (Node*)makeRangeVar(NULL, PG_DESCRIPTION, -1);
    Node* quals3 = (Node*)makeSimpleA_Expr(AEXPR_OP, "=", 
                            plpsMakeColumnRef(PG_CLASS_NAME, "oid"), 
                            plpsMakeColumnRef(PG_DESCRIPTION, "objoid"), -1);
    Node* qualsObjsubid = (Node*)makeSimpleA_Expr(AEXPR_OP, "=", 
                            plpsMakeColumnRef(PG_DESCRIPTION, "objsubid"), 
                            plpsMakeIntConst(0), -1);
    Node* joinDescription = plpsMakeSimpleJoinNode(JOIN_LEFT,  joinObject, pgDescription,
                                (Node*)makeA_Expr(AEXPR_AND, NIL, quals3, qualsObjsubid, -1));

    Node* pgConstraint = (Node*)makeRangeVar(NULL, PG_CONSTRAINT, -1);
    Node* quals4 = (Node*)makeSimpleA_Expr(AEXPR_OP, "=", 
                            plpsMakeColumnRef(PG_CLASS_NAME, "oid"), 
                            plpsMakeColumnRef(PG_CONSTRAINT, "conrelid"), -1);
    Node* quals5 = (Node*)makeSimpleA_Expr(AEXPR_OP, "=", 
                            plpsMakeColumnRef(PG_CONSTRAINT, "contype"), 
                            plpsMakeStringConst("p"), -1);
    Node* quals6 = (Node*)makeSimpleA_Expr(AEXPR_OP, "=",
                            plpsMakeFunc("array_length", (List*)list_make2(plpsMakeColumnRef(PG_CONSTRAINT, "conkey"), plpsMakeIntConst(1))),
                            plpsMakeIntConst(1), -1);
    Node* joinConstraint = plpsMakeSimpleJoinNode(JOIN_LEFT,  joinDescription, pgConstraint, 
                                (Node*)makeA_Expr(AEXPR_AND, NIL, 
                                    (Node*)makeA_Expr(AEXPR_AND, NIL, quals4, quals5, -1), 
                                    quals6, -1));
    
    Node* pgAttrdef = (Node*)makeRangeVar(NULL, PG_ATTRDEF, -1);
    Node* quals7 = (Node*)makeSimpleA_Expr(AEXPR_OP, "=", 
                            plpsMakeColumnRef(PG_CLASS_NAME, "oid"), 
                            plpsMakeColumnRef(PG_ATTRDEF, "adrelid"), -1);
    A_Indirection* a = makeNode(A_Indirection);
    a->arg = (Node *)plpsMakeColumnRef(PG_CONSTRAINT, "conkey");
    A_Indices *ai = makeNode(A_Indices);
	ai->lidx = NULL;
	ai->uidx = plpsMakeIntConst(1);
    a->indirection = (List*)list_make1((Node*)ai);
    Node* quals8 = (Node*)makeSimpleA_Expr(AEXPR_OP, "=", 
                            plpsMakeColumnRef(PG_ATTRDEF, "adnum"), 
                            (Node*)a, -1);
    Node* joinAttrdef = plpsMakeSimpleJoinNode(JOIN_LEFT,  joinConstraint,  pgAttrdef,
                                (Node*)makeA_Expr(AEXPR_AND, NIL, quals7, quals8, -1));
    
    Node* pgDepend = (Node*)makeRangeVar(NULL, PG_DEPEND, -1);
    Node* quals9 = (Node*)makeSimpleA_Expr(AEXPR_OP, "=", 
                            plpsMakeColumnRef(PG_ATTRDEF, "oid"), 
                            plpsMakeColumnRef(PG_DEPEND, "objid"), -1);
    Node* quals10 = (Node*)makeSimpleA_Expr(AEXPR_OP, "=", 
                            plpsMakeColumnRef(PG_DEPEND, "refobjsubid"), 
                            plpsMakeIntConst(0), -1);
    Node* joinDepend = plpsMakeSimpleJoinNode(JOIN_LEFT,  joinAttrdef,  pgDepend,
                                (Node*)makeA_Expr(AEXPR_AND, NIL, quals9, quals10, -1));
    
    return (Node*)joinDepend;
}

/**
 * SELECT "name" as "Name", "engine" as "Engine", "version" as "Version", "row_format" as "Row_format", "rows" as "Rows",
 *     "avg_row_length" as "Avg_row_length", "data_length" as "Data_length", "max_data_length" as "Max_data_length",
 *     "index_length" as "Index_length", "data_free" as "Data_free", "auto_increment" as "Auto_increment",
 *     "create_time" as "Create_time", "update_time" as "Update_time", "check_time" as "Check_time",
 *     "collation" as "Collation", "checksum" as "Checksum", "create_options" as "Create_options", "comment" as "Comment"
 * FROM
 * (
 *     SELECT pg_class.relname AS "name",
 *         CASE WHEN pg_class.relkind='v' THEN NULL 
 *         WHEN POSITION('ustore' IN pg_class.reloptions::TEXT) > 0 THEN 'USTORE' ELSE 'ASTORE' END AS "engine",
 *         NULL AS "version",
 *         CASE WHEN pg_class.relkind='v' THEN NULL 
 *         WHEN POSITION('column' IN pg_class.reloptions::TEXT) > 0 THEN 'COLUMN' ELSE 'ROW' END AS "row_format",
 *         pg_class.reltuples as "rows",
 *         0 as "avg_row_length",
 *         pg_relation_size(pg_class.oid) as "data_length",
 *         0 as "max_data_length",
 *         pg_indexes_size(pg_class.oid) as "index_length",
 *         0 as "data_free",
 *         (pg_sequence_last_value(pg_depend.refobjid)).last_value as "auto_increment",
 *         date_trunc('second', pg_object.ctime::timestamp without time zone) as "create_time",
 *         date_trunc('second', pg_object.mtime::timestamp without time zone) as "update_time",
 *         NULL as "check_time",
 *         current_setting('lc_collate') as "collation",
 *         NULL as "checksum",
 *         pg_class.reloptions::TEXT as "create_options",
 *         pg_description.description as "comment"
 *     FROM
 *         pg_class
 *         LEFT JOIN pg_namespace pg_namespace ON pg_class.relnamespace = pg_namespace.oid
 *         LEFT JOIN pg_object ON pg_class.oid = pg_object.object_oid
 *         LEFT JOIN pg_description ON pg_class.oid = pg_description.classoid and pg_description.objsubid=0
 *         LEFT JOIN pg_constraint ON pg_class.oid=pg_constraint.conrelid and pg_constraint.contype='p' and array_length(pg_constraint.conkey, 1)=1
 *         LEFT JOIN pg_attrdef ON pg_class.oid=pg_attrdef.adrelid and pg_attrdef.adnum=pg_constraint.conkey[1]
 *         LEFT JOIN pg_depend ON pg_attrdef.oid=pg_depend.objid and pg_depend.refobjsubid=0
 *     WHERE
 *         pg_class.relkind in ('r', 'v') 
 *         AND pg_namespace.nspname = `current_schema or input schemaname` // the input parameter, not valid sql
 *         AND a_expr // where clause or like 'pattern' converted
 * )
 * 
 * @param schemaName
 * @param likeWhereOpt
 * @param isLikeExpr
 * @return The parsed tree for 'SHOW TABLE STATUS [{FROM|IN} dbname] [LIKE 'pattern' | WHERE expr]'
*/
SelectStmt* makeShowTableStatusQuery(char* schemaName, Node* likeWhereOpt, bool isLikeExpr)
{
    if (schemaName == NULL) {
        schemaName = DatumGetCString(DirectFunctionCall1(current_schema, PointerGetDatum(NULL)));
    }
    (void)plps_check_schema_or_table_valid(schemaName, NULL, FALSE);

    List* upperColumns = (List*)list_make1(plpsMakeNormalColumn(NULL, "name", "Name"));
    upperColumns = lappend(upperColumns, plpsMakeNormalColumn(NULL, "engine", "Engine"));
    upperColumns = lappend(upperColumns, plpsMakeNormalColumn(NULL, "version", "Version"));
    upperColumns = lappend(upperColumns, plpsMakeNormalColumn(NULL, "row_format", "Row_format"));
    upperColumns = lappend(upperColumns, plpsMakeNormalColumn(NULL, "rows", "Rows"));
    upperColumns = lappend(upperColumns, plpsMakeNormalColumn(NULL, "avg_row_length", "Avg_row_length"));
    upperColumns = lappend(upperColumns, plpsMakeNormalColumn(NULL, "data_length", "Data_length"));
    upperColumns = lappend(upperColumns, plpsMakeNormalColumn(NULL, "max_data_length", "Max_data_length"));
    upperColumns = lappend(upperColumns, plpsMakeNormalColumn(NULL, "index_length", "Index_length"));
    upperColumns = lappend(upperColumns, plpsMakeNormalColumn(NULL, "data_free", "Data_free"));
    upperColumns = lappend(upperColumns, plpsMakeNormalColumn(NULL, "auto_increment", "Auto_increment"));
    upperColumns = lappend(upperColumns, plpsMakeNormalColumn(NULL, "create_time", "Create_time"));
    upperColumns = lappend(upperColumns, plpsMakeNormalColumn(NULL, "update_time", "Update_time"));
    upperColumns = lappend(upperColumns, plpsMakeNormalColumn(NULL, "check_time", "Check_time"));
    upperColumns = lappend(upperColumns, plpsMakeNormalColumn(NULL, "collation", "Collation"));
    upperColumns = lappend(upperColumns, plpsMakeNormalColumn(NULL, "checksum", "Checksum"));
    upperColumns = lappend(upperColumns, plpsMakeNormalColumn(NULL, "create_options", "Create_options"));
    upperColumns = lappend(upperColumns, plpsMakeNormalColumn(NULL, "comment", "Comment"));

    List* lowerColumns = makeShowTableStatusColumns();
    Node* joinTable = makeShowTableStatusJoinTable();

    if (isLikeExpr && likeWhereOpt != NULL) {
        likeWhereOpt = (Node*)makeSimpleA_Expr(AEXPR_OP, "~~", plpsMakeColumnRef(PG_CLASS_NAME, "relname"), likeWhereOpt, -1);
    }

    Node* wc = makeShowTablesWhereTarget(schemaName, likeWhereOpt);

    SelectStmt* subSelect = plpsMakeSelectStmt(lowerColumns, list_make1(joinTable), wc, NULL);

    List* fl = list_make1(makeRangeSubselect(subSelect));

    SelectStmt* stmt = plpsMakeSelectStmt(upperColumns, fl, NULL, NULL);
    return stmt;
}

typedef struct PrivilegeRowData {
    char *priv_type;
    char *priv_context;
    char *priv_comment;
} PrivilegeRowData;

static PrivilegeRowData g_privlist[] = {
    {"Alter", "Large object,Sequence,Database,Foreign Server,Function,Node group,Schema,Tablespace,Type,Directory,Package",  "To alter the 'objects'"},
    {"Alter any index", "Index", "To alter any index"},
    {"Alter any sequence", "Sequence", "To alter any sequence"},
    {"Alter any table", "Table", "To alter any table"},
    {"Alter any trigger", "Trigger", "To alter any trigger"},
    {"Alter any type", "Type", "To alter any type"},
    {"Comment", "Table", "To comment on table"},
    {"Compute", "Node group", "To compute on node group"},
    {"Connect", "Database", "To connect database"},
    {"Create", "Database,Schema,Tablespace,Node group", "To create database,schema,tablespace,node group"},
    {"Create any function", "Function", "To create any function"},
    {"Create any index", "Index", "To create any index"},
    {"Create any package", "Package", "To create any package"},
    {"Create any sequence", "Sequence", "To create any sequence"},
    {"Create any synonym", "Synonym", "To create any synonym"},
    {"Create any table", "Table", "To create any table"},
    {"Create any trigger", "Trigger", "To create any trigger"},
    {"Create any type", "Type",  "To create any type"},
    {"Delte", "Table", "To delete table"},
    {"Delte any table", "Table", "To delete any table"},
    {"Drop any sequence", "Sequence", "To drop any sequence"},
    {"Drop any synonym", "Synonym", "To drop any synonym"},
    {"Drop any table", "Table", "To drop any table"},
    {"Drop any trigger", "Trigger", "To drop any trigger"},
    {"Drop any type", "Type", "To drop any type"},
    {"Execute", "Function,Procedure,Package", "To execute function, procedure,Package"},
    {"Execute any function", "Function", "To execute any function"},
    {"Execute any package", "Package", "To execute any package"},
    {"Index", "Table", "To create index on table"},
    {"Insert", "Table", "To insert into table"},
    {"Insert any table", "Table", "To insert any table"},
    {"References", "Table", "To have references on table"},
    {"Select", "Large object,Sequence,Table", "To select on large object,sequence and table"},
    {"Select any sequence", "Sequence", "To select any sequence"},
    {"Select any table", "Table", "To select on any table"},
    {"Temporary", "Database", "To create temporary table in database"},
    {"Temp", "Database", "To create temporary table in database"},
    {"Truncate", "Table", "To truncate table"},
    {"Update", "Large object,Sequence,Table", "To update large object,Sequence,Table"},
    {"Update any table", "Table", "To update any table"},
    {"Usage", "Domain,Foreign data wrapper,Foreign server,Language,Schema,Sequence,Type", "To use domain,fdw,foreign server,language,schema,sequence and type"},
    {"Vacuum", "Table", "To vacuum table"}
};

SelectStmt* makeShowPrivilegesSubQuery(void)
{
    SelectStmt* stmt = makeNode(SelectStmt);
    List* vl = NULL;
    List* sub_vl = NULL;
    int priv_len = sizeof(g_privlist)/sizeof(PrivilegeRowData);
    int i;

    for(i = 0; i < priv_len; i++) {
        sub_vl = list_make3(plpsMakeStringConst(g_privlist[i].priv_type),
                            plpsMakeStringConst(g_privlist[i].priv_context),
                            plpsMakeStringConst(g_privlist[i].priv_comment));
        vl = lappend(vl, sub_vl);
    }

    stmt->distinctClause = NIL;
    stmt->targetList = NIL;
    stmt->valuesLists = vl;
    stmt->fromClause = NIL;
    stmt->whereClause = NULL;
    stmt->sortClause = NIL;
    stmt->groupClause = NIL;
    stmt->havingClause = NULL;
    stmt->windowClause = NIL;
    stmt->hintState = NULL;
    stmt->hasPlus = false;

    return stmt;
}

SelectStmt* makeShowPrivilegesQuery(void)
{
    SelectStmt* stmt = makeNode(SelectStmt);
    Alias* alias = makeNode(Alias);
    ColumnRef* colRef = makeNode(ColumnRef);
    ResTarget* rt = makeNode(ResTarget);
    List* cl = NULL;

    cl = list_make3(makeString(SHOW_PRIVILEGES_COL), makeString(SHOW_CONTEXT_COL),
                        makeString(SHOW_COMMENT_COL));

    alias->aliasname = "__unnamed_subquery__";
    alias->colnames = cl;

    RangeSubselect* rsubselect = makeNode(RangeSubselect);
    rsubselect->subquery = (Node*)makeShowPrivilegesSubQuery();
    rsubselect->alias = alias;

    colRef->fields = list_make1(makeNode(A_Star));
    colRef->location = PLPS_LOC_UNKNOWN;
    colRef->indnum = 0;

    rt->val = (Node*)colRef;

    stmt->targetList = list_make1(rt);
    stmt->fromClause = list_make1(rsubselect);
    stmt->whereClause = NULL;
    stmt->sortClause = NIL;
    stmt->havingClause = NULL;
    stmt->windowClause = NULL;
    stmt->hintState = NULL;
    stmt->hasPlus = FALSE;

    return stmt;
}
