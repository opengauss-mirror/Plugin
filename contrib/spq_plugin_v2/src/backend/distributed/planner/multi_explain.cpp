/*-------------------------------------------------------------------------
 *
 * multi_explain.c
 *	  Citus explain support.
 *
 * Copyright (c) Citus Data, Inc.
 *-------------------------------------------------------------------------
 */

#include "postgres.h"

#include "fmgr.h"
#include "libpq/libpq-fe.h"
#include "miscadmin.h"

#include "access/htup.h"
#include "access/xact.h"
#include "catalog/namespace.h"
#include "catalog/pg_class.h"
#include "catalog/pg_collation.h"
#include "catalog/pg_type.h"
#include "commands/copy.h"
#include "commands/createas.h"
#include "commands/dbcommands.h"
#include "commands/explain.h"
#include "commands/tablecmds.h"
#include "commands/matview.h"
#include "executor/tstoreReceiver.h"
#include "lib/stringinfo.h"
#include "nodes/plannodes.h"
#include "nodes/primnodes.h"
#include "nodes/print.h"
#include "optimizer/clauses.h"
#include "optimizer/cost.h"
#include "optimizer/planner.h"
#include "parser/analyze.h"
#include "portability/instr_time.h"
#include "pgstat.h"
#include "rewrite/rewriteHandler.h"
#include "tcop/dest.h"
#include "tcop/tcopprot.h"
#include "tcop/utility.h"
#include "utils/builtins.h"
#include "utils/json.h"
#include "utils/lsyscache.h"
#include "utils/snapmgr.h"

#include "pg_version_constants.h"

#include "distributed/citus_depended_object.h"
#include "distributed/citus_nodefuncs.h"
#include "distributed/combine_query_planner.h"
#include "distributed/commands/utility_hook.h"
#include "distributed/connection_management.h"
#include "distributed/deparse_shard_query.h"
#include "distributed/distributed_planner.h"
#include "distributed/executor_util.h"
#include "distributed/insert_select_executor.h"
#include "distributed/insert_select_planner.h"
#include "distributed/jsonbutils.h"
#include "distributed/listutils.h"
#include "distributed/merge_planner.h"
#include "distributed/multi_executor.h"
#include "distributed/multi_explain.h"
#include "distributed/multi_logical_optimizer.h"
#include "distributed/multi_logical_planner.h"
#include "distributed/multi_physical_planner.h"
#include "distributed/multi_router_planner.h"
#include "distributed/multi_server_executor.h"
#include "distributed/placement_connection.h"
#include "distributed/recursive_planning.h"
#include "distributed/remote_commands.h"
#include "distributed/tuple_destination.h"
#include "distributed/tuplestore.h"
#include "distributed/session_ctx.h"
#include "distributed/version_compat.h"
#include "distributed/worker_protocol.h"

#include "utils/jsonb.h"

/* struct to save explain flags */
typedef struct {
    bool verbose;
    bool costs;
    bool buffers;
    bool timing;
    ExplainFormat format;
} ExplainOptions;

static THR_LOCAL ExplainOptions CurrentDistributedQueryExplainOptions = {
    0, 0, 0, 0, EXPLAIN_FORMAT_TEXT};

/* Result for a single remote EXPLAIN command */
typedef struct RemoteExplainPlan {
    int placementIndex;
    List* explainOutputList;
} RemoteExplainPlan;

/*
 * ExplainAnalyzeDestination is internal representation of a TupleDestination
 * which collects EXPLAIN ANALYZE output after the main query is run.
 */
typedef struct ExplainAnalyzeDestination {
    TupleDestination pub;
    Task* originalTask;
    TupleDestination* originalTaskDestination;
    TupleDesc lastSavedExplainAnalyzeTupDesc;
} ExplainAnalyzeDestination;

/* Explain functions for distributed queries */
static void ExplainSubPlans(DistributedPlan* distributedPlan, ExplainState* es);
static void ExplainJob(CitusScanState* scanState, Job* job, ExplainState* es,
                       ParamListInfo params);
static void ExplainMapMergeJob(MapMergeJob* mapMergeJob, ExplainState* es);
static void ExplainTaskList(CitusScanState* scanState, List* taskList, ExplainState* es,
                            ParamListInfo params);
static RemoteExplainPlan* RemoteExplain(Task* task, ExplainState* es,
                                        ParamListInfo params);
static RemoteExplainPlan* GetSavedRemoteExplain(Task* task, ExplainState* es);
static RemoteExplainPlan* FetchRemoteExplainFromWorkers(Task* task, ExplainState* es,
                                                        ParamListInfo params);
static void ExplainTask(CitusScanState* scanState, Task* task, int placementIndex,
                        List* explainOutputList, ExplainState* es);
static void ExplainTaskPlacement(ShardPlacement* taskPlacement, List* explainOutputList,
                                 ExplainState* es);
static StringInfo BuildRemoteExplainQuery(char* queryString, ExplainState* es);
static const char* ExplainFormatStr(ExplainFormat format);

static void ExplainWorkerPlan(PlannedStmt* plannedStmt, DestReceiver* dest,
                              ExplainState* es, const char* queryString,
                              ParamListInfo params, QueryEnvironment* queryEnv,
                              const instr_time* planduration,
                              double* executionDurationMillisec);

static ExplainFormat ExtractFieldExplainFormat(Datum jsonbDoc, const char* fieldName,
                                               ExplainFormat defaultValue);

static TupleDestination* CreateExplainAnlyzeDestination(Task* task,
                                                        TupleDestination* taskDest);
static void ExplainAnalyzeDestPutTuple(TupleDestination* self, Task* task,
                                       int placementIndex, int queryNumber,
                                       HeapTuple heapTuple, uint64 tupleLibpqSize);
static TupleDesc ExplainAnalyzeDestTupleDescForQuery(TupleDestination* self,
                                                     int queryNumber);
static char* WrapQueryForExplainAnalyze(const char* queryString, TupleDesc tupleDesc,
                                        ParamListInfo params);
static char* FetchPlanQueryForExplainAnalyze(const char* queryString,
                                             ParamListInfo params);
static char* ParameterResolutionSubquery(ParamListInfo params);
static List* SplitString(const char* str, char delimiter, int maxLength);

/* Static Explain functions copied from explain.c */
static void ExplainOneQuery(Query* query, int cursorOptions, IntoClause* into,
                            ExplainState* es, const char* queryString,
                            ParamListInfo params, QueryEnvironment* queryEnv);

static void ExplainPropertyBytes(const char* qlabel, int64 bytes, ExplainState* es);
static uint64 TaskReceivedTupleData(Task* task);
static bool ShowReceivedTupleData(CitusScanState* scanState, ExplainState* es);

/* exports for SQL callable functions */
PG_FUNCTION_INFO_V1(worker_last_saved_explain_analyze);
PG_FUNCTION_INFO_V1(worker_save_query_explain_analyze);

extern "C" Datum worker_last_saved_explain_analyze(PG_FUNCTION_ARGS);
extern "C" Datum worker_save_query_explain_analyze(PG_FUNCTION_ARGS);

/* OR-able flags for ExplainXMLTag() */
#define X_OPENING 0
#define X_CLOSING 1
#define X_CLOSE_IMMEDIATE 2
#define X_NOWHITESPACE 4

static void ExplainXMLTag(const char* tagname, unsigned int flags, ExplainState* es);
static void ExplainYAMLLineStarting(ExplainState* es);
static void ExplainJSONLineEnding(ExplainState* es);
static void ExplainOpenGroup(const char* objtype, const char* labelname, bool labeled,
                             ExplainState* es);
static void ExplainCloseGroup(const char* objtype, const char* labelname, bool labeled,
                              ExplainState* es);

static void ExplainXMLTag(const char* tagname, unsigned int flags, ExplainState* es)
{
    const char* s = NULL;

    if ((flags & X_NOWHITESPACE) == 0)
        appendStringInfoSpaces(es->str, 2 * es->indent);
    appendStringInfoCharMacro(es->str, '<');
    if ((flags & X_CLOSING) != 0)
        appendStringInfoCharMacro(es->str, '/');
    for (s = tagname; *s; s++)
        appendStringInfoCharMacro(es->str, (*s == ' ') ? '-' : *s);
    if ((flags & X_CLOSE_IMMEDIATE) != 0)
        appendStringInfoString(es->str, " /");
    appendStringInfoCharMacro(es->str, '>');
    if ((flags & X_NOWHITESPACE) == 0)
        appendStringInfoCharMacro(es->str, '\n');
}

/*
 * Indent a YAML line.
 *
 * YAML lines are ordinarily indented by two spaces per indentation level.
 * The text emitted for each property begins just prior to the preceding
 * line-break, except for the first property in an unlabelled group, for which
 * it begins immediately after the "- " that introduces the group.	The first
 * property of the group appears on the same line as the opening "- ".
 */
