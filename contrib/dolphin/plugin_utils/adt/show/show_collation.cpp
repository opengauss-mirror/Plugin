/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 *
 * openGauss is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *
 *          http://license.coscl.org.cn/MulanPSL2
 *
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 * -------------------------------------------------------------------------
 *
 * show_collation.cpp
 *    SHOW COLLATION [LIKE 'pattern' | WHERE expr]
 *
 * IDENTIFICATION
 *    contrib/dolphin/plugin_utils/adt/show/show_collation.cpp
 *
 * -------------------------------------------------------------------------
 */
#include "plugin_utils/show_common.h"
#include "funcapi.h"

PG_FUNCTION_INFO_V1_PUBLIC(ShowCollation);
extern "C" DLL_PUBLIC Datum ShowCollation(PG_FUNCTION_ARGS);

/**
 * tuple desc of show function status
 * @return tuple desc
 */
TupleDesc ShowCollationTupleDesc()
{
    TupleDescEntry tupleDescEntries[] = {
        {"collation", NAMEOID}, {"charset", NAMEOID},  {"id", OIDOID},
        {"default", TEXTOID},   {"compiled", TEXTOID}, {"sortlen", INT4OID},
    };
    return ConstructTupleDesc(tupleDescEntries, sizeof(tupleDescEntries) / sizeof(TupleDescEntry));
}

/**
 * first call of show function status
 */
void ShowCollationFirstCall(PG_FUNCTION_ARGS)
{
    FuncCallContext *fctx = SRF_FIRSTCALL_INIT();
    MemoryContext oldContext = MemoryContextSwitchTo(fctx->multi_call_memory_ctx);
    fctx->tuple_desc = ShowCollationTupleDesc();
    char *query = "select collname as collation, "
                  "       lower(pg_encoding_to_char(collencoding))::name as charset, oid as id, "
                  "       collisdef::text       as default, "
                  "       'Yes'                 as compiled, "
                  "       NULL                  as sortlen "
                  "from pg_collation "
                  "where charset IN (select charset from show_character_set() where server = true) "
                  "order by collname;";
    CallSPIAndCheck(query);
    fctx->max_calls = SPI_processed;
    fctx->call_cntr = 0;
    MemoryContextSwitchTo(oldContext);
}

Datum ShowCollation(PG_FUNCTION_ARGS)
{
    /* First Call Init */
    if (SRF_IS_FIRSTCALL()) {
        ShowCollationFirstCall(fcinfo);
    }

    FuncCallContext *fctx = SRF_PERCALL_SETUP();

    if (fctx->call_cntr < fctx->max_calls) {
        HeapTuple tuple = SPI_tuptable->vals[fctx->call_cntr];
        static const int collationsNAttrs = fctx->tuple_desc->natts;
        Datum values[collationsNAttrs];
        bool nulls[collationsNAttrs];
        for (int columnIndex = 0; columnIndex < collationsNAttrs; columnIndex++) {
            bool isNull = false;
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

SelectStmt *MakeShowCollationQuery(List *args, Node *likeWhereOpt, bool isLikeExpr)
{
    SelectStmt *selectStmt = MakeCommonQuery("show_collation", "collation", args, likeWhereOpt, isLikeExpr);
    ColumnRef *columnRef = makeNode(ColumnRef);
    columnRef->fields = list_make1(makeString("collation"));
    SortBy *sortBy = makeNode(SortBy);
    sortBy->node = (Node *)columnRef;
    selectStmt->sortClause = list_make1(sortBy);
    return selectStmt;
}