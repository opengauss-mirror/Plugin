#include "postgres.h"
#include "plugin_parser/parser.h"
#include "plugin_parser/analyze.h"
#include "plugin_postgres.h"
#include "plugin_commands/extension.h"
#include "plugin_commands/dbcommands.h"
#include "plugin_commands/copy.h"
#include "plugin_commands/tablespace.h"
#include "plugin_commands/comment.h"
#include "plugin_commands/prepare.h"
#include "plugin_commands/alter.h"
#include "plugin_commands/directory.h"
#include "plugin_commands/user.h"
#include "plugin_commands/conversioncmds.h"
#include "plugin_commands/discard.h"
#include "plugin_commands/lockcmds.h"
#include "plugin_commands/typecmds.h"
#include "plugin_commands/shutdown.h"
#include "plugin_commands/proclang.h"
#include "plugin_commands/async.h"
#include "plugin_commands/sequence.h"
#include "plugin_commands/createas.h"
#include "plugin_commands/view.h"
#include "plugin_commands/portalcmds.h"
#include "plugin_commands/collationcmds.h"
#include "plugin_commands/schemacmds.h"
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
#ifndef WIN32_ONLY_COMPILER
#include "dynloader.h"
#else
#include "port/dynloader/win32.h"
#endif

static RemoteQueryExecType ExecUtilityFindNodes(ObjectType object_type, Oid rel_id, bool* is_temp);
static RemoteQueryExecType exec_utility_find_nodes_relkind(Oid rel_id, bool* is_temp);
static RemoteQueryExecType get_nodes_4_comment_utility(CommentStmt* stmt, bool* is_temp, ExecNodes** exec_nodes);
static RemoteQueryExecType get_nodes_4_rules_utility(RangeVar* relation, bool* is_temp);
static void drop_stmt_pre_treatment(
    DropStmt* stmt, const char* query_string, bool sent_to_remote, bool* is_temp, RemoteQueryExecType* exec_type);
static void ExecUtilityWithMessage(const char* query_string, bool sent_to_remote, bool is_temp);

static void exec_utility_with_message_parallel_ddl_mode(
    const char* query_string, bool sent_to_remote, bool is_temp, const char* first_exec_node, RemoteQueryExecType exec_type);
static bool is_stmt_allowed_in_locked_mode(Node* parse_tree, const char* query_string);

static ExecNodes* assign_utility_stmt_exec_nodes(Node* parse_tree);

PG_MODULE_MAGIC_PUBLIC;

extern void initASQLBuiltinFuncs();
extern struct HTAB* a_nameHash;
extern struct HTAB* a_oidHash;
extern void set_hypopg_prehook(ProcessUtility_hook_type func);
extern void set_pgaudit_prehook(ProcessUtility_hook_type func);
extern bool isAllTempObjects(Node* parse_tree, const char* query_string, bool sent_to_remote);
extern void ts_check_feature_disable();
extern void ExecAlterDatabaseSetStmt(Node* parse_tree, const char* query_string, bool sent_to_remote);
extern void DoVacuumMppTable(VacuumStmt* stmt, const char* query_string, bool is_top_level, bool sent_to_remote);
extern bool IsVariableinBlackList(const char* name);
extern void ExecAlterRoleSetStmt(Node* parse_tree, const char* query_string, bool sent_to_remote);
static bool need_full_dn_execution(const char* group_name);
static ExecNodes* GetFunctionNodes(Oid func_id);
static const int LOADER_COL_BUF_CNT = 5;
void ProcessUtilityMain(Node* parse_tree, const char* query_string, ParamListInfo params, bool is_top_level,
    DestReceiver* dest,
#ifdef PGXC
    bool sent_to_remote,
#endif /* PGXC */
    char* completion_tag,
    bool isCTAS);
void asql_ProcessUtility(Node* parse_tree, const char* query_string, ParamListInfo params, bool is_top_level,
    DestReceiver* dest,
#ifdef PGXC
    bool sent_to_remote,
#endif /* PGXC */
    char* completion_tag,
    bool isCTAS);

PG_FUNCTION_INFO_V1_PUBLIC(whale_invoke);
void whale_invoke(void)
{
    ereport(DEBUG2, (errmsg("dummy function to let process load this library.")));
    return;
}

