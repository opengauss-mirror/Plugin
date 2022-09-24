#include "plugin_utils/show_common.h"
#include "funcapi.h"
#include "nodes/makefuncs.h"
#include "plugin_parser/parse_show.h"

PG_FUNCTION_INFO_V1_PUBLIC(ShowTriggers);
extern "C" DLL_PUBLIC Datum ShowTriggers(PG_FUNCTION_ARGS);

/**
 * tuple desc of show trigger
 * @return return tuple desc of show trigger
 */
TupleDesc ShowTriggersDesc()
{
    TupleDescEntry tupleDescEntries[] = {
        {"Trigger", VARCHAROID},
        {"Event", VARCHAROID},
        {"Table", VARCHAROID},
        {"Statement", VARCHAROID},
        {"Timing", VARCHAROID},
        {"Created", TIMESTAMPTZOID},
        {"sql_mode", VARCHAROID},
        {"Definer", NAMEOID},
        {"character_set_client", VARCHAROID},
        {"collation_connection", VARCHAROID},
        {"Database Collation", NAMEOID},
    };
    return ConstructTupleDesc(tupleDescEntries, sizeof(tupleDescEntries) / sizeof(TupleDescEntry));
}

/**
 * show triggers
 * args: schema name
 */
Datum ShowTriggers(PG_FUNCTION_ARGS)
{
    if (SRF_IS_FIRSTCALL()) {
        FuncCallContext *fctx = SRF_FIRSTCALL_INIT();
        MemoryContext oldContext = MemoryContextSwitchTo(fctx->multi_call_memory_ctx);
        fctx->tuple_desc = ShowTriggersDesc();
        char *schema = PG_GETARG_CSTRING(0);
        if (schema != NULL) {
            (void)plps_check_schema_or_table_valid(schema, NULL, FALSE);
        }
        StringInfo stringInfo = makeStringInfo();
        appendStringInfoString(stringInfo, "select CAST(t.tgname AS character varying)  as \"Trigger\",\n"
                                           "       CAST(em.text AS character varying)   as \"Event\",\n"
                                           "       CAST(c.relname AS character varying) as \"Table\",\n"
                                           "       substring(pg_catalog.pg_get_triggerdef(t.oid) from\n"
                                           "                 position('EXECUTE PROCEDURE' in substring(pg_catalog.pg_get_triggerdef(t.oid) from 48)) + 47)\n"
                                           "                 AS \"Statement\",\n"
                                           "       CASE t.tgtype & 66 WHEN 2 THEN 'BEFORE' WHEN 64 THEN 'INSTEAD OF' ELSE 'AFTER' END  AS \"Timing\",\n"
                                           "       null                   as \"Created\",\n"
                                           "       null                   as \"sql_mode\",\n"
                                           "       r.rolname            AS \"Definer\",\n"
                                           "       null                   as \"character_set_client\",\n"
                                           "       null                   as \"collation_connection\",\n"
                                           "       b.datcollate         AS \"Database Collation\"\n"
                                           "from pg_namespace n, pg_class c, (VALUES (4, 'INSERT'),\n"
                                           "    (8, 'DELETE'),\n"
                                           "    (16, 'UPDATE'), (32, 'TRUNCATE')) AS em (num, text), pg_trigger t\n"
                                           "    left join pg_roles r\n"
                                           "on t.tgowner = r.oid\n"
                                           "    left join pg_database b on b.datname = Current_database()\n"
                                           "where n.oid = c.relnamespace\n"
                                           "  AND c.oid = t.tgrelid\n"
                                           "  AND t.tgtype & em.num <> 0\n"
                                           "  AND NOT t.tgisinternal\n"
                                           "  AND (NOT pg_catalog.pg_is_other_temp_schema(n.oid))\n"
                                           "  AND (pg_catalog.pg_has_role(c.relowner\n"
                                           "    , 'USAGE')\n"
                                           "   OR pg_catalog.has_table_privilege(c.oid\n"
                                           "    , 'INSERT, UPDATE, DELETE, TRUNCATE, REFERENCES, TRIGGER')\n"
                                           "   OR pg_catalog.has_any_column_privilege(c.oid\n"
                                           "    , 'INSERT, UPDATE, REFERENCES'))");
        if (schema != NULL && strlen(schema) != 0) {
            appendStringInfo(stringInfo, " AND n.nspname = '%s'", schema);
        }
        CallSPIAndCheck(stringInfo->data);
        DestroyStringInfo(stringInfo);
        fctx->max_calls = SPI_processed;
        fctx->call_cntr = 0;
        MemoryContextSwitchTo(oldContext);
    }

    FuncCallContext *fctx = SRF_PERCALL_SETUP();

    if (fctx->call_cntr < fctx->max_calls) {
        HeapTuple tuple = SPI_tuptable->vals[fctx->call_cntr];
        const static int TRIGGER_ATTR_NUM = 11;
        Datum values[TRIGGER_ATTR_NUM];
        bool nulls[TRIGGER_ATTR_NUM];
        for (int columnIndex = 0; columnIndex < TRIGGER_ATTR_NUM; columnIndex++) {
            bool isNull;
            values[columnIndex] = SPI_getbinval(tuple, SPI_tuptable->tupdesc, columnIndex + 1, &isNull);
            nulls[columnIndex] = isNull;
        }

        HeapTuple returnTuple = heap_form_tuple(fctx->tuple_desc, values, nulls);
        SRF_RETURN_NEXT(fctx, HeapTupleGetDatum(returnTuple));
    } else {
        SPI_STACK_LOG("finish", NULL, NULL);
        if (SPI_finish() != SPI_OK_FINISH) {
            ereport(ERROR, (errcode(ERRCODE_SPI_FINISH_FAILURE), errmsg("SPI_finish failed")));
        }
        SRF_RETURN_DONE(fctx);
    }
}

/**
 * make selectstmt show trigger
 * @param args args of show_function_status
 * @param likeWhereOpt filter contiiton
 * @param isLikeExpr true for like clause else where clause
 * @return selectstmt
 */
SelectStmt *makeShowTriggersQuery(List *args, Node *likeWhereOpt, bool isLikeExpr)
{
    SelectStmt *selectStmt = MakeCommonQuery("show_triggers", args);
    /* set where clause */
    if (isLikeExpr) {
        ColumnRef *columnRef = makeNode(ColumnRef);
        columnRef->fields = list_make1(makeString("Trigger"));
        likeWhereOpt = (Node *)makeSimpleA_Expr(AEXPR_OP, "~~", (Node *)columnRef, likeWhereOpt, 0);
    }
    selectStmt->whereClause = likeWhereOpt;

    return selectStmt;
}