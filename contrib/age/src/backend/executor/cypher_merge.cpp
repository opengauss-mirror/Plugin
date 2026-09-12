/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include "postgres.h"

#include "access/hbindex_am.h"
#include "access/htup.h"
#include "access/tableam.h"
#include "access/xact.h"
#include "executor/tuptable.h"
#include "nodes/execnodes.h"
#include "nodes/ag_extensible.h"
#include "nodes/nodeFuncs.h"
#include "nodes/nodes.h"
#include "nodes/plannodes.h"
#include "parser/parse_relation.h"
#include "rewrite/rewriteHandler.h"
#include "utils/datum.h"
#include "utils/rel.h"
#include "executor/executor.h"

#include "catalog/ag_label.h"
#include "executor/cypher_executor.h"
#include "executor/cypher_utils.h"
#include "nodes/cypher_nodes.h"
#include "utils/agtype.h"
#include "utils/ag_cache.h"
#include "utils/graphid.h"

#include "executor/node/nodeMemoize.h"

/*
 * MERGE keeps a compact representation of paths created by this executor.
 * The child join cannot see tuples added after it was materialized, so later
 * input rows must be able to reuse an equivalent path without reinserting it.
 */
typedef struct path_entry {
    bool actual;
    cypher_rel_dir direction;
    graphid id;
    bool id_is_null;
    graphid start_id;
    graphid end_id;
    Oid label;
    Datum properties;
    bool properties_is_null;
    Datum entity_properties;
    bool entity_properties_is_null;
    uint32 properties_hash;
} path_entry;

typedef struct created_path {
    struct created_path *next;
    path_entry **entries;
} created_path;

static THR_LOCAL cypher_merge_custom_scan_state *initializing_merge = NULL;

static void begin_cypher_merge(ExtensiblePlanState *node, EState *estate,
                               int eflags);
static TupleTableSlot *exec_cypher_merge(ExtensiblePlanState *node);
static void end_cypher_merge(ExtensiblePlanState *node);
static void rescan_cypher_merge(ExtensiblePlanState *node);
static Datum merge_vertex(cypher_merge_custom_scan_state *css,
                          cypher_target_node *node, ListCell *next,
                          path_entry **path_array, int path_index,
                          bool should_insert);
static void merge_edge(cypher_merge_custom_scan_state *css,
                       cypher_target_node *node, Datum prev_vertex_id,
                       ListCell *next, path_entry **path_array, int path_index,
                       bool should_insert);
static void process_simple_merge(ExtensiblePlanState *node);
static bool check_path(cypher_merge_custom_scan_state *css,
                       TupleTableSlot *slot);
static void process_path(cypher_merge_custom_scan_state *css,
                         path_entry **path_array, bool should_insert);
static bool process_merge_input(ExtensiblePlanState *node,
                                TupleTableSlot *slot);
static TupleTableSlot *fetch_merge_input(ExtensiblePlanState *node,
                                         EState *estate);
static void store_merge_variable(TupleTableSlot *scantuple,
                                 int variable_position, Datum value,
                                 const char *caller);
static void mark_tts_isnull(TupleTableSlot *slot);
static void mark_scan_slot_valid(TupleTableSlot *slot);
static path_entry **prebuild_path(ExtensiblePlanState *node);
static bool compare_paths(path_entry **left, path_entry **right,
                          int path_length);
static path_entry **find_duplicate_path(ExtensiblePlanState *node,
                                        path_entry **path_array);
static void free_path_entries(path_entry **path_array, int path_length);
static void sync_created_path_entities(
    cypher_merge_custom_scan_state *css, path_entry **path_array,
    cypher_update_information *set_info);
static bool refresh_merge_scan(PlanState *planstate, void *context);
static void refresh_parent_merge_scans(cypher_merge_custom_scan_state *parent);

const ExtensibleExecMethods cypher_merge_exec_methods = {MERGE_SCAN_STATE_NAME,
    begin_cypher_merge,
    exec_cypher_merge,
    end_cypher_merge,
    rescan_cypher_merge,
    NULL};

static bool refresh_merge_scan(PlanState *planstate, void *context)
{
    (void)context;

    if (IsA(planstate, ExtensiblePlanState)) {
        ExtensiblePlanState *extensible_state =
            (ExtensiblePlanState *)planstate;

        if (extensible_state->methods == &cypher_merge_exec_methods) {
            return false;
        }
    }

    if (IsA(planstate, SeqScanState)) {
        SeqScanState *seq_state = (SeqScanState *)planstate;

        if (seq_state->ss_currentScanDesc != NULL)
            ExecReScan(planstate);

        return false;
    }

    if (IsA(planstate, IndexScanState)) {
        IndexScanState *index_state = (IndexScanState *)planstate;

        if (index_state->iss_ScanDesc != NULL &&
            index_state->iss_NumRuntimeKeys == 0) {
            scan_handler_idx_rescan(index_state->iss_ScanDesc,
                                    index_state->iss_ScanKeys,
                                    index_state->iss_NumScanKeys,
                                    index_state->iss_OrderByKeys,
                                    index_state->iss_NumOrderByKeys);
            scan_handler_idx_rescan_parallel(index_state->iss_ScanDesc);
            index_state->iss_ReachedEnd = false;
            ExecScanReScan(&index_state->ss);
        }

        return false;
    }

    return planstate_tree_walker(planstate, refresh_merge_scan, context);
}

static void refresh_parent_merge_scans(cypher_merge_custom_scan_state *parent)
{
    PlanState *subplan = parent->css.ss.ps.lefttree;

    if (subplan != NULL)
        refresh_merge_scan(subplan, NULL);
}

/*
 * Initializes the MERGE Execution Node at the beginning of the execution
 * phase.
 */
