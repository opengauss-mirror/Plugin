#include "plugin_utils/show_common.h"

#include "utils/relcache.h"

#include "catalog/pg_class.h"
#include "catalog/pg_foreign_server.h"
#include "catalog/pg_foreign_data_wrapper.h"
#include "catalog/gs_package.h"
#include "catalog/pg_proc.h"
#include "catalog/gs_db_privilege.h"
#include "catalog/pg_language.h"
#include "catalog/pg_authid.h"
#include "catalog/pg_database.h"
#include "catalog/pg_extension_data_source.h"
#include "catalog/gs_client_global_keys.h"
#include "catalog/gs_column_keys.h"
#include "utils/acl.h"
#include "libpq/auth.h"
#include "funcapi.h"

PG_FUNCTION_INFO_V1_PUBLIC(ShowAnyPrivileges);
extern "C" DLL_PUBLIC Datum ShowAnyPrivileges(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1_PUBLIC(ShowRolePrivilege);
extern "C" DLL_PUBLIC Datum ShowRolePrivilege(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1_PUBLIC(ShowObjectGrants);
extern "C" DLL_PUBLIC Datum ShowObjectGrants(PG_FUNCTION_ARGS);

static bool IsUserSchema(Oid namespaceId);
const static GrantRelation GRANT_RELATIONS[] = {
    {RelationRelationId, RELOID, Anum_pg_class_relname, Anum_pg_class_relacl, "TABLE", PgClassFilter},
    {ForeignServerRelationId, FOREIGNSERVEROID, Anum_pg_foreign_server_srvname, Anum_pg_foreign_server_srvacl,
     "FOREIGN SERVER"},
    {ForeignDataWrapperRelationId, FOREIGNDATAWRAPPEROID, Anum_pg_foreign_data_wrapper_fdwname,
     Anum_pg_foreign_data_wrapper_fdwacl, "FOREIGN DATA WRAPPER"},
    {PackageRelationId, PACKAGEOID, Anum_gs_package_pkgname, Anum_gs_package_pkgacl, "PACKAGE"},
    {ProcedureRelationId, PROCOID, Anum_pg_proc_proname, Anum_pg_proc_proacl, "PROCEDURE", PgProcProcedureFilter},
    {ProcedureRelationId, PROCOID, Anum_pg_proc_proname, Anum_pg_proc_proacl, "FUNCTION", PgProcFunctionFilter},
    {LanguageRelationId, LANGOID, Anum_pg_language_lanname, Anum_pg_language_lanacl, "LANGUAGE"},
    {TypeRelationId, TYPEOID, Anum_pg_type_typname, Anum_pg_type_typacl, "TYPE", PgTypeFilter},
    {PgDirectoryRelationId, DIRECTORYOID, Anum_pg_directory_directory_name, Anum_pg_directory_directory_acl,
     "DIRECTORY"},
    {NamespaceRelationId, NAMESPACEOID, Anum_pg_namespace_nspname, Anum_pg_namespace_nspacl, "SCHEMA",
     PgNamespaceFilter},
    {DatabaseRelationId, DATABASEOID, Anum_pg_database_datname, Anum_pg_database_datacl, "DATABASE"},
    {DataSourceRelationId, DATASOURCEOID, Anum_pg_extension_data_source_srcname, Anum_pg_extension_data_source_srcacl,
     "DATA SOURCE"},
    {ClientLogicGlobalSettingsId, GLOBALSETTINGOID, Anum_gs_client_global_keys_global_key_name,
     Anum_gs_client_global_keys_key_acl, "CLIENT_MASTER_KEY"},
    {ClientLogicColumnSettingsId, COLUMNSETTINGOID, Anum_gs_client_global_keys_global_key_name,
     Anum_gs_client_global_keys_key_acl, "COLUMN_ENCRYPTION_KEY"},
};

bool PgTypeFilter(HeapTuple heapTuple)
{
    SysCacheIdentifier sysCacheId = GRANT_RELATIONS[7].sysCacheId;
    bool isNull = false;
    Datum namespaceDatum = SysCacheGetAttr(sysCacheId, heapTuple, Anum_pg_type_typnamespace, &isNull);
    Assert(!isNull);
    Oid namespaceId = DatumGetObjectId(namespaceDatum);
    return IsUserSchema(namespaceId);
}

bool PgNamespaceFilter(HeapTuple heapTuple)
{
    Oid oid = HeapTupleGetOid(heapTuple);
    return IsUserSchema(oid);
}

bool PgClassFilter(HeapTuple heapTuple)
{
    bool isNull = false;
    SysCacheIdentifier sysCacheId = GRANT_RELATIONS[0].sysCacheId;
    Datum namespaceDatum = SysCacheGetAttr(sysCacheId, heapTuple, Anum_pg_class_relnamespace, &isNull);
    Assert(!isNull);
    Oid namespaceId = DatumGetObjectId(namespaceDatum);
    if (!IsUserSchema(namespaceId)) {
        return false;
    }
    Datum relKindDatum = SysCacheGetAttr(sysCacheId, heapTuple, Anum_pg_class_relkind, &isNull);
    Assert(!isNull);
    return DatumGetChar(relKindDatum) != RELKIND_INDEX;
}

bool PgProcFilter(HeapTuple heapTuple, char procType)
{
    bool isNull = false;
    SysCacheIdentifier sysCacheId = GRANT_RELATIONS[4].sysCacheId;
    Datum namespaceDatum = SysCacheGetAttr(sysCacheId, heapTuple, Anum_pg_proc_pronamespace, &isNull);
    Assert(!isNull);
    Oid namespaceId = DatumGetObjectId(namespaceDatum);
    if (!IsUserSchema(namespaceId)) {
        return false;
    }
    Datum proKindDatum = SysCacheGetAttr(sysCacheId, heapTuple, Anum_pg_proc_prokind, &isNull);
    Assert(!isNull);

    return DatumGetChar(proKindDatum) == procType;
}

bool PgProcProcedureFilter(HeapTuple heapTuple)
{
    return PgProcFilter(heapTuple, PROKIND_PROCEDURE);
}

bool PgProcFunctionFilter(HeapTuple heapTuple)
{
    return PgProcFilter(heapTuple, PROKIND_FUNCTION);
}

/**
 * defaultFilter, return true always
 * @param tuple tuple
 * @return return true always
 */
bool DefaultFilter(HeapTuple tuple)
{
    return true;
}

static const priv_map ACl_OPTION_CHARS[] = {
    {"INSERT", ACL_INSERT},   {"SELECT", ACL_SELECT},     {"UPDATE", ACL_UPDATE},
    {"DELETE", ACL_DELETE},   {"TRUNCATE", ACL_TRUNCATE}, {"REFERENCES", ACL_REFERENCES},
    {"TRIGGER", ACL_TRIGGER}, {"EXECUTE", ACL_EXECUTE},   {"USAGE", ACL_USAGE},
    {"CREATE", ACL_CREATE},   {"TEMP", ACL_CREATE_TEMP},  {"TEMPORARY", ACL_CREATE_TEMP},
    {"CONNECT", ACL_CONNECT}, {"COMPUTE", ACL_COMPUTE},   {"READ", ACL_READ},
    {"WRITE", ACL_WRITE},
};

static const priv_map DDL_RIGHT[] = {
    {"ALTER", (1 << 0)}, {"DROP", (1 << 1)}, {"COMMENT", (1 << 2)}, {"INDEX", (1 << 3)}, {"VACUUM", (1 << 4)},
};

char *ConstructObjectGrantSQL(AclItem *aclItem, char *objectType, ShowGrantState *grantStatus)
{
    uint32 privs = ACLITEM_GET_PRIVS(*aclItem);
    uint32 grantOption = ACLITEM_GET_GOPTIONS(*aclItem);

    StringInfo noGrantOptionStr = makeStringInfo();
    StringInfo grantOptionStr = makeStringInfo();
    char *object = DatumGetCString(grantStatus->currentRelName);
    const priv_map *privMaps = ACLMODE_FOR_DDL(privs) ? DDL_RIGHT : ACl_OPTION_CHARS;
    size_t ddlLength = sizeof(DDL_RIGHT) / sizeof(priv_map);
    size_t aclOptionLength = sizeof(ACl_OPTION_CHARS) / sizeof(priv_map);
    size_t length = ACLMODE_FOR_DDL(privs) ? ddlLength : aclOptionLength;
    for (size_t i = 0; i < length; i++) {
        priv_map privMap = privMaps[i];
        AclMode value = privMap.value;
        if ((privs & value) != 0) {
            if ((grantOption & value) != 0) {
                appendStringInfo(grantOptionStr, ", %s", privMap.name);
            } else {
                appendStringInfo(noGrantOptionStr, ", %s", privMap.name);
            }
        }
    }

    Assert(grantOptionStr->len != 0 || noGrantOptionStr != 0);
    StringInfo noGrantResult = makeStringInfo();
    StringInfo grantResult = makeStringInfo();
    if (noGrantOptionStr->len != 0) {
        appendStringInfo(noGrantResult, "GRANT %s ON %s %s TO %s", noGrantOptionStr->data + 2, objectType, object,
                         grantStatus->role.roleName);
    }
    if (grantOptionStr->len != 0) {
        appendStringInfo(grantResult, "GRANT %s ON %s %s TO %s WITH ADMIN OPTION", grantOptionStr->data + 2, objectType,
                         object, grantStatus->role.roleName);
    }

    DestroyStringInfo(noGrantOptionStr);
    DestroyStringInfo(grantOptionStr);
    if (noGrantResult->len != 0 && grantResult->len != 0) {
        int rc = memcpy_s(grantStatus->aclStatus.scanSQL, SCAN_SQL_LEN, grantResult->data, grantResult->len);
        securec_check(rc, "\0", "\0");
        DestroyStringInfo(grantResult);
        return noGrantResult->data;
    } else if (noGrantResult->len != 0) {
        DestroyStringInfo(grantResult);
        return noGrantResult->data;
    } else {
        DestroyStringInfo(noGrantResult);
        return grantResult->data;
    }
}

/**
 * return true if namespaceId is user-schema
 * @param namespaceId namespaceId
 * @return return true if namespaceId is user-schema
 */
static bool IsUserSchema(Oid namespaceId)
{
    /* GetInformationSchemaOid will only be called once, Concurrently function-invoke is fine */
    static Oid informationSchemaOid = GetInformationSchemaOid();
    return namespaceId != informationSchemaOid && namespaceId != PG_TOAST_NAMESPACE &&
           namespaceId != PG_PKG_SERVICE_NAMESPACE && namespaceId != PG_CATALOG_NAMESPACE &&
           namespaceId != CSTORE_NAMESPACE && namespaceId != PG_SNAPSHOT_NAMESPACE &&
           namespaceId != PG_TOAST_NAMESPACE && namespaceId != PG_DBEPERF_NAMESPACE &&
           namespaceId != PG_BLOCKCHAIN_NAMESPACE && namespaceId != PG_SQLADVISOR_NAMESPACE &&
           namespaceId != DBE_PLDEVELOPER_NAMESPACE && namespaceId != PG_PLDEBUG_NAMESPACE &&
           namespaceId != PG_DB4AI_NAMESPACE;
}

HeapTuple ObjectGrantTuple(FuncCallContext *fctx, ShowGrantState *grantStatus, char *type, AclItem *aclItem, char *sql)
{
    Datum values[6];
    values[0] = grantStatus->currentRelName;
    values[1] = ObjectIdGetDatum(aclItem->ai_grantee);
    values[2] = ObjectIdGetDatum(aclItem->ai_grantor);
    values[3] = UInt32GetDatum(aclItem->ai_privs);
    values[4] = CStringGetTextDatum(type);
    values[5] = CStringGetTextDatum(sql);
    /* Build and return the result tuple. */
    bool nulls[6];
    securec_check(memset_s(nulls, sizeof(nulls), 0, sizeof(nulls)), "\0", "\0");
    HeapTuple tuple = heap_form_tuple(fctx->tuple_desc, values, nulls);
    return tuple;
}

/**
 * acl array loop for grant sql
 * @param fctx fctx
 * @param type kind of table
 * @return tuple if success else null
 */
HeapTuple LoopAcl(FuncCallContext *fctx, char *type)
{
    ShowGrantState *grantStatus = (ShowGrantState *)fctx->user_fctx;
    Datum aclDatum = grantStatus->aclStatus.aclDatum;
    Acl *acl = DatumGetAclP(aclDatum);
    AclItem *aclItem = ACL_DAT(acl);

    if (strlen(grantStatus->aclStatus.scanSQL) != 0) {
        HeapTuple tuple = ObjectGrantTuple(fctx, grantStatus, type, &aclItem[grantStatus->aclStatus.aclIndex],
                                           grantStatus->aclStatus.scanSQL);
        grantStatus->aclStatus.scanSQL[0] = '\0';
        grantStatus->aclStatus.aclIndex++;
        return tuple;
    }

    int aclSize = ACL_NUM(acl);
    while (grantStatus->aclStatus.aclIndex < aclSize &&
           aclItem[grantStatus->aclStatus.aclIndex].ai_grantee != grantStatus->role.roleOid) {
        grantStatus->aclStatus.aclIndex++;
    }
    if (grantStatus->aclStatus.aclIndex >= aclSize ||
        aclItem[grantStatus->aclStatus.aclIndex].ai_grantee != grantStatus->role.roleOid) {
        grantStatus->aclStatus.aclIndex = -1;
        return NULL;
    }
    HeapTuple tuple = ObjectGrantTuple(
        fctx, grantStatus, type, &aclItem[grantStatus->aclStatus.aclIndex],
        ConstructObjectGrantSQL(&aclItem[grantStatus->aclStatus.aclIndex],
                                GRANT_RELATIONS[grantStatus->relationScan.relationIndex].grantType, grantStatus));
    if (grantStatus->aclStatus.aclIndex < aclSize - 1 && strlen(grantStatus->aclStatus.scanSQL) == 0) {
        grantStatus->aclStatus.aclIndex++;
    } else if (grantStatus->aclStatus.aclIndex == aclSize - 1 && strlen(grantStatus->aclStatus.scanSQL) == 0) {
        grantStatus->aclStatus.aclIndex = -1;
    }
    return tuple;
}

void InitScanState(ShowGrantState *showGrantState)
{
    GrantRelation grantRelation = GRANT_RELATIONS[showGrantState->relationScan.relationIndex];
    showGrantState->relationScan.relation = heap_open(grantRelation.relationOid, AccessShareLock);
    showGrantState->relationScan.scanDesc =
        tableam_scan_begin(showGrantState->relationScan.relation, SnapshotNow, 0, NULL);
}

void ShowGrantFunctionInit(PG_FUNCTION_ARGS)
{
    FuncCallContext *fctx = SRF_FIRSTCALL_INIT();
    MemoryContext oldContext = MemoryContextSwitchTo(fctx->multi_call_memory_ctx);
    char *roleName = text_to_cstring(PG_GETARG_TEXT_P(0));
    if (strlen(roleName) == 0) {
        roleName = GetUserNameFromId(GetUserId());
    }
    Oid oid = SearchRoleIdFromAuthId(roleName);
    ShowGrantState *showGrantState = (ShowGrantState *)palloc0(sizeof(ShowGrantState));
    showGrantState->aclStatus.aclIndex = -1;
    showGrantState->role.roleOid = oid;
    showGrantState->role.roleName = roleName;
    fctx->user_fctx = (void *)showGrantState;

    TupleDescEntry tupleDescEntries[] = {{"relname", NAMEOID}, {"grantee", OIDOID},     {"grantor", OIDOID},
                                         {"privs", INT4OID},   {"grant_type", TEXTOID}, {"grant_sql", TEXTOID}};
    fctx->tuple_desc = ConstructTupleDesc(tupleDescEntries, sizeof(tupleDescEntries) / sizeof(TupleDescEntry));
    MemoryContextSwitchTo(oldContext);
}

Datum ShowRolePrivilege(PG_FUNCTION_ARGS)
{
    if (SRF_IS_FIRSTCALL()) {
        FuncCallContext *fctx = SRF_FIRSTCALL_INIT();
        MemoryContext oldContext = MemoryContextSwitchTo(fctx->multi_call_memory_ctx);
        TupleDescEntry tupleDescEntries[] = {{"role_name", NAMEOID}, {"grant_sql", TEXTOID}};
        fctx->tuple_desc = ConstructTupleDesc(tupleDescEntries, sizeof(tupleDescEntries) / sizeof(TupleDescEntry));
        MemoryContextSwitchTo(oldContext);
    } else {
        FuncCallContext *fctx = SRF_PERCALL_SETUP();
        SRF_RETURN_DONE(fctx);
    };

    char *roleName = text_to_cstring(PG_GETARG_TEXT_P(0));
    Oid sessionUserId = GetUserId();
    if (strlen(roleName) == 0) {
        roleName = GetUserNameFromId(sessionUserId);
    }
    Datum nameDatum = DirectFunctionCall1(namein, CStringGetDatum(roleName));
    HeapTuple tuple = SearchSysCache1(AUTHNAME, nameDatum);
    if (!HeapTupleIsValid(tuple)) {
        ereport(ERROR, (errcode(ERRCODE_UNDEFINED_OBJECT), errmsg("role \"%s\" does not exist", roleName)));
    }
    FuncCallContext *fctx = SRF_PERCALL_SETUP();
    Oid userId = HeapTupleGetOid(tuple);
    bool havePrivilege =
        userId == sessionUserId || pg_class_aclcheck(AuthIdRelationId, sessionUserId, ACL_SELECT) == ACLCHECK_OK;
    if (HeapTupleGetOid(tuple) == INITIAL_USER_ID || !havePrivilege) {
        ReleaseSysCache(tuple);
        SRF_RETURN_DONE(fctx);
    }
    struct RolePrivilege {
        int attrIndex;
        char *roleSpecific;
    };

    RolePrivilege rolePrivileges[] = {
        {Anum_pg_authid_rolcreaterole, "CREATEROLE"}, {Anum_pg_authid_rolcreatedb, "CREATEDB"},
        {Anum_pg_authid_rolcanlogin, "LOGIN"},        {Anum_pg_authid_rolreplication, "REPLICATION"},
        {Anum_pg_authid_rolauditadmin, "AUDITADMIN"}, {Anum_pg_authid_rolsystemadmin, "SYSADMIN"},
        {Anum_pg_authid_rolmonitoradmin, "MONADMIN"}, {Anum_pg_authid_roloperatoradmin, "OPRADMIN"},
        {Anum_pg_authid_rolpolicyadmin, "POLADMIN"},
    };
    StringInfo stringInfo = makeStringInfo();
    appendStringInfo(stringInfo, "ALTER ROLE %s WITH", roleName);
    int len = stringInfo->len;
    for (size_t i = 0; i < sizeof(rolePrivileges) / sizeof(RolePrivilege); ++i) {
        RolePrivilege rolePrivilege = rolePrivileges[i];
        int attrIndex = rolePrivilege.attrIndex;
        char *roleSpecific = rolePrivilege.roleSpecific;
        bool isNull;
        Datum haveRole = SysCacheGetAttr(AUTHOID, tuple, attrIndex, &isNull);
        if (!isNull && DatumGetBool(haveRole)) {
            appendStringInfo(stringInfo, " %s", roleSpecific);
        }
    }
    ReleaseSysCache(tuple);
    /* have privilege changes */
    if (len != stringInfo->len) {
        Datum values[2];
        values[0] = nameDatum;
        values[1] = CStringGetTextDatum(stringInfo->data);
        bool nulls[2];
        nulls[0] = false;
        nulls[1] = false;
        HeapTuple tuple = heap_form_tuple(fctx->tuple_desc, values, nulls);
        DestroyStringInfo(stringInfo);
        SRF_RETURN_NEXT(fctx, HeapTupleGetDatum(tuple));
    }
    DestroyStringInfo(stringInfo);
    SRF_RETURN_DONE(fctx);
}

Datum ShowObjectGrants(PG_FUNCTION_ARGS)
{
    /* First Call Init */
    if (SRF_IS_FIRSTCALL()) {
        ShowGrantFunctionInit(fcinfo);
    }

    FuncCallContext *fctx = SRF_PERCALL_SETUP();
    ShowGrantState *showGrantState = (ShowGrantState *)fctx->user_fctx;

    if (showGrantState->aclStatus.aclIndex != -1) {
        HeapTuple tuple = LoopAcl(fctx, GRANT_RELATIONS[showGrantState->relationScan.relationIndex].grantType);
        if (HeapTupleIsValid(tuple)) {
            SRF_RETURN_NEXT(fctx, HeapTupleGetDatum(tuple));
        }
    }
    /* we are sure that max value are less than MAX_INT */
    while (showGrantState->relationScan.relationIndex < (int)(sizeof(GRANT_RELATIONS) / sizeof(GrantRelation))) {
        /* init relation scan status */
        if (showGrantState->relationScan.relation == NULL) {
            MemoryContext oldContext = MemoryContextSwitchTo(fctx->multi_call_memory_ctx);
            InitScanState(showGrantState);
            MemoryContextSwitchTo(oldContext);
        }

        HeapTuple tuple = NULL;
        GrantRelation grantRelation = GRANT_RELATIONS[showGrantState->relationScan.relationIndex];
        while ((tuple = (HeapTuple)tableam_scan_getnexttuple(showGrantState->relationScan.scanDesc,
                                                             ForwardScanDirection)) != NULL) {
            if (tuple != NULL && (grantRelation.tupleFilter)(tuple)) {
                bool isNull = true;
                SysCacheIdentifier cacheId = grantRelation.sysCacheId;

                /* get object name from cache */
                Datum relName = SysCacheGetAttr(cacheId, tuple, grantRelation.nameAttributeId, &isNull);
                Assert(!isNull);
                /* get object acl from cache */
                Datum aclDatum = SysCacheGetAttr(cacheId, tuple, grantRelation.aclAttributeId, &isNull);
                if (isNull || (ACL_NUM(DatumGetAclP(aclDatum)) == 0)) {
                    continue;
                }
                /* init grant option for next LoopAcl */
                showGrantState->currentRelName = relName;
                showGrantState->aclStatus.aclDatum = aclDatum;
                showGrantState->aclStatus.aclIndex = 0;
                HeapTuple returnTuple = LoopAcl(fctx, grantRelation.grantType);
                if (!HeapTupleIsValid(returnTuple)) {
                    continue;
                }
                SRF_RETURN_NEXT(fctx, HeapTupleGetDatum(returnTuple));
            }
        }
        tableam_scan_end(showGrantState->relationScan.scanDesc);
        heap_close(showGrantState->relationScan.relation, AccessShareLock);
        showGrantState->relationScan.scanDesc = NULL;
        showGrantState->relationScan.relation = NULL;
        showGrantState->relationScan.relationIndex++;
    }
    /* SRF_RETURN_DONE */
    SRF_RETURN_DONE(fctx);
}

char *ConstraintAnyPrivilege(char *name, char *privilege, bool adminOption)
{
    StringInfoData grantSQL;
    initStringInfo(&grantSQL);
    appendStringInfo(&grantSQL, "GRANT %s TO %s", privilege, name);
    if (adminOption) {
        appendStringInfoString(&grantSQL, " WITH ADMIN OPTION");
    }
    return grantSQL.data;
}

Datum ShowAnyPrivileges(PG_FUNCTION_ARGS)
{
    char *roleName = text_to_cstring(PG_GETARG_TEXT_P(0));
    Oid oid = GetUserId();
    if (strlen(roleName) != 0) {
        oid = SearchRoleIdFromAuthId(roleName);
    } else {
        roleName = GetUserNameFromId(oid);
    }
    /* First Call Init */
    if (SRF_IS_FIRSTCALL()) {
        FuncCallContext *fctx = SRF_FIRSTCALL_INIT();
        MemoryContext oldContext = MemoryContextSwitchTo(fctx->multi_call_memory_ctx);
        TupleDescEntry tupleDescEntries[] = {
            {"roleid", OIDOID},
            {"privilege_type", TEXTOID},
            {"admin_option", BOOLOID},
            {"grant_sql", TEXTOID},
        };
        fctx->tuple_desc = ConstructTupleDesc(tupleDescEntries, sizeof(tupleDescEntries) / sizeof(TupleDescEntry));
        ScanStatusInFunction *scanStatusInFunction = (ScanStatusInFunction *)palloc0(sizeof(ScanStatusInFunction));
        fctx->user_fctx = (void *)scanStatusInFunction;

        scanStatusInFunction->relation = heap_open(DbPrivilegeId, AccessShareLock);
        /* specify user name */
        ScanKeyData key[1];
        ScanKeyInit(&key[0], Anum_gs_db_privilege_roleid, BTEqualStrategyNumber, F_OIDEQ, ObjectIdGetDatum(oid));
        scanStatusInFunction->scanDesc = systable_beginscan(scanStatusInFunction->relation, 0, false, NULL, 1, key);
        MemoryContextSwitchTo(oldContext);
    }

    FuncCallContext *fctx = SRF_PERCALL_SETUP();
    ScanStatusInFunction *scanStatusInFunction = (ScanStatusInFunction *)fctx->user_fctx;

    HeapTuple heapTuple = NULL;
    while ((heapTuple = systable_getnext(scanStatusInFunction->scanDesc)) != NULL) {
        bool isNull = false;
        Datum oid = SysCacheGetAttr(DBPRIVOID, heapTuple, Anum_gs_db_privilege_roleid, &isNull);
        Assert(!isNull);
        Datum privilegeType = SysCacheGetAttr(DBPRIVOID, heapTuple, Anum_gs_db_privilege_privilege_type, &isNull);
        Assert(!isNull);
        Datum adminOption = SysCacheGetAttr(DBPRIVOID, heapTuple, Anum_gs_db_privilege_admin_option, &isNull);
        Assert(!isNull);
        Datum values[4];
        values[0] = oid;
        values[1] = privilegeType;
        values[2] = adminOption;
        values[3] = CStringGetTextDatum(
            ConstraintAnyPrivilege(roleName, text_to_cstring(DatumGetTextP(privilegeType)), DatumGetBool(adminOption)));
        bool nulls[4] = {false, false, false, false};
        HeapTuple tuple = heap_form_tuple(fctx->tuple_desc, values, nulls);
        SRF_RETURN_NEXT(fctx, HeapTupleGetDatum(tuple));
    }

    systable_endscan(scanStatusInFunction->scanDesc);
    heap_close(scanStatusInFunction->relation, AccessShareLock);
    SRF_RETURN_DONE(fctx);
}