void ProcessUtilityMain(Node* parse_tree, const char* query_string, ParamListInfo params, bool is_top_level,
    DestReceiver* dest,
#ifdef PGXC
    bool sent_to_remote,
#endif /* PGXC */
    char* completion_tag,
    bool isCTAS) {
    if (DB_IS_CMPT(A_FORMAT) && CheckIfExtensionExists("whale")) {
        return asql_ProcessUtility(parse_tree,
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

void init_plugin_object()
{
    u_sess->hook_cxt.transformStmtHook = (void*)transformStmt;
    set_processutility_prehook();
}

void _PG_init(void)
{
    if (!u_sess->misc_cxt.process_shared_preload_libraries_in_progress && !DB_IS_CMPT(A_FORMAT)) {
        ereport(ERROR, (errmsg("Can't create whale extension since current database compatibility is not 'A'")));
    }
    if (a_oidHash == NULL || a_nameHash == NULL) {
        initASQLBuiltinFuncs();
    }
    g_instance.raw_parser_hook[DB_CMPT_A] = (void*)raw_parser;
    init_plugin_object();
}

void _PG_fini(void)
{
    hash_destroy(a_nameHash);
    hash_destroy(a_oidHash);
    g_instance.raw_parser_hook[DB_CMPT_A] = NULL;
}

#ifdef PGXC
/*
 * GetCommentObjectId
 * Change to return the nodes to execute the utility on in future.
 *
 * Return Object ID of object commented
 * Note: This function uses portions of the code of CommentObject,
 * even if this code is duplicated this is done like this to facilitate
 * merges with PostgreSQL head.
 */
static RemoteQueryExecType get_nodes_4_comment_utility(CommentStmt* stmt, bool* is_temp, ExecNodes** exec_nodes)
{
    ObjectAddress address;
    Relation relation;
    RemoteQueryExecType exec_type = EXEC_ON_ALL_NODES; /* By default execute on all nodes */
    Oid object_id;

    if (exec_nodes != NULL)
        *exec_nodes = NULL;

    if (stmt->objtype == OBJECT_DATABASE && list_length(stmt->objname) == 1) {
        char* database = strVal(linitial(stmt->objname));
        if (!OidIsValid(get_database_oid(database, true)))
            ereport(WARNING, (errcode(ERRCODE_UNDEFINED_DATABASE), errmsg("database \"%s\" does not exist", database)));
        /* No clue, return the default one */
        return exec_type;
    }

    address =
        get_object_address(stmt->objtype, stmt->objname, stmt->objargs, &relation, ShareUpdateExclusiveLock, false);
    object_id = address.objectId;

    /*
     * If the object being commented is a rule, the nodes are decided by the
     * object to which rule is applicable, so get the that object's oid
     */
    if (stmt->objtype == OBJECT_RULE) {
        if (!relation || !OidIsValid(relation->rd_id)) {
            /* This should not happen, but prepare for the worst */
            char* rulename = strVal(llast(stmt->objname));
            ereport(WARNING,
                (errcode(ERRCODE_UNDEFINED_OBJECT),
                    errmsg("can not find relation for rule \"%s\" does not exist", rulename)));
            object_id = InvalidOid;
        } else {
            object_id = RelationGetRelid(relation);
            if (exec_nodes != NULL) {
                *exec_nodes = RelidGetExecNodes(object_id, false);
            }
        }
    }

    if (stmt->objtype == OBJECT_CONSTRAINT) {
        if (!relation || !OidIsValid(relation->rd_id)) {
            /* This should not happen, but prepare for the worst */
            char* constraintname = strVal(llast(stmt->objname));
            ereport(WARNING,
                (errcode(ERRCODE_UNDEFINED_OBJECT),
                    errmsg("can not find relation for constraint \"%s\" does not exist", constraintname)));
            object_id = InvalidOid;
        } else {
            object_id = RelationGetRelid(relation);
            if (exec_nodes != NULL) {
                *exec_nodes = RelidGetExecNodes(object_id, false);
            }
        }
    }

    if (relation != NULL)
        relation_close(relation, NoLock);

    /* Commented object may not have a valid object ID, so move to default */
    if (OidIsValid(object_id)) {
        exec_type = ExecUtilityFindNodes(stmt->objtype, object_id, is_temp);
        if (exec_nodes != NULL && (exec_type == EXEC_ON_DATANODES || exec_type == EXEC_ON_ALL_NODES)) {
            switch (stmt->objtype) {
                case OBJECT_TABLE:
                case OBJECT_FOREIGN_TABLE:
                case OBJECT_STREAM:
                case OBJECT_INDEX:
                case OBJECT_COLUMN:
                    *exec_nodes = RelidGetExecNodes(object_id, false);
                    break;
                case OBJECT_FUNCTION:
                    *exec_nodes = GetFunctionNodes(object_id);
                    break;
                default:
                    break;
            }
        }
    }
    return exec_type;
}

/*
 * get_nodes_4_rules_utility
 * Get the nodes to execute this RULE related utility statement.
 * A rule is expanded on Coordinator itself, and does not need any
 * existence on Datanode. In fact, if it were to exist on Datanode,
 * there is a possibility that it would expand again
 */
static RemoteQueryExecType get_nodes_4_rules_utility(RangeVar* relation, bool* is_temp)
{
    Oid rel_id = RangeVarGetRelid(relation, NoLock, true);
    RemoteQueryExecType exec_type;

    /* Skip if this Oid does not exist */
    if (!OidIsValid(rel_id))
        return EXEC_ON_NONE;

    /*
     * See if it's a temporary object, do we really need
     * to care about temporary objects here? What about the
     * temporary objects defined inside the rule?
     */
    exec_type = ExecUtilityFindNodes(OBJECT_RULE, rel_id, is_temp);
    return exec_type;
}

/*
 * TreatDropStmtOnCoord
 * Do a pre-treatment of Drop statement on a remote Coordinator
 */
static void drop_stmt_pre_treatment(
    DropStmt* stmt, const char* query_string, bool sent_to_remote, bool* is_temp, RemoteQueryExecType* exec_type)
{
    bool res_is_temp = false;
    RemoteQueryExecType res_exec_type = EXEC_ON_ALL_NODES;

#ifndef ENABLE_MULTIPLE_NODES
    switch (stmt->removeType) {
        case OBJECT_TABLE: {
            /*
             * Check whether the tables are in blockchain schema.
             */
            ListCell* cell = NULL;
            foreach (cell, stmt->objects) {
                RangeVar* rel = makeRangeVarFromNameList((List*)lfirst(cell));
                Oid rel_id = RangeVarGetRelid(rel, AccessShareLock, true);
                if (is_ledger_hist_table(rel_id)) {
                    ereport(ERROR,
                        (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                            errmsg("DROP not supported for userchain table.")));
                }
                if (OidIsValid(rel_id)) {
                    UnlockRelationOid(rel_id, AccessShareLock);
                }
            }
        } break;
        case OBJECT_SCHEMA: {
            ListCell* cell = NULL;
            foreach (cell, stmt->objects) {
                List* objname = (List*)lfirst(cell);
                char* name = NameListToString(objname);

                if (get_namespace_oid(name, true) == PG_BLOCKCHAIN_NAMESPACE) {
                    ereport(ERROR,
                        (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                            errmsg("DROP not supported for blockchain schema.")));
                } else if (!u_sess->attr.attr_common.IsInplaceUpgrade &&
                    get_namespace_oid(name, true) == PG_SQLADVISOR_NAMESPACE) {
                    ereport(ERROR,
                        (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                            errmsg("DROP not supported for sqladvisor schema.")));
                } else if (!u_sess->attr.attr_common.IsInplaceUpgrade && IsPackageSchemaName(name)) {
                    ereport(ERROR,
                        (errmodule(MOD_COMMAND), errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                            errmsg("DROP not supported for %s schema.", name),
                            errdetail("Schemas in a package cannot be deleted."),
                            errcause("The schema in the package does not support object drop."),
                            erraction("N/A")));
                }
            }
        } break;
        default:
            break;
    }
#endif

    /* Nothing to do if not local Coordinator */
    if (IS_PGXC_DATANODE || IsConnFromCoord())
        return;

    /*
     * Check for shared-cache-inval messages before trying to access the
     * relation.  This is needed to cover the case where the name
     * identifies a rel that has been dropped and recreated since the
     * start of our transaction: if we don't flush the old syscache entry,
     * then we'll latch onto that entry and suffer an error later.
     */
    AcceptInvalidationMessages();

    switch (stmt->removeType) {
        case OBJECT_TABLE:
        case OBJECT_SEQUENCE:
        case OBJECT_LARGE_SEQUENCE:
        case OBJECT_VIEW:
        case OBJECT_MATVIEW:
        case OBJECT_CONTQUERY:
        case OBJECT_INDEX: {
            /*
             * Check the list of objects going to be dropped.
             * XC does not allow yet to mix drop of temporary and
             * non-temporary objects because this involves to rewrite
             * query to process for tables.
             */
            ListCell* cell = NULL;
            bool is_first = true;

            foreach (cell, stmt->objects) {
                RangeVar* rel = makeRangeVarFromNameList((List*)lfirst(cell));
                Oid rel_id;

                /*
                 * Do not print result at all, error is thrown
                 * after if necessary
                 * Notice : need get lock here to forbid parallel drop
                 */
                rel_id = RangeVarGetRelid(rel, AccessShareLock, true);

                /*
                 * In case this relation ID is incorrect throw
                 * a correct DROP error.
                 */
                if (!OidIsValid(rel_id) && !stmt->missing_ok)
                    DropTableThrowErrorExternal(rel, stmt->removeType, stmt->missing_ok);

                /* In case of DROP ... IF EXISTS bypass */
                if (!OidIsValid(rel_id) && stmt->missing_ok)
                    continue;

                if (is_ledger_hist_table(rel_id)) {
                    ereport(ERROR,
                        (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                            errmsg("DROP not supported for userchain table.")));
                }

                if (is_first) {
                    res_exec_type = ExecUtilityFindNodes(stmt->removeType, rel_id, &res_is_temp);
                    is_first = false;
                } else {
                    RemoteQueryExecType exec_type_loc;
                    bool is_temp_loc = false;
                    exec_type_loc = ExecUtilityFindNodes(stmt->removeType, rel_id, &is_temp_loc);
                    if (exec_type_loc != res_exec_type || is_temp_loc != res_is_temp)
                        ereport(ERROR,
                            (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                errmsg("DROP not supported for TEMP and non-TEMP objects"),
                                errdetail("You should separate TEMP and non-TEMP objects")));
                }
                UnlockRelationOid(rel_id, AccessShareLock);
            }
        } break;

        case OBJECT_SCHEMA: {
            ListCell* cell = NULL;
            bool has_temp = false;
            bool has_nontemp = false;

            foreach (cell, stmt->objects) {
                List* objname = (List*)lfirst(cell);
                char* name = NameListToString(objname);
                if (isTempNamespaceName(name) || isToastTempNamespaceName(name)) {
                    has_temp = true;
                    res_exec_type = EXEC_ON_DATANODES;
                } else {
                    has_nontemp = true;
                }

                if (get_namespace_oid(name, true) == PG_BLOCKCHAIN_NAMESPACE) {
                    ereport(ERROR,
                        (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                            errmsg("DROP not supported for blockchain schema.")));
                } else if (!u_sess->attr.attr_common.IsInplaceUpgrade &&
                    get_namespace_oid(name, true) == PG_SQLADVISOR_NAMESPACE) {
                    ereport(ERROR,
                        (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                            errmsg("DROP not supported for sqladvisor schema.")));
                } else if (!u_sess->attr.attr_common.IsInplaceUpgrade && IsPackageSchemaName(name)) {
                    ereport(ERROR,
                        (errmodule(MOD_COMMAND), errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                            errmsg("DROP not supported for %s schema.", name),
                            errdetail("Schemas in a package cannot be deleted."),
                            errcause("The schema in the package does not support object drop."),
                            erraction("N/A")));

                }

                if (has_temp && has_nontemp)
                    ereport(ERROR,
                        (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                            errmsg("DROP not supported for TEMP and non-TEMP objects"),
                            errdetail("You should separate TEMP and non-TEMP objects")));
            }

            res_is_temp = has_temp;
            if (has_temp)
                res_exec_type = EXEC_ON_DATANODES;
            else
                res_exec_type = EXEC_ON_ALL_NODES;

            break;
        }
        /*
         * Those objects are dropped depending on the nature of the relationss
         * they are defined on. This evaluation uses the temporary behavior
         * and the relkind of the relation used.
         */
        case OBJECT_RULE:
        case OBJECT_TRIGGER: {
            List* objname = (List*)linitial(stmt->objects);
            Relation relation = NULL;

            get_object_address(stmt->removeType, objname, NIL, &relation, AccessExclusiveLock, stmt->missing_ok);

            /* Do nothing if no relation */
            if (relation && OidIsValid(relation->rd_id))
                res_exec_type = ExecUtilityFindNodes(stmt->removeType, relation->rd_id, &res_is_temp);
            else
                res_exec_type = EXEC_ON_NONE;

            /* Close relation if necessary */
            if (relation)
                relation_close(relation, NoLock);
        } break;
        /* Datanode do not have rls information */
        case OBJECT_RLSPOLICY:
            res_is_temp = false;
            res_exec_type = EXEC_ON_COORDS;
            break;
        default:
            res_is_temp = false;
            res_exec_type = EXEC_ON_ALL_NODES;
            break;
    }

    /* Save results */
    *is_temp = res_is_temp;
    *exec_type = res_exec_type;
}
#endif

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

static ExecNodes* GetOwnedByNodes(Node* seq_stmt)
{
    Oid goid;
    Oid table_id;
    List* owned_by = NIL;
    List* rel_name = NIL;
    ListCell* option = NULL;
    int nnames;
    RangeVar* rel = NULL;
    List* options = NULL;

    Assert(seq_stmt != NULL);

    if (IsA(seq_stmt, CreateSeqStmt))
        options = ((CreateSeqStmt*)seq_stmt)->options;
    else if (IsA(seq_stmt, AlterSeqStmt))
        options = ((AlterSeqStmt*)seq_stmt)->options;
    else
        return NULL;

    foreach (option, options) {
        DefElem* defel = (DefElem*)lfirst(option);

        if (strcmp(defel->defname, "owned_by") == 0) {
            owned_by = defGetQualifiedName(defel);
            break;
        }
    }

    if (owned_by == NULL)
        return NULL;

    nnames = list_length(owned_by);
    Assert(nnames > 0);
    if (nnames == 1) {
        /* Must be OWNED BY NONE */
        return NULL;
    }

    /* Separate rel_name */
    rel_name = list_truncate(list_copy(owned_by), nnames - 1);
    rel = makeRangeVarFromNameList(rel_name);
    if (rel->schemaname != NULL)
        table_id = get_valid_relname_relid(rel->schemaname, rel->relname);
    else
        table_id = RelnameGetRelid(rel->relname);

    Assert(OidIsValid(table_id));

    /* Fetching group_oid for given relation */
    goid = get_pgxc_class_groupoid(table_id);
    if (!OidIsValid(goid))
        return NULL;

    return GetNodeGroupExecNodes(goid);
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

static const char* GetCreateFuncStringInDN(CreateFunctionStmt* stmt, const char* query_string)
{
    errno_t rc;
    ListCell* option = NULL;
    bool is_sql_lang = false;
    size_t str_len;
    char* tmp = NULL;
    const char* query_str = query_string;

    if (!in_logic_cluster() || !u_sess->attr.attr_sql.check_function_bodies)
        return query_string;

    foreach (option, stmt->options) {
        DefElem* defel = (DefElem*)lfirst(option);

        if (strcmp(defel->defname, "language") == 0) {
            if (strcmp(strVal(defel->arg), "sql") == 0) {
                is_sql_lang = true;
                break;
            }
        }
    }

    if (!is_sql_lang)
        return query_string;

    str_len = strlen(query_string) + 128;
    tmp = (char*)palloc(str_len);

    rc = snprintf_s(
        tmp, str_len, str_len - 1, "SET check_function_bodies = off;%s;SET check_function_bodies = on;", query_string);
    securec_check_ss(rc, "\0", "\0");

    query_str = tmp;

    return query_str;
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

static bool DropObjectsInSameNodeGroup(DropStmt* stmt)
{
    ListCell* cell = NULL;
    ListCell* cell2 = NULL;
    Oid group_oid = InvalidOid;
    Oid goid = InvalidOid;
    bool find_first = false;
    ObjectType object_type = stmt->removeType;
    Oid ins_group_oid;

    if (stmt->objects == NIL || list_length(stmt->objects) == 1) {
        return true;
    }

    ins_group_oid = ng_get_installation_group_oid();

    /* Scanning dropping list to error-out un-allowed cases */
    foreach (cell, stmt->objects) {
        ObjectAddress address;
        List* objname = (List*)lfirst(cell);
        List* objargs = NIL;
        Relation relation = NULL;
        if (stmt->arguments) {
            cell2 = (!cell2 ? list_head(stmt->arguments) : lnext(cell2));
            objargs = (List*)lfirst(cell2);
        }

        /* Get an ObjectAddress for the object. */
        address = get_object_address(stmt->removeType, objname, objargs, &relation, AccessShareLock, stmt->missing_ok);
        /* Issue NOTICE if supplied object was not found. */
        if (!OidIsValid(address.objectId)) {
            continue;
        }

        /* Fetching group_oid for given relation */
        if (object_type == OBJECT_FUNCTION) {
            goid = GetFunctionNodeGroupByFuncid(address.objectId);
            if (!OidIsValid(goid))
                goid = ins_group_oid;
        } else if (OBJECT_IS_SEQUENCE(object_type)) {
            Oid tableId = InvalidOid;
            int32 colId;
            if (!sequenceIsOwned(address.objectId, &tableId, &colId))
                goid = ins_group_oid;
            else
                goid = ng_get_baserel_groupoid(tableId, RELKIND_RELATION);
        }

        /* Close relation if necessary */
        if (relation)
            relation_close(relation, AccessShareLock);

        if (!find_first) {
            group_oid = goid;
            find_first = true;
            continue;
        }

        if (goid != group_oid) {
            return false;
        }
    }

    return true;
}

static ExecNodes* GetDropFunctionNodes(DropStmt* stmt)
{
    ListCell* cell = NULL;
    ListCell* cell2 = NULL;

    if (stmt->objects == NIL) {
        return NULL;
    }

    /* Scanning dropping list to error-out un-allowed cases */
    foreach (cell, stmt->objects) {
        ObjectAddress address;
        List* obj_name = (List*)lfirst(cell);
        List* obj_args = NIL;
        Relation relation = NULL;

        if (stmt->arguments) {
            cell2 = (!cell2 ? list_head(stmt->arguments) : lnext(cell2));
            obj_args = (List*)lfirst(cell2);
        }

        /* Get an ObjectAddress for the object. */
        address = get_object_address(stmt->removeType, obj_name, obj_args, &relation, AccessShareLock, stmt->missing_ok);

        if (relation)
            heap_close(relation, AccessShareLock);

        /* Issue NOTICE if supplied object was not found. */
        if (!OidIsValid(address.objectId)) {
            continue;
        }

        return GetFunctionNodes(address.objectId);
    }

    return NULL;
}

static void assemble_drop_sequence_msg(List* seqs, StringInfo str)
{
    Oid seq_id;
    ListCell* s = NULL;
    const char* rel_name = NULL;
    const char* nsp_name = NULL;
    const char* schema_name = NULL;
    const char* seq_name = NULL;
    char query[256];
    errno_t rc = EOK;

    foreach (s, seqs) {
        seq_id = lfirst_oid(s);

        rel_name = get_rel_name(seq_id);
        nsp_name = get_namespace_name(get_rel_namespace(seq_id));
        schema_name = quote_identifier(nsp_name);
        seq_name = quote_identifier(rel_name);

        Assert(seq_name != NULL && schema_name != NULL);

        rc = snprintf_s(
            query, sizeof(query), sizeof(query) - 1, "DROP SEQUENCE IF EXISTS %s.%s CASCADE;", schema_name, seq_name);
        securec_check_ss(rc, "\0", "\0");

        if (str->data == NULL) {
            initStringInfo(str);
        }
        appendStringInfoString(str, query);

        if (schema_name != nsp_name)
            pfree_ext(schema_name);
        if (seq_name != rel_name)
            pfree_ext(seq_name);
        pfree_ext(rel_name);
        pfree_ext(nsp_name);
    }
}

static char* get_drop_seq_query_string(AlterTableStmt* stmt, Oid rel_id)
{
    HeapTuple tuple;
    char* col_name = NULL;
    ListCell* cell = NULL;
    AlterTableCmd* cmd = NULL;
    List* seq_list = NIL;
    List* attr_list = NIL;

    foreach (cell, stmt->cmds) {
        cmd = (AlterTableCmd*)lfirst(cell);
        if (cmd->subtype == AT_DropColumn || cmd->subtype == AT_DropColumnRecurse) {
            Form_pg_attribute target_att;
            col_name = cmd->name;
            /*
             * get the number of the attribute
             */
            tuple = SearchSysCacheAttName(rel_id, col_name);
            if (!HeapTupleIsValid(tuple))
                continue;

            target_att = (Form_pg_attribute)GETSTRUCT(tuple);
            attr_list = lappend_int(attr_list, target_att->attnum);
            ReleaseSysCache(tuple);
        }
    }

    if (attr_list != NIL) {
        StringInfoData str;
        seq_list = getOwnedSequences(rel_id, attr_list);
        list_free(attr_list);

        if (seq_list != NULL) {
            str.data = NULL;
            assemble_drop_sequence_msg(seq_list, &str);
            return str.data;
        }
    }

    return NULL;
}

static void add_remote_query_4_alter_stmt(bool is_first_node, AlterTableStmt* atstmt, const char* query_string, List** stmts,
    char** drop_seq_string, ExecNodes** exec_nodes)
{
    if (!PointerIsValid(atstmt) || !PointerIsValid(stmts) || !PointerIsValid(drop_seq_string) ||
        !PointerIsValid(exec_nodes)) {
        return;
    }

    bool is_temp = false;
    RemoteQueryExecType exec_type;
    Oid rel_id = RangeVarGetRelid(atstmt->relation, NoLock, true);

    if (!OidIsValid(rel_id)) {
        return;
    }

    exec_type = ExecUtilityFindNodes(atstmt->relkind, rel_id, &is_temp);
    if (u_sess->attr.attr_sql.enable_parallel_ddl && !is_first_node) {
        if (exec_type == EXEC_ON_ALL_NODES || exec_type == EXEC_ON_DATANODES) {
            *stmts = AddRemoteQueryNode(*stmts, query_string, EXEC_ON_DATANODES, is_temp);
        }
    } else {
        *stmts = AddRemoteQueryNode(*stmts, query_string, exec_type, is_temp);
    }

    /* nodegroup attch execnodes */
    if (IS_PGXC_COORDINATOR && !IsConnFromCoord()) {
        Node* node = (Node*)lfirst(list_tail(*stmts));

        if (IsA(node, RemoteQuery)) {
            RemoteQuery* rquery = (RemoteQuery*)node;
            rquery->exec_nodes = RelidGetExecNodes(rel_id);
            if (rquery->exec_nodes->nodeList != NULL &&
                rquery->exec_nodes->nodeList->length < u_sess->pgxc_cxt.NumDataNodes) {
                *drop_seq_string = get_drop_seq_query_string(atstmt, rel_id);
                *exec_nodes = rquery->exec_nodes;
            }
        }
    }
}

static void TransformLoadGetRelation(LoadStmt* stmt, StringInfo buf)
{
    RangeVar* relation = stmt->relation;

    if (relation->schemaname && relation->schemaname[0]) {
        appendStringInfo(buf, "%s.", quote_identifier(relation->schemaname));
    }
    appendStringInfo(buf, "%s ", quote_identifier(relation->relname));
}

static void TransformLoadType(LoadStmt* stmt, StringInfo buf)
{
    switch (stmt->load_type) {
        case LOAD_DATA_APPEND: {
            // COPY's default mode, no need to handle
            break;
        }
        case LOAD_DATA_REPLACE:
        case LOAD_DATA_TRUNCATE: {
            appendStringInfo(buf, "TRUNCATE TABLE ");
            TransformLoadGetRelation(stmt, buf);
            appendStringInfo(buf, "; ");
            break;
        }
        case LOAD_DATA_INSERT: {
            appendStringInfo(buf, "SELECT 'has_data_in_table' FROM ");
            TransformLoadGetRelation(stmt, buf);
            appendStringInfo(buf, "LIMIT 1; ");
            break;
        }
        default: {
            elog(ERROR, "load type(%d) is not supported", stmt->load_type);
            break;
        }
    }
}

static char * TransformPreLoadOptionsData(LoadStmt* stmt, StringInfo buf)
{
    ListCell* option = NULL;

    foreach (option, stmt->pre_load_options) {
        DefElem* def = (DefElem*)lfirst(option);
        if (strcmp(def->defname, "data") == 0) {
            char* data = defGetString(def);
            return data;
        }
    }

    return NULL;
}

static void TransformLoadDatafile(LoadStmt* stmt, StringInfo buf)
{
    ListCell* option = NULL;
    char* filename = NULL;

    appendStringInfo(buf, "FROM ");

    // has datafile in options
    filename = TransformPreLoadOptionsData(stmt, buf);
    if (filename == NULL) {
        foreach (option, stmt->load_options) {
            DefElem* def = (DefElem*)lfirst(option);
            if (strcmp(def->defname, "infile") == 0) {
                filename = defGetString(def);
                break;
            }
        }
    }

    if (filename != NULL) {
        appendStringInfo(buf, "\'%s\' LOAD ", filename);
    }
    else {
        appendStringInfo(buf, "STDIN LOAD ");
    }
}

static void TransformFieldsListScalar(SqlLoadColExpr* coltem, StringInfo transformbuf, StringInfo formatterbuf)
{
    SqlLoadScalarSpec* scalarSpec = (SqlLoadScalarSpec *)coltem->scalar_spec;
    if (scalarSpec->nullif_col != NULL) {
        appendStringInfo(transformbuf, "%s AS nullif(trim(%s), \'\'),", quote_identifier(coltem->colname),
                                                                        quote_identifier(scalarSpec->nullif_col));
    }
    if (scalarSpec->sqlstr != NULL) {
        appendStringInfo(transformbuf, "%s", quote_identifier(coltem->colname));
        A_Const* colexprVal = (A_Const *)scalarSpec->sqlstr;
        appendStringInfo(transformbuf, " AS %s,", colexprVal->val.val.str);
    }
    if (scalarSpec->position_info) {
        appendStringInfo(formatterbuf, "%s", quote_identifier(coltem->colname));
        SqlLoadColPosInfo* posInfo = (SqlLoadColPosInfo *)scalarSpec->position_info;
        int offset = posInfo->start - 1;
        int length = posInfo->end - posInfo->start + 1;
        appendStringInfo(formatterbuf, "(%d, %d),", offset, length);
    }
}

static int64 getCopySequenceCountval(const char *nspname, const char *relname)
{
    StringInfoData buf;
    int ret;
    bool isnull;
    Datum attval;
    if (relname == NULL) {
        return 0;
    }
    initStringInfo(&buf);
    if (nspname == NULL) {
        appendStringInfo(&buf, "select cast(count(*) as bigint) from %s", quote_identifier(relname));
    } else {
        appendStringInfo(&buf, "select cast(count(*) as bigint) from %s.%s", quote_identifier(nspname),
                         quote_identifier(relname));
    }
    if ((ret = SPI_connect()) < 0) {
        /* internal error */
        ereport(ERROR, (errcode(ERRCODE_SPI_CONNECTION_FAILURE), errmsg("SPI connect failure - returned %d", ret)));
    }
    ret = SPI_execute(buf.data, true, INT_MAX);
    if (ret != SPI_OK_SELECT)
        ereport(ERROR, (errcode(ERRCODE_SPI_EXECUTE_FAILURE), errmsg("SPI_execute failed: error code %d", ret)));
    attval = SPI_getbinval(SPI_tuptable->vals[0], SPI_tuptable->tupdesc, 1, &isnull);
    if (isnull) {
        attval = 0;
    }
    SPI_finish();
    return DatumGetInt64(attval);
}

static int64 getCopySequenceMaxval(const char *nspname, const char *relname, const char *colname)
{
    StringInfoData buf;
    int ret;
    bool isnull;
    Datum attval;
    if (relname == NULL || colname == NULL) {
        return 0;
    }
    initStringInfo(&buf);
    if (nspname == NULL) {
        appendStringInfo(&buf, "select cast(nvl(max(%s),0) as bigint) from %s ", quote_identifier(colname),
                         quote_identifier(relname));
    } else {
        appendStringInfo(&buf, "select cast(nvl(max(%s),0) as bigint) from %s.%s ", quote_identifier(colname),
                         quote_identifier(nspname), quote_identifier(relname));
    }
    if ((ret = SPI_connect()) < 0) {
        /* internal error */
        ereport(ERROR, (errcode(ERRCODE_SPI_CONNECTION_FAILURE), errmsg("SPI connect failure - returned %d", ret)));
    }
    ret = SPI_execute(buf.data, true, INT_MAX);
    if (ret != SPI_OK_SELECT)
        ereport(ERROR, (errcode(ERRCODE_SPI_EXECUTE_FAILURE), errmsg("SPI_execute failed: error code %d", ret)));
    attval = SPI_getbinval(SPI_tuptable->vals[0], SPI_tuptable->tupdesc, 1, &isnull);
    if (isnull) {
        attval = 0;
    }
    SPI_finish();
    return DatumGetInt64(attval);
}

static void TransformFieldsListSeque(const char *schemaname, const char *relname,
                                     SqlLoadColExpr* coltem, StringInfo sequencebuf)
{
    SqlLoadSequInfo* sequenceInfo = (SqlLoadSequInfo *)coltem->sequence_info;
    int64 start_num = sequenceInfo->start;
    int64 step_num = sequenceInfo->step;

    if (start_num == LOADER_SEQUENCE_COUNT_FLAG) {
        start_num = getCopySequenceCountval(schemaname, relname) + step_num;
    } else if (start_num == LOADER_SEQUENCE_MAX_FLAG) {
        start_num = getCopySequenceMaxval(schemaname, relname, coltem->colname) + step_num;
    }

    appendStringInfo(sequencebuf, "%s", quote_identifier(coltem->colname));
    appendStringInfo(sequencebuf, "(%ld, %ld),", start_num, step_num);
}

static void TransformFieldsListAppInfo(StringInfo fieldDataBuf, int dataBufCnt, StringInfo buf)
{
    if (dataBufCnt < LOADER_COL_BUF_CNT) {
        return;
    }

    StringInfo constantbuf = &fieldDataBuf[0];
    StringInfo formatterbuf = &fieldDataBuf[1];
    StringInfo transformbuf = &fieldDataBuf[2];
    StringInfo sequencebuf = &fieldDataBuf[3];
    StringInfo fillerbuf = &fieldDataBuf[4];

    if (constantbuf->len) {
        constantbuf->data[constantbuf->len - 1] = '\0';
        appendStringInfo(buf, "CONSTANT(%s) ", constantbuf->data);
    }

    if (formatterbuf->len) {
        formatterbuf->data[formatterbuf->len - 1] = '\0';
        appendStringInfo(buf, "fixed FORMATTER(%s) ", formatterbuf->data);
    }

    if (sequencebuf->len) {
        sequencebuf->data[sequencebuf->len - 1] = '\0';
        appendStringInfo(buf, "SEQUENCE(%s) ", sequencebuf->data);
    }
    if (fillerbuf->len) {
        fillerbuf->data[fillerbuf->len - 1] = '\0';
        appendStringInfo(buf, "FILLER(%s) ", fillerbuf->data);
    }
    if (transformbuf->len) {
        transformbuf->data[transformbuf->len - 1] = '\0';
        appendStringInfo(buf, "TRANSFORM(%s) ", transformbuf->data);
    }
}

static void TransformFieldsListOpt(LoadStmt* stmt, DefElem* def, StringInfo buf)
{
    List* field_list = (List *)def->arg;
    ListCell* option = NULL;
    StringInfoData fieldDataBuf[LOADER_COL_BUF_CNT];
    RangeVar* relation = stmt->relation;
    const char *schemaname = NULL;
    const char *relname = quote_identifier(relation->relname);
    if (relation->schemaname && relation->schemaname[0]) {
        schemaname = quote_identifier(relation->schemaname);
    }

    for (int i = 0; i < LOADER_COL_BUF_CNT; i++) {
        initStringInfo(&fieldDataBuf[i]);
    }

    StringInfo constantbuf = &fieldDataBuf[0];
    StringInfo formatterbuf = &fieldDataBuf[1];
    StringInfo transformbuf = &fieldDataBuf[2];
    StringInfo sequencebuf = &fieldDataBuf[3];
    StringInfo fillerbuf = &fieldDataBuf[4];

    foreach (option, field_list) {
        SqlLoadColExpr* coltem = (SqlLoadColExpr *)lfirst(option);
        if (coltem->const_info != NULL) {
            appendStringInfo(constantbuf, "%s", quote_identifier(coltem->colname));
            A_Const* constVal = (A_Const *)coltem->const_info;
            appendStringInfo(constantbuf, " \'%s\',", constVal->val.val.str);
        }
        else if (coltem->sequence_info != NULL) {
            TransformFieldsListSeque(schemaname, relname, coltem, sequencebuf);
        }else if (coltem->is_filler == true) {
            appendStringInfo(fillerbuf, " %s,", quote_identifier(coltem->colname));
        }
        else if (coltem->scalar_spec != NULL) {
            TransformFieldsListScalar(coltem, transformbuf, formatterbuf);
        }
    }

    TransformFieldsListAppInfo(fieldDataBuf, LOADER_COL_BUF_CNT, buf);
}

static void TransformLoadOptions(LoadStmt* stmt, StringInfo buf)
{
    ListCell* option = NULL;

    foreach (option, stmt->load_options) {

        DefElem* def = (DefElem*)lfirst(option);
        if (strcmp(def->defname, "characterset") == 0) {
            char* encoding = defGetString(def);
            if (pg_strcasecmp(encoding, "AL32UTF8") == 0) {
                encoding = "utf8";
            }
            if (pg_strcasecmp(encoding, "zhs16gbk") == 0) {
                encoding = "gbk";
            }
            if (pg_strcasecmp(encoding, "zhs32gb18030") == 0) {
                encoding = "gb18030";
            }
            appendStringInfo(buf, "ENCODING \'%s\' ", encoding);
            break;
        }
    }
}

static void TransformLoadRelationOptionsWhen(List *when_list, StringInfo buf)
{
    ListCell* lc = NULL;
    bool is_frist = true;

    foreach (lc, when_list) {
        LoadWhenExpr *when = (LoadWhenExpr *)lfirst(lc);
        CheckCopyWhenExprOptions(when);

        if (is_frist == true) {
            appendStringInfo(buf, "WHEN ");
            is_frist = false;
        } else {
            appendStringInfo(buf, "AND ");
        }

        if (when->whentype == 0) {
            appendStringInfo(buf, "(%d-%d) ", when->start, when->end);
            appendStringInfo(buf, "%s ", when->oper);
            appendStringInfo(buf, "\'%s\' ", when->val);
        } else {
            appendStringInfo(buf, "%s ", quote_identifier(when->attname));
            appendStringInfo(buf, "%s ", when->oper);
            appendStringInfo(buf, "\'%s\' ", when->val);
        }
    }
}

static void TransformLoadRelationOptions(LoadStmt* stmt, StringInfo buf)
{
    ListCell* option = NULL;
    bool fields_csv = false;
    bool optionally_enclosed_by = false;

    foreach (option, stmt->rel_options) {

        DefElem* def = (DefElem*)lfirst(option);
        if (strcmp(def->defname, "when_expr") == 0) {
            List *when_list = (List*)(def->arg);
            TransformLoadRelationOptionsWhen(when_list, buf);
        }
        else if (strcmp(def->defname, "fields_csv") == 0) {
            if (stmt->is_only_special_filed == false) {
                appendStringInfo(buf, "csv ");
                fields_csv = true;
            }
        } else if (strcmp(def->defname, "trailing_nullcols") == 0) {
            appendStringInfo(buf, "FILL_MISSING_FIELDS 'multi' ");
        } else if (strcmp(def->defname, "fields_terminated_by") == 0) {
            if (stmt->is_only_special_filed == false) {
                char* terminated_by = defGetString(def);
                appendStringInfo(buf, "DELIMITER \'%s\' ", terminated_by);
            }
        } else if (strcmp(def->defname, "optionally_enclosed_by") == 0) {
            if (stmt->is_only_special_filed == false) {
                char *quote = defGetString(def);
                appendStringInfo(buf, "QUOTE \'%s\' ", quote);
                optionally_enclosed_by = true;
            }
        } else if (strcmp(def->defname, "fields_list") == 0) {
            TransformFieldsListOpt(stmt, def, buf);
        } else {
            ereport(ERROR, (errcode(ERRCODE_SYNTAX_ERROR), errmsg("option \"%s\" not recognized", def->defname)));
        }
    }

    if (optionally_enclosed_by && !fields_csv && stmt->is_only_special_filed == false) {
        appendStringInfo(buf, "csv ");
    }
}

static bool LoadDataHasWhenExpr(LoadStmt* stmt)
{
    ListCell* option = NULL;

    foreach (option, stmt->rel_options) {
        DefElem* def = (DefElem*)lfirst(option);
        if (strcmp(def->defname, "when_expr") == 0) {
            return list_length((List*)(def->arg)) != 0;
        }
    }

    return false;
}

static void TransformPreLoadOptions(LoadStmt* stmt, StringInfo buf)
{
    ListCell* option = NULL;

    int64 errors = 0;

    foreach (option, stmt->pre_load_options) {
        DefElem* def = (DefElem*)lfirst(option);
        if (strcmp(def->defname, "errors") == 0) {
            errors = defGetInt64(def);
            if (errors < 0) {
                ereport(ERROR, (errcode(ERRCODE_SYNTAX_ERROR), errmsg("ERRORS=%ld in OPTIONS should be >= 0", errors)));
            }
            else if (errors > 0) {
                appendStringInfo(buf, "LOG ERRORS DATA REJECT LIMIT \'%ld\' ", errors);
            }
        }
    }

    if (errors == 0 && LoadDataHasWhenExpr(stmt)) {
        appendStringInfo(buf, "LOG ERRORS DATA ");
    }

    foreach (option, stmt->pre_load_options) {
        DefElem* def = (DefElem*)lfirst(option);
        if (strcmp(def->defname, "skip") == 0) {
            int64 skip = defGetInt64(def);
            if (skip < 0) {
                ereport(ERROR, (errcode(ERRCODE_SYNTAX_ERROR), errmsg("SKIP=%ld in OPTIONS should be >= 0", skip)));
            }
            appendStringInfo(buf, "SKIP %ld ", skip);
        }
    }
}

static void SendCopySqlToClient(StringInfo copy_sql)
{
    StringInfoData buf;

    /* Send a RowDescription message */
    pq_beginmessage(&buf, 'T');
    pq_sendint16(&buf, 1); /* 1 fields */

    /* first field */
    pq_sendstring(&buf, "LOAD TRANSFORM TO COPY RESULT"); /* col name */
    pq_sendint32(&buf, 0);           /* table oid */
    pq_sendint16(&buf, 0);           /* attnum */
    pq_sendint32(&buf, TEXTOID);     /* type oid */
    pq_sendint16(&buf, UINT16_MAX);  /* typlen */
    pq_sendint32(&buf, 0);           /* typmod */
    pq_sendint16(&buf, 0);           /* format code */
    pq_endmessage_noblock(&buf);

    /* Send a DataRow message */
    pq_beginmessage(&buf, 'D');
    pq_sendint16(&buf, 1);             /* # of columns */
    pq_sendint32(&buf, copy_sql->len); /* col1 len */
    pq_sendbytes(&buf, (char *)copy_sql->data, copy_sql->len);
    pq_endmessage_noblock(&buf);

    /* Send CommandComplete and ReadyForQuery messages */
    EndCommand_noblock("LOAD", DestTuplestore);
}

static void TransformLoadGetFiledList(LoadStmt* stmt, StringInfo buf)
{
    ListCell* option = NULL;
    ListCell* field_option = NULL;
    int special_filed = 0;

    foreach (option, stmt->rel_options) {
        DefElem* def = (DefElem*)lfirst(option);
        if (strcmp(def->defname, "fields_list") == 0) {
            List* field_list = (List *)def->arg;
            appendStringInfo(buf, "(");
            foreach (field_option, field_list) {
                SqlLoadColExpr* coltem = (SqlLoadColExpr *)lfirst(field_option);
                special_filed += ((coltem->const_info != NULL) || (coltem->sequence_info != NULL) ||
                                         (coltem->is_filler == true) ||
                                         ((coltem->scalar_spec != NULL) &&
                                          ((SqlLoadScalarSpec *)coltem->scalar_spec)->position_info != NULL))
                                     ? 1
                                     : 0;
                appendStringInfo(buf, " %s,", coltem->colname);
            }
            buf->data[buf->len - 1] = ' ';
            appendStringInfo(buf, ") ");
            stmt->is_only_special_filed = (special_filed == list_length(field_list));
            break;
        }
    }
}

static void TransformLoadDataToCopy(LoadStmt* stmt)
{
    StringInfoData bufData;
    StringInfo buf = &bufData;

    initStringInfo(buf);

    TransformLoadType(stmt, buf);

    appendStringInfo(buf, "\\COPY ");

    TransformLoadGetRelation(stmt, buf);

    TransformLoadGetFiledList(stmt, buf);

    TransformLoadDatafile(stmt, buf);

    TransformPreLoadOptions(stmt, buf);

    TransformLoadOptions(stmt, buf);

    TransformLoadRelationOptions(stmt, buf);

    appendStringInfo(buf, "IGNORE_EXTRA_DATA;");

    elog(LOG, "load data is recv type: %d tablename:%s\ncopy sql:%s",
        stmt->type, stmt->relation->relname, buf->data);

    SendCopySqlToClient(buf);
}

static void report_utility_time(void* parse_tree)
{
    ListCell* lc = NULL;
    RangeVar* relation = NULL;

    if (u_sess->attr.attr_sql.enable_save_datachanged_timestamp == false)
        return;

    if (!IS_SINGLE_NODE && !IS_PGXC_COORDINATOR)
        return;

    if (nodeTag(parse_tree) != T_CopyStmt && nodeTag(parse_tree) != T_AlterTableStmt)
        return;

    if (nodeTag(parse_tree) == T_CopyStmt) {
        CopyStmt* cs = (CopyStmt*)parse_tree;
        if (cs->is_from == false) {
            ereport(DEBUG1, (errmsg("\"copy to\" found")));
            return;
        }
        relation = cs->relation;
    }

    if (nodeTag(parse_tree) == T_AlterTableStmt) {
        AlterTableStmt* ats = (AlterTableStmt*)parse_tree;

        bool found = false;
        AlterTableCmd* cmd = NULL;
        foreach (lc, ats->cmds) {
            cmd = (AlterTableCmd*)lfirst(lc);
            if (cmd->subtype == AT_TruncatePartition || cmd->subtype == AT_ExchangePartition ||
                cmd->subtype == AT_DropPartition) {
                found = true;
            }
        }

        if (found == false) {
            ereport(DEBUG1, (errmsg("truncate/exchange/drop partition not found")));
            return;
        }

        relation = ats->relation;
    }

    if (relation == NULL) {
        ereport(DEBUG1, (errmsg("relation is NULL in CopyStmt/AlterTableStmt")));
        return;
    }

    MemoryContext current_ctx = CurrentMemoryContext;

    Relation rel = NULL;

    PG_TRY();
    {
        rel = heap_openrv(relation, AccessShareLock);

        Oid rid = rel->rd_id;

        if (rel->rd_rel->relkind == RELKIND_RELATION && rid >= FirstNormalObjectId) {
            if (rel->rd_rel->relpersistence == RELPERSISTENCE_PERMANENT ||
                rel->rd_rel->relpersistence == RELPERSISTENCE_UNLOGGED) {
                pgstat_report_data_changed(rid, STATFLG_RELATION, rel->rd_rel->relisshared);
            }
        }

        heap_close(rel, AccessShareLock);
    }
    PG_CATCH();
    {
        (void)MemoryContextSwitchTo(current_ctx);

        ErrorData* edata = CopyErrorData();

        ereport(DEBUG1, (errmsg("Failed to send data changed time, cause: %s", edata->message)));

        FlushErrorState();

        FreeErrorData(edata);

        if (rel != NULL)
            heap_close(rel, AccessShareLock);
    }
    PG_END_TRY();
}

static Oid GrantStmtGetNodeGroup(GrantStmt* stmt)
{
    Oid goid = InvalidOid;
    Oid ins_group_oid = InvalidOid;
    List* oids = NIL;
    bool find_first = false;
    ListCell* cell = NULL;
    Oid group_oid = InvalidOid;

    /* Collect the OIDs of the target objects */
    oids = GrantStmtGetObjectOids(stmt);

    ins_group_oid = ng_get_installation_group_oid();

    /* Scanning dropping list to error-out un-allowed cases */
    foreach (cell, oids) {
        Oid object_oid = lfirst_oid(cell);
        char relkind;

        /* Issue NOTICE if supplied object was not found. */
        if (!OidIsValid(object_oid)) {
            continue;
        }

        if (stmt->objtype == ACL_OBJECT_FUNCTION) {
            goid = GetFunctionNodeGroupByFuncid(object_oid);
            if (!OidIsValid(goid))
                goid = ins_group_oid;
        } else if (stmt->objtype == ACL_OBJECT_RELATION || stmt->objtype == ACL_OBJECT_SEQUENCE) {
            relkind = get_rel_relkind(object_oid);

            /* Fetching group_oid for given relation */
            if (relkind == RELKIND_RELATION || relkind == RELKIND_FOREIGN_TABLE || relkind == RELKIND_STREAM) {
                goid = ng_get_baserel_groupoid(object_oid, RELKIND_RELATION);
            } else {
                /* maybe views, sequences */
                continue;
            }
        } else {
            /* Should not enter here */
            Assert(false);
        }

        if (!find_first) {
            group_oid = goid;
            find_first = true;
            continue;
        }

        if (goid != group_oid) {
            list_free(oids);
            return InvalidOid;
        }
    }

    list_free(oids);

    return find_first ? group_oid : ins_group_oid;
}

#ifdef PGXC

/*
 * is_stmt_allowed_in_locked_mode
 *
 * Allow/Disallow a utility command while cluster is locked
 * A statement will be disallowed if it makes such changes
 * in catalog that are backed up by pg_dump except
 * CREATE NODE that has to be allowed because
 * a new node has to be created while the cluster is still
 * locked for backup
 */
static bool is_stmt_allowed_in_locked_mode(Node* parse_tree, const char* query_string)
{
#define ALLOW 1
#define DISALLOW 0

    switch (nodeTag(parse_tree)) {
        /* To allow creation of temp tables */
        case T_CreateStmt: /* CREATE TABLE */
        {
            CreateStmt* stmt = (CreateStmt*)parse_tree;

            /* Don't allow temp table in online scenes for metadata sync problem. */
            if (stmt->relation->relpersistence == RELPERSISTENCE_TEMP &&
                !u_sess->attr.attr_sql.enable_online_ddl_waitlock)
                return ALLOW;
            return DISALLOW;
        } break;
        case T_CreateGroupStmt: {
            CreateGroupStmt* stmt = (CreateGroupStmt*)parse_tree;

            /* Enable create node group in logic and physical cluster online expansion. */
            if ((in_logic_cluster() && stmt->src_group_name) ||
                (u_sess->attr.attr_sql.enable_online_ddl_waitlock && superuser()))
                return ALLOW;
            return DISALLOW;
        } break;
        case T_ExecuteStmt:
            /*
             * Prepared statememts can only have
             * SELECT, INSERT, UPDATE, DELETE,
             * or VALUES statement, there is no
             * point stopping EXECUTE.
             */
        case T_CreateNodeStmt:
        case T_AlterNodeStmt:
            /*
             * This has to be allowed so that the new node
             * can be created, while the cluster is still
             * locked for backup
             */
            /* two cases: when a cluster is first deployed;     */
            /* when new nodes are added into cluster            */
            /* for latter, OM has lock DDL and backup           */
        case T_AlterGroupStmt:
        case T_AlterCoordinatorStmt:
        case T_DropNodeStmt:
            /*
             * This has to be allowed so that DROP NODE
             * can be issued to drop a node that has crashed.
             * Otherwise system would try to acquire a shared
             * advisory lock on the crashed node.
             */
        case T_TransactionStmt:
        case T_PlannedStmt:
        case T_ClosePortalStmt:
        case T_FetchStmt:
        case T_CopyStmt:
        case T_PrepareStmt:
            /*
             * Prepared statememts can only have
             * SELECT, INSERT, UPDATE, DELETE,
             * or VALUES statement, there is no
             * point stopping PREPARE.
             */
        case T_DeallocateStmt:
            /*
             * If prepare is allowed the deallocate should
             * be allowed also
             */
        case T_DoStmt:
        case T_NotifyStmt:
        case T_ListenStmt:
        case T_UnlistenStmt:
        case T_LoadStmt:
        case T_ClusterStmt:
        case T_ExplainStmt:
        case T_VariableSetStmt:
        case T_VariableShowStmt:
        case T_DiscardStmt:
        case T_LockStmt:
        case T_ConstraintsSetStmt:
        case T_CheckPointStmt:
        case T_BarrierStmt:
        case T_ReindexStmt:
        case T_RemoteQuery:
        case T_CleanConnStmt:
        case T_CreateFunctionStmt:  // @Temp Table. create function's lock check is moved in CreateFunction
            return ALLOW;

        default:
            return DISALLOW;
    }
    return DISALLOW;
}

/*
 * ExecUtilityWithMessage:
 * Execute the query on remote nodes in a transaction block.
 * If this fails on one of the nodes :
 * Add a context message containing the failed node names.
 * Rethrow the error with the message about the failed nodes.
 * If all are successful, just return.
 */
static void ExecUtilityWithMessage(const char* query_string, bool sent_to_remote, bool is_temp)
{
    PG_TRY();
    {
        ExecUtilityStmtOnNodes(query_string, NULL, sent_to_remote, false, EXEC_ON_ALL_NODES, is_temp);
    }
    PG_CATCH();
    {

        /*
         * Some nodes failed. Add context about what all nodes the query
         * failed
         */
        ExecNodes* coord_success_nodes = NULL;
        ExecNodes* data_success_nodes = NULL;
        char* msg_failed_nodes = NULL;

        pgxc_all_success_nodes(&data_success_nodes, &coord_success_nodes, &msg_failed_nodes);
        if (msg_failed_nodes != NULL)
            errcontext("%s", msg_failed_nodes);
        PG_RE_THROW();
    }
    PG_END_TRY();
}

static void exec_utility_with_message_parallel_ddl_mode(
    const char* query_string, bool sent_to_remote, bool is_temp, const char* first_exec_node, RemoteQueryExecType exec_type)
{
    PG_TRY();
    {
        ExecUtilityStmtOnNodes_ParallelDDLMode(
            query_string, NULL, sent_to_remote, false, exec_type, is_temp, first_exec_node);
    }
    PG_CATCH();
    {

        /*
         * Some nodes failed. Add context about what all nodes the query
         * failed
         */
        ExecNodes* coord_success_nodes = NULL;
        ExecNodes* data_success_nodes = NULL;
        char* msg_failed_nodes = NULL;

        pgxc_all_success_nodes(&data_success_nodes, &coord_success_nodes, &msg_failed_nodes);
        if (msg_failed_nodes != NULL)
            errcontext("%s", msg_failed_nodes);
        PG_RE_THROW();
    }
    PG_END_TRY();
}

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

static RemoteQueryExecType set_exec_type(Oid object_id, bool* is_temp)
{
    RemoteQueryExecType type;
    if ((*is_temp = IsTempTable(object_id)))
        type = EXEC_ON_DATANODES;
    else
        type = EXEC_ON_ALL_NODES;
    return type;
}

/*
 * ExecUtilityFindNodes
 *
 * Determine the list of nodes to launch query on.
 * This depends on temporary nature of object and object type.
 * Return also a flag indicating if relation is temporary.
 *
 * If object is a RULE, the object id sent is that of the object to which the
 * rule is applicable.
 */
static RemoteQueryExecType ExecUtilityFindNodes(ObjectType object_type, Oid object_id, bool* is_temp)
{
    RemoteQueryExecType exec_type;

    switch (object_type) {
        case OBJECT_SEQUENCE:
        case OBJECT_LARGE_SEQUENCE:
            exec_type = set_exec_type(object_id, is_temp);
            break;

        /* Triggers are evaluated based on the relation they are defined on */
        case OBJECT_TABLE:
        case OBJECT_TRIGGER:
            /* Do the check on relation kind */
            exec_type = exec_utility_find_nodes_relkind(object_id, is_temp);
            break;

        /*
         * Views and rules, both permanent or temporary are created
         * on Coordinators only.
         */
        case OBJECT_RULE:
        case OBJECT_CONTQUERY:
        case OBJECT_VIEW:
            /* Check if object is a temporary view */
            if ((*is_temp = IsTempTable(object_id)))
                exec_type = EXEC_ON_NONE;
            else
                exec_type = u_sess->attr.attr_common.IsInplaceUpgrade ? EXEC_ON_ALL_NODES : EXEC_ON_COORDS;
            break;

        case OBJECT_INDEX:
        case OBJECT_CONSTRAINT:
            // Temp table. For index or constraint, we should check if it's parent is temp object.
            // NOTICE: the argument object_id should be it's parent's oid, not itself's oid.
            exec_type = set_exec_type(object_id, is_temp);
            break;

        case OBJECT_COLUMN:
        case OBJECT_INTERNAL:
            /*
             * 1. temp view column, exec on current CN and exec_type is EXEC_ON_NONE;
             * 2. temp table column, exec on current CN and DNs, so exec_type is EXEC_ON_DATANODES;
             * 3. normal view column, exec on all CNs, so exec_type is EXEC_ON_COORDS;
             * 4. normal table column, exec on all CNs and DNs, so exec_type is EXEC_ON_ALL_NODES;
             */
            if ((*is_temp = IsTempTable(object_id))) {
                if (IsRelaionView(object_id))
                    exec_type = EXEC_ON_NONE;
                else
                    exec_type = EXEC_ON_DATANODES;
            } else {
                if (IsRelaionView(object_id))
                    exec_type = u_sess->attr.attr_common.IsInplaceUpgrade ? EXEC_ON_ALL_NODES : EXEC_ON_COORDS;
                else
                    exec_type = EXEC_ON_ALL_NODES;
            }
            break;

        default:
            *is_temp = false;
            exec_type = EXEC_ON_ALL_NODES;
            break;
    }

    return exec_type;
}

/*
 * exec_utility_find_nodes_relkind
 *
 * Get node execution and temporary type
 * for given relation depending on its relkind
 */
static RemoteQueryExecType exec_utility_find_nodes_relkind(Oid rel_id, bool* is_temp)
{
    char relkind_str = get_rel_relkind(rel_id);
    RemoteQueryExecType exec_type;

    switch (relkind_str) {
        case RELKIND_SEQUENCE:
        case RELKIND_LARGE_SEQUENCE:
            *is_temp = IsTempTable(rel_id);
            exec_type = CHOOSE_EXEC_NODES(*is_temp);
            break;

        case RELKIND_RELATION:
            *is_temp = IsTempTable(rel_id);
            exec_type = CHOOSE_EXEC_NODES(*is_temp);
            break;

        case RELKIND_VIEW:
        case RELKIND_CONTQUERY:
            if ((*is_temp = IsTempTable(rel_id)))
                exec_type = EXEC_ON_NONE;
            else
                exec_type = u_sess->attr.attr_common.IsInplaceUpgrade ? EXEC_ON_ALL_NODES : EXEC_ON_COORDS;
            break;

        default:
            *is_temp = false;
            exec_type = EXEC_ON_ALL_NODES;
            break;
    }

    return exec_type;
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

static void check_xact_readonly(Node* parse_tree)
{
    if (!u_sess->attr.attr_common.XactReadOnly)
        return;

    /*
     *Note:Disk space usage reach the threshold causing database open read-only.
     *In this case,if xc_maintenance_mode=on T_DropStmt and T_TruncateStmt is allowed
     *otherwise,just allow read-only stmt
     */
    if (u_sess->attr.attr_common.xc_maintenance_mode) {
        switch (nodeTag(parse_tree)) {
            case T_DropStmt:
            case T_TruncateStmt:
                return;
            default:
                break;
        }
    }

    /*
     * Note: Commands that need to do more complicated checking are handled
     * elsewhere, in particular COPY and plannable statements do their own
     * checking.  However they should all call PreventCommandIfReadOnly to
     * actually throw the error.
     */
    switch (nodeTag(parse_tree)) {
        case T_AlterDatabaseStmt:
        case T_AlterDatabaseSetStmt:
        case T_AlterDomainStmt:
        case T_AlterFunctionStmt:
        case T_AlterRoleSetStmt:
        case T_AlterObjectSchemaStmt:
        case T_AlterOwnerStmt:
        case T_AlterSchemaStmt:
        case T_AlterSeqStmt:
        case T_AlterTableStmt:
        case T_RenameStmt:
        case T_CommentStmt:
        case T_DefineStmt:
        case T_CreateCastStmt:
        case T_CreateConversionStmt:
        case T_CreatedbStmt:
        case T_CreateDomainStmt:
        case T_CreateFunctionStmt:
        case T_CreateRoleStmt:
        case T_IndexStmt:
        case T_CreatePLangStmt:
        case T_CreateOpClassStmt:
        case T_CreateOpFamilyStmt:
        case T_AlterOpFamilyStmt:
        case T_RuleStmt:
        case T_CreateSchemaStmt:
        case T_CreateSeqStmt:
        case T_CreateStmt:
        case T_CreateTableAsStmt:
        case T_RefreshMatViewStmt:
        case T_CreateTableSpaceStmt:
        case T_CreateTrigStmt:
        case T_CompositeTypeStmt:
        case T_CreateEnumStmt:
        case T_CreateRangeStmt:
        case T_AlterEnumStmt:
        case T_ViewStmt:
        case T_DropStmt:
        case T_DropdbStmt:
        case T_DropTableSpaceStmt:
        case T_DropRoleStmt:
        case T_GrantStmt:
        case T_GrantRoleStmt:
        case T_AlterDefaultPrivilegesStmt:
        case T_TruncateStmt:
        case T_DropOwnedStmt:
        case T_ReassignOwnedStmt:
        case T_AlterTSDictionaryStmt:
        case T_AlterTSConfigurationStmt:
        case T_CreateExtensionStmt:
        case T_AlterExtensionStmt:
        case T_AlterExtensionContentsStmt:
        case T_CreateFdwStmt:
        case T_AlterFdwStmt:
        case T_CreateForeignServerStmt:
        case T_AlterForeignServerStmt:
        case T_CreateUserMappingStmt:
        case T_AlterUserMappingStmt:
        case T_DropUserMappingStmt:
        case T_AlterTableSpaceOptionsStmt:
        case T_CreateForeignTableStmt:
        case T_SecLabelStmt:
        case T_CreateResourcePoolStmt:
        case T_AlterResourcePoolStmt:
        case T_DropResourcePoolStmt:
        case T_CreatePolicyLabelStmt:
        case T_AlterPolicyLabelStmt:
        case T_DropPolicyLabelStmt:
        case T_CreateAuditPolicyStmt:
        case T_AlterAuditPolicyStmt:
        case T_DropAuditPolicyStmt:
        case T_CreateWeakPasswordDictionaryStmt:
        case T_DropWeakPasswordDictionaryStmt:        
        case T_CreateMaskingPolicyStmt:
        case T_AlterMaskingPolicyStmt:
        case T_DropMaskingPolicyStmt:
        case T_ClusterStmt:
        case T_ReindexStmt:
        case T_CreateDataSourceStmt:
        case T_AlterDataSourceStmt:
        case T_CreateDirectoryStmt:
        case T_DropDirectoryStmt:
        case T_CreateRlsPolicyStmt:
        case T_AlterRlsPolicyStmt:
        case T_CreateSynonymStmt:
        case T_DropSynonymStmt:
        case T_CreateClientLogicGlobal:
        case T_CreateClientLogicColumn:
        case T_CreatePackageStmt:
        case T_CreatePackageBodyStmt:
        case T_CreatePublicationStmt:
		case T_AlterPublicationStmt:
		case T_CreateSubscriptionStmt:
		case T_AlterSubscriptionStmt:
		case T_DropSubscriptionStmt:
            PreventCommandIfReadOnly(CreateCommandTag(parse_tree));
            break;
        case T_VacuumStmt: {
            VacuumStmt* stmt = (VacuumStmt*)parse_tree;
            /* on verify mode, do nothing */
            if (!(stmt->options & VACOPT_VERIFY)) {
                PreventCommandIfReadOnly(CreateCommandTag(parse_tree));
            }
            break;
        }
        case T_AlterRoleStmt: {
            AlterRoleStmt* stmt = (AlterRoleStmt*)parse_tree;
            if (!(DO_NOTHING != stmt->lockstatus && t_thrd.postmaster_cxt.HaShmData->current_mode == STANDBY_MODE)) {
                PreventCommandIfReadOnly(CreateCommandTag(parse_tree));
            }
            break;
        }
        default:
            /* do nothing */
            break;
    }
}

static void set_item_arg_according_to_def_name(DefElem* item)
{
    if (strcmp(item->defname, "transaction_isolation") == 0) {
        SetPGVariable("transaction_isolation", list_make1(item->arg), true);
    } else if (strcmp(item->defname, "transaction_read_only") == 0) {
        /* Set read only state from CN when this DN is not read only. */
        if (u_sess->attr.attr_storage.DefaultXactReadOnly == false) {
            SetPGVariable("transaction_read_only", list_make1(item->arg), true);
        }
    } else if (strcmp(item->defname, "transaction_deferrable") == 0) {
        SetPGVariable("transaction_deferrable", list_make1(item->arg), true);
    }
}

static void CheckProcCsnValid()
{
    /* if coordinator send the 'N' and commit message, check the csn number valid */
    if (!(useLocalXid || !IsPostmasterEnvironment || GTM_FREE_MODE) &&
        IsConnFromCoord() && (GetCommitCsn() == InvalidCommitSeqNo)) {
        ereport(FATAL, (errmsg("invalid commit csn: %lu.", GetCommitCsn())));
    }
}

static void CheckRestrictedOperation(const char* cmd_name)
{
    if (InSecurityRestrictedOperation())
        ereport(ERROR,
            (errcode(ERRCODE_INSUFFICIENT_PRIVILEGE),
                /* translator: %s is name of a SQL command, eg PREPARE */
                errmsg("cannot execute %s within security-restricted operation", cmd_name)));
}

void asql_ProcessUtility(Node* parse_tree, const char* query_string, ParamListInfo params, bool is_top_level,
    DestReceiver* dest,
#ifdef PGXC
    bool sent_to_remote,
#endif /* PGXC */
    char* completion_tag,
    bool isCTAS)
{
    /* This can recurse, so check for excessive recursion */
    check_stack_depth();

#ifdef ENABLE_MULTIPLE_NODES
    /*
     * If in scenario of create index for timeseries table, some object
     * replacement needs to be performed to replace the target object
     * with the timeseries tag rel.
     */
    char tag_tbl_name[NAMEDATALEN] = {0};
    Oid nspname;
    bool ts_idx_create = check_ts_create_idx(parse_tree, &nspname);
    if (ts_idx_create) {
        IndexStmt* stmt = (IndexStmt*)parse_tree;
        stmt->relation->schemaname = get_namespace_name(nspname);
        get_ts_idx_tgt(tag_tbl_name, stmt->relation);
        /* replace with the actual tag rel */
        stmt->relation->schemaname = pstrdup("cstore");
        stmt->relation->relname = tag_tbl_name;
    }
#endif

    /* Check the statement during online expansion. */
    BlockUnsupportedDDL(parse_tree);
#ifdef ENABLE_MULTIPLE_NODES
    block_ts_rangevar_unsupport_ddl(parse_tree);
#endif

    /* reset t_thrd.vacuum_cxt.vac_context in case that invalid t_thrd.vacuum_cxt.vac_context would be used */
    t_thrd.vacuum_cxt.vac_context = NULL;

#ifdef PGXC
    // if use rule in cluster resize, we will get the new query string for each stmt
#ifdef ENABLE_MULTIPLE_NODES
    char* new_query = NULL;
    new_query = rewrite_query_string(parse_tree);
    if (new_query != NULL) {
        /*
         * cannot free old queryString. it may point to other memory context, 
         * after switch to this context, it can be free. left to free with this context,
         * rewrite to 1 table, can free, rewrite 2 table, cannot free
         */
        query_string = new_query;
    }
#endif
    /*
     * For more detail see comments in function pgxc_lock_for_backup.
     *
     * Cosider the following scenario:
     * Imagine a two cordinator cluster CO1, CO2
     * Suppose a client connected to CO1 issues select pgxc_lock_for_backup()
     * Now assume that a client connected to CO2 issues a create table
     * select pgxc_lock_for_backup() would try to acquire the advisory lock
     * in exclusive mode, whereas create table would try to acquire the same
     * lock in shared mode. Both these requests will always try acquire the
     * lock in the same order i.e. they would both direct the request first to
     * CO1 and then to CO2. One of the two requests would therefore pass
     * and the other would fail.
     *
     * Consider another scenario:
     * Suppose we have a two cooridnator cluster CO1 and CO2
     * Assume one client connected to each coordinator
     * Further assume one client starts a transaction
     * and issues a DDL. This is an unfinished transaction.
     * Now assume the second client issues
     * select pgxc_lock_for_backup()
     * This request would fail because the unfinished transaction
     * would already hold the advisory lock.
     */
    if (IS_PGXC_COORDINATOR && !IsConnFromCoord() && IsNormalProcessingMode()) {
        /* Is the statement a prohibited one? */
        if (!is_stmt_allowed_in_locked_mode(parse_tree, query_string))
            pgxc_lock_for_utility_stmt(parse_tree, isAllTempObjects(parse_tree, query_string, sent_to_remote));
    }
#endif
    if (t_thrd.proc->workingVersionNum >= 91275) {
        if (IS_PGXC_COORDINATOR && !IsConnFromCoord() && !IsAbortedTransactionBlockState()) {
            Oid node_oid = get_pgxc_nodeoid(g_instance.attr.attr_common.PGXCNodeName);
            bool nodeis_active = true;
            nodeis_active = is_pgxc_nodeactive(node_oid);
            if (OidIsValid(node_oid) && nodeis_active == false && !IS_CN_OBS_DISASTER_RECOVER_MODE)
                ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("Current Node is not active")));
        }
    }

    check_xact_readonly(parse_tree);

    if (completion_tag != NULL)
        completion_tag[0] = '\0';

    errno_t errorno = EOK;
    switch (nodeTag(parse_tree)) {
            /*
             * ******************** transactions ********************
             */
        case T_TransactionStmt: {
            TransactionStmt* stmt = (TransactionStmt*)parse_tree;

            switch (stmt->kind) {
                    /*
                     * START TRANSACTION, as defined by SQL99: Identical
                     * to BEGIN.  Same code for both.
                     */
                case TRANS_STMT_BEGIN:
                case TRANS_STMT_START: {
                    ListCell* lc = NULL;
                    BeginTransactionBlock();
                    foreach (lc, stmt->options) {
                        DefElem* item = (DefElem*)lfirst(lc);
                        set_item_arg_according_to_def_name(item);
                    }
                    u_sess->need_report_top_xid = true;
                } break;

                case TRANS_STMT_COMMIT:
                    /* Only generate one time when u_sess->debug_query_id = 0 in CN */
                    if ((IS_SINGLE_NODE || IS_PGXC_COORDINATOR) && u_sess->debug_query_id == 0) {
                        u_sess->debug_query_id = generate_unique_id64(&gt_queryId);
                        pgstat_report_queryid(u_sess->debug_query_id);
                    }
                    /* only check write nodes csn valid */
                    if (TransactionIdIsValid(GetTopTransactionIdIfAny())) {
                        CheckProcCsnValid();
                    }
                    if (!EndTransactionBlock()) {
                        /* report unsuccessful commit in completion_tag */
                        if (completion_tag != NULL) {
                            errorno = strcpy_s(completion_tag, COMPLETION_TAG_BUFSIZE, "ROLLBACK");
                            securec_check(errorno, "\0", "\0");
                        }
                    }
                    FreeSavepointList();
                    break;

                case TRANS_STMT_PREPARE:
                    PreventCommandDuringRecovery("PREPARE TRANSACTION");
#ifdef PGXC

                    if (IS_PGXC_COORDINATOR && !IsConnFromCoord() && !u_sess->attr.attr_common.xc_maintenance_mode) {
                        /* Explicit prepare transaction is not support */
                        ereport(ERROR,
                            (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                (errmsg("Explicit prepare transaction is not supported."))));

                        /* Add check if xid is valid */
                        if (IsXidImplicit((const char*)stmt->gid)) {
                            ereport(ERROR,
                                (errcode(ERRCODE_INVALID_TRANSACTION_STATE),
                                    (errmsg("Invalid transaciton_id to prepare."))));
                            break;
                        }
                    }
#endif

                    if (!PrepareTransactionBlock(stmt->gid)) {
                        /* report unsuccessful commit in completion_tag */
                        if (completion_tag != NULL) {
                            errorno = strcpy_s(completion_tag, COMPLETION_TAG_BUFSIZE, "ROLLBACK");
                            securec_check(errorno, "\0", "\0");
                        }
                    }
                    break;

                case TRANS_STMT_COMMIT_PREPARED:
                    PreventTransactionChain(is_top_level, "COMMIT PREPARED");
                    PreventCommandDuringRecovery("COMMIT PREPARED");

                    /* for commit in progress, extract the latest local csn for set */
                    if (COMMITSEQNO_IS_COMMITTING(stmt->csn)) {
                        stmt->csn = GET_COMMITSEQNO(stmt->csn);
                    }
#ifdef PGXC
                    /*
                     * Commit a transaction which was explicitely prepared
                     * before
                     */
                    if (IS_PGXC_COORDINATOR && !IsConnFromCoord()) {
                        if (!u_sess->attr.attr_common.xc_maintenance_mode) {
                            ereport(ERROR,
                                (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                    (errmsg("Explicit commit prepared transaction is not supported."))));
                        } else {
                            if (!(useLocalXid || !IsPostmasterEnvironment || GTM_FREE_MODE) &&
                                    COMMITSEQNO_IS_COMMITTED(stmt->csn)) {
                                setCommitCsn(stmt->csn);
                            }
                            FinishPreparedTransaction(stmt->gid, true);
                        }
                    } else {
#endif
#ifdef ENABLE_DISTRIBUTE_TEST
                        if (IS_PGXC_DATANODE && TEST_STUB(DN_COMMIT_PREPARED_SLEEP, twophase_default_error_emit)) {
                            ereport(g_instance.distribute_test_param_instance->elevel,
                                (errmsg("GTM_TEST  %s: DN commit prepare sleep",
                                    g_instance.attr.attr_common.PGXCNodeName)));
                            /* sleep 30s or more */
                            pg_usleep(g_instance.distribute_test_param_instance->sleep_time * 1000000);
                        }

                        /* white box test start */
                        if (IS_PGXC_DATANODE)
                            execute_whitebox(WHITEBOX_LOC, stmt->gid, WHITEBOX_WAIT, 0.0001);
                            /* white box test end */
#endif
                        if (!(useLocalXid || !IsPostmasterEnvironment || GTM_FREE_MODE) &&
                            COMMITSEQNO_IS_COMMITTED(stmt->csn)) {
                            setCommitCsn(stmt->csn);
                        }
                        CheckProcCsnValid();
                        FinishPreparedTransaction(stmt->gid, true);
#ifdef PGXC
                    }
#endif
                    break;

                case TRANS_STMT_ROLLBACK_PREPARED:
                    PreventTransactionChain(is_top_level, "ROLLBACK PREPARED");
                    PreventCommandDuringRecovery("ROLLBACK PREPARED");
#ifdef PGXC
                    /*
                     * Abort a transaction which was explicitely prepared
                     * before
                     */
                    if (IS_PGXC_COORDINATOR && !IsConnFromCoord()) {
                        if (!u_sess->attr.attr_common.xc_maintenance_mode) {
                            ereport(ERROR,
                                (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                    (errmsg("Explicit rollback prepared transaction is not supported."))));
                        } else
                            FinishPreparedTransaction(stmt->gid, false);
                    } else
#endif
                        FinishPreparedTransaction(stmt->gid, false);
                    break;

                case TRANS_STMT_ROLLBACK:
                    UserAbortTransactionBlock();
                    FreeSavepointList();
                    break;

                case TRANS_STMT_SAVEPOINT: {
                    char* name = NULL;

                    RequireTransactionChain(is_top_level, "SAVEPOINT");

                    name = GetSavepointName(stmt->options);

                    /*
                     *  CN send the following info to DNs and other CNs before itself DefineSavepoint
                     * 1)parent xid
                     * 2)start transaction command if need
                     * 3)SAVEPOINT command
                     */
                    if (IS_PGXC_COORDINATOR && !IsConnFromCoord()) {
                        /* record the savepoint cmd, send to other CNs until DDL is executed. */
                        RecordSavepoint(query_string, name, false, SUB_STMT_SAVEPOINT);
                        pgxc_node_remote_savepoint(query_string, EXEC_ON_DATANODES, true, true);

#ifdef ENABLE_DISTRIBUTE_TEST
                        if (TEST_STUB(CN_SAVEPOINT_BEFORE_DEFINE_LOCAL_FAILED, twophase_default_error_emit)) {
                            ereport(g_instance.distribute_test_param_instance->elevel,
                                (errmsg("SUBXACT_TEST %s: cn local define savepoint \"%s\" failed",
                                    g_instance.attr.attr_common.PGXCNodeName,
                                    name)));
                        }
                        /* white box test start */
                        if (execute_whitebox(WHITEBOX_LOC, NULL, WHITEBOX_DEFAULT, 0.0001)) {
                            ereport(g_instance.distribute_test_param_instance->elevel,
                                (errmsg("WHITE_BOX TEST  %s: cn local define savepoint failed",
                                    g_instance.attr.attr_common.PGXCNodeName)));
                        }
                        /* white box test end */
#endif
                    } else {
                        /* Dn or other cn should assign next_xid from cn to local, set parent xid for
                         * CurrentTransactionState */
                        GetCurrentTransactionId();

#ifdef ENABLE_DISTRIBUTE_TEST
                        if (TEST_STUB(DN_SAVEPOINT_BEFORE_DEFINE_LOCAL_FAILED, twophase_default_error_emit)) {
                            ereport(g_instance.distribute_test_param_instance->elevel,
                                (errmsg("SUBXACT_TEST %s: dn local define savepoint \"%s\" failed",
                                    g_instance.attr.attr_common.PGXCNodeName,
                                    name)));
                        }
                        /* white box test start */
                        if (execute_whitebox(WHITEBOX_LOC, NULL, WHITEBOX_DEFAULT, 0.0001)) {
                            ereport(g_instance.distribute_test_param_instance->elevel,
                                (errmsg("WHITE_BOX TEST  %s: dn local define savepoint failed",
                                    g_instance.attr.attr_common.PGXCNodeName)));
                        }
                        /* white box test end */
#endif
                    }

                    DefineSavepoint(name);
                } break;

                case TRANS_STMT_RELEASE:
                    RequireTransactionChain(is_top_level, "RELEASE SAVEPOINT");

                    ReleaseSavepoint(GetSavepointName(stmt->options), false);

#ifdef ENABLE_DISTRIBUTE_TEST
                    /* CN send xid for remote nodes for it assignning parentxid when need. */
                    if (IS_PGXC_COORDINATOR && !IsConnFromCoord()) {

                        if (TEST_STUB(CN_RELEASESAVEPOINT_BEFORE_SEND_FAILED, twophase_default_error_emit)) {
                            ereport(g_instance.distribute_test_param_instance->elevel,
                                (errmsg("SUBXACT_TEST %s: cn release savepoint before send to remote nodes failed.",
                                    g_instance.attr.attr_common.PGXCNodeName)));
                        }
                    } else if (TEST_STUB(DN_RELEASESAVEPOINT_AFTER_LOCAL_DEAL_FAILED, twophase_default_error_emit)) {
                        ereport(g_instance.distribute_test_param_instance->elevel,
                            (errmsg("SUBXACT_TEST %s: dn release savepoint after loal deal failed",
                                g_instance.attr.attr_common.PGXCNodeName)));
                    }

                    /* white box test start */
                    if (execute_whitebox(WHITEBOX_LOC, NULL, WHITEBOX_DEFAULT, 0.0001)) {
                        ereport(g_instance.distribute_test_param_instance->elevel,
                            (errmsg("WHITE_BOX TEST  %s: release savepoint failed",
                                g_instance.attr.attr_common.PGXCNodeName)));
                    }
                    /* white box test end */
#endif
                    if (IS_PGXC_COORDINATOR && !IsConnFromCoord()) {
                        HandleReleaseOrRollbackSavepoint(
                            query_string, GetSavepointName(stmt->options), SUB_STMT_RELEASE);
                        pgxc_node_remote_savepoint(query_string, EXEC_ON_DATANODES, false, false);
                    }

                    break;

                case TRANS_STMT_ROLLBACK_TO:
                    RequireTransactionChain(is_top_level, "ROLLBACK TO SAVEPOINT");
                    RollbackToSavepoint(GetSavepointName(stmt->options), false);

#ifdef ENABLE_DISTRIBUTE_TEST
                    if (IS_PGXC_COORDINATOR && !IsConnFromCoord()) {
                        if (TEST_STUB(CN_ROLLBACKTOSAVEPOINT_BEFORE_SEND_FAILED, twophase_default_error_emit)) {
                            ereport(g_instance.distribute_test_param_instance->elevel,
                                (errmsg("SUBXACT_TEST %s: cn rollback to savepoint failed before send to remote nodes.",
                                    g_instance.attr.attr_common.PGXCNodeName)));
                        }
                    } else if (TEST_STUB(DN_ROLLBACKTOSAVEPOINT_AFTER_LOCAL_DEAL_FAILED, twophase_default_error_emit)) {
                        ereport(g_instance.distribute_test_param_instance->elevel,
                            (errmsg("SUBXACT_TEST %s: dn rollback to savepoint failed after local deal failed",
                                g_instance.attr.attr_common.PGXCNodeName)));
                    }
                    /* white box test start */
                    if (execute_whitebox(WHITEBOX_LOC, NULL, WHITEBOX_DEFAULT, 0.0001)) {
                        ereport(LOG,
                            (errmsg("WHITE_BOX TEST  %s: rollback to savepoint failed",
                                g_instance.attr.attr_common.PGXCNodeName)));
                    }
                    /* white box test end */
#endif

                    /*
                     * CN needn't send xid, as savepoint must be sent and executed before.
                     * And the parent xid must be in transaction state pushed and remained.
                     */
                    if (IS_PGXC_COORDINATOR && !IsConnFromCoord()) {
                        HandleReleaseOrRollbackSavepoint(
                            query_string, GetSavepointName(stmt->options), SUB_STMT_ROLLBACK_TO);
                        pgxc_node_remote_savepoint(query_string, EXEC_ON_DATANODES, false, false);
                    }
                    /*
                     * CommitTransactionCommand is in charge of
                     * re-defining the savepoint again
                     */
                    break;
                default:
                    break;
            }
        } break;

            /*
             * Portal (cursor) manipulation
             *
             * Note: DECLARE CURSOR is processed mostly as a SELECT, and
             * therefore what we will get here is a PlannedStmt not a bare
             * DeclareCursorStmt.
             */
        case T_PlannedStmt: {
            PlannedStmt* stmt = (PlannedStmt*)parse_tree;

            if (stmt->utilityStmt == NULL || !IsA(stmt->utilityStmt, DeclareCursorStmt)) {
                ereport(ERROR,
                    (errcode(ERRCODE_OBJECT_NOT_IN_PREREQUISITE_STATE),
                        errmsg("non-DECLARE CURSOR PlannedStmt passed to ProcessUtility")));
            }
            PerformCursorOpen(stmt, params, query_string, is_top_level);
        } break;

        case T_ClosePortalStmt: {
            ClosePortalStmt* stmt = (ClosePortalStmt*)parse_tree;

            CheckRestrictedOperation("CLOSE");
            stop_query();  // stop datanode query asap.
            PerformPortalClose(stmt->portalname);
        } break;

        case T_FetchStmt:
            PerformPortalFetch((FetchStmt*)parse_tree, dest, completion_tag);
            break;

            /*
             * relation and attribute manipulation
             */
        case T_CreateSchemaStmt:
#ifdef PGXC
            CreateSchemaCommand((CreateSchemaStmt*)parse_tree, query_string, sent_to_remote);
#else
            CreateSchemaCommand((CreateSchemaStmt*)parse_tree, query_string);
#endif
            break;

        case T_AlterSchemaStmt:
#ifdef ENABLE_MULTIPLE_NODES
            if (IS_PGXC_COORDINATOR) {
                char* first_exec_node = find_first_exec_cn();
                bool is_first_node = (strcmp(first_exec_node, g_instance.attr.attr_common.PGXCNodeName) == 0);

                if (u_sess->attr.attr_sql.enable_parallel_ddl && !is_first_node) {
                    ExecUtilityStmtOnNodes_ParallelDDLMode(
                        query_string, NULL, sent_to_remote, false, EXEC_ON_COORDS, false, first_exec_node);
                    AlterSchemaCommand((AlterSchemaStmt*)parse_tree);
                    ExecUtilityStmtOnNodes_ParallelDDLMode(
                        query_string, NULL, sent_to_remote, false, EXEC_ON_DATANODES, false, first_exec_node);
                } else {
                    AlterSchemaCommand((AlterSchemaStmt*)parse_tree);
                    ExecUtilityStmtOnNodes(query_string, NULL, sent_to_remote, false, EXEC_ON_ALL_NODES, false);
                }
            } else {
                AlterSchemaCommand((AlterSchemaStmt*)parse_tree);
            }
#else
            AlterSchemaCommand((AlterSchemaStmt*)parse_tree);
#endif
            break;

        case T_CreateForeignTableStmt:
#ifdef ENABLE_MULTIPLE_NODES
            if (!IsInitdb && IS_SINGLE_NODE) {
                ereport(ERROR,
                    (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                        errmsg("Current mode does not support FOREIGN table yet"),
                        errdetail("The feature is not currently supported")));
            }
#endif
            /* fall through */
        case T_CreateStmt: {
#ifdef PGXC
            CreateCommand((CreateStmt*)parse_tree, query_string, params, is_top_level, sent_to_remote, isCTAS);
#else
            CreateCommand((CreateStmt*)parse_tree, query_string, params, is_top_level, isCTAS);
#endif
        } break;

        case T_CreateTableSpaceStmt:
#ifdef PGXC
            if (IS_PGXC_COORDINATOR && !IsConnFromCoord()) {
                PreventTransactionChain(is_top_level, "CREATE TABLESPACE");

                char* first_exec_node = find_first_exec_cn();
                bool is_first_node = (strcmp(first_exec_node, g_instance.attr.attr_common.PGXCNodeName) == 0);

                if (u_sess->attr.attr_sql.enable_parallel_ddl && !is_first_node) {
                    exec_utility_with_message_parallel_ddl_mode(
                        query_string, sent_to_remote, false, first_exec_node, EXEC_ON_COORDS);
                    CreateTableSpace((CreateTableSpaceStmt*)parse_tree);
                    exec_utility_with_message_parallel_ddl_mode(
                        query_string, sent_to_remote, false, first_exec_node, EXEC_ON_DATANODES);
                } else {
                    CreateTableSpace((CreateTableSpaceStmt*)parse_tree);
                    ExecUtilityWithMessage(query_string, sent_to_remote, false);
                }
            } else {
                /* Don't allow this to be run inside transaction block on single node */
                if (IS_SINGLE_NODE)
                    PreventTransactionChain(is_top_level, "CREATE TABLESPACE");
                CreateTableSpace((CreateTableSpaceStmt*)parse_tree);
            }
#else
            PreventTransactionChain(is_top_level, "CREATE TABLESPACE");
            CreateTableSpace((CreateTableSpaceStmt*)parse_tree);
#endif
            break;

        case T_DropTableSpaceStmt:
#ifdef ENABLE_MULTIPLE_NODES
            if (IS_PGXC_COORDINATOR && !IsConnFromCoord())
                PreventTransactionChain(is_top_level, "DROP TABLESPACE");
            /* Allow this to be run inside transaction block on remote nodes */
            if (IS_PGXC_COORDINATOR) {
                char* first_exec_node = find_first_exec_cn();
                bool is_first_node = (strcmp(first_exec_node, g_instance.attr.attr_common.PGXCNodeName) == 0);

                if (u_sess->attr.attr_sql.enable_parallel_ddl && !is_first_node) {
                    ExecUtilityStmtOnNodes_ParallelDDLMode(
                        query_string, NULL, sent_to_remote, false, EXEC_ON_COORDS, false, first_exec_node);
                    DropTableSpace((DropTableSpaceStmt*)parse_tree);
                    ExecUtilityStmtOnNodes_ParallelDDLMode(
                        query_string, NULL, sent_to_remote, false, EXEC_ON_DATANODES, false, first_exec_node);
                } else {
                    DropTableSpace((DropTableSpaceStmt*)parse_tree);
                    ExecUtilityStmtOnNodes(query_string, NULL, sent_to_remote, false, EXEC_ON_ALL_NODES, false);
                }
            } else {
                DropTableSpace((DropTableSpaceStmt*)parse_tree);
            }
#else
            PreventTransactionChain(is_top_level, "DROP TABLESPACE");
            DropTableSpace((DropTableSpaceStmt*)parse_tree);
#endif
            break;

        case T_AlterTableSpaceOptionsStmt:
#ifdef PGXC
            if (IS_PGXC_COORDINATOR) {
                char* first_exec_node = find_first_exec_cn();
                bool is_first_node = (strcmp(first_exec_node, g_instance.attr.attr_common.PGXCNodeName) == 0);

                if (u_sess->attr.attr_sql.enable_parallel_ddl && !is_first_node) {
                    ExecUtilityStmtOnNodes_ParallelDDLMode(
                        query_string, NULL, sent_to_remote, false, EXEC_ON_COORDS, false, first_exec_node);
                    AlterTableSpaceOptions((AlterTableSpaceOptionsStmt*)parse_tree);
                    ExecUtilityStmtOnNodes_ParallelDDLMode(
                        query_string, NULL, sent_to_remote, false, EXEC_ON_DATANODES, false, first_exec_node);
                } else {
                    AlterTableSpaceOptions((AlterTableSpaceOptionsStmt*)parse_tree);
                    ExecUtilityStmtOnNodes(query_string, NULL, sent_to_remote, false, EXEC_ON_ALL_NODES, false);
                }
            } else {
                AlterTableSpaceOptions((AlterTableSpaceOptionsStmt*)parse_tree);
            }
#else
            AlterTableSpaceOptions((AlterTableSpaceOptionsStmt*)parse_tree);
#endif
            break;

        case T_CreateExtensionStmt:
            CreateExtension((CreateExtensionStmt*)parse_tree);
#ifdef PGXC
            if (IS_PGXC_COORDINATOR)
                ExecUtilityStmtOnNodes(query_string, NULL, sent_to_remote, false, EXEC_ON_ALL_NODES, false);
#endif
            break;

        case T_AlterExtensionStmt:
#ifdef PGXC
            FEATURE_NOT_PUBLIC_ERROR("EXTENSION is not yet supported.");
#endif /* PGXC */
            ExecAlterExtensionStmt((AlterExtensionStmt*)parse_tree);
#ifdef PGXC
            if (IS_PGXC_COORDINATOR)
                ExecUtilityStmtOnNodes(query_string, NULL, sent_to_remote, false, EXEC_ON_ALL_NODES, false);
#endif
            break;

        case T_AlterExtensionContentsStmt:
#ifdef PGXC
            FEATURE_NOT_PUBLIC_ERROR("EXTENSION is not yet supported.");
#endif /* PGXC */
            ExecAlterExtensionContentsStmt((AlterExtensionContentsStmt*)parse_tree);
#ifdef PGXC
            if (IS_PGXC_COORDINATOR)
                ExecUtilityStmtOnNodes(query_string, NULL, sent_to_remote, false, EXEC_ON_ALL_NODES, false);
#endif
            break;

        case T_CreateFdwStmt:
#ifdef ENABLE_MULTIPLE_NODES		
#ifdef PGXC
            /* enable CREATE FOREIGN DATA WRAPPER when initdb */
            if (!IsInitdb && !u_sess->attr.attr_common.IsInplaceUpgrade) {
                ereport(ERROR,
                    (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                        errmsg("openGauss does not support FOREIGN DATA WRAPPER yet"),
                        errdetail("The feature is not currently supported")));
            }
#endif
#endif
            CreateForeignDataWrapper((CreateFdwStmt*)parse_tree);

#ifdef PGXC
            if (IS_PGXC_COORDINATOR && !IsConnFromCoord() && !IsInitdb)
                ExecUtilityStmtOnNodes(query_string, NULL, sent_to_remote, false, EXEC_ON_ALL_NODES, false);
#endif
            break;

        case T_CreateWeakPasswordDictionaryStmt:
            CreateWeakPasswordDictionary((CreateWeakPasswordDictionaryStmt*)parse_tree);
#ifdef ENABLE_MULTIPLE_NODES
        if (IS_PGXC_COORDINATOR && !IsConnFromCoord() && !IsInitdb)
            ExecUtilityStmtOnNodes(query_string, NULL, sent_to_remote, false, EXEC_ON_COORDS, false);
#endif
            break;
            /* We decide to apply ExecuteTruncate for realizing DROP WEAK PASSWORD DICTIONARY operation
               by set gs_weak_password name directly, making the system find our system table */
        case T_DropWeakPasswordDictionaryStmt:
            {
                DropWeakPasswordDictionary();
            }
#ifdef ENABLE_MULTIPLE_NODES
        if (IS_PGXC_COORDINATOR && !IsConnFromCoord() && !IsInitdb)
            ExecUtilityStmtOnNodes(query_string, NULL, sent_to_remote, false, EXEC_ON_COORDS, false);
#endif            
            break;
    
        case T_AlterFdwStmt:
#ifdef ENABLE_MULTIPLE_NODES
#ifdef PGXC
            ereport(ERROR,
                (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                    errmsg("openGauss does not support FOREIGN DATA WRAPPER yet"),
                    errdetail("The feature is not currently supported")));
#endif
#endif
            AlterForeignDataWrapper((AlterFdwStmt*)parse_tree);
            break;

        case T_CreateForeignServerStmt:
#ifdef ENABLE_MULTIPLE_NODES		
            if (!IsInitdb && IS_SINGLE_NODE) {
                ereport(ERROR,
                    (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                        errmsg("Current mode does not support FOREIGN server yet"),
                        errdetail("The feature is not currently supported")));
            }
#endif			
            CreateForeignServer((CreateForeignServerStmt*)parse_tree);
#ifdef PGXC
            if (IS_PGXC_COORDINATOR && !IsConnFromCoord() && !IsInitdb)
                ExecUtilityStmtOnNodes(query_string, NULL, sent_to_remote, false, EXEC_ON_ALL_NODES, false);
#endif
            break;

        case T_AlterForeignServerStmt:
            AlterForeignServer((AlterForeignServerStmt*)parse_tree);
#ifdef PGXC
            if (IS_PGXC_COORDINATOR && !IsConnFromCoord())
                ExecUtilityStmtOnNodes(query_string, NULL, sent_to_remote, false, EXEC_ON_ALL_NODES, false);
#endif
            break;

        case T_CreateUserMappingStmt:
#ifdef ENABLE_MULTIPLE_NODES		
#ifdef PGXC
            ereport(ERROR,
                (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                    errmsg("openGauss does not support USER MAPPING yet"),
                    errdetail("The feature is not currently supported")));
#endif
#endif	
            CreateUserMapping((CreateUserMappingStmt*)parse_tree);
            break;

        case T_AlterUserMappingStmt:
#ifdef ENABLE_MULTIPLE_NODES			
#ifdef PGXC
            ereport(ERROR,
                (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                    errmsg("openGauss does not support USER MAPPING yet"),
                    errdetail("The feature is not currently supported")));
#endif
#endif	
            AlterUserMapping((AlterUserMappingStmt*)parse_tree);
            break;

        case T_DropUserMappingStmt:
#ifdef ENABLE_MULTIPLE_NODES		
#ifdef PGXC
            ereport(ERROR,
                (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                    errmsg("openGauss does not support USER MAPPING yet"),
                    errdetail("The feature is not currently supported")));
#endif
#endif	
            RemoveUserMapping((DropUserMappingStmt*)parse_tree);
            break;

        case T_CreateDataSourceStmt:
            CreateDataSource((CreateDataSourceStmt*)parse_tree);
#ifdef PGXC
            if (IS_PGXC_COORDINATOR && !IsConnFromCoord() && !IsInitdb)
                ExecUtilityStmtOnNodes(query_string, NULL, sent_to_remote, false, EXEC_ON_ALL_NODES, false);
#endif
            break;

        case T_AlterDataSourceStmt:
            AlterDataSource((AlterDataSourceStmt*)parse_tree);
#ifdef PGXC
            if (IS_PGXC_COORDINATOR && !IsConnFromCoord())
                ExecUtilityStmtOnNodes(query_string, NULL, sent_to_remote, false, EXEC_ON_ALL_NODES, false);
#endif
            break;

        case T_CreateRlsPolicyStmt: /* CREATE ROW LEVEL SECURITY POLICY */
            CreateRlsPolicy((CreateRlsPolicyStmt*)parse_tree);
#ifdef PGXC
            if (IS_PGXC_COORDINATOR && !IsConnFromCoord() && !IsInitdb)
                ExecUtilityStmtOnNodes(query_string, NULL, sent_to_remote, false, EXEC_ON_COORDS, false);
#endif
            break;

        case T_AlterRlsPolicyStmt: /* ALTER ROW LEVEL SECURITY POLICY */
            AlterRlsPolicy((AlterRlsPolicyStmt*)parse_tree);
#ifdef PGXC
            if (IS_PGXC_COORDINATOR && !IsConnFromCoord() && !IsInitdb)
                ExecUtilityStmtOnNodes(query_string, NULL, sent_to_remote, false, EXEC_ON_COORDS, false);
#endif
            break;

        case T_DropStmt:
            CheckObjectInBlackList(((DropStmt*)parse_tree)->removeType, query_string);

            /*
             * performMultipleDeletions() needs to know is_top_level by
             * "DfsDDLIsTopLevelXact" to prevent "drop hdfs table"
             * running inside a transaction block.
             */
            if (IS_PGXC_COORDINATOR && !IsConnFromCoord())
                u_sess->exec_cxt.DfsDDLIsTopLevelXact = is_top_level;

            switch (((DropStmt*)parse_tree)->removeType) {
                case OBJECT_INDEX:
#ifdef ENABLE_MULTIPLE_NODES
                    if (((DropStmt*)parse_tree)->concurrent) {
                        ereport(ERROR,
                            (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                errmsg("PGXC does not support concurrent INDEX yet"),
                                errdetail("The feature is not currently supported")));
                    }
#endif
                    if (((DropStmt*)parse_tree)->concurrent)
                        PreventTransactionChain(is_top_level, "DROP INDEX CONCURRENTLY");
                    /* fall through */
                case OBJECT_FOREIGN_TABLE:
                case OBJECT_STREAM:
                case OBJECT_MATVIEW:
                case OBJECT_TABLE: {
#ifdef PGXC
                    /*
                     * For table batch-dropping, we we only support to drop tables
                     * belonging same nodegroup.
                     *
                     * Note: we only have to such kind of check at CN node
                     */
                    if (IS_PGXC_COORDINATOR && !IsConnFromCoord()) {
                        DropStmt* ds = (DropStmt*)parse_tree;

                        if (!ObjectsInSameNodeGroup(ds->objects, T_DropStmt)) {
                            ereport(ERROR,
                                (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                    errmsg("NOT-SUPPORT: Not support DROP multiple objects different nodegroup")));
                        }
                    }
                    /*
                     * Need to let ProcSleep know if we could cancel redistribution transaction which
                     * locks the table we want to drop. ProcSleep will make sure we only cancel the
                     * transaction doing redistribution.
                     *
                     * need to refactor this part into a common function where all supported cancel-redistribution
                     * DDL statements sets it
                     */
                    if (IS_PGXC_COORDINATOR && ((DropStmt*)parse_tree)->removeType == OBJECT_TABLE) {
                        u_sess->exec_cxt.could_cancel_redistribution = true;
                    }
#endif
                }
                    /* fall through */
                case OBJECT_SEQUENCE:
                case OBJECT_LARGE_SEQUENCE:
                case OBJECT_VIEW:
                case OBJECT_CONTQUERY:
#ifdef PGXC
                {
                    if (((DropStmt*)parse_tree)->removeType == OBJECT_FOREIGN_TABLE ||
                        ((DropStmt*)parse_tree)->removeType == OBJECT_STREAM) {
                        /*
                         * In the security mode, the useft privilege of a user must be
                         * checked before the user creates a foreign table.
                         */
                        if (isSecurityMode && !have_useft_privilege()) {
                            ereport(ERROR,
                                (errcode(ERRCODE_INSUFFICIENT_PRIVILEGE),
                                    errmsg("permission denied to drop foreign table in security mode")));
                        }
                    }
                    bool is_temp = false;
                    RemoteQueryExecType exec_type = EXEC_ON_ALL_NODES;
                    ObjectAddresses* new_objects = NULL;

                    /*
                     * For DROP TABLE/INDEX/VIEW/... IF EXISTS query, only notice is emitted
                     * if the referred objects are not found. In such case, the atomicity and consistency
                     * of the query or transaction among local CN and remote nodes can not be guaranteed
                     * against concurrent CREATE TABLE/INDEX/VIEW/... query.
                     *
                     * To ensure such atomicity and consistency, we only refer to local CN about
                     * the visibility of the objects to be deleted and rewrite the query into tmp_query_string
                     * without the inivisible objects. Later, if the objects in tmp_query_string are not
                     * found on remote nodes, which should not happen, just ERROR.
                     */
                    StringInfo tmp_query_string = makeStringInfo();

                    /* Check restrictions on objects dropped */
                    drop_stmt_pre_treatment((DropStmt*)parse_tree, query_string, sent_to_remote, &is_temp, &exec_type);

                    char* first_exec_node = NULL;
                    bool is_first_node = false;

                    if (IS_PGXC_COORDINATOR && !IsConnFromCoord()) {
                        first_exec_node = find_first_exec_cn();
                        is_first_node = (strcmp(first_exec_node, g_instance.attr.attr_common.PGXCNodeName) == 0);
                    }

                    if (IS_PGXC_COORDINATOR && !IsConnFromCoord() && u_sess->attr.attr_sql.enable_parallel_ddl) {
                        if (!is_first_node) {
                            new_objects = PreCheckforRemoveRelation((DropStmt*)parse_tree, tmp_query_string, &exec_type);
                        }
                    }

                    /*
                     * If I am the main execute CN but not CCN,
                     * Notify the CCN to create firstly, and then notify other CNs except me.
                     */
                    if (IS_PGXC_COORDINATOR && !IsConnFromCoord() &&
                        (exec_type == EXEC_ON_ALL_NODES || exec_type == EXEC_ON_COORDS)) {
                        if (u_sess->attr.attr_sql.enable_parallel_ddl && !is_first_node) {
                            RemoteQuery* step = makeNode(RemoteQuery);
                            step->combine_type = COMBINE_TYPE_SAME;
                            step->sql_statement = tmp_query_string->data[0] ? tmp_query_string->data : (char*)query_string;
                            step->exec_type = EXEC_ON_COORDS;
                            step->exec_nodes = NULL;
                            step->is_temp = false;
                            ExecRemoteUtility_ParallelDDLMode(step, first_exec_node);
                            pfree_ext(step);
                        }
                    }

                    /*
                     * @NodeGroup Support
                     *
                     * Scan for first object from drop-list in DropStmt to find target DNs,
                     * here for TO-GROUP aware objects, we need pass DropStmt handler into
                     * ExecUtilityStmtOnNodes() to further evaluate which DNs wend utility.
                     */
                    ExecNodes* exec_nodes = NULL;
                    Node* reparse = NULL;
                    ObjectType object_type = ((DropStmt*)parse_tree)->removeType;
                    if (IS_PGXC_COORDINATOR && !IsConnFromCoord() &&
                        (object_type == OBJECT_TABLE || object_type == OBJECT_INDEX ||
                        object_type == OBJECT_MATVIEW)) {
                        reparse = (Node*)parse_tree;
                        ListCell* lc = list_head(((DropStmt*)parse_tree)->objects);
                        RangeVar* rel = makeRangeVarFromNameList((List*)lfirst(lc));
                        Oid rel_id;
                        LOCKMODE lockmode = NoLock;
                        if (object_type == OBJECT_TABLE || object_type == OBJECT_MATVIEW)
                            lockmode = AccessExclusiveLock;

                        rel_id = RangeVarGetRelid(rel, lockmode, ((DropStmt*)parse_tree)->missing_ok);
                        if (OidIsValid(rel_id)) {
                            Oid check_id = rel_id;
                            char relkind = get_rel_relkind(rel_id);
                            /*
                             * If a view is droped using drop index or drop table cmd, the errmsg will
                             * be somewhat ambiguous since it does not exist in pgxc_class. So we check
                             * in advance to give clear error message.
                             */
                            CheckDropViewValidity(object_type, relkind, rel->relname);
                            if (relkind == RELKIND_INDEX || relkind == RELKIND_GLOBAL_INDEX) {
                                check_id = IndexGetRelation(rel_id, false);
                            }
                            Oid group_oid = get_pgxc_class_groupoid(check_id);
                            char* group_name = get_pgxc_groupname(group_oid);

                            /*
                             * Reminding, when supported user-defined node group expansion,
                             * we need create ExecNodes from target node group.
                             *
                             * Notice!!
                             * In cluster resizing stage we need special processing logics in dropping table as:
                             *	[1]. create table delete_delta ... to group old_group on all DN
                             *	[2]. display pgxc_group.group_members
                             *	[3]. drop table delete_delta ==> drop delete_delta on all DN
                             *
                             * So, as normal, when target node group's status is marked as 'installation' or
                             * 'redistribution', we have to issue a full-DN drop table request, remeber
                             * pgxc_class.group_members still reflects table's logic distribution to tell pgxc
                             * planner to build Scan operator in multi_nodegroup way. The reason we have to so is
                             * to be compatible with current gs_switch_relfilenode() invokation in cluster expand
                             * and shrunk mechanism.
                             */
                            if (need_full_dn_execution(group_name)) {
                                exec_nodes = makeNode(ExecNodes);
                                exec_nodes->nodeList = GetAllDataNodes();
                            } else {
                                exec_nodes = RelidGetExecNodes(rel_id);
                            }
                        } else {
                            exec_nodes = RelidGetExecNodes(rel_id);
                        }
                    } else if (IS_PGXC_COORDINATOR && !IsConnFromCoord() && 
                               (object_type == OBJECT_FOREIGN_TABLE ||
                               object_type == OBJECT_STREAM) &&
                               in_logic_cluster()) {
                        ListCell* lc = list_head(((DropStmt*)parse_tree)->objects);
                        RangeVar* relvar = makeRangeVarFromNameList((List*)lfirst(lc));
                        Oid rel_id = RangeVarGetRelid(relvar, NoLock, true);
                        if (OidIsValid(rel_id))
                            exec_nodes = RelidGetExecNodes(rel_id);
                        else if (!((DropStmt*)parse_tree)->missing_ok) {
                            if (relvar->schemaname)
                                ereport(ERROR,
                                    (errcode(ERRCODE_UNDEFINED_TABLE),
                                        errmsg("foreign table \"%s.%s\" does not exist",
                                            relvar->schemaname,
                                            relvar->relname)));
                            else
                                ereport(ERROR,
                                    (errcode(ERRCODE_UNDEFINED_TABLE),
                                        errmsg("foreign table \"%s\" does not exist", relvar->relname)));
                        }
                    }

#ifdef ENABLE_MULTIPLE_NODES
                    if (IS_PGXC_COORDINATOR && !IsConnFromCoord()) {
                        drop_sequence_4_node_group((DropStmt*)parse_tree, exec_nodes);
                    }
#endif
                    if (IS_PGXC_COORDINATOR && !IsConnFromCoord() && u_sess->attr.attr_sql.enable_parallel_ddl) {
                        if (!is_first_node)
                            RemoveRelationsonMainExecCN((DropStmt*)parse_tree, new_objects);
                        else
                            RemoveRelations((DropStmt*)parse_tree, tmp_query_string, &exec_type);
                    } else
                        RemoveRelations((DropStmt*)parse_tree, tmp_query_string, &exec_type);

                    /* DROP is done depending on the object type and its temporary type */
                    if (IS_PGXC_COORDINATOR && !IsConnFromCoord()) {
                        if (u_sess->attr.attr_sql.enable_parallel_ddl && !is_first_node) {
                            if (exec_type == EXEC_ON_ALL_NODES || exec_type == EXEC_ON_DATANODES)
                                ExecUtilityStmtOnNodes_ParallelDDLMode(
                                    tmp_query_string->data[0] ? tmp_query_string->data : query_string,
                                    exec_nodes,
                                    sent_to_remote,
                                    false,
                                    EXEC_ON_DATANODES,
                                    is_temp,
                                    first_exec_node,
                                    reparse);
                        } else {
                            ExecUtilityStmtOnNodes(tmp_query_string->data[0] ? tmp_query_string->data : query_string,
                                exec_nodes,
                                sent_to_remote,
                                false,
                                exec_type,
                                is_temp,
                                reparse);
                        }
                    }

                    pfree_ext(tmp_query_string->data);
                    pfree_ext(tmp_query_string);
                    FreeExecNodes(&exec_nodes);

                    if (IS_PGXC_COORDINATOR && !IsConnFromCoord() && new_objects != NULL)
                        free_object_addresses(new_objects);
#endif
                } break;
                case OBJECT_SCHEMA:
                case OBJECT_FUNCTION: {
#ifdef PGXC
                    bool is_temp = false;
                    RemoteQueryExecType exec_type = EXEC_ON_ALL_NODES;
                    ObjectAddresses* new_objects = NULL;
                    StringInfo tmp_query_string = makeStringInfo();

                    /* Check restrictions on objects dropped */
                    drop_stmt_pre_treatment((DropStmt*)parse_tree, query_string, sent_to_remote, &is_temp, &exec_type);

                    char* first_exec_node = NULL;
                    bool is_first_node = false;

                    if (IS_PGXC_COORDINATOR && !IsConnFromCoord()) {
                        first_exec_node = find_first_exec_cn();
                        is_first_node = (strcmp(first_exec_node, g_instance.attr.attr_common.PGXCNodeName) == 0);
                    }

                    if (IS_PGXC_COORDINATOR && !IsConnFromCoord() && u_sess->attr.attr_sql.enable_parallel_ddl) {
                        new_objects =
                            PreCheckforRemoveObjects((DropStmt*)parse_tree, tmp_query_string, &exec_type, is_first_node);
                    }

                    /*
                     * @NodeGroup Support
                     *
                     * Scan for first object from drop-list in DropStmt to find target DNs.
                     */
                    ExecNodes* exec_nodes = NULL;
                    ObjectType object_type = ((DropStmt*)parse_tree)->removeType;
                    if (IS_PGXC_COORDINATOR && !IsConnFromCoord() && object_type == OBJECT_FUNCTION &&
                        in_logic_cluster()) {
                        if (!DropObjectsInSameNodeGroup((DropStmt*)parse_tree)) {
                            ereport(ERROR,
                                (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                    errmsg("NOT-SUPPORT: Not support DROP multiple functions in different nodegroup")));
                        }

                        exec_nodes = GetDropFunctionNodes((DropStmt*)parse_tree);
                    }

                    /*
                     * If I am the main execute CN but not CCN,
                     * Notify the CCN to create firstly, and then notify other CNs except me.
                     */
                    if (IS_PGXC_COORDINATOR && !IsConnFromCoord() &&
                        (exec_type == EXEC_ON_ALL_NODES || exec_type == EXEC_ON_COORDS)) {
                        if (u_sess->attr.attr_sql.enable_parallel_ddl && !is_first_node) {
                            RemoteQuery* step = makeNode(RemoteQuery);
                            step->combine_type = COMBINE_TYPE_SAME;
                            step->sql_statement = tmp_query_string->data[0] ? tmp_query_string->data : (char*)query_string;
                            step->exec_type = EXEC_ON_COORDS;
                            step->exec_nodes = NULL;
                            step->is_temp = false;
                            ExecRemoteUtility_ParallelDDLMode(step, first_exec_node);
                            pfree_ext(step);
                        }
                    }

                    if (IS_PGXC_COORDINATOR && !IsConnFromCoord() && u_sess->attr.attr_sql.enable_parallel_ddl) {
                        RemoveObjectsonMainExecCN((DropStmt*)parse_tree, new_objects, is_first_node);
                    } else {
                        if (IS_SINGLE_NODE) {
                            RemoveObjects((DropStmt*)parse_tree, true);
                        } else {
                            if (u_sess->attr.attr_sql.enable_parallel_ddl)
                                RemoveObjects((DropStmt*)parse_tree, false);
                            else
                                RemoveObjects((DropStmt*)parse_tree, true);
                        }
                    }

                    if (IS_PGXC_COORDINATOR && !IsConnFromCoord()) {
                        if (u_sess->attr.attr_sql.enable_parallel_ddl && !is_first_node) {
                            if (exec_type == EXEC_ON_ALL_NODES || exec_type == EXEC_ON_DATANODES)
                                ExecUtilityStmtOnNodes_ParallelDDLMode(
                                    tmp_query_string->data[0] ? tmp_query_string->data : query_string,
                                    exec_nodes,
                                    sent_to_remote,
                                    false,
                                    EXEC_ON_DATANODES,
                                    is_temp,
                                    first_exec_node);
                        } else {
                            ExecUtilityStmtOnNodes(tmp_query_string->data[0] ? tmp_query_string->data : query_string,
                                exec_nodes,
                                sent_to_remote,
                                false,
                                exec_type,
                                is_temp);
                        }
                    }

                    pfree_ext(tmp_query_string->data);
                    pfree_ext(tmp_query_string);
                    FreeExecNodes(&exec_nodes);

                    if (IS_PGXC_COORDINATOR && !IsConnFromCoord() && new_objects != NULL)
                        free_object_addresses(new_objects);
#endif
                } break;
                case OBJECT_PACKAGE:
                case OBJECT_PACKAGE_BODY: {
#ifdef ENABLE_MULTIPLE_NODES
                        ereport(ERROR,
                            (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                errmsg("NOT-SUPPORT: Not support DROP PACKAGE in distributed database")));
#endif
                    RemoveObjects((DropStmt*)parse_tree, true);
                } break;
                case OBJECT_GLOBAL_SETTING: {
                    bool is_temp = false;
                    RemoteQueryExecType exec_type = EXEC_ON_ALL_NODES;

                    /* Check restrictions on objects dropped */
                    drop_stmt_pre_treatment((DropStmt *) parse_tree, query_string, sent_to_remote,
                                            &is_temp, &exec_type);

                    /*
                    * If I am the main execute CN but not CCN,
                    * Notify the CCN to create firstly, and then notify other CNs except me.
                    */
                    char *FirstExecNode = NULL;
                    bool isFirstNode = false;

                    if (IS_PGXC_COORDINATOR && !IsConnFromCoord()) {
                        FirstExecNode = find_first_exec_cn();
                        isFirstNode = (strcmp(FirstExecNode, g_instance.attr.attr_common.PGXCNodeName) == 0);
                    }
                    if (IS_PGXC_COORDINATOR && !IsConnFromCoord() &&
                        (exec_type == EXEC_ON_ALL_NODES || exec_type == EXEC_ON_COORDS)) {
                        if (u_sess->attr.attr_sql.enable_parallel_ddl && !isFirstNode) {
                            RemoteQuery *step = makeNode(RemoteQuery);
                            step->combine_type = COMBINE_TYPE_SAME;
                            step->sql_statement = (char *) query_string;
                            step->exec_type = EXEC_ON_COORDS;
                            step->exec_nodes = NULL;
                            step->is_temp = false;
                            ExecRemoteUtility_ParallelDDLMode(step, FirstExecNode);
                            pfree_ext(step);
                        }
                    }
                    (void)drop_global_settings((DropStmt *)parse_tree);
                    if (IS_PGXC_COORDINATOR && !IsConnFromCoord()) {
                        if (u_sess->attr.attr_sql.enable_parallel_ddl && !isFirstNode) {
                            if (exec_type == EXEC_ON_ALL_NODES || exec_type == EXEC_ON_DATANODES)
                                ExecUtilityStmtOnNodes_ParallelDDLMode(query_string, NULL, sent_to_remote, false,
                                    EXEC_ON_DATANODES, is_temp, FirstExecNode);
                        } else {
                            ExecUtilityStmtOnNodes(query_string, NULL, sent_to_remote, false, exec_type, is_temp);
                        }
                    }
                    break;
                }
                case OBJECT_COLUMN_SETTING: {
                    bool is_temp = false;
                    RemoteQueryExecType exec_type = EXEC_ON_ALL_NODES;

                    /* Check restrictions on objects dropped */
                    drop_stmt_pre_treatment((DropStmt *) parse_tree, query_string, sent_to_remote,
                                            &is_temp, &exec_type);

                    /*
                    * If I am the main execute CN but not CCN,
                    * Notify the CCN to create firstly, and then notify other CNs except me.
                    */
                    char *FirstExecNode = NULL;
                    bool isFirstNode = false;

                    if (IS_PGXC_COORDINATOR && !IsConnFromCoord()) {
                        FirstExecNode = find_first_exec_cn();
                        isFirstNode = (strcmp(FirstExecNode, g_instance.attr.attr_common.PGXCNodeName) == 0);
                    }
                    if (IS_PGXC_COORDINATOR && !IsConnFromCoord() &&
                        (exec_type == EXEC_ON_ALL_NODES || exec_type == EXEC_ON_COORDS)) {
                        if (u_sess->attr.attr_sql.enable_parallel_ddl && !isFirstNode) {
                            RemoteQuery *step = makeNode(RemoteQuery);
                            step->combine_type = COMBINE_TYPE_SAME;
                            step->sql_statement = (char *) query_string;
                            step->exec_type = EXEC_ON_COORDS;
                            step->exec_nodes = NULL;
                            step->is_temp = false;
                            ExecRemoteUtility_ParallelDDLMode(step, FirstExecNode);
                            pfree_ext(step);
                        }
                    }
                    (void)drop_column_settings((DropStmt *)parse_tree);
                    if (IS_PGXC_COORDINATOR && !IsConnFromCoord()) {	
                        if (u_sess->attr.attr_sql.enable_parallel_ddl && !isFirstNode) {
                            if (exec_type == EXEC_ON_ALL_NODES || exec_type == EXEC_ON_DATANODES)
                                ExecUtilityStmtOnNodes_ParallelDDLMode(query_string, NULL, sent_to_remote, false,
                                    EXEC_ON_DATANODES, is_temp, FirstExecNode);
                        } else {
                            ExecUtilityStmtOnNodes(query_string, NULL, sent_to_remote, false, exec_type, is_temp);
                        }
                    }
                    break;
                }
                default: {
#ifdef PGXC
                    bool is_temp = false;
                    RemoteQueryExecType exec_type = EXEC_ON_ALL_NODES;

                    /* Check restrictions on objects dropped */
                    drop_stmt_pre_treatment((DropStmt*)parse_tree, query_string, sent_to_remote, &is_temp, &exec_type);

                    /*
                     * If I am the main execute CN but not CCN,
                     * Notify the CCN to create firstly, and then notify other CNs except me.
                     */
                    char* first_exec_node = NULL;
                    bool is_first_node = false;

                    if (IS_PGXC_COORDINATOR && !IsConnFromCoord()) {
                        first_exec_node = find_first_exec_cn();
                        is_first_node = (strcmp(first_exec_node, g_instance.attr.attr_common.PGXCNodeName) == 0);
                    }
                    if (IS_PGXC_COORDINATOR && !IsConnFromCoord() &&
                        (exec_type == EXEC_ON_ALL_NODES || exec_type == EXEC_ON_COORDS)) {
                        if (u_sess->attr.attr_sql.enable_parallel_ddl && !is_first_node && !sent_to_remote) {
                            RemoteQuery* step = makeNode(RemoteQuery);
                            step->combine_type = COMBINE_TYPE_SAME;
                            step->sql_statement = (char*)query_string;
                            step->exec_type = EXEC_ON_COORDS;
                            step->exec_nodes = NULL;
                            step->is_temp = false;
                            ExecRemoteUtility_ParallelDDLMode(step, first_exec_node);
                            pfree_ext(step);
                        }
                    }
#endif
                    RemoveObjects((DropStmt*)parse_tree, true);
#ifdef PGXC
                    if (IS_PGXC_COORDINATOR && !IsConnFromCoord()) {
                        if (u_sess->attr.attr_sql.enable_parallel_ddl && !is_first_node) {
                            if (exec_type == EXEC_ON_ALL_NODES || exec_type == EXEC_ON_DATANODES)
                                ExecUtilityStmtOnNodes_ParallelDDLMode(
                                    query_string, NULL, sent_to_remote, false, EXEC_ON_DATANODES, is_temp, first_exec_node);
                        } else {
                            ExecUtilityStmtOnNodes(query_string, NULL, sent_to_remote, false, exec_type, is_temp);
                        }
                    }
#endif
                } break;
            }
            break;

        case T_TruncateStmt:
            PG_TRY();
            {
                u_sess->exec_cxt.isExecTrunc = true;
#ifdef PGXC
            /*
             * In Postgres-XC, TRUNCATE needs to be launched to remote nodes
             * before AFTER triggers. As this needs an internal control it is
             * managed by this function internally.
             */
            ExecuteTruncate((TruncateStmt*)parse_tree, query_string);
#else
        ExecuteTruncate((TruncateStmt*)parse_tree);
#endif
                u_sess->exec_cxt.isExecTrunc = false;
            }
            PG_CATCH();
            {
                u_sess->exec_cxt.isExecTrunc = false;
                PG_RE_THROW();
            }
            PG_END_TRY();

            break;

        case T_PurgeStmt:
            ExecutePurge((PurgeStmt*)parse_tree);
            break;

        case T_TimeCapsuleStmt:
            ExecuteTimeCapsule((TimeCapsuleStmt*)parse_tree);
            break;

        case T_CommentStmt:
            CommentObject((CommentStmt*)parse_tree);

#ifdef PGXC
            /* Comment objects depending on their object and temporary types */
            if (IS_PGXC_COORDINATOR && !IsConnFromCoord()) {
                bool is_temp = false;
                ExecNodes* exec_nodes = NULL;
                CommentStmt* stmt = (CommentStmt*)parse_tree;
                RemoteQueryExecType exec_type = get_nodes_4_comment_utility(stmt, &is_temp, &exec_nodes);
                ExecUtilityStmtOnNodes(query_string, exec_nodes, sent_to_remote, false, exec_type, is_temp);
                FreeExecNodes(&exec_nodes);
            }
#endif
            break;

        case T_SecLabelStmt:
#ifdef PGXC
            ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED), errmsg("SECURITY LABEL is not yet supported.")));
#endif /* PGXC */
            ExecSecLabelStmt((SecLabelStmt*)parse_tree);
            break;

        case T_CopyStmt: {
            if (((CopyStmt*)parse_tree)->filename != NULL && isSecurityMode && !IsInitdb) {
                ereport(ERROR,
                    (errcode(ERRCODE_INVALID_OPERATION),
                        errmsg("operation copy with file is forbidden in security mode.")));
            }
            uint64 processed;
            uint64 histhash;
            bool has_histhash;
            processed = DoCopy((CopyStmt*)parse_tree, query_string);
            has_histhash = ((CopyStmt*)parse_tree)->hashstate.has_histhash;
            histhash = ((CopyStmt*)parse_tree)->hashstate.histhash;
            if (completion_tag != NULL) {
                if (has_histhash && !IsConnFromApp()) {
                    errorno = snprintf_s(completion_tag,
                    COMPLETION_TAG_BUFSIZE,
                    COMPLETION_TAG_BUFSIZE - 1,
                    "COPY " UINT64_FORMAT " " UINT64_FORMAT,
                    processed, histhash);
                } else {
                    errorno = snprintf_s(completion_tag,
                    COMPLETION_TAG_BUFSIZE,
                    COMPLETION_TAG_BUFSIZE - 1,
                    "COPY " UINT64_FORMAT,
                    processed);
                }
                securec_check_ss(errorno, "\0", "\0");
            }
            report_utility_time(parse_tree);
        } break;

        case T_PrepareStmt:
            CheckRestrictedOperation("PREPARE");
            PrepareQuery((PrepareStmt*)parse_tree, query_string);
            break;

        case T_ExecuteStmt:
            /*
             * Check the prepared stmt is ok for executing directly, otherwise
             * RePrepareQuery proc should be called to re-generated a new prepared stmt.
             */
            if (needRecompileQuery((ExecuteStmt*)parse_tree))
                RePrepareQuery((ExecuteStmt*)parse_tree);
            ExecuteQuery((ExecuteStmt*)parse_tree, NULL, query_string, params, dest, completion_tag);
            break;

        case T_DeallocateStmt:
            CheckRestrictedOperation("DEALLOCATE");
            DeallocateQuery((DeallocateStmt*)parse_tree);
            break;

            /*
             * schema
             */
        case T_RenameStmt:
#ifdef PGXC
            if (IS_PGXC_COORDINATOR && !IsConnFromCoord()) {
                RenameStmt* stmt = (RenameStmt*)parse_tree;
                RemoteQueryExecType exec_type;
                bool is_temp = false;
                char* first_exec_node = find_first_exec_cn();
                bool is_first_node = (strcmp(first_exec_node, g_instance.attr.attr_common.PGXCNodeName) == 0);

                CheckObjectInBlackList(stmt->renameType, query_string);

                /* Try to use the object relation if possible */
                if (stmt->relation) {
                    /*
                     * When a relation is defined, it is possible that this object does
                     * not exist but an IF EXISTS clause might be used. So we do not do
                     * any error check here but block the access to remote nodes to
                     * this object as it does not exisy
                     */
                    Oid rel_id = RangeVarGetRelid(stmt->relation, AccessShareLock, true);

                    if (OidIsValid(rel_id)) {
                        // Check relations's internal mask
                        Relation rel = relation_open(rel_id, NoLock);
                        if ((RelationGetInternalMask(rel) & INTERNAL_MASK_DALTER))
                            ereport(ERROR,
                                (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                    errmsg("Un-support feature"),
                                    errdetail("internal relation doesn't allow ALTER")));

                        if (rel->rd_rel->relkind == RELKIND_FOREIGN_TABLE)
                            ereport(ERROR,
                                (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                    errmsg("Un-support feature"),
                                    errdetail("target table is a foreign table")));

                        if (rel->rd_rel->relkind == RELKIND_STREAM)
                            ereport(ERROR,
                                (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                    errmsg("Un-support feature"),
                                    errdetail("target table is a stream")));

                        if (RelationIsPAXFormat(rel)) {
                            ereport(ERROR,
                                (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                    errmsg("Un-support feature"),
                                    errdetail("RENAME operation is not supported for DFS table.")));
                        }
                        relation_close(rel, NoLock);

                        UnlockRelationOid(rel_id, AccessShareLock);

                        exec_type = ExecUtilityFindNodes(stmt->renameType, rel_id, &is_temp);
                    } else
                        exec_type = EXEC_ON_NONE;
                } else {
                    exec_type = ExecUtilityFindNodes(stmt->renameType, InvalidOid, &is_temp);
                }

                /* Clean also remote Coordinators */
                if (stmt->renameType == OBJECT_DATABASE) {
                    /* clean all connections with dbname on all CNs before db operations */
                    PreCleanAndCheckConns(stmt->subname, stmt->missing_ok);
                } else if (stmt->renameType == OBJECT_USER || stmt->renameType == OBJECT_ROLE) {
                    /* clean all connections with username on all CNs before user operations */
                    PreCleanAndCheckUserConns(stmt->subname, stmt->missing_ok);
                }

                /*
                 * If I am the main execute CN but not CCN,
                 * Notify the CCN to create firstly, and then notify other CNs except me.
                 */
                if (u_sess->attr.attr_sql.enable_parallel_ddl && !is_first_node &&
                    (exec_type == EXEC_ON_ALL_NODES || exec_type == EXEC_ON_COORDS)) {
                    ExecUtilityStmtOnNodes_ParallelDDLMode(query_string,
                        NULL,
                        sent_to_remote,
                        false,
                        EXEC_ON_COORDS,
                        is_temp,
                        first_exec_node,
                        (Node*)parse_tree);
                }

                ExecRenameStmt((RenameStmt*)parse_tree);

                if (u_sess->attr.attr_sql.enable_parallel_ddl && !is_first_node) {
                    if (exec_type == EXEC_ON_ALL_NODES || exec_type == EXEC_ON_DATANODES)
                        ExecUtilityStmtOnNodes_ParallelDDLMode(query_string,
                            NULL,
                            sent_to_remote,
                            false,
                            EXEC_ON_DATANODES,
                            is_temp,
                            first_exec_node,
                            (Node*)parse_tree);
                } else {
                    ExecUtilityStmtOnNodes(
                        query_string, NULL, sent_to_remote, false, exec_type, is_temp, (Node*)parse_tree);
                }
#ifdef ENABLE_MULTIPLE_NODES
                UpdatePartPolicyWhenRenameRelation((RenameStmt*)parse_tree);
#endif
            } else {
                if (IS_SINGLE_NODE) {
                    CheckObjectInBlackList(((RenameStmt*)parse_tree)->renameType, query_string);
                    RenameStmt* stmt = (RenameStmt*)parse_tree;
                    /* Try to use the object relation if possible */
                    if (stmt->relation) {
                        Oid rel_id = RangeVarGetRelid(stmt->relation, AccessShareLock, true);
                        if (OidIsValid(rel_id)) {
                            // Check relations's internal mask
                            Relation rel = relation_open(rel_id, NoLock);
                            if ((RelationGetInternalMask(rel) & INTERNAL_MASK_DALTER))
                                ereport(ERROR,
                                    (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                        errmsg("Un-support feature"),
                                        errdetail("internal relation doesn't allow ALTER")));

                            relation_close(rel, NoLock);
                            UnlockRelationOid(rel_id, AccessShareLock);
                        }
                    }
                }
                ExecRenameStmt((RenameStmt*)parse_tree);
            }
#else
        ExecRenameStmt((RenameStmt*)parse_tree);
#endif
            break;

        case T_AlterObjectSchemaStmt:
#ifdef PGXC
            if (IS_PGXC_COORDINATOR && !IsConnFromCoord()) {
                AlterObjectSchemaStmt* stmt = (AlterObjectSchemaStmt*)parse_tree;
                RemoteQueryExecType exec_type;
                bool is_temp = false;

                CheckObjectInBlackList(stmt->objectType, query_string);

                /* Try to use the object relation if possible */
                if (stmt->relation) {
                    /*
                     * When a relation is defined, it is possible that this object does
                     * not exist but an IF EXISTS clause might be used. So we do not do
                     * any error check here but block the access to remote nodes to
                     * this object as it does not exisy
                     */
                    Oid rel_id = RangeVarGetRelid(stmt->relation, AccessShareLock, true);

                    if (OidIsValid(rel_id)) {
                        Relation rel = relation_open(rel_id, NoLock);
                        if ((RelationGetInternalMask(rel) & INTERNAL_MASK_DALTER))
                            ereport(ERROR,
                                (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                    errmsg("Un-support feature"),
                                    errdetail("internal relation doesn't allow ALTER")));

                        if (rel->rd_rel->relkind == RELKIND_RELATION && RelationIsPAXFormat(rel)) {
                            ereport(ERROR,
                                (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                    errmsg("Un-support feature"),
                                    errdetail("DFS table doesn't allow ALTER TABLE SET SCHEMA")));
                        }
                        relation_close(rel, NoLock);
                        UnlockRelationOid(rel_id, AccessShareLock);
                        exec_type = ExecUtilityFindNodes(stmt->objectType, rel_id, &is_temp);
                    } else
                        exec_type = EXEC_ON_NONE;
                } else {
                    exec_type = ExecUtilityFindNodes(stmt->objectType, InvalidOid, &is_temp);
                }

                char* first_exec_node = find_first_exec_cn();
                bool is_first_node = (strcmp(first_exec_node, g_instance.attr.attr_common.PGXCNodeName) == 0);

                /*
                 * If I am the main execute CN but not CCN,
                 * Notify the CCN to create firstly, and then notify other CNs except me.
                 */
                if (u_sess->attr.attr_sql.enable_parallel_ddl && !is_first_node) {
                    if (exec_type == EXEC_ON_ALL_NODES || exec_type == EXEC_ON_COORDS) {
                        ExecUtilityStmtOnNodes_ParallelDDLMode(query_string,
                            NULL,
                            sent_to_remote,
                            false,
                            EXEC_ON_COORDS,
                            is_temp,
                            first_exec_node,
                            (Node*)parse_tree);
                    }
                }

                ExecAlterObjectSchemaStmt((AlterObjectSchemaStmt*)parse_tree);

                if (u_sess->attr.attr_sql.enable_parallel_ddl && !is_first_node) {
                    if (exec_type == EXEC_ON_ALL_NODES || exec_type == EXEC_ON_DATANODES) {
                        ExecUtilityStmtOnNodes_ParallelDDLMode(query_string,
                            NULL,
                            sent_to_remote,
                            false,
                            EXEC_ON_DATANODES,
                            is_temp,
                            first_exec_node,
                            (Node*)parse_tree);
                    }
                } else {
                    ExecUtilityStmtOnNodes(
                        query_string, NULL, sent_to_remote, false, exec_type, is_temp, (Node*)parse_tree);
                }
            } else {
                if (IS_SINGLE_NODE) {
                    CheckObjectInBlackList(((AlterObjectSchemaStmt*)parse_tree)->objectType, query_string);
                    AlterObjectSchemaStmt* stmt = (AlterObjectSchemaStmt*)parse_tree;
                    if (stmt->relation) {
                        Oid rel_id = RangeVarGetRelid(stmt->relation, AccessShareLock, true);
                        if (OidIsValid(rel_id)) {
                            // Check relations's internal mask
                            Relation rel = relation_open(rel_id, NoLock);
                            if ((RelationGetInternalMask(rel) & INTERNAL_MASK_DALTER))
                                ereport(ERROR,
                                    (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                        errmsg("Un-support feature"),
                                        errdetail("internal relation doesn't allow ALTER")));

                            relation_close(rel, NoLock);
                            UnlockRelationOid(rel_id, AccessShareLock);
                        }
                    }
                }
                ExecAlterObjectSchemaStmt((AlterObjectSchemaStmt*)parse_tree);
            }
#else
        ExecAlterObjectSchemaStmt((AlterObjectSchemaStmt*)parse_tree);
#endif
            break;

        case T_AlterOwnerStmt:
            CheckObjectInBlackList(((AlterOwnerStmt*)parse_tree)->objectType, query_string);
#ifdef PGXC
            if (IS_PGXC_COORDINATOR) {
                ExecNodes* exec_nodes = NULL;
                char* first_exec_node = find_first_exec_cn();
                bool is_first_node = (strcmp(first_exec_node, g_instance.attr.attr_common.PGXCNodeName) == 0);
                AlterOwnerStmt* OwnerStmt = (AlterOwnerStmt*)parse_tree;

                if (OwnerStmt->objectType == OBJECT_FUNCTION) {
                    Oid funcid = LookupFuncNameTypeNames(OwnerStmt->object, OwnerStmt->objarg, false);

                    exec_nodes = GetFunctionNodes(funcid);
                }

                if (u_sess->attr.attr_sql.enable_parallel_ddl && !is_first_node) {
                    ExecUtilityStmtOnNodes_ParallelDDLMode(
                        query_string, NULL, sent_to_remote, false, EXEC_ON_COORDS, false, first_exec_node);
                    ExecAlterOwnerStmt((AlterOwnerStmt*)parse_tree);
                    ExecUtilityStmtOnNodes_ParallelDDLMode(
                        query_string, exec_nodes, sent_to_remote, false, EXEC_ON_DATANODES, false, first_exec_node);
                } else {
                    ExecAlterOwnerStmt((AlterOwnerStmt*)parse_tree);
                    ExecUtilityStmtOnNodes(query_string, exec_nodes, sent_to_remote, false, EXEC_ON_ALL_NODES, false);
                }
            } else {
                ExecAlterOwnerStmt((AlterOwnerStmt*)parse_tree);
            }
#else
        ExecAlterOwnerStmt((AlterOwnerStmt*)parse_tree);
#endif

            break;

        case T_AlterTableStmt: {
            AlterTableStmt* atstmt = (AlterTableStmt*)parse_tree;
            LOCKMODE lockmode;
            char* first_exec_node = NULL;
            bool is_first_node = false;

            if (IS_PGXC_COORDINATOR && !IsConnFromCoord()) {
                first_exec_node = find_first_exec_cn();
                is_first_node = (strcmp(first_exec_node, g_instance.attr.attr_common.PGXCNodeName) == 0);
            }

            /*
             * Figure out lock mode, and acquire lock.	This also does
             * basic permissions checks, so that we won't wait for a lock
             * on (for example) a relation on which we have no
             * permissions.
             */
            lockmode = AlterTableGetLockLevel(atstmt->cmds);

#ifdef PGXC
            /*
             * If I am the main execute CN but not CCN,
             * Notify the CCN to create firstly, and then notify other CNs except me.
             */
            if (IS_PGXC_COORDINATOR && !IsConnFromCoord()) {
                if (u_sess->attr.attr_sql.enable_parallel_ddl && !is_first_node) {
                    if (!sent_to_remote) {
                        bool isTemp = false;
                        RemoteQueryExecType exec_type;
                        RemoteQuery* step = makeNode(RemoteQuery);

                        Oid rel_id = RangeVarGetRelid(atstmt->relation, lockmode, true);

                        if (OidIsValid(rel_id)) {
                            exec_type = ExecUtilityFindNodes(atstmt->relkind, rel_id, &isTemp);

                            if (exec_type == EXEC_ON_ALL_NODES || exec_type == EXEC_ON_COORDS)
                                step->exec_type = EXEC_ON_COORDS;
                            else
                                step->exec_type = EXEC_ON_NONE;

                            step->combine_type = COMBINE_TYPE_SAME;
                            step->sql_statement = (char*)query_string;
                            step->is_temp = isTemp;
                            step->exec_nodes = NULL;
                            UnlockRelationOid(rel_id, lockmode);
                            ExecRemoteUtility_ParallelDDLMode(step, first_exec_node);
                            pfree_ext(step);
                        }
                    }
                }
            }
#endif
            Oid rel_id;
            List* stmts = NIL;
            ListCell* l = NULL;
            char* drop_seq_string = NULL;
            ExecNodes* exec_nodes = NULL;

            rel_id = AlterTableLookupRelation(atstmt, lockmode);
            elog(DEBUG1,
                "[GET LOCK] Get the lock %d successfully on relation %s for altering operator.",
                lockmode,
                atstmt->relation->relname);

            if (OidIsValid(rel_id)) {
                TrForbidAccessRbObject(RelationRelationId, rel_id, atstmt->relation->relname);

                /* Run parse analysis ... */
                stmts = transformAlterTableStmt(rel_id, atstmt, query_string);

                if (u_sess->attr.attr_sql.enable_cluster_resize) {
                    ATMatviewGroup(stmts, rel_id, lockmode);
                }
#ifdef PGXC
                /*
                 * Add a RemoteQuery node for a query at top level on a remote
                 * Coordinator, if not already done so
                 */
                if (!sent_to_remote && !ISMATMAP(atstmt->relation->relname) && !ISMLOG(atstmt->relation->relname)) {
                    /* nodegroup attch execnodes */
                    add_remote_query_4_alter_stmt(is_first_node, atstmt, query_string, &stmts, &drop_seq_string, &exec_nodes);
                }
#endif

                /* ... and do it */
                foreach (l, stmts) {
                    Node* stmt = (Node*)lfirst(l);

                    if (IsA(stmt, AlterTableStmt)) {
                        /* Do the table alteration proper */
                        AlterTable(rel_id, lockmode, (AlterTableStmt*)stmt);
                    } else {
                        /* Recurse for anything else */
                        ProcessUtility(stmt,
                            query_string,
                            params,
                            false,
                            None_Receiver,
#ifdef PGXC
                            true,
#endif /* PGXC */
                            NULL);
                    }

                    /* Need CCI between commands */
                    if (lnext(l) != NULL)
                        CommandCounterIncrement();
                }
#ifdef ENABLE_MULTIPLE_NODES
                if (drop_seq_string != NULL) {
                    Assert(exec_nodes != NULL);
                    exec_remote_query_4_seq(exec_nodes, drop_seq_string, INVALIDSEQUUID);
                }
#endif
            } else {
                ereport(NOTICE, (errmsg("relation \"%s\" does not exist, skipping", atstmt->relation->relname)));
            }
            report_utility_time(parse_tree);
            pfree_ext(drop_seq_string);
        } break;

        case T_AlterDomainStmt:
#ifdef ENABLE_MULTIPLE_NODES
            ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED), errmsg("domain is not yet supported.")));
