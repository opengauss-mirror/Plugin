#include "postgres.h"
#include "commands/dbcommands.h"
#include "nodes/makefuncs.h"
#include "catalog/namespace.h"
#include "utils/lsyscache.h"
#include "utils/builtins.h"
#include "plugin_parser/parse_show.h"

static Node* makeIntConst(int val);
static Node* makeStringConst(char* str);
static Node* makeHostColumn();
static List* makeStateChangeIntervalFunc();
static Node* makeTimeColumn();
static Node* makeInfoColumn(bool isFull);
static List* makeSortList();

static Node* makePluginsStatusColumn(bool smallcase = FALSE);
static Node* makePluginsTypeColumn(bool smallcase = FALSE);
static Node* makePluginsLibraryColumn(bool smallcase = FALSE);
static Node* makePluginsLicenseColumn(bool smallcase = FALSE);

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

Node* plpsMakeFunc(char* funcname, List* args, int location)
{
    return (Node*)makeFuncCall(list_make1(makeString(funcname)), args, location);
}

static Node* makeColumnRef(char* colName, int loc = PLPS_LOC_UNKNOWN)
{
    ColumnRef* cr = makeNode(ColumnRef);
    cr->fields = lcons(makeString(colName), NIL);
    cr->location = loc;
    return (Node*)cr;
}

Node* plpsMakeColumnRef(char* relname, char *colname, int location)
{
    if (relname == NULL) {
    	return makeColumnRef(colname, location);
    }
    
    return (Node*)makeColumnRef(relname, colname, location);
}

Node* plpsMakeNormalColumn(char *relname, char* colname, char* aliasname, int location)
{
    Node* n = plpsMakeColumnRef(relname, colname);
    ResTarget* rt = makeNode(ResTarget);
    rt->name = aliasname;
    rt->indirection = NIL;
    rt->val = n;
    rt->location = location;
    return (Node*)rt;
}

static Node* makeHostColumn()
{
    Node* first = plpsMakeColumnRef(NULL, "client_hostname");
    Node* second = plpsMakeColumnRef(NULL, "client_addr");

    CoalesceExpr* expr = makeNode(CoalesceExpr);
    expr->args = list_make2(first, plpsMakeFunc("host", list_make1(second)));
    expr->isnvl = true;

    List* l = list_make3(makeStringConst(":"), expr, plpsMakeColumnRef(NULL, "client_port"));

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
    rt->val = plpsMakeFunc("abs", list_make1(plpsMakeFunc("round", makeStateChangeIntervalFunc())));
    rt->location = -1;
    return (Node*)rt;
}

static Node* makeInfoColumn(bool isFull)
{
    ResTarget* rt = makeNode(ResTarget);
    rt->name = "Info";
    rt->indirection = NIL;
    rt->val = plpsMakeFunc("left", list_make2(plpsMakeColumnRef(NULL, "query"), makeIntConst(isFull ? 1024 : 100)));
    rt->location = -1;
    return (Node*)rt;
}

static List* makeSortList()
{
    SortBy* n = makeNode(SortBy);
    n->node = plpsMakeColumnRef(NULL, "backend_start");
    n->sortby_dir = SORTBY_DEFAULT;
    n->sortby_nulls = SORTBY_NULLS_DEFAULT;
    n->useOp = NIL;
    n->location = -1;
    List* sl = (List*)list_make1(n);
    return sl;
}

static Node* makePluginsStatusColumn(bool smallcase)
{
    Node* cargnode = (Node*)makeSimpleA_Expr(AEXPR_OP, ">",
 					     plpsMakeFunc("length", list_make1(plpsMakeColumnRef(NULL, "installed_version"))),
    					     makeIntConst(0), -1);
    CaseExpr *c = makeNode(CaseExpr);
    c->casetype = InvalidOid;
    c->arg = (Expr *)cargnode;
    
    CaseWhen *w = makeNode(CaseWhen);
    w->expr = (Expr *)makeStringConst("t");
    w->result = (Expr *)makeStringConst("ACTIVE");
    c->args = list_make1(w);
    c->defresult = (Expr *)makeStringConst("DISABLED");
    
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
    rt->val = makeStringConst("");
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
    rt->val = makeStringConst("NULL");
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
    rt->val = makeStringConst("");
    rt->location = PLPS_LOC_UNKNOWN;
    
    return (Node*)rt;
}