static void begin_cypher_merge(ExtensiblePlanState *node, EState *estate,
                               int eflags)
{
    cypher_merge_custom_scan_state *css =
        (cypher_merge_custom_scan_state *)node;
    ListCell *lc;
    Plan *subplan;

    css->created_paths_list = NULL;
    css->eager_tuples = NIL;
    css->eager_tuples_index = 0;
    css->eager_buffer_filled = false;

    Assert(list_length(css->cs->extensible_plans) == 1);

    // initialize the subplan
    subplan = (Plan*)linitial(css->cs->extensible_plans);
    css->parent_merge = initializing_merge;
    initializing_merge = css;

    PG_TRY();
    {
        node->ss.ps.lefttree = ExecInitNode(subplan, estate, eflags);
    }
    PG_CATCH();
    {
        initializing_merge = css->parent_merge;
        PG_RE_THROW();
    }
    PG_END_TRY();

    initializing_merge = css->parent_merge;

    ResultState* rs = (ResultState *)node->ss.ps.lefttree;
    TupleTableSlot* slot = rs->ps.ps_ResultTupleSlot;
    ExecStoreAllNullTuple(slot);

    ExecAssignExprContext(estate, &node->ss.ps);

    ExecInitScanTupleSlot(estate, &node->ss);

    /*
     * When MERGE is not the last clause in a cypher query. Setup projection
     * information to pass to the parent execution node.
     */
    if (!CYPHER_CLAUSE_IS_TERMINAL(css->flags))
    {
        TupleDesc tupdesc = node->ss.ss_ScanTupleSlot->tts_tupleDescriptor;

        ExecAssignProjectionInfo(&node->ss.ps, tupdesc);
    }

    /*
     * For each vertex and edge in the path, setup the information
     * needed if we need to create them.
     */
    foreach(lc, css->path->target_nodes)
    {
        cypher_target_node *cypher_node =
            (cypher_target_node *)lfirst(lc);
        Relation rel;

        /*
         * This entity references an entity that is already declared. Either
         * by a previous clause or an entity earlier in the MERGE path. In both
         * cases, this target_entry will not create data, only reference data
         * that already exists.
         */
        if (!CYPHER_TARGET_NODE_INSERT_ENTITY(cypher_node->flags))
        {
            continue;
        }

        // Open relation and aquire a row exclusive lock.
        rel = heap_open(cypher_node->relid, RowExclusiveLock);

        // Initialize resultRelInfo for the vertex
        cypher_node->resultRelInfo = makeNode(ResultRelInfo);
        InitResultRelInfo(cypher_node->resultRelInfo, rel,
                          list_length(estate->es_range_table),
                          estate->es_instrument);

        // Open all indexes for the relation
        ExecOpenIndices(cypher_node->resultRelInfo, false);

        // Setup the relation's tuple slot
        cypher_node->elemTupleSlot = ExecInitExtraTupleSlot(estate);

        ExecSetSlotDescriptor(cypher_node->elemTupleSlot,
            RelationGetDescr(cypher_node->resultRelInfo->ri_RelationDesc));

        if (cypher_node->id_expr != NULL)
        {
            cypher_node->id_expr_state =
                ExecInitExpr(cypher_node->id_expr, (PlanState *)node);
        }

        if (cypher_node->prop_expr != NULL)
        {
            cypher_node->prop_expr_state =
                ExecInitExpr(cypher_node->prop_expr, (PlanState *)node);
        }
    }

    /* Initialize ON CREATE/MATCH SET expressions once per plan. */
    if (css->on_create_set_info != NULL)
    {
        foreach (lc, css->on_create_set_info->set_items)
        {
            cypher_update_item *item = (cypher_update_item *)lfirst(lc);

            if (item->prop_expr != NULL) {
                item->prop_expr_state = ExecInitExpr(
                    (Expr *)item->prop_expr, (PlanState *)node);
            }
        }
    }

    if (css->on_match_set_info != NULL)
    {
        foreach (lc, css->on_match_set_info->set_items)
        {
            cypher_update_item *item = (cypher_update_item *)lfirst(lc);

            if (item->prop_expr != NULL) {
                item->prop_expr_state = ExecInitExpr(
                    (Expr *)item->prop_expr, (PlanState *)node);
            }
        }
    }

    /*
     * Postgres does not assign the es_output_cid in queries that do
     * not write to disk, ie: SELECT commands. We need the command id
     * for our clauses, and we may need to initialize it. We cannot use
     * GetCurrentCommandId because there may be other cypher clauses
     * that have modified the command id.
     */
    if (estate->es_output_cid == 0)
        estate->es_output_cid = estate->es_snapshot->curcid;

    /* store the currentCommandId for this instance */
    css->base_currentCommandId = GetCurrentCommandId(false);

    /* the child subtree must keep seeing the state before this clause */
    css->child_curcid = estate->es_snapshot->curcid;

    Increment_Estate_CommandId(estate);
}

/*
 * Checks the subtree to see if the lateral join representing the MERGE path
 * found results. Returns true if the path does not exist and must be created,
 * false otherwise.
 */
static bool check_path(cypher_merge_custom_scan_state *css,
                       TupleTableSlot *slot)
{
    cypher_create_path *path = css->path;
    ListCell *lc;

    foreach(lc, path->target_nodes)
    {
        cypher_target_node *node = (cypher_target_node*)lfirst(lc);

        /*
         * If target_node as a valid attribute number and is a node not
         * declared in a previous clause, check the tuple position in the
         * slot. If the slot is null, the path was not found. The rules
         * state that if one part of the path does not exists, the whole
         * path must be created.
         */
        if (node->tuple_position != InvalidAttrNumber ||
            ((node->flags & CYPHER_TARGET_NODE_MERGE_EXISTS) == 0))
        {
            /*
             * Attribute number is 1 indexed and tts_values is 0 indexed,
             * offset by 1.
             */
            if (slot->tts_isnull[node->tuple_position - 1])
            {
                return true;
            }
        }

    }

    return false;
}

static void process_path(cypher_merge_custom_scan_state *css,
                         path_entry **path_array, bool should_insert)
{
    cypher_create_path *path = css->path;
    ListCell *lc = list_head(path->target_nodes);

    if (css->path_values != NIL)
        list_free(css->path_values);
    css->path_values = NIL;

    /*
     * Create the first vertex. The create_vertex function will
     * create the rest of the path, if necessary.
     */
    merge_vertex(css, (cypher_target_node *)lfirst(lc), lnext(lc),
                 path_array, 0, should_insert);

    /*
     * If this path is a variable, take the list that was accumulated
     * in the vertex/edge creation, create a path datum, and add to the
     * scantuple slot.
     */
    if (path->path_attr_num != InvalidAttrNumber)
    {
        ExprContext *econtext = css->css.ss.ps.ps_ExprContext;
        TupleTableSlot *scantuple = econtext->ecxt_scantuple;
        Datum result;

        result = make_path(css->path_values);

        scantuple->tts_values[path->path_attr_num - 1] = result;
        scantuple->tts_isnull[path->path_attr_num - 1] = false;
    }
}

