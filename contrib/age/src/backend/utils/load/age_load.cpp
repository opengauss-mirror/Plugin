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

#include <cerrno>
#include <cctype>
#include <sys/stat.h>

#include "access/heapam.h"
#include "access/xact.h"
#include "catalog/dependency.h"
#include "catalog/namespace.h"
#include "catalog/objectaddress.h"
#include "commands/copy.h"
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
#include "executor/executor.h"
#include "utils/snapmgr.h"

#include "executor/cypher_utils.h"
#include "utils/builtins.h"
#include "utils/inval.h"
#include "utils/jsonapi.h"
#include "utils/lsyscache.h"
#include "utils/rel.h"
#include "utils/resowner.h"
#include "utils/sec_rls_utils.h"

#include "catalog/ag_graph.h"
#include "catalog/ag_label.h"
#include "commands/label_commands.h"
#include "utils/ag_cache.h"
#include "utils/agtype.h"
#include "utils/graphid.h"

#include "utils/load/age_load.h"
#include "utils/load/ag_load_labels.h"
#include "utils/load/ag_load_edges.h"

extern "C" Datum agtype_in(PG_FUNCTION_ARGS);
extern "C" Datum create_graph(PG_FUNCTION_ARGS);

#define CSV_FILE_EXTENSION_LENGTH (sizeof(".csv") - 1)
#define DECIMAL_RADIX 10
#define LOADER_GRAPH_ARGUMENT_INDEX 0
#define LOADER_LABEL_ARGUMENT_INDEX 1
#define LOADER_FILE_ARGUMENT_INDEX 2
#define LOADER_ID_FIELD_ARGUMENT_INDEX 3
#define LOADER_AGTYPE_ARGUMENT_INDEX 4
#define LOADER_DELIMITER_ARGUMENT_INDEX 5
#define LOADER_EDGE_AGTYPE_ARGUMENT_INDEX 3
#define LOADER_EDGE_DELIMITER_ARGUMENT_INDEX 4

/* Path component that would climb out of the loader sandbox. */
#define PARENT_DIRECTORY_COMPONENT ".."
#define PARENT_DIRECTORY_COMPONENT_LENGTH (sizeof(PARENT_DIRECTORY_COMPONENT) - 1)

/*
 * Role/GUC authorization that does not depend on the file path. It must run
 * before any path resolution so that an unauthorized caller cannot tell
 * whether a server file exists.
 */
static void check_server_file_load_authorization(void)
{
    if (initialuser()) {
        return;
    }

    if (!u_sess->attr.attr_storage.enable_copy_server_files)
        ereport(ERROR,
                (errcode(ERRCODE_INSUFFICIENT_PRIVILEGE),
                 errmsg("loading AGE data from a server file is prohibited"),
                 errhint("Enable enable_copy_server_files or load data through a client-side path.")));

    Oid copy_files_role_oid = get_role_oid("gs_role_copy_files", true);
    if (!superuser() &&
        (!OidIsValid(copy_files_role_oid) ||
         !is_member_of_role(GetUserId(), copy_files_role_oid)))
        ereport(ERROR,
                (errcode(ERRCODE_INSUFFICIENT_PRIVILEGE),
                 errmsg("must be system admin or a member of gs_role_copy_files to load AGE server files")));
}

/* Path policy check on the already-resolved sandbox path. */
static void check_server_file_load_path(const char *file_name)
{
    if (initialuser()) {
        return;
    }

    if (u_sess->attr.attr_common.safe_data_path != NULL &&
        !copyCheckSecurityPath((char *)file_name))
        ereport(ERROR,
                (errcode(ERRCODE_INSUFFICIENT_PRIVILEGE),
                 errmsg("AGE loader file must be under safe_data_path \"%s\"",
                        u_sess->attr.attr_common.safe_data_path)));
}

static void check_loader_rls_support(Oid graph_id, const char *label_name)
{
    Oid relation_id = get_label_relation(label_name, graph_id);
    Relation relation = relation_open(relation_id, AccessShareLock);
    /* same bypass rule as the kernel: superuser, BYPASSRLS, or the owner */
    if (CheckEnableRlsPolicies(relation, GetUserId()) == RLS_ENABLED) {
        relation_close(relation, AccessShareLock);
        ereport(
            ERROR,
            (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
             errmsg("LOAD from file is not supported with row-level security"),
             errhint("Use the Cypher CREATE clause instead.")));
    }

    relation_close(relation, AccessShareLock);
}