static void ExplainYAMLLineStarting(ExplainState* es)
{
    AssertEreport(es->format == EXPLAIN_FORMAT_YAML, MOD_EXECUTOR,
                  "unexpect explain state format");
    if (linitial_int(es->grouping_stack) == 0) {
        linitial_int(es->grouping_stack) = 1;
    } else {
        appendStringInfoChar(es->str, '\n');
        appendStringInfoSpaces(es->str, es->indent * 2);
    }
}

/*
 * Emit a JSON line ending.
 *
 * JSON requires a comma after each property but the last.	To facilitate this,
 * in JSON format, the text emitted for each property begins just prior to the
 * preceding line-break (and comma, if applicable).
 */
static void ExplainJSONLineEnding(ExplainState* es)
{
    AssertEreport(es->format == EXPLAIN_FORMAT_JSON, MOD_EXECUTOR,
                  "unexpect explain state format");
    if (linitial_int(es->grouping_stack) != 0)
        appendStringInfoChar(es->str, ',');
    else
        linitial_int(es->grouping_stack) = 1;
    appendStringInfoChar(es->str, '\n');
}

/*
 * Open a group of related objects.
 *
 * objtype is the type of the group object, labelname is its label within
 * a containing object (if any).
 *
 * If labeled is true, the group members will be labeled properties,
 * while if it's false, they'll be unlabeled objects.
 */
static void ExplainOpenGroup(const char* objtype, const char* labelname, bool labeled,
                             ExplainState* es)
{
    switch (es->format) {
        case EXPLAIN_FORMAT_TEXT:
            /* nothing to do */
            break;

        case EXPLAIN_FORMAT_XML:
            ExplainXMLTag(objtype, X_OPENING, es);
            es->indent++;
            break;

        case EXPLAIN_FORMAT_JSON:
            ExplainJSONLineEnding(es);
            appendStringInfoSpaces(es->str, 2 * es->indent);
            if (labelname != NULL) {
                escape_json(es->str, labelname);
                appendStringInfoString(es->str, ": ");
            }
            appendStringInfoChar(es->str, labeled ? '{' : '[');

            /*
             * In JSON format, the grouping_stack is an integer list.  0 means
             * we've emitted nothing at this grouping level, 1 means we've
             * emitted something (and so the next item needs a comma). See
             * ExplainJSONLineEnding().
             */
            es->grouping_stack = lcons_int(0, es->grouping_stack);
            es->indent++;
            break;

        case EXPLAIN_FORMAT_YAML:

            /*
             * In YAML format, the grouping stack is an integer list.  0 means
             * we've emitted nothing at this grouping level AND this grouping
             * level is unlabelled and must be marked with "- ".  See
             * ExplainYAMLLineStarting().
             */
            ExplainYAMLLineStarting(es);
            if (labelname != NULL) {
                appendStringInfo(es->str, "%s: ", labelname);
                es->grouping_stack = lcons_int(1, es->grouping_stack);
            } else {
                appendStringInfoString(es->str, "- ");
                es->grouping_stack = lcons_int(0, es->grouping_stack);
            }
            es->indent++;
            break;
        default:
            break;
    }
}

/*
 * Close a group of related objects.
 * Parameters must match the corresponding ExplainOpenGroup call.
 */
static void ExplainCloseGroup(const char* objtype, const char* labelname, bool labeled,
                              ExplainState* es)
{
    switch (es->format) {
        case EXPLAIN_FORMAT_TEXT:
            /* nothing to do */
            break;

        case EXPLAIN_FORMAT_XML:
            es->indent--;
            ExplainXMLTag(objtype, X_CLOSING, es);
            break;

        case EXPLAIN_FORMAT_JSON:
            es->indent--;
            appendStringInfoChar(es->str, '\n');
            appendStringInfoSpaces(es->str, 2 * es->indent);
            appendStringInfoChar(es->str, labeled ? '}' : ']');
            es->grouping_stack = list_delete_first(es->grouping_stack);
            break;

        case EXPLAIN_FORMAT_YAML:
            es->indent--;
            es->grouping_stack = list_delete_first(es->grouping_stack);
            break;
        default:
            break;
    }
}

/*
 * BufferUsageAccumDiff
 * calculate every element of dst like: dst += add - sub
 */
static void BufferUsageAccumDiff(BufferUsage* dst, const BufferUsage* add,
                                 const BufferUsage* sub)
{
    dst->shared_blks_hit += add->shared_blks_hit - sub->shared_blks_hit;
    dst->shared_blks_read += add->shared_blks_read - sub->shared_blks_read;
    dst->shared_blks_dirtied += add->shared_blks_dirtied - sub->shared_blks_dirtied;
    dst->shared_blks_written += add->shared_blks_written - sub->shared_blks_written;
    dst->local_blks_hit += add->local_blks_hit - sub->local_blks_hit;
    dst->local_blks_read += add->local_blks_read - sub->local_blks_read;
    dst->local_blks_dirtied += add->local_blks_dirtied - sub->local_blks_dirtied;
    dst->local_blks_written += add->local_blks_written - sub->local_blks_written;
    dst->temp_blks_read += add->temp_blks_read - sub->temp_blks_read;
    dst->temp_blks_written += add->temp_blks_written - sub->temp_blks_written;
    INSTR_TIME_ACCUM_DIFF(dst->blk_read_time, add->blk_read_time, sub->blk_read_time);
    INSTR_TIME_ACCUM_DIFF(dst->blk_write_time, add->blk_write_time, sub->blk_write_time);
}

static List* pg_rewrite_query(Query* query)
{
    List* querytree_list = NIL;
    PGSTAT_INIT_TIME_RECORD();

    if (u_sess->attr.attr_sql.Debug_print_parse &&
        u_sess->attr.attr_common.log_min_messages <= LOG)
        elog_node_display(LOG, "parse tree", query,
                          u_sess->attr.attr_sql.Debug_pretty_print);

    if (u_sess->attr.attr_common.log_parser_stats)
        ResetUsage();

    PGSTAT_START_TIME_RECORD();

    if (query->commandType == CMD_UTILITY) {
#ifdef ENABLE_MULTIPLE_NODES
        if (IsA(query->utilityStmt, CreateTableAsStmt)) {
            /*
             * CREATE TABLE AS SELECT and SELECT INTO are rewritten so that the
             * target table is created first. The SELECT query is then transformed
             * into an INSERT INTO statement
             */
            /* handle materilized view. */
            CreateTableAsStmt* stmt = (CreateTableAsStmt*)query->utilityStmt;
            if (IS_PGXC_COORDINATOR && stmt->relkind == OBJECT_MATVIEW) {
                /* Check if OK to create matview */
                check_basetable((Query*)stmt->query, true, stmt->into->ivm);
                /* Check for select privilege */
                (void)ExecCheckRTPerms(query->rtable, true);

                if (stmt->into) {
                    if (stmt->into->ivm) {
                        check_matview_op_supported(stmt);
                        stmt->into->distributeby = infer_incmatview_distkey(stmt);
                    }
                }
            }

            if (stmt->relkind == OBJECT_MATVIEW && IS_PGXC_DATANODE) {
                querytree_list = list_make1(query);
            } else {
                /*
                 * CREATE TABLE AS SELECT and SELECT INTO are rewritten so that the
                 * target table is created first. The SELECT query is then transformed
                 * into an INSERT INTO statement
                 */
                querytree_list = QueryRewriteCTAS(query);
            }
        } else if (IsA(query->utilityStmt, CopyStmt)) {
            querytree_list = query_rewrite_copy(query);
        } else if (IsA(query->utilityStmt, RefreshMatViewStmt)) {
            RefreshMatViewStmt* stmt = (RefreshMatViewStmt*)query->utilityStmt;
            if (!stmt->incremental && !isIncMatView(stmt->relation)) {
                querytree_list = QueryRewriteRefresh(query);
            } else {
                querytree_list = list_make1(query);
            }
        } else if (IsA(query->utilityStmt, AlterTableStmt)) {
            querytree_list = query_rewrite_alter_table(query);
        } else {
            /* don't rewrite utilities, just dump them into result list */
            querytree_list = list_make1(query);
        }
#else
        if (IsA(query->utilityStmt, CreateTableAsStmt)) {
            CreateTableAsStmt* stmt = (CreateTableAsStmt*)query->utilityStmt;
            if (stmt->relkind == OBJECT_MATVIEW) {

                /* Check if OK to create matview */
                check_basetable((Query*)stmt->query, true, stmt->into->ivm);
                /* Check for select privilege */
                (void)ExecCheckRTPerms(query->rtable, true);

                if (stmt->into && stmt->into->ivm) {
                    check_matview_op_supported(stmt);
                }

                querytree_list = list_make1(query);
            } else {
                querytree_list = QueryRewriteCTAS(query);
            }
        } else if (IsA(query->utilityStmt, PrepareStmt)) {
            PrepareStmt* stmt = (PrepareStmt*)query->utilityStmt;
            if (IsA(stmt->query, Const)) {
                if (((Const*)stmt->query)->constisnull) {
                    ereport(ERROR, (errcode(ERRCODE_UNRECOGNIZED_NODE_TYPE),
                                    errmsg("userdefined variable in prepare statement "
                                           "must be text type.")));
                }
            } else {
                querytree_list = list_make1(query);
            }
        } else if (IsA(query->utilityStmt, VariableMultiSetStmt)) {
            querytree_list = query_rewrite_multiset_stmt(query);
        } else if (IsA(query->utilityStmt, VariableSetStmt)) {
            querytree_list = query_rewrite_set_stmt(query);
        } else {
            querytree_list = list_make1(query);
        }
#endif
    } else {
        /* rewrite regular queries */
        querytree_list = QueryRewrite(query);
    }

    /*
     * After rewriting the querytree_list, we need to check every query to ensure they can
     * execute in new expression framework.  If we find a query's is_flt_frame is false,
     * which indicates the query not support the new expression framework, the top level
     * query need be reverted to old. One more things, CMD_UTILITY statements cannot be
     * reverted here.  We solve it at explain_proc_query()
     */
    if (u_sess->attr.attr_common.enable_expr_fusion &&
        u_sess->attr.attr_sql.query_dop_tmp == 1) {
        ListCell* lc = NULL;
        foreach (lc, querytree_list) {
            Query* query_tmp = (Query*)lfirst(lc);
            query_tmp->is_flt_frame = !query_check_no_flt(query_tmp);
        }
    }

    PGSTAT_END_TIME_RECORD(REWRITE_TIME);

    if (u_sess->attr.attr_common.log_parser_stats)
        ShowUsage("REWRITER STATISTICS");

#ifdef COPY_PARSE_PLAN_TREES
    /* Optional debugging check: pass querytree output through copyObject() */
    {
        List* new_list = NIL;

        new_list = (List*)copyObject(querytree_list);
        /* This checks both copyObject() and the equal() routines... */
        if (!equal(new_list, querytree_list))
            ereport(WARNING, (errmsg("copyObject() failed to produce equal parse tree")));
        else
            querytree_list = new_list;
    }
#endif

    if (u_sess->attr.attr_sql.Debug_print_rewritten &&
        u_sess->attr.attr_common.log_min_messages <= LOG)
        elog_node_display(LOG, "rewritten parse tree", querytree_list,
                          u_sess->attr.attr_sql.Debug_pretty_print);

    return querytree_list;
}
/*
 * CitusExplainScan is a custom scan explain callback function which is used to
 * print explain information of a Citus plan which includes both combine query and
 * distributed plan.
 */
