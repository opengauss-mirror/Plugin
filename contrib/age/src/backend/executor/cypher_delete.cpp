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

#include "access/sysattr.h"
#include "access/htup.h"
#include "access/multixact.h"
#include "access/xact.h"
#include "storage/buf/bufmgr.h"
#include "executor/tuptable.h"
#include "nodes/execnodes.h"
#include "nodes/ag_extensible.h"
#include "nodes/nodes.h"
#include "nodes/plannodes.h"
#include "parser/parsetree.h"
#include "parser/parse_relation.h"
#include "parser/scansup.h"
#include "rewrite/rewriteHandler.h"
#include "utils/builtins.h"
#include "utils/rel.h"
#include "utils/acl.h"
#include "executor/executor.h"

#include "catalog/ag_label.h"
#include "commands/label_commands.h"
#include "executor/cypher_executor.h"
#include "executor/cypher_utils.h"
#include "parser/cypher_parse_node.h"
#include "nodes/cypher_nodes.h"
#include "utils/agtype.h"
#include "utils/graphid.h"

static void begin_cypher_delete(ExtensiblePlanState *node, EState *estate,
                                int eflags);

static TupleTableSlot *exec_cypher_delete(ExtensiblePlanState *node);
static void end_cypher_delete(ExtensiblePlanState *node);
static void rescan_cypher_delete(ExtensiblePlanState *node);

static void process_delete_list(ExtensiblePlanState *node);

#define ENTITY_HASH_INITIAL_SIZE 32

static void find_connected_edges(ExtensiblePlanState *node, char *graph_name,
                                 List *labels, char *var_name, graphid id,
                                 bool detach_delete);
static List *collect_connected_edges_by_index(
    Relation relation, Snapshot snapshot, Oid index_oid,
    AttrNumber heap_attnum, graphid vertex_id, bool skip_self_loops);
static agtype_value *extract_entity(ExtensiblePlanState *node,
                                    TupleTableSlot *scanTupleSlot,
                                    int entity_position);
static bool delete_entity_tuple(EState *estate, ResultRelInfo *resultRelInfo,
                                HeapTuple tuple);

typedef struct deleted_entity_key {
    Oid relation_oid;
    graphid entity_id;
} deleted_entity_key;

static bool entity_delete_is_registered(HTAB *deleted_entities,
                                        Oid relation_oid, graphid entity_id);
static void register_entity_delete(HTAB *deleted_entities, Oid relation_oid,
                                   graphid entity_id);

const ExtensibleExecMethods cypher_delete_exec_methods = {DELETE_SCAN_STATE_NAME,
    begin_cypher_delete,
    exec_cypher_delete,
    end_cypher_delete,
    rescan_cypher_delete,
    NULL};

/*
 * Initialization at the beginning of execution. Setup the child node,
 * setup its scan tuple slot and projection infp, expression context,
 * collect metadata about visible edges, and alter the commandid for
 * the transaction.
 */
static void begin_cypher_delete(ExtensiblePlanState *node, EState *estate,
                                int eflags)
{
    cypher_delete_custom_scan_state *css =
        (cypher_delete_custom_scan_state *)node;
    Plan *subplan;
    HASHCTL hash_control;

    Assert(list_length(css->cs->extensible_plans) == 1);

    MemSet(&hash_control, 0, sizeof(hash_control));
    hash_control.keysize = sizeof(deleted_entity_key);
    hash_control.entrysize = sizeof(deleted_entity_key);
    css->deleted_entities =
        hash_create("cypher DELETE deleted entities", ENTITY_HASH_INITIAL_SIZE, &hash_control,
                    HASH_ELEM | HASH_BLOBS);

    // setup child
    subplan = (Plan*)linitial(css->cs->extensible_plans);
    node->ss.ps.lefttree = ExecInitNode(subplan, estate, eflags);

    // setup expr context
    ExecAssignExprContext(estate, &node->ss.ps);

    // setup scan tuple slot and projection info
    TupleDesc tupledesc = ExecGetResultType(node->ss.ps.lefttree);
    ExecInitScanTupleSlot(estate, &node->ss);
    ExecAssignScanType(&node->ss, tupledesc);

    if (!CYPHER_CLAUSE_IS_TERMINAL(css->flags))
    {
        TupleDesc tupdesc = node->ss.ss_ScanTupleSlot->tts_tupleDescriptor;

        ExecAssignProjectionInfo(&node->ss.ps, tupdesc);
    }

    /*
     * Get all the labels that are visible to this delete clause at this point
     * in the transaction. To be used later when the delete clause finds
     * vertices.
     */
    css->edge_labels = get_all_edge_labels_per_graph(estate, css->delete_data->graph_oid);

    /*
     * Postgres does not assign the es_output_cid in queries that do
     * not write to disk, ie: SELECT commands. We need the command id
     * for our clauses, and we may need to initialize it. We cannot use
     * GetCurrentCommandId because there may be other cypher clauses
     * that have modified the command id.
     */
    if (estate->es_output_cid == 0)
        estate->es_output_cid = estate->es_snapshot->curcid;

    /* the child subtree must keep seeing the state before this clause */
    css->child_curcid = estate->es_snapshot->curcid;

    Increment_Estate_CommandId(estate);
}