/*
 * Reject a relative CSV path whose components could climb out of the loader
 * sandbox. Only ".." is refused: "." and empty components (from "//") cannot
 * leave the directory they appear in.
 */
static void check_csv_path_components(const char *relative_file_name)
{
    const char *component = relative_file_name;

    while (component != NULL) {
        const char *separator = strchr(component, '/');
        size_t component_length = (separator != NULL)
            ? (size_t)(separator - component)
            : strlen(component);
        if (component_length == PARENT_DIRECTORY_COMPONENT_LENGTH &&
            strncmp(component, PARENT_DIRECTORY_COMPONENT,
                    component_length) == 0) {
            ereport(ERROR,
                    (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                     errmsg("CSV file path must not contain \"..\" components")));
        }

        component = (separator != NULL) ? separator + 1 : NULL;
    }
}

/*
 * Refuse symbolic links anywhere below the sandbox root, so that a link cannot
 * redirect the loader outside it, and require the final component to be a
 * regular file. Every prefix of candidate_path below the root is inspected
 * with lstat(); the separators are temporarily replaced by terminators while
 * doing so, which is why the buffer must be writable.
 */
static void check_csv_path_is_regular_file(char *candidate_path,
                                           size_t sandbox_root_length)
{
    char *separator = candidate_path + sandbox_root_length;
    struct stat candidate_stat = {0};

    while (separator != NULL) {
        char *next_separator = strchr(separator + 1, '/');
        if (next_separator != NULL) {
            *next_separator = '\0';
        }

        if (lstat(candidate_path, &candidate_stat) != 0) {
            ereport(ERROR,
                    (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                     errmsg("CSV file does not exist or cannot be inspected: \"%s\"",
                            candidate_path),
                     errdetail("lstat failed: %s", strerror(errno))));
        }

        if (S_ISLNK(candidate_stat.st_mode)) {
            ereport(ERROR,
                    (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                     errmsg("CSV file path must not contain symbolic links"),
                     errdetail("\"%s\" is a symbolic link",
                               candidate_path + sandbox_root_length + 1)));
        }

        if (next_separator != NULL) {
            *next_separator = '/';
        }
        separator = next_separator;
    }

    if (!S_ISREG(candidate_stat.st_mode)) {
        ereport(ERROR,
                (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                 errmsg("CSV file path must refer to a regular file: \"%s\"",
                        candidate_path)));
    }
}

static char *build_safe_csv_path(const char *file_name)
{
    const char *age_load_prefix = "age_load/";
    const size_t age_load_prefix_length = strlen(age_load_prefix);
    const char *relative_file_name;
    size_t relative_file_name_length;
    char *sandbox_root;
    char *candidate_path;
    char *safe_path;
    struct stat sandbox_stat = {0};

    if (file_name == NULL) {
        ereport(ERROR,
                (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                 errmsg("CSV file path must not be NULL")));
    }

    if (file_name[0] == '\0') {
        ereport(ERROR,
                (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                 errmsg("CSV file path must not be empty")));
    }

    if (file_name[0] == '/') {
        ereport(ERROR,
                (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                 errmsg("CSV file path must be relative")));
    }

    relative_file_name = file_name;
    if (strncmp(file_name, age_load_prefix, age_load_prefix_length) == 0) {
        relative_file_name += age_load_prefix_length;
    }

    relative_file_name_length = strlen(relative_file_name);
    if (relative_file_name_length < CSV_FILE_EXTENSION_LENGTH ||
        strcmp(relative_file_name + relative_file_name_length -
            CSV_FILE_EXTENSION_LENGTH, ".csv") != 0) {
        ereport(ERROR,
                (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                 errmsg("CSV file path must end with .csv")));
    }

    check_csv_path_components(relative_file_name);

    if (t_thrd.proc_cxt.DataDir == NULL || t_thrd.proc_cxt.DataDir[0] == '\0') {
        ereport(ERROR,
                (errcode(ERRCODE_OBJECT_NOT_IN_PREREQUISITE_STATE),
                 errmsg("database data directory is not initialized")));
    }

    sandbox_root = psprintf("%s/age_load", t_thrd.proc_cxt.DataDir);
    if (stat(sandbox_root, &sandbox_stat) != 0 || !S_ISDIR(sandbox_stat.st_mode)) {
        ereport(ERROR,
                (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                 errmsg("CSV sandbox root does not exist or is not a directory: \"%s\"",
                        sandbox_root)));
    }

    /*
     * The relative name cannot climb out of the sandbox and no symbolic link
     * below the root is followed, so the candidate is confined to the sandbox.
     */
    candidate_path = psprintf("%s/%s", sandbox_root, relative_file_name);
    check_csv_path_is_regular_file(candidate_path, strlen(sandbox_root));

    /*
     * The returned path must survive the per-row reset of the loader's
     * private row context, and any ereport(ERROR) before the loader finishes
     * must not leak it. The top transaction context satisfies both: it is
     * unaffected by row resets and is reclaimed on transaction abort.
     */
    safe_path = MemoryContextStrdup(u_sess->top_transaction_mem_cxt,
                                    candidate_path);

    pfree(candidate_path);
    pfree(sandbox_root);

    return safe_path;
}

