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


#ifndef AG_LOAD_LABELS_H
#define AG_LOAD_LABELS_H


#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

#include "postgres.h"

#include "access/heapam.h"
#include "access/xact.h"
#include "catalog/dependency.h"
#include "catalog/namespace.h"
#include "catalog/objectaddress.h"
#include "catalog/pg_class_d.h"
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
#include "storage/lockdefs.h"
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


#define AGE_VERTIX 1
#define AGE_EDGE 2


struct counts {
    long unsigned fields;
    long unsigned allvalues;
    long unsigned rows;
};

typedef struct {
    size_t row;
    char **header;
    size_t *header_len;
    size_t header_num;
    char **fields;
    size_t *fields_len;
    size_t alloc;
    size_t cur_field;
    int error;
    size_t header_row_length;
    size_t curr_row_length;
    char *graph_name;
    Oid graph_id;
    char *object_name;
    int object_id;
    bool id_field_exists;
} csv_vertex_reader;


void vertex_field_cb(void *field, size_t field_len, void *data);
void vertex_row_cb(int delim __attribute__((unused)), void *data);

int create_labels_from_csv_file(char *file_path, char *graph_name, Oid graph_id,
                                char *object_name, int object_id,
                                bool id_field_exists);

#endif //AG_LOAD_LABELS_H
