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

#include "utils/builtins.h"
#include "utils/memutils.h"

#include "catalog/ag_label.h"
#include "utils/agtype.h"
#include "utils/graphid.h"

#include "utils/load/ag_csv_reader.h"
#include "utils/load/ag_load_labels.h"
#include "utils/load/age_load.h"

#define DECIMAL_RADIX 10

/* Per-file state of the vertex loader; everything lives in load_context. */
typedef struct csv_vertex_reader {
    /* header row: trimmed column names, NULL/"" columns carry no property */
    char **header;
    int header_num;
    /* neo4j-style header only: per-column type and the "id" column index */
    agtype_value_type *col_type;
    int id_col;

    Oid graph_id;
    char *object_name;
    int object_id;
    bool id_field_exists;
    bool load_as_agtype;
    bool with_neo4j_like_header;
    bool adjust_id;

    Oid sequence_id;
    /* last value handed out by the sequence when the load started */
    int64 current_sequence_value;
    /* highest explicit id seen; the sequence is moved past it once at the end */
    int64 highest_explicit_id;

    loader_target *target;
} csv_vertex_reader;

static void parse_vertex_header(csv_vertex_reader *cr, char **fields,
                                int nfields);
static int64 resolve_vertex_row_id(csv_vertex_reader *cr, char **fields,
                                   int nfields);
static void process_vertex_row(csv_vertex_reader *cr, char **fields,
                               int nfields);

/*
 * The header row.  Plain files carry property names; neo4j-import style
 * files carry "name:TYPE" columns, with the "id" column giving the vertex id.
 * File content is untrusted: every column is validated before use.
 */
static void parse_vertex_header(csv_vertex_reader *cr, char **fields,
                                int nfields)
{
    int i;

    cr->header_num = nfields;
    cr->header = (char **)palloc0(sizeof(char *) * nfields);
    if (cr->with_neo4j_like_header) {
        cr->col_type = (agtype_value_type *)palloc0(
            sizeof(agtype_value_type) * nfields);
        cr->id_col = -1;
    }

    for (i = 0; i < nfields; i++) {
        char *field = trim_csv_value(fields[i]);

        if (!cr->with_neo4j_like_header) {
            cr->header[i] = field;
            continue;
        }

        char *colon = strchr(field, ':');
        if (colon == NULL) {
            ereport(ERROR,
                    (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                     errmsg("label file header column %d (\"%s\") "
                            "must have the form name:TYPE",
                            i + 1, field)));
        }

        *colon = '\0';
        cr->header[i] = field;
        const char *type = colon + 1;

        if (strcmp(cr->header[i], "id") == 0) {
            cr->id_col = i;
            cr->col_type[i] = AGTV_NUMERIC;
        }
        else if (strcmp(type, "LONG") == 0) {
            cr->col_type[i] = AGTV_NUMERIC;
        }
        else if (strcmp(type, "BOOL") == 0) {
            cr->col_type[i] = AGTV_BOOL;
        }
        else {
            /* STRING and any unknown type are loaded as strings */
            cr->col_type[i] = AGTV_STRING;
        }
    }
}

/*
 * Determine the vertex id for the current CSV row: either the id column
 * supplied by the file (keeping the loader sequence ahead of it) or the next
 * loader sequence value.
 */
static int64 resolve_vertex_row_id(csv_vertex_reader *cr, char **fields,
                                   int nfields)
{
    int id_index;
    const char *id_text;
    int64 label_id_int;

    if (!cr->id_field_exists) {
        return next_loader_sequence_value(cr->sequence_id);
    }

    id_index = cr->id_col >= 0 ? cr->id_col : 0;
    if (id_index >= nfields) {
        ereport(ERROR,
                (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                 errmsg("label file row has %d columns and does not "
                        "contain required id column %d",
                        nfields, id_index + 1)));
    }

    id_text = fields[id_index] != NULL ? fields[id_index] : "";
    label_id_int = strtol(id_text, NULL, DECIMAL_RADIX);
    if (cr->adjust_id) {
        label_id_int++;
    }

    if (label_id_int > cr->highest_explicit_id) {
        cr->highest_explicit_id = label_id_int;
    }

    return label_id_int;
}