#endif /* ENABLE_MULTIPLE_NODES */
            {
                AlterDomainStmt* stmt = (AlterDomainStmt*)parse_tree;

                /*
                 * Some or all of these functions are recursive to cover
                 * inherited things, so permission checks are done there.
                 */
                switch (stmt->subtype) {
                    case 'T': /* ALTER DOMAIN DEFAULT */

                        /*
                         * Recursively alter column default for table and, if
                         * requested, for descendants
                         */
                        AlterDomainDefault(stmt->typname, stmt->def);
                        break;
                    case 'N': /* ALTER DOMAIN DROP NOT NULL */
                        AlterDomainNotNull(stmt->typname, false);
                        break;
                    case 'O': /* ALTER DOMAIN SET NOT NULL */
                        AlterDomainNotNull(stmt->typname, true);
                        break;
                    case 'C': /* ADD CONSTRAINT */
                        AlterDomainAddConstraint(stmt->typname, stmt->def);
                        break;
                    case 'X': /* DROP CONSTRAINT */
                        AlterDomainDropConstraint(stmt->typname, stmt->name, stmt->behavior, stmt->missing_ok);
                        break;
                    case 'V': /* VALIDATE CONSTRAINT */
                        AlterDomainValidateConstraint(stmt->typname, stmt->name);
                        break;
                    default: /* oops */
                    {
                        ereport(ERROR,
                            (errcode(ERRCODE_UNRECOGNIZED_NODE_TYPE),
                                errmsg("unrecognized alter domain type: %d", (int)stmt->subtype)));
                    } break;
                }
            }
