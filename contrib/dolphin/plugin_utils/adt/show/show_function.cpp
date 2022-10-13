#include "plugin_utils/show_common.h"
#include "funcapi.h"

PG_FUNCTION_INFO_V1_PUBLIC(ShowFunctionStatus);
extern "C" DLL_PUBLIC Datum ShowFunctionStatus(PG_FUNCTION_ARGS);

/*
 * before functionCall, you need to change context by your self
 */
/**
 * tuple desc of show function status
 * @return tuple desc
 */
TupleDesc ShowFunctionTupleDesc()
{
    TupleDescEntry tupleDescEntries[] = {
        {"Db", NAMEOID},
        {"Name", NAMEOID},
        {"Type", TEXTOID},
        {"Definer", NAMEOID},
        {"Modified", TIMESTAMPTZOID},
        {"Created", TIMESTAMPTZOID},
        {"Security_type", TEXTOID},
        {"Comment", TEXTOID},
        {"character_set_client", TEXTOID},
        {"collation_connection", TEXTOID},
        {"Database Collation", NAMEOID},
    };
    return ConstructTupleDesc(tupleDescEntries, sizeof(tupleDescEntries) / sizeof(TupleDescEntry));
}

/**
 * first call of show function status
 */
void ShowFunctionStatusFirstCall(PG_FUNCTION_ARGS)
{
    FuncCallContext *fctx = SRF_FIRSTCALL_INIT();
    MemoryContext oldContext = MemoryContextSwitchTo(fctx->multi_call_memory_ctx);
    fctx->tuple_desc = ShowFunctionTupleDesc();
    BpChar *searchType = PG_GETARG_BPCHAR_P(0);
    char query[1024];
    int ret = sprintf_s(query, sizeof(query),
                        "SELECT "
                        " n.nspname AS \"Db\", "
                        " p.proname AS \"Name\", "
                        " CASE WHEN p.prokind = 'f' THEN 'FUNCTION' ELSE 'PROCEDURE' END AS \"Type\", "
                        " r.rolname AS \"Definer\", "
                        " o.mtime AS \"Modified\", "
                        " O.ctime AS \"Created\", "
                        " CASE WHEN p.prosecdef THEN 'DEFINER' ELSE 'INVOKER' END AS \"Security_type\", "
                        " d.description AS \"Comment\", "
                        " null AS \"character_set_client\", "
                        " null AS \"collation_connection\", "
                        " b.datcollate AS \"Database Collation\"  "
                        "FROM "
                        " pg_namespace n, "
                        " pg_database b, "
                        " pg_proc p "
                        " LEFT JOIN pg_description d ON d.objoid = p.oid "
                        " LEFT JOIN pg_object o ON o.object_oid = p.oid "
                        " LEFT JOIN pg_roles r ON p.proowner = r.oid  "
                        "WHERE "
                        " n.oid = p.pronamespace  "
                        " AND b.datname = Current_database () "
                        "AND "
                        " p.prokind = '%c'",
                        VARDATA_ANY(searchType)[0]);
    securec_check_ss(ret, "\0", "\0");
    CallSPIAndCheck(query);
    fctx->max_calls = SPI_processed;
    fctx->call_cntr = 0;
    MemoryContextSwitchTo(oldContext);
}

Datum ShowFunctionStatus(PG_FUNCTION_ARGS)
{
    /* First Call Init */
    if (SRF_IS_FIRSTCALL()) {
        ShowFunctionStatusFirstCall(fcinfo);
    }

    FuncCallContext *fctx = SRF_PERCALL_SETUP();

    if (fctx->call_cntr < fctx->max_calls) {
        HeapTuple tuple = SPI_tuptable->vals[fctx->call_cntr];
        static const int FUNCTION_STATUS_ATTR_NUM = fctx->tuple_desc->natts;
        Datum values[FUNCTION_STATUS_ATTR_NUM];
        bool nulls[FUNCTION_STATUS_ATTR_NUM];
        for (int columnIndex = 0; columnIndex < FUNCTION_STATUS_ATTR_NUM; columnIndex++) {
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
 * make selectstmt show function status
 * @param args args of show_function_status
 * @param likeWhereOpt filter contiiton
 * @param isLikeExpr true for like clause else where clause
 * @return selectstmt
 */
SelectStmt *MakeShowFuncProQuery(List *args, Node *likeWhereOpt, bool isLikeExpr)
{
    return MakeCommonQuery("show_function_status", "Name", args, likeWhereOpt, isLikeExpr);
}