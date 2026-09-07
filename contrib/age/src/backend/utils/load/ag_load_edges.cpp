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

#include <cctype>

#include "utils/builtins.h"
#include "utils/memutils.h"

#include "catalog/ag_label.h"
#include "utils/agtype.h"
#include "utils/graphid.h"

#include "utils/load/ag_csv_reader.h"
#include "utils/load/ag_load_edges.h"
#include "utils/load/age_load.h"

#define DECIMAL_RADIX 10
#define EDGE_CSV_MINIMUM_FIELD_COUNT 4
#define EDGE_CSV_START_ID_COL 0
#define EDGE_CSV_START_LABEL_COL 1
#define EDGE_CSV_END_ID_COL 2
#define EDGE_CSV_END_LABEL_COL 3
#define EDGE_CSV_NO_COLUMN ((size_t)-1)

/* Per-file state of the edge loader; everything lives in load_context. */
typedef struct csv_edge_reader {
    /* header row: trimmed column names, NULL/"" columns carry no property */
    char **header;
    int header_num;
    /* neo4j-style header only */
    agtype_value_type *col_type;
    size_t start_col;
    size_t end_col;
    char *start_vertex;
    char *end_vertex;

    Oid graph_id;
    char *object_name;
    int object_id;
    bool load_as_agtype;
    bool with_neo4j_like_header;
    bool adjust_id;

    Oid sequence_id;

    loader_target *target;
} csv_edge_reader;

static char *parse_endpoint_label(const char *header_column);
static void parse_edge_header(csv_edge_reader *cr, char **fields, int nfields);
static void process_edge_row(csv_edge_reader *cr, char **fields, int nfields);

/*
 * ":START_ID(Person)" -> "person".  neo4j-import labels are matched case
 * insensitively, AGE labels are not, so the name is lower-cased.
 */
static char *parse_endpoint_label(const char *header_column)
{
    const char *start_lab = strchr(header_column, '(');
    const char *end_lab = strchr(header_column, ')');
    char *label;
    size_t i;

    if (start_lab == NULL || end_lab == NULL || end_lab <= start_lab + 1) {
        ereport(ERROR,
                (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                 errmsg("invalid vertex header: \"%s\"", header_column)));
    }

    label = pnstrdup(start_lab + 1, end_lab - start_lab - 1);
    for (i = 0; label[i] != '\0'; i++) {
        label[i] = tolower((unsigned char)label[i]);
    }

    return label;
}

/*
 * The header row.  Plain files: start_id, start_vertex_type, end_id,
 * end_vertex_type, then property names.  neo4j-import style files:
 * ":START_ID(Label)", ":END_ID(Label)" and "name:TYPE" columns in any order.
 * File content is untrusted: every column is validated before use.
 */