#ifdef PGXC
            if (IS_PGXC_COORDINATOR)
                ExecUtilityStmtOnNodes(query_string, NULL, sent_to_remote, false, EXEC_ON_ALL_NODES, false);
#endif
            break;

        case T_GrantStmt:
#ifdef PGXC
            if (IS_PGXC_COORDINATOR && !IsConnFromCoord()) {
                RemoteQueryExecType remoteExecType = EXEC_ON_ALL_NODES;
                GrantStmt* stmt = (GrantStmt*)parse_tree;
                bool is_temp = false;
                ExecNodes* exec_nodes = NULL;

                /* Launch GRANT on Coordinator if object is a sequence */
                if ((stmt->objtype == ACL_OBJECT_RELATION && stmt->targtype == ACL_TARGET_OBJECT)) {
                    /*
                     * In case object is a relation, differenciate the case
                     * of a sequence, a view and a table
                     */
                    ListCell* cell = NULL;
                    /* Check the list of objects */
                    bool first = true;
                    RemoteQueryExecType type_local = remoteExecType;

                    foreach (cell, stmt->objects) {
                        RangeVar* relvar = (RangeVar*)lfirst(cell);
                        Oid rel_id = RangeVarGetRelid(relvar, NoLock, true);

                        /* Skip if object does not exist */
                        if (!OidIsValid(rel_id))
                            continue;

                        remoteExecType = exec_utility_find_nodes_relkind(rel_id, &is_temp);

                        /* Check if object node type corresponds to the first one */
                        if (first) {
                            type_local = remoteExecType;
                            first = false;
                        } else {
                            if (type_local != remoteExecType)
                                ereport(ERROR,
                                    (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                        errmsg("PGXC does not support GRANT on multiple object types"),
                                        errdetail("Grant VIEW/TABLE with separate queries")));
                        }
                    }
                } else if (stmt->objtype == ACL_OBJECT_NODEGROUP && stmt->targtype == ACL_TARGET_OBJECT) {
                    /* For NodeGroup's grant/revoke operation we only issue comments on CN nodes */
                    remoteExecType = EXEC_ON_COORDS;
                }

                if (remoteExecType != EXEC_ON_COORDS &&
                    (stmt->objtype == ACL_OBJECT_RELATION || stmt->objtype == ACL_OBJECT_SEQUENCE ||
                        stmt->objtype == ACL_OBJECT_FUNCTION)) {
                    /* Only for tables, foreign tables, sequences and functions, not views */
                    Oid group_oid = GrantStmtGetNodeGroup(stmt);
                    if (!OidIsValid(group_oid))
                        ereport(ERROR,
                            (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                errmsg("NOT-SUPPORT: Not support Grant/Revoke privileges"
                                       " to objects in different nodegroup")));

                    exec_nodes = GetNodeGroupExecNodes(group_oid);
                }

                char* first_exec_node = find_first_exec_cn();
                bool is_first_node = (strcmp(first_exec_node, g_instance.attr.attr_common.PGXCNodeName) == 0);

                /*
                 * If I am the main execute CN but not CCN,
                 * Notify the CCN to create firstly, and then notify other CNs except me.
                 */
                if (u_sess->attr.attr_sql.enable_parallel_ddl && !is_first_node) {
                    if (remoteExecType == EXEC_ON_ALL_NODES || remoteExecType == EXEC_ON_COORDS) {
                        ExecUtilityStmtOnNodes_ParallelDDLMode(query_string,
                            NULL,
                            sent_to_remote,
                            false,
                            EXEC_ON_COORDS,
                            is_temp,
                            first_exec_node,
                            (Node*)stmt);
                    }
                }

                ExecuteGrantStmt((GrantStmt*)parse_tree);

                if (u_sess->attr.attr_sql.enable_parallel_ddl && !is_first_node) {
                    if (remoteExecType == EXEC_ON_ALL_NODES || remoteExecType == EXEC_ON_DATANODES) {
                        ExecUtilityStmtOnNodes_ParallelDDLMode(query_string,
                            exec_nodes,
                            sent_to_remote,
                            false,
                            EXEC_ON_DATANODES,
                            is_temp,
                            first_exec_node,
                            (Node*)stmt);
                    }
                } else {
                    ExecUtilityStmtOnNodes(
                        query_string, exec_nodes, sent_to_remote, false, remoteExecType, is_temp, (Node*)stmt);
                }
            } else {
                ExecuteGrantStmt((GrantStmt*)parse_tree);
            }
