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

#ifndef AG_CYPHER_UTILS_H
#define AG_CYPHER_UTILS_H

#include "access/genam.h"
#include "access/xact.h"
#include "utils/acl.h"
#include "nodes/execnodes.h"
#include "nodes/nodes.h"
#include "nodes/plannodes.h"
#include "utils/hsearch.h"
#include "utils/rel.h"

#include "nodes/cypher_nodes.h"
#include "utils/agtype.h"

// declaration of a useful postgres macro that isn't in a header file
#define DatumGetItemPointer(X)	 ((ItemPointer) DatumGetPointer(X))
#define ItemPointerGetDatum(X)	 PointerGetDatum(X)

/*
 * When executing the children of the CREATE, SET, REMOVE, and
 * DELETE clasues, we need to alter the command id in the estate
 * and the snapshot. That way we can hide the modified tuples from
 * the sub clauses that should not know what their parent clauses are
 * doing.
 */
#define Increment_Estate_CommandId(estate) \
    estate->es_output_cid++; \
    estate->es_snapshot->curcid++;

#define Decrement_Estate_CommandId(estate) \
    estate->es_output_cid--; \
    estate->es_snapshot->curcid--;

/*
 * The write clauses advance the command counter after their writes so that
 * later clauses of the same query observe them (openCypher semantics).  The
 * fixed +1/-1 arithmetic above cannot describe that, so every write clause
 * remembers the command id its child subtree has to run under (the state
 * before the clause) and, after its own writes, exposes the newest command id
 * to everything above it.
 */
static inline void age_enter_child_scan(EState *estate, CommandId child_curcid)
{
    estate->es_snapshot->curcid = child_curcid;
}

static inline void age_leave_child_scan(EState *estate, CommandId child_curcid)
{
    CommandId latest = GetCurrentCommandId(false);

    estate->es_snapshot->curcid = Max(child_curcid + 1, latest);
}

/*
 * Called after a heap write: make the row change visible to every scan that
 * still runs under estate->es_snapshot (later clauses, the RETURN).
 */
static inline void age_note_write(EState *estate)
{
    CommandCounterIncrement();
    estate->es_snapshot->curcid = GetCurrentCommandId(false);
}

/* privilege / RLS / read-only checks shared by the heap write paths */
void check_entity_write_allowed(Relation relation, AclMode mode,
                                const char *clause_name);

typedef struct cypher_create_custom_scan_state
{
    ExtensiblePlanState css;

    ExtensiblePlan *cs;

    List *pattern;
    List *path_values;
    uint32 flags;
    TupleTableSlot *slot;
    Oid graph_oid;
    List *modified_relids;
    /* command id the child subtree runs under (state before this clause) */
    CommandId child_curcid;
} cypher_create_custom_scan_state;

typedef struct cypher_set_custom_scan_state
{
    ExtensiblePlanState css;

    ExtensiblePlan *cs;

    cypher_update_information *set_list;
    int flags;
    List *modified_relids;
    HTAB *updated_entities;
    /* command id the child subtree runs under (state before this clause) */
    CommandId child_curcid;
} cypher_set_custom_scan_state;

typedef struct cypher_delete_custom_scan_state
{
    ExtensiblePlanState css;

    ExtensiblePlan *cs;

    cypher_delete_information *delete_data;
    int flags;
    List *edge_labels;
    List *modified_relids;
    HTAB *deleted_entities;
    /* command id the child subtree runs under (state before this clause) */
    CommandId child_curcid;
} cypher_delete_custom_scan_state;

typedef struct cypher_merge_custom_scan_state
{
    ExtensiblePlanState css;

    ExtensiblePlan *cs;

    cypher_merge_information *merge_information;
    int flags;
    cypher_create_path *path;
    List *path_values;
    Oid graph_oid;
    AttrNumber merge_function_attr;
    bool created_new_path;
    List *modified_relids;
    bool found_a_path;
    struct created_path *created_paths_list;
    struct cypher_merge_custom_scan_state *parent_merge;
    /* command id the child subtree runs under (state before this clause) */
    CommandId child_curcid;
    List *eager_tuples;
    int eager_tuples_index;
    bool eager_buffer_filled;
    cypher_update_information *on_match_set_info;
    cypher_update_information *on_create_set_info;
    CommandId base_currentCommandId;
} cypher_merge_custom_scan_state;

