#include "postgres.h"
#include "plugin_parser/parser.h"
#include "plugin_parser/analyze.h"
#include "plugin_parser/parse_oper.h"
#include "plugin_storage/hash.h"
#include "plugin_postgres.h"
#include "plugin_utils/plpgsql.h"
#include "plugin_optimizer/planner.h"
#include "commands/extension.h"
#include "commands/dbcommands.h"
#include "commands/copy.h"
#include "commands/tablespace.h"
#include "commands/comment.h"
#include "commands/prepare.h"
#include "commands/alter.h"
#include "commands/directory.h"
#include "commands/user.h"
#include "commands/conversioncmds.h"
#include "commands/discard.h"
#include "commands/lockcmds.h"
#include "commands/typecmds.h"
#include "commands/shutdown.h"
#include "commands/proclang.h"
#include "commands/async.h"
#include "commands/sequence.h"
#include "commands/createas.h"
#include "commands/view.h"
#include "commands/portalcmds.h"
#include "commands/collationcmds.h"
#include "commands/schemacmds.h"
#include "commands/sec_rls_cmds.h"
#include "commands/seclabel.h"
#include "commands/subscriptioncmds.h"
#include "commands/publicationcmds.h"
#include "commands/cluster.h"
#include "commands/matview.h"
#include "utils/guc.h"
#include "plugin_mb/pg_wchar.h"
#include "utils/builtins.h"
#include "catalog/pg_collation.h"
#include "catalog/pg_synonym.h"
#include "catalog/gs_matview.h"
#include "catalog/gs_db_privilege.h"
#include "catalog/pg_extension.h"
#include "catalog/pg_operator.h"
#include "catalog/pg_attribute.h"
#include "executor/spi_priv.h"
#include "tcop/utility.h"
#include "gs_ledger/ledger_utils.h"
#include "storage/lmgr.h"
#include "utils/inval.h"
#include "utils/lsyscache.h"
#include "utils/acl.h"
#include "utils/knl_catcache.h"
#include "plugin_utils/date.h"
#include "utils/nabstime.h"
#include "utils/geo_decls.h"
#include "utils/varbit.h"
#include "utils/json.h"
#include "utils/jsonb.h"
#include "utils/xml.h"
#include "pgxc/groupmgr.h"
#include "libpq/pqformat.h"
#include "optimizer/nodegroups.h"
#include "plugin_optimizer/pgxcplan.h"
#include "pgstat.h"
#include "client_logic/client_logic.h"
#include "db4ai/create_model.h"
#include "gs_policy/gs_policy_masking.h"
#include "gs_policy/gs_policy_audit.h"
#include "gs_policy/policy_common.h"
#include "pgxc/poolutils.h"
#include "access/reloptions.h"
#include "access/cstore_delta.h"
#include "access/twophase.h"
#include "rewrite/rewriteDefine.h"
#include "storage/procarray.h"
#include "storage/tcap.h"
#include "plugin_parser/parse_func.h"
#include "plugin_parser/parse_utilcmd.h"
#include "plugin_vector/vecfunc.h"
#include "replication/archive_walreceiver.h"
#include "plugin_commands/mysqlmode.h"
#include "plugin_protocol/startup.h"
#include "libpq/libpq.h"
#include "plugin_protocol/printtup.h"
#include "plugin_protocol/dqformat.h"
#ifdef DOLPHIN
#include "plugin_utils/my_locale.h"
#endif
#ifndef WIN32_ONLY_COMPILER
#include "dynloader.h"
#else
#include "port/dynloader/win32.h"
#endif

typedef struct sql_mode_entry {
    const char* name; /* name of sql mode entry */
    int flag;         /* bit flag position */
} sql_mode_entry;

static const struct sql_mode_entry sql_mode_options[OPT_SQL_MODE_MAX] = {
    {"sql_mode_defaults", OPT_SQL_MODE_DEFAULT},
    {"sql_mode_strict", OPT_SQL_MODE_STRICT},
    {"sql_mode_full_group", OPT_SQL_MODE_FULL_GROUP},
    {"pipes_as_concat", OPT_SQL_MODE_PIPES_AS_CONCAT},
    {"ansi_quotes", OPT_SQL_MODE_ANSI_QUOTES},
    {"no_zero_date", OPT_SQL_MODE_NO_ZERO_DATE},
    {"pad_char_to_full_length", OPT_SQL_MODE_PAD_CHAR_TO_FULL_LENGTH},
    {"block_return_multi_results", OPT_SQL_MODE_BLOCK_RETURN_MULTI_RESULTS},
    {"auto_recompile_function", OPT_SQL_MODE_ATUO_RECOMPILE_FUNCTION},
    {"error_for_division_by_zero", OPT_SQL_MODE_ERROR_FOR_DIVISION_BY_ZERO},
    {"treat_bxconst_as_binary", OPT_SQL_MODE_TREAT_BXCONST_AS_BINARY},
};

#define DOLPHIN_TYPES_NUM 12
#define TYPE_ATTRIBUTES_NUM 3
#define NAMEEQ_FN_OID 62
/* typname, enable precision, enable scale */
static const char* dolphinTypes[DOLPHIN_TYPES_NUM][TYPE_ATTRIBUTES_NUM] = {
    {"uint1", "false", "false"},
    {"uint2", "false", "false"},
    {"uint4", "false", "false"},
    {"uint8", "false", "false"},
    {"year", "true", "false"},
    {"binary", "true", "false"},
    {"varbinary", "true", "false"},
    {"tinyblob", "false", "false"},
    {"mediumblob", "false", "false"},
    {"longblob", "false", "false"},
    {"set", "false", "false"},
    {"enum", "false", "false"}
};
/*
 * For loading plpgsql function.
 */
typedef struct {
    char* func_name;
    PGFunction func_addr;
} RegExternFunc;

PG_MODULE_MAGIC_PUBLIC;

extern void InitLockNameHash();
extern struct HTAB* lockNameHash;
extern pthread_mutex_t gNameHashLock;
extern void initBSQLBuiltinFuncs();
extern struct HTAB* b_nameHash;
extern struct HTAB* b_oidHash;
extern RegExternFunc b_plpgsql_function_table[3];
extern int tmp_b_fmgr_nbuiltins;
extern FmgrBuiltin tmp_b_fmgr_builtins[];
extern void deparse_query(Query* query, StringInfo buf, List* parentnamespace, bool finalise_aggs, bool sortgroup_colno,
    void* parserArg, bool qrw_phase, bool is_fqs);
extern bool isAllTempObjects(Node* parse_tree, const char* query_string, bool sent_to_remote);
extern void ts_check_feature_disable();
extern void ExecAlterDatabaseSetStmt(Node* parse_tree, const char* query_string, bool sent_to_remote);
extern void DoVacuumMppTable(VacuumStmt* stmt, const char* query_string, bool is_top_level, bool sent_to_remote);
extern bool IsVariableinBlackList(const char* name);
extern void ExecAlterRoleSetStmt(Node* parse_tree, const char* query_string, bool sent_to_remote);
static bool CheckSqlMode(char** newval, void** extra, GucSource source);
static void AssignSqlMode(const char* newval, void* extra);
static bool check_b_db_timestamp(double* newval, void** extra, GucSource source);
static void assign_b_db_timestamp(double newval, void* extra);
#ifdef DOLPHIN
static bool check_sql_mode(char** newval, void** extra, GucSource source);
static bool check_lower_case_table_names(int* newval, void** extra, GucSource source);
static bool check_default_week_format(int* newval, void** extra, GucSource source);
static void assign_default_week_format(int newval, void* extra);
static bool check_lc_time_names(char** newval, void** extra, GucSource source);
static bool check_auto_increment_increment(int* newval, void** extra, GucSource source);
static bool check_character_set_client(char** newval, void** extra, GucSource source);
static bool check_character_set_results(char** newval, void** extra, GucSource source);
static bool check_character_set_server(char** newval, void** extra, GucSource source);
static bool check_collation_server(char** newval, void** extra, GucSource source);
static bool check_init_connect(char** newval, void** extra, GucSource source);
static bool check_interactive_timeout(int* newval, void** extra, GucSource source);
static bool check_license(char** newval, void** extra, GucSource source);
static bool check_max_allowed_packet(int* newval, void** extra, GucSource source);
static bool check_net_buffer_length(int* newval, void** extra, GucSource source);
static bool check_net_write_timeout(int* newval, void** extra, GucSource source);
static bool check_query_cache_size(long int* newval, void** extra, GucSource source);
static bool check_query_cache_type(int* newval, void** extra, GucSource source);
static bool check_system_time_zone(char** newval, void** extra, GucSource source);
static bool check_time_zone(char** newval, void** extra, GucSource source);
static bool check_wait_timeout(int* newval, void** extra, GucSource source);
static int SpiIsExecMultiSelect(PLpgSQL_execstate* estate, PLpgSQL_expr* expr,
    PLpgSQL_stmt_execsql* pl_stmt, ParamListInfo paramLI, long tcount, bool* multi_res);