#else
        ExecuteGrantStmt((GrantStmt*)parse_tree);
#endif
            break;

        case T_GrantRoleStmt:
#ifdef PGXC
            if (IS_PGXC_COORDINATOR) {
                char* first_exec_node = find_first_exec_cn();
                bool is_first_node = (strcmp(first_exec_node, g_instance.attr.attr_common.PGXCNodeName) == 0);

                if (u_sess->attr.attr_sql.enable_parallel_ddl && !is_first_node) {
                    ExecUtilityStmtOnNodes_ParallelDDLMode(
                        query_string, NULL, sent_to_remote, false, EXEC_ON_COORDS, false, first_exec_node);
                    GrantRole((GrantRoleStmt*)parse_tree);
                    ExecUtilityStmtOnNodes_ParallelDDLMode(
                        query_string, NULL, sent_to_remote, false, EXEC_ON_DATANODES, false, first_exec_node);
                } else {
                    GrantRole((GrantRoleStmt*)parse_tree);
                    ExecUtilityStmtOnNodes(query_string, NULL, sent_to_remote, false, EXEC_ON_ALL_NODES, false);
                }
            } else {
                GrantRole((GrantRoleStmt*)parse_tree);
            }
#else
        GrantRole((GrantRoleStmt*)parse_tree);
#endif
            break;

        case T_GrantDbStmt:
#ifdef ENABLE_MULTIPLE_NODES
            if (IS_PGXC_COORDINATOR) {
                char* first_exec_node = find_first_exec_cn();
                bool is_first_node = (strcmp(first_exec_node, g_instance.attr.attr_common.PGXCNodeName) == 0);

                if (u_sess->attr.attr_sql.enable_parallel_ddl && !is_first_node) {
                    ExecUtilityStmtOnNodes_ParallelDDLMode(
                        query_string, NULL, sent_to_remote, false, EXEC_ON_COORDS, false, first_exec_node);
                    ExecuteGrantDbStmt((GrantDbStmt*)parse_tree);
                    ExecUtilityStmtOnNodes_ParallelDDLMode(
                        query_string, NULL, sent_to_remote, false, EXEC_ON_DATANODES, false, first_exec_node);
                } else {
                    ExecuteGrantDbStmt((GrantDbStmt*)parse_tree);
                    ExecUtilityStmtOnNodes(query_string, NULL, sent_to_remote, false, EXEC_ON_ALL_NODES, false);
                }
            } else {
                ExecuteGrantDbStmt((GrantDbStmt*)parse_tree);
            }
#else
        ExecuteGrantDbStmt((GrantDbStmt*)parse_tree);
#endif
            break;

        case T_AlterDefaultPrivilegesStmt:
#ifdef PGXC
            if (IS_PGXC_COORDINATOR) {
                char* first_exec_node = find_first_exec_cn();
                bool is_first_node = (strcmp(first_exec_node, g_instance.attr.attr_common.PGXCNodeName) == 0);

                if (u_sess->attr.attr_sql.enable_parallel_ddl && !is_first_node) {
                    ExecUtilityStmtOnNodes_ParallelDDLMode(
                        query_string, NULL, sent_to_remote, false, EXEC_ON_COORDS, false, first_exec_node);
                    ExecAlterDefaultPrivilegesStmt((AlterDefaultPrivilegesStmt*)parse_tree);
                    ExecUtilityStmtOnNodes_ParallelDDLMode(
                        query_string, NULL, sent_to_remote, false, EXEC_ON_DATANODES, false, first_exec_node);
                } else {
                    ExecAlterDefaultPrivilegesStmt((AlterDefaultPrivilegesStmt*)parse_tree);
                    ExecUtilityStmtOnNodes(query_string, NULL, sent_to_remote, false, EXEC_ON_ALL_NODES, false);
                }
            } else {
                ExecAlterDefaultPrivilegesStmt((AlterDefaultPrivilegesStmt*)parse_tree);
            }
#else
        ExecAlterDefaultPrivilegesStmt((AlterDefaultPrivilegesStmt*)parse_tree);
#endif
            break;

            /*
             * **************** object creation / destruction *****************
             */
        case T_DefineStmt: {
            DefineStmt* stmt = (DefineStmt*)parse_tree;

            switch (stmt->kind) {
                case OBJECT_AGGREGATE:
#ifdef ENABLE_MULTIPLE_NODES
                    if (!u_sess->attr.attr_common.IsInplaceUpgrade && !u_sess->exec_cxt.extension_is_valid)
                        ereport(ERROR,
                            (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                errmsg("user defined aggregate is not yet supported.")));
#endif /* ENABLE_MULTIPLE_NODES */
                    DefineAggregate(stmt->defnames, stmt->args, stmt->oldstyle, stmt->definition);
                    break;
                case OBJECT_OPERATOR:
#ifdef ENABLE_MULTIPLE_NODES
                    if (!u_sess->attr.attr_common.IsInplaceUpgrade && !u_sess->exec_cxt.extension_is_valid)
                        ereport(ERROR,
                            (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                errmsg("user defined operator is not yet supported.")));
#endif /* ENABLE_MULTIPLE_NODES */
                    AssertEreport(stmt->args == NIL, MOD_EXECUTOR, "stmt args is NULL");
                    DefineOperator(stmt->defnames, stmt->definition);
                    break;
                case OBJECT_TYPE:
                    AssertEreport(stmt->args == NIL, MOD_EXECUTOR, "stmt args is NULL");
                    DefineType(stmt->defnames, stmt->definition);
                    break;
                case OBJECT_TSPARSER:
#ifdef PGXC
                    if (!IsInitdb) {
                        ereport(ERROR,
                            (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                errmsg("user-defined text search parser is not yet supported.")));
                    }
#endif /* PGXC */
                    AssertEreport(stmt->args == NIL, MOD_EXECUTOR, "stmt args is NULL");
                    DefineTSParser(stmt->defnames, stmt->definition);
                    break;
                case OBJECT_TSDICTIONARY:
                    /* not support with 300 */
                    ts_check_feature_disable();
                    AssertEreport(stmt->args == NIL, MOD_EXECUTOR, "stmt args is NULL");
                    DefineTSDictionary(stmt->defnames, stmt->definition);
                    break;
                case OBJECT_TSTEMPLATE:
#ifdef PGXC
                    /*
                     * An erroneous text search template definition could confuse or
                     * even crash the server, so we just forbid user to create a user
                     * defined text search template definition
                     */
                    if (!IsInitdb) {
                        ereport(ERROR,
                            (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                errmsg("user-defined text search template is not yet supported.")));
                    }
#endif /* PGXC */
                    AssertEreport(stmt->args == NIL, MOD_EXECUTOR, "stmt args is NULL");
                    DefineTSTemplate(stmt->defnames, stmt->definition);
                    break;
                case OBJECT_TSCONFIGURATION:
                    ts_check_feature_disable();
                    /* use 'args' filed to record configuration options */
                    DefineTSConfiguration(stmt->defnames, stmt->definition, stmt->args);
                    break;
                case OBJECT_COLLATION:
#ifdef PGXC
                    ereport(ERROR,
                        (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                            errmsg("user defined collation is not yet supported.")));
#endif /* PGXC */
                    AssertEreport(stmt->args == NIL, MOD_EXECUTOR, "stmt args is NULL");
                    DefineCollation(stmt->defnames, stmt->definition);
                    break;
                default: {
                    ereport(ERROR,
                        (errcode(ERRCODE_UNRECOGNIZED_NODE_TYPE),
                            errmsg("unrecognized define stmt type: %d", (int)stmt->kind)));
                } break;
            }
        }
#ifdef PGXC
            if (IS_PGXC_COORDINATOR)
                ExecUtilityStmtOnNodes(query_string, NULL, sent_to_remote, false, EXEC_ON_ALL_NODES, false);
#endif
            break;

        case T_CompositeTypeStmt: /* CREATE TYPE (composite) */
        {
            CompositeTypeStmt* stmt = (CompositeTypeStmt*)parse_tree;

#ifdef PGXC
            if (IS_PGXC_COORDINATOR) {
                char* first_exec_node = find_first_exec_cn();
                bool is_first_node = (strcmp(first_exec_node, g_instance.attr.attr_common.PGXCNodeName) == 0);

                if (u_sess->attr.attr_sql.enable_parallel_ddl && !is_first_node) {
                    ExecUtilityStmtOnNodes_ParallelDDLMode(
                        query_string, NULL, sent_to_remote, false, EXEC_ON_COORDS, false, first_exec_node);
                    DefineCompositeType(stmt->typevar, stmt->coldeflist);
                    ExecUtilityStmtOnNodes_ParallelDDLMode(
                        query_string, NULL, sent_to_remote, false, EXEC_ON_DATANODES, false, first_exec_node);
                } else {
                    DefineCompositeType(stmt->typevar, stmt->coldeflist);
                    ExecUtilityStmtOnNodes(query_string, NULL, sent_to_remote, false, EXEC_ON_ALL_NODES, false);
                }
            } else
#endif
            {
                DefineCompositeType(stmt->typevar, stmt->coldeflist);
            }
        } break;

        case T_TableOfTypeStmt: /* CREATE TYPE AS TABLE OF */
        {
            TableOfTypeStmt* stmt = (TableOfTypeStmt*)parse_tree;

            if (IS_PGXC_COORDINATOR) {
                char* first_exec_node = find_first_exec_cn();
                bool is_first_node = (strcmp(first_exec_node, g_instance.attr.attr_common.PGXCNodeName) == 0);

                if (u_sess->attr.attr_sql.enable_parallel_ddl && !is_first_node) {
                    ExecUtilityStmtOnNodes_ParallelDDLMode(
                        query_string, NULL, sent_to_remote, false, EXEC_ON_COORDS, false, first_exec_node);
                    DefineTableOfType(stmt);
                    ExecUtilityStmtOnNodes_ParallelDDLMode(
                        query_string, NULL, sent_to_remote, false, EXEC_ON_DATANODES, false, first_exec_node);
                } else {
                    DefineTableOfType(stmt);
                    ExecUtilityStmtOnNodes(query_string, NULL, sent_to_remote, false, EXEC_ON_ALL_NODES, false);
                }
            } else {
                DefineTableOfType(stmt);
            }
        } break;

        case T_CreateEnumStmt: /* CREATE TYPE AS ENUM */
        {
#ifdef PGXC
            if (IS_PGXC_COORDINATOR) {
                char* first_exec_node = find_first_exec_cn();
                bool is_first_node = (strcmp(first_exec_node, g_instance.attr.attr_common.PGXCNodeName) == 0);

                if (u_sess->attr.attr_sql.enable_parallel_ddl && !is_first_node) {
                    ExecUtilityStmtOnNodes_ParallelDDLMode(
                        query_string, NULL, sent_to_remote, false, EXEC_ON_COORDS, false, first_exec_node);
                    DefineEnum((CreateEnumStmt*)parse_tree);
                    ExecUtilityStmtOnNodes_ParallelDDLMode(
                        query_string, NULL, sent_to_remote, false, EXEC_ON_DATANODES, false, first_exec_node);
                } else {
                    DefineEnum((CreateEnumStmt*)parse_tree);
                    ExecUtilityStmtOnNodes(query_string, NULL, sent_to_remote, false, EXEC_ON_ALL_NODES, false);
                }
            } else
#endif
            {
                DefineEnum((CreateEnumStmt*)parse_tree);
            }
        } break;

        case T_CreateRangeStmt: /* CREATE TYPE AS RANGE */
#ifdef ENABLE_MULTIPLE_NODES
            ereport(ERROR,
                (errcode(ERRCODE_FEATURE_NOT_SUPPORTED), errmsg("user defined range type is not yet supported.")));
#endif /* ENABLE_MULTIPLE_NODES */
            DefineRange((CreateRangeStmt*)parse_tree);
#ifdef PGXC
            if (IS_PGXC_COORDINATOR)
                ExecUtilityStmtOnNodes(query_string, NULL, sent_to_remote, false, EXEC_ON_ALL_NODES, false);
#endif
            break;

        case T_AlterEnumStmt: /* ALTER TYPE (enum) */
        {
#ifdef PGXC
            if (IS_PGXC_COORDINATOR && !IsConnFromCoord())
#endif
            {
                /*
                 * We disallow this in transaction blocks, because we can't cope
                 * with enum OID values getting into indexes and then having
                 * their defining pg_enum entries go away.
                 */
                PreventTransactionChain(is_top_level, "ALTER TYPE ... ADD");
            }

#ifdef PGXC
            if (IS_PGXC_COORDINATOR) {
                char* first_exec_node = find_first_exec_cn();
                bool is_first_node = (strcmp(first_exec_node, g_instance.attr.attr_common.PGXCNodeName) == 0);

                if (u_sess->attr.attr_sql.enable_parallel_ddl && !is_first_node) {
                    ExecUtilityStmtOnNodes_ParallelDDLMode(
                        query_string, NULL, sent_to_remote, false, EXEC_ON_COORDS, false, first_exec_node);
                    AlterEnum((AlterEnumStmt*)parse_tree);
                    ExecUtilityStmtOnNodes_ParallelDDLMode(
                        query_string, NULL, sent_to_remote, false, EXEC_ON_DATANODES, false, first_exec_node);
                } else {
                    AlterEnum((AlterEnumStmt*)parse_tree);
                    ExecUtilityStmtOnNodes(query_string, NULL, sent_to_remote, false, EXEC_ON_ALL_NODES, false);
                }
            } else
#endif
            {
                AlterEnum((AlterEnumStmt*)parse_tree);
            }
        } break;

        case T_ViewStmt: /* CREATE VIEW */
        {
#ifdef PGXC
            if (IS_PGXC_COORDINATOR && !IsConnFromCoord()) {
                char* first_exec_node = find_first_exec_cn();
                bool is_first_node = (strcmp(first_exec_node, g_instance.attr.attr_common.PGXCNodeName) == 0);
                ViewStmt *vstmt = (ViewStmt*)parse_tree;

                /*
                 * Run parse analysis to convert the raw parse tree to a Query.  Note this
                 * also acquires sufficient locks on the source table(s).
                 *
                 * Since parse analysis scribbles on its input, copy the raw parse tree;
                 * this ensures we don't corrupt a prepared statement, for example.
                 */
                if (u_sess->attr.attr_sql.enable_parallel_ddl && !is_first_node) {
                    bool is_temp = IsViewTemp((ViewStmt*)parse_tree, query_string);

                    if (!is_temp) {
                        ExecUtilityStmtOnNodes_ParallelDDLMode(
                            query_string, NULL, sent_to_remote, false, EXEC_ON_COORDS, false, first_exec_node);
                    }

                    if (vstmt->relkind == OBJECT_MATVIEW) {
                        CreateCommand((CreateStmt *)vstmt->mv_stmt, vstmt->mv_sql, NULL, true, true);
                        CommandCounterIncrement();

                        acquire_mativew_tables_lock((Query *)vstmt->query, true);
                    }
                    DefineView((ViewStmt*)parse_tree, query_string, sent_to_remote, is_first_node);

                    if (!is_temp) {
                        ExecUtilityStmtOnNodes_ParallelDDLMode(query_string,
                            NULL,
                            sent_to_remote,
                            false,
                            (u_sess->attr.attr_common.IsInplaceUpgrade ? EXEC_ON_DATANODES : EXEC_ON_NONE),
                            false,
                            first_exec_node);
                    }

                } else {
                    if (vstmt->relkind == OBJECT_MATVIEW) {
                        CreateCommand((CreateStmt *)vstmt->mv_stmt, vstmt->mv_sql, NULL, true, true);
                        CommandCounterIncrement();

                        acquire_mativew_tables_lock((Query *)vstmt->query, true);
                    }
                    DefineView((ViewStmt*)parse_tree, query_string, sent_to_remote, is_first_node);

                    char* schema_name = ((ViewStmt*)parse_tree)->view->schemaname;
                    if (schema_name == NULL)
                        schema_name = DatumGetCString(DirectFunctionCall1(current_schema, PointerGetDatum(NULL)));

                    bool temp_schema = false;
                    if (schema_name != NULL)
                        temp_schema = (strncasecmp(schema_name, "pg_temp", 7) == 0) ? true : false;
                    if (!ExecIsTempObjectIncluded() && !temp_schema)
                        ExecUtilityStmtOnNodes(query_string,
                            NULL,
                            sent_to_remote,
                            false,
                            (u_sess->attr.attr_common.IsInplaceUpgrade ? EXEC_ON_ALL_NODES : EXEC_ON_COORDS),
                            false);
                }
            } else {
                ViewStmt *vstmt = (ViewStmt*)parse_tree;
                if (vstmt->relkind == OBJECT_MATVIEW) {
                    CreateCommand((CreateStmt *)vstmt->mv_stmt, vstmt->mv_sql, NULL, true, true);
                    CommandCounterIncrement();

                    acquire_mativew_tables_lock((Query *)vstmt->query, true);
                }
                DefineView((ViewStmt*)parse_tree, query_string, sent_to_remote, query_string);
            }
#else
        DefineView((ViewStmt*)parse_tree, query_string);
#endif
        } break;

        case T_CreateFunctionStmt: /* CREATE FUNCTION */
        {
            PG_TRY();
            {
                CreateFunction((CreateFunctionStmt*)parse_tree, query_string, InvalidOid);
            }
            PG_CATCH();
            {
                if (u_sess->plsql_cxt.debug_query_string) {
                    pfree_ext(u_sess->plsql_cxt.debug_query_string);
                }
                PG_RE_THROW();
            }
            PG_END_TRY();
#ifdef PGXC
            Oid group_oid;
            bool multi_group = false;
            ExecNodes* exec_nodes = NULL;
            const char* query_str = NULL;

            if (IS_PGXC_COORDINATOR) {
                group_oid = GetFunctionNodeGroup((CreateFunctionStmt*)parse_tree, &multi_group);
                if (multi_group) {
                    ereport(ERROR,
                        (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                            errmsg("Does not support FUNCTION with multiple nodegroup table type in logic cluster."),
                            errdetail("The feature is not currently supported")));
                }

                query_str = GetCreateFuncStringInDN((CreateFunctionStmt*)parse_tree, query_string);

                if (OidIsValid(group_oid)) {
                    exec_nodes = GetNodeGroupExecNodes(group_oid);
                }

                ExecUtilityStmtOnNodes(
                    query_str, exec_nodes, sent_to_remote, false, CHOOSE_EXEC_NODES(ExecIsTempObjectIncluded()), false);

                FreeExecNodes(&exec_nodes);
                if (query_str != query_string)
                    pfree_ext(query_str);
            }
#endif
        } break;

        case T_CreatePackageStmt: /* CREATE PACKAGE SPECIFICATION*/
        {
#ifdef ENABLE_MULTIPLE_NODES
                ereport(ERROR, (errcode(ERRCODE_INVALID_PACKAGE_DEFINITION),
                    errmsg("not support create package in distributed database")));
#endif
            PG_TRY();
            {
                CreatePackageCommand((CreatePackageStmt*)parse_tree, query_string);
            }
            PG_CATCH();
            {
                if (u_sess->plsql_cxt.debug_query_string) {
                    pfree_ext(u_sess->plsql_cxt.debug_query_string);
                }
                PG_RE_THROW();
            }
            PG_END_TRY();
        } break;
        case T_CreatePackageBodyStmt: /* CREATE PACKAGE SPECIFICATION*/
        {
#ifdef ENABLE_MULTIPLE_NODES
            ereport(ERROR, (errcode(ERRCODE_INVALID_PACKAGE_DEFINITION),
                    errmsg("not support create package in distributed database")));
#endif
            PG_TRY();
            {
                CreatePackageBodyCommand((CreatePackageBodyStmt*)parse_tree, query_string);
            }
            PG_CATCH();
            {
                if (u_sess->plsql_cxt.debug_query_string) {
                    pfree_ext(u_sess->plsql_cxt.debug_query_string);
                }
                PG_RE_THROW();
            }
            PG_END_TRY();
        } break;


        case T_AlterFunctionStmt: /* ALTER FUNCTION */
#ifdef PGXC
            if (IS_PGXC_COORDINATOR) {
                char* first_exec_node = find_first_exec_cn();
                bool is_first_node = (strcmp(first_exec_node, g_instance.attr.attr_common.PGXCNodeName) == 0);
                ExecNodes* exec_nodes = NULL;

                if (u_sess->attr.attr_sql.enable_parallel_ddl && !is_first_node) {
                    bool is_temp = IsFunctionTemp((AlterFunctionStmt*)parse_tree);
                    if (!is_temp) {
                        ExecUtilityStmtOnNodes_ParallelDDLMode(
                            query_string, NULL, sent_to_remote, false, EXEC_ON_COORDS, false, first_exec_node);
                    }

                    AlterFunction((AlterFunctionStmt*)parse_tree);
                    Oid group_oid = GetFunctionNodeGroup((AlterFunctionStmt*)parse_tree);
                    if (OidIsValid(group_oid)) {
                        exec_nodes = GetNodeGroupExecNodes(group_oid);
                    }

                    ExecUtilityStmtOnNodes_ParallelDDLMode(
                        query_string, exec_nodes, sent_to_remote, false, EXEC_ON_DATANODES, false, first_exec_node);
                } else {
                    AlterFunction((AlterFunctionStmt*)parse_tree);
                    Oid group_oid = GetFunctionNodeGroup((AlterFunctionStmt*)parse_tree);
                    if (OidIsValid(group_oid)) {
                        exec_nodes = GetNodeGroupExecNodes(group_oid);
                    }

                    ExecUtilityStmtOnNodes(query_string,
                        exec_nodes,
                        sent_to_remote,
                        false,
                        CHOOSE_EXEC_NODES(ExecIsTempObjectIncluded()),
                        false);
                }
                FreeExecNodes(&exec_nodes);
            } else {
                AlterFunction((AlterFunctionStmt*)parse_tree);
            }
#else
        AlterFunction((AlterFunctionStmt*)parse_tree);