void CitusExplainScan(ExtensiblePlanState* node, List* ancestors, struct ExplainState* es)
{
    CitusScanState* scanState = (CitusScanState*)node;
    DistributedPlan* distributedPlan = scanState->distributedPlan;
    EState* executorState = ScanStateGetExecutorState(scanState);
    ParamListInfo params = executorState->es_param_list_info;

    if (!Session_ctx::Vars().ExplainDistributedQueries) {
        appendStringInfoSpaces(es->str, es->indent * 2);
        appendStringInfo(es->str, "explain statements for distributed queries ");
        appendStringInfo(es->str, "are not enabled\n");
        return;
    }

    ExplainOpenGroup("Distributed Query", "Distributed Query", true, es);

    /*
     * ExplainOnePlan function of postgres might be called in this codepath.
     * It requires an ActiveSnapshot being set. Make sure to make ActiveSnapshot available
     * before calling into Citus Explain functions.
     */
    PushActiveSnapshot(executorState->es_snapshot);

    if (distributedPlan->subPlanList != NIL) {
        ExplainSubPlans(distributedPlan, es);
    }

    ExplainJob(scanState, distributedPlan->workerJob, es, params);

    PopActiveSnapshot();

    ExplainCloseGroup("Distributed Query", "Distributed Query", true, es);
}

/*
 * NonPushableInsertSelectExplainScan is a custom scan explain callback function
 * which is used to print explain information of a Citus plan for an INSERT INTO
 * distributed_table SELECT ... query that is evaluated on the coordinator or
 * uses repartitioning.
 */
void NonPushableInsertSelectExplainScan(ExtensiblePlanState* node, List* ancestors,
                                        struct ExplainState* es)
{
    CitusScanState* scanState = (CitusScanState*)node;
    DistributedPlan* distributedPlan = scanState->distributedPlan;
    Query* insertSelectQuery = distributedPlan->modifyQueryViaCoordinatorOrRepartition;
    RangeTblEntry* selectRte = ExtractSelectRangeTableEntry(insertSelectQuery);

    /*
     * Create a copy because ExplainOneQuery can modify the query, and later
     * executions of prepared statements might require it. See
     * https://github.com/citusdata/citus/issues/3947 for what can happen.
     */
    Query* queryCopy = static_cast<Query*>(copyObject(selectRte->subquery));

    bool repartition =
        distributedPlan->modifyWithSelectMethod == MODIFY_WITH_SELECT_REPARTITION;

    if (es->analyze) {
        ereport(ERROR, (errmsg("EXPLAIN ANALYZE is currently not supported for INSERT "
                               "... SELECT commands %s",
                               repartition ? "with repartitioning" : "via coordinator")));
    }

    if (repartition) {
        ExplainPropertyText("INSERT/SELECT method", "repartition", es);
    } else {
        ExplainPropertyText("INSERT/SELECT method", "pull to coordinator", es);
    }

    ExplainOpenGroup("Select Query", "Select Query", false, es);

    /* explain the inner SELECT query */
    IntoClause* into = NULL;
    ParamListInfo params = NULL;

    /*
     * With PG14, we need to provide a string here,
     * for now we put an empty string, which is valid according to postgres.
     */
    char* queryString = pstrdup("");

    ExplainOneQuery(queryCopy, 0, into, es, queryString, params, NULL);

    ExplainCloseGroup("Select Query", "Select Query", false, es);
}

/*
 * NonPushableMergeSqlExplainScan is a custom scan explain callback function
 * which is used to print explain information of a Citus plan for MERGE INTO
 * distributed_table USING (source query/table), where source can be any query
 * whose results are repartitioned to colocated with the target table.
 */
void NonPushableMergeCommandExplainScan(ExtensiblePlanState* node, List* ancestors,
                                        struct ExplainState* es)
{
    CitusScanState* scanState = (CitusScanState*)node;
    DistributedPlan* distributedPlan = scanState->distributedPlan;
    Query* mergeQuery = distributedPlan->modifyQueryViaCoordinatorOrRepartition;
    RangeTblEntry* sourceRte = ExtractMergeSourceRangeTableEntry(mergeQuery, false);

    /*
     * Create a copy because ExplainOneQuery can modify the query, and later
     * executions of prepared statements might require it. See
     * https://github.com/citusdata/citus/issues/3947 for what can happen.
     */
    Query* sourceQueryCopy = static_cast<Query*>(copyObject(sourceRte->subquery));
    bool repartition =
        distributedPlan->modifyWithSelectMethod == MODIFY_WITH_SELECT_REPARTITION;

    if (es->analyze) {
        ereport(ERROR, (errmsg("EXPLAIN ANALYZE is currently not supported for "
                               "MERGE INTO ... commands with repartitioning")));
    }

    Oid targetRelationId = ModifyQueryResultRelationId(mergeQuery);
    StringInfo mergeMethodMessage = makeStringInfo();
    appendStringInfo(mergeMethodMessage, "MERGE INTO %s method",
                     get_rel_name(targetRelationId));

    if (repartition) {
        ExplainPropertyText(mergeMethodMessage->data, "repartition", es);
    } else {
        ExplainPropertyText(mergeMethodMessage->data, "pull to coordinator", es);
    }

    ExplainOpenGroup("Source Query", "Source Query", false, es);

    /* explain the MERGE source query */
    IntoClause* into = NULL;
    ParamListInfo params = NULL;

    /*
     * With PG14, we need to provide a string here, for now we put an empty
     * string, which is valid according to postgres.
     */
    char* queryString = pstrdup("");
    ExplainOneQuery(sourceQueryCopy, 0, into, es, queryString, params, NULL);

    ExplainCloseGroup("Source Query", "Source Query", false, es);
}

/*
 * ExplainSubPlans generates EXPLAIN output for subplans for CTEs
 * and complex subqueries. Because the planning for these queries
 * is done along with the top-level plan, we cannot determine the
 * planning time and set it to 0.
 */