static void SpiMultiSelectException();
#endif
static const int LOADER_COL_BUF_CNT = 5;
static uint32 dolphin_index;
extern void set_hypopg_prehook(ProcessUtility_hook_type func);
extern void set_pgaudit_prehook(ProcessUtility_hook_type func);
extern bool check_plugin_function(Oid funcId);
extern PGFunction SearchFuncByOid(Oid funcId);
static bool protocol_inited;
static bool global_hook_inited = false;
static pthread_mutex_t gInitPluginObjectLock;

extern "C" DLL_PUBLIC void _PG_init(void);
extern "C" DLL_PUBLIC void _PG_fini(void);

PG_FUNCTION_INFO_V1_PUBLIC(dolphin_types);
extern "C" DLL_PUBLIC Datum dolphin_types();
extern "C" Datum dolphin_binaryin(PG_FUNCTION_ARGS);
static void InitDolphinTypeId(BSqlPluginContext* cxt);
static void InitDolphinOperator(BSqlPluginContext* cxt);
static Datum DolphinGetTypeZeroValue(Form_pg_attribute att_tup);

PG_FUNCTION_INFO_V1_PUBLIC(dolphin_invoke);
void dolphin_invoke(void)
{
    ereport(DEBUG2, (errmsg("dummy function to let process load this library.")));
    return;
}

void set_default_guc()
{
    /* add display_leading_zero to behavior_compat_options */
    StringInfo extra_option = makeStringInfo();
    bool first = true;
    if (!DISPLAY_LEADING_ZERO) {
        appendStringInfo(extra_option, "display_leading_zero");
        first = false;
    }

    if (!SELECT_INTO_RETURN_NULL) {
        appendStringInfo(extra_option, "%sselect_into_return_null", first ? "" : ",");
        first = false;
    }

    if (!first) {
        if (u_sess->attr.attr_sql.behavior_compat_string[0] != 0) {
            appendStringInfo(extra_option, ",%s", u_sess->attr.attr_sql.behavior_compat_string);
        }

        set_config_option("behavior_compat_options", extra_option->data, PGC_USERSET,
                          PGC_S_SESSION, GUC_ACTION_SET, true, 0, false);
    }
    DestroyStringInfo(extra_option);

    set_config_option("enable_custom_parser", "true", PGC_USERSET, PGC_S_SESSION, GUC_ACTION_SET, true, 0, false);
    set_config_option("datestyle", "ISO, YMD", PGC_USERSET, PGC_S_SESSION, GUC_ACTION_SET, true, 0, false);
}

void init_dolphin_proto(char* database_name)
{
    if (g_proto_ctx.database_name.data[0] == '\0') {
        int ret = strcpy_s(g_proto_ctx.database_name.data, NAMEDATALEN, database_name);
        securec_check(ret, "\0", "\0");
    }
    
    if (protocol_inited) {
        define_dolphin_server_guc();
        return;
    }

    AutoMutexLock protoLock(&gInitPluginObjectLock);
    protoLock.lock();
    /* double check and quick exit */
    if (protocol_inited) {
        protoLock.unLock();
        define_dolphin_server_guc();
        return;
    }

    /* use try-catch to unlock if error happend */
    PG_TRY();
    {
        define_dolphin_server_guc();
        server_listen_init();
        protocol_inited = true;
        g_instance.listen_cxt.reload_fds = true;
    }
    PG_CATCH();
    {
        protoLock.unLock();
        PG_RE_THROW();
    }
    PG_END_TRY();

    protoLock.unLock();
}

HeapTuple searchCat(Relation relation, Oid indexoid,
    int cacheId, int nkeys, ScanKeyData* cur_skey, SysScanDesc* scandesc);
bool ccHashEqFuncs(Oid keytype, CCHashFN *hashfunc,
    RegProcedure *eqfunc, CCFastEqualFN *fasteqfunc, int cacheId);
/*
 * NOTE: this function will be called concurently, when you add code here, please make sure your code is thread-safe.
 * If not, please use gInitPluginObjectLock to control your code.
 */
void init_plugin_object()
{
    if (!global_hook_inited) {
        g_instance.raw_parser_hook[DB_CMPT_B] = (void*)raw_parser;
        g_instance.llvmIrFilePath[DB_CMPT_B] = "share/postgresql/extension/openGauss_expr_dolphin.ir";

        b_plpgsql_function_table[0] = {"plpgsql_call_handler", b_plpgsql_call_handler};
        b_plpgsql_function_table[1] = {"plpgsql_inline_handler", b_plpgsql_inline_handler};
        b_plpgsql_function_table[2] = {"plpgsql_validator", b_plpgsql_validator};

        global_hook_inited = true;
    }

    u_sess->hook_cxt.deparseQueryHook = (void*)deparse_query;
    u_sess->hook_cxt.transformStmtHook = (void*)transformStmt;
    u_sess->hook_cxt.execInitExprHook = (void*)ExecInitExpr;
    u_sess->hook_cxt.computeHashHook  = (void*)compute_hash_default;
    u_sess->hook_cxt.aggSmpHook = (void*)check_plugin_function;
    u_sess->hook_cxt.standardProcessUtilityHook = (void*)standard_ProcessUtility;
    u_sess->hook_cxt.searchFuncHook = (void*)SearchFuncByOid;
    u_sess->hook_cxt.plannerHook = (void*)planner;
    u_sess->hook_cxt.pluginSearchCatHook = (void*)searchCat;
    u_sess->hook_cxt.pluginCCHashEqFuncs = (void*)ccHashEqFuncs;
    u_sess->hook_cxt.plpgsqlParserSetHook = (void*)b_plpgsql_parser_setup;
    u_sess->hook_cxt.coreYYlexHook = (void*)core_yylex;
    u_sess->hook_cxt.pluginProcDestReciverHook = (void*)CreateSqlProcSpiDestReciver;
    u_sess->hook_cxt.pluginSpiReciverParamHook = (void*)SetSqlProcSpiStmtParams;
    u_sess->hook_cxt.pluginSpiExecuteMultiResHook = (void*)SpiIsExecMultiSelect;
    u_sess->hook_cxt.pluginMultiResExceptionHook = (void*)SpiMultiSelectException;
    u_sess->hook_cxt.getTypeZeroValueHook = (void*)DolphinGetTypeZeroValue;
    set_default_guc();

    if (g_instance.attr.attr_network.enable_dolphin_proto && u_sess->proc_cxt.MyProcPort &&
        u_sess->proc_cxt.MyProcPort->database_name) {
        init_dolphin_proto(u_sess->proc_cxt.MyProcPort->database_name);
    }

    /* init types and operators */
    BSqlPluginContext* cxt = GetSessionContext();
    start_xact_command();
    InitDolphinTypeId(cxt);
    InitDolphinOperator(cxt);
    finish_xact_command();
}

void _PG_init(void)
{
    if (!u_sess->misc_cxt.process_shared_preload_libraries_in_progress && !DB_IS_CMPT(B_FORMAT)) {
        ereport(ERROR, (errmsg("Can't create dolphin extension since current database compatibility is not 'B'")));
    }

    if (b_oidHash == NULL || b_nameHash == NULL) {
        initBSQLBuiltinFuncs();
    }
    if (b_fmgr_builtins == NULL) {
        b_fmgr_builtins = tmp_b_fmgr_builtins;
        pg_memory_barrier(); /* make sure b_fmgr_builtins has been assigned before b_fmgr_nbuiltins */
        b_fmgr_nbuiltins = tmp_b_fmgr_nbuiltins;
    }
    AutoMutexLock nameHashLock(&gNameHashLock);
    nameHashLock.lock();
    if (lockNameHash == NULL)
        InitLockNameHash();
    nameHashLock.unLock();

    if (g_instance.plugin_vec_func_cxt.vec_func_plugin[DOLPHIN_VEC] == NULL) {
        InitGlobalVecFuncMap();
    }
}

void _PG_fini(void)
{
    hash_destroy(b_nameHash);
    hash_destroy(b_oidHash);
    g_instance.raw_parser_hook[DB_CMPT_B] = NULL;
    g_instance.llvmIrFilePath[DB_CMPT_B] = NULL;
}