/*
 * Copy of value without surrounding whitespace; "" for a NULL field.
 */
char *trim_csv_value(const char *value)
{
    const char *start;
    const char *end;

    if (value == NULL) {
        return pstrdup("");
    }

    start = value;
    while (*start != '\0' && isspace((unsigned char)*start)) {
        start++;
    }

    end = value + strlen(value);
    while (end > start && isspace((unsigned char)*(end - 1))) {
        end--;
    }

    return pnstrdup(start, end - start);
}

static bool is_json_number(const char *value)
{
    const char *cursor = value;

    if (*cursor == '-') {
        cursor++;
    }

    if (*cursor == '0') {
        cursor++;
        if (isdigit((unsigned char)*cursor)) {
            return false;
        }
    } else if (isdigit((unsigned char)*cursor)) {
        while (isdigit((unsigned char)*cursor)) {
            cursor++;
        }
    } else {
        return false;
    }

    if (*cursor == '.') {
        cursor++;
        if (!isdigit((unsigned char)*cursor)) {
            return false;
        }
        while (isdigit((unsigned char)*cursor)) {
            cursor++;
        }
    }

    if (*cursor == 'e' || *cursor == 'E') {
        cursor++;
        if (*cursor == '+' || *cursor == '-') {
            cursor++;
        }
        if (!isdigit((unsigned char)*cursor)) {
            return false;
        }
        while (isdigit((unsigned char)*cursor)) {
            cursor++;
        }
    }

    return *cursor == '\0';
}

/*
 * Upstream Apache AGE relies on json_validate(..., throw_error = false), a
 * soft-error JSON validator that openGauss does not have.  Run the kernel
 * JSON parser inside a subtransaction instead and treat only its syntax
 * errors (ERRCODE_INVALID_TEXT_REPRESENTATION) as "not a JSON value";
 * anything else (out of memory, query cancel, ...) is re-thrown.
 */
static bool is_valid_json_value(const char *value)
{
    MemoryContext original_context = CurrentMemoryContext;
    ResourceOwner original_owner = t_thrd.utils_cxt.CurrentResourceOwner;
    volatile bool valid = false;

    BeginInternalSubTransaction(NULL);
    MemoryContextSwitchTo(original_context);

    PG_TRY();
    {
        JsonLexContext *lex = makeJsonLexContextCstringLen(
            (char *)value, strlen(value), false);
        JsonSemAction semantic_actions;

        memset(&semantic_actions, 0, sizeof(JsonSemAction));
        pg_parse_json(lex, &semantic_actions);
        ReleaseCurrentSubTransaction();
        MemoryContextSwitchTo(original_context);
        t_thrd.utils_cxt.CurrentResourceOwner = original_owner;
        valid = true;
    }
    PG_CATCH();
    {
        ErrorData *edata;

        MemoryContextSwitchTo(original_context);
        edata = CopyErrorData();
        FlushErrorState();

        RollbackAndReleaseCurrentSubTransaction();
        MemoryContextSwitchTo(original_context);
        t_thrd.utils_cxt.CurrentResourceOwner = original_owner;

        if (edata->sqlerrcode != ERRCODE_INVALID_TEXT_REPRESENTATION) {
            ReThrowError(edata);
        }

        FreeErrorData(edata);
        valid = false;
    }
    PG_END_TRY();

    return valid;
}

static bool looks_like_agtype_value(const char *value)
{
    if (strcmp(value, "null") == 0 || strcmp(value, "true") == 0 ||
        strcmp(value, "false") == 0 || is_json_number(value)) {
        return true;
    }

    if (value[0] == '"' || value[0] == '[' || value[0] == '{') {
        return is_valid_json_value(value);
    }

    return false;
}