/*
 * Function that handles the case where MERGE is the only clause in the query.
 */
static void process_simple_merge(ExtensiblePlanState *node)
{
    cypher_merge_custom_scan_state *css =
        (cypher_merge_custom_scan_state *)node;
    EState *estate = css->css.ss.ps.state;
    TupleTableSlot *slot;

    /*Process the subtree first */
    age_enter_child_scan(estate, css->child_curcid);
    slot = ExecProcNode(node->ss.ps.lefttree);
    age_leave_child_scan(estate, css->child_curcid);

    if (TupIsNull(slot))
    {
        ExprContext *econtext = node->ss.ps.ps_ExprContext;

        /* setup the scantuple that the process_path needs */
        econtext->ecxt_scantuple = node->ss.ps.lefttree->ps_ResultTupleSlot;
        mark_tts_isnull(econtext->ecxt_scantuple);

        process_path(css, NULL, true);
        mark_scan_slot_valid(econtext->ecxt_scantuple);

        if (css->on_create_set_info != NULL) {
            apply_update_list(node, css->on_create_set_info, true,
                              &css->modified_relids, NULL);
        }
    } else if (css->on_match_set_info != NULL) {
        node->ss.ps.ps_ExprContext->ecxt_scantuple = slot;
        apply_update_list(node, css->on_match_set_info, true,
            &css->modified_relids, NULL);
    }
}

/*
 * Iterate through the TupleTableSlot's tts_values and marks the isnull field
 * with true.
 */
static void mark_tts_isnull(TupleTableSlot *slot)
{
    int numberOfAttributes = slot->tts_tupleDescriptor->natts;
    int i;

    for (i = 0; i < numberOfAttributes; i++)
    {
        Datum val;

        val = slot->tts_values[i];

        if (val == (Datum)NULL)
        {
            slot->tts_isnull[i] = true;
        }
    }
}

static void mark_scan_slot_valid(TupleTableSlot *slot)
{
    slot->tts_flags &= ~TTS_FLAG_EMPTY;
    slot->tts_nvalid = slot->tts_tupleDescriptor->natts;
}

static void free_path_entries(path_entry **path_array, int path_length)
{
    int index;

    if (path_array == NULL)
        return;

    for (index = 0; index < path_length; index++) {
        path_entry *entry = path_array[index];

        if (entry == NULL)
            continue;

        if (!entry->actual && !entry->entity_properties_is_null &&
            (entry->properties_is_null ||
             entry->entity_properties != entry->properties)) {
            pfree(DatumGetPointer(entry->entity_properties));
        }

        if (!entry->actual && !entry->properties_is_null)
            pfree(DatumGetPointer(entry->properties));

        pfree(entry);
    }

    pfree(path_array);
}

/*
 * Evaluate the input-dependent part of a MERGE path without consuming graph
 * identifiers. Later rows can compare this representation with paths created
 * by this executor even when the child join cannot see those new tuples.
 */
static path_entry **prebuild_path(ExtensiblePlanState *node)
{
    cypher_merge_custom_scan_state *css =
        (cypher_merge_custom_scan_state *)node;
    ExprContext *econtext = css->css.ss.ps.ps_ExprContext;
    TupleTableSlot *scan_slot = econtext->ecxt_scantuple;
    ListCell *lc;
    int path_length = list_length(css->path->target_nodes);
    int index = 0;
    path_entry **path_array =
        (path_entry **)palloc0(sizeof(path_entry *) * path_length);

    foreach (lc, css->path->target_nodes)
    {
        cypher_target_node *target = (cypher_target_node *)lfirst(lc);
        path_entry *entry = (path_entry *)palloc0(sizeof(path_entry));

        entry->direction = target->dir;
        entry->label = target->relid;

        if (CYPHER_TARGET_NODE_INSERT_ENTITY(target->flags)) {
            bool is_null = false;
            Datum properties = ExecEvalExpr(target->prop_expr_state, econtext,
                                            &is_null, NULL);

            entry->actual = false;
            entry->id_is_null = true;
            entry->properties_is_null = is_null;
            entry->entity_properties_is_null = is_null;

            if (!is_null) {
                entry->properties = datumCopy(properties, false, -1);
                entry->entity_properties = entry->properties;
                entry->properties_hash =
                    datum_image_hash(entry->properties, false, -1);
            }
        } else {
            agtype *entity;
            agtype_value *entity_value;
            agtype_value *id_value;
            Datum value;

            if (target->tuple_position == InvalidAttrNumber ||
                scan_slot->tts_isnull[target->tuple_position - 1]) {
                ereport(ERROR,
                        (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                         errmsg("Existing variable %s cannot be NULL in MERGE clause",
                                target->variable_name)));
            }

            value = scan_slot->tts_values[target->tuple_position - 1];
            entity = DATUM_GET_AGTYPE_P(value);

            entity_value =
                get_ith_agtype_value_from_container(&entity->root, 0);
            if (entity_value->type != AGTV_VERTEX) {
                ereport(ERROR,
                        (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                         errmsg("agtype must resolve to a vertex")));
            }

            id_value = GET_AGTYPE_VALUE_OBJECT_VALUE(entity_value, "id");

            entry->actual = true;
            entry->id = id_value->val.int_value;
            entry->id_is_null = false;
            entry->properties_is_null = true;

            if (!SAFE_TO_SKIP_EXISTENCE_CHECK(target->flags) &&
                !entity_exists(css->css.ss.ps.state, css->graph_oid,
                               entry->id)) {
                ereport(ERROR,
                        (errcode(ERRCODE_OBJECT_NOT_IN_PREREQUISITE_STATE),
                         errmsg("vertex assigned to variable %s was deleted",
                                target->variable_name)));
            }
        }

        path_array[index++] = entry;
    }

    return path_array;
}