#endif
            break;

        case T_IndexStmt: /* CREATE INDEX */
        {
            IndexStmt* stmt = (IndexStmt*)parse_tree;
            Oid rel_id;
            LOCKMODE lockmode;

            if (stmt->concurrent) {
                PreventTransactionChain(is_top_level, "CREATE INDEX CONCURRENTLY");
            }

            /* forbid user to set or change inner options */
            ForbidOutUsersToSetInnerOptions(stmt->options);
            ForbidToSetTdeOptionsForNonTdeTbl(stmt->options);

#ifdef PGXC
            bool is_temp = false;
            ExecNodes* exec_nodes = NULL;
            RemoteQueryExecType exec_type = EXEC_ON_ALL_NODES;
            char* first_exec_node = NULL;
            bool is_first_node = false;

            if (IS_PGXC_COORDINATOR && !IsConnFromCoord()) {
                first_exec_node = find_first_exec_cn();
                is_first_node = (strcmp(first_exec_node, g_instance.attr.attr_common.PGXCNodeName) == 0);
            }

#ifdef ENABLE_MULTIPLE_NODES
            if (stmt->concurrent && t_thrd.proc->workingVersionNum < CREATE_INDEX_CONCURRENTLY_DIST_VERSION_NUM) {
                ereport(ERROR,
                    (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                        errmsg("PGXC does not support concurrent INDEX yet"),
                        errdetail("The feature is not currently supported")));
            }
#endif
            /* INDEX on a temporary table cannot use 2PC at commit */
            rel_id = RangeVarGetRelidExtended(stmt->relation, AccessShareLock, true, false, false, true, NULL, NULL);

            if (OidIsValid(rel_id)) {
                exec_type = ExecUtilityFindNodes(OBJECT_INDEX, rel_id, &is_temp);
                UnlockRelationOid(rel_id, AccessShareLock);
            } else {
                exec_type = EXEC_ON_NONE;
            }

            /*
             * If I am the main execute CN but not CCN,
             * Notify the CCN to create firstly, and then notify other CNs except me.
             */
            if (IS_PGXC_COORDINATOR && !IsConnFromCoord() && !stmt->isconstraint) {
                if (u_sess->attr.attr_sql.enable_parallel_ddl && !is_first_node &&
                    (exec_type == EXEC_ON_ALL_NODES || exec_type == EXEC_ON_COORDS)) {
                    ExecUtilityStmtOnNodes_ParallelDDLMode(
                        query_string, NULL, sent_to_remote, stmt->concurrent, EXEC_ON_COORDS, is_temp, first_exec_node);
                }
            }
#endif
            /*
             * Look up the relation OID just once, right here at the
             * beginning, so that we don't end up repeating the name
             * lookup later and latching onto a different relation
             * partway through.  To avoid lock upgrade hazards, it's
             * important that we take the strongest lock that will
             * eventually be needed here, so the lockmode calculation
             * needs to match what DefineIndex() does.
             */
            lockmode = stmt->concurrent ? ShareUpdateExclusiveLock : ShareLock;
            rel_id = RangeVarGetRelidExtended(
                stmt->relation, lockmode, false, false, false, true, RangeVarCallbackOwnsRelation, NULL);
            /* Run parse analysis ... */
            stmt = transformIndexStmt(rel_id, stmt, query_string);
#ifdef PGXC
            /* Find target datanode list that we need send CREATE-INDEX on.
             * If it's a view, skip it. */
            if (IS_PGXC_COORDINATOR && !IsConnFromCoord() && get_rel_relkind(rel_id) == RELKIND_RELATION) {
                int nmembers = 0;
                Oid group_oid = InvalidOid;
                Oid* members = NULL;
                exec_nodes = makeNode(ExecNodes);
                char in_redistribution = 'n';

                AssertEreport(rel_id != InvalidOid, MOD_EXECUTOR, "relation OID is invalid");

                /* Get nodegroup Oid from the index's base table */
                group_oid = get_pgxc_class_groupoid(rel_id);
                AssertEreport(group_oid != InvalidOid, MOD_EXECUTOR, "group OID is invalid");

                in_redistribution = get_pgxc_group_redistributionstatus(group_oid);
                char* group_name = get_pgxc_groupname(group_oid);
                /* Get node list and appending to exec_nodes */
                if (need_full_dn_execution(group_name)) {
                    /* Sepcial path, issue full-DN create index request */
                    exec_nodes->nodeList = GetAllDataNodes();
                } else {
                    nmembers = get_pgxc_groupmembers(group_oid, &members);
                    exec_nodes->nodeList = GetNodeGroupNodeList(members, nmembers);
                    pfree_ext(members);
                }
            }
#endif
            pgstat_set_io_state(IOSTATE_WRITE);
            /* ... and do it */
            WaitState oldStatus = pgstat_report_waitstatus(STATE_CREATE_INDEX);
#ifdef ENABLE_MULTIPLE_NODES
            /*
             * timeseries index create should be in allowSystemTableMods for
             * the index should be create in cstore namespace.
             */
            bool origin_sysTblMode = g_instance.attr.attr_common.allowSystemTableMods;
            if (ts_idx_create && !origin_sysTblMode) {
                g_instance.attr.attr_common.allowSystemTableMods = true;
            }
#endif

            Oid indexRelOid = DefineIndex(rel_id,
                stmt,
                InvalidOid,                                /* no predefined OID */
                false,                                     /* is_alter_table */
                true,                                      /* check_rights */
                !u_sess->upg_cxt.new_catalog_need_storage, /* skip_build */
                false);                                    /* quiet */

#ifndef ENABLE_MULTIPLE_NODES
            if (RelationIsCUFormatByOid(rel_id) && (stmt->primary || stmt->unique)) {
                DefineDeltaUniqueIndex(rel_id, stmt, indexRelOid);
            }
#endif
            pgstat_report_waitstatus(oldStatus);
#ifdef PGXC
            if (IS_PGXC_COORDINATOR && !stmt->isconstraint && !IsConnFromCoord()) {
                query_string = ConstructMesageWithMemInfo(query_string, stmt->memUsage);
                if (u_sess->attr.attr_sql.enable_parallel_ddl && !is_first_node &&
                    (exec_type == EXEC_ON_ALL_NODES || exec_type == EXEC_ON_DATANODES)) {
                    ExecUtilityStmtOnNodes(
                        query_string, exec_nodes, sent_to_remote, stmt->concurrent, EXEC_ON_DATANODES, is_temp);
                } else {
                    ExecUtilityStmtOnNodes(query_string, exec_nodes, sent_to_remote, stmt->concurrent, exec_type, is_temp);
                }
#ifdef ENABLE_MULTIPLE_NODES
                /* Force non-concurrent build on temporary relations, even if CONCURRENTLY was requested */
                char relPersistence = get_rel_persistence(rel_id);
                if (stmt->concurrent &&
                    !(relPersistence == RELPERSISTENCE_TEMP || relPersistence == RELPERSISTENCE_GLOBAL_TEMP)) {
                    /* for index if caller didn't specify, use oid get indexname. */
                    mark_indisvalid_all_cns(stmt->relation->schemaname, get_rel_name(indexRelOid));
                }
#endif
            }
            FreeExecNodes(&exec_nodes);
#endif
        } break;

        case T_RuleStmt: /* CREATE RULE */
        {
#ifdef ENABLE_MULTIPLE_NODES
            bool isredis_rule = false;
            isredis_rule = is_redis_rule((RuleStmt*)parse_tree);
            if (!IsInitdb && !u_sess->attr.attr_sql.enable_cluster_resize && !u_sess->exec_cxt.extension_is_valid 
                            && !IsConnFromCoord() && !isredis_rule)
                ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED), errmsg("RULE is not yet supported.")));
#endif
            DefineRule((RuleStmt*)parse_tree, query_string);
#ifdef PGXC
            if (IS_PGXC_COORDINATOR && !IsConnFromCoord()) {
                RemoteQueryExecType exec_type;
                bool is_temp = false;
                exec_type = get_nodes_4_rules_utility(((RuleStmt*)parse_tree)->relation, &is_temp);
                ExecUtilityStmtOnNodes(query_string, NULL, sent_to_remote, false, exec_type, is_temp);
            }
#endif
        }
            break;

        case T_CreateSeqStmt:
        {
#ifdef PGXC
            CreateSeqStmt* stmt = (CreateSeqStmt*)parse_tree;
            /*
             * We must not scribble on the passed-in CreateSeqStmt, so copy it.  (This is
             * overkill, but easy.)
             */
            stmt = (CreateSeqStmt*)copyObject(stmt);
            if (IS_PGXC_COORDINATOR) {
                ExecNodes* exec_nodes = NULL;

                char* query_stringWithUUID = gen_hybirdmsg_for_CreateSeqStmt(stmt, query_string);

                char* first_exec_node = find_first_exec_cn();
                bool is_first_node = (strcmp(first_exec_node, g_instance.attr.attr_common.PGXCNodeName) == 0);

                /*
                 * If I am the main execute CN but not CCN,
                 * Notify the CCN to create firstly, and then notify other CNs except me.
                 */
                if (u_sess->attr.attr_sql.enable_parallel_ddl && !is_first_node) {
                    /* In case this query is related to a SERIAL execution, just bypass */
                    if (!stmt->is_serial) {
                        bool is_temp = stmt->sequence->relpersistence == RELPERSISTENCE_TEMP;

                        if (!is_temp) {
                            ExecUtilityStmtOnNodes_ParallelDDLMode(
                                query_stringWithUUID, NULL, sent_to_remote, false, EXEC_ON_COORDS, is_temp, first_exec_node);
                        }
                    }
                }

                DefineSequenceWrapper(stmt);

                if (u_sess->attr.attr_sql.enable_parallel_ddl && !is_first_node) {
                    /* In case this query is related to a SERIAL execution, just bypass */
                    if (!stmt->is_serial) {
                        bool is_temp = stmt->sequence->relpersistence == RELPERSISTENCE_TEMP;

                        exec_nodes = GetOwnedByNodes((Node*)stmt);
                        ExecUtilityStmtOnNodes_ParallelDDLMode(query_stringWithUUID,
                            exec_nodes,
                            sent_to_remote,
                            false,
                            EXEC_ON_DATANODES,
                            is_temp,
                            first_exec_node);
                    }
                } else {
                    /* In case this query is related to a SERIAL execution, just bypass */
                    if (!stmt->is_serial) {
                        bool is_temp = stmt->sequence->relpersistence == RELPERSISTENCE_TEMP;
                        exec_nodes = GetOwnedByNodes((Node*)stmt);
                        /* Set temporary object flag in pooler */
                        ExecUtilityStmtOnNodes(
                            query_stringWithUUID, exec_nodes, sent_to_remote, false, CHOOSE_EXEC_NODES(is_temp), is_temp);
                    }
                }
#ifdef ENABLE_MULTIPLE_NODES
                if (IS_MAIN_COORDINATOR && exec_nodes != NULL &&
                    exec_nodes->nodeList->length < u_sess->pgxc_cxt.NumDataNodes) {
                    /* NodeGroup: Create sequence in other datanodes without owned by */
                    char* msg = deparse_create_sequence((Node*)stmt, true);
                    exec_remote_query_4_seq(exec_nodes, msg, stmt->uuid);
                    pfree_ext(msg);
                }
#endif
                pfree_ext(query_stringWithUUID);
                FreeExecNodes(&exec_nodes);
            } else {
                DefineSequenceWrapper(stmt);
            }
#else
        DefineSequenceWrapper(stmt);
#endif

            ClearCreateSeqStmtUUID(stmt);
            break;
        }
        case T_AlterSeqStmt:
#ifdef PGXC
            if (IS_MAIN_COORDINATOR || IS_SINGLE_NODE) {
                PreventAlterSeqInTransaction(is_top_level, (AlterSeqStmt*)parse_tree);
            }
            if (IS_PGXC_COORDINATOR) {
                AlterSeqStmt* stmt = (AlterSeqStmt*)parse_tree;

                char* first_exec_node = find_first_exec_cn();
                bool is_first_node = (strcmp(first_exec_node, g_instance.attr.attr_common.PGXCNodeName) == 0);
                ExecNodes* exec_nodes = NULL;

                if (u_sess->attr.attr_sql.enable_parallel_ddl && !is_first_node) {
                    /* In case this query is related to a SERIAL execution, just bypass */
                    if (!stmt->is_serial) {
                        bool is_temp = false;
                        RemoteQueryExecType exec_type;
                        Oid rel_id = RangeVarGetRelid(stmt->sequence, NoLock, stmt->missing_ok);

                        if (!OidIsValid(rel_id))
                            break;

                        exec_type = ExecUtilityFindNodes(OBJECT_SEQUENCE, rel_id, &is_temp);

                        if (exec_type == EXEC_ON_ALL_NODES || exec_type == EXEC_ON_COORDS) {
                            ExecUtilityStmtOnNodes_ParallelDDLMode(
                                query_string, NULL, sent_to_remote, false, EXEC_ON_COORDS, is_temp, first_exec_node);
                        }
                    }

                    AlterSequenceWrapper((AlterSeqStmt*)parse_tree);
#ifdef ENABLE_MULTIPLE_NODES
                    /* In case this query is related to a SERIAL execution, just bypass */
                    if (IS_MAIN_COORDINATOR && !stmt->is_serial) {
                        bool is_temp = false;
                        RemoteQueryExecType exec_type;
                        Oid rel_id = RangeVarGetRelid(stmt->sequence, NoLock, true);

                        if (!OidIsValid(rel_id))
                            break;

                        exec_type = ExecUtilityFindNodes(OBJECT_SEQUENCE, rel_id, &is_temp);

                        if (exec_type == EXEC_ON_ALL_NODES || exec_type == EXEC_ON_DATANODES) {
                            exec_nodes = GetOwnedByNodes((Node*)stmt);
                            alter_sequence_all_nodes(stmt, exec_nodes);

                            ExecUtilityStmtOnNodes_ParallelDDLMode(query_string,
                                exec_nodes,
                                sent_to_remote,
                                false,
                                EXEC_ON_DATANODES,
                                is_temp,
                                first_exec_node);
                        }
                    }
#endif
                } else {
                    AlterSequenceWrapper((AlterSeqStmt*)parse_tree);

#ifdef ENABLE_MULTIPLE_NODES
                    /* In case this query is related to a SERIAL execution, just bypass */
                    if (IS_MAIN_COORDINATOR && !stmt->is_serial) {
                        bool is_temp = false;
                        RemoteQueryExecType exec_type;
                        Oid rel_id = RangeVarGetRelid(stmt->sequence, NoLock, true);

                        if (!OidIsValid(rel_id))
                            break;

                        exec_type = ExecUtilityFindNodes(OBJECT_SEQUENCE, rel_id, &is_temp);

                        exec_nodes = GetOwnedByNodes((Node*)stmt);
                        alter_sequence_all_nodes(stmt, exec_nodes);

                        ExecUtilityStmtOnNodes(query_string, exec_nodes, sent_to_remote, false, exec_type, is_temp);
                    }
#endif
                }
                FreeExecNodes(&exec_nodes);
            } else {
                AlterSequenceWrapper((AlterSeqStmt*)parse_tree);
            }
#else
        PreventAlterSeqInTransaction(is_top_level, (AlterSeqStmt*)parse_tree);
        AlterSequenceWrapper((AlterSeqStmt*)parse_tree);
#endif
            break;

        case T_DoStmt:
            /* This change is from PG11 commit/rollback patch */
            ExecuteDoStmt((DoStmt*) parse_tree, 
                (!u_sess->SPI_cxt.is_allow_commit_rollback));
            break;

        case T_CreatedbStmt:
#ifdef PGXC
            if (IS_PGXC_COORDINATOR && !IsConnFromCoord()) {
                PreventTransactionChain(is_top_level, "CREATE DATABASE");

                char* first_exec_node = find_first_exec_cn();
                bool is_first_node = (strcmp(first_exec_node, g_instance.attr.attr_common.PGXCNodeName) == 0);

                if (u_sess->attr.attr_sql.enable_parallel_ddl && !is_first_node) {
                    exec_utility_with_message_parallel_ddl_mode(
                        query_string, sent_to_remote, false, first_exec_node, EXEC_ON_COORDS);
                    createdb((CreatedbStmt*)parse_tree);
                    exec_utility_with_message_parallel_ddl_mode(
                        query_string, sent_to_remote, false, first_exec_node, EXEC_ON_DATANODES);
                } else {
                    createdb((CreatedbStmt*)parse_tree);
                    ExecUtilityWithMessage(query_string, sent_to_remote, false);
                }
            } else {
                if (IS_SINGLE_NODE)
                    PreventTransactionChain(is_top_level, "CREATE DATABASE");
                createdb((CreatedbStmt*)parse_tree);
            }
#else
        PreventTransactionChain(is_top_level, "CREATE DATABASE");
        createdb((CreatedbStmt*)parse_tree);
#endif
            break;

        case T_AlterDatabaseStmt:
#ifdef PGXC
            if (IS_PGXC_COORDINATOR && !IsConnFromCoord()) {
                char* first_exec_node = find_first_exec_cn();
                bool is_first_node = (strcmp(first_exec_node, g_instance.attr.attr_common.PGXCNodeName) == 0);
                if (u_sess->attr.attr_sql.enable_parallel_ddl && !is_first_node) {
                    /*
                     * If this is not a SET TABLESPACE statement, just propogate the
                     * cmd as usual.
                     */
                    if (!IsSetTableSpace((AlterDatabaseStmt*)parse_tree))
                        ExecUtilityStmtOnNodes_ParallelDDLMode(
                            query_string, NULL, sent_to_remote, false, EXEC_ON_COORDS, false, first_exec_node);
                    else
                        exec_utility_with_message_parallel_ddl_mode(
                            query_string, sent_to_remote, false, first_exec_node, EXEC_ON_COORDS);

                    AlterDatabase((AlterDatabaseStmt*)parse_tree, is_top_level);
                    if (!IsSetTableSpace((AlterDatabaseStmt*)parse_tree))
                        ExecUtilityStmtOnNodes_ParallelDDLMode(
                            query_string, NULL, sent_to_remote, false, EXEC_ON_DATANODES, false, first_exec_node);
                    else
                        exec_utility_with_message_parallel_ddl_mode(
                            query_string, sent_to_remote, false, first_exec_node, EXEC_ON_DATANODES);
                } else {
                    AlterDatabase((AlterDatabaseStmt*)parse_tree, is_top_level);
                    /*
                     * If this is not a SET TABLESPACE statement, just propogate the
                     * cmd as usual.
                     */
                    if (!IsSetTableSpace((AlterDatabaseStmt*)parse_tree))
                        ExecUtilityStmtOnNodes(query_string, NULL, sent_to_remote, false, EXEC_ON_ALL_NODES, false);
                    else
                        ExecUtilityWithMessage(query_string, sent_to_remote, false);
                }
            } else {
                AlterDatabase((AlterDatabaseStmt*)parse_tree, is_top_level);
            }
#else
        AlterDatabase((AlterDatabaseStmt*)parse_tree, is_top_level);
#endif
            break;
        case T_AlterDatabaseSetStmt:
            ExecAlterDatabaseSetStmt(parse_tree, query_string, sent_to_remote);
            break;

        case T_DropdbStmt: {
            DropdbStmt* stmt = (DropdbStmt*)parse_tree;
#ifdef PGXC
#ifdef ENABLE_MULTIPLE_NODES
            if (IS_PGXC_COORDINATOR && !IsConnFromCoord()) {
                /* clean all connections with dbname on all CNs before db operations */
                PreCleanAndCheckConns(stmt->dbname, stmt->missing_ok);
                /* Allow this to be run inside transaction block on remote nodes */
                PreventTransactionChain(is_top_level, "DROP DATABASE");
            }
#else
            /* Disallow dropping db to be run inside transaction block on single node */
            PreventTransactionChain(is_top_level, "DROP DATABASE");
#endif
            if (IS_PGXC_COORDINATOR) {
                char* first_exec_node = find_first_exec_cn();
                bool is_first_node = (strcmp(first_exec_node, g_instance.attr.attr_common.PGXCNodeName) == 0);

                if (u_sess->attr.attr_sql.enable_parallel_ddl && !is_first_node) {
                    ExecUtilityStmtOnNodes_ParallelDDLMode(
                        query_string, NULL, sent_to_remote, false, EXEC_ON_COORDS, false, first_exec_node);
                    dropdb(stmt->dbname, stmt->missing_ok);
                    ExecUtilityStmtOnNodes_ParallelDDLMode(
                        query_string, NULL, sent_to_remote, false, EXEC_ON_DATANODES, false, first_exec_node);
                } else {
                    dropdb(stmt->dbname, stmt->missing_ok);
                    ExecUtilityStmtOnNodes(query_string, NULL, sent_to_remote, false, EXEC_ON_ALL_NODES, false);
                }
            } else {
                dropdb(stmt->dbname, stmt->missing_ok);
            }
#else
        PreventTransactionChain(is_top_level, "DROP DATABASE");
        dropdb(stmt->dbname, stmt->missing_ok);
#endif
        } break;

            /* Query-level asynchronous notification */
        case T_NotifyStmt:
#ifdef PGXC
            ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED), errmsg("NOFITY statement is not yet supported.")));
#endif /* PGXC */
            {
                NotifyStmt* stmt = (NotifyStmt*)parse_tree;

                PreventCommandDuringRecovery("NOTIFY");
                Async_Notify(stmt->conditionname, stmt->payload);
            }
            break;

        case T_ListenStmt:
#ifdef PGXC
            ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED), errmsg("LISTEN statement is not yet supported.")));
#endif /* PGXC */
            {
                ListenStmt* stmt = (ListenStmt*)parse_tree;

                PreventCommandDuringRecovery("LISTEN");
                CheckRestrictedOperation("LISTEN");
                Async_Listen(stmt->conditionname);
            }
            break;

        case T_UnlistenStmt:
#ifdef PGXC
            ereport(
                ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED), errmsg("UNLISTEN statement is not yet supported.")));
#endif /* PGXC */
            {
                UnlistenStmt* stmt = (UnlistenStmt*)parse_tree;

                PreventCommandDuringRecovery("UNLISTEN");
                CheckRestrictedOperation("UNLISTEN");
                if (stmt->conditionname)
                    Async_Unlisten(stmt->conditionname);
                else
                    Async_UnlistenAll();
            }
            break;

        case T_LoadStmt:
        {
            LoadStmt* stmt = (LoadStmt*)parse_tree;
            if (stmt->is_load_data) {
                if (IS_PGXC_COORDINATOR) {
                    ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                        errmsg("LOAD DATA statement is not yet supported.")));
                }
                TransformLoadDataToCopy(stmt);
                break;
            }

#ifdef PGXC
            ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED), errmsg("LOAD statement is not yet supported.")));
#endif /* PGXC */

                closeAllVfds(); /* probably not necessary... */
                /* Allowed names are restricted if you're not superuser */
                load_file(stmt->filename, !superuser());
            }
#ifdef PGXC
            if (IS_PGXC_COORDINATOR)
                ExecUtilityStmtOnNodes(query_string, NULL, sent_to_remote, false, EXEC_ON_DATANODES, false);
#endif
            break;

        case T_ClusterStmt:
            /* we choose to allow this during "read only" transactions */
            PreventCommandDuringRecovery("CLUSTER");
#ifdef PGXC
            if (IS_PGXC_COORDINATOR) {
                ExecNodes* exec_nodes = NULL;
                char* first_exec_node = find_first_exec_cn();
                bool is_first_node = (strcmp(first_exec_node, g_instance.attr.attr_common.PGXCNodeName) == 0);
                bool is_temp = false;
                RemoteQueryExecType exec_type = EXEC_ON_ALL_NODES;
                ClusterStmt* cstmt = (ClusterStmt*)parse_tree;

                if (cstmt->relation) {
                    Oid rel_id = RangeVarGetRelid(cstmt->relation, NoLock, true);
                    (void)ExecUtilityFindNodes(OBJECT_TABLE, rel_id, &is_temp);
                    exec_type = CHOOSE_EXEC_NODES(is_temp);
                } else if (in_logic_cluster()) {
                    /*
                     * In logic cluster mode, superuser and system DBA can execute CLUSTER
                     * on all nodes; logic cluster users can execute CLUSTER on its node group;
                     * other users can't execute CLUSTER in DNs, only CLUSTER one table.
                     */
                    Oid group_oid = get_current_lcgroup_oid();
                    if (OidIsValid(group_oid)) {
                        exec_nodes = GetNodeGroupExecNodes(group_oid);
                    } else if (!superuser()) {
                        exec_type = EXEC_ON_NONE;
                        ereport(NOTICE,
                            (errmsg("CLUSTER do not run in DNs because User \"%s\" don't "
                                    "attach to any logic cluster.",
                                GetUserNameFromId(GetUserId()))));
                    }
                }
                query_string = ConstructMesageWithMemInfo(query_string, cstmt->memUsage);
                /*
                 * If I am the main execute CN but not CCN,
                 * Notify the CCN to create firstly, and then notify other CNs except me.
                 */
                if (u_sess->attr.attr_sql.enable_parallel_ddl && !is_first_node) {
                    if (!is_temp) {
                        ExecUtilityStmtOnNodes_ParallelDDLMode(query_string,
                            NULL,
                            sent_to_remote,
                            true,
                            EXEC_ON_COORDS,
                            false,
                            first_exec_node,
                            (Node*)parse_tree);
                    }
                }

                cluster((ClusterStmt*)parse_tree, is_top_level);
                if (u_sess->attr.attr_sql.enable_parallel_ddl && !is_first_node) {
                    ExecUtilityStmtOnNodes_ParallelDDLMode(query_string,
                        exec_nodes,
                        sent_to_remote,
                        true,
                        EXEC_ON_DATANODES,
                        false,
                        first_exec_node,
                        (Node*)parse_tree);
                } else {
                    ExecUtilityStmtOnNodes(
                        query_string, exec_nodes, sent_to_remote, true, exec_type, false, (Node*)parse_tree);
                }
                FreeExecNodes(&exec_nodes);
            } else {
                cluster((ClusterStmt*)parse_tree, is_top_level);
            }
#else
        cluster((ClusterStmt*)parse_tree, is_top_level);
#endif
            break;

        case T_VacuumStmt: {
            bool tmp_enable_autoanalyze = u_sess->attr.attr_sql.enable_autoanalyze;
            VacuumStmt* stmt = (VacuumStmt*)parse_tree;
            stmt->dest = dest;

            // for vacuum lazy, no need do IO collection and IO scheduler
            if (ENABLE_WORKLOAD_CONTROL && !(stmt->options & VACOPT_FULL) && u_sess->wlm_cxt->wlm_params.iotrack == 0)
                WLMCleanIOHashTable();

            /*
             * "vacuum full compact" means "vacuum full" in fact
             * for dfs table if VACOPT_COMPACT is enabled
             */
            if (IS_PGXC_COORDINATOR && !IsConnFromCoord()) {
                if (stmt->options & VACOPT_COMPACT)
                    stmt->options |= VACOPT_FULL;
            }

            /* @hdfs
             * isForeignTableAnalyze will be set to true when we need to 
             * analyze a foreign table/foreign tables
             */
            bool isForeignTableAnalyze = IsHDFSForeignTableAnalyzable(stmt);

#ifdef ENABLE_MOT
            if (stmt->isMOTForeignTable && (stmt->options & VACOPT_VACUUM)) {
                stmt->options |= VACOPT_FULL;
            }
#endif

            /*
             * @hdfs
             * Log in data node and run analyze foreign table/tables command is illegal.
             * We need to do scheduling to run analyze foreign table/tables command which
             * can only be done on coordinator node.
             */
            if (IS_PGXC_DATANODE && isForeignTableAnalyze && !IsConnFromCoord()) {
                ereport(WARNING,
                    (errcode(ERRCODE_SYNTAX_ERROR),
                        errmsg("Running analyze on table/tables reside in HDFS directly from data node is not "
                               "supported.")));
                break;
            }

            /*
             * @hdfs
             * On data node, we got hybridmesage includeing data node No.
             * We judge if analyze work belongs to us by PGXCNodeId. If not, we break out.
             */
            if (IS_PGXC_DATANODE && IsConnFromCoord() && isForeignTableAnalyze &&
                (u_sess->pgxc_cxt.PGXCNodeId != (int)stmt->nodeNo)) {
                break;
            }

            if (stmt->isPgFdwForeignTables && stmt->va_cols) {
                ereport(ERROR,
                    (errcode(ERRCODE_UNDEFINED_TABLE), errmsg("This relation doesn't support analyze with column.")));
            }

            /* @hdfs Process MPP Local table "vacuum" "analyze" command. */
            /* forbid auto-analyze inside vacuum/analyze */
            u_sess->attr.attr_sql.enable_autoanalyze = false;
            pgstat_set_io_state(IOSTATE_VACUUM);

            if (IS_PGXC_COORDINATOR && !IsConnFromCoord()) {
                /* init the special nodeId identify which receive analyze command from client */
                stmt->orgCnNodeNo = ~0;
            }

            DoVacuumMppTable(stmt, query_string, is_top_level, sent_to_remote);
            u_sess->attr.attr_sql.enable_autoanalyze = tmp_enable_autoanalyze;
        } break;

        case T_ExplainStmt:
            /*
             * To pass all the llt, do not generate parallel plan
             * if we need to explain the query plan.
             */
            if (u_sess->opt_cxt.parallel_debug_mode == LLT_MODE) {
                u_sess->opt_cxt.query_dop = 1;
                u_sess->opt_cxt.skew_strategy_opt = SKEW_OPT_OFF;
            }
            u_sess->attr.attr_sql.under_explain = true;
            /* Set PTFastQueryShippingStore value. */
            PTFastQueryShippingStore = u_sess->attr.attr_sql.enable_fast_query_shipping;
            PG_TRY();
            {
                if (completion_tag != NULL) {
                    ExplainQuery((ExplainStmt*)parse_tree, query_string, params, dest, completion_tag);
                }
            }
            PG_CATCH();
            {
                u_sess->attr.attr_sql.under_explain = false;
                PG_RE_THROW();
            }
            PG_END_TRY();
            u_sess->attr.attr_sql.under_explain = false;
            /* Reset u_sess->opt_cxt.query_dop. */
            if (u_sess->opt_cxt.parallel_debug_mode == LLT_MODE) {
                u_sess->opt_cxt.query_dop = u_sess->opt_cxt.query_dop_store;
                u_sess->opt_cxt.skew_strategy_opt = u_sess->attr.attr_sql.skew_strategy_store;
            }

            /* Rest PTFastQueryShippingStore. */
            PTFastQueryShippingStore = true;
            break;

        case T_CreateTableAsStmt: {
            CreateTableAsStmt *stmt = (CreateTableAsStmt*)parse_tree;

            /* ExecCreateMatInc */
            if (stmt->into->ivm) {
                ExecCreateMatViewInc((CreateTableAsStmt*)parse_tree, query_string, params);
            } else {
                ExecCreateTableAs((CreateTableAsStmt*)parse_tree, query_string, params, completion_tag);
            }
        } break;

        case T_RefreshMatViewStmt: {
            RefreshMatViewStmt *stmt = (RefreshMatViewStmt *)parse_tree;
 
#ifdef ENABLE_MULTIPLE_NODES
            Query *query = NULL;
            if (IS_PGXC_COORDINATOR && !IsConnFromCoord()) {
                char* first_exec_node = find_first_exec_cn();
                bool is_first_node = (strcmp(first_exec_node, g_instance.attr.attr_common.PGXCNodeName) == 0);
                Relation matview = NULL;

                PushActiveSnapshot(GetTransactionSnapshot());

                /* if current node are not fist node, then need to send to first node first */
                if (u_sess->attr.attr_sql.enable_parallel_ddl && !is_first_node) {
                    ExecUtilityStmtOnNodes_ParallelDDLMode(query_string,
                        NULL,
                        sent_to_remote,
                        false,
                        EXEC_ON_COORDS,
                        false,
                        first_exec_node);

                    matview = heap_openrv(stmt->relation, ExclusiveLock);
                    query = get_matview_query(matview);
                    acquire_mativew_tables_lock(query, stmt->incremental);
                } else {
                    matview = heap_openrv(stmt->relation, ExclusiveLock);
                    query = get_matview_query(matview);
                    acquire_mativew_tables_lock(query, stmt->incremental);

                    ExecUtilityStmtOnNodes(query_string, NULL, sent_to_remote, false, EXEC_ON_COORDS, false);
                }

                /* once all coordinators acquire lock then send to datanode */
                CheckRefreshMatview(matview, is_incremental_matview(matview->rd_id));
                ExecNodes *exec_nodes = getRelationExecNodes(matview->rd_id);

                ExecUtilityStmtOnNodes(query_string,
                    exec_nodes,
                    sent_to_remote,
                    false,
                    EXEC_ON_DATANODES,
                    false);
 
                 /* Pop active snapshow */
                if (ActiveSnapshotSet()) {
                    PopActiveSnapshot();
                }

                heap_close(matview, NoLock);
            } else if (IS_PGXC_COORDINATOR) {
                /* attach lock on matview and its table */
                Relation matview = heap_openrv(stmt->relation, ExclusiveLock);
                query = get_matview_query(matview);
                acquire_mativew_tables_lock(query, stmt->incremental);
                heap_close(matview, NoLock);
            }

            /* something happen on datanodes */
            if (IS_PGXC_DATANODE)
#else
            Relation matview = heap_openrv(stmt->relation, stmt->incremental ? ExclusiveLock : AccessExclusiveLock);
            CheckRefreshMatview(matview, is_incremental_matview(matview->rd_id));
            heap_close(matview, NoLock);
#endif
            {
                if (stmt->incremental) {
                    ExecRefreshMatViewInc(stmt, query_string, params, completion_tag);
                }
                if (!stmt->incremental) {
                    ExecRefreshMatView(stmt, query_string, params, completion_tag);
                }
            }
        } break;
		
#ifndef ENABLE_MULTIPLE_NODES
        case T_AlterSystemStmt:
            /*
             * 1.AlterSystemSet don't care whether the node is PRIMARY or STANDBY as same as gs_guc.
             * 2.AlterSystemSet don't care whether the database is read-only, as same as gs_guc.
             * 3.It cannot be executed in a transaction because it is not rollbackable.
             */
            PreventTransactionChain(is_top_level, "ALTER SYSTEM SET");

            AlterSystemSetConfigFile((AlterSystemStmt*)parse_tree);
            break;
#endif

        case T_VariableSetStmt:
            ExecSetVariableStmt((VariableSetStmt*)parse_tree);