static void process_vertex_row(csv_vertex_reader *cr, char **fields,
                               int nfields)
{
    int64 entry_id;
    graphid vertex_id;
    agtype *props;

    if (nfields > cr->header_num) {
        ereport(ERROR,
                (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                 errmsg("label file row has %d columns, more than the "
                        "header's %d columns",
                        nfields, cr->header_num)));
    }

    entry_id = resolve_vertex_row_id(cr, fields, nfields);
    vertex_id = make_graphid(cr->object_id, entry_id);

    props = create_agtype_from_list(cr->header, fields, cr->col_type,
                                    nfields, entry_id, cr->load_as_agtype);
    insert_vertex_simple(cr->target, vertex_id, props);
}

int create_labels_from_csv_file(char *file_path,
                                char *graph_name,
                                Oid graph_id,
                                char *object_name,
                                int object_id,
                                bool id_field_exists,
                                bool load_as_agtype,
                                char delimiter,
                                bool with_header,
                                bool adjust_id)
{
    csv_vertex_reader *reader;
    ag_csv_reader *volatile csv = NULL;
    char **fields;
    int nfields;
    bool is_header_row = true;
    MemoryContext load_context;
    MemoryContext row_context;
    MemoryContext old_context;

    /*
     * load_context holds the reader, the header and the CSV record buffers
     * for the whole file; row_context holds one row's properties and SPI
     * query text and is reset after every row.  The caller's context is never
     * touched: it still owns the function arguments (label name, path).
     */
    load_context = AllocSetContextCreate(CurrentMemoryContext,
                                         "AGE CSV label loader",
                                         ALLOCSET_DEFAULT_SIZES);
    row_context = AllocSetContextCreate(load_context,
                                        "AGE CSV label loader row",
                                        ALLOCSET_DEFAULT_SIZES);
    old_context = MemoryContextSwitchTo(load_context);

    reader = (csv_vertex_reader *)palloc0(sizeof(csv_vertex_reader));
    reader->graph_id = graph_id;
    reader->object_name = object_name;
    reader->object_id = object_id;
    reader->id_field_exists = id_field_exists;
    reader->load_as_agtype = load_as_agtype;
    reader->with_neo4j_like_header = with_header;
    reader->adjust_id = adjust_id;
    reader->id_col = -1;
    reader->sequence_id = get_loader_label_sequence_oid(graph_id, object_name);
    reader->current_sequence_value =
        id_field_exists ? next_loader_sequence_value(reader->sequence_id) : 0;
    reader->highest_explicit_id = reader->current_sequence_value;

    /* neo4j-import style files are always '|' separated */
    if (with_header)
    {
        delimiter = '|';
    }

    PG_TRY();
    {
        reader->target = open_loader_target(graph_id, graph_name, object_name);

        /* file_path was resolved and checked by build_safe_csv_path() */
        csv = ag_csv_open(file_path, delimiter);

        while (ag_csv_next_record(csv, &fields, &nfields))
        {
            if (is_header_row) {
                parse_vertex_header(reader, fields, nfields);
                is_header_row = false;
                continue;
            }

            MemoryContextSwitchTo(row_context);
            process_vertex_row(reader, fields, nfields);
            MemoryContextSwitchTo(load_context);
            MemoryContextReset(row_context);
        }

        /* keep the label sequence ahead of the ids the file supplied */
        if (reader->highest_explicit_id > reader->current_sequence_value)
        {
            advance_loader_sequence(reader->sequence_id,
                                    reader->highest_explicit_id);
        }
    }
    PG_CATCH();
    {
        /* the file handle and the relation are also released by the abort */
        if (csv != NULL)
        {
            ag_csv_close(csv);
        }
        MemoryContextSwitchTo(old_context);
        MemoryContextDelete(load_context);
        PG_RE_THROW();
    }
    PG_END_TRY();

    ag_csv_close(csv);
    close_loader_target(reader->target);
    MemoryContextSwitchTo(old_context);
    MemoryContextDelete(load_context);
    return EXIT_SUCCESS;
}