HeapTuple searchCat(Relation relation, Oid indexoid, int cacheId, int nkeys,
    ScanKeyData* cur_skey, SysScanDesc* scandesc)
{
    HeapTuple ntp = NULL;
    if (cacheId == ATTNAME && nkeys == 2 && cur_skey[1].sk_func.fn_oid == NAMEEQ_FN_OID) {
        Oid typeId[2];
        typeId[0] = NAMEOID;
        typeId[1] = NAMEOID;
        Oid func_oid = LookupFuncName(list_make1(makeString("dolphin_attname_eq")), 2, typeId, true);
        if (OidIsValid(func_oid)) {
            fmgr_info(func_oid, &cur_skey[1].sk_func);
            *scandesc = systable_beginscan(relation, indexoid, IndexScanOK(cacheId), NULL, 1, cur_skey);
        }
    }
    if (*scandesc == NULL) {
        *scandesc = systable_beginscan(relation, indexoid, IndexScanOK(cacheId), NULL, nkeys, cur_skey);
    }
    while (HeapTupleIsValid(ntp = systable_getnext(*scandesc))) {
        if (cacheId == ATTNAME && cur_skey[1].sk_func.fn_oid != NAMEEQ_FN_OID) {
            if (DatumGetBool(FunctionCall2(&cur_skey[1].sk_func, cur_skey[1].sk_argument,
                NameGetDatum(&((Form_pg_attribute)GETSTRUCT(ntp))->attname)))) {
                break; /* assume only one match */
            } else {
                continue;
            }
        }
        break;
    }
    return ntp;
}

static uint32 dolphinnamehashfast(Datum datum)
{
    char* key = NameStr(*DatumGetName(datum));
    /* to lower change values of datum,but it's ok in dolphin colname match
     * except alter add column may has affect */
    char *ckey = (char*)palloc0(NAMEDATALEN);
    errno_t ss_rc = memcpy_s(ckey, NAMEDATALEN, key, strlen(key) + 1);
    securec_check(ss_rc, "\0", "\0");
     
    pg_strtolower(ckey);
    return hash_any((unsigned char*)ckey, strlen(ckey));
}

static bool dolphinnameeqfast(Datum a, Datum b)
{
    char* ca = NameStr(*DatumGetName(a));
    char* cb = NameStr(*DatumGetName(b));
    return strncasecmp(ca, cb, NAMEDATALEN) == 0;
}

bool ccHashEqFuncs(Oid keytype, CCHashFN *hashfunc, RegProcedure *eqfunc, CCFastEqualFN *fasteqfunc, int cacheId)
{
    if (cacheId == ATTNAME && keytype == NAMEOID) {
        *hashfunc = dolphinnamehashfast;
        *fasteqfunc = dolphinnameeqfast;
        *eqfunc = F_NAMEEQ;
        return true;
    }
    return false;
}

#define IS_CLIENT_CONN_VALID_PROC_SPI(port) \
    (((port) == NULL)                       \
    ? false                                 \
    : (((port)->is_logic_conn) ? ((port)->gs_sock.type != GSOCK_INVALID) : ((port)->sock != NO_SOCKET)))

static int SpiIsExecMultiSelect(PLpgSQL_execstate* estate, PLpgSQL_expr* expr, PLpgSQL_stmt_execsql* pl_stmt,
    ParamListInfo param_li, long tcount, bool* multi_res)
{
    bool outPutSelRes = false;
    Port* MyProcPort = u_sess->proc_cxt.MyProcPort;
    int tmpPos = t_thrd.libpq_cxt.PqSendPointer;
    CommandDest origDest = u_sess->SPI_cxt._current->dest;
    int rc;
    if (SQL_MODE_AllOW_PROCEDURE_WITH_SELECT() && GetSessionContext()->is_dolphin_call_stmt) {
        CachedPlan* cplan = SPI_plan_get_cached_plan(expr->plan);
        List *stmt_list = NULL;
        if (cplan)
            stmt_list = cplan->stmt_list;
        if (stmt_list) {
            Node *stmt = (Node *)linitial(stmt_list);
            if (IsA(stmt, PlannedStmt) && ((PlannedStmt *)stmt)->commandType == CMD_SELECT &&
                !pl_stmt->into) {
                t_thrd.libpq_cxt.PqSendStart += t_thrd.libpq_cxt.PqSendPointer;
                u_sess->SPI_cxt._current->dest = DestSqlProcSPI;
                outPutSelRes = true;
            }
        }
        if (cplan)
            ReleaseCachedPlan(cplan, true);
    }
    rc = SPI_execute_plan_with_paramlist(expr->plan, param_li, estate->readonly_func, tcount);
    if (outPutSelRes) {
        if (MyProcPort && MyProcPort->protocol_config->fn_printtup_create_DR &&
            MyProcPort->protocol_config->fn_printtup_create_DR == dophin_printtup_create_DR) {
            StringInfo buf = makeStringInfo();
            send_network_fetch_packet(buf);
            DestroyStringInfo(buf);
        } else {
            const char* strs = "CALL";
            EndCommand(strs, DestRemote);
        }
        if (IS_CLIENT_CONN_VALID_PROC_SPI(MyProcPort) && (!t_thrd.int_cxt.ClientConnectionLost)) {
            CHECK_FOR_INTERRUPTS();
            pq_flush();
            t_thrd.libpq_cxt.PqSendPointer = tmpPos;
        }
    }
    *multi_res = outPutSelRes;
    u_sess->SPI_cxt._current->dest = origDest;
    return rc;
}


static void SpiMultiSelectException()
{
    Port* MyProcPort = u_sess->proc_cxt.MyProcPort;
    if (u_sess->SPI_cxt._current->dest == DestSqlProcSPI) {
        if (MyProcPort && MyProcPort->protocol_config->fn_printtup_create_DR &&
            MyProcPort->protocol_config->fn_printtup_create_DR == dophin_printtup_create_DR) {
            StringInfo buf = makeStringInfo();
            send_network_fetch_packet(buf);
            DestroyStringInfo(buf);
        } else {
            const char* strs = "CALL";
            EndCommand(strs, DestRemote);
        }
        if (IS_CLIENT_CONN_VALID_PROC_SPI(MyProcPort) && (!t_thrd.int_cxt.ClientConnectionLost)) {
            CHECK_FOR_INTERRUPTS();
            pq_flush();
        }
    }
}
/*
 * check_behavior_compat_options: GUC check_hook for behavior compat options
 */
static bool CheckSqlMode(char** newval, void** extra, GucSource source)
{
    char* rawstring = NULL;
    List* elemlist = NULL;
    ListCell* cell = NULL;
    int start = 0;

    /* Need a modifiable copy of string */
    rawstring = pstrdup(*newval);
    /* Parse string into list of identifiers */
    if (!SplitIdentifierString(rawstring, ',', &elemlist)) {
        /* syntax error in list */
        GUC_check_errdetail("invalid paramater for sql_mode.");
        pfree(rawstring);
        list_free(elemlist);

        return false;
    }

    foreach (cell, elemlist) {
        const char* item = (const char*)lfirst(cell);
        bool nfound = true;

        for (start = 0; start < OPT_SQL_MODE_MAX; start++) {
            if (strcmp(item, sql_mode_options[start].name) == 0) {
                nfound = false;
                break;
            }
        }

        if (nfound) {
            GUC_check_errdetail("invalid sql_mode option \"%s\"", item);
            pfree(rawstring);
            list_free(elemlist);
            return false;
        }
    }

    pfree(rawstring);
    list_free(elemlist);

    return true;
}

/*
 * assign_distribute_test_param: GUC assign_hook for distribute_test_param
 */
static void AssignSqlMode(const char* newval, void* extra)
{
    char* rawstring = NULL;
    List* elemlist = NULL;
    ListCell* cell = NULL;
    int start = 0;
    int result = 0;

    rawstring = pstrdup(newval);
    (void)SplitIdentifierString(rawstring, ',', &elemlist);

    GetSessionContext()->sqlModeFlags = 0;
    foreach (cell, elemlist) {
        for (start = 0; start < OPT_SQL_MODE_MAX; start++) {
            const char* item = (const char*)lfirst(cell);

            if (strcmp(item, sql_mode_options[start].name) == 0)
                result += sql_mode_options[start].flag;
        }
    }

    pfree(rawstring);
    list_free(elemlist);

    GetSessionContext()->sqlModeFlags = result;
}