/**
 * Build a parsed tree for 'SHOW PLUGINS'.
 * This is actually a parse of the following statement:
 *
 * SELECT name AS "Name",
 *   CASE length(installed_version) > 0
 *     WHEN 't' then 'ACTIVE'
 *     ELSE 'DISABLED'
 *   END AS "Status",
 *   '' AS "Type",
 *   'NULL' AS "Library",
 *   '' AS "License",
 *   comment AS "Comment"
 * FROM
 *   pg_available_extensions
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
    
    SelectStmt *stmt = makeNode(SelectStmt);
    stmt->distinctClause = NIL;
    stmt->targetList = tl;
    stmt->intoClause = NULL;
    stmt->fromClause = list_make1(makeRangeVar(PG_CATALOG_NAME, "pg_available_extensions", -1));
    stmt->whereClause = NULL;
    stmt->sortClause = NIL;
    stmt->groupClause = NIL;
    stmt->havingClause = NULL;
    stmt->windowClause = NIL;
    stmt->hintState = NULL;
    stmt->hasPlus = false;

    return stmt;	
}

static Node* makeShowTablesTypeColumn(bool smallcase)
{
    CaseExpr *c = makeNode(CaseExpr);
    c->casetype = InvalidOid;
    c->arg = (Expr*)plpsMakeColumnRef(NULL, "relkind");
    
    CaseWhen *w = makeNode(CaseWhen);
    w->expr = (Expr*)makeStringConst("r");
    w->result = (Expr *)makeStringConst("BASE TABLE");
    
    List *cl = list_make1(w);
    w = makeNode(CaseWhen);
    w->expr = (Expr*)makeStringConst("v");
    w->result = (Expr*)makeStringConst("VIEW");
    cl = lappend(cl, w);
    
    c->args = cl;
    c->defresult = (Expr*)makeStringConst("UNKNOWN");
    
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
    List *rkl = list_make2(makeStringConst("r"), makeStringConst("v"));
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
				    makeStringConst(schemaname), -1);  
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
    
    SelectStmt *stmt = makeNode(SelectStmt);
    stmt->distinctClause = NIL;
    stmt->targetList = tl;
    stmt->intoClause = NULL;
    stmt->fromClause = list_make2(makeRangeVar(NULL, PG_CLASS_NAME, -1),
				  makeRangeVar(NULL, PG_NAMESPACE_NAME, -1));
    stmt->whereClause = makeShowTablesWhereTarget(schemaname, likeWhereOpt);
    stmt->sortClause = NIL;
    stmt->groupClause = NIL;
    stmt->havingClause = NULL;
    stmt->windowClause = NIL;
    stmt->hintState = NULL;
    stmt->hasPlus = false;

    return stmt;
}

/**
 * Build a parsed tree for 'SHOW [FULL] TABLES [{FROM|IN} dbname] [LIKE 'pattern' | WHERE expr]'.
 * This is actually a parse of the following statement:
 *
 * SELECT "table_in_dbname(like_pattern)" AS "Table_in_dbname(like_pattern)",
 *        "type" AS "Type"
 * FROM
 * (
 *   SELECT c.relname AS "tables_in_dbname (like_pattern)",
 *     CASE c.relkind
 *       WHEN 'r' THEN 'NORMAL TABLE'
 *       WHEN 'v' then 'VIEW'
 *       ELSE 'UNKNOWN'
 *     END AS "type"
 *   FROM
 *     pg_class c, pg_namespace n
 *   WHERE c.relnamespace = n.oid
 *     AND c.relkind in ('r', 'v')
 *     AND n.nspname = `current_schema or input schemaname` // the input parameter, not valid sql
 *     AND a_expr // where clause or like 'pattern' converted
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
    
    RangeSubselect *rsubselect = makeNode(RangeSubselect);
    Alias *alias = makeNode(Alias);
    bool smallcase_beneath = TRUE; 
    
    alias->aliasname = pstrdup("__unnamed_subquery__");
    rsubselect->subquery = (Node*)makeShowTablesDirectQuery(colTbl, schemaname, fullmode, smallcase_beneath, likeWhereOpt);
    rsubselect->alias = alias;

    List *tl = list_make1(plpsMakeNormalColumn(NULL, colTbl, retColTbl));
    if (fullmode) {
    	tl = lappend(tl, plpsMakeNormalColumn(NULL, SHOW_TBL_TYPE_COL_S, SHOW_TBL_TYPE_COL));
    }
    
    SelectStmt *stmt = makeNode(SelectStmt);
    stmt->targetList = tl;
    stmt->fromClause = list_make1(rsubselect);
    stmt->whereClause = NULL;
    stmt->sortClause = NIL;
    stmt->havingClause = NULL;
    stmt->windowClause = NULL;
    stmt->hintState = NULL;
    stmt->hasPlus = FALSE;

    return stmt;
}