static void ExplainSubPlans(DistributedPlan* distributedPlan, ExplainState* es)
{
    ListCell* subPlanCell = NULL;
    uint64 planId = distributedPlan->planId;

    ExplainOpenGroup("Subplans", "Subplans", false, es);

    foreach (subPlanCell, distributedPlan->subPlanList) {
        DistributedSubPlan* subPlan = (DistributedSubPlan*)lfirst(subPlanCell);
        PlannedStmt* plan = subPlan->plan;
        IntoClause* into = NULL;
        ParamListInfo params = NULL;

        /*
         * With PG14, we need to provide a string here,
         * for now we put an empty string, which is valid according to postgres.
         */
        char* queryString = pstrdup("");
        instr_time planduration;
        BufferUsage bufusage_start, bufusage;

        if (es->buffers) {
            bufusage_start = *u_sess->instr_cxt.pg_buffer_usage;
        }

        if (es->format == EXPLAIN_FORMAT_TEXT) {
            char* resultId = GenerateResultId(planId, subPlan->subPlanId);

            appendStringInfoSpaces(es->str, es->indent * 2);
            appendStringInfo(es->str, "->  Distributed Subplan %s\n", resultId);
            es->indent += 3;
        }

        ExplainOpenGroup("Subplan", NULL, true, es);

        if (es->analyze) {
            if (es->timing) {
                ExplainPropertyFloat("Subplan Duration", subPlan->durationMillisecs, 2,
                                     es);
            }

            ExplainPropertyBytes("Intermediate Data Size", subPlan->bytesSentPerWorker,
                                 es);

            StringInfo destination = makeStringInfo();
            if (subPlan->remoteWorkerCount && subPlan->writeLocalFile) {
                appendStringInfo(destination, "Send to %d nodes, write locally",
                                 subPlan->remoteWorkerCount);
            } else if (subPlan->writeLocalFile) {
                appendStringInfoString(destination, "Write locally");
            } else {
                appendStringInfo(destination, "Send to %d nodes",
                                 subPlan->remoteWorkerCount);
            }

            ExplainPropertyText("Result destination", destination->data, es);
        }

        INSTR_TIME_SET_ZERO(planduration);

        /* calc differences of buffer counters. */
        if (es->buffers) {
            memset(&bufusage, 0, sizeof(BufferUsage));
            BufferUsageAccumDiff(&bufusage, u_sess->instr_cxt.pg_buffer_usage,
                                 &bufusage_start);
        }

        ExplainOpenGroup("PlannedStmt", "PlannedStmt", false, es);
        ExplainOnePlan(plan, into, es, queryString, NULL, params);

        ExplainCloseGroup("PlannedStmt", "PlannedStmt", false, es);
        ExplainCloseGroup("Subplan", NULL, true, es);

        if (es->format == EXPLAIN_FORMAT_TEXT) {
            es->indent -= 3;
        }
    }

    ExplainCloseGroup("Subplans", "Subplans", false, es);
}

/*
 * ExplainPropertyBytes formats bytes in a human readable way by using
 * pg_size_pretty.
 */
static void ExplainPropertyBytes(const char* qlabel, int64 bytes, ExplainState* es)
{
    Datum textDatum = DirectFunctionCall1(pg_size_pretty, Int64GetDatum(bytes));
    ExplainPropertyText(qlabel, TextDatumGetCString(textDatum), es);
}

/*
 * ShowReceivedTupleData returns true if explain should show received data.
 * This is only the case when using EXPLAIN ANALYZE on queries that return
 * rows.
 */
static bool ShowReceivedTupleData(CitusScanState* scanState, ExplainState* es)
{
    TupleDesc tupDesc = ScanStateGetTupleDescriptor(scanState);
    return es->analyze && tupDesc != NULL && tupDesc->natts > 0;
}

/*
 * ExplainJob shows the EXPLAIN output for a Job in the physical plan of
 * a distributed query by showing the remote EXPLAIN for the first task,
 * or all tasks if spq.explain_all_tasks is on.
 */
static void ExplainJob(CitusScanState* scanState, Job* job, ExplainState* es,
                       ParamListInfo params)
{
    List* dependentJobList = job->dependentJobList;
    int dependentJobCount = list_length(dependentJobList);
    ListCell* dependentJobCell = NULL;
    List* taskList = job->taskList;
    int taskCount = list_length(taskList);

    ExplainOpenGroup("Job", "Job", true, es);

    ExplainPropertyInteger("Task Count", taskCount, es);
    if (ShowReceivedTupleData(scanState, es)) {
        Task* task = NULL;
        uint64 totalReceivedTupleDataForAllTasks = 0;
        foreach_declared_ptr(task, taskList)
        {
            totalReceivedTupleDataForAllTasks += TaskReceivedTupleData(task);
        }
        ExplainPropertyBytes("Tuple data received from nodes",
                             totalReceivedTupleDataForAllTasks, es);
    }

    if (dependentJobCount > 0) {
        ExplainPropertyText("Tasks Shown",
                            "None, not supported for re-partition "
                            "queries",
                            es);
    } else if (Session_ctx::Vars().ExplainAllTasks || taskCount <= 1) {
        ExplainPropertyText("Tasks Shown", "All", es);
    } else {
        StringInfo tasksShownText = makeStringInfo();
        appendStringInfo(tasksShownText, "One of %d", taskCount);

        ExplainPropertyText("Tasks Shown", tasksShownText->data, es);
    }

    /*
     * We cannot fetch EXPLAIN plans for jobs that have dependencies, since the
     * intermediate tables have not been created.
     */
    if (dependentJobCount == 0) {
        ExplainOpenGroup("Tasks", "Tasks", false, es);

        ExplainTaskList(scanState, taskList, es, params);

        ExplainCloseGroup("Tasks", "Tasks", false, es);
    } else {
        ExplainOpenGroup("Dependent Jobs", "Dependent Jobs", false, es);

        /* show explain output for dependent jobs, if any */
        foreach (dependentJobCell, dependentJobList) {
            Job* dependentJob = (Job*)lfirst(dependentJobCell);

            if (CitusIsA(dependentJob, MapMergeJob)) {
                ExplainMapMergeJob((MapMergeJob*)dependentJob, es);
            }
        }

        ExplainCloseGroup("Dependent Jobs", "Dependent Jobs", false, es);
    }

    ExplainCloseGroup("Job", "Job", true, es);
}

/*
 * TaskReceivedTupleData returns the amount of data that was received by the
 * coordinator for the task. If it's a RETURNING DML task the value stored in
 * totalReceivedTupleData is not correct yet because it only counts the bytes for
 * one placement.
 */
static uint64 TaskReceivedTupleData(Task* task)
{
    if (task->taskType == MODIFY_TASK) {
        return task->totalReceivedTupleData * list_length(task->taskPlacementList);
    }
    return task->totalReceivedTupleData;
}

/*
 * ExplainMapMergeJob shows a very basic EXPLAIN plan for a MapMergeJob. It does
 * not yet show the EXPLAIN plan for the individual tasks, because this requires
 * specific logic for getting the query (which is wrapped in a UDF), and the
 * queries may use intermediate tables that have not been created.
 */
static void ExplainMapMergeJob(MapMergeJob* mapMergeJob, ExplainState* es)
{
    List* dependentJobList = mapMergeJob->job.dependentJobList;
    int dependentJobCount = list_length(dependentJobList);
    ListCell* dependentJobCell = NULL;
    int mapTaskCount = list_length(mapMergeJob->mapTaskList);
    int mergeTaskCount = list_length(mapMergeJob->mergeTaskList);

    if (es->format == EXPLAIN_FORMAT_TEXT) {
        appendStringInfoSpaces(es->str, es->indent * 2);
        appendStringInfo(es->str, "->  MapMergeJob\n");
        es->indent += 3;
    }

    ExplainOpenGroup("MapMergeJob", NULL, true, es);
    ExplainPropertyInteger("Map Task Count", mapTaskCount, es);
    ExplainPropertyInteger("Merge Task Count", mergeTaskCount, es);

    if (dependentJobCount > 0) {
        ExplainOpenGroup("Dependent Jobs", "Dependent Jobs", false, es);

        foreach (dependentJobCell, dependentJobList) {
            Job* dependentJob = (Job*)lfirst(dependentJobCell);

            if (CitusIsA(dependentJob, MapMergeJob)) {
                ExplainMapMergeJob((MapMergeJob*)dependentJob, es);
            }
        }

        ExplainCloseGroup("Dependent Jobs", "Dependent Jobs", false, es);
    }

    ExplainCloseGroup("MapMergeJob", NULL, true, es);

    if (es->format == EXPLAIN_FORMAT_TEXT) {
        es->indent -= 3;
    }
}

/*
 * CompareTasksByFetchedExplainAnalyzeDuration is a helper function to compare two tasks
 * by their execution duration.
 */
static int CompareTasksByFetchedExplainAnalyzeDuration(const void* leftElement,
                                                       const void* rightElement)
{
    const Task* leftTask = *((const Task**)leftElement);
    const Task* rightTask = *((const Task**)rightElement);

    double leftTaskExecutionDuration = leftTask->fetchedExplainAnalyzeExecutionDuration;
    double rightTaskExecutionDuration = rightTask->fetchedExplainAnalyzeExecutionDuration;

    double diff = leftTaskExecutionDuration - rightTaskExecutionDuration;
    if (diff > 0) {
        return -1;
    } else if (diff < 0) {
        return 1;
    }
    return 0;
}

/*
 * ExplainTaskList shows the remote EXPLAIN and execution time for the first task
 * in taskList, or all tasks if spq.explain_all_tasks is on.
 */