typedef struct cypher_vle_custom_scan_state
{
    ExtensiblePlanState css;

    ExtensiblePlan *cs;

	TupleTableSlot *slot;

    Oid graph_oid;
    char * label_name;

	/* For Subplan tuples */
	PlanState  *subplan;
	TupleTableSlot *subplan_tuple;
	bool		need_new_sp_tuple;

	/* target edges. */
	ResultRelInfo *target_rel_infos;
	TupleTableSlot *current_scan_tuple;
	int			num_target_rel_info;
	Oid		   *start_id_index_oids;
	Oid		   *end_id_index_oids;

	/* Results */
	graphid		first_start_id;
	graphid		last_end_id;
	ArrayBuildState *edge_ids;
	ArrayBuildState *edges;
	ArrayBuildState *vertices;

	/* About VLE Path. */
	int			minimum_output_depth;
	int			maximum_output_depth;
	cypher_rel_dir		cypher_rel_direction;

	/* Scanning depth infos */
	List	   *table_scan_desc_list;	/* List for saving scan descriptions. */
	bool		use_vertex_output;
    ExprState *prop_expr_state;  
    Node* edge_property_constraint_expr; 
    agtype * edge_property_constraint;
} cypher_vle_custom_scan_state;

typedef struct AgeBtreeEqScan AgeBtreeEqScan;

Oid find_usable_btree_index_for_attr(Relation relation,
                                     AttrNumber heap_attnum);
AgeBtreeEqScan *age_btree_eq_beginscan(Relation relation, Snapshot snapshot,
                                       AttrNumber heap_attnum,
                                       RegProcedure equality_function,
                                       Datum value, LOCKMODE index_lockmode);
AgeBtreeEqScan *age_btree_eq_beginscan_with_index(
    Relation relation, Snapshot snapshot, AttrNumber heap_attnum,
    RegProcedure equality_function, Datum value, LOCKMODE index_lockmode,
    Oid index_oid);
HeapTuple age_btree_eq_getnext(AgeBtreeEqScan *scan);
void age_btree_eq_endscan(AgeBtreeEqScan *scan);

void apply_update_list(ExtensiblePlanState *node,
                       cypher_update_information *set_info,
                       bool allow_update_self, List **modified_relids,
                       HTAB *updated_entities);
void mark_entity_relation_modified(List **modified_relids, Oid relid);
void notify_modified_entity_relations(List **modified_relids);
void ensure_age_relation_supports_raw_access(Relation relation);

TupleTableSlot *populate_vertex_tts(TupleTableSlot *elemTupleSlot, agtype_value *id, agtype_value *properties);
TupleTableSlot *populate_edge_tts(
    TupleTableSlot *elemTupleSlot, agtype_value *id, agtype_value *startid,
    agtype_value *endid, agtype_value *properties);

ResultRelInfo *create_entity_result_rel_info(EState *estate, char *graph_name, char *label_name);

void destroy_entity_result_rel_info(ResultRelInfo *result_rel_info);

bool entity_exists(EState *estate, Oid graph_oid, graphid id);
HeapTuple insert_entity_tuple(ResultRelInfo *resultRelInfo,
                              TupleTableSlot *elemTupleSlot,
                              EState *estate);
HeapTuple insert_entity_tuple_cid(ResultRelInfo *resultRelInfo,
                                  TupleTableSlot *elemTupleSlot,
                                  EState *estate, CommandId cid);

void clear_entity_slot(TupleTableSlot *elemTupleSlot);
void compute_stored_generated(ResultRelInfo *resultRelInfo,
                              TupleTableSlot *elemTupleSlot, EState *estate,
                              CmdType cmdtype);

#endif