static void sync_created_path_entities(
    cypher_merge_custom_scan_state *css, path_entry **path_array,
    cypher_update_information *set_info)
{
    TupleTableSlot *scan_slot =
        css->css.ss.ps.ps_ExprContext->ecxt_scantuple;
    ListCell *lc;
    int path_length = list_length(css->path->target_nodes);

    foreach (lc, set_info->set_items)
    {
        cypher_update_item *item = (cypher_update_item *)lfirst(lc);
        agtype *entity;
        agtype *properties;
        agtype_value *entity_value;
        agtype_value *id_value;
        agtype_value *properties_value;
        int index;

        if (scan_slot->tts_isnull[item->entity_position - 1])
            continue;

        entity = DATUM_GET_AGTYPE_P(
            scan_slot->tts_values[item->entity_position - 1]);
        entity_value =
            get_ith_agtype_value_from_container(&entity->root, 0);
        id_value = GET_AGTYPE_VALUE_OBJECT_VALUE(entity_value, "id");
        properties_value =
            GET_AGTYPE_VALUE_OBJECT_VALUE(entity_value, "properties");
        properties = agtype_value_to_agtype(properties_value);

        for (index = 0; index < path_length; index++) {
            path_entry *entry = path_array[index];

            if (entry == NULL || entry->actual || entry->id_is_null ||
                entry->id != id_value->val.int_value) {
                continue;
            }

            if (!entry->entity_properties_is_null &&
                (entry->properties_is_null ||
                 entry->entity_properties != entry->properties)) {
                pfree(DatumGetPointer(entry->entity_properties));
            }

            entry->entity_properties = AGTYPE_P_GET_DATUM(properties);
            entry->entity_properties_is_null = false;
            properties = NULL;
            break;
        }

        if (properties != NULL)
            pfree(properties);
    }
}

static bool compare_paths(path_entry **left, path_entry **right,
                          int path_length)
{
    int index;

    for (index = 0; index < path_length; index++) {
        path_entry *left_entry = left[index];
        path_entry *right_entry = right[index];

        if (left_entry->actual != right_entry->actual)
            return false;

        if (left_entry->actual) {
            if (left_entry->id != right_entry->id)
                return false;

            continue;
        }

        if (left_entry->label != right_entry->label ||
            left_entry->direction != right_entry->direction ||
            left_entry->properties_is_null !=
                right_entry->properties_is_null) {
            return false;
        }

        if (left_entry->properties_is_null)
            continue;

        if (left_entry->properties_hash != right_entry->properties_hash ||
            !DatumImageEq(left_entry->properties, right_entry->properties,
                          false, -1)) {
            return false;
        }
    }

    return true;
}

static path_entry **find_duplicate_path(ExtensiblePlanState *node,
                                        path_entry **path_array)
{
    cypher_merge_custom_scan_state *css =
        (cypher_merge_custom_scan_state *)node;
    int path_length = list_length(css->path->target_nodes);
    created_path *current =
        (created_path *)css->created_paths_list;

    while (current != NULL) {
        if (compare_paths(path_array, current->entries, path_length))
            return current->entries;

        current = current->next;
    }

    return NULL;
}

static bool process_merge_input(ExtensiblePlanState *node,
                                TupleTableSlot *slot)
{
    cypher_merge_custom_scan_state *css =
        (cypher_merge_custom_scan_state *)node;
    ExprContext *econtext = css->css.ss.ps.ps_ExprContext;
    bool path_missing;

    tableam_tslot_getallattrs(slot);
    econtext->ecxt_scantuple = slot;
    path_missing = check_path(css, slot);
    if (path_missing) {
        int path_length = list_length(css->path->target_nodes);
        path_entry **prebuilt_path = prebuild_path(node);
        path_entry **duplicate_path =
            find_duplicate_path(node, prebuilt_path);

        if (duplicate_path != NULL) {
            free_path_entries(prebuilt_path, path_length);
            process_path(css, duplicate_path, false);
            mark_scan_slot_valid(slot);

            if (css->on_match_set_info != NULL) {
                apply_update_list(node, css->on_match_set_info, true,
                    &css->modified_relids, NULL);
                sync_created_path_entities(css, duplicate_path,
                    css->on_match_set_info);
            }
        } else {
            created_path *new_path =
                (created_path *)palloc0(sizeof(created_path));

            new_path->next =
                (created_path *)css->created_paths_list;
            new_path->entries = prebuilt_path;
            css->created_paths_list = new_path;

            process_path(css, prebuilt_path, true);
            mark_scan_slot_valid(slot);

            if (css->on_create_set_info != NULL) {
                apply_update_list(node, css->on_create_set_info, true,
                    &css->modified_relids, NULL);
                sync_created_path_entities(css, prebuilt_path,
                    css->on_create_set_info);
            }

            return true;
        }
    } else if (css->on_match_set_info != NULL) {
        apply_update_list(node, css->on_match_set_info, true,
            &css->modified_relids, NULL);
    }

    return false;
}

/*
 * Pull the next input tuple from the child plan. The command id is stepped
 * back while the child runs so that it does not see entities created by this
 * MERGE, then restored for our own writes.
 */
static TupleTableSlot *fetch_merge_input(ExtensiblePlanState *node,
                                         EState *estate)
{
    cypher_merge_custom_scan_state *css =
        (cypher_merge_custom_scan_state *)node;
    TupleTableSlot *slot = NULL;

    age_enter_child_scan(estate, css->child_curcid);
    slot = ExecProcNode(node->ss.ps.lefttree);
    age_leave_child_scan(estate, css->child_curcid);

    return slot;
}

/*
 * Function that is called mid-execution. This function will call
 * its subtree in the execution tree, and depending on the results
 * create the new path, and depending on the the context of the MERGE
 * within the query pass data to the parent execution node.
 *
 * Returns a TupleTableSlot with the next tuple to it parent or
 * Returns NULL when MERGE has no more tuples to emit.
 */