#ifdef PGXC
            /* Let the pooler manage the statement */
            if (IS_PGXC_COORDINATOR && !IsConnFromCoord()) {
                VariableSetStmt* stmt = (VariableSetStmt*)parse_tree;
                char* mask_string = NULL;

                mask_string = maskPassword(query_string);
                if (mask_string == NULL)
                    mask_string = (char*)query_string;

                // do not send the variable which in blacklist  to other nodes
                if (IsVariableinBlackList(stmt->name)) {
                    break;
                }

                if (u_sess->catalog_cxt.overrideStack && (!pg_strcasecmp(stmt->name, SEARCH_PATH_GUC_NAME) ||
                                                             !pg_strcasecmp(stmt->name, CURRENT_SCHEMA_GUC_NAME))) {
                    /*
                     * set search_path or current_schema inside stored procedure is invalid,
                     * do not send to other nodes.
                     */
                    OverrideStackEntry* entry = NULL;
                    entry = (OverrideStackEntry*)linitial(u_sess->catalog_cxt.overrideStack);
                    if (entry->inProcedure)
                        break;
                }

                /*
                 * If command is local and we are not in a transaction block do NOT
                 * send this query to backend nodes, it is just bypassed by the backend.
                 */
                if (stmt->is_local) {
                    if (IsTransactionBlock()) {
                        if (PoolManagerSetCommand(POOL_CMD_LOCAL_SET, mask_string, stmt->name) < 0) {
                            /* Add retry query error code ERRCODE_SET_QUERY for error "ERROR SET query". */
                            ereport(ERROR, (errcode(ERRCODE_SET_QUERY), errmsg("openGauss: ERROR SET query")));
                        }
                    }
                } else {
                    /* when set command in function, treat it as in transaction. */
                    if (!IsTransactionBlock() && !(dest->mydest == DestSPI)) {
                        if (PoolManagerSetCommand(POOL_CMD_GLOBAL_SET, mask_string, stmt->name) < 0) {
                            /* Add retry query error code ERRCODE_SET_QUERY for error "ERROR SET query". */
                            ereport(ERROR, (errcode(ERRCODE_SET_QUERY), errmsg("openGauss: ERROR SET query")));
                        }
                    } else {
                        ExecUtilityStmtOnNodes(mask_string, NULL, sent_to_remote, false, EXEC_ON_ALL_NODES, false);

                        // Append set command to input_set_message, this will be sent to pool when this Transaction
                        // commit.
                        int ret = check_set_message_to_send(stmt, query_string);

                        if (ret != -1) {
                            if (ret == 0)
                                append_set_message(query_string);
                            else
                                make_set_message(); /* make new message string */
                        }
                    }
                }
                if (mask_string != query_string)
                    pfree(mask_string);
            }
#endif
            break;

        case T_VariableShowStmt: {
            VariableShowStmt* n = (VariableShowStmt*)parse_tree;

            GetPGVariable(n->name, n->likename, dest);
        } break;
        case T_ShutdownStmt: {
            ShutdownStmt* n = (ShutdownStmt*)parse_tree;

            DoShutdown(n);
        } break;

        case T_DiscardStmt:
#ifdef PGXC
            ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED), errmsg("DISCARD statement is not yet supported.")));
#endif /* PGXC */
            /* should we allow DISCARD PLANS? */
            CheckRestrictedOperation("DISCARD");
            DiscardCommand((DiscardStmt*)parse_tree, is_top_level);
#ifdef PGXC
            /*
             * Discard objects for all the sessions possible.
             * For example, temporary tables are created on all Datanodes
             * and Coordinators.
             */
            if (IS_PGXC_COORDINATOR)
                ExecUtilityStmtOnNodes(query_string,
                    NULL,
                    sent_to_remote,
                    true,
                    CHOOSE_EXEC_NODES(((DiscardStmt*)parse_tree)->target == DISCARD_TEMP),
                    false);
#endif
            break;

        case T_CreateTrigStmt:
            (void)CreateTrigger(
                (CreateTrigStmt*)parse_tree, query_string, InvalidOid, InvalidOid, InvalidOid, InvalidOid, false);
#ifdef PGXC
            if (IS_PGXC_COORDINATOR) {
                CreateTrigStmt* stmt = (CreateTrigStmt*)parse_tree;
                RemoteQueryExecType exec_type;
                bool is_temp = false;
                Oid rel_id = RangeVarGetRelidExtended(stmt->relation, NoLock, false, false, false, true, NULL, NULL);

                exec_type = ExecUtilityFindNodes(OBJECT_TABLE, rel_id, &is_temp);

                ExecUtilityStmtOnNodes(query_string, NULL, sent_to_remote, false, exec_type, is_temp, (Node*)stmt);
            }
#endif
            break;

        case T_CreatePLangStmt:
            if (!IsInitdb)
                ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED), errmsg("new language is not yet supported.")));
            CreateProceduralLanguage((CreatePLangStmt*)parse_tree);
#ifdef PGXC
            if (IS_PGXC_COORDINATOR)
                ExecUtilityStmtOnNodes(query_string, NULL, sent_to_remote, false, EXEC_ON_ALL_NODES, false);
#endif
            break;

            /*
             * ******************************** DOMAIN statements ****
             */
        case T_CreateDomainStmt:
            if (!IsInitdb && !u_sess->attr.attr_common.IsInplaceUpgrade)
                ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED), errmsg("domain is not yet supported.")));
            DefineDomain((CreateDomainStmt*)parse_tree);
#ifdef PGXC
            if (IS_PGXC_COORDINATOR)
                ExecUtilityStmtOnNodes(query_string, NULL, sent_to_remote, false, EXEC_ON_ALL_NODES, false);
#endif
            break;

            /*
             * ******************************** ROLE statements ****
             */
        case T_CreateRoleStmt:
#ifdef PGXC
            if (IS_PGXC_COORDINATOR) {
                char* first_exec_node = find_first_exec_cn();
                bool is_first_node = (strcmp(first_exec_node, g_instance.attr.attr_common.PGXCNodeName) == 0);

                if (u_sess->attr.attr_sql.enable_parallel_ddl && !is_first_node) {
                    ExecUtilityStmtOnNodes_ParallelDDLMode(
                        query_string, NULL, sent_to_remote, false, EXEC_ON_COORDS, false, first_exec_node);
                    CreateRole((CreateRoleStmt*)parse_tree);
                    ExecUtilityStmtOnNodes_ParallelDDLMode(
                        query_string, NULL, sent_to_remote, false, EXEC_ON_DATANODES, false, first_exec_node);
                } else {
                    CreateRole((CreateRoleStmt*)parse_tree);
                    ExecUtilityStmtOnNodes(query_string, NULL, sent_to_remote, false, EXEC_ON_ALL_NODES, false);
                }
            } else {
                CreateRole((CreateRoleStmt*)parse_tree);
            }
#else
        CreateRole((CreateRoleStmt*)parse_tree);
#endif
            break;

        case T_AlterRoleStmt:
#ifdef PGXC
            if (IS_PGXC_COORDINATOR) {
                char* first_exec_node = find_first_exec_cn();
                bool is_first_node = (strcmp(first_exec_node, g_instance.attr.attr_common.PGXCNodeName) == 0);

                if (u_sess->attr.attr_sql.enable_parallel_ddl && !is_first_node) {
                    ExecUtilityStmtOnNodes_ParallelDDLMode(
                        query_string, NULL, sent_to_remote, false, EXEC_ON_COORDS, false, first_exec_node);
                    AlterRole((AlterRoleStmt*)parse_tree);
                    ExecUtilityStmtOnNodes_ParallelDDLMode(
                        query_string, NULL, sent_to_remote, false, EXEC_ON_DATANODES, false, first_exec_node);
                } else {
                    AlterRole((AlterRoleStmt*)parse_tree);
                    ExecUtilityStmtOnNodes(query_string, NULL, sent_to_remote, false, EXEC_ON_ALL_NODES, false);
                }
            } else {
                AlterRole((AlterRoleStmt*)parse_tree);
            }
#else
        AlterRole((AlterRoleStmt*)parse_tree);
#endif
            break;
        case T_AlterRoleSetStmt:
            ExecAlterRoleSetStmt(parse_tree, query_string, sent_to_remote);
            break;

        case T_DropRoleStmt:
#ifdef PGXC
            /* Clean connections before dropping a user on local node */
            if (IS_PGXC_COORDINATOR && !IsConnFromCoord()) {
                ListCell* item = NULL;
                foreach (item, ((DropRoleStmt*)parse_tree)->roles) {
                    const char* role = strVal(lfirst(item));
                    DropRoleStmt* stmt = (DropRoleStmt*)parse_tree;

                    /* clean all connections with role on all CNs */
                    PreCleanAndCheckUserConns(role, stmt->missing_ok);
                }
            }

            if (IS_PGXC_COORDINATOR) {
                char* first_exec_node = find_first_exec_cn();
                bool is_first_node = (strcmp(first_exec_node, g_instance.attr.attr_common.PGXCNodeName) == 0);

                if (u_sess->attr.attr_sql.enable_parallel_ddl && !is_first_node) {
                    ExecUtilityStmtOnNodes_ParallelDDLMode(
                        query_string, NULL, sent_to_remote, false, EXEC_ON_COORDS, false, first_exec_node);
                    DropRole((DropRoleStmt*)parse_tree);
                    ExecUtilityStmtOnNodes_ParallelDDLMode(
                        query_string, NULL, sent_to_remote, false, EXEC_ON_DATANODES, false, first_exec_node);
                } else {
                    DropRole((DropRoleStmt*)parse_tree);
                    ExecUtilityStmtOnNodes(query_string, NULL, sent_to_remote, false, EXEC_ON_ALL_NODES, false);
                }
            } else {
                DropRole((DropRoleStmt*)parse_tree);
            }
#else
        DropRole((DropRoleStmt*)parse_tree);
#endif
            break;

        case T_DropOwnedStmt:
#ifdef PGXC
            if (IS_PGXC_COORDINATOR) {
                char* first_exec_node = find_first_exec_cn();
                bool is_first_node = (strcmp(first_exec_node, g_instance.attr.attr_common.PGXCNodeName) == 0);

                if (u_sess->attr.attr_sql.enable_parallel_ddl && !is_first_node) {
                    ExecUtilityStmtOnNodes_ParallelDDLMode(
                        query_string, NULL, sent_to_remote, false, EXEC_ON_COORDS, false, first_exec_node);
                    DropOwnedObjects((DropOwnedStmt*)parse_tree);
                    ExecUtilityStmtOnNodes_ParallelDDLMode(
                        query_string, NULL, sent_to_remote, false, EXEC_ON_DATANODES, false, first_exec_node);
                } else {
                    DropOwnedObjects((DropOwnedStmt*)parse_tree);
                    ExecUtilityStmtOnNodes(query_string, NULL, sent_to_remote, false, EXEC_ON_ALL_NODES, false);
                }
            } else {
                DropOwnedObjects((DropOwnedStmt*)parse_tree);
            }
#else
        DropOwnedObjects((DropOwnedStmt*)parse_tree);
#endif
            break;

        case T_ReassignOwnedStmt:
#ifdef PGXC
            if (IS_PGXC_COORDINATOR) {
                char* first_exec_node = find_first_exec_cn();
                bool is_first_node = (strcmp(first_exec_node, g_instance.attr.attr_common.PGXCNodeName) == 0);

                if (u_sess->attr.attr_sql.enable_parallel_ddl && !is_first_node) {
                    ExecUtilityStmtOnNodes_ParallelDDLMode(
                        query_string, NULL, sent_to_remote, false, EXEC_ON_COORDS, false, first_exec_node);
                    ReassignOwnedObjects((ReassignOwnedStmt*)parse_tree);
                    ExecUtilityStmtOnNodes_ParallelDDLMode(
                        query_string, NULL, sent_to_remote, false, EXEC_ON_DATANODES, false, first_exec_node);
                } else {
                    ReassignOwnedObjects((ReassignOwnedStmt*)parse_tree);
                    ExecUtilityStmtOnNodes(query_string, NULL, sent_to_remote, false, EXEC_ON_ALL_NODES, false);
                }
            } else {
                ReassignOwnedObjects((ReassignOwnedStmt*)parse_tree);
            }
#else
        ReassignOwnedObjects((ReassignOwnedStmt*)parse_tree);
#endif
            break;

        case T_LockStmt:

            /*
             * Since the lock would just get dropped immediately, LOCK TABLE
             * outside a transaction block is presumed to be user error.
             */
            RequireTransactionChain(is_top_level, "LOCK TABLE");
#ifdef PGXC
            /* only lock local table if cm_agent do Lock Stmt */
            if (IS_PGXC_COORDINATOR && !IsConnFromCoord() &&
                !(u_sess->attr.attr_common.xc_maintenance_mode &&
                    (strcmp(u_sess->attr.attr_common.application_name, "cm_agent") == 0))) {
                ListCell* cell = NULL;
                bool has_nontemp = false;
                bool has_temp = false;
                bool is_temp = false;
                char* first_exec_node = find_first_exec_cn();
                bool is_first_node = (strcmp(first_exec_node, g_instance.attr.attr_common.PGXCNodeName) == 0);

                foreach (cell, ((LockStmt*)parse_tree)->relations) {
                    RangeVar* r = (RangeVar*)lfirst(cell);
                    Oid rel_id = RangeVarGetRelid(r, NoLock, true);
                    (void)ExecUtilityFindNodes(OBJECT_TABLE, rel_id, &is_temp);
                    has_temp |= is_temp;
                    has_nontemp |= !is_temp;

                    if (has_temp && has_nontemp)
                        ereport(ERROR,
                            (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                errmsg("LOCK not supported for TEMP and non-TEMP objects together"),
                                errdetail("You should separate TEMP and non-TEMP objects")));
                }

                if (u_sess->attr.attr_sql.enable_parallel_ddl && !is_first_node) {
                    RemoteQuery* step = makeNode(RemoteQuery);
                    step->combine_type = COMBINE_TYPE_SAME;
                    step->sql_statement = (char*)query_string;
                    step->exec_type = has_temp ? EXEC_ON_NONE : EXEC_ON_COORDS;
                    step->exec_nodes = NULL;
                    step->is_temp = has_temp;
                    ExecRemoteUtility_ParallelDDLMode(step, first_exec_node);
                    pfree_ext(step);
                }

                LockTableCommand((LockStmt*)parse_tree);

                if (u_sess->attr.attr_sql.enable_parallel_ddl && !is_first_node) {
                    ExecUtilityStmtOnNodes_ParallelDDLMode(query_string,
                        NULL,
                        sent_to_remote,
                        false,
                        EXEC_ON_DATANODES,
                        false,
                        first_exec_node,
                        (Node*)parse_tree);
                } else {
                    ExecUtilityStmtOnNodes(
                        query_string, NULL, sent_to_remote, false, CHOOSE_EXEC_NODES(has_temp), false, (Node*)parse_tree);
                }
            } else {
                LockTableCommand((LockStmt*)parse_tree);
            }
#else
        LockTableCommand((LockStmt*)parse_tree);
#endif
            break;

        case T_ConstraintsSetStmt:
            AfterTriggerSetState((ConstraintsSetStmt*)parse_tree);
#ifdef PGXC
            /*
             * Let the pooler manage the statement, SET CONSTRAINT can just be used
             * inside a transaction block, hence it has no effect outside that, so use
             * it as a local one.
             */
            if (IS_PGXC_COORDINATOR && !IsConnFromCoord() && IsTransactionBlock()) {
                const int MAX_PARAM_LEN = 1024;
                char tmpName[MAX_PARAM_LEN + 1] = {0};
                char *guc_name = GetGucName(query_string, tmpName);
                if (PoolManagerSetCommand(POOL_CMD_LOCAL_SET, query_string, guc_name) < 0) {
                    /* Add retry query error code ERRCODE_SET_QUERY for error "ERROR SET query". */
                    ereport(ERROR, (errcode(ERRCODE_SET_QUERY), errmsg("openGauss: ERROR SET query")));
                }
            }
#endif
            break;

        case T_CheckPointStmt:
            if (!(superuser() || isOperatoradmin(GetUserId())))
                ereport(
                    ERROR, (errcode(ERRCODE_INSUFFICIENT_PRIVILEGE),
                        errmsg("must be system admin or operator admin to do CHECKPOINT")));
            /*
             * You might think we should have a PreventCommandDuringRecovery()
             * here, but we interpret a CHECKPOINT command during recovery as
             * a request for a restartpoint instead. We allow this since it
             * can be a useful way of reducing switchover time when using
             * various forms of replication.
             */
            RequestCheckpoint(CHECKPOINT_IMMEDIATE | CHECKPOINT_WAIT | (RecoveryInProgress() ? 0 : CHECKPOINT_FORCE));
#ifdef PGXC
            if (IS_PGXC_COORDINATOR)
                ExecUtilityStmtOnNodes(query_string, NULL, sent_to_remote, true, EXEC_ON_DATANODES, false);
#endif
            break;

#ifdef PGXC
        case T_BarrierStmt:
#ifndef ENABLE_MULTIPLE_NODES
            DISTRIBUTED_FEATURE_NOT_SUPPORTED();
#endif
            RequestBarrier(((BarrierStmt*)parse_tree)->id, completion_tag);
            break;

        /*
         * Node DDL is an operation local to Coordinator.
         * In case of a new node being created in the cluster,
         * it is necessary to create this node on all the Coordinators independently.
         */
        case T_AlterNodeStmt:
#ifndef ENABLE_MULTIPLE_NODES
            DISTRIBUTED_FEATURE_NOT_SUPPORTED();
#endif
            PgxcNodeAlter((AlterNodeStmt*)parse_tree);
            break;

        case T_CreateNodeStmt:
#ifndef ENABLE_MULTIPLE_NODES
            DISTRIBUTED_FEATURE_NOT_SUPPORTED();
#endif
            PgxcNodeCreate((CreateNodeStmt*)parse_tree);
            break;

        case T_AlterCoordinatorStmt: {
            AlterCoordinatorStmt* stmt = (AlterCoordinatorStmt*)parse_tree;
            if (IS_PGXC_COORDINATOR && IsConnFromCoord())
                PgxcCoordinatorAlter((AlterCoordinatorStmt*)parse_tree);
            const char* coor_name = stmt->node_name;
            Oid noid = get_pgxc_nodeoid(coor_name);
            char node_type = get_pgxc_nodetype(noid);
            List* cn_list = NIL;

            if (node_type != 'C')
                ereport(ERROR,
                    (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                        errmsg("PGXC Node %s is not a valid coordinator", coor_name)));

            /* alter coordinator node should run on CN nodes only */
            if (IS_PGXC_COORDINATOR && !IsConnFromCoord() && node_type == PGXC_NODE_COORDINATOR) {
                /* exec cn record stmt method */
                t_thrd.xact_cxt.AlterCoordinatorStmt = true;
                /* generate cn list */
                ListCell* lc = NULL;
                foreach (lc, stmt->coor_nodes) {
                    char* lc_name = strVal(lfirst(lc));
                    int lc_idx = PgxcGetNodeIndex(lc_name);

                    /* only support cn in with clause */
                    if (lc_idx == -1)
                        ereport(ERROR,
                            (errcode(ERRCODE_SYNTAX_ERROR), errmsg("Invalid value \"%s\" in WITH clause", lc_name)));
                    /* except myself */
                    if (u_sess->pgxc_cxt.PGXCNodeId - 1 != lc_idx)
                        cn_list = lappend_int(cn_list, lc_idx);
                    else
                        PgxcCoordinatorAlter((AlterCoordinatorStmt*)parse_tree);
                }

                /* need to alter on remote CN(s) */
                if (cn_list != NIL) {
                    ExecNodes* nodes = makeNode(ExecNodes);
                    nodes->nodeList = cn_list;
                    ExecUtilityStmtOnNodes(query_string, nodes, sent_to_remote, false, EXEC_ON_COORDS, false);
                    FreeExecNodes(&nodes);
                }
            }
        } break;

        case T_DropNodeStmt:
#ifndef ENABLE_MULTIPLE_NODES
            DISTRIBUTED_FEATURE_NOT_SUPPORTED();
#endif
            {
                DropNodeStmt* stmt = (DropNodeStmt*)parse_tree;
                char node_type = PgxcNodeRemove(stmt);

                /* drop node should run on CN nodes only */
                if (IS_PGXC_COORDINATOR && !IsConnFromCoord() && node_type != PGXC_NODE_NONE &&
                    (!g_instance.attr.attr_storage.IsRoachStandbyCluster)) {
                    List* cn_list = NIL;
                    int cn_num = 0;
                    int cn_delete_idx = -2;

                    /* for drop node cn */
                    if (node_type == PGXC_NODE_COORDINATOR) {
                        /* get the node index of the dropped cn */
                        const char* node_name = stmt->node_name;
                        cn_delete_idx = PgxcGetNodeIndex(node_name);

                        /* special case: node is created just in this session */
                        if (cn_delete_idx == -1) {
                            elog(DEBUG2, "Drop Node %s: get node index -1", node_name);
                        }
                    }

                    /* generate cn list */
                    if (stmt->remote_nodes) {
                        ListCell* lc = NULL;
                        foreach (lc, stmt->remote_nodes) {
                            char* lc_name = strVal(lfirst(lc));
                            int lc_idx = PgxcGetNodeIndex(lc_name);

                            /* only support cn in with clause */
                            if (lc_idx == -1 || lc_idx == cn_delete_idx)
                                ereport(ERROR,
                                    (errcode(ERRCODE_SYNTAX_ERROR),
                                        errmsg("Invalid value \"%s\" in WITH clause", lc_name)));

                            /* except myself */
                            if (u_sess->pgxc_cxt.PGXCNodeId - 1 != lc_idx)
                                cn_list = lappend_int(cn_list, lc_idx);
                        }
                    } else {
                        cn_list = GetAllCoordNodes();
                        cn_num = list_length(cn_list);
                        /* remove the dropped cn from nodelist */
                        if (cn_num > 0 && cn_delete_idx > -1) {
                            cn_list = list_delete_int(cn_list, cn_delete_idx);
                            Assert(cn_num - 1 == list_length(cn_list));
                        }
                    }

                    cn_num = list_length(cn_list);
                    /* need to drop on remote CN(s) */
                    if (cn_num > 0) {
                        ExecNodes* nodes = makeNode(ExecNodes);
                        nodes->nodeList = cn_list;
                        ExecUtilityStmtOnNodes(query_string, nodes, sent_to_remote, false, EXEC_ON_COORDS, false);
                        FreeExecNodes(&nodes);
                    } else if (cn_list != NULL) {
                        list_free_ext(cn_list);
                    }
                }
            }
            break;

        case T_CreateGroupStmt:
#ifndef ENABLE_MULTIPLE_NODES
            DISTRIBUTED_FEATURE_NOT_SUPPORTED();
#endif
            if (IS_SINGLE_NODE)
                ereport(ERROR,
                    (errcode(ERRCODE_FEATURE_NOT_SUPPORTED), errmsg("Don't support node group in single_node mode.")));

            PgxcGroupCreate((CreateGroupStmt*)parse_tree);

            /* create node group should run on CN nodes only */
            if (IS_PGXC_COORDINATOR && !IsConnFromCoord()) {
                ExecUtilityStmtOnNodes(query_string, NULL, sent_to_remote, false, EXEC_ON_COORDS, false);
            }
            break;

        case T_AlterGroupStmt:
#ifndef ENABLE_MULTIPLE_NODES
            DISTRIBUTED_FEATURE_NOT_SUPPORTED();
#endif
            if (IS_SINGLE_NODE)
                ereport(ERROR,
                    (errcode(ERRCODE_FEATURE_NOT_SUPPORTED), errmsg("Don't support node group in single_node mode.")));

#ifdef ENABLE_MULTIPLE_NODES
            PgxcGroupAlter((AlterGroupStmt*)parse_tree);
            /* alter node group should run on CN nodes only */
            {
                AlterGroupType alter_type = ((AlterGroupStmt*)parse_tree)->alter_type;
                if (IS_PGXC_COORDINATOR && !IsConnFromCoord() &&
                    (alter_type != AG_SET_DEFAULT && alter_type != AG_SET_SEQ_ALLNODES &&
                        alter_type != AG_SET_SEQ_SELFNODES)) {
                    ExecUtilityStmtOnNodes(query_string, NULL, sent_to_remote, false, EXEC_ON_COORDS, false);
                }
            }
#endif
            break;

        case T_DropGroupStmt:
#ifndef ENABLE_MULTIPLE_NODES
            DISTRIBUTED_FEATURE_NOT_SUPPORTED();
#endif
            if (IS_SINGLE_NODE)
                ereport(ERROR,
                    (errcode(ERRCODE_FEATURE_NOT_SUPPORTED), errmsg("Don't support node group in single_node mode.")));

            PgxcGroupRemove((DropGroupStmt*)parse_tree);

            /* drop node group should run on CN nodes only */
            if (IS_PGXC_COORDINATOR && !IsConnFromCoord()) {
                ExecUtilityStmtOnNodes(query_string, NULL, sent_to_remote, false, EXEC_ON_COORDS, false);
            }
            break;

        case T_CreatePolicyLabelStmt:
#ifdef PGXC
            if (IS_PGXC_COORDINATOR) {
                char *FirstExecNode = find_first_exec_cn();
                bool isFirstNode = (strcmp(FirstExecNode, g_instance.attr.attr_common.PGXCNodeName) == 0);

                if (u_sess->attr.attr_sql.enable_parallel_ddl && !isFirstNode) {
                    ExecUtilityStmtOnNodes_ParallelDDLMode(query_string, NULL, sent_to_remote,
                                                           false, EXEC_ON_COORDS, false, FirstExecNode);
                    create_policy_label((CreatePolicyLabelStmt *) parse_tree);
                    ExecUtilityStmtOnNodes_ParallelDDLMode(query_string, NULL, sent_to_remote,
                                                           false, EXEC_ON_DATANODES, false, FirstExecNode);
                } else {
                    create_policy_label((CreatePolicyLabelStmt *) parse_tree);
                    ExecUtilityStmtOnNodes(query_string, NULL, sent_to_remote, false, EXEC_ON_ALL_NODES, false);
                }
            } else {
                create_policy_label((CreatePolicyLabelStmt *) parse_tree);
            }
#else
            create_policy_label((CreatePolicyLabelStmt *) parse_tree);
#endif
            break;

        case T_AlterPolicyLabelStmt:
#ifdef PGXC
            if (IS_PGXC_COORDINATOR) {
                char *FirstExecNode = find_first_exec_cn();
                bool isFirstNode = (strcmp(FirstExecNode, g_instance.attr.attr_common.PGXCNodeName) == 0);

                if (u_sess->attr.attr_sql.enable_parallel_ddl && !isFirstNode) {
                    ExecUtilityStmtOnNodes_ParallelDDLMode(query_string, NULL, sent_to_remote,
                                                           false, EXEC_ON_COORDS, false, FirstExecNode);
                    alter_policy_label((AlterPolicyLabelStmt *) parse_tree);
                    ExecUtilityStmtOnNodes_ParallelDDLMode(query_string, NULL, sent_to_remote,
                                                           false, EXEC_ON_DATANODES, false, FirstExecNode);
                } else {
                    alter_policy_label((AlterPolicyLabelStmt *) parse_tree);
                    ExecUtilityStmtOnNodes(query_string, NULL, sent_to_remote, false, EXEC_ON_ALL_NODES, false);
                }
            } else {
                alter_policy_label((AlterPolicyLabelStmt *) parse_tree);
            }
#else
            alter_policy_label((AlterPolicyLabelStmt *) parse_tree);
#endif
            break;

        case T_DropPolicyLabelStmt:
#ifdef PGXC
            if (IS_PGXC_COORDINATOR) {
                char *FirstExecNode = find_first_exec_cn();
                bool isFirstNode = (strcmp(FirstExecNode, g_instance.attr.attr_common.PGXCNodeName) == 0);

                if (u_sess->attr.attr_sql.enable_parallel_ddl && !isFirstNode) {
                    ExecUtilityStmtOnNodes_ParallelDDLMode(query_string, NULL, sent_to_remote, false, EXEC_ON_COORDS,
                        false, FirstExecNode);
                    drop_policy_label((DropPolicyLabelStmt *) parse_tree);
                    ExecUtilityStmtOnNodes_ParallelDDLMode(query_string, NULL, sent_to_remote, false, EXEC_ON_DATANODES,
                        false, FirstExecNode);
                } else {
                    drop_policy_label((DropPolicyLabelStmt *) parse_tree);
                    ExecUtilityStmtOnNodes(query_string, NULL, sent_to_remote, false, EXEC_ON_ALL_NODES, false);
                }
            } else {
                drop_policy_label((DropPolicyLabelStmt *) parse_tree);
            }
#else
            drop_policy_label((DropPolicyLabelStmt *) parse_tree);
#endif
            break;

        case T_CreateAuditPolicyStmt:
#ifdef PGXC
            if (IS_PGXC_COORDINATOR) {
                char *FirstExecNode = find_first_exec_cn();
                bool isFirstNode = (strcmp(FirstExecNode, g_instance.attr.attr_common.PGXCNodeName) == 0);

                if (u_sess->attr.attr_sql.enable_parallel_ddl && !isFirstNode) {
                    ExecUtilityStmtOnNodes_ParallelDDLMode(query_string, NULL, sent_to_remote, false, EXEC_ON_COORDS,
                        false, FirstExecNode);
                    create_audit_policy((CreateAuditPolicyStmt *)parse_tree);
                } else {
                    create_audit_policy((CreateAuditPolicyStmt *)parse_tree);
                    ExecUtilityStmtOnNodes(query_string, NULL, sent_to_remote, false, EXEC_ON_COORDS, false);
                }
            } 
            if (IS_SINGLE_NODE) {
                create_audit_policy((CreateAuditPolicyStmt *) parse_tree);
            }
#else
            create_audit_policy((CreateAuditPolicyStmt *) parse_tree);
#endif
            break;

        case T_AlterAuditPolicyStmt:
#ifdef PGXC
           if (IS_PGXC_COORDINATOR) {
                char *FirstExecNode = find_first_exec_cn();
                bool isFirstNode = (strcmp(FirstExecNode, g_instance.attr.attr_common.PGXCNodeName) == 0);

                if (u_sess->attr.attr_sql.enable_parallel_ddl && !isFirstNode) {
                    ExecUtilityStmtOnNodes_ParallelDDLMode(query_string, NULL, sent_to_remote, false, EXEC_ON_COORDS,
                        false, FirstExecNode);
                    alter_audit_policy((AlterAuditPolicyStmt *)parse_tree);
                } else {
                    alter_audit_policy((AlterAuditPolicyStmt *) parse_tree);
                    ExecUtilityStmtOnNodes(query_string, NULL, sent_to_remote, false, EXEC_ON_COORDS, false);
                }
            }
            if (IS_SINGLE_NODE) {
                alter_audit_policy((AlterAuditPolicyStmt *) parse_tree);
            }
#else
            alter_audit_policy((AlterAuditPolicyStmt *) parse_tree);
#endif

            break;

        case T_DropAuditPolicyStmt:
#ifdef PGXC
            if (IS_PGXC_COORDINATOR) {
                char *FirstExecNode = find_first_exec_cn();
                bool isFirstNode = (strcmp(FirstExecNode, g_instance.attr.attr_common.PGXCNodeName) == 0);

                if (u_sess->attr.attr_sql.enable_parallel_ddl && !isFirstNode) {
                    ExecUtilityStmtOnNodes_ParallelDDLMode(query_string, NULL, sent_to_remote, false, EXEC_ON_COORDS,
                        false, FirstExecNode);
                    drop_audit_policy((DropAuditPolicyStmt *) parse_tree);
                } else {
                    drop_audit_policy((DropAuditPolicyStmt *) parse_tree);
                    ExecUtilityStmtOnNodes(query_string, NULL, sent_to_remote, false, EXEC_ON_COORDS, false);
                }
            } 
            if (IS_SINGLE_NODE) {
                drop_audit_policy((DropAuditPolicyStmt *) parse_tree);
            }
#else
            drop_audit_policy((DropAuditPolicyStmt *) parse_tree);
#endif
            break;

        case T_CreateMaskingPolicyStmt:
#ifdef PGXC
           if (IS_PGXC_COORDINATOR)
           {
               char *FirstExecNode = find_first_exec_cn();
               bool isFirstNode = (strcmp(FirstExecNode, g_instance.attr.attr_common.PGXCNodeName) == 0);

			   if(u_sess->attr.attr_sql.enable_parallel_ddl && !isFirstNode)
               {
                   ExecUtilityStmtOnNodes_ParallelDDLMode(query_string, NULL, sent_to_remote, false, EXEC_ON_COORDS, false, FirstExecNode);
                   create_masking_policy((CreateMaskingPolicyStmt *) parse_tree);
                   ExecUtilityStmtOnNodes_ParallelDDLMode(query_string, NULL, sent_to_remote, false, EXEC_ON_DATANODES, false, FirstExecNode);
               } else {
                   create_masking_policy((CreateMaskingPolicyStmt *) parse_tree);
                   ExecUtilityStmtOnNodes(query_string, NULL, sent_to_remote, false, EXEC_ON_ALL_NODES, false);
               }
           } else {
               create_masking_policy((CreateMaskingPolicyStmt *) parse_tree);
           }
#else
           create_masking_policy((CreateMaskingPolicyStmt *) parse_tree);
#endif
            break;

        case T_AlterMaskingPolicyStmt:
#ifdef PGXC
           if (IS_PGXC_COORDINATOR)
           {
               char *FirstExecNode = find_first_exec_cn();
               bool isFirstNode = (strcmp(FirstExecNode, g_instance.attr.attr_common.PGXCNodeName) == 0);

			   if(u_sess->attr.attr_sql.enable_parallel_ddl && !isFirstNode)
               {
                   ExecUtilityStmtOnNodes_ParallelDDLMode(query_string, NULL, sent_to_remote, false, EXEC_ON_COORDS, false, FirstExecNode);
                   alter_masking_policy((AlterMaskingPolicyStmt *) parse_tree);
                   ExecUtilityStmtOnNodes_ParallelDDLMode(query_string, NULL, sent_to_remote, false, EXEC_ON_DATANODES, false, FirstExecNode);
               } else {
                   alter_masking_policy((AlterMaskingPolicyStmt *) parse_tree);
                   ExecUtilityStmtOnNodes(query_string, NULL, sent_to_remote, false, EXEC_ON_ALL_NODES, false);
               }
           } else {
               alter_masking_policy((AlterMaskingPolicyStmt *) parse_tree);
           }
#else
           alter_masking_policy((AlterMaskingPolicyStmt *) parse_tree);
#endif

            break;

        case T_DropMaskingPolicyStmt:
#ifdef PGXC
           if (IS_PGXC_COORDINATOR)
           {
               char *FirstExecNode = find_first_exec_cn();
               bool isFirstNode = (strcmp(FirstExecNode, g_instance.attr.attr_common.PGXCNodeName) == 0);

			   if(u_sess->attr.attr_sql.enable_parallel_ddl && !isFirstNode)
               {
                   ExecUtilityStmtOnNodes_ParallelDDLMode(query_string, NULL, sent_to_remote, false, EXEC_ON_COORDS, false, FirstExecNode);
                   drop_masking_policy((DropMaskingPolicyStmt *) parse_tree);
                   ExecUtilityStmtOnNodes_ParallelDDLMode(query_string, NULL, sent_to_remote, false, EXEC_ON_DATANODES, false, FirstExecNode);
               } else {
                   drop_masking_policy((DropMaskingPolicyStmt *) parse_tree);
                   ExecUtilityStmtOnNodes(query_string, NULL, sent_to_remote, false, EXEC_ON_ALL_NODES, false);
               }
           } else {
               drop_masking_policy((DropMaskingPolicyStmt *) parse_tree);
           }
