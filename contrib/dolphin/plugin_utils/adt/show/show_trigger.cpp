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
        appendStringInfoString(stringInfo, "select i.trigger_name       as \"Trigger\","
                                           "       i.event_manipulation as \"Event\","
                                           "       i.event_object_table as \"Table\","
                                           "       i.action_statement   AS \"Statement\","
                                           "       i.action_timing      AS \"Timing\","
                                           "       ''                   as \"Created\","
                                           "       ''                   as \"sql_mode\","
                                           "       r.rolname            AS \"Definer\","
                                           "       ''                   as \"character_set_client\","
                                           "       ''                   as \"collation_connection\","
                                           "       b.datcollate         AS \"Database Collation\""
                                           "from information_schema.triggers i"
                                           "         left join"
                                           "     pg_trigger t on i.trigger_name = t.tgname"
                                           "         left join pg_roles r"
                                           "                   on t.tgowner = r.oid"
                                           "         left join pg_database b on b.datname = Current_database()");
        if (schema != NULL && strlen(schema) != 0) {
            appendStringInfo(stringInfo, " AND i.trigger_schema = '%s'", schema);
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