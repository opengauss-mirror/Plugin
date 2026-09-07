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

#ifndef AG_LOAD_EDGES_H
#define AG_LOAD_EDGES_H

#include "postgres.h"

#include "utils/agtype.h"
#include "utils/graphid.h"

/*
 * Load edges from a CSV file into an edge label using the kernel COPY parser.
 * Without with_header the first four columns are
 * start_id, start_vertex_type, end_id, end_vertex_type; with with_header the
 * first row is a neo4j-import style header (":START_ID(Label)",
 * ":END_ID(Label)", "name:TYPE" columns, '|' delimiter) and adjust_id adds
 * one to every endpoint id.
 */
int create_edges_from_csv_file(char *file_path, char *graph_name,
                               Oid graph_id, char *object_name,
                               int object_id, bool load_as_agtype = false,
                               char delimiter = ',',
                               bool with_header = false,
                               bool adjust_id = false);

#endif //AG_LOAD_EDGES_H
