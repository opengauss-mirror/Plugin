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
 *    database AS "Database"
 * FROM
 *    (select nspname as "database" from pg_catalog.pg_namespace where expr)
 * ORDER BY
 *    database = 'information_schema' desc,
 *    1;
 *
 * @param likeNode
 * @param whereExpr
 * @return The parsed tree for 'SHOW {DATABASES | SCHEMAS} [LIKE 'pattern' | WHERE expr]'.
 */
SelectStmt* makeShowDatabasesQuery(Node* likeNode, Node* whereExpr)
{
    List* tl = list_make1(plpsMakeNormalColumn(NULL, "nspname", "database"));
    List* fl = list_make1(makeRangeVar(NULL, "pg_namespace", -1));
    Node* wc = NULL;
    if (likeNode) {
        wc = (Node*)makeSimpleA_Expr(AEXPR_OP, "~~", plpsMakeColumnRef(NULL, "database"), likeNode, -1);
    } else if (whereExpr) {
        wc = whereExpr;
    }

    SelectStmt* substmt = plpsMakeSelectStmt(tl, fl, wc, NULL);

    List* tl2 = list_make1(plpsMakeNormalColumn(NULL, "database", "Database"));
    List* fl2 = list_make1(makeRangeSubselect(substmt));
    Node* sn1 = plpsMakeSortByNode((Node*)makeSimpleA_Expr(AEXPR_OP, "=", plpsMakeColumnRef(NULL, "database"), plpsMakeStringConst("information_schema"), -1), SORTBY_DESC);
    Node* sn2 = plpsMakeSortByNode(plpsMakeIntConst(1));
    List* sl2 = (List*)list_make2(sn1, sn2);

    SelectStmt* stmt = plpsMakeSelectStmt(tl2, fl2, NULL, sl2);
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