static bool check_b_db_timestamp(double* newval, void** extra, GucSource source)
{
    double newval_interval = *newval;
    if ((newval_interval < DEFAULT_GUC_B_DB_TIMESTAMP) || (newval_interval > DEFAULT_GUC_B_DB_TIMESTAMP && newval_interval < 1.0) || (newval_interval > MAX_GUC_B_DB_TIMESTAMP)) {
            GUC_check_errmsg("Variable \'b_db_timestamp\' can not be set to the value of \'%lf\'", newval_interval);
            return false;
    }
    return true;
}

static void assign_b_db_timestamp(double newval, void* extra)
{
    GetSessionContext()->b_db_timestamp = newval;
}

#ifdef DOLPHIN
static bool check_sql_mode(char** newval, void** extra, GucSource source)
{
    if (source == PGC_S_SESSION) {
        ereport(WARNING,
            (errmsg("Variable 'sql_mode' has no actual meaning, please use variable 'dolphin.sql_mode'.")));
    }
    return true;
}

static bool check_lower_case_table_names(int* newval, void** extra, GucSource source)
{
    if (source == PGC_S_SESSION) {
        ereport(WARNING,
            (errmsg("Variable 'lower_case_table_names' has no actual meaning, "
                "please use variable 'dolphin.lower_case_table_names'.")));
    }
    return true;
}

static bool check_default_week_format(int* newval, void** extra, GucSource source)
{
    int newval_interval = *newval;
    if (newval_interval < DEFAULT_GUC_WEEK_FORMAT) {
        *newval = DEFAULT_GUC_WEEK_FORMAT;
        ereport(WARNING,
                (errmsg("Truncated incorrect default_week_format value: \'%d\'",newval_interval)));
    } else if (newval_interval > MAX_GUC_WEEK_FORMAT) {
        *newval = MAX_GUC_WEEK_FORMAT;
        ereport(WARNING,
                (errmsg("Truncated incorrect default_week_format value: \'%d\'",newval_interval)));
    }
    return true;
}

static void assign_default_week_format(int newval, void* extra)
{
    GetSessionContext()->default_week_format = newval;
}

static bool check_lc_time_names(char** newval, void** extra, GucSource source)
{
    MyLocale *locale = MyLocaleSearch(*newval);
    if (locale == NULL) {
        GUC_check_errmsg("Unknown locale:\'%s\'", *newval);
        return false;
    }
    return true;
}

static bool check_auto_increment_increment(int* newval, void** extra, GucSource source)
{
    if (source == PGC_S_SESSION) {
        ereport(WARNING, (errmsg("Variable 'auto_increment_increment' has no actual meaning.")));
    }
    return true;
}

static bool check_character_set_client(char** newval, void** extra, GucSource source)
{
    if (source == PGC_S_SESSION) {
        ereport(WARNING, (errmsg("Variable 'character_set_client' has no actual meaning.")));
    }
    return true;
}

static bool check_character_set_results(char** newval, void** extra, GucSource source)
{
    if (source == PGC_S_SESSION) {
        ereport(WARNING, (errmsg("Variable 'character_set_results' has no actual meaning.")));
    }
    return true;
}

static bool check_character_set_server(char** newval, void** extra, GucSource source)
{
    if (source == PGC_S_SESSION) {
        ereport(WARNING, (errmsg("Variable 'character_set_server' has no actual meaning.")));
    }
    return true;
}
static bool check_collation_server(char** newval, void** extra, GucSource source)
{
    if (source == PGC_S_SESSION) {
        ereport(WARNING, (errmsg("Variable 'collation_server' has no actual meaning.")));
    }
    return true;
}

static bool check_init_connect(char** newval, void** extra, GucSource source)
{
    if (source == PGC_S_SESSION) {
        ereport(WARNING, (errmsg("Variable 'init_connect' has no actual meaning.")));
    }
    return true;
}

static bool check_interactive_timeout(int* newval, void** extra, GucSource source)
{
    if (source == PGC_S_SESSION) {
        ereport(WARNING, (errmsg("Variable 'interactive_timeout' has no actual meaning.")));
    }
    return true;
}

static bool check_license(char** newval, void** extra, GucSource source)
{
    if (source == PGC_S_DEFAULT) {
        return true;
    }
    ereport(ERROR,
        (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
            errmsg("Variable 'license' is a read only variable.")));
    return false;
}

static bool check_max_allowed_packet(int* newval, void** extra, GucSource source)
{
    if (source == PGC_S_SESSION) {
        ereport(WARNING, (errmsg("Variable 'max_allowed_packet' has no actual meaning.")));
    }
    return true;
}

static bool check_net_buffer_length(int* newval, void** extra, GucSource source)
{
    if (source == PGC_S_SESSION) {
        ereport(WARNING, (errmsg("Variable 'net_buffer_length' has no actual meaning.")));
    }
    return true;
}

static bool check_net_write_timeout(int* newval, void** extra, GucSource source)
{
    if (source == PGC_S_SESSION) {
        ereport(WARNING, (errmsg("Variable 'net_write_timeout' has no actual meaning.")));
    }
    return true;
}

static bool check_query_cache_size(long int* newval, void** extra, GucSource source)
{
    if (source == PGC_S_SESSION) {
        ereport(WARNING, (errmsg("Variable 'query_cache_size' has no actual meaning.")));
    }
    return true;
}

static bool check_query_cache_type(int* newval, void** extra, GucSource source)
{
    if (source == PGC_S_SESSION) {
        ereport(WARNING, (errmsg("Variable 'query_cache_type' has no actual meaning.")));
    }
    return true;
}

static bool check_system_time_zone(char** newval, void** extra, GucSource source)
{
    if (source == PGC_S_SESSION) {
        ereport(WARNING, (errmsg("Variable 'system_time_zone' has no actual meaning.")));
    }
    return true;
}

static bool check_time_zone(char** newval, void** extra, GucSource source)
{
    if (source == PGC_S_SESSION) {
        ereport(WARNING, (errmsg("Variable 'time_zone' has no actual meaning.")));
    }
    return true;
}

static bool check_wait_timeout(int* newval, void** extra, GucSource source)
{
    if (source == PGC_S_SESSION) {
        ereport(WARNING, (errmsg("Variable 'wait_timeout' has no actual meaning.")));
    }
    return true;
}

BSqlPluginContext* GetSessionContext()
{
    if (u_sess->attr.attr_common.extension_session_vars_array[dolphin_index] == NULL) {
        init_session_vars();
    }
    return (BSqlPluginContext *) u_sess->attr.attr_common.extension_session_vars_array[dolphin_index];
}

void set_extension_index(uint32 index)
{
    dolphin_index = index;
}

static bool check_optimizer_switch(char** newval, void** extra, GucSource source)
{
    char* rawstring = NULL;
    List* elemlist = NULL;
    ListCell* cell = NULL;
    int start = 0;
    unsigned int hasSet = 0;

    /* Need a modifiable copy of string */
    rawstring = pstrdup(*newval);
    if (strcmp(rawstring, "default") == 0) {
        pfree(rawstring);
        return true;
    }

    /* Parse string into list of identifiers */
    if (!SplitIdentifierString(rawstring, ',', &elemlist)) {
        /* syntax error in list */
        GUC_check_errdetail("invalid paramater for optimizer_switch.");
        pfree(rawstring);
        list_free(elemlist);

        return false;
    }

    foreach (cell, elemlist) {
        char* item = (char*)lfirst(cell);
        bool nfound = true;
        List* kv = NULL;

#define LENGTH_OF_KV 2

        if (!SplitIdentifierString(item, '=', &kv) || list_length(kv) != LENGTH_OF_KV) {
            /* syntax error in list */
            GUC_check_errdetail("invalid paramater for optimizer_switch.");
            pfree(rawstring);
            list_free(elemlist);

            return false;
        }

        const char* key = (const char*)linitial(kv);
        const char* value = (const char*)lsecond(kv);

        for (start = 0; start < OPT_OPTIMIZER_SWITCH_MAX; start++) {
            if (strcmp(key, optimizer_switch_options[start].name) == 0) {
                nfound = false;
                break;
            }
        }

        if (nfound) {
            GUC_check_errdetail("invalid optimizer switch option \"%s\"", item);
            pfree(rawstring);
            list_free(elemlist);
            return false;
        }

        if ((hasSet & optimizer_switch_options[start].flag) != 0) {
            GUC_check_errdetail("duplicate optimizer switch option \"%s\"", item);
            pfree(rawstring);
            list_free(elemlist);
            return false;
        }

        /* value must be on/off/default */
        if (strcmp(value, "on") != 0 && strcmp(value, "off") != 0 && strcmp(value, "default") != 0) {
            GUC_check_errdetail("invalid optimizer switch value \"%s\"", value);
            pfree(rawstring);
            list_free(elemlist);
            return false;
        }

        hasSet |= optimizer_switch_options[start].flag;
    }

    pfree(rawstring);
    list_free(elemlist);

    return true;
}

