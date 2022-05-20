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
};

static ExecNodes* assign_utility_stmt_exec_nodes(Node* parse_tree);

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
static bool need_full_dn_execution(const char* group_name);
static ExecNodes* GetFunctionNodes(Oid func_id);
static const int LOADER_COL_BUF_CNT = 5;
static uint32 dolphin_index;

PG_FUNCTION_INFO_V1_PUBLIC(dolphin_invoke);
void dolphin_invoke(void)
{
    ereport(DEBUG2, (errmsg("dummy function to let process load this library.")));
    return;
}

void init_plugin_object()
{
    u_sess->hook_cxt.transformStmtHook = (void*)transformStmt;
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

    g_instance.raw_parser_hook[DB_CMPT_B] = (void*)raw_parser;
    init_plugin_object();
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

static ExecNodes* GetNodeGroupExecNodes(Oid group_oid)
{
    ExecNodes* exec_nodes = NULL;
    Oid* members = NULL;
    int nmembers;
    bool need_full_dn = false;

    exec_nodes = makeNode(ExecNodes);

    if (!in_logic_cluster()) {
        char* group_name = get_pgxc_groupname(group_oid);
        if (group_name == NULL) {
            ereport(ERROR,
                (errmodule(MOD_EXECUTOR),
                    errcode(ERRCODE_DATA_EXCEPTION),
                    errmsg("computing nodegroup is not a valid group.")));
        }
        need_full_dn = need_full_dn_execution(group_name);
        pfree_ext(group_name);
    }

    if (need_full_dn) {
        exec_nodes->nodeList = GetAllDataNodes();
        return exec_nodes;
    }

    nmembers = get_pgxc_groupmembers_redist(group_oid, &members);

    exec_nodes->nodeList = GetNodeGroupNodeList(members, nmembers);

    pfree_ext(members);

    return exec_nodes;
}

static ExecNodes* GetFunctionNodes(Oid func_id)
{
    Oid goid;

    /* Fetching group_oid for given relation */
    goid = GetFunctionNodeGroupByFuncid(func_id);
    if (!OidIsValid(goid))
        return NULL;

    return GetNodeGroupExecNodes(goid);
}

static bool need_full_dn_execution(const char* group_name)
{
    if (in_logic_cluster()) {
        return false;
    }
    const char* redistribution_group_name = PgxcGroupGetInRedistributionGroup();
    const char* installation_group_name = PgxcGroupGetInstallationGroup();

    if ((installation_group_name && strcmp(group_name, installation_group_name) == 0) ||
        (redistribution_group_name && strcmp(group_name, redistribution_group_name) == 0)) {
        return true;
    }

    Oid group_oid = get_pgxc_groupoid(group_name, false);
    const char* group_parent_name = get_pgxc_groupparent(group_oid);
    if (group_parent_name != NULL) {
        if ((installation_group_name && strcmp(group_parent_name, installation_group_name) == 0) ||
            (redistribution_group_name && strcmp(group_parent_name, redistribution_group_name) == 0)) {
            return true;
        }
    }
    return false;
}

#ifdef PGXC

/*
 * Execute a Utility statement on nodes, including Coordinators
 * If the DDL is received from a remote Coordinator,
 * it is not possible to push down DDL to Datanodes
 * as it is taken in charge by the remote Coordinator.
 */
void ExecUtilityStmtOnNodes(const char* query_string, ExecNodes* nodes, bool sent_to_remote, bool force_auto_commit,
    RemoteQueryExecType exec_type, bool is_temp, Node* parse_tree)
{
    bool need_free_nodes = false;
    /*
     * @NodeGroup Support
     *
     * Binding necessary datanodes under exec_nodes to run utility, we put the logic here to
     * re-calculate the exec_nodes for target relation in some statements like VACUUM, GRANT,
     * as we don't want to put the similar logic in each swith-case branches in standard_ProcessUtility()
     */
    if (IS_PGXC_COORDINATOR && !IsConnFromCoord() && parse_tree && nodes == NULL && exec_type != EXEC_ON_COORDS) {
        nodes = assign_utility_stmt_exec_nodes(parse_tree);
        need_free_nodes = true;
    }

    /* single-user mode */
    if (!IsPostmasterEnvironment)
        return;

    /* gs_dump cn do not connect datanode */
    if (u_sess->attr.attr_common.application_name &&
        !strncmp(u_sess->attr.attr_common.application_name, "gs_dump", strlen("gs_dump")))
        return;

    /* Return if query is launched on no nodes */
    if (exec_type == EXEC_ON_NONE)
        return;

    /* Nothing to be done if this statement has been sent to the nodes */
    if (sent_to_remote)
        return;

    /*
     * If no Datanodes defined and the remote utility sent to DN, the query cannot
     * be launched
     */
    if ((exec_type == EXEC_ON_DATANODES || exec_type == EXEC_ON_ALL_NODES) && u_sess->pgxc_cxt.NumDataNodes == 0)
        ereport(ERROR,
            (errcode(ERRCODE_UNDEFINED_OBJECT),
                errmsg("No Datanode defined in cluster"),
                errhint("You need to define at least 1 Datanode with "
                        "CREATE NODE.")));

#ifdef ENABLE_MULTIPLE_NODES
    if (!IsConnFromCoord()) {
        RemoteQuery* step = makeNode(RemoteQuery);
        step->combine_type = COMBINE_TYPE_SAME;
        step->exec_nodes = nodes;
        step->sql_statement = pstrdup(query_string);
        step->force_autocommit = force_auto_commit;
        step->exec_type = exec_type;
        step->is_temp = is_temp;
        ExecRemoteUtility(step);
        pfree_ext(step->sql_statement);
        pfree_ext(step);
        if (need_free_nodes)
            FreeExecNodes(&nodes);
    }
#endif
}

/*
 * Execute a Utility statement on nodes, including Coordinators
 * If the DDL is received from a remote Coordinator,
 * it is not possible to push down DDL to Datanodes
 * as it is taken in charge by the remote Coordinator.
 */
void ExecUtilityStmtOnNodes_ParallelDDLMode(const char* query_string, ExecNodes* nodes, bool sent_to_remote,
    bool force_auto_commit, RemoteQueryExecType exec_type, bool is_temp, const char* first_exec_node, Node* parse_tree)
{
    /*
     * @NodeGroup Support
     *
     * Binding necessary datanodes under exec_nodes to run utility, we put the logic here to
     * re-calculate the exec_nodes for target relation in some statements like VACUUM, GRANT,
     * as we don't want to put the similar logic in each swith-case branches in standard_ProcessUtility()
     */
    if (IS_PGXC_COORDINATOR && !IsConnFromCoord() && parse_tree && nodes == NULL && exec_type != EXEC_ON_COORDS) {
        nodes = assign_utility_stmt_exec_nodes(parse_tree);
    }

    // single-user mode
    //
    if (!IsPostmasterEnvironment)
        return;

    /* gs_dump cn do not connect datanode */
    if (u_sess->attr.attr_common.application_name &&
        !strncmp(u_sess->attr.attr_common.application_name, "gs_dump", strlen("gs_dump")))
        return;

    /* Return if query is launched on no nodes */
    if (exec_type == EXEC_ON_NONE)
        return;

    /* Nothing to be done if this statement has been sent to the nodes */
    if (sent_to_remote)
        return;

    /*
     * If no Datanodes defined and the remote utility sent to DN, the query cannot
     * be launched
     */
    if ((exec_type == EXEC_ON_DATANODES || exec_type == EXEC_ON_ALL_NODES) && u_sess->pgxc_cxt.NumDataNodes == 0)
        ereport(ERROR,
            (errcode(ERRCODE_UNDEFINED_OBJECT),
                errmsg("No Datanode defined in cluster"),
                errhint("You need to define at least 1 Datanode with "
                        "CREATE NODE.")));

    if (!IsConnFromCoord()) {
        RemoteQuery* step = makeNode(RemoteQuery);
        step->combine_type = COMBINE_TYPE_SAME;
        step->exec_nodes = nodes;
        step->sql_statement = pstrdup(query_string);
        step->force_autocommit = force_auto_commit;
        step->exec_type = exec_type;
        step->is_temp = is_temp;
        ExecRemoteUtility_ParallelDDLMode(step, first_exec_node);
        pfree_ext(step->sql_statement);
        pfree_ext(step);
    }
}
#endif

static ExecNodes* assign_utility_stmt_exec_nodes(Node* parse_tree)
{
    ExecNodes* nodes = NULL;
    Oid rel_id = InvalidOid;

    AssertEreport(parse_tree, MOD_EXECUTOR, "parser tree is NULL");
    AssertEreport(IS_PGXC_COORDINATOR && !IsConnFromCoord(), MOD_EXECUTOR, "the node is not a CN node");

    /* Do special processing for nodegroup support */
    switch (nodeTag(parse_tree)) {
        case T_ReindexStmt: {
            ReindexStmt* stmt = (ReindexStmt*)parse_tree;
            if (stmt->relation) {
                rel_id = RangeVarGetRelid(stmt->relation, NoLock, false);
                nodes = RelidGetExecNodes(rel_id);
            }
            break;
        }
        case T_GrantStmt: {
            GrantStmt* stmt = (GrantStmt*)parse_tree;
            if (stmt->objects && stmt->objtype == ACL_OBJECT_RELATION && stmt->targtype == ACL_TARGET_OBJECT) {
                RangeVar* relvar = (RangeVar*)list_nth(stmt->objects, 0);
                Oid relation_id = RangeVarGetRelid(relvar, NoLock, false);
                nodes = RelidGetExecNodes(relation_id);
            }
            break;
        }
        case T_ClusterStmt: {
            ClusterStmt* stmt = (ClusterStmt*)parse_tree;
            if (stmt->relation) {
                Oid relation_id = RangeVarGetRelid(stmt->relation, NoLock, false);
                nodes = RelidGetExecNodes(relation_id);
            }
            break;
        }
        case T_LockStmt: {
            LockStmt* stmt = (LockStmt*)parse_tree;
            if (stmt->relations) {
                RangeVar* relvar = (RangeVar*)list_nth(stmt->relations, 0);
                Oid relation_id = RangeVarGetRelid(relvar, NoLock, false);
                nodes = RelidGetExecNodes(relation_id);
            }
            break;
        }
        case T_VacuumStmt: {
            VacuumStmt* stmt = (VacuumStmt*)parse_tree;
            if (stmt->relation) {
                Oid relation_id = RangeVarGetRelid(stmt->relation, NoLock, false);
                nodes = RelidGetExecNodes(relation_id);
            } else if (stmt->options & VACOPT_VERIFY) {
                nodes = RelidGetExecNodes(stmt->curVerifyRel);
            }
            break;
        }
        case T_RenameStmt: {
            RenameStmt* stmt = (RenameStmt*)parse_tree;
            if (stmt->relation) {
                Oid relation_id = RangeVarGetRelid(stmt->relation, NoLock, stmt->missing_ok);
                nodes = RelidGetExecNodes(relation_id);
            } else if (stmt->renameType == OBJECT_FUNCTION) {
                Oid funcid = LookupFuncNameTypeNames(stmt->object, stmt->objarg, false);

                nodes = GetFunctionNodes(funcid);
            }
            break;
        }
        case T_AlterObjectSchemaStmt: {
            AlterObjectSchemaStmt* stmt = (AlterObjectSchemaStmt*)parse_tree;
            if (stmt->relation) {
                rel_id = RangeVarGetRelid(stmt->relation, NoLock, true);
                nodes = RelidGetExecNodes(rel_id);
            } else if (stmt->objectType == OBJECT_FUNCTION) {
                Oid funcid = LookupFuncNameTypeNames(stmt->object, stmt->objarg, false);

                nodes = GetFunctionNodes(funcid);
            }

            break;
        }
        case T_CreateTrigStmt: {
            CreateTrigStmt* stmt = (CreateTrigStmt*)parse_tree;
            if (stmt->relation) {
                /*
                 * Notice : When support create or replace trigger in future,
                 * we may need adjust the missing_ok parameter here.
                 */
                rel_id = RangeVarGetRelidExtended(stmt->relation, NoLock, false, false, false, true, NULL, NULL);
                nodes = RelidGetExecNodes(rel_id);
            }
            break;
        }
        default: {
            ereport(ERROR,
                (errcode(ERRCODE_UNRECOGNIZED_NODE_TYPE),
                    errmsg("unrecognized nodes %s in node group utility execution", nodeTagToString(parse_tree->type))));
        }
    }

    return nodes;
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
    cxt->lockNameList = NIL;

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
                               "sql_mode_strict",
                               PGC_USERSET,
                               GUC_LIST_INPUT | GUC_REPORT,
                               CheckSqlMode,
                               AssignSqlMode,
                               NULL);
}