static void ExplainTaskList(CitusScanState* scanState, List* taskList, ExplainState* es,
                            ParamListInfo params)
{
    List* remoteExplainList = NIL;

    /* if tasks are executed, we sort them by time; unless we are on a test env */
    if (es->analyze &&
        Session_ctx::Vars().ExplainAnalyzeSortMethod == EXPLAIN_ANALYZE_SORT_BY_TIME) {
        /* sort by execution duration only in case of ANALYZE */
        taskList = SortList(taskList, CompareTasksByFetchedExplainAnalyzeDuration);
    } else {
        /* make sure that the output is consistent */
        taskList = SortList(taskList, CompareTasksByTaskId);
    }

    Task* task = NULL;
    foreach_declared_ptr(task, taskList)
    {
        RemoteExplainPlan* remoteExplain = RemoteExplain(task, es, params);
        remoteExplainList = lappend(remoteExplainList, remoteExplain);

        if (!Session_ctx::Vars().ExplainAllTasks) {
            break;
        }
    }

    RemoteExplainPlan* remoteExplain = NULL;
    forboth_ptr(task, taskList, remoteExplain, remoteExplainList)
    {
        ExplainTask(scanState, task, remoteExplain->placementIndex,
                    remoteExplain->explainOutputList, es);
    }
}

/*
 * RemoteExplain fetches the remote EXPLAIN output for a single task.
 */
static RemoteExplainPlan* RemoteExplain(Task* task, ExplainState* es,
                                        ParamListInfo params)
{
    /*
     * For EXPLAIN EXECUTE we still use the old method, so task->fetchedExplainAnalyzePlan
     * can be NULL for some cases of es->analyze == true.
     */
    if (es->analyze && task->fetchedExplainAnalyzePlan) {
        return GetSavedRemoteExplain(task, es);
    } else {
        return FetchRemoteExplainFromWorkers(task, es, params);
    }
}

/*
 * GetSavedRemoteExplain creates a remote EXPLAIN output from information saved
 * in task.
 */
static RemoteExplainPlan* GetSavedRemoteExplain(Task* task, ExplainState* es)
{
    RemoteExplainPlan* remotePlan =
        (RemoteExplainPlan*)palloc0(sizeof(RemoteExplainPlan));

    /*
     * Similar to postgres' ExplainQuery(), we split by newline only for
     * text format.
     */
    if (es->format == EXPLAIN_FORMAT_TEXT) {
        /*
         * We limit the size of EXPLAIN plans to RSIZE_MAX_MEM (256MB).
         */
        remotePlan->explainOutputList =
            SplitString(task->fetchedExplainAnalyzePlan, '\n', RSIZE_MAX_MEM);
    } else {
        StringInfo explainAnalyzeString = makeStringInfo();
        appendStringInfoString(explainAnalyzeString, task->fetchedExplainAnalyzePlan);
        remotePlan->explainOutputList = list_make1(explainAnalyzeString);
    }

    remotePlan->placementIndex = task->fetchedExplainAnalyzePlacementIndex;

    return remotePlan;
}

/*
 * FetchRemoteExplainFromWorkers fetches the remote EXPLAIN output for a single
 * task by querying it from worker nodes. It tries each shard placement until
 * one succeeds or all failed.
 */
static RemoteExplainPlan* FetchRemoteExplainFromWorkers(Task* task, ExplainState* es,
                                                        ParamListInfo params)
{
    List* taskPlacementList = task->taskPlacementList;
    int placementCount = list_length(taskPlacementList);

    RemoteExplainPlan* remotePlan =
        (RemoteExplainPlan*)palloc0(sizeof(RemoteExplainPlan));

    StringInfo explainQuery = BuildRemoteExplainQuery(TaskQueryString(task), es);

    /*
     * Use a coordinated transaction to ensure that we open a transaction block
     * such that we can set a savepoint.
     */
    UseCoordinatedTransaction();

    for (int placementIndex = 0; placementIndex < placementCount; placementIndex++) {
        ShardPlacement* taskPlacement =
            static_cast<ShardPlacement*>(list_nth(taskPlacementList, placementIndex));
        int connectionFlags = 0;

        remotePlan->placementIndex = placementIndex;

        MultiConnection* connection =
            GetPlacementConnection(connectionFlags, taskPlacement, NULL);

        /*
         * This code-path doesn't support optional connections, so we don't expect
         * NULL connections.
         */
        Assert(connection != NULL);

        /* try other placements if we fail to connect this one */
        if (PQstatus(connection->pgConn) != CONNECTION_OK) {
            continue;
        }

        RemoteTransactionBeginIfNecessary(connection);

        /*
         * Start a savepoint for the explain query. After running the explain
         * query, we will rollback to this savepoint. This saves us from side
         * effects of EXPLAIN ANALYZE on DML queries.
         */
        ExecuteCriticalRemoteCommand(connection, "SAVEPOINT citus_explain_savepoint");

        /* run explain query */
        int numParams = params ? params->numParams : 0;
        Oid* paramTypes = NULL;
        const char** paramValues = NULL;
        PGresult* queryResult = NULL;

        if (params) {
            ExtractParametersFromParamList(params, &paramTypes, &paramValues, false);
        }

        int sendStatus = SendRemoteCommandParams(
            connection, explainQuery->data, numParams, paramTypes, paramValues, false);
        if (sendStatus != 0) {
            queryResult = GetRemoteCommandResult(connection, false);
            if (!IsResponseOK(queryResult)) {
                PQclear(queryResult);
                ForgetResults(connection);
                continue;
            }
        }

        /* read explain query results */
        remotePlan->explainOutputList = ReadFirstColumnAsText(queryResult);

        PQclear(queryResult);
        ForgetResults(connection);

        /* rollback to the savepoint */
        ExecuteCriticalRemoteCommand(connection,
                                     "ROLLBACK TO SAVEPOINT citus_explain_savepoint");

        if (remotePlan->explainOutputList != NIL) {
            break;
        }
    }

    return remotePlan;
}

/*
 * ExplainTask shows the EXPLAIN output for an single task. The output has been
 * fetched from the placement at index placementIndex. If explainOutputList is NIL,
 * then the EXPLAIN output could not be fetched from any placement.
 */
static void ExplainTask(CitusScanState* scanState, Task* task, int placementIndex,
                        List* explainOutputList, ExplainState* es)
{
    ExplainOpenGroup("Task", NULL, true, es);

    if (es->format == EXPLAIN_FORMAT_TEXT) {
        appendStringInfoSpaces(es->str, es->indent * 2);
        appendStringInfo(es->str, "->  Task\n");
        es->indent += 3;
    }

    if (es->verbose) {
        const char* queryText = TaskQueryString(task);
        ExplainPropertyText("Query", queryText, es);
    }

    if (ShowReceivedTupleData(scanState, es)) {
        ExplainPropertyBytes("Tuple data received from node", TaskReceivedTupleData(task),
                             es);
    }

    if (explainOutputList != NIL) {
        List* taskPlacementList = task->taskPlacementList;
        ShardPlacement* taskPlacement =
            static_cast<ShardPlacement*>(list_nth(taskPlacementList, placementIndex));

        ExplainTaskPlacement(taskPlacement, explainOutputList, es);
    } else {
        ExplainPropertyText("Error", "Could not get remote plan.", es);
    }

    ExplainCloseGroup("Task", NULL, true, es);

    if (es->format == EXPLAIN_FORMAT_TEXT) {
        es->indent -= 3;
    }
}

/*
 * ExplainTaskPlacement shows the EXPLAIN output for an individual task placement.
 * It corrects the indentation of the remote explain output to match the local
 * output.
 */
static void ExplainTaskPlacement(ShardPlacement* taskPlacement, List* explainOutputList,
                                 ExplainState* es)
{
    int savedIndentation = es->indent;
    StringInfo nodeAddress = makeStringInfo();
    char* nodeName = taskPlacement->nodeName;
    uint32 nodePort = taskPlacement->nodePort;
    const char* nodeDatabase = CurrentDatabaseName();
    ListCell* explainOutputCell = NULL;
    int rowIndex = 0;

    appendStringInfo(nodeAddress, "host=%s port=%d dbname=%s", nodeName, nodePort,
                     nodeDatabase);
    ExplainPropertyText("Node", nodeAddress->data, es);

    ExplainOpenGroup("Remote Plan", "Remote Plan", false, es);

    if (es->format == EXPLAIN_FORMAT_JSON || es->format == EXPLAIN_FORMAT_YAML) {
        /* prevent appending the remote EXPLAIN on the same line */
        appendStringInfoChar(es->str, '\n');
    }

    foreach (explainOutputCell, explainOutputList) {
        StringInfo rowString = (StringInfo)lfirst(explainOutputCell);

        int rowLength = strlen(rowString->data);
        char* lineStart = rowString->data;

        /* parse the lines in the remote EXPLAIN for proper indentation */
        while (lineStart < rowString->data + rowLength) {
            /* find the end-of-line */
            char* lineEnd = strchr(lineStart, '\n');

            if (lineEnd == NULL) {
                /* no end-of-line, use end of row string instead */
                lineEnd = rowString->data + rowLength;
            }

            /* convert line to a separate string */
            *lineEnd = '\0';

            /* indentation that is applied to all lines */
            appendStringInfoSpaces(es->str, es->indent * 2);

            if (es->format == EXPLAIN_FORMAT_TEXT && rowIndex == 0) {
                /* indent the first line of the remote plan with an arrow */
                appendStringInfoString(es->str, "->  ");
                es->indent += 2;
            }

            /* show line in the output */
            appendStringInfo(es->str, "%s\n", lineStart);

            /* continue at the start of the next line */
            lineStart = lineEnd + 1;
        }

        rowIndex++;
    }

    ExplainCloseGroup("Remote Plan", "Remote Plan", false, es);

    if (es->format == EXPLAIN_FORMAT_TEXT) {
        es->indent = savedIndentation;
    }
}

