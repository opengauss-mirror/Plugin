#include "postgres.h"
#include "nodes/makefuncs.h"
#include "catalog/indexing.h"
#include "catalog/namespace.h"
#include "catalog/pg_proc.h"
#include "catalog/pg_trigger.h"
#include "plugin_parser/parser.h"
#include "plugin_parser/parse_show.h"
#include "utils/fmgroids.h"
#include "utils/lsyscache.h"
#include "utils/builtins.h"
#include "knl/knl_variable.h"

static Node *makeNameString(char *str, char *aliasName);
static Node *makeTypeCast(Node *arg, TypeName *typname);
static Node *makeStringConstCast(char *str, TypeName *typname);
static Node *makeTargetFuncAlias(char *funcName, List *fl, char *aliasName);
static Node *makeWhereTargetForFunc(char *schemaName, char *name);
static Node *makeWhereTargetForTrg(char *schemaName, char *name);
static Node *makeCurrentSchemaFunc();
static SelectStmt *makeShowCreateViewQuery(char *fullName, char *name);
static SelectStmt *makeShowCreateTableQuery(char *fullName, char *tableName);
static SelectStmt *makeShowCreateFuncQuery(char *schemaName, char *name, int model);
static SelectStmt *makeShowCreateTriggerQuery(char *schemaName, char *name);

/**
 * Build a parsed tree for 'show create function' and 'show create procedure'.
 * This is actually a parse of the following statement:
 *
 * select
 * 'name' as Function,
 * (pg_get_functiondef(pg_proc.oid)).definition as "Create function",
 * current_setting('sql_mode') as sql_mode,
 * current_setting('client_encoding') as character_set_client,
 * current_setting('lc_collate') as collation_connection,
 * current_setting('lc_collate') as "Database Collation"
 * from
 * pg_proc,
 * pg_namespace
 * where
 * pg_proc.proname = 'name'
 * and pg_namespace.nspname = 'schemaName'
 * and pg_proc.pronamespace = pg_namespace.oid;
 *
 */
SelectStmt *makeShowCreateFuncQuery(char *schemaName, char *name, int model)
{
    List *tl =
        (List *)list_make1(makeNameString(name, (char *)(model == GS_SHOW_CREATE_FUNCTION ? "Function" : "Procedure")));
    tl = lappend(tl, plpsMakeTargetFuncDirecAlias(
                         "pg_get_functiondef", list_make1(plpsMakeColumnRef("pg_proc", "oid")),
                         list_make1(makeString("definition")),
                         (char *)(model == GS_SHOW_CREATE_FUNCTION ? "Create Function" : "Create Procedure")));
    tl = lappend(
        tl, makeTargetFuncAlias("current_setting", (List *)list_make1(plpsMakeStringConst("dolphin.sql_mode")), "sql_mode"));
    tl = lappend(tl, makeTargetFuncAlias("current_setting", (List *)list_make1(plpsMakeStringConst("client_encoding")),
                                         "character_set_client"));
    tl = lappend(tl, makeTargetFuncAlias("current_setting", (List *)list_make1(plpsMakeStringConst("lc_collate")),
                                         "collation_connection"));
    tl = lappend(tl, makeTargetFuncAlias("current_setting", (List *)list_make1(plpsMakeStringConst("lc_collate")),
                                         "Database Collation"));
    List *fl = list_make2(makeRangeVar(NULL, "pg_proc", -1), makeRangeVar(NULL, "pg_namespace", -1));
    SelectStmt *stmt = plpsMakeSelectStmt(tl, fl, makeWhereTargetForFunc(schemaName, name), NIL);
    return stmt;
}

/**
 * Build a parsed tree for 'show create table'.
 * This is actually a parse of the following statement:
 *
 * select
 * 'tableName' as Table,
 * pg_get_tabledef('tableName'::regclass::oid) as "Create Table";
 *
 */
static SelectStmt *makeShowCreateTableQuery(char *fullName, char *tableName)
{
    List *tl = (List *)list_make1(makeNameString(tableName, "Table"));
    tl = lappend(tl,
                 makeTargetFuncAlias("pg_get_tabledef",
                                     list_make1(makeTypeCast(makeStringConstCast(fullName, SystemTypeName("regclass")),
                                                             SystemTypeName("oid"))),
                                     "Create Table"));

    SelectStmt *stmt = plpsMakeSelectStmt(tl, NIL, NULL, NIL);
    return stmt;
}

/**
 * Build a parsed tree for 'show create view'.
 * This is actually a parse of the following statement:
 *
 * select
 * 'name' as View,
 * pg_get_viewdef('name')  as "Create View",
 * current_setting('client_encoding') as character_set_client,
 * current_setting('lc_collate') as collation_connection;
 *
 */