static agtype_value *csv_value_to_agtype_value(const char *value,
                                               bool load_as_agtype)
{
    char *trimmed_value = trim_csv_value(value);
    agtype_value *result;

    /*
     * Apache AGE (#2310) loads empty typed CSV fields as agtype null.
     * String mode keeps them as empty strings.
     */
    if (load_as_agtype && trimmed_value[0] == '\0') {
        pfree(trimmed_value);
        result = (agtype_value *)palloc0(sizeof(agtype_value));
        result->type = AGTV_NULL;
        return result;
    }

    if (!load_as_agtype || !looks_like_agtype_value(trimmed_value)) {
        /*
         * Strings keep the field as the CSV reader delivered it: unquoted
         * fields arrive trimmed, quoted fields keep their padding.
         */
        char *original_value = pstrdup(value == NULL ? "" : value);

        result = string_to_agtype_value(original_value);
        pfree(trimmed_value);
        return result;
    }

    agtype *parsed_value = DATUM_GET_AGTYPE_P(
        DirectFunctionCall1(agtype_in, CStringGetDatum(trimmed_value)));
    if (AGT_ROOT_IS_SCALAR(parsed_value)) {
        result = get_ith_agtype_value_from_container(&parsed_value->root, 0);
    } else {
        result = (agtype_value *)palloc0(sizeof(agtype_value));
        result->type = AGTV_BINARY;
        result->val.binary.data = &parsed_value->root;
        result->val.binary.len = VARSIZE(parsed_value) - VARHDRSZ;
    }

    pfree(trimmed_value);
    return result;
}

/*
 * Value of a column typed by a neo4j-import style header (LONG / BOOL /
 * STRING).  A NULL raw field (unquoted empty CSV field) counts as "".
 */
static agtype_value *typed_csv_value_to_agtype_value(const char *value,
                                                     agtype_value_type type)
{
    char *trimmed_value = trim_csv_value(value);

    switch (type) {
        case AGTV_NUMERIC:
            return integer_to_agtype_value(
                strtol(trimmed_value, NULL, DECIMAL_RADIX));
        case AGTV_BOOL:
            return boolean_to_agtype_value(
                pg_strcasecmp(trimmed_value, "true") == 0);
        default:
            return string_to_agtype_value(trimmed_value);
    }
}

agtype *create_agtype_from_list(char **header, char **fields,
                                agtype_value_type *column_types,
                                size_t fields_length, int64 vertex_id,
                                bool load_as_agtype)
{
    agtype_in_state result;

    memset(&result, 0, sizeof(agtype_in_state));
    result.res = push_agtype_value(&result.parse_state, WAGT_BEGIN_OBJECT,
                                   NULL);
    result.res = push_agtype_value(&result.parse_state, WAGT_KEY,
                                   string_to_agtype_value("__id__"));
    result.res = push_agtype_value(&result.parse_state, WAGT_VALUE,
                                   integer_to_agtype_value(vertex_id));

    for (size_t field_index = 0; field_index < fields_length; field_index++)
    {
        if (header[field_index] == NULL || header[field_index][0] == '\0')
            continue;

        result.res = push_agtype_value(
            &result.parse_state, WAGT_KEY,
            string_to_agtype_value(header[field_index]));

        agtype_value *property_value;
        if (column_types != NULL) {
            property_value = typed_csv_value_to_agtype_value(
                fields[field_index], column_types[field_index]);
        } else {
            property_value = csv_value_to_agtype_value(fields[field_index],
                                                       load_as_agtype);
        }

        result.res = push_agtype_value(&result.parse_state, WAGT_VALUE,
                                       property_value);
    }

    result.res = push_agtype_value(&result.parse_state, WAGT_END_OBJECT, NULL);
    return agtype_value_to_agtype(result.res);
}

agtype *create_agtype_from_list_i(char **header, char **fields,
                                  agtype_value_type *column_types,
                                  size_t fields_length, size_t start_index,
                                  size_t end_index, bool load_as_agtype)
{
    agtype_in_state result;
    size_t first_property_index = column_types == NULL ? 4 : 0;

    memset(&result, 0, sizeof(agtype_in_state));
    result.res = push_agtype_value(&result.parse_state, WAGT_BEGIN_OBJECT,
                                   NULL);

    for (size_t field_index = first_property_index;
         field_index < fields_length; field_index++) {
        if (field_index == start_index || field_index == end_index ||
            header[field_index] == NULL || header[field_index][0] == '\0')
            continue;

        result.res = push_agtype_value(
            &result.parse_state, WAGT_KEY,
            string_to_agtype_value(header[field_index]));

        agtype_value *property_value;
        if (column_types != NULL)
        {
            property_value = typed_csv_value_to_agtype_value(
                fields[field_index], column_types[field_index]);
        } else {
            property_value = csv_value_to_agtype_value(fields[field_index],
                                                       load_as_agtype);
        }

        result.res = push_agtype_value(&result.parse_state, WAGT_VALUE,
                                       property_value);
    }

    result.res = push_agtype_value(&result.parse_state, WAGT_END_OBJECT, NULL);
    return agtype_value_to_agtype(result.res);
}