/*
 * Called once per tuple. If this is a terminal DELETE clause
 * process everyone of its child tuple, otherwise process the
 * next tuple.
 */
static TupleTableSlot *exec_cypher_delete(ExtensiblePlanState *node)
{
    cypher_delete_custom_scan_state *css =
        (cypher_delete_custom_scan_state *)node;
    EState *estate = css->css.ss.ps.state;
    ExprContext *econtext = css->css.ss.ps.ps_ExprContext;
    TupleTableSlot *slot;

    if (CYPHER_CLAUSE_IS_TERMINAL(css->flags))
    {
        /*
         * If the DELETE clause was the final cypher clause written
         * then we aren't returning anything from this result node.
         * So the exec_cypher_delete function will only be called once.
         * Therefore we will process all tuples from the subtree at once.
         */
        while(true)
        {
            //Process the subtree first
            age_enter_child_scan(estate, css->child_curcid);
            slot = ExecProcNode(node->ss.ps.lefttree);
            age_leave_child_scan(estate, css->child_curcid);

            if (TupIsNull(slot)) {
                break;
            }

            // setup the scantuple that the process_delete_list needs
            econtext->ecxt_scantuple = slot;

            process_delete_list(node);
        }

        return NULL;
    }
    else
    {
        //Process the subtree first
        age_enter_child_scan(estate, css->child_curcid);
        slot = ExecProcNode(node->ss.ps.lefttree);
        age_leave_child_scan(estate, css->child_curcid);

        if (TupIsNull(slot))
            return NULL;

        // setup the scantuple that the process_delete_list needs
        econtext->ecxt_scantuple = slot;
        process_delete_list(node);

        econtext->ecxt_scantuple =
            ExecProject(node->ss.ps.lefttree->ps_ProjInfo, NULL);

        return  econtext->ecxt_scantuple;
    }
}

/*
 * Called at the end of execution. Tell its child to
 * end its execution.
 */
static void end_cypher_delete(ExtensiblePlanState *node)
{
    cypher_delete_custom_scan_state *css =
        (cypher_delete_custom_scan_state *)node;

    ExecEndNode(node->ss.ps.lefttree);
    notify_modified_entity_relations(&css->modified_relids);
    hash_destroy(css->deleted_entities);
    css->deleted_entities = NULL;
}

/*
 * Used for rewinding the scan state and reprocessing the results.
 *
 * XXX: This is not currently supported. We need to find out
 * when this function will be called and determine a process
 * for allowing the Delete clause to run multiple times without
 * redundant edits to the database.
 */
static void rescan_cypher_delete(ExtensiblePlanState *node)
{
     ereport(ERROR,
             (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                      errmsg("cypher DELETE clause cannot be rescanned"),
                      errhint("its unsafe to use joins in a query with a Cypher DELETE clause")));
}

/*
 * Create the CustomScanState from the CustomScan and pass
 * necessary metadata.
 */
Node *create_cypher_delete_plan_state(ExtensiblePlan *cscan)
{
    cypher_delete_custom_scan_state *cypher_css =
       (cypher_delete_custom_scan_state*)palloc0(sizeof(cypher_delete_custom_scan_state));
    cypher_delete_information *delete_data;
    char *serialized_data;
    Const *c;

    cypher_css->cs = cscan;

    // get the serialized data structure from the Const and deserialize it.
    c = (Const*)linitial(cscan->extensible_private);
    serialized_data = (char *)c->constvalue;
    delete_data = (cypher_delete_information*)stringToAGNode(serialized_data);

    Assert(is_ag_node(delete_data, cypher_delete_information));

    cypher_css->delete_data = delete_data;
    cypher_css->flags = delete_data->flags;

    cypher_css->css.ss.ps.type = T_ExtensiblePlanState;
    cypher_css->css.methods = &cypher_delete_exec_methods;

    return (Node *)cypher_css;
}

