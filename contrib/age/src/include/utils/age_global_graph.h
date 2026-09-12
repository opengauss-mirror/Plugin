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

#ifndef AG_AGE_GLOBAL_GRAPH_H
#define AG_AGE_GLOBAL_GRAPH_H

#include "utils/graphid.h"
#include "utils/age_graphid_ds.h"

/*
 * Flat dynamic-array adjacency container for vertex edges. Replaces a
 * linked-list (ListGraphId) of GraphIdNodes for vertex_entry's edges_*.
 *
 * Storage: a single palloc'd graphid array, doubled on growth. The struct is
 * embedded by value in vertex_entry so the (array, size, capacity) triple lives
 * in the same cache line as the surrounding entry fields, saving one
 * indirection and enabling contiguous iteration on the DFS/BFS hot path.
 *
 * Empty arrays carry array == NULL, size == 0, capacity == 0 and incur no
 * allocation until the first append.
 */
typedef struct VertexEdgeArray {
    graphid *array;     /* contiguous edge graphid array; NULL when empty */
    int32 size;         /* number of edges currently stored */
    int32 capacity;     /* allocated capacity (in graphid slots) */
} VertexEdgeArray;

/*
 * We declare the graph nodes and edges here, and in this way, so that it may be
 * used elsewhere. However, we keep the contents private by defining it in
 * age_global_graph.c
 */

/* vertex entry for the vertex_hastable */
typedef struct vertex_entry vertex_entry;

/* edge entry for the edge_hashtable */
typedef struct edge_entry edge_entry;

typedef struct GRAPH_global_context GRAPH_global_context;

/* GRAPH global context functions */
void register_GRAPH_global_context_relcache_callback(void);
void invalidate_GRAPH_global_contexts_by_relid(Oid relid);
void notify_GRAPH_global_contexts_relation_modified(Oid relid);
void notify_GRAPH_global_contexts_catalog_modified(void);
GRAPH_global_context *manage_GRAPH_global_contexts(char *graph_name,
                                                   Oid graph_oid);
/*
 * Same, but only loads the named edge label relations. Pass NIL for every edge
 * label. Loading edge tables dominates cache build cost, so traversals that
 * know their edge labels should name them.
 */
GRAPH_global_context *manage_GRAPH_global_contexts_for_labels(
    char *graph_name, Oid graph_oid, List *edge_label_relids);
GRAPH_global_context *find_GRAPH_global_context(Oid graph_oid);
bool is_ggctx_invalid(GRAPH_global_context *ggctx);
uint64 get_GRAPH_global_context_generation(GRAPH_global_context *ggctx);
/* GRAPH retrieval functions */
ListGraphId *get_graph_vertices(GRAPH_global_context *ggctx);
vertex_entry *get_vertex_entry(GRAPH_global_context *ggctx,
                               graphid vertex_id);
edge_entry *get_edge_entry(GRAPH_global_context *ggctx, graphid edge_id);
edge_entry *get_edge_entry_with_hash(GRAPH_global_context *ggctx,
                                     graphid edge_id, uint32 hashvalue);
/* vertex entry accessor functions*/
graphid get_vertex_entry_id(vertex_entry *ve);
VertexEdgeArray *get_vertex_entry_edges_in_array(vertex_entry *ve);
VertexEdgeArray *get_vertex_entry_edges_out_array(vertex_entry *ve);
VertexEdgeArray *get_vertex_entry_edges_self_array(vertex_entry *ve);
Oid get_vertex_entry_label_table_oid(vertex_entry *ve);
Datum get_vertex_entry_properties(vertex_entry *ve);
/* edge entry accessor functions */
graphid get_edge_entry_id(edge_entry *ee);
Oid get_edge_entry_label_table_oid(edge_entry *ee);
Datum get_edge_entry_properties(edge_entry *ee);
graphid get_edge_entry_start_vertex_id(edge_entry *ee);
graphid get_edge_entry_end_vertex_id(edge_entry *ee);

/*
 * Fast hash function for graphid (int64) keys, replacing tag_hash. Signature
 * matches both dynahash's HashValueFunc and agehash_hash_fn.
 */
uint32 graphid_hash(const void *key, Size keysize);

/* Equality predicate for graphid (int64) keys; agehash_keyeq_fn signature. */
bool graphid_keyeq(const void *a, const void *b, Size keysize);
#endif