static Oid get_or_create_loader_graph(Name graph_name)
{
    Oid graph_id = get_graph_oid(NameStr(*graph_name));
    if (OidIsValid(graph_id))
        return graph_id;

    DirectFunctionCall1(create_graph, NameGetDatum(graph_name));
    graph_id = get_graph_oid(NameStr(*graph_name));
    if (!OidIsValid(graph_id))
        ereport(ERROR,
                (errcode(ERRCODE_INTERNAL_ERROR),
                 errmsg("graph \"%s\" was not created", NameStr(*graph_name))));

    return graph_id;
}

static int32 get_or_create_loader_label(Oid graph_id, const char *graph_name,
                                        const char *label_name,
                                        char expected_label_kind)
{
    int32 label_id = get_label_id(label_name, graph_id);
    if (label_id_is_valid(label_id)) {
        label_cache_data *label_cache =
            search_label_name_graph_cache(label_name, graph_id);
        if (label_cache->kind != expected_label_kind)
            ereport(ERROR,
                    (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                     errmsg("label \"%s\" already exists as %s label",
                            label_name,
                            expected_label_kind == LABEL_KIND_VERTEX
                                ? "edge" : "vertex")));
        return label_id;
    }

    const char *parent_label = expected_label_kind == LABEL_KIND_VERTEX
                                   ? AG_DEFAULT_LABEL_VERTEX
                                   : AG_DEFAULT_LABEL_EDGE;
    RangeVar *parent_range = get_label_range_var((char *)graph_name, graph_id,
        (char *)parent_label);
    create_label((char *)graph_name, (char *)label_name,
                 expected_label_kind == LABEL_KIND_VERTEX
                     ? LABEL_TYPE_VERTEX : LABEL_TYPE_EDGE,
                 list_make1(parent_range));
    CommandCounterIncrement();

    label_id = get_label_id(label_name, graph_id);
    if (!label_id_is_valid(label_id))
        ereport(ERROR,
                (errcode(ERRCODE_INTERNAL_ERROR),
                 errmsg("label \"%s\" was not created", label_name)));

    ereport(NOTICE,
            (errmsg("%s label \"%s\" has been created",
                    expected_label_kind == LABEL_KIND_VERTEX
                        ? "vertex" : "edge",
                    label_name)));
    return label_id;
}

static char get_loader_delimiter(FunctionCallInfo fcinfo, int argument_index)
{
    char delimiter;

    if (PG_NARGS() <= argument_index || PG_ARGISNULL(argument_index)) {
        return ',';
    }

    text *delimiter_text = PG_GETARG_TEXT_P(argument_index);
    char *delimiter_string = text_to_cstring(delimiter_text);
    if (strlen(delimiter_string) != 1)
        ereport(ERROR,
                (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                 errmsg("delimiter must be a single character")));

    delimiter = delimiter_string[0];
    pfree(delimiter_string);
    return delimiter;
}

Oid get_loader_label_sequence_oid(Oid graph_id, const char *label_name)
{
    char *relation_name = get_label_relation_name(label_name, graph_id);
    char *sequence_name = psprintf("%s_id_seq", relation_name);
    Oid graph_namespace_id = get_rel_namespace(
        get_label_relation(label_name, graph_id));
    Oid sequence_id = get_relname_relid(sequence_name, graph_namespace_id);
    if (!OidIsValid(sequence_id))
        ereport(ERROR,
                (errcode(ERRCODE_UNDEFINED_TABLE),
                 errmsg("AGE label sequence \"%s\" does not exist",
                        sequence_name)));

    pfree(sequence_name);
    return sequence_id;
}

int64 next_loader_sequence_value(Oid sequence_id)
{
    int128 sequence_value = nextval_internal(sequence_id, true, false);
    if (sequence_value > PG_INT64_MAX || sequence_value < PG_INT64_MIN)
        ereport(ERROR,
                (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
                 errmsg("AGE label sequence value is outside graphid range")));

    return (int64)sequence_value;
}

/*
 * Move the label sequence past the highest explicit id the file supplied, so
 * ids handed out by later Cypher CREATEs cannot collide with loaded rows.
 * setval_oid() applies the usual UPDATE privilege check on the sequence.
 */