/*
 * Extract the vertex or edge to be deleted, perform some type checking to
 * validate datum is an agtype vertex or edge.
 */
static agtype_value *extract_entity(ExtensiblePlanState *node,
                                    TupleTableSlot *scanTupleSlot,
                                    int entity_position)
{
    agtype_value *original_entity_value;
    agtype *original_entity;
    TupleDesc tupleDescriptor;

    tupleDescriptor = scanTupleSlot->tts_tupleDescriptor;

    // type checking, make sure the entity is an agtype vertex or edge
    if (tupleDescriptor->attrs[entity_position -1].atttypid != AGTYPEOID)
        ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                errmsg("DELETE clause can only delete agtype")));

    original_entity = DATUM_GET_AGTYPE_P(scanTupleSlot->tts_values[entity_position - 1]);
    original_entity_value = get_ith_agtype_value_from_container(&original_entity->root, 0);

    if (original_entity_value->type != AGTV_VERTEX && original_entity_value->type != AGTV_EDGE)
        ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                errmsg("DELETE clause can only delete vertices and edges")));

    return original_entity_value;
}

/*
 * Delete the given row of the label relation of resultRelInfo through the
 * heap, the way the Cypher CREATE clause inserts.  Returns false when the row
 * is already gone (deleted earlier in this query or by a committed concurrent
 * transaction).
 */
static bool delete_entity_tuple(EState *estate, ResultRelInfo *resultRelInfo,
                                HeapTuple tuple)
{
    Relation relation = resultRelInfo->ri_RelationDesc;
    TM_FailureData tmfd;
    TM_Result result;

    check_entity_write_allowed(relation, ACL_DELETE, "DELETE");

    result = heap_delete(relation, &tuple->t_self, GetCurrentCommandId(true),
                         estate->es_crosscheck_snapshot, true, &tmfd);
    switch (result) {
        case TM_Ok:
            age_note_write(estate);
            return true;
        case TM_SelfModified:
        case TM_Deleted:
        case TM_Invisible:
            /* already deleted by this command or a concurrent transaction */
            return false;
        case TM_Updated:
            ereport(ERROR,
                    (errcode(ERRCODE_T_R_SERIALIZATION_FAILURE),
                     errmsg("could not serialize access due to concurrent update")));
            break;
        default:
            ereport(ERROR, (errcode(ERRCODE_INTERNAL_ERROR),
                            errmsg("Entity failed to be deleted: %i", result)));
    }

    return false;
}

static bool entity_delete_is_registered(HTAB *deleted_entities,
                                        Oid relation_oid, graphid entity_id)
{
    deleted_entity_key key;

    MemSet(&key, 0, sizeof(key));
    key.relation_oid = relation_oid;
    key.entity_id = entity_id;

    return hash_search(deleted_entities, &key, HASH_FIND, NULL) != NULL;
}

static void register_entity_delete(HTAB *deleted_entities, Oid relation_oid,
                                   graphid entity_id)
{
    deleted_entity_key key;
    bool found;

    Assert(deleted_entities != NULL);

    MemSet(&key, 0, sizeof(key));
    key.relation_oid = relation_oid;
    key.entity_id = entity_id;
    (void)hash_search(deleted_entities, &key, HASH_ENTER, &found);

    Assert(!found);
}

/*
 * After the delete's subtress has been processed, we then go through the list
 * of variables to be deleted.
 */
