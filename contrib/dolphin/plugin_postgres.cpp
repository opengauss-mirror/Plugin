#include "postgres.h"
#include "plugin_parser/parser.h"
#include "plugin_parser/analyze.h"
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
#include "plugin_utils/vecfunc_plugin.h"
#include "replication/archive_walreceiver.h"
#include "plugin_commands/mysqlmode.h"
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
};

PG_MODULE_MAGIC_PUBLIC;

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
static const int LOADER_COL_BUF_CNT = 5;
static uint32 dolphin_index;
extern void set_hypopg_prehook(ProcessUtility_hook_type func);
extern void set_pgaudit_prehook(ProcessUtility_hook_type func);

void ProcessUtilityMain(Node* parse_tree, const char* query_string, ParamListInfo params, bool is_top_level,
    DestReceiver* dest,
#ifdef PGXC
    bool sent_to_remote,
#endif /* PGXC */
    char* completion_tag,
    bool isCTAS) {
    if (u_sess->attr.attr_sql.dolphin) {
        return ProcessUtility(parse_tree,
            query_string,
            params,
            is_top_level,
            dest,
#ifdef PGXC
            sent_to_remote,
#endif /* PGXC */
            completion_tag,
            isCTAS);
    } else {
        return standard_ProcessUtility(parse_tree,
            query_string,
            params,
            is_top_level,
            dest,
#ifdef PGXC
            sent_to_remote,
#endif /* PGXC */
            completion_tag,
            isCTAS);
    }
}

/* 
 * Set ProcessUtility PreHook of other ProcessUtilityHook Users, so that
 * we can hook standard_ProcessUtility.
 */
void set_processutility_prehook()
{
    DynamicFileList* file_scanner = NULL;
    void (*set_gsaudit_prehook)(ProcessUtility_hook_type);

    char* securityPluginPath = expand_dynamic_library_name("security_plugin");
    check_backend_env(securityPluginPath);

    for (file_scanner = file_list; file_scanner != NULL && strcmp(securityPluginPath, file_scanner->filename) != 0;
         file_scanner = file_scanner->next);

    if (file_scanner != NULL) {
        set_gsaudit_prehook = (void(*)(ProcessUtility_hook_type))pg_dlsym(file_scanner->handle, "set_gsaudit_prehook");
        if (set_gsaudit_prehook != NULL) {
            (*set_gsaudit_prehook)((ProcessUtility_hook_type)ProcessUtilityMain);
            return;
        }
    }

    if (u_sess->attr.attr_security.Audit_enabled) {
        set_pgaudit_prehook((ProcessUtility_hook_type)ProcessUtilityMain);
    } else {
        set_hypopg_prehook((ProcessUtility_hook_type)ProcessUtilityMain);
    }
}

PG_FUNCTION_INFO_V1_PUBLIC(dolphin_invoke);
void dolphin_invoke(void)
{
    ereport(DEBUG2, (errmsg("dummy function to let process load this library.")));
    return;
}

void set_default_guc()
{
    set_config_option("behavior_compat_options", "display_leading_zero", PGC_USERSET, PGC_S_SESSION, GUC_ACTION_SAVE, true, 0, false);
}

void init_plugin_object()
{
    u_sess->hook_cxt.transformStmtHook = (void*)transformStmt;
    set_processutility_prehook();
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
    g_instance.raw_parser_hook[DB_CMPT_B] = (void*)raw_parser;
    init_plugin_object();
    InitVecsubarrayPlugin();
}

void _PG_fini(void)
{
    hash_destroy(b_nameHash);
    hash_destroy(b_oidHash);
    g_instance.raw_parser_hook[DB_CMPT_B] = NULL;
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
    cxt->enableBFormatMode = false;

    DefineCustomBoolVariable("b_format_mode",
                             "Enable mysql functions override opengauss's when collision happens.",
                             NULL,
                             &ENABLE_B_FORMAT_MODE,
                             false,
                             PGC_USERSET,
                             0,
                             NULL, NULL, NULL);

    DefineCustomStringVariable("sql_mode",
                               gettext_noop("CUSTOM_OPTIONS"),
                               NULL,
                               &GetSessionContext()->sqlModeString,
                               "sql_mode_strict,sql_mode_full_group",
                               PGC_USERSET,
                               GUC_LIST_INPUT | GUC_REPORT,
                               CheckSqlMode,
                               AssignSqlMode,
                               NULL);
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
