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

#include "nodes/execnodes.h"
#include "nodes/nodes.h"
#include "nodes/plannodes.h"

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

typedef struct cypher_create_custom_scan_state
{
    ExtensiblePlanState css;

    ExtensiblePlan *cs;

    List *pattern;
    List *path_values;
    uint32 flags;
    TupleTableSlot *slot;
    Oid graph_oid;
} cypher_create_custom_scan_state;

typedef struct cypher_set_custom_scan_state
{
    ExtensiblePlanState css;

    ExtensiblePlan *cs;

    cypher_update_information *set_list;
    int flags;
} cypher_set_custom_scan_state;

typedef struct cypher_delete_custom_scan_state
{
    ExtensiblePlanState css;

    ExtensiblePlan *cs;

    cypher_delete_information *delete_data;
    int flags;
    List *edge_labels;
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
    bool found_a_path;
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

#endif