static TupleTableSlot *exec_cypher_merge(ExtensiblePlanState *node)
{
    cypher_merge_custom_scan_state *css =
        (cypher_merge_custom_scan_state *)node;
    EState *estate = css->css.ss.ps.state;
    TupleTableSlot *slot;
    bool terminal = CYPHER_CLAUSE_IS_TERMINAL(css->flags);

    /*
     * There are three cases that dictate the flow of the execution logic.
     *
     * 1. MERGE is not the first clause in the cypher query.
     * 2. MERGE is the first clause and there are no following clauses.
     * 3. MERGE is the first clause and there are following clauses.
     * CYPHER_CLAUSE_FLAG_PREVIOUS_CLAUSE
     */
    if (CYPHER_CLAUSE_HAS_PREVIOUS_CLAUSE(css->flags))
    {
        /*
         * A non-terminal MERGE is an eager pipeline breaker. All child rows
         * must be consumed before a parent MERGE starts reading them.
         */
        if (!terminal && !css->eager_buffer_filled)
        {
            MemoryContext old_context;

            old_context = MemoryContextSwitchTo(estate->es_query_cxt);
            css->eager_tuples = NIL;
            css->eager_tuples_index = 0;
            MemoryContextSwitchTo(old_context);

            slot = fetch_merge_input(node, estate);
            while (!TupIsNull(slot)) {
                TupleTableSlot *projected;
                HeapTuple buffered_tuple;

                process_merge_input(node, slot);
                projected = ExecProject(node->ss.ps.ps_ProjInfo, NULL);

                old_context = MemoryContextSwitchTo(estate->es_query_cxt);
                buffered_tuple = ExecCopySlotTuple(projected);
                css->eager_tuples =
                    lappend(css->eager_tuples, buffered_tuple);
                MemoryContextSwitchTo(old_context);

                slot = fetch_merge_input(node, estate);
            }

            css->eager_buffer_filled = true;

            if (css->parent_merge != NULL)
                refresh_parent_merge_scans(css->parent_merge);
        }

        if (!terminal)
        {
            if (css->eager_tuples_index < list_length(css->eager_tuples))
            {
                HeapTuple buffered_tuple =
                    (HeapTuple)list_nth(css->eager_tuples,
                                        css->eager_tuples_index++);

                return ExecStoreTuple(buffered_tuple,
                                      node->ss.ps.ps_ResultTupleSlot,
                                      InvalidBuffer, false);
            }

            return NULL;
        }

        slot = fetch_merge_input(node, estate);
        while (!TupIsNull(slot)) {
            process_merge_input(node, slot);
            slot = fetch_merge_input(node, estate);
        }

        return NULL;
    }
    else if (terminal)
    {
        /*
         * Case 2: MERGE is the first clause and there are no following clauses
         *
         * For case 2, check to see if we found the pattern, if not create it.
         * Return NULL in either cases, because no rows are expected.
         */
        process_simple_merge(node);

        /*
         * Case 2 always returns NULL the first time exec_cypher_merge is
         * called.
         */
        return NULL;
    }
    else
    {
        /*
         * Case 3: MERGE is the first clause and there are following clauses.
         *
         * Case three has two subcases:
         *
         * 1. The already path exists.
         * 2. The path does not exist.
         */

        /*
         * Part of Case 2.
         *
         * If created_new_path is marked as true. The path did not exist and
         * MERGE created it. We have already passed that information up the
         * execution tree, and now we tell MERGE's parents in the execution
         * tree there is no more tuples to pass.
         */
        if (css->created_new_path)
        {
            /*
             * If the created_new_path is set to true. Then MERGE should not
             * have found a path, because this should only be set to true if
             * merge found sub-case 1
             */
            Assert(css->found_a_path == false);

            return NULL;
        }

        /*
         * Process the subtree. The subtree will only consist of the MERGE
         * path.
         */
        age_enter_child_scan(estate, css->child_curcid);
        slot = ExecProcNode(node->ss.ps.lefttree);
        age_leave_child_scan(estate, css->child_curcid);

        if (!TupIsNull(slot))
        {
            /*
             * Part of Sub-Case 1.
             *
             * If we found a path, mark the found_a_path flag to true and
             * pass the tuple to the next execution tree. When the path
             * exists, we don't need to create/modify anything.
             */
            css->found_a_path = true;

            if (css->on_match_set_info != NULL)
            {
                node->ss.ps.ps_ExprContext->ecxt_scantuple = slot;
                apply_update_list(node, css->on_match_set_info, true,
                    &css->modified_relids, NULL);
            }

            return slot;
        }
        else if (TupIsNull(slot) && css->found_a_path)
        {
            /*
             * Part of Sub-Case 2.
             *
             * MERGE found the path(s) that already exists and we are done passing
             * all the found path(s) up the execution tree.
             */
            return NULL;
        }
        else
        {
            /*
             * Part of Sub-Case 1.
             *
             * MERGE could not find the path in memory and the sub-node in the
             * execution tree returned NULL. We need to create the path and
             * pass the tuple to the next execution node. The subtrees will
             * begin its cleanup process when there are no more tuples found.
             * So we will need to create a TupleTableSlot and populate with the
             * information from the newly created path that the query needs.
             */
            ExprContext *econtext = node->ss.ps.ps_ExprContext;
            ResultState *sss = (ResultState *)node->ss.ps.lefttree;
            HeapTuple heap_tuple;

            /*
             * Our child execution node is always a subquery. If not there
             * is an issue.
             */

            /*
             * found_a_path should only be set to true if MERGE is following
             * sub-case 2.
             */
            Assert(css->found_a_path == false);

            /*
             * This block of sub-case 1 should only be executed once. To
             * create the single path if the path does not exist. If we find
             * ourselves here again, the internal state of the MERGE execution
             * node was incorrectly altered.
             */
            Assert(css->created_new_path == false);

            /*
             *  Postgres cleared the child tuple table slot, we need to remake
             *  it.
             */

            /* setup the scantuple that the process_path needs */
            econtext->ecxt_scantuple = sss->ps.ps_ResultTupleSlot;
            mark_tts_isnull(econtext->ecxt_scantuple);

            // create the path
            process_path(css, NULL, true);
            mark_scan_slot_valid(econtext->ecxt_scantuple);

            if (css->on_create_set_info != NULL)
            {
                apply_update_list(node, css->on_create_set_info, true,
                    &css->modified_relids, NULL);
            }

            // mark the create_new_path flag to true.
            css->created_new_path = true;

            /*
             *  find the tts_values that process_path did not populate and
             *  mark as null.
             */
            mark_tts_isnull(econtext->ecxt_scantuple);

            // create the physical heap tuple
            heap_tuple = heap_form_tuple(
                                econtext->ecxt_scantuple->tts_tupleDescriptor,
                                econtext->ecxt_scantuple->tts_values,
                                econtext->ecxt_scantuple->tts_isnull);

            // store the heap tuple
            ExecStoreTuple(heap_tuple, econtext->ecxt_scantuple, InvalidBuffer, false);

            /*
             * make the subquery's projection scan slot be the tuple table we
             * created and run the projection logic.
             */
            sss->ps.ps_ProjInfo->pi_exprContext->ecxt_scantuple = econtext->ecxt_scantuple;

            // assign this to be our scantuple
            econtext->ecxt_scantuple = ExecProject(node->ss.ps.ps_ProjInfo, NULL);

            /*
             *  run the merge's projection logic and pass to its parent
             *  execution node
             */
            return econtext->ecxt_scantuple;
        }
    }
}
/*
 * Function called at the end of the execution phase to cleanup
 * MERGE.
 */