/*
 * BuildRemoteExplainQuery returns an EXPLAIN query string
 * to run on a worker node which explicitly contains all
 * the options in the explain state.
 */
static StringInfo BuildRemoteExplainQuery(char* queryString, ExplainState* es)
{
    StringInfo explainQuery = makeStringInfo();
    const char* formatStr = ExplainFormatStr(es->format);

    appendStringInfo(explainQuery,
                     "EXPLAIN (ANALYZE %s, VERBOSE %s, "
                     "COSTS %s, BUFFERS %s, "
                     "TIMING %s, FORMAT %s) %s",
                     es->analyze ? "TRUE" : "FALSE", es->verbose ? "TRUE" : "FALSE",
                     es->costs ? "TRUE" : "FALSE", es->buffers ? "TRUE" : "FALSE",
                     es->timing ? "TRUE" : "FALSE", formatStr, queryString);

    return explainQuery;
}

/*
 * ExplainFormatStr converts the given explain format to string.
 */
static const char* ExplainFormatStr(ExplainFormat format)
{
    switch (format) {
        case EXPLAIN_FORMAT_XML: {
            return "XML";
        }

        case EXPLAIN_FORMAT_JSON: {
            return "JSON";
        }

        case EXPLAIN_FORMAT_YAML: {
            return "YAML";
        }

        default: {
            return "TEXT";
        }
    }
}

/*
 * worker_last_saved_explain_analyze returns the last saved EXPLAIN ANALYZE output of
 * a worker task query. It returns NULL if nothing has been saved yet.
 */
Datum worker_last_saved_explain_analyze(PG_FUNCTION_ARGS)
{
    CheckCitusVersion(ERROR);

    TupleDesc tupleDescriptor = NULL;
    Tuplestorestate* tupleStore = SetupTuplestore(fcinfo, &tupleDescriptor);

    if (Session_ctx::PlanCtx().SavedExplainPlan != nullptr) {
        int columnCount = tupleDescriptor->natts;
        if (columnCount != 2) {
            ereport(ERROR, (errmsg("expected 3 output columns in definition of "
                                   "worker_last_saved_explain_analyze, but got %d",
                                   columnCount)));
        }

        bool columnNulls[2] = {false};
        Datum columnValues[2] = {
            CStringGetTextDatum(Session_ctx::PlanCtx().SavedExplainPlan),
            Float8GetDatum(Session_ctx::PlanCtx().SavedExecutionDurationMillisec)};

        tuplestore_putvalues(tupleStore, tupleDescriptor, columnValues, columnNulls);
    }
    PG_RETURN_DATUM(0);
}

/*
 * worker_save_query_explain_analyze executes and returns results of query while
 * saving its EXPLAIN ANALYZE to be fetched later.
 */
Datum worker_save_query_explain_analyze(PG_FUNCTION_ARGS)
{
    CheckCitusVersion(ERROR);

    text* queryText = PG_GETARG_TEXT_P(0);
    char* queryString = text_to_cstring(queryText);
    double executionDurationMillisec = 0.0;

    Datum explainOptions = PG_GETARG_DATUM(1);
    ExplainState* es = (ExplainState*)palloc0(sizeof(ExplainState));

    /* Set default options (most fields can be left as zeroes). */
    es->costs = true;
    /* Prepare output buffer. */
    es->str = makeStringInfo();
    es->analyze = true;

    /* use the same defaults as NewExplainState() for following options */
    es->buffers = ExtractFieldBoolean(explainOptions, "buffers", es->buffers);
    es->costs = ExtractFieldBoolean(explainOptions, "costs", es->costs);
    es->verbose = ExtractFieldBoolean(explainOptions, "verbose", es->verbose);
    es->timing = ExtractFieldBoolean(explainOptions, "timing", es->timing);
    es->format = ExtractFieldExplainFormat(explainOptions, "format", es->format);

    TupleDesc tupleDescriptor = NULL;
    Tuplestorestate* tupleStore = SetupTuplestore(fcinfo, &tupleDescriptor);
    DestReceiver* tupleStoreDest = CreateTuplestoreDestReceiver();
    SetTuplestoreDestReceiverParams(tupleStoreDest, tupleStore, CurrentMemoryContext,
                                    false);

    List* parseTreeList = pg_parse_query(queryString);
    if (list_length(parseTreeList) != 1) {
        ereport(ERROR, (errmsg("cannot EXPLAIN ANALYZE multiple queries")));
    }

    RawStmt* parseTree = static_cast<RawStmt*>(linitial(parseTreeList));

    ParamListInfo boundParams = ExecutorBoundParams();
    int numParams = boundParams ? boundParams->numParams : 0;
    Oid* paramTypes = NULL;
    const char** paramValues = NULL;

    if (boundParams != NULL) {
        ExtractParametersFromParamList(boundParams, &paramTypes, &paramValues, false);
    }

    /* resolve OIDs of unknown (user-defined) types */
    Query* analyzedQuery =
        parse_analyze_varparams(parseTree, queryString, &paramTypes, &numParams);

    /* pg_rewrite_query is a wrapper around QueryRewrite with some debugging logic */
    List* queryList = pg_rewrite_query(analyzedQuery);

    if (list_length(queryList) != 1) {
        ereport(ERROR, (errmsg("cannot EXPLAIN ANALYZE a query rewritten "
                               "into multiple queries")));
    }

    Query* query = static_cast<Query*>(linitial(queryList));

    ExplainBeginOutput(es);

    /* plan query and record planning stats */
    instr_time planStart;
    instr_time planDuration;
    INSTR_TIME_SET_CURRENT(planStart);

    PlannedStmt* plan = pg_plan_query(query, CURSOR_OPT_CUSTOM_PLAN, NULL);

    INSTR_TIME_SET_CURRENT(planDuration);
    INSTR_TIME_SUBTRACT(planDuration, planStart);

    /* do the actual EXPLAIN ANALYZE */
    ExplainWorkerPlan(plan, tupleStoreDest, es, queryString, boundParams, NULL,
                      &planDuration, &executionDurationMillisec);

    ExplainEndOutput(es);

    /* save EXPLAIN ANALYZE result to be fetched later */
    MemoryContext oldContext = MemoryContextSwitchTo(u_sess->top_transaction_mem_cxt);
    FreeSavedExplainPlan();

    Session_ctx::PlanCtx().SavedExplainPlan = pstrdup(es->str->data);
    Session_ctx::PlanCtx().SavedExecutionDurationMillisec = executionDurationMillisec;

    MemoryContextSwitchTo(oldContext);

    PG_RETURN_DATUM(0);
}

/*
 * FreeSavedExplainPlan frees allocated saved explain plan if any.
 */
void FreeSavedExplainPlan(void)
{
    if (Session_ctx::PlanCtx().SavedExplainPlan) {
        pfree(Session_ctx::PlanCtx().SavedExplainPlan);
        Session_ctx::PlanCtx().SavedExplainPlan = nullptr;
    }
}

/*
 * ExtractFieldExplainFormat gets value of fieldName from jsonbDoc, or returns
 * defaultValue if it doesn't exist.
 */
static ExplainFormat ExtractFieldExplainFormat(Datum jsonbDoc, const char* fieldName,
                                               ExplainFormat defaultValue)
{
    Datum jsonbDatum = 0;
    bool found = ExtractFieldJsonbDatum(jsonbDoc, fieldName, &jsonbDatum);
    if (!found) {
        return defaultValue;
    }

    const char* formatStr = DatumGetCString(DirectFunctionCall1(jsonb_out, jsonbDatum));
    if (pg_strcasecmp(formatStr, "\"text\"") == 0) {
        return EXPLAIN_FORMAT_TEXT;
    } else if (pg_strcasecmp(formatStr, "\"xml\"") == 0) {
        return EXPLAIN_FORMAT_XML;
    } else if (pg_strcasecmp(formatStr, "\"yaml\"") == 0) {
        return EXPLAIN_FORMAT_YAML;
    } else if (pg_strcasecmp(formatStr, "\"json\"") == 0) {
        return EXPLAIN_FORMAT_JSON;
    }

    ereport(ERROR, (errmsg("Invalid explain analyze format: %s", formatStr)));
    return EXPLAIN_FORMAT_TEXT;
}

/*
 * CreateExplainAnlyzeDestination creates a destination suitable for collecting
 * explain analyze output from workers.
 */