#else
           drop_masking_policy((DropMaskingPolicyStmt *) parse_tree);
#endif
            break;

        case T_CreateSynonymStmt:
#ifdef PGXC
            if (IS_PGXC_COORDINATOR) {
                char* first_exec_node = find_first_exec_cn();
                bool is_first_node = (strcmp(first_exec_node, g_instance.attr.attr_common.PGXCNodeName) == 0);

                if (u_sess->attr.attr_sql.enable_parallel_ddl && !is_first_node) {
                    ExecUtilityStmtOnNodes_ParallelDDLMode(
                        query_string, NULL, sent_to_remote, false, EXEC_ON_COORDS, false, first_exec_node);
                    CreateSynonym((CreateSynonymStmt*)parse_tree);
                    ExecUtilityStmtOnNodes_ParallelDDLMode(
                        query_string, NULL, sent_to_remote, false, EXEC_ON_DATANODES, false, first_exec_node);
                } else {
                    CreateSynonym((CreateSynonymStmt*)parse_tree);
                    ExecUtilityStmtOnNodes(query_string, NULL, sent_to_remote, false, EXEC_ON_ALL_NODES, false);
                }
            } else {
                CreateSynonym((CreateSynonymStmt*)parse_tree);
            }
#else
            CreateSynonym((CreateSynonymStmt*)parse_tree);
#endif
            break;

        case T_DropSynonymStmt:
#ifdef PGXC
            if (IS_PGXC_COORDINATOR) {
                char* first_exec_node = find_first_exec_cn();
                bool is_first_node = (strcmp(first_exec_node, g_instance.attr.attr_common.PGXCNodeName) == 0);

                if (u_sess->attr.attr_sql.enable_parallel_ddl && !is_first_node) {
                    ExecUtilityStmtOnNodes_ParallelDDLMode(
                        query_string, NULL, sent_to_remote, false, EXEC_ON_COORDS, false, first_exec_node);
                    DropSynonym((DropSynonymStmt*)parse_tree);
                    ExecUtilityStmtOnNodes_ParallelDDLMode(
                        query_string, NULL, sent_to_remote, false, EXEC_ON_DATANODES, false, first_exec_node);
                } else {
                    DropSynonym((DropSynonymStmt*)parse_tree);
                    ExecUtilityStmtOnNodes(query_string, NULL, sent_to_remote, false, EXEC_ON_ALL_NODES, false);
                }
            } else {
                DropSynonym((DropSynonymStmt*)parse_tree);
            }
#else
            DropSynonym((DropSynonymStmt*)parse_tree);
#endif
            break;

        case T_CreateResourcePoolStmt:
            if (IS_PGXC_COORDINATOR && !IsConnFromCoord()) {
                char* first_exec_node = find_first_exec_cn();
                bool is_first_node = (strcmp(first_exec_node, g_instance.attr.attr_common.PGXCNodeName) == 0);

                if (u_sess->attr.attr_sql.enable_parallel_ddl && !is_first_node) {
                    if (in_logic_cluster() && get_current_lcgroup_name()) {
                        char* create_respool_stmt = NULL;
                        create_respool_stmt = GenerateResourcePoolStmt((CreateResourcePoolStmt*)parse_tree, query_string);
                        Assert(create_respool_stmt != NULL);

                        ExecUtilityStmtOnNodes_ParallelDDLMode(
                            create_respool_stmt, NULL, sent_to_remote, false, EXEC_ON_COORDS, false, first_exec_node);

                        pfree_ext(create_respool_stmt);
                    } else {
                        ExecUtilityStmtOnNodes_ParallelDDLMode(
                            query_string, NULL, sent_to_remote, false, EXEC_ON_COORDS, false, first_exec_node);
                    }

                    CreateResourcePool((CreateResourcePoolStmt*)parse_tree);

                    if (in_logic_cluster() && get_current_lcgroup_name()) {
                        char* create_respool_stmt = NULL;
                        create_respool_stmt = GenerateResourcePoolStmt((CreateResourcePoolStmt*)parse_tree, query_string);
                        Assert(create_respool_stmt != NULL);

                        ExecUtilityStmtOnNodes_ParallelDDLMode(
                            create_respool_stmt, NULL, sent_to_remote, false, EXEC_ON_DATANODES, false, first_exec_node);

                        pfree_ext(create_respool_stmt);
                    } else {
                        ExecUtilityStmtOnNodes_ParallelDDLMode(
                            query_string, NULL, sent_to_remote, false, EXEC_ON_DATANODES, false, first_exec_node);
                    }
                } else {
                    CreateResourcePool((CreateResourcePoolStmt*)parse_tree);

                    if (in_logic_cluster() && get_current_lcgroup_name()) {
                        char* create_respool_stmt = NULL;
                        create_respool_stmt = GenerateResourcePoolStmt((CreateResourcePoolStmt*)parse_tree, query_string);
                        Assert(create_respool_stmt != NULL);

                        ExecUtilityStmtOnNodes(
                            create_respool_stmt, NULL, sent_to_remote, false, EXEC_ON_ALL_NODES, false);

                        pfree_ext(create_respool_stmt);
                    } else {
                        ExecUtilityStmtOnNodes(query_string, NULL, sent_to_remote, false, EXEC_ON_ALL_NODES, false);
                    }
                }
            } else {
                CreateResourcePool((CreateResourcePoolStmt*)parse_tree);
            }
            break;

        case T_AlterResourcePoolStmt:
            if (IS_PGXC_COORDINATOR) {
                char* first_exec_node = find_first_exec_cn();
                bool is_first_node = (strcmp(first_exec_node, g_instance.attr.attr_common.PGXCNodeName) == 0);

                if (u_sess->attr.attr_sql.enable_parallel_ddl && !is_first_node) {
                    ExecUtilityStmtOnNodes_ParallelDDLMode(
                        query_string, NULL, sent_to_remote, false, EXEC_ON_COORDS, false, first_exec_node);
                    AlterResourcePool((AlterResourcePoolStmt*)parse_tree);
                    ExecUtilityStmtOnNodes_ParallelDDLMode(
                        query_string, NULL, sent_to_remote, false, EXEC_ON_DATANODES, false, first_exec_node);
                } else {
                    AlterResourcePool((AlterResourcePoolStmt*)parse_tree);
                    ExecUtilityStmtOnNodes(query_string, NULL, sent_to_remote, false, EXEC_ON_ALL_NODES, false);
                }
            } else {
                AlterResourcePool((AlterResourcePoolStmt*)parse_tree);
            }
            break;

        case T_DropResourcePoolStmt:
            if (IS_PGXC_COORDINATOR) {
                char* first_exec_node = find_first_exec_cn();
                bool is_first_node = (strcmp(first_exec_node, g_instance.attr.attr_common.PGXCNodeName) == 0);

                if (u_sess->attr.attr_sql.enable_parallel_ddl && !is_first_node) {
                    ExecUtilityStmtOnNodes_ParallelDDLMode(
                        query_string, NULL, sent_to_remote, false, EXEC_ON_COORDS, false, first_exec_node);
                    DropResourcePool((DropResourcePoolStmt*)parse_tree);
                    ExecUtilityStmtOnNodes_ParallelDDLMode(
                        query_string, NULL, sent_to_remote, false, EXEC_ON_DATANODES, false, first_exec_node);
                } else {
                    DropResourcePool((DropResourcePoolStmt*)parse_tree);
                    ExecUtilityStmtOnNodes(query_string, NULL, sent_to_remote, false, EXEC_ON_ALL_NODES, false);
                }
            } else {
                DropResourcePool((DropResourcePoolStmt*)parse_tree);
            }
            break;

        case T_AlterGlobalConfigStmt:
#ifdef ENABLE_MULTIPLE_NODES
            if (IS_PGXC_COORDINATOR) {
                char* first_exec_node = find_first_exec_cn();
                bool is_first_node = (strcmp(first_exec_node, g_instance.attr.attr_common.PGXCNodeName) == 0);
                if (u_sess->attr.attr_sql.enable_parallel_ddl && !is_first_node) {
                    ExecUtilityStmtOnNodes_ParallelDDLMode(
                        query_string, NULL, sent_to_remote, false, EXEC_ON_COORDS, false, first_exec_node);
                    AlterGlobalConfig((AlterGlobalConfigStmt*)parse_tree);
                    ExecUtilityStmtOnNodes_ParallelDDLMode(
                        query_string, NULL, sent_to_remote, false, EXEC_ON_DATANODES, false, first_exec_node);
                } else {
                    AlterGlobalConfig((AlterGlobalConfigStmt*)parse_tree);
                    ExecUtilityStmtOnNodes(query_string, NULL, sent_to_remote, false, EXEC_ON_ALL_NODES, false);
                }
            } else {
                AlterGlobalConfig((AlterGlobalConfigStmt*)parse_tree);
            }
#else
            AlterGlobalConfig((AlterGlobalConfigStmt*)parse_tree);
#endif
            break;

        case T_DropGlobalConfigStmt:
#ifdef ENABLE_MULTIPLE_NODES
            if (IS_PGXC_COORDINATOR) {
                char* first_exec_node = find_first_exec_cn();
                bool is_first_node = (strcmp(first_exec_node, g_instance.attr.attr_common.PGXCNodeName) == 0);
                if (u_sess->attr.attr_sql.enable_parallel_ddl && !is_first_node) {
                    ExecUtilityStmtOnNodes_ParallelDDLMode(
                        query_string, NULL, sent_to_remote, false, EXEC_ON_COORDS, false, first_exec_node);
                    DropGlobalConfig((DropGlobalConfigStmt*)parse_tree);
                    ExecUtilityStmtOnNodes_ParallelDDLMode(
                        query_string, NULL, sent_to_remote, false, EXEC_ON_DATANODES, false, first_exec_node);
                } else {
                    DropGlobalConfig((DropGlobalConfigStmt*)parse_tree);
                    ExecUtilityStmtOnNodes(query_string, NULL, sent_to_remote, false, EXEC_ON_ALL_NODES, false);
                }
            } else {
                DropGlobalConfig((DropGlobalConfigStmt*)parse_tree);
            }
#else
            DropGlobalConfig((DropGlobalConfigStmt*)parse_tree);
#endif
            break;

        case T_CreateWorkloadGroupStmt:
#ifndef ENABLE_MULTIPLE_NODES
            DISTRIBUTED_FEATURE_NOT_SUPPORTED();
#endif
            if (!IsConnFromCoord())
                PreventTransactionChain(is_top_level, "CREATE WORKLOAD GROUP");
            if (IS_PGXC_COORDINATOR) {
                char* first_exec_node = find_first_exec_cn();
                bool is_first_node = (strcmp(first_exec_node, g_instance.attr.attr_common.PGXCNodeName) == 0);

                if (u_sess->attr.attr_sql.enable_parallel_ddl && !is_first_node) {
                    ExecUtilityStmtOnNodes_ParallelDDLMode(
                        query_string, NULL, sent_to_remote, false, EXEC_ON_COORDS, false, first_exec_node);
                    CreateWorkloadGroup((CreateWorkloadGroupStmt*)parse_tree);
                    ExecUtilityStmtOnNodes_ParallelDDLMode(
                        query_string, NULL, sent_to_remote, false, EXEC_ON_DATANODES, false, first_exec_node);
                } else {
                    CreateWorkloadGroup((CreateWorkloadGroupStmt*)parse_tree);
                    ExecUtilityStmtOnNodes(query_string, NULL, sent_to_remote, false, EXEC_ON_ALL_NODES, false);
                }
            } else {
                CreateWorkloadGroup((CreateWorkloadGroupStmt*)parse_tree);
            }
            break;
        case T_AlterWorkloadGroupStmt:
#ifndef ENABLE_MULTIPLE_NODES
            DISTRIBUTED_FEATURE_NOT_SUPPORTED();
#endif
            if (!IsConnFromCoord())
                PreventTransactionChain(is_top_level, "ALTER WORKLOAD GROUP");
            if (IS_PGXC_COORDINATOR) {
                char* first_exec_node = find_first_exec_cn();
                bool is_first_node = (strcmp(first_exec_node, g_instance.attr.attr_common.PGXCNodeName) == 0);
                if (u_sess->attr.attr_sql.enable_parallel_ddl && !is_first_node) {
                    ExecUtilityStmtOnNodes_ParallelDDLMode(
                        query_string, NULL, sent_to_remote, false, EXEC_ON_COORDS, false, first_exec_node);
                    AlterWorkloadGroup((AlterWorkloadGroupStmt*)parse_tree);
                    ExecUtilityStmtOnNodes_ParallelDDLMode(
                        query_string, NULL, sent_to_remote, false, EXEC_ON_DATANODES, false, first_exec_node);
                } else {
                    AlterWorkloadGroup((AlterWorkloadGroupStmt*)parse_tree);
                    ExecUtilityStmtOnNodes(query_string, NULL, sent_to_remote, false, EXEC_ON_ALL_NODES, false);
                }
            } else {
                AlterWorkloadGroup((AlterWorkloadGroupStmt*)parse_tree);
            }
            break;
        case T_DropWorkloadGroupStmt:
#ifndef ENABLE_MULTIPLE_NODES
            DISTRIBUTED_FEATURE_NOT_SUPPORTED();
#endif
            if (!IsConnFromCoord())
                PreventTransactionChain(is_top_level, "DROP WORKLOAD GROUP");
            if (IS_PGXC_COORDINATOR) {
                char* first_exec_node = find_first_exec_cn();
                bool is_first_node = (strcmp(first_exec_node, g_instance.attr.attr_common.PGXCNodeName) == 0);
                if (u_sess->attr.attr_sql.enable_parallel_ddl && !is_first_node) {
                    ExecUtilityStmtOnNodes_ParallelDDLMode(
                        query_string, NULL, sent_to_remote, false, EXEC_ON_COORDS, false, first_exec_node);
                    DropWorkloadGroup((DropWorkloadGroupStmt*)parse_tree);
                    ExecUtilityStmtOnNodes_ParallelDDLMode(
                        query_string, NULL, sent_to_remote, false, EXEC_ON_DATANODES, false, first_exec_node);
                } else {
                    DropWorkloadGroup((DropWorkloadGroupStmt*)parse_tree);

                    ExecUtilityStmtOnNodes(query_string, NULL, sent_to_remote, false, EXEC_ON_ALL_NODES, false);
                }
            } else {
                DropWorkloadGroup((DropWorkloadGroupStmt*)parse_tree);
            }
            break;

        case T_CreateAppWorkloadGroupMappingStmt:
#ifndef ENABLE_MULTIPLE_NODES
            DISTRIBUTED_FEATURE_NOT_SUPPORTED();
#endif
            if (!IsConnFromCoord())
                PreventTransactionChain(is_top_level, "CREATE APP WORKLOAD GROUP MAPPING");
            if (IS_PGXC_COORDINATOR) {
                char* first_exec_node = find_first_exec_cn();
                bool is_first_node = (strcmp(first_exec_node, g_instance.attr.attr_common.PGXCNodeName) == 0);

                if (u_sess->attr.attr_sql.enable_parallel_ddl && !is_first_node) {
                    ExecUtilityStmtOnNodes_ParallelDDLMode(
                        query_string, NULL, sent_to_remote, false, EXEC_ON_COORDS, false, first_exec_node);
                    CreateAppWorkloadGroupMapping((CreateAppWorkloadGroupMappingStmt*)parse_tree);
                    ExecUtilityStmtOnNodes_ParallelDDLMode(
                        query_string, NULL, sent_to_remote, false, EXEC_ON_DATANODES, false, first_exec_node);
                } else {
                    CreateAppWorkloadGroupMapping((CreateAppWorkloadGroupMappingStmt*)parse_tree);
                    ExecUtilityStmtOnNodes(query_string, NULL, sent_to_remote, false, EXEC_ON_ALL_NODES, false);
                }
            } else {
                CreateAppWorkloadGroupMapping((CreateAppWorkloadGroupMappingStmt*)parse_tree);
            }
            break;

        case T_AlterAppWorkloadGroupMappingStmt:
#ifndef ENABLE_MULTIPLE_NODES
            DISTRIBUTED_FEATURE_NOT_SUPPORTED();
#endif
            if (!IsConnFromCoord())
                PreventTransactionChain(is_top_level, "ALTER APP WORKLOAD GROUP MAPPING");
            if (IS_PGXC_COORDINATOR) {
                char* first_exec_node = find_first_exec_cn();
                bool is_first_node = (strcmp(first_exec_node, g_instance.attr.attr_common.PGXCNodeName) == 0);
                if (u_sess->attr.attr_sql.enable_parallel_ddl && !is_first_node) {
                    ExecUtilityStmtOnNodes_ParallelDDLMode(
                        query_string, NULL, sent_to_remote, false, EXEC_ON_COORDS, false, first_exec_node);
                    AlterAppWorkloadGroupMapping((AlterAppWorkloadGroupMappingStmt*)parse_tree);
                    ExecUtilityStmtOnNodes_ParallelDDLMode(
                        query_string, NULL, sent_to_remote, false, EXEC_ON_DATANODES, false, first_exec_node);
                } else {
                    AlterAppWorkloadGroupMapping((AlterAppWorkloadGroupMappingStmt*)parse_tree);
                    ExecUtilityStmtOnNodes(query_string, NULL, sent_to_remote, false, EXEC_ON_ALL_NODES, false);
                }
            } else {
                AlterAppWorkloadGroupMapping((AlterAppWorkloadGroupMappingStmt*)parse_tree);
            }
            break;

        case T_DropAppWorkloadGroupMappingStmt:
#ifndef ENABLE_MULTIPLE_NODES
            DISTRIBUTED_FEATURE_NOT_SUPPORTED();
#endif
            if (!IsConnFromCoord())
                PreventTransactionChain(is_top_level, "DROP APP WORKLOAD GROUP MAPPING");
            if (IS_PGXC_COORDINATOR) {
                char* first_exec_node = find_first_exec_cn();
                bool is_first_node = (strcmp(first_exec_node, g_instance.attr.attr_common.PGXCNodeName) == 0);

                if (u_sess->attr.attr_sql.enable_parallel_ddl && !is_first_node) {
                    ExecUtilityStmtOnNodes_ParallelDDLMode(
                        query_string, NULL, sent_to_remote, false, EXEC_ON_COORDS, false, first_exec_node);
                    DropAppWorkloadGroupMapping((DropAppWorkloadGroupMappingStmt*)parse_tree);
                    ExecUtilityStmtOnNodes_ParallelDDLMode(
                        query_string, NULL, sent_to_remote, false, EXEC_ON_DATANODES, false, first_exec_node);
                } else {
                    DropAppWorkloadGroupMapping((DropAppWorkloadGroupMappingStmt*)parse_tree);
                    ExecUtilityStmtOnNodes(query_string, NULL, sent_to_remote, false, EXEC_ON_ALL_NODES, false);
                }
            } else {
                DropAppWorkloadGroupMapping((DropAppWorkloadGroupMappingStmt*)parse_tree);
            }
            break;
#endif

        case T_ReindexStmt: {
            ReindexStmt* stmt = (ReindexStmt*)parse_tree;
            RemoteQueryExecType exec_type;
            bool is_temp = false;
            pgstat_set_io_state(IOSTATE_WRITE);
#ifdef PGXC
            if (IS_PGXC_COORDINATOR) {
                char* first_exec_node = find_first_exec_cn();
                bool is_first_node = (strcmp(first_exec_node, g_instance.attr.attr_common.PGXCNodeName) == 0);

                if (stmt->relation) {
                    Oid rel_id = InvalidOid;
                    rel_id = RangeVarGetRelid(stmt->relation, AccessShareLock, false);
                    if (OidIsValid(rel_id)) {
                        exec_type = ExecUtilityFindNodes(stmt->kind, rel_id, &is_temp);
                        UnlockRelationOid(rel_id, AccessShareLock);
                    } else
                        exec_type = EXEC_ON_NONE;
                } else
                    exec_type = EXEC_ON_ALL_NODES;

                query_string = ConstructMesageWithMemInfo(query_string, stmt->memUsage);
                /*
                 * If I am the main execute CN but not CCN,
                 * Notify the CCN to create firstly, and then notify other CNs except me.
                 */
                if (u_sess->attr.attr_sql.enable_parallel_ddl && !is_first_node &&
                    (exec_type == EXEC_ON_ALL_NODES || exec_type == EXEC_ON_COORDS)) {
                    ExecUtilityStmtOnNodes_ParallelDDLMode(query_string,
                        NULL,
                        sent_to_remote,
                        stmt->kind == OBJECT_DATABASE,
                        EXEC_ON_COORDS,
                        false,
                        first_exec_node,
                        (Node*)stmt);
                }

                ReindexCommand(stmt, is_top_level);

                if (u_sess->attr.attr_sql.enable_parallel_ddl && !is_first_node &&
                    (exec_type == EXEC_ON_ALL_NODES || exec_type == EXEC_ON_DATANODES)) {
                    ExecUtilityStmtOnNodes_ParallelDDLMode(query_string,
                        NULL,
                        sent_to_remote,
                        stmt->kind == OBJECT_DATABASE,
                        EXEC_ON_DATANODES,
                        false,
                        first_exec_node,
                        (Node*)stmt);
                } else {
                    ExecUtilityStmtOnNodes(
                        query_string, NULL, sent_to_remote, stmt->kind == OBJECT_DATABASE, exec_type, false, (Node*)stmt);
                }
            } else {
                ReindexCommand(stmt, is_top_level);
            }
#else
        ReindexCommand(stmt, is_top_level);
#endif
        } break;

        case T_CreateConversionStmt:
#ifdef PGXC
            if (!IsInitdb)
                ereport(ERROR,
                    (errcode(ERRCODE_FEATURE_NOT_SUPPORTED), errmsg("user defined conversion is not yet supported.")));
#endif /* PGXC */
            CreateConversionCommand((CreateConversionStmt*)parse_tree);
#ifdef PGXC
            if (IS_PGXC_COORDINATOR)
                ExecUtilityStmtOnNodes(query_string, NULL, sent_to_remote, false, EXEC_ON_ALL_NODES, false);
#endif
            break;

        case T_CreateCastStmt:
#ifdef ENABLE_MULTIPLE_NODES
            if (!IsInitdb && !u_sess->exec_cxt.extension_is_valid && !u_sess->attr.attr_common.IsInplaceUpgrade)
                ereport(
                    ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED), errmsg("user defined cast is not yet supported.")));
#endif /* ENABLE_MULTIPLE_NODES */
            CreateCast((CreateCastStmt*)parse_tree);
#ifdef PGXC
            if (IS_PGXC_COORDINATOR)
                ExecUtilityStmtOnNodes(query_string, NULL, sent_to_remote, false, EXEC_ON_ALL_NODES, false);
#endif
            break;

        case T_CreateOpClassStmt:
#ifdef ENABLE_MULTIPLE_NODES
            if (!u_sess->attr.attr_common.IsInplaceUpgrade && !u_sess->exec_cxt.extension_is_valid)
                ereport(ERROR,
                    (errcode(ERRCODE_FEATURE_NOT_SUPPORTED), errmsg("user defined operator is not yet supported.")));
#endif /* ENABLE_MULTIPLE_NODES */
            DefineOpClass((CreateOpClassStmt*)parse_tree);
#ifdef PGXC
            if (IS_PGXC_COORDINATOR)
                ExecUtilityStmtOnNodes(query_string, NULL, sent_to_remote, false, EXEC_ON_ALL_NODES, false);
#endif
            break;

        case T_CreateOpFamilyStmt:
#ifdef PGXC
            if (!u_sess->attr.attr_common.IsInplaceUpgrade)
                ereport(ERROR,
                    (errcode(ERRCODE_FEATURE_NOT_SUPPORTED), errmsg("user defined operator is not yet supported.")));
#endif /* PGXC */
            DefineOpFamily((CreateOpFamilyStmt*)parse_tree);
#ifdef PGXC
            if (IS_PGXC_COORDINATOR)
                ExecUtilityStmtOnNodes(query_string, NULL, sent_to_remote, false, EXEC_ON_ALL_NODES, false);
#endif
            break;

        case T_AlterOpFamilyStmt:
#ifdef PGXC
            if (!u_sess->attr.attr_common.IsInplaceUpgrade)
                ereport(ERROR,
                    (errcode(ERRCODE_FEATURE_NOT_SUPPORTED), errmsg("user defined operator is not yet supported.")));
#endif /* PGXC */
            AlterOpFamily((AlterOpFamilyStmt*)parse_tree);
#ifdef PGXC
            if (IS_PGXC_COORDINATOR)
                ExecUtilityStmtOnNodes(query_string, NULL, sent_to_remote, false, EXEC_ON_ALL_NODES, false);
#endif
            break;

        case T_AlterTSDictionaryStmt:
            ts_check_feature_disable();
            AlterTSDictionary((AlterTSDictionaryStmt*)parse_tree);
#ifdef PGXC
            if (IS_PGXC_COORDINATOR)
                ExecUtilityStmtOnNodes(query_string, NULL, sent_to_remote, false, EXEC_ON_ALL_NODES, false);
#endif
            break;

        case T_AlterTSConfigurationStmt:
            ts_check_feature_disable();
            AlterTSConfiguration((AlterTSConfigurationStmt*)parse_tree);
#ifdef PGXC
            if (IS_PGXC_COORDINATOR)
                ExecUtilityStmtOnNodes(query_string, NULL, sent_to_remote, false, EXEC_ON_ALL_NODES, false);
#endif
            break;
#ifdef PGXC
        case T_RemoteQuery:
            AssertEreport(IS_PGXC_COORDINATOR, MOD_EXECUTOR, "not a coordinator node");
            /*
             * Do not launch query on Other Datanodes if remote connection is a Coordinator one
             * it will cause a deadlock in the cluster at Datanode levels.
             */
#ifdef ENABLE_MULTIPLE_NODES
            if (!IsConnFromCoord())
                ExecRemoteUtility((RemoteQuery*)parse_tree);
#endif
            break;

        case T_CleanConnStmt:
            CleanConnection((CleanConnStmt*)parse_tree);

            if (IS_PGXC_COORDINATOR)
                ExecUtilityStmtOnNodes(query_string, NULL, sent_to_remote, true, EXEC_ON_COORDS, false);
            break;
#endif
        case T_CreateDirectoryStmt:
            if (IS_PGXC_COORDINATOR) {
                char* first_exec_node = find_first_exec_cn();
                bool is_first_node = (strcmp(first_exec_node, g_instance.attr.attr_common.PGXCNodeName) == 0);
                if (u_sess->attr.attr_sql.enable_parallel_ddl && !is_first_node) {
                    ExecUtilityStmtOnNodes_ParallelDDLMode(
                        query_string, NULL, sent_to_remote, false, EXEC_ON_COORDS, false, first_exec_node);
                    CreatePgDirectory((CreateDirectoryStmt*)parse_tree);
                    ExecUtilityStmtOnNodes_ParallelDDLMode(
                        query_string, NULL, sent_to_remote, false, EXEC_ON_DATANODES, false, first_exec_node);
                } else {
                    CreatePgDirectory((CreateDirectoryStmt*)parse_tree);
                    ExecUtilityStmtOnNodes(query_string, NULL, sent_to_remote, false, EXEC_ON_ALL_NODES, false);
                }
            } else
                CreatePgDirectory((CreateDirectoryStmt*)parse_tree);
            break;

        case T_DropDirectoryStmt:
            if (IS_PGXC_COORDINATOR) {
                char* first_exec_node = find_first_exec_cn();
                bool is_first_node = (strcmp(first_exec_node, g_instance.attr.attr_common.PGXCNodeName) == 0);
                if (u_sess->attr.attr_sql.enable_parallel_ddl && !is_first_node) {
                    ExecUtilityStmtOnNodes_ParallelDDLMode(
                        query_string, NULL, sent_to_remote, false, EXEC_ON_COORDS, false, first_exec_node);
                    DropPgDirectory((DropDirectoryStmt*)parse_tree);
                    ExecUtilityStmtOnNodes_ParallelDDLMode(
                        query_string, NULL, sent_to_remote, false, EXEC_ON_DATANODES, false, first_exec_node);
                } else {
                    DropPgDirectory((DropDirectoryStmt*)parse_tree);
                    ExecUtilityStmtOnNodes(query_string, NULL, sent_to_remote, false, EXEC_ON_ALL_NODES, false);
                }
            } else
                DropPgDirectory((DropDirectoryStmt*)parse_tree);
            break;
        /* Client Logic */
        case T_CreateClientLogicGlobal: 
#ifdef ENABLE_MULTIPLE_NODES
            if (IS_MAIN_COORDINATOR && !u_sess->attr.attr_common.enable_full_encryption) {
#else
            if (!u_sess->attr.attr_common.enable_full_encryption) {
#endif
                ereport(ERROR,
                    (errcode(ERRCODE_OPERATE_NOT_SUPPORTED),
                        errmsg("Un-support to create client master key when client encryption is disabled.")));
            }
            if (IS_PGXC_COORDINATOR) {
                char *FirstExecNode = find_first_exec_cn();
                bool isFirstNode = (strcmp(FirstExecNode, g_instance.attr.attr_common.PGXCNodeName) == 0);

                if (u_sess->attr.attr_sql.enable_parallel_ddl && !isFirstNode) {
                    ExecUtilityStmtOnNodes_ParallelDDLMode(query_string, NULL, sent_to_remote, false, EXEC_ON_COORDS,
                        false, FirstExecNode);
                    (void)process_global_settings((CreateClientLogicGlobal *)parse_tree);
                    ExecUtilityStmtOnNodes_ParallelDDLMode(query_string, NULL, sent_to_remote, false, EXEC_ON_DATANODES,
                        false, FirstExecNode);
                } else {
                    (void)process_global_settings((CreateClientLogicGlobal *)parse_tree);
                    ExecUtilityStmtOnNodes(query_string, NULL, sent_to_remote, false, EXEC_ON_ALL_NODES, false);
                }
            } else {
                (void)process_global_settings((CreateClientLogicGlobal *)parse_tree);
            }
            break;
        case T_CreateClientLogicColumn:
#ifdef ENABLE_MULTIPLE_NODES
            if (IS_MAIN_COORDINATOR && !u_sess->attr.attr_common.enable_full_encryption) {
#else
            if (!u_sess->attr.attr_common.enable_full_encryption) {
#endif
                ereport(ERROR,
                    (errcode(ERRCODE_OPERATE_NOT_SUPPORTED),
                        errmsg("Un-support to create column encryption key when client encryption is disabled.")));
            }
            if (IS_PGXC_COORDINATOR) {
                char *FirstExecNode = find_first_exec_cn();
                bool isFirstNode = (strcmp(FirstExecNode, g_instance.attr.attr_common.PGXCNodeName) == 0);

                if (u_sess->attr.attr_sql.enable_parallel_ddl && !isFirstNode) {
                    ExecUtilityStmtOnNodes_ParallelDDLMode(query_string, NULL, sent_to_remote, false, EXEC_ON_COORDS,
                        false, FirstExecNode);
                    (void)process_column_settings((CreateClientLogicColumn *)parse_tree);
                    ExecUtilityStmtOnNodes_ParallelDDLMode(query_string, NULL, sent_to_remote, false, EXEC_ON_DATANODES,
                        false, FirstExecNode);
                } else {
                    (void)process_column_settings((CreateClientLogicColumn *)parse_tree);
                    ExecUtilityStmtOnNodes(query_string, NULL, sent_to_remote, false, EXEC_ON_ALL_NODES, false);
                }
            } else {
                (void)process_column_settings((CreateClientLogicColumn *)parse_tree);
            }
            break;

        case T_CreateModelStmt:{ // DB4AI

            exec_create_model((CreateModelStmt*) parse_tree, query_string, params, completion_tag);

            break;
        }

        case T_CreatePublicationStmt:
#if defined(ENABLE_MULTIPLE_NODES) || defined(ENABLE_LITE_MODE)
            ereport(ERROR,
                (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                    errmsg("openGauss does not support PUBLICATION yet"),
                    errdetail("The feature is not currently supported")));
#endif
            CreatePublication((CreatePublicationStmt *) parse_tree);
            break;
        case T_AlterPublicationStmt:
#if defined(ENABLE_MULTIPLE_NODES) || defined(ENABLE_LITE_MODE)
            ereport(ERROR,
                (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                    errmsg("openGauss does not support PUBLICATION yet"),
                    errdetail("The feature is not currently supported")));
#endif
            AlterPublication((AlterPublicationStmt *) parse_tree);
            break;
        case T_CreateSubscriptionStmt:
#if defined(ENABLE_MULTIPLE_NODES) || defined(ENABLE_LITE_MODE)
            ereport(ERROR,
                (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                    errmsg("openGauss does not support SUBSCRIPTION yet"),
                    errdetail("The feature is not currently supported")));
#endif
            CreateSubscription((CreateSubscriptionStmt *) parse_tree, is_top_level);
            break;
        case T_AlterSubscriptionStmt:
#if defined(ENABLE_MULTIPLE_NODES) || defined(ENABLE_LITE_MODE)
            ereport(ERROR,
                (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                    errmsg("openGauss does not support SUBSCRIPTION yet"),
                    errdetail("The feature is not currently supported")));
#endif
            AlterSubscription((AlterSubscriptionStmt *) parse_tree);
            break;
        case T_DropSubscriptionStmt:
#if defined(ENABLE_MULTIPLE_NODES) || defined(ENABLE_LITE_MODE)
            ereport(ERROR,
                (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                    errmsg("openGauss does not support SUBSCRIPTION yet"),
                    errdetail("The feature is not currently supported")));
#endif
            DropSubscription((DropSubscriptionStmt *) parse_tree, is_top_level);
            break;
        default: {
            ereport(ERROR,
                (errcode(ERRCODE_UNRECOGNIZED_NODE_TYPE),
                    errmsg("unrecognized node type: %d", (int)nodeTag(parse_tree))));
        } break;
    }
}
