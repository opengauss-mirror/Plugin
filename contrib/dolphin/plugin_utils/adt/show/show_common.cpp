#include "plugin_utils/show_common.h"
#include "funcapi.h"

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
    TupleDesc tupleDesc = CreateTemplateTupleDesc(size, false, TAM_HEAP);
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