static TupleDestination* CreateExplainAnlyzeDestination(Task* task,
                                                        TupleDestination* taskDest)
{
    ExplainAnalyzeDestination* tupleDestination = static_cast<ExplainAnalyzeDestination*>(
        palloc0(sizeof(ExplainAnalyzeDestination)));
    tupleDestination->originalTask = task;
    tupleDestination->originalTaskDestination = taskDest;

    TupleDesc lastSavedExplainAnalyzeTupDesc = CreateTemplateTupleDesc(2, false);

    TupleDescInitEntry(lastSavedExplainAnalyzeTupDesc, 1, "explain analyze", TEXTOID, 0,
                       0);
    TupleDescInitEntry(lastSavedExplainAnalyzeTupDesc, 2, "duration", FLOAT8OID, 0, 0);

    tupleDestination->lastSavedExplainAnalyzeTupDesc = lastSavedExplainAnalyzeTupDesc;

    tupleDestination->pub.putTuple = ExplainAnalyzeDestPutTuple;
    tupleDestination->pub.tupleDescForQuery = ExplainAnalyzeDestTupleDescForQuery;

    return (TupleDestination*)tupleDestination;
}

/*
 * ExplainAnalyzeDestPutTuple implements TupleDestination->putTuple
 * for ExplainAnalyzeDestination.
 */
static void ExplainAnalyzeDestPutTuple(TupleDestination* self, Task* task,
                                       int placementIndex, int queryNumber,
                                       HeapTuple heapTuple, uint64 tupleLibpqSize)
{
    ExplainAnalyzeDestination* tupleDestination = (ExplainAnalyzeDestination*)self;
    if (queryNumber == 0) {
        TupleDestination* originalTupDest = tupleDestination->originalTaskDestination;
        originalTupDest->putTuple(originalTupDest, task, placementIndex, 0, heapTuple,
                                  tupleLibpqSize);
        tupleDestination->originalTask->totalReceivedTupleData += tupleLibpqSize;
    } else if (queryNumber == 1) {
        bool isNull = false;
        TupleDesc tupDesc = tupleDestination->lastSavedExplainAnalyzeTupDesc;
        Datum explainAnalyze = heap_getattr(heapTuple, 1, tupDesc, &isNull);

        if (isNull) {
            ereport(WARNING,
                    (errmsg("received null explain analyze output from worker")));
            return;
        }

        Datum executionDuration = heap_getattr(heapTuple, 2, tupDesc, &isNull);

        if (isNull) {
            ereport(WARNING, (errmsg("received null execution time from worker")));
            return;
        }

        char* fetchedExplainAnalyzePlan = TextDatumGetCString(explainAnalyze);
        double fetchedExplainAnalyzeExecutionDuration = DatumGetFloat8(executionDuration);

        /*
         * Allocate fetchedExplainAnalyzePlan in the same context as the Task, since we
         * are currently in execution context and a Task can span multiple executions.
         *
         * Although we won't reuse the same value in a future execution, but we have
         * calls to CheckNodeCopyAndSerialization() which asserts copy functions of the
         * task work as expected, which will try to copy this value in a future execution.
         *
         * Why don't we just allocate this field in executor context and reset it before
         * the next execution? Because when an error is raised we can skip pretty much
         * most of the meaningful places that we can insert the reset.
         *
         * TODO: Take all EXPLAIN ANALYZE related fields out of Task and store them in a
         * Task to ExplainAnalyzePrivate mapping in multi_explain.c, so we don't need to
         * do these hacky memory context management tricks.
         */
        MemoryContext taskContext = GetMemoryChunkContext(tupleDestination->originalTask);

        tupleDestination->originalTask->fetchedExplainAnalyzePlan =
            MemoryContextStrdup(taskContext, fetchedExplainAnalyzePlan);
        tupleDestination->originalTask->fetchedExplainAnalyzePlacementIndex =
            placementIndex;
        tupleDestination->originalTask->fetchedExplainAnalyzeExecutionDuration =
            fetchedExplainAnalyzeExecutionDuration;
    } else {
        ereport(ERROR, (errmsg("cannot get EXPLAIN ANALYZE of multiple queries"),
                        errdetail("while receiving tuples for query %d", queryNumber)));
    }
}

/*
 * ResetExplainAnalyzeData reset fields in Task that are used by multi_explain.c
 */
void ResetExplainAnalyzeData(List* taskList)
{
    Task* task = NULL;
    foreach_declared_ptr(task, taskList)
    {
        if (task->fetchedExplainAnalyzePlan != NULL) {
            pfree(task->fetchedExplainAnalyzePlan);
        }

        task->totalReceivedTupleData = 0;
        task->fetchedExplainAnalyzePlacementIndex = 0;
        task->fetchedExplainAnalyzePlan = NULL;
    }
}

/*
 * ExplainAnalyzeDestTupleDescForQuery implements TupleDestination->tupleDescForQuery
 * for ExplainAnalyzeDestination.
 */
static TupleDesc ExplainAnalyzeDestTupleDescForQuery(TupleDestination* self,
                                                     int queryNumber)
{
    ExplainAnalyzeDestination* tupleDestination = (ExplainAnalyzeDestination*)self;
    if (queryNumber == 0) {
        TupleDestination* originalTupDest = tupleDestination->originalTaskDestination;
        return originalTupDest->tupleDescForQuery(originalTupDest, 0);
    } else if (queryNumber == 1) {
        return tupleDestination->lastSavedExplainAnalyzeTupDesc;
    }

    ereport(ERROR, (errmsg("cannot get EXPLAIN ANALYZE of multiple queries"),
                    errdetail("while requesting for tuple descriptor of query %d",
                              queryNumber)));
    return NULL;
}

/*
 * RequestedForExplainAnalyze returns true if we should get the EXPLAIN ANALYZE
 * output for the given custom scan node.
 */
bool RequestedForExplainAnalyze(CitusScanState* node)
{
    return (node->customScanState.ss.ps.state->es_instrument != 0);
}

/*
 * ExplainAnalyzeTaskList returns a task list suitable for explain analyze. After
 * executing these tasks, fetchedExplainAnalyzePlan of originalTaskList should be
 * populated.
 */
List* ExplainAnalyzeTaskList(List* originalTaskList, TupleDestination* defaultTupleDest,
                             TupleDesc tupleDesc, ParamListInfo params)
{
    List* explainAnalyzeTaskList = NIL;
    Task* originalTask = NULL;

    foreach_declared_ptr(originalTask, originalTaskList)
    {
        if (originalTask->queryCount != 1) {
            ereport(ERROR, (errmsg("cannot get EXPLAIN ANALYZE of multiple queries")));
        }

        Task* explainAnalyzeTask = static_cast<Task*>(copyObject(originalTask));
        const char* queryString = TaskQueryString(explainAnalyzeTask);
        ParamListInfo taskParams = params;

        /*
         * We will not send parameters if they have already been resolved in the query
         * string.
         */
        if (explainAnalyzeTask->parametersInQueryStringResolved) {
            taskParams = NULL;
        }

        char* wrappedQuery =
            WrapQueryForExplainAnalyze(queryString, tupleDesc, taskParams);
        char* fetchQuery = FetchPlanQueryForExplainAnalyze(queryString, taskParams);

        SetTaskQueryStringList(explainAnalyzeTask, list_make2(wrappedQuery, fetchQuery));

        TupleDestination* originalTaskDest =
            originalTask->tupleDest ? originalTask->tupleDest : defaultTupleDest;

        explainAnalyzeTask->tupleDest =
            CreateExplainAnlyzeDestination(originalTask, originalTaskDest);

        explainAnalyzeTaskList = lappend(explainAnalyzeTaskList, explainAnalyzeTask);
    }

    return explainAnalyzeTaskList;
}

/*
 * WrapQueryForExplainAnalyze wraps a query into a worker_save_query_explain_analyze()
 * call so we can fetch its explain analyze after its execution.
 */
static char* WrapQueryForExplainAnalyze(const char* queryString, TupleDesc tupleDesc,
                                        ParamListInfo params)
{
    StringInfo columnDef = makeStringInfo();
    for (int columnIndex = 0; columnIndex < tupleDesc->natts; columnIndex++) {
        if (columnIndex != 0) {
            appendStringInfoString(columnDef, ", ");
        }

        Form_pg_attribute attr = &tupleDesc->attrs[columnIndex];
        char* attrType =
            format_type_extended(attr->atttypid, attr->atttypmod,
                                 FORMAT_TYPE_TYPEMOD_GIVEN | FORMAT_TYPE_FORCE_QUALIFY);

        appendStringInfo(columnDef, "field_%d %s", columnIndex, attrType);
    }

    /*
     * column definition cannot be empty, so create a dummy column definition for
     * queries with no results.
     */
    if (tupleDesc->natts == 0) {
        appendStringInfo(columnDef, "dummy_field int");
    }

    StringInfo explainOptions = makeStringInfo();
    appendStringInfo(explainOptions,
                     "{\"verbose\": %s, \"costs\": %s, \"buffers\": %s, "
                     "\"timing\": %s, \"format\": \"%s\"}",
                     CurrentDistributedQueryExplainOptions.verbose ? "true" : "false",
                     CurrentDistributedQueryExplainOptions.costs ? "true" : "false",
                     CurrentDistributedQueryExplainOptions.buffers ? "true" : "false",
                     CurrentDistributedQueryExplainOptions.timing ? "true" : "false",
                     ExplainFormatStr(CurrentDistributedQueryExplainOptions.format));

    StringInfo wrappedQuery = makeStringInfo();

    /*
     * We do not include dummy column if original query didn't return any columns.
     * Otherwise, number of columns that original query returned wouldn't match
     * number of columns returned by worker_save_query_explain_analyze.
     */
    const char* workerSaveQueryFetchCols = (tupleDesc->natts == 0) ? "" : "*";

    if (params != NULL) {
        /*
         * Add a dummy CTE to ensure all parameters are referenced, such that their
         * types can be resolved.
         */
        appendStringInfo(wrappedQuery, "WITH unused AS (%s) ",
                         ParameterResolutionSubquery(params));
    }

    appendStringInfo(wrappedQuery,
                     "SELECT %s FROM worker_save_query_explain_analyze(%s, %s) AS (%s)",
                     workerSaveQueryFetchCols, quote_literal_cstr(queryString),
                     quote_literal_cstr(explainOptions->data), columnDef->data);

    return wrappedQuery->data;
}