static void end_cypher_merge(ExtensiblePlanState *node)
{
    cypher_merge_custom_scan_state *css =
        (cypher_merge_custom_scan_state *)node;
    cypher_create_path *path = css->path;
    ListCell *lc;
    int path_length = list_length(path->target_nodes);

    // increment the command counter
    CommandCounterIncrement();

    ExecEndNode(node->ss.ps.lefttree);

    foreach (lc, path->target_nodes)
    {
        cypher_target_node *cypher_node =
            (cypher_target_node *)lfirst(lc);

        if (!CYPHER_TARGET_NODE_INSERT_ENTITY(cypher_node->flags))
            continue;

        // close all indices for the node
        ExecCloseIndices(cypher_node->resultRelInfo);

        // close the relation itself
        heap_close(cypher_node->resultRelInfo->ri_RelationDesc,
                   RowExclusiveLock);
    }

    while (css->created_paths_list != NULL) {
        created_path *current =
            (created_path *)css->created_paths_list;

        css->created_paths_list = current->next;
        free_path_entries(current->entries, path_length);
        pfree(current);
    }

    foreach (lc, css->eager_tuples)
        heap_freetuple((HeapTuple)lfirst(lc));
    list_free(css->eager_tuples);
    css->eager_tuples = NIL;

    if (css->path_values != NIL) {
        list_free(css->path_values);
        css->path_values = NIL;
    }

    notify_modified_entity_relations(&css->modified_relids);
}

/*
 * Rescan is mostly used by join execution nodes, and several others.
 * Since we are creating data here its not safe to rescan the node. Throw
 * an error and try to help the uer understand what went wrong.
 */
static void rescan_cypher_merge(ExtensiblePlanState *node)
{
    ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                    errmsg("cypher merge clause cannot be rescanned"),
                    errhint("its unsafe to use joins in a query with a Cypher MERGE clause")));
}

/*
 * Extracts the metadata information that MERGE needs from the
 * merge_custom_scan node and creates the cypher_merge_custom_scan_state
 * for the execution phase.
 */
Node *create_cypher_merge_plan_state(ExtensiblePlan *cscan)
{
    cypher_merge_custom_scan_state *cypher_css =
       (cypher_merge_custom_scan_state*)palloc0(sizeof(cypher_merge_custom_scan_state));
    cypher_merge_information *merge_information;
    char *serialized_data;
    Const *c;

    cypher_css->cs = cscan;

    // get the serialized data structure from the Const and deserialize it.
    c = (Const*)linitial(cscan->extensible_private);
    serialized_data = (char *)c->constvalue;
    merge_information = (cypher_merge_information*)stringToAGNode(serialized_data);

    Assert(is_ag_node(merge_information, cypher_merge_information));

    cypher_css->merge_information = merge_information;
    cypher_css->flags = merge_information->flags;
    cypher_css->merge_function_attr = merge_information->merge_function_attr;
    cypher_css->path = merge_information->path;
    cypher_css->created_new_path = false;
    cypher_css->found_a_path = false;
    cypher_css->graph_oid = merge_information->graph_oid;
    cypher_css->on_match_set_info = merge_information->on_match_set_info;
    cypher_css->on_create_set_info = merge_information->on_create_set_info;

    cypher_css->css.ss.ps.type = T_ExtensiblePlanState;
    cypher_css->css.methods = &cypher_merge_exec_methods;

    return (Node *)cypher_css;
}

/*
 * Put a newly created entity into the scan tuple at the (1-based) position
 * assigned to its variable so parent execution nodes can reference it.
 */
static void store_merge_variable(TupleTableSlot *scantuple,
                                 int variable_position, Datum value,
                                 const char *caller)
{
    int tuple_position = variable_position - 1;

    if (tuple_position < 0 ||
        tuple_position >= scantuple->tts_tupleDescriptor->natts) {
        ereport(ERROR,
                (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                 errmsg("%s: invalid tuple position", caller)));
    }

    scantuple->tts_values[tuple_position] = value;
    scantuple->tts_isnull[tuple_position] = false;
}

/*
 * Creates the vertex entity, returns the vertex's id in case the caller is
 * the create_edge function.
 */