static void assign_optimizer_switch(const char* newval, void* extra)
{
    char* rawstring = NULL;
    List* elemlist = NULL;
    ListCell* cell = NULL;
    int start = 0;
    unsigned int result = 0;

    if (strcmp(newval, "default") == 0) {
        for (start = 0; start < OPT_OPTIMIZER_SWITCH_MAX; start++) {
            if (optimizer_switch_options[start].defaultValue) {
                result += optimizer_switch_options[start].flag;
            }
        }
        GetSessionContext()->optimizer_switch_flags = result;
        return;
    }

    rawstring = pstrdup(newval);
    (void)SplitIdentifierString(rawstring, ',', &elemlist);

    GetSessionContext()->optimizer_switch_flags = 0;
    foreach (cell, elemlist) {
        char* item = (char*)lfirst(cell);
        List* kv = NULL;

        (void)!SplitIdentifierString(item, '=', &kv);

        const char* key = (const char*)linitial(kv);
        const char* value = (const char*)lsecond(kv);

        for (start = 0; start < OPT_OPTIMIZER_SWITCH_MAX; start++) {
            if (strcmp(key, optimizer_switch_options[start].name) == 0) {
                if (strcmp(value, "on") == 0 || (strcmp(value, "default") == 0 &&
                    optimizer_switch_options[start].defaultValue)) {
                    result += optimizer_switch_options[start].flag;
                }
                break;
            }
        }
    }

    pfree(rawstring);
    list_free(elemlist);

    GetSessionContext()->optimizer_switch_flags = result;
}
#endif