void advance_loader_sequence(Oid sequence_id, int64 sequence_value)
{
    Datum next = DirectFunctionCall1(int8_numeric,
                                     Int64GetDatum(sequence_value));

    (void)DirectFunctionCall2(setval_oid, ObjectIdGetDatum(sequence_id), next);
}

/*
 * Rows are written through the same heap path the Cypher CREATE clause uses
 * (insert_entity_tuple(): stored generated columns, constraints, index
 * maintenance) instead of one SPI INSERT statement per row.  The relation,
 * its indexes and the executor state are set up once per file.
 */
loader_target *open_loader_target(Oid graph_id, char *graph_name,
                                  char *label_name)
{
    loader_target *target = (loader_target *)palloc0(sizeof(loader_target));
    Oid relation_id = get_label_relation(label_name, graph_id);
    AclResult acl_result;

    if (!OidIsValid(relation_id))
        ereport(ERROR,
                (errcode(ERRCODE_UNDEFINED_TABLE),
                 errmsg("could not resolve AGE label relation for \"%s\"",
                        label_name)));

    /* the SQL executor is bypassed, so check the INSERT privilege here */
    acl_result = pg_class_aclcheck(relation_id, GetUserId(), ACL_INSERT);
    if (acl_result != ACLCHECK_OK)
        aclcheck_error(acl_result, ACL_KIND_CLASS, get_rel_name(relation_id));

    target->estate = CreateExecutorState();
    target->estate->es_snapshot = GetActiveSnapshot();
    target->estate->es_output_cid = GetCurrentCommandId(true);
    target->result_rel_info = create_entity_result_rel_info(
        target->estate, graph_name, label_name);
    /* ExecInsertIndexTuples() takes the target relation from the estate */
    target->estate->es_result_relation_info = target->result_rel_info;
    target->relation_id = relation_id;
    target->slot = MakeSingleTupleTableSlot(
        RelationGetDescr(target->result_rel_info->ri_RelationDesc));

    return target;
}

void close_loader_target(loader_target *target)
{
    List *modified_relids = NIL;

    if (target == NULL) {
        return;
    }

    ExecDropSingleTupleTableSlot(target->slot);
    destroy_entity_result_rel_info(target->result_rel_info);
    FreeExecutorState(target->estate);

    /* what the AFTER statement trigger does for SQL DML on label tables */
    mark_entity_relation_modified(&modified_relids, target->relation_id);
    notify_modified_entity_relations(&modified_relids);
}

static void loader_insert_tuple(loader_target *target)
{
    ResetPerTupleExprContext(target->estate);
    (void)insert_entity_tuple(target->result_rel_info, target->slot,
                              target->estate);
    ExecClearTuple(target->slot);
}

void insert_edge_simple(loader_target *target, graphid edge_id,
                        graphid start_id, graphid end_id,
                        agtype *edge_properties)
{
    TupleTableSlot *slot = target->slot;

    ExecClearTuple(slot);
    slot->tts_values[Anum_ag_label_edge_table_id - 1] =
        GRAPHID_GET_DATUM(edge_id);
    slot->tts_values[Anum_ag_label_edge_table_start_id - 1] =
        GRAPHID_GET_DATUM(start_id);
    slot->tts_values[Anum_ag_label_edge_table_end_id - 1] =
        GRAPHID_GET_DATUM(end_id);
    slot->tts_values[Anum_ag_label_edge_table_properties - 1] =
        AGTYPE_P_GET_DATUM(edge_properties);
    slot->tts_isnull[Anum_ag_label_edge_table_id - 1] = false;
    slot->tts_isnull[Anum_ag_label_edge_table_start_id - 1] = false;
    slot->tts_isnull[Anum_ag_label_edge_table_end_id - 1] = false;
    slot->tts_isnull[Anum_ag_label_edge_table_properties - 1] = false;

    loader_insert_tuple(target);
}

void insert_vertex_simple(loader_target *target, graphid vertex_id,
                          agtype *vertex_properties)
{
    TupleTableSlot *slot = target->slot;

    ExecClearTuple(slot);
    slot->tts_values[Anum_ag_label_vertex_table_id - 1] =
        GRAPHID_GET_DATUM(vertex_id);
    slot->tts_values[Anum_ag_label_vertex_table_properties - 1] =
        AGTYPE_P_GET_DATUM(vertex_properties);
    slot->tts_isnull[Anum_ag_label_vertex_table_id - 1] = false;
    slot->tts_isnull[Anum_ag_label_vertex_table_properties - 1] = false;

    loader_insert_tuple(target);
}

