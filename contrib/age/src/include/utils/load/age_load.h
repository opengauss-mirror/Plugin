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

#include "access/heapam.h"
#include "access/xact.h"
#include "catalog/dependency.h"
#include "catalog/namespace.h"
#include "catalog/objectaddress.h"
#include "commands/defrem.h"
#include "commands/sequence.h"
#include "commands/tablecmds.h"
#include "miscadmin.h"
#include "nodes/makefuncs.h"
#include "nodes/nodes.h"
#include "nodes/parsenodes.h"
#include "nodes/pg_list.h"
#include "nodes/plannodes.h"
#include "nodes/primnodes.h"
#include "nodes/value.h"
#include "parser/parse_node.h"
#include "parser/parser.h"
#include "tcop/dest.h"
#include "tcop/utility.h"
#include "utils/acl.h"
#include "utils/builtins.h"
#include "utils/inval.h"
#include "utils/lsyscache.h"
#include "utils/rel.h"

#include "catalog/ag_graph.h"
#include "catalog/ag_label.h"
#include "commands/label_commands.h"
#include "utils/ag_cache.h"
#include "utils/agtype.h"
#include "utils/graphid.h"
#include "nodes/execnodes.h"

#ifndef INCUBATOR_AGE_ENTITY_CREATOR_H
#define INCUBATOR_AGE_ENTITY_CREATOR_H

/* Shared helper for the CSV loaders; accepts a NULL field as "". */
char *trim_csv_value(const char *value);

agtype *create_agtype_from_list(char **header, char **fields,
                                agtype_value_type *column_types,
                                size_t fields_length, int64 vertex_id,
                                bool load_as_agtype);
agtype *create_agtype_from_list_i(char **header, char **fields,
                                  agtype_value_type *column_types,
                                  size_t fields_length, size_t start_index,
                                  size_t end_index, bool load_as_agtype);
Oid get_loader_label_sequence_oid(Oid graph_id, const char *label_name);
int64 next_loader_sequence_value(Oid sequence_id);
void advance_loader_sequence(Oid sequence_id, int64 sequence_value);

/*
 * Destination of a load: the label relation opened once per file together
 * with the executor state the heap insert path needs.
 */
typedef struct loader_target {
    EState *estate;
    ResultRelInfo *result_rel_info;
    TupleTableSlot *slot;
    Oid relation_id;
} loader_target;

loader_target *open_loader_target(Oid graph_id, char *graph_name,
                                  char *label_name);
void close_loader_target(loader_target *target);
void insert_vertex_simple(loader_target *target, graphid vertex_id,
                          agtype *vertex_properties);
void insert_edge_simple(loader_target *target, graphid edge_id,
                        graphid start_id, graphid end_id,
                        agtype *edge_properties);

#endif //INCUBATOR_AGE_ENTITY_CREATOR_H