void init_session_vars(void)
{
    RepallocSessionVarsArrayIfNecessary();

    BSqlPluginContext *cxt = (BSqlPluginContext *) MemoryContextAlloc(u_sess->self_mem_cxt, sizeof(bSqlPluginContext));
    u_sess->attr.attr_common.extension_session_vars_array[dolphin_index] = cxt;
    cxt->enableBCmptMode = false;
    cxt->lockNameList = NIL;
    cxt->scan_from_pl = false;
    cxt->is_b_declare = false;
    cxt->default_database_name = NULL;
    cxt->paramIdx = 0;
    cxt->isUpsert = false;
    cxt->single_line_trigger_begin = 0;
    cxt->do_sconst = NULL;
    cxt->single_line_proc_begin = 0;
    cxt->is_schema_name = false;
    cxt->b_stmtInputTypeHash = NULL;
    cxt->b_sendBlobHash = NULL;
    cxt->is_dolphin_call_stmt = false;
    cxt->is_binary_proto = false;
    cxt->is_ast_stmt = false;
    cxt->group_by_error = false;
    cxt->is_create_alter_stmt = false;
    cxt->isDoCopy = false;

    DefineCustomBoolVariable("dolphin.b_compatibility_mode",
                             "Enable mysql behavior override opengauss's when collision happens.",
                             NULL,
                             &ENABLE_B_CMPT_MODE,
                             false,
                             PGC_USERSET,
                             0,
                             NULL, NULL, NULL);

    DefineCustomStringVariable("dolphin.sql_mode",
                               gettext_noop("CUSTOM_OPTIONS"),
                               NULL,
                               &GetSessionContext()->sqlModeString,
                               "sql_mode_strict,sql_mode_full_group,pipes_as_concat,ansi_quotes,no_zero_date,"
                               "pad_char_to_full_length,auto_recompile_function,error_for_division_by_zero",
                               PGC_USERSET,
                               GUC_LIST_INPUT | GUC_REPORT,
                               CheckSqlMode,
                               AssignSqlMode,
                               NULL);
    DefineCustomRealVariable("dolphin.b_db_timestamp",
                             "A flag influces function now(n) in B compatibility.",
                             "If b_db_timestamp is 0, now(n) will return current timestamp. "
                             "If b_db_timestamp is between 1 and 2147483647, the b_db_timestamp "
                             "will be considered as the seconds offset, and now(n) will return the "
                             "timestamp based on 1970-01-01 00:00:00 UTC + seconds offset + current time zone offset.",
                             &GetSessionContext()->b_db_timestamp,
                             DEFAULT_GUC_B_DB_TIMESTAMP,
                             DEFAULT_GUC_B_DB_TIMESTAMP,
                             MAX_GUC_B_DB_TIMESTAMP,
                             PGC_USERSET,
                             0,
                             check_b_db_timestamp,
                             assign_b_db_timestamp,
                             NULL);
    DefineCustomIntVariable("dolphin.lower_case_table_names",
                            gettext_noop("used to set the sensitive of identifier"),
                            NULL,
                            &GetSessionContext()->lower_case_table_names,
                            1,
                            0,
                            2,
                            PGC_USERSET,
                            0,
                            NULL,
                            NULL,
                            NULL);
#ifdef DOLPHIN
    DefineCustomStringVariable("sql_mode",
                               gettext_noop("CUSTOM_OPTIONS"),
                               NULL,
                               &GetSessionContext()->useless_sql_mode,
                               "sql_mode_strict,sql_mode_full_group,pipes_as_concat,ansi_quotes,no_zero_date,"
                               "pad_char_to_full_length",
                               PGC_USERSET,
                               GUC_LIST_INPUT | GUC_REPORT,
                               check_sql_mode,
                               NULL,
                               NULL);
    DefineCustomIntVariable("lower_case_table_names",
                            gettext_noop("used to set the sensitive of identifier"),
                            NULL,
                            &GetSessionContext()->useless_lower_case_table_names,
                            1,
                            0,
                            2,
                            PGC_USERSET,
                            0,
                            check_lower_case_table_names,
                            NULL,
                            NULL);
    DefineCustomIntVariable("dolphin.default_week_format",
                            gettext_noop("Set the default week format for function week."),
                            gettext_noop("If the given value is less than 0, default_week_format will be set to 0. "
                                        "If the given value is greater than 7, default_week_format will be set to 7."),
                            &GetSessionContext()->default_week_format,
                            DEFAULT_GUC_WEEK_FORMAT,
                            INT_MIN,
                            INT_MAX,
                            PGC_SIGHUP,
                            0,
                            check_default_week_format,
                            assign_default_week_format,
                            NULL);
    DefineCustomStringVariable("dolphin.lc_time_names",
                               gettext_noop("Set the language charset for the return value of "
                               "function DAYNAME, MONTHNAME, and DATE_FORMAT."),
                               NULL,
                               &GetSessionContext()->lc_time_names,
                               "en_US",
                               PGC_SIGHUP,
                               0,
                               check_lc_time_names,
                               NULL,
                               NULL);
    DefineCustomStringVariable("version_comment",
                               gettext_noop("Comment of the server and the type of license the server has."),
                               NULL,
                               &GetSessionContext()->version_comment,
                               "openGauss Server(MulanPSL-2.0)",
                               PGC_INTERNAL,
                               0,
                               NULL,
                               NULL,
                               NULL);
    DefineCustomIntVariable("auto_increment_increment",
                            gettext_noop("Self-increasing step length of AUTO_INCREMENT columns."),
                            NULL,
                            &GetSessionContext()->auto_increment_increment,
                            DEFAULT_AUTO_INCREMENT,
                            MIN_AUTO_INCREMENT,
                            MAX_AUTO_INCREMENT,
                            PGC_USERSET,
                            0,
                            check_auto_increment_increment,
                            NULL,
                            NULL);
    DefineCustomStringVariable("character_set_client",
                               gettext_noop("Client uses this character set."),
                               NULL,
                               &GetSessionContext()->character_set_client,
                               "utf8",
                               PGC_USERSET,
                               0,
                               check_character_set_client,
                               NULL,
                               NULL);
    DefineCustomStringVariable("character_set_results",
                               gettext_noop("The server uses this character set "
                               "to return the query results to the client"),
                               NULL,
                               &GetSessionContext()->character_set_results,
                               "utf8",
                               PGC_USERSET,
                               0,
                               check_character_set_results,
                               NULL,
                               NULL);
    DefineCustomStringVariable("character_set_server",
                               gettext_noop("Server uses this character set."),
                               NULL,
                               &GetSessionContext()->character_set_server,
                               "latin1",
                               PGC_USERSET,
                               0,
                               check_character_set_server,
                               NULL,
                               NULL);
    DefineCustomStringVariable("collation_server",
                               gettext_noop("Server uses this collation."),
                               NULL,
                               &GetSessionContext()->collation_server,
                               "latin1_swedish_ci",
                               PGC_USERSET,
                               0,
                               check_collation_server,
                               NULL,
                               NULL);
    DefineCustomStringVariable("init_connect",
                               gettext_noop("SQL statements executed when each connection is initialized."),
                               NULL,
                               &GetSessionContext()->init_connect,
                               "",
                               PGC_USERSET,
                               0,
                               check_init_connect,
                               NULL,
                               NULL);
    DefineCustomIntVariable("interactive_timeout",
                            gettext_noop("After this number of seconds of inactivity, "
                            "the server will close the interactive connection."),
                            NULL,
                            &GetSessionContext()->interactive_timeout,
                            DEFAULT_INTERACTIVE_TIMEOUT,
                            MIN_INTERACTIVE_TIMEOUT,
                            MAX_INTERACTIVE_TIMEOUT,
                            PGC_USERSET,
                            0,
                            check_interactive_timeout,
                            NULL,
                            NULL);
    DefineCustomStringVariable("license",
                               gettext_noop("The server uses this license."),
                               NULL,
                               &GetSessionContext()->license,
                               "MulanPSL-2.0",
                               PGC_USERSET,
                               0,
                               check_license,
                               NULL,
                               NULL);
    DefineCustomIntVariable("max_allowed_packet",
                            gettext_noop("The upper limit of the size of a packet."),
                            NULL,
                            &GetSessionContext()->max_allowed_packet,
                            DEFAULT_MAX_ALLOWED_PACKET,
                            MIN_MAX_ALLOWED_PACKET,
                            MAX_MAX_ALLOWED_PACKET,
                            PGC_USERSET,
                            0,
                            check_max_allowed_packet,
                            NULL,
                            NULL);
    DefineCustomIntVariable("net_buffer_length",
                            gettext_noop("The default size of the buffer."),
                            NULL,
                            &GetSessionContext()->net_buffer_length,
                            DEFAULT_NET_BUFFER_LENGTH,
                            MIN_NET_BUFFER_LENGTH,
                            MAX_NET_BUFFER_LENGTH,
                            PGC_USERSET,
                            0,
                            check_net_buffer_length,
                            NULL,
                            NULL);
    DefineCustomIntVariable("net_write_timeout",
                            gettext_noop("After this number of seconds of waiting, "
                            "the server will abort the write."),
                            NULL,
                            &GetSessionContext()->net_write_timeout,
                            DEFAULT_NET_WRITE_TIMEOUT,
                            MIN_NET_WRITE_TIMEOUT,
                            MAX_NET_WRITE_TIMEOUT,
                            PGC_USERSET,
                            0,
                            check_net_write_timeout,
                            NULL,
                            NULL);
    DefineCustomInt64Variable("query_cache_size",
                            gettext_noop("When caching query results, this bytes of memory will be allocated."),
                            NULL,
                            &GetSessionContext()->query_cache_size,
                            DEFAULT_QUREY_CACHE_SIZE,
                            MIN_QUREY_CACHE_SIZE,
                            MAX_QUREY_CACHE_SIZE,
                            PGC_USERSET,
                            0,
                            check_query_cache_size,
                            NULL,
                            NULL);
    DefineCustomIntVariable("query_cache_type",
                            gettext_noop("The type of query cache."),
                            NULL,
                            &GetSessionContext()->query_cache_type,
                            QUERY_CACHE_OFF,
                            QUERY_CACHE_OFF,
                            QUERY_CACHE_DEMAND,
                            PGC_USERSET,
                            0,
                            check_query_cache_type,
                            NULL,
                            NULL);
    DefineCustomStringVariable("system_time_zone",
                               gettext_noop("The time zone set by server."),
                               NULL,
                               &GetSessionContext()->system_time_zone,
                               "",
                               PGC_USERSET,
                               0,
                               check_system_time_zone,
                               NULL,
                               NULL);
    DefineCustomStringVariable("time_zone",
                               gettext_noop("The time zone set currently."),
                               NULL,
                               &GetSessionContext()->time_zone,
                               "SYSTEM",
                               PGC_USERSET,
                               0,
                               check_time_zone,
                               NULL,
                               NULL);
    DefineCustomIntVariable("wait_timeout",
                            gettext_noop("After this number of seconds of inactivity, "
                            "the server will close the noninteractive connection."),
                            NULL,
                            &GetSessionContext()->wait_timeout,
                            DEFAULT_WAIT_TIMEOUT,
                            MIN_WAIT_TIMEOUT,
                            MAX_WAIT_TIMEOUT,
                            PGC_USERSET,
                            0,
                            check_wait_timeout,
                            NULL,
                            NULL);
    DefineCustomStringVariable("dolphin.optimizer_switch",
                               gettext_noop("Control over optimizer behavior."),
                               NULL,
                               &GetSessionContext()->optimizer_switch_string,
                               "default",
                               PGC_USERSET,
                               0,
                               check_optimizer_switch,
                               assign_optimizer_switch,
                               NULL);
    DefineCustomIntVariable("dolphin.div_precision_increment",
                            gettext_noop("This variable indicates the number of digits by which to increase"
                            "the scale of the result of division operations performed with the \"/\" operator."),
                            NULL,
                            &GetSessionContext()->div_precision_increment,
                            DEFAULT_DIV_PRECISION_INC,
                            MIN_DIV_PRECISION_INC,
                            MAX_DIV_PRECISION_INC,
                            PGC_USERSET,
                            0,
                            NULL,
                            NULL,
                            NULL);
    DefineCustomStringVariable("performance_schema",
                               gettext_noop("CUSTOM_OPTIONS"),
                               NULL,
                               &GetSessionContext()->performance_schema,
                               "",
                               PGC_USERSET,
                               GUC_LIST_INPUT | GUC_REPORT,
                               NULL,
                               NULL,
                               NULL);
#endif

}
static void load_dblink_extension()
{
    char* dest_str = "create extension if not exists dblink;\n";
    int rc = 0;

    SPI_STACK_LOG("connect", NULL, NULL);
    if ((rc = SPI_connect()) != SPI_OK_CONNECT) {
        ereport(ERROR, (errcode(ERRCODE_INTERNAL_ERROR),
            errmsg("dblink SPI_connect failed: %s", SPI_result_code_string(rc)),
            errdetail("SPI_connect failed"),
            errcause("System error."),
            erraction("Check whether the snapshot retry is successful")));
    }

    if (SPI_execute(dest_str, false, 0) != SPI_OK_UTILITY) {
        ereport(WARNING, (errcode(ERRCODE_DATA_EXCEPTION), errmsg("invalid query : %s", dest_str)));
    }
    SPI_STACK_LOG("finish", NULL, NULL);
    SPI_finish();
}


static void execute_sql_file()
{
    char* dest_str = "create extension dolphin;\n";
    int rc = 0;

    SPI_STACK_LOG("connect", NULL, NULL);
    if ((rc = SPI_connect()) != SPI_OK_CONNECT) {
        ereport(ERROR, (errcode(ERRCODE_INTERNAL_ERROR),
            errmsg("dolphin SPI_connect failed: %s", SPI_result_code_string(rc)),
            errdetail("SPI_connect failed"),
            errcause("System error."),
            erraction("Check whether the snapshot retry is successful")));
    }

    if (SPI_execute(dest_str, false, 0) != SPI_OK_UTILITY) {
        ereport(WARNING, (errcode(ERRCODE_DATA_EXCEPTION), errmsg("invalid query : %s", dest_str)));
    }
    SPI_STACK_LOG("finish", NULL, NULL);
    SPI_finish();
}

void create_dolphin_extension()
{
    if (u_sess->attr.attr_sql.dolphin) {
        return;
    }
    u_sess->hook_cxt.pluginCCHashEqFuncs = (void*)ccHashEqFuncs;
    start_xact_command();
    /*
    * change enable_full_encryption to false here to avoid SPI crush
    * when dealing with sql contains polymorphic type.
    */   
    bool pre_enable_full_encryption = u_sess->attr.attr_common.enable_full_encryption;
    u_sess->attr.attr_common.enable_full_encryption = false;
#ifndef ENABLE_LITE_MODE
    load_dblink_extension();
#endif
    execute_sql_file();
    u_sess->attr.attr_common.enable_full_encryption = pre_enable_full_encryption;
    finish_xact_command();
}

