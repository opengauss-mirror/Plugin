#include "plugin_utils/show_common.h"
#include "funcapi.h"
#include "nodes/makefuncs.h"


/**
 * search oid of role name
 * @param role role name of user
 * @return oid of user
 * @throw ereport if role name if invalid
 */
Oid SearchRoleIdFromAuthId(char *role)
{
    HeapTuple tuple = SearchSysCache1(AUTHNAME, PointerGetDatum(role));
    if (!HeapTupleIsValid(tuple)) {
        /* ereport for invalid role name */
        ereport(ERROR, (errcode(ERRCODE_UNDEFINED_OBJECT), errmsg("role \"%s\" does not exist", role)));
    }

    Oid result = HeapTupleGetOid(tuple);
    ReleaseSysCache(tuple);
    return result;
}

/**
 * common function for construct tuple desc of function
 * @param tupleDescEntries columns details
 * @param size columns size
 * @return tuple desc
 */
TupleDesc ConstructTupleDesc(TupleDescEntry *tupleDescEntries, int size)
{
    TupleDesc tupleDesc = CreateTemplateTupleDesc(size, false);
    for (int i = 0; i < size; i++) {
        TupleDescEntry entry = tupleDescEntries[i];
        AttrNumber attrNumber = (AttrNumber)(i + 1);
        TupleDescInitEntry(tupleDesc, attrNumber, entry.attributeName, entry.oidTypeId, entry.typmod, entry.attdim);
    }
    return BlessTupleDesc(tupleDesc);
}

/**
 * get oid from pg_namespace
 * @return oid of information_schema
 */
Oid GetInformationSchemaOid()
{
    return get_namespace_oid("information_schema", false);
}

/**
 * you need to call SPI_finish by yourself
 * @param query querySQL
 */
void CallSPIAndCheck(char *query)
{
    SPI_STACK_LOG("connect", NULL, NULL);
    if (SPI_connect() != SPI_OK_CONNECT) {
        ereport(ERROR, (errcode(ERRCODE_SPI_CONNECTION_FAILURE), errmsg("SPI_connect failed")));
    }

    if (SPI_execute(query, true, 0) != SPI_OK_SELECT) {
        ereport(ERROR, (errcode(ERRCODE_DATA_EXCEPTION), errmsg("failed to exec query SPIApiForShow")));
    }
}

/**
 * make sure you called SPI_connect already
 * @return sql_mode value
 */
char *GetSqlMode()
{
    if (SPI_execute("show dolphin.sql_mode", false, 0) != SPI_OK_UTILITY || SPI_processed != 1) {
        ereport(ERROR, (errcode(ERRCODE_DATA_EXCEPTION), errmsg("failed to exec query 'show dolphin.sql_mode'")));
    }

    return SPI_getvalue(SPI_tuptable->vals[0], SPI_tuptable->tupdesc, 1);
}

/**
 * make sure you called SPI_connect already
 * @param sql_mode value
 */
void SetSqlMode(const char* sqlMode, bool raiseError)
{
    char query[SCAN_SQL_LEN];
    int rc = sprintf_s(query, SCAN_SQL_LEN, "set dolphin.sql_mode = '%s'", sqlMode);
    securec_check_ss(rc, "", "");

    if (SPI_execute(query, false, 0) != SPI_OK_UTILITY) {
        ereport(raiseError ? ERROR : WARNING,
            (errcode(ERRCODE_DATA_EXCEPTION), errmsg("failed to exec query '%s'", query)));
    }
}

/**
 * common function select
 * @param functionName function name  
 * @param args args pf function
 * @return selectstmt
 */
SelectStmt *MakeCommonQuery(char *functionName, List *args)
{
    SelectStmt *selectStmt = makeNode(SelectStmt);
    /* Make Target List */
    ColumnRef *columnRef = makeNode(ColumnRef);
    columnRef->fields = list_make1(makeNode(A_Star));
    ResTarget *resTarget = makeNode(ResTarget);
    resTarget->val = (Node *)columnRef;
    selectStmt->targetList = list_make1(resTarget);

    /* Make RageFunction Call */
    FuncCall *funcCall = makeNode(FuncCall);
    funcCall->funcname = list_make1(makeString(functionName));
    funcCall->args = args;
    RangeFunction *rangeFunction = makeNode(RangeFunction);
    rangeFunction->funccallnode = (Node *)funcCall;
    selectStmt->fromClause = list_make1(rangeFunction);

    return selectStmt;
}

SelectStmt *MakeCommonQuery(char *functionName, char *filterName, List *args, Node *likeWhereOpt, bool isLikeExpr)
{
    SelectStmt *selectStmt = MakeCommonQuery(functionName, args);
    /* set where clause */
    if (isLikeExpr) {
        ColumnRef *columnRef = makeNode(ColumnRef);
        columnRef->fields = list_make1(makeString(filterName));
        likeWhereOpt = (Node *)makeSimpleA_Expr(AEXPR_OP, "~~", (Node *)columnRef, likeWhereOpt, -1);
    }
    selectStmt->whereClause = likeWhereOpt;

    return selectStmt;
}