static void process_delete_list(ExtensiblePlanState *node)
{
    cypher_delete_custom_scan_state *css =
        (cypher_delete_custom_scan_state *)node;
    ListCell *lc;
    ExprContext *econtext = css->css.ss.ps.ps_ExprContext;
    TupleTableSlot *scanTupleSlot = econtext->ecxt_scantuple;
    EState *estate = node->ss.ps.state;

    foreach(lc, css->delete_data->delete_items)
    {
        cypher_delete_item *item;
        agtype_value *original_entity_value, *id, *label;
        AgeBtreeEqScan *scan_desc;
        ResultRelInfo *resultRelInfo;
        HeapTuple heap_tuple;
        HeapTuple original_tuple;
        Relation relation;
        AttrNumber id_attnum;
        char *label_name;
        Value *pos;
        int entity_position;

        item = (cypher_delete_item*)lfirst(lc);

        pos = item->entity_position;
        entity_position = pos->val.ival;

        /* skip if the entity is null */
        if (scanTupleSlot->tts_isnull[entity_position - 1])
            continue;

        original_entity_value = extract_entity(node, scanTupleSlot,
                                               entity_position);

        id = GET_AGTYPE_VALUE_OBJECT_VALUE(original_entity_value, "id");
        label = GET_AGTYPE_VALUE_OBJECT_VALUE(original_entity_value, "label");
        label_name = pnstrdup(label->val.string.val, label->val.string.len);

        resultRelInfo = create_entity_result_rel_info(estate, css->delete_data->graph_name, label_name);
        relation = resultRelInfo->ri_RelationDesc;

        /*
         * Check the target label before inspecting tuple or adjacency state.
         * Otherwise a user without DELETE privilege could learn whether a
         * vertex has connected edges from the error raised below.
         */
        AclResult acl_result = pg_class_aclcheck(
            RelationGetRelid(relation), GetUserId(), ACL_DELETE);
        if (acl_result != ACLCHECK_OK)
            aclcheck_error(acl_result, ACL_KIND_CLASS,
                           RelationGetRelationName(relation));

        /*
         * Select the on-disk id attribute for the entity type.
         */
        if (original_entity_value->type == AGTV_VERTEX)
        {
            id_attnum = Anum_ag_label_vertex_table_id;
        }
        else if (original_entity_value->type == AGTV_EDGE)
        {
            id_attnum = Anum_ag_label_edge_table_id;
        }
        else
        {
            ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                    errmsg("DELETE clause can only delete vertices and edges")));
        }

        scan_desc = age_btree_eq_beginscan(
            relation, estate->es_snapshot, id_attnum, F_GRAPHIDEQ,
            GRAPHID_GET_DATUM(id->val.int_value), RowExclusiveLock);
        heap_tuple = age_btree_eq_getnext(scan_desc);
        original_tuple = HeapTupleIsValid(heap_tuple)
                             ? heap_copytuple(heap_tuple)
                             : NULL;
        age_btree_eq_endscan(scan_desc);

        /*
         * If the heap tuple still exists (It wasn't deleted after this variable
         * was created) we can delete it. Otherwise, it is safe to skip this
         * delete. The copied tuple outlives the closed lookup scan.
         */
        if (!HeapTupleIsValid(original_tuple))
        {
            destroy_entity_result_rel_info(resultRelInfo);

            continue;
        }

        /*
         * Delete the target before touching connected edges. If RLS hides the
         * target row, no edge may be removed. For a plain DELETE with connected
         * edges, or for a DETACH DELETE that later fails, the raised ERROR rolls
         * the target deletion back with the rest of the statement.
         */
        if (!delete_entity_tuple(estate, resultRelInfo, original_tuple))
        {
            heap_freetuple(original_tuple);
            destroy_entity_result_rel_info(resultRelInfo);
            continue;
        }

        register_entity_delete(css->deleted_entities,
                               RelationGetRelid(relation),
                               id->val.int_value);
        mark_entity_relation_modified(
            &css->modified_relids, RelationGetRelid(relation));

        /*
         * For vertices, check for connected edges after the target row has
         * passed ACL and RLS enforcement. Statement rollback preserves the
         * vertex when a plain DELETE or an edge deletion raises an error.
         */
        if (original_entity_value->type == AGTV_VERTEX)
        {
            find_connected_edges(node, css->delete_data->graph_name,
                                 css->edge_labels, item->var_name,
                                 id->val.int_value, css->delete_data->detach);
        }

        heap_freetuple(original_tuple);
        destroy_entity_result_rel_info(resultRelInfo);
    }
}

static List *collect_connected_edges_by_index(
    Relation relation, Snapshot snapshot, Oid index_oid,
    AttrNumber heap_attnum, graphid vertex_id, bool skip_self_loops)
{
    AgeBtreeEqScan *scan;
    HeapTuple tuple;
    List *tuples = NIL;

    scan = age_btree_eq_beginscan_with_index(
        relation, snapshot, heap_attnum, F_GRAPHIDEQ,
        GRAPHID_GET_DATUM(vertex_id), RowExclusiveLock, index_oid);

    while (HeapTupleIsValid(tuple = age_btree_eq_getnext(scan))) {
        if (skip_self_loops) {
            bool is_null = false;
            Datum start_id = heap_getattr(
                tuple, Anum_ag_label_edge_table_start_id,
                RelationGetDescr(relation), &is_null);
            if (!is_null && DATUM_GET_GRAPHID(start_id) == vertex_id) {
                continue;
            }
        }

        tuples = lappend(tuples, heap_copytuple(tuple));
    }

    age_btree_eq_endscan(scan);

    return tuples;
}

/*
 * Find the edges connected to the given node. If there is any edges either
 * delete them or throw an error, depending on the detach delete option.
 */
