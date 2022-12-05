#ifndef PLUGIN_SHOW_COMMON_H
#define PLUGIN_SHOW_COMMON_H

#include "postgres.h"
#include "plugin_postgres.h"
#include "access/htup.h"
#include "plugin_nodes/parsenodes_common.h"
#include "access/tableam.h"
typedef bool (*TupleFilter)(HeapTuple heapTuple);

/* scan SQL buffer */
const static size_t SCAN_SQL_LEN = 1024;
const static size_t MAX_OBJECT_NAME_LEN = 128;

typedef struct {
    Oid roleOid;
    char *roleName;
} RoleInShow;

typedef struct {
    Datum aclDatum;
    int aclIndex;
    char scanSQL[SCAN_SQL_LEN];
} AclSanStatus;

typedef struct {
    const char *attributeName;
    Oid oidTypeId;
    int32 typmod = -1;
    int attdim = 0;
} TupleDescEntry;

typedef struct {
    Relation relation;
    TableScanDesc scanDesc;
    int relationIndex;
} RelationScan;

typedef struct {
    RoleInShow role;
    RelationScan relationScan;
    char currentObjectName[MAX_OBJECT_NAME_LEN];
    char tableNameUsedForColumn[MAX_OBJECT_NAME_LEN];
    AclSanStatus aclStatus;
} ShowGrantState;

bool PgClassFilter(HeapTuple heapTuple);
void CallSPIAndCheck(char *query);
char *GetSqlMode();
void SetSqlMode(const char *sqlMode);
bool PgProcProcedureFilter(HeapTuple heapTuple);
bool PgProcFunctionFilter(HeapTuple heapTuple);
bool PgAttributeFilter(HeapTuple heapTuple);

bool PgTypeFilter(HeapTuple heapTuple);

bool PgNamespaceFilter(HeapTuple heapTuple);
extern bool DefaultFilter(HeapTuple tuple);
Oid GetInformationSchemaOid();
TupleDesc ConstructTupleDesc(TupleDescEntry *tupleDescEntries, int size);
Oid SearchRoleIdFromAuthId(char *role);
SelectStmt *MakeCommonQuery(char *functionName, List *args);
SelectStmt *MakeCommonQuery(char *functionName, char *filterName, List *args, Node *likeWhereOpt, bool isLikeExpr);
typedef struct {
    Relation relation;
    SysScanDesc scanDesc;
} ScanStatusInFunction;

#define INVALID_ATTRIBUTE_ID -1

typedef struct {
    Oid relationOid;
    SysCacheIdentifier sysCacheId;
    int nameAttributeId;
    int aclAttributeId;
    char *grantType;
    TupleFilter tupleFilter = DefaultFilter;
} GrantRelation;
#endif  // PLUGIN_SHOW_COMMON_H