static Datum merge_vertex(cypher_merge_custom_scan_state *css,
                          cypher_target_node *node, ListCell *next,
                          path_entry **path_array, int path_index,
                          bool should_insert)
{
    bool isNull;
    Datum id;
    EState *estate = css->css.ss.ps.state;
    ExprContext *econtext = css->css.ss.ps.ps_ExprContext;
    ResultRelInfo *resultRelInfo = node->resultRelInfo;
    TupleTableSlot *elemTupleSlot = node->elemTupleSlot;
    TupleTableSlot *scanTupleSlot = econtext->ecxt_scantuple;

    Assert(node->type == LABEL_KIND_VERTEX);

    /*
     * Vertices in a path might already exists. If they do get the id
     * to pass to the edges before and after it. Otherwise, insert the
     * new vertex into it's table and then pass the id along.
     */
    if (CYPHER_TARGET_NODE_INSERT_ENTITY(node->flags))
    {
        ResultRelInfo *old_estate_es_result_relation_info = NULL;
        Datum prop;
        /*
         * Set estate's result relation to the vertex's result
         * relation.
         *
         * Note: This obliterates what was their previously
         */

        /* save the old result relation info */
        old_estate_es_result_relation_info = estate->es_result_relation_info;

        estate->es_result_relation_info = resultRelInfo;

        /* Null-init every attribute before AGE fills id/properties (issue #2450). */
        clear_entity_slot(elemTupleSlot);

        if (!should_insert)
        {
            if (path_array == NULL || path_array[path_index] == NULL)
            {
                ereport(ERROR,
                        (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                         errmsg("invalid MERGE path reuse state")));
            }

            id = GRAPHID_GET_DATUM(path_array[path_index]->id);
            isNull = path_array[path_index]->id_is_null;
        } else {
            id = ExecEvalExpr(node->id_expr_state, econtext, &isNull, NULL);

            if (path_array != NULL && path_array[path_index] != NULL)
            {
                path_array[path_index]->id = DATUM_GET_GRAPHID(id);
                path_array[path_index]->id_is_null = isNull;
            }
        }

        elemTupleSlot->tts_values[vertex_tuple_id] = id;
        elemTupleSlot->tts_isnull[vertex_tuple_id] = isNull;

        if (path_array != NULL && path_array[path_index] != NULL)
        {
            prop = path_array[path_index]->entity_properties;
            isNull = path_array[path_index]->entity_properties_is_null;
        } else {
            prop = ExecEvalExpr(node->prop_expr_state, econtext, &isNull,
                                NULL);
        }

        elemTupleSlot->tts_values[vertex_tuple_properties] = prop;
        elemTupleSlot->tts_isnull[vertex_tuple_properties] = isNull;

        /*
         * Insert the new vertex.
         *
         * Depending on the currentCommandId, we need to do this one of two
         * different ways -
         *
         * 1) If the base_currentCommandId and the currentCommandId are equal,
         *    the currentCommandId hasn't been used for an update, or it hasn't
         *    been incremented after being used. In either case, we need to use
         *    the current one and then increment it so that the following
         *    commands (SET, specifically) will have visibility of this update.
         *
         * 2) If they are not equal, the currentCommandId has been used and/or
         *    updated. In this case, we can't use it. Otherwise our update
         *    won't be visible to anything that follows until the
         *    currentCommandId is updated again. Remember, a tuple is visible
         *    only to commands whose commandId is strictly greater than the
         *    tuple's cmin. So, in this case, we need to use the original
         *    currentCommandId from when begin_cypher_merge was initiated, as
         *    everything under this instance of MERGE needs to be based off of
         *    that initial currentCommandId.
         */
        if (should_insert)
        {
            if (css->base_currentCommandId == GetCurrentCommandId(false))
            {
                insert_entity_tuple(resultRelInfo, elemTupleSlot, estate);

                /*
                 * Increment the currentCommandId since we processed an
                 * update. We don't want to do this outside of this block
                 * because we don't want to inadvertently or unnecessarily
                 * update the commandCounterId of another command.
                 */
                CommandCounterIncrement();
            } else {
                insert_entity_tuple_cid(resultRelInfo, elemTupleSlot, estate,
                                        css->base_currentCommandId);
            }

            mark_entity_relation_modified(
                &css->modified_relids,
                RelationGetRelid(resultRelInfo->ri_RelationDesc));
        }

        /* restore the old result relation info */
        estate->es_result_relation_info = old_estate_es_result_relation_info;

        /*
         * When the vertex is used by clauses higher in the execution tree
         * we need to create a vertex datum. When the vertex is a variable,
         * add to the scantuple slot. When the vertex is part of a path
         * variable, add to the list.
         */
        if (CYPHER_TARGET_NODE_OUTPUT(node->flags))
        {
            Datum result;

            // make the vertex agtype
            result = make_vertex(
                id, string_to_agtype(node->label_name), prop);

            // append to the path list
            if (CYPHER_TARGET_NODE_IN_PATH(node->flags))
            {
                css->path_values = lappend(css->path_values,
                                           DatumGetPointer(result));
            }

            /*
             * Put the vertex in the correct spot in the scantuple, so parent
             * execution nodes can reference the newly created variable.
             */
            if (CYPHER_TARGET_NODE_IS_VARIABLE(node->flags))
            {
                store_merge_variable(scanTupleSlot, node->tuple_position,
                                     result, "merge_vertex");
            }
        }
    }
    else if (path_array != NULL && path_array[path_index] != NULL)
    {
        id = GRAPHID_GET_DATUM(path_array[path_index]->id);

        if (CYPHER_TARGET_NODE_IN_PATH(node->flags))
        {
            Datum vertex =
                scanTupleSlot->tts_values[node->tuple_position - 1];
            css->path_values = lappend(css->path_values,
                                       DatumGetPointer(vertex));
        }
    }
    else
    {
        agtype *a;
        Datum d;
        agtype_value *v;
        agtype_value *id_value;
        TupleTableSlot *scantuple;

        scantuple = econtext->ecxt_scantuple;

        if (scantuple->tts_isnull[node->tuple_position - 1])
        {
            ereport(ERROR,
                (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                 errmsg("Existing variable %s cannot be NULL in MERGE clause",
                 node->variable_name)));
        }

        // get the vertex agtype in the scanTupleSlot
        d = scantuple->tts_values[node->tuple_position - 1];
        a = DATUM_GET_AGTYPE_P(d);

        // Convert to an agtype value
        v = get_ith_agtype_value_from_container(&a->root, 0);

        if (v->type != AGTV_VERTEX)
            ereport(ERROR,
                    (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                     errmsg("agtype must resolve to a vertex")));

        // extract the id agtype field
        id_value = GET_AGTYPE_VALUE_OBJECT_VALUE(v, "id");

        // extract the graphid and cast to a Datum
        id = GRAPHID_GET_DATUM(id_value->val.int_value);

        /*
         * Its possible the variable has already been deleted. There are two
         * ways this can happen. One is the query explicitly deleted the
         * variable, the is_deleted flag will catch that. However, it is
         * possible the user deleted the vertex using another variable name. We
         * need to scan the table to find the vertex's current status relative
         * to this CREATE clause. If the variable was initially created in this
         * clause, we can skip this check, because the transaction system
         * guarantees that nothing can happen to that tuple, as far as we are
         * concerned with at this time.
         */
        if (!SAFE_TO_SKIP_EXISTENCE_CHECK(node->flags))
        {
            if (!entity_exists(estate, css->graph_oid, DATUM_GET_GRAPHID(id)))
            {
                ereport(ERROR,
                    (errcode(ERRCODE_OBJECT_NOT_IN_PREREQUISITE_STATE),
                     errmsg("vertex assigned to variable %s was deleted",
                            node->variable_name)));
            }
        }

        // add the Datum to the list of entities for creating the path variable
        if (CYPHER_TARGET_NODE_IN_PATH(node->flags))
        {
            Datum vertex = scanTupleSlot->tts_values[node->tuple_position - 1];
            css->path_values = lappend(css->path_values,
                                       DatumGetPointer(vertex));
        }
    }

    // If the path continues, create the next edge, passing the vertex's id.
    if (next != NULL)
    {
        merge_edge(css, (cypher_target_node *)lfirst(next), id, lnext(next),
                   path_array, path_index + 1, should_insert);
    }

    return id;
}