static SelectStmt *makeShowCreateViewQuery(char *fullName, char *name)
{
    List *tl = (List *)list_make1(makeNameString(name, "View"));
    tl = lappend(tl, makeTargetFuncAlias("gs_get_viewdef_name", (List *)list_make1(plpsMakeStringConst(fullName)),
                                         "Create View"));
    tl = lappend(tl, makeTargetFuncAlias("current_setting", (List *)list_make1(plpsMakeStringConst("client_encoding")),
                                         "character_set_client"));
    tl = lappend(tl, makeTargetFuncAlias("current_setting", (List *)list_make1(plpsMakeStringConst("lc_collate")),
                                         "collation_connection"));

    SelectStmt *stmt = plpsMakeSelectStmt(tl, NIL, NULL, NIL);
    return stmt;
}

/**
 * Build a parsed tree for 'show create view'.
 * This is actually a parse of the following statement:
 *
 * select
 * 'before_ins_stmt_trig' as Trigger,
 * current_setting('sql_mode') as sql_mode,
 * pg_get_triggerdef(pg_trigger.oid) as "SQL Original Statement",
 * current_setting('client_encoding') as character_set_client,
 * current_setting('lc_collate') as collation_connection,
 * current_setting('lc_collate') as "Database Collation"
 * from
 * pg_trigger,
 * pg_namespace,
 * pg_class
 * where
 * pg_trigger.tgname = 'before_ins_stmt_trig'
 * and pg_namespace.nspname = 'a'
 * and pg_class.oid = pg_trigger.tgrelid
 * and pg_class.relnamespace = pg_namespace.oid;
 *
 */
SelectStmt *makeShowCreateTriggerQuery(char *schemaName, char *name)
{
    List *tl = (List *)list_make1(makeNameString(name, "Trigger"));
    tl = lappend(
        tl, makeTargetFuncAlias("current_setting", (List *)list_make1(plpsMakeStringConst("dolphin.sql_mode")), "sql_mode"));
    tl =
        lappend(tl, makeTargetFuncAlias("pg_get_triggerdef", (List *)list_make1(plpsMakeColumnRef("pg_trigger", "oid")),
                                        "SQL Original Statement"));
    tl = lappend(tl, makeTargetFuncAlias("current_setting", (List *)list_make1(plpsMakeStringConst("client_encoding")),
                                         "character_set_client"));
    tl = lappend(tl, makeTargetFuncAlias("current_setting", (List *)list_make1(plpsMakeStringConst("lc_collate")),
                                         "collation_connection"));
    tl = lappend(tl, makeTargetFuncAlias("current_setting", (List *)list_make1(plpsMakeStringConst("lc_collate")),
                                         "Database Collation"));
    List *fl = list_make3(makeRangeVar(NULL, "pg_trigger", -1), makeRangeVar(NULL, "pg_namespace", -1),
                          makeRangeVar(NULL, "pg_class", -1));
    SelectStmt *stmt = plpsMakeSelectStmt(tl, fl, makeWhereTargetForTrg(schemaName, name), NIL);
    return stmt;
}

/**
 * Build a parsed tree for 'show create database'.
 * This is actually a parse of the following statement:
 *
 * select
 * 'databaseName' as Database,
 * gs_get_schemadef_name('databaseName') as "Create Database";
 *
 */
SelectStmt *makeShowCreateDatabaseQuery(bool ifexists, char *databaseName)
{
    List *tl = (List *)list_make1(makeNameString(databaseName, "Database"));
    tl = lappend(
        tl, makeTargetFuncAlias("gs_get_schemadef_name",
                                (List *)list_make2(plpsMakeStringConst(databaseName), makeBoolAConst(ifexists, -1)),
                                "Create Database"));

    SelectStmt *stmt = plpsMakeSelectStmt(tl, NIL, NULL, NIL);
    return stmt;
}

static Node *makeTargetFuncAlias(char *funcName, List *fl, char *aliasName)
{
    FuncCall *fn = (FuncCall *)makeNode(FuncCall);
    fn->funcname = SystemFuncName(funcName);
    fn->args = fl;

    ResTarget *rt = makeNode(ResTarget);
    rt->name = aliasName;
    rt->indirection = NIL;
    rt->val = (Node *)fn;
    rt->location = -1;
    return (Node *)rt;
}

static Node *makeNameString(char *str, char *aliasName)
{
    ResTarget *rt = makeNode(ResTarget);
    rt->name = aliasName;
    rt->indirection = NIL;
    rt->val = plpsMakeStringConst(str);
    rt->location = -1;
    return (Node *)rt;
}