/*
 * FetchPlanQueryForExplainAnalyze generates a query to fetch the plan saved
 * by worker_save_query_explain_analyze from the worker.
 */
static char* FetchPlanQueryForExplainAnalyze(const char* queryString,
                                             ParamListInfo params)
{
    StringInfo fetchQuery = makeStringInfo();

    if (params != NULL) {
        /*
         * Add a dummy CTE to ensure all parameters are referenced, such that their
         * types can be resolved.
         */
        appendStringInfo(fetchQuery, "WITH unused AS (%s) ",
                         ParameterResolutionSubquery(params));
    }

    appendStringInfoString(fetchQuery,
                           "SELECT explain_analyze_output, execution_duration "
                           "FROM worker_last_saved_explain_analyze()");

    return fetchQuery->data;
}

/*
 * ParameterResolutionSubquery generates a subquery that returns all parameters
 * in params with explicit casts to their type names. This can be used in cases
 * where we use custom type parameters that are not directly referenced.
 */
static char* ParameterResolutionSubquery(ParamListInfo params)
{
    StringInfo paramsQuery = makeStringInfo();

    appendStringInfo(paramsQuery, "SELECT");

    for (int paramIndex = 0; paramIndex < params->numParams; paramIndex++) {
        ParamExternData* param = &params->params[paramIndex];
        char* typeName =
            format_type_extended(param->ptype, -1, FORMAT_TYPE_FORCE_QUALIFY);

        appendStringInfo(paramsQuery, "%s $%d::%s", paramIndex > 0 ? "," : "",
                         paramIndex + 1, typeName);
    }

    return paramsQuery->data;
}

/*
 * SplitString splits the given string by the given delimiter.
 *
 * Why not use strtok_s()? Its signature and semantics are difficult to understand.
 *
 * Why not use strchr() (similar to do_text_output_multiline)? Although not banned,
 * it isn't safe if by any chance str is not null-terminated.
 */
static List* SplitString(const char* str, char delimiter, int maxLength)
{
    size_t len = strnlen(str, maxLength);
    if (len == 0) {
        return NIL;
    }

    List* tokenList = NIL;
    StringInfo token = makeStringInfo();

    for (size_t index = 0; index < len; index++) {
        if (str[index] == delimiter) {
            tokenList = lappend(tokenList, token);
            token = makeStringInfo();
        } else {
            appendStringInfoChar(token, str[index]);
        }
    }

    /* append last token */
    tokenList = lappend(tokenList, token);

    return tokenList;
}

/* below are private functions copied from explain.c */

/* *INDENT-OFF* */
/*
 * ExplainOneQuery -
 *	  print out the execution plan for one Query
 *
 * "into" is NULL unless we are explaining the contents of a CreateTableAsStmt.
 */
static void ExplainOneQuery(Query* query, int cursorOptions, IntoClause* into,
                            ExplainState* es, const char* queryString,
                            ParamListInfo params, QueryEnvironment* queryEnv)
{
    instr_time planstart, planduration;
    BufferUsage bufusage_start, bufusage;

    if (es->buffers)
        bufusage_start = *u_sess->instr_cxt.pg_buffer_usage;
    INSTR_TIME_SET_CURRENT(planstart);

    /* plan the query */
    PlannedStmt* plan = pg_plan_query(query, cursorOptions, params);

    INSTR_TIME_SET_CURRENT(planduration);
    INSTR_TIME_SUBTRACT(planduration, planstart);

    /* calc differences of buffer counters. */
    if (es->buffers) {
        memset(&bufusage, 0, sizeof(BufferUsage));
        BufferUsageAccumDiff(&bufusage, u_sess->instr_cxt.pg_buffer_usage,
                             &bufusage_start);
    }

    /* run it (if needed) and produce output */
    ExplainOnePlan(plan, into, es, queryString, NULL, params);
}

/*
 * ExplainWorkerPlan produces explain output into es. If es->analyze, it also executes
 * the given plannedStmt and sends the results to dest. It puts total time to execute in
 * executionDurationMillisec.
 *
 * This is based on postgres' ExplainOnePlan(). We couldn't use an IntoClause to store
 * results into tupleStore, so we had to copy the same functionality with some minor
 * changes.
 *
 * Keeping the formatting to make comparing with the ExplainOnePlan() easier.
 *
 * TODO: Send a PR to postgres to change ExplainOnePlan's API to use a more generic result
 * destination.
 */
static void ExplainWorkerPlan(PlannedStmt* plannedstmt, DestReceiver* dest,
                              ExplainState* es, const char* queryString,
                              ParamListInfo params, QueryEnvironment* queryEnv,
                              const instr_time* planduration,
                              double* executionDurationMillisec)
{
    QueryDesc* queryDesc;
    instr_time starttime;
    double totaltime = 0;
    int eflags;
    int instrument_option = 0;

    Assert(plannedstmt->commandType != CMD_UTILITY);

    if (es->analyze && es->timing)
        instrument_option |= INSTRUMENT_TIMER;
    else if (es->analyze)
        instrument_option |= INSTRUMENT_ROWS;

    if (es->buffers)
        instrument_option |= INSTRUMENT_BUFFERS;

    /*
     * We always collect timing for the entire statement, even when node-level
     * timing is off, so we don't look at es->timing here.  (We could skip
     * this if !es->summary, but it's hardly worth the complication.)
     */
    INSTR_TIME_SET_CURRENT(starttime);

    /*
     * Use a snapshot with an updated command ID to ensure this query sees
     * results of any previously executed queries.
     */
    PushCopiedSnapshot(GetActiveSnapshot());
    UpdateActiveSnapshotCommandId();

    /* Create a QueryDesc for the query */
    queryDesc = CreateQueryDesc(plannedstmt, queryString, GetActiveSnapshot(),
                                InvalidSnapshot, dest, params, instrument_option);

    /* Select execution options */
    if (es->analyze)
        eflags = 0; /* default run-to-completion flags */
    else
        eflags = EXEC_FLAG_EXPLAIN_ONLY;

    /* call ExecutorStart to prepare the plan for execution */
    ExecutorStart(queryDesc, eflags);

    /* Execute the plan for statistics if asked for */
    if (es->analyze) {
        ScanDirection dir = ForwardScanDirection;

        /* run the plan */
        ExecutorRun(queryDesc, dir, 0L);

        /* run cleanup too */
        ExecutorFinish(queryDesc);

        /* We can't run ExecutorEnd 'till we're done printing the stats... */
        totaltime += elapsed_time(&starttime);
    }

    ExplainOpenGroup("Query", NULL, true, es);

    /* Create textual dump of plan tree */
    ExplainPrintPlan(es, queryDesc);

    if (planduration) {
        double plantime = INSTR_TIME_GET_DOUBLE(*planduration);

        ExplainPropertyFloat("Planning Time", 1000.0 * plantime, 3, es);
    }

    /*
     * Close down the query and free resources.  Include time for this in the
     * total execution time (although it should be pretty minimal).
     */
    INSTR_TIME_SET_CURRENT(starttime);

    ExecutorEnd(queryDesc);

    FreeQueryDesc(queryDesc);

    PopActiveSnapshot();

    /* We need a CCI just in case query expanded to multiple plans */
    if (es->analyze)
        CommandCounterIncrement();

    totaltime += elapsed_time(&starttime);

    /*
     * We only report execution time if we actually ran the query (that is,
     * the user specified ANALYZE), and if summary reporting is enabled (the
     * user can set SUMMARY OFF to not have the timing information included in
     * the output).  By default, ANALYZE sets SUMMARY to true.
     */
    if (es->analyze) {
        ExplainPropertyFloat("Execution Time", 1000.0 * totaltime, 3, es);
    }

    *executionDurationMillisec = totaltime * 1000;

    ExplainCloseGroup("Query", NULL, true, es);
}