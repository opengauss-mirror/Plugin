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

#include "postgres.h"

#include "utils/agtype.h"
#include "utils/graphid.h"

/*
 * Load vertices from a CSV file into a vertex label using the kernel COPY
 * parser.  With with_header the first row is a neo4j-import style header
 * ("name:TYPE" columns, '|' delimiter) and adjust_id adds one to every id.
 */
int create_labels_from_csv_file(char *file_path, char *graph_name,
                                Oid graph_id, char *object_name,
                                int object_id, bool id_field_exists,
                                bool load_as_agtype = false,
                                char delimiter = ',',
                                bool with_header = false,
                                bool adjust_id = false);

#endif //AG_LOAD_LABELS_H