PG_FUNCTION_INFO_V1(load_labels_from_file);
extern "C" Datum load_labels_from_file(PG_FUNCTION_ARGS);
Datum load_labels_from_file(PG_FUNCTION_ARGS)
{
    Name graph_name;
    Name label_name;
    text *file_path;
    char *graph_name_string;
    char *label_name_string;
    char *input_file_path;
    char *safe_file_path;
    Oid graph_id;
    int32 label_id;
    bool id_field_exists;
    bool load_as_agtype;
    char delimiter;

    if (PG_ARGISNULL(0))
        ereport(ERROR,
                (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                 errmsg("graph name must not be NULL")));
    if (PG_ARGISNULL(1))
        ereport(ERROR,
                (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                 errmsg("label name must not be NULL")));
    if (PG_ARGISNULL(2))
        ereport(ERROR,
                (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                 errmsg("file path must not be NULL")));

    graph_name = PG_GETARG_NAME(LOADER_GRAPH_ARGUMENT_INDEX);
    label_name = PG_GETARG_NAME(LOADER_LABEL_ARGUMENT_INDEX);
    file_path = PG_GETARG_TEXT_P(LOADER_FILE_ARGUMENT_INDEX);
    id_field_exists = PG_GETARG_BOOL(LOADER_ID_FIELD_ARGUMENT_INDEX);
    load_as_agtype = PG_NARGS() > LOADER_AGTYPE_ARGUMENT_INDEX &&
                     !PG_ARGISNULL(LOADER_AGTYPE_ARGUMENT_INDEX)
                         ? PG_GETARG_BOOL(LOADER_AGTYPE_ARGUMENT_INDEX)
                         : false;
    delimiter = get_loader_delimiter(fcinfo, LOADER_DELIMITER_ARGUMENT_INDEX);

    graph_name_string = NameStr(*graph_name);
    label_name_string = NameStr(*label_name);
    if (label_name_string[0] == '\0')
        label_name_string = AG_DEFAULT_LABEL_VERTEX;

    check_server_file_load_authorization();
    input_file_path = text_to_cstring(file_path);
    safe_file_path = build_safe_csv_path(input_file_path);
    check_server_file_load_path(safe_file_path);
    pfree(input_file_path);

    graph_id = get_or_create_loader_graph(graph_name);
    label_id = get_or_create_loader_label(
        graph_id, graph_name_string, label_name_string, LABEL_KIND_VERTEX);
    check_loader_rls_support(graph_id, label_name_string);

    create_labels_from_csv_file(
        safe_file_path, graph_name_string, graph_id, label_name_string,
        label_id, id_field_exists, load_as_agtype, delimiter);
    pfree(safe_file_path);
    PG_RETURN_VOID();
}

PG_FUNCTION_INFO_V1(load_edges_from_file);
extern "C" Datum load_edges_from_file(PG_FUNCTION_ARGS);
Datum load_edges_from_file(PG_FUNCTION_ARGS)
{
    Name graph_name;
    Name label_name;
    text *file_path;
    char *graph_name_string;
    char *label_name_string;
    char *input_file_path;
    char *safe_file_path;
    Oid graph_id;
    int32 label_id;
    bool load_as_agtype;
    char delimiter;

    if (PG_ARGISNULL(0))
        ereport(ERROR,
                (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                 errmsg("graph name must not be NULL")));
    if (PG_ARGISNULL(1))
        ereport(ERROR,
                (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                 errmsg("label name must not be NULL")));
    if (PG_ARGISNULL(2))
        ereport(ERROR,
                (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                 errmsg("file path must not be NULL")));

    graph_name = PG_GETARG_NAME(LOADER_GRAPH_ARGUMENT_INDEX);
    label_name = PG_GETARG_NAME(LOADER_LABEL_ARGUMENT_INDEX);
    file_path = PG_GETARG_TEXT_P(LOADER_FILE_ARGUMENT_INDEX);
    load_as_agtype = PG_NARGS() > LOADER_EDGE_AGTYPE_ARGUMENT_INDEX &&
                     !PG_ARGISNULL(LOADER_EDGE_AGTYPE_ARGUMENT_INDEX)
                         ? PG_GETARG_BOOL(LOADER_EDGE_AGTYPE_ARGUMENT_INDEX)
                         : false;
    delimiter = get_loader_delimiter(fcinfo,
                                     LOADER_EDGE_DELIMITER_ARGUMENT_INDEX);

    graph_name_string = NameStr(*graph_name);
    label_name_string = NameStr(*label_name);
    if (label_name_string[0] == '\0')
        label_name_string = AG_DEFAULT_LABEL_EDGE;

    check_server_file_load_authorization();
    input_file_path = text_to_cstring(file_path);
    safe_file_path = build_safe_csv_path(input_file_path);
    check_server_file_load_path(safe_file_path);
    pfree(input_file_path);

    graph_id = get_or_create_loader_graph(graph_name);
    label_id = get_or_create_loader_label(
        graph_id, graph_name_string, label_name_string, LABEL_KIND_EDGE);
    check_loader_rls_support(graph_id, label_name_string);

    create_edges_from_csv_file(
        safe_file_path, graph_name_string, graph_id, label_name_string,
        label_id, load_as_agtype, delimiter);
    pfree(safe_file_path);
    PG_RETURN_VOID();
}