PGFunction SearchFuncByOid(Oid funcId)
{
    FmgrInfo* fmgrInfo = NULL;
    fmgrInfo = (FmgrInfo*)palloc0(sizeof(FmgrInfo));
    fmgr_info(funcId, fmgrInfo);
    PGFunction func = fmgrInfo->fn_addr;
    pfree(fmgrInfo);
    return func;
}

Datum dolphin_types()
{
    Datum* datums = NULL;
    ArrayType* dolphinTypesArray = NULL;
    int dimension = 2;
    int cstringLength = -2;
    datums = (Datum*)palloc(DOLPHIN_TYPES_NUM * TYPE_ATTRIBUTES_NUM * sizeof(Datum));
    for (int row = 0; row < DOLPHIN_TYPES_NUM; row++) {
        for (int col = 0; col < TYPE_ATTRIBUTES_NUM; col++) {
            datums[row * TYPE_ATTRIBUTES_NUM + col] = CStringGetDatum(dolphinTypes[row][col]);
        }
    }
    int dims[dimension];
    int lbs[dimension];
    dims[0] = DOLPHIN_TYPES_NUM;
    dims[1] = TYPE_ATTRIBUTES_NUM;
    lbs[0] = 1;
    lbs[1] = 1;

    dolphinTypesArray = construct_md_array(datums, NULL, dimension, dims, lbs, CSTRINGOID, cstringLength, false, 'c');
    pfree_ext(datums);
    PG_RETURN_ARRAYTYPE_P(dolphinTypesArray);
}

static void InitDolphinTypeId(BSqlPluginContext* cxt)
{
    cxt->uint1Oid = get_typeoid(PG_CATALOG_NAMESPACE, "uint1");
    cxt->uint2Oid = get_typeoid(PG_CATALOG_NAMESPACE, "uint2");
    cxt->uint4Oid = get_typeoid(PG_CATALOG_NAMESPACE, "uint4");
    cxt->uint8Oid = get_typeoid(PG_CATALOG_NAMESPACE, "uint8");
    cxt->binaryOid = get_typeoid(PG_CATALOG_NAMESPACE, "binary");
    cxt->varbinaryOid = get_typeoid(PG_CATALOG_NAMESPACE, "varbinary");
    cxt->tinyblobOid = get_typeoid(PG_CATALOG_NAMESPACE, "tinyblob");
    cxt->mediumblobOid = get_typeoid(PG_CATALOG_NAMESPACE, "mediumblob");
    cxt->longblobOid = get_typeoid(PG_CATALOG_NAMESPACE, "longblob");
    cxt->yearOid = get_typeoid(PG_CATALOG_NAMESPACE, "year");
}

static void InitUintOprs(BSqlPluginContext* cxt)
{
    cxt->dolphin_oprs[AEXPR_PLUS_INT4][UINT_INT_OP] =
        binary_oper_exact_extern(list_make1(makeString("+")), cxt->uint4Oid, INT4OID, false);
    cxt->dolphin_oprs[AEXPR_MINUS_INT4][UINT_INT_OP] =
        binary_oper_exact_extern(list_make1(makeString("-")), cxt->uint4Oid, INT4OID, false);
    cxt->dolphin_oprs[AEXPR_MUL_INT4][UINT_INT_OP] =
        binary_oper_exact_extern(list_make1(makeString("*")), cxt->uint4Oid, INT4OID, false);
    cxt->dolphin_oprs[AEXPR_PLUS_INT8][UINT_INT_OP] =
        binary_oper_exact_extern(list_make1(makeString("+")), cxt->uint8Oid, INT8OID, false);
    cxt->dolphin_oprs[AEXPR_MINUS_INT8][UINT_INT_OP] =
        binary_oper_exact_extern(list_make1(makeString("-")), cxt->uint8Oid, INT8OID, false);
    cxt->dolphin_oprs[AEXPR_MUL_INT8][UINT_INT_OP] =
        binary_oper_exact_extern(list_make1(makeString("*")), cxt->uint8Oid, INT8OID, false);
    cxt->dolphin_oprs[AEXPR_PLUS_INT4][INT_UINT_OP] =
        binary_oper_exact_extern(list_make1(makeString("+")), INT4OID, cxt->uint4Oid, false);
    cxt->dolphin_oprs[AEXPR_MINUS_INT4][INT_UINT_OP] =
        binary_oper_exact_extern(list_make1(makeString("-")), INT4OID, cxt->uint4Oid, false);
    cxt->dolphin_oprs[AEXPR_MUL_INT4][INT_UINT_OP] =
        binary_oper_exact_extern(list_make1(makeString("*")), INT4OID, cxt->uint4Oid, false);
    cxt->dolphin_oprs[AEXPR_PLUS_INT8][INT_UINT_OP] =
        binary_oper_exact_extern(list_make1(makeString("+")), INT8OID, cxt->uint8Oid, false);
    cxt->dolphin_oprs[AEXPR_MINUS_INT8][INT_UINT_OP] =
        binary_oper_exact_extern(list_make1(makeString("-")), INT8OID, cxt->uint8Oid, false);
    cxt->dolphin_oprs[AEXPR_MUL_INT8][INT_UINT_OP] =
        binary_oper_exact_extern(list_make1(makeString("*")), INT8OID, cxt->uint8Oid, false);
    cxt->dolphin_oprs[AEXPR_PLUS_INT4][UINT_OP] =
        binary_oper_exact_extern(list_make1(makeString("+")), cxt->uint4Oid, cxt->uint4Oid, false);
    cxt->dolphin_oprs[AEXPR_MINUS_INT4][UINT_OP] =
        binary_oper_exact_extern(list_make1(makeString("-")), cxt->uint4Oid, cxt->uint4Oid, false);
    cxt->dolphin_oprs[AEXPR_MUL_INT4][UINT_OP] =
        binary_oper_exact_extern(list_make1(makeString("*")), cxt->uint4Oid, cxt->uint4Oid, false);
    cxt->dolphin_oprs[AEXPR_PLUS_INT8][UINT_OP] =
        binary_oper_exact_extern(list_make1(makeString("+")), cxt->uint8Oid, cxt->uint8Oid, false);
    cxt->dolphin_oprs[AEXPR_MINUS_INT8][UINT_OP] =
        binary_oper_exact_extern(list_make1(makeString("-")), cxt->uint8Oid, cxt->uint8Oid, false);
    cxt->dolphin_oprs[AEXPR_MUL_INT8][UINT_OP] =
        binary_oper_exact_extern(list_make1(makeString("*")), cxt->uint8Oid, cxt->uint8Oid, false);
}

static void InitDolphinOperator(BSqlPluginContext* cxt)
{
    cxt->dolphin_oprs[AEXPR_PLUS_INT4][INT_OP] = INT4PLOID;
    cxt->dolphin_oprs[AEXPR_PLUS_INT8][INT_OP] = INT8PLOID;
    cxt->dolphin_oprs[AEXPR_MINUS_INT4][INT_OP] = INT4MIOID;
    cxt->dolphin_oprs[AEXPR_MINUS_INT8][INT_OP] = INT8MIOID;
    cxt->dolphin_oprs[AEXPR_MUL_INT4][INT_OP] = INT4MULOID;
    cxt->dolphin_oprs[AEXPR_MUL_INT8][INT_OP] = INT8MULOID;
    cxt->dolphin_oprs[AEXPR_DIV_INT4][INT_OP] = NUMERICDIVOID;
    cxt->dolphin_oprs[AEXPR_DIV_INT8][INT_OP] = NUMERICDIVOID;

    cxt->dolphin_oprs[AEXPR_PLUS_INT4][REAL_OP] = FLOAT8PLOID;
    cxt->dolphin_oprs[AEXPR_PLUS_INT8][REAL_OP] = FLOAT8PLOID;
    cxt->dolphin_oprs[AEXPR_MINUS_INT4][REAL_OP] = FLOAT8MIOID;
    cxt->dolphin_oprs[AEXPR_MINUS_INT8][REAL_OP] = FLOAT8MIOID;
    cxt->dolphin_oprs[AEXPR_MUL_INT4][REAL_OP] = FLOAT8MULOID;
    cxt->dolphin_oprs[AEXPR_MUL_INT8][REAL_OP] = FLOAT8MULOID;
    cxt->dolphin_oprs[AEXPR_DIV_INT4][REAL_OP] = FLOAT8DIVOID;
    cxt->dolphin_oprs[AEXPR_DIV_INT8][REAL_OP] = FLOAT8DIVOID;

    cxt->dolphin_oprs[AEXPR_PLUS_INT4][DECIMAL_OP] = NUMERICADDOID;
    cxt->dolphin_oprs[AEXPR_PLUS_INT8][DECIMAL_OP] = NUMERICADDOID;
    cxt->dolphin_oprs[AEXPR_MINUS_INT4][DECIMAL_OP] = NUMERICSUBOID;
    cxt->dolphin_oprs[AEXPR_MINUS_INT8][DECIMAL_OP] = NUMERICSUBOID;
    cxt->dolphin_oprs[AEXPR_MUL_INT4][DECIMAL_OP] = NUMERICMULOID;
    cxt->dolphin_oprs[AEXPR_MUL_INT8][DECIMAL_OP] = NUMERICMULOID;
    cxt->dolphin_oprs[AEXPR_DIV_INT4][DECIMAL_OP] = NUMERICDIVOID;
    cxt->dolphin_oprs[AEXPR_DIV_INT8][DECIMAL_OP] = NUMERICDIVOID;

    cxt->dolphin_oprs[AEXPR_DIV_INT4][INT_UINT_OP] = NUMERICDIVOID;
    cxt->dolphin_oprs[AEXPR_DIV_INT8][INT_UINT_OP] = NUMERICDIVOID;
    cxt->dolphin_oprs[AEXPR_DIV_INT4][UINT_INT_OP] = NUMERICDIVOID;
    cxt->dolphin_oprs[AEXPR_DIV_INT8][UINT_INT_OP] = NUMERICDIVOID;
    cxt->dolphin_oprs[AEXPR_DIV_INT4][UINT_OP] = NUMERICDIVOID;
    cxt->dolphin_oprs[AEXPR_DIV_INT8][UINT_OP] = NUMERICDIVOID;
    InitUintOprs(cxt);
}

