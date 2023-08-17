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
#include "commands/variable.h"
#include "utils/guc.h"
#include "plugin_mb/pg_wchar.h"
#include "utils/builtins.h"
#include "catalog/pg_collation.h"
#include "catalog/pg_synonym.h"
#include "catalog/gs_matview.h"
#include "catalog/gs_db_privilege.h"
#include "catalog/pg_extension.h"
#include "catalog/pg_operator.h"
#include "executor/spi_priv.h"
#include "tcop/utility.h"
#include "gs_ledger/ledger_utils.h"
#include "storage/lmgr.h"
#include "utils/inval.h"
#include "utils/lsyscache.h"
#include "utils/acl.h"
#include "utils/knl_catcache.h"
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
#include "replication/archive_walreceiver.h"
#include "plugin_vector/vecfunc.h"
#include "libpq/libpq.h"
#ifndef WIN32_ONLY_COMPILER
#include "dynloader.h"
#else
#include "port/dynloader/win32.h"
#endif

#define ENABLE_A_CMPT_MODE (GetSessionContext()->enableACmptMode)

/*
 * For loading plpgsql function.
 */
typedef struct {
    char* func_name;
    PGFunction func_addr;
} RegExternFunc;

PG_MODULE_MAGIC_PUBLIC;

extern void initASQLBuiltinFuncs();
extern struct HTAB* a_nameHash;
extern struct HTAB* a_oidHash;
extern RegExternFunc a_plpgsql_function_table[3];
extern bool isAllTempObjects(Node* parse_tree, const char* query_string, bool sent_to_remote);
extern void ts_check_feature_disable();
extern void ExecAlterDatabaseSetStmt(Node* parse_tree, const char* query_string, bool sent_to_remote);
extern void DoVacuumMppTable(VacuumStmt* stmt, const char* query_string, bool is_top_level, bool sent_to_remote);
extern bool IsVariableinBlackList(const char* name);
extern void ExecAlterRoleSetStmt(Node* parse_tree, const char* query_string, bool sent_to_remote);
extern bool check_plugin_function(Oid funcId);
extern PGFunction SearchFuncByOid(Oid funcId);
static void init_slots();
static const int LOADER_COL_BUF_CNT = 5;
static uint32 whale_index;
static bool global_hook_inited = false;

extern "C" DLL_PUBLIC void _PG_init(void);
extern "C" DLL_PUBLIC void _PG_fini(void);

PG_FUNCTION_INFO_V1_PUBLIC(whale_invoke);
void whale_invoke(void)
{
    ereport(DEBUG2, (errmsg("dummy function to let process load this library.")));
    return;
}

void init_plugin_object()
{
    if (!global_hook_inited) {
        g_instance.raw_parser_hook[DB_CMPT_A] = (void*)raw_parser;

        a_plpgsql_function_table[0] = {"plpgsql_call_handler", a_plpgsql_call_handler};
        a_plpgsql_function_table[1] = {"plpgsql_inline_handler", a_plpgsql_inline_handler};
        a_plpgsql_function_table[2] = {"plpgsql_validator", a_plpgsql_validator};

        global_hook_inited = true;
    }
    u_sess->hook_cxt.transformStmtHook = (void*)transformStmt;
    u_sess->hook_cxt.execInitExprHook = (void*)ExecInitExpr;
    u_sess->hook_cxt.computeHashHook  = (void*)compute_hash_default;
    u_sess->hook_cxt.aggSmpHook = (void*)check_plugin_function;
    u_sess->hook_cxt.standardProcessUtilityHook = (void*)standard_ProcessUtility;
    u_sess->hook_cxt.searchFuncHook = (void*)SearchFuncByOid;
    u_sess->hook_cxt.plannerHook = (void*)planner;
    u_sess->hook_cxt.pluginSearchCatHook = NULL;
    u_sess->hook_cxt.pluginCCHashEqFuncs = NULL;
    u_sess->hook_cxt.plpgsqlParserSetHook = NULL;
    u_sess->hook_cxt.coreYYlexHook = (void*)core_yylex;
    u_sess->hook_cxt.pluginProcDestReciverHook = NULL;
    u_sess->hook_cxt.pluginSpiReciverParamHook = NULL;
    u_sess->hook_cxt.pluginSpiExecuteMultiResHook = NULL;
    u_sess->hook_cxt.pluginMultiResExceptionHook = NULL;
}

void _PG_init(void)
{
    if (!u_sess->misc_cxt.process_shared_preload_libraries_in_progress && !DB_IS_CMPT(A_FORMAT)) {
        ereport(ERROR, (errmsg("Can't create whale extension since current database compatibility is not 'A'")));
    }
    if (a_oidHash == NULL || a_nameHash == NULL) {
        initASQLBuiltinFuncs();
    }

    if (g_instance.plugin_vec_func_cxt.vec_func_plugin[WHALE_VEC] == NULL) {
        InitGlobalVecFuncMap();
    }
    init_plugin_object();
}

void _PG_fini(void)
{
    hash_destroy(a_nameHash);
    hash_destroy(a_oidHash);
    g_instance.raw_parser_hook[DB_CMPT_A] = NULL;
}

