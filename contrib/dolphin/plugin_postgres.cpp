#include "postgres.h"
#include "plugin_parser/parser.h"
#include "plugin_parser/analyze.h"
#include "plugin_storage/hash.h"
#include "plugin_postgres.h"
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
#include "executor/spi_priv.h"
#include "tcop/utility.h"
#include "gs_ledger/ledger_utils.h"
#include "storage/lmgr.h"
#include "utils/inval.h"
#include "utils/lsyscache.h"
#include "utils/acl.h"
#include "pgxc/groupmgr.h"
#include "libpq/pqformat.h"
#include "optimizer/nodegroups.h"
#include "optimizer/pgxcplan.h"
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
};

PG_MODULE_MAGIC_PUBLIC;

extern void InitLockNameHash();
extern struct HTAB* lockNameHash;
extern pthread_mutex_t gNameHashLock;
extern void initBSQLBuiltinFuncs();
extern struct HTAB* b_nameHash;
extern struct HTAB* b_oidHash;
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
static bool check_default_week_format(int* newval, void** extra, GucSource source);
static void assign_default_week_format(int newval, void* extra);
static bool check_lc_time_names(char** newval, void** extra, GucSource source);
#endif
static const int LOADER_COL_BUF_CNT = 5;
static uint32 dolphin_index;
extern void set_hypopg_prehook(ProcessUtility_hook_type func);
extern void set_pgaudit_prehook(ProcessUtility_hook_type func);
extern bool check_plugin_function(Oid funcId);

PG_FUNCTION_INFO_V1_PUBLIC(dolphin_invoke);
void dolphin_invoke(void)
{
    ereport(DEBUG2, (errmsg("dummy function to let process load this library.")));
    return;
}

void set_default_guc()
{
    set_config_option("behavior_compat_options", "display_leading_zero", PGC_USERSET, PGC_S_SESSION, GUC_ACTION_SET, true, 0, false);
    set_config_option("enable_custom_parser", "true", PGC_USERSET, PGC_S_SESSION, GUC_ACTION_SET, true, 0, false);
}

void init_plugin_object()
{
    u_sess->hook_cxt.transformStmtHook = (void*)transformStmt;
    u_sess->hook_cxt.execInitExprHook = (void*)ExecInitExpr;
    u_sess->hook_cxt.computeHashHook  = (void*)compute_hash_default;
    u_sess->hook_cxt.aggSmpHook = (void*)check_plugin_function;
    u_sess->hook_cxt.standardProcessUtilityHook = (void*)standard_ProcessUtility;
    set_default_guc();
}

void _PG_init(void)
{
    if (!u_sess->misc_cxt.process_shared_preload_libraries_in_progress && !DB_IS_CMPT(B_FORMAT)) {
        ereport(ERROR, (errmsg("Can't create dolphin extension since current database compatibility is not 'B'")));
    }
    if (b_oidHash == NULL || b_nameHash == NULL) {
        initBSQLBuiltinFuncs();
    }

    AutoMutexLock nameHashLock(&gNameHashLock);
    nameHashLock.lock();
    if (lockNameHash == NULL)
        InitLockNameHash();
    nameHashLock.unLock();

    if (g_instance.plugin_vec_func_cxt.vec_func_plugin[DOLPHIN_VEC] == NULL) {
        InitGlobalVecFuncMap();
    }
    g_instance.raw_parser_hook[DB_CMPT_B] = (void*)raw_parser;
    g_instance.plsql_parser_hook[DB_CMPT_B] = (void*)plpgsql_yyparse;
    g_instance.llvmIrFilePath[DB_CMPT_B] = "share/postgresql/extension/openGauss_expr_dolphin.ir";
}

void _PG_fini(void)
{
    hash_destroy(b_nameHash);
    hash_destroy(b_oidHash);
    g_instance.raw_parser_hook[DB_CMPT_B] = NULL;
    g_instance.llvmIrFilePath[DB_CMPT_B] = NULL;
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
#endif

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

void init_session_vars(void)
{
    RepallocSessionVarsArrayIfNecessary();

    BSqlPluginContext *cxt = (BSqlPluginContext *) MemoryContextAlloc(u_sess->self_mem_cxt, sizeof(bSqlPluginContext));
    u_sess->attr.attr_common.extension_session_vars_array[dolphin_index] = cxt;
    cxt->enableBCmptMode = false;
    cxt->lockNameList = NIL;
    cxt->scan_from_pl = false;

    DefineCustomBoolVariable("b_compatibility_mode",
                             "Enable mysql behavior override opengauss's when collision happens.",
                             NULL,
                             &ENABLE_B_CMPT_MODE,
                             false,
                             PGC_USERSET,
                             0,
                             NULL, NULL, NULL);

    DefineCustomStringVariable("sql_mode",
                               gettext_noop("CUSTOM_OPTIONS"),
                               NULL,
                               &GetSessionContext()->sqlModeString,
                               "sql_mode_strict,sql_mode_full_group,pipes_as_concat",
                               PGC_USERSET,
                               GUC_LIST_INPUT | GUC_REPORT,
                               CheckSqlMode,
                               AssignSqlMode,
                               NULL);
    DefineCustomRealVariable("b_db_timestamp",
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
    DefineCustomIntVariable("lower_case_table_names",
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
    DefineCustomIntVariable("default_week_format",
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
    DefineCustomStringVariable("lc_time_names",
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
#endif

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
    start_xact_command();
    /*
    * change enable_full_encryption to false here to avoid SPI crush
    * when dealing with sql contains polymorphic type.
    */   
    bool pre_enable_full_encryption = u_sess->attr.attr_common.enable_full_encryption;
    u_sess->attr.attr_common.enable_full_encryption = false;
    execute_sql_file();
    u_sess->attr.attr_common.enable_full_encryption = pre_enable_full_encryption;
    finish_xact_command();
}