static void find_connected_edges(ExtensiblePlanState *node, char *graph_name,
                                 List *labels, char *var_name, graphid id,
                                 bool detach_delete)
{
    cypher_delete_custom_scan_state *css =
        (cypher_delete_custom_scan_state *)node;
    EState *estate = css->css.ss.ps.state;
    ListCell *lc;

    /*
     * es_snapshot->curcid already points past the vertex deletion (see
     * age_note_write()), so the edge scans below see every edge that is
     * still alive and none of the ones this clause deleted before.
     */

    /*
     * We need to scan through all the edges to see if this vertex has
     * any edges attached to it.
     *
     * XXX: If we implement an on-disc graph storage system. Such as
     * an adjacency matrix, the performace of this check can be massively
     * improved. However, right now we have to scan every edge to see if
     * one has this vertex as a start or end vertex.
     */
    foreach(lc, labels)
    {
        char *label_name = (char*)lfirst(lc);
        ResultRelInfo *resultRelInfo;
        Relation relation;
        Oid start_index_oid;
        Oid end_index_oid;
        List *tuples = NIL;
        ListCell *tuple_cell;

        resultRelInfo = create_entity_result_rel_info(estate,
                                                      graph_name, label_name);
        relation = resultRelInfo->ri_RelationDesc;
        start_index_oid = find_usable_btree_index_for_attr(
            relation, Anum_ag_label_edge_table_start_id);
        end_index_oid = find_usable_btree_index_for_attr(
            relation, Anum_ag_label_edge_table_end_id);
        if (OidIsValid(start_index_oid) && OidIsValid(end_index_oid))
        {
            tuples = collect_connected_edges_by_index(
                relation, estate->es_snapshot, start_index_oid,
                Anum_ag_label_edge_table_start_id, id, false);
            tuples = list_concat(
                tuples,
                collect_connected_edges_by_index(
                    relation, estate->es_snapshot, end_index_oid,
                    Anum_ag_label_edge_table_end_id, id, true));
        } else {
            TableScanDesc scan_desc;
            HeapTuple tuple;

            scan_desc = heap_beginscan(relation, estate->es_snapshot, 0, NULL);
            while (HeapTupleIsValid(
                tuple = heap_getnext(scan_desc, ForwardScanDirection)))
            {
                bool start_is_null = false;
                bool end_is_null = false;
                Datum start_id = heap_getattr(
                    tuple, Anum_ag_label_edge_table_start_id,
                    RelationGetDescr(relation), &start_is_null);
                Datum end_id = heap_getattr(
                    tuple, Anum_ag_label_edge_table_end_id,
                    RelationGetDescr(relation), &end_is_null);
                if ((!start_is_null && DATUM_GET_GRAPHID(start_id) == id) ||
                    (!end_is_null && DATUM_GET_GRAPHID(end_id) == id)) {
                    tuples = lappend(tuples, heap_copytuple(tuple));
                }
            }
            heap_endscan(scan_desc);
        }

        if (tuples != NIL && !detach_delete)
        {
            ereport(ERROR,
                    (errcode(ERRCODE_INTERNAL_ERROR),
                     errmsg("Cannot delete vertex %s, because it still has edges attached. "
                            "To delete this vertex, you must first delete the attached edges.",
                            var_name)));
        }

        foreach (tuple_cell, tuples)
        {
            HeapTuple tuple = (HeapTuple)lfirst(tuple_cell);
            bool is_null = false;
            Datum edge_id = heap_getattr(
                tuple, Anum_ag_label_edge_table_id,
                RelationGetDescr(relation), &is_null);

            if (is_null)
                ereport(ERROR,
                        (errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED),
                         errmsg("edge row contains a null id")));

            graphid connected_edge_id = DATUM_GET_GRAPHID(edge_id);
            Oid relation_oid = RelationGetRelid(relation);
            if (entity_delete_is_registered(
                css->deleted_entities, relation_oid, connected_edge_id))
            {
                heap_freetuple(tuple);
                continue;
            }

            if (!delete_entity_tuple(estate, resultRelInfo, tuple))
            {
                /* removed concurrently since the scan above; nothing to do */
                heap_freetuple(tuple);
                continue;
            }

            register_entity_delete(css->deleted_entities, relation_oid,
                                   connected_edge_id);
            mark_entity_relation_modified(
                &css->modified_relids, relation_oid);
            heap_freetuple(tuple);
        }
        list_free(tuples);
        destroy_entity_result_rel_info(resultRelInfo);
    }
}