static void parse_edge_header(csv_edge_reader *cr, char **fields, int nfields)
{
    int i;

    cr->header_num = nfields;
    cr->header = (char **)palloc0(sizeof(char *) * nfields);

    if (!cr->with_neo4j_like_header) {
        if (nfields < EDGE_CSV_MINIMUM_FIELD_COUNT) {
            ereport(ERROR,
                    (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                     errmsg("edge file must have at least 4 columns "
                            "(start_id, start_vertex_type, end_id, "
                            "end_vertex_type), but the header has %d",
                            nfields),
                     errhint("load_edges_from_file expects a comma-delimited "
                             "CSV; check the file's delimiter.")));
        }

        for (i = 0; i < nfields; i++) {
            cr->header[i] = trim_csv_value(fields[i]);
        }
        return;
    }

    cr->col_type = (agtype_value_type *)palloc0(
        sizeof(agtype_value_type) * nfields);

    for (i = 0; i < nfields; i++) {
        char *field = trim_csv_value(fields[i]);

        // creationDate:LONG|:START_ID(Person)|:END_ID(Organisation)|workFrom:LONG
        if (strstr(field, "START_ID") != NULL) {
            if (cr->start_col != EDGE_CSV_NO_COLUMN) {
                ereport(ERROR,
                        (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                         errmsg("edge file header has more than one "
                                "START_ID column")));
            }
            cr->start_col = i;
            cr->start_vertex = parse_endpoint_label(field);
            cr->header[i] = "start_id";
            cr->col_type[i] = AGTV_NUMERIC;
            continue;
        }

        if (strstr(field, "END_ID") != NULL) {
            if (cr->end_col != EDGE_CSV_NO_COLUMN) {
                ereport(ERROR,
                        (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                         errmsg("edge file header has more than one "
                                "END_ID column")));
            }
            cr->end_col = i;
            cr->end_vertex = parse_endpoint_label(field);
            cr->header[i] = "end_id";
            cr->col_type[i] = AGTV_NUMERIC;
            continue;
        }

        char *colon = strchr(field, ':');
        if (colon == NULL) {
            ereport(ERROR,
                    (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                     errmsg("edge file header column %d (\"%s\") "
                            "must have the form name:TYPE",
                            i + 1, field)));
        }

        *colon = '\0';
        cr->header[i] = field;
        const char *type = colon + 1;

        if (strcmp(type, "LONG") == 0) {
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

    if (cr->start_col == EDGE_CSV_NO_COLUMN || cr->end_col == EDGE_CSV_NO_COLUMN) {
        ereport(ERROR,
                (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                 errmsg("edge file header must contain START_ID and END_ID columns")));
    }
}

static int64 edge_endpoint_id(const char *field, bool adjust_id)
{
    int64 id = strtol(field != NULL ? field : "", NULL, DECIMAL_RADIX);

    return adjust_id ? id + 1 : id;
}

static int32 edge_endpoint_label_id(csv_edge_reader *cr, const char *field)
{
    char *label_name = trim_csv_value(field);

    return get_label_id(label_name, cr->graph_id);
}

static void process_edge_row(csv_edge_reader *cr, char **fields, int nfields)
{
    int64 start_id_int;
    int64 end_id_int;
    int32 start_vertex_type_id;
    int32 end_vertex_type_id;
    graphid object_graph_id;
    graphid start_vertex_graph_id;
    graphid end_vertex_graph_id;
    agtype *props;

    if (!cr->with_neo4j_like_header) {
        if (nfields < EDGE_CSV_MINIMUM_FIELD_COUNT || nfields > cr->header_num) {
            ereport(ERROR,
                    (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                     errmsg("edge file row has %d columns; expected at least "
                            "4 and no more than the header's %d columns",
                            nfields, cr->header_num)));
        }

        start_id_int = edge_endpoint_id(fields[EDGE_CSV_START_ID_COL], false);
        start_vertex_type_id = edge_endpoint_label_id(cr, fields[EDGE_CSV_START_LABEL_COL]);
        end_id_int = edge_endpoint_id(fields[EDGE_CSV_END_ID_COL], false);
        end_vertex_type_id = edge_endpoint_label_id(cr, fields[EDGE_CSV_END_LABEL_COL]);
    }
    else {
        if (nfields > cr->header_num) {
            ereport(ERROR,
                    (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                     errmsg("edge file row has %d columns, more than the "
                            "header's %d columns",
                            nfields, cr->header_num)));
        }

        if (cr->start_col >= (size_t)nfields || cr->end_col >= (size_t)nfields) {
            ereport(ERROR,
                    (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                     errmsg("edge file row has %d columns and does not "
                            "contain both endpoint id columns",
                            nfields)));
        }

        start_id_int = edge_endpoint_id(fields[cr->start_col], cr->adjust_id);
        start_vertex_type_id = get_label_id(cr->start_vertex, cr->graph_id);
        end_id_int = edge_endpoint_id(fields[cr->end_col], cr->adjust_id);
        end_vertex_type_id = get_label_id(cr->end_vertex, cr->graph_id);
    }

    /* make_graphid() rejects unknown labels (id 0) and out-of-range ids */
    object_graph_id = make_graphid(
        cr->object_id, next_loader_sequence_value(cr->sequence_id));
    start_vertex_graph_id = make_graphid(start_vertex_type_id, start_id_int);
    end_vertex_graph_id = make_graphid(end_vertex_type_id, end_id_int);

    props = create_agtype_from_list_i(cr->header, fields, cr->col_type,
                                      nfields, cr->start_col, cr->end_col,
                                      cr->load_as_agtype);

    insert_edge_simple(cr->target, object_graph_id, start_vertex_graph_id,
                       end_vertex_graph_id, props);
}

int create_edges_from_csv_file(char *file_path,
                               char *graph_name,
                               Oid graph_id,
                               char *object_name,
                               int object_id,
                               bool load_as_agtype,
                               char delimiter,
                               bool with_header,
                               bool adjust_id)
{
    csv_edge_reader *reader;
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
                                         "AGE CSV edge loader",
                                         ALLOCSET_DEFAULT_SIZES);
    row_context = AllocSetContextCreate(load_context,
                                        "AGE CSV edge loader row",
                                        ALLOCSET_DEFAULT_SIZES);
    old_context = MemoryContextSwitchTo(load_context);

    reader = (csv_edge_reader *)palloc0(sizeof(csv_edge_reader));
    reader->start_col = EDGE_CSV_NO_COLUMN;
    reader->end_col = EDGE_CSV_NO_COLUMN;
    reader->graph_id = graph_id;
    reader->object_name = object_name;
    reader->object_id = object_id;
    reader->load_as_agtype = load_as_agtype;
    reader->with_neo4j_like_header = with_header;
    reader->adjust_id = adjust_id;
    reader->sequence_id = get_loader_label_sequence_oid(graph_id, object_name);

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
                parse_edge_header(reader, fields, nfields);
                is_header_row = false;
                continue;
            }

            MemoryContextSwitchTo(row_context);
            process_edge_row(reader, fields, nfields);
            MemoryContextSwitchTo(load_context);
            MemoryContextReset(row_context);
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