/*
 * Create the edge entity.
 */
static void merge_edge(cypher_merge_custom_scan_state *css,
                       cypher_target_node *node, Datum prev_vertex_id,
                       ListCell *next, path_entry **path_array, int path_index,
                       bool should_insert)
{
    bool isNull;
    EState *estate = css->css.ss.ps.state;
    ExprContext *econtext = css->css.ss.ps.ps_ExprContext;
    ResultRelInfo *resultRelInfo = node->resultRelInfo;
    ResultRelInfo *old_estate_es_result_relation_info = NULL;
    TupleTableSlot *elemTupleSlot = node->elemTupleSlot;
    Datum id;
    Datum start_id = (Datum)0;
    Datum end_id = (Datum)0;
    Datum next_vertex_id;
    List *prev_path = css->path_values;
    Datum prop;

    Assert(node->type == LABEL_KIND_EDGE);
    Assert(lfirst(next) != NULL);

    /*
     * Create the next vertex before creating the edge. We need the
     * next vertex's id.
     */
    css->path_values = NIL;
    next_vertex_id = merge_vertex(css,
                                  (cypher_target_node *)lfirst(next),
                                  lnext(next), path_array, path_index + 1,
                                  should_insert);

    /*
     * Set the start and end vertex ids
     */
    if (node->dir == CYPHER_REL_DIR_RIGHT || node->dir == CYPHER_REL_DIR_NONE)
    {
        // create pattern (prev_vertex)-[edge]->(next_vertex)
        start_id = prev_vertex_id;
        end_id = next_vertex_id;
    }
    else if (node->dir == CYPHER_REL_DIR_LEFT)
    {
        // create pattern (prev_vertex)<-[edge]-(next_vertex)
        start_id = next_vertex_id;
        end_id = prev_vertex_id;
    }
    else
    {
        ereport(ERROR,
                (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                 errmsg("edge direction must be specified in a MERGE clause")));
    }

    /*
     * Set estate's result relation to the vertex's result
     * relation.
     *
     * Note: This obliterates what was their previously
     */

    /* save the old result relation info */
    old_estate_es_result_relation_info = estate->es_result_relation_info;

    estate->es_result_relation_info = resultRelInfo;

    /* Null-init every attribute before AGE fills the edge columns (issue #2450). */
    clear_entity_slot(elemTupleSlot);

    if (!should_insert)
    {
        if (path_array == NULL || path_array[path_index] == NULL)
        {
            ereport(ERROR,
                    (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                     errmsg("invalid MERGE path reuse state")));
        }

        id = GRAPHID_GET_DATUM(path_array[path_index]->id);
        isNull = path_array[path_index]->id_is_null;
        start_id = GRAPHID_GET_DATUM(path_array[path_index]->start_id);
        end_id = GRAPHID_GET_DATUM(path_array[path_index]->end_id);
    }
    else
    {
        id = ExecEvalExpr(node->id_expr_state, econtext, &isNull, NULL);

        if (path_array != NULL && path_array[path_index] != NULL)
        {
            path_array[path_index]->id = DATUM_GET_GRAPHID(id);
            path_array[path_index]->id_is_null = isNull;
            path_array[path_index]->start_id = DATUM_GET_GRAPHID(start_id);
            path_array[path_index]->end_id = DATUM_GET_GRAPHID(end_id);
        }
    }

    elemTupleSlot->tts_values[edge_tuple_id] = id;
    elemTupleSlot->tts_isnull[edge_tuple_id] = isNull;

    elemTupleSlot->tts_values[edge_tuple_start_id] = start_id;
    elemTupleSlot->tts_isnull[edge_tuple_start_id] = false;
    elemTupleSlot->tts_values[edge_tuple_end_id] = end_id;
    elemTupleSlot->tts_isnull[edge_tuple_end_id] = false;

    if (path_array != NULL && path_array[path_index] != NULL)
    {
        prop = path_array[path_index]->entity_properties;
        isNull = path_array[path_index]->entity_properties_is_null;
    }
    else
    {
        prop = ExecEvalExpr(node->prop_expr_state, econtext, &isNull, NULL);
    }

    elemTupleSlot->tts_values[edge_tuple_properties] = prop;
    elemTupleSlot->tts_isnull[edge_tuple_properties] = isNull;

    /*
     * Insert the new edge. See the comment in merge_vertex for why the cid
     * used for the insert depends on the base_currentCommandId (upstream
     * commits 99e7c625d9 and e481556ee1: MERGE visibility in chained
     * commands, SET specifically).
     */
    if (should_insert)
    {
        if (css->base_currentCommandId == GetCurrentCommandId(false))
        {
            insert_entity_tuple(resultRelInfo, elemTupleSlot, estate);

            /*
             * Increment the currentCommandId since we processed an update.
             * We don't want to do this outside of this block because we
             * don't want to inadvertently or unnecessarily update the
             * commandCounterId of another command.
             */
            CommandCounterIncrement();
        } else {
            insert_entity_tuple_cid(resultRelInfo, elemTupleSlot, estate,
                                    css->base_currentCommandId);
        }

        mark_entity_relation_modified(
            &css->modified_relids,
            RelationGetRelid(resultRelInfo->ri_RelationDesc));
    }

    /* restore the old result relation info */
    estate->es_result_relation_info = old_estate_es_result_relation_info;

    /*
     * When the edge is used by clauses higher in the execution tree
     * we need to create an edge datum. When the edge is a variable,
     * add to the scantuple slot. When the edge is part of a path
     * variable, add to the list.
     */
    if (CYPHER_TARGET_NODE_OUTPUT(node->flags))
    {
        Datum result;

        result = make_edge(
            id, start_id, end_id, string_to_agtype(node->label_name), prop);

        // add the Datum to the list of entities for creating the path variable
        if (CYPHER_TARGET_NODE_IN_PATH(node->flags))
        {
            prev_path = lappend(prev_path, DatumGetPointer(result));
            css->path_values = list_concat(prev_path, css->path_values);
        }

        // Add the entity to the TupleTableSlot if necessary
        if (CYPHER_TARGET_NODE_IS_VARIABLE(node->flags))
        {
            store_merge_variable(econtext->ecxt_scantuple, node->tuple_position,
                                 result, "merge_edge");
        }
    }
}
