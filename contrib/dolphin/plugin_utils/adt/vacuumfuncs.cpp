#include "postgres.h"
#include "plugin_postgres.h"
#include "nodes/parsenodes.h"
#include "nodes/execnodes.h"
#include "nodes/primnodes.h"
#include "utils/builtins.h"

PG_FUNCTION_INFO_V1_PUBLIC(analyze_tables);
extern "C" DLL_PUBLIC Datum analyze_tables(PG_FUNCTION_ARGS);

extern void DoVacuumMppTable(VacuumStmt* stmt, const char* queryString, bool isTopLevel, bool sentToRemote);

#define ANALYZE_TABLES_COLS 4

static void prepare_rsinfo_tupledesc(ReturnSetInfo *rsinfo, TupleDesc& tupdesc)
{
    int idx = 0;

    if (rsinfo == NULL || !IsA(rsinfo, ReturnSetInfo)) {
        ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
            errmsg("set-valued function called in context that cannot accept a set")));
    }
    if (!(rsinfo->allowedModes & SFRM_Materialize)) {
        ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
            errmsg("materialize mode required, but it is not allowed in this context")));
    }

    tupdesc = CreateTemplateTupleDesc(ANALYZE_TABLES_COLS, false, TAM_HEAP);
    TupleDescInitEntry(tupdesc, (AttrNumber)++idx, "Table", TEXTOID, -1, 0);
    TupleDescInitEntry(tupdesc, (AttrNumber)++idx, "Op", TEXTOID, -1, 0);
    TupleDescInitEntry(tupdesc, (AttrNumber)++idx, "Msg_type", TEXTOID, -1, 0);
    TupleDescInitEntry(tupdesc, (AttrNumber)++idx, "Msg_text", TEXTOID, -1, 0);

    rsinfo->returnMode = SFRM_Materialize;
    rsinfo->setDesc = tupdesc;
}

static void init_vacstmt_vacstr(VacuumStmt& vacstmt, const char* relStr, RangeVar* rangevar, StringInfo str)
{
    char *tmp = pstrdup(relStr);
    char *savePtr = NULL;
    char *p = NULL;
    char delims[] = ".";
    int rc;

    rangevar->schemaname = (char*)palloc0(NAMEDATALEN);
    rangevar->relname = (char*)palloc0(NAMEDATALEN);

    p = strtok_r(tmp, delims, &savePtr);
    rc = snprintf_s(rangevar->schemaname, NAMEDATALEN, NAMEDATALEN - 1, "%s", p);
    securec_check_ss(rc, "\0", "\0");

    p = strtok_r(NULL, delims, &savePtr);
    rc = snprintf_s(rangevar->relname, NAMEDATALEN, NAMEDATALEN - 1, "%s", p);
    securec_check_ss(rc, "\0", "\0");

    vacstmt.type = T_VacuumStmt;
    vacstmt.options = (unsigned int)vacstmt.options | VACOPT_ANALYZE;
    vacstmt.freeze_min_age = -1;
    vacstmt.freeze_table_age = -1;
    vacstmt.relation = rangevar;

    initStringInfo(str);
    appendStringInfo(str, "ANALYZE ");
    appendStringInfo(str, "%s", relStr);

    pfree(tmp);
}

static void do_actual_analyze(VacuumStmt* vacstmt, const char* relStr, StringInfoData str,  Tuplestorestate *tupstore, TupleDesc tupdesc)
{
    Datum values[ANALYZE_TABLES_COLS] = {0};
    bool nulls[ANALYZE_TABLES_COLS] = {false};
    int idx = 0;
    int rc;

    values[idx++] = CStringGetTextDatum(pstrdup(relStr));
    values[idx++] = CStringGetTextDatum("analyze");

    MemoryContext currentContext = CurrentMemoryContext;

    PG_TRY();
    {
        DoVacuumMppTable(vacstmt, str.data, true, false);

        values[idx++] = CStringGetTextDatum("status");
        values[idx++] = CStringGetTextDatum("OK");

        tuplestore_putvalues(tupstore, tupdesc, values, nulls);
    }
    PG_CATCH();
    {
        MemoryContextSwitchTo(currentContext);

        ErrorData* edata = CopyErrorData();

        values[idx++] = CStringGetTextDatum("Error");
        values[idx++] = CStringGetTextDatum(pstrdup(edata->message));

        tuplestore_putvalues(tupstore, tupdesc, values, nulls);

        /* additional tuple tells the operation is failed while the Mst_type is status. */
        rc = memset_s(values, sizeof(values), 0, sizeof(values));
        securec_check(rc, "", "");
        rc = memset_s(nulls, sizeof(nulls), 0, sizeof(nulls));
        securec_check(rc, "", "");

        idx = 0;
        values[idx++] = CStringGetTextDatum(pstrdup(relStr));
        values[idx++] = CStringGetTextDatum("analyze");
        values[idx++] = CStringGetTextDatum("status");
        values[idx++] = CStringGetTextDatum("Operation failed");

        tuplestore_putvalues(tupstore, tupdesc, values, nulls);

        FlushErrorState();
        FreeErrorData(edata);
    }
    PG_END_TRY();
}

Datum analyze_tables(PG_FUNCTION_ARGS)
{
    ReturnSetInfo *rsinfo = (ReturnSetInfo *)fcinfo->resultinfo;
    TupleDesc tupdesc;
    Tuplestorestate *tupstore = NULL;
    MemoryContext per_query_ctx;
    MemoryContext oldcontext;
    ArrayType* array = PG_GETARG_ARRAYTYPE_P(0);
    Datum* elem_values = NULL;
    int elem_num;
    int rc;
    bool tmp_enable_autoanalyze = u_sess->attr.attr_sql.enable_autoanalyze;
    /* forbid auto-analyze inside vacuum/analyze */
    u_sess->attr.attr_sql.enable_autoanalyze = false;

    deconstruct_array(array, CSTRINGOID, -2, false, 'c', &elem_values, NULL, &elem_num);

    prepare_rsinfo_tupledesc(rsinfo, tupdesc);
    per_query_ctx = rsinfo->econtext->ecxt_per_query_memory;
    oldcontext = MemoryContextSwitchTo(per_query_ctx);

    tupstore = tuplestore_begin_heap(true, false, u_sess->attr.attr_memory.work_mem);
    rsinfo->setResult = tupstore;
    
    MemoryContextSwitchTo(oldcontext);

    for (int i = 0; i < elem_num; i++) {
        const char* relStr = DatumGetCString(elem_values[i]);
        VacuumStmt vacstmt;
        RangeVar rangevar;
        StringInfoData str;
        
        rc = memset_s(&vacstmt, sizeof(vacstmt), 0, sizeof(vacstmt));
        securec_check(rc, "", "");
        rc = memset_s(&rangevar, sizeof(rangevar), 0, sizeof(rangevar));
        securec_check(rc, "", "");

        init_vacstmt_vacstr(vacstmt, relStr, &rangevar, &str);
        do_actual_analyze(&vacstmt, relStr, str, tupstore, tupdesc);

        pfree_ext(rangevar.relname);
        pfree_ext(rangevar.schemaname);
        FreeStringInfo(&str);
    }

    pfree_ext(elem_values);
    tuplestore_donestoring(tupstore);

    u_sess->attr.attr_sql.enable_autoanalyze = tmp_enable_autoanalyze;
    PG_RETURN_VOID();
}