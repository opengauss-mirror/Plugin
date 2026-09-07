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

#include "access/genam.h"
#include "access/xact.h"
#include "commands/sequence.h"
#include "catalog/ag_graph.h"
#include "commands/label_commands.h"
#include "utils/ag_cache.h"
#include "utils/agtype.h"
#include "utils/graphid.h"
#include "utils/lsyscache.h"
#include "utils/load/age_load.h"

extern "C" Datum create_graph(PG_FUNCTION_ARGS);
extern "C" Datum create_vlabel(PG_FUNCTION_ARGS);
extern "C" Datum create_elabel(PG_FUNCTION_ARGS);

int64 get_nextval_internal(graph_cache_data* graph_cache,
                           label_cache_data* label_cache);
static agtype *create_empty_agtype(void);
static char *get_label_sequence_name(const char *label_relation_name);

#define GRAPH_NAME_ARGUMENT_INDEX 0
#define GRAPH_SIZE_ARGUMENT_INDEX 1
#define EDGE_LABEL_ARGUMENT_INDEX 2
#define NODE_LABEL_ARGUMENT_INDEX 3
#define BRIDGE_SIZE_ARGUMENT_INDEX 2
#define EDGE_LABEL_BARbell_ARGUMENT_INDEX 5
#define COMPLETE_GRAPH_ARGUMENT_COUNT 4
#define COMPLETE_GRAPH_FIRST_NODE_INDEX 1
#define BARBELL_GRAPH_COUNT 2
#define COMPLETE_GRAPH_MINIMUM_SIZE 3

static char *get_label_sequence_name(const char *label_relation_name)
{
    return psprintf("%s_id_seq", label_relation_name);
}

static agtype *create_empty_agtype(void)
{
    agtype_in_state result;

    memset(&result, 0, sizeof(agtype_in_state));

    result.res = push_agtype_value(&result.parse_state,
                                   WAGT_BEGIN_OBJECT, NULL);
    result.res = push_agtype_value(&result.parse_state,
                                   WAGT_END_OBJECT, NULL);

    return agtype_value_to_agtype(result.res);
}

/*
 * Auxiliary function to get the next internal value in the graph,
 * so a new object (node or edge) graph id can be composed.
 */

int64 get_nextval_internal(graph_cache_data* graph_cache,
                           label_cache_data* label_cache)
{
    Oid obj_seq_id;
    Oid label_relation;
    char* label_seq_name_str;
    char* label_name_str;

    label_relation = get_label_relation(NameStr(label_cache->name),
                                        label_cache->graph);
    label_name_str = get_rel_name(label_relation);
    label_seq_name_str = get_label_sequence_name(label_name_str);
    obj_seq_id = get_relname_relid(label_seq_name_str,
                                   graph_cache->namespaceoid);

    return (int64)nextval_internal(obj_seq_id, true, false);
}

PG_FUNCTION_INFO_V1(create_complete_graph);
extern "C" Datum create_complete_graph(PG_FUNCTION_ARGS);

/*
* SELECT * FROM ag_catalog.create_complete_graph('graph_name',no_of_nodes, 'edge_label', 'node_label'=NULL);
*/

Datum create_complete_graph(PG_FUNCTION_ARGS)
{
    Oid graph_oid;
    Name graph_name;

    int64 no_vertices;
    int64 i;
    int64 j;
    int64 vid = 1;
    int64 eid;
    int64 start_vid;
    int64 end_vid;

    Name vtx_label_name = NULL;
    Name edge_label_name;
    int32 vtx_label_id;
    int32 edge_label_id;

    agtype *props = NULL;
    graphid object_graph_id;
    graphid start_vertex_graph_id;
    graphid end_vertex_graph_id;

    Oid vtx_seq_id;
    Oid edge_seq_id;

    char* graph_name_str;
    loader_target *target;
    char* vtx_name_str;
    char* edge_name_str;

    graph_cache_data *graph_cache;
    label_cache_data *vertex_cache;
    label_cache_data *edge_cache;

    Oid nsp_id;
    Name vtx_seq_name;
    char *vtx_seq_name_str;

    Name edge_seq_name;
    char *edge_seq_name_str;

    int64 lid;

    if (PG_ARGISNULL(GRAPH_NAME_ARGUMENT_INDEX)) {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                        errmsg("graph name can not be NULL")));
    }

    if (PG_ARGISNULL(GRAPH_SIZE_ARGUMENT_INDEX)) {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                        errmsg("number of nodes can not be NULL")));
    }

    if (PG_ARGISNULL(EDGE_LABEL_ARGUMENT_INDEX)) {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                        errmsg("edge label can not be NULL")));
    }

    graph_name = PG_GETARG_NAME(GRAPH_NAME_ARGUMENT_INDEX);
    no_vertices = (int64) PG_GETARG_INT32(GRAPH_SIZE_ARGUMENT_INDEX);
    edge_label_name = PG_GETARG_NAME(EDGE_LABEL_ARGUMENT_INDEX);

    graph_name_str = NameStr(*graph_name);
    vtx_name_str = AG_DEFAULT_LABEL_VERTEX;
    edge_name_str = NameStr(*edge_label_name);

    if (!PG_ARGISNULL(NODE_LABEL_ARGUMENT_INDEX)) {
        vtx_label_name = PG_GETARG_NAME(NODE_LABEL_ARGUMENT_INDEX);
        vtx_name_str = NameStr(*vtx_label_name);
        /* Check if vertex and edge label are same */
        if (strcmp(vtx_name_str, edge_name_str) == 0) {
            ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                    errmsg("vertex and edge label can not be same")));
        }
    }

    if (!graph_exists(graph_name_str)) {
        DirectFunctionCall1(create_graph, NameGetDatum(graph_name));
        CommandCounterIncrement();
    }

    graph_oid = get_graph_oid(graph_name_str);

    if (!PG_ARGISNULL(NODE_LABEL_ARGUMENT_INDEX)) {
        /* Check if label with the input name already exists */
        if (!label_exists(vtx_name_str, graph_oid)) {
            DirectFunctionCall2(create_vlabel,
                                CStringGetDatum(graph_name->data),
                                CStringGetDatum(vtx_label_name->data));
            CommandCounterIncrement();
        }
    }

    if (!label_exists(edge_name_str, graph_oid)) {
        DirectFunctionCall2(create_elabel,
                            CStringGetDatum(graph_name->data),
                            CStringGetDatum(edge_label_name->data));
        CommandCounterIncrement();
    }

    vtx_label_id = get_label_id(vtx_name_str, graph_oid);
    edge_label_id = get_label_id(edge_name_str, graph_oid);

    graph_cache = search_graph_name_cache(graph_name_str);
    vertex_cache = search_label_name_graph_cache(vtx_name_str, graph_oid);
    edge_cache = search_label_name_graph_cache(edge_name_str, graph_oid);

    nsp_id = graph_cache->namespaceoid;
    vtx_name_str = get_label_relation_name(vtx_name_str, graph_oid);
    vtx_seq_name_str = get_label_sequence_name(vtx_name_str);

    edge_name_str = get_label_relation_name(edge_name_str, graph_oid);
    edge_seq_name_str = get_label_sequence_name(edge_name_str);

    vtx_seq_id = get_relname_relid(vtx_seq_name_str, nsp_id);
    edge_seq_id = get_relname_relid(edge_seq_name_str, nsp_id);

    props = create_empty_agtype();

    /* Creating vertices*/
    target = open_loader_target(graph_oid, graph_name_str, vtx_name_str);
    for (i=(int64)1; i<=no_vertices; i++) {
        vid = (int64)nextval_internal(vtx_seq_id, true, false);
        object_graph_id = make_graphid(vtx_label_id, vid);
        insert_vertex_simple(target, object_graph_id, props);
    }
    close_loader_target(target);

    lid = vid;

    /* Creating edges*/
    target = open_loader_target(graph_oid, graph_name_str, edge_name_str);
    for (i = 1; i <= no_vertices - 1; i++) {
        start_vid = lid-no_vertices+i;
        for (j = i + 1; j <= no_vertices; j++) {
            end_vid = lid-no_vertices+j;
            eid = (int64)nextval_internal(edge_seq_id, true, false);
            object_graph_id = make_graphid(edge_label_id, eid);

            start_vertex_graph_id = make_graphid(vtx_label_id, start_vid);
            end_vertex_graph_id = make_graphid(vtx_label_id, end_vid);

            insert_edge_simple(target, object_graph_id,
                               start_vertex_graph_id, end_vertex_graph_id,
                               props);
        }
    }
    close_loader_target(target);
    PG_RETURN_VOID();
}


PG_FUNCTION_INFO_V1(age_create_barbell_graph);
extern "C" Datum age_create_barbell_graph(PG_FUNCTION_ARGS);

static void call_create_complete_graph(Datum graph_name, Datum graph_size,
                                       Datum edge_label, Datum node_label,
                                       bool node_label_is_null)
{
    FunctionCallInfoData complete_graph_fcinfo;

    InitFunctionCallInfoData(complete_graph_fcinfo, NULL,
                             COMPLETE_GRAPH_ARGUMENT_COUNT, InvalidOid,
                             NULL, NULL);
    complete_graph_fcinfo.arg[GRAPH_NAME_ARGUMENT_INDEX] = graph_name;
    complete_graph_fcinfo.arg[GRAPH_SIZE_ARGUMENT_INDEX] = graph_size;
    complete_graph_fcinfo.arg[EDGE_LABEL_ARGUMENT_INDEX] = edge_label;
    complete_graph_fcinfo.arg[NODE_LABEL_ARGUMENT_INDEX] = node_label;
    complete_graph_fcinfo.argnull[GRAPH_NAME_ARGUMENT_INDEX] = false;
    complete_graph_fcinfo.argnull[GRAPH_SIZE_ARGUMENT_INDEX] = false;
    complete_graph_fcinfo.argnull[EDGE_LABEL_ARGUMENT_INDEX] = false;
    complete_graph_fcinfo.argnull[NODE_LABEL_ARGUMENT_INDEX] = node_label_is_null;

    create_complete_graph(&complete_graph_fcinfo);
}

/*
 * The barbell graph is two complete graphs connected by a bridge path
 * Syntax:
 * ag_catalog.age_create_barbell_graph(graph_name Name,
 *                                     m int,
 *                                     n int,
 *                                     vertex_label_name Name DEFAULT = NULL,
 *                                     vertex_properties agtype DEFAULT = NULL,
 *                                     edge_label_name Name DEFAULT = NULL,
 *                                     edge_properties agtype DEFAULT = NULL)
 * Input:
 *
 * graph_name - Name of the graph to be created.
 * m - number of vertices in one complete graph.
 * n - number of vertices in the bridge path.
 * vertex_label_name - Name of the label to assign each vertex to.
 * vertex_properties - Property values to assign each vertex. Default is NULL
 * edge_label_name - Name of the label to assign each edge to.
 * edge_properties - Property values to assign each edge. Default is NULL
 *
 * https://en.wikipedia.org/wiki/Barbell_graph
 */