static void execute_sql_file()
{
    char* dest_str = "create extension whale;\n";
    int rc = 0;

    SPI_STACK_LOG("connect", NULL, NULL);
    if ((rc = SPI_connect()) != SPI_OK_CONNECT) {
        ereport(ERROR, (errcode(ERRCODE_INTERNAL_ERROR),
            errmsg("whale SPI_connect failed: %s", SPI_result_code_string(rc)),
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

void create_whale_extension()
{
    if (u_sess->attr.attr_sql.whale) {
        return;
    }

    u_sess->hook_cxt.pluginCCHashEqFuncs = NULL;
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

static void init_orafce_vars(ASqlPluginContext* orafce_psc)
{
    orafce_psc->nls_date_format = NULL;
    orafce_psc->orafce_timezone = NULL;
    orafce_psc->orafce_varchar2_null_safe_concat = false;
/* pipe.c */
    orafce_psc->output_buffer = NULL;
    orafce_psc->input_buffer = NULL;
    orafce_psc->pipe_sid = 0;
    orafce_psc->pipes = NULL;
    orafce_psc->shmem_lockid = NULL;
/* putline.c */
    orafce_psc->is_server_output = false;
    orafce_psc->buffer = NULL;
    orafce_psc->buffer_size = 0;
    orafce_psc->buffer_len = 0;
    orafce_psc->buffer_get = 0;
/* convert.c */
    orafce_psc->orafce_Utf8ToServerConvProc = NULL;
/* datefce.c */
    orafce_psc->mru_weekdays = NULL;
/* file.c */
    orafce_psc->slotid = 0;
    init_slots();
/* others.c */
    orafce_psc->lc_collate_cache = NULL;
    orafce_psc->multiplication = 1;
    orafce_psc->def_locale = NULL;
/* plvdate.c */
    orafce_psc->use_easter = true;
    orafce_psc->use_great_friday = true;
    orafce_psc->include_start = true;
    orafce_psc->country_id = -1;
    orafce_psc->holidays_c = 0;
    orafce_psc->exceptions_c = 0;
/* plvlex.c */
    orafce_psc->__node = NULL;
    orafce_psc->__result = NULL;
/* sqlparse.y */
    orafce_psc->scanbuf = NULL;
    orafce_psc->scanbuflen = 0;
/* sqlscan.l */
    orafce_psc->xcdepth = 0;    /* depth of nesting in slash-star comments */
    orafce_psc->dolqstart = NULL;      /* current $foo$ quote start string */
    orafce_psc->extended_string = false;
/* plvsubst.c */
    orafce_psc->c_subst = NULL;
/* random.c */
    orafce_psc->seed = 0;
/* alert.c */
    orafce_psc->alert_events = NULL;
    orafce_psc->alert_locks = NULL;
    orafce_psc->alert_session_lock = NULL;
    orafce_psc->sensitivity = 250.0;
/* shmmc.c */
    orafce_psc->list_c = NULL;
    orafce_psc->list = NULL;
    orafce_psc->max_size = 0;
}

void init_session_vars(void)
{
    RepallocSessionVarsArrayIfNecessary();

    ASqlPluginContext *cxt = (ASqlPluginContext *) MemoryContextAlloc(u_sess->self_mem_cxt, sizeof(aSqlPluginContext));
    u_sess->attr.attr_common.extension_session_vars_array[whale_index] = cxt;
    cxt->enableACmptMode = false;

    init_orafce_vars(cxt);

    DefineCustomBoolVariable("whale.a_compatibility_mode",
                             "Enable o* behavior override opengauss's when collision happens.",
                             NULL,
                             &ENABLE_A_CMPT_MODE,
                             false,
                             PGC_USERSET,
                             0,
                             NULL, NULL, NULL);
    DefineCustomBoolVariable("whale.varchar2_null_safe_concat",
                            "Specify timezone used for sysdate function.",
                            NULL,
                            &(GetSessionContext()->orafce_varchar2_null_safe_concat),
                            false,
                            PGC_USERSET,
                            0,
                            NULL, NULL, NULL);
    DefineCustomStringVariable("whale.nls_date_format",
                            "Emulate orafce's date output behaviour.",
                            NULL,
                            &(GetSessionContext()->nls_date_format),
                            NULL,
                            PGC_USERSET,
                            0,
                            NULL, NULL, NULL);
    DefineCustomStringVariable("whale.timezone",
                            "Specify timezone used for sysdate function.",
                            NULL,
                            &(GetSessionContext()->orafce_timezone),
                            "GMT",
                            PGC_USERSET,
                            0,
                            check_timezone, NULL, NULL);
}

ASqlPluginContext* GetSessionContext()
{
    if (u_sess->attr.attr_common.extension_session_vars_array[whale_index] == NULL) {
        init_session_vars();
    }
    return (ASqlPluginContext *) u_sess->attr.attr_common.extension_session_vars_array[whale_index];
}

void set_extension_index(uint32 index)
{
    whale_index = index;
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

static void init_slots()
{
    for (int i = 0; i < MAX_SLOTS; i++) {
        GetSessionContext()->slots[i].file = NULL;
        GetSessionContext()->slots[i].id = 0;
    }
}