PG_FUNCTION_INFO_V1(load_labels_from_file_with_analysefile);
extern "C" Datum  load_labels_from_file_with_analysefile(PG_FUNCTION_ARGS);
Datum load_labels_from_file_with_analysefile(PG_FUNCTION_ARGS)
{

    Name graph_name;
    Name label_name;
    text* file_path;
    char* graph_name_str;
    char* label_name_str;
    char* input_file_path;
    char* file_path_str;
    Oid graph_id;
    int32 label_id;
    bool id_field_exists;
    bool with_header = true;
    bool adjust_id = false;/** if true  the csv id will add 1*/

    if (PG_ARGISNULL(0))
    {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                errmsg("graph name must not be NULL")));
    }

    if (PG_ARGISNULL(1))
    {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                errmsg("label name must not be NULL")));
    }

    if (PG_ARGISNULL(2))
    {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                errmsg("file path must not be NULL")));
    }

    graph_name = PG_GETARG_NAME(0);
    label_name = PG_GETARG_NAME(1);
    file_path = PG_GETARG_TEXT_P(2);
    id_field_exists = true; // PG_GETARG_BOOL(3);
    graph_name_str = NameStr(*graph_name);
    label_name_str = NameStr(*label_name);
    check_server_file_load_authorization();
    input_file_path = text_to_cstring(file_path);
    file_path_str = build_safe_csv_path(input_file_path);
    check_server_file_load_path(file_path_str);
    pfree(input_file_path);

    graph_id = get_graph_oid(graph_name_str);
    label_id = get_label_id(label_name_str, graph_id);
    check_loader_rls_support(graph_id, label_name_str);

    create_labels_from_csv_file(file_path_str, graph_name_str,
                                graph_id, label_name_str, label_id,
                                id_field_exists, false, ',', with_header,
                                adjust_id);
    pfree(file_path_str);
    PG_RETURN_VOID();
}

PG_FUNCTION_INFO_V1(load_edges_from_file_with_analysefile);
extern "C" Datum  load_edges_from_file_with_analysefile(PG_FUNCTION_ARGS);
Datum load_edges_from_file_with_analysefile(PG_FUNCTION_ARGS)
{

    Name graph_name;
    Name label_name;
    text* file_path;
    char* graph_name_str;
    char* label_name_str;
    char* input_file_path;
    char* file_path_str;
    Oid graph_id;
    int32 label_id;
    bool with_header = true;
    bool adjust_id = false;/** if true  the csv id will add 1*/

    if (PG_ARGISNULL(0))
    {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                errmsg("graph name must not be NULL")));
    }

    if (PG_ARGISNULL(1))
    {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                errmsg("label name must not be NULL")));
    }

    if (PG_ARGISNULL(2))
    {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                errmsg("file path must not be NULL")));
    }

    graph_name = PG_GETARG_NAME(0);
    label_name = PG_GETARG_NAME(1);
    file_path = PG_GETARG_TEXT_P(2);

    graph_name_str = NameStr(*graph_name);
    label_name_str = NameStr(*label_name);
    check_server_file_load_authorization();
    input_file_path = text_to_cstring(file_path);
    file_path_str = build_safe_csv_path(input_file_path);
    check_server_file_load_path(file_path_str);
    pfree(input_file_path);

    graph_id = get_graph_oid(graph_name_str);
    label_id = get_label_id(label_name_str, graph_id);
    check_loader_rls_support(graph_id, label_name_str);

    create_edges_from_csv_file(file_path_str, graph_name_str,
                               graph_id, label_name_str, label_id,
                               false, ',', with_header, adjust_id);
    pfree(file_path_str);
    PG_RETURN_VOID();
}