Datum age_create_barbell_graph(PG_FUNCTION_ARGS)
{
    Oid graph_oid;
    Name graph_name;
    char* graph_name_str;
    loader_target *target;

    int64 start_node_index;
    int64 end_node_index;
    int64 nextval;

    NameData default_node_label_name;
    Name node_label_name;
    int32 node_label_id;
    char* node_label_str;

    Name edge_label_name;
    int32 edge_label_id;
    char* edge_label_str;

    graphid object_graph_id;
    graphid start_node_graph_id;
    graphid end_node_graph_id;

    graph_cache_data* graph_cache;
    label_cache_data* edge_cache;

    agtype* properties = NULL;

    /* Checking for possible NULL arguments */
    /* Name graph_name */
    if (PG_ARGISNULL(0)) {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                        errmsg("Graph name cannot be NULL")));
    }

    graph_name = PG_GETARG_NAME(GRAPH_NAME_ARGUMENT_INDEX);
    graph_name_str = NameStr(*graph_name);

    /* int graph size (number of nodes in each complete graph) */
    if (PG_ARGISNULL(GRAPH_SIZE_ARGUMENT_INDEX) ||
        PG_GETARG_INT32(GRAPH_SIZE_ARGUMENT_INDEX) < COMPLETE_GRAPH_MINIMUM_SIZE) {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                errmsg("Graph size cannot be NULL or lower than 3")));
    }

    /*
     * int64 bridge_size: currently only stays at zero.
     * to do: implement bridge with variable number of nodes.
     */
    if (PG_ARGISNULL(BRIDGE_SIZE_ARGUMENT_INDEX) ||
        PG_GETARG_INT32(BRIDGE_SIZE_ARGUMENT_INDEX) < 0) {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                errmsg("Bridge size cannot be NULL or lower than 0")));
    }

    /* node label: if null, gets default label, which is "_ag_label_vertex" */
    if (PG_ARGISNULL(NODE_LABEL_ARGUMENT_INDEX)) {
        namestrcpy(&default_node_label_name, AG_DEFAULT_LABEL_VERTEX);
        node_label_name = &default_node_label_name;
    } else {
        node_label_name = PG_GETARG_NAME(NODE_LABEL_ARGUMENT_INDEX);
    }
    node_label_str = NameStr(*node_label_name);

    /* Name edge_label */
    if (PG_ARGISNULL(EDGE_LABEL_BARbell_ARGUMENT_INDEX)) {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                errmsg("edge label can not be NULL")));
    }

    edge_label_name = PG_GETARG_NAME(EDGE_LABEL_BARbell_ARGUMENT_INDEX);
    edge_label_str = NameStr(*edge_label_name);
    /* create two separate complete graphs */
    call_create_complete_graph(PG_GETARG_DATUM(GRAPH_NAME_ARGUMENT_INDEX),
                               PG_GETARG_DATUM(GRAPH_SIZE_ARGUMENT_INDEX),
                               PG_GETARG_DATUM(EDGE_LABEL_BARbell_ARGUMENT_INDEX),
                               NameGetDatum(node_label_name),
                               PG_ARGISNULL(NODE_LABEL_ARGUMENT_INDEX));
    call_create_complete_graph(PG_GETARG_DATUM(GRAPH_NAME_ARGUMENT_INDEX),
                               PG_GETARG_DATUM(GRAPH_SIZE_ARGUMENT_INDEX),
                               PG_GETARG_DATUM(EDGE_LABEL_BARbell_ARGUMENT_INDEX),
                               NameGetDatum(node_label_name),
                               PG_ARGISNULL(NODE_LABEL_ARGUMENT_INDEX));

    graph_oid = get_graph_oid(graph_name_str);
    node_label_id = get_label_id(node_label_str, graph_oid);
    edge_label_id = get_label_id(edge_label_str, graph_oid);

    /*
     * Fetching caches to get next values for graph id's, and access nodes
     * to be connected with edges.
     */
    graph_cache = search_graph_name_cache(graph_name_str);
    edge_cache = search_label_name_graph_cache(edge_label_str, graph_oid);

    /* connect a node from each graph */
    /* first created node, from the first complete graph */
    start_node_index = COMPLETE_GRAPH_FIRST_NODE_INDEX;
    /* last created node, second graph */
    end_node_index = PG_GETARG_INT32(GRAPH_SIZE_ARGUMENT_INDEX) *
                     BARBELL_GRAPH_COUNT;

    /* next index to be assigned to a node or edge */
    nextval = get_nextval_internal(graph_cache, edge_cache);

    /* build the graph id's of the edge to be created */
    object_graph_id = make_graphid(edge_label_id, nextval);
    start_node_graph_id = make_graphid(node_label_id, start_node_index);
    end_node_graph_id = make_graphid(node_label_id, end_node_index);
    properties = create_empty_agtype();

    /* connect two nodes */
    target = open_loader_target(graph_oid, graph_name_str, edge_label_str);
    insert_edge_simple(target, object_graph_id, start_node_graph_id,
                       end_node_graph_id, properties);
    close_loader_target(target);

    PG_RETURN_VOID();
}