static Node *makeTypeCast(Node *arg, TypeName *typname)
{
    TypeCast *n = makeNode(TypeCast);
    n->arg = arg;
    n->typname = typname;
    n->location = -1;
    return (Node *)n;
}

static Node *makeStringConstCast(char *str, TypeName *typname)
{
    Node *s = plpsMakeStringConst(str);

    return makeTypeCast(s, typname);
}

static Node *makeWhereTargetForFunc(char *schemaName, char *name)
{
    Node *cond1 =
        (Node *)makeSimpleA_Expr(AEXPR_OP, "=", plpsMakeColumnRef("pg_proc", "proname"), plpsMakeStringConst(name), -1);
    Node *cond2 = (Node *)makeSimpleA_Expr(AEXPR_OP, "=", plpsMakeColumnRef("pg_namespace", "nspname"),
                                           schemaName ? plpsMakeStringConst(schemaName) : makeCurrentSchemaFunc(), -1);
    Node *cond3 = (Node *)makeSimpleA_Expr(AEXPR_OP, "=", plpsMakeColumnRef("pg_proc", "pronamespace"),
                                           plpsMakeColumnRef("pg_namespace", "oid"), -1);
    Node *wt = plpsAddCond(cond1, plpsAddCond(cond2, cond3));
    return wt;
}

static Node *makeWhereTargetForTrg(char *schemaName, char *name)
{
    Node *cond1 = (Node *)makeSimpleA_Expr(AEXPR_OP, "=", plpsMakeColumnRef("pg_trigger", "tgname"),
                                           plpsMakeStringConst(name), -1);
    Node *cond2 = (Node *)makeSimpleA_Expr(AEXPR_OP, "=", plpsMakeColumnRef("pg_namespace", "nspname"),
                                           schemaName ? plpsMakeStringConst(schemaName) : makeCurrentSchemaFunc(), -1);
    Node *cond3 = (Node *)makeSimpleA_Expr(AEXPR_OP, "=", plpsMakeColumnRef("pg_class", "oid"),
                                           plpsMakeColumnRef("pg_trigger", "tgrelid"), -1);
    Node *cond4 = (Node *)makeSimpleA_Expr(AEXPR_OP, "=", plpsMakeColumnRef("pg_class", "relnamespace"),
                                           plpsMakeColumnRef("pg_namespace", "oid"), -1);
    Node *wt = plpsAddCond(cond1, plpsAddCond(cond2, plpsAddCond(cond3, cond4)));
    return wt;
}

static Node *makeCurrentSchemaFunc()
{
    FuncCall *n = makeNode(FuncCall);
    n->funcname = SystemFuncName("current_schema");
    n->args = NIL;
    n->agg_order = NIL;
    n->agg_star = FALSE;
    n->agg_distinct = FALSE;
    n->func_variadic = FALSE;
    n->over = NULL;
    n->location = -1;
    n->call_func = false;
    return (Node *)n;
}

SelectStmt *findCreateClass(RangeVar *classrel, int mode)
{
    Oid namespaceId = InvalidOid;
    Oid classoid = InvalidOid;
    Form_pg_class classForm = NULL;
    HeapTuple tuple = NULL;
    char *fullName = NULL;
    SelectStmt *n = NULL;
    recomputeNamespacePath();

    if (classrel->schemaname != NULL) {
        namespaceId = get_namespace_oid(classrel->schemaname, false);
    } else {
        namespaceId = getCurrentNamespace();
    }

    if (!OidIsValid(namespaceId))
        ereport(ERROR, (errcode(ERRCODE_UNDEFINED_SCHEMA), errmsg("Schema doesn't exist")));
    classoid = get_relname_relid(classrel->relname, namespaceId);
    if (!OidIsValid(classoid)) {
        classoid = get_relname_relid(classrel->relname, u_sess->catalog_cxt.myTempNamespace);
        if (!OidIsValid(classoid)) {
            ereport(ERROR,
                    (errcode(ERRCODE_UNDEFINED_TABLE),
                     errmsg("Table \'%s.%s\' doesn't exist", get_namespace_name(namespaceId), classrel->relname)));
        }
    }

    tuple = SearchSysCache1(RELOID, ObjectIdGetDatum(classoid));
    if (!HeapTupleIsValid(tuple)) {
        ereport(ERROR, (errcode(ERRCODE_CACHE_LOOKUP_FAILED), errmsg("cache lookup failed for %u.", classoid)));
    }
    fullName = quote_qualified_identifier(classrel->schemaname, classrel->relname);
    
    classForm = (Form_pg_class)GETSTRUCT(tuple);
    if (classForm->relkind == RELKIND_VIEW || classForm->relkind == RELKIND_MATVIEW) {
        n = makeShowCreateViewQuery(fullName, classrel->relname);
    } else {
        if (mode == GS_SHOW_CREATE_VIEW)
            ereport(ERROR, (errcode(ERRCODE_UNDEFINED_TABLE),
                            errmsg("\'%s.%s\' is not VIEW", get_namespace_name(namespaceId), classrel->relname)));
        n = makeShowCreateTableQuery(fullName, classrel->relname);
    }
    ReleaseSysCache(tuple);
    return n;
}