/* copy from openGauss-server's execUtils.cpp GetTypeZeroValue */
static Datum DolphinGetTypeZeroValue(Form_pg_attribute att_tup)
{
    Datum result;
    switch (att_tup->atttypid) {
        case TIMESTAMPOID: {
            result = (Datum)DirectFunctionCall3(timestamp_in, CStringGetDatum("now"), ObjectIdGetDatum(InvalidOid),
                                                Int32GetDatum(-1));
            break;
        }
        case TIMESTAMPTZOID: {
            result = clock_timestamp(NULL);
            break;
        }
        case TIMETZOID: {
            result = (Datum)DirectFunctionCall3(
                timetz_in, CStringGetDatum("00:00:00"), ObjectIdGetDatum(0), Int32GetDatum(-1));
            break;
        }
        case INTERVALOID: {
            result = (Datum)DirectFunctionCall3(
                interval_in, CStringGetDatum("00:00:00"), ObjectIdGetDatum(0), Int32GetDatum(-1));
            break;
        }
        case TINTERVALOID: {
            Datum epoch = (Datum)DirectFunctionCall1(timestamp_abstime, (TimestampGetDatum(SetEpochTimestamp())));
            result = (Datum)DirectFunctionCall2(mktinterval, epoch, epoch);
            break;
        }
        case SMALLDATETIMEOID: {
            result = (Datum)DirectFunctionCall3(
                smalldatetime_in, CStringGetDatum("1970-01-01 00:00:00"), ObjectIdGetDatum(0), Int32GetDatum(-1));
            break;
        }
        case DATEOID: {
            result = timestamp2date(SetEpochTimestamp());
            break;
        }
        case UUIDOID: {
            result = (Datum)DirectFunctionCall3(uuid_in, CStringGetDatum("00000000-0000-0000-0000-000000000000"),
                                                ObjectIdGetDatum(0), Int32GetDatum(-1));
            break;
        }
        case NAMEOID: {
            result = (Datum)DirectFunctionCall1(namein, CStringGetDatum(""));
            break;
        }
        case POINTOID: {
            result = (Datum)DirectFunctionCall1(point_in, CStringGetDatum("(0,0)"));
            break;
        }
        case PATHOID: {
            result = (Datum)DirectFunctionCall1(path_in, CStringGetDatum("0,0"));
            break;
        }
        case POLYGONOID: {
            result = (Datum)DirectFunctionCall1(poly_in, CStringGetDatum("(0,0)"));
            break;
        }
        case CIRCLEOID: {
            result = (Datum)DirectFunctionCall1(circle_in, CStringGetDatum("0,0,0"));
            break;
        }
        case LSEGOID:
        case BOXOID: {
            result = (Datum)DirectFunctionCall1(box_in, CStringGetDatum("0,0,0,0"));
            break;
        }
        case JSONOID: {
            result = (Datum)DirectFunctionCall1(json_in, CStringGetDatum("null"));
            break;
        }
        case JSONBOID: {
            result = (Datum)DirectFunctionCall1(jsonb_in, CStringGetDatum("null"));
            break;
        }
        case XMLOID: {
            result = (Datum)DirectFunctionCall1(xml_in, CStringGetDatum("null"));
            break;
        }
        case BITOID: {
            result = (Datum)DirectFunctionCall3(bit_in, CStringGetDatum(""), ObjectIdGetDatum(0), Int32GetDatum(-1));
            /* for dolphin, use att_tup's typmod, to do extra padding */
            result = (Datum)DirectFunctionCall2(bit, result, Int32GetDatum(att_tup->atttypmod));
            break;
        }
        case VARBITOID: {
            result = (Datum)DirectFunctionCall3(varbit_in, CStringGetDatum(""), ObjectIdGetDatum(0), Int32GetDatum(-1));
            break;
        }
        case NUMERICOID: {
            result =
                (Datum)DirectFunctionCall3(numeric_in, CStringGetDatum("0"), ObjectIdGetDatum(0), Int32GetDatum(0));
            break;
        }
        case CIDROID: {
            result = DirectFunctionCall1(cidr_in, CStringGetDatum("0.0.0.0"));
            break;
        }
        case INETOID: {
            result = DirectFunctionCall1(inet_in, CStringGetDatum("0.0.0.0"));
            break;
        }
        case MACADDROID: {
            result = (Datum)DirectFunctionCall1(macaddr_in, CStringGetDatum("00:00:00:00:00:00"));
            break;
        }
        case NUMRANGEOID:
        case INT8RANGEOID:
        case INT4RANGEOID: {
            Type targetType = typeidType(att_tup->atttypid);
            result = stringTypeDatum(targetType, "(0,0)", att_tup->atttypmod, true);
            ReleaseSysCache(targetType);
            break;
        }
        case TSRANGEOID:
        case TSTZRANGEOID: {
            Type targetType = typeidType(att_tup->atttypid);
            result = stringTypeDatum(targetType, "(1970-01-01 00:00:00,1970-01-01 00:00:00)", att_tup->atttypmod, true);
            ReleaseSysCache(targetType);
            break;
        }
        case DATERANGEOID: {
            Type targetType = typeidType(att_tup->atttypid);
            result = stringTypeDatum(targetType, "(1970-01-01,1970-01-01)", att_tup->atttypmod, true);
            ReleaseSysCache(targetType);
            break;
        }
        case HASH16OID: {
            Type targetType = typeidType(att_tup->atttypid);
            result = stringTypeDatum(targetType, "0", att_tup->atttypmod, true);
            ReleaseSysCache(targetType);
            break;
        }
        case HASH32OID: {
            Type targetType = typeidType(att_tup->atttypid);
            result = stringTypeDatum(targetType, "00000000000000000000000000000000", att_tup->atttypmod, true);
            ReleaseSysCache(targetType);
            break;
        }
        case TSVECTOROID: {
            Type targetType = typeidType(att_tup->atttypid);
            result = stringTypeDatum(targetType, "", att_tup->atttypmod, true);
            ReleaseSysCache(targetType);
            break;
        }
        case BPCHAROID: {
            /* for dolphin, bpchar should use att_tup's typmod, to do extea padding */
            result = (Datum)DirectFunctionCall3(bpcharin, CStringGetDatum(""),
                ObjectIdGetDatum(0), Int32GetDatum(att_tup->atttypmod));
            break;
        }
        default: {
            if (att_tup->atttypid == BINARYOID) {
                /* binary should use att_tup's typmod, to do extra padding */
                result = (Datum)DirectFunctionCall3(dolphin_binaryin, CStringGetDatum(""),
                    ObjectIdGetDatum(0), Int32GetDatum(att_tup->atttypmod));
                break;
            }
            bool typeIsVarlena = (!att_tup->attbyval) && (att_tup->attlen == -1);
            if (typeIsVarlena) {
                result = CStringGetTextDatum("");
            } else {
                result = (Datum)0;
            }
            break;
        }
    }
    return result;
}