SelectStmt *findCreateProc(char *schemaName, char *name, int model)
{
    Oid namespaceId = InvalidOid;
    bool is_found = false;
    CatCList *catlist = NULL;

    recomputeNamespacePath();

    namespaceId = schemaName != NULL ? get_namespace_oid(schemaName, false) : getCurrentNamespace();
    if (!OidIsValid(namespaceId))
        ereport(ERROR, (errcode(ERRCODE_UNDEFINED_SCHEMA), errmsg("schema does not exist")));

#ifndef ENABLE_MULTIPLE_NODES
    if (t_thrd.proc->workingVersionNum < 92470) {
        catlist = SearchSysCacheList1(PROCNAMEARGSNSP, CStringGetDatum(name));
    } else {
        catlist = SearchSysCacheList1(PROCALLARGS, CStringGetDatum(name));
    }
#else
    catlist = SearchSysCacheList1(PROCNAMEARGSNSP, CStringGetDatum(name));
#endif
    if (catlist != NULL) {
        for (int i = 0; i < catlist->n_members && !is_found; ++i) {
            HeapTuple proctup = t_thrd.lsc_cxt.FetchTupleFromCatCList(catlist, i);
            Form_pg_proc procform = (Form_pg_proc)GETSTRUCT(proctup);
            if (procform != NULL && procform->pronamespace == namespaceId) {
                Oid funcid = HeapTupleGetOid(proctup);
                is_found = OidIsValid(funcid);
            }
        }
        ReleaseSysCacheList(catlist);
    }
    if (!is_found)
        ereport(ERROR,
                (errcode(ERRCODE_WRONG_OBJECT_TYPE),
                 errmsg("%s %s does not exist", model == GS_SHOW_CREATE_FUNCTION ? "FUNCTION" : "PROCEDURE", name)));
    return makeShowCreateFuncQuery(schemaName, name, model);
}

SelectStmt *findCreateTrigger(RangeVar *trel)
{
    Relation tgrel;
    ScanKeyData keys[1];
    SysScanDesc tgscan;
    HeapTuple tuple;
    Form_pg_class reltup;
    Form_pg_trigger trigrec;
    char *nspname = NULL;
    char *schemaName = NULL;
    bool found = false;

    if (trel->schemaname == NULL) {
        List *search_path = fetch_search_path(false);
        if (search_path == NIL)
            ereport(ERROR, (errcode(ERRCODE_UNDEFINED_OBJECT), errmsg("there is no default schema")));
        schemaName = get_namespace_name(linitial_oid(search_path));
        list_free_ext(search_path);
        if (schemaName == NULL)
            ereport(ERROR, (errcode(ERRCODE_UNDEFINED_OBJECT), errmsg("there is no default schema")));
    } else
        schemaName = trel->schemaname;

    tgrel = heap_open(TriggerRelationId, AccessShareLock);
    ScanKeyInit(&keys[0], Anum_pg_trigger_tgname, BTEqualStrategyNumber, F_NAMEEQ, CStringGetDatum(trel->relname));
    tgscan = systable_beginscan(tgrel, TriggerRelidNameIndexId, true, NULL, 1, keys);

    while (HeapTupleIsValid(tuple = systable_getnext(tgscan))) {
        trigrec = (Form_pg_trigger)GETSTRUCT(tuple);

        HeapTuple tp = SearchSysCache1(RELOID, ObjectIdGetDatum(trigrec->tgrelid));
        if (!HeapTupleIsValid(tp))
            ereport(ERROR, (errcode(ERRCODE_CACHE_LOOKUP_FAILED),
                            errmsg("cache lookup failed for relation %u", trigrec->tgrelid)));
        reltup = (Form_pg_class)GETSTRUCT(tp);
        nspname = get_namespace_name(reltup->relnamespace);
        ReleaseSysCache(tp);
        if (strcmp(nspname, schemaName) == 0) {
            found = true;
            break;
        }
    }
    systable_endscan(tgscan);
    heap_close(tgrel, AccessShareLock);
    if (!found)
        ereport(ERROR, (errcode(ERRCODE_UNDEFINED_OBJECT), errmsg("Trigger does not exist")));
    return makeShowCreateTriggerQuery(schemaName, trel->relname);